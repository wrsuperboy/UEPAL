// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PAL.h"
#include "PALGameData.h"
#include "PALRoleData.h"
#include "PALBattleEnemyData.h"
#include "PALPlayerStateData.generated.h"

struct FTrail
{
	FPALPosition3d Position; // position
	EPALDirection Direction;
};

UCLASS()
class PAL_API UPALPlayerStateData : public UObject
{
	GENERATED_BODY()

public:
	uint32 Cash;

	// value of "collected" items
	int16 CollectValue;

	int16 Layer;

	UPROPERTY(VisibleAnywhere)
	TArray<UPALRoleData*> Party;

	UPROPERTY(VisibleAnywhere)
	TArray<UPALRoleData*> Follow;
	
	TArray<FTrail> Trails;

	EPALDirection PartyDirection;

	TArray<FInventoryItem> Inventory;

	FPoisonStatus PoisonStatus[MAX_POISONS][MAX_PLAYABLE_PLAYER_ROLES];

	FExperienceAll ExpAll;

	FPlayerRoles PlayerRoles;

// Below are properties that don't save, load when running

	FPlayerRoles EquipmentEffects[EPALBodyPart::_BodyPartCount];

	uint16 RoleStatus[MAX_PLAYER_ROLES][EPALStatus::_StatusCount];

	bool bAutoBattle;

	int16 CurrentEquipPart;

	SIZE_T LastPartyRoleId[MAX_PLAYER_ROLES];

	bool bInBattle;

	SIZE_T BattleMusicNum;

	SIZE_T BattleFieldNum;

	EPALBattleResult LastBattleResult;

	bool bHasTriggerScriptToRun;

	uint16 ScriptEntryToRun;

	uint16 EventObjectIdToRun;

	UPROPERTY(VisibleAnywhere)
	TArray<UPALBattleEnemyData*> CurrentEnemies;

	bool bCurrentEnemyIsBoss;

	int32 ExpGainedInBattle;

	int32 CashGainedInBattle;

	bool bBattleHiding;
};
