// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Components/AudioComponent.h"
#include "PALMusicProcedural.h"
#include "PALAudioManager.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALAudioManager : public UWorldSubsystem
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UPALMusicProcedural* MusicProcedural;

	UPROPERTY()
	UAudioComponent* MusicAudioComponent;

	bool bIsSoundEnabled;

public:
	void PlayMusic(SIZE_T MusicNum, bool bLoop, float FadeTime);

	void PlaySound(int32 SoundNum, AActor* OwningActor = nullptr);

	bool PlayCDTrack(int32 TrackNum);

	void EnableMusic(bool bEnabled);

	void EnableSound(bool bEnabled);

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
};
