// Copyright (C) 2022 Meizhouxuanhan.


#include "PALOpeningMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "PALCommon.h"

void UPALOpeningMenu::NewGame()
{
	SelectSaveSlot(0);
}

void UPALOpeningMenu::LoadGame()
{
	if (SaveSlotMenu && SaveSlotMenu->IsValidLowLevel())
	{
		return;
	}

	SaveSlotMenu = WidgetTree->ConstructWidget<UPALSaveSlotMenu>(UPALSaveSlotMenu::StaticClass());
	UCanvasPanelSlot* SaveSlotMenuSlot = Canvas->AddChildToCanvas(SaveSlotMenu);
	SaveSlotMenuSlot->SetAnchors(FAnchors(1, 0, 1, 1));
	SaveSlotMenuSlot->SetAlignment(FVector2D(1, 0));
	SaveSlotMenuSlot->SetZOrder(2);
	SaveSlotMenuSlot->SetAutoSize(true);
	SaveSlotMenuSlot->SetPosition(FVector2D(-13, 0) * UI_PIXEL_TO_UNIT);
	SaveSlotMenu->OnSaveSlotSelected.BindUObject(this, &UPALOpeningMenu::SelectSaveSlot);
}

void UPALOpeningMenu::SelectSaveSlot(SIZE_T SaveSlotNum)
{
	OnSaveSlotSelected.Broadcast(SaveSlotNum);
}

bool UPALOpeningMenu::GoBack()
{
	if (!SaveSlotMenu)
	{
		SelectSaveSlot(0);
	}
	else {
		SaveSlotMenu->RemoveFromParent();
		SaveSlotMenu = nullptr;
	}

	return true;
}

void UPALOpeningMenu::NativeConstruct()
{
	Super::NativeConstruct();
	UImage* Background = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass());
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	const SIZE_T MainMenuBackgroundFBPNum = Common->GetGameDistribution() == EGameDistribution::DOS ? 60 : 2;
	UTexture2D* BackgroundPicture = Common->GetBackgroundPicture(MainMenuBackgroundFBPNum);
	Background->SetBrushFromTexture(BackgroundPicture);
	UCanvasPanelSlot* BackgroundSlot = Canvas->AddChildToCanvas(Background);
	BackgroundSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	BackgroundSlot->SetOffsets(FMargin(0));
	BackgroundSlot->SetZOrder(0);

	UVerticalBox* MenuBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UCanvasPanelSlot* MenuBoxSlot = Canvas->AddChildToCanvas(MenuBox);
	MenuBoxSlot->SetAnchors(FAnchors(0.5, 0.5, 0.5, 0.5));
	MenuBoxSlot->SetAlignment(FVector2D(0.5, 0.5));
	MenuBoxSlot->SetZOrder(1);

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
	FSlateFontInfo FontInfo(Common->GetDefaultFont(), FMath::RoundToInt32(12 * UI_PIXEL_TO_UNIT));

	UButton* NewGameButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	NewGameButton->SetStyle(ButtonStyle);
	UTextBlock* NewGameButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NewGameButtonText->SetText(FText::FromString(Common->GetWord(7)));
	NewGameButtonText->SetFont(FontInfo);
	NewGameButtonText->SetColorAndOpacity(FSlateColor::UseForeground());
	NewGameButtonText->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	NewGameButtonText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UButtonSlot* NewGameButtonTextSlot = Cast<UButtonSlot>(NewGameButton->AddChild(NewGameButtonText));
	NewGameButtonTextSlot->SetPadding(FMargin(0, 1., 0, 1.) * UI_PIXEL_TO_UNIT);
	NewGameButtonTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	NewGameButtonTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	TScriptDelegate<> NewGameDelegate;
	NewGameDelegate.BindUFunction(this, "NewGame");
	NewGameButton->OnClicked.Add(NewGameDelegate);
	MenuBox->AddChildToVerticalBox(NewGameButton);

	UButton* LoadGameButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	LoadGameButton->SetStyle(ButtonStyle);
	UTextBlock* LoadGameButtonText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	LoadGameButtonText->SetText(FText::FromString(Common->GetWord(8)));
	LoadGameButtonText->SetFont(FontInfo);
	LoadGameButtonText->SetColorAndOpacity(FSlateColor::UseForeground());
	LoadGameButtonText->SetShadowOffset(FVector2D(1 * UI_PIXEL_TO_UNIT, 1 * UI_PIXEL_TO_UNIT));
	LoadGameButtonText->SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 165.f / 255.f));
	UButtonSlot* LoadGameButtonTextSlot = Cast<UButtonSlot>(LoadGameButton->AddChild(LoadGameButtonText));
	LoadGameButtonTextSlot->SetPadding(FMargin(0, 1., 0, 1.) * UI_PIXEL_TO_UNIT);
	LoadGameButtonTextSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	LoadGameButtonTextSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	TScriptDelegate<> LoadGameDelegate;
	LoadGameDelegate.BindUFunction(this, "LoadGame");
	LoadGameButton->OnClicked.Add(LoadGameDelegate);
	MenuBox->AddChildToVerticalBox(LoadGameButton);
}
