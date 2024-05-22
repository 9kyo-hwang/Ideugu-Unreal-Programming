// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "InputActionValue.h"
#include "ABCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API AABCharacterPlayer : public AABCharacterBase
{
	GENERATED_BODY()

public:
	AABCharacterPlayer();

protected:
	virtual void BeginPlay() override;  // 입력 매핑 컨텍스트 할당 역할

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;  // Action, Function을 매핑시키는 역할

	// Character Control Section
protected:
	void ChangeCharacterControl();
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterDataAsset(const UABCharacterDataAsset* CharacterDataAsset) override;

	// Camera Section
protected:
	// 카메라를 지탱해주는 지지대 역할
	// meta 정보를 통해 blueprint에서도 private field를 접근할 수 있음
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	// 실제 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

	// Input Section
protected:
	// 에셋들을 다른 에셋으로 변경할 수 있도록 하기 위해 EditAnywhere
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UInputAction> ChangeControlAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UInputAction> QuarterMoveAction;

	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);
	void QuarterMove(const FInputActionValue& Value);

	ECharacterControlType CurrentCharacterControlType;
};
