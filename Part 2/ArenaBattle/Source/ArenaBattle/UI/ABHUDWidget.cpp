// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHUDWidget.h"

#include "ABCharacterStatWidget.h"
#include "ABHpBarWidget.h"
#include "Interface/ABCharacterHUDInterface.h"

UABHUDWidget::UABHUDWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UABHUDWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	HpBar->UpdateStat(BaseStat, ModifierStat);
	CharacterStat->UpdateStat(BaseStat, ModifierStat);
}

void UABHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	HpBar->UpdateHpBar(NewCurrentHp);
}

void UABHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpBar = Cast<UABHpBarWidget>(GetWidgetFromName(TEXT("WidgetHpBar")));
	ensure(HpBar);

	CharacterStat = Cast<UABCharacterStatWidget>(GetWidgetFromName(TEXT("WidgetCharacterStat")));
	ensure(CharacterStat);

	// GetOwningPlayerPawn을 통해 컨트롤러가 빙의 중인 폰을 가져와
	if(auto HUDPawn = Cast<IABCharacterHUDInterface>(GetOwningPlayerPawn()))
	{
		// 인터페이스를 통해 폰으로 하여금 셋업하는 기능 추가
		HUDPawn->SetupHUDWidget(this);
	}
}
