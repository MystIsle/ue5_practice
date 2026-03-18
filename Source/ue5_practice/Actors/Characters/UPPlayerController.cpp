// Copyright (c) 2026 Team Sparta. All rights reserved.

#include "UPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

void AUPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(InputAction_Move, ETriggerEvent::Triggered, this, &ThisClass::OnInputMoveTriggered);
		EnhancedInput->BindAction(InputAction_Sprint, ETriggerEvent::Started, this, &ThisClass::OnInputSprint);
		EnhancedInput->BindAction(InputAction_Sprint, ETriggerEvent::Completed, this, &ThisClass::OnInputSprint);
		EnhancedInput->BindAction(InputAction_Jump, ETriggerEvent::Started, this, &ThisClass::OnInputJumpStarted);
	}
}

void AUPPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSubsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void AUPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
	}
}

void AUPPlayerController::OnInputMoveTriggered(const FInputActionValue& InputActionValue)
{
	
}

void AUPPlayerController::OnInputSprint(const FInputActionInstance& InputActionInstance)
{

}

void AUPPlayerController::OnInputJumpStarted()
{
	
}
