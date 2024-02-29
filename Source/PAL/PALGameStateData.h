// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PALGameData.h"
#include "PALGameStateData.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALGameStateData : public UObject
{
	GENERATED_BODY()

public:
	FGDScene Scenes[MAX_SCENES];
	
	SIZE_T MusicNum;

	SIZE_T PaletteNum;

	bool bNightPalette;

	// Starts from 1
	SIZE_T SceneNum;

	uint32 ScreenWave;

	uint16 ChaseRange;

	uint16 ChaseSpeedChangeCycles;

	FObject Objects[MAX_OBJECTS];

	FEventObject EventObjects[MAX_EVENT_OBJECTS];
};
