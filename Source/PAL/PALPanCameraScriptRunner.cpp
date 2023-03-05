// Copyright (C) 2022 Meizhouxuanhan.


#include "PALPanCameraScriptRunner.h"

void APALPanCameraScriptRunner::Init(const FPALPosition3d& TargetPosition, float InTime)
{
	Destination = TargetPosition;
	const FVector3d& FromLocation = PlayerController->GetViewport().toLocation();
	Speed = FVector3d::Dist(FromLocation, Destination.toLocation()) / InTime;
	CameraActor = GetWorld()->SpawnActor<APALSceneCameraActor>();
	CameraActor->SetActorLocation(FromLocation);
	PlayerController->CameraRestoreNormal(); // Clear potential camera actors
	PlayerController->SetViewTarget(CameraActor);
	MarkInitialized();
}

void APALPanCameraScriptRunner::TickRun(float DeltaTime)
{
	const FVector3d& DestLocation = Destination.toLocation();
	FVector3d Step = DestLocation - CameraActor->GetActorLocation();
	if (!Step.IsNearlyZero())
	{
		Step.Normalize();
		Step *= Speed * DeltaTime;
		FVector3d Dest = CameraActor->GetActorLocation() + Step;
		if (FVector3d::DotProduct(Dest - DestLocation, Step) > 0)
		{
			CameraActor->SetActorLocation(DestLocation);
		}
		else
		{
			CameraActor->SetActorLocation(Dest);
		}
	}
	else
	{
		MarkCompeted(true);
	}
}
