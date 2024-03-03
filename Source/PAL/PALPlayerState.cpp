// Copyright (C) 2022 Meizhouxuanhan.


#include "PALPlayerState.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALScriptManager.h"
#include "PALCommon.h"

UPALPlayerStateData* APALPlayerState::InitPlayerStateData(SIZE_T SaveSlot)
{
	if (SaveSlot == 0)
	{
		PlayerStateData = LoadDefaultGame();
	}
	else
	{
		PlayerStateData = LoadSavedGame(SaveSlot);
		CompressInventory();
	}
	UpdateEquipments();
	return PlayerStateData;
}

void APALPlayerState::SavePlayerStateData(FSavedGame& s)
{
	s.Cash = PlayerStateData->Cash;
	s.CollectValue = PlayerStateData->CollectValue;
	s.Layer = PlayerStateData->Layer;
	check(PlayerStateData->Party.Num() > 0);
	s.PartyMember = PlayerStateData->Party.Num() - 1;
	const FPALPosition2d& PartyPostion2d = PlayerStateData->Party[0]->Position.to2d();
	s.ViewportX = FMath::RoundToInt(PartyPostion2d.X) - 320 / 2;
	s.ViewportY = FMath::RoundToInt(PartyPostion2d.Y) - 200 / 2 - 12;
	FMemory::Memzero(s.Party, sizeof(s.Party));
	for (SIZE_T i = 0; i < s.PartyMember + 1; i++)
	{
		UPALRoleData* RoleData = PlayerStateData->Party[i];
		const FPALPosition2d& Position2d = RoleData->Position.to2d();
		s.Party[i].X = FMath::RoundToInt(Position2d.X) - s.ViewportX;
		s.Party[i].Y = FMath::RoundToInt(Position2d.Y) - s.ViewportY;
		s.Party[i].Frame = RoleData->FrameNum;
		s.Party[i].PlayerRole = RoleData->RoleId;
	}
	s.Follower = PlayerStateData->Follow.Num();
	for (SIZE_T i = s.PartyMember + 1; i < s.PartyMember + 1 + s.Follower; i++)
	{
		UPALRoleData* RoleData = PlayerStateData->Party[i - (s.PartyMember + 1)];
		const FPALPosition2d& Position2d = RoleData->Position.to2d();
		s.Party[i].X = FMath::RoundToInt(Position2d.X) - s.ViewportX;
		s.Party[i].Y = FMath::RoundToInt(Position2d.Y) - s.ViewportY;
		s.Party[i].Frame = RoleData->FrameNum;
		s.Party[i].PlayerRole = RoleData->RoleId;
	}
	FMemory::Memzero(s.Trail, sizeof(s.Trail));
	for (SIZE_T i = 0; i < PlayerStateData->Trails.Num(); i++)
	{
		const FPALPosition2d& Position2d = PlayerStateData->Trails[i].Position.to2d();
		s.Trail[i].X = FMath::RoundToInt(Position2d.X);
		s.Trail[i].Y = FMath::RoundToInt(Position2d.Y);
		s.Trail[i].Direction = PlayerStateData->Trails[i].Direction;
	}
	s.PartyDirection = PlayerStateData->PartyDirection;
	FMemory::Memzero(s.Inventory, sizeof(s.Inventory));
	SIZE_T IventoryItemIndex = 0;
	for (const FInventoryItem& IventoryItem : PlayerStateData->Inventory)
	{
		s.Inventory[IventoryItemIndex].Item = IventoryItem.Item;
		s.Inventory[IventoryItemIndex].Amount = IventoryItem.Amount;
		s.Inventory[IventoryItemIndex].AmountInUse = IventoryItem.InUseAmount;
		IventoryItemIndex++;
	}
	FMemory::Memcpy(s.PoisonStatus, PlayerStateData->PoisonStatus, sizeof(s.PoisonStatus));
	FMemory::Memcpy(&s.Exp, &PlayerStateData->ExpAll, sizeof(s.Exp));
	FMemory::Memcpy(&s.PlayerRoles, &PlayerStateData->PlayerRoles, sizeof(s.PlayerRoles));
}

void APALPlayerState::SetPlayerStateData(UPALPlayerStateData* NewPlayerStateData)
{
	PlayerStateData = NewPlayerStateData;
}

UPALPlayerStateData* APALPlayerState::GetPlayerStateData() const
{
	return PlayerStateData;
}

UPALPlayerStateData* APALPlayerState::LoadDefaultGame() const
{
	UPALPlayerStateData* Data = NewObject<UPALPlayerStateData>();
	// Set some default player data.
	Data->Cash = 0;
	Data->CollectValue = 0;
	Data->Layer = 0;

	FMemory::Memzero(Data->PoisonStatus, sizeof(Data->PoisonStatus));
	FMemory::Memzero(&(Data->ExpAll), sizeof(Data->ExpAll));

	UPALGameData* GameData = GetGameInstance<UPALGameInstance>()->GetGameData();
	FMemory::Memcpy(Data->PlayerRoles, GameData->_PlayerRoles);

	for (SIZE_T i = 0; i < MAX_PLAYER_ROLES; i++)
	{
		Data->ExpAll.PrimaryExp[i].Level = Data->PlayerRoles.Level[i];
		Data->ExpAll.HealthExp[i].Level = Data->PlayerRoles.Level[i];
		Data->ExpAll.MagicExp[i].Level = Data->PlayerRoles.Level[i];
		Data->ExpAll.AttackExp[i].Level = Data->PlayerRoles.Level[i];
		Data->ExpAll.MagicPowerExp[i].Level = Data->PlayerRoles.Level[i];
		Data->ExpAll.DefenseExp[i].Level = Data->PlayerRoles.Level[i];
		Data->ExpAll.DexterityExp[i].Level = Data->PlayerRoles.Level[i];
		Data->ExpAll.FleeExp[i].Level = Data->PlayerRoles.Level[i];
	}

	FMemory::Memzero(Data->EquipmentEffects, sizeof(Data->EquipmentEffects));
	FMemory::Memzero(Data->RoleStatus, sizeof(Data->RoleStatus));
	Data->bAutoBattle = false;
	Data->CurrentEquipPart = -1;

	// Add one default here for the scripts to run
	Data->Party.Add(NewObject<UPALRoleData>());
	Data->LastPartyRoleId[0] = 0;

	Data->bInBattle = false;
	Data->bHasTriggerScriptToRun = false;
	Data->BattleMusicNum = 0;
	Data->BattleFieldNum = 0;
	return Data;
}

UPALPlayerStateData* APALPlayerState::LoadSavedGame(SIZE_T SaveSlot)
{
	FSavedGame* s = (FSavedGame*)FMemory::Malloc(sizeof(FSavedGame));
	bool bSuccess = GetGameInstance()->GetSubsystem<UPALCommon>()->GetSavedGame(SaveSlot, *s);
	if (!bSuccess)
	{
		FMemory::Free(s);
		return LoadDefaultGame();
	}

	UPALPlayerStateData* Data = NewObject<UPALPlayerStateData>();
	Data->Cash = s->Cash;
	Data->CollectValue = s->CollectValue;
	Data->Layer = s->Layer;
	for (SIZE_T i = 0; i < s->PartyMember + 1; i++)
	{
		UPALRoleData* RoleData = NewObject<UPALRoleData>();
		RoleData->RoleId = s->Party[i].PlayerRole;
		RoleData->Position = FPALPosition3d(s->ViewportX + s->Party[i].X, s->ViewportY * 2 + s->Party[i].Y * 2, 0); // TODO
		RoleData->FrameNum = s->Party[i].Frame;
		Data->Party.Add(RoleData);
	}
	if (!Data->Party.IsEmpty())
	{
		// Party positions not count however
		Data->Party[0]->Position = FPALPosition3d(s->ViewportX + 320 / 2, (s->ViewportY + 200 / 2 + 12) * 2, 0);
	}
	for (SIZE_T i = 0; i < s->Follower; i++)
	{
		UPALRoleData* RoleData = NewObject<UPALRoleData>();
		RoleData->RoleId = s->Party[s->PartyMember + 1 + i].PlayerRole;
		RoleData->Position = FPALPosition3d(s->ViewportX + s->Party[s->PartyMember + 1 + i].X, s->ViewportY * 2 + s->Party[s->PartyMember + 1 + i].Y * 2, 0); // TODO
		RoleData->FrameNum = s->Party[s->PartyMember + 1 + i].Frame;
		Data->Follow.Add(RoleData);
	}
	for (SIZE_T i = 0; i < s->PartyMember + 1 + s->Follower; i++)
	{
		FTrail Trail;
		Trail.Position = FPALPosition3d(s->Trail[i].X, s->Trail[i].Y * 2, 0); // TODO
		Trail.Direction = static_cast<EPALDirection>(s->Trail[i].Direction);
		Data->Trails.Add(Trail);
	}
	Data->PartyDirection = static_cast<EPALDirection>(s->PartyDirection);
	for (SIZE_T i = 0; i < MAX_INVENTORY; i++)
	{
		if (s->Inventory[i].Amount > 0)
		{
			FInventoryItem InventoryItem(s->Inventory[i].Item, s->Inventory[i].Amount, s->Inventory[i].AmountInUse < 0 ? 0 : s->Inventory[i].AmountInUse);
			Data->Inventory.Add(InventoryItem);
		}
	}
	FMemory::Memcpy(Data->PoisonStatus, s->PoisonStatus, sizeof(Data->PoisonStatus));
	FMemory::Memcpy(&Data->ExpAll, &s->Exp, sizeof(Data->ExpAll));
	FMemory::Memcpy(&Data->PlayerRoles, &s->PlayerRoles, sizeof(Data->PlayerRoles));
	FMemory::Memzero(Data->EquipmentEffects, sizeof(Data->EquipmentEffects));
	Data->bAutoBattle = false;
	Data->CurrentEquipPart = -1;
	for (SIZE_T i = 0; i < MAX_INVENTORY; i++)
	{
		Data->LastPartyRoleId[i] = s->Party[i].PlayerRole;
	}
	Data->bInBattle = false;
	Data->bHasTriggerScriptToRun = false;
	Data->BattleMusicNum = s->NumBattleMusic;
	Data->BattleFieldNum = s->NumBattleField;
	FMemory::Free(s);
	return Data;
}

void APALPlayerState::RemoveEquipmentEffect(const SIZE_T RoleId, const EPALBodyPart EquipPart)
{
	uint16* p = (uint16*)(&PlayerStateData->EquipmentEffects[EquipPart]); // HACKHACK
	for (SIZE_T i = 0; i < sizeof(FPlayerRoles) / sizeof(PLAYERS); i++)
	{
		p[i * MAX_PLAYER_ROLES + RoleId] = 0;
	}

	// Reset some parameters to default when appropriate
	if (EquipPart == EPALBodyPart::Hand)
	{
		// reset the dual attack status
		PlayerStateData->RoleStatus[RoleId][EPALStatus::DualAttack] = 0;
	}
	else if (EquipPart == EPALBodyPart::Wear)
	{
		// Remove all poisons leveled 99
		for (SIZE_T RoleIndex = 0; RoleIndex < PlayerStateData->Party.Num(); RoleIndex++)
		{
			if (PlayerStateData->Party[RoleIndex]->RoleId == RoleId)
			{
				SIZE_T j = 0;

				for (SIZE_T i = 0; i < MAX_POISONS; i++)
				{
					uint16 PoisonId = PlayerStateData->PoisonStatus[i][RoleIndex].PoisonID;

					if (PoisonId == 0)
					{
						break;
					}

					if (GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->Objects[PoisonId].Poison.PoisonLevel < 99)
					{
						PlayerStateData->PoisonStatus[j][RoleIndex] = PlayerStateData->PoisonStatus[i][RoleIndex];
						j++;
					}
				}

				while (j < MAX_POISONS)
				{
					PlayerStateData->PoisonStatus[j][RoleIndex].PoisonID = 0;
					PlayerStateData->PoisonStatus[j][RoleIndex].PoisonScript = 0;
					j++;
				}
				break;
			}
		}
	}
}

bool APALPlayerState::AddItemToInventory(const uint16 ObjectId, const int32 Count)
{
	if (ObjectId == 0)
	{
		return false;
	}

	// Search for the specified item in the inventory
	SIZE_T Index = 0;
	bool bFound = false;
	while (Index < PlayerStateData->Inventory.Num())
	{
		if (PlayerStateData->Inventory[Index].Item == ObjectId)
		{
			bFound = true;
			break;
		}
		Index++;
	}

	if (Count > 0)
	{
		// Add item
		if (Index >= MAX_INVENTORY)
		{
			// inventory is full. cannot add item
			return false;
		}

		if (bFound)
		{
			PlayerStateData->Inventory[Index].Amount += Count;
			if (PlayerStateData->Inventory[Index].Amount > 99)
			{
				// Maximum number is 99
				PlayerStateData->Inventory[Index].Amount = 99;
			}
		}
		else
		{
			PlayerStateData->Inventory.Add(FInventoryItem(ObjectId, Count > 99 ? 99 : Count));
		}

		return true;
	}
	else
	{
		// Remove item
		if (bFound)
		{
			const int32 RemoveCount = -Count;
			if (PlayerStateData->Inventory[Index].Amount < RemoveCount)
			{
				// This item has been run out
				PlayerStateData->Inventory[Index].Amount = 0;
				return false;
			}

			PlayerStateData->Inventory[Index].Amount -= RemoveCount;

			return true;
		}

		return false;
	}
}

bool APALPlayerState::IncreaseHPMP(const SIZE_T RoleId, const int16 HP, const int16 MP)
{
	int32 OriginalHP = PlayerStateData->PlayerRoles.HP[RoleId];
	int32 OriginalMP = PlayerStateData->PlayerRoles.MP[RoleId];

	// Only care about alive players
	if (PlayerStateData->PlayerRoles.HP[RoleId] > 0)
	{
		// change HP
		PlayerStateData->PlayerRoles.HP[RoleId] += HP;

		if ((int16)(PlayerStateData->PlayerRoles.HP[RoleId]) < 0)
		{
			PlayerStateData->PlayerRoles.HP[RoleId] = 0;
		}
		else if (PlayerStateData->PlayerRoles.HP[RoleId] > PlayerStateData->PlayerRoles.MaxHP[RoleId])
		{
			PlayerStateData->PlayerRoles.HP[RoleId] = PlayerStateData->PlayerRoles.MaxHP[RoleId];
		}

		// Change MP
		PlayerStateData->PlayerRoles.MP[RoleId] += MP;

		if ((int16)(PlayerStateData->PlayerRoles.MP[RoleId]) < 0)
		{
			PlayerStateData->PlayerRoles.MP[RoleId] = 0;
		}
		else if (PlayerStateData->PlayerRoles.MP[RoleId] > PlayerStateData->PlayerRoles.MaxMP[RoleId])
		{
			PlayerStateData->PlayerRoles.MP[RoleId] = PlayerStateData->PlayerRoles.MaxMP[RoleId];
		}

		// Avoid over treatment
		if (OriginalHP != PlayerStateData->PlayerRoles.HP[RoleId] || OriginalMP != PlayerStateData->PlayerRoles.MP[RoleId])
		{
			return true;
		}
	}

	return false;
}

SIZE_T APALPlayerState::CountItem(const uint16 ObjectId)
{
	if (ObjectId == 0)
	{
		return 0;
	}

	// Search for the specified item in the inventory
	SIZE_T Count = 0;
	for (const FInventoryItem& InventoryItem : PlayerStateData->Inventory)
	{
		if (InventoryItem.Item == ObjectId) {
			Count = InventoryItem.Amount;
			break;
		}
	}

	for (UPALRoleData* PartyMember : PlayerStateData->Party)
	{
		SIZE_T RoleId = PartyMember->RoleId;

		for (SIZE_T j = 0; j < MAX_PLAYER_EQUIPMENTS; j++)
		{
			if (PlayerStateData->PlayerRoles.Equipments[j][RoleId] == ObjectId)
			{
				Count++;
			}
		}
	}
	return Count;
}

void APALPlayerState::AddPoisonForRole(const SIZE_T RoleId, const uint16 PoisonId)
{
	SIZE_T Index = 0;
	while (Index < PlayerStateData->Party.Num())
	{
		if (PlayerStateData->Party[Index]->RoleId == RoleId)
		{
			break;
		}
		Index++;
	}

	if (Index >= PlayerStateData->Party.Num())
	{
		return;
	}

	for (SIZE_T i = 0; i < MAX_POISONS; i++)
	{
		uint16 w = PlayerStateData->PoisonStatus[i][Index].PoisonID;

		if (w == 0)
		{
			PlayerStateData->PoisonStatus[i][Index].PoisonID = PoisonId;
			uint16 Script = GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->Objects[PoisonId].Poison.PlayerScript;
			GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(Script, RoleId, true);
			PlayerStateData->PoisonStatus[i][Index].PoisonScript = Script;
			break;
		}

		if (w == PoisonId)
		{
			return;
		}
	}
}

void APALPlayerState::CurePoisonForRoleByKind(const SIZE_T RoleId, const uint16 PoisonId)
{
	SIZE_T Index = 0;
	while (Index < PlayerStateData->Party.Num())
	{
		if (PlayerStateData->Party[Index]->RoleId == RoleId)
		{
			break;
		}
		Index++;
	}

	if (Index >= PlayerStateData->Party.Num())
	{
		return;
	}

	for (SIZE_T i = 0; i < MAX_POISONS; i++)
	{
		if (PlayerStateData->PoisonStatus[i][Index].PoisonID == PoisonId)
		{
			PlayerStateData->PoisonStatus[i][Index].PoisonID = 0;
			PlayerStateData->PoisonStatus[i][Index].PoisonScript = 0;
		}
	}
}

void APALPlayerState::CurePoisonForRoleByLevel(const SIZE_T RoleId, const uint16 MaxLevel)
{
	SIZE_T Index = 0;
	while (Index < PlayerStateData->Party.Num())
	{
		if (PlayerStateData->Party[Index]->RoleId == RoleId)
		{
			break;
		}
		Index++;
	}

	if (Index >= PlayerStateData->Party.Num())
	{
		return;
	}

	for (SIZE_T i = 0; i < MAX_POISONS; i++)
	{
		uint16 w = PlayerStateData->PoisonStatus[i][Index].PoisonID;

		if (GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->Objects[w].Poison.PoisonLevel <= MaxLevel)
		{
			PlayerStateData->PoisonStatus[i][Index].PoisonID = 0;
			PlayerStateData->PoisonStatus[i][Index].PoisonScript = 0;
		}
	}
}

bool APALPlayerState::IsRolePoisonedByLevel(const SIZE_T RoleId, const uint16 MinLevel)
{
	SIZE_T Index = 0;
	while (Index < PlayerStateData->Party.Num())
	{
		if (PlayerStateData->Party[Index]->RoleId == RoleId)
		{
			break;
		}
		Index++;
	}

	if (Index >= PlayerStateData->Party.Num())
	{
		return false;
	}

	for (SIZE_T i = 0; i < MAX_POISONS; i++)
	{
		uint16 w = GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->Objects[PlayerStateData->PoisonStatus[i][Index].PoisonID].Poison.PoisonLevel;

		if (w >= 99)
		{
			// Ignore poisons which has a level of 99 (usually effect of equipment)
			continue;
		}

		if (w >= MinLevel)
		{
			return true;
		}
	}

	return false;
}

bool APALPlayerState::IsRolePoisonedByKind(const SIZE_T RoleId, const uint16 PoisonId)
{
	SIZE_T Index = 0;
	while (Index < PlayerStateData->Party.Num())
	{
		if (PlayerStateData->Party[Index]->RoleId == RoleId)
		{
			break;
		}
		Index++;
	}

	if (Index > PlayerStateData->Party.Num())
	{
		return false;
	}

	for (SIZE_T i = 0; i < MAX_POISONS; i++)
	{
		if (PlayerStateData->PoisonStatus[i][Index].PoisonID == PoisonId)
		{
			return true;
		}
	}

	return false;
}

uint16 APALPlayerState::GetRoleHighestLevelPoisonId(const SIZE_T RoleId)
{
	SIZE_T Index = 0;
	while (Index < PlayerStateData->Party.Num())
	{
		if (PlayerStateData->Party[Index]->RoleId == RoleId)
		{
			break;
		}
		Index++;
	}

	if (Index > PlayerStateData->Party.Num())
	{
		return 0;
	}

	uint16 HighestLevel = 0;
	uint16 PoisonId = 0;
	for (SIZE_T i = 0; i < MAX_POISONS; i++)
	{
		if (PlayerStateData->PoisonStatus[i][Index].PoisonID != 0)
		{
			uint16 w = GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->Objects[PlayerStateData->PoisonStatus[i][Index].PoisonID].Poison.PoisonLevel;
			if (HighestLevel <= w)
			{
				PoisonId = PlayerStateData->PoisonStatus[i][Index].PoisonID;
				HighestLevel = w;
			}
		}
	}
	
	return PoisonId;
}

void APALPlayerState::ClearAllRolePoisons()
{
	FMemory::Memzero(PlayerStateData->PoisonStatus, sizeof(PlayerStateData->PoisonStatus));
}

uint16 APALPlayerState::GetRoleAttackStrength(const SIZE_T RoleId)
{
	uint16 w = PlayerStateData->PlayerRoles.AttackStrength[RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		w += PlayerStateData->EquipmentEffects[i].AttackStrength[RoleId];
	}

	return w;
}

uint16 APALPlayerState::GetRoleMagicStrength(const SIZE_T RoleId)
{
	uint16 w = PlayerStateData->PlayerRoles.MagicStrength[RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		w += PlayerStateData->EquipmentEffects[i].MagicStrength[RoleId];
	}

	return w;
}

uint16 APALPlayerState::GetRoleDefense(const SIZE_T RoleId)
{
	uint16 w = PlayerStateData->PlayerRoles.Defense[RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		w += PlayerStateData->EquipmentEffects[i].Defense[RoleId];
	}

	return w;
}

uint16 APALPlayerState::GetRoleDexterity(const SIZE_T RoleId)
{
	uint16 w = PlayerStateData->PlayerRoles.Dexterity[RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		w += PlayerStateData->EquipmentEffects[i].Dexterity[RoleId];
	}

	return w;
}

uint16 APALPlayerState::GetRoleFleeRate(const SIZE_T RoleId)
{
	uint16 w = PlayerStateData->PlayerRoles.FleeRate[RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		w += PlayerStateData->EquipmentEffects[i].FleeRate[RoleId];
	}

	return w;
}

uint16 APALPlayerState::GetRolePoisonResistance(const SIZE_T RoleId)
{
	uint16 w = PlayerStateData->PlayerRoles.PoisonResistance[RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		w += PlayerStateData->EquipmentEffects[i].PoisonResistance[RoleId];
	}

	if (w > 100)
	{
		w = 100;
	}

	return w;
}

uint16 APALPlayerState::GetRoleElementalResistance(const SIZE_T RoleId, SIZE_T AttributeNum)
{
	uint16 w = PlayerStateData->PlayerRoles.ElementalResistances[AttributeNum][RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		w += PlayerStateData->EquipmentEffects[i].ElementalResistances[AttributeNum][RoleId];
	}

	if (w > 100)
	{
		w = 100;
	}

	return w;
}

void APALPlayerState::SetRoleStatus(const SIZE_T RoleId, const EPALStatus Status, const uint16 RoundNum)
{
	switch (Status)
	{
	case Confused:
	case Sleep:
	case Silence:
	case Paralyzed:
		// for "bad" statuses, don't set the status when we already have it
		if (PlayerStateData->RoleStatus[RoleId][Status] == 0)
		{
			PlayerStateData->RoleStatus[RoleId][Status] = RoundNum;
		}
		break;

	case Puppet:
		// only allow dead players for "puppet" status
		if (PlayerStateData->PlayerRoles.HP[RoleId] == 0 &&
			PlayerStateData->RoleStatus[RoleId][Status] < RoundNum)
		{
			PlayerStateData->RoleStatus[RoleId][Status] = RoundNum;
		}
		break;

	case Bravery:
	case Protect:
	case DualAttack:
	case Haste:
		// for "good" statuses, reset the status if the status to be set lasts longer
		if (PlayerStateData->PlayerRoles.HP[RoleId] != 0 &&
			PlayerStateData->RoleStatus[RoleId][Status] < RoundNum)
		{
			PlayerStateData->RoleStatus[RoleId][Status] = RoundNum;
		}
		break;

	default:
		checkNoEntry();
		break;
	}
}

void APALPlayerState::RemoveRoleStatus(const SIZE_T RoleId, const EPALStatus Status)
{
	// Don't remove effects of equipments
	if (PlayerStateData->RoleStatus[RoleId][Status] <= 999)
	{
		PlayerStateData->RoleStatus[RoleId][Status] = 0;
	}
}

bool APALPlayerState::IsRoleInStatus(const SIZE_T RoleId, const EPALStatus Status)
{
	return PlayerStateData->RoleStatus[RoleId][Status] > 0;
}

void APALPlayerState::ClearAllRoleStatus()
{
	for (SIZE_T i = 0; i < MAX_PLAYER_ROLES; i++)
	{
		for (SIZE_T j = 0; j < EPALStatus::_StatusCount; j++)
		{
			// Don't remove effects of equipments
			if (PlayerStateData->RoleStatus[i][j] <= 999)
			{
				PlayerStateData->RoleStatus[i][j] = 0;
			}
		}
	}
}

bool APALPlayerState::AddMagic(const SIZE_T RoleId, const uint16 Magic)
{
	for (SIZE_T i = 0; i < MAX_PLAYER_MAGICS; i++)
	{
		if (PlayerStateData->PlayerRoles.Magics[i][RoleId] == Magic)
		{
			// already have this magic
			return false;
		}
	}

	for (SIZE_T i = 0; i < MAX_PLAYER_MAGICS; i++)
	{
		if (PlayerStateData->PlayerRoles.Magics[i][RoleId] == 0)
		{
			PlayerStateData->PlayerRoles.Magics[i][RoleId] = Magic;
			return true;
		}
	}

	return false;
}

void APALPlayerState::RemoveMagic(const SIZE_T RoleId, const uint16 Magic)
{
	for (SIZE_T i = 0; i < MAX_PLAYER_MAGICS; i++)
	{
		if (PlayerStateData->PlayerRoles.Magics[i][RoleId] == Magic)
		{
			PlayerStateData->PlayerRoles.Magics[i][RoleId] = 0;
			break;
		}
	}
}

SIZE_T APALPlayerState::GetItemAmount(const uint16 Item)
{
	for (const FInventoryItem& InventoryItem : PlayerStateData->Inventory)
	{
		if (InventoryItem.Item == 0)
		{
			break;
		}

		if (InventoryItem.Item == Item)
		{
			return InventoryItem.Amount;
		}
	}

	return 0;
}

void APALPlayerState::UpdateEquipments()
{
	FMemory::Memzero(&(PlayerStateData->EquipmentEffects), sizeof(PlayerStateData->EquipmentEffects));

	for (SIZE_T i = 0; i < MAX_PLAYER_ROLES; i++)
	{
		for (SIZE_T j = 0; j < MAX_PLAYER_EQUIPMENTS; j++)
		{
			uint16 w = PlayerStateData->PlayerRoles.Equipments[j][i];
			if (w != 0)
			{
				GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->Objects[w].Item.ScriptOnEquip, static_cast<uint16>(i), true);
			}
		}
	}
}

void APALPlayerState::RoleLevelUp(const SIZE_T RoleId, const SIZE_T LevelNum)
{
	// Add the level
	PlayerStateData->PlayerRoles.Level[RoleId] += LevelNum;
	if (PlayerStateData->PlayerRoles.Level[RoleId] > MAX_LEVELS)
	{
		PlayerStateData->PlayerRoles.Level[RoleId] = MAX_LEVELS;
	}

	for (SIZE_T i = 0; i < LevelNum; i++)
	{
		// Increase player's stats
		PlayerStateData->PlayerRoles.MaxHP[RoleId] += 10 + FMath::RandRange(0, 8);
		PlayerStateData->PlayerRoles.MaxMP[RoleId] += 8 + FMath::RandRange(0, 6);
		PlayerStateData->PlayerRoles.AttackStrength[RoleId] += 4 + FMath::RandRange(0, 1);
		PlayerStateData->PlayerRoles.MagicStrength[RoleId] += 4 + FMath::RandRange(0, 1);
		PlayerStateData->PlayerRoles.Defense[RoleId] += 2 + FMath::RandRange(0, 1);
		PlayerStateData->PlayerRoles.Dexterity[RoleId] += 2 + FMath::RandRange(0, 1);
		PlayerStateData->PlayerRoles.FleeRate[RoleId] += 2;
	}

#define STAT_LIMIT(t) { if ((t) > 999) (t) = 999; }
	STAT_LIMIT(PlayerStateData->PlayerRoles.MaxHP[RoleId]);
	STAT_LIMIT(PlayerStateData->PlayerRoles.MaxMP[RoleId]);
	STAT_LIMIT(PlayerStateData->PlayerRoles.AttackStrength[RoleId]);
	STAT_LIMIT(PlayerStateData->PlayerRoles.MagicStrength[RoleId]);
	STAT_LIMIT(PlayerStateData->PlayerRoles.Defense[RoleId]);
	STAT_LIMIT(PlayerStateData->PlayerRoles.Dexterity[RoleId]);
	STAT_LIMIT(PlayerStateData->PlayerRoles.FleeRate[RoleId]);
#undef STAT_LIMIT

	// Reset experience points to zero
	PlayerStateData->ExpAll.PrimaryExp[RoleId].Exp = 0;
	PlayerStateData->ExpAll.PrimaryExp[RoleId].Level = PlayerStateData->PlayerRoles.Level[RoleId];
}

void APALPlayerState::AlignPartyFacing()
{
	for (UPALRoleData* RoleData : PlayerStateData->Party)
	{
		RoleData->FrameNum = PlayerStateData->PartyDirection * 3;
	}
}

void APALPlayerState::CompressInventory()
{
	PlayerStateData->Inventory.RemoveAll([](const FInventoryItem& InventoryItem) {
		return InventoryItem.Amount == 0;
		});
}

TArray<FInventoryItem> APALPlayerState::GetUseableEquippedItems()
{
	TArray<FInventoryItem> Result;
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	for (UPALRoleData* RoleData : PlayerStateData->Party)
	{
		SIZE_T RoleId = RoleData->RoleId;
		for (SIZE_T j = 0; j < MAX_PLAYER_EQUIPMENTS; j++)
		{
			if (GameStateData->Objects[PlayerStateData->PlayerRoles.Equipments[j][RoleId]].Item.Flags & EPALItemFlag::ItemFlagUsable)
			{
				Result.Add(FInventoryItem(PlayerStateData->PlayerRoles.Equipments[j][RoleId], 0));
			}
		}
	}
	
	return Result;
}

void APALPlayerState::UseItem(const SIZE_T RoleId, int16 Item)
{
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();

	// Run the script
	bool bScriptSuccess;
	GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[Item].Item.ScriptOnUse, RoleId, true, bScriptSuccess);

	// Remove the item if the item is consuming and the script succeeded
	if ((GameStateData->Objects[Item].Item.Flags & EPALItemFlag::ItemFlagConsuming) && bScriptSuccess)
	{
		AddItemToInventory(Item, -1);
	}
}

void APALPlayerState::UseItemToAll(int16 Item)
{
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	// Run the script
	bool bScriptSuccess;
	GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[Item].Item.ScriptOnUse, UPALScriptManager::LastTriggeredEventObjectId, true, bScriptSuccess);

	// Remove the item if the item is consuming and the script succeeded
	if ((GameStateData->Objects[Item].Item.Flags & EPALItemFlag::ItemFlagConsuming) && bScriptSuccess)
	{
		AddItemToInventory(Item, -1);
	}
}

bool APALPlayerState::Revive(const SIZE_T RoleId, double HPRatio)
{
	bool bRevived = false;
	if (PlayerStateData->PlayerRoles.HP[RoleId] == 0)
	{
		PlayerStateData->PlayerRoles.HP[RoleId] =
			FMath::CeilToInt(PlayerStateData->PlayerRoles.MaxHP[RoleId] * HPRatio);

		CurePoisonForRoleByLevel(RoleId, 3);
		for (SIZE_T x = 0; x < EPALStatus::_StatusCount; x++)
		{
			RemoveRoleStatus(RoleId, static_cast<EPALStatus>(x));
		}
		bRevived = true;
	}
	return bRevived;
}

bool APALPlayerState::ReviveAll(double HPRatio)
{
	bool bRevived = false;
	for (UPALRoleData* PartyMember : PlayerStateData->Party)
	{
		SIZE_T RoleId = PartyMember->RoleId;
		if (Revive(RoleId, HPRatio))
		{
			bRevived = true;
		}
	}
	return bRevived;
}

void APALPlayerState::StartBattle(const SIZE_T EnemyTeamNum, const bool bIsBoss)
{
	// Make sure everyone in the party is alive, also clear all hidden EXP count records
	for (UPALRoleData* PartyMember : PlayerStateData->Party)
	{
		SIZE_T RoleId = PartyMember->RoleId;
		if (PlayerStateData->PlayerRoles.HP[RoleId] == 0)
		{
			PlayerStateData->PlayerRoles.HP[RoleId] = 1;
			PlayerStateData->RoleStatus[RoleId][EPALStatus::Puppet] = 0;
		}
		PlayerStateData->ExpAll.HealthExp[RoleId].Count = 0;
		PlayerStateData->ExpAll.MagicExp[RoleId].Count = 0;
		PlayerStateData->ExpAll.AttackExp[RoleId].Count = 0;
		PlayerStateData->ExpAll.MagicPowerExp[RoleId].Count = 0;
		PlayerStateData->ExpAll.DefenseExp[RoleId].Count = 0;
		PlayerStateData->ExpAll.DexterityExp[RoleId].Count = 0;
		PlayerStateData->ExpAll.FleeExp[RoleId].Count = 0;
	}

	for (FInventoryItem& IventoryItem : PlayerStateData->Inventory)
	{
		IventoryItem.InUseAmount = 0;
	}

	PlayerStateData->CurrentEnemies.Empty();
	UPALGameData* GameData = GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	for (SIZE_T i = 0; i < MAX_ENEMIES_IN_TEAM; i++)
	{
		uint16 ObjectId = GameData->EnemyTeams[EnemyTeamNum].Enemy[i];
		if (ObjectId == 0xFFFF)
		{
			break;
		}

		if (ObjectId != 0)
		{
			UPALBattleEnemyData* Enemy = NewObject<UPALBattleEnemyData>();
			Enemy->Init(ObjectId, GameData->Enemies[GameStateData->Objects[ObjectId].Enemy.EnemyID]);
			Enemy->ScriptOnTurnStart = GameStateData->Objects[ObjectId].Enemy.ScriptOnTurnStart;
			Enemy->ScriptOnBattleEnd = GameStateData->Objects[ObjectId].Enemy.ScriptOnBattleEnd;
			Enemy->ScriptOnReady = GameStateData->Objects[ObjectId].Enemy.ScriptOnReady;
			PlayerStateData->CurrentEnemies.Add(Enemy);
		}
	}
	PlayerStateData->bCurrentEnemyIsBoss = bIsBoss;

	UpdateEquipments();
	PlayerStateData->ExpGainedInBattle = 0;
	PlayerStateData->CashGainedInBattle = 0;
	PlayerStateData->bBattleHiding = false;
}

SIZE_T APALPlayerState::GetRoleBattleSpriteNum(const SIZE_T RoleId)
{
	SIZE_T BattleSpriteNum = PlayerStateData->PlayerRoles.SpriteNumInBattle[RoleId];

	for (SIZE_T i = 0; i <= MAX_PLAYER_EQUIPMENTS; i++)
	{
		if (PlayerStateData->EquipmentEffects[i].SpriteNumInBattle[RoleId] != 0)
		{
			BattleSpriteNum = PlayerStateData->EquipmentEffects[i].SpriteNumInBattle[RoleId];
		}
	}

	return BattleSpriteNum;
}

void APALPlayerState::DamageEnemy(const SIZE_T EnemyIndex, const int32 Damage)
{
	PlayerStateData->CurrentEnemies[EnemyIndex]->Enemy.Health -= Damage;
}

void APALPlayerState::DamageAllEnemies(const int32 Damage)
{
	for (UPALBattleEnemyData* EnemyData : PlayerStateData->CurrentEnemies)
	{
		EnemyData->Enemy.Health -= Damage;
	}
}

void APALPlayerState::AddPoisonForEnemy(const SIZE_T EnemyIndex, const int16 PoisonId)
{
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	UPALBattleEnemyData* EnemyData = PlayerStateData->CurrentEnemies[EnemyIndex];
	SIZE_T j;
	for (j = 0; j < MAX_POISONS; j++)
	{
		if (EnemyData->PoisonStatus[j].PoisonID == PoisonId)
		{
			break;
		}
	}

	if (j >= MAX_POISONS)
	{
		for (j = 0; j < MAX_POISONS; j++)
		{
			if (EnemyData->PoisonStatus[j].PoisonID == 0)
			{
				EnemyData->PoisonStatus[j].PoisonID = PoisonId;
				uint16 Script = GameStateData->Objects[PoisonId].Poison.EnemyScript;
				GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(Script, EnemyIndex, true);
				EnemyData->PoisonStatus[j].PoisonScript = Script;
				break;
			}
		}
	}
}

void APALPlayerState::CurePoisonForEnemyByKind(const SIZE_T EnemyIndex, const uint16 PoisonId)
{
	UPALBattleEnemyData* EnemyData = PlayerStateData->CurrentEnemies[EnemyIndex];
	for (SIZE_T j = 0; j < MAX_POISONS; j++)
	{
		if (EnemyData->PoisonStatus[j].PoisonID == PoisonId)
		{
			EnemyData->PoisonStatus[j].PoisonID = 0;
			EnemyData->PoisonStatus[j].PoisonScript = 0;
			break;
		}
	}
}

bool APALPlayerState::IsRoleDying(const SIZE_T RoleId)
{
	return PlayerStateData->PlayerRoles.HP[RoleId] < FMath::Min(100, PlayerStateData->PlayerRoles.MaxHP[RoleId] / 5);
}

uint16 APALPlayerState::GetRoleActualDexterity(const SIZE_T RoleId)
{
	uint16 Dexterity = GetRoleDexterity(RoleId);

	if (PlayerStateData->RoleStatus[RoleId][EPALStatus::Haste] != 0)
	{
		Dexterity *= 3;
	}

	if (Dexterity > 999)
	{
		Dexterity = 999;
	}
	return Dexterity;
}
