// Copyright (C) 2022 Meizhouxuanhan.


#include "PALBattleCameraActor.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"
#include "PAL.h"

APALBattleCameraActor::APALBattleCameraActor()
{
	GetCameraComponent()->SetConstraintAspectRatio(false);
	GetCameraComponent()->SetRelativeLocation(FVector3d(0, 1300., 450.));
	GetCameraComponent()->SetRelativeRotation(FRotator(-20., -90., 0));

	GetCameraComponent()->PostProcessSettings.bOverride_AutoExposureLowPercent = true;
	GetCameraComponent()->PostProcessSettings.AutoExposureLowPercent = 70;
	GetCameraComponent()->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	GetCameraComponent()->PostProcessSettings.AutoExposureMinBrightness = 2.3;
}
