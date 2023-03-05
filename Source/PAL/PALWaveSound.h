// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWaveProcedural.h"
#include "PALWaveSound.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALWaveSound : public USoundWaveProcedural
{
	GENERATED_BODY()

public:
	UPALWaveSound(const FObjectInitializer& ObjectInitializer);
	
public:
	void Init(const uint8* InSrcBufferData, uint32 InSrcBufferDataSize);
};
