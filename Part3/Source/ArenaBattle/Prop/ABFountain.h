// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABFountain.generated.h"

UCLASS()
class ARENABATTLE_API AABFountain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABFountain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> Water;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;  // lifetime
	virtual void OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection) override;  // Logic Channel
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
	UPROPERTY(ReplicatedUsing=OnRep_ServerRotationYaw)  // 키워드 필요
	float ServerRotationYaw;

	// 속성이 변경됐을 때 호출될 콜백 함수
	UFUNCTION()
	void OnRep_ServerRotationYaw();

	float RotationRate = 30.0f;
	float ClientTimeSinceUpdate = 0.0f;  // 서버로부터 패킷을 받은 후 얼마나 시간이 흘렀는가
	float ClientTimeBetweenLastUpdate = 0.0f;  // 데이터를 받은 후 다시 데이터를 받기까지 걸린 시간
};
