// Copyright (C) 2023 Meizhouxuanhan.


#include "PALOpeningPlayerController.h"
#include "PALGameState.h"


void APALOpeningPlayerController::OnEscape()
{
	if (!GetWorld()->GetGameState<APALGameState>()->IsInMainGame() && OpeningMenu)
	{
		OpeningMenu->GoBack();
		return;
	}
}

void APALOpeningPlayerController::ControllOpeningMenu(UPALOpeningMenu* InOpeningMenu)
{
	check(!GetWorld()->GetGameState<APALGameState>()->IsInMainGame());
	OpeningMenu = InOpeningMenu;
	OpeningMenu->SetFocus();
	SetShowMouseCursor(true);
}

void APALOpeningPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("Menu", IE_Pressed, this, &APALOpeningPlayerController::OnEscape);
}