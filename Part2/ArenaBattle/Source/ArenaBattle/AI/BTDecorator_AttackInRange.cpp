// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator_AttackInRange.h"

#include "ABAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/ABCharacterAIInterface.h"

UBTDecorator_AttackInRange::UBTDecorator_AttackInRange()
{
	NodeName = TEXT("CanAttack");
}

bool UBTDecorator_AttackInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult =  Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	// Pawn
	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if(!ControllingPawn)
	{
		return false;
	}

	// Interface
	auto AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if(!AIPawn)
	{
		return false;
	}

	// Target
	auto Target = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if(!Target)
	{
		return false;
	}

	// 플레이어와 NPC 간 거리 <= NPC의 공격 범위 == 공격 범위 안에 플레이어가 존재하면 공격 가능.
	return ControllingPawn->GetDistanceTo(Target) <= AIPawn->GetAIAttackRange();
}
