// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBattleAutoAttackMessage.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "PALCommon.h"

TSharedRef<SWidget> UPALBattleAutoAttackMessage::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALBattleAutoAttackMessage::NativeConstruct()
{
	UCanvasPanel* Canvas = Cast<UCanvasPanel>(GetRootWidget());
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UTextBlock* Message = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Message->SetText(FText::FromString(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(56)));
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	Message->SetFont(FontInfo);
	Message->SetColorAndOpacity(FLinearColor(Common->GetDefaultPalette()[44]));
	Message->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	Message->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* MessageSlot = Canvas->AddChildToCanvas(Message);
	MessageSlot->SetAlignment(FVector2D(1, 0));
	MessageSlot->SetPosition(FVector2D(312, 10) * UI_PIXEL_TO_UNIT);
}
