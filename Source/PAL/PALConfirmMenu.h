// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PALConfirmMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALConfirmMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FConfirmSelect, bool);

	FConfirmSelect OnConfirm;

public:
	UFUNCTION()
	void SelectYes();

	UFUNCTION()
	void SelectNo();

private:
	UWidget* BuildBox(UWidget* Content);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};