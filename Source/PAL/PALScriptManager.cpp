// Copyright (C) 2022 Meizhouxuanhan.


#include "PALScriptManager.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALGameData.h"
#include "PALSceneGameMode.h"
#include "PALPlayerState.h"
#include "PALAudioManager.h"
#include "PALTriggerScriptActor.h"
#include "PALEventObjectActor.h"
#include "PALRideEventObjectScriptRunner.h"
#include "PALTimedWaitScriptRunner.h"
#include "PALPartyWalkScriptRunner.h"
#include "PALPanCameraScriptRunner.h"
#include "PALMapManager.h"
#include "EngineUtils.h"
#include "PALTimedFadeScriptRunner.h"

bool UPALScriptManager::RunTriggerScript(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bRewriteScriptEntry, bool& bOutSuccess)
{
	//extern BOOL	   g_fUpdatedInBattle; // HACKHACK
	//g_fUpdatedInBattle = FALSE;

	for (TActorIterator<APALTriggerScriptActor> It(GetWorld(), APALTriggerScriptActor::StaticClass()); It; ++It)
	{
		APALTriggerScriptActor* TriggerScriptActor = *It;
		if (TriggerScriptActor->IsValidLowLevelFast() && !TriggerScriptActor->AreNewTriggersAllowed())
		{
			bOutSuccess = false;
			return true;
		}
	}

	uint16 EventObjectId = InEventObjectId;
	static uint16 LastEventObjectId = 0;
	if (EventObjectId == UPALScriptManager::LastTriggeredEventObjectId)
	{
		EventObjectId = LastEventObjectId;
	}

	LastEventObjectId = EventObjectId;

	APALTriggerScriptActor* TriggerScriptActor = GetWorld()->SpawnActor<APALTriggerScriptActor>();
	return TriggerScriptActor->RunTriggerScript(InOutScriptEntry, InEventObjectId, bRewriteScriptEntry, bOutSuccess);
}



void UPALScriptManager::RunAutoScript(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bRewriteScriptEntry, bool& bOutSuccess)
{
	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	FScriptEntry* Script = &(GameData->ScriptEntries[InOutScriptEntry]);
	FEventObject* EventObject = &(GameStateData->EventObjects[InEventObjectId - 1]);

	UE_LOG(LogScript, Log, TEXT("[AUTOSCRIPT] %04x %.4x: %.4x %.4x %.4x %.4x"), InEventObjectId, InOutScriptEntry,
		Script->Operation, Script->Operand[0], Script->Operand[1], Script->Operand[2]);

	// For autoscript, we should interpret one instruction per frame (except jumping) and save the address of next instruction.
	uint16 ScriptEntry = InOutScriptEntry;
	switch (Script->Operation)
	{
	case 0x0000:
		// Stop running
		break;

	case 0x0001:
		// Stop running and replace the entry with the next line
		ScriptEntry++;
		break;

	case 0x0002:
		// Stop running and replace the entry with the specified one
		if (Script->Operand[1] == 0 ||
			++(EventObject->ScriptIdleFrameCountAuto) < Script->Operand[1])
		{
			ScriptEntry = Script->Operand[0];
		}
		else
		{
			EventObject->ScriptIdleFrameCountAuto = 0;
			ScriptEntry++;
		}
		break;

	case 0x0003:
		// unconditional jump
		if (Script->Operand[1] == 0 ||
			++(EventObject->ScriptIdleFrameCountAuto) < Script->Operand[1])
		{
			ScriptEntry = Script->Operand[0];
			RunAutoScript(ScriptEntry, InEventObjectId, bRewriteScriptEntry, bOutSuccess);
		}
		else
		{
			EventObject->ScriptIdleFrameCountAuto = 0;
			ScriptEntry++;
		}
		break;

	case 0x0004:
		// Call subroutine
		GetWorld()->SpawnActor<APALTriggerScriptActor>()
			->RunTriggerScript(Script->Operand[0], Script->Operand[1] ? Script->Operand[1] : InEventObjectId, false, bOutSuccess);
		ScriptEntry++;
		break;

	case 0x0006:
		// jump to the specified address by the specified rate
		if (FMath::RandRange(1, 100) >= Script->Operand[0])
		{
			if (Script->Operand[1] != 0)
			{
				ScriptEntry = Script->Operand[1];
				RunAutoScript(ScriptEntry, InEventObjectId, bRewriteScriptEntry, bOutSuccess);
			}
		}
		else
		{
			ScriptEntry++;
		}
		break;

	case 0x0009:
		// Wait for a certain number of frames
		if (++(EventObject->ScriptIdleFrameCountAuto) >= Script->Operand[0])
		{
			// waiting ended; go further
			EventObject->ScriptIdleFrameCountAuto = 0;
			ScriptEntry++;
		}
		break;

	case 0xFFFF:
		/*TODO if (gConfig.fIsWIN95)
		{
			int XBase = (wEventObjectID & PAL_ITEM_DESC_BOTTOM) ? (DRAW_WIDTH - 320) / 2 + 71 : gConfig.ScreenLayout.MagicDescMsgPos;
			int YBase = (wEventObjectID & PAL_ITEM_DESC_BOTTOM) ? (DRAW_HEIGHT - 49) - gConfig.ScreenLayout.ExtraItemDescLines * 16 : 3;
			int iDescLine = (wEventObjectID & ~PAL_ITEM_DESC_BOTTOM);

			if (gConfig.pszMsgFile)
			{
				int msgSpan = MESSAGE_GetSpan(&ScriptEntry);
				int idx = 0, iMsg;
				while ((iMsg = PAL_GetMsgNum(Script->Operand[0], msgSpan, idx++)) >= 0)
				{
					if (iMsg > 0)
					{
						PAL_DrawText(PAL_GetMsg(iMsg), PAL_XY(XBase, iDescLine * 16 + YBase), DESCTEXT_COLOR, TRUE, FALSE, FALSE);
						iDescLine++;
					}
				}
			}
			else
			{
				PAL_DrawText(PAL_GetMsg(Script->Operand[0]), PAL_XY(XBase, iDescLine * 16 + YBase), DESCTEXT_COLOR, TRUE, FALSE, FALSE);
				ScriptEntry++;
			}
		}
		else
		{*/
			ScriptEntry++;
		//}
		break;

	case 0x00A7:
		ScriptEntry++;
		break;

	default:
		// Other operations
		InterpretInstruction(ScriptEntry, InEventObjectId, true, bOutSuccess);
		break;
	}

	if (bRewriteScriptEntry)
	{
		InOutScriptEntry = ScriptEntry;
	}
}


APALScriptRunnerBase* UPALScriptManager::InterpretInstruction(uint16& InOutScriptEntry, const uint16 InEventObjectId, bool bRewriteScriptEntry, bool& bOutSuccess)
{
	APALGameState* GameState = GetWorld()->GetGameState<APALGameState>();
	UPALGameStateData* GameStateData = GameState->GetGameStateData();
	FEventObject* EventObject;
	if (InEventObjectId != 0)
	{
		EventObject = &(GameStateData->EventObjects[InEventObjectId - 1]);
	}
	else
	{
		EventObject = nullptr;
	}

	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	FScriptEntry* Script = &(GameData->ScriptEntries[InOutScriptEntry]);
	FEventObject* Current;
	uint16 CurrentEventObjectId;
	uint16 EventObjectId = InEventObjectId;
	if (Script->Operand[0] == 0 || Script->Operand[0] == 0xFFFF)
	{
		Current = EventObject;
		CurrentEventObjectId = EventObjectId;
	}
	else
	{
		uint16 i = Script->Operand[0] - 1;
		if (i > 0x9000)
		{
			// HACK for Dream 2.11 to avoid crash
			i -= 0x9000;
		}
		Current = &(GameStateData->EventObjects[i]);
		CurrentEventObjectId = Script->Operand[0];
	}


	APALSceneGameMode* SceneGameMode = GetWorld()->GetAuthGameMode<APALSceneGameMode>();
	APlayerController* PlayerController = GetWorld()->GetGameInstance<UPALGameInstance>()->GetMainPlayer()->GetPlayerController(nullptr);
	APALPlayerState* PlayerState = PlayerController->GetPlayerState<APALPlayerState>();
	APALScenePlayerController* ScenePlayerController = Cast<APALScenePlayerController>(PlayerController);
	UPALPlayerStateData* PlayerStateData = PlayerState->GetPlayerStateData();
	UPALAudioManager* AudioManager = GetWorld()->GetSubsystem<UPALAudioManager>();

	uint16 ScriptEntry = InOutScriptEntry;
	bool bSuccess = true;
	APALScriptRunnerBase* ScriptRunner = nullptr;
	switch (Script->Operation)
	{
	case 0x000B:
	case 0x000C:
	case 0x000D:
	case 0x000E:
		// walk one step
		if (EventObject)
		{
			EventObject->Direction = Script->Operation - 0x000B;
		}
		GameState->NPCWalkOneStep(EventObjectId, 2);
		break;

	case 0x000F:
		// Set the direction and/or gesture for event object
		if (Script->Operand[0] != 0xFFFF && EventObject)
		{
			EventObject->Direction = Script->Operand[0];
		}
		if (Script->Operand[1] != 0xFFFF && EventObject)
		{
			EventObject->CurrentFrameNum = Script->Operand[1];
		}
		break;

	case 0x0010:
		// Walk straight to the specified position
		if (!GameState->NPCWalkTo(EventObjectId, FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 3))
		{
			ScriptEntry--;
		}
		break;

	case 0x0011:
		// Walk straight to the specified position, at a lower speed
		if (!GameState->NPCWalkTo(EventObjectId, FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 1))
		{
			ScriptEntry--;
		}
		break;

	case 0x0012:
		// Set the position of the event object, relative to the party
	{
		FPALPosition2d PartyPosition2d = ScenePlayerController->GetPartyPosition().to2d();
		Current->X = Script->Operand[1] + FMath::RoundToInt(PartyPosition2d.X);
		Current->Y = Script->Operand[2] + FMath::RoundToInt(PartyPosition2d.Y);
	}
	break;

	case 0x0013:
		// Set the position of the event object
		Current->X = Script->Operand[1];
		Current->Y = Script->Operand[2];
		break;

	case 0x0014:
		// Set the gesture of the event object
		check(EventObject);
		EventObject->CurrentFrameNum = Script->Operand[0];
		EventObject->Direction = EPALDirection::South;
		break;

	case 0x0015:
		// Set the direction and gesture for a party member
		PlayerStateData->PartyDirection = static_cast<EPALDirection>(Script->Operand[0]);
		if (PlayerStateData->Party.IsValidIndex(Script->Operand[2]))
		{
			ScenePlayerController->RoleStopWalking(PlayerStateData->Party[Script->Operand[2]]->RoleId);
			ScenePlayerController->RoleAtEase(PlayerStateData->Party[Script->Operand[2]]->RoleId, true);
			PlayerStateData->Party[Script->Operand[2]]->FrameNum = PlayerStateData->PartyDirection * 3 + Script->Operand[1];
		}
		else
		{
			PAL_DebugMsg("Trying to set non-existing party member. 0x0015");
		}
		break;

	case 0x0016:
		// Set the direction and gesture for an event object
		if (Script->Operand[0] != 0)
		{
			Current->Direction = Script->Operand[1];
			Current->CurrentFrameNum = Script->Operand[2];
		}
		break;

	case 0x0017:
		// set the player's extra attribute
	{
		uint16* p = (uint16*)(&PlayerStateData->EquipmentEffects[Script->Operand[0] - 0xB]); // HACKHACK
		p[Script->Operand[1] * MAX_PLAYER_ROLES + EventObjectId] = static_cast<int16>(Script->Operand[2]);
	}
	break;

	case 0x0018:
		// Equip the selected item
	{
		EPALBodyPart i = static_cast<EPALBodyPart>(Script->Operand[0] - 0x0B);
		PlayerStateData->CurrentEquipPart = i;

		const SIZE_T RoleId = static_cast<SIZE_T>(EventObjectId);
		PlayerState->RemoveEquipmentEffect(RoleId, i);

		if (PlayerStateData->PlayerRoles.Equipments[i][RoleId] != Script->Operand[1])
		{
			uint16 w = PlayerStateData->PlayerRoles.Equipments[i][RoleId];
			PlayerStateData->PlayerRoles.Equipments[i][RoleId] = Script->Operand[1];

			PlayerState->AddItemToInventory(Script->Operand[1], -1);
			if (w != 0)
			{
				PlayerState->AddItemToInventory(w, 1);
			}
		}
	}
	break;

	case 0x0019:
		// Increase/decrease the player's attribute
	{
		uint16* p = (uint16*)(&PlayerStateData->PlayerRoles); // HACKHACK
		SIZE_T RoleId;
		if (Script->Operand[2] == 0)
		{
			RoleId = EventObjectId;
		}
		else
		{
			RoleId = static_cast<SIZE_T>(Script->Operand[2] - 1);
		}

		p[Script->Operand[0] * MAX_PLAYER_ROLES + RoleId] += static_cast<int16>(Script->Operand[1]);
	}
	break;

	case 0x001A:
		// Set player's stat
	{
		uint16* p = (uint16*)(&PlayerStateData->PlayerRoles); // HACKHACK

		if (PlayerStateData->CurrentEquipPart != -1)
		{
			// In the progress of equipping items
			p = (uint16*)&(PlayerStateData->EquipmentEffects[PlayerStateData->CurrentEquipPart]);
		}
		SIZE_T RoleId;
		if (Script->Operand[2] == 0)
		{
			// Apply to the current player. The EventObjectId should
			// indicate the player role.
			RoleId = EventObjectId;
		}
		else
		{
			RoleId = static_cast<SIZE_T>(Script->Operand[2] - 1);
		}

		p[Script->Operand[0] * MAX_PLAYER_ROLES + RoleId] = static_cast<int16>(Script->Operand[1]);
	}
	break;

	case 0x001B:
		// Increase/decrease player's HP
		if (Script->Operand[0])
		{
			// Apply to everyone
			for (UPALRoleData* PartyMember : PlayerStateData->Party)
			{
				PlayerState->IncreaseHPMP(PartyMember->RoleId, static_cast<int16>(Script->Operand[1]), 0);
				bSuccess = true;
			}
		}
		else
		{
			// Apply to one player. The EventObjectId parameter should indicate the player role.
			if (!PlayerState->IncreaseHPMP(EventObjectId, static_cast<int16>(Script->Operand[1]), 0))
			{
				bSuccess = false;
			}
		}
		break;

	case 0x001C:
		// Increase/decrease player's MP
		if (Script->Operand[0])
		{
			// Apply to everyone
			for (UPALRoleData* PartyMember : PlayerStateData->Party)
			{
				if (PlayerState->IncreaseHPMP(PartyMember->RoleId, 0, static_cast<int16>(Script->Operand[1])))
				{
					bSuccess = true;
				}
			}
		}
		else
		{
			// Apply to one player. The EventObjectId parameter should indicate the player role.
			if (!PlayerState->IncreaseHPMP(EventObjectId, 0, static_cast<int16>(Script->Operand[1])))
			{
				bSuccess = false;
			}
		}
		break;

	case 0x001D:
		// Increase/decrease player's HP and MP
		if (Script->Operand[0])
		{
			// Apply to everyone
			for (UPALRoleData* PartyMember : PlayerStateData->Party)
			{
				if (PlayerState->IncreaseHPMP(PartyMember->RoleId, static_cast<int16>(Script->Operand[1]), static_cast<int16>(Script->Operand[1])))
				{
					bSuccess = true;
				}
			}
		}
		else
		{
			// Apply to one player. The EventObjectId parameter should indicate the player role.
			if (!PlayerState->IncreaseHPMP(EventObjectId, static_cast<int16>(Script->Operand[1]), static_cast<int16>(Script->Operand[1])))
			{
				bSuccess = false;
			}
		}
		break;

	case 0x001E:
		// Increase or decrease cash by the specified amount
		if (static_cast<int16>(Script->Operand[0]) < 0 && PlayerStateData->Cash < static_cast<uint32>(-static_cast<int16>(Script->Operand[0])))
		{
			// not enough cash
			ScriptEntry = Script->Operand[1] - 1;
		}
		else
		{
			PlayerStateData->Cash += static_cast<int16>(Script->Operand[0]);
		}
		break;

	case 0x001F:
		// Add item to inventory
		if (Script->Operand[1])
		{
			PlayerState->AddItemToInventory(Script->Operand[0], static_cast<int16>(Script->Operand[1]));
		}
		else
		{
			PlayerState->AddItemToInventory(Script->Operand[0]);
		}
		break;

	case 0x0020:
		// Remove item from inventory
	{
		int32 x = Script->Operand[1];
		if (x == 0)
		{
			x = 1;
		}
		if (x <= PlayerState->CountItem(Script->Operand[0]) || Script->Operand[2] == 0)
		{
			if (!PlayerState->AddItemToInventory(Script->Operand[0], -x))
			{
				// Try removing equipped item
				bool bEnd = false;
				for (UPALRoleData* PartyMember : PlayerStateData->Party)
				{
					SIZE_T RoleId = PartyMember->RoleId;

					for (SIZE_T j = 0; j < MAX_PLAYER_EQUIPMENTS; j++)
					{
						if (PlayerStateData->PlayerRoles.Equipments[j][RoleId] == Script->Operand[0])
						{
							PlayerState->RemoveEquipmentEffect(RoleId, static_cast<EPALBodyPart>(j));
							PlayerStateData->PlayerRoles.Equipments[j][RoleId] = 0;

							if (--x == 0)
							{
								bEnd = true;
								break;
							}
						}
					}
					if (bEnd)
					{
						break;
					}
				}
			}
		}
		else
		{
			ScriptEntry = Script->Operand[2] - 1;
		}
	}
	break;

	case 0x0021:
		// Inflict damage to the enemy
		if (Script->Operand[0])
		{
			// Inflict damage to all enemies
			PlayerState->DamageAllEnemies(Script->Operand[1]);
		}
		else
		{
			// Inflict damage to one enemy
			PlayerState->DamageEnemy(EventObjectId, Script->Operand[1]);
		}
		break;

	case 0x0022:
		// Revive player
		if (Script->Operand[0])
		{
			// Apply to everyone
			bSuccess = PlayerState->ReviveAll(Script->Operand[1] / static_cast<double>(10));
		}
		else
		{
			// Apply to one player
			bSuccess = PlayerState->Revive(EventObjectId, Script->Operand[1] / static_cast<double>(10));
		}
		break;

	case 0x0023:
		// Remove equipment from the specified player
	{
		SIZE_T RoleId;
		if (Script->Operand[0] < MAX_PLAYABLE_PLAYER_ROLES)
		{
			if (PlayerStateData->Party.IsValidIndex(Script->Operand[0]))
			{
				RoleId = PlayerStateData->Party[Script->Operand[0]]->RoleId;
			}
			else
			{
				RoleId = PlayerStateData->LastPartyRoleId[Script->Operand[0]];
			}
		}
		else
		{
			RoleId = PlayerStateData->Party[0]->RoleId;
		}
		if (Script->Operand[1] == 0)
		{
			// Remove all equipments
			for (SIZE_T i = 0; i < MAX_PLAYER_EQUIPMENTS; i++)
			{
				uint16 w = PlayerStateData->PlayerRoles.Equipments[i][RoleId];
				if (w != 0)
				{
					PlayerState->AddItemToInventory(w, 1);
					PlayerStateData->PlayerRoles.Equipments[i][RoleId] = 0;
				}
				PlayerState->RemoveEquipmentEffect(RoleId, static_cast<EPALBodyPart>(i));
			}
		}
		else
		{
			uint16 w = PlayerStateData->PlayerRoles.Equipments[Script->Operand[1] - 1][RoleId];
			if (w != 0)
			{
				PlayerState->RemoveEquipmentEffect(RoleId, static_cast<EPALBodyPart>(Script->Operand[1] - 1));
				PlayerState->AddItemToInventory(w, 1);
				PlayerStateData->PlayerRoles.Equipments[Script->Operand[1] - 1][RoleId] = 0;
			}
		}
	}
	break;

	case 0x0024:
		// Set the autoscript entry address for an event object
		if (Script->Operand[0] != 0)
		{
			Current->AutoScript = Script->Operand[1];
		}
		break;

	case 0x0025:
		// Set the trigger script entry address for an event object
		if (Script->Operand[0] != 0)
		{
			Current->TriggerScript = Script->Operand[1];
		}
		break;

	case 0x0026:
		// Show the buy item menu
		ScenePlayerController->Buy(Script->Operand[0]);
		break;

	case 0x0027:
		// Show the sell item menu
		ScenePlayerController->Sell();
		break;

	case 0x0028:
		// Apply poison to enemy
		if (Script->Operand[0])
		{
			// Apply to everyone
			for (SIZE_T i = 0; i < PlayerStateData->CurrentEnemies.Num(); i++)
			{
				UPALBattleEnemyData* EnemyData = PlayerStateData->CurrentEnemies[i];
				uint16 ObjectId = EnemyData->GetObjectId();
				if (FMath::RandRange(0, 9) >= GameStateData->Objects[ObjectId].Enemy.ResistanceToSorcery)
				{
					PlayerState->AddPoisonForEnemy(EventObjectId, Script->Operand[1]);
				}
			}
		}
		else
		{
			// Apply to one enemy
			uint16 ObjectId = PlayerStateData->CurrentEnemies[EventObjectId]->GetObjectId();
			if (FMath::RandRange(0, 9) >= GameStateData->Objects[ObjectId].Enemy.ResistanceToSorcery)
			{
				PlayerState->AddPoisonForEnemy(EventObjectId, Script->Operand[1]);
			}
		}
		break;

	case 0x0029:
		// Apply poison to role
		if (Script->Operand[0])
		{
			// Apply to everyone
			for (UPALRoleData* PartyMember : PlayerStateData->Party)
			{
				SIZE_T RoleId = PartyMember->RoleId;
				if (FMath::RandRange(1, 100) > PlayerState->GetRolePoisonResistance(RoleId))
				{
					PlayerState->AddPoisonForRole(RoleId, Script->Operand[1]);
				}
			}
		}
		else
		{
			// Apply to one role
			if (FMath::RandRange(1, 100) > PlayerState->GetRolePoisonResistance(EventObjectId))
			{
				PlayerState->AddPoisonForRole(EventObjectId, Script->Operand[1]);
			}
		}
		break;

	case 0x002A:
		// Cure poison by object ID for enemy
		if (Script->Operand[0])
		{
			// Apply to all enemies
			for (SIZE_T i = 0; i < PlayerStateData->CurrentEnemies.Num(); i++)
			{
				PlayerState->CurePoisonForEnemyByKind(i, Script->Operand[1]);
			}
		}
		else
		{
			// Apply to one enemy
			PlayerState->CurePoisonForEnemyByKind(EventObjectId, Script->Operand[1]);
		}
		break;

	case 0x002B:
		// Cure poison by object ID for player
		if (Script->Operand[0])
		{
			for (UPALRoleData* PartyMember : PlayerStateData->Party)
			{
				SIZE_T RoleId = PartyMember->RoleId;
				PlayerState->CurePoisonForRoleByKind(RoleId, Script->Operand[1]);
			}
		}
		else
		{
			PlayerState->CurePoisonForRoleByKind(EventObjectId, Script->Operand[1]);
		}
		break;

	case 0x002C:
		// Cure poisons by level
		if (Script->Operand[0])
		{
			for (UPALRoleData* PartyMember : PlayerStateData->Party)
			{
				SIZE_T RoleId = PartyMember->RoleId;
				PlayerState->CurePoisonForRoleByLevel(RoleId, Script->Operand[1]);
			}
		}
		else
		{
			PlayerState->CurePoisonForRoleByLevel(EventObjectId, Script->Operand[1]);
		}
		break;

	case 0x002D:
		// Set the status for player
		PlayerState->SetRoleStatus(EventObjectId, static_cast<EPALStatus>(Script->Operand[0]), Script->Operand[1]);
		break;

	case 0x002E:
		// Set the status for enemy
	{
		uint16 ObjectId = PlayerStateData->CurrentEnemies[EventObjectId]->GetObjectId();
		if (FMath::RandRange(0, 9) >= GameStateData->Objects[ObjectId].Enemy.ResistanceToSorcery)
		{
			PlayerStateData->CurrentEnemies[EventObjectId]->Status[Script->Operand[0]] = Script->Operand[1];
		}
		else
		{
			ScriptEntry = Script->Operand[2] - 1;
		}
		break;
	}

	case 0x002F:
		// Remove player's status
		PlayerState->RemoveRoleStatus(EventObjectId, static_cast<EPALStatus>(Script->Operand[0]));
		break;

	case 0x0030:
		// Increase player's stat temporarily by percent
	{
		uint16* p = (uint16*)(&PlayerStateData->EquipmentEffects[EPALBodyPart::Extra]); // HACKHACK
		uint16* p1 = (uint16*)(&PlayerStateData->PlayerRoles);

		SIZE_T RoleId;
		if (Script->Operand[2] == 0)
		{
			RoleId = EventObjectId;
		}
		else
		{
			RoleId = Script->Operand[2] - 1;
		}

		p[Script->Operand[0] * MAX_PLAYER_ROLES + RoleId] = p1[Script->Operand[0] * MAX_PLAYER_ROLES + RoleId] * (int16)Script->Operand[1] / 100;
	}
	break;

	case 0x0031:
		// Change battle sprite temporarily for player
		PlayerStateData->EquipmentEffects[EPALBodyPart::Extra].SpriteNumInBattle[EventObjectId] = Script->Operand[0];
		break;

	case 0x0033:
		// collect the enemy for items
		if (PlayerStateData->CurrentEnemies[EventObjectId]->Enemy.CollectValue != 0)
		{
			PlayerStateData->CollectValue += PlayerStateData->CurrentEnemies[EventObjectId]->Enemy.CollectValue;
		}
		else
		{
			ScriptEntry = Script->Operand[0] - 1;
		}
		break;

	case 0x0034:
		//
		// Transform collected enemies into items
		//
		/*TODO
		if (gpGlobals->wCollectValue > 0)
		{
			WCHAR s[256];

#ifdef PAL_CLASSIC
			i = RandomLong(1, gpGlobals->wCollectValue);
			if (i > 9)
			{
				i = 9;
			}
#else
			i = RandomLong(1, 9);
			if (i > gpGlobals->wCollectValue)
			{
				i = gpGlobals->wCollectValue;
			}
#endif

			gpGlobals->wCollectValue -= i;
			i--;

			AddItemToInventory(gpGlobals->g.lprgStore[0].rgwItems[i], 1);

			g_TextLib.iDialogShadow = 5;
			PAL_StartDialogWithOffset(kDialogCenterWindow, 0, 0, FALSE, 0, -10);
			PAL_swprintf(s, sizeof(s) / sizeof(WCHAR), L"%ls@%ls@", PAL_GetWord(42),
				PAL_GetWord(gpGlobals->g.lprgStore[0].rgwItems[i]));
			LPCBITMAPRLE pBG = PAL_SpriteGetFrame(gpSpriteUI, SPRITENUM_ITEMBOX);
			INT iBGWidth = PAL_RLEGetWidth(pBG), iBGHeight = PAL_RLEGetHeight(pBG);
			INT iBG_X = (DRAW_WIDTH - iBGWidth) / 2, iBG_Y = (DRAW_HEIGHT - iBGHeight) / 2;
			PAL_POS pos = PAL_XY(iBG_X, iBG_Y);
			SDL_Rect rect = { iBG_X, iBG_Y, iBGWidth, iBGHeight };
			PAL_RLEBlitToSurface(pBG, gpScreen, pos);

			WORD wObject = gpGlobals->g.lprgStore[0].rgwItems[i];
			static WORD wPrevImageIndex = 0xFFFF;
			static BYTE bufImage[2048];
			if (gpGlobals->g.rgObject[wObject].item.wBitmap != wPrevImageIndex)
			{
				if (PAL_MKFReadChunk(bufImage, 2048,
					gpGlobals->g.rgObject[wObject].item.wBitmap, gpGlobals->f.fpBALL) > 0)
				{
					wPrevImageIndex = gpGlobals->g.rgObject[wObject].item.wBitmap;
				}
				else
				{
					wPrevImageIndex = 0xFFFF;
				}
			}
			if (wPrevImageIndex != 0xFFFF)
			{
				PAL_RLEBlitToSurface(bufImage, gpScreen, PAL_XY(PAL_X(pos) + 8, PAL_Y(pos) + 7));
			}

			VIDEO_UpdateScreen(&rect);

			PAL_ShowDialogText(s);
			g_TextLib.iDialogShadow = 0;
		}
		else
		{
			ScriptEntry = Script->Operand[0] - 1;
		}*/
		break;

	case 0x0035:
		// Shake the screen
	{
		uint32 ShakeLevel = Script->Operand[1];
		if (ShakeLevel == 0)
		{
			ShakeLevel = 4;
		}
		SceneGameMode->ShakeScreen(Script->Operand[0] * FRAME_TIME, ShakeLevel);
	}
	break;

	case 0x0036:
		//
		// Set the current playing RNG animation
		//
		/*TODO
		gpGlobals->iCurPlayingRNG = Script->Operand[0];*/
		break;

	case 0x0037:
		//
		// Play RNG animation
		//
		/*TODO
		PAL_RNGPlay(gpGlobals->iCurPlayingRNG,
			Script->Operand[0],
			Script->Operand[1] > 0 ? Script->Operand[1] : -1,
			Script->Operand[2] > 0 ? Script->Operand[2] : 16,
			0, FALSE);*/
		break;

	case 0x0038:
		// Teleport the party out of the scene
		if (!PlayerStateData->bInBattle &&
			GameStateData->Scenes[GameStateData->SceneNum - 1].ScriptOnTeleport != 0)
		{
			RunTriggerScript(GameStateData->Scenes[GameStateData->SceneNum - 1].ScriptOnTeleport, UPALScriptManager::LastTriggeredEventObjectId, false);
		}
		else
		{
			// failed
			bSuccess = false;
			ScriptEntry = Script->Operand[0] - 1;
		}
		break;

	case 0x0039:
		//
		// Drain HP from enemy
		//
		/*TODO
		w = gpGlobals->rgParty[g_Battle.wMovingPlayerIndex].wPlayerRole;

		g_Battle.rgEnemy[EventObjectId].e.wHealth -= Script->Operand[0];
		PlayerStateData->PlayerRoles.rgwHP[w] += Script->Operand[0];

		if (PlayerStateData->PlayerRoles.rgwHP[w] > PlayerStateData->PlayerRoles.rgwMaxHP[w])
		{
			PlayerStateData->PlayerRoles.rgwHP[w] = PlayerStateData->PlayerRoles.rgwMaxHP[w];
		}*/
		break;

	case 0x003A:
		//
		// Player flee from the battle
		//
		/*TODO
		if (g_Battle.fIsBoss)
		{
			//
			// Cannot flee from bosses
			//
			ScriptEntry = Script->Operand[0] - 1;
		}
		else
		{
			PAL_BattlePlayerEscape();
		}*/
		break;

	case 0x003F:
		// Ride the event object to the specified position, at a low speed
		ScriptRunner = GetWorld()->SpawnActor<APALRideEventObjectScriptRunner>();
		Cast<APALRideEventObjectScriptRunner>(ScriptRunner)->Init(EventObjectId, FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 2);
		break;

	case 0x0040:
		// set the trigger method for a event object
		if (Script->Operand[0] != 0)
		{
			Current->TriggerMode = Script->Operand[1];
		}
		break;

	case 0x0041:
		// Mark the script as failed
		bSuccess = false;
		break;

	case 0x0042:
		// Simulate a magic for player
	{
		int16 i = static_cast<int16>(Script->Operand[2]) - 1;
		if (i < 0)
		{
			i = EventObjectId;
		}
		// TODO PAL_BattleSimulateMagic(i, Script->Operand[0], Script->Operand[1]);
	}
		break;

	case 0x0043:
		// Set background music
		GameStateData->MusicNum = Script->Operand[0];
		AudioManager->PlayMusic(Script->Operand[0], Script->Operand[1] != 1, (Script->Operand[1] == 3 && Script->Operand[0] != 9) ? 3.0f : 0.0f);
		break;

	case 0x0044:
		// Ride the event object to the specified position, at the normal speed
		ScriptRunner = GetWorld()->SpawnActor<APALRideEventObjectScriptRunner>();
		Cast<APALRideEventObjectScriptRunner>(ScriptRunner)->Init(EventObjectId, FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 4);
		break;

	case 0x0045:
		// Set battle music
		PlayerStateData->BattleMusicNum = Script->Operand[0];
		break;

	case 0x0046:
		// Set the party position on the map
	{
		double XOffset = ((PlayerStateData->PartyDirection == EPALDirection::West || PlayerStateData->PartyDirection == EPALDirection::South) ? 16 : -16);
		double YOffset = ((PlayerStateData->PartyDirection == EPALDirection::West || PlayerStateData->PartyDirection == EPALDirection::North) ? 16 : -16);

		double Z = ScenePlayerController->GetPartyPosition().Z;
		double X = Script->Operand[0] * 32 + Script->Operand[2] * 16;
		double Y = Script->Operand[1] * 32 + Script->Operand[2] * 16 + Z * SQRT_3;

		// TODO height problem

		double XAccumulativeOffset = 0;
		double YAccumulativeOffset = 0;
		for (UPALRoleData* PartyMember : PlayerStateData->Party)
		{
			PartyMember->Position.X = X + XAccumulativeOffset;
			PartyMember->Position.Y = Y + YAccumulativeOffset;
			XAccumulativeOffset += XOffset;
			YAccumulativeOffset += YOffset;
		}
		for (UPALRoleData* Follower : PlayerStateData->Follow)
		{
			Follower->Position.X = X + XAccumulativeOffset;
			Follower->Position.Y = Y + YAccumulativeOffset;
			XAccumulativeOffset += XOffset;
			YAccumulativeOffset += YOffset;
		}

		XAccumulativeOffset = 0;
		YAccumulativeOffset = 0;
		for (FTrail& Trail : PlayerStateData->Trails)
		{
			Trail.Position.X = X + XAccumulativeOffset;
			Trail.Position.Y = Y + YAccumulativeOffset;
			Trail.Direction = PlayerStateData->PartyDirection;
			XAccumulativeOffset += XOffset;
			YAccumulativeOffset += YOffset;
		}
	}
	break;

	case 0x0047:
		// Play sound effect
	{
		APALEventObjectActor* EventObjectActor = nullptr;
		if (CurrentEventObjectId)
		{
			for (TActorIterator<APALEventObjectActor> It(GetWorld(), APALEventObjectActor::StaticClass()); It; ++It)
			{
				if ((*It)->IsValidLowLevelFast() && (*It)->GetEventObjectId() == CurrentEventObjectId)
				{
					EventObjectActor = *It;
					break;
				}
			}
		}
		AudioManager->PlaySound(Script->Operand[0], EventObjectActor);
	}
	break;

	case 0x0049:
		// Set the state of event object
		if (Script->Operand[0] != 0)
		{
			Current->State = Script->Operand[1];
		}
		break;

	case 0x004A:
		// Set the current battlefield
		PlayerStateData->BattleFieldNum = Script->Operand[0];
		break;

	case 0x004B:
		// Nullify the event object for a short while
		EventObject->VanishTime = -15;
		break;

	case 0x004C:
		// chase the player
	{
		uint16 MaxDistance = Script->Operand[0];
		int32 Speed = Script->Operand[1];

		if (MaxDistance == 0)
		{
			MaxDistance = 8;
		}

		if (Speed == 0)
		{
			Speed = 4;
		}

		GameState->MonsterChasePlayer(EventObjectId, Speed, MaxDistance, static_cast<bool>(Script->Operand[2]), ScenePlayerController);
	}
	break;

	case 0x004D:
		//
		// wait for any key
		//
		// TODO PAL_WaitForKey(0);
		break;

	case 0x004E:
		// Load the last saved game
		ScriptRunner = GetWorld()->SpawnActor<APALTimedFadeScriptRunner>();
		Cast<APALTimedFadeScriptRunner>(ScriptRunner)->Init(false, 1);
		SceneGameMode->ReloadInNextTick(PlayerState->CurrentSaveSlot);
		return nullptr; // don't go further

	case 0x004F:
		//
		// Fade the screen to red color (game over)
		//
		// TODO PAL_FadeToRed();
		break;

	case 0x0050:
		// screen fade out
		ScriptRunner = GetWorld()->SpawnActor<APALTimedFadeScriptRunner>();
		Cast<APALTimedFadeScriptRunner>(ScriptRunner)->Init(false, Script->Operand[0] ? Script->Operand[0] : 1);
		break;

	case 0x0051:
		// screen fade in
		ScriptRunner = GetWorld()->SpawnActor<APALTimedFadeScriptRunner>(); 
		Cast<APALTimedFadeScriptRunner>(ScriptRunner)->Init(true, (static_cast<int16>(Script->Operand[0]) > 0) ? Script->Operand[0] : 1);
		break;

	case 0x0052:
		// hide the event object for a while, default 800 frames
		EventObject->State *= -1;
		EventObject->VanishTime = (Script->Operand[0] ? Script->Operand[0] : 800);
		break;

	case 0x0053:
		// use the day palette
		if (SceneGameMode)
		{
			SceneGameMode->SetDayNight(false);
		}
		break;

	case 0x0054:
		// use the night palette
		SceneGameMode->SetDayNight(true);
		break;

	case 0x0055:
		// Add magic to a player
	{
		uint16 i = Script->Operand[1];
		if (i == 0)
		{
			i = EventObjectId;
		}
		else
		{
			i--;
		}
		PlayerState->AddMagic(i, Script->Operand[0]);
	}
	break;

	case 0x0056:
		// Remove magic from a player
	{
		uint16 i = Script->Operand[1];
		if (i == 0)
		{
			i = EventObjectId;
		}
		else
		{
			i--;
		}
		PlayerState->RemoveMagic(i, Script->Operand[0]);
	}
	break;

	case 0x0057:
		// Set the base damage of magic according to MP value
	{
		uint16 i = ((Script->Operand[1] == 0) ? 8 : Script->Operand[1]);
		uint16 j = GameStateData->Objects[Script->Operand[0]].Magic.MagicNumber;
		GameData->Magics[j].BaseDamage = PlayerStateData->PlayerRoles.MP[EventObjectId] * i; // HACKHACK
		PlayerStateData->PlayerRoles.MP[EventObjectId] = 0;
	}
	break;

	case 0x0058:
		// Jump if there is less than the specified number of the specified items in the inventory
		if (PlayerState->GetItemAmount(Script->Operand[0]) < static_cast<int16>(Script->Operand[1]))
		{
			ScriptEntry = Script->Operand[2] - 1;
		}
		break;

	case 0x0059:
		// Change to the specified scene
		if (Script->Operand[0] > 0 && Script->Operand[0] <= MAX_SCENES && GameStateData->SceneNum != Script->Operand[0])
		{
			// Set data to load the scene in the next frame
			GameStateData->SceneNum = Script->Operand[0];
			check(SceneGameMode);
			SceneGameMode->LoadScene();
			PlayerStateData->Layer = 0;
		}
		break;

	case 0x005A:
		// Halve the player's HP
		// The EventObjectId parameter here should indicate the player role
		PlayerStateData->PlayerRoles.HP[EventObjectId] /= 2;
		break;

	case 0x005B:
		// Halve the enemy's HP
	{
		uint16 HalfHP = PlayerStateData->CurrentEnemies[EventObjectId]->Enemy.Health / 2 + 1;
		if (HalfHP > Script->Operand[0])
		{
			HalfHP = Script->Operand[0];
		}
		PlayerStateData->CurrentEnemies[EventObjectId]->Enemy.Health -= HalfHP;
	}
		break;

	case 0x005C:
		//
		// Hide for a while
		//
		/*TODO g_Battle.iHidingTime = -(INT)(Script->Operand[0]);*/
		break;

	case 0x005D:
		// Jump if player doesn't have the specified poison
		if (!PlayerState->IsRolePoisonedByKind(EventObjectId, Script->Operand[0]))
		{
			ScriptEntry = Script->Operand[1] - 1;
		}
		break;

	case 0x005E:
		// Jump if enemy doesn't have the specified poison
		/*TODO
		for (SIZE_T i = 0; i < MAX_POISONS; i++)
		{
			if (g_Battle.rgEnemy[EventObjectId].rgPoisons[i].wPoisonID == Script->Operand[0])
			{
				break;
			}
		}

		if (i >= MAX_POISONS)
		{
			ScriptEntry = Script->Operand[1] - 1;
		}*/
		break;

	case 0x005F:
		// Kill the player immediately
		// The EventObjectId parameter here should indicate the player role
		PlayerStateData->PlayerRoles.HP[EventObjectId] = 0;
		break;

	case 0x0060:
		//
		// Immediate KO of the enemy
		//
		// TODO g_Battle.rgEnemy[EventObjectId].e.wHealth = 0;
		break;

	case 0x0061:
		// Jump if player is not poisoned
		if (!PlayerState->IsRolePoisonedByLevel(EventObjectId, 1))
		{
			ScriptEntry = Script->Operand[0] - 1;
		}
		break;

	case 0x0062:
		// Pause enemy chasing for a while
		GameStateData->ChaseSpeedChangeCycles = Script->Operand[0];
		GameStateData->ChaseRange = 0;
		break;

	case 0x0063:
		// Speed up enemy chasing for a while
		GameStateData->ChaseSpeedChangeCycles = Script->Operand[0];
		GameStateData->ChaseRange = 3;
		break;

	case 0x0064:
		//
		// Jump if enemy's HP is more than the specified percentage
		//
		/*TODO i = gpGlobals->g.rgObject[g_Battle.rgEnemy[EventObjectId].wObjectID].enemy.wEnemyID;
		if ((INT)(g_Battle.rgEnemy[EventObjectId].e.wHealth) * 100 >
			(INT)(gpGlobals->g.lprgEnemy[i].wHealth) * Script->Operand[0])
		{
			ScriptEntry = Script->Operand[1] - 1;
		}*/
		break;

	case 0x0065:
		// Set the player's sprite
		PlayerStateData->PlayerRoles.SpriteNum[Script->Operand[0]] = Script->Operand[1];
		if (!PlayerStateData->bInBattle && Script->Operand[2])
		{
			ScenePlayerController->PartyStopWalking();
			ScenePlayerController->ReloadRoleSprites();
		}
		break;

	case 0x0066:
		//
		// Throw weapon to enemy
		//
		/*TODO w = Script->Operand[1] * 5;
		w += PlayerStateData->PlayerRoles.rgwAttackStrength[gpGlobals->rgParty[g_Battle.wMovingPlayerIndex].wPlayerRole] * RandomFloat(0, 4);
		PAL_BattleSimulateMagic((SHORT)EventObjectId, Script->Operand[0], w);*/
		break;

	case 0x0067:
		//
		// Enemy use magic
		//
		/*TODO g_Battle.rgEnemy[EventObjectId].e.wMagic = Script->Operand[0];
		g_Battle.rgEnemy[EventObjectId].e.wMagicRate =
			((Script->Operand[1] == 0) ? 10 : Script->Operand[1]);*/
		break;

	case 0x0068:
		//
		// Jump if it's enemy's turn
		//
		/*TODO if (g_Battle.fEnemyMoving)
		{
			ScriptEntry = Script->Operand[0] - 1;
		}*/
		break;

	case 0x0069:
		//
		// Enemy escape in battle
		//
		// TODO PAL_BattleEnemyEscape();
		break;

	case 0x006A:
		//
		// Steal from the enemy
		//
		// TODO PAL_BattleStealFromEnemy(EventObjectId, Script->Operand[0]);
		break;

	case 0x006B:
		//
		// Blow away enemies
		//
		// TODO g_Battle.iBlow = static_cast<int16>(Script->Operand[0]);
		break;

	case 0x006C:
		// Walk the NPC in one step
		Current->X += static_cast<int16>(Script->Operand[1]);
		Current->Y += static_cast<int16>(Script->Operand[2]);
		GameState->NPCWalkOneStep(CurrentEventObjectId, 0);
		break;

	case 0x006D:
		// Set the enter script and teleport script for a scene
		if (Script->Operand[0])
		{
			if (Script->Operand[1])
			{
				GameStateData->Scenes[Script->Operand[0] - 1].ScriptOnEnter =
					Script->Operand[1];
			}

			if (Script->Operand[2])
			{
				GameStateData->Scenes[Script->Operand[0] - 1].ScriptOnTeleport =
					Script->Operand[2];
			}

			if (Script->Operand[1] == 0 && Script->Operand[2] == 0)
			{
				GameStateData->Scenes[Script->Operand[0] - 1].ScriptOnEnter = 0;
				GameStateData->Scenes[Script->Operand[0] - 1].ScriptOnTeleport = 0;
			}
		}
		break;

	case 0x006E:
		// Move the player to the specified position in one step
	{
		const FPALPosition3d& PartyPosition = ScenePlayerController->GetPartyPosition();
		double X = PartyPosition.X + static_cast<int16>(Script->Operand[0]);
		double Y = PartyPosition.Y + static_cast<int16>(Script->Operand[1] * 2);

		PlayerStateData->Layer = Script->Operand[2] * 8;
		if (Script->Operand[0] != 0 || Script->Operand[1] != 0)
		{
			ScenePlayerController->PartyWalkTo(FPALPosition3d(X, Y, PartyPosition.Z));
		}
	}
	break;

	case 0x006F:
		// Sync the state of current event object with another event object
		if (Current->State == static_cast<int16>(Script->Operand[1]))
		{
			EventObject->State = static_cast<int16>(Script->Operand[1]);
		}
		break;

	case 0x0070:
		// Walk the party to the specified position
		ScriptRunner = GetWorld()->SpawnActor<APALPartyWalkScriptRunner>();
		Cast<APALPartyWalkScriptRunner>(ScriptRunner)->Init(FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 2);
		break;

	case 0x0071:
		// Wave the screen
		check(SceneGameMode);
		SceneGameMode->WaveScreen(Script->Operand[0], static_cast<int16>(Script->Operand[1]) / FRAME_TIME);
		break;

	case 0x0073:
		//
		// Fade the screen to scene
		//
		/*TODO VIDEO_BackupScreen(gpScreen);
		PAL_MakeScene();
		VIDEO_FadeScreen(Script->Operand[0]);*/
		break;

	case 0x0074:
		// Jump if not all players are full HP
		for (UPALRoleData* PartyMember : PlayerStateData->Party)
		{
			SIZE_T RoleId = PartyMember->RoleId;
			if (PlayerStateData->PlayerRoles.HP[RoleId] < PlayerStateData->PlayerRoles.MaxHP[RoleId])
			{
				ScriptEntry = Script->Operand[0] - 1;
				break;
			}
		}
		break;

	case 0x0075:
		// Set the player party
	{
		for (SIZE_T i = 0; i < PlayerStateData->Party.Num(); i++)
		{
			PlayerStateData->LastPartyRoleId[i] = PlayerStateData->Party[i]->RoleId;
		}
		const FPALPosition3d& Position = ScenePlayerController->GetPartyPosition();
		ScenePlayerController->RemoveAllPartyRoles();
		for (SIZE_T i = 0; i < 3; i++)
		{
			if (Script->Operand[i] != 0)
			{
				ScenePlayerController->AddPartyRole(static_cast<SIZE_T>(Script->Operand[i] - 1));
			}
		}

		if (PlayerStateData->Party.IsEmpty())
		{
			// HACK for Dream 2.11
			ScenePlayerController->AddPartyRole(0);
		}

		for (UPALRoleData* PartyMember : PlayerStateData->Party)
		{
			PartyMember->Position = Position;
		}

		// Reload the player sprites
		check(SceneGameMode);
		SceneGameMode->LoadRoleSprites();

		PlayerState->ClearAllRolePoisons();
		PlayerState->UpdateEquipments();
	}
	break;

	case 0x0076:
		//
		// Show FBP picture
		//
		/*
		if (gConfig.fIsWIN95)
		{
			SDL_FillRect(gpScreen, NULL, 0);
			VIDEO_UpdateScreen(NULL);
		}
		else
		{
			PAL_EndingSetEffectSprite(0);
			PAL_ShowFBP(Script->Operand[0], Script->Operand[1]);
		}*/
		break;

	case 0x0077:
		// Stop current playing music
		AudioManager->PlayMusic(0, false, (Script->Operand[0] == 0) ? 2.0f : (float)(Script->Operand[0]) * 3);
		GameStateData->MusicNum = 0;
		break;

	case 0x0078:
		// FIXME: ???
		break;

	case 0x0079:
		// Jump if the specified player is in the party
		for (UPALRoleData* PartyMember : PlayerStateData->Party)
		{
			if (PlayerStateData->PlayerRoles.Name[PartyMember->RoleId] == Script->Operand[0])
			{
				ScriptEntry = Script->Operand[1] - 1;
				break;
			}
		}
		break;

	case 0x007A:
		// Walk the party to the specified position, at a higher speed
		ScriptRunner = GetWorld()->SpawnActor<APALPartyWalkScriptRunner>();
		Cast<APALPartyWalkScriptRunner>(ScriptRunner)->Init(FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 4);
		break;

	case 0x007B:
		// Walk the party to the specified position, at the highest speed
		ScriptRunner = GetWorld()->SpawnActor<APALPartyWalkScriptRunner>();
		Cast<APALPartyWalkScriptRunner>(ScriptRunner)->Init(FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 8);
		break;

	case 0x007C:
		// Walk straight to the specified position
		if (!GameState->NPCWalkTo(EventObjectId, FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 2))
		{
			ScriptEntry--;
		}
		break;

	case 0x007D:
		// Move the event object
		Current->X += static_cast<int16>(Script->Operand[1]);
		Current->Y += static_cast<int16>(Script->Operand[2]);
		break;

	case 0x007E:
		// Set the layer of event object
		Current->Layer = static_cast<int16>(Script->Operand[1]);
		break;

	case 0x007F:
		// Move the viewport
	{
		if (Script->Operand[0] == 0 && Script->Operand[1] == 0 || Script->Operand[2] == 0)
		{
			// Move the viewport back to normal state
			ScenePlayerController->CameraRestoreNormal();
		}
		else if (Script->Operand[2] == 0xFFFF)
		{
			ScenePlayerController->CameraMoveTo(FPALPosition3d(Script->Operand[0] * 32, Script->Operand[1] * 32, 0));
		}
		else
		{
			int16 X = static_cast<int16>(Script->Operand[0]);
			int16 Y = static_cast<int16>(Script->Operand[1]) * 2;
			ScriptRunner = GetWorld()->SpawnActor<APALPanCameraScriptRunner>();
			const FPALPosition3d& Viewport = ScenePlayerController->GetViewport();
 			Cast<APALPanCameraScriptRunner>(ScriptRunner)->Init(
				FPALPosition3d(Viewport.X + X * Script->Operand[2], Viewport.Y + Y * Script->Operand[2], 0),
				Script->Operand[2] * FRAME_TIME);
			break;
		}
	}
		break;

	case 0x0080:
		// Toggle day/night palette
		check(SceneGameMode);
		SceneGameMode->SetDayNight(!GameStateData->bNightPalette);
		// TODO PAL_PaletteFade(gpGlobals->wNumPalette, GameStateData->bNightPalette, !(Script->Operand[0]));
		break;

	case 0x0081:
		// Jump if the player is not facing the specified event object
	{
		if (Script->Operand[0] <= GameStateData->Scenes[GameStateData->SceneNum - 1].EventObjectIndex 
			|| Script->Operand[0] > GameStateData->Scenes[GameStateData->SceneNum].EventObjectIndex)
		{
			// The event object is not in the current scene
			ScriptEntry = Script->Operand[2] - 1;
			bSuccess = false;
			break;
		}

		double X = Current->X + ((PlayerStateData->PartyDirection == EPALDirection::West || PlayerStateData->PartyDirection == EPALDirection::South) ? 16 : -16);
		double Y = Current->Y * 2 + ((PlayerStateData->PartyDirection == EPALDirection::West || PlayerStateData->PartyDirection == EPALDirection::North) ? 16 : -16);

		FPALPosition3d PartyPosition = ScenePlayerController->GetPartyPosition();

		if (FMath::Abs(X - PartyPosition.X) + FMath::Abs(Y - PartyPosition.Y) < Script->Operand[1] * 32 + 16 
			&& GameStateData->EventObjects[Script->Operand[0] - 1].State > 0)
		{
			if (Script->Operand[1] > 0)
			{
				// Change the trigger mode so that the object can be triggered in next frame
				Current->TriggerMode = ETriggerMode::TouchNormal + Script->Operand[1];
			}
		}
		else
		{
			ScriptEntry = Script->Operand[2] - 1;
			bSuccess = false;
		}
	}
	break;

	case 0x0082:
		// Walk straight to the specified position, at a high speed
		if (!GameState->NPCWalkTo(EventObjectId, FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 8))
		{
			ScriptEntry--;
		}
		break;

	case 0x0083:
		// Jump if event object is not in the specified zone of the current event object
		if (Script->Operand[0] <= GameStateData->Scenes[GameStateData->SceneNum - 1].EventObjectIndex ||
			Script->Operand[0] > GameStateData->Scenes[GameStateData->SceneNum].EventObjectIndex)
		{
			// The event object is not in the current scene
			ScriptEntry = Script->Operand[2] - 1;
			bSuccess = false;
			break;
		}

		if (FMath::Abs(EventObject->X - Current->X) + FMath::Abs((EventObject->Y - Current->Y) * 2) >= Script->Operand[1] * 32 + 16)
		{
			ScriptEntry = Script->Operand[2] - 1;
			bSuccess = false;
		}
		break;

	case 0x0084:
		// Place the item which player used as an event object to the scene
		if (Script->Operand[0] <= GameStateData->Scenes[GameStateData->SceneNum - 1].EventObjectIndex ||
			Script->Operand[0] > GameStateData->Scenes[GameStateData->SceneNum].EventObjectIndex)
		{
			// The event object is not in the current scene
			ScriptEntry = Script->Operand[2] - 1;
			bSuccess = false;
			break;
		}

		{
			FPALPosition3d Position = ScenePlayerController->GetPartyPosition();
			Position.X += ((PlayerStateData->PartyDirection == EPALDirection::West || PlayerStateData->PartyDirection == EPALDirection::South) ? -16 : 16);
			Position.Y += ((PlayerStateData->PartyDirection == EPALDirection::West || PlayerStateData->PartyDirection == EPALDirection::North) ? -16 : 16);
			if (GetWorld()->GetSubsystem<UPALMapManager>()->CheckObstacle(Position, false))
			{
				ScriptEntry = Script->Operand[2] - 1;
				bSuccess = false;
			}
			else
			{
				const FPALPosition2d& Position2d = Position.to2d();
				Current->X = Position2d.X;
				Current->Y = Position2d.Y;
				Current->State = static_cast<int16>(Script->Operand[1]);
			}
		}
		break;

	case 0x0085:
		// Delay for a period
		ScriptRunner = GetWorld()->SpawnActor<APALTimedWaitScriptRunner>();
		Cast<APALTimedWaitScriptRunner>(ScriptRunner)->Init(Script->Operand[0] * 0.08);
		break;

	case 0x0086:
		// Jump if the specified item is not equipped
	{
		bool bFound = false;
		for (UPALRoleData* PartyMember : PlayerStateData->Party)
		{
			SIZE_T RoleId = PartyMember->RoleId;
			for (SIZE_T x = 0; x < MAX_PLAYER_EQUIPMENTS; x++)
			{
				if (PlayerStateData->PlayerRoles.Equipments[x][RoleId] == Script->Operand[0])
				{
					bFound = true;
					break;
				}
			}
			if (bFound)
			{
				break;
			}
		}
		if (!bFound)
		{
			ScriptEntry = Script->Operand[2] - 1;
		}
	}
	break;

	case 0x0087:
		// Animate the event object
		GameState->NPCWalkOneStep(CurrentEventObjectId, 0);
		break;

	case 0x0088:
		// Set the base damage of magic according to amount of money
	{
		uint32 i = ((PlayerStateData->Cash > 5000) ? 5000 : PlayerStateData->Cash);
		PlayerStateData->Cash -= i;
		uint16 j = GameStateData->Objects[Script->Operand[0]].Magic.MagicNumber;
		GameData->Magics[j].BaseDamage = i * 2 / 5;
	}
	break;

	case 0x0089:
		//
		// Set the battle result
		//
		// TODO g_Battle.BattleResult = Script->Operand[0];
		break;

	case 0x008A:
		// Enable Auto-Battle for next battle
		PlayerStateData->bAutoBattle = true;
		break;

	case 0x008B:
		// change the current palette
		check(SceneGameMode);
		SceneGameMode->SetTone(Script->Operand[0]);
		break;

	case 0x008C:
		//
		// Fade from/to color
		//
		/*TODO PAL_ColorFade(Script->Operand[1], (BYTE)(Script->Operand[0]),
			Script->Operand[2]);
		gpGlobals->fNeedToFadeIn = FALSE;*/
		break;

	case 0x008D:
		// Increase player's level
		PlayerState->RoleLevelUp(EventObjectId, Script->Operand[0]);
		break;

	case 0x008F:
		// Halve the cash amount
		PlayerStateData->Cash /= 2;
		break;

	case 0x0090:
		// Set the object script
		GameStateData->Objects[Script->Operand[0]].Data[2 + Script->Operand[2]] = Script->Operand[1];
		break;

	case 0x0091:
		// Jump if the enemy is not first of same kind
	{/*TODO
		int self_pos = 0;
		int count = 0;
		if (GameStateData->bInBattle)
		{
			for (i = 0; i <= g_Battle.wMaxEnemyIndex; i++)
			{
				if (g_Battle.rgEnemy[i].wObjectID == g_Battle.rgEnemy[EventObjectId].wObjectID)
				{
					count++;
					if (i == EventObjectId)
						self_pos = count;
				}
			}
		}
		if (self_pos > 1)
			ScriptEntry = Script->Operand[0] - 1;*/
	}
	break;

	case 0x0092:
		//
		// Show a magic-casting animation for a player in battle
		//
		/*TODO
		if (gpGlobals->fInBattle)
		{
			if (Script->Operand[0] != 0)
			{
				PAL_BattleShowPlayerPreMagicAnim(Script->Operand[0] - 1, FALSE);
				g_Battle.rgPlayer[Script->Operand[0] - 1].wCurrentFrame = 6;
			}

			for (i = 0; i < 5; i++)
			{
				for (j = 0; j <= gpGlobals->wMaxPartyMemberIndex; j++)
				{
					g_Battle.rgPlayer[j].iColorShift = i * 2;
				}
				PAL_BattleDelay(1 * BATTLE_FRAME_TIME, 0, TRUE);
			}
			VIDEO_BackupScreen(g_Battle.lpSceneBuf);
			PAL_BattleUpdateFighters();
			PAL_BattleMakeScene();
			PAL_BattleFadeScene();
		}*/
		break;

	case 0x0093:
		//
		// Fade the screen. Update scene in the process.
		//
		/*TODO
		PAL_SceneFade(gpGlobals->wNumPalette, gpGlobals->fNightPalette,
			static_cast<int16>(Script->Operand[0]));
		gpGlobals->fNeedToFadeIn = (static_cast<int16>(Script->Operand[0]) < 0);*/
		break;

	case 0x0094:
		// Jump if the state of event object is the specified one
		if (Current->State == static_cast<int16>(Script->Operand[1]))
		{
			ScriptEntry = Script->Operand[2] - 1;
		}
		break;

	case 0x0095:
		// Jump if the current scene is the specified one
		if (GameStateData->SceneNum == Script->Operand[0])
		{
			ScriptEntry = Script->Operand[1] - 1;
		}
		break;

	case 0x0096:
		//
		// Show the ending animation
		//
		// TODO if (!gConfig.fIsWIN95) PAL_EndingAnimation();
		break;

	case 0x0097:
		// Ride the event object to the specified position, at a higher speed
		ScriptRunner = GetWorld()->SpawnActor<APALRideEventObjectScriptRunner>();
		Cast<APALRideEventObjectScriptRunner>(ScriptRunner)->Init(EventObjectId, FPALPosition3d(Script->Operand[0] * 32 + Script->Operand[2] * 16, Script->Operand[1] * 32 + Script->Operand[2] * 16, 0), 8);
		break;

	case 0x0098:
		// Set follower of the party
	{
		ScenePlayerController->RemoveAllFollowerRoles();
		for (SIZE_T i = 0; i < 2; i++)
		{
			if (Script->Operand[i] > 0)
			{
				ScenePlayerController->AddFollowerRole(Script->Operand[i]);
			}
		}
		check(SceneGameMode);
		SceneGameMode->LoadRoleSprites();
	}
	break;

	case 0x0099:
		// Change the map for the specified scene
		if (Script->Operand[0] == 0xFFFF)
		{
			GameStateData->Scenes[GameStateData->SceneNum - 1].MapNum = Script->Operand[1];
			check(SceneGameMode);
			SceneGameMode->LoadScene();
		}
		else
		{
			GameStateData->Scenes[Script->Operand[0] - 1].MapNum = Script->Operand[1];
		}
		break;

	case 0x009A:
		// Set the state for multiple event objects
		for (SIZE_T i = Script->Operand[0]; i <= Script->Operand[1]; i++)
		{
			GameStateData->EventObjects[i - 1].State = Script->Operand[2];
		}
		break;

	case 0x009B:
		//
		// Fade to the current scene
		// FIXME: This is obviously wrong
		//
		/*TODO VIDEO_BackupScreen(gpScreen);
		PAL_MakeScene();
		VIDEO_FadeScreen(2);*/
		break;

	case 0x009C:
		//
		// Enemy division itself
		//
		/*TODO
		w = 0;

		for (i = 0; i <= g_Battle.wMaxEnemyIndex; i++)
		{
			if (g_Battle.rgEnemy[i].wObjectID != 0)
			{
				w++;
			}
		}

		if (w != 1 || g_Battle.rgEnemy[CurrentEventObjectId].e.wHealth <= 1)
		{
			//
			// Division is only possible when only 1 enemy left
			// health too low also cannot division
			//
			if (Script->Operand[1] != 0)
			{
				ScriptEntry = Script->Operand[1] - 1;
			}
			break;
		}

		w = Script->Operand[0];
		if (w == 0)
		{
			w = 1;
		}
		x = w + 1;
		y = w;

		//division does not limited by original team layout
		for (i = 0; i < MAX_ENEMIES_IN_TEAM; i++)
		{
			if (w > 0 && g_Battle.rgEnemy[i].wObjectID == 0)
			{
				w--;

				//notice: MAX MAY VARYING IN DIVISION!
				memset(&(g_Battle.rgEnemy[i]), 0, sizeof(BATTLEENEMY));

				g_Battle.rgEnemy[i].wObjectID = g_Battle.rgEnemy[EventObjectId].wObjectID;
				g_Battle.rgEnemy[i].e = g_Battle.rgEnemy[EventObjectId].e;
				g_Battle.rgEnemy[i].e.wHealth = (g_Battle.rgEnemy[EventObjectId].e.wHealth + y) / x;
				g_Battle.rgEnemy[i].ScriptOnTurnStart = g_Battle.rgEnemy[EventObjectId].ScriptOnTurnStart;
				g_Battle.rgEnemy[i].ScriptOnBattleEnd = g_Battle.rgEnemy[EventObjectId].ScriptOnBattleEnd;
				g_Battle.rgEnemy[i].ScriptOnReady = g_Battle.rgEnemy[EventObjectId].ScriptOnReady;

				g_Battle.rgEnemy[i].state = kFighterWait;
				g_Battle.rgEnemy[i].flTimeMeter = 50;
				g_Battle.rgEnemy[i].iColorShift = 0;

			}
		}
		g_Battle.rgEnemy[CurrentEventObjectId].e.wHealth = (g_Battle.rgEnemy[EventObjectId].e.wHealth + y) / x;

		w = 0;
		for (i = 0; i < MAX_ENEMIES_IN_TEAM; i++)
			if (g_Battle.rgEnemy[i].wObjectID != 0)
				w = i;
		g_Battle.wMaxEnemyIndex = w;

		PAL_LoadBattleSprites();

		for (i = 0; i <= g_Battle.wMaxEnemyIndex; i++)
		{
			if (g_Battle.rgEnemy[i].wObjectID == 0)
			{
				continue;
			}
			g_Battle.rgEnemy[i].pos = g_Battle.rgEnemy[EventObjectId].pos;
		}

		for (i = 0; i < 10; i++)
		{
			for (j = 0; j <= g_Battle.wMaxEnemyIndex; j++)
			{
				x = (PAL_X(g_Battle.rgEnemy[j].pos) + PAL_X(g_Battle.rgEnemy[j].posOriginal)) / 2;
				y = (PAL_Y(g_Battle.rgEnemy[j].pos) + PAL_Y(g_Battle.rgEnemy[j].posOriginal)) / 2;

				g_Battle.rgEnemy[j].pos = PAL_XY(x, y);
			}

			PAL_BattleDelay(1 * BATTLE_FRAME_TIME, 0, TRUE);
		}

		PAL_BattleUpdateFighters();
		PAL_BattleDelay(1 * BATTLE_FRAME_TIME, 0, TRUE);*/
		break;

	case 0x009E:
		//
		// Enemy summons another monster
		//
		/*TODO
		for (i = 0; i < g_Battle.rgEnemy[EventObjectId].e.wMagicFrames; i++)
		{
			g_Battle.rgEnemy[EventObjectId].wCurrentFrame =
				g_Battle.rgEnemy[EventObjectId].e.wIdleFrames + i;
			PAL_BattleDelay(g_Battle.rgEnemy[EventObjectId].e.wActWaitFrames * BATTLE_FRAME_TIME, 0, FALSE);
		}

		x = 0;
		w = Script->Operand[0];
		y = ((static_cast<int16>(Script->Operand[1]) <= 0) ? 1 : (SHORT)Script->Operand[1]);

		if (w == 0 || w == 0xFFFF)
		{
			w = g_Battle.rgEnemy[EventObjectId].wObjectID;
		}

		for (i = 0; i <= g_Battle.wMaxEnemyIndex; i++)
		{
			if (g_Battle.rgEnemy[i].wObjectID == 0)
			{
				x++;
			}
		}

		if (x < y || g_Battle.iHidingTime > 0 ||
			g_Battle.rgEnemy[EventObjectId].rgwStatus[kStatusSleep] != 0 ||
			g_Battle.rgEnemy[EventObjectId].rgwStatus[kStatusParalyzed] != 0 ||
			g_Battle.rgEnemy[EventObjectId].rgwStatus[kStatusConfused] != 0)
		{
			if (Script->Operand[2] != 0)
			{
				ScriptEntry = Script->Operand[2] - 1;
			}
		}
		else
		{
			for (i = 0; i <= g_Battle.wMaxEnemyIndex; i++)
			{
				if (g_Battle.rgEnemy[i].wObjectID == 0)
				{
					memset(&(g_Battle.rgEnemy[i]), 0, sizeof(BATTLEENEMY));

					g_Battle.rgEnemy[i].wObjectID = w;
					g_Battle.rgEnemy[i].e = gpGlobals->g.lprgEnemy[gpGlobals->g.rgObject[w].enemy.wEnemyID];

					g_Battle.rgEnemy[i].state = kFighterWait;
					g_Battle.rgEnemy[i].ScriptOnTurnStart = gpGlobals->g.rgObject[w].enemy.ScriptOnTurnStart;
					g_Battle.rgEnemy[i].ScriptOnBattleEnd = gpGlobals->g.rgObject[w].enemy.ScriptOnBattleEnd;
					g_Battle.rgEnemy[i].ScriptOnReady = gpGlobals->g.rgObject[w].enemy.ScriptOnReady;
					g_Battle.rgEnemy[i].flTimeMeter = 50;
					g_Battle.rgEnemy[i].iColorShift = 8;

					y--;
					if (y <= 0)
					{
						break;
					}
				}
			}

			VIDEO_BackupScreen(g_Battle.lpSceneBuf);
			PAL_LoadBattleSprites();
			PAL_BattleMakeScene();
			AUDIO_PlaySound(212);
			PAL_BattleFadeScene();

			// avoid releasing gesture disappears before summon done
			PAL_BattleDelay(2 * BATTLE_FRAME_TIME, 0, TRUE);

			for (i = 0; i <= g_Battle.wMaxEnemyIndex; i++)
			{
				g_Battle.rgEnemy[i].iColorShift = 0;
			}

			VIDEO_BackupScreen(g_Battle.lpSceneBuf);
			PAL_BattleMakeScene();
			PAL_BattleFadeScene();
		}*/
		break;

	case 0x009F:
		//
		// Enemy transforms into something else
		//
		/*TODO
		if (g_Battle.iHidingTime <= 0 &&
			g_Battle.rgEnemy[EventObjectId].rgwStatus[kStatusSleep] == 0 &&
			g_Battle.rgEnemy[EventObjectId].rgwStatus[kStatusParalyzed] == 0 &&
			g_Battle.rgEnemy[EventObjectId].rgwStatus[kStatusConfused] == 0)
		{
			w = g_Battle.rgEnemy[EventObjectId].e.wHealth;

			g_Battle.rgEnemy[EventObjectId].wObjectID = Script->Operand[0];
			g_Battle.rgEnemy[EventObjectId].e =
				gpGlobals->g.lprgEnemy[gpGlobals->g.rgObject[Script->Operand[0]].enemy.wEnemyID];

			g_Battle.rgEnemy[EventObjectId].e.wHealth = w;
			g_Battle.rgEnemy[EventObjectId].wCurrentFrame = 0;

			for (i = 0; i < 6; i++)
			{
				g_Battle.rgEnemy[EventObjectId].iColorShift = i;
				PAL_BattleDelay(1 * BATTLE_FRAME_TIME, 0, FALSE);
			}

			g_Battle.rgEnemy[EventObjectId].iColorShift = 0;

			AUDIO_PlaySound(47);
			VIDEO_BackupScreen(g_Battle.lpSceneBuf);
			PAL_LoadBattleSprites();
			PAL_BattleMakeScene();
			PAL_BattleFadeScene();
		}*/
		break;

	case 0x00A0:
		//
		// Quit game
		//
		/*TODO
		if (gConfig.fIsWIN95)
			PAL_EndingScreen();
		PAL_AdditionalCredits();
		PAL_Shutdown(0);*/
		break;

	case 0x00A1:
		// Set the positions of all party members to the same as the first one
		ScenePlayerController->PartyMoveClose();
		break;

	case 0x00A2:
		// Jump to one of the following instructions randomly
		ScriptEntry += FMath::RandRange(0, Script->Operand[0] - 1);
		break;

	case 0x00A3:
		// Play CD music. Use the RIX music for fallback.
	{
		GameStateData->MusicNum = Script->Operand[1];
		int32 TrackNum = static_cast<int16>(Script->Operand[0]);
		if (!AudioManager->PlayCDTrack(TrackNum == -1 ? -2 : TrackNum))
		{
			AudioManager->PlayMusic(Script->Operand[1], true, 0);
		}
	}
	break;

	case 0x00A4:
		//
		// Scroll FBP to the screen
		//
		/*TODO if (!gConfig.fIsWIN95)
		{
			if (Script->Operand[0] == 68)
			{
				//
				// HACKHACK: to make the ending picture show correctly
				//
				PAL_ShowFBP(69, 0);
			}
			PAL_ScrollFBP(Script->Operand[0], Script->Operand[2], TRUE);
		}*/
		break;

	case 0x00A5:
		//
		// Show FBP picture with sprite effects
		//
		/*TODO if (!gConfig.fIsWIN95)
		{
			if (Script->Operand[1] != 0xFFFF)
			{
				PAL_EndingSetEffectSprite(Script->Operand[1]);
			}
			PAL_ShowFBP(Script->Operand[0], Script->Operand[2]);
		}*/
		break;

	case 0x00A6:
		//
		// backup screen
		//
		// VIDEO_BackupScreen(gpScreen);
		break;

	default:
		UE_LOG(LogScript, Warning, TEXT("SCRIPT: Invalid Instruction at %4x: (%4x - %4x, %4x, %4x)"),
			ScriptEntry, Script->Operation, Script->Operand[0], Script->Operand[1], Script->Operand[2]);
		break;
	}

	if (bRewriteScriptEntry)
	{
		InOutScriptEntry = ScriptEntry + 1;
	}

	bOutSuccess = bSuccess;
	return ScriptRunner;
}

