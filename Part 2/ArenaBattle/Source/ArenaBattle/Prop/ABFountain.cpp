// Fill out your copyright notice in the Description page of Project Settings.

#include "Prop/ABFountain.h"

// Sets default values
AABFountain::AABFountain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// TODO: 컴포넌트 부착
	// 여기서는 더 이상 전방선언이 불가능하므로 헤더 include 필요
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Water"));

	// TODO: Root Component 지정
	RootComponent = Body;
	Water->SetupAttachment(Body);  // Water는 Body의 자식 컴포넌트이므로 부착
	Water->SetRelativeLocation(FVector(0.0f, 0.0f, 132.0f));  // 부모 기준 좌표 조정

	// TODO: 컴포넌트 안에 Static Mesh 지정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyStaticMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01'"));
	if(BodyStaticMeshRef.Object)
	{
		Body->SetStaticMesh(BodyStaticMeshRef.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WaterStaticMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ArenaBattle/Environment/Props/SM_Plains_Fountain_02.SM_Plains_Fountain_02'"));
	if(WaterStaticMeshRef.Object)
	{
		Water->SetStaticMesh(WaterStaticMeshRef.Object);
	}
}

// Called when the game starts or when spawned
void AABFountain::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AABFountain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

