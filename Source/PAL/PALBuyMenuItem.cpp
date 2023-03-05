// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBuyMenuItem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "PALCommon.h"
#include "PAL.h"

void UPALBuyMenuItem::Init(int16 InItem, const FString& InItemName, uint16 InPrice)
{
	Item = InItem;
	ItemName = InItemName;
	Price = InPrice;
}

void UPALBuyMenuItem::Select()
{
	OnSelect.ExecuteIfBound(Item);
}

void UPALBuyMenuItem::Hover()
{
	OnHover.ExecuteIfBound(Item);
}

TSharedRef<SWidget> UPALBuyMenuItem::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());;
	}

	return Super::RebuildWidget();
}

void UPALBuyMenuItem::NativeConstruct()
{
	Super::NativeConstruct();
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UButton* RootWidget = Cast<UButton>(GetRootWidget());
	FButtonStyle ButtonStyle;
	FSlateBrush SlatBrushNone;
	SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
	ButtonStyle.SetNormal(SlatBrushNone);
	ButtonStyle.SetHovered(SlatBrushNone);
	ButtonStyle.SetPressed(SlatBrushNone);
	ButtonStyle.SetDisabled(SlatBrushNone);
	ButtonStyle.SetNormalForeground(FLinearColor(Common->GetDefaultPalette()[79]));
	ButtonStyle.SetDisabledForeground(FLinearColor(Common->GetDefaultPalette()[28])); // Not used
	ButtonStyle.SetPressedForeground(FLinearColor(Common->GetDefaultPalette()[44]));
	ButtonStyle.SetHoveredForeground(FLinearColor(Common->GetDefaultPalette()[249]));
	RootWidget->SetStyle(ButtonStyle);
	RootWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);
	TScriptDelegate<> OnClicked;
	OnClicked.BindUFunction(this, "Select");
	RootWidget->OnClicked.Add(OnClicked);
	TScriptDelegate<> OnHovered;
	OnHovered.BindUFunction(this, "Hover");
	RootWidget->OnHovered.Add(OnHovered);

	UHorizontalBox* Layout = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UButtonSlot* LayoutSlot = Cast<UButtonSlot>(RootWidget->AddChild(Layout));
	LayoutSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	LayoutSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);

	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	UTextBlock* ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemNameText->SetText(FText::FromString(ItemName));
	ItemNameText->SetFont(FontInfo);
	ItemNameText->SetColorAndOpacity(FSlateColor::UseForeground());
	ItemNameText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemNameText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* ItemNameTextSlot = Layout->AddChildToHorizontalBox(ItemNameText);
	ItemNameTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemNameTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);

	UTextBlock* ItemPriceText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemPriceText->SetText(FText::FromString(FString::FromInt(Price)));
	ItemPriceText->SetColorAndOpacity(FLinearColor(FColor(80, 184, 148)));
	ItemPriceText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemPriceText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* ItemPriceTextSlot = Layout->AddChildToHorizontalBox(ItemPriceText);
	ItemPriceTextSlot->SetPadding(FMargin(0));
	ItemPriceTextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ItemPriceTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemPriceTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
}