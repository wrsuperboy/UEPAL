// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleGameMode.h"
#include "EngineUtils.h"
#include "LandscapeProxy.h"
#include "LandscapeComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Kismet/GameplayStatics.h"
#include "PALGameState.h"
#include "PALAudioManager.h"
#include "PALGameInstance.h"
#include "PALScriptManager.h"
#include "PALBattleRoleActor.h"
#include "PALBattleCameraActor.h"
#include "PALBattleGroundActor.h"
#include "PALCommon.h"
#include "PALBattle.h"

const FPALPosition3d RolePosition[3][3] =
{
   {FPALPosition3d(80, 140, 0)}, // one player
   {FPALPosition3d(40, 152, 0), FPALPosition3d(96, 104, 0)}, // two players
   {FPALPosition3d(20, 160, 0), FPALPosition3d(74, 140, 0), FPALPosition3d(110, 92, 0)} // three players
};

APALBattleGameMode::APALBattleGameMode() : Super()
{
	PlayerControllerClass = APALBattlePlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	PrimaryActorTick.bCanEverTick = true;
	RolesPreviousHP.SetNumUninitialized(MAX_PLAYER_ROLES);
	RolesPreviousMP.SetNumUninitialized(MAX_PLAYER_ROLES);
}

bool APALBattleGameMode::IsEnemyCleared() const
{
	return bEnemyCleared;
}

// Local test
void APALBattleGameMode::ClearEnemies()
{
	bEnemyCleared = true;
}

void APALBattleGameMode::LoadBattleActors()
{
	// Load battle actors for roles
	SIZE_T PartyCount = MainPlayerStatePrivate->GetPlayerStateData()->Party.Num();
	for (SIZE_T i = 0; i < PartyCount; i++)
	{
		const FPALPosition3d& Position = RolePosition[PartyCount - 1][i];
		APALBattleRoleActor* RoleActor = GetWorld()->SpawnActor<APALBattleRoleActor>();
		RoleActor->Init(MainPlayerStatePrivate->GetPlayerStateData()->Party[i], MainPlayerStatePrivate, Position);
		FBattleRole BattleRole;
		BattleRole.State = EPALFighterState::Waiting;
		BattleRole.TimeMeter = 0;
		BattleRole.bDefending = false;
		BattleRoleMap.Add(MainPlayerStatePrivate->GetPlayerStateData()->Party[i]->RoleId, BattleRole);
	}

	// Load battle actors for enemies
	UPALGameData* GameData = GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = Cast<APALGameState>(GameState)->GetGameStateData();
	SIZE_T EnemyCount = MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies.Num();
	EnemiesPreviousHP.SetNumUninitialized(EnemyCount);
	EnemyActors.SetNumUninitialized(EnemyCount);
	EnemiesFighterState.SetNumUninitialized(EnemyCount);
	for (SIZE_T i = 0; i < EnemyCount; i++)
	{
		UPALBattleEnemyData* EnemyData = MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i];
		double X = GameData->EnemyPos.Pos[i][EnemyCount - 1].X - 320 / 2;
		double Y = GameData->EnemyPos.Pos[i][EnemyCount - 1].Y - 200 / 2;
		FPALPosition3d Position(X, Y * 2, 0);
		APALBattleEnemyActor* EnemyActor = GetWorld()->SpawnActor<APALBattleEnemyActor>();
		EnemyActor->Init(EnemyData, GameStateData->Objects[EnemyData->GetObjectId()].Enemy.EnemyID, Position);
		EnemyActors[i] = EnemyActor;
		EnemiesFighterState[i] = EPALFighterState::Waiting;
	}
}

void APALBattleGameMode::LoadBattleBackground()
{
	UTexture2D* Texture = GetGameInstance()->GetSubsystem<UPALCommon>()->GetBackgroundPicture(MainPlayerStatePrivate->GetPlayerStateData()->BattleFieldNum);
	//APALBattleGroundActor* BattleGoundActor = GetWorld()->SpawnActor<APALBattleGroundActor>();
	//BattleGoundActor->SetGroundTexture(Texture);

	for (TActorIterator<ALandscapeProxy> It(GetWorld(), ALandscapeProxy::StaticClass()); It; ++It)
	{
		ALandscapeProxy* Landscape = *It;
		if (Landscape)
		{
			static FName TextureParameterName("PAL_Texture2D");
			static FName TextureSizeXParameterName("PAL_Texture_Size_X");
			static FName TextureSizeYParameterName("PAL_Texture_Size_Y");
			Landscape->SetLandscapeMaterialTextureParameterValue(TextureParameterName, Texture);
			Landscape->SetLandscapeMaterialScalarParameterValue(TextureSizeXParameterName, Texture->GetSizeX() * PIXEL_TO_UNIT);
			Landscape->SetLandscapeMaterialScalarParameterValue(TextureSizeYParameterName, Texture->GetSizeY() * 2 * PIXEL_TO_UNIT);
		}
	}
}

void APALBattleGameMode::BattleSettle()
{
	UPALPlayerStateData* PlayerStateData = MainPlayerStatePrivate->GetPlayerStateData();
	if (Result == EPALBattleResult::BattleResultWon)
	{
		// BattleWon();
	}
	PlayerStateData->LastBattleResult = Result;
	UGameplayStatics::OpenLevel(this, TEXT("PAL_Scene"));
}

void APALBattleGameMode::BattleDelay(float Duration, bool bUpdateGesture)
{
	DelayTime += Duration;
	for (APALBattleEnemyActor* EnemyActor : EnemyActors)
	{
		if (bUpdateGesture)
		{
			EnemyActor->ResumeGuesture();
		}
		else
		{
			EnemyActor->StopGuesture();
		}
	}
}

void APALBattleGameMode::BattleBackupStat()
{
	SIZE_T EnemyCount = MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies.Num();
	for (SIZE_T i = 0; i < EnemyCount; i++)
	{
		UPALBattleEnemyData* EnemyData = MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i];
		if (!EnemyData->IsKnockedOuted())
		{
			EnemiesPreviousHP[i] = EnemyData->Enemy.Health;
		}
	}

	UPALPlayerStateData* PlayerStateData = MainPlayerStatePrivate->GetPlayerStateData();
	for (UPALRoleData* RoleData : PlayerStateData->Party)
	{
		const SIZE_T RoleId = RoleData->RoleId;
		RolesPreviousHP[RoleId] = PlayerStateData->PlayerRoles.HP[RoleId];
		RolesPreviousMP[RoleId] = PlayerStateData->PlayerRoles.MP[RoleId];
	}
}

void APALBattleGameMode::BattlePostActionCheck(bool bCheckRoles)
{
	bool bEnemyRemaining = false;
	bool bFade = false;
	UPALPlayerStateData* PlayerStateData = MainPlayerStatePrivate->GetPlayerStateData();
	SIZE_T EnemyCount = PlayerStateData->CurrentEnemies.Num();
	for (SIZE_T i = 0; i < EnemyCount; i++)
	{
		UPALBattleEnemyData* EnemyData = PlayerStateData->CurrentEnemies[i];
		if (EnemyData->IsKnockedOuted())
		{
			continue;
		}

		if (EnemyData->Enemy.Health <= 0)
		{
			// This enemy is KO'ed
			PlayerStateData->ExpGainedInBattle += EnemyData->Enemy.Exp;
			PlayerStateData->CashGainedInBattle += EnemyData->Enemy.Cash;

			GetWorld()->GetSubsystem<UPALAudioManager>()->PlaySound(EnemyData->Enemy.DeathSound);
			EnemyData->KnockOut();
			bFade = true;

			continue;
		}

		bEnemyRemaining = true;
	}

	if (!bEnemyRemaining)
	{
		bEnemyCleared = true;
		MainPlayerControllerPrivate->BattleUIWait();
	}

	if (bCheckRoles && !PlayerStateData->bAutoBattle)
	{
		for (UPALRoleData* RoleData : PlayerStateData->Party)
		{
			const SIZE_T RoleId = RoleData->RoleId;
			if (PlayerStateData->PlayerRoles.HP[RoleId] < RolesPreviousHP[RoleId] &&
				PlayerStateData->PlayerRoles.HP[RoleId] == 0)
			{
				const SIZE_T CoveringRoleId = PlayerStateData->PlayerRoles.CoveredBy[RoleId];

				for (UPALRoleData* RoleDataToFind : PlayerStateData->Party)
				{
					if (RoleDataToFind->RoleId == CoveringRoleId)
					{
						if (PlayerStateData->PlayerRoles.HP[CoveringRoleId] > 0 &&
							PlayerStateData->RoleStatus[CoveringRoleId][EPALStatus::Sleep] == 0 &&
							PlayerStateData->RoleStatus[CoveringRoleId][EPALStatus::Paralyzed] == 0 &&
							PlayerStateData->RoleStatus[CoveringRoleId][EPALStatus::Confused] == 0
							)
						{
							const uint16 CoveringRoleNameIndex = PlayerStateData->PlayerRoles.Name[CoveringRoleId];
							UPALGameStateData* GameStateData = Cast<APALGameState>(GameState)->GetGameStateData();
							if (GameStateData->Objects[CoveringRoleNameIndex].Player.ScriptOnFriendDeath != 0)
							{
								BattleDelay(10 * BATTLE_FRAME_TIME, true);

								Result = EPALBattleResult::BattleResultPause;

								GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[CoveringRoleNameIndex].Player.ScriptOnFriendDeath, CoveringRoleNameIndex, true);

								Result = EPALBattleResult::BattleResultOnGoing;
								return;
							}
						}
						break;
					}
				}
			}
		}

		for (UPALRoleData* RoleData : PlayerStateData->Party)
		{
			const SIZE_T RoleId = RoleData->RoleId;

			if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Sleep] != 0 ||
				PlayerStateData->RoleStatus[RoleId][EPALStatus::Confused] != 0)
			{
				continue;
			}

			if (PlayerStateData->PlayerRoles.HP[RoleId] < RolesPreviousHP[RoleId])
			{
				if (PlayerStateData->PlayerRoles.HP[RoleId] > 0 && MainPlayerStatePrivate->IsRoleDying(RoleId) &&
					RolesPreviousHP[RoleId] >= PlayerStateData->PlayerRoles.MaxHP[RoleId] / 5)
				{
					const SIZE_T CoveringRoleId = PlayerStateData->PlayerRoles.CoveredBy[RoleId];

					if (PlayerStateData->RoleStatus[CoveringRoleId][EPALStatus::Sleep] != 0 ||
						PlayerStateData->RoleStatus[CoveringRoleId][EPALStatus::Paralyzed] != 0 ||
						PlayerStateData->RoleStatus[CoveringRoleId][EPALStatus::Confused] != 0)
					{
						continue;
					}

					const uint16 RoleNameIndex = PlayerStateData->PlayerRoles.Name[RoleId];

					GetWorld()->GetSubsystem<UPALAudioManager>()->PlaySound(PlayerStateData->PlayerRoles.DyingSound[RoleId]);

					for (UPALRoleData* RoleDataToFind : PlayerStateData->Party)
					{
						if (RoleDataToFind->RoleId == CoveringRoleId)
						{
							if (PlayerStateData->PlayerRoles.HP[CoveringRoleId] == 0)
							{
								continue;
							}

							UPALGameStateData* GameStateData = Cast<APALGameState>(GameState)->GetGameStateData();
							if (GameStateData->Objects[RoleNameIndex].Player.ScriptOnDying != 0)
							{
								BattleDelay(10 * BATTLE_FRAME_TIME, true);

								Result = EPALBattleResult::BattleResultPause;

								GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[RoleNameIndex].Player.ScriptOnDying, RoleId, true);

								Result = EPALBattleResult::BattleResultOnGoing;
							}
							break;
						}
					}
				}
			}
		}
	}
}

void APALBattleGameMode::BattleEnemyPerformAction(SIZE_T EnemyIndex) {
	BattleBackupStat();

	UPALPlayerStateData* PlayerStateData = MainPlayerStatePrivate->GetPlayerStateData();
	UPALBattleEnemyData* CurrentEnemyData = PlayerStateData->CurrentEnemies[EnemyIndex];
	uint16 Magic = CurrentEnemyData->Enemy.Magic;

	if (CurrentEnemyData->Status[EPALStatus::Sleep] > 0 ||
		CurrentEnemyData->Status[EPALStatus::Paralyzed] > 0 ||
		HidingTime > 0)
	{
		// Do nothing
		return;
	}

	if (CurrentEnemyData->Status[EPALStatus::Confused] > 0)
	{
		SIZE_T TargetEnemyIndex = FMath::RandRange(0, PlayerStateData->CurrentEnemies.Num() - 1);
		while (PlayerStateData->CurrentEnemies[TargetEnemyIndex]->IsKnockedOuted())
		{
			TargetEnemyIndex = FMath::RandRange(0, PlayerStateData->CurrentEnemies.Num() - 1);
		}
		if (TargetEnemyIndex == EnemyIndex)
		{
			return;
		}
		EnemyActors[EnemyIndex]->ConfusedToAttack(EnemyActors[TargetEnemyIndex]);
		BattleDelay(11 * BATTLE_FRAME_TIME, true);
		return;
	}

	const TArray<UPALRoleData*>& AliveParty =
		PlayerStateData->Party.FilterByPredicate([PlayerStateData](const UPALRoleData* PartyMember) {
			return PlayerStateData->PlayerRoles.HP[PartyMember->RoleId] != 0;
		});
	UPALRoleData* TargetRoleData = AliveParty[FMath::RandRange(0, AliveParty.Num() - 1)];
	SIZE_T TargetRoleId = TargetRoleData->RoleId;
}

void APALBattleGameMode::BattleCommitAction(bool bRepeat) {

}

void APALBattleGameMode::BattleRolePerformAction(SIZE_T RoleId) {

}

void APALBattleGameMode::BattleRoleCheckReady()
{
	float Max = 0;
	SIZE_T MaxRoleId = 0;

	// Start the UI for the fastest and ready player
	UPALPlayerStateData* PlayerStateData = MainPlayerStatePrivate->GetPlayerStateData();
	for (UPALRoleData* RoleData : PlayerStateData->Party)
	{
		FBattleRole& BattleRole = *BattleRoleMap.Find(RoleData->RoleId);
		if (BattleRole.State == EPALFighterState::AcceptingCommand ||
			(BattleRole.State == EPALFighterState::DoingMove && BattleRole.Action.ActionType == EBattleActionType::BattleActionCoopMagic))
		{
			Max = 0;
			break;
		}
		else if (BattleRole.State == EPALFighterState::Waiting)
		{
			if (BattleRole.TimeMeter > Max)
			{
				MaxRoleId = RoleData->RoleId;
				Max = BattleRole.TimeMeter;
			}
		}
	}

	if (Max >= 100.0f)
	{
		BattleRoleMap.Find(MaxRoleId)->State = EPALFighterState::AcceptingCommand;
		BattleRoleMap.Find(MaxRoleId)->bDefending = false;
	}
}

void APALBattleGameMode::DrawScreenMessage()
{
	//TODO	
	//if (EventObjectId != 0)
	//{
	//	if (EventObjectId == 31) // HACKHACK: BATTLE_LABEL_ESCAPEFAIL
	//	{
	//		PAL_DrawText(PAL_GetWord(wObjectID), PAL_XY(130, 75), 15, TRUE, FALSE, FALSE);
	//	}
	//	else if ((int16)EventObjectId < 0)
	//	{
	//		PAL_DrawText(PAL_GetWord(-((int16)EventObjectId)), PAL_XY(170, 45), DESCTEXT_COLOR, TRUE, FALSE, FALSE);
	//	}
	//	else
	//	{
	//		PAL_DrawText(PAL_GetWord(wObjectID), PAL_XY(210, 50), 15, TRUE, FALSE, FALSE);
	//	}
	//}
}

void APALBattleGameMode::StartPlay()
{
	UPlayer* MainPlayer = GetGameInstance<UPALGameInstance>()->GetMainPlayer();
	check(MainPlayer);
	MainPlayerControllerPrivate = Cast<APALBattlePlayerController>(MainPlayer->GetPlayerController(nullptr));
	check(MainPlayerControllerPrivate);
	MainPlayerStatePrivate = MainPlayerControllerPrivate->GetPlayerState<APALPlayerState>();

	LoadBattleActors();
	//LoadBattleBackground();
	bBattleGroundInitialized = false;
	MainPlayerControllerPrivate->SetViewTarget(GetWorld()->SpawnActor<APALBattleCameraActor>());
	DelayTime = 0;

	MainPlayerStatePrivate->GetPlayerStateData()->bInBattle = true;
	Result = EPALBattleResult::BattleResultPreBattle;
	bEnemyCleared = false;
	bEnemyMoving = false;
	HidingTime = 0;

	MainPlayerControllerPrivate->Init();

	Phase = EPALBattlePhase::SelectAction;
	bPreviousTurnAutoAttack = false;
	bPreviousRoleAutoAttack = false;

	GetWorld()->GetSubsystem<UPALAudioManager>()->PlayMusic(0, false, 1);

	GetWorld()->GetSubsystem<UPALAudioManager>()->PlayMusic(MainPlayerStatePrivate->GetPlayerStateData()->BattleMusicNum, true, 0);

	// Run the pre-battle scripts for each enemies
	for (SIZE_T i = 0; i < MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies.Num(); i++)
	{
		UPALBattleEnemyData* EnemyData = MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i];
		GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(EnemyData->ScriptOnTurnStart, i, true);

		if (Result != EPALBattleResult::BattleResultPreBattle)
		{
			break;
		}
	}

	if (Result == EPALBattleResult::BattleResultPreBattle)
	{
		Result = EPALBattleResult::BattleResultOnGoing;
	}

	GameState->HandleBeginPlay();
}

void APALBattleGameMode::Tick(float DeltaTime)
{
	if (!bBattleGroundInitialized)
	{
		bBattleGroundInitialized = true;
		LoadBattleBackground();
	}
	if (Result != EPALBattleResult::BattleResultOnGoing)
	{
		SetActorTickEnabled(false);
		BattleSettle();
		return;
	}

	if (bEnemyCleared)
	{
		Result = EPALBattleResult::BattleResultWon;
		GetWorld()->GetSubsystem<UPALAudioManager>()->PlaySound(0);
		return;
	}

	if (!FMath::IsNearlyZero(DelayTime))
	{
		if (DelayTime < DeltaTime)
		{
			DelayTime = 0;
			for (APALBattleEnemyActor* EnemyActor : EnemyActors)
			{
				EnemyActor->ResumeGuesture();
			}
		}
		else
		{
			DelayTime -= DeltaTime;
			return;
		}
	}

	bool bEnded = true;
	bool bOnlyPuppet = true;
	UPALPlayerStateData* PlayerStateData = MainPlayerStatePrivate->GetPlayerStateData();
	for (UPALRoleData* RoleData : MainPlayerStatePrivate->GetPlayerStateData()->Party)
	{
		uint16 RoleId = RoleData->RoleId;
		if (PlayerStateData->PlayerRoles.HP[RoleId] != 0)
		{
			bOnlyPuppet = false;
			bEnded = false;
			break;
		}
		else if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Puppet] != 0)
		{
			bOnlyPuppet = false;
		}
	}

	if (bEnded)
	{
		// All roles are dead. Lost the battle.
		Result = EPALBattleResult::BattleResultLost;
		return;
	}

	if (Phase == EPALBattlePhase::SelectAction)
	{
		if (MainPlayerControllerPrivate->GetUIStatus() == EPALBattleUIStatus::BattleUIWait)
		{
			bool bAllSelected = true;
			for (UPALRoleData* RoleData : PlayerStateData->Party)
			{
				uint16 RoleId = RoleData->RoleId;
				// Don't select action for this role if it is KO'ed,
				// sleeped, confused or paralyzed
				if (PlayerStateData->PlayerRoles.HP[RoleId] == 0 ||
					PlayerStateData->RoleStatus[RoleId][EPALStatus::Sleep] ||
					PlayerStateData->RoleStatus[RoleId][EPALStatus::Confused] ||
					PlayerStateData->RoleStatus[RoleId][EPALStatus::Paralyzed])
				{
					continue;
				}

				// Start the menu for the first player whose action is not
				// yet selected
				if (BattleRoleMap.Find(RoleId)->State == EPALFighterState::Waiting)
				{
					CurrentMovingRoleId = RoleId;
					BattleRoleMap.Find(RoleId)->State = EPALFighterState::AcceptingCommand;
					MainPlayerControllerPrivate->BattleUIRoleReady(RoleId);
					bAllSelected = false;
					break;
				}
				else if (BattleRoleMap.Find(RoleId)->Action.ActionType == EBattleActionType::BattleActionCoopMagic)
				{
					// Skip other players if someone selected coopmagic
					break;
				}
			}

			if (bAllSelected)
			{
				// Backup all actions once not repeating.
				if (MainPlayerControllerPrivate->IsRepeat())
				{
					for (TPair<SIZE_T, FBattleRole>& entry : BattleRoleMap)
					{
						entry.Value.PreviousAction = entry.Value.Action;
					}
				}

				// actions for all roles are decided. fill in the action queue.
				MainPlayerControllerPrivate->ClearTeamAction();
				bPreviousTurnAutoAttack = MainPlayerControllerPrivate->IsAutoAttack();
				bPreviousRoleAutoAttack = false;

				ActionQueue.Empty();

				for (SIZE_T i = 0; i < PlayerStateData->CurrentEnemies.Num(); i++)
				{
					FBattleActionQueueItem Action;
					Action.bIsEnemy = true;
					Action.Index = i;
					Action.bIsSecond = false;
					Action.Dexterity = PlayerStateData->CurrentEnemies[i]->GetDexterity();
					Action.Dexterity = static_cast<int16>(Action.Dexterity * FMath::RandRange(0.9, 1.1));

					if (PlayerStateData->CurrentEnemies[i]->Enemy.DualMove)
					{
						FBattleActionQueueItem Action2;
						Action2.bIsEnemy = true;
						Action2.Index = i;
						Action2.bIsSecond = false;
						Action2.Dexterity = PlayerStateData->CurrentEnemies[i]->GetDexterity();
						Action2.Dexterity = static_cast<int16>(Action2.Dexterity * FMath::RandRange(0.9, 1.1));

						if (Action.Dexterity <= Action2.Dexterity)
						{
							Action.bIsSecond = true;
						}
						else
						{
							Action2.bIsSecond = true;
						}
						ActionQueue.Add(Action2);
					}

					ActionQueue.Add(Action);
				}

				// Put all roles into action queue
				for (SIZE_T i = 0; i < MainPlayerStatePrivate->GetPlayerStateData()->Party.Num(); i++)
				{
					SIZE_T RoleId = MainPlayerStatePrivate->GetPlayerStateData()->Party[i]->RoleId;

					FBattleActionQueueItem Action;
					Action.bIsEnemy = false;
					Action.Index = i;

					if (PlayerStateData->PlayerRoles.HP[RoleId] == 0 ||
						PlayerStateData->RoleStatus[RoleId][EPALStatus::Sleep] > 0 ||
						PlayerStateData->RoleStatus[RoleId][EPALStatus::Paralyzed] > 0)
					{
						// players who are unable to move should attack physically if recovered
						// in the same turn
						Action.Dexterity = 0;
						BattleRoleMap.Find(RoleId)->Action.ActionType = EBattleActionType::BattleActionAttack;
						BattleRoleMap.Find(RoleId)->Action.ItemOrMagicId = 0;
						BattleRoleMap.Find(RoleId)->State = EPALFighterState::DoingMove;
					}
					else
					{
						uint16 Dexterity = MainPlayerStatePrivate->GetRoleActualDexterity(RoleId);

						if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Confused] > 0)
						{
							BattleRoleMap.Find(RoleId)->Action.ActionType = EBattleActionType::BattleActionAttack;
							BattleRoleMap.Find(RoleId)->Action.ItemOrMagicId = 0; //avoid be deduced to autoattack
							BattleRoleMap.Find(RoleId)->State = EPALFighterState::DoingMove;
						}

						UPALGameStateData* GameStateData = Cast<APALGameState>(GameState)->GetGameStateData();
						switch (BattleRoleMap.Find(RoleId)->Action.ActionType)
						{
						case EBattleActionType::BattleActionCoopMagic:
							Dexterity *= 10;
							break;

						case EBattleActionType::BattleActionDefend:
							Dexterity *= 5;
							break;

						case EBattleActionType::BattleActionMagic:
							if ((GameStateData->Objects[BattleRoleMap.Find(RoleId)->Action.ItemOrMagicId].Magic.Flags & EMagicFlag::MagicFlagUsableToEnemy) == 0)
							{
								Dexterity *= 3;
							}
							break;

						case EBattleActionType::BattleActionFlee:
							Dexterity /= 2;
							break;

						case EBattleActionType::BattleActionUseItem:
							Dexterity *= 3;
							break;

						default:
							break;
						}

						if (MainPlayerStatePrivate->IsRoleDying(RoleId))
						{
							Dexterity /= 2;
						}

						Dexterity *= static_cast<int16>(Dexterity * FMath::RandRange(0.9, 1.1));

						Action.Dexterity = Dexterity;
					}

					ActionQueue.Add(Action);
				}


				// Sort the action queue by dexterity value
				ActionQueue.Sort([](const FBattleActionQueueItem& A, const FBattleActionQueueItem& B) {
					return A.Dexterity < B.Dexterity;
					});
				Phase = EPALBattlePhase::PerformAction;
			}
		}
	}
	else
	{
		// Are all actions finished?
		if (ActionQueue.IsEmpty())
		{
			for (TActorIterator<APALBattleRoleActor> It(GetWorld(), APALBattleRoleActor::StaticClass()); It; ++It)
			{
				APALBattleRoleActor* RoleActor = *It;
				if (RoleActor)
				{
					RoleActor->SetDefending(false);
					// Restore player pos from MANUAL defending
					RoleActor->RestorePosition();
				}
			}

			// Run poison scripts
			BattleBackupStat();

			for (SIZE_T i = 0; i < MainPlayerStatePrivate->GetPlayerStateData()->Party.Num(); i++)
			{
				SIZE_T RoleId = MainPlayerStatePrivate->GetPlayerStateData()->Party[i]->RoleId;

				for (SIZE_T j = 0; j < MAX_POISONS; j++)
				{
					if (MainPlayerStatePrivate->GetPlayerStateData()->PoisonStatus[j][i].PoisonID != 0)
					{
						GetWorld()->GetSubsystem<UPALScriptManager>()
							->RunTriggerScript(MainPlayerStatePrivate->GetPlayerStateData()->PoisonStatus[j][i].PoisonScript, RoleId, true);
					}
				}

				// Update statuses
				for (SIZE_T j = 0; j < EPALStatus::_StatusCount; j++)
				{
					if (MainPlayerStatePrivate->GetPlayerStateData()->RoleStatus[RoleId][j] > 0)
					{
						MainPlayerStatePrivate->GetPlayerStateData()->RoleStatus[RoleId][j]--;
					}
				}
			}

			for (SIZE_T i = 0; i < MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies.Num(); i++)
			{
				for (SIZE_T j = 0; j < MAX_POISONS; j++)
				{
					if (MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i]->PoisonStatus[j].PoisonID != 0)
					{
						GetWorld()->GetSubsystem<UPALScriptManager>()
							->RunTriggerScript(MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i]->PoisonStatus[j].PoisonScript, static_cast<uint16>(i), true);
					}
				}

				// Update statuses
				for (SIZE_T j = 0; j < EPALStatus::_StatusCount; j++)
				{
					if (MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i]->Status[j] > 0)
					{
						MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i]->Status[j]--;
					}
				}
			}

			BattlePostActionCheck(false);

			if (HidingTime > 0)
			{
				HidingTime -= DeltaTime;
				if (HidingTime <= 0)
				{
					for (TActorIterator<APALBattleRoleActor> It(GetWorld(), APALBattleRoleActor::StaticClass()); It; ++It)
					{
						APALBattleRoleActor* RoleActor = *It;
						if (RoleActor)
						{
							RoleActor->SetActorHiddenInGame(false);
						}
					}
					for (APALBattleEnemyActor* EnemyActor : EnemyActors)
					{
						EnemyActor->SetActorHiddenInGame(false);
					}
				}
			}

			if (HidingTime <= 0 && MainPlayerStatePrivate->GetPlayerStateData()->bBattleHiding)
			{
				for (SIZE_T i = 0; i < MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies.Num(); i++)
				{
					GetWorld()->GetSubsystem<UPALScriptManager>()->
						RunTriggerScript(MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i]->ScriptOnTurnStart, i, true);
				}
			}

			// Clear all item-using records
			for (FInventoryItem& InventoryItem : MainPlayerStatePrivate->GetPlayerStateData()->Inventory)
			{
				InventoryItem.InUseAmount = 0;
			}

			// Proceed to next turn...
			Phase = EPALBattlePhase::SelectAction;
			bThisTurnCooperation = false;
		}
		else
		{
			FBattleActionQueueItem& CurrentAction = ActionQueue[0];
			SIZE_T i = CurrentAction.Index;

			if (CurrentAction.bIsEnemy)
			{
				if (HidingTime <= 0 && !bOnlyPuppet &&
					MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i]->GetObjectId() != 0)
				{
					GetWorld()->GetSubsystem<UPALScriptManager>()->
						RunTriggerScript(MainPlayerStatePrivate->GetPlayerStateData()->CurrentEnemies[i]->ScriptOnReady, i, true);

					bEnemyMoving = true;
					BattleEnemyPerformAction(i);
					bEnemyMoving = false;
				}
			}
			else {
				SIZE_T RoleId = MainPlayerStatePrivate->GetPlayerStateData()->Party[i]->RoleId;
				if (BattleRoleMap.Find(RoleId)->State == EPALFighterState::DoingMove)
				{
					if (PlayerStateData->PlayerRoles.HP[RoleId] == 0)
					{
						if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Puppet] == 0)
						{
							BattleRoleMap.Find(RoleId)->Action.ActionType = EBattleActionType::BattleActionPass;
						}
					}
					else if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Sleep] > 0 ||
						PlayerStateData->RoleStatus[RoleId][EPALStatus::Paralyzed] > 0)
					{
						BattleRoleMap.Find(RoleId)->Action.ActionType = EBattleActionType::BattleActionPass;
					}
					else if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Confused] > 0)
					{
						BattleRoleMap.Find(RoleId)->Action.ActionType =
							(MainPlayerStatePrivate->IsRoleDying(RoleId) ? EBattleActionType::BattleActionPass : EBattleActionType::BattleActionAttackMate);
					}
					else if (BattleRoleMap.Find(RoleId)->Action.ActionType == EBattleActionType::BattleActionAttack &&
						BattleRoleMap.Find(RoleId)->Action.ItemOrMagicId != 0)
					{
						bPreviousRoleAutoAttack = true;
					}
					else if (bPreviousRoleAutoAttack)
					{
						MainPlayerControllerPrivate->SetUISourceTargetAction(i, BattleRoleMap.Find(RoleId)->Action.Target, EBattleActionType::BattleActionAttack);
						BattleCommitAction(false);
					}

					// Perform the action for this player.
					CurrentMovingRoleId = RoleId;
					BattleRolePerformAction(RoleId);
				}
			}
			ActionQueue.RemoveAt(0);
		}
	}
}
