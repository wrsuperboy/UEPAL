// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALMenuBase.h"
#include "PALRoleInfoPanel.h"
#include "PALBattleMenu.generated.h"

enum EPALBattleMenuState : uint8
{
	BattleMenuMain,
	BattleMenuMagicSelect,
	BattleMenuUseItemSelect,
	BattleMenuThrowItemSelect,
	BattleMenuMisc,
	BattleMenuMiscItemSubMenu,
};

/**
 * 
 */
UCLASS()
class PAL_API UPALBattleMenu : public UPALMenuBase
{
	GENERATED_BODY()

private:
	EPALBattleMenuState MenuState = EPALBattleMenuState::BattleMenuMain;

	UPROPERTY()
	UPALRoleInfoPanel* RoleInfoPanel;

public:
	void SetMenuState(EPALBattleMenuState NewState);

protected:
	virtual void NativeConstruct() override;

	virtual bool GoBack() override;
};
