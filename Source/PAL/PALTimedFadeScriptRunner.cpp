// Copyright (C) 2023 Meizhouxuanhan.


#include "PALTimedFadeScriptRunner.h"
#include "PALPlayerCameraManager.h"

void APALTimedFadeScriptRunner::Init(bool bInFadeInOrOut, float InDelayTime)
{
	EndWaitTime = GetWorld()->GetTimeSeconds() + bInFadeInOrOut ? DelayTime : (DelayTime / 2);
	bFadeInOrOut = bInFadeInOrOut;
	DelayTime = InDelayTime;
	bStarted = false;
	MarkInitialized();
}

// Called every frame
void APALTimedFadeScriptRunner::TickRun(float DeltaTime)
{
	if (!bStarted)
	{
		bStarted = true;
		if (bFadeInOrOut)
		{
			Cast<APALPlayerCameraManager>(PlayerController->PlayerCameraManager)->StartCameraFade(1, 0, DelayTime, FLinearColor::Black, false, false);
		}
		else
		{
			Cast<APALPlayerCameraManager>(PlayerController->PlayerCameraManager)->StartCameraAutoFade(1, DelayTime, FLinearColor::Black, false);
		}
	}
	else if (EndWaitTime < GetWorld()->GetTimeSeconds())
	{
		MarkCompeted(true);
	}
}
