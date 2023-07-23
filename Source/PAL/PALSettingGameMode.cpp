// Copyright (C) 2023 Meizhouxuanhan.


#include "PALSettingGameMode.h"
#include "PALConfigMenu.h"

void APALSettingGameMode::StartPlay()
{
	GetGameInstance()->GetPrimaryPlayerController()->SetShowMouseCursor(true);
	UPALConfigMenu* ConfigMenu = CreateWidget<UPALConfigMenu>(GetGameInstance()->GetPrimaryPlayerController(), UPALConfigMenu::StaticClass());
	ConfigMenu->AddToViewport();
}
