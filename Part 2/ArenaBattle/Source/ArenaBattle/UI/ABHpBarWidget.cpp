// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABHpBarWidget.h"

#include "Components/ProgressBar.h"
#include "Interface/ABCharacterWidgetInterface.h"

UABHpBarWidget::UABHpBarWidget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer), MaxHp(-1.0f)
{
	
}

void UABHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	// 초기값이 -1이기 때문에 검증
	ensure(MaxHp > 0.0f);

	if(HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}

void UABHpBarWidget::NativeConstruct()
{
	// 함수 호출 시 UI 관련 모든 기능이 초기화 완료됨
	Super::NativeConstruct();

	// HpProgressBar에 대한 정보를 받아오면 됨
	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(HpProgressBar);  // 반드시 존재해야 함

	// UABUserWidget을 상속받고 난 뒤 OwningActor를 사용할 수 있게 됨

	// TODO: OwningActor에다가 UpdateHpBar 함수를 전달해 델리게이트에 등록 -> Stat 업데이트 시 함수 호출, HpBar 변경
	if(auto CharacterWidget = Cast<IABCharacterWidgetInterface>(OwningActor))
	{
		CharacterWidget->SetupCharacterWidget(this);  // HpBar인 나 자신을 넘겨서 OwningActor에 바인딩
	}
}
