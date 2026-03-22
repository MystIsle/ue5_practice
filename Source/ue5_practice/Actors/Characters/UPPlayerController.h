// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UPPlayerController.generated.h"

class AUPCharacter;
struct FInputActionValue;
struct FInputActionInstance;
class UInputMappingContext;
class UInputAction;

UCLASS()
class UE5_PRACTICE_API AUPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
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

private:
	void OnInputMoveTriggered(const FInputActionValue& InputActionValue);
	void OnInputSprint(const FInputActionInstance& InputActionInstance);
	void OnInputJumpStarted();
	void OnInputAttack();
};
