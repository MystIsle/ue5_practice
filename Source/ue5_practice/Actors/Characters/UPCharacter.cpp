// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


AUPCharacter::AUPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AUPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OriginalWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void AUPCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateMaxWalkSpeed();
}

void AUPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovementState();
}

void AUPCharacter::ToggleSprint(bool bActive)
{
	if (bSprinting == bActive)
	{
		return;
	}

	bSprinting = bActive;
	UpdateMaxWalkSpeed();
}

bool AUPCharacter::CanJumpInternal_Implementation() const
{
	if (bIsAttacking)
	{
		return false;
	}

	return Super::CanJumpInternal_Implementation();
}

void AUPCharacter::Attack()
{
	if (AttackMontage == nullptr)
	{
		return;
	}
	
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return;
	}

	if (AnimInstance->Montage_IsPlaying(AttackMontage))
	{
		return;
	}

	AnimInstance->Montage_Play(AttackMontage);
	
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(AttackMontage);
	if (MontageInstance == nullptr)
	{
		return;
	}
	
	bIsAttacking = true;
	MontageInstance->OnMontageBlendingOutStarted.BindUObject(this, &ThisClass::OnAttackMontageEnded);
	GetController()->SetIgnoreMoveInput(true);
}


void AUPCharacter::UpdateMovementState()
{
	if (bIsAttacking || GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
	{
		MovementState = EUPMovementState::Idle;
	}
	else if (bSprinting)
	{
		MovementState = EUPMovementState::Run;
	}
	else
	{
		MovementState = EUPMovementState::Walk;
	}
}

void AUPCharacter::UpdateMaxWalkSpeed()
{
	auto CharMovementComp = GetCharacterMovement();
	CharMovementComp->MaxWalkSpeed = bSprinting ? OriginalWalkSpeed * SprintSpeedRate : OriginalWalkSpeed;
}

void AUPCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
	if (const auto CharController = GetController())
	{
		CharController->SetIgnoreMoveInput(false);	
	}
}
