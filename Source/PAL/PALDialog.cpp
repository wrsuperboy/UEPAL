// Copyright (C) 2022 Meizhouxuanhan.


#include "PALDialog.h"
#include "Misc/Char.h"
#include "Blueprint/WidgetTree.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/ScrollBox.h"
#include "Components/ScrollBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Slate/SlateBrushAsset.h"
#include "PALDialogDataTable.h"
#include "PALCommon.h"
#include "PAL.h"

void UPALDialog::Start(EPALDialogLocation InDialogLocation, SIZE_T FontColorNum, SIZE_T InCharacterFaceNum)
{
	DialogLocation = InDialogLocation;
	CharacterFaceNum = InCharacterFaceNum;
	Tag = EDisplayTag::TagNone;
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	switch (DialogLocation)
	{
	case DialogUpper:
	{
		Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Title->SetFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
		Title->SetColorAndOpacity(Common->GetDefaultPalette()[140]);
		Title->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		Title->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		Text = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass());
		Text->SetDefaultFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
		FColor FontColor = FontColorNum == 0 ? Common->GetDefaultPalette()[79] : Common->GetDefaultPalette()[FontColorNum];
		Text->SetDefaultColorAndOpacity(FontColor);
		Text->SetDefaultShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		Text->SetDefaultShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		UPALDialogDataTable* DataTable = NewObject<UPALDialogDataTable>();
		DataTable->Init(Common);
		Text->SetTextStyleSet(DataTable);
	}
	break;

	case DialogCenter:
	{
		Text = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass());
		Text->SetDefaultFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
		FColor FontColor = FontColorNum == 0 ? Common->GetDefaultPalette()[79] : Common->GetDefaultPalette()[FontColorNum];
		Text->SetDefaultColorAndOpacity(FontColor);
		Text->SetDefaultShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		Text->SetDefaultShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		UPALDialogDataTable* DataTable = NewObject<UPALDialogDataTable>();
		DataTable->Init(Common);
		Text->SetTextStyleSet(DataTable);
	}
	break;

	case DialogLower:
	{
		Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Title->SetFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
		Title->SetColorAndOpacity(Common->GetDefaultPalette()[140]);
		Title->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		Title->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		Text = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass());
		Text->SetDefaultFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
		FColor FontColor = FontColorNum == 0 ? Common->GetDefaultPalette()[79] : Common->GetDefaultPalette()[FontColorNum];
		Text->SetDefaultColorAndOpacity(FontColor);
		Text->SetDefaultShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		Text->SetDefaultShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		UPALDialogDataTable* DataTable = NewObject<UPALDialogDataTable>();
		DataTable->Init(Common);
		Text->SetTextStyleSet(DataTable);
	}
	break;

	case DialogCenterWindow:
	{
		Text = WidgetTree->ConstructWidget<URichTextBlock>(URichTextBlock::StaticClass());
		FColor FontColor = FontColorNum == 0 ? Common->GetDefaultPalette()[0] : Common->GetDefaultPalette()[FontColorNum];
		Text->SetDefaultColorAndOpacity(FontColor);
		Text->SetDefaultFont(FSlateFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT)));
		UPALDialogDataTable* DataTable = NewObject<UPALDialogDataTable>();
		DataTable->Init(Common);
		Text->SetTextStyleSet(DataTable);
	}
	break;
	}
}

FString UPALDialog::DisplayText(UPALDialog* Dialog, const FString& TextString)
{
	FString Converted;
	if (Tag != EDisplayTag::TagNone)
	{
		switch (Tag)
		{
		case TagCyan:
			Converted.Append(TEXT("<Cyan>"));
			break;

		case TagRed:
			Converted.Append(TEXT("<Red>"));
			break;

		case TagRedAlt:
			Converted.Append(TEXT("<RedAlt>"));
			break;

		case TagYellow:
			Converted.Append(TEXT("<Yellow>"));
			break;
		default:
			checkNoEntry();
			break;
		}
	}
	int32 TextStringLength = TextString.Len();
	int32 Index = 0;
	while (Index < TextStringLength)
	{
		switch (TextString[Index])
		{
		case TEXT('-'):
			if (Tag == EDisplayTag::TagCyan)
			{
				Converted.Append(TEXT("</>"));
				Tag = EDisplayTag::TagNone;
			}
			else
			{
				Converted.Append(TEXT("<Cyan>"));
				Tag = EDisplayTag::TagCyan;
			}
			Index++;
			break;

		case TEXT('\''):
			if (Tag == EDisplayTag::TagRed)
			{
				Converted.Append(TEXT("</>"));
				Tag = EDisplayTag::TagNone;
			}
			else
			{
				Converted.Append(TEXT("<Red>"));
				Tag = EDisplayTag::TagRed;
			}
			Index++;
			break;

		case TEXT('@'):
			if (Tag == EDisplayTag::TagRedAlt)
			{
				Converted.Append(TEXT("</>"));
				Tag = EDisplayTag::TagNone;
			}
			else
			{
				Converted.Append(TEXT("<RedAlt>"));
				Tag = EDisplayTag::TagRedAlt;
			}
			Index++;
			break;

		case TEXT('\"'):
			if (Tag == EDisplayTag::TagYellow)
			{
				Converted.Append(TEXT("</>"));
				Tag = EDisplayTag::TagNone;
			}
			else
			{
				Converted.Append(TEXT("<Yellow>"));
				Tag = EDisplayTag::TagYellow;
			}
			Index++;
			break;

		case TEXT('$'):
			// TODO
			// Set the delay time of text-displaying
			Index += 3;
			break;

		case TEXT('~'):
			// TODO
			// Delay for a period and quit
			if (Tag != EDisplayTag::TagNone)
			{
				Converted.Append(TEXT("</>"));
			}
			return Converted;

		case TEXT(')'):
			// Set the waiting icon
			//g_TextLib.bIcon = 1;
			Index++;
			break;

		case TEXT('('):
			// Set the waiting icon
			//g_TextLib.bIcon = 2;
			Index++;
			break;

		case TEXT('\\'):
			// TODO
			Index++;
			break;

		default:
			if (Dialog->IsDialog())
			{
				if (TChar<TCHAR>::IsDigit(TextString[Index]))
				{
					Converted.Append(TEXT("<Num>")).AppendChar(TextString[Index]).Append(TEXT("</>"));
					Index++;
					break;
				}
			}
			Converted.AppendChar(TextString[Index]);
			Index++;
			break;
		}
	}

	if (Tag != EDisplayTag::TagNone)
	{
		Converted.Append(TEXT("</>"));
	}
	return Converted;
}

void UPALDialog::ShowText(const FString& TextString)
{
	if (DialogLocation == DialogCenterWindow)
	{
		Text->SetText(FText::FromString(DisplayText(this, TextString)));
		if (!IsInViewport())
		{
			AddToViewport(0);
		}
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UPALDialog::RemoveFromParent, 1.4, false);
	}
	else
	{
		if (Text->GetText().IsEmpty() && DialogLocation != DialogCenter
			&& (TextString.EndsWith(TEXT("\uff1a")) ||
				TextString.EndsWith(TEXT("\u2236")) || // Special case for Pal WIN95 Simplified Chinese version
				TextString.EndsWith(TEXT(":"))))
		{
			// name of character
			Title->SetText(FText::FromString(TextString));
		}
		else
		{
			if (Text->GetText().IsEmpty())
			{
				Text->SetText(FText::FromString(DisplayText(this, TextString)));
			}
			else
			{
				Text->SetText(FText::FromString(FString(Text->GetText().ToString()).Append("\n").Append(DisplayText(this, TextString))));
			}
		}
		if (!IsInViewport())
		{
			AddToViewport(0);
		}
	}
}

bool UPALDialog::IsDialog() const
{
	return DialogLocation == EPALDialogLocation::DialogCenterWindow;
}

bool UPALDialog::IsScrollToEnd() const
{
	if (TextScrollBox)
	{
		float CurrentScrollOffset = TextScrollBox->GetScrollOffset();
		float MaxScrollOffset = TextScrollBox->GetScrollOffsetOfEnd();

		return CurrentScrollOffset >= MaxScrollOffset - 1.;
	}
	else 
	{
		return true;
	}
}

void UPALDialog::ScrollToEnd()
{
	if (TextScrollBox)
	{
		float CurrentScrollOffset = TextScrollBox->GetScrollOffset();
		float MaxScrollOffset = TextScrollBox->GetScrollOffsetOfEnd();

		if (CurrentScrollOffset < MaxScrollOffset)
		{
			TextScrollBox->SetScrollOffset(MaxScrollOffset);
		}
		TextScrollBox->ScrollToEnd();
	}
}

void UPALDialog::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (TextScrollBox)
	{
		float CurrentScrollOffset = TextScrollBox->GetScrollOffset();
		float MaxScrollOffset = TextScrollBox->GetScrollOffsetOfEnd();

		if (CurrentScrollOffset < MaxScrollOffset)
		{
			float ScrollSpeed = 12.f * UI_PIXEL_TO_UNIT;
			CurrentScrollOffset += ScrollSpeed * InDeltaTime;
			TextScrollBox->SetScrollOffset(CurrentScrollOffset);
		}
	}
}

TSharedRef<SWidget> UPALDialog::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALDialog::NativeConstruct()
{
	Super::NativeConstruct();
	UCanvasPanel* RootWidget = Cast<UCanvasPanel>(GetRootWidget());
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	switch (DialogLocation)
	{
	case DialogUpper:
	{
		UGridPanel* Grid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
		UCanvasPanelSlot* GridSlot = RootWidget->AddChildToCanvas(Grid);
		GridSlot->SetAnchors(FAnchors(0, 0, 0, 0));
		GridSlot->SetAutoSize(true);
		GridSlot->SetPosition(FVector2D(5, 5) * UI_PIXEL_TO_UNIT);
		USizeBox* CharacterFaceImageSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		UGridSlot* CharacterFaceImageSizeBoxSlot = Grid->AddChildToGrid(CharacterFaceImageSizeBox);
		CharacterFaceImageSizeBox->SetMinDesiredWidth(50 * UI_PIXEL_TO_UNIT);
		CharacterFaceImageSizeBoxSlot->SetRow(0);
		CharacterFaceImageSizeBoxSlot->SetRowSpan(2);
		CharacterFaceImageSizeBoxSlot->SetColumn(0);
		CharacterFaceImageSizeBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		CharacterFaceImageSizeBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		CharacterFaceImageSizeBoxSlot->SetPadding(FMargin(5, 0, 5, 0) * UI_PIXEL_TO_UNIT);
		if (CharacterFaceNum > 0)
		{
			// Display the character face at the upper part of the screen
			UTexture2D* CharacterFace = Common->GetCharacterFace(CharacterFaceNum);
			if (CharacterFace)
			{
				CharacterFaceImageSizeBox->SetWidthOverride(CharacterFace->GetSizeX() * UI_PIXEL_TO_UNIT);
				CharacterFaceImageSizeBox->SetHeightOverride(CharacterFace->GetSizeY() * UI_PIXEL_TO_UNIT);
				UImage* CharacterFaceImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
				CharacterFaceImage->SetBrushFromTexture(CharacterFace);
				USizeBoxSlot* CharacterFaceImageSlot = Cast<USizeBoxSlot>(CharacterFaceImageSizeBox->AddChild(CharacterFaceImage));
			}
		}
		UGridSlot* TitleSlot = Grid->AddChildToGrid(Title);
		TitleSlot->SetRow(0);
		TitleSlot->SetColumn(1);
		TitleSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		USizeBox* TextSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		TextSizeBox->SetHeightOverride(56 * UI_PIXEL_TO_UNIT);
		UGridSlot* TextSizeBoxSlot = Grid->AddChildToGrid(TextSizeBox);
		TextSizeBoxSlot->SetRow(1);
		TextSizeBoxSlot->SetColumn(1);
		TextScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
		TextScrollBox->SetScrollBarVisibility(ESlateVisibility::Hidden);
		TextScrollBox->SetScrollbarPadding(FMargin(0));
		USizeBoxSlot* TextScrollBoxSlot = Cast<USizeBoxSlot>(TextSizeBox->AddChild(TextScrollBox));
		TextScrollBoxSlot->SetPadding(FMargin(16, 0, 0, 0) * UI_PIXEL_TO_UNIT);
		UScrollBoxSlot* TextSlot = Cast<UScrollBoxSlot>(TextScrollBox->AddChild(Text));
	}
	break;

	case DialogCenter:
	{
		UCanvasPanelSlot* TextSlot = RootWidget->AddChildToCanvas(Text);
		TextSlot->SetAnchors(FAnchors(0.5, 0.25, 0.5, 0.25));
		TextSlot->SetAlignment(FVector2D(0.5, 0.5));
		TextSlot->SetAutoSize(true);
	}
	break;

	case DialogLower:
	{
		UGridPanel* Grid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
		UCanvasPanelSlot* GridSlot = RootWidget->AddChildToCanvas(Grid);
		GridSlot->SetAnchors(FAnchors(1, 1, 1, 1));
		GridSlot->SetAlignment(FVector2D(1, 1));
		GridSlot->SetAutoSize(true);
		GridSlot->SetPosition(FVector2D(-5, -5) * UI_PIXEL_TO_UNIT);
		USizeBox* CharacterFaceImageSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		CharacterFaceImageSizeBox->SetMinDesiredWidth(50 * UI_PIXEL_TO_UNIT);
		UGridSlot* CharacterFaceImageSizeBoxSlot = Grid->AddChildToGrid(CharacterFaceImageSizeBox);
		CharacterFaceImageSizeBoxSlot->SetRow(0);
		CharacterFaceImageSizeBoxSlot->SetRowSpan(2);
		CharacterFaceImageSizeBoxSlot->SetColumn(1);
		CharacterFaceImageSizeBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		CharacterFaceImageSizeBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		CharacterFaceImageSizeBoxSlot->SetPadding(FMargin(5, 0, 5, 0) * UI_PIXEL_TO_UNIT);
		if (CharacterFaceNum > 0)
		{
			// Display the character face at the lower part of the screen
			UTexture2D* CharacterFace = Common->GetCharacterFace(CharacterFaceNum);
			if (CharacterFace)
			{
				CharacterFaceImageSizeBox->SetWidthOverride(CharacterFace->GetSizeX() * UI_PIXEL_TO_UNIT);
				CharacterFaceImageSizeBox->SetHeightOverride(CharacterFace->GetSizeY() * UI_PIXEL_TO_UNIT);
				UImage* CharacterFaceImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
				CharacterFaceImage->SetBrushFromTexture(CharacterFace);
				USizeBoxSlot* CharacterFaceImageSlot = Cast<USizeBoxSlot>(CharacterFaceImageSizeBox->AddChild(CharacterFaceImage));
			}
		}
		UGridSlot* TitleSlot = Grid->AddChildToGrid(Title);
		TitleSlot->SetRow(0);
		TitleSlot->SetColumn(0);
		TitleSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		USizeBox* TextSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		TextSizeBox->SetHeightOverride(56 * UI_PIXEL_TO_UNIT);
		UGridSlot* TextSizeBoxSlot = Grid->AddChildToGrid(TextSizeBox);
		TextSizeBoxSlot->SetRow(1);
		TextSizeBoxSlot->SetColumn(0);
		TextScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
		TextScrollBox->SetScrollBarVisibility(ESlateVisibility::Hidden);
		TextScrollBox->SetScrollbarPadding(FMargin(0));
		USizeBoxSlot* TextScrollBoxSlot = Cast<USizeBoxSlot>(TextSizeBox->AddChild(TextScrollBox));
		TextScrollBoxSlot->SetPadding(FMargin(16, 0, 0, 0) * UI_PIXEL_TO_UNIT);
		UScrollBoxSlot* TextSlot = Cast<UScrollBoxSlot>(TextScrollBox->AddChild(Text));
	}
	break;

	case DialogCenterWindow:
	{
		// The text should be shown in a small window at the center of the screen
		UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();

		USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		SizeBox->SetHeightOverride(34 * UI_PIXEL_TO_UNIT);
		UCanvasPanelSlot* SizeBoxSlot = RootWidget->AddChildToCanvas(SizeBox);
		SizeBoxSlot->SetAnchors(FAnchors(0.5, 0, 0.5, 0));
		SizeBoxSlot->SetAlignment(FVector2D(0.5, 0));
		SizeBoxSlot->SetPosition(FVector2D(0, 40) * UI_PIXEL_TO_UNIT);
		SizeBoxSlot->SetAutoSize(true);
		UHorizontalBox* HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
		SizeBox->AddChild(HorizontalBox);
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
		UTexture2D* MiddleTexture = UISprite->GetFrame(45);
		USlateBrushAsset* BrushAsset = NewObject<USlateBrushAsset>();
		BrushAsset->Brush.SetResourceObject(MiddleTexture);
		BrushAsset->Brush.Tiling = ESlateBrushTileType::Horizontal;
		BrushAsset->Brush.SetImageSize(FVector2D(MiddleTexture->GetSizeX(), MiddleTexture->GetSizeY())* UI_PIXEL_TO_UNIT);
		Middle->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		Middle->SetBrushFromAsset(BrushAsset);
		UBorderSlot* LabelSlot = Cast<UBorderSlot>(Middle->AddChild(Text));
		LabelSlot->SetPadding(0);
		LabelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		LabelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		UHorizontalBoxSlot* MiddleSlot = HorizontalBox->AddChildToHorizontalBox(Middle);
		MiddleSlot->SetPadding(FMargin(0));
		MiddleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
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
	break;
	}
}
