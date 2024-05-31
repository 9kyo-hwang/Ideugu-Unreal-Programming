// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/ABStageGimmick.h"

#include "Character/ABCharacterNonPlayer.h"
#include "Components/BoxComponent.h"
#include "Item/ABItemBox.h"
#include "Physics/ABCollision.h"

// Sets default values
AABStageGimmick::AABStageGimmick() :
Stage(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"))),
StageTrigger(CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"))),
CurrentStage(EStageState::Ready),
OpponentClass(AABCharacterNonPlayer::StaticClass()),
OpponentSpawnTime(2.0f),
RewardBoxClass(AABItemBox::StaticClass())
{
	// Stage 세팅
	// Stage = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stage"));
	RootComponent = Stage;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StageMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Stages/SM_SQUARE.SM_SQUARE'"));
	if(StageMeshRef.Object)
	{
		Stage->SetStaticMesh(StageMeshRef.Object);
	}

	// Stage Trigger 세팅
	//StageTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("StageTrigger"));
	StageTrigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	StageTrigger->SetupAttachment(Stage);
	StageTrigger->SetRelativeLocation(FVector(0, 0, 250));
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);  // Pawn만 감지하도록 설정
	StageTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnStageTriggerBeginOverlap);

	// Gates 세팅
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GateMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_GATE.SM_GATE'"));
	static FName GateSockets[] = {TEXT("+XGate"), TEXT("-XGate"), TEXT("+YGate"), TEXT("-YGate")};
	for(const FName& GateSocket : GateSockets)
	{
		// 개별 Gate
		UStaticMeshComponent* Gate = CreateDefaultSubobject<UStaticMeshComponent>(GateSocket);
		Gate->SetStaticMesh(GateMeshRef.Object);
		Gate->SetupAttachment(Stage, GateSocket);  // 메시에 Socket 이름도 같이 지정.
		Gate->SetRelativeLocation(FVector(0, -80.5, 0));
		Gate->SetRelativeRotation(FRotator(0, -90, 0));  // 열린 상태로 초기화
		Gates.Add(GateSocket, Gate);

		// 해당 Gate에 대한 Gate Trigger
		FName TriggerName = *GateSocket.ToString().Append(TEXT("Trigger"));
		UBoxComponent* GateTrigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
		GateTrigger->SetBoxExtent(FVector(100, 100, 300));
		GateTrigger->SetupAttachment(Stage, GateSocket);
		GateTrigger->SetRelativeLocation(FVector(70, 0, 250));
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);  // 캐릭터가 탐지되도록
		GateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::OnGateTriggerBeginOverlap);
		GateTrigger->ComponentTags.Add(GateSocket);  // 게이트 이름으로 태그 부착.
		GateTriggers.Add(GateTrigger);
	}

	// Stage Section
	// CurrentState = EStageState::Ready;
	StateChangeActions.Add(EStageState::Ready, FOnStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetReady)));
	StateChangeActions.Add(EStageState::Fight, FOnStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetFight)));
	StateChangeActions.Add(EStageState::Reward, FOnStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseReward)));
	StateChangeActions.Add(EStageState::Next, FOnStageChangedDelegateWrapper(FOnStageChangedDelegate::CreateUObject(this, &AABStageGimmick::SetChooseNext)));

	// Fight Section
	// OpponentSpawnTime = 2.0f;  // NPC 스폰 시간
	// OpponentClass = AABCharacterNonPlayer::StaticClass();  // 스폰될 NPC 클래스 정보

	// Box Section
	// RewardBoxClass = AABItemBox::StaticClass();
	for(const FName& GateSocket : GateSockets)
	{
		FVector BoxLocation = Stage->GetSocketLocation(GateSocket) / 2;
		RewardBoxLocations.Add(GateSocket, BoxLocation);
	}
}

void AABStageGimmick::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 모든 속성이 변경될 때 호출됨 -> CurrentState 값 변경 시 SetState 함수를 호출하도록 설정
	SetState(CurrentStage);
}

void AABStageGimmick::OnStageTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 스테이지 트리거와 접촉 시 Fight 모드로 전환
	SetState(EStageState::Fight);
}

void AABStageGimmick::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 위에서 태그를 달아줬으니 1개 존재해야 함 -> 검증
	check(OverlappedComponent->ComponentTags.Num() == 1);
	
	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));  // +X, -X, +Y, -Y
	check(Stage->DoesSocketExist(SocketName));  // 위에서 추출한 소켓 이름에 대응되는 소켓이 존재하는 지 검사

	// 해당 위치에 무언가가 존재하는 지 검사
	if(TArray<FOverlapResult> OutOverlaps; !GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		Stage->GetSocketLocation(SocketName),
		FQuat::Identity,
		FCollisionObjectQueryParams::InitType::AllStaticObjects,  // 자신을 제외한 모든 스태틱 오브젝트 타입에 대해 검사
		FCollisionShape::MakeSphere(775),  // 큰 크기의 구체 생성
		FCollisionQueryParams(SCENE_QUERY_STAT(GateTrigger), false, this)
	))
	{
		// 아무것도 없을 시 해당 위치에 똑같은 기믹 액터 스폰
		GetWorld()->SpawnActor<AABStageGimmick>(Stage->GetSocketLocation(SocketName), FRotator::ZeroRotator);
	}
}

void AABStageGimmick::OpenAllGates()
{
	for(const auto& [Name, Mesh] : Gates)
	{
		Mesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void AABStageGimmick::CloseAllGates()
{
	for(const auto& [Name, Mesh] : Gates)
	{
		Mesh->SetRelativeRotation(FRotator::ZeroRotator);
	}
}

void AABStageGimmick::SetState(EStageState NewState)
{
	// Switch문의 경우, 상태가 많아질 수록 굉장히 코드가 복잡해짐
	// 대신 Delegate Array로, 함수 포인터를 사용해 구현

	CurrentStage = NewState;
	if(StateChangeActions.Contains(NewState))  // 새로 둘어온 State가 Map에 존재한다면
	{
		// Binding 되어있다면 델리게이트 수행
		StateChangeActions[CurrentStage].StageDelegate.ExecuteIfBound();
	}
}

void AABStageGimmick::SetReady()
{
	// 맵 가운데에 있는 Stage Trigger 활성화
	StageTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	for(const auto& GateTrigger : GateTriggers)
	{
		// 각 문에 대해서는 콜리전 비활성화
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}
	OpenAllGates();  // Ready에는 문이 열려있어야 함.
}

void AABStageGimmick::SetFight()
{
	// 대전 시작 시 스테이지 트리거도 콜리전 비활성화
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for(const auto& GateTrigger : GateTriggers)
	{
		// 여전히 비활성화
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}
	CloseAllGates();  // 확인 차원에서 문 닫기

	// 월드의 타이머 매니저에 접근해 OnOpponentSpawn 함수를 OpponentSpawnTime 후에 진행
	GetWorld()->GetTimerManager().SetTimer(OpponentTimerHandle, this, &AABStageGimmick::OnOpponentSpawn, OpponentSpawnTime, false);
}

void AABStageGimmick::SetChooseReward()
{
	// 보상 단계에서도 동일하게 스테이지 트리거 콜리전 비활성화
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for(const auto& GateTrigger : GateTriggers)
	{
		// 여전히 비활성화
		GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	}
	CloseAllGates();  // 확인 차원에서 문 닫기
	SpawnRewardBoxes();  // Reward 상태이므로 상자 스폰
}

void AABStageGimmick::SetChooseNext()
{
	// 게이트를 선택해야 하므로 스테이지 트리거 콜리전은 비활성화
	StageTrigger->SetCollisionProfileName(TEXT("NoCollision"));
	for(const auto& GateTrigger : GateTriggers)
	{
		// 문을 골라야 하므로 ABTrigger 상태
		GateTrigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);
	}
	OpenAllGates();  // 이제 모든 문을 열어놔야 함.
}

void AABStageGimmick::OnOpponentDestroyed(AActor* DestroyedActor)
{
	// NPC 사망 시 보상 선택 단계로 이동
	SetState(EStageState::Reward);
}

void AABStageGimmick::OnOpponentSpawn()
{
	const FVector SpawnLocation = GetActorLocation() + FVector::UpVector * 88;  // Spawn 위치
	AActor* OpponentActor = GetWorld()->SpawnActor(OpponentClass, &SpawnLocation, &FRotator::ZeroRotator);  // 지정한 클래스를 넘겨 액터 스폰
	if(AABCharacterNonPlayer* ABOpponentCharacter = Cast<AABCharacterNonPlayer>(OpponentActor))  // NPC로 캐스팅
	{
		ABOpponentCharacter->OnDestroyed.AddDynamic(this, &AABStageGimmick::OnOpponentDestroyed);  // OnDestroy 델리게이트에 위 함수 등록
	}
}

void AABStageGimmick::OnRewardTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for(const auto& RewardBox : RewardBoxes)
	{
		if(!RewardBox.IsValid()) continue;  // 약참조이기 때문에 유효한 지 알 수 없음. 검사 필요

		// 유효하다면 Get() 메서드를 통해 포인터를 얻어와야 함.
		// Valid한 리워드 박스가, 캐릭터와 Overlap하지 않다면 먹지 않은 박스이므로 파괴
		if(AABItemBox* ValidRewardBox = RewardBox.Get(); ValidRewardBox != OverlappedComponent->GetOwner())
		{
			ValidRewardBox->Destroy();
		}
	}
	SetState(EStageState::Next);  // 보상을 선택했으니 Next 상태로 변경
}

void AABStageGimmick::SpawnRewardBoxes()
{
	for(const auto& [Name, Location] : RewardBoxLocations)
	{
		FVector WorldSpawnLocation = GetActorLocation() + Location + FVector(0, 0, 30);
		AActor* ItemActor = GetWorld()->SpawnActor(RewardBoxClass, &WorldSpawnLocation, &FRotator::ZeroRotator);
		if(AABItemBox* RewardBoxActor = Cast<AABItemBox>(ItemActor))
		{
			RewardBoxActor->Tags.Add(Name);
			// Trigger Component가 protected로 되어있어서 Getter를 하나 파줘야 함.
			RewardBoxActor->GetTrigger()->OnComponentBeginOverlap.AddDynamic(this, &AABStageGimmick::AABStageGimmick::OnRewardTriggerBeginOverlap);
			RewardBoxes.Add(RewardBoxActor);
		}
	}
}