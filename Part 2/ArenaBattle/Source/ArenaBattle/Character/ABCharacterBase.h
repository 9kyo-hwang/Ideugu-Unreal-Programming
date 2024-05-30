// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/ABAnimationAttackInterface.h"
#include "Interface/ABCharacterWidgetInterface.h"
#include "ABCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quarter
};

UCLASS()
class ARENABATTLE_API AABCharacterBase : public ACharacter, public IABAnimationAttackInterface, public IABCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AABCharacterBase();

	virtual void PostInitializeComponents() override;

protected:
	virtual void SetCharacterControlData(const class UABCharacterControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category = CharacterControl, Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UABCharacterControlData*> CharacterControlManager;

	// Combo Action Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animation)
	TObjectPtr<class UAnimMontage> ComboActionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UABComboActionData> ComboActionData;
	
	void ProcessComboCommand();

	// 몽타주 시작 시 호출
	void ComboActionBegin();

	// 몽타주 종료 시 호출
	// 몽타주에 설정된 Delegate를 통해 호출될 수 있도록 파라미터 설정
	void ComboActionEnd(class UAnimMontage* TargetMontage, bool IsProperlyEnded);

	void SetComboCheckTimer();  // 타이머를 발동시킬 함수
	void ComboCheck();  // 타이머 발동 시 입력이 들어왔는 지 점검하는 함수

	int32 CurrentCombo = 0;  // 현재까지 진행된 콤보 수
	FTimerHandle ComboTimerHandle;
	bool HasNextComboCommand = false;  // 발동한 타이머 이전에 입력 커맨드가 들어왔는가

	// Attack Hit Section
protected:
	virtual void AttackHitCheck() override;
	// 언리얼 엔진 액터 설정에 기본으로 존재하는 함수
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Dead Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

	virtual void SetDead();
	void PlayDeadAnimation();

	float DeadEventDelayTime = 5.0f;

	// Stat Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UABCharacterStatComponent> Stat;
	
	
	// UI Widget Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Widget, meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UABWidgetComponent> HpBar;  // UABWidgetComponent로 변경!

	virtual void SetupCharacterWidget(UABUserWidget* InUserWidget) override;
};