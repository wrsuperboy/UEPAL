// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Components/MenuAnchor.h"
#include "PALMenuAnchor.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALMenuAnchor : public UMenuAnchor
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OpenAndFocus();

	UWidget* GetMenuWidget();

private:
	UPROPERTY()
	UWidget* MenuWidget;

protected:
	TSharedRef<SWidget> HandleGetMenuContentNew();

protected:
	TSharedRef<SWidget> RebuildWidget() override;

};
