// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PALSaveSlotMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALSaveSlotMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FSaveSlotMenuSelect, SIZE_T);
	
	FSaveSlotMenuSelect OnSaveSlotSelected;

	void SelectSaveSlot(SIZE_T SaveSlotNum);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};
