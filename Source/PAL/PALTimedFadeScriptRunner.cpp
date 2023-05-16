// Copyright (C) 2023 Meizhouxuanhan.


#include "PALTimedFadeScriptRunner.h"
#include "PALPlayerCameraManager.h"

void APALTimedFadeScriptRunner::Init(bool bFadeInOrOut, float DelayTime)
{
	if (bFadeInOrOut)
	{
		EndWaitTime = GetWorld()->GetTimeSeconds() + DelayTime;
		Cast<APALPlayerCameraManager>(PlayerController->PlayerCameraManager)->StartCameraFade(1, 0, DelayTime, FLinearColor::Black, false, false);
	}
	else
	{
		EndWaitTime = GetWorld()->GetTimeSeconds() + DelayTime / 2;
		Cast<APALPlayerCameraManager>(PlayerController->PlayerCameraManager)->StartCameraAutoFade(1, DelayTime, FLinearColor::Black, false);
	}
	MarkInitialized();
}

// Called every frame
void APALTimedFadeScriptRunner::TickRun(float DeltaTime)
{
	if (EndWaitTime < GetWorld()->GetTimeSeconds())
	{
		MarkCompeted(true);
	}
}
