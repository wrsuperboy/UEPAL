// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShakeBase.h"
#include "PALCameraShake.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UPALCameraShake(const FObjectInitializer& ObjectInitializer);
	
};
