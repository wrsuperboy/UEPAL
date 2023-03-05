// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PALCommon.h"
#include "PALSceneActor.h"
#include "PALEventObjectActor.h"
#include "PALMapManager.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALMapManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPALMapManager();

private:
	SIZE_T CurrentMapNum;

	uint32 Tiles[128][64][2];

	UPROPERTY(VisibleAnywhere)
	APALSceneActor* SceneActor;

	UPROPERTY(VisibleAnywhere)
	TArray<APALEventObjectActor*> EventObjectActors;

public:
	void LoadMap(const SIZE_T SceneNum, UWorld* CurrentWorld);

	// Check if the map tile at the specified position is blocking
	bool CheckObstacle(const FPALPosition3d& Position, const bool bCheckEventObject, const uint16 SelfEventObjectId = 0) const;

public:
	bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
};
