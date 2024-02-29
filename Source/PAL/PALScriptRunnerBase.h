// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PALGameData.h"
#include "PALGameModeBase.h"
#include "PALGameStateData.h"
#include "PALScenePlayerController.h"
#include "PALPlayerStateData.h"
#include "PALScriptRunnerBase.generated.h"

UCLASS()
class PAL_API APALScriptRunnerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APALScriptRunnerBase();

protected:
	UPROPERTY()
	UPALGameData* GameData;

	// could be a scene game mode or a battle game mode
	UPROPERTY()
	APALGameModeBase* GameMode;

	UPROPERTY()
	UPALGameStateData* GameStateData;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	APALScenePlayerController* ScenePlayerController;

	UPROPERTY()
	UPALPlayerStateData* PlayerStateData;

private:
	bool bInitialized;

	bool bCompleted;

	bool bSuccessful;

public:
	bool IsCompleted() const;

	bool IsSuccessful() const;

protected:
	void MarkInitialized();

	void MarkCompeted(bool bInSuccessful);

	virtual void TickRun(float DeltaTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
