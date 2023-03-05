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

	UPROPERTY(VisibleAnywhere)
	UPALGameData* GameData;

	UPROPERTY(VisibleAnywhere)
	UPALGameStateData* GameStateData;

	UPROPERTY(VisibleAnywhere)
	UPALPlayerStateData* MainPlayerStateData;

	UPROPERTY(VisibleAnywhere)
	FUniqueNetIdRepl MainPlayerUniqueId;

public:
	EGameDistribution GetGameDistribution() const;

	UPALGameData* GetGameData() const;

	UPALGameStateData* GetGameStateData() const;

	void SetGameStateData(UPALGameStateData* NewGameStateData);

	UPALPlayerStateData* GetMainPlayerStateData() const;

	void SetMainPlayerStateData(UPALPlayerStateData* NewMainPlayerStateData);

	ULocalPlayer* GetMainPlayer() const;

	bool IsMainPlayerSet() const;

	void SetMainPlayerByUniqueId(const FUniqueNetIdRepl& PlayerId);

private:
	void InitGameData();

public:
	virtual void Init() override;

};
