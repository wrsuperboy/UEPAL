// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PALGameStateData.h"
#include "PALScriptRunnerBase.h"
#include "PALScriptManager.generated.h"

static bool bDefaultSuccess;

/**
 * 
 */
UCLASS()
class PAL_API UPALScriptManager : public UWorldSubsystem
{
	GENERATED_BODY()

	friend class APALTriggerScriptActor;

public:
	static const uint16 LastTriggeredEventObjectId = 0xFFFF;
	
public:
	// Only trigger script can wait (Subroutines are neglected).
	bool RunTriggerScript(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bRewriteScriptEntry, bool& bOutSuccess = bDefaultSuccess);

	void RunAutoScript(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bRewriteScriptEntry, bool& bOutSuccess = bDefaultSuccess);

private:
	APALScriptRunnerBase* InterpretInstruction(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bRewriteScriptEntry, bool& bOutSuccess);
};
