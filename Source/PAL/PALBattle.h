#pragma once

#include "CoreMinimal.h"

constexpr float BATTLE_FRAME_TIME = 0.04f;

static uint16
CalculateBaseDamage(uint16 Strength, uint16 Defense)
{
	int16 Damage;

	// Formula courtesy of palxex and shenyanduxing
	if (Strength > Defense)
	{
		Damage = (int16)(Strength * 2 - Defense * 1.6 + 0.5);
	}
	else if (Strength > Defense * 0.6)
	{
		Damage = (int16)(Strength - Defense * 0.6 + 0.5);
	}
	else
	{
		Damage = 0;
	}

	return Damage;
}
