// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PAL.h"
#include "PALPlayerStateData.h"
#include "PALPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	SIZE_T CurrentSaveSlot;

private:
	UPROPERTY(VisibleAnywhere)
	UPALPlayerStateData* PlayerStateData = nullptr;

public:
	UPALPlayerStateData* InitPlayerStateData(SIZE_T SaveSlot);

	void SavePlayerStateData(FSavedGame& OutSavedGame);

	void SetPlayerStateData(UPALPlayerStateData* NewPlayerStateData);

	UPALPlayerStateData* GetPlayerStateData() const;

	void RemoveEquipmentEffect(const SIZE_T RoleId, const EPALBodyPart EquipPart);

	bool AddItemToInventory(const uint16 ObjectId, const int32 Count = 1);

	bool IncreaseHPMP(const SIZE_T RoleId, const int16 HP, const int16 MP);

	SIZE_T CountItem(const uint16 ObjectId);

	void AddPoisonForRole(const SIZE_T RoleId, const uint16 PoisonId);

	void CurePoisonForRoleByKind(const SIZE_T RoleId, const uint16 PoisonId);

	void CurePoisonForRoleByLevel(const SIZE_T RoleId, const uint16 MaxLevel);

	bool IsRolePoisonedByLevel(const SIZE_T RoleId, const uint16 MinLevel);

	bool IsRolePoisonedByKind(const SIZE_T RoleId, const uint16 PoisonId);

	uint16 GetRoleHighestLevelPoisonId(const SIZE_T RoleId);

	void ClearAllRolePoisons();

	uint16 GetRoleAttackStrength(const SIZE_T RoleId);

	uint16 GetRoleMagicStrength(const SIZE_T RoleId);

	uint16 GetRoleDefense(const SIZE_T RoleId);

	uint16 GetRoleDexterity(const SIZE_T RoleId);

	uint16 GetRoleFleeRate(const SIZE_T RoleId);

	uint16 GetRolePoisonResistance(const SIZE_T RoleId);

	uint16 GetRoleElementalResistance(const SIZE_T RoleId, SIZE_T AttributeNum);

	void SetRoleStatus(const SIZE_T RoleId, const EPALStatus Status, const uint16 RoundNum);

	void RemoveRoleStatus(const SIZE_T RoleId, const EPALStatus Status);

	bool IsRoleInStatus(const SIZE_T RoleId, const EPALStatus Status);

	void ClearAllRoleStatus();

	bool AddMagic(const SIZE_T RoleId, const uint16 Magic);

	void RemoveMagic(const SIZE_T RoleId, const uint16 Magic);

	SIZE_T GetItemAmount(const uint16 Item);

	void UpdateEquipments();

	void RoleLevelUp(const SIZE_T RoleId, const SIZE_T LevelNum);

	void AlignPartyFacing();

	void CompressInventory();

	TArray<FInventoryItem> GetUseableEquippedItems();

	void UseItem(const SIZE_T RoleId, int16 Item);

	void UseItemToAll(int16 Item);

	bool Revive(const SIZE_T RoleId, double HPRatio);

	bool ReviveAll(double HPRatio);

	void StartBattle(const SIZE_T EnemyTeamNum, const bool bIsBoss);

	SIZE_T GetRoleBattleSpriteNum(const SIZE_T RoleId);

	void DamageEnemy(const SIZE_T EnemyIndex, const int32 Damage);

	void DamageAllEnemies(const int32 Damage);

	void AddPoisonForEnemy(const SIZE_T EnemyIndex, const int16 PoisonId);

	void CurePoisonForEnemyByKind(const SIZE_T EnemyIndex, const uint16 PoisonId);

	bool IsRoleDying(const SIZE_T RoleId);

	uint16 GetEnemyDexterity(const SIZE_T EnemyIndex);

	uint16 GetRoleActualDexterity(const SIZE_T RoleId);

private:
	UPALPlayerStateData* LoadDefaultGame() const;

	UPALPlayerStateData* LoadSavedGame(SIZE_T SaveSlot);

};
