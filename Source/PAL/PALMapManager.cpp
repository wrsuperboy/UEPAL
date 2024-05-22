// Copyright (C) 2022 Meizhouxuanhan.


#include "PALMapManager.h"
#include "EngineUtils.h"
#include "PALGameInstance.h"
#include "PALCommon.h"
#include "PALGameState.h"
#include "PALSceneGameMode.h"
#include "PAL.h"

UPALMapManager::UPALMapManager() : CurrentMapNum(0)
{
	Tiles = (uint32(*)[128][64][2])FMemory::Malloc(sizeof(uint32) * 128 * 64 * 2);
}

int32 GetMapTileTextureFrameNum(uint32(&Tiles)[128][64][2], const SIZE_T Layer, const SIZE_T X, const SIZE_T Y, const SIZE_T H)
{
	// Check for invalid parameters.
	check(X < 64 && Y < 128 && H < 2);

	// Get the tile data of the specified location.
	uint32 D = Tiles[Y][X][H];

	if (Layer == 0)
	{
		// Bottom layer
		return (D & 0xFF) | ((D >> 4) & 0x100);
	}
	else
	{
		// Top layer
		D >>= 16;
		return ((D & 0xFF) | ((D >> 4) & 0x100)) - 1;
	}
}

int32 GetMapTileHeight(uint32(&Tiles)[128][64][2], const SIZE_T Layer, const SIZE_T X, const SIZE_T Y, const SIZE_T H)
{
	// Check for invalid parameters.
	check(X < 64 && Y < 128 && H < 2);

	uint32 D = Tiles[Y][X][H];

	if (Layer)
	{
		D >>= 16;
	}

	D >>= 8;
	return (D & 0xf);
}

void UPALMapManager::LoadMap(const SIZE_T SceneNum, UWorld* CurrentWorld)
{
	UPALGameStateData* GameStateData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameStateData();
	SIZE_T MapNum = GameStateData->Scenes[SceneNum - 1].MapNum;

	UPALSprite* TileSprite;
	bool bSuccess = GetWorld()->GetGameInstance()->GetSubsystem<UPALCommon>()->GetMap(MapNum, *Tiles, TileSprite);
	if (!bSuccess)
	{
		return;
	}

	if (CurrentMapNum != MapNum)
	{
		SceneActor->ClearDecorators();
		PAL_DebugMsg(FString(TEXT("Creating map tiles for map number ")).Append(FString::FromInt(MapNum)));

		TileSprite->Preload();
		for (SIZE_T X = 0; X < 64; X++)
		{
			for (SIZE_T H = 0; H < 2; H++)
			{
				for (SIZE_T Y = 0; Y < 128; Y++)
				{
					int32 FrameNum = GetMapTileTextureFrameNum(*Tiles, 0, X, Y, H);
					if (FrameNum >= 0)
					{
						UTexture2D* Texture = TileSprite->GetFrame(FrameNum);
						int32 Z = GetMapTileHeight(*Tiles, 0, X, Y, H);
						SceneActor->SetTile(X, Y, H, FPALPosition3d((double)X * 32 + (double)H * 16, (double)Y * 32 + (double)H * 16 + (double)Z * 8 * SQRT_3, (double)Z * 8), Texture);
					}
				}
			}
		}

		for (SIZE_T X = 0; X < 64; X++)
		{
			for (SIZE_T H = 0; H < 2; H++)
			{
				for (SIZE_T Y = 0; Y < 128; Y++)
				{
					int32 FrameNum = GetMapTileTextureFrameNum(*Tiles, 1, X, Y, H);
					if (FrameNum >= 0)
					{
						UTexture2D* Texture = TileSprite->GetFrame(FrameNum);
						int32 Z = GetMapTileHeight(*Tiles, 1, X, Y, H);
						SceneActor->AddDecorator(FPALPosition3d((double)X * 32 + (double)H * 16, (double)Y * 32 + (double)H * 16 + (double)Z * 8 * SQRT_3, (double)Z * 8), Texture);
					}
				}
			}
		}

		CurrentMapNum = MapNum;
	}

	PAL_DebugMsg(TEXT("Destroying actors of the current map"));
	for (APALEventObjectActor* EventObjectActor : EventObjectActors)
	{
		EventObjectActor->SetActorTickEnabled(false);
		EventObjectActor->Destroy();
	}
	EventObjectActors.Reset();

	// Load event objects
	const uint16 IndexEnd = GameStateData->Scenes[SceneNum].EventObjectIndex;
	for (uint16 Index = GameStateData->Scenes[SceneNum - 1].EventObjectIndex; Index < IndexEnd; Index++)
	{
		APALEventObjectActor* EventObjectActor = CurrentWorld->SpawnActor<APALEventObjectActor>();
		EventObjectActor->Init(Index + 1);
		EventObjectActors.Add(EventObjectActor);
	}
}

bool IsMapTileBlocked(const uint32(&Tiles)[128][64][2], const SIZE_T X, const SIZE_T Y, const SIZE_T H)
{
	if (X >= 64 || Y >= 128 || H > 1)
	{
		return true;
	}

	return (Tiles[Y][X][H] & 0x2000) >> 13;
}

bool UPALMapManager::CheckObstacle(const FPALPosition3d& Position, const bool bCheckEventObject, const uint16 SelfEventObjectId) const
{
	FPALPosition2d Position2d = Position.to2d();
	int32 X = FMath::FloorToInt32(Position2d.X) / 32;
	int32 Y = FMath::FloorToInt32(Position2d.Y) / 16;
	int32 H = 0;

	// Avoid walk out of range, but allow to look out of map
	if (X < 0 || X >= 2048 || Y < 0 || Y >= 2048)
	{
		return true;
	}

	double Xr = Position2d.X - X * 32;
	double Yr = Position2d.Y - Y * 16;

	if (Xr + Yr * 2 >= 16)
	{
		if (Xr + Yr * 2 >= 48)
		{
			X++;
			Y++;
		}
		else if (32 - Xr + Yr * 2 < 16)
		{
			X++;
		}
		else if (32 - Xr + Yr * 2 < 48)
		{
			H = 1;
		}
		else
		{
			Y++;
		}
	}

	if (IsMapTileBlocked(*Tiles, X, Y, H))
	{
		return true;
	}

	if (bCheckEventObject)
	{
		// Loop through all event objects in the current scene

		for (TActorIterator<APALEventObjectActor> It(GetWorld(), APALEventObjectActor::StaticClass()); It; ++It)
		{
			APALEventObjectActor* EventObjectActor = *It;
			if (EventObjectActor->IsValidLowLevelFast() && EventObjectActor->GetEventObjectId() != SelfEventObjectId)
			{
				if (EventObjectActor->IsBlocking())
				{
					// Check for collision
					const FPALPosition3d& EventObjectPosition = EventObjectActor->GetPosition();
					if (FMath::Abs(EventObjectPosition.X - Position.X)
						+ FMath::Abs(EventObjectPosition.Y - Position.Y) < 16)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool UPALMapManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	UWorld* World = Cast<UWorld>(Outer);
	return World && World->GetWorldSettings()->DefaultGameMode == APALSceneGameMode::StaticClass();
}

void UPALMapManager::Initialize(FSubsystemCollectionBase& Collection)
{
	SceneActor = GetWorld()->SpawnActor<APALSceneActor>();
}

void UPALMapManager::FinishDestroy()
{
	FMemory::Free(Tiles);
	Super::FinishDestroy();
}
