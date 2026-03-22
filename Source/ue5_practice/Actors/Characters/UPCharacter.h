// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPCharacter.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EUPMovementState : uint8
{
	Idle,
	Walk,
	Run
};

UCLASS()
class UE5_PRACTICE_API AUPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AUPCharacter();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual bool CanJumpInternal_Implementation() const override;

public:
	FORCEINLINE bool IsSprinting() const { return bSprinting; }
	FORCEINLINE bool IsAttacking() const { return bIsAttacking; }
	FORCEINLINE EUPMovementState GetMovementState() const { return MovementState; }

	void ToggleSprint(bool bActive);
	void Attack();

private:
	void UpdateMovementState();
	void UpdateMaxWalkSpeed();
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float SprintSpeedRate = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

private:
	float OriginalWalkSpeed;
	bool bSprinting = false;
	bool bIsAttacking = false;
	EUPMovementState MovementState = EUPMovementState::Idle;
};
