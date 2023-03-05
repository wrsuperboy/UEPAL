// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PALGameStateData.h"
#include "PALOpeningMenu.h"
#include "PALGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	APALGameModeBase();

public:
	void FadeOut(float DelayTime);

	void FadeIn(float DelayTime);

public:
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer);
};
