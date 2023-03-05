// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALMenuBase.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/GridPanel.h"
#include "Components/HorizontalBox.h"
#include "PALUseMagicMenuItem.h"
#include "PALInGameUseMagicMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALInGameUseMagicMenu : public UPALMenuBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UPALRoleSelectMenu* RoleSelectMenu;

	UPROPERTY()
	UTextBlock* MagicMPNumber;

	UPROPERTY()
	UTextBlock* RoleMPNumber;

	UPROPERTY()
	UHorizontalBox* MetaBox;

	UPROPERTY()
	UTextBlock* DescriptionText;

	UPROPERTY()
	UGridPanel* NineGrid;

	UPROPERTY()
	UHorizontalBox* RoleInfoBoxArray;

	enum EStatus : uint8 { ROLE_UNSELECTED, ROLE_SELECTED, MAGIC_SELECTED };
	EStatus Status;

	SIZE_T SelectedRoleId;

	uint32 SelectedMagic;

	SIZE_T ItemsPerLine = 3;

	UPROPERTY()
	UUniformGridPanel* ItemContainer;

	UPROPERTY()
	TArray<UPALUseMagicMenuItem*> MenuItemList;

public:
	void SelectRole(SIZE_T InRoleId);

	void RefreshMagic();

	void SelectMagic(uint16 Magic);

	void ChangeMagic(uint16 Magic);

	void SelectTarget(SIZE_T InRoleId);

protected:
	virtual void NativeConstruct() override;

	virtual bool GoBack() override;
	
};
