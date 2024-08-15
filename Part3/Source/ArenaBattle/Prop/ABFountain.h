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
	// virtual void OnActorChannelOpen(FInBunch& InBunch, UNetConnection* Connection) override;  // Logic Channel
	// virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	// virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	
	UPROPERTY(ReplicatedUsing=OnRep_ServerRotationYaw)  // 키워드 필요
	float ServerRotationYaw;

	UPROPERTY(ReplicatedUsing=OnRep_ServerLightColor)
	FLinearColor ServerLightColor;

	// 네트워크 대역폭 테스트
	// UPROPERTY(Replicated)
	// TArray<float> BigData;
	//
	// float BigDataElement = 0.0f;

	// 속성이 변경됐을 때 호출될 콜백 함수
	UFUNCTION()
	void OnRep_ServerRotationYaw();

	UFUNCTION()
	void OnRep_ServerLightColor();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastRPCChangeLightColor(const FLinearColor& NewLightColor);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerRPCChangeLightColor();

	UFUNCTION(Client, Unreliable)
	void ClientRPCChangeLightColor(const FLinearColor& NewLightColor);

	float RotationRate = 30.0f;
	float ClientTimeSinceUpdate = 0.0f;  // 서버로부터 패킷을 받은 후 얼마나 시간이 흘렀는가
	float ClientTimeBetweenLastUpdate = 0.0f;  // 데이터를 받은 후 다시 데이터를 받기까지 걸린 시간
};
