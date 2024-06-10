// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ABCharacterBase.h"
#include "Engine/StreamableManager.h"
#include "Interface/ABCharacterAIInterface.h"
#include "ABCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS(Config=ArenaBattle)  // Config 폴더에 Default"ArenaBattle".ini 파일을 사용함
class ARENABATTLE_API AABCharacterNonPlayer : public AABCharacterBase, public IABCharacterAIInterface
{
	GENERATED_BODY()

public:
	AABCharacterNonPlayer();

protected:
	// 로딩은 여기에서 진행
	virtual void PostInitializeComponents() override;

protected:
	void SetDead() override;
	void NPCMeshLoadCompleted();  // 로딩이 완료됐을 때 신호를 받는 함수

	UPROPERTY(Config)
	TArray<FSoftObjectPath> NPCMeshes;  // 경로들을 저장함

	TSharedPtr<FStreamableHandle> NPCMeshHandle;  // 비동기 로드

	// AI Section
protected:
	virtual float GetAIAttackRange() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIPatrolRadius() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FAICharacterAttackFinished OnAttackFinished;  // 전달받은 델리게이트 저장 변수

	virtual void NotifyComboActionEnd() override;
};