// Copyright (C) 2022 Meizhouxuanhan.


#include "PALTimedWaitScriptRunner.h"

void APALTimedWaitScriptRunner::Init(float TimeSpan)
{
	EndWaitTime = GetWorld()->GetTimeSeconds() + TimeSpan;
	MarkInitialized();
}

// Called every frame
void APALTimedWaitScriptRunner::TickRun(float DeltaTime)
{
	if (EndWaitTime < GetWorld()->GetTimeSeconds())
	{
		MarkCompeted(true);
	}
}

