// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ABPlayerController.h"

// 게임 시작 시 동작하는 함수
void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 이렇게 해주면 게임 시작 시 포커스가 자동으로 뷰포트 안으로 들어감
	FInputModeGameOnly InputModeGameOnly;
	SetInputMode(InputModeGameOnly);
}