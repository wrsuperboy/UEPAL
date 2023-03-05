// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "PALPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

private:
	bool bAutoFadeOut = false;

public:
	void StartCameraAutoFade(float ToAlpha, float InFadeTime, FLinearColor InFadeColor, bool bInFadeAudio);

	bool IsFadingOut();
	
public:
	virtual void StartCameraFade(float FromAlpha, float ToAlpha, float Duration, FLinearColor Color, bool bShouldFadeAudio = false, bool bHoldWhenFinished = false) override;
	
	virtual void SetManualCameraFade(float InFadeAmount, FLinearColor Color, bool bInFadeAudio) override;

protected:
	/** Internal function conditionally called from UpdateCamera to do the actual work of updating the camera. */
	virtual void DoUpdateCamera(float DeltaTime) override;
};
