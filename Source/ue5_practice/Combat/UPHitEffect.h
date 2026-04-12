// Copyright (c) 2026 Team Sparta. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UPHitEffect.generated.h"

class UCurveVector;

USTRUCT(BlueprintType)
struct FUPHitEffect
{
	GENERATED_BODY()

	/** 히트스탑 지속 시간 (초). 0이면 히트스탑 없음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitStop", meta=(ClampMin="0.0", ClampMax="1.0"))
	float HitStopDuration = 0.0f;

	/** 히트스탑 중 애니메이션 재생 속도. 0.01=거의 정지, 1=정상 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HitStop", meta=(ClampMin="0.01", ClampMax="1.0"))
	float AnimPlayRate = 0.01f;

	/** 넉백 이동 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Knockback", meta=(ClampMin="0.0"))
	float KnockbackDistance = 150.f;

	/** 넉백 이동 시간 (초). 0이면 넉백 없음 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Knockback", meta=(ClampMin="0.0"))
	float KnockbackDuration = 0.3f;

	/** 넉백 경로 커브. null이면 직선 이동 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Knockback")
	TObjectPtr<UCurveVector> KnockbackCurve;

	FORCEINLINE bool HasHitStop() const { return HitStopDuration > 0.f && AnimPlayRate < 1.0f; }
	FORCEINLINE bool HasKnockback() const { return KnockbackDuration > 0.f; }
	FORCEINLINE bool HasAnyEffect() const { return HasHitStop() || HasKnockback(); }
};
