// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSaveSlot.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "Slate/SlateBrushAsset.h"
#include "Styling/SlateBrush.h"
#include "PALCommon.h"

void UPALSaveSlot::Init(SIZE_T InSaveSlotNum)
{
	SaveSlotNum = InSaveSlotNum;
}

void UPALSaveSlot::Select()
{
	OnSelected.ExecuteIfBound(SaveSlotNum);
}

UButton* UPALSaveSlot::BuildSaveSlotButton()
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UButton* SaveSlotButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
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
	SaveSlotButton->SetStyle(ButtonStyle);
	SaveSlotButton->SetTouchMethod(EButtonTouchMethod::PreciseTap);
	TScriptDelegate<> OnClick;
	OnClick.BindUFunction(this, "Select");
	SaveSlotButton->OnClicked.Add(OnClick);

	UHorizontalBox* Layout = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UButtonSlot* LayoutSlot = Cast<UButtonSlot>(SaveSlotButton->AddChild(Layout));
	LayoutSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	LayoutSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);

	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	UTextBlock* ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	FString ItemName = SaveSlotNum <= 5 ? Common->GetWord(42 + SaveSlotNum) : FString("-- ").Append(FString::FromInt(SaveSlotNum)).Append(" --");
	ItemNameText->SetText(FText::FromString(ItemName));
	ItemNameText->SetFont(FontInfo);
	ItemNameText->SetColorAndOpacity(FSlateColor::UseForeground());
	ItemNameText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemNameText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* ItemNameTextSlot = Layout->AddChildToHorizontalBox(ItemNameText);
	ItemNameTextSlot->SetPadding(FMargin(2, 0, 0, 0) * UI_PIXEL_TO_UNIT);
	ItemNameTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemNameTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);

	UTextBlock* ItemCountText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemCountText->SetText(FText::FromString(FString::FromInt(Common->GetSavedTimes(SaveSlotNum))));
	ItemCountText->SetColorAndOpacity(FLinearColor(FColor(212, 200, 168)));
	ItemCountText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemCountText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* ItemCountTextSlot = Layout->AddChildToHorizontalBox(ItemCountText);
	ItemCountTextSlot->SetPadding(FMargin(0, 0, 1, 0) * UI_PIXEL_TO_UNIT);
	ItemCountTextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ItemCountTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemCountTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);

	return SaveSlotButton;
}

TSharedRef<SWidget> UPALSaveSlot::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALSaveSlot::NativeConstruct()
{
	USizeBox* RootWidget = Cast<USizeBox>(GetRootWidget());
	RootWidget->SetWidthOverride(112 * UI_PIXEL_TO_UNIT);
	RootWidget->SetHeightOverride(34 * UI_PIXEL_TO_UNIT);
	UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();

	UHorizontalBox* HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	RootWidget->AddChild(HorizontalBox);
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
	UBorderSlot* LabelSlot = Cast<UBorderSlot>(Middle->AddChild(BuildSaveSlotButton()));
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
}