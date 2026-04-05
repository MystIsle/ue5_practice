// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPMonsterController.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"


AUPMonsterController::AUPMonsterController()
{
	PrimaryActorTick.bCanEverTick = false;

	UAIPerceptionComponent* PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);
}

FGenericTeamId AUPMonsterController::GetGenericTeamId() const
{
	if (IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetPawn()))
	{
		return TeamAgentInterface->GetGenericTeamId();
	}

	return FGenericTeamId();
}

void AUPMonsterController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UAIPerceptionComponent* PerceptionComp = GetAIPerceptionComponent())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
	}
}

void AUPMonsterController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComp->SetValueAsObject(TargetActorKeyName, Actor);
		return;
	}
	
	if (BlackboardComp->GetValueAsObject(TargetActorKeyName) == Actor)
	{
		BlackboardComp->ClearValue(TargetActorKeyName);
	}
}

