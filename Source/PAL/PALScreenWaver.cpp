// Copyright (C) 2022 Meizhouxuanhan.


#include "PALScreenWaver.h"
#include "PAL.h"

APALScreenWaver::APALScreenWaver() : APostProcessVolume()
{
	bUnbound = true;
	static ConstructorHelpers::FObjectFinder<UMaterial> ScreenWaveMaterialRef(TEXT("Material'/Game/PAL_ScreenWave.PAL_ScreenWave'"));
	Settings.AddBlendable(ScreenWaveMaterialRef.Object, 1.f);
}

