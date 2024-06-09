// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);  // 여러 객체가 등록할 수 있음. Hp == 0라는 시그널만 보냄(구조체)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);  // 변경된 현재 Hp 값을 보내도록 파라미터 지정
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChangedDelegate, const FABCharacterStat& /*BaseStat*/, const FABCharacterStat& /*ModifierStat*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	virtual void InitializeComponent() override;  // 초기화 시점 이동

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;
	FOnStatChangedDelegate OnStatChanged;

	FORCEINLINE const FABCharacterStat& GetBaseStat() const { return BaseStat; }
	FORCEINLINE const FABCharacterStat& GetModifierStat() const { return ModifierStat; }
	FORCEINLINE FABCharacterStat GetTotalStat() const { return BaseStat + ModifierStat; }
	FORCEINLINE void AddBaseStat(const FABCharacterStat& InAddStat) { BaseStat = BaseStat + InAddStat; OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat()); }
	FORCEINLINE void SetBaseStat(const FABCharacterStat& InBaseStat) { BaseStat = InBaseStat; OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat()); }
	FORCEINLINE void SetModifierStat(const FABCharacterStat& InModifierStat) { ModifierStat = InModifierStat; OnStatChanged.Broadcast(GetBaseStat(), GetModifierStat()); }
	FORCEINLINE void HealHp(float InHealAmount) { CurrentHp = FMath::Clamp(CurrentHp + InHealAmount, 0, GetTotalStat().MaxHp); OnHpChanged.Broadcast(CurrentHp); }
	FORCEINLINE float GetCurrentLevel() const { return CurrentLevel; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	FORCEINLINE float GetAttackRadius() const { return AttackRadius; }
		
	void SetLevelStat(int32 NewLevel);
	float ApplyDamage(float InDamage);

protected:
	// HP값 변동 시 실행될 함수
	void SetHp(float NewHp);
	
	// MaxHp는 Base Stat으로 대체되기 때문에 삭제

	// 아래 4가지 속성들은 캐릭터가 초기화될 때마다 언제든지 바뀔 수 있기 때문에, Transient로 저장하지 않도록 설정 & 에디터에서는 읽기 전용
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat)
	float CurrentHp;
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat)
	float CurrentLevel;  // 캐릭터 스탯은 현재 레벨 정보를 기반으로 Singleton으로부터 제공받음
	UPROPERTY(VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	float AttackRadius;
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	FABCharacterStat BaseStat;
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat, meta=(AllowPrivateAccess="true"))
	FABCharacterStat ModifierStat;
};