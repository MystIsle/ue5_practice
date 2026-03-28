// Copyright (c) 2026 Team Sparta. All rights reserved.

#include "UPPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UPCharacter.h"

void AUPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(InputAction_Move, ETriggerEvent::Started, this, &ThisClass::OnInputMove);
		EnhancedInput->BindAction(InputAction_Move, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
		EnhancedInput->BindAction(InputAction_Move, ETriggerEvent::Completed, this, &ThisClass::OnInputMove);
		EnhancedInput->BindAction(InputAction_Sprint, ETriggerEvent::Started, this, &ThisClass::OnInputSprint);
		EnhancedInput->BindAction(InputAction_Sprint, ETriggerEvent::Completed, this, &ThisClass::OnInputSprint);
		EnhancedInput->BindAction(InputAction_Jump, ETriggerEvent::Started, this, &ThisClass::OnInputJumpStarted);
		EnhancedInput->BindAction(InputAction_Attack, ETriggerEvent::Started, this, &ThisClass::OnInputAttack);
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

void AUPPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledCharacter = Cast<AUPCharacter>(InPawn);
}

void AUPPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	ControlledCharacter = nullptr;
}

void AUPPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (auto InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
	}
}
FGenericTeamId AUPPlayerController::GetGenericTeamId() const
{
	return ControlledCharacter ? ControlledCharacter->GetGenericTeamId() : FGenericTeamId();
}

void AUPPlayerController::InputMove(const FInputActionInstance& InputActionInstance)
{
	if (ControlledCharacter == nullptr)
	{
		return;
	}

	const FRotator Rotation = PlayerCameraManager->GetCameraRotation();
	const FRotator Rotation2D(0, Rotation.Yaw, 0);

	FVector2D InputValue = InputActionInstance.GetValue().Get<FVector2D>();
	if (InputValue.IsNearlyZero())
	{
		SetControlRotation(ControlledCharacter->GetActorRotation());
		return;
	}

	FVector InputDir = FVector(InputValue, 0.f).GetSafeNormal();
	FVector WorldInputDir = Rotation2D.RotateVector(InputDir);
	ControlledCharacter->AddMovementInput(WorldInputDir);
	SetControlRotation(WorldInputDir.Rotation());
}

void AUPPlayerController::OnInputMove(const FInputActionInstance& InputActionInstance)
{
	switch (InputActionInstance.GetTriggerEvent())
	{
	// 입력 시작
	case ETriggerEvent::Started:
		bInputMove = true;
		InputMove(InputActionInstance);
		break;
	case ETriggerEvent::Triggered:
		InputMove(InputActionInstance);
		break;
	case ETriggerEvent::Completed:
		// 입력 끝
		bInputMove = false;
		return;
	default:
		return;
	}
}

void AUPPlayerController::OnInputSprint(const FInputActionInstance& InputActionInstance)
{
	if (ControlledCharacter == nullptr)
	{
		return;
	}

	switch (InputActionInstance.GetTriggerEvent())
	{
	case ETriggerEvent::Started:
		ControlledCharacter->ToggleSprint(true);
		break;
	case ETriggerEvent::Completed:
		ControlledCharacter->ToggleSprint(false);
		break;
	default:
		break;
	}
}

void AUPPlayerController::OnInputJumpStarted()
{
	if (ControlledCharacter == nullptr)
	{
		return;
	}

	ControlledCharacter->Jump();
}

void AUPPlayerController::OnInputAttack()
{
	if (ControlledCharacter == nullptr)
	{
		return;
	}

	auto AttackRotation = ControlledCharacter->GetActorRotation();
	if (bInputMove)
	{
		// Control Rotation
		AttackRotation = GetDesiredRotation();
	}

	ControlledCharacter->Attack(AttackRotation);
}
