// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ABAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UABAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;  // 처음 생성될 때
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;  // 매 프레임마다 호출

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	TObjectPtr<class ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	FVector Velocity;  // 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	float GroundSpeed;  // 지상 속력

	// 사이즈를 추정할 수 없어서 bool 타입을 쓰지 않고, uint8을 사용함.
	// 접두사 b를 붙여야 하지만, 여기서는 생략. 또 비트 플래그를 달아줌.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	uint8 IsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	float MovingThreshold;  // 움직이고 있는 지를 점검하는 쓰레시홀드 값

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	uint8 IsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	uint8 IsJumping : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	float JumpingThreshold;  // 점프중인 지를 점검하는 쓰레시홀드 값
};