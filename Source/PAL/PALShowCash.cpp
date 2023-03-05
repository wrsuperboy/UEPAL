// Copyright (C) 2022 Meizhouxuanhan.


#include "PALShowCash.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Slate/SlateBrushAsset.h"
#include "Styling/SlateBrush.h"
#include "PALPlayerState.h"
#include "PALCommon.h"
#include "PAL.h"

void UPALShowCash::Refresh()
{
	uint32 Cash = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData()->Cash;
	Value->SetText(FText::FromString(FString::FromInt(Cash)));
}

UHorizontalBox* UPALShowCash::BuildLabel()
{
	UHorizontalBox* LabelRoot = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());

	UTextBlock* Name = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Name->SetText(FText::FromString(GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(21)));
	FSlateFontInfo FontInfo(GetGameInstance()->GetSubsystem<UPALCommon>()->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	Name->SetFont(FontInfo);
	Name->SetColorAndOpacity(FLinearColor::Black);
	UHorizontalBoxSlot* NameSlot = LabelRoot->AddChildToHorizontalBox(Name);
	NameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	NameSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
	NameSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

	Value = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Value->SetText(FText::FromString(TEXT("-")));
	Value->SetColorAndOpacity(FLinearColor(FColor(212, 200, 168)));
	Value->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	Value->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* ValueSlot = LabelRoot->AddChildToHorizontalBox(Value);
	ValueSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ValueSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
	ValueSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

	uint32 Cash = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData()->Cash;
	Value->SetText(FText::FromString(FString::FromInt(Cash)));
	return LabelRoot;
}

TSharedRef<SWidget> UPALShowCash::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALShowCash::NativeConstruct()
{
	UHorizontalBox* RootWidget = Cast<UHorizontalBox>(GetRootWidget());
	UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();

	UImage* LeftMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* LeftMarginTexture = UISprite->GetFrame(44);
	LeftMargin->SetBrushFromTexture(LeftMarginTexture);
	LeftMargin->SetDesiredSizeOverride(FVector2D(LeftMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, LeftMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* LeftMarginSlot = RootWidget->AddChildToHorizontalBox(LeftMargin);
	LeftMarginSlot->SetPadding(FMargin(0));
	LeftMarginSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	LeftMarginSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	LeftMarginSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	
	UBorder* Middle = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UBorderSlot* LabelSlot = Cast<UBorderSlot>(Middle->AddChild(BuildLabel()));
	LabelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	LabelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	UTexture2D* MiddleTexture = UISprite->GetFrame(45);
	USlateBrushAsset* BrushAsset = NewObject<USlateBrushAsset>();
	BrushAsset->Brush.SetResourceObject(MiddleTexture);
	BrushAsset->Brush.Tiling = ESlateBrushTileType::Horizontal;
	BrushAsset->Brush.SetImageSize(FVector2D(MiddleTexture->GetSizeX() , MiddleTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	Middle->SetBrushFromAsset(BrushAsset);
	UHorizontalBoxSlot* MiddleSlot = RootWidget->AddChildToHorizontalBox(Middle);
	MiddleSlot->SetPadding(FMargin(0));
	MiddleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MiddleSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	MiddleSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	UImage* RightMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* RightMarginTexture = UISprite->GetFrame(46);
	RightMargin->SetBrushFromTexture(RightMarginTexture);
	RightMargin->SetDesiredSizeOverride(FVector2D(RightMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, RightMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* RightMarginSlot = RootWidget->AddChildToHorizontalBox(RightMargin);
	RightMarginSlot->SetPadding(FMargin(0));
	RightMarginSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	RightMarginSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	RightMarginSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
}
