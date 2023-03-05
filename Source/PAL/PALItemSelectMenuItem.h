// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PALItemSelectMenuItem.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALItemSelectMenuItem : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FMenuItemSelect, int16);

	FMenuItemSelect OnSelect;

	FMenuItemSelect OnHover;

private:
	int16 Item;

	FString ItemName;

	SIZE_T ItemCount;

	bool bSelectable;

	bool bEquipped;

public:
	void Init(int16 InItem, const FString& InItemName, SIZE_T InItemCount, bool bSelectable, bool bEquipped);

	UFUNCTION()
	void Select();

	UFUNCTION()
	void Hover();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;

};
