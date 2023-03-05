// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALGameData.h"
#include "PALGameModeBase.h"
#include "PALPlayerController.h"
#include "PALGameStateData.h"
#include "PALPlayerStateData.h"
#include "PALScriptRunnerBase.h"
#include "PALDialog.h"
#include "PALTriggerScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALTriggerScriptActor : public AActor
{
	GENERATED_BODY()

public:
	APALTriggerScriptActor();
	
private:
	UPROPERTY()
	UPALGameData* GameData;

	// could be a scene game mode or a battle game mode
	UPROPERTY()
	APALGameModeBase* GameMode;

	UPROPERTY()
	UPALGameStateData* GameStateData;

	UPROPERTY()
	APALPlayerController* PlayerController;

	UPROPERTY()
	UPALPlayerStateData* PlayerStateData;

	uint16* OutScriptEntryPtr;

	bool bRewriteScriptEntry;

	uint16 ScriptEntry;

	uint16 NextScriptEntry;

	uint16 EventObjectId;

	FEventObject* EventObject;

	bool bSuccess;

	bool bWaiting;

	UPROPERTY()
	APALScriptRunnerBase* CurrentRunner;

	TFunction<void()> WaitCallback;

	bool bWaitingExclusive;

	UPROPERTY()
	UPALDialog* Dialog;

public:
	// returns true if it has an immediate return
	bool RunTriggerScript(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bRewriteScriptEntry, bool& bOutSuccess);

	bool AllowNewTriggers();

private:
	bool RunTriggerScript();

	void WaitForRunner(APALScriptRunnerBase* ScriptRunner, bool bInWaitingExclusive, TFunction<void()> Callback);

public:
	virtual void PreInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;
};
