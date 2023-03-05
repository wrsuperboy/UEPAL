// Copyright (C) 2023 Meizhouxuanhan.


#include "PALRoleStatusMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "PALPlayerController.h"
#include "PALPlayerState.h"
#include "PALPlayerStateData.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALCommon.h"

void UPALRoleStatusMenu::SelectRole(SIZE_T InRoleId)
{
	int32* Find = RoleIdIndexMap.Find(InRoleId);
	if (Find)
	{
		int32 Index = *Find;
		Switcher->SetActiveWidgetIndex(Index);
	}
}


void UPALRoleStatusMenu::NativeConstruct()
{
	Super::NativeConstruct();

	UHorizontalBox* Box = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	UCanvasPanelSlot* BoxSlot = Canvas->AddChildToCanvas(Box);
	BoxSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	BoxSlot->SetAlignment(FVector2D(0.5, 0.5));
	BoxSlot->SetAutoSize(true);

	RoleSelectMenu = CreateWidget<UPALRoleSelectMenu>(GetOwningPlayer(), UPALRoleSelectMenu::StaticClass());
	APALPlayerState* PlayerState = Cast<APALPlayerController>(GetOwningPlayer())->GetPlayerState<APALPlayerState>();
	for (UPALRoleData* RoleData : PlayerState->GetPlayerStateData()->Party)
	{
		RoleSelectMenu->AddPartyRole(RoleData->RoleId, PlayerState, true);
	}
	RoleSelectMenu->OnRoleSelected.BindUObject(this, &UPALRoleStatusMenu::SelectRole);
	UHorizontalBoxSlot* RoleSelectMenuSlot = Box->AddChildToHorizontalBox(RoleSelectMenu);
	RoleSelectMenuSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Top);

	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	USizeBox* BorderSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass());
	BorderSize->SetWidthOverride(320 * UI_PIXEL_TO_UNIT);
	BorderSize->SetHeightOverride(200 * UI_PIXEL_TO_UNIT);
	UHorizontalBoxSlot* BorderSizeSlot = Cast<UHorizontalBoxSlot>(Box->AddChild(BorderSize));
	BorderSizeSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	Switcher = WidgetTree->ConstructWidget<UWidgetSwitcher>(UWidgetSwitcher::StaticClass());
	BorderSize->AddChild(Switcher);

	UPALPlayerStateData* PlayerStateData = PlayerState->GetPlayerStateData();
	UPALGameData* GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	UPALGameStateData* GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	UPALSprite* UISprite = Common->GetUISprite();
	for (int32 Index = 0; Index < PlayerState->GetPlayerStateData()->Party.Num(); Index++)
	{
		UPALRoleData* RoleData = PlayerState->GetPlayerStateData()->Party[Index];
		SIZE_T RoleId = RoleData->RoleId;
		UBorder* Border = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
		Border->SetBrushFromTexture(Common->GetBackgroundPicture(0));
		Switcher->AddChild(Border);
		RoleIdIndexMap.Add(TTuple<SIZE_T, int32>(RoleId, Index));

		UCanvasPanel* BorderCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass());
		Border->AddChild(BorderCanvas);

		int16 CurrentExp = PlayerStateData->ExpAll.PrimaryExp[RoleId].Exp;
		uint16 NextLevelExp = GameData->LevelUpExps[PlayerStateData->PlayerRoles.Level[RoleId]];
		UWidget* ExpLabelText = BuildLabelText(Common->GetWord(2));
		UCanvasPanelSlot* ExpLabelTextSlot = BorderCanvas->AddChildToCanvas(ExpLabelText);
		ExpLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		ExpLabelTextSlot->SetPosition(FVector2D(6, 14) * UI_PIXEL_TO_UNIT);
		ExpLabelTextSlot->SetAutoSize(true);
		UTextBlock* CurrentExpNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		CurrentExpNumber->SetColorAndOpacity(FColor(212, 200, 168));
		CurrentExpNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		CurrentExpNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		CurrentExpNumber->SetText(FText::FromString(FString::FromInt(CurrentExp)));
		UCanvasPanelSlot* CurrentExpNumberSlot = BorderCanvas->AddChildToCanvas(CurrentExpNumber);
		CurrentExpNumberSlot->SetAlignment(FVector2D(1, 0.5));
		CurrentExpNumberSlot->SetPosition(FVector2D(72, 14) * UI_PIXEL_TO_UNIT);
		CurrentExpNumberSlot->SetAutoSize(true);
		UTextBlock* NextExpNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		NextExpNumber->SetColorAndOpacity(FColor(80, 184, 148));
		NextExpNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
		NextExpNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		NextExpNumber->SetText(FText::FromString(FString::FromInt(NextLevelExp)));
		UCanvasPanelSlot* NextExpNumberSlot = BorderCanvas->AddChildToCanvas(NextExpNumber);
		NextExpNumberSlot->SetAlignment(FVector2D(1, 0.5));
		NextExpNumberSlot->SetPosition(FVector2D(72, 21) * UI_PIXEL_TO_UNIT);
		NextExpNumberSlot->SetAutoSize(true);
		UProgressBar* ExpProgress = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass());
		ExpProgress->SetPercent(static_cast<float>(CurrentExp) / NextLevelExp);
		ExpProgress->SetBarFillType(EProgressBarFillType::BottomToTop);
		UCanvasPanelSlot* ExpProgressSlot = BorderCanvas->AddChildToCanvas(ExpProgress);
		ExpProgress->SetFillColorAndOpacity(FColor(80, 184, 148));
		ExpProgressSlot->SetAlignment(FVector2D(0, 0.5));
		ExpProgressSlot->SetPosition(FVector2D(72, 17) * UI_PIXEL_TO_UNIT);
		ExpProgressSlot->SetSize(FVector2D(16, 16) * UI_PIXEL_TO_UNIT);

		FVector2D LabelOffset(6 * UI_PIXEL_TO_UNIT, 32 * UI_PIXEL_TO_UNIT);
		UWidget* LevelLabelText = BuildLabelText(Common->GetWord(48));
		UCanvasPanelSlot* LevelLabelTextSlot = BorderCanvas->AddChildToCanvas(LevelLabelText);
		LevelLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		LevelLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 8) * UI_PIXEL_TO_UNIT);
		LevelLabelTextSlot->SetAutoSize(true);
		UWidget* HPLabelText = BuildLabelText(Common->GetWord(49));
		UCanvasPanelSlot* HPLabelTextSlot = BorderCanvas->AddChildToCanvas(HPLabelText);
		HPLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		HPLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 26) * UI_PIXEL_TO_UNIT);
		HPLabelTextSlot->SetAutoSize(true);
		UWidget* MPLabelText = BuildLabelText(Common->GetWord(50));
		UCanvasPanelSlot* MPLabelTextSlot = BorderCanvas->AddChildToCanvas(MPLabelText);
		MPLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		MPLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 44) * UI_PIXEL_TO_UNIT);
		MPLabelTextSlot->SetAutoSize(true);
		UWidget* AttackStrengthLabelText = BuildLabelText(Common->GetWord(51));
		UCanvasPanelSlot* AttackStrengthLabelTextSlot = BorderCanvas->AddChildToCanvas(AttackStrengthLabelText);
		AttackStrengthLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		AttackStrengthLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 62) * UI_PIXEL_TO_UNIT);
		AttackStrengthLabelTextSlot->SetAutoSize(true);
		UWidget* MagicStrengthLabelText = BuildLabelText(Common->GetWord(52));
		UCanvasPanelSlot* MagicStrengthLabelTextSlot = BorderCanvas->AddChildToCanvas(MagicStrengthLabelText);
		MagicStrengthLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		MagicStrengthLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 80) * UI_PIXEL_TO_UNIT);
		MagicStrengthLabelTextSlot->SetAutoSize(true);
		UWidget* ResistenceLabelText = BuildLabelText(Common->GetWord(53));
		UCanvasPanelSlot* DefenseLabelTextSlot = BorderCanvas->AddChildToCanvas(ResistenceLabelText);
		DefenseLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		DefenseLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 98) * UI_PIXEL_TO_UNIT);
		DefenseLabelTextSlot->SetAutoSize(true);
		UWidget* DexterityLabelText = BuildLabelText(Common->GetWord(54));
		UCanvasPanelSlot* DexterityLabelTextSlot = BorderCanvas->AddChildToCanvas(DexterityLabelText);
		DexterityLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		DexterityLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 116) * UI_PIXEL_TO_UNIT);
		DexterityLabelTextSlot->SetAutoSize(true);
		UWidget* FleeRateLabelText = BuildLabelText(Common->GetWord(55));
		UCanvasPanelSlot* FleeRateLabelTextSlot = BorderCanvas->AddChildToCanvas(FleeRateLabelText);
		FleeRateLabelTextSlot->SetAlignment(FVector2D(0, 0.5));
		FleeRateLabelTextSlot->SetPosition(LabelOffset + FVector2D(0, 134) * UI_PIXEL_TO_UNIT);
		FleeRateLabelTextSlot->SetAutoSize(true);

		UTextBlock* LevelNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.Level[RoleId]);
		UCanvasPanelSlot* LevelNumberSlot = BorderCanvas->AddChildToCanvas(LevelNumber);
		LevelNumberSlot->SetAlignment(FVector2D(1, 0.5));
		LevelNumberSlot->SetPosition(LabelOffset + FVector2D(64, 8) * UI_PIXEL_TO_UNIT);
		LevelNumberSlot->SetAutoSize(true);
		UTextBlock* HPNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.HP[RoleId]);
		UCanvasPanelSlot* HPNumberSlot = BorderCanvas->AddChildToCanvas(HPNumber);
		HPNumberSlot->SetAlignment(FVector2D(1, 0.5));
		HPNumberSlot->SetPosition(LabelOffset + FVector2D(64, 26) * UI_PIXEL_TO_UNIT);
		HPNumberSlot->SetAutoSize(true);
		UTextBlock* MPNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.MP[RoleId]);
		UCanvasPanelSlot* MPNumberSlot = BorderCanvas->AddChildToCanvas(MPNumber);
		MPNumberSlot->SetAlignment(FVector2D(1, 0.5));
		MPNumberSlot->SetPosition(LabelOffset + FVector2D(64, 44) * UI_PIXEL_TO_UNIT);
		MPNumberSlot->SetAutoSize(true);
		UTextBlock* AttackStrengthNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.AttackStrength[RoleId]);
		UCanvasPanelSlot* AttackStrengthNumberSlot = BorderCanvas->AddChildToCanvas(AttackStrengthNumber);
		AttackStrengthNumberSlot->SetAlignment(FVector2D(1, 0.5));
		AttackStrengthNumberSlot->SetPosition(LabelOffset + FVector2D(64, 62) * UI_PIXEL_TO_UNIT);
		AttackStrengthNumberSlot->SetAutoSize(true);
		UTextBlock* MagicStrengthNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.MagicStrength[RoleId]);
		UCanvasPanelSlot* MagicStrengthNumberSlot = BorderCanvas->AddChildToCanvas(MagicStrengthNumber);
		MagicStrengthNumberSlot->SetAlignment(FVector2D(1, 0.5));
		MagicStrengthNumberSlot->SetPosition(LabelOffset + FVector2D(64, 80) * UI_PIXEL_TO_UNIT);
		MagicStrengthNumberSlot->SetAutoSize(true);
		UTextBlock* DefenseNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.Defense[RoleId]);
		UCanvasPanelSlot* ResistenceNumberSlot = BorderCanvas->AddChildToCanvas(DefenseNumber);
		ResistenceNumberSlot->SetAlignment(FVector2D(1, 0.5));
		ResistenceNumberSlot->SetPosition(LabelOffset + FVector2D(64, 98) * UI_PIXEL_TO_UNIT);
		ResistenceNumberSlot->SetAutoSize(true);
		UTextBlock* DexterityNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.Dexterity[RoleId]);
		UCanvasPanelSlot* DexterityNumberSlot = BorderCanvas->AddChildToCanvas(DexterityNumber);
		DexterityNumberSlot->SetAlignment(FVector2D(1, 0.5));
		DexterityNumberSlot->SetPosition(LabelOffset + FVector2D(64, 116) * UI_PIXEL_TO_UNIT);
		DexterityNumberSlot->SetAutoSize(true);
		UTextBlock* FleeRateNumber = BuildLabelNumber(PlayerStateData->PlayerRoles.FleeRate[RoleId]);
		UCanvasPanelSlot* FleeRateNumberSlot = BorderCanvas->AddChildToCanvas(FleeRateNumber);
		FleeRateNumberSlot->SetAlignment(FVector2D(1, 0.5));
		FleeRateNumberSlot->SetPosition(LabelOffset + FVector2D(64, 134) * UI_PIXEL_TO_UNIT);
		FleeRateNumberSlot->SetAutoSize(true);

		UTexture2D* SlashTexture = UISprite->GetFrame(39);
		UImage* HPSlash = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		HPSlash->SetBrushFromTexture(SlashTexture);
		UCanvasPanelSlot* HPSlashSlot = BorderCanvas->AddChildToCanvas(HPSlash);
		HPSlashSlot->SetPosition(LabelOffset + FVector2D(63, 23) * UI_PIXEL_TO_UNIT);
		HPSlashSlot->SetSize(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		UImage* MPSlash = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
		MPSlash->SetBrushFromTexture(SlashTexture);
		MPSlash->SetDesiredSizeOverride(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
		UCanvasPanelSlot* MPSlashSlot = BorderCanvas->AddChildToCanvas(MPSlash);
		MPSlashSlot->SetPosition(LabelOffset + FVector2D(63, 41) * UI_PIXEL_TO_UNIT);
		MPSlashSlot->SetSize(FVector2D(SlashTexture->GetSizeX(), SlashTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);

		UTextBlock* MaxHPNumber = BuildLabelMaxNumber(PlayerStateData->PlayerRoles.MaxHP[RoleId]);
		UCanvasPanelSlot* MaxHPNumberSlot = BorderCanvas->AddChildToCanvas(MaxHPNumber);
		MaxHPNumberSlot->SetAlignment(FVector2D(1, 0.5));
		MaxHPNumberSlot->SetPosition(LabelOffset + FVector2D(87, 29) * UI_PIXEL_TO_UNIT);
		MaxHPNumberSlot->SetAutoSize(true);
		UTextBlock* MaxMPNumber = BuildLabelMaxNumber(PlayerStateData->PlayerRoles.MaxMP[RoleId]);
		UCanvasPanelSlot* MaxMPNumberSlot = BorderCanvas->AddChildToCanvas(MaxMPNumber);
		MaxMPNumberSlot->SetAlignment(FVector2D(1, 0.5));
		MaxMPNumberSlot->SetPosition(LabelOffset + FVector2D(87, 47) * UI_PIXEL_TO_UNIT);
		MaxMPNumberSlot->SetAutoSize(true);

		FSlateFontInfo NameTextFontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
		UTextBlock* NameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		NameText->SetFont(NameTextFontInfo);
		NameText->SetColorAndOpacity(Common->GetDefaultPalette()[44]);
		NameText->SetShadowOffset(FVector2D(1, 1)* UI_PIXEL_TO_UNIT);
		NameText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
		NameText->SetText(FText::FromString(Common->GetWord(PlayerStateData->PlayerRoles.Name[RoleId])));
		UCanvasPanelSlot* NameTextSlot = BorderCanvas->AddChildToCanvas(NameText);
		NameTextSlot->SetAlignment(FVector2D(0.5, 0.5));
		NameTextSlot->SetPosition(FVector2D(142, 16) * UI_PIXEL_TO_UNIT);
		NameTextSlot->SetAutoSize(true);

		UTexture2D* CharacterFace = Common->GetCharacterFace(PlayerStateData->PlayerRoles.Avatar[RoleId]);
		if (CharacterFace)
		{
			UImage* CharacterFaceImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			CharacterFaceImage->SetBrushFromTexture(CharacterFace);
			UCanvasPanelSlot* CharacterFaceImageSlot = BorderCanvas->AddChildToCanvas(CharacterFaceImage);
			CharacterFaceImageSlot->SetSize(FVector2D(CharacterFace->GetSizeX(), CharacterFace->GetSizeY()) * UI_PIXEL_TO_UNIT);
			CharacterFaceImageSlot->SetPosition(FVector2D(110, 30) * UI_PIXEL_TO_UNIT);
		}

		static const FVector2D RoleEquipImageLayouts[MAX_PLAYER_EQUIPMENTS] = { 
			FVector2D(189, -1) * UI_PIXEL_TO_UNIT, FVector2D(247, 39) * UI_PIXEL_TO_UNIT, 
			FVector2D(251, 101) * UI_PIXEL_TO_UNIT, FVector2D(201, 133) * UI_PIXEL_TO_UNIT,
			FVector2D(141, 141)* UI_PIXEL_TO_UNIT, FVector2D(81, 125)* UI_PIXEL_TO_UNIT };

		static const FVector2D RoleEquipNameLayouts[MAX_PLAYER_EQUIPMENTS] = {
			FVector2D(211, 46) * UI_PIXEL_TO_UNIT, FVector2D(269, 86) * UI_PIXEL_TO_UNIT,
			FVector2D(273, 148) * UI_PIXEL_TO_UNIT, FVector2D(223, 180) * UI_PIXEL_TO_UNIT,
			FVector2D(163, 188) * UI_PIXEL_TO_UNIT, FVector2D(103, 172) * UI_PIXEL_TO_UNIT };

		for (SIZE_T i = 0; i < MAX_PLAYER_EQUIPMENTS; i++)
		{
			uint16 Item = PlayerStateData->PlayerRoles.Equipments[i][RoleId];
			if (Item == 0)
			{
				continue;
			}

			// Draw the image
			UTexture2D* ItemImageTexture = Common->GetItemImage(GameStateData->Objects[Item].Item.Bitmap);
			UImage* ItemImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
			ItemImage->SetBrushFromTexture(ItemImageTexture);
			UCanvasPanelSlot* ItemImageSlot = BorderCanvas->AddChildToCanvas(ItemImage);
			ItemImageSlot->SetSize(FVector2D(ItemImageTexture->GetSizeX(), ItemImageTexture->GetSizeY()) * UI_PIXEL_TO_UNIT);
			ItemImageSlot->SetPosition(RoleEquipImageLayouts[i]);

			// Draw the text label
			UWidget* ItemNameText = BuildItemNameText(Common->GetWord(Item));
			UCanvasPanelSlot* ItemNameTextSlot = BorderCanvas->AddChildToCanvas(ItemNameText);
			ItemNameTextSlot->SetAlignment(FVector2D(0.5, 0.5));
			ItemNameTextSlot->SetPosition(RoleEquipNameLayouts[i]);
			ItemNameTextSlot->SetAutoSize(true);
		}
	}
	
}

UWidget* UPALRoleStatusMenu::BuildLabelText(const FString& Name)
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LabelText->SetFont(FontInfo);
	LabelText->SetColorAndOpacity(Common->GetDefaultPalette()[79]);
	LabelText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	LabelText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	LabelText->SetText(FText::FromString(Name));
	return LabelText;
}

UTextBlock* UPALRoleStatusMenu::BuildLabelNumber(int32 Number)
{
	UTextBlock* LabelNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LabelNumber->SetColorAndOpacity(FColor(212, 200, 168));
	LabelNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	LabelNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	LabelNumber->SetText(FText::FromString(FString::FromInt(Number)));
	return LabelNumber;
}

UTextBlock* UPALRoleStatusMenu::BuildLabelMaxNumber(int32 Number)
{
	UTextBlock* LabelNumber = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LabelNumber->SetColorAndOpacity(FColor(120, 156, 232));
	LabelNumber->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	LabelNumber->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	LabelNumber->SetText(FText::FromString(FString::FromInt(Number)));
	return LabelNumber;
}

UWidget* UPALRoleStatusMenu::BuildItemNameText(const FString& Name)
{
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));
	UTextBlock* LabelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LabelText->SetFont(FontInfo);
	LabelText->SetColorAndOpacity(Common->GetDefaultPalette()[190]);
	LabelText->SetShadowOffset(FVector2D(1, 1) * UI_PIXEL_TO_UNIT);
	LabelText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	LabelText->SetText(FText::FromString(Name));
	return LabelText;
}
