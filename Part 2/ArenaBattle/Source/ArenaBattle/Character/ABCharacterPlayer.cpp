// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterPlayer.h"

#include "ABCharacterDataAsset.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ABCharacterDataAsset.h"

AABCharacterPlayer::AABCharacterPlayer()
{
	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);  // root component에 부착
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);  // SpringArm에 부착. SocketName을 통해 Arm 끝에 부착
	FollowCamera->bUsePawnControlRotation = false;

	// Mapping Context & Input Action Binding
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_Jump.IA_Jump'"));
	if(InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionChangeControlRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ChangeControl.IA_ChangeControl'"));
	if(InputActionChangeControlRef.Object)
	{
		ChangeControlAction = InputActionChangeControlRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderMove.IA_ShoulderMove'"));
	if(InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_ShoulderLook.IA_ShoulderLook'"));
	if(InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuarterMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/ArenaBattle/Input/Actions/IA_QuarterMove.IA_QuarterMove'"));
	if(InputActionQuarterMoveRef.Object)
	{
		QuarterMoveAction = InputActionQuarterMoveRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Quarter;
}

void AABCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);
}

void AABCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// CastChecked: 반드시 EnhancedInputComponent를 사용하도록 강제
	const auto EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Jump, StopJump는 상위 클래스인 ACharacter에서 제공하는 함수로 바인딩
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	EnhancedInputComponent->BindAction(ChangeControlAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ChangeCharacterControl);
	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::ShoulderLook);
	EnhancedInputComponent->BindAction(QuarterMoveAction, ETriggerEvent::Triggered, this, &AABCharacterPlayer::QuarterMove);
}

void AABCharacterPlayer::ChangeCharacterControl()
{
	if(CurrentCharacterControlType == ECharacterControlType::Quarter)
	{
		SetCharacterControl(ECharacterControlType::Shoulder);
	}
	else if(CurrentCharacterControlType == ECharacterControlType::Shoulder)
	{
		SetCharacterControl(ECharacterControlType::Quarter);
	}
}

void AABCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	// 컨트롤 데이터 에셋이 존재한다면 기본 속성 Set 호출
	if(auto NewCharacterControl = CharacterControlManager[NewCharacterControlType])
	{
		SetCharacterDataAsset(NewCharacterControl);
		// BeginPlay에서 담당하던 내용을 이쪽으로 이관
		auto PlayerController = CastChecked<APlayerController>(GetController());
		if(auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();  // 우선 매핑 제거
			if(auto NewMappingContext = NewCharacterControl->InputMappingContext)
			{
				Subsystem->AddMappingContext(NewMappingContext, 0);  // 우선순위: 0
				CurrentCharacterControlType = NewCharacterControlType;
			}
		}
	}
}

void AABCharacterPlayer::SetCharacterDataAsset(const UABCharacterDataAsset* CharacterDataAsset)
{
	Super::SetCharacterDataAsset(CharacterDataAsset);

	// Camera SpringArm
	CameraBoom->TargetArmLength = CharacterDataAsset->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterDataAsset->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterDataAsset->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterDataAsset->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterDataAsset->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterDataAsset->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterDataAsset->bDoCollisionTest;
}

// InputActionValue에서 XY 값을 가져와서 MovementComponent와 연결
void AABCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);  // 원래대로 하더라도
	AddMovementInput(RightDirection, MovementVector.Y);  // 논리적으로 맞는 방향
} 

void AABCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// 입력값을 받아 컨트롤러의 Control Rotation 값을 속성을 업데이트함
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AABCharacterPlayer::QuarterMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	float MovementVectorSize = 1.0f;
	float MovementVectorSizeSquared = MovementVector.SquaredLength();

	// 크기가 1이 되도록 조정
	if(MovementVectorSizeSquared > 1.0f)
	{
		MovementVector.Normalize();
		MovementVectorSizeSquared = 1.0f;
	}
	else
	{
		MovementVectorSize = FMath::Sqrt(MovementVectorSizeSquared);
	}

	// Modifier를 통해 방향을 조절해줘서 그대로 사용하면 됨
	FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
	// Forward 방향으로 지정, MovementController에서 설정한 옵션에 의해 이동 방향으로 자동 회전
	GetController()->SetControlRotation(FRotationMatrix::MakeFromX(MoveDirection).Rotator());
	AddMovementInput(MoveDirection, MovementVectorSize);
}
