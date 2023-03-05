// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALScriptRunnerBase.h"
#include "PALSceneCameraActor.h"
#include "PALPanCameraScriptRunner.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALPanCameraScriptRunner : public APALScriptRunnerBase
{
	GENERATED_BODY()
	
private:
	FPALPosition3d Destination;

	float Speed;

	UPROPERTY()
	APALSceneCameraActor* CameraActor;

public:
	void Init(const FPALPosition3d& InDestination, float InTime);

protected:
	// Called every frame
	virtual void TickRun(float DeltaTime) override;
};
