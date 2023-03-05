// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PALRoleSelectMenu.h"
#include "PALPlayerState.h"
#include "PALEquipItemMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALEquipItemMenu : public UUserWidget
{
	GENERATED_BODY()

private:
	int16 CurrentItem;

	UPROPERTY()
	UImage* ItemImage;

	UPROPERTY()
	UTextBlock* ItemCountText;

	UPROPERTY()
	UTextBlock* ItemNameText;

	UPROPERTY()
	UPALRoleSelectMenu* RoleSelectMenu;

	UPROPERTY()
	TArray<UTextBlock*> EquipNameTexts;

	UPROPERTY()
	TArray<UTextBlock*> StatTexts;

public:
	void Init(int16 Item);

	void ChangeRole(SIZE_T RoleId);

	void SelectRole(SIZE_T RoleId);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};
