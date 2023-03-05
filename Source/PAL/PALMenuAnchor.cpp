// Copyright (C) 2022 Meizhouxuanhan.


#include "PALMenuAnchor.h"

void UPALMenuAnchor::OpenAndFocus()
{
	Open(true);
}

UWidget* UPALMenuAnchor::GetMenuWidget()
{
	return MenuWidget;
}

TSharedRef<SWidget> UPALMenuAnchor::HandleGetMenuContentNew()
{
	TSharedPtr<SWidget> SlateMenuWidget;

	if (OnGetUserMenuContentEvent.IsBound())
	{
		MenuWidget = OnGetUserMenuContentEvent.Execute();
		if (MenuWidget)
		{
			SlateMenuWidget = MenuWidget->TakeWidget();
		}
	}
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	else if (OnGetMenuContentEvent.IsBound())
	{
		// Remove when OnGetMenuContentEvent is fully deprecated.
		MenuWidget = OnGetMenuContentEvent.Execute();
		if (MenuWidget)
		{
			SlateMenuWidget = MenuWidget->TakeWidget();
		}
	}
PRAGMA_ENABLE_DEPRECATION_WARNINGS
	else
	{
		if (MenuClass != nullptr && !MenuClass->HasAnyClassFlags(CLASS_Abstract))
		{
			if (UWorld* World = GetWorld())
			{
				MenuWidget = CreateWidget(World, MenuClass);
				if (MenuWidget)
				{
					SlateMenuWidget = MenuWidget->TakeWidget();
				}
			}
		}
	}

	return SlateMenuWidget.IsValid() ? SlateMenuWidget.ToSharedRef() : SNullWidget::NullWidget;
}

TSharedRef<SWidget> UPALMenuAnchor::RebuildWidget()
{
	MyMenuAnchor = SNew(SMenuAnchor)
		.Placement(Placement)
		.FitInWindow(bFitInWindow)
		.OnGetMenuContent(BIND_UOBJECT_DELEGATE(FOnGetContent, HandleGetMenuContentNew))
		.OnMenuOpenChanged(BIND_UOBJECT_DELEGATE(FOnIsOpenChanged, HandleMenuOpenChanged))
		.ShouldDeferPaintingAfterWindowContent(ShouldDeferPaintingAfterWindowContent)
		.UseApplicationMenuStack(UseApplicationMenuStack);

	if (GetChildrenCount() > 0)
	{
		MyMenuAnchor->SetContent(GetContentSlot()->Content ? GetContentSlot()->Content->TakeWidget() : SNullWidget::NullWidget);
	}

	return MyMenuAnchor.ToSharedRef();
}