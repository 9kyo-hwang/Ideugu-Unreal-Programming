// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

#include "GameData/ABGameSingleton.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent() : CurrentLevel(1), AttackRadius(50.0f)
{
	// 해당 필드를 true로 설정해줘야 InitializeComponent가 호출됨
	bWantsInitializeComponent = true;
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	SetLevelStat(CurrentLevel);
	SetHp(BaseStat.MaxHp);
}

void UABCharacterStatComponent::SetLevelStat(int32 NewLevel)
{
	CurrentLevel = FMath::Clamp(NewLevel, 1, UABGameSingleton::Get().CharacterMaxLevel);
	SetBaseStat(UABGameSingleton::Get().GetCharacterStat(CurrentLevel));
	check(BaseStat.MaxHp > 0);
}

float UABCharacterStatComponent::ApplyDamage(float InDamage)
{
	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(InDamage, 0, InDamage);  // 음수 보정
	SetHp(PrevHp - ActualDamage);
	
	if(CurrentHp <= KINDA_SMALL_NUMBER)
	{
		// TODO: Invoke Dead Event
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UABCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, BaseStat.MaxHp);
	// MULTICAST이므로 Broadcast. OneParam으로 했으므로 CurrentHp를 넘겨줌
	OnHpChanged.Broadcast(CurrentHp);
}