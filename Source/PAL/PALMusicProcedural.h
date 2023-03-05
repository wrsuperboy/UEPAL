// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundWaveProcedural.h"
#include "Opl.h"
#include "RixPlayer.h"
#include "PALMusicProcedural.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALMusicProcedural : public USoundWaveProcedural
{
	GENERATED_BODY()

public:
	UPALMusicProcedural(const FObjectInitializer& ObjectInitializer);

private:
	Opl* MyOpl;
	RixPlayer* MyRixPlayer;
	int16 Buffer[((49716 - 1) / 70  + 1) * 2];
	int16* Position;

	int32 MusicNum;
	bool bLoop;

	float StartFadeTime;
	int32 TotalFadeOutSamples;
	int32 TotalFadeInSamples;
	int32 RemainingFadeSamples;
	enum EFadeType : uint8 { NONE, FADE_IN, FADE_OUT };
	EFadeType FadeType;

	int32 NextMusicNum;
	bool bNextLoop;

	bool bReady;

public:
	void Play(int32 InMusicNum, bool bInLoop, float FadeTime);

	void FillBuffer(USoundWaveProcedural* Self, int32 SamplesToGenerate);

public:
	virtual void FinishDestroy() override;
};
