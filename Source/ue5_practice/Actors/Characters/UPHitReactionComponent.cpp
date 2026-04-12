// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPHitReactionComponent.h"
#include "UPCharacter.h"
#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Combat/UPHitEffect.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"

UUPHitReactionComponent::UUPHitReactionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUPHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AUPCharacter>(GetOwner());
	if (OwnerCharacter == nullptr)
	{
		return;
	}

	auto MeshComponent = OwnerCharacter->GetMesh();
	AnimInstance = MeshComponent->GetAnimInstance();
	CharacterMovementComp = OwnerCharacter->GetCharacterMovement();
}

void UUPHitReactionComponent::PlayReact(const FUPHitEffect& HitEffect, const FVector& KnockbackDirection)
{
	if (AnimInstance == nullptr)
	{
		return;
	}

	if (IsReacting())
	{
		CancelReaction();
	}

	CurrentHitEffect = HitEffect;
	CurrentKnockbackDirection = KnockbackDirection;
	OwnerCharacter->SetControlLocked(true, TEXT("HitReaction"));

	ReactState = EUPHitReact::React;
	FAnimMontageInstance* MontageInstance = PlayReactionMontage();
	MontageInstanceID = MontageInstance ? MontageInstance->GetInstanceID() : INDEX_NONE;

	if (MontageInstance != nullptr && HitEffect.HasAnyEffect() == false)
	{
		//경직 넉백 없이 들어오는 경우, 몽타주 블렌드 아웃에서 종료할 수 있도록 함.
		MontageInstance->OnMontageBlendingOutStarted.BindUObject(this, &UUPHitReactionComponent::OnReactionMontageBlendingOut);
		return;
	}

	if (MontageInstanceID == INDEX_NONE || HitEffect.HasHitStop() == false)
	{
		//히트스탑 없는 경우 즉시 넉백
		StartKnockback();
		return;
	}

	//히트스탑 처리
	ReactState = EUPHitReact::HitStop;
	MontageInstance->SetPlayRate(HitEffect.AnimPlayRate);

	GetWorld()->GetTimerManager().SetTimer(
		ReactTimerHandle, this, &UUPHitReactionComponent::OnHitStopFinished,
		HitEffect.HitStopDuration, false
	);
}

FAnimMontageInstance* UUPHitReactionComponent::PlayReactionMontage()
{
	if (AnimInstance == nullptr)
	{
		return nullptr;
	}

	UAnimMontage* Montage = nullptr;
	if (OwnerCharacter->IsDead())
	{
		if (DeathMontages.Num() > 0)
		{
			const int Index = FMath::RandRange(0, DeathMontages.Num() - 1);
			Montage = DeathMontages[Index];
		}
	}
	else
	{
		if (HitReactMontages.Num() > 0)
		{
			Montage = HitReactMontages[HitMontageIndex];
			HitMontageIndex = (HitMontageIndex + 1) % HitReactMontages.Num();
		}
	}

	if (Montage == nullptr)
	{
		return nullptr;
	}

	AnimInstance->Montage_Play(Montage);
	return AnimInstance->GetActiveInstanceForMontage(Montage);
}

void UUPHitReactionComponent::RestorePlayRate(int32 ReactMontageInstanceID)
{
	if (AnimInstance == nullptr)
	{
		return;
	}

	FAnimMontageInstance* MontageInstance = AnimInstance->GetMontageInstanceForID(ReactMontageInstanceID);
	if (MontageInstance == nullptr)
	{
		return;
	}

	MontageInstance->SetPlayRate(1.f);
}

void UUPHitReactionComponent::StartKnockback()
{
	if (CurrentHitEffect.HasKnockback() == false)
	{
		return;
	}

	const FVector KnockbackDir2D = CurrentKnockbackDirection.GetSafeNormal2D(
		UE_SMALL_NUMBER,
		OwnerCharacter->GetActorForwardVector().GetSafeNormal2D()
	);

	TSharedPtr<FRootMotionSource_MoveToForce> MoveToForce = MakeShared<FRootMotionSource_MoveToForce>();
	MoveToForce->InstanceName = FName("Knockback");
	MoveToForce->AccumulateMode = ERootMotionAccumulateMode::Override;
	MoveToForce->Priority = 10;
	MoveToForce->Duration = CurrentHitEffect.KnockbackDuration;
	MoveToForce->StartLocation = OwnerCharacter->GetActorLocation();
	MoveToForce->TargetLocation = MoveToForce->StartLocation + KnockbackDir2D * CurrentHitEffect.KnockbackDistance;
	MoveToForce->bRestrictSpeedToExpected = true;
	MoveToForce->PathOffsetCurve = CurrentHitEffect.KnockbackCurve;
	MoveToForce->FinishVelocityParams.Mode = ERootMotionFinishVelocityMode::SetVelocity;
	MoveToForce->FinishVelocityParams.SetVelocity = FVector::ZeroVector;

	KnockbackRootMotionSourceID = CharacterMovementComp->ApplyRootMotionSource(MoveToForce);
	if (KnockbackRootMotionSourceID == static_cast<uint16>(ERootMotionSourceID::Invalid))
	{
		return;
	}

	ReactState = EUPHitReact::Knockback;
	GetWorld()->GetTimerManager().SetTimer(
		ReactTimerHandle, this, &UUPHitReactionComponent::FinishReaction,
		CurrentHitEffect.KnockbackDuration, false
	);
}

void UUPHitReactionComponent::FinishReaction()
{
	GetWorld()->GetTimerManager().ClearTimer(ReactTimerHandle);
	ReactState = EUPHitReact::None;

	if (AnimInstance != nullptr && MontageInstanceID != INDEX_NONE)
	{
		if (FAnimMontageInstance* MontageInstance = AnimInstance->GetMontageInstanceForID(MontageInstanceID))
		{
			MontageInstance->OnMontageBlendingOutStarted.Unbind();
		}
	}

	if (OwnerCharacter == nullptr)
	{
		return;
	}

	if (OwnerCharacter->IsDead())
	{
		OwnerCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
	else
	{
		OwnerCharacter->SetControlLocked(false, TEXT("HitReaction"));
	}
}

void UUPHitReactionComponent::CancelReaction()
{
	GetWorld()->GetTimerManager().ClearTimer(ReactTimerHandle);

	if (ReactState == EUPHitReact::Knockback)
	{
		if (CharacterMovementComp != nullptr)
		{
			CharacterMovementComp->RemoveRootMotionSourceByID(KnockbackRootMotionSourceID);
		}
		KnockbackRootMotionSourceID = 0;
	}

	FinishReaction();

	if (AnimInstance != nullptr)
	{
		if (auto MontageInstance = AnimInstance->GetMontageInstanceForID(MontageInstanceID))
		{
			MontageInstance->Stop(MontageInstance->Montage->GetBlendOutArgs(), true);
		}
	}
}

void UUPHitReactionComponent::OnReactionMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	FinishReaction();
}

void UUPHitReactionComponent::OnHitStopFinished()
{
	RestorePlayRate(MontageInstanceID);

	if (CurrentHitEffect.HasKnockback())
	{
		StartKnockback();
		return;
	}
	
	FinishReaction();
}
