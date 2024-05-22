// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ABCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8 { Shoulder, Quarter };

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

protected:
	virtual void SetCharacterDataAsset(const class UABCharacterDataAsset* CharacterDataAsset);

	// Enum 데이터를 키로 해서 컨트롤 데이터 에셋을 받아올 수 있도록 변수 선언
	UPROPERTY(EditAnywhere, Category=CharacterControl, meta=(AllowPrivateAccess="true"))
	TMap<ECharacterControlType, class UABCharacterDataAsset*> CharacterControlManager;
};