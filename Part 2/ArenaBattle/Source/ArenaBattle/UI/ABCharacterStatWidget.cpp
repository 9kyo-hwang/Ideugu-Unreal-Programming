// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ABCharacterStatWidget.h"

#include "Components/TextBlock.h"

void UABCharacterStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// FABCharacterStat이 가지는 속성들을 읽어 매칭되는 텍스트 블록의 포인터를 가져옴
	for(TFieldIterator<FNumericProperty> PropIt(FABCharacterStat::StaticStruct()); PropIt; ++PropIt)
	{
		const FName PropKey(PropIt->GetName());
		const FName TextBaseControlName = *FString::Printf(TEXT("Txt%sBase"), *PropIt->GetName());
		const FName TextModifierControlName = *FString::Printf(TEXT("Txt%sModifier"), *PropIt->GetName());

		// LookupTable에 저장
		if(auto BaseTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextBaseControlName)))
		{
			BaseLookup.Add(PropKey, BaseTextBlock);
		}

		if(auto ModifierTextBlock = Cast<UTextBlock>(GetWidgetFromName(TextModifierControlName)))
		{
			ModifierLookup.Add(PropKey, ModifierTextBlock);
		}
	}
}

void UABCharacterStatWidget::UpdateStat(const FABCharacterStat& BaseStat, const FABCharacterStat& ModifierStat)
{
	// TODO: 값이 변경되면 Lookup 테이블을 이용해 컨트롤러의 값을 업데이트
	for(TFieldIterator<FNumericProperty> PropIt(FABCharacterStat::StaticStruct()); PropIt; ++PropIt)
	{
		const FName PropKey(PropIt->GetName());

		float BaseData = 0.0f;
		PropIt->GetValue_InContainer(&BaseStat, &BaseData);
		float ModifierData = 0.0f;
		PropIt->GetValue_InContainer(&ModifierStat, &ModifierData);

		if(auto BaseTextBlockPtr = BaseLookup.Find(PropKey))
		{
			(*BaseTextBlockPtr)->SetText(FText::FromString(FString::SanitizeFloat(BaseData)));
		}

		if(auto ModifierTextBlockPtr = ModifierLookup.Find(PropKey))
		{
			(*ModifierTextBlockPtr)->SetText(FText::FromString(FString::SanitizeFloat(ModifierData)));
		}
	}
}
