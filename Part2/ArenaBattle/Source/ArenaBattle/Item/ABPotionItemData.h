// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/ABCharacterStat.h"
#include "Item/ABItemData.h"
#include "ABPotionItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABPotionItemData : public UABItemData
{
	GENERATED_BODY()

public:
	UABPotionItemData();

	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ABItemData", GetFName());
	}
	
	UPROPERTY(EditAnywhere, Category=Hp)
	float HealAmount;
};