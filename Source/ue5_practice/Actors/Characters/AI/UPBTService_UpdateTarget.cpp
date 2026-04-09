// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPBTService_UpdateTarget.h"

#include "AIController.h"
#include "Actors/Characters/UPMonsterController.h"
#include "BehaviorTree/BlackboardComponent.h"


UUPBTService_UpdateClosestTarget::UUPBTService_UpdateClosestTarget()
{
	NodeName = TEXT("Update Closest Target");
	Interval = 0.5f;
	RandomDeviation = 0.1f;

	TargetKey.AddObjectFilter(
		this,
		GET_MEMBER_NAME_CHECKED(UUPBTService_UpdateClosestTarget, TargetKey),
		AActor::StaticClass()
	);
}

void UUPBTService_UpdateClosestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AUPMonsterController* MonsterController = Cast<AUPMonsterController>(OwnerComp.GetAIOwner());
	if (MonsterController == nullptr)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	AActor* ClosestEnemy = MonsterController->FindClosestEnemy();
	if (ClosestEnemy != nullptr)
	{
		BlackboardComp->SetValueAsObject(TargetKey.SelectedKeyName, ClosestEnemy);
	}
	else
	{
		BlackboardComp->ClearValue(TargetKey.SelectedKeyName);
	}
}
