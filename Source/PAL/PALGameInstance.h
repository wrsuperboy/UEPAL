// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PALGameData.h"
#include "PALGameStateData.h"
#include "PALPlayerStateData.h"
#include "PAL.h"
#include "PALGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	SIZE_T MainPlayerCurrentSaveSlot;
	
private:
	EGameDistribution GameDistribution;

	FString GameResourcePath;

	UPROPERTY(VisibleAnywhere)
	UPALGameData* GameData;

	UPROPERTY(VisibleAnywhere)
	UPALGameStateData* GameStateData;

	UPROPERTY(VisibleAnywhere)
	UPALPlayerStateData* MainPlayerStateData;

	UPROPERTY(VisibleAnywhere)
	FUniqueNetIdRepl MainPlayerUniqueId;

public:
	void SetGameResourcePath(const FString& InGameResourcePath);

	FString GetGameResourcePath() const;

	void InitGameData();

	UPALGameData* GetGameData() const;

	UPALPlayerStateData* GetMainPlayerStateData() const;

	void SetMainPlayerStateData(UPALPlayerStateData* NewMainPlayerStateData);

	void SetGameStateData(UPALGameStateData* NewGameStateData);

	EGameDistribution GetGameDistribution() const;

	UPALGameStateData* GetGameStateData() const;

	ULocalPlayer* GetMainPlayer() const;

	bool IsMainPlayerSet() const;

	void SetMainPlayerByUniqueId(const FUniqueNetIdRepl& PlayerId);
};
