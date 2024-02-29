// Copyright (C) 2023 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALRoleInfoBox.h"
#include "Blueprint/UserWidget.h"
#include "PALRoleInfoPanel.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API UPALRoleInfoPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UPALRoleInfoBox::FRoleInfoSelect OnSelected;

private:
	bool bSelectionEnabled = true;

public:
	void SetSelectionEnabled(bool bEnable);

	void Refresh();

	void Select(SIZE_T RoleId);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void NativeConstruct() override;

};
