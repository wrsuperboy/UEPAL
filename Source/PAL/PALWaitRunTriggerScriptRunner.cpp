// Copyright (C) 2022 Meizhouxuanhan.


#include "PALWaitRunTriggerScriptRunner.h"
#include "EngineUtils.h"

void APALWaitRunTriggerScriptRunner::Init(APALTriggerScriptActor* InWaiter)
{
	Waiter = InWaiter;
	MarkInitialized();
}

void APALWaitRunTriggerScriptRunner::TickRun(float DeltaTime)
{
	for (TActorIterator<APALTriggerScriptActor> It(GetWorld(), APALTriggerScriptActor::StaticClass()); It; ++It)
	{
		APALTriggerScriptActor* Actor = *It;
		if (Actor && Actor->IsValidLowLevel() && Actor != Waiter)
		{
			return;
		}
	}
	MarkCompeted(true);
}
