// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/ABGE_SkillDamageExecCalc.h"
#include "AbilitySystemComponent.h"
#include "Attribute/ABCharacterSkillAttributeSet.h"
#include "Attribute/ABCharacterAttributeSet.h"  // 최종적으로 Damage 어트리뷰트로 반환해야 하기 때문

// ExecutionParams: Effect를 발동시키는 정보
// OutExecutionOutput: 바꿀 Attribute 값들에 대해, 최종 데이터를 넘겨주면 됨
void UABGE_SkillDamageExecCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	auto SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	auto TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	if(!SourceASC || !TargetASC)
	{
		return;
	}

	auto SourceActor = SourceASC->GetAvatarActor();
	auto TargetActor = TargetASC->GetAvatarActor();
	if(!SourceASC || !TargetActor)
	{
		return;
	}

	const float MaxDamageRange = SourceASC->GetNumericAttributeBase(UABCharacterSkillAttributeSet::GetSkillRangeAttribute());
	const float MaxDamage = SourceASC->GetNumericAttributeBase(UABCharacterSkillAttributeSet::GetSkillAttackRateAttribute());
	const float Distance = FMath::Clamp(SourceActor->GetDistanceTo(TargetActor), 0, MaxDamageRange);  // 멀 수록 데미지가 강해지기 때문에
	const float InverseDamageRatio = 1.0f - Distance / MaxDamageRange;  // 뒤집어서 계산하도록 역산 비율 정의
	float Damage = MaxDamage * InverseDamageRatio;  // 곡선 형태로 데미지가 떨어지도록 하는 것이 더 좋음

	// 캐릭터의 "데미지" 어트리뷰트에 넘겨줘야 함.
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UABCharacterAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, Damage));
}
