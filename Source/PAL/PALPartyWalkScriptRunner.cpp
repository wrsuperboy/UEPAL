// Copyright (C) 2022 Meizhouxuanhan.


#include "PALPartyWalkScriptRunner.h"

void APALPartyWalkScriptRunner::Init(const FPALPosition3d& Position, int32 InSpeed)
{
	Destination = Position;
	Speed = InSpeed;
	AccTime = 0;
	MarkInitialized();
}

void APALPartyWalkScriptRunner::TickRun(float DeltaTime)
{
	AccTime += DeltaTime;
	if (AccTime > FRAME_TIME)
	{
		AccTime = 0;

		const FPALPosition3d& PartyPosition = ScenePlayerController->GetPartyPosition();

		double XOffset = Destination.X - PartyPosition.X;
		double YOffset = Destination.Y - PartyPosition.Y;

		if (FMath::IsNearlyZero(XOffset) && FMath::IsNearlyZero(YOffset))
		{
			ScenePlayerController->PartyResetSpeed();
			MarkCompeted(true);
			return;
		}

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

		ScenePlayerController->PartyWalkTo(FPALPosition3d(PartyPosition.X + DX, PartyPosition.Y + DY, PartyPosition.Z));
		ScenePlayerController->PartySetSpeed(Speed * 2 * FMath::Sqrt(2.0) * PIXEL_TO_UNIT / FRAME_TIME);
	}
}