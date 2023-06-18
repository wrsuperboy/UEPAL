// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "PALSprite.h"
#include "ProceduralMeshComponent.h"
#include "PALSpriteMeshComponent.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALSpriteMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:
	UPALSpriteMeshComponent(const FObjectInitializer& ObjectInitializer);
	
private:
	UPROPERTY()
	UPALSprite* Sprite;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DynamicMaterial;

	SIZE_T CurrentFrameNum;

	FVector3d LocationOffset;

	UPROPERTY(VisibleAnywhere)
	int16 Layer;

	bool bChangedLayer;

public:
	void SetLocationOffset(const FVector3d& InLocationOffSet);

	void SetSprite(UPALSprite* Sprite);

	void SetFrame(SIZE_T FrameNum);

	void SetLayer(int16 InLayer);

private:
	void UpdateFrame();
};
