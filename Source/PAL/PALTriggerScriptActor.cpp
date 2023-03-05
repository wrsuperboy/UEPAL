// Copyright (C) 2022 Meizhouxuanhan.


#include "PALTriggerScriptActor.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALScriptManager.h"
#include "PALSceneGameMode.h"
#include "PALTimedWaitScriptRunner.h"
#include "PALDialogWaitScriptRunner.h"
#include "PALWaitRunTriggerScriptRunner.h"
#include "PALConfirmScriptRunner.h"
#include "PALCommon.h"

APALTriggerScriptActor::APALTriggerScriptActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool APALTriggerScriptActor::RunTriggerScript(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bInRewriteScriptEntry, bool& bOutSuccess)
{
	OutScriptEntryPtr = &InOutScriptEntry;
	bRewriteScriptEntry = bInRewriteScriptEntry;
	EventObjectId = InEventObjectId;
	bSuccess = true;
	EventObject = nullptr;
	if (EventObjectId != 0)
	{
		EventObject = &(GameStateData->EventObjects[EventObjectId - 1]);
	}
	ScriptEntry = InOutScriptEntry;
	NextScriptEntry = InOutScriptEntry;
	if (RunTriggerScript())
	{
		bOutSuccess = bSuccess;
		if (bRewriteScriptEntry)
		{
			*OutScriptEntryPtr = NextScriptEntry;
		}
		SetActorTickEnabled(false);
		Destroy();
		return true;
	}
	
	bOutSuccess = true;
	// wait
	return false;
}

bool APALTriggerScriptActor::AllowNewTriggers()
{
	return !bWaitingExclusive;
}

bool APALTriggerScriptActor::RunTriggerScript()
{
#define ClearDialog() \
	if (Dialog && Dialog->IsValidLowLevel()) \
	{ \
		APALDialogWaitScriptRunner* Runner = GetWorld()->SpawnActor<APALDialogWaitScriptRunner>(); \
		Runner->Init(Dialog); \
		TWeakObjectPtr<APALTriggerScriptActor> TriggerScriptActorWP(this); \
		WaitForRunner(Runner, true, [TriggerScriptActorWP]() { \
			if (TriggerScriptActorWP.IsValid()) \
			{ \
				TriggerScriptActorWP->Dialog = nullptr; \
			} \
			}); \
		break; \
	}
	

	bool bEnded = false;
	while (ScriptEntry != 0 && !bEnded)
	{
		FScriptEntry* Script = &(GameData->ScriptEntries[ScriptEntry]);
		UE_LOG(LogScript, Display, TEXT("[SCRIPT] %.4x: %.4x %.4x %.4x %.4x"), ScriptEntry,
			Script->Operation, Script->Operand[0], Script->Operand[1], Script->Operand[2]);

		switch (Script->Operation)
		{
		case 0x0000:
			// Stop running
			bEnded = true;
			break;

		case 0x0001:
			// Stop running and replace the entry with the next line
			bEnded = true;
			NextScriptEntry = ScriptEntry + 1;
			break;

		case 0x0002:
			// Stop running and replace the entry with the specified one
			if (Script->Operand[1] == 0 || EventObject && ++(EventObject->ScriptIdleFrame) < Script->Operand[1])
			{
				bEnded = true;
				NextScriptEntry = Script->Operand[0];
			}
			else
			{
				// failed
				if (EventObject)
				{
					EventObject->ScriptIdleFrame = 0;
				}
				ScriptEntry++;
			}
			break;

		case 0x0003:
			// unconditional jump
			if (Script->Operand[1] == 0 || EventObject && ++(EventObject->ScriptIdleFrame) < Script->Operand[1])
			{
				ScriptEntry = Script->Operand[0];
			}
			else
			{
				// failed
				if (EventObject)
				{
					EventObject->ScriptIdleFrame = 0;
				}
				ScriptEntry++;
			}
			break;

		case 0x0004:
			// Call script
		{
			GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(Script->Operand[0], ((Script->Operand[1] == 0) ? EventObjectId : Script->Operand[1]), false, bSuccess);
			APALWaitRunTriggerScriptRunner* Runner = GetWorld()->SpawnActor<APALWaitRunTriggerScriptRunner>();
			Runner->Init(this);
			WaitForRunner(Runner, !static_cast<bool>(Script->Operand[1]), []{});
			ScriptEntry++;
		}
		break;

		case 0x0005:
			// Redraw screen
			// TODO
			ClearDialog();
			/*
			if (PAL_DialogIsPlayingRNG())
			{
				VIDEO_RestoreScreen(gpScreen);
			}
			else if (gpGlobals->fInBattle)
			{
				PAL_BattleMakeScene();
				VIDEO_CopyEntireSurface(g_Battle.lpSceneBuf, gpScreen);
				VIDEO_UpdateScreen(NULL);
			}
			else
			{
				if (Script->Operand[2])
				{
					PAL_UpdatePartyGestures(FALSE);
				}

				PAL_MakeScene();

				VIDEO_UpdateScreen(NULL);
				UTIL_Delay((Script->Operand[1] == 0) ? 60 : (Script->Operand[1] * 60));
			}*/

			ScriptEntry++;
			break;

		case 0x0006:
			// Jump to the specified address by the specified rate
			if (static_cast<uint16>(FMath::RandRange(1, 100)) >= Script->Operand[0])
			{
				ScriptEntry = Script->Operand[1];
				continue;
			}
			else
			{
				ScriptEntry++;
			}
			break;

		case 0x0007:
			// Start battle
		{
			EPALBattleResult BattleResult = Cast<APALSceneGameMode>(GameMode)->StartBattle(static_cast<SIZE_T>(Script->Operand[0]), static_cast<bool>(!Script->Operand[2]));

			if (BattleResult == EPALBattleResult::PreBattle)
			{
				// If battle has not started yet
				continue;
			}
			if (BattleResult == EPALBattleResult::Lost && Script->Operand[1] != 0)
			{
				ScriptEntry = Script->Operand[1];
			}
			else if (BattleResult == EPALBattleResult::Fleed && Script->Operand[2] != 0)
			{
				ScriptEntry = Script->Operand[2];
			}
			else
			{
				ScriptEntry++;
			}
			PlayerStateData->bAutoBattle = false;
		}
		break;

		case 0x0008:
			// Replace the entry with the next instruction
			ScriptEntry++;
			NextScriptEntry = ScriptEntry;
			break;

		case 0x0009:
			// wait for the specified number of frames
		{
			ClearDialog();
			if (!Script->Operand[2])
			{
				PlayerController->PartyAtEase(true);
			}
			TWeakObjectPtr<APALTriggerScriptActor> ThisWP(this);
			APALTimedWaitScriptRunner* Runner = GetWorld()->SpawnActor<APALTimedWaitScriptRunner>();
			WaitForRunner(Runner,
				!static_cast<bool>(Script->Operand[1]),
				[ThisWP, Script] {
					if (!Script->Operand[2])
					{
						// ThisWP->PlayerController->PartyAtEase(false);
					}
				});
			Runner->Init((Script->Operand[0] ? Script->Operand[0] : 1) * FRAME_TIME);
			ScriptEntry++;
		}
		break;

		case 0x000A:
			// Goto the specified address if player selected no
		{
			APALConfirmScriptRunner* Runner = GetWorld()->SpawnActor<APALConfirmScriptRunner>();
			TWeakObjectPtr<APALTriggerScriptActor> ThisWP(this);
			uint16 GotoScriptEntry = Script->Operand[0];
			Runner->Init([ThisWP, GotoScriptEntry](bool bYes) {
				if (ThisWP.IsValid())
				{
					if (ThisWP->Dialog && ThisWP->Dialog->IsValidLowLevel())
					{
						ThisWP->Dialog->RemoveFromParent();
						ThisWP->Dialog = nullptr;
					}
					if (!bYes)
					{
						ThisWP->ScriptEntry = GotoScriptEntry;
					}
					else
					{
						ThisWP->ScriptEntry++;
					}
				}
				});
			WaitForRunner(Runner, false, nullptr);
		}
		break;

		case 0x003B:
			// Show dialog in the middle part of the screen
			ClearDialog();
			Dialog = CreateWidget<UPALDialog>(PlayerController, UPALDialog::StaticClass());
			Dialog->Start(EPALDialogLocation::DialogCenter, Script->Operand[0]);
			// fPlayingRNG = Script->Operand[2] ? TRUE : FALSE
			ScriptEntry++;

			break;

		case 0x003C:
			// Show dialog in the upper part of the screen
			ClearDialog();
			Dialog = CreateWidget<UPALDialog>(PlayerController, UPALDialog::StaticClass());
			Dialog->Start(EPALDialogLocation::DialogUpper, Script->Operand[1], Script->Operand[0]);
			// fPlayingRNG = Script->Operand[2] ? TRUE : FALSE
			ScriptEntry++;
			break;

		case 0x003D:
			// Show dialog in the lower part of the screen
			ClearDialog();
			Dialog = CreateWidget<UPALDialog>(PlayerController, UPALDialog::StaticClass());
			Dialog->Start(EPALDialogLocation::DialogLower, Script->Operand[1], Script->Operand[0]);
			// fPlayingRNG = Script->Operand[2] ? TRUE : FALSE
			ScriptEntry++;
			break;

		case 0x003E:
			// Show text in a window at the center of the screen
			ClearDialog();
			Dialog = CreateWidget<UPALDialog>(PlayerController, UPALDialog::StaticClass());
			Dialog->Start(EPALDialogLocation::DialogCenterWindow, Script->Operand[0]);
			ScriptEntry++;
			break;

		case 0x008E:
			// TODO
			// Restore the screen
			ClearDialog();
			/*
			VIDEO_RestoreScreen(gpScreen);
			VIDEO_UpdateScreen(NULL);
			*/
			ScriptEntry++;
			break;

		case 0xFFFF:
			// TODO
			// Print dialog text
			//
			/*
			if (gConfig.pszMsgFile)
			{
				int msgSpan = MESSAGE_GetSpan(&ScriptEntry);
				int idx = 0, iMsg;
				while ((iMsg = PAL_GetMsgNum(Script->Operand[0], msgSpan, idx++)) >= 0)
				{
					if (iMsg == 0)
					{
						//
						// Restore the screen
						//
						PAL_ClearDialog(TRUE);
						VIDEO_RestoreScreen(gpScreen);
						VIDEO_UpdateScreen(NULL);
					}
					else
						PAL_ShowDialogText(PAL_GetMsg(iMsg));
				}
			}
			else
			{
			
				PAL_ShowDialogText(PAL_GetMsg(Script->Operand[0]));*/
			if (!Dialog || !Dialog->IsValidLowLevel())
			{
				// Set some default parameters, as there are some parts of script
				// which doesn't have a "start dialog" instruction before showing the dialog.
				Dialog = CreateWidget<UPALDialog>(PlayerController, UPALDialog::StaticClass());
				Dialog->Start(EPALDialogLocation::DialogUpper, 79);
			}
			Dialog->ShowText(GetGameInstance()->GetSubsystem<UPALCommon>()->GetMessage(Script->Operand[0]));
			ScriptEntry++;
			//}
			break;

		default:
			ClearDialog();
			APALScriptRunnerBase* ScriptRunner = GetWorld()->GetSubsystem<UPALScriptManager>()->InterpretInstruction(ScriptEntry, EventObjectId, true, bSuccess);
			if (ScriptRunner)
			{
				WaitForRunner(ScriptRunner, true, nullptr);
			}
			break;
		}

		if (bWaiting)
		{
			return false;
		}
	}

	// The same as ClearDialog();
	if (Dialog && Dialog->IsValidLowLevel())
	{
		APALDialogWaitScriptRunner* Runner = GetWorld()->SpawnActor<APALDialogWaitScriptRunner>();
		Runner->Init(Dialog);
		TWeakObjectPtr<APALTriggerScriptActor> TriggerScriptActorWP(this);
		WaitForRunner(Runner, false, [TriggerScriptActorWP]() {
			if (TriggerScriptActorWP.IsValid())
			{
				TriggerScriptActorWP->Dialog = nullptr;
			}
			});
		if (bWaiting)
		{
			return false;
		}
	}

	PlayerStateData->CurrentEquipPart = -1;

	// don't have to wait
	return true;
}

void APALTriggerScriptActor::WaitForRunner(APALScriptRunnerBase* ScriptRunner, bool bInWaitingExclusive, TFunction<void()> Callback)
{
	PlayerController->ControllByGame();
	WaitCallback = Callback;
	CurrentRunner = ScriptRunner;
	bWaitingExclusive = bInWaitingExclusive;
	bWaiting = true;
}

void APALTriggerScriptActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	// could be a scene game mode or a battle game mode
	GameMode = GetWorld()->GetAuthGameMode<APALGameModeBase>();
	GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	PlayerController = Cast<APALPlayerController>(GetWorld()->GetGameInstance<UPALGameInstance>()
		->GetMainPlayer()->GetPlayerController(nullptr));
	PlayerStateData = PlayerController->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	bWaiting = false;
	bWaitingExclusive = false;
}

void APALTriggerScriptActor::Tick(float DeltaTime)
{
	if (bWaiting)
	{
		if (CurrentRunner && !CurrentRunner->IsCompleted())
		{
			return;
		}
		else
		{
			if (CurrentRunner)
			{
				bSuccess = CurrentRunner->IsSuccessful();
			}
			if (WaitCallback)
			{
				WaitCallback();
				WaitCallback = nullptr;
			}
			PlayerController->ReleaseControllFromGame();
			bWaitingExclusive = false;
			bWaiting = false;
		}
	}

	if (RunTriggerScript())
	{
		if (bRewriteScriptEntry)
		{
			*OutScriptEntryPtr = NextScriptEntry;
		}
		SetActorTickEnabled(false);
		Destroy();
	}
}