// Copyright (c) 2026 Team Sparta. All rights reserved.

#include "UPCharacterAnimInstance.h"
#include "UPCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUPCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<AUPCharacter>(TryGetPawnOwner());
	if (OwningCharacter == nullptr)
	{
		return;
	}

	MovementComponent = OwningCharacter->GetCharacterMovement();
}

void UUPCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (MovementComponent == nullptr)
	{
		return;
	}

	Speed = MovementComponent->Velocity.Size2D();
	bInAir = MovementComponent->IsFalling();
	bIsFalling = MovementComponent->Velocity.Z <= 0.f;

	if (MovementComponent->GetCurrentAcceleration().IsNearlyZero() == false)
	{
		if (OwningCharacter->IsSprinting())
		{
			MovementState = EUPMovementState::Run;
		}
		else
		{
			MovementState = EUPMovementState::Walk;
		}
	}
	else
	{
		MovementState = EUPMovementState::Idle;
	}
}
