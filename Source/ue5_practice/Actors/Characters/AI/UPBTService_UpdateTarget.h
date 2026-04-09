// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UPBTService_UpdateTarget.generated.h"

UCLASS()
class UE5_PRACTICE_API UUPBTService_UpdateClosestTarget : public UBTService
{
	GENERATED_BODY()

public:
	UUPBTService_UpdateClosestTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetKey;
};
