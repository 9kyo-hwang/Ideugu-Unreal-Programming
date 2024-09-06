// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "ABGameMode.h"
#include "Player/ABPlayerController.h"
#include "ArenaBattle.h"
#include "ABGameState.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

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

void AABGameMode::OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
}

FTransform AABGameMode::GetRandomStartTransform() const
{
	// 현재 레벨의 Player Start 배열을 가져와야 함
	if(PlayerStarts.IsEmpty())
	{
		return FTransform(FVector(0.0f, 0.0f, 0.0f));
	}

	int32 RandIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
	return PlayerStarts[RandIndex]->GetActorTransform();
}

void AABGameMode::StartPlay()
{
	Super::StartPlay();

	for(APlayerStart* PlayerStart : TActorRange<APlayerStart>(GetWorld()))
	{
		PlayerStarts.Add(PlayerStart);
	}
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
