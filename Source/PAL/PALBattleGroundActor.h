// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PALBattleGroundActor.generated.h"

UCLASS()
class PAL_API APALBattleGroundActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APALBattleGroundActor();

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

public:
	void SetGroundTexture(UTexture2D* Texture);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
