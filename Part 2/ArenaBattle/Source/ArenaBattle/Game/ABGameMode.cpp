// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"

AABGameMode::AABGameMode()
{
	// TODO: DefaultPawnClass에 대한 설정
	// BP_ThirdPersonCharacter를 지정할 것이기 때문에, Reference 경로를 가져와서 설정
	// 에셋 경로만 지정해주면 됨. Script/Engine.Blueprint 부분은 필요없으며, Class 정보를 가져와야 해서 맨 뒤에 _C 접미사를 붙여야 함.
	static ConstructorHelpers::FClassFinder<APawn> ThirdPersonClassRef(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C"));
	if(ThirdPersonClassRef.Class != nullptr)
	{
		DefaultPawnClass = ThirdPersonClassRef.Class;
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