// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UPBTTask_FindRoamLocation.generated.h"

UCLASS()
class UE5_PRACTICE_API UUPBTTask_FindRoamLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UUPBTTask_FindRoamLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector SpawnLocationKey;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector RoamingLocationKey;

	UPROPERTY(EditAnywhere, Category="Roaming", meta=(ClampMin="100.0"))
	float RoamingRadius = 500.f;
};
