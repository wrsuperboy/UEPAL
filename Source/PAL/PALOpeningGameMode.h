// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALGameModeBase.h"
#include "PALOpeningGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALOpeningGameMode : public APALGameModeBase
{
	GENERATED_BODY()

public:
	APALOpeningGameMode();

public:
	void StartMainGame(SIZE_T SaveSlotNum);

public:
	virtual void StartPlay() override;
};
