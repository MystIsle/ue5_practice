#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EUPTeamId : uint8
{
	Player = 0,
	Monster = 1,
	
	NoTeam = 255
};