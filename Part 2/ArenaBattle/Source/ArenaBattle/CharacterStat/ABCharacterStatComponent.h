// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	
	FORCEINLINE float GetMaxHp() { return MaxHp; }
	FORCEINLINE float GetCurrentHp() { return CurrentHp; }
	float ApplyDamage(float InDamage);

protected:
	// HP값 변동 시 실행될 함수
	void SetHp(float NewHp);
	
	// 배치한 캐릭터들마다 다른 값을 가지기 때문에 인스턴스마다 별도로 수행되는 것이 좋음: VisibleInstanceOnly
	UPROPERTY(VisibleInstanceOnly, Category=Stat)
	float MaxHp;

	// Stat Component Object들은 Disk에 저장됨. 그런데 Hp같은 스탯은 새로 지정되기 때문에 저장할 필요가 없음: Transient
	UPROPERTY(Transient, VisibleInstanceOnly, Category=Stat)
	float CurrentHp;
};