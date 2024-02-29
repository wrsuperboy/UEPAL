// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleEnemyData.h"

void UPALBattleEnemyData::Init(uint16 InObjectId, const FEnemy& InEnemy)
{
	FMemory::Memcpy(&Enemy, &InEnemy, sizeof(FEnemy));
	ObjectId = InObjectId;
}

uint16 UPALBattleEnemyData::GetObjectId() const
{
	return ObjectId;
}
