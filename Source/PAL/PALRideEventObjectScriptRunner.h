// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PALEventObjectActor.h"
#include "PAL.h"
#include "PALRideEventObjectScriptRunner.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALRideEventObjectScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()

private:
	float AccTime;

	UPROPERTY()
	APALEventObjectActor* EventObject;

	FPALPosition3d Destination;

	int32 Speed;
	
public:
	void Init(uint16 InEventObjectId, const FPALPosition3d& Position, int32 InSpeed);

protected:
	// Called every frame
	virtual void TickRun(float DeltaTime) override;

};
