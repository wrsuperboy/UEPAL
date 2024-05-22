// Copyright (C) 2022 Meizhouxuanhan.


#include "PALCameraShake.h"
#include "Shakes/PerlinNoiseCameraShakePattern.h"
#include "PAL.h"

UPALCameraShake::UPALCameraShake(const FObjectInitializer& ObjectInitializer) : UCameraShakeBase(ObjectInitializer)
{
	UPerlinNoiseCameraShakePattern* ShakePattern = NewObject<UPerlinNoiseCameraShakePattern>();
	ShakePattern->LocationAmplitudeMultiplier = 4 * PIXEL_TO_UNIT;
	ShakePattern->LocationFrequencyMultiplier = 5.f;
	ShakePattern->Duration = 60.f;
	SetRootShakePattern(ShakePattern);
}
