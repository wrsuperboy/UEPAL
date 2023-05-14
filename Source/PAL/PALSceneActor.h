// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PALSprite.h"
#include "PAL.h"
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
	UStaticMesh* TileMesh;

	UPROPERTY()
	UStaticMesh* DecoratorMesh;

	UPROPERTY()
	UMaterial* TileMaterial;

	UPROPERTY()
	UMaterial* DecoratorMaterial;

	UPROPERTY()
	TArray<UStaticMeshComponent*> TileComponents;

	UPROPERTY()
	TArray<UStaticMeshComponent*> DecoratorMeshComponents;

public:
	void SetTile(SIZE_T X, SIZE_T Y, SIZE_T H, const FPALPosition3d& Position, UTexture2D* Texture);

	void AddDecorator(const FPALPosition3d& Position, UTexture2D* Texture);

	void ClearDecorators();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PreInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
