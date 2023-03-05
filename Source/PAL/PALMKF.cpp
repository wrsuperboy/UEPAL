// Copyright (C) 2022 Meizhouxuanhan.


#include "PALMKF.h"
#include "YJ.h"

UPALMKF* UPALMKF::Create(IFileHandle* FileHandle, const EGameDistribution& GameDistribution)
{
	UPALMKF* MKF = NewObject<UPALMKF>();
	MKF->FileHandle = FileHandle;
	MKF->GameDistribution = GameDistribution;
	if (GameDistribution == EGameDistribution::Win95)
	{
		MKF->Decompress = YJ2_Decompress;
	}
	else
	{
		MKF->Decompress = YJ1_Decompress;
	}
	return MKF;
}

SIZE_T UPALMKF::GetChunkCount()
{
	int32 NumChunk;
	FileHandle->Seek(0);

	if (FileHandle->Read((uint8*)&NumChunk, sizeof(int32)))
	{
		return (NumChunk - 4) >> 2;
	}
	return 0;
}

SIZE_T UPALMKF::GetChunkSize(const SIZE_T ChunkNum)
{

	check(ChunkNum < GetChunkCount());

	//
	// Get the offset of the specified chunk and the next chunk.
	//
	FileHandle->Seek(4 * ChunkNum);
	uint32 Offset;
	uint32 NextOffset;
	FileHandle->Read((uint8*)&Offset, sizeof(uint32));
	FileHandle->Read((uint8*)&NextOffset, sizeof(uint32));

	return NextOffset - Offset;
}

SIZE_T UPALMKF::ReadChunk(uint8* OutBuffer, const SIZE_T BufferSize, const SIZE_T ChunkNum)
{
 	check(BufferSize > 0);
	check(ChunkNum < GetChunkCount());

	//
	// Get the offset of the specified chunk and the next chunk.
	//
	FileHandle->Seek(4 * ChunkNum);
	uint32 Offset;
	uint32 NextOffset;
	FileHandle->Read((uint8*)&Offset, sizeof(uint32));
	FileHandle->Read((uint8*)&NextOffset, sizeof(uint32));

	uint32 ChunkLength = NextOffset - Offset;

	check(ChunkLength <= BufferSize);

	if (ChunkLength != 0)
	{
		FileHandle->Seek(Offset);
		FileHandle->Read(OutBuffer, ChunkLength);
		return ChunkLength;
	}

	return 0;
}

SIZE_T UPALMKF::GetDecompressedSize(const SIZE_T ChunkNum)
{
	check(ChunkNum < GetChunkCount());

	// Get the offset of the chunk.
	FileHandle->Seek(4 * ChunkNum);
	uint32 Offset;
	FileHandle->Read((uint8*)&Offset, 4);

	// Read the header.
	FileHandle->Seek(Offset);
	if (GameDistribution == EGameDistribution::Win95)
	{
		uint32 Buffer;
		FileHandle->Read((uint8*)&Buffer, sizeof(uint32));
		return Buffer;
	}
	else
	{
		uint32 Buffer[2];
		FileHandle->Read((uint8*)Buffer, 2 * sizeof(uint32));
		ensure(Buffer[0] == 0x315f4a59);
		return (Buffer[0] != 0x315f4a59) ? 0 : Buffer[1];
	}
}

SIZE_T UPALMKF::DecompressChunk(uint8* OutBuffer, const SIZE_T BufferSize, const SIZE_T ChunkNum)
{
	SIZE_T ChunkLength = GetChunkSize(ChunkNum);
	ensure(ChunkLength > 0);

	uint8* Buffer = (uint8*)FMemory::Malloc(ChunkLength);
	ReadChunk(Buffer, ChunkLength, ChunkNum);

	ChunkLength = Decompress(Buffer, OutBuffer, BufferSize);
	FMemory::Free(Buffer);

	return ChunkLength;
}

void UPALMKF::FinishDestroy()
{
	delete FileHandle;
	Super::FinishDestroy();
}
