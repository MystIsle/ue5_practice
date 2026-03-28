// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "UPMonsterController.generated.h"

enum class EUPTeamID : uint8;

UCLASS()
class UE5_PRACTICE_API AUPMonsterController : public AAIController
{
	GENERATED_BODY()

public:
	AUPMonsterController();

	virtual FGenericTeamId GetGenericTeamId() const override;
};
