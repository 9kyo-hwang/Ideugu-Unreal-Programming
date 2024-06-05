// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_TurnToTarget.h"

#include "ABAI.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interface/ABCharacterAIInterface.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());
	if(!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	auto TargetPawn = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(BBKEY_TARGET));
	if(!TargetPawn)
	{
		return EBTNodeResult::Failed;
	}

	auto AIPawn = Cast<IABCharacterAIInterface>(ControllingPawn);
	if(!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	float TurnSpeed = AIPawn->GetAITurnSpeed();
	FVector LookVector = TargetPawn->GetActorLocation() - ControllingPawn->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRotator = FRotationMatrix::MakeFromX(LookVector).Rotator();

	// 서서히 해당 로테이션으로 회전
	ControllingPawn->SetActorRotation(FMath::RInterpTo(ControllingPawn->GetActorRotation(), TargetRotator, GetWorld()->GetDeltaSeconds(), TurnSpeed));
	return EBTNodeResult::Succeeded;
}