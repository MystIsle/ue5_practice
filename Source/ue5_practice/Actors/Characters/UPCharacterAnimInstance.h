// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UPCharacterAnimInstance.generated.h"

class AUPCharacter;
class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EUPMovementState : uint8
{
	Idle,
	Walk,
	Run
};

UCLASS()
class UE5_PRACTICE_API UUPCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	float Speed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool bInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	EUPMovementState MovementState = EUPMovementState::Idle;
	
	UPROPERTY()
	TObjectPtr<AUPCharacter> OwningCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
};
