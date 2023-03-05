// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PALMenuBase.h"
#include "PALSaveSlotMenu.h"
#include "PALOpeningMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALOpeningMenu : public UPALMenuBase
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOpeningMenuSaveSlotSelect, SIZE_T);

	FOpeningMenuSaveSlotSelect OnSaveSlotSelected;

private:
	UPROPERTY()
	UPALSaveSlotMenu* SaveSlotMenu;

public:
	UFUNCTION()
	void NewGame();

	UFUNCTION()
	void LoadGame();

	void SelectSaveSlot(SIZE_T SaveSlotNum);

public:
	virtual bool GoBack() override;

protected:
	virtual void NativeConstruct() override;
};
