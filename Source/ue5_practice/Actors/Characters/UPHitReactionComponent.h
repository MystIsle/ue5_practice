// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Combat/UPHitEffect.h"
#include "Components/ActorComponent.h"
#include "UPHitReactionComponent.generated.h"

class UCharacterMovementComponent;
struct FAnimMontageInstance;
class AUPCharacter;
class UAnimMontage;

enum class EUPHitReact : uint8
{
	None,
	React,
	HitStop,
	Knockback,
};

UCLASS()
class UE5_PRACTICE_API UUPHitReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUPHitReactionComponent();

	virtual void BeginPlay() override;

	FORCEINLINE bool IsReacting() const { return ReactState != EUPHitReact::None; }

	void PlayReact(const FUPHitEffect& HitEffect, const FVector& KnockbackDirection);

private:
	FAnimMontageInstance* PlayReactionMontage();
	void StartKnockback();
	void FinishReaction();
	void CancelReaction();
	void RestorePlayRate(int32 ReactMontageInstanceID);
	
	void OnHitStopFinished();
	void OnReactionMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TArray<TObjectPtr<UAnimMontage> > HitReactMontages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat")
	TArray<TObjectPtr<UAnimMontage> > DeathMontages;

private:
	UPROPERTY()
	TObjectPtr<AUPCharacter> OwnerCharacter;
	
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComp;

	UPROPERTY()
	TObjectPtr<UAnimInstance> AnimInstance;

	EUPHitReact ReactState = EUPHitReact::None;
	FUPHitEffect CurrentHitEffect;
	FVector CurrentKnockbackDirection;
	FTimerHandle ReactTimerHandle;
	int HitMontageIndex = 0;
	int32 MontageInstanceID = INDEX_NONE;
	uint16 KnockbackRootMotionSourceID = 0;
};
