// Copyright (C) 2022 Meizhouxuanhan.


#include "PALItemUseMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/Border.h"
#include "Slate/SlateBrushAsset.h"
#include "PALGameState.h"
#include "PALGameStateData.h"
#include "PALCommon.h"

void UPALItemUseMenu::Init(APALPlayerState* PlayerState, int16 InItem)
{
	PlayerStatePrivate = PlayerState;
	Item = InItem;
}

void UPALItemUseMenu::ChangeRole(SIZE_T RoleId)
{
	UPALPlayerStateData* PlayerStateData = PlayerStatePrivate->GetPlayerStateData();
	LevelNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.Level[RoleId])));
	MaxHPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.MaxHP[RoleId])));
	HPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.HP[RoleId])));
	MaxMPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.MaxMP[RoleId])));
	MPNumber->SetText(FText::FromString(FString::FromInt(PlayerStateData->PlayerRoles.MP[RoleId])));
	AttackStrengthNumber->SetText(FText::FromString(FString::FromInt(PlayerStatePrivate->GetRoleAttackStrength(RoleId))));
	MagicStrengthNumber->SetText(FText::FromString(FString::FromInt(PlayerStatePrivate->GetRoleMagicStrength(RoleId))));
	DefenseNumber->SetText(FText::FromString(FString::FromInt(PlayerStatePrivate->GetRoleMagicStrength(RoleId))));
	DexterityNumber->SetText(FText::FromString(FString::FromInt(PlayerStatePrivate->GetRoleDexterity(RoleId))));
	FleeRateNumber->SetText(FText::FromString(FString::FromInt(PlayerStatePrivate->GetRoleFleeRate(RoleId))));
}

void UPALItemUseMenu::SelectRole(SIZE_T RoleId)
{
	PlayerStatePrivate->UseItem(RoleId, Item);
	SIZE_T ItemCount = PlayerStatePrivate->GetItemAmount(Item);

	if (ItemCount == 0)
	{
		RoleSelectMenu->ClearRoles();
		for (UPALRoleData* RoleData : PlayerStatePrivate->GetPlayerStateData()->Party)
		{
			RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerStatePrivate, false);
		}
		FSlateBrush SlatBrushNone;
		SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
		ItemImage->SetBrush(SlatBrushNone);
		ItemCountText->SetText(FText::GetEmpty());
	}
	else
	{
		ItemCountText->SetText(FText::FromString(FString::FromInt(PlayerStatePrivate->GetItemAmount(Item))));
		ChangeRole(RoleId);
	}
}

UWidget* UPALItemUseMenu::BuildContent()
{
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();

	UTexture2D* ItemBoxTexture = Common->GetUISprite()->GetFrame(70);
	USizeBox* ItemBoxSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ItemBoxSize->SetWidthOverride(ItemBoxTexture->GetSizeX() * UI_PIXEL_TO_UNIT);
	ItemBoxSize->SetHeightOverride(ItemBoxTexture->GetSizeY() * UI_PIXEL_TO_UNIT);
	UCanvasPanelSlot* ItemBoxSizeSlot = Canvas->AddChildToCanvas(ItemBoxSize);
	ItemBoxSizeSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	ItemBoxSizeSlot->SetAlignment(FVector2D(0, 1));
	ItemBoxSizeSlot->SetPosition(FVector2D(0, -15) * UI_PIXEL_TO_UNIT);
	ItemBoxSizeSlot->SetAutoSize(true);
	UBorder* ItemBox = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	ItemBox->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	ItemBox->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	ItemBox->SetPadding(FMargin(0));
	ItemBox->SetBrushFromTexture(ItemBoxTexture);
	ItemBoxSize->AddChild(ItemBox);
	UCanvasPanel* ItemBoxCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	ItemBox->AddChild(ItemBoxCanvas);
	ItemImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UCanvasPanelSlot* ItemImageSlot = ItemBoxCanvas->AddChildToCanvas(ItemImage);
	ItemImageSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	ItemImageSlot->SetAlignment(FVector2D(0.5, 0.5));
	UTexture2D* ItemImageTexture = Common->GetItemImage(GameStateData->Objects[Item].Item.Bitmap);
	if (ItemImageTexture)
	{
		ItemImageSlot->SetSize(FVector2D(ItemImageTexture->GetSizeX(), ItemImageTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		ItemImage->SetBrushFromTexture(ItemImageTexture);
	}

	ItemCountText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemCountText->SetColorAndOpacity(FColor(80, 184, 148));
	ItemCountText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemCountText->SetShadowColorAndOpacity(FLinearColor::Black);
	UCanvasPanelSlot* ItemCountTextSlot = Canvas->AddChildToCanvas(ItemCountText);
	ItemCountTextSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	ItemCountTextSlot->SetAlignment(FVector2D(0.5, 0.5));
	ItemCountTextSlot->SetPosition(FVector2D(52, -23) * UI_PIXEL_TO_UNIT);
	ItemCountTextSlot->SetAutoSize(true);
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	UTextBlock* ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemNameText->SetFont(FontInfo);
	ItemNameText->SetColorAndOpacity(Common->GetDefaultPalette()[190]);
	ItemNameText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemNameText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* ItemNameTextSlot = Canvas->AddChildToCanvas(ItemNameText);
	ItemNameTextSlot->SetAnchors(FAnchors(0, 1, 0, 1));
	ItemNameTextSlot->SetAlignment(FVector2D(0, 1));
	ItemNameTextSlot->SetPosition(FVector2D(-2, 0) * UI_PIXEL_TO_UNIT);
	ItemNameTextSlot->SetAutoSize(true);
	ItemCountText->SetText(FText::FromString(FString::FromInt(PlayerStatePrivate->GetItemAmount(Item))));
	ItemNameText->SetText(FText::FromString(Common->GetWord(Item)));

	RoleSelectMenu = WidgetTree->ConstructWidget<UPALRoleSelectMenu>(UPALRoleSelectMenu::StaticClass());
	RoleSelectMenu->bHasBorder = false;
	UCanvasPanelSlot* RoleSelectMenuSlot = Canvas->AddChildToCanvas(RoleSelectMenu);
	RoleSelectMenuSlot->SetAnchors(FAnchors(0));

	for (UPALRoleData* RoleData : PlayerStatePrivate->GetPlayerStateData()->Party)
	{
		RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerStatePrivate, true);
	}
	RoleSelectMenu->OnRoleChanged.BindUObject(this, &UPALItemUseMenu::ChangeRole);
	RoleSelectMenu->OnRoleSelected.BindUObject(this, &UPALItemUseMenu::SelectRole);

	FVector2D LabelOffset(100 * UI_PIXEL_TO_UNIT, 0 * UI_PIXEL_TO_UNIT);
	UWidget* LevelLabelText = BuildLabelText(Common->GetWord(48));
	UCanvasPanelSlot* LevelLabelTextSlot = Canvas->AddChildToCanvas(LevelLabelText);
	LevelLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	LevelLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 8) * UI_PIXEL_TO_UNIT);
	LevelLabelTextSlot->SetAutoSize(true);
	UWidget* HPLabelText = BuildLabelText(Common->GetWord(49));
	UCanvasPanelSlot* HPLabelTextSlot = Canvas->AddChildToCanvas(HPLabelText);
	HPLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	HPLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 26) * UI_PIXEL_TO_UNIT);
	HPLabelTextSlot->SetAutoSize(true);
	UWidget* MPLabelText = BuildLabelText(Common->GetWord(50));
	UCanvasPanelSlot* MPLabelTextSlot = Canvas->AddChildToCanvas(MPLabelText);
	MPLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	MPLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 44) * UI_PIXEL_TO_UNIT);
	MPLabelTextSlot->SetAutoSize(true);
	UWidget* AttackStrengthLabelText = BuildLabelText(Common->GetWord(51));
	UCanvasPanelSlot* AttackStrengthLabelTextSlot = Canvas->AddChildToCanvas(AttackStrengthLabelText);
	AttackStrengthLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	AttackStrengthLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 62) * UI_PIXEL_TO_UNIT);
	AttackStrengthLabelTextSlot->SetAutoSize(true);
	UWidget* MagicStrengthLabelText = BuildLabelText(Common->GetWord(52));
	UCanvasPanelSlot* MagicStrengthLabelTextSlot = Canvas->AddChildToCanvas(MagicStrengthLabelText);
	MagicStrengthLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	MagicStrengthLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 80) * UI_PIXEL_TO_UNIT);
	MagicStrengthLabelTextSlot->SetAutoSize(true);
	UWidget* ResistenceLabelText = BuildLabelText(Common->GetWord(53));
	UCanvasPanelSlot* DefenseLabelTextSlot = Canvas->AddChildToCanvas(ResistenceLabelText);
	DefenseLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	DefenseLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 98) * UI_PIXEL_TO_UNIT);
	DefenseLabelTextSlot->SetAutoSize(true);
	UWidget* DexterityLabelText = BuildLabelText(Common->GetWord(54));
	UCanvasPanelSlot* DexterityLabelTextSlot = Canvas->AddChildToCanvas(DexterityLabelText);
	DexterityLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	DexterityLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 116) * UI_PIXEL_TO_UNIT);
	DexterityLabelTextSlot->SetAutoSize(true);
	UWidget* FleeRateLabelText = BuildLabelText(Common->GetWord(55));
	UCanvasPanelSlot* FleeRateLabelTextSlot = Canvas->AddChildToCanvas(FleeRateLabelText);
	FleeRateLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
	FleeRateLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 134) * UI_PIXEL_TO_UNIT);
	FleeRateLabelTextSlot->SetAutoSize(true);

	LevelNumber = BuildLabelNumber();
	UCanvasPanelSlot* LevelNumberSlot = Canvas->AddChildToCanvas(LevelNumber);
	LevelNumberSlot->SetAlignment(FVector2D(1, 0.5));
	LevelNumberSlot->SetPosition(LabelOffset + FVector2D(64, 8) * UI_PIXEL_TO_UNIT);
	LevelNumberSlot->SetAutoSize(true);
	HPNumber = BuildLabelNumber();
	UCanvasPanelSlot* HPNumberSlot = Canvas->AddChildToCanvas(HPNumber);
	HPNumberSlot->SetAlignment(FVector2D(1, 0.5));
	HPNumberSlot->SetPosition(LabelOffset + FVector2D(64, 26) * UI_PIXEL_TO_UNIT);
	HPNumberSlot->SetAutoSize(true);
	MPNumber = BuildLabelNumber();
	UCanvasPanelSlot* MPNumberSlot = Canvas->AddChildToCanvas(MPNumber);
	MPNumberSlot->SetAlignment(FVector2D(1, 0.5));
	MPNumberSlot->SetPosition(LabelOffset + FVector2D(64, 44) * UI_PIXEL_TO_UNIT);
	MPNumberSlot->SetAutoSize(true);
	AttackStrengthNumber = BuildLabelNumber();
	UCanvasPanelSlot* AttackStrengthNumberSlot = Canvas->AddChildToCanvas(AttackStrengthNumber);
	AttackStrengthNumberSlot->SetAlignment(FVector2D(1, 0.5));
	AttackStrengthNumberSlot->SetPosition(LabelOffset + FVector2D(64, 62) * UI_PIXEL_TO_UNIT);
	AttackStrengthNumberSlot->SetAutoSize(true);
	MagicStrengthNumber = BuildLabelNumber();
	UCanvasPanelSlot* MagicStrengthNumberSlot = Canvas->AddChildToCanvas(MagicStrengthNumber);
	MagicStrengthNumberSlot->SetAlignment(FVector2D(1, 0.5));
	MagicStrengthNumberSlot->SetPosition(LabelOffset + FVector2D(64, 80) * UI_PIXEL_TO_UNIT);
	MagicStrengthNumberSlot->SetAutoSize(true);
	DefenseNumber = BuildLabelNumber();
	UCanvasPanelSlot* ResistenceNumberSlot = Canvas->AddChildToCanvas(DefenseNumber);
	ResistenceNumberSlot->SetAlignment(FVector2D(1, 0.5));
	ResistenceNumberSlot->SetPosition(LabelOffset + FVector2D(64, 98) * UI_PIXEL_TO_UNIT);
	ResistenceNumberSlot->SetAutoSize(true);
	DexterityNumber = BuildLabelNumber();
	UCanvasPanelSlot* DexterityNumberSlot = Canvas->AddChildToCanvas(DexterityNumber);
	DexterityNumberSlot->SetAlignment(FVector2D(1, 0.5));
	DexterityNumberSlot->SetPosition(LabelOffset + FVector2D(64, 116) * UI_PIXEL_TO_UNIT);
	DexterityNumberSlot->SetAutoSize(true);
	FleeRateNumber = BuildLabelNumber();
	UCanvasPanelSlot* FleeRateNumberSlot = Canvas->AddChildToCanvas(FleeRateNumber);
	FleeRateNumberSlot->SetAlignment(FVector2D(1, 0.5));
	FleeRateNumberSlot->SetPosition(LabelOffset + FVector2D(64, 134) * UI_PIXEL_TO_UNIT);
	FleeRateNumberSlot->SetAutoSize(true);

	UPALSprite* UISprite = GetGameInstance()->GetSubsystem<UPALCommon>()->GetUISprite();
	UTexture2D* SlashTexture = UISprite->GetFrame(39);
	UImage* HPSlash = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	HPSlash->SetBrushFromTexture(SlashTexture);
	UCanvasPanelSlot* HPSlashSlot = Canvas->AddChildToCanvas(HPSlash);
	HPSlashSlot->SetPosition(LabelOffset + FVector2D(63, 23) * UI_PIXEL_TO_UNIT);
	HPSlashSlot->SetSize(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	UImage* MPSlash = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	MPSlash->SetBrushFromTexture(SlashTexture);
	MPSlash->SetDesiredSizeOverride(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	UCanvasPanelSlot* MPSlashSlot = Canvas->AddChildToCanvas(MPSlash);
	MPSlashSlot->SetPosition(LabelOffset + FVector2D(63, 41) * UI_PIXEL_TO_UNIT);
	MPSlashSlot->SetSize(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY())* UI_PIXEL_TO_UNIT);


	MaxHPNumber = BuildLabelMaxNumber();
	UCanvasPanelSlot* MaxHPNumberSlot = Canvas->AddChildToCanvas(MaxHPNumber);
	MaxHPNumberSlot->SetAlignment(FVector2D(1, 0.5));
	MaxHPNumberSlot->SetPosition(LabelOffset + FVector2D(87, 29) * UI_PIXEL_TO_UNIT);
	MaxHPNumberSlot->SetAutoSize(true);
	MaxMPNumber = BuildLabelMaxNumber();
	UCanvasPanelSlot* MaxMPNumberSlot = Canvas->AddChildToCanvas(MaxMPNumber);
	MaxMPNumberSlot->SetAlignment(FVector2D(1, 0.5));
	MaxMPNumberSlot->SetPosition(LabelOffset + FVector2D(87, 47) * UI_PIXEL_TO_UNIT);
	MaxMPNumberSlot->SetAutoSize(true);

	return Canvas;
}

UWidget* UPALItemUseMenu::BuildLabelText(const FString& Name)
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LabelText->SetFont(FontInfo);
	LabelText->SetColorAndOpacity(Common->GetDefaultPalette()[187]);
	LabelText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	LabelText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	LabelText->SetText(FText::FromString(Name));
	return LabelText;
}

UTextBlock* UPALItemUseMenu::BuildLabelNumber()
{
	UTextBlock* LabelNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LabelNumber->SetColorAndOpacity(FColor(212, 200, 168));
	LabelNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	LabelNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	return LabelNumber;
}

UTextBlock* UPALItemUseMenu::BuildLabelMaxNumber()
{
	UTextBlock* LabelNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LabelNumber->SetColorAndOpacity(FColor(120, 156, 232));
	LabelNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	LabelNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	return LabelNumber;
}

TSharedRef<SWidget> UPALItemUseMenu::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());;
	}

	return Super::RebuildWidget();
}

void UPALItemUseMenu::NativeConstruct()
{
	Super::NativeConstruct();
	UCanvasPanel* RootWidget = Cast<UCanvasPanel>(GetRootWidget());

	UGridPanel* NineGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
	UCanvasPanelSlot* NineGridSlot = RootWidget->AddChildToCanvas(NineGrid);
	NineGridSlot->SetAnchors(FAnchors(0.5, 0, 0.5, 0));
	NineGridSlot->SetAlignment(FVector2D(0.5, 0));
	NineGridSlot->SetAutoSize(true);
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
	
	USizeBox* ContentSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	ContentSizeBox->SetWidthOverride(169 * UI_PIXEL_TO_UNIT);
	ContentSizeBox->SetHeightOverride(134 * UI_PIXEL_TO_UNIT);
	NineGrid->AddChildToGrid(ContentSizeBox, 1, 1);
	UBorder* ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	UTexture2D* ContentTexture = UISprite->GetFrame(1 * 3 + 1);
	USlateBrushAsset* ContentBrushAsset = NewObject<USlateBrushAsset>();
	ContentBrushAsset->Brush.SetResourceObject(ContentTexture);
	ContentBrushAsset->Brush.Tiling = ESlateBrushTileType::Both;
	ContentBrushAsset->Brush.SetImageSize(FVector2D(ContentTexture->GetSizeX(), ContentTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	ContentBorder->SetBrushFromAsset(ContentBrushAsset);
	ContentSizeBox->AddChild(ContentBorder);

	ContentBorder->AddChild(BuildContent());
	ContentBorder->SetPadding(FMargin(-8) * UI_PIXEL_TO_UNIT);

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