// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PALGameStateData.h"
#include "PALScenePlayerController.h"
#include "PAL.h"
#include "PALGameState.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALGameState : public AGameStateBase
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnyWhere)
	UPALGameStateData* GameStateData = nullptr;

public:
	UPALGameStateData* GetGameStateData() const;

	bool IsInMainGame() const;

	void InitGameStateData(SIZE_T SaveSlot);

	void SaveGameStateData(FSavedGame& OutSavedGame);

	void NPCWalkOneStep(const uint16 EventObjectId, const int32 Speed);

	bool NPCWalkTo(const uint16 EventObjectId, const FPALPosition3d& Position, const int32 Speed);

	void MonsterChasePlayer(const uint16 EventObjectId, const int32 Speed, uint16 ChaseRange, bool bFloating, APALScenePlayerController* PlayerController);

private:
	UPALGameStateData* LoadDefaultGame() const;

	UPALGameStateData* LoadSavedGame(SIZE_T SaveSlot);

public:
	virtual void ReceivedGameModeClass();

	virtual void HandleBeginPlay();

};
