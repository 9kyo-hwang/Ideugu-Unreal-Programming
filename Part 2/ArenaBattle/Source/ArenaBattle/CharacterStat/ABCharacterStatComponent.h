// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameData/ABCharacterStat.h"
#include "ABCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);  // 여러 객체가 등록할 수 있음. Hp == 0라는 시그널만 보냄(구조체)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);  // 변경된 현재 Hp 값을 보내도록 파라미터 지정

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARENABATTLE_API UABCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UABCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;
	
	void SetLevelStat(int32 NewLevel);
	FORCEINLINE float GetCurrentLevel() const { return CurrentLevel; }
	// 무기 획득 시 Modifier 스탯을 변경할 수 있도록
	FORCEINLINE void SetModifierStat(const FABCharacterStat& InModifierStat) { ModifierStat = InModifierStat; }
	// FABCharacterStat에서 정의한 operator+ 오버로딩을 이용해 Base + Modifier 값을 반환
	FORCEINLINE FABCharacterStat GetTotalStat() const { return BaseStat + ModifierStat; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	FORCEINLINE float GetAttackRadius() const { return AttackRadius; }
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