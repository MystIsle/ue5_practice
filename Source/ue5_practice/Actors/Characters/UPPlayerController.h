// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "UPPlayerController.generated.h"

class AUPCharacter;
struct FInputActionValue;
struct FInputActionInstance;
class UInputMappingContext;
class UInputAction;

UCLASS()
class UE5_PRACTICE_API AUPPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InputAction_Move;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InputAction_Sprint;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InputAction_Jump;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> InputAction_Attack;

	UPROPERTY()
	TObjectPtr<AUPCharacter> ControlledCharacter;

	bool bInputMove = false;

private:
	void InputMove(const FInputActionInstance& InputActionInstance);

	void OnInputMove(const FInputActionInstance& InputActionInstance);
	void OnInputSprint(const FInputActionInstance& InputActionInstance);
	void OnInputJumpStarted();
	void OnInputAttack();
};
