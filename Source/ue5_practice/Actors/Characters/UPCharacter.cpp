// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPCharacter.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "MotionWarpingComponent.h"
#include "UPHitReactionComponent.h"
#include "Animation/AnimMontage.h"
#include "Combat/UPHitEffect.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"

static TAutoConsoleVariable<bool> CVarShowAttackDirection(
	TEXT("Debug.ShowAttackDirection"),
	false,
	TEXT("Draw attack direction arrows")
);

AUPCharacter::AUPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	StimuliSourceComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	HitReactionComp = CreateDefaultSubobject<UUPHitReactionComponent>(TEXT("HitReaction"));
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
	if (IsDead() || bAttacking == true)
	{
		return false;
	}

	return Super::CanJumpInternal_Implementation();
}

void AUPCharacter::ApplyDamage(int Damage, const FUPHitEffect& HitEffect, const FVector& KnockbackDirection)
{
	if (IsDead())
	{
		return;
	}

	HP -= Damage;

	if (bAttacking == true)
	{
		StopAttack();
	}

	const bool bLethal = HP <= 0;
	if (bLethal)
	{
		HP = 0;
		StimuliSourceComp->UnregisterFromPerceptionSystem();

		if (AAIController* AIC = Cast<AAIController>(GetController()))
		{
			if (AIC->BrainComponent != nullptr)
			{
				AIC->BrainComponent->PauseLogic(TEXT("Die"));
			}
		}
	}

	HitReactionComp->PlayReact(HitEffect, KnockbackDirection);
}

void AUPCharacter::StopAttack()
{
	if (bAttacking == false)
	{
		return;
	}

	bAttacking = false;
	StopAnimMontage(AttackMontage);
}

void AUPCharacter::Attack(const FRotator& InRotation)
{
	if (IsDead())
	{
		return;
	}

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

	MotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(TEXT("Target"), GetActorLocation(), InRotation);
	AnimInstance->Montage_Play(AttackMontage);

	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(AttackMontage);
	if (MontageInstance == nullptr)
	{
		return;
	}

	if (CVarShowAttackDirection.GetValueOnGameThread())
	{
		DebugShowAttackDirection(InRotation);
	}

	bAttacking = true;
	MontageInstance->OnMontageBlendingOutStarted.BindUObject(this, &ThisClass::OnAttackMontageBlendingOutStarted);
	if (AController* CharController = GetController())
	{
		CharController->SetIgnoreMoveInput(true);
	}
}
void AUPCharacter::SetControlLocked(bool bLocked, const TCHAR* PauseLogicReason)
{
	AController* PossessedController = GetController();
	if (PossessedController == nullptr)
	{
		return;
	}

	PossessedController->SetIgnoreMoveInput(bLocked);

	const AAIController* AiController = Cast<AAIController>(PossessedController);
	UBrainComponent* BrainComp = AiController ? AiController->BrainComponent : nullptr;
	if (BrainComp == nullptr)
	{
		return;
	}

	if (bLocked)
	{
		BrainComp->PauseLogic(PauseLogicReason);
	}
	else
	{
		BrainComp->ResumeLogic(PauseLogicReason);
	}
}

void AUPCharacter::UpdateMovementState()
{
	const bool bMoving = GetCharacterMovement()->Velocity.IsNearlyZero() == false
		|| GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero() == false;

	if (bAttacking == true || bMoving == false)
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

void AUPCharacter::UpdateMaxWalkSpeed() const
{
	auto CharMovementComp = GetCharacterMovement();
	CharMovementComp->MaxWalkSpeed = bSprinting ? OriginalWalkSpeed * SprintSpeedRate : OriginalWalkSpeed;
}

void AUPCharacter::DebugShowAttackDirection(const FRotator& InRotation) const
{
	const FVector ActorLocation = GetActorLocation();

	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		ActorLocation + InRotation.Vector() * 200,
		30,
		FColor::Red,
		false,
		5.f,
		2,
		5
	);

	DrawDebugDirectionalArrow(
		GetWorld(),
		GetActorLocation(),
		ActorLocation + GetActorRotation().Vector() * 200,
		30,
		FColor::Yellow,
		false,
		5.f,
		3,
		5
	);
}

void AUPCharacter::ApplyAttackerHitStop(int32 MontageInstanceID, const FUPHitEffect& HitEffect)
{
	if (HitEffect.HasHitStop() == false)
	{
		return;
	}

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst == nullptr)
	{
		return;
	}

	FAnimMontageInstance* MontageInstance = AnimInst->GetMontageInstanceForID(MontageInstanceID);
	if (MontageInstance == nullptr)
	{
		return;
	}

	AttackerHitStopMontageInstanceID = MontageInstanceID;
	MontageInstance->SetPlayRate(HitEffect.AnimPlayRate);

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::RestoreAttackMontagePlayRate, MontageInstanceID);
	GetWorldTimerManager().SetTimer(
		AttackerHitStopTimerHandle, TimerDelegate,
		HitEffect.HitStopDuration, false
	);
}

void AUPCharacter::RestoreAttackMontagePlayRate(int32 MontageInstanceID)
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst == nullptr)
	{
		return;
	}

	FAnimMontageInstance* MontageInstance = AnimInst->GetMontageInstanceForID(MontageInstanceID);
	if (MontageInstance == nullptr)
	{
		return;
	}

	MontageInstance->SetPlayRate(1.f);
}

void AUPCharacter::OnAttackMontageBlendingOutStarted(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetWorldTimerManager().IsTimerActive(AttackerHitStopTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AttackerHitStopTimerHandle);
		RestoreAttackMontagePlayRate(AttackerHitStopMontageInstanceID);
	}

	bAttacking = false;
	if (AController* CharController = GetController())
	{
		CharController->SetIgnoreMoveInput(false);
	}
}
