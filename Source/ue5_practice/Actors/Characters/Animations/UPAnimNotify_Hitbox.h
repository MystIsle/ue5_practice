// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "UPAnimNotify_Hitbox.generated.h"

UENUM(BlueprintType)
enum class EUPHitboxShape : uint8
{
	Sphere,
	Box,
	Capsule
};

UCLASS(DisplayName="Hitbox")
class UE5_PRACTICE_API UUPAnimNotify_Hitbox : public UAnimNotify
{
	GENERATED_BODY()

public:
	UUPAnimNotify_Hitbox();

	virtual FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override;

	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hitbox")
	EUPHitboxShape Shape = EUPHitboxShape::Sphere;

	//NOTE: 소켓을 이용하는 방법도 있습니다. RnD?
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hitbox")
	// FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hitbox")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hitbox", meta=(ClampMin="1.0"))
	float Radius = 50.0f;

private:
	void PerformSphereOverlap(USkeletalMeshComponent* MeshComp) const;
	FVector GetHitboxLocation(USkeletalMeshComponent* MeshComp) const;
	void DrawDebugHitbox(const UWorld* World, const FVector& Center) const;
};
