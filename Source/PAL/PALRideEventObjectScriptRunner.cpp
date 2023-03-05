// Copyright (C) 2022 Meizhouxuanhan.


#include "PALRideEventObjectScriptRunner.h"
#include "EngineUtils.h"

void APALRideEventObjectScriptRunner::Init(uint16 InEventObjectId, const FPALPosition3d& Position, int32 InSpeed)
{
	for (TActorIterator<APALEventObjectActor> It(GetWorld(), APALEventObjectActor::StaticClass()); It; ++It)
	{
		APALEventObjectActor* EventObjectActor = *It;
		if (EventObjectActor->IsValidLowLevelFast() && EventObjectActor->GetEventObjectId() == InEventObjectId)
		{
			EventObject = EventObjectActor;
		}
	}
	if (!EventObject)
	{
		MarkCompeted(false);
	}
	Destination = Position;
	Speed = InSpeed;
	AccTime = 0;
	MarkInitialized();
}

void APALRideEventObjectScriptRunner::TickRun(float DeltaTime)
{
	AccTime += DeltaTime;
	if (AccTime > FRAME_TIME)
	{
		AccTime = 0;
		const FPALPosition3d& PartyPosition = PlayerController->GetPartyPosition();
		double XOffset = Destination.X - PartyPosition.X;
		double YOffset = Destination.Y - PartyPosition.Y;
		double ZOffset = Destination.Z - PartyPosition.Z;

		if (FMath::IsNearlyZero(XOffset) && FMath::IsNearlyZero(YOffset) && FMath::IsNearlyZero(ZOffset))
		{
			PlayerController->PartySetWalking();
			MarkCompeted(true);
			return;
		}

		PlayerController->PartyStopWalking();
		float RealSpeed = Speed * 2 * FMath::Sqrt(3.) * PIXEL_TO_UNIT / FRAME_TIME;
		PlayerController->PartySetSpeed(RealSpeed);
		EventObject->SetSpeed(RealSpeed);

		if (YOffset < 0)
		{
			PlayerStateData->PartyDirection = ((XOffset < 0) ? EPALDirection::West : EPALDirection::North);
		}
		else
		{
			PlayerStateData->PartyDirection = ((XOffset < 0) ? EPALDirection::South : EPALDirection::East);
		}

		double DX = (FMath::Abs(XOffset) > Speed * 2) ? (Speed * (XOffset < 0 ? -2 : 2)) : XOffset;
		double DY = (FMath::Abs(YOffset) > Speed * 2) ? (Speed * (YOffset < 0 ? -2 : 2)) : YOffset;
		double DZ = (FMath::Abs(ZOffset) > Speed * 2) ? (Speed * (ZOffset < 0 ? -2 : 2)) : ZOffset;

		PlayerController->PartyMoveTo(FPALPosition3d(PartyPosition.X + DX, PartyPosition.Y + DY, PartyPosition.Z + DZ));
		const FPALPosition3d& EventObjectPosition = EventObject->GetPosition();
		EventObject->MoveTo(FPALPosition3d(EventObjectPosition.X + DX, EventObjectPosition.Y + DY, EventObjectPosition.Z + DZ));
	}
}