// Copyright (C) 2022 Meizhouxuanhan.


#include "PALDialogWaitScriptRunner.h"

void APALDialogWaitScriptRunner::Init(UPALDialog* InDialog)
{
	Dialog = InDialog;
	MarkInitialized();
}

void APALDialogWaitScriptRunner::Clear()
{
	if (Dialog && Dialog->IsValidLowLevel())
	{
		Dialog->RemoveFromParent();
		PlayerController->InputComponent->RemoveActionBindingForHandle(BindedAction.GetHandle());
		MarkCompeted(true);
	}
}

void APALDialogWaitScriptRunner::BeginPlay()
{
	Super::BeginPlay();
	BindedAction = PlayerController->InputComponent->BindAction("Search", IE_Pressed, this, &APALDialogWaitScriptRunner::Clear);
}

void APALDialogWaitScriptRunner::TickRun(float DeltaTime)
{
	if (!Dialog || !Dialog->IsValidLowLevel() || !Dialog->IsInViewport())
	{
		if (BindedAction.IsValid())
		{
			PlayerController->InputComponent->RemoveActionBindingForHandle(BindedAction.GetHandle());
		}
		MarkCompeted(true);
	}
	// TODO Dialog icon
}