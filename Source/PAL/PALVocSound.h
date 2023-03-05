// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWaveProcedural.h"
#include "PALVocSound.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALVocSound : public USoundWaveProcedural
{
	GENERATED_BODY()
	
public:
	UPALVocSound(const FObjectInitializer& ObjectInitializer);

public:
	void Init(const uint8* InSrcBufferData, uint32 InSrcBufferDataSize);
};
