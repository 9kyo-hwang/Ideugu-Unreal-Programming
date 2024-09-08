// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ABCharacterMovementComponent.generated.h"

class FABSavedMove_Character : public FSavedMove_Character
{
	using Super = FSavedMove_Character;
	
public:
	virtual void Clear() override;  // 움직임 초기화
	virtual void SetInitialPosition(ACharacter* Character) override;  // 최초 위치와 (텔레포트)상태 지정
	virtual uint8 GetCompressedFlags() const override;  // 클라이언트 상태 정보를 플래그 정보로 바꿔서 서버에 보내는 용도

	uint8 bPressedTeleport : 1;  // 상태를 저장해야 하므로
	uint8 bDidTeleport : 1;  // CMC에서 선언한 변수와 동일하게 지정
};

class FABNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;
	
public:
	FABNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientCMC);
	virtual FSavedMovePtr AllocateNewMove() override;
};

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UABCharacterMovementComponent();
	void SetTeleportCommand();

public:
	uint8 bPressedTeleport : 1;  // 클라이언트의 입력이 들어오면 참
	uint8 bDidTeleport : 1;  // 텔레포트가 실행되어 쿨타임이 도는 동안 참

protected:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	
	virtual void ABTeleport();
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
protected:
	UPROPERTY()
	float TeleportOffset;

	UPROPERTY()
	float TeleportCooldown;
};
