// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ABAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UABAnimInstance::UABAnimInstance() : MovingThreshold(3.0f), JumpingThreshold(100.0f)
{
}

void UABAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 오브젝트에 대한 포인터 값을 초기화 코드 정의
	// 액터 타입이기 때문에 ACharacter인 지 알 수 없음 -> 변환
	if((Owner = Cast<ACharacter>(GetOwningActor())))
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 매 프레임마다 Movement 객체가 존재하면
	if(Movement)
	{
		Velocity = Movement->Velocity;
		GroundSpeed = Velocity.Size2D();  // Z축 값은 제외
		IsIdle = GroundSpeed < MovingThreshold;
		IsFalling = Movement->IsFalling();
		IsJumping = IsFalling && Velocity.Z > JumpingThreshold;  // 떨어지는 중이면서 쓰레시홀드 값보다 Z축 Velocity가 크다면
	}
}
