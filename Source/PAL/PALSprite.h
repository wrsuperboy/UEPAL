// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PALSprite.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALSprite : public UObject
{
	GENERATED_BODY()

public:
	static UPALSprite* Create(uint8* Data, UObject* Outer);

private:
	uint8* Ptr;

	UPROPERTY(VisibleInstanceOnly)
	TArray<UTexture2D*> CachedFrames;

public:
	SIZE_T GetFrameCount() const;

	UTexture2D* GetFrame(const SIZE_T FrameNum);

	void Preload();

public:
	virtual void FinishDestroy() override;

};
