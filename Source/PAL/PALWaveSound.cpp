// Copyright (C) 2022 Meizhouxuanhan.


#include "PALWaveSound.h"

/**
* Data used to parse the wave file formats
*/

struct FRiffWaveHeader
{
	uint32 ChunkId;
	uint32 ChunkDataSize;
	uint32 TypeId;
};

struct FChunkHeader
{
	uint32 ChunkId;
	uint32 ChunkDataSize;
};

struct FFormatChunk
{
	// format ID
	uint16 FormatTag;

	// number of channels, 1 = mono, 2 = stereo
	uint16 NumChannels;

	// sampling rate
	uint32 SamplesPerSec;

	// average bite rate
	uint32 AverageBytesPerSec;

	// audio block size. ((mono) 1 = 8bit, 2 = 16bit), ((stereo) 2 = 8bit, 4 = 16bit)
	uint16 BlockAlign;

	// Quantization rate, 8, 12, 16
	uint16 BitsPerSample;
};

/**
* The header of an wave formatted file
*/
struct FWaveFormatInfo
{
	FRiffWaveHeader RiffWaveHeader;
	FChunkHeader FmtChunkHeader;
	FFormatChunk FmtChunk;
	FChunkHeader DataChunkHeader;
	uint32 DataStartOffset;

	FWaveFormatInfo()
	{
		FMemory::Memzero(this, sizeof(FWaveFormatInfo));
	}
};

// Chunk IDs and Type identifiers
#define CHUNK_ID_RIFF			(0x46464952)	// "RIFF"
#define CHUNK_TYPE_WAVE			(0x45564157)	// "WAVE"
#define CHUNK_ID_FMT			(0x20746D66)	// "fmt "
#define CHUNK_ID_DATA			(0x61746164)	// "data"

bool PALParseWaveFormatHeader(const uint8* InSrcBufferData, uint32 InSrcBufferDataSize, FWaveFormatInfo& OutHeader)
{
	if (!InSrcBufferData || InSrcBufferDataSize < sizeof(OutHeader.RiffWaveHeader))
	{
		UE_LOG(LogAudio, Error, TEXT("Failed to parse wave format header. Buffer data was null or empty."));
		return false;
	}

	OutHeader.DataStartOffset = 0;

	uint32 CurrByte = 0;
	uint32 CurrentChunkDataSize = 0;

	// first get the RIFF chunk to make sure we have the correct file type
	FMemory::Memcpy(&OutHeader.RiffWaveHeader, &InSrcBufferData[CurrByte], sizeof(OutHeader.RiffWaveHeader));

	CurrByte += sizeof(OutHeader.RiffWaveHeader);

	// Check for "RIFF" in the ChunkID
	if (OutHeader.RiffWaveHeader.ChunkId != CHUNK_ID_RIFF)
	{
		UE_LOG(LogAudio, Error, TEXT("Beginning of wave file was not \"RIFF\""));
		return false;
	}

	// Check to see if we've found the "WAVE" chunk (apparently there could be more than one "RIFF" chunk?)
	if (OutHeader.RiffWaveHeader.TypeId != CHUNK_TYPE_WAVE)
	{
		UE_LOG(LogAudio, Error, TEXT("First wave RIFF chunk is not a \"WAVE\" type"));
		return false;
	}

	// Now read the other chunk headers to get file information
	while (CurrByte < InSrcBufferDataSize)
	{
		// Read the next chunk header
		FChunkHeader ChunkHeader;
		FMemory::Memcpy(&ChunkHeader, &InSrcBufferData[CurrByte], sizeof(FChunkHeader));

		// Offset the byte index byt he sizeof chunk header
		CurrByte += sizeof(FChunkHeader);

		// Now read which type of chunk this is and get the header info
		switch (ChunkHeader.ChunkId)
		{
		case CHUNK_ID_FMT:
		{
			OutHeader.FmtChunkHeader = ChunkHeader;

			FMemory::Memcpy(&OutHeader.FmtChunk, &InSrcBufferData[CurrByte], sizeof(FFormatChunk));

			CurrByte += sizeof(FFormatChunk);

			// The rest of the data in this chunk is unknown so skip it
			CurrentChunkDataSize = (CurrentChunkDataSize < sizeof(FFormatChunk)) ? 0 : CurrentChunkDataSize - sizeof(FFormatChunk);
		}
		break;

		case CHUNK_ID_DATA:
		{
			OutHeader.DataChunkHeader = ChunkHeader;

			// This is where bit-stream data starts in the AT9 file
			OutHeader.DataStartOffset = CurrByte;

			CurrentChunkDataSize = OutHeader.DataChunkHeader.ChunkDataSize;
		}
		break;

		default:
		{
			UE_LOG(LogAudio, Warning, TEXT("Wave file contained unknown RIFF chunk type (%d)"), ChunkHeader.ChunkId);
		}
		break;

		}

		// Offset the byte read index by the current chunk's data size
		CurrByte += CurrentChunkDataSize;
	}

	return true;
}

UPALWaveSound::UPALWaveSound(const FObjectInitializer& ObjectInitializer) : USoundWaveProcedural(ObjectInitializer)
{
	SoundGroup = ESoundGroup::SOUNDGROUP_Effects;
	bLooping = false;
}

void UPALWaveSound::Init(const uint8* InSrcBufferData, uint32 InSrcBufferDataSize)
{
	FWaveFormatInfo WaveFormatInfo;
	bool bSuccess = PALParseWaveFormatHeader(InSrcBufferData, InSrcBufferDataSize, WaveFormatInfo);
	if (!bSuccess)
	{
		return;
	}

	NumChannels = WaveFormatInfo.FmtChunk.NumChannels;
	verify(WaveFormatInfo.FmtChunk.BitsPerSample == 16);
	SetSampleRate(WaveFormatInfo.FmtChunk.SamplesPerSec);

	QueueAudio(InSrcBufferData + WaveFormatInfo.DataStartOffset, WaveFormatInfo.DataChunkHeader.ChunkDataSize);
}