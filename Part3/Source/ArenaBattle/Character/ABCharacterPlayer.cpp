// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterControlData.h"
#include "UI/ABHUDWidget.h"
#include "CharacterStat/ABCharacterStatComponent.h"
#include "Interface/ABGameInterface.h"
#include "ABCharacterPlayer.h"

#include "ABCharacterMovementComponent.h"
#include "ArenaBattle.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "Physics/ABCollision.h"

AABCharacterPlayer::AABCharacterPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UABCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputChangeActionControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if (nullptr != InputChangeActionControlRef.Object)
	{
		ChangeControlAction = InputChangeActionControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuaterMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuaterMove.IA_QuaterMove'"));
	if (nullptr != InputActionQuaterMoveRef.Object)
	{
		QuaterMoveAction = InputActionQuaterMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Attack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionTeleportRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Teleport.IA_Teleport'"));
	if(nullptr != InputActionTeleportRef.Object)
	{
		TeleportAction = InputActionTeleportRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Quater;
	bCanAttack = true;
}

void AABCharacterPlayer::BeginPlay()
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		EnableInput(PlayerController);
	}

	SetCharacterControl(CurrentCharacterControlType);
}

void AABCharacterPlayer::SetDead()
{
	// Super에서는 죽는 애니메이션 재생 후, HpBar Hidden, Collision = false 등 설정
	Super::SetDead();

	// ResetPlayer에서 이를 다 해제하는 기능을 구현함. 따라서 타이머를 걸어 5초 후에 ResetPlayer를 호출하도록 설정
	GetWorldTimerManager().SetTimer(DeadTimerHandle, this, &AABCharacterPlayer::ResetPlayer, 5.0f, false);

	// if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	// {
	// 	DisableInput(PlayerController);
	// }
}

void AABCharacterPlayer::PossessedBy(AController *NewController)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	AActor* OwnerActor = GetOwner();
	if(OwnerActor)
	{
		AB_LOG(LogABNetwork, Log, TEXT("Owner: %s"), *OwnerActor->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No Owner"));
	}

	Super::PossessedBy(NewController);

	OwnerActor = GetOwner();
	if(OwnerActor)
	{
		AB_LOG(LogABNetwork, Log, TEXT("Owner: %s"), *OwnerActor->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No Owner"));
	}

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}

void AABCharacterPlayer::PostNetInit()
{
	AB_LOG(LogABNetwork, Log, TEXT("%s %s"), TEXT("Begin"), *GetName());

	Super::PostNetInit();
	
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));	
}

void AABCharacterPlayer::OnRep_Owner()
{
	AB_LOG(LogABNetwork, Log, TEXT("%s %s"), *GetName(), TEXT("Begin"));

	Super::OnRep_Owner();

	if(AActor* OwnerActor = GetOwner())
	{
		AB_LOG(LogABNetwork, Log, TEXT("Owner: %s"), *OwnerActor->GetName());
	}
	else
	{
		AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("No Owner"));
	}

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("End"));
}

void AABCharacterPlayer::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuaterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuaterMove);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Attack);
	EnhancedInputComponent->BindAction(TeleportAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::Teleport);
}

void AABCharacterPlayer::ChangeCharacterControl()
{
	if (CurrentCharacterControlType == ECharacterControlType::Quater)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if (CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quater);
	}
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	UABCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		if (UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AABCharacterPlayer::SetCharacterControlData(const UABCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	// 공격할 수 없는 경우에는 움직이지 못하도록 처리
	if(!bCanAttack)
	{
		return;
	}
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::QuaterMove(const FInputActionValue& Value)
{
	// 공격할 수 없는 경우에는 움직이지 못하도록 처리
	if(!bCanAttack)
	{
		return;
	}
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	float InputSizeSquared = MovementVector.SquaredLength();
	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();
	if (MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}

void AABCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AABCharacterPlayer, bCanAttack);
}

void AABCharacterPlayer::Attack()
{
	// ProcessComboCommand();

	if(!bCanAttack)
	{
		return;
	}

	// 공격 애니메이션은 즉시 재생하도록 변경
	if(!HasAuthority())
	{
		bCanAttack = false;
		GetCharacterMovement()->SetMovementMode(MOVE_None);  // OnRep을 호출하지 않고 다이렉트로 수행. 추후 결과 업데이트
		
		// FTimerHandle Handle;
		// GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
		// {
		// 	bCanAttack = true;
		// 	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		// }), AttackTime, false, -1.0f);

		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AABCharacterPlayer::ResetAttack, AttackTime, false);

		PlayAttackAnimation();
	}

	// GetTimeSeconds()는 시뮬레이션 되고 있는 클라이언트의 시간값이 반환됨
	// 즉 서버보다 항상 늦은 시간값을 가지고 있으므로, 아래와 같이 서버 시간을 넘겨줘야 함
	ServerRPCAttack(GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
}

void AABCharacterPlayer::PlayAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);  // 혹시 실행중이라면 멈추고 다시 재생
	AnimInstance->Montage_Play(ComboActionMontage);
}

void AABCharacterPlayer::AttackHitCheck()
{
	if(!IsLocallyControlled())
	{
		// 소유권을 가진 클라이언트에서 진행해야 함
		return;
	}

	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
		
	FHitResult OutHit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);

	const float AttackRange = Stat->GetTotalStat().AttackRange;
	const float AttackRadius = Stat->GetAttackRadius();
	const float AttackDamage = Stat->GetTotalStat().Attack;
	const FVector Start = GetActorLocation() + GetActorForwardVector() * GetCapsuleComponent()->GetScaledCapsuleRadius();
	const FVector End = Start + GetActorForwardVector() * AttackRange;
	const FVector Dir = GetActorForwardVector();
	bool HitDetected = GetWorld()->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, CCHANNEL_ABACTION, FCollisionShape::MakeSphere(AttackRadius), Params);
	float HitCheckTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	if(!HasAuthority())  // 클라이언트라면 서버에 결과를 보내 검증을 거쳐야 함
	{
		HitDetected ? ServerRPCNotifyHit(OutHit, HitCheckTime) : ServerRPCNotifyMiss(Start, End, Dir, HitCheckTime);
	}
	else  // 서버라면 맞았을 경우 검증없이 바로 처리하면 됨
	{
		FColor DebugColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugAttackRange(DebugColor, Start, End, Dir);
		if(HitDetected)
		{
			AttackHitConfirm(OutHit.GetActor());
		}
	}
}

void AABCharacterPlayer::AttackHitConfirm(AActor* HitActor)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
	
	if(!HasAuthority())
	{
		return;
	}

	const float AttackDamage = Stat->GetTotalStat().Attack;
	FDamageEvent DamageEvent;
	HitActor->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
}

void AABCharacterPlayer::DrawDebugAttackRange(const FColor& Color, const FVector& Start, const FVector& End,
                                              const FVector& Forward)
{
#if ENABLE_DRAW_DEBUG
	FVector Center = Start + (End - Start) * 0.5f;
	float HalfHeight = Stat->GetTotalStat().AttackRange * 0.5f;

	DrawDebugCapsule(GetWorld(), Center, HalfHeight, Stat->GetAttackRadius(), FRotationMatrix::MakeFromZ(Forward).ToQuat(), Color, false, 5.0f);
#endif
}

void AABCharacterPlayer::ServerRPCAttack_Implementation(float AttackStartTime)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));

	// 기존 NetMulticast에서 수행하던 걸 직접 구현
	bCanAttack = false;
	OnRep_CanAttack();

	AttackTimeDifference = GetWorld()->GetTimeSeconds() - AttackStartTime;
	AB_LOG(LogABNetwork, Log, TEXT("LagTime: %f"), AttackTimeDifference);
	AttackTimeDifference = FMath::Clamp(AttackTimeDifference, 0.0f, AttackTime - 0.01f);
		
	// FTimerHandle Handle;
	// GetWorld()->GetTimerManager().SetTimer(Handle, FTimerDelegate::CreateLambda([&]
	// {
	// 	bCanAttack = true;
	// 	OnRep_CanAttack();
	// }), AttackTime - AttackTimeDifference, false, -1.0f);

	// 서버에서 전송되는 시간 오차를 검사해 Difference를 뺀 타이머로 적용
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AABCharacterPlayer::ResetAttack, AttackTime - AttackTimeDifference, false);
	LastAttackStartTime = AttackStartTime;

	PlayAttackAnimation();  // Server에서도 애니메이션 재생은 해야지
	
	// NetMulticastRPCAttack();

	// 명령을 내린 클라이언트는 애니메이션 재생을 할 필요가 없음.
	// 하지만 재생하지 않도록 걸러준다고 해도, 패킷 자체는 전송됨
	// 이를 최적화하고자 클라이언트 RPC로 변경
	for(APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if(!PlayerController || GetController() == PlayerController || PlayerController->IsLocalController())
		{
			continue;
		}
		
		// 나머지 Simulated Proxy들 중 재생하지 않은 플레이어에 한해
		if(AABCharacterPlayer* OtherPlayer = Cast<AABCharacterPlayer>(PlayerController->GetPawn()))
		{
			OtherPlayer->ClientRPCPlayAnimation(this);
		}
	}
}

void AABCharacterPlayer::ClientRPCPlayAnimation_Implementation(AABCharacterPlayer* CharacterToPlay)
{
	AB_LOG(LogABNetwork, Log, TEXT("%s"), TEXT("Begin"));
	if(CharacterToPlay)
	{
		CharacterToPlay->PlayAttackAnimation();
	}
}

void AABCharacterPlayer::NetMulticastRPCAttack_Implementation()
{
	// NetMulticast는 Unreliable. 즉 Cosmetic 들을 수행하도록 역할 변경
	if(!IsLocallyControlled())  // 현재 클라이언트를 제외한 나머지 프록시들에 대해서만 수행
	{
		PlayAttackAnimation();
	}
}

bool AABCharacterPlayer::ServerRPCAttack_Validate(float AttackStartTime)
{
	if(LastAttackStartTime == 0.0f)
	{
		return true;
	}

	// 마지막 공격 시간 - 재공격 시간 < 기본 공격 시간 == 문제
	return AttackStartTime - LastAttackStartTime > (AttackTime - 0.4f);
}

void AABCharacterPlayer::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime)
{
	AActor* HitActor = HitResult.GetActor();
	
	if(!IsValid(HitActor))
	{
		return;
	}
	
	const FVector HitLocation = HitResult.Location;
	const FBox HitBox = HitActor->GetComponentsBoundingBox();
	const FVector HitBoxCenter = (HitBox.Min + HitBox.Max) * 0.5f;
	if(FVector::DistSquared(HitLocation, HitBoxCenter) <= AcceptCheckDistance * AcceptCheckDistance)
	{
		AttackHitConfirm(HitActor);
	}
	else  // 공격 범위를 벗어남
	{
		AB_LOG(LogABNetwork, Warning, TEXT("%s"), TEXT("HitTest Rejected"));
	}

#if ENABLE_DRAW_DEBUG
	DrawDebugPoint(GetWorld(), HitBoxCenter, 50.0f, FColor::Cyan, false, 5.0f);
	DrawDebugPoint(GetWorld(), HitLocation, 50.0f, FColor::Magenta, false, 5.0f);
#endif

	// 여기서는 Hit한 것이 보장됨
	DrawDebugAttackRange(FColor::Green, HitResult.TraceStart, HitResult.TraceEnd, HitActor->GetActorForwardVector());
}

bool AABCharacterPlayer::ServerRPCNotifyHit_Validate(const FHitResult& HitResult, float HitCheckTime)
{
	return HitCheckTime - LastAttackStartTime > AcceptMinCheckTime;
}

void AABCharacterPlayer::ServerRPCNotifyMiss_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir,
                                                            float HitCheckTime)
{
	// 여기서는 Miss한 것이 보장됨
	DrawDebugAttackRange(FColor::Red, TraceStart, TraceEnd, TraceDir);
}

bool AABCharacterPlayer::ServerRPCNotifyMiss_Validate(FVector_NetQuantize TraceStart, FVector_NetQuantize TraceEnd, FVector_NetQuantizeNormal TraceDir,
	float HitCheckTime)
{
	return HitCheckTime - LastAttackStartTime > AcceptMinCheckTime;
}

void AABCharacterPlayer::OnRep_CanAttack()
{
	// 값이 업데이트될 때마다 수행됨
	if(!bCanAttack)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_None);  // 움직임 고정
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);  // 고정 해제
	}
}

void AABCharacterPlayer::SetupHUDWidget(UABHUDWidget* InHUDWidget)
{
	if (InHUDWidget)
	{
		InHUDWidget->UpdateStat(Stat->GetBaseStat(), Stat->GetModifierStat());
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp(), Stat->GetMaxHp());

		Stat->OnStatChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateStat);
		Stat->OnHpChanged.AddUObject(InHUDWidget, &UABHUDWidget::UpdateHpBar);
	}
}

void AABCharacterPlayer::Teleport()
{
	AB_LOG(LogABTeleport, Log, TEXT("%s"), TEXT("Begin"));
	if(UABCharacterMovementComponent* ABCMC = Cast<UABCharacterMovementComponent>(GetCharacterMovement()))
	{
		ABCMC->SetTeleportCommand();
	}
}

void AABCharacterPlayer::ResetPlayer()
{
	if(UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		// 혹시 모를 애니메이션(사망, 공격 등) 즉각 중단
		AnimInstance->StopAllMontages(0.0f);
	}

	// 스탯 초기화
	Stat->SetLevelStat(1);
	Stat->ResetStat();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	SetActorEnableCollision(true);
	HpBar->SetHiddenInGame(false);

	if(HasAuthority())
	{
		if(IABGameInterface* ABGameMode = GetWorld()->GetAuthGameMode<IABGameInterface>())
		{
			FTransform NewTransform = ABGameMode->GetRandomStartTransform();
			TeleportTo(NewTransform.GetLocation(), NewTransform.GetRotation().Rotator());
		}
	}
}

void AABCharacterPlayer::ResetAttack()
{
	bCanAttack = true;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

float AABCharacterPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if(Stat->GetCurrentHp() <= 0.0f)
	{
		// GameMode에게 알려줌
		if(IABGameInterface* ABGameMode = GetWorld()->GetAuthGameMode<IABGameInterface>())
		{
			ABGameMode->OnPlayerKilled(EventInstigator, GetController(), this);
		}
	}

	return ActualDamage;
}
