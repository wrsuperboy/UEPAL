// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PALDialog.h"
#include "PALDialogWaitScriptRunner.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALDialogWaitScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UPALDialog* Dialog;

	FInputActionBinding BindedAction;

public:
	void Init(UPALDialog* Dialog);

	void OnSearch();

	void Clear();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickRun(float DeltaTime) override;
};
