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
