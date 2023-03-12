// Copyright (C) 2022 Meizhouxuanhan.


#include "PALItemSelectMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/UniformGridSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Slate/SlateBrushAsset.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALScriptManager.h"
#include "PALCommon.h"

void UPALItemSelectMenu::Init(APALPlayerState* PlayerState, uint16 InItemFlags)
{
	PlayerStatePrivate = PlayerState;
	ItemFlags = InItemFlags;
	InitInternal();
}

void UPALItemSelectMenu::Refresh()
{
	while (!SubWidgets.IsEmpty()
		&& (!SubWidgets[SubWidgets.Num() - 1] || !SubWidgets[SubWidgets.Num() - 1]->IsValidLowLevel() || !SubWidgets[SubWidgets.Num() - 1]->IsInViewport()))
	{
		SubWidgets.Pop();
	}
	if (SubWidgets.IsEmpty())
	{
		SetIsEnabled(true);
	}
	InitInternal();
	ItemContainer->ClearChildren();
	SIZE_T Count = 0;
	for (UPALItemSelectMenuItem*& MenuItem : MenuItemList)
	{
		UUniformGridSlot* MenuItemSlot = ItemContainer->AddChildToUniformGrid(MenuItem, Count / ItemsPerLine, Count % ItemsPerLine);
		MenuItemSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		MenuItemSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		Count++;
	}
}

void UPALItemSelectMenu::SelectItem(int16 Item)
{
	PAL_DebugMsg("Item select menu selected item");
	OnMenuItemSelected.Broadcast(Item);
}

void UPALItemSelectMenu::ChangeItem(int16 Item)
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
	if (Common->GetGameDistribution() != EGameDistribution::DOS)
	{
		FString DescriptionString;
		UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
		uint16 ScriptEntry = GameStateData->Objects[Item].Item.ScriptDesc;
		while (ScriptEntry && GameData->ScriptEntries[ScriptEntry].Operation != 0)
		{
			FScriptEntry* Script = &(GameData->ScriptEntries[ScriptEntry]);
			if (GameData->ScriptEntries[ScriptEntry].Operation == 0xFFFF)
			{
				DescriptionString.Append(Common->GetMessage(Script->Operand[0]));
				if ((GameData->ScriptEntries[ScriptEntry].Operand[1] != 1))
				{
					DescriptionString.AppendChar('\n');
				}
				ScriptEntry++;
			}
			else
			{
				GetWorld()->GetSubsystem<UPALScriptManager>()->RunAutoScript(ScriptEntry, 0, true);
			}
		}
		DescriptionText->SetText(FText::FromString(DescriptionString));
	}
	OnMenuItemChanged.Broadcast(Item);
}

void UPALItemSelectMenu::Takeover(UUserWidget* Widget)
{
	if (SubWidgets.IsEmpty())
	{
		SetIsEnabled(false);
	}
	SubWidgets.Push(Widget);
}

void UPALItemSelectMenu::InitInternal()
{
	PlayerStatePrivate->CompressInventory();

	TArray<FInventoryItem> Inventory(PlayerStatePrivate->GetPlayerStateData()->Inventory);
	// Also add usable equipped items to the list
	if ((ItemFlags & EPALItemFlag::ItemFlagUsable) && !GetWorld()->GetGameState<APALGameState>()->GetGameStateData()->bInBattle)
	{
		Inventory.Append(PlayerStatePrivate->GetUseableEquippedItems());
	}

	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();

	MenuItemList.Empty();
	for (FInventoryItem& IventoryItem : Inventory)
	{
		bool bEquipped = IventoryItem.Amount == 0;
		
		bool bSelectable = (GameStateData->Objects[IventoryItem.Item].Item.Flags & ItemFlags) &&
			(IventoryItem.Amount > IventoryItem.InUseAmount || bEquipped);

		verify(IventoryItem.Amount >= IventoryItem.InUseAmount);
		UPALItemSelectMenuItem* MenuItem = CreateWidget<UPALItemSelectMenuItem>(this, UPALItemSelectMenuItem::StaticClass());
		MenuItem->Init(IventoryItem.Item, Common->GetWord(IventoryItem.Item), IventoryItem.Amount - IventoryItem.InUseAmount, bSelectable, bEquipped);
		MenuItem->OnSelected.BindUObject(this, &UPALItemSelectMenu::SelectItem);
		MenuItem->OnHovered.BindUObject(this, &UPALItemSelectMenu::ChangeItem);
		MenuItemList.Add(MenuItem);
	}
}

void UPALItemSelectMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UGridPanel* NineGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
	UCanvasPanelSlot* NineGridSlot = Canvas->AddChildToCanvas(NineGrid);
	NineGridSlot->SetAnchors(FAnchors(0.5, 0, 0.5, 0));
	NineGridSlot->SetAlignment(FVector2D(0.5, 0));
	NineGridSlot->SetAutoSize(true);
	UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();

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
	CornerTLSlot->SetSize(FVector2D(CornerTLTexture->GetSizeX(), CornerTLTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	UCanvasPanel* CornerTRCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* CornerTRSlot = CornerTRCanvas->AddChildToCanvas(CornerTR);
	CornerTRSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	CornerTRSlot->SetAlignment(FVector2D(0, 1));
	CornerTRSlot->SetSize(FVector2D(CornerTRTexture->GetSizeX(), CornerTRTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	UCanvasPanel* CornerBLCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* CornerBLSlot = CornerBLCanvas->AddChildToCanvas(CornerBL);
	CornerBLSlot->SetAnchors(FAnchors(1, 0, 1, 0));
	CornerBLSlot->SetAlignment(FVector2D(1, 0));
	CornerBLSlot->SetSize(FVector2D(CornerBLTexture->GetSizeX(), CornerBLTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	UCanvasPanel* CornerBRCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UCanvasPanelSlot* CornerBRSlot = CornerBRCanvas->AddChildToCanvas(CornerBR);
	CornerBRSlot->SetAnchors(FAnchors(0, 0, 0, 0));
	CornerBRSlot->SetAlignment(FVector2D(0, 0));
	CornerBRSlot->SetSize(FVector2D(CornerBRTexture->GetSizeX(), CornerBRTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	NineGrid->AddChildToGrid(CornerTLCanvas, 0, 0);
	NineGrid->AddChildToGrid(CornerTRCanvas, 0, 2);
	NineGrid->AddChildToGrid(CornerBLCanvas, 2, 0);
	NineGrid->AddChildToGrid(CornerBRCanvas, 2, 2);
	
	UBorder* ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UTexture2D* ContentTexture = UISprite->GetFrame(1 * 3 + 1 + 1 * 9);
	USlateBrushAsset* ContentBrushAsset = NewObject<USlateBrushAsset>();
	ContentBrushAsset->Brush.SetResourceObject(ContentTexture);
	ContentBrushAsset->Brush.Tiling = ESlateBrushTileType::Both;
	ContentBrushAsset->Brush.SetImageSize(FVector2D(ContentTexture->GetSizeX(), ContentTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	ContentBorder->SetBrushFromAsset(ContentBrushAsset);
	USizeBox* ContentSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ContentSize->SetHeightOverride(150 * UI_PIXEL_TO_UNIT);
	ContentSize->SetMinDesiredWidth(300 * UI_PIXEL_TO_UNIT);
	ContentSize->AddChild(ContentBorder);
	NineGrid->AddChildToGrid(ContentSize, 1, 1);
	UScrollBox* ScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
	ScrollBox->SetAnimateWheelScrolling(true);
	ContentBorder->AddChild(ScrollBox);
	ItemContainer = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass());
	ItemContainer->SetMinDesiredSlotWidth(100 * UI_PIXEL_TO_UNIT);
	ItemContainer->SetSlotPadding(FMargin(0, 1, 0, 1) * UI_PIXEL_TO_UNIT);
	ScrollBox->AddChild(ItemContainer);
	SIZE_T Count = 0;
	for (UPALItemSelectMenuItem*& MenuItem : MenuItemList)
	{
		UUniformGridSlot* MenuItemSlot = ItemContainer->AddChildToUniformGrid(MenuItem, Count / ItemsPerLine, Count % ItemsPerLine);
		MenuItemSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		MenuItemSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		Count++;
	}
	ContentBorder->SetPadding(FMargin(-8) * UI_PIXEL_TO_UNIT);

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

	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	USizeBox* DescriptionSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	DescriptionSize->SetHeightOverride(100 * UI_PIXEL_TO_UNIT);
	DescriptionSize->SetMinDesiredWidth(300 * UI_PIXEL_TO_UNIT);
	UCanvasPanelSlot* DescriptionSizeSlot = Canvas->AddChildToCanvas(DescriptionSize);
	DescriptionSizeSlot->SetAnchors(FAnchors(0.5, 1, 0.5, 1));
	DescriptionSizeSlot->SetAlignment(FVector2D(0.5, 1));
	DescriptionSizeSlot->SetAutoSize(true);
	UHorizontalBox* DescriptionBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	DescriptionSize->AddChild(DescriptionBox);

	UTexture2D* ItemBoxTexture = Common->GetUISprite()->GetFrame(70);
	USizeBox* ItemBoxSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ItemBoxSize->SetWidthOverride(ItemBoxTexture->GetSizeX() * UI_PIXEL_TO_UNIT);
	ItemBoxSize->SetHeightOverride(ItemBoxTexture->GetSizeY() * UI_PIXEL_TO_UNIT);
	UHorizontalBoxSlot* ItemBoxSizeSlot = DescriptionBox->AddChildToHorizontalBox(ItemBoxSize);
	ItemBoxSizeSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
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
	DescriptionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	DescriptionText->SetFont(FontInfo);
	DescriptionText->SetColorAndOpacity(Common->GetDefaultPalette()[46]);
	DescriptionText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	DescriptionText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* DescriptionTextSlot = DescriptionBox->AddChildToHorizontalBox(DescriptionText);
	DescriptionTextSlot->SetPadding(FVector2D(5, 5) * UI_PIXEL_TO_UNIT);
	DescriptionTextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
}

bool UPALItemSelectMenu::GoBack()
{
	while (!SubWidgets.IsEmpty())
	{
		UUserWidget* Widget = SubWidgets.Pop();
		if (Widget && Widget->IsValidLowLevel())
		{
			bool bIsInViewport = Widget->IsInViewport();
			Widget->RemoveFromParent();
			Refresh();
			if (bIsInViewport)
			{
				return true;
			}
		}
	}

	return false;
}