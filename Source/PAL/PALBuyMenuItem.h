// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PALBuyMenuItem.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALBuyMenuItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	DECLARE_DELEGATE_OneParam(FMenuBuyItemSelect, int16);

	FMenuBuyItemSelect OnSelected;

	FMenuBuyItemSelect OnHovered;

private:
	int16 Item;

	FString ItemName;

	uint16 Price;

public:
	void Init(int16 InItem, const FString& InItemName, uint16 InPrice);

	UFUNCTION()
	void Select();

	UFUNCTION()
	void Hover();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};
