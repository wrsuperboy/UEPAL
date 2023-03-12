// Copyright (C) 2022 Meizhouxuanhan.


#include "PALUseMagicMenuItem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "PALCommon.h"

void UPALUseMagicMenuItem::Init(uint16 InMagic, const FString& InMagicName, uint16 InMP, bool bInEnabled)
{
	Magic = InMagic;
	MP = InMP;
	MagicName = InMagicName;
	bEnabled = bInEnabled;
}

uint16 UPALUseMagicMenuItem::GetMagic() const
{
	return Magic;
}

void UPALUseMagicMenuItem::Select()
{
	OnSelected.ExecuteIfBound(Magic);
}

void UPALUseMagicMenuItem::Hover()
{
	OnHovered.ExecuteIfBound(Magic);
}

TSharedRef<SWidget> UPALUseMagicMenuItem::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());;
	}

	return Super::RebuildWidget();
}

void UPALUseMagicMenuItem::NativeConstruct()
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
	ButtonStyle.SetNormalForeground(FLinearColor(Common->GetDefaultPalette()[bEnabled ? 79 : 28]));
	ButtonStyle.SetDisabledForeground(FLinearColor(Common->GetDefaultPalette()[28])); // Not used
	ButtonStyle.SetPressedForeground(FLinearColor(Common->GetDefaultPalette()[bEnabled ? 44 : 31]));
	ButtonStyle.SetHoveredForeground(FLinearColor(Common->GetDefaultPalette()[bEnabled ? 249 : 31]));
	RootWidget->SetStyle(ButtonStyle);
	RootWidget->SetTouchMethod(EButtonTouchMethod::PreciseTap);
	if (bEnabled)
	{
		TScriptDelegate<> OnClickedDelegate;
		OnClickedDelegate.BindUFunction(this, "Select");
		RootWidget->OnClicked.Add(OnClickedDelegate);
	}
	TScriptDelegate<> OnHoveredDelegate;
	OnHoveredDelegate.BindUFunction(this, "Hover");
	RootWidget->OnHovered.Add(OnHoveredDelegate);

	UHorizontalBox* Layout = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UButtonSlot* LayoutSlot = Cast<UButtonSlot>(RootWidget->AddChild(Layout));
	LayoutSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	LayoutSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);

	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	UTextBlock* MagicNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MagicNameText->SetText(FText::FromString(MagicName));
	MagicNameText->SetFont(FontInfo);
	MagicNameText->SetColorAndOpacity(FSlateColor::UseForeground());
	MagicNameText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	MagicNameText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* MagicNameTextSlot = Layout->AddChildToHorizontalBox(MagicNameText);
	MagicNameTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	MagicNameTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
}
