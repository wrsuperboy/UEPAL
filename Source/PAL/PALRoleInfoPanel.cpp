// Copyright (C) 2023 Meizhouxuanhan.


#include "PALRoleInfoPanel.h"
#include "Components/HorizontalBox.h"
#include "PALPlayerState.h"
#include "Blueprint/WidgetTree.h"

void UPALRoleInfoPanel::SetSelectionEnabled(bool bEnable)
{
	if (GetRootWidget()) {
		for (UWidget*& Widget : Cast<UHorizontalBox>(GetRootWidget())->GetAllChildren())
		{
			UPALRoleInfoBox* RoleInfoBox = Cast<UPALRoleInfoBox>(Widget);
			RoleInfoBox->SetSelectionEnabled(bEnable);
		}
	}
	bSelectionEnabled = bEnable;
}

void UPALRoleInfoPanel::Refresh()
{
	for (UWidget*& Widget : Cast<UHorizontalBox>(GetRootWidget())->GetAllChildren())
	{
		UPALRoleInfoBox* RoleInfoBox = Cast<UPALRoleInfoBox>(Widget);
		RoleInfoBox->Refresh();
	}
}

void UPALRoleInfoPanel::Select(SIZE_T RoleId)
{
	OnSelected.ExecuteIfBound(RoleId);
}

TSharedRef<SWidget> UPALRoleInfoPanel::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALRoleInfoPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	APALPlayerState* PlayerState = GetOwningPlayer()->GetPlayerState<APALPlayerState>();
	UHorizontalBox* RootWidget = Cast<UHorizontalBox>(GetRootWidget());
	for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
	{
		UPALRoleInfoBox* RoleInfoBox = CreateWidget<UPALRoleInfoBox>(GetOwningPlayer(), UPALRoleInfoBox::StaticClass());
		RoleInfoBox->Init(RoleData->RoleId);
		RoleInfoBox->SetPadding(FMargin(0, 0, 3, 0) * UI_PIXEL_TO_UNIT);
		RoleInfoBox->SetSelectionEnabled(bSelectionEnabled);
		RoleInfoBox->OnSelected.BindUObject(this, &UPALRoleInfoPanel::Select);
		RootWidget->AddChildToHorizontalBox(RoleInfoBox);
	}
}