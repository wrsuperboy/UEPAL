// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PALUseMagicMenuItem.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALUseMagicMenuItem : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FMenuItemSelect, uint16);

	FMenuItemSelect OnSelect;

	FMenuItemSelect OnHover;

private:
	uint16 Magic;

	FString MagicName;

	uint16 MP;

	bool bEnabled;

public:
	void Init(uint16 InMagic, const FString& InMagicName, uint16 InMP, bool bInEnabled);

	uint16 GetMagic() const;

	UFUNCTION()
	void Select();

	UFUNCTION()
	void Hover();

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;
	
};
