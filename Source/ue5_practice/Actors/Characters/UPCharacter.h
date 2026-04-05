// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Team/UPTeamID.h"
#include "UPCharacter.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UAnimMontage;
class UMotionWarpingComponent;

UENUM(BlueprintType)
enum class EUPMovementState : uint8
{
	Idle,
	Walk,
	Run
};

UCLASS()
class UE5_PRACTICE_API AUPCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AUPCharacter();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual FGenericTeamId GetGenericTeamId() const override { return static_cast<uint8>(TeamID); }

protected:
	virtual bool CanJumpInternal_Implementation() const override;

public:
	FORCEINLINE bool IsSprinting() const { return bSprinting; }
	FORCEINLINE bool IsAttacking() const { return bAttacking; }
	FORCEINLINE EUPMovementState GetMovementState() const { return MovementState; }
	FORCEINLINE EUPTeamID GetTeamID() const { return TeamID; }
	FORCEINLINE int GetATK() const { return ATK; }
	FORCEINLINE int GetHP() const { return HP; }
	FORCEINLINE bool IsDead() const { return HP <= 0; }

	void ApplyDamage(int Damage);
	void ToggleSprint(bool bActive);
	void Attack(const FRotator& InRotation);

private:
	void HitReact();
	void Die();
	void StopAttack();
	void UpdateMovementState();
	void UpdateMaxWalkSpeed();
	void DebugShowAttackDirection(const FRotator& InRotation);
	void OnAttackMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted);
	void OnHitReactMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSourceComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Team")
	EUPTeamID TeamID = EUPTeamID::NoTeam;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float SprintSpeedRate = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TArray<TObjectPtr<UAnimMontage>> HitReactMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TArray<TObjectPtr<UAnimMontage>> DeathMontages;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	int HP = 100;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	int ATK = 20;

private:
	float OriginalWalkSpeed;
	bool bSprinting = false;
	bool bAttacking = false;
	int HitReactIndex = 0;
	EUPMovementState MovementState = EUPMovementState::Idle;
};
