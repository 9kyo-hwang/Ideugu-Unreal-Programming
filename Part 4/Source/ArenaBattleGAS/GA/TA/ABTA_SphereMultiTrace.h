// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/TA/ABTA_Trace.h"
#include "ABTA_SphereMultiTrace.generated.h"

/**
 * 
 */
UCLASS()
class ARENABATTLEGAS_API AABTA_SphereMultiTrace : public AABTA_Trace
{
	GENERATED_BODY()

protected:
	// 기본 기능은 모두 동일해서 이 부분만 오버라이딩
	virtual FGameplayAbilityTargetDataHandle MakeTargetData() const override;
};
