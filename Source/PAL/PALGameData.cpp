// Copyright (C) 2022 Meizhouxuanhan.


#include "PALGameData.h"

void UPALGameData::FinishDestroy()
{
	if (_EventObjects)
	{
		FMemory::Free(_EventObjects);
	}

	if (ScriptEntries)
	{
		FMemory::Free(ScriptEntries);
	}

	if (Stores)
	{
		FMemory::Free(Stores);
	}

	if (Enemies)
	{
		FMemory::Free(Enemies);
	}

	if (EnemyTeams)
	{
		FMemory::Free(EnemyTeams);
	}

	if (Magics)
	{
		FMemory::Free(Magics);
	}

	if (BattleFields)
	{
		FMemory::Free(BattleFields);
	}

	if (LevelUpMagicAlls)
	{
		FMemory::Free(LevelUpMagicAlls);
	}

	Super::FinishDestroy();
}