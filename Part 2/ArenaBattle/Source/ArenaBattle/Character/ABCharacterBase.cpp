// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABCharacterControlData.h"
#include "Animation/AnimMontage.h"
#include "ABComboActionData.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "Engine/DamageEvents.h"
#include "Physics/ABCollision.h"
#include "Item/ABItemData.h"
#include "Item/ABWeaponItemData.h"
#include "UI/ABWidgetComponent.h"
#include "UI/ABHpBarWidget.h"

DEFINE_LOG_CATEGORY(LogABCharacter);

// Sets default values
AABCharacterBase::AABCharacterBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_ABCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));

	// Cardboard 캐릭터로 변경
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Cardboard.SK_CharM_Cardboard'"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// 애님 인스턴스 경로도 ABP_ABCharacter로 변경
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/ArenaBattle/Animation/ABP_ABCharacter.ABP_ABCharacter_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> ShoulderDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if (ShoulderDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterControlData> QuarterDataRef(TEXT("/Script/ArenaBattle.ABCharacterControlData'/Game/ArenaBattle/CharacterControl/ABC_Quater.ABC_Quater'"));
	if (QuarterDataRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quarter, QuarterDataRef.Object);
	}

	// TODO: 몽타주 에셋 지정 -> ConstructorHelpers를 쓰는 것이 아닌, Blueprint 클래스로 상속받아 그곳에서 지정하도록 설정.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ComboActionMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_ComboAttack.AM_ComboAttack'"));
	if(ComboActionMontageRef.Object)
	{
		ComboActionMontage = ComboActionMontageRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UABComboActionData> ComboActionDataRef(TEXT("/Script/ArenaBattle.ABComboActionData'/Game/ArenaBattle/CharacterAction/ABA_ComboAttack.ABA_ComboAttack'"));
	if(ComboActionDataRef.Object)
	{
		ComboActionData = ComboActionDataRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(TEXT("/Script/Engine.AnimMontage'/Game/ArenaBattle/Animation/AM_Dead.AM_Dead'"));
	if(DeadMontageRef.Object)
	{
		DeadMontage = DeadMontageRef.Object;
	}

	// Stat Component: 트랜스폼 없음
	Stat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("Stat"));
	
	// Widget Component: 트랜스폼 있음
	HpBar = CreateDefaultSubobject<UABWidgetComponent>(TEXT("Widget"));  // UABWidgetComponent로 변경
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0, 0, 180));
	// Widget은 AnimationBlueprint와 비슷. 클래스 정보를 등록해, BeginPlay 실행 시 클래스 정보로부터 인스턴스가 생성됨
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/ArenaBattle/UI/WBP_HpBar.WBP_HpBar_C'"));
	if(HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);  // World: 3D, Screen: 2D
		HpBar->SetDrawSize(FVector2D(150, 15));  // 실제 HpBar 크기를 여기서 지정함
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌이 발생하지 않도록 Off
	}

	// Item Actions
	// 클래스 멤버와 바인딩된 델리게이트를 바로 집어넣음: CreateUObject()를 이용.
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::EquipWeapon)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::DrinkPotion)));
	TakeItemActions.Add(FTakeItemDelegateWrapper(FOnTakeItemDelegate::CreateUObject(this, &AABCharacterBase::ReadScroll)));

	// Weapon Component
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));  // 캐릭터 특정 본에 부착되도록 설정. 에셋에 지정되어있는 소켓명
}

void AABCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &AABCharacterBase::SetDead);
}

void AABCharacterBase::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AABCharacterBase::ProcessComboCommand()
{
	if(CurrentCombo == 0)  // 콤보가 진행중이지 않을 경우 Begin 호출
	{
		ComboActionBegin();
		return;
	}

	// 1. 타이머가 설정된 상태로 입력이 들어왔다면, 체크하기 전 다음 섹션으로 이동시킬 커맨드가 발동했다는 의미
	// 2. 타이머가 설정되어 있지 않은 상태였다면, 이미 타이머가 발동되어 시기를 놓쳤거나 더 이상 진행을 필요가 없다는 의미
	HasNextComboCommand = ComboTimerHandle.IsValid();
}

void AABCharacterBase::ComboActionBegin()
{
	// Combo Status
	CurrentCombo = 1;

	// Movement Setting
	GetCharacterMovement()->SetMovementMode(MOVE_None);  // 이동 제한

	// Animation Setting
	constexpr float AttackSpeedRate = 1.0f;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();  // AnimInstance는 Skeletal Mesh에 존재
	AnimInstance->Montage_Play(ComboActionMontage, AttackSpeedRate);  // Montage Asset을 지정해 Play

	// 몽타주 종료 시 End 함수가 호출되도록 Delegate 등록
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AABCharacterBase::ComboActionEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, ComboActionMontage);

	// 콤보가 시작했을 때 타이머 발동
	ComboTimerHandle.Invalidate();
	SetComboCheckTimer();
}

void AABCharacterBase::ComboActionEnd(UAnimMontage* TargetMontage, bool IsProperlyEnded)
{
	ensure(CurrentCombo != 0);  // 콤보가 종료되는 시점에서는 반드시 콤보가 진행되고 있어야 함.
	CurrentCombo = 0;

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);  // 정상적으로 움직일 수 있도록
}

void AABCharacterBase::SetComboCheckTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(ComboActionData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	constexpr float AttackSpeedRate = 1.0f;
	float ComboEffectiveTime = (ComboActionData->EffectiveFrameCount[ComboIndex] / ComboActionData->FrameRate) / AttackSpeedRate;
	if(ComboEffectiveTime > 0.0f) // 입력 시간이 유효한 경우
	{
		// 콤보 체크 함수를 1번 호출
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &AABCharacterBase::ComboCheck, ComboEffectiveTime, false);
	}
}

void AABCharacterBase::ComboCheck()
{
	ComboTimerHandle.Invalidate();
	if(HasNextComboCommand)
	{
		// 다음 섹션으로 넘겨줌
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		
		CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, ComboActionData->MaxComboCount);
		FName NextSection = *FString::Printf(TEXT("%s%d"), *ComboActionData->MontageSectionNamePrefix, CurrentCombo);
		AnimInstance->Montage_JumpToSection(NextSection, ComboActionMontage);  // 해당하는 이름의 몽타주 섹션으로 바로 점프

		SetComboCheckTimer();
		HasNextComboCommand = false;
 	}
}

void AABCharacterBase::AttackHitCheck()
{
	// TODO: Trace 채널을 이용해 물체가 서로 충돌하는 지 검사하는 로직 작성
	constexpr float AttackRange = 40.0f;
	constexpr float AttackRadius = 50.0f;
	constexpr float AttackDamage = 100.0f;
	FHitResult OutHit;
	// SweepSingleByChannel: World가 제공 -> GetWorld로부터 획득
	if(GetWorld()->SweepSingleByChannel(
		OutHit,
		GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius(),
		GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		CCHANNEL_ABACTION,
		FCollisionShape::MakeSphere(AttackRadius),
		FCollisionQueryParams(SCENE_QUERY_STAT(Attack), true, this)
	))
	{
		// TODO: TakeDamage 함수를 호출해 데미지 전달
		FDamageEvent DamageEvent;
		OutHit.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
	}
	
#if ENABLE_DRAW_DEBUG
	FVector CapsuleOrigin = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius() + (GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius() + GetActorForwardVector() * AttackRange - (GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius())) * 0.5f;
	float CapsuleHalfHeight = AttackRange * 0.5f;
	FColor DrawColor = GetWorld()->SweepSingleByChannel(
		                   OutHit,
		                   GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius(),
		                   GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius() + GetActorForwardVector() * AttackRange,
		                   FQuat::Identity,
		                   CCHANNEL_ABACTION,
		                   FCollisionShape::MakeSphere(AttackRadius),
		                   FCollisionQueryParams(SCENE_QUERY_STAT(Attack), true, this)
	                   ) ? FColor::Green : FColor::Red;

	DrawDebugCapsule(GetWorld(), CapsuleOrigin, CapsuleHalfHeight, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
#endif 
}

// return: 최종적으로 액터가 받는 데미지 양
float AABCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// Instigator: 나에게 피해를 입힌 가해자
	// Causer:가해자가 사용한 무기/빙의한 폰
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// TODO: 사망 모션 재생
	Stat->ApplyDamage(DamageAmount);
	
	return DamageAmount;
}

void AABCharacterBase::SetDead()
{
	// Free Movement
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	PlayDeadAnimation();  // 사망 모션 재생
	SetActorEnableCollision(false);  // 콜리전 자체에 대한 기능 Off -> Player의 이동 방해 X
	HpBar->SetHiddenInGame(true);  // 사망 시 HPBar가 사라지도록
}

void AABCharacterBase::PlayDeadAnimation()
{
	auto AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);  // 몽타주 중지
	AnimInstance->Montage_Play(DeadMontage, 1.0f);  // 사망 모션 재생
}

void AABCharacterBase::SetupCharacterWidget(UABUserWidget* InUserWidget)
{
	// TODO: Hp Bar 등록
	if(auto HpBarWidget = Cast<UABHpBarWidget>(InUserWidget))
	{
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(HpBarWidget, &UABHpBarWidget::UpdateHpBar);  // 델리게이트 등록
	}
}

void AABCharacterBase::TakeItem(UABItemData* InItemData)
{
	if(InItemData)
	{
		TakeItemActions[static_cast<uint8>(InItemData->Type)].ItemDelegate.ExecuteIfBound(InItemData);
	}
}

void AABCharacterBase::DrinkPotion(UABItemData* InItemData)
{
	UE_LOG(LogABCharacter, Log, TEXT("Drink Potion"));
}

void AABCharacterBase::EquipWeapon(UABItemData* InItemData)
{
	// 에셋의 특정 본에 웨폰을 부착하도록 했으므로, 웨폰이 가진 스켈레탈 메시 지정
	if(auto WeaponItemData = Cast<UABWeaponItemData>(InItemData))
	{
		// Soft Referencing으로 변경되면서 컴파일 타임에 로딩이 됐는 지 안됐는 지 알 수 없음 -> 변경
		if(WeaponItemData->WeaponMesh.IsPending())  // 아직 로딩되어 있지 않다면
		{
			WeaponItemData->WeaponMesh.LoadSynchronous();  // 동기적 로딩
		}
		Weapon->SetSkeletalMesh(WeaponItemData->WeaponMesh.Get());  // Get()을 사용해 가져옴
	}
}

void AABCharacterBase::ReadScroll(UABItemData* InItemData)
{
	UE_LOG(LogABCharacter, Log, TEXT("Read Scroll"));
}
