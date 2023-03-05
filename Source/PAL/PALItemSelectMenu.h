// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Components/UniformGridPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "PALMenuBase.h"
#include "PALPlayerState.h"
#include "PALItemSelectMenuItem.h"
#include "PALItemSelectMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALItemSelectMenu : public UPALMenuBase
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FItemSelect, int16);

	FItemSelect OnMenuItemChanged;

	FItemSelect OnMenuItemSelected;

private:
	UPROPERTY()
	UUniformGridPanel* ItemContainer;

	UPROPERTY()
	APALPlayerState* PlayerStatePrivate;

	uint16 ItemFlags;

	UPROPERTY()
	TArray<UPALItemSelectMenuItem*> MenuItemList;

	SIZE_T ItemsPerLine = 3;

	UPROPERTY()
	UImage* ItemImage;

	UPROPERTY()
	UTextBlock* DescriptionText;

	UPROPERTY()
	TArray<UUserWidget*> SubWidgets;

public:
	void Init(APALPlayerState* PlayerState, uint16 InItemFlags);

	void Refresh();

	void SelectItem(int16 Item);

	void ChangeItem(int16 Item);

	void Takeover(UUserWidget* Widget);

protected:
	void InitInternal();
	
protected:
	virtual void NativeConstruct() override;

	virtual bool GoBack() override;
};
