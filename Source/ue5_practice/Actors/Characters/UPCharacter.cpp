// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


AUPCharacter::AUPCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
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

void AUPCharacter::ToggleSprint(bool bActive)
{
	if (bSprinting == bActive)
	{
		return;
	}

	bSprinting = bActive;
	UpdateMaxWalkSpeed();
}

void AUPCharacter::Attack()
{
	if (AttackMontage == nullptr)
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
}

void AUPCharacter::UpdateMaxWalkSpeed()
{
	auto CharMovementComp = GetCharacterMovement();
	CharMovementComp->MaxWalkSpeed = bSprinting ? OriginalWalkSpeed * SprintSpeedRate : OriginalWalkSpeed;
}
