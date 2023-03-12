// Copyright (C) 2022 Meizhouxuanhan.


#include "PALMusicProcedural.h"

constexpr int32 SAMPLE_RATE = 44100;
constexpr int32 NUM_CHANNEL = 2;
constexpr int32 MIX_MAX_VOLUME = 128;

UPALMusicProcedural::UPALMusicProcedural(const FObjectInitializer& ObjectInitializer) : USoundWaveProcedural(ObjectInitializer)
{
	SoundGroup = ESoundGroup::SOUNDGROUP_Music;
	SetSampleRate(SAMPLE_RATE);
	NumChannels = NUM_CHANNEL;

	MyOpl = EmuOpl::CreateEmuopl(EOplCoreType::OPLCORE_NUKED, EOplChipType::OPLCHIP_OPL3, SAMPLE_RATE);
	MyRixPlayer = new RixPlayer(MyOpl);

	bReady = false;
	FadeType = EFadeType::NONE;
	MusicNum = -1;
	NextMusicNum = -1;
	Position = nullptr;
	bLoop = false;
	bNextLoop = false;

	OnSoundWaveProceduralUnderflow.BindUObject(this, &UPALMusicProcedural::FillBuffer);
}

void UPALMusicProcedural::Load(IFileHandle* RixMKFFileHandle)
{
	if (RixMKFFileHandle)
	{
		MyRixPlayer->Load(RixMKFFileHandle, true);
		bReady = true;
	}
}

void UPALMusicProcedural::Play(int32 InMusicNum, bool bInLoop, float FadeTime)
{
	if (InMusicNum == MusicNum && NextMusicNum == -1)
	{
		/* Will play the same music without any pending play changes,
		   just change the loop attribute */
		bLoop = bInLoop;
		return;
	}

	if (FadeType != EFadeType::FADE_OUT)
	{
		if (FadeType == EFadeType::FADE_IN && TotalFadeInSamples > 0 && RemainingFadeSamples > 0)
		{
			StartFadeTime = GetWorld()->GetTimeSeconds() - ((float)RemainingFadeSamples / TotalFadeInSamples * FadeTime / 2);
		}
		else
		{
			StartFadeTime = GetWorld()->GetTimeSeconds();
		}
		TotalFadeOutSamples = FMath::RoundToInt32(FadeTime / 2.0f * SAMPLE_RATE) * NUM_CHANNEL;
		RemainingFadeSamples = TotalFadeOutSamples;
		TotalFadeInSamples = TotalFadeOutSamples;
	}
	else
	{
		TotalFadeInSamples = FMath::RoundToInt32(FadeTime / 2.0f * SAMPLE_RATE) * NUM_CHANNEL;
	}

	NextMusicNum = InMusicNum;
	FadeType = EFadeType::FADE_OUT;
	bNextLoop = bInLoop;
}

void UPALMusicProcedural::FillBuffer(USoundWaveProcedural* Self, int32 SamplesToGenerate)
{
	if (!bReady)
	{
		// Not initialized
		return;
	}
		
	while (SamplesToGenerate > 0)
	{
		int32 CurrentVolume;
		int32 DeltaSamples = 0; 
		int32 DeltaVolume = 0;

		// fading in or fading out
		switch (FadeType)
		{
		case FADE_IN:
			if (RemainingFadeSamples <= 0)
			{
				FadeType = EFadeType::NONE;
				CurrentVolume = MIX_MAX_VOLUME;
			}
			else
			{
				CurrentVolume = (int32)(MIX_MAX_VOLUME * (1.0 - (double)RemainingFadeSamples / TotalFadeInSamples));
				DeltaSamples = (TotalFadeInSamples / MIX_MAX_VOLUME) & ~(NUM_CHANNEL - 1);
				DeltaVolume = 1;
			}
			break;
		case FADE_OUT:
			if (TotalFadeOutSamples == RemainingFadeSamples && TotalFadeOutSamples > 0)
			{
				float Now = GetWorld()->GetTimeSeconds();
				int32 PassedSamples = Now > StartFadeTime ? (int32)((Now - StartFadeTime) * GetSampleRateForCurrentPlatform()) : 0;
				RemainingFadeSamples -= PassedSamples;
			}
			if (MusicNum == -1 || RemainingFadeSamples <= 0)
			{
				// There is no current playing music, or fading time has passed.
				// Start playing the next one or stop playing.
				if (NextMusicNum > 0)
				{
					MusicNum = NextMusicNum;
					NextMusicNum = -1;
					bLoop = bNextLoop;
					FadeType = EFadeType::FADE_IN;
					if (MusicNum > 0)
					{
						StartFadeTime += TotalFadeOutSamples / static_cast<float>(SAMPLE_RATE);
					}
					else
					{
						StartFadeTime = GetWorld()->GetTimeSeconds();
					}
					TotalFadeOutSamples = 0;
					RemainingFadeSamples = TotalFadeInSamples;
					MyRixPlayer->Rewind(MusicNum);
					continue;
				}
				else
				{
					MusicNum = -1;
					FadeType = EFadeType::NONE;
					return;
				}
			}
			else
			{
				CurrentVolume = (int32)(MIX_MAX_VOLUME * ((double)RemainingFadeSamples / TotalFadeOutSamples));
				DeltaSamples = (TotalFadeOutSamples / MIX_MAX_VOLUME) & ~(NUM_CHANNEL - 1);
				DeltaVolume = -1;
			}
			break;
		default:
			if (MusicNum <= 0)
			{
				// No current playing music
				return;
			}
			else
			{
				CurrentVolume = MIX_MAX_VOLUME;
			}
		}

		// Fill the buffer with sound data
		int32 BufferMaxLength = SAMPLE_RATE / MyRixPlayer->GetRefresh() * NUM_CHANNEL;
		bool bContinue = true;
		while (SamplesToGenerate > 0 && bContinue)
		{
			if (Position == nullptr || Position - Buffer >= BufferMaxLength)
			{
				Position = Buffer;
				if (!MyRixPlayer->Update())
				{
					if (!bLoop)
					{
						// Not loop, simply terminate the music
						MusicNum = -1;
						if (FadeType != EFadeType::FADE_OUT && NextMusicNum == -1)
						{
							FadeType = EFadeType::NONE;
						}
						return;
					}
					MyRixPlayer->RewindReInit(MusicNum, false);
					if (!MyRixPlayer->Update())
					{
						// Something must be wrong
						MusicNum = -1;
						FadeType = EFadeType::NONE;
						return;
					}
				}
				int32 SampleCount = SAMPLE_RATE / MyRixPlayer->GetRefresh();
				MyOpl->Update(Buffer, SampleCount);
			}

			int32 l = BufferMaxLength - (Position - Buffer);
			l = (l > SamplesToGenerate) ? SamplesToGenerate : l;

			// Put audio data into buffer and adjust CurrentVolume
			if (FadeType != EFadeType::NONE)
			{
				int16* StartPosition = Position;
				for (int32 i = 0; i < l && RemainingFadeSamples > 0; CurrentVolume += DeltaVolume)
				{
					int32 j = 0;
					for (j = 0; i < l && j < DeltaSamples; i++, j++)
					{
						*Position *= CurrentVolume / MIX_MAX_VOLUME;
						Position++;
					}
					RemainingFadeSamples -= j;
				}
				bContinue = (RemainingFadeSamples > 0);
				int32 Samples = Position - StartPosition;
				QueueAudio(reinterpret_cast<uint8*>(StartPosition), Samples * sizeof(int16));
				SamplesToGenerate -= Samples;
			}
			else
			{
				QueueAudio(reinterpret_cast<uint8*>(Position), l * sizeof(int16));
				Position += l;
				SamplesToGenerate -= l;
			}
		}
	}
}

void UPALMusicProcedural::FinishDestroy()
{
	delete MyRixPlayer;
	delete MyOpl;
	Super::FinishDestroy();
}