// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPCharacter.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

static TAutoConsoleVariable<bool> CVarShowAttackDirection(
	TEXT("Debug.ShowAttackDirection"),
	false,
	TEXT("Draw attack direction arrows")
);

AUPCharacter::AUPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
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
	if (bAttacking)
	{
		return false;
	}

	return Super::CanJumpInternal_Implementation();
}

void AUPCharacter::ApplyDamage(int Damage)
{
	UE_LOG(LogTemp, Log, TEXT("AUPCharacter::ApplyDamage() %s"), *GetFName().ToString());
}

void AUPCharacter::Attack(const FRotator& InRotation)
{
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
	MontageInstance->OnMontageBlendingOutStarted.BindUObject(this, &ThisClass::OnAttackMontageEnded);
	GetController()->SetIgnoreMoveInput(true);
}

void AUPCharacter::UpdateMovementState()
{
	if (bAttacking || GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
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

void AUPCharacter::UpdateMaxWalkSpeed()
{
	auto CharMovementComp = GetCharacterMovement();
	CharMovementComp->MaxWalkSpeed = bSprinting ? OriginalWalkSpeed * SprintSpeedRate : OriginalWalkSpeed;
}

void AUPCharacter::DebugShowAttackDirection(const FRotator& InRotation)
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

void AUPCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bAttacking = false;
	if (const auto CharController = GetController())
	{
		CharController->SetIgnoreMoveInput(false);
	}
}
