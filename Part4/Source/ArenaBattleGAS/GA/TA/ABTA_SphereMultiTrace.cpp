// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/ABTA_SphereMultiTrace.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ArenaBattleGAS.h"
#include "GameFramework/Character.h"
#include "Physics/ABCollision.h"
#include "Attribute/ABCharacterSkillAttributeSet.h"

FGameplayAbilityTargetDataHandle AABTA_SphereMultiTrace::MakeTargetData() const
{
	// 캐릭터 받아오기
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);
	auto ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if(!ASC)
	{
		ABGAS_LOG(LogABGAS, Error, TEXT("ASC not found!"));
		return FGameplayAbilityTargetDataHandle();
	}

	// 스킬 어트리뷰트 가져오기
	auto SkillAttribute = ASC->GetSet<UABCharacterSkillAttributeSet>();
	if(!SkillAttribute)
	{
		ABGAS_LOG(LogABGAS, Error, TEXT("SkillAttribute not found!"));
		return FGameplayAbilityTargetDataHandle();
	}
	
	// 구체와 겹친 액터들의 정보를 받아옴
	TArray<FOverlapResult> Overlaps;
	const float SkillRadius = SkillAttribute->GetSkillRange();
	
	FVector Origin = Character->GetActorLocation();
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AABTA_SphereMultiTrace), false, Character);
	GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(SkillRadius), Params);

	// Target Data Handle에 넣어줌: Target Data를 만들어야 함
	// TargetDataHandles의 SetActors가 약포인터 형태로 받기 때문에 해당 형태로 만들어줘야 함.
	TArray<TWeakObjectPtr<AActor>> HitActors;
	for(const FOverlapResult& Overlap : Overlaps)
	{
		if(AActor* HitActor = Overlap.OverlapObjectHandle.FetchActor<AActor>(); HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
		}
	}
	
	FGameplayAbilityTargetData_ActorArray* HitActorsTargetData = new FGameplayAbilityTargetData_ActorArray();
	HitActorsTargetData->SetActors(HitActors);

#if ENABLE_DRAW_DEBUG
	if(bShowDebug)
	{
		FColor DrawColor = HitActors.Num() > 0 ? FColor::Green : FColor::Red;
		DrawDebugSphere(GetWorld(), Origin, SkillRadius, 16, DrawColor, false, 5.0f);
	}
#endif
	
	return FGameplayAbilityTargetDataHandle(HitActorsTargetData);
}
