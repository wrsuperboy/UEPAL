// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PAL.h"
#include "PALSprite.h"
#include "PALSceneActor.generated.h"

UCLASS()
class PAL_API APALSceneActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APALSceneActor();

private:
	UPROPERTY()
	UStaticMesh* StaticMesh;

	UPROPERTY()
	UMaterial* Material;

	UPROPERTY()
	TArray<UStaticMeshComponent*> TileComponents;

	UPROPERTY()
	TArray<UStaticMeshComponent*> DecolaratorComponents;

public:
	void SetTile(SIZE_T X, SIZE_T Y, SIZE_T H, const FPALPosition3d& Position, UTexture2D* Texture);

	void AddDecolarator(const FPALPosition3d& Position, UTexture2D* Texture);

	void ClearDecolarators();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PreInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
