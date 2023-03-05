// Copyright (C) 2022 Meizhouxuanhan.


#include "PALGameState.h"
#include "PALGameInstance.h"
#include "PALSceneGameMode.h"
#include "PALMapManager.h"
#include "PALCommon.h"
#include "PAL.h"

UPALGameStateData* APALGameState::GetGameStateData() const
{
	return GameStateData;
}

bool APALGameState::IsInMainGame() const
{
	return GameStateData != nullptr;
}

void APALGameState::InitGameStateData(SIZE_T SaveSlot)
{
	if (SaveSlot == 0)
	{
		GameStateData = LoadDefaultGame();
	}
	else
	{
		GameStateData = LoadSavedGame(SaveSlot);
	}
	GetGameInstance<UPALGameInstance>()->SetGameStateData(GameStateData);
}

void APALGameState::SaveGameStateData(FSavedGame& s)
{
	FMemory::Memcpy(s.Scene, GameStateData->Scenes, sizeof(s.Scene));
	s.NumMusic = GameStateData->MusicNum;
	s.PaletteOffset = (GameStateData->bNightPalette ? 0x180 : 0);
	s.NumScene = GameStateData->SceneNum;
	s.ScreenWave = GameStateData->ScreenWave;
	s.ChaseRange = GameStateData->ChaseRange;
	s.ChasespeedChangeCycles = GameStateData->ChaseSpeedChangeCycles;
	FMemory::Memcpy(s.Object, GameStateData->Objects, sizeof(s.Object));
	FMemory::Memcpy(s.EventObject, GameStateData->EventObjects, sizeof(s.EventObject));
}

UPALGameStateData* APALGameState::LoadDefaultGame() const
{
	UPALGameStateData* Data = NewObject<UPALGameStateData>();
	// Set some default game data.
	Data->MusicNum = 0;
	Data->PaletteNum = 0;
	Data->SceneNum = 1;
	Data->ScreenWave = 0;
	Data->bNightPalette = false;
	Data->ChaseRange = 1;
	Data->ChaseSpeedChangeCycles = 0;
	UPALGameData* GameData = GetGameInstance<UPALGameInstance>()->GetGameData();
	FMemory::Memcpy(Data->Scenes, GameData->_Scenes);
	FMemory::Memcpy(Data->Objects, GameData->_Objects);
	FMemory::Memcpy(Data->EventObjects, GameData->_EventObjects, GameData->EventObjectCount * sizeof(FEventObject));
	Data->bInBattle = false;
	return Data;
}

UPALGameStateData* APALGameState::LoadSavedGame(SIZE_T SaveSlot)
{
	FSavedGame* s = (FSavedGame*)FMemory::Malloc(sizeof(FSavedGame));
	bool bSuccess = GetGameInstance()->GetSubsystem<UPALCommon>()->GetSavedGame(SaveSlot, *s);
	if (!bSuccess)
	{
		FMemory::Free(s);
		return LoadDefaultGame();
	}

	UPALGameStateData* Data = NewObject<UPALGameStateData>();
	FMemory::Memcpy(Data->Scenes, s->Scene, sizeof(Data->Scenes));
	Data->MusicNum = s->NumMusic;
	Data->PaletteNum = 0;
	Data->bNightPalette = (s->PaletteOffset != 0);
	Data->SceneNum = s->NumScene;
	Data->ScreenWave = s->ScreenWave;
	Data->ChaseRange = s->ChaseRange;
	Data->ChaseSpeedChangeCycles = s->ChasespeedChangeCycles;
	FMemory::Memcpy(Data->Objects, s->Object, sizeof(Data->Objects));
	FMemory::Memcpy(Data->EventObjects, s->EventObject, sizeof(Data->EventObjects));
	Data->bInBattle = false;
	FMemory::Free(s);
	return Data;
}

void APALGameState::NPCWalkOneStep(const uint16 EventObjectId, const int32 Speed)
{
	// Check for invalid parameters

	FEventObject* p = &(GameStateData->EventObjects[EventObjectId - 1]);

	// Move the event object by the specified direction
	p->X += ((p->Direction == EPALDirection::West || p->Direction == EPALDirection::South) ? -2 : 2) * Speed;
	p->Y += ((p->Direction == EPALDirection::West || p->Direction == EPALDirection::North) ? -1 : 1) * Speed;

	// Update the gesture
	if (p->SpriteFrames > 0)
	{
		p->CurrentFrameNum++;
		p->CurrentFrameNum %= (p->SpriteFrames == 3 ? 4 : p->SpriteFrames);
	}
	else if (p->SpriteFramesAuto > 0)
	{
		p->CurrentFrameNum++;
		p->CurrentFrameNum %= p->SpriteFramesAuto;
	}
}

bool APALGameState::NPCWalkTo(const uint16 EventObjectId, const FPALPosition3d& Position, const int32 Speed)
{
	FEventObject* EventObject = &(GameStateData->EventObjects[EventObjectId - 1]);

	double XOffset = Position.X - EventObject->X;
	double YOffset = Position.Y - EventObject->Y * 2;

	if (YOffset < 0)
	{
		EventObject->Direction = ((XOffset < 0) ? EPALDirection::West : EPALDirection::North);
	}
	else
	{
		EventObject->Direction = ((XOffset < 0) ? EPALDirection::South : EPALDirection::East);
	}

	if (FMath::Abs(XOffset) < Speed * 2 || FMath::Abs(YOffset) < Speed * 2)
	{
		EventObject->X = FMath::RoundToInt(Position.X);
		EventObject->Y = FMath::RoundToInt(Position.Y / 2);
	}
	else
	{
		NPCWalkOneStep(EventObjectId, Speed);
	}

	if (FMath::IsNearlyEqual(Position.X, EventObject->X) && FMath::IsNearlyEqual(Position.Y, EventObject->Y * 2))
	{
		EventObject->CurrentFrameNum = 0;
		return true;
	}

	return false;
}

void APALGameState::MonsterChasePlayer(const uint16 EventObjectId, const int32 Speed, uint16 ChaseRange, bool bFloating, APALPlayerController* PlayerController)
{
	int32 MonsterSpeed = 0;
	if (GameStateData->ChaseRange != 0)
	{
		FEventObject* EventObject = &(GameStateData->EventObjects[EventObjectId - 1]);
		const FPALPosition3d& PartyPosition = PlayerController->GetPartyPosition();

		double X = PartyPosition.X - EventObject->X;
		double Y = PartyPosition.Y - EventObject->Y * 2;

		if (FMath::IsNearlyZero(X))
		{
			X = FMath::RandRange(-1, 1);
		}

		if (FMath::IsNearlyZero(Y))
		{
			Y = FMath::RandRange(-1, 1);
		}

		uint16 PrevX = EventObject->X;
		uint16 PrevY = EventObject->Y;

		uint16 i = PrevX % 32;
		uint16 j = PrevY % 16;

		PrevX /= 32;
		PrevY /= 16;
		uint16 h = 0;

		if (i + j * 2 >= 16)
		{
			if (i + j * 2 >= 48)
			{
				PrevX++;
				PrevY++;
			}
			else if (32 - i + j * 2 < 16)
			{
				PrevX++;
			}
			else if (32 - i + j * 2 < 48)
			{
				h = 1;
			}
			else
			{
				PrevY++;
			}
		}

		PrevX = PrevX * 32 + h * 16;
		PrevY = PrevY * 16 + h * 8;

		//
		// Is the party near to the event object?
		//
		if (FMath::Abs(X) + FMath::Abs(Y) < ChaseRange * 32 * GameStateData->ChaseRange)
		{
			if (X < 0)
			{
				if (Y < 0)
				{
					EventObject->Direction = EPALDirection::West;
				}
				else
				{
					EventObject->Direction = EPALDirection::South;
				}
			}
			else
			{
				if (Y < 0)
				{
					EventObject->Direction = EPALDirection::North;
				}
				else
				{
					EventObject->Direction = EPALDirection::East;
				}
			}

			if (!FMath::IsNearlyZero(X))
			{
				X = EventObject->X + X / FMath::Abs(X) * 16;
			}
			else
			{
				X = EventObject->X;
			}

			if (Y != 0)
			{
				Y = EventObject->Y * 2 + Y / FMath::Abs(Y) * 16;
			}
			else
			{
				X = EventObject->Y * 2;
			}

			if (bFloating)
			{
				MonsterSpeed = Speed;
			}
			else
			{
				if (!GetWorld()->GetSubsystem<UPALMapManager>()->CheckObstacle(FPALPosition3d(X, Y, 0), true, EventObjectId))
				{
					MonsterSpeed = Speed;
				}
				else
				{
					EventObject->X = PrevX;
					EventObject->Y = PrevY;
				}

				for (SIZE_T l = 0; l < 4; l++)
				{
					switch (l)
					{
					case 0:
						EventObject->X -= 4;
						EventObject->Y += 2;
						break;

					case 1:
						EventObject->X -= 4;
						EventObject->Y -= 2;
						break;

					case 2:
						EventObject->X += 4;
						EventObject->Y -= 2;
						break;

					case 3:
						EventObject->X += 4;
						EventObject->Y += 2;
						break;
					}

					if (GetWorld()->GetSubsystem<UPALMapManager>()->CheckObstacle(FPALPosition3d(EventObject->X, EventObject->Y * 2, 0), false))
					{
						EventObject->X = PrevX;
						EventObject->Y = PrevY;
					}
				}
			}
		}
	}

	NPCWalkOneStep(EventObjectId, MonsterSpeed);
}

void APALGameState::SetDayNight(bool bNight)
{
	if (GameStateData->bNightPalette == bNight)
	{
		return;
	}
	GameStateData->bNightPalette = bNight;
	if (GameModeClass == APALSceneGameMode::StaticClass())
	{
		Cast<APALSceneGameMode>(AuthorityGameMode)->SetLight(GameStateData->PaletteNum, GameStateData->bNightPalette);
	}
}

void APALGameState::SetTone(SIZE_T PaletteNum)
{
	if (GameStateData->PaletteNum == PaletteNum)
	{
		return;
	}
	GameStateData->PaletteNum = PaletteNum;
	if (GameModeClass == APALSceneGameMode::StaticClass())
	{
		Cast<APALSceneGameMode>(AuthorityGameMode)->SetLight(GameStateData->PaletteNum, GameStateData->bNightPalette);
	}
}

void APALGameState::ReceivedGameModeClass() {
	Super::ReceivedGameModeClass();
	GameStateData = GetGameInstance<UPALGameInstance>()->GetGameStateData();
}

void APALGameState::HandleBeginPlay()
{
	PAL_DebugMsg("HandleBeginPlay");
	Super::HandleBeginPlay();
}
