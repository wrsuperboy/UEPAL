// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Components/HorizontalBox.h"
#include "PALBattleMenu.h"
#include "PALBattleAutoAttackMessage.h"
#include "PALBattlePlayerController.generated.h"

enum EPALBattleUIStatus : uint8
{
	BattleUIWait,
	BattleUISelectMove,
	BattleUISelectTargetEnemy,
	BattleUISelectTargetPlayer,
	BattleUISelectTargetEnemyAll,
	BattleUISelectTargetPlayerAll,
};

enum EBattleActionType : uint8
{
	BattleActionPass,          // do nothing
	BattleActionDefend,        // defend
	BattleActionAttack,        // physical attack
	BattleActionMagic,         // use magic
	BattleActionCoopMagic,     // use cooperative magic
	BattleActionFlee,          // flee from the battle
	BattleActionThrowItem,     // throw item onto enemy
	BattleActionUseItem,       // use item
	BattleActionAttackMate,    // attack teammate (confused only)
};

struct FBattleAction
{
	EBattleActionType  ActionType;
	uint16             ItemOrMagicId;   // item/magic to use
	int16              Target;     // -1 for everyone
	//FLOAT              flRemainingTime;  // remaining waiting time before the action start
};

/**
 *
 */
UCLASS()
class PAL_API APALBattlePlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	bool bInit = false;

	EPALBattleUIStatus UIStatus;

	bool bAutoAttack;

	bool bRepeat;

	bool bForce;

	bool bFlee;

	SIZE_T CurrentRoleId;

	int16 SelectedIndex;

	int16 PreviousEnemyTarget;

	EBattleActionType ActionType;

	int16 ObjectId;

	UPROPERTY()
	UPALBattleMenu* BattleMenu;

	UPROPERTY()
	UPALBattleAutoAttackMessage* AutoAttackMessage;

public:
	void Init();

	EPALBattleUIStatus GetUIStatus() const;

	void BattleUIRoleReady(SIZE_T RoleId);

	void BattleUIWait();

	SIZE_T GetCurrentRoleId() const;

	FBattleAction GetBattleAction() const;

	bool IsAutoAttack() const;

	bool IsRepeat() const;

	bool IsForce() const;

	bool IsFlee() const;

	void Flee();

	void SetAutoAttack(bool bInAutoAttack);

	void ClearTeamAction();

	void SetUISourceTargetAction(SIZE_T NewRoleId, int16 ToSelectIndex, EBattleActionType ActionType);

	void OnEscape();

public:
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaTime) override;
};
