// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PALSwitchMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALSwitchMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FSwitchSelect, bool);

	FSwitchSelect OnToggled;

public:
	UFUNCTION()
	void Enable();

	UFUNCTION()
	void Disable();

private:
	UWidget* BuildBox(UWidget* Content);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
};
