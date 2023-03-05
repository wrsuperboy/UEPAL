// Copyright (C) 2022 Meizhouxuanhan.


#include "PALPlayerCameraManager.h"

void APALPlayerCameraManager::StartCameraAutoFade(float ToAlpha, float InFadeTime, FLinearColor InFadeColor, bool bInFadeAudio)
{
	StartCameraFade(FadeAmount, ToAlpha, InFadeTime, InFadeColor, bInFadeAudio, true);
	bAutoFadeOut = true;
}

bool APALPlayerCameraManager::IsFadingOut()
{
	return bEnableFading && (FadeAlpha.X < FadeAlpha.Y || FMath::IsNearlyEqual(FadeAlpha.Y, 1.0f));
}

void APALPlayerCameraManager::StartCameraFade(float FromAlpha, float ToAlpha, float Duration, FLinearColor Color, bool bShouldFadeAudio, bool bHoldWhenFinished)
{
	Super::StartCameraFade(FromAlpha, ToAlpha, Duration, Color, bShouldFadeAudio, bHoldWhenFinished);
	bAutoFadeOut = false;
}

void APALPlayerCameraManager::SetManualCameraFade(float InFadeAmount, FLinearColor Color, bool bInFadeAudio)
{
	Super::SetManualCameraFade(InFadeAmount, Color, bInFadeAudio);
	bAutoFadeOut = false;
}

void APALPlayerCameraManager::DoUpdateCamera(float DeltaTime)
{
	bool bOriginalEnableFading = bEnableFading;
	// bEnableFading = false;

	Super::DoUpdateCamera(DeltaTime);

	bEnableFading = bOriginalEnableFading;
	if (bEnableFading)
	{
		if (bAutoAnimateFade)
		{
			FadeTimeRemaining = FMath::Max(FadeTimeRemaining - DeltaTime, 0.0f);
			if (FadeTime > 0.0f)
			{
				FadeAmount = FadeAlpha.X + ((1.f - FadeTimeRemaining / FadeTime) * (FadeAlpha.Y - FadeAlpha.X));
			}

			if ((bHoldFadeWhenFinished == false) && (FadeTimeRemaining <= 0.f))
			{
				// done
				StopCameraFade();
			}
			if (bHoldFadeWhenFinished && (FadeTimeRemaining <= 0.f) && bAutoFadeOut)
			{
				StartCameraFade(FadeAmount, 0, FadeTime, FLinearColor::Black, bFadeAudio, false);
			}
		}

		if (bFadeAudio)
		{
			ApplyAudioFade();
		}
	}
}
