// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSprite.h"
#include "PALCommon.h"
#include "GenericPlatform/GenericPlatformMath.h"

UPALSprite* UPALSprite::Create(uint8* Data, UObject* Outer)
{
	UPALSprite* Sprite = NewObject<UPALSprite>(Outer);
	Sprite->Ptr = Data;
	return Sprite;
}

SIZE_T UPALSprite::GetFrameCount() const
{
	return (Ptr[0] | (Ptr[1] << 8)) - 1;
}

UTexture2D* UPALSprite::GetFrame(const SIZE_T FrameNum)
{
	// Hack for broken sprites like the Bloody-Mouth Bug
	SIZE_T FrameCount = (Ptr[0] | (Ptr[1] << 8));

	if (FrameNum >= FrameCount)
	{
		return NewObject<UTexture2D>(GetOuter());
	}

	if (!CachedFrames.IsValidIndex(FrameNum))
	{
		CachedFrames.SetNum(FrameCount);
	}

	if (CachedFrames[FrameNum] == nullptr)
	{
		int32 Offset = ((Ptr[FrameNum << 1] | (Ptr[(FrameNum << 1) + 1] << 8)) << 1);
		if (Offset == 0x18444)
		{
			Offset = (int16)Offset;
		}
		const uint8* FramePtr = &Ptr[Offset];
		CachedFrames[FrameNum] = Cast<UPALCommon>(GetOuter())->CreateTextureFromRLE(FramePtr);
	}

	return CachedFrames[FrameNum];
}

void UPALSprite::Preload()
{
	SIZE_T FrameCount = GetFrameCount();
	for (SIZE_T FrameNum = 0; FrameNum < FrameCount; FrameNum++)
	{
		GetFrame(FrameNum);
	}
}

void UPALSprite::FinishDestroy()
{
	FMemory::Free(Ptr);
	Super::FinishDestroy();
}
