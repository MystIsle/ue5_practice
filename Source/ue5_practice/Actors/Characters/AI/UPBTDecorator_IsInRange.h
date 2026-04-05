// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "UPBTDecorator_IsInRange.generated.h"

UCLASS()
class UE5_PRACTICE_API UUPBTDecorator_IsInRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UUPBTDecorator_IsInRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category="Combat")
	float Range = 150.f;
};
