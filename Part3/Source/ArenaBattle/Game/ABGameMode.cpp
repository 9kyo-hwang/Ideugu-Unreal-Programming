// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/ABGameMode.h"
#include "ABGameMode.h"
#include "Player/ABPlayerController.h"
#include "ArenaBattle.h"
#include "ABGameState.h"
#include "ABPlayerState.h"
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
	PlayerStateClass = AABPlayerState::StaticClass();
}

void AABGameMode::OnPlayerKilled(AController* Killer, AController* KilledPlayer, APawn* KilledPawn)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	// 가해자 스코어를 1 증가
	if(APlayerState* KillerPlayerState = Killer->PlayerState)
	{
		KillerPlayerState->SetScore(KillerPlayerState->GetScore() + 1);

		// 스코어가 2 초과이면 매치 종료
		if(KillerPlayerState->GetScore() > 2)
		{
			FinishMatch();
		}
	}
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

void AABGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 1초 당 한 번씩 default game timer를 수행하도록 처리
	GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AABGameMode::DefaultGameTimer, GetWorldSettings()->GetEffectiveTimeDilation(), true);
}

void AABGameMode::DefaultGameTimer()
{
	// 매칭 게임의 상태 변경
	if(AABGameState* const ABGameState = Cast<AABGameState>(GameState); ABGameState->RemainingTime > 0)
	{
		ABGameState->RemainingTime--;
		AB_LOG(LogABNetwork, Log, TEXT("Remaining Time: %d"), ABGameState->RemainingTime);
		if(ABGameState->RemainingTime <= 0)
		{
			if(const FName CurrentMatchState = GetMatchState(); CurrentMatchState == MatchState::InProgress)
			{
				FinishMatch();
			}
			else if(CurrentMatchState == MatchState::WaitingPostMatch)
			{
				// 서버에 접속한 모든 클라이언트의 접속을 유지한 채 새 레벨로 이동
				GetWorld()->ServerTravel(TEXT("/Game/ArenaBattle/Maps/Part3Step2?listen"));
			}
		}
	}
}

void AABGameMode::FinishMatch()
{
	if(AABGameState* const ABGameState = Cast<AABGameState>(GameState); IsMatchInProgress())
	{
		// 호출 시 현재 매치 상태가 WaitingPostMatch로 변경
		// 다음 번 틱에서는 ServerTravel()이 발동돼야 하나, RemainingTime이 0보다 작기 때문에 reset 필요
		EndMatch();
		ABGameState->RemainingTime = ABGameState->ShowResultWaitingTime;  // 20초 동안 게임을 기다리면 종료 -> 5초를 기다리면 서버 트래블
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
