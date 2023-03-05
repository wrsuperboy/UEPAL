// Copyright (C) 2022 Meizhouxuanhan.


#include "PALConfirmScriptRunner.h"

void APALConfirmScriptRunner::TickRun(float DeltaTime)
{
	if (!ConfirmMenu || !ConfirmMenu->IsValidLowLevel() || !ConfirmMenu->IsInViewport())
	{
		MarkCompeted(true);
	}
}
