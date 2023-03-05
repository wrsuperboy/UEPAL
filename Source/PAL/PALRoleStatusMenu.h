// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "PALMenuBase.h"
#include "PALRoleSelectMenu.h"
#include "PALRoleStatusMenu.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALRoleStatusMenu : public UPALMenuBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UPALRoleSelectMenu* RoleSelectMenu;

	TMap<SIZE_T, int32> RoleIdIndexMap;

	UPROPERTY()
	UWidgetSwitcher* Switcher;

public:
	void SelectRole(SIZE_T InRoleId);

private:
	UWidget* BuildLabelText(const FString& Name);

	UTextBlock* BuildLabelNumber(int32 Number);

	UTextBlock* BuildLabelMaxNumber(int32 Number);

	UWidget* BuildItemNameText(const FString& Name);

protected:
	virtual void NativeConstruct() override;
};
