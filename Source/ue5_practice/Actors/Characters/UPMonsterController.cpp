// Copyright (c) 2026 Team Sparta. All rights reserved.


#include "UPMonsterController.h"


AUPMonsterController::AUPMonsterController()
{
	PrimaryActorTick.bCanEverTick = false;
}

FGenericTeamId AUPMonsterController::GetGenericTeamId() const
{
	if (IGenericTeamAgentInterface* TeamAgentInterface = Cast<IGenericTeamAgentInterface>(GetPawn()))
	{
		return TeamAgentInterface->GetGenericTeamId();
	}
	
	return FGenericTeamId();
}

