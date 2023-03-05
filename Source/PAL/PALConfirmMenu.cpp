// Copyright (C) 2022 Meizhouxuanhan.


#include "PALConfirmMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "Components/Spacer.h"
#include "Slate/SlateBrushAsset.h"
#include "PALCommon.h"
#include "PAL.h"

void UPALConfirmMenu::SelectYes()
{
	OnConfirm.Broadcast(true);
}

void UPALConfirmMenu::SelectNo()
{
	OnConfirm.Broadcast(false);
}

TSharedRef<SWidget> UPALConfirmMenu::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());;
	}

	return Super::RebuildWidget();
}

UWidget* UPALConfirmMenu::BuildBox(UWidget* Content)
{
	USizeBox* Box = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	Box->SetHeightOverride(34 * UI_PIXEL_TO_UNIT);
	UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();

	UHorizontalBox* HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	Box->AddChild(HorizontalBox);
	UImage* LeftMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* LeftMarginTexture = UISprite->GetFrame(44);
	LeftMargin->SetBrushFromTexture(LeftMarginTexture);
	LeftMargin->SetDesiredSizeOverride(FVector2D(LeftMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, LeftMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* LeftMarginSlot = HorizontalBox->AddChildToHorizontalBox(LeftMargin);
	LeftMarginSlot->SetPadding(FMargin(0));
	LeftMarginSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	LeftMarginSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	LeftMarginSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	UBorder* Middle = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UBorderSlot* LabelSlot = Cast<UBorderSlot>(Middle->AddChild(Content));
	LabelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	LabelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	UTexture2D* MiddleTexture = UISprite->GetFrame(45);
	USlateBrushAsset* BrushAsset = NewObject<USlateBrushAsset>();
	BrushAsset->Brush.SetResourceObject(MiddleTexture);
	BrushAsset->Brush.Tiling = ESlateBrushTileType::Horizontal;
	BrushAsset->Brush.SetImageSize(FVector2D(MiddleTexture->GetSizeX(), MiddleTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	Middle->SetBrushFromAsset(BrushAsset);
	UHorizontalBoxSlot* MiddleSlot = HorizontalBox->AddChildToHorizontalBox(Middle);
	MiddleSlot->SetPadding(FMargin(0));
	MiddleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MiddleSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	MiddleSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	UImage* RightMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* RightMarginTexture = UISprite->GetFrame(46);
	RightMargin->SetBrushFromTexture(RightMarginTexture);
	RightMargin->SetDesiredSizeOverride(FVector2D(RightMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, RightMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* RightMarginSlot = HorizontalBox->AddChildToHorizontalBox(RightMargin);
	RightMarginSlot->SetPadding(FMargin(0));
	RightMarginSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	RightMarginSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	RightMarginSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	return Box;
}

void UPALConfirmMenu::NativeConstruct()
{
	Super::NativeConstruct();
	UCanvasPanel* RootWidget = Cast<UCanvasPanel>(GetRootWidget());
	UHorizontalBox* Container = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UCanvasPanelSlot* ContainerSlot = RootWidget->AddChildToCanvas(Container);
	ContainerSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	ContainerSlot->SetAlignment(FVector2D(0.5, 0.5));
	ContainerSlot->SetAutoSize(true);
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();

	FButtonStyle ButtonStyle;
	FSlateBrush SlatBrushNone;
	SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
	ButtonStyle.SetNormal(SlatBrushNone);
	ButtonStyle.SetHovered(SlatBrushNone);
	ButtonStyle.SetPressed(SlatBrushNone);
	ButtonStyle.SetDisabled(SlatBrushNone);
	ButtonStyle.SetNormalForeground(FLinearColor(Common->GetDefaultPalette()[79]));
	ButtonStyle.SetDisabledForeground(FLinearColor(Common->GetDefaultPalette()[28]));
	ButtonStyle.SetPressedForeground(FLinearColor(Common->GetDefaultPalette()[44]));
	ButtonStyle.SetHoveredForeground(FLinearColor(Common->GetDefaultPalette()[249]));
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));

	UButton* NoButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	NoButton->SetStyle(ButtonStyle);
	TScriptDelegate<> OnClickedNo;
	OnClickedNo.BindUFunction(this, "SelectNo");
	NoButton->OnClicked.Add(OnClickedNo);
	UTextBlock* NoButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NoButtonText->SetText(FText::FromString(Common->GetWord(19)));
	NoButtonText->SetFont(FontInfo);
	NoButtonText->SetColorAndOpacity(FSlateColor::UseForeground());
	NoButtonText->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	NoButtonText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UButtonSlot* NoButtonTextSlot = Cast<UButtonSlot>(NoButton->AddChild(NoButtonText));
	NoButtonTextSlot->SetPadding(FMargin(0, 1., 0, 1.) * UI_PIXEL_TO_UNIT);
	NoButtonTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	NoButtonTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	UWidget* NoBox = BuildBox(NoButton);
	UHorizontalBoxSlot* NoBoxSlot = Container->AddChildToHorizontalBox(NoBox);

	USpacer* Space = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
	Space->SetSize(FVector2D(45, 0) * UI_PIXEL_TO_UNIT);
	Container->AddChildToHorizontalBox(Space);

	UButton* YesButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	YesButton->SetStyle(ButtonStyle);
	TScriptDelegate<> OnClickedYes;
	OnClickedYes.BindUFunction(this, "SelectYes");
	YesButton->OnClicked.Add(OnClickedYes);
	UTextBlock* YesButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	YesButtonText->SetText(FText::FromString(Common->GetWord(20)));
	YesButtonText->SetFont(FontInfo);
	YesButtonText->SetColorAndOpacity(FSlateColor::UseForeground());
	YesButtonText->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	YesButtonText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UButtonSlot* YesButtonTextSlot = Cast<UButtonSlot>(YesButton->AddChild(YesButtonText));
	YesButtonTextSlot->SetPadding(FMargin(0, 1., 0, 1.) * UI_PIXEL_TO_UNIT);
	YesButtonTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	YesButtonTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	UWidget* YesBox = BuildBox(YesButton);
	UHorizontalBoxSlot* YesBoxSlot = Container->AddChildToHorizontalBox(YesBox);
}
