// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "UPMonsterController.generated.h"

class UBehaviorTree;

enum class EUPTeamID : uint8;

UCLASS()
class UE5_PRACTICE_API AUPMonsterController : public AAIController
{
	GENERATED_BODY()

public:
	AUPMonsterController();

	virtual FGenericTeamId GetGenericTeamId() const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="AI")
	FName TargetActorKeyName = TEXT("TargetActor");
};
