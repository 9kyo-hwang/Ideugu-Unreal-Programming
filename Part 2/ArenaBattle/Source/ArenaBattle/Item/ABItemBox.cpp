// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ABItemBox.h"

#include "Components/BoxComponent.h"
#include "Interface/ABCharacterItemInterface.h"
#include "Particles/ParticleSystemComponent.h"
#include "Physics/ABCollision.h"

// Sets default values
AABItemBox::AABItemBox()
{
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect"));

	// 액터 내부에 씬 셋업
	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	Effect->SetupAttachment(Trigger);

	// Trigger Setup
	Trigger->SetCollisionProfileName(CPROFILE_ABTRIGGER);  // Trigger용 전용 Collision Profile
	Trigger->SetBoxExtent(FVector(40, 42, 30));  // 박스 컴포넌트에 크기 지정
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnOverlapBegin);  // Dynamic Delegate -> 연결하는 함수는 UFUNCTION() 지정을 해줘야 함

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'"));
	if(BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}
	Mesh->SetRelativeLocation(FVector(0, -3.5, -30));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	// Effect Component
	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ArenaBattle/Effect/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
	if(EffectRef.Object)
	{
		Effect->SetTemplate(EffectRef.Object);  // 이펙트 지정
		Effect->bAutoActivate = false;  // 처음엔 발동하지 않도록
	}
}

void AABItemBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if(nullptr == Item)  // Item이 꽝이라면 즉시 파괴 후 return
	{
		Destroy();
		return;
	}
	
	// TODO: 아이템 획득 시, 획득한 액터에게 TakeItem 함수를 호출하도록 설정
	if(auto OverlappingPawn = Cast<IABCharacterItemInterface>(OtherActor))
	{
		// 이 클래스엔 아이템 정보가 없어서 추가 -> nullptr 검사 후 넘겨줌
		OverlappingPawn->TakeItem(Item);
	}
	
	// Trigger 발동 시 함수 호출 -> 이펙트 발동
	Effect->Activate(true);
	Mesh->SetHiddenInGame(true);  // 이펙트 발동 시 메시를 숨김
	SetActorEnableCollision(false);
	Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
}

void AABItemBox::OnEffectFinished(UParticleSystemComponent* ParticleSystem)
{
	Destroy();  // Actor가 스스로 없어지도록 선언
}
