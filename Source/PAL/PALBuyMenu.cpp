// Copyright (C) 2023 Meizhouxuanhan.


#include "PALBuyMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/GridPanel.h"
#include "Components/ScrollBox.h"
#include "Slate/SlateBrushAsset.h"
#include "PALPlayerState.h"
#include "PALGameInstance.h"
#include "PALBuyMenuItem.h"
#include "PALGameState.h"
#include "PALCommon.h"

void UPALBuyMenu::Init(uint16 InStoreNum)
{
	StoreNum = InStoreNum;
	bItemSelected = false;
}

void UPALBuyMenu::OnItemChange(int16 Item)
{
	if (!bItemSelected)
	{
		UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
		UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
		UTexture2D* ItemImageTexture = Common->GetItemImage(GameStateData->Objects[Item].Item.Bitmap);
		if (ItemImageTexture)
		{
			USlateBrushAsset* ItemImageBrushAsset = NewObject<USlateBrushAsset>();
			ItemImageBrushAsset->Brush.SetResourceObject(ItemImageTexture);
			ItemImageBrushAsset->Brush.SetImageSize(FVector2D(ItemImageTexture->GetSizeX(), ItemImageTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
			ItemImage->SetBrushFromAsset(ItemImageBrushAsset);
		}

		// See how many of this item we have in the inventory
		SIZE_T Count = 0;
		UPALPlayerStateData* PlayerStateData = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
		for (const FInventoryItem& InventoryItem : PlayerStateData->Inventory)
		{
			if (InventoryItem.Item == Item)
			{
				Count += InventoryItem.Amount;
			}
		}
		ItemCountNumber->SetText(FText::FromString(FString::FromInt(Count)));
	}
}

void UPALBuyMenu::OnItemSelect(int16 Item)
{
	if (!bItemSelected)
	{
		SelectedItem = Item;
		bItemSelected = true;
		ConfirmMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPALBuyMenu::OnConfirm(bool bYesOrNo)
{
	if (bItemSelected)
	{
		if (bYesOrNo)
		{
			UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
			uint16 Price = GameStateData->Objects[SelectedItem].Item.Price;
			if (Price <= GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData()->Cash)
			{
				GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData()->Cash -= Price;
				GetOwningPlayer()->GetPlayerState<APALPlayerState>()->AddItemToInventory(SelectedItem);
			}
			ShowCash->Refresh();

			// See how many of this item we have in the inventory
			SIZE_T Count = 0;
			UPALPlayerStateData* PlayerStateData = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
			for (const FInventoryItem& InventoryItem : PlayerStateData->Inventory)
			{
				if (InventoryItem.Item == SelectedItem)
				{
					Count += InventoryItem.Amount;
				}
			}
			ItemCountNumber->SetText(FText::FromString(FString::FromInt(Count)));
		}
		ConfirmMenu->SetVisibility(ESlateVisibility::Hidden);
		bItemSelected = false;
	}
}

void UPALBuyMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UHorizontalBox* HorizontalBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UCanvasPanelSlot* HorizontalBoxSlot = Canvas->AddChildToCanvas(HorizontalBox);
	HorizontalBoxSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	HorizontalBoxSlot->SetAlignment(FVector2D(0.5, 0.5));
	HorizontalBoxSlot->SetAutoSize(true);
	UVerticalBox* VerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UHorizontalBoxSlot* VerticalBoxSlot = HorizontalBox->AddChildToHorizontalBox(VerticalBox);
	VerticalBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UTexture2D* ItemBoxTexture = Common->GetUISprite()->GetFrame(70);
	USizeBox* ItemBoxSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ItemBoxSize->SetWidthOverride(ItemBoxTexture->GetSizeX() * UI_PIXEL_TO_UNIT);
	ItemBoxSize->SetHeightOverride(ItemBoxTexture->GetSizeY() * UI_PIXEL_TO_UNIT);
	UVerticalBoxSlot* ItemBoxSizeSlot = VerticalBox->AddChildToVerticalBox(ItemBoxSize);
	ItemBoxSizeSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
	ItemBoxSizeSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	UBorder* ItemBox = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	ItemBox->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	ItemBox->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemBox->SetPadding(FMargin(0));
	ItemBox->SetBrushFromTexture(ItemBoxTexture);
	ItemBoxSize->AddChild(ItemBox);
	UCanvasPanel* ItemBoxCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	ItemBox->AddChild(ItemBoxCanvas);
	ItemImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	FSlateBrush SlatBrushNone;
	SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
	ItemImage->SetBrush(SlatBrushNone);
	UCanvasPanelSlot* ItemImageSlot = ItemBoxCanvas->AddChildToCanvas(ItemImage);
	ItemImageSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	ItemImageSlot->SetAlignment(FVector2D(0.5, 0.5));
	ItemImageSlot->SetAutoSize(true);

	// Draw the amount of this item in the inventory
	USizeBox* ItemCountSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ItemCountSizeBox->SetHeightOverride(34 * UI_PIXEL_TO_UNIT);
	ItemCountSizeBox->SetWidthOverride(96 * UI_PIXEL_TO_UNIT);
	UVerticalBoxSlot* ItemCountSizeBoxSlot = VerticalBox->AddChildToVerticalBox(ItemCountSizeBox);
	ItemCountSizeBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	ItemCountSizeBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
	ItemCountSizeBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	UHorizontalBox* ItemCountBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	ItemCountSizeBox->AddChild(ItemCountBox);
	UPALSprite* UISprite = Common->GetUISprite();
	UImage* LeftMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* LeftMarginTexture = UISprite->GetFrame(44);
	LeftMargin->SetBrushFromTexture(LeftMarginTexture);
	LeftMargin->SetDesiredSizeOverride(FVector2D(LeftMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, LeftMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* LeftMarginSlot = ItemCountBox->AddChildToHorizontalBox(LeftMargin);
	LeftMarginSlot->SetPadding(FMargin(0));
	LeftMarginSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	LeftMarginSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	LeftMarginSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	UBorder* Middle = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UTexture2D* MiddleTexture = UISprite->GetFrame(45);
	USlateBrushAsset* BrushAsset = NewObject<USlateBrushAsset>();
	BrushAsset->Brush.SetResourceObject(MiddleTexture);
	BrushAsset->Brush.Tiling = ESlateBrushTileType::Horizontal;
	BrushAsset->Brush.SetImageSize(FVector2D(MiddleTexture->GetSizeX(), MiddleTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	Middle->SetBrushFromAsset(BrushAsset);
	UHorizontalBoxSlot* MiddleSlot = ItemCountBox->AddChildToHorizontalBox(Middle);
	MiddleSlot->SetPadding(FMargin(0));
	MiddleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MiddleSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	MiddleSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	UHorizontalBox* LabelRoot = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UBorderSlot* LabelRootSlot = Cast<UBorderSlot>(Middle->AddChild(LabelRoot));
	LabelRootSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	LabelRootSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	UTextBlock* ItemCountName = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemCountName->SetText(FText::FromString(Common->GetWord(35)));
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	ItemCountName->SetFont(FontInfo);
	ItemCountName->SetColorAndOpacity(FLinearColor::Black);
	UHorizontalBoxSlot* ItemCountNameSlot = LabelRoot->AddChildToHorizontalBox(ItemCountName);
	ItemCountNameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	ItemCountNameSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
	ItemCountNameSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemCountNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemCountNumber->SetColorAndOpacity(FLinearColor(FColor(212, 200, 168)));
	ItemCountNumber->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	ItemCountNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* ItemCountNumberSlot = LabelRoot->AddChildToHorizontalBox(ItemCountNumber);
	ItemCountNumberSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ItemCountNumberSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Right);
	ItemCountNumberSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	UImage* RightMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* RightMarginTexture = UISprite->GetFrame(46);
	RightMargin->SetBrushFromTexture(RightMarginTexture);
	RightMargin->SetDesiredSizeOverride(FVector2D(RightMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, RightMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* RightMarginSlot = ItemCountBox->AddChildToHorizontalBox(RightMargin);
	RightMarginSlot->SetPadding(FMargin(0));
	RightMarginSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	RightMarginSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	RightMarginSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	// Draw the cash amount
	USizeBox* ShowCashSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ShowCashSizeBox->SetHeightOverride(34 * UI_PIXEL_TO_UNIT);
	ShowCashSizeBox->SetWidthOverride(96 * UI_PIXEL_TO_UNIT);
	UVerticalBoxSlot* ShowCashSizeBoxSlot = VerticalBox->AddChildToVerticalBox(ShowCashSizeBox);
	ShowCashSizeBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	ShowCashSizeBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);
	ShowCashSizeBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	ShowCash = CreateWidget<UPALShowCash>(GetOwningPlayer(), UPALShowCash::StaticClass());
	ShowCashSizeBox->AddChild(ShowCash);

	// Draw the box
	UGridPanel* NineGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
	UHorizontalBoxSlot* NineGridSlot = HorizontalBox->AddChildToHorizontalBox(NineGrid);
	NineGridSlot->SetPadding(FMargin(16, 0, 0, 0)* UI_PIXEL_TO_UNIT);

	UImage* CornerTL = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UImage* CornerTR = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UImage* CornerBL = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UImage* CornerBR = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* CornerTLTexture = UISprite->GetFrame(0 * 3 + 0 + 1 * 9);
	UTexture2D* CornerTRTexture = UISprite->GetFrame(0 * 3 + 2 + 1 * 9);
	UTexture2D* CornerBLTexture = UISprite->GetFrame(2 * 3 + 0 + 1 * 9);
	UTexture2D* CornerBRTexture = UISprite->GetFrame(2 * 3 + 2 + 1 * 9);
	CornerTL->SetBrushFromTexture(CornerTLTexture);
	CornerTR->SetBrushFromTexture(CornerTRTexture);
	CornerBL->SetBrushFromTexture(CornerBLTexture);
	CornerBR->SetBrushFromTexture(CornerBRTexture);
	UCanvasPanel* CornerTLCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* CornerTLSlot = CornerTLCanvas->AddChildToCanvas(CornerTL);
	CornerTLSlot->SetAnchors(FAnchors(1, 1, 1, 1));
	CornerTLSlot->SetAlignment(FVector2D(1, 1));
	CornerTLSlot->SetSize(FVector2D(CornerTLTexture->GetSizeX(), CornerTLTexture->GetSizeY())* UI_PIXEL_TO_UNIT);
	UCanvasPanel* CornerTRCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* CornerTRSlot = CornerTRCanvas->AddChildToCanvas(CornerTR);
	CornerTRSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	CornerTRSlot->SetAlignment(FVector2D(0, 1));
	CornerTRSlot->SetSize(FVector2D(CornerTRTexture->GetSizeX(), CornerTRTexture->GetSizeY())* UI_PIXEL_TO_UNIT);
	UCanvasPanel* CornerBLCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* CornerBLSlot = CornerBLCanvas->AddChildToCanvas(CornerBL);
	CornerBLSlot->SetAnchors(FAnchors(1, 0, 1, 0));
	CornerBLSlot->SetAlignment(FVector2D(1, 0));
	CornerBLSlot->SetSize(FVector2D(CornerBLTexture->GetSizeX(), CornerBLTexture->GetSizeY())* UI_PIXEL_TO_UNIT);
	UCanvasPanel* CornerBRCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* CornerBRSlot = CornerBRCanvas->AddChildToCanvas(CornerBR);
	CornerBRSlot->SetAnchors(FAnchors(0, 0, 0, 0));
	CornerBRSlot->SetAlignment(FVector2D(0, 0));
	CornerBRSlot->SetSize(FVector2D(CornerBRTexture->GetSizeX(), CornerBRTexture->GetSizeY())* UI_PIXEL_TO_UNIT);
	NineGrid->AddChildToGrid(CornerTLCanvas, 0, 0);
	NineGrid->AddChildToGrid(CornerTRCanvas, 0, 2);
	NineGrid->AddChildToGrid(CornerBLCanvas, 2, 0);
	NineGrid->AddChildToGrid(CornerBRCanvas, 2, 2);

	UBorder* ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UTexture2D* ContentTexture = UISprite->GetFrame(1 * 3 + 1 + 1 * 9);
	USlateBrushAsset* ContentBrushAsset = NewObject<USlateBrushAsset>();
	ContentBrushAsset->Brush.SetResourceObject(ContentTexture);
	ContentBrushAsset->Brush.Tiling = ESlateBrushTileType::Both;
	ContentBrushAsset->Brush.SetImageSize(FVector2D(ContentTexture->GetSizeX(), ContentTexture->GetSizeY())* UI_PIXEL_TO_UNIT);
	ContentBorder->SetBrushFromAsset(ContentBrushAsset);
	USizeBox* ContentSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ContentSize->SetMinDesiredHeight(180 * UI_PIXEL_TO_UNIT);
	ContentSize->SetMinDesiredWidth(180 * UI_PIXEL_TO_UNIT);
	ContentSize->AddChild(ContentBorder);
	NineGrid->AddChildToGrid(ContentSize, 1, 1);
	UScrollBox* ScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
	ScrollBox->SetAnimateWheelScrolling(true);
	ContentBorder->AddChild(ScrollBox);
	ContentBorder->SetPadding(FMargin(-8)* UI_PIXEL_TO_UNIT);

	UVerticalBox* ItemListBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	ScrollBox->AddChild(ItemListBox);
	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	for (SIZE_T i = 0; i < MAX_STORE_ITEM; i++)
	{
		int16 Item = GameData->Stores[StoreNum].Items[i];
		if (Item == 0)
		{
			break;
		}
		
		UPALBuyMenuItem* BuyMenuItem = CreateWidget<UPALBuyMenuItem>(this, UPALBuyMenuItem::StaticClass());
		BuyMenuItem->Init(Item, Common->GetWord(Item), GameStateData->Objects[Item].Item.Price);
		BuyMenuItem->OnSelect.BindUObject(this, &UPALBuyMenu::OnItemSelect);
		BuyMenuItem->OnHover.BindUObject(this, &UPALBuyMenu::OnItemChange);
		ItemListBox->AddChildToVerticalBox(BuyMenuItem);
	}

	UImage* MarginT = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UImage* MarginB = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UImage* MarginL = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UImage* MarginR = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* MarginTTexture = UISprite->GetFrame(0 * 3 + 1 + 1 * 9);
	UTexture2D* MarginBTexture = UISprite->GetFrame(2 * 3 + 1 + 1 * 9);
	UTexture2D* MarginLTexture = UISprite->GetFrame(1 * 3 + 0 + 1 * 9);
	UTexture2D* MarginRTexture = UISprite->GetFrame(1 * 3 + 2 + 1 * 9);
	USlateBrushAsset* BrushAssetT = NewObject<USlateBrushAsset>();
	BrushAssetT->Brush.SetResourceObject(MarginTTexture);
	BrushAssetT->Brush.Tiling = ESlateBrushTileType::Horizontal;
	BrushAssetT->Brush.SetImageSize(FVector2D(MarginTTexture->GetSizeX(), MarginTTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	MarginT->SetBrushFromAsset(BrushAssetT);
	USlateBrushAsset* BrushAssetB = NewObject<USlateBrushAsset>();
	BrushAssetB->Brush.SetResourceObject(MarginBTexture);
	BrushAssetB->Brush.Tiling = ESlateBrushTileType::Horizontal;
	BrushAssetB->Brush.SetImageSize(FVector2D(MarginBTexture->GetSizeX(), MarginBTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	MarginB->SetBrushFromAsset(BrushAssetB);
	USlateBrushAsset* BrushAssetL = NewObject<USlateBrushAsset>();
	BrushAssetL->Brush.SetResourceObject(MarginLTexture);
	BrushAssetL->Brush.Tiling = ESlateBrushTileType::Vertical;
	BrushAssetL->Brush.SetImageSize(FVector2D(MarginLTexture->GetSizeX(), MarginLTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	MarginL->SetBrushFromAsset(BrushAssetL);
	USlateBrushAsset* BrushAssetR = NewObject<USlateBrushAsset>();
	BrushAssetR->Brush.SetResourceObject(MarginRTexture);
	BrushAssetR->Brush.Tiling = ESlateBrushTileType::Vertical;
	BrushAssetR->Brush.SetImageSize(FVector2D(MarginRTexture->GetSizeX(), MarginRTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	MarginR->SetBrushFromAsset(BrushAssetR);
	NineGrid->AddChildToGrid(MarginT, 0, 1);
	NineGrid->AddChildToGrid(MarginB, 2, 1);
	NineGrid->AddChildToGrid(MarginL, 1, 0);
	NineGrid->AddChildToGrid(MarginR, 1, 2);

	ConfirmMenu = CreateWidget<UPALConfirmMenu>(GetOwningPlayer(), UPALConfirmMenu::StaticClass());
	ConfirmMenu->OnConfirm.AddUObject(this, &UPALBuyMenu::OnConfirm);
	UCanvasPanelSlot* ConfirmMenuSlot = Canvas->AddChildToCanvas(ConfirmMenu);
	ConfirmMenuSlot->SetAlignment(FVector2D(0.5, 0.5));
	ConfirmMenuSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	ConfirmMenuSlot->SetAutoSize(true);
	ConfirmMenuSlot->SetZOrder(1);
	ConfirmMenu->SetVisibility(ESlateVisibility::Hidden);
}

bool UPALBuyMenu::GoBack()
{
	if (bItemSelected)
	{
		ConfirmMenu->SetVisibility(ESlateVisibility::Hidden);
		bItemSelected = false;
		return true;
	}
	return false;
}
