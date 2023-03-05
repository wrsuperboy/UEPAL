// Copyright (C) 2022 Meizhouxuanhan.


#include "PALEquipItemMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/BackgroundBlur.h"
#include "Components/BackgroundBlurSlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Slate/SlateBrushAsset.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALGameStateData.h"
#include "PALPlayerController.h"
#include "PALCommon.h"
#include "PALScriptManager.h"
#include "PAL.h"

void UPALEquipItemMenu::Init(int16 Item)
{
	CurrentItem = Item;
}

void UPALEquipItemMenu::ChangeRole(SIZE_T RoleId)
{
	// Draw the current equipment of the selected player
	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	UPALPlayerStateData* PlayerStateData = PlayerState->GetPlayerStateData();
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	for (SIZE_T i = 0; i < MAX_PLAYER_EQUIPMENTS; i++)
	{
		if (PlayerStateData->PlayerRoles.Equipments[i][RoleId] != 0)
		{
			EquipNameTexts[i]->SetText(FText::FromString(Common->GetWord(PlayerStateData->PlayerRoles.Equipments[i][RoleId])));
		}
		else
		{
			EquipNameTexts[i]->SetText(FText());
		}
	}

	StatTexts[0]->SetText(FText::FromString(FString::FromInt(PlayerState->GetRoleAttackStrength(RoleId))));
	StatTexts[1]->SetText(FText::FromString(FString::FromInt(PlayerState->GetRoleMagicStrength(RoleId))));
	StatTexts[2]->SetText(FText::FromString(FString::FromInt(PlayerState->GetRoleDefense(RoleId))));
	StatTexts[3]->SetText(FText::FromString(FString::FromInt(PlayerState->GetRoleDexterity(RoleId))));
	StatTexts[4]->SetText(FText::FromString(FString::FromInt(PlayerState->GetRoleFleeRate(RoleId))));
}

void UPALEquipItemMenu::SelectRole(SIZE_T RoleId)
{
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	UPALPlayerStateData* PlayerStateData = PlayerState->GetPlayerStateData();
	
	// Get the equip part
	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	FScriptEntry* Script = &(GameData->ScriptEntries[GameStateData->Objects[CurrentItem].Item.ScriptOnEquip]);
	int16 UnequippingItem = -1;
	if (Script->Operation == 0x0018)
	{
		EPALBodyPart i = static_cast<EPALBodyPart>(Script->Operand[0] - 0x0B);
		UnequippingItem = PlayerStateData->PlayerRoles.Equipments[i][RoleId];
	}
	else
	{
		PAL_DebugMsg("Equip item script operation is not 0x0018");
	}
	// Run the equip script
	bool bSuccess = false;
	GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Objects[CurrentItem].Item.ScriptOnEquip, RoleId, true, bSuccess);
	if (bSuccess)
	{
		if (UnequippingItem == 0)
		{
			FSlateBrush SlatBrushNone;
			SlatBrushNone.DrawAs = ESlateBrushDrawType::NoDrawType;
			ItemImage->SetBrush(SlatBrushNone);
			ItemCountText->SetText(FText::GetEmpty());
			ItemNameText->SetText(FText::GetEmpty());
			RoleSelectMenu->ClearRoles();
			for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
			{
				RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerState, false);
			}
			RoleSelectMenu->OnRoleChanged.BindUObject(this, &UPALEquipItemMenu::ChangeRole);
			return;
		}
		if (UnequippingItem > 0)
		{
			CurrentItem = UnequippingItem;
			RoleSelectMenu->ClearRoles();
			for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
			{
				bool bSelectable = GameStateData->Objects[CurrentItem].Item.Flags & (EPALItemFlag::ItemFlagEquipableByPlayerRole_First << RoleData->RoleId);
				RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerState, bSelectable);
			}
			RoleSelectMenu->OnRoleChanged.BindUObject(this, &UPALEquipItemMenu::ChangeRole);
			RoleSelectMenu->OnRoleSelected.BindUObject(this, &UPALEquipItemMenu::SelectRole);
			UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
			UTexture2D* ItemImageTexture = Common->GetItemImage(GameStateData->Objects[CurrentItem].Item.Bitmap);
			if (ItemImageTexture)
			{
				Cast<UCanvasPanelSlot>(ItemImage->Slot)->SetSize(FVector2D(ItemImageTexture->GetSizeX(), ItemImageTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
				ItemImage->SetBrushFromTexture(ItemImageTexture);
			}
			ItemCountText->SetText(FText::FromString(FString::FromInt(PlayerState->GetItemAmount(CurrentItem))));
			ItemNameText->SetText(FText::FromString(Common->GetWord(CurrentItem)));
		}
	}
}

TSharedRef<SWidget> UPALEquipItemMenu::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UBackgroundBlur>(UBackgroundBlur::StaticClass());
	}

	return Super::RebuildWidget();
}

void UPALEquipItemMenu::NativeConstruct()
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	UBackgroundBlur* RootWidget = Cast<UBackgroundBlur>(GetRootWidget());
	RootWidget->SetBlurStrength(10);
	USizeBox* BorderSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	BorderSize->SetWidthOverride(320 * UI_PIXEL_TO_UNIT);
	BorderSize->SetHeightOverride(200 * UI_PIXEL_TO_UNIT);
	UBackgroundBlurSlot* BorderSizeSlot = Cast<UBackgroundBlurSlot>(RootWidget->AddChild(BorderSize));
	BorderSizeSlot->SetPadding(FMargin(0));
	BorderSizeSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	BorderSizeSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
	Border->SetBrushFromTexture(Common->GetBackgroundPicture(1));
	BorderSize->AddChild(Border);
	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
	Border->AddChild(Canvas);

	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	ItemImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UCanvasPanelSlot* ItemImageSlot = Canvas->AddChildToCanvas(ItemImage);
	ItemImageSlot->SetAnchors(FAnchors(40. / 320., 40. / 200., 40. / 320., 40. / 200.));
	ItemImageSlot->SetAlignment(FVector2D(0.5, 0.5));
	ItemCountText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemCountText->SetColorAndOpacity(FColor(80, 184, 148));
	ItemCountText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemCountText->SetShadowColorAndOpacity(FLinearColor::Black);
	UCanvasPanelSlot* ItemCountTextSlot = Canvas->AddChildToCanvas(ItemCountText);
	ItemCountTextSlot->SetAnchors(FAnchors(54. / 320., 53. / 200., 54. / 320., 53. / 200.));
	ItemCountTextSlot->SetAlignment(FVector2D(0.5, 0.5));
	ItemCountTextSlot->SetOffsets(FMargin(0));
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	ItemNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	ItemNameText->SetFont(FontInfo);
	ItemNameText->SetColorAndOpacity(Common->GetDefaultPalette()[44]);
	ItemNameText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	ItemNameText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UCanvasPanelSlot* ItemNameTextSlot = Canvas->AddChildToCanvas(ItemNameText);
	ItemNameTextSlot->SetAnchors(FAnchors(5. / 320., 70. / 200., 5. / 320., 70. / 200.));
	ItemNameTextSlot->SetOffsets(FMargin(0));
	if (CurrentItem != 0)
	{
		UTexture2D* ItemImageTexture = Common->GetItemImage(GameStateData->Objects[CurrentItem].Item.Bitmap);
		if (ItemImageTexture)
		{
			ItemImageSlot->SetSize(FVector2D(ItemImageTexture->GetSizeX(), ItemImageTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
			ItemImage->SetBrushFromTexture(ItemImageTexture);
		}
		ItemCountText->SetText(FText::FromString(FString::FromInt(PlayerState->GetItemAmount(CurrentItem))));
		ItemNameText->SetText(FText::FromString(Common->GetWord(CurrentItem)));
	}

	RoleSelectMenu = CreateWidget<UPALRoleSelectMenu>(GetOwningPlayer(), UPALRoleSelectMenu::StaticClass());
	for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
	{
		bool bSelectable = GameStateData->Objects[CurrentItem].Item.Flags & (EPALItemFlag::ItemFlagEquipableByPlayerRole_First << RoleData->RoleId);
		RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerState, bSelectable);
	}
	RoleSelectMenu->OnRoleChanged.BindUObject(this, &UPALEquipItemMenu::ChangeRole);
	RoleSelectMenu->OnRoleSelected.BindUObject(this, &UPALEquipItemMenu::SelectRole);

	UCanvasPanelSlot* RoleSelectMenuSlot = Canvas->AddChildToCanvas(RoleSelectMenu);
	RoleSelectMenuSlot->SetAnchors(FAnchors(2. / 320., 95. / 200., 2. / 320., 95. / 200.));
	RoleSelectMenuSlot->SetOffsets(FMargin(0));

	for (SIZE_T i = 0; i < MAX_PLAYER_EQUIPMENTS; i++)
	{
		UTextBlock* EquipNameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		EquipNameText->SetFont(FontInfo);
		EquipNameText->SetColorAndOpacity(Common->GetDefaultPalette()[79]);
		EquipNameText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		EquipNameText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		UCanvasPanelSlot* EquipNameTextSlot = Canvas->AddChildToCanvas(EquipNameText);
		EquipNameTextSlot->SetAnchors(FAnchors(130. / 320., (22. * i + 19.) / 200., 130. / 320., (22. * i + 19.) / 200.));
		EquipNameTextSlot->SetAlignment(FVector2D(0, 0.5));
		EquipNameTextSlot->SetOffsets(FMargin(0));
		EquipNameTextSlot->SetAutoSize(true);
		EquipNameTexts.Add(EquipNameText);
	}

	for (SIZE_T i = 0; i < 5; i++)
	{
		UTextBlock* StatText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		StatText->SetColorAndOpacity(FColor(80, 184, 148));
		StatText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		StatText->SetShadowColorAndOpacity(FLinearColor::Black);
		UCanvasPanelSlot* StatTextSlot = Canvas->AddChildToCanvas(StatText);
		StatTextSlot->SetAnchors(FAnchors(262. / 320., (22. * i + 18.) / 200., 262. / 320., (22. * i + 18.) / 200.));
		StatTextSlot->SetAlignment(FVector2D(0., 0.5));
		StatTextSlot->SetOffsets(FMargin(0));
		StatTextSlot->SetAutoSize(true);
		StatTexts.Add(StatText);
	}
}