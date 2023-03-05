// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PAL.h"
#include "PALMKF.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALMKF : public UObject
{
	GENERATED_BODY()

public:
	static UPALMKF* Create(IFileHandle* FileHandle, const EGameDistribution& GameDistribution);

private:
	IFileHandle* FileHandle;

	EGameDistribution GameDistribution;

	SIZE_T(*Decompress)(uint8* SourceBuffer, uint8* OutBuffer, const SIZE_T BufferSize);

public:
	SIZE_T GetChunkCount();

	SIZE_T GetChunkSize(const SIZE_T ChunkNum);

	SIZE_T ReadChunk(uint8* OutBuffer, const SIZE_T BufferSize, const SIZE_T ChunkNum);

	SIZE_T GetDecompressedSize(const SIZE_T ChunkNum);

	SIZE_T DecompressChunk(uint8* OutBuffer, const SIZE_T BufferSize, const SIZE_T ChunkNum);

public:
	virtual void FinishDestroy() override;
};
