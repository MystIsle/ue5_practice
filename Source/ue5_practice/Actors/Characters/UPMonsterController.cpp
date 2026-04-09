// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPMonsterController.h"

#include "BrainComponent.h"
#include "UPCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"


AUPMonsterController::AUPMonsterController()
{
	PrimaryActorTick.bCanEverTick = true;

	UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);
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

void AUPMonsterController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UE_LOG(LogTemp, Log, TEXT("[%s] OnTargetPerceptionUpdated: %s, Sensed: %s"),
		*GetName(),
		*Actor->GetName(),
		Stimulus.WasSuccessfullySensed() ? TEXT("true") : TEXT("false"));

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComp->SetValueAsObject(TargetActorKeyName, Actor);
	}
}

void AUPMonsterController::OnTargetPerceptionForgotten(AActor* Actor)
{
	UE_LOG(LogTemp, Log, TEXT("[%s] OnTargetPerceptionForgotten: %s"),
		*GetName(),
		*Actor->GetName());

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	if (BlackboardComp->GetValueAsObject(TargetActorKeyName) == Actor)
	{
		BlackboardComp->ClearValue(TargetActorKeyName);
	}
}
