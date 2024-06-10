// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ABUserWidget.h"
#include "ABHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLE_API UABHpBarWidget : public UABUserWidget
{
	GENERATED_BODY()

public:
	UABHpBarWidget(const FObjectInitializer& ObjectInitializer);  // 이 생성자만 지원
	
	void UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat);
	void UpdateHpBar(float NewCurrentHp);
	FString GetHpStatText();  // Stat 업데이트 시 "100 / 100" 형태로 String을 만들어주는 함수
	
protected:
	virtual void NativeConstruct() override; 
	
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	TObjectPtr<class UTextBlock> HpStat;

	UPROPERTY()  // HpProgressBar 업데이트를 위해 이제부터 현재 HP 값을 보관
	float CurrentHp;

	UPROPERTY()
	float MaxHp;
};
