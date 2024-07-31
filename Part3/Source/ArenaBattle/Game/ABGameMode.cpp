// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "ABGameMode.h"
#include "Player/ABPlayerController.h"
#include "ArenaBattle.h"
#include "ABGameState.h"

AABGameMode::AABGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/Blueprint/BP_ABCharacterPlayer.BP_ABCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ArenaBattle.ABPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	GameStateClass = AABGameState::StaticClass();  // GameMode ���� �� ���� ������ GameState�� �⺻���� �����ϴ� ���� �ʿ�
}

void AABGameMode::OnPlayerDead()
{

}

// void AABGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
// {
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("================================================================================"));
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
// 	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
// 	//ErrorMessage = TEXT("Server is full.");
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
// }
//
// APlayerController* AABGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
// {
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
// 	auto NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
//
// 	return NewPlayerController;
// }
//
// void AABGameMode::PostLogin(APlayerController* NewPlayer)
// {
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
// 	Super::PostLogin(NewPlayer);
//
// 	if(UNetDriver* NetDriver = GetNetDriver())
// 	{
// 		if(NetDriver->ClientConnections.Num() == 0)
// 		{
// 			AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No Client Connection"));
// 		}
// 		else
// 		{
// 			for(const auto& ClientConnection : NetDriver->ClientConnections)
// 			{
// 				AB_LOG(LogABNetwork, Log, TEXT("Client Connections: %s"), *ClientConnection->GetName());
// 			}
// 		}
// 	}
// 	else
// 	{
// 		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No NetDriver"));
// 	}
//
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
// }
//
// void AABGameMode::StartPlay()
// {
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
// 	Super::StartPlay();
// 	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
// }
