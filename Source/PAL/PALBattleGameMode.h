// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALGameModeBase.h"
#include "PALBattlePlayerController.h"
#include "PALPlayerState.h"
#include "PALBattleGameMode.generated.h"

enum EPALBattlePhase : uint8
{
	SelectAction,
	PerformAction
};

enum EPALFighterState : uint8
{
	Waiting,  // waiting time
	AcceptingCommand,   // accepting command
	DoingMove,   // doing the actual move
};

struct FBattleAction
{
	EBattleActionType  ActionType;
	uint16             ItemOrMagicId;   // item/magic to use
	int16              Target;     // -1 for everyone
	//FLOAT              flRemainingTime;  // remaining waiting time before the action start
};

struct FBattleRole
{
	float TimeMeter;          // time-charging meter (0 = empty, 100 = full).
	EPALFighterState State;             // state of this role
	FBattleAction Action;               // action to perform
	FBattleAction PreviousAction;       // action of the previous turn
	bool bDefending;           // TRUE if player is defending
};

struct FBattleActionQueueItem
{
	bool bIsEnemy;
	uint16 Dexterity;
	SIZE_T Index;
	bool bIsSecond;
};

/**
 * 
 */
UCLASS()
class PAL_API APALBattleGameMode : public APALGameModeBase
{
	GENERATED_BODY()

public:
	APALBattleGameMode();
	
private:
	UPROPERTY(VisibleAnywhere)
	APALBattlePlayerController* MainPlayerControllerPrivate;

	UPROPERTY(VisibleAnywhere)
	APALPlayerState* MainPlayerStatePrivate;

	bool bBattleGroundInitialized;

	EPALBattleResult Result;

	TMap<SIZE_T, FBattleRole> BattleRoleMap;

	TMap<SIZE_T, EPALFighterState> EnemyIndexFighterStateMap;

	SIZE_T CurrentMovingRoleId;

	bool bEnemyCleared;

	bool bEnemyMoving;

	EPALBattlePhase Phase;

	bool bPreviousTurnAutoAttack;

	bool bPreviousRoleAutoAttack;

	bool bThisTurnCooperation;

	TArray<FBattleActionQueueItem> ActionQueue;

	float HidingTime;

public:
	bool IsEnemyCleared() const;

private:
	void LoadBattleActors();

	void LoadBattleBackground();

	void BattleSettle();

	void BattleBackupStat();

	void BattlePostActionCheck(bool bCheckRole);

	void BattleDisplayStatChange();

	void BattleEnemyPerformAction(SIZE_T EnemyIndex);

	void BattleCommitAction(bool bRepeat);

	void BattleRolePerformAction(SIZE_T RoleId);

	void BattleRoleCheckReady();

public:
	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;
};
