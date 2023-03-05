// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PALTimedWaitScriptRunner.generated.h"

UCLASS()
class PAL_API APALTimedWaitScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()

private:
	float EndWaitTime;

public:
	void Init(float TimeSpan);

protected:	
	// Called every frame
	virtual void TickRun(float DeltaTime) override;

};
