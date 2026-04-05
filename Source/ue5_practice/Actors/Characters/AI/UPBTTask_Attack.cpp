// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "Actors/Characters/AI/UPBTTask_Attack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Actors/Characters/UPCharacter.h"


UUPBTTask_Attack::UUPBTTask_Attack()
{
	NodeName = TEXT("Attack");
	bCreateNodeInstance = true;
	bNotifyTick = true;

	TargetKey.AddObjectFilter(this,
	                          GET_MEMBER_NAME_CHECKED(UUPBTTask_Attack, TargetKey),
	                          AUPCharacter::StaticClass()
	);
}

EBTNodeResult::Type UUPBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (AIC == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AUPCharacter* Character = Cast<AUPCharacter>(AIC->GetPawn());
	if (Character == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FVector Direction = Target->GetActorLocation() - Character->GetActorLocation();
	Direction.Z = 0.f;
	FRotator TargetRotation = Direction.Rotation();

	Character->Attack(TargetRotation);

	return EBTNodeResult::InProgress;
}

void UUPBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (AIC == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AUPCharacter* Character = Cast<AUPCharacter>(AIC->GetPawn());
	if (Character == nullptr)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Character->IsAttacking() == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
