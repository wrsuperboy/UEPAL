// Copyright (C) 2022 Meizhouxuanhan.


#include "PALMenuBase.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SafeZone.h"
#include "Components/SafeZoneSlot.h"
#include "Components/CanvasPanelSlot.h"

bool UPALMenuBase::GoBack()
{
	return false;
}

TSharedRef<SWidget> UPALMenuBase::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget = 
			WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SafeScreen"));;
	}

	return Super::RebuildWidget();
}

void UPALMenuBase::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SafeCanvas"));
	UCanvasPanel* RootWidget = Cast<UCanvasPanel>(GetRootWidget());
	RootWidget->SetIsEnabled(true);
	RootWidget->AddChildToCanvas(Canvas)->SetAnchors(FAnchors(0, 0, 1, 1));

	/*USafeZoneSlot* SafeZoneSlot = Cast<USafeZoneSlot>(Canvas->Slot);
	SafeZoneSlot->bIsTitleSafe = true;
	SafeZoneSlot->SafeAreaScale = 1.0;
	SafeZoneSlot->HAlign = EHorizontalAlignment::HAlign_Fill;
	SafeZoneSlot->VAlign = EVerticalAlignment::VAlign_Fill;
	SafeZoneSlot->Padding = 0;*/
}