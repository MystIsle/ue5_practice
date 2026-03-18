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

void AUPCharacter::UpdateMaxWalkSpeed()
{
	auto CharMovementComp = GetCharacterMovement();
	CharMovementComp->MaxWalkSpeed = bSprinting ? OriginalWalkSpeed * SprintSpeedRate : OriginalWalkSpeed;
}
