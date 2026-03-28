// Copyright (c) 2026 Team Sparta. All rights reserved.

#include "UPAnimNotify_Hitbox.h"

#include "Actors/Characters/UPCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

UUPAnimNotify_Hitbox::UUPAnimNotify_Hitbox()
{
	bShouldFireInEditor = true;
	bIsNativeBranchingPoint = true;
}

static TAutoConsoleVariable<bool> CVarShowHitbox(
	TEXT("Debug.ShowHitbox"),
	false,
	TEXT("Draw hitbox debug shapes")
);

FString UUPAnimNotify_Hitbox::GetNotifyName_Implementation() const
{
	const UEnum* ShapeEnum = StaticEnum<EUPHitboxShape>();
	const FString ShapeName = ShapeEnum->GetNameStringByValue(static_cast<int64>(Shape));
	return FString::Printf(TEXT("히트박스: %s R=%.0f"), *ShapeName, Radius);
}

FLinearColor UUPAnimNotify_Hitbox::GetEditorColor()
{
	return FLinearColor::Red;
}

void UUPAnimNotify_Hitbox::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;
	if (MeshComp == nullptr)
	{
		return;
	}

	const UWorld* World = MeshComp->GetWorld();
	const bool bGameWorld = World != nullptr && World->IsGameWorld();
	const FVector Center = GetHitboxLocation(MeshComp);

	if (CVarShowHitbox.GetValueOnGameThread() || bGameWorld == false)
	{
		DrawDebugHitbox(World, Center);
	}

	if (bGameWorld == false)
	{
		return;
	}

	switch (Shape)
	{
	case EUPHitboxShape::Sphere:
		PerformSphereOverlap(MeshComp);
		break;
	default:
		break;
	}
}

void UUPAnimNotify_Hitbox::PerformSphereOverlap(USkeletalMeshComponent* MeshComp) const
{
	const AUPCharacter* OwnerCharacter = Cast<AUPCharacter>(MeshComp->GetOwner());
	if (OwnerCharacter == nullptr)
	{
		return;
	}

	const FVector Center = GetHitboxLocation(MeshComp);

	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams Params;

	const bool bHit = OwnerCharacter->GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Center,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	if (bHit == false)
	{
		return;
	}

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* HitActor = Overlap.GetActor();
		if (HitActor == nullptr)
		{
			continue;
		}

		// Attitude 기반 팀 판정: Friendly면 무시
		const ETeamAttitude::Type Attitude = FGenericTeamId::GetAttitude(OwnerCharacter, HitActor);
		if (Attitude == ETeamAttitude::Friendly)
		{
			continue;
		}

		AUPCharacter* HitCharacter = Cast<AUPCharacter>(HitActor);
		if (HitCharacter == nullptr)
		{
			continue;
		}

		HitCharacter->ApplyDamage(OwnerCharacter->GetATK());
	}
}

FVector UUPAnimNotify_Hitbox::GetHitboxLocation(USkeletalMeshComponent* MeshComp) const
{
	// 에디터 노출: X=Forward, Y=Right, Z=Up (액터 컨벤션)
	// 메시 로컬:  Y=Forward, -X=Right, Z=Up
	// 메시 트랜스폼으로 통일하면 에디터 프리뷰/게임 모두 동일하게 동작
	const FVector MeshLocalOffset(-Offset.Y, Offset.X, Offset.Z);
	return MeshComp->GetComponentTransform().TransformPosition(MeshLocalOffset);
}

void UUPAnimNotify_Hitbox::DrawDebugHitbox(const UWorld* World, const FVector& Center) const
{
	if (World == nullptr)
	{
		return;
	}

	DrawDebugSphere(
		World,
		Center,
		Radius,
		16,
		FColor::Red,
		false,
		1.0f
	);
}
