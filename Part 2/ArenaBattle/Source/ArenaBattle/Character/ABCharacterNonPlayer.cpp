// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterNonPlayer.h"
#include "Engine/AssetManager.h"

AABCharacterNonPlayer::AABCharacterNonPlayer()
{
	GetMesh()->SetHiddenInGame(true);  // 메시가 로딩될 때까지는 Hidden == true
}

void AABCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ensure(NPCMeshes.Num() > 0);
	int32 RandIndex = FMath::RandRange(0, NPCMeshes.Num() - 1);
	NPCMeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[RandIndex], FStreamableDelegate::CreateUObject(this, &AABCharacterNonPlayer::NPCMeshLoadCompleted));
}

void AABCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	FTimerHandle DeadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([&]
	{
		Destroy();
	}), DeadEventDelayTime, false);
}

void AABCharacterNonPlayer::NPCMeshLoadCompleted()
{
	if(NPCMeshHandle.IsValid())  // 핸들이 유효하면 
	{
		if(auto NPCMesh = Cast<USkeletalMesh>(NPCMeshHandle->GetLoadedAsset()))  // 메시 로딩
		{
			GetMesh()->SetSkeletalMesh(NPCMesh);  // 메시 지정
			GetMesh()->SetHiddenInGame(false);  // 로딩 시 메시가 보이도록
		}
	}
	NPCMeshHandle->ReleaseHandle();  // 핸들 해제
}