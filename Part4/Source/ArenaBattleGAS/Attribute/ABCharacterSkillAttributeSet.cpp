// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/ABCharacterSkillAttributeSet.h"

UABCharacterSkillAttributeSet::UABCharacterSkillAttributeSet() :
SkillRange(800),
MaxSkillRange(1200),
SkillAttackRate(150),
MaxSkillAttackRate(300),
SkillCost(100),
MaxSkillCost(100)
{
	
}

void UABCharacterSkillAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if(Attribute == GetSkillRangeAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.1f, GetMaxSkillRange());  // range가 0인 경우에 대비
	}
	else if(Attribute == GetSkillAttackRateAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSkillAttackRate());
	}
}
