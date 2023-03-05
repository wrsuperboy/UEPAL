// Copyright (C) 2022 Meizhouxuanhan.


#include "PALItemSelectMenuItem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "PALCommon.h"
#include "PAL.h"

void UPALItemSelectMenuItem::Init(int16 InItem, const FString& InItemName, SIZE_T InItemCount, bool bInSelectable, bool bInEquipped)
{
	Item = InItem;
	ItemName = InItemName;
	ItemCount = InItemCount;
	bSelectable = bInSelectable;
	bEquipped = bInEquipped;
}

void UPALItemSelectMenuItem::Select()
{
	OnSelect.ExecuteIfBound(Item);
}

void UPALItemSelectMenuItem::Hover()
{
	OnHover.ExecuteIfBound(Item);
}

TSharedRef<SWidget> UPALItemSelectMenuItem::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());;
	}

	return Super::RebuildWidget();
}

void UPALItemSelectMenuItem::NativeConstruct()
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
	ButtonStyle.SetNormalForeground(FLinearColor(Common->GetDefaultPalette()[bSelectable? (bEquipped ? 200 : 79) : 28]));
	ButtonStyle.SetDisabledForeground(FLinearColor(Common->GetDefaultPalette()[28])); // Not used
	ButtonStyle.SetPressedForeground(FLinearColor(Common->GetDefaultPalette()[bSelectable ? 44 : 31]));
	ButtonStyle.SetHoveredForeground(FLinearColor(Common->GetDefaultPalette()[bSelectable ? 249 : 31]));
	RootWidget->SetStyle(ButtonStyle);
	RootWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);
	if (bSelectable)
	{
		TScriptDelegate<> OnClicked;
		OnClicked.BindUFunction(this, "Select");
		RootWidget->OnClicked.Add(OnClicked);
	}
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

	UTextBlock* ItemCountText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	if (ItemCount > 1)
	{
		ItemCountText->SetText(FText::FromString(FString::FromInt(ItemCount)));
	}
	ItemCountText->SetColorAndOpacity(FLinearColor(FColor(80, 184, 148)));
	ItemCountText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemCountText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* ItemCountTextSlot = Layout->AddChildToHorizontalBox(ItemCountText);
	ItemCountTextSlot->SetPadding(FMargin(0));
	ItemCountTextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ItemCountTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemCountTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
}