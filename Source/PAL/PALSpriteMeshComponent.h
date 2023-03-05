// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "PALSprite.h"
#include "PALSpriteMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALSpriteMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UPALSpriteMeshComponent();
	
private:
	UPROPERTY(VisibleAnywhere)
	UPALSprite* Sprite;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DynamicMaterial;

	bool bChangedSprite;

	SIZE_T CurrentFrameNum;

	FVector3d LocationOffset;

	int16 Layer;

	bool bChangedLayer;

public:
	void SetLocationOffset(const FVector3d& InLocationOffSet);

	void SetSprite(UPALSprite* Sprite);

	void SetFrame(SIZE_T FrameNum);

	void SetLayer(int16 InLayer);

};
