// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"

AABGameMode::AABGameMode()
{
	// TODO: ThirdPerson이 아닌 ABCharacterPlayer를 스폰하도록 변경
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/ArenaBattle.ABCharacterPlayer"));
	if(DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}
	
	// TODO: PlayerControllerClass에 대한 설정
	// 이 자체가 클래스이기 때문에 별도로 경로를 수정할 것은 없음(끝에 작은 따옴표 하나만 제거)
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattle.ABPlayerController"));
	if(PlayerControllerClassRef.Class != nullptr)
	{
		// 이런 식으로 에셋으로부터 직접 참조받기 때문에 헤더 의존성을 제거할 수 있음
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}