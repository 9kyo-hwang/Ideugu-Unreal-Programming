// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterMovementComponent.h"

#include "ArenaBattle.h"
#include "GameFramework/Character.h"

void FABSavedMove_Character::Clear()
{
	// 언리얼 오브젝트가 아니므로 Super란 키워드가 아님!
	// 이를 해결하기 위해 클래스 선언에서 typedef / using을 이용해 type alias를 진행함
	// __super::Clear(); // 참고로 VC++7.0에서 __super 키워드를 지원함
	Super::Clear();

	bPressedTeleport = false;
	bDidTeleport = false;
}

void FABSavedMove_Character::SetInitialPosition(ACharacter* Character)
{
	Super::SetInitialPosition(Character);

	// 텔레포트 전에 CMC에 설정된 프로퍼티 값을 캐릭터 움직임 데이터에 저장
	if(UABCharacterMovementComponent* ABCMC = Cast<UABCharacterMovementComponent>(Character->GetCharacterMovement()))
	{
		bPressedTeleport = ABCMC->bPressedTeleport;
		bDidTeleport = ABCMC->bDidTeleport;
	}
}

uint8 FABSavedMove_Character::GetCompressedFlags() const
{
	uint8 Flags = Super::GetCompressedFlags();
	
	if(bPressedTeleport)
	{
		Flags |= FLAG_Custom_0;
	}
	if(bDidTeleport)
	{
		Flags |= FLAG_Custom_1;
	}
	
	return Flags;
}

FABNetworkPredictionData_Client_Character::FABNetworkPredictionData_Client_Character(
	const UCharacterMovementComponent& ClientCMC)
		: Super(ClientCMC)
{
}

FSavedMovePtr FABNetworkPredictionData_Client_Character::AllocateNewMove()
{
	// Super 로직과 동일하나, 우리의 FABSavedMove_Character로 변경
	return FSavedMovePtr(new FABSavedMove_Character());
}

UABCharacterMovementComponent::UABCharacterMovementComponent()
	: bPressedTeleport(false), bDidTeleport(false), TeleportOffset(600.0f), TeleportCooldown(3.0f)
{
}

void UABCharacterMovementComponent::SetTeleportCommand()
{
	// 텔레포트 입력 명령이 들어오면 참
	bPressedTeleport = true;
}

void UABCharacterMovementComponent::ABTeleport()
{
	if(!CharacterOwner)
	{
		return;
	}

	AB_SUBLOG(LogABTeleport, Log, TEXT("%s"), TEXT("Teleport Begin"));

	FVector DestLocation = CharacterOwner->GetActorLocation() + CharacterOwner->GetActorForwardVector() * TeleportOffset;
	// bIsATest: 테스트할 것인가. bNoCheck: 경로 도중에 장애물이 있는 지 확인할 것인가
	CharacterOwner->TeleportTo(DestLocation, CharacterOwner->GetActorRotation(), false, true);
	bDidTeleport = true;

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
	{
		bDidTeleport = false;  // 쿨타임 이후에 [텔레포트 실행 중] 플래그 초기화
		AB_SUBLOG(LogABTeleport, Log, TEXT("%s"), TEXT("Teleport End"));
	}), TeleportCooldown, false, -1.0f);  // 반복없이 쿨타임 이후에 람다 함수 호출
}

void UABCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	if(bPressedTeleport && !bDidTeleport)  // 키를 눌렀으면서 텔레포트 중이 아닐 때만 수행
	{
		ABTeleport();
	}

	if(bPressedTeleport) bPressedTeleport = false;  // 그 후 플래그 초기화
}

void UABCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	if(!CharacterOwner || CharacterOwner->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	// GetCompressedFlags에서 인코딩한 정보를 디코딩
	bPressedTeleport = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bDidTeleport = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

	if(bPressedTeleport && !bDidTeleport)
	{
		AB_SUBLOG(LogABTeleport, Log, TEXT("%s"), TEXT("Teleport Begin"));
		ABTeleport();
	}
}

FNetworkPredictionData_Client* UABCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UABCharacterMovementComponent* MutableThis = const_cast<UABCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FABNetworkPredictionData_Client_Character(*this);
	}

	return ClientPredictionData;
}
