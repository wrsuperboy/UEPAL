// Copyright (C) 2022 Meizhouxuanhan.


#include "PALAudioManager.h"
#include "AudioDevice.h"
#include "Audio/ActorSoundParameterInterface.h"
#include "Sound/SoundWaveProcedural.h"
#include "PALGameInstance.h"
#include "PALWaveSound.h"
#include "PALVocSound.h"
#include "PALCommon.h"

static UAudioComponent* CreateSound2D(const UObject* WorldContextObject, USoundBase* Sound, float VolumeMultiplier, float PitchMultiplier, float StartTime, USoundConcurrency* ConcurrencySettings, bool bPersistAcrossLevelTransition, bool bAutoDestroy)
{
	if (!Sound || !GEngine || !GEngine->UseSound())
	{
		return nullptr;
	}

	UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!ThisWorld || ThisWorld->IsNetMode(NM_DedicatedServer))
	{
		return nullptr;
	}

	// Derive an owner from the WorldContextObject
	UObject* MutableWorldContext = const_cast<UObject*>(WorldContextObject);
	AActor* WorldContextOwner = Cast<AActor>(MutableWorldContext);

	FAudioDevice::FCreateComponentParams Params = bPersistAcrossLevelTransition
		? FAudioDevice::FCreateComponentParams(ThisWorld->GetAudioDeviceRaw())
		: FAudioDevice::FCreateComponentParams(ThisWorld, WorldContextOwner);

	if (ConcurrencySettings)
	{
		Params.ConcurrencySet.Add(ConcurrencySettings);
	}

	UAudioComponent* AudioComponent = FAudioDevice::CreateComponent(Sound, Params);
	if (AudioComponent)
	{
		AudioComponent->SetVolumeMultiplier(VolumeMultiplier);
		AudioComponent->SetPitchMultiplier(PitchMultiplier);
		AudioComponent->bAllowSpatialization = false;
		AudioComponent->bIsUISound = true;
		AudioComponent->bAutoDestroy = bAutoDestroy;
		AudioComponent->bIgnoreForFlushing = bPersistAcrossLevelTransition;
		AudioComponent->SubtitlePriority = Sound->GetSubtitlePriority();
		AudioComponent->bStopWhenOwnerDestroyed = false;
	}
	return AudioComponent;
}

static void PlaySoundForActor(const UObject* WorldContextObject, class USoundBase* Sound, AActor* OwningActor, float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f, class USoundAttenuation* AttenuationSettings = nullptr, USoundConcurrency* ConcurrencySettings = nullptr, UInitialActiveSoundParams* InitialParams = nullptr)
{
	if (!Sound || !GEngine || !GEngine->UseSound() || !OwningActor)
	{
		return;
	}

	UWorld* ThisWorld = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!ThisWorld || ThisWorld->IsNetMode(NM_DedicatedServer))
	{
		return;
	}

	if (FAudioDeviceHandle AudioDevice = ThisWorld->GetAudioDevice())
	{
		TArray<FAudioParameter> Params;
		if (InitialParams)
		{
			Params.Append(MoveTemp(InitialParams->AudioParams));
		}

		const AActor* ActiveSoundOwner = OwningActor;
		const FVector Location = OwningActor->GetActorLocation();

		UActorSoundParameterInterface::Fill(ActiveSoundOwner, Params);

		AudioDevice->PlaySoundAtLocation(Sound, ThisWorld, VolumeMultiplier, PitchMultiplier, StartTime, Location, FRotator::ZeroRotator, AttenuationSettings, ConcurrencySettings, &Params, ActiveSoundOwner);
	}
}

void UPALAudioManager::PlayMusic(SIZE_T MusicNum, bool bLoop, float FadeTime)
{
	MusicProcedural->Play(MusicNum, bLoop, FadeTime);
	MusicAudioComponent->Activate();
}

void UPALAudioManager::PlaySound(int32 InSoundNum, AActor* OwningActor)
{
	if (!bIsSoundEnabled)
	{
		return;
	}

	SIZE_T SoundNum = FMath::Abs<int32>(InSoundNum);
	int32 SoundDataSize;
	UPALCommon* Common = GetWorld()->GetGameInstance()->GetSubsystem<UPALCommon>();
	uint8* SoundData = Common->LoadSound(SoundNum, SoundDataSize);
	if (!SoundData)
	{
		return;
	}
	USoundWave* SoundWave;
	if (Common->GetSoundFormat() == EPALSoundFormat::VOC)
	{
		UPALVocSound* Sound = NewObject<UPALVocSound>(GetWorld());
		Sound->Init(SoundData, SoundDataSize);
		SoundWave = Sound;
	}
	else
	{
		UPALWaveSound* Sound = NewObject<UPALWaveSound>(GetWorld());
		Sound->Init(SoundData, SoundDataSize);
		SoundWave = Sound;
	}
	FMemory::Free(SoundData);

	if (OwningActor)
	{
		SoundWave->SoundGroup = ESoundGroup::SOUNDGROUP_Voice;
		PlaySoundForActor(GetWorld(), SoundWave, OwningActor);
	}
	else
	{
		CreateSound2D(GetWorld(), SoundWave, 1.f, 1.f, 0.f, nullptr, false, true)->Activate();
	}
}

bool UPALAudioManager::PlayCDTrack(int32 TrackNum)
{
	return false;
}

void UPALAudioManager::EnableMusic(bool bEnabled)
{
	if (MusicAudioComponent)
	{
		MusicAudioComponent->SetVolumeMultiplier(bEnabled ? 1 : 0);
		MusicAudioComponent->Activate();
	}
}

void UPALAudioManager::EnableSound(bool bEnabled)
{
	bIsSoundEnabled = bEnabled;
}

bool UPALAudioManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	UWorld* World = Cast<UWorld>(Outer);
	return World && World->IsGameWorld();
}

void UPALAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	UPALGameInstance* GameInstance = GetWorld()->GetGameInstance<UPALGameInstance>();
	if (GameInstance)
	{
		FString MusicMKFPath = GameInstance->GetGameResourcePath();
		MusicMKFPath.Append("Mus.mkf");
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		IFileHandle* MusicFileHanlde = PlatformFile.OpenRead(*MusicMKFPath);
		MusicProcedural = NewObject<UPALMusicProcedural>(GetWorld());
		MusicProcedural->Load(MusicFileHanlde);
		MusicAudioComponent = CreateSound2D(GetWorld(), MusicProcedural, 1.f, 1.f, 0.f, nullptr, false, false);
		bIsSoundEnabled = true;
	}
}

void UPALAudioManager::Deinitialize()
{
}
