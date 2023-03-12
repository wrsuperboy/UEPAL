// Copyright (C) 2022 Meizhouxuanhan.


#include "PALRoleSelectMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Slate/SlateBrushAsset.h"
#include "PALGameState.h"
#include "PALCommon.h"

void UPALRoleSelectMenu::AddPartyRole(SIZE_T RoleId, APALPlayerState* PlayerState, bool bSelectable)
{
	UPALRoleSelectMenuItem* MenuItem = CreateWidget<UPALRoleSelectMenuItem>(GetOwningPlayer(), UPALRoleSelectMenuItem::StaticClass());
	const FString& RoleName = GetGameInstance()->GetSubsystem<UPALCommon>()->GetWord(PlayerState->GetPlayerStateData()->PlayerRoles.Name[RoleId]);
	MenuItem->Init(RoleId, RoleName, bSelectable);
	MenuItem->OnSelected.BindUObject(this, &UPALRoleSelectMenu::SelectRole);
	MenuItem->OnHovered.BindUObject(this, &UPALRoleSelectMenu::ChangeRole);
	ItemList.Add(MenuItem);
	if (ItemContainer)
	{
		ItemContainer->AddChild(MenuItem);
	}
}

void UPALRoleSelectMenu::ClearRoles()
{
	ItemContainer->ClearChildren();
	ItemList.Empty();
}

void UPALRoleSelectMenu::SelectRole(SIZE_T RoleId)
{
	OnRoleSelected.ExecuteIfBound(RoleId);
}

void UPALRoleSelectMenu::ChangeRole(SIZE_T RoleId)
{
	OnRoleChanged.ExecuteIfBound(RoleId);
}

TSharedRef<SWidget> UPALRoleSelectMenu::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALRoleSelectMenu::NativeConstruct()
{
	Super::NativeConstruct();
	UCanvasPanel* RootWidget = Cast<UCanvasPanel>(GetRootWidget());
	if (!bHasBorder)
	{
		ItemContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
		UCanvasPanelSlot* ItemContainerSlot = RootWidget->AddChildToCanvas(ItemContainer);
		ItemContainerSlot->SetAnchors(FAnchors(0));
		ItemContainerSlot->SetAutoSize(true);
		for (UPALRoleSelectMenuItem* Item : ItemList)
		{
			ItemContainer->AddChild(Item);
		}
	}
	else
	{
		USizeBox* SizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		UCanvasPanelSlot* SizeBoxSlot = RootWidget->AddChildToCanvas(SizeBox);
		SizeBoxSlot->SetAnchors(FAnchors(0));
		SizeBoxSlot->SetAutoSize(true);

		UGridPanel* NineGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
		USizeBoxSlot* NineGridSlot = Cast<USizeBoxSlot>(SizeBox->AddChild(NineGrid));
		NineGridSlot->SetPadding(0);
		NineGridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		NineGridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();

		UImage* CornerTL = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UImage* CornerTR = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UImage* CornerBL = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UImage* CornerBR = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UTexture2D* CornerTLTexture = UISprite->GetFrame(0 * 3 + 0);
		UTexture2D* CornerTRTexture = UISprite->GetFrame(0 * 3 + 2);
		UTexture2D* CornerBLTexture = UISprite->GetFrame(2 * 3 + 0);
		UTexture2D* CornerBRTexture = UISprite->GetFrame(2 * 3 + 2);
		CornerTL->SetBrushFromTexture(CornerTLTexture);
		CornerTR->SetBrushFromTexture(CornerTRTexture);
		CornerBL->SetBrushFromTexture(CornerBLTexture);
		CornerBR->SetBrushFromTexture(CornerBRTexture);
		USizeBox* CornerTLSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		CornerTLSizeBox->AddChild(CornerTL);
		CornerTLSizeBox->SetWidthOverride(CornerTLTexture->GetSizeX() * UI_PIXEL_TO_UNIT);
		CornerTLSizeBox->SetHeightOverride(CornerTLTexture->GetSizeY() * UI_PIXEL_TO_UNIT);
		UCanvasPanel* CornerTRCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
		UCanvasPanelSlot* CornerTRSlot = CornerTRCanvas->AddChildToCanvas(CornerTR);
		CornerTRSlot->SetAnchors(FAnchors(0, 0, 0, 1));
		CornerTRSlot->SetOffsets(FMargin(0));
		CornerTRSlot->SetSize(FVector2D(CornerTRTexture->GetSizeX() * UI_PIXEL_TO_UNIT, CornerTRSlot->GetSize().Y));
		USizeBox* CornerBLSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
		CornerBLSizeBox->AddChild(CornerBL);
		CornerBLSizeBox->SetWidthOverride(CornerBLTexture->GetSizeX() * UI_PIXEL_TO_UNIT);
		CornerBLSizeBox->SetHeightOverride(CornerBLTexture->GetSizeY() * UI_PIXEL_TO_UNIT);
		UCanvasPanel* CornerBRCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
		UCanvasPanelSlot* CornerBRSlot = CornerBRCanvas->AddChildToCanvas(CornerBR);
		CornerBRSlot->SetAnchors(FAnchors(0, 0, 0, 1));
		CornerBRSlot->SetOffsets(FMargin(0));
		CornerBRSlot->SetSize(FVector2D(CornerBRTexture->GetSizeX() * UI_PIXEL_TO_UNIT, CornerBRSlot->GetSize().Y));
		NineGrid->AddChildToGrid(CornerTLSizeBox, 0, 0);
		NineGrid->AddChildToGrid(CornerTRCanvas, 0, 2);
		NineGrid->AddChildToGrid(CornerBLSizeBox, 2, 0);
		NineGrid->AddChildToGrid(CornerBRCanvas, 2, 2);

		UBorder* ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		UTexture2D* ContentTexture = UISprite->GetFrame(1 * 3 + 1);
		USlateBrushAsset* ContentBrushAsset = NewObject<USlateBrushAsset>();
		ContentBrushAsset->Brush.SetResourceObject(ContentTexture);
		ContentBrushAsset->Brush.Tiling = ESlateBrushTileType::Both;
		//ContentBrushAsset->Brush.SetImageSize(FVector2D(ContentTexture->GetSizeX(), ContentTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		ContentBorder->SetBrushFromAsset(ContentBrushAsset);
		NineGrid->AddChildToGrid(ContentBorder, 1, 1);
		ItemContainer = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
		ContentBorder->AddChild(ItemContainer);
		ContentBorder->SetPadding(FMargin(-8) * UI_PIXEL_TO_UNIT);
		for (UPALRoleSelectMenuItem* Item : ItemList)
		{
			ItemContainer->AddChild(Item);
		}

		UImage* MarginT = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UImage* MarginB = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UImage* MarginL = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UImage* MarginR = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		UTexture2D* MarginTTexture = UISprite->GetFrame(0 * 3 + 1);
		UTexture2D* MarginBTexture = UISprite->GetFrame(2 * 3 + 1);
		UTexture2D* MarginLTexture = UISprite->GetFrame(1 * 3 + 0);
		UTexture2D* MarginRTexture = UISprite->GetFrame(1 * 3 + 2);
		USlateBrushAsset* BrushAssetT = NewObject<USlateBrushAsset>();
		BrushAssetT->Brush.SetResourceObject(MarginTTexture);
		BrushAssetT->Brush.Tiling = ESlateBrushTileType::Horizontal;
		//BrushAssetT->Brush.SetImageSize(FVector2D(MarginTTexture->GetSizeX(), MarginTTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		MarginT->SetBrushFromAsset(BrushAssetT);
		USlateBrushAsset* BrushAssetB = NewObject<USlateBrushAsset>();
		BrushAssetB->Brush.SetResourceObject(MarginBTexture);
		BrushAssetB->Brush.Tiling = ESlateBrushTileType::Horizontal;
		//BrushAssetB->Brush.SetImageSize(FVector2D(MarginBTexture->GetSizeX(), MarginBTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		MarginB->SetBrushFromAsset(BrushAssetB);
		USlateBrushAsset* BrushAssetL = NewObject<USlateBrushAsset>();
		BrushAssetL->Brush.SetResourceObject(MarginLTexture);
		BrushAssetL->Brush.Tiling = ESlateBrushTileType::Vertical;
		//BrushAssetL->Brush.SetImageSize(FVector2D(MarginLTexture->GetSizeX(), MarginLTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		MarginL->SetBrushFromAsset(BrushAssetL);
		USlateBrushAsset* BrushAssetR = NewObject<USlateBrushAsset>();
		BrushAssetR->Brush.SetResourceObject(MarginRTexture);
		BrushAssetR->Brush.Tiling = ESlateBrushTileType::Vertical;
		//BrushAssetR->Brush.SetImageSize(FVector2D(MarginRTexture->GetSizeX(), MarginRTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		MarginR->SetBrushFromAsset(BrushAssetR);
		UCanvasPanel* MarginRCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
		UCanvasPanelSlot* MarginRSlot = MarginRCanvas->AddChildToCanvas(MarginR);
		MarginRSlot->SetAnchors(FAnchors(0, 0, 0, 1));
		MarginRSlot->SetOffsets(FMargin(0));
		MarginRSlot->SetSize(FVector2D(MarginRTexture->GetSizeX() * UI_PIXEL_TO_UNIT, MarginRSlot->GetSize().Y));
		NineGrid->AddChildToGrid(MarginT, 0, 1);
		NineGrid->AddChildToGrid(MarginB, 2, 1);
		NineGrid->AddChildToGrid(MarginL, 1, 0);
		NineGrid->AddChildToGrid(MarginRCanvas, 1, 2);
	}
}
