// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleEnemyData.h"

void UPALBattleEnemyData::Init(uint16 InObjectId, const FEnemy& InEnemy)
{
	FMemory::Memcpy(&Enemy, &InEnemy, sizeof(FEnemy));
	ObjectId = InObjectId;
	bKnockOuted = false;
}

uint16 UPALBattleEnemyData::GetObjectId() const
{
	return ObjectId;
}

void UPALBattleEnemyData::KnockOut()
{
	bKnockOuted = true;
}

bool UPALBattleEnemyData::IsKnockedOuted() const
{
	return bKnockOuted;
}

uint16 UPALBattleEnemyData::GetDexterity() const
{
	return Enemy.Dexterity + (Enemy.Level + 6) * 3;
}

uint16 UPALBattleEnemyData::GetDefense() const
{
	return Enemy.Defense + (Enemy.Level + 6) * 4;
}

uint16 UPALBattleEnemyData::GetAttackStrength() const
{
	return Enemy.AttackStrength + (Enemy.Level + 6) * 6;
}

uint16 UPALBattleEnemyData::GetPhysicalResistance() const
{
	return Enemy.PhysicalResistance;
}
