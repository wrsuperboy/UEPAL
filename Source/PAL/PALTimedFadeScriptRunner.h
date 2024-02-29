// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PALTimedFadeScriptRunner.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALTimedFadeScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()

private:
	bool bFadeInOrOut;

	float DelayTime;

	float EndWaitTime;

	bool bStarted;

public:
	void Init(bool bInFadeInOrOut, float InDelayTime);

protected:
	// Called every frame
	virtual void TickRun(float DeltaTime) override;
};
