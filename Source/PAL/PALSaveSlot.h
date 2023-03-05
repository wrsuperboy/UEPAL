// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "PALSaveSlot.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALSaveSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FSaveSlotSelect, SIZE_T);

	FSaveSlotSelect OnSelected;

private:
	SIZE_T SaveSlotNum;

public:
	void Init(SIZE_T InSaveSlotNum);

	UFUNCTION()
	void Select();

private:
	UButton* BuildSaveSlotButton();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;

};
