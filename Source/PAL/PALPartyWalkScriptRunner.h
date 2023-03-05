// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PAL.h"
#include "PALPartyWalkScriptRunner.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALPartyWalkScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()

private:
	float AccTime;

	FPALPosition3d Destination;

	int32 Speed;

public:
	void Init(const FPALPosition3d& Position, int32 InSpeed);

protected:
	// Called every frame
	virtual void TickRun(float DeltaTime) override;

};
