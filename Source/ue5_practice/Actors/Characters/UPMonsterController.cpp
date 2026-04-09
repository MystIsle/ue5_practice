// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPMonsterController.h"

#include "BrainComponent.h"
#include "UPCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Damage.h"


AUPMonsterController::AUPMonsterController()
{
	PrimaryActorTick.bCanEverTick = true;

	UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);

	UAISenseConfig_Damage* DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	DamageSenseConfig->SetMaxAge(5.0f);
	PerceptionComp->ConfigureSense(*DamageSenseConfig);
}

FGenericTeamId AUPMonsterController::GetGenericTeamId() const
{
	return OwnerCharacter ? OwnerCharacter->GetGenericTeamId() : FGenericTeamId();
}

void AUPMonsterController::BeginPlay()
{
	Super::BeginPlay();

	if (UAIPerceptionComponent* PerceptionComp = GetAIPerceptionComponent())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
		PerceptionComp->OnTargetPerceptionForgotten.AddDynamic(this, &ThisClass::OnTargetPerceptionForgotten);
	}
}

void AUPMonsterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerCharacter = Cast<AUPCharacter>(InPawn);
	
	FVector SpawnLocation = OwnerCharacter->GetActorLocation();
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsVector(SpawnLocationKeyName, SpawnLocation);
	}
}

AActor* AUPMonsterController::FindClosestEnemy() const
{
	const UAIPerceptionComponent* PerceptionComp = GetAIPerceptionComponent();
	if (PerceptionComp == nullptr)
	{
		return nullptr;
	}

	TArray<AActor*> PerceivedActors;
	PerceptionComp->GetCurrentlyPerceivedActors(nullptr, PerceivedActors);

	APawn* OwnerPawn = GetPawn();
	if (OwnerPawn == nullptr)
	{
		return nullptr;
	}

	const FVector OwnerLocation = OwnerPawn->GetActorLocation();

	AActor* ClosestEnemy = nullptr;
	float ClosestDistSq = TNumericLimits<float>::Max();

	for (AActor* Actor : PerceivedActors)
	{
		if (Actor == nullptr)
		{
			continue;
		}

		const float DistSq = FVector::DistSquared2D(OwnerLocation, Actor->GetActorLocation());
		if (DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestEnemy = Actor;
		}
	}

	return ClosestEnemy;
}

void AUPMonsterController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	AActor* ClosestEnemy = FindClosestEnemy();
	if (ClosestEnemy != nullptr)
	{
		BlackboardComp->SetValueAsObject(TargetActorKeyName, ClosestEnemy);
	}
	else
	{
		BlackboardComp->ClearValue(TargetActorKeyName);
	}
}

void AUPMonsterController::OnTargetPerceptionForgotten(AActor* Actor)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	AActor* ClosestEnemy = FindClosestEnemy();
	if (ClosestEnemy != nullptr)
	{
		BlackboardComp->SetValueAsObject(TargetActorKeyName, ClosestEnemy);
	}
	else
	{
		BlackboardComp->ClearValue(TargetActorKeyName);
	}
}
