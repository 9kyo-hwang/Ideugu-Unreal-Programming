// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_AttackHitCheck.h"
#include "Interface/ABAnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// Actor에 명령을 내려야 함 -> 매시 컴포넌트를 소유하고 있는 Owner를 가져와야 함
	if(!MeshComp)
		return;

	// 만약 메시를 소유한 오너가 우리가 선언한 캐릭터라면 공격 판정 명령
	// 캐릭터 베이스에서 구현한 인터페이스가 존재한다면 이 캐릭터가 맞음
	if(auto AttackPawn = Cast<IABAnimationAttackInterface>(MeshComp->GetOwner()))
	{
		AttackPawn->AttackHitCheck();
	}
}
