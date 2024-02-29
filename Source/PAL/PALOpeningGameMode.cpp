// Copyright (C) 2022 Meizhouxuanhan.


#include "PALOpeningGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALOpeningPlayerController.h"
#include "PALOpeningMenu.h"
#include "PALAudioManager.h"

APALOpeningGameMode::APALOpeningGameMode() : Super()
{
	PlayerControllerClass = APALOpeningPlayerController::StaticClass();
}

void APALOpeningGameMode::StartMainGame(SIZE_T SaveSlotNum)
{
	UPlayer* MainPlayer = GetGameInstance<UPALGameInstance>()->GetMainPlayer();
	if (MainPlayer)
	{
		GetGameInstance<UPALGameInstance>()->MainPlayerCurrentSaveSlot = MainPlayer->GetPlayerController(nullptr)->GetPlayerState<APALPlayerState>()->CurrentSaveSlot = SaveSlotNum;
		UGameplayStatics::OpenLevel(this, TEXT("PAL_Scene"));
	}
}

void APALOpeningGameMode::StartPlay()
{
	Super::StartPlay();
	check(!Cast<APALGameState>(GameState)->IsInMainGame());
	UPlayer* MainPlayer = GetGameInstance<UPALGameInstance>()->GetMainPlayer();
	check(MainPlayer);

	UPALOpeningMenu* OpeningMenu = CreateWidget<UPALOpeningMenu>(MainPlayer->GetPlayerController(nullptr), UPALOpeningMenu::StaticClass());
	TWeakObjectPtr<UPALOpeningMenu> OpeningMenuWP(OpeningMenu);
	OpeningMenu->OnSaveSlotSelected.AddUObject(this, &APALOpeningGameMode::StartMainGame);
	OpeningMenu->OnSaveSlotSelected.AddLambda([OpeningMenuWP](SIZE_T SaveSlotNum) {
		if (OpeningMenuWP.IsValid())
		{
			OpeningMenuWP.Get()->RemoveFromParent();
		}});
	Cast<APALOpeningPlayerController>(MainPlayer->GetPlayerController(nullptr))->ControllOpeningMenu(OpeningMenu);
	OpeningMenu->AddToViewport();
	if (!GetWorld()->GetSubsystem<UPALAudioManager>()->PlayCDTrack(7))
	{
		GetWorld()->GetSubsystem<UPALAudioManager>()->PlayMusic(4, true, 1);
	}
}
