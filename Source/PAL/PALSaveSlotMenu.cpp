// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSaveSlotMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "PALSaveSlot.h"
#include "PAL.h"

TSharedRef<SWidget> UPALSaveSlotMenu::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALSaveSlotMenu::SelectSaveSlot(SIZE_T SaveSlotNum)
{
	OnSaveSlotSelected.Execute(SaveSlotNum);
}

void UPALSaveSlotMenu::NativeConstruct()
{
	UScrollBox* RootWidget = Cast<UScrollBox>(GetRootWidget());
	UVerticalBox* Container = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	RootWidget->AddChild(Container);
	RootWidget->SetAnimateWheelScrolling(true);

	for (SIZE_T SaveSlotNum = 1; SaveSlotNum <= 20; SaveSlotNum++)
	{
		UPALSaveSlot* SaveSlot = CreateWidget<UPALSaveSlot>(this, UPALSaveSlot::StaticClass());
		SaveSlot->Init(SaveSlotNum);
		SaveSlot->OnSelected.BindUObject(this, &UPALSaveSlotMenu::SelectSaveSlot);
		UVerticalBoxSlot* SaveSlotSlot = Container->AddChildToVerticalBox(SaveSlot);
		SaveSlotSlot->SetPadding(FMargin(0, 4, 0, 0) * UI_PIXEL_TO_UNIT);
	}
}