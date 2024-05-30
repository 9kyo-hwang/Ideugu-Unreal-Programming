// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ABComboActionData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABComboActionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UABComboActionData();

	UPROPERTY(EditAnywhere, Category=Name)
	FString MontageSectionNamePrefix;

	UPROPERTY(EditAnywhere, Category=Name)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category=Name)
	float FrameRate;  // 프레임의 기준 재생 속도

	UPROPERTY(EditAnywhere, Category=ComboData)
	TArray<float> EffectiveFrameCount;  // 사전에 입력됐는지 감지하는 프레임
};