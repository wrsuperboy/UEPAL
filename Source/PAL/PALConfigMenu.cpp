// Copyright (C) 2023 Meizhouxuanhan.


#include "PALConfigMenu.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScrollBox.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "PALGameInstance.h"

void UPALConfigMenu::SelectGameResourcePath()
{
	FString DefaultPath = FPaths::ProjectDir();
	FString DefaultFile = "default.txt";
	FString SelectedFolderPath = "";

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		FString OutFolderName;
		FString DefaultDirectory = FPaths::ConvertRelativePathToFull(DefaultPath);

		bool bSuccess = DesktopPlatform->OpenDirectoryDialog(
			nullptr,
			"Select a folder",
			DefaultDirectory,
			OutFolderName
		);

		if (bSuccess && !OutFolderName.IsEmpty())
		{
			SelectedFolderPath = OutFolderName;
			GameResourcePathText->SetText(FText::FromString(SelectedFolderPath));
		}
	}
}

void UPALConfigMenu::StartGame()
{
	if (GameResourcePathText) {
		GetGameInstance<UPALGameInstance>()->SetGameResourcePath(GameResourcePathText->GetText().ToString());
		GetGameInstance<UPALGameInstance>()->InitGameData();
	}
	UGameplayStatics::OpenLevel(this, TEXT("PAL_Opening"));
}

UWidget* UPALConfigMenu::BuildMenuLabel(FText Text)
{
	UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Label->SetText(Text);
	Label->SetShadowColorAndOpacity(FLinearColor::Gray);
	return Label;
}

TSharedRef<SWidget> UPALConfigMenu::RebuildWidget()
{
	if (!GetRootWidget())
	{
		WidgetTree->RootWidget =
			WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("SafeScreen"));
	}

	return Super::RebuildWidget();
}

void UPALConfigMenu::NativeConstruct()
{
	Super::NativeConstruct();
	UCanvasPanel* Canvas = Cast<UCanvasPanel>(GetRootWidget());
	UVerticalBox* VerticalBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
	UCanvasPanelSlot* VerticalBoxSlot = Canvas->AddChildToCanvas(VerticalBox);
	VerticalBoxSlot->SetAnchors(FAnchors(0, 0, 1, 1));
	VerticalBoxSlot->SetOffsets(FMargin(30));

	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	Title->SetText(FText::FromString("Settings"));
	Title->SetShadowColorAndOpacity(FLinearColor::Gray);
	UVerticalBoxSlot* TitleSlot = VerticalBox->AddChildToVerticalBox(Title);
	TitleSlot->SetPadding(FMargin(0, 10));

	UScrollBox* ScrollBox = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass());
	UVerticalBoxSlot* ScrollBoxSlot = VerticalBox->AddChildToVerticalBox(ScrollBox);
	ScrollBoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	UGridPanel* Grid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass());
	Grid->SetColumnFill(1, 1.0);
	ScrollBox->AddChild(Grid);
	
	Grid->AddChildToGrid(BuildMenuLabel(FText::FromString("Game resource path")), 0, 0);
	UHorizontalBox* GameResourcePathSelectionBox = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
	Grid->AddChildToGrid(GameResourcePathSelectionBox, 0, 1);
	GameResourcePathText = WidgetTree->ConstructWidget<UEditableText>(UEditableText::StaticClass());
	GameResourcePathText->SetText(FText::FromString("X:\\yyy\\PAL98"));
	UHorizontalBoxSlot* GameResourcePathTextSlot = GameResourcePathSelectionBox->AddChildToHorizontalBox(GameResourcePathText);
	GameResourcePathTextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	UButton* GameResourcePathSelectionButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	TScriptDelegate<> OnClickGameResourcePathSelection;
	OnClickGameResourcePathSelection.BindUFunction(this, "SelectGameResourcePath");
	GameResourcePathSelectionButton->OnClicked.Add(OnClickGameResourcePathSelection);
	GameResourcePathSelectionBox->AddChildToHorizontalBox(GameResourcePathSelectionButton);
	UTextBlock* SelectGameResourcePathText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	SelectGameResourcePathText->SetText(FText::FromString("Select..."));
	SelectGameResourcePathText->SetShadowColorAndOpacity(FLinearColor::Gray);
	GameResourcePathSelectionButton->AddChild(SelectGameResourcePathText);

	UButton* StartGameButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
	TScriptDelegate<> OnStartGame;
	OnStartGame.BindUFunction(this, "StartGame");
	StartGameButton->OnClicked.Add(OnStartGame);
	VerticalBox->AddChildToVerticalBox(StartGameButton);
	UTextBlock* StartGameText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	StartGameText->SetText(FText::FromString("StartGame"));
	StartGameText->SetShadowColorAndOpacity(FLinearColor::Gray);
	StartGameButton->AddChild(StartGameText);
}