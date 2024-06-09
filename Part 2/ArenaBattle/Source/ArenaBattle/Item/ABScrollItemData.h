// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameData/ABCharacterStat.h"
#include "Item/ABItemData.h"
#include "ABScrollItemData.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABScrollItemData : public UABItemData
{
	GENERATED_BODY()

public:
	UABScrollItemData();
	FORCEINLINE virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ABItemData", GetFName());
	}

	// 기본 스탯을 늘려주는 역할을 하기 때문에, 캐릭터 스탯이 필요함
	UPROPERTY(EditAnywhere, Category=Stat)
	FABCharacterStat BaseStat;
};
