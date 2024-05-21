// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/ABGA_AttackHitCheck.h"
#include "ArenaBattleGAS.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/AT/ABAT_Trace.h"
#include "GA/TA/ABTA_Trace.h"
#include "Attribute/ABCharacterAttributeSet.h"
#include "ArenaBattleGAS.h"
#include "Tag/ABGameplayTag.h"

UABGA_AttackHitCheck::UABGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UABGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 체크
	ABGAS_LOG(LogABGAS, Log, TEXT("%s"), *TriggerEventData->EventTag.GetTagName().ToString());
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	// Direct로 AABTA_Trace를 바로 넘겨주지 않고, 앞서 넘겨받은 TargetActorClass를 넘겨주도록 변경
	UABAT_Trace* AttackTraceTask = UABAT_Trace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UABGA_AttackHitCheck::OnTraceResult);
	AttackTraceTask->ReadyForActivation();
}

void UABGA_AttackHitCheck::OnTraceResult(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);
		ABGAS_LOG(LogABGAS, Log, TEXT("Target %s Detected"), *(HitResult.GetActor()->GetName()));

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
		//if (!SourceASC || !TargetASC)
		//{
		//	ABGAS_LOG(LogABGAS, Error, TEXT("ASC not found!"));
		//	return;
		//}

		const UABCharacterAttributeSet* SourceAttribute = SourceASC->GetSet<UABCharacterAttributeSet>();
		//UABCharacterAttributeSet* TargetAttribute = const_cast<UABCharacterAttributeSet*>(TargetASC->GetSet<UABCharacterAttributeSet>());
		//if (!SourceAttribute || !TargetAttribute)
		//{
		//	ABGAS_LOG(LogABGAS, Error, TEXT("ASC not found!"));
		//	return;
		//}

		//const float AttackDamage = SourceAttribute->GetAttackRate();
		//TargetAttribute->SetHealth(TargetAttribute->GetHealth() - AttackDamage);

		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
		if (EffectSpecHandle.IsValid())
		{
			//EffectSpecHandle.Data->SetSetByCallerMagnitude(ABTAG_DATA_DAMAGE, -SourceAttribute->GetAttackRate());			
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

			FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
			CueContextHandle.AddHitResult(HitResult);
			FGameplayCueParameters CueParam;
			CueParam.EffectContext = CueContextHandle;

			TargetASC->ExecuteGameplayCue(ABTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, CueParam);
		}

		FGameplayEffectSpecHandle BuffEffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackBuffEffect);
		if (BuffEffectSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffEffectSpecHandle);
		}
	}
	else if(UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))  // Actor Array가 넘어온 경우
	{
		// 다수의 Actor들이 배열로 넘어왔기 때문에, 각 Actor들에 대해 데미지를 부여하면 됨
		auto SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
		auto AttackDamageSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
		if(AttackDamageSpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, AttackDamageSpecHandle, TargetDataHandle);

			auto CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(AttackDamageSpecHandle);
			CueContextHandle.AddActors(TargetDataHandle.Data[0].Get()->GetActors(), false);  // TWeakObjectPtr.

			FGameplayCueParameters Parameters;
			Parameters.EffectContext = CueContextHandle;
			SourceASC->ExecuteGameplayCue(ABTAG_GAMEPLAYCUE_CHARACTER_ATTACKHIT, Parameters);  // Target이 여럿이라 Source에서 Effect를 재생함
		}
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
