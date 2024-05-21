// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/ABGA_Skill.h"
#include "Character/ABGASCharacterPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UABGA_Skill::UABGA_Skill()
{
}

void UABGA_Skill::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 액터 정보에서 아바타 정보 가져와 스킬 애니메이션 몽타주 재생
	AABGASCharacterPlayer* TargetCharacter = Cast<AABGASCharacterPlayer>(ActorInfo->AvatarActor.Get());
	if(!TargetCharacter)
	{
		return;
	}

	// AABGASCharacterPlayer에만 존재하는 스킬 액션 몽타주를 가져와 설정
	ActiveSkillActionMontage = TargetCharacter->GetSkillActionMontage();
	if(!ActiveSkillActionMontage)
	{
		return;
	}

	// 스킬 발동 중엔 movement 모드 = none
	TargetCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 애니메이션 재생 AbilityTask 실행
	// Skill Ability 소유 객체(this), 이름, 재생시킬 몽타주, 속도
	UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("SkillMontage"), ActiveSkillActionMontage, 1.0f);
	PlayMontageTask->OnCompleted.AddDynamic(this, &UABGA_Skill::OnComplete);
	PlayMontageTask->OnInterrupted.AddDynamic(this, &UABGA_Skill::OnInterrupted);

	PlayMontageTask->ReadyForActivation();  // 빼먹지 마!
}

void UABGA_Skill::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(AABGASCharacterPlayer* TargetCharacter = Cast<AABGASCharacterPlayer>(ActorInfo->AvatarActor.Get()))
	{
		// 스킬 발동 종료 시 walking 모드로 전환
		TargetCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UABGA_Skill::OnComplete()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UABGA_Skill::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}