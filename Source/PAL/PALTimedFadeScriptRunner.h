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
	float EndWaitTime;

public:
	void Init(bool bFadeInOrOut, float DelayTime);

protected:
	// Called every frame
	virtual void TickRun(float DeltaTime) override;
};
