// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALMenuBase.h"
#include "Components/Image.h"
#include "PALShowCash.h"
#include "PALConfirmMenu.h"
#include "PALBuyMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALBuyMenu : public UPALMenuBase
{
	GENERATED_BODY()

private:
	uint16 StoreNum;

	bool bItemSelected;
	int16 SelectedItem;

	UPROPERTY()
	UPALShowCash* ShowCash;

	UPROPERTY()
	UPALConfirmMenu* ConfirmMenu;

	UPROPERTY()
	UImage* ItemImage;

	UPROPERTY()
	UTextBlock* ItemCountNumber;

public:
	void Init(uint16 InStoreNum);

	void OnItemChange(int16 Item);

	void OnItemSelect(int16 Item);

	void OnConfirm(bool bYesOrNo);
	
protected:
	virtual void NativeConstruct() override;

	virtual bool GoBack() override;
};
