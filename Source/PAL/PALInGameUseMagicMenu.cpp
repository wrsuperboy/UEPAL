// Copyright (C) 2022 Meizhouxuanhan.


#include "PALInGameUseMagicMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/GridPanel.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/SizeBox.h"
#include "Components/ScrollBox.h"
#include "Components/UniformGridSlot.h"
#include "Slate/SlateBrushAsset.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALPlayerState.h"
#include "PALRoleSelectMenu.h"
#include "PALRoleInfoBox.h"
#include "PALPlayerController.h"
#include "PALScriptManager.h"
#include "PALCommon.h"
#include "PAL.h"

void UPALInGameUseMagicMenu::SelectRole(SIZE_T InRoleId)
{
	check(Status == EStatus::ROLE_UNSELECTED);
	Status = ROLE_SELECTED;
	SelectedRoleId = InRoleId;
	RefreshMagic();
	if (RoleSelectMenu)
	{
		RoleSelectMenu->SetVisibility(ESlateVisibility::Hidden);
	}
	if (NineGrid)
	{
		NineGrid->SetVisibility(ESlateVisibility::Visible);
	}
	if (RoleInfoBoxArray)
	{
		RoleInfoBoxArray->SetVisibility(ESlateVisibility::Visible);
	}
	if (MetaBox)
	{

		MetaBox->SetVisibility(ESlateVisibility::Visible);
	}
}

void UPALInGameUseMagicMenu::RefreshMagic()
{
	if (Status == EStatus::ROLE_UNSELECTED)
	{
		return;
	}

	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	UPALPlayerStateData* PlayerStateData = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	MenuItemList.Empty();
	for (int32 i = 0; i < MAX_PLAYER_MAGICS; i++)
	{
		int16 Magic = PlayerStateData->PlayerRoles.Magics[i][SelectedRoleId];
		if (Magic != 0)
		{
			uint16 MagicNumber = GameStateData->Objects[Magic].Magic.MagicNumber;
			uint16 MP = GameData->Magics[MagicNumber].CostMP;
			bool bEnabled = MP <= PlayerStateData->PlayerRoles.MP[SelectedRoleId];

			uint16 Flags = GameStateData->Objects[Magic].Magic.Flags;
			if (GameStateData->bInBattle)
			{
				if (!(Flags & EMagicFlag::MagicFlagUsableInBattle))
				{
					bEnabled = false;
				}
			}
			else
			{
				if (!(Flags & EMagicFlag::MagicFlagUsableOutsideBattle))
				{
					bEnabled = false;
				}
			}

			UPALUseMagicMenuItem* MagicItem = CreateWidget<UPALUseMagicMenuItem>(this, UPALUseMagicMenuItem::StaticClass());
			MagicItem->Init(Magic, Common->GetWord(Magic), MP, bEnabled);
			MagicItem->OnSelected.BindUObject(this, &UPALInGameUseMagicMenu::SelectMagic);
			MagicItem->OnHovered.BindUObject(this, &UPALInGameUseMagicMenu::ChangeMagic);
			MenuItemList.Add(MagicItem);
		}
	}

	MenuItemList.Sort([](const UPALUseMagicMenuItem& A, const UPALUseMagicMenuItem& B) {
		return A.GetMagic() < B.GetMagic();
	});

	if (ItemContainer) {
		ItemContainer->ClearChildren();
		SIZE_T Count = 0;
		for (UPALUseMagicMenuItem*& MenuItem : MenuItemList)
		{
			ItemContainer->AddChildToUniformGrid(MenuItem, Count / ItemsPerLine, Count % ItemsPerLine);
			Count++;
		}
	}
}

void UPALInGameUseMagicMenu::SelectMagic(uint16 Magic)
{
	PAL_DebugMsg("In-game use magic menu selected magic");
	NineGrid->SetIsEnabled(false);
	SelectedMagic = Magic;
	Status = EStatus::MAGIC_SELECTED;

	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	UPALPlayerStateData* PlayerStateData = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	if (GameStateData->Objects[Magic].Magic.Flags & EMagicFlag::MagicFlagApplyToAll)
	{
		bool bScriptSuccess = false;
		GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[Magic].Magic.ScriptOnUse, 0, true, bScriptSuccess);

		if (bScriptSuccess)
		{
			GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[Magic].Magic.ScriptOnSuccess, 0, true, bScriptSuccess);

			if (bScriptSuccess)
			{
				uint16 MagicNumber = GameStateData->Objects[Magic].Magic.MagicNumber;
				uint16 MP = GameData->Magics[MagicNumber].CostMP;
				PlayerStateData->PlayerRoles.MP[SelectedRoleId] -= MP;
			}
		}
		ChangeMagic(Magic);
		Status = EStatus::ROLE_SELECTED;
		RefreshMagic();
		NineGrid->SetIsEnabled(true);
		for (UWidget*& Widget : RoleInfoBoxArray->GetAllChildren())
		{
			UPALRoleInfoBox* RoleInfoBox = Cast<UPALRoleInfoBox>(Widget);
			RoleInfoBox->SetSelectionEnabled(false);
			RoleInfoBox->Refresh();
		}
		return;
	}

	for (UWidget*& Widget : RoleInfoBoxArray->GetAllChildren())
	{
		UPALRoleInfoBox* RoleInfoBox = Cast<UPALRoleInfoBox>(Widget);
		RoleInfoBox->SetSelectionEnabled(true);
	}
}

void UPALInGameUseMagicMenu::ChangeMagic(uint16 Magic)
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	if (Common->GetGameDistribution() != EGameDistribution::DOS)
	{
		FString DescriptionString;
		UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
		// Why not .Magic but .Item? Strange.
		uint16 ScriptEntry = GameStateData->Objects[Magic].Item.ScriptDesc;
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


	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALPlayerStateData* PlayerStateData = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	uint16 MagicNumber = GameStateData->Objects[Magic].Magic.MagicNumber;
	uint16 MP = GameData->Magics[MagicNumber].CostMP;
	uint16 RoleMP = PlayerStateData->PlayerRoles.MP[SelectedRoleId];
	MagicMPNumber->SetText(FText::FromString(FString::FromInt(MP)));
	RoleMPNumber->SetText(FText::FromString(FString::FromInt(RoleMP)));
}

void UPALInGameUseMagicMenu::SelectTarget(SIZE_T InRoleId)
{
	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	UPALPlayerStateData* PlayerStateData = GetOwningPlayer()->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
	bool bScriptSuccess = false;
	GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[SelectedMagic].Magic.ScriptOnUse, static_cast<uint16>(InRoleId), true, bScriptSuccess);

	if (bScriptSuccess)
	{
		GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[SelectedMagic].Magic.ScriptOnSuccess, static_cast<uint16>(InRoleId), true, bScriptSuccess);

		if (bScriptSuccess)
		{
			uint16 MagicNumber = GameStateData->Objects[SelectedMagic].Magic.MagicNumber;
			uint16 MP = GameData->Magics[MagicNumber].CostMP;
			PlayerStateData->PlayerRoles.MP[SelectedRoleId] -= MP;

			// Check if we have run out of MP
			if (PlayerStateData->PlayerRoles.MP[SelectedRoleId] < MP)
			{
				Status = EStatus::ROLE_SELECTED;
				NineGrid->SetIsEnabled(true);
				for (UWidget*& Widget : RoleInfoBoxArray->GetAllChildren())
				{
					UPALRoleInfoBox* RoleInfoBox = Cast<UPALRoleInfoBox>(Widget);
					RoleInfoBox->SetSelectionEnabled(false);
					RoleInfoBox->Refresh();
				}
			}
		}
	}

	ChangeMagic(SelectedMagic);
	RefreshMagic();
	for (UWidget*& Widget : RoleInfoBoxArray->GetAllChildren())
	{
		UPALRoleInfoBox* RoleInfoBox = Cast<UPALRoleInfoBox>(Widget);
		RoleInfoBox->Refresh();
	}
}

void UPALInGameUseMagicMenu::NativeConstruct()
{
	Super::NativeConstruct();

	RoleSelectMenu = CreateWidget<UPALRoleSelectMenu>(GetOwningPlayer(), UPALRoleSelectMenu::StaticClass());
	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
	{
		RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerState, true);
	}
	RoleSelectMenu->OnRoleSelected.BindUObject(this, &UPALInGameUseMagicMenu::SelectRole);

	UCanvasPanelSlot* RoleSelectMenuSlot = Canvas->AddChildToCanvas(RoleSelectMenu);
	RoleSelectMenuSlot->SetAnchors(FAnchors(0, 0, 0, 0));
	RoleSelectMenuSlot->SetPosition(FVector2D(35, 62) * UI_PIXEL_TO_UNIT);
	RoleSelectMenuSlot->SetAutoSize(true);

	if (Status != EStatus::ROLE_UNSELECTED)
	{
		RoleSelectMenu->SetVisibility(ESlateVisibility::Hidden);
		RefreshMagic();
	}

	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();

	USizeBox* MetaSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	MetaSize->SetHeightOverride(100 * UI_PIXEL_TO_UNIT);
	MetaSize->SetMinDesiredWidth(340 * UI_PIXEL_TO_UNIT);
	UCanvasPanelSlot* MetaSizeSlot = Canvas->AddChildToCanvas(MetaSize);
	MetaSizeSlot->SetAnchors(FAnchors(0.5, 0, 0.5, 0));
	MetaSizeSlot->SetAlignment(FVector2D(0.5, 0));
	MetaSizeSlot->SetAutoSize(true);
	MetaSizeSlot->SetPosition(FVector2D(0, 3) * UI_PIXEL_TO_UNIT);
	MetaBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	MetaSize->AddChild(MetaBox);

	
	USizeBox* MPSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	MPSizeBox->SetHeightOverride(34 * UI_PIXEL_TO_UNIT);
	MPSizeBox->SetWidthOverride(96 * UI_PIXEL_TO_UNIT);
	UHorizontalBoxSlot* MPSizeBoxSlot = MetaBox->AddChildToHorizontalBox(MPSizeBox);
	MPSizeBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Left);
	MPSizeBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);
	UHorizontalBox* MPBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	MPSizeBox->AddChild(MPBox);
	UPALSprite* UISprite = Common->GetUISprite();
	UImage* LeftMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* LeftMarginTexture = UISprite->GetFrame(44);
	LeftMargin->SetBrushFromTexture(LeftMarginTexture);
	LeftMargin->SetDesiredSizeOverride(FVector2D(LeftMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, LeftMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* LeftMarginSlot = MPBox->AddChildToHorizontalBox(LeftMargin);
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
	UHorizontalBoxSlot* MiddleSlot = MPBox->AddChildToHorizontalBox(Middle);
	MiddleSlot->SetPadding(FMargin(0));
	MiddleSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	MiddleSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	MiddleSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	UCanvasPanel* Label = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	UBorderSlot* LabelSlot = Cast<UBorderSlot>(Middle->AddChild(Label));
	LabelSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	LabelSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	UTexture2D* SlashTexture = UISprite->GetFrame(39);
	UImage* MPSlash = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	MPSlash->SetBrushFromTexture(SlashTexture);
	UCanvasPanelSlot* MPSlashSlot = Label->AddChildToCanvas(MPSlash);
	MPSlashSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	MPSlashSlot->SetAlignment(FVector2D(0.5, 0.5));
	MPSlashSlot->SetSize(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
	MagicMPNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	MagicMPNumber->SetColorAndOpacity(FColor(212, 200, 168));
	MagicMPNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	MagicMPNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* MagicMPNumberSlot = Label->AddChildToCanvas(MagicMPNumber);
	MagicMPNumberSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	MagicMPNumberSlot->SetAlignment(FVector2D(1, 0.5));
	MagicMPNumberSlot->SetPosition(FVector2D(-6, 0) * UI_PIXEL_TO_UNIT);
	MagicMPNumberSlot->SetAutoSize(true);
	RoleMPNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	RoleMPNumber->SetColorAndOpacity(FColor(80, 184, 148));
	RoleMPNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	RoleMPNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* RoleMPNumberSlot = Label->AddChildToCanvas(RoleMPNumber);
	RoleMPNumberSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	RoleMPNumberSlot->SetAlignment(FVector2D(0, 0.5));
	RoleMPNumberSlot->SetPosition(FVector2D(6, 0) * UI_PIXEL_TO_UNIT);
	RoleMPNumberSlot->SetAutoSize(true);
	UImage* RightMargin = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UTexture2D* RightMarginTexture = UISprite->GetFrame(46);
	RightMargin->SetBrushFromTexture(RightMarginTexture);
	RightMargin->SetDesiredSizeOverride(FVector2D(RightMarginTexture->GetSizeX() * UI_PIXEL_TO_UNIT, RightMarginTexture->GetSizeY() * UI_PIXEL_TO_UNIT));
	UHorizontalBoxSlot* RightMarginSlot = MPBox->AddChildToHorizontalBox(RightMargin);
	RightMarginSlot->SetPadding(FMargin(0));
	RightMarginSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
	RightMarginSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	RightMarginSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	DescriptionText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	DescriptionText->SetFont(FontInfo);
	DescriptionText->SetColorAndOpacity(Common->GetDefaultPalette()[46]);
	DescriptionText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	DescriptionText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UHorizontalBoxSlot* DescriptionTextSlot = MetaBox->AddChildToHorizontalBox(DescriptionText);
	DescriptionTextSlot->SetPadding(FMargin(5, 0, 0, 0) * UI_PIXEL_TO_UNIT);
	DescriptionTextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	DescriptionTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	DescriptionTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	NineGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
	UCanvasPanelSlot* NineGridSlot = Canvas->AddChildToCanvas(NineGrid);
	NineGridSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	NineGridSlot->SetAlignment(FVector2D(0.5, 0.5));
	NineGridSlot->SetAutoSize(true);

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
	for (UPALUseMagicMenuItem*& MenuItem : MenuItemList)
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

	RoleInfoBoxArray = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
	{
		UPALRoleInfoBox* RoleInfoBox = CreateWidget<UPALRoleInfoBox>(GetOwningPlayer(), UPALRoleInfoBox::StaticClass());
		RoleInfoBox->Init(RoleData->RoleId);
		RoleInfoBox->SetSelectionEnabled(false);
		RoleInfoBox->SetPadding(FMargin(0, 0, 3, 0) * UI_PIXEL_TO_UNIT);
		RoleInfoBox->OnSelected.BindUObject(this, &UPALInGameUseMagicMenu::SelectTarget);
		RoleInfoBoxArray->AddChildToHorizontalBox(RoleInfoBox);
	}
	UCanvasPanelSlot* RoleInfoBoxArraySlot = Canvas->AddChildToCanvas(RoleInfoBoxArray);
	RoleInfoBoxArraySlot->SetAlignment(FVector2D(0.5, 1));
	RoleInfoBoxArraySlot->SetAnchors(FAnchors(0.5, 1, 0.5, 1));
	RoleInfoBoxArraySlot->SetAutoSize(true);
}

bool UPALInGameUseMagicMenu::GoBack()
{
	if (Status == EStatus::ROLE_SELECTED)
	{
		Status = EStatus::ROLE_UNSELECTED;
		RoleSelectMenu->SetVisibility(ESlateVisibility::Visible);
		NineGrid->SetVisibility(ESlateVisibility::Hidden);
		RoleInfoBoxArray->SetVisibility(ESlateVisibility::Hidden);
		MetaBox->SetVisibility(ESlateVisibility::Hidden);
		return true;
	}
	else if (Status == EStatus::MAGIC_SELECTED)
	{
		Status = EStatus::ROLE_SELECTED;
		for (UWidget*& Widget : RoleInfoBoxArray->GetAllChildren())
		{
			UPALRoleInfoBox* RoleInfoBox = Cast<UPALRoleInfoBox>(Widget);
			RoleInfoBox->SetSelectionEnabled(false);
		}
		NineGrid->SetIsEnabled(true);
		return true;
	}

	return false;
}
