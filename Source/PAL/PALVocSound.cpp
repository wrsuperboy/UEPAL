// Copyright (C) 2022 Meizhouxuanhan.


#include "PALVocSound.h"

struct FVocHeader
{
	ANSICHAR Signature[0x14];	/* "Creative Voice File\x1A" */
	uint16 DataOffset;		/* little endian */
	uint16 Version;
	uint16 VersionChecksum;
};

UPALVocSound::UPALVocSound(const FObjectInitializer& ObjectInitializer) : USoundWaveProcedural(ObjectInitializer)
{
	SoundGroup = ESoundGroup::SOUNDGROUP_Effects;
	bLooping = false;
}

void UPALVocSound::Init(const uint8* InSrcBufferData, uint32 InSrcBufferDataSize)
{
	verify(InSrcBufferDataSize >= sizeof(FVocHeader));
	const FVocHeader* VocHeader = reinterpret_cast<const FVocHeader*>(InSrcBufferData);
	verify(FMemory::Memcmp(VocHeader->Signature, "Creative Voice File\x1A", 0x14) == 0);
	verify(VocHeader->DataOffset < InSrcBufferDataSize);

	const uint8* Data = InSrcBufferData + VocHeader->DataOffset;
	uint32 Length = InSrcBufferDataSize - VocHeader->DataOffset;

	while (Length && *Data)
	{
		uint32 CurrentLength;
		if (Length >= 4)
		{
			CurrentLength = Data[1] | (Data[2] << 8) | (Data[3] << 16);
			if (Length >= CurrentLength + 4)
			{
				Length -= CurrentLength + 4;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
		if (*Data == 0x01)
		{
			if (Data[5] != 0)
			{
				/* Only 8-bit is supported */
				return;
			}

			NumChannels = 1;
			SIZE_T ConvertedLength = (CurrentLength - 2) * sizeof(int16);
			int16* ConvertedBuffer = (int16*)FMemory::Malloc(ConvertedLength);
			const uint8* SrcBuffer = Data + 6;
			for (SIZE_T i = 0; i < CurrentLength - 2; i++)
			{
				ConvertedBuffer[i] = static_cast<int16>(*SrcBuffer ^ 0x80) << 8;
				SrcBuffer++;
			}
			SetSampleRate(((1000000 / (256 - Data[4]) + 99) / 100) * 100); /* Round to next 100Hz */
			QueueAudio(reinterpret_cast<uint8*>(ConvertedBuffer), ConvertedLength);
			FMemory::Free(ConvertedBuffer);
			return;
		}
		else
		{
			Data += CurrentLength + 4;
		}
	}
}