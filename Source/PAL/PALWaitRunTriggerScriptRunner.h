// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PALTriggerScriptActor.h"
#include "PALWaitRunTriggerScriptRunner.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALWaitRunTriggerScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	APALTriggerScriptActor* Waiter;

public:
	void Init(APALTriggerScriptActor* InWaiter);

protected:
	// Called every frame
	virtual void TickRun(float DeltaTime) override;
};
