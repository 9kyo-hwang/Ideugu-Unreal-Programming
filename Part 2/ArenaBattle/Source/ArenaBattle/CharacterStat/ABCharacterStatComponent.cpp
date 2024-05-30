// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStat/ABCharacterStatComponent.h"

// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent() : MaxHp(200.0f), CurrentHp(MaxHp)
{
	
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHp(MaxHp);
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
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
	// MULTICAST이므로 Broadcast. OneParam으로 했으므로 CurrentHp를 넘겨줌
	OnHpChanged.Broadcast(CurrentHp);
}
