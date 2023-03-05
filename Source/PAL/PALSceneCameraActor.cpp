// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSceneCameraActor.h"
#include "Camera/CameraComponent.h"
#include "Math/UnrealMathUtility.h"
#include "PAL.h"

APALSceneCameraActor::APALSceneCameraActor()
{
	GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
	GetCameraComponent()->SetOrthoWidth(PIXEL_TO_UNIT * 500.);
	GetCameraComponent()->SetConstraintAspectRatio(false);
	GetCameraComponent()->SetRelativeLocation(FVector3d(0, FMath::Sqrt(3.) * 800., 800.));
	GetCameraComponent()->SetRelativeRotation(FRotator(-30., -90., 0));

	GetCameraComponent()->PostProcessSettings.bOverride_AutoExposureLowPercent = true;
	GetCameraComponent()->PostProcessSettings.AutoExposureLowPercent = 70;
	GetCameraComponent()->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	GetCameraComponent()->PostProcessSettings.AutoExposureMinBrightness = 2.3;
}
