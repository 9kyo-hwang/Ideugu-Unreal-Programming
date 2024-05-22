// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ABCharacterBase.h"

#include "ABCharacterDataAsset.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AABCharacterBase::AABCharacterBase()
{
 	// TODO: Pawn Setting
	
	// 회전을 위해 세팅
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule 설정
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// Movement Component
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500, 0);
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// 실제 에셋 부착
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/Script/Engine.SkeletalMesh'/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple'"));
	if(CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/Game/Characters/Mannequins/Animations/ABP_Quinn.ABP_Quinn_C"));
	if(AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	static ConstructorHelpers::FObjectFinder<UABCharacterDataAsset> ShoulderDataAssetRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/CharacterControl/ABC_Shoulder.ABC_Shoulder'"));
	if(ShoulderDataAssetRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Shoulder, ShoulderDataAssetRef.Object);
	}
	static ConstructorHelpers::FObjectFinder<UABCharacterDataAsset> QuarterDataAssetRef(TEXT("/Script/Engine.Blueprint'/Game/ArenaBattle/CharacterControl/ABC_Quarter.ABC_Quarter'"));
	if(QuarterDataAssetRef.Object)
	{
		CharacterControlManager.Add(ECharacterControlType::Quarter, QuarterDataAssetRef.Object);
	}
}

void AABCharacterBase::SetCharacterDataAsset(const UABCharacterDataAsset* CharacterDataAsset)
{
	// Pawn
	bUseControllerRotationYaw = CharacterDataAsset->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterDataAsset->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterDataAsset->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterDataAsset->RotationRate;
}