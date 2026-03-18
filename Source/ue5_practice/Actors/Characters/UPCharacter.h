// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UPCharacter.generated.h"

UCLASS()
class UE5_PRACTICE_API AUPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AUPCharacter();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	
	void ToggleSprint(bool bActive);
	
private:
	void UpdateMaxWalkSpeed();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Movement")
	float SprintSpeedRate = 2.f;
	
private:
	float OriginalWalkSpeed;
	bool bSprinting = false;
};
