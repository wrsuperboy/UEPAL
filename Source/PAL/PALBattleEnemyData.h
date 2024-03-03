// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PALGameData.h"
#include "PALBattleEnemyData.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALBattleEnemyData : public UObject
{
	GENERATED_BODY()

public:
	uint16 ScriptOnTurnStart;

	uint16 ScriptOnBattleEnd;

	uint16 ScriptOnReady;

	FEnemy Enemy;

	bool bKnockOuted;

	uint16 Status[EPALStatus::_StatusCount];

	FPoisonStatus PoisonStatus[MAX_POISONS];

public:
	void Init(uint16 InObjectId, const FEnemy& InEnemy);

	uint16 GetObjectId() const;

	void KnockOut();

	bool IsKnockedOuted() const;

	uint16 GetDexterity() const;

	uint16 GetDefense() const;

	uint16 GetAttackStrength() const;

	uint16 GetPhysicalResistance() const;
	
private:
	uint16 ObjectId;
};
