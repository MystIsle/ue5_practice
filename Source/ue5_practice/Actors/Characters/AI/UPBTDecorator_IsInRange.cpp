// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "Actors/Characters/AI/UPBTDecorator_IsInRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKey.h"
#include "Actors/Characters/UPCharacter.h"


UUPBTDecorator_IsInRange::UUPBTDecorator_IsInRange()
{
	NodeName = TEXT("Is In Range");

	TargetKey.AddObjectFilter(this,
	                          GET_MEMBER_NAME_CHECKED(UUPBTDecorator_IsInRange, TargetKey),
	                          AUPCharacter::StaticClass()
	);
}

bool UUPBTDecorator_IsInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return false;
	}

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetKey.SelectedKeyName));
	AActor* Self = Cast<AActor>(BlackboardComp->GetValueAsObject(FBlackboard::KeySelf));
	if (Target == nullptr || Self == nullptr)
	{
		return false;
	}
	
	// - 근접 공격인 경우 : 대상에 네비게이션 이동 가능한지 확인, 장애물 확인 

	const float Dist = FVector::Dist2D(Target->GetActorLocation(), Self->GetActorLocation());
	return Dist <= Range;
}
