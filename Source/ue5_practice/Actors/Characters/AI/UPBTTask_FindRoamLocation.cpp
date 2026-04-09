// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "Actors/Characters/AI/UPBTTask_FindRoamLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"


UUPBTTask_FindRoamLocation::UUPBTTask_FindRoamLocation()
{
	NodeName = TEXT("Find Roam Location");

	SpawnLocationKey.AddVectorFilter(this,
		GET_MEMBER_NAME_CHECKED(UUPBTTask_FindRoamLocation, SpawnLocationKey));
	SpawnLocationKey.SelectedKeyName = TEXT("SpawnLocation");
	
	RoamingLocationKey.AddVectorFilter(this,
		GET_MEMBER_NAME_CHECKED(UUPBTTask_FindRoamLocation, RoamingLocationKey));
	RoamingLocationKey.SelectedKeyName = TEXT("RoamingLocation");
}

EBTNodeResult::Type UUPBTTask_FindRoamLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AiController = OwnerComp.GetAIOwner();
	if (AiController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const FVector SpawnLocation = BlackboardComp->GetValueAsVector(SpawnLocationKey.SelectedKeyName);

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(AiController->GetWorld());
	if (NavigationSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation Result;
	if (NavigationSystem->GetRandomReachablePointInRadius(SpawnLocation, RoamingRadius, Result) == false)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComp->SetValueAsVector(RoamingLocationKey.SelectedKeyName, Result.Location);

	return EBTNodeResult::Succeeded;
}
