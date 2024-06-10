// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ABGA_Skill.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLEGAS_API UABGA_Skill : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UABGA_Skill();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnComplete();

	UFUNCTION()
	void OnInterrupted();

protected:
	// 몽타주 값을 가져와야 함
	UPROPERTY()
	TObjectPtr<class UAnimMontage> ActiveSkillActionMontage;
};
