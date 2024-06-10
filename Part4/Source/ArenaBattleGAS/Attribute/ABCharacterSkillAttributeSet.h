// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ABCharacterSkillAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class ARENABATTLEGAS_API UABCharacterSkillAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UABCharacterSkillAttributeSet();
	
	ATTRIBUTE_ACCESSORS(UABCharacterSkillAttributeSet, SkillRange)
	ATTRIBUTE_ACCESSORS(UABCharacterSkillAttributeSet, MaxSkillRange)
	ATTRIBUTE_ACCESSORS(UABCharacterSkillAttributeSet, SkillAttackRate)
	ATTRIBUTE_ACCESSORS(UABCharacterSkillAttributeSet, MaxSkillAttackRate)
	ATTRIBUTE_ACCESSORS(UABCharacterSkillAttributeSet, SkillCost)
	ATTRIBUTE_ACCESSORS(UABCharacterSkillAttributeSet, MaxSkillCost)
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category="Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillRange;

	UPROPERTY(BlueprintReadOnly, Category="Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillRange;

	UPROPERTY(BlueprintReadOnly, Category="Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillAttackRate;

	UPROPERTY(BlueprintReadOnly, Category="Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillAttackRate;

	UPROPERTY(BlueprintReadOnly, Category="Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SkillCost;

	UPROPERTY(BlueprintReadOnly, Category="Skill", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxSkillCost;
};