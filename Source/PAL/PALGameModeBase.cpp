// Copyright Epic Games, Inc. All Rights Reserved.


#include "PALGameModeBase.h"
#include "GameFramework/PlayerState.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALPlayerState.h"
#include "PALPlayerController.h"
#include "PALPlayerCameraManager.h"
#include "PAL.h"

APALGameModeBase::APALGameModeBase() : Super()
{
	PlayerControllerClass = APALPlayerController::StaticClass();
	PlayerStateClass = APALPlayerState::StaticClass();
	GameStateClass = APALGameState::StaticClass();
}

void APALGameModeBase::FadeOut(float DelayTime)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerActor = Iterator->Get();
		if (PlayerActor && PlayerActor->IsValidLowLevel())
		{
			Cast<APALPlayerCameraManager>(PlayerActor->PlayerCameraManager)->StartCameraAutoFade( 1, DelayTime, FLinearColor::Black, false);
		}
	}
}	

void APALGameModeBase::FadeIn(float DelayTime)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerActor = Iterator->Get();
		if (PlayerActor && PlayerActor->IsValidLowLevel())
		{
			PlayerActor->PlayerCameraManager->StartCameraFade(1, 0, DelayTime, FLinearColor::Black, false, false);
		}
	}
}
		
APlayerController* APALGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* PlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	
	APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
	if (PlayerState)
	{
		if (!GetGameInstance<UPALGameInstance>()->IsMainPlayerSet() && !PlayerState->IsOnlyASpectator())
		{
			GetGameInstance<UPALGameInstance>()->SetMainPlayerByUniqueId(PlayerState->GetUniqueId());
			PAL_DebugMsg(FString(TEXT("Main player is set to player with UniqueId ")).Append(PlayerState->GetUniqueId()->ToString()));
		}
		if (!GetGameInstance<UPALGameInstance>()->IsMainPlayerSet())
		{
			PlayerState->SetIsOnlyASpectator(true);
		}
	}
	return PlayerController;
}

void APALGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	APALPlayerState* PlayerState = NewPlayer->GetPlayerState<APALPlayerState>();
	if (PlayerState)
	{
		UPlayer* MainPlayer = GetGameInstance<UPALGameInstance>()->GetMainPlayer();
		if (MainPlayer && MainPlayer->GetPlayerController(nullptr)->GetPlayerState<APlayerState>()->GetUniqueId() == PlayerState->GetUniqueId())
		{
			// New player is main player, restore data
			PlayerState->SetPlayerStateData(GetGameInstance<UPALGameInstance>()->GetMainPlayerStateData());
			PlayerState->CurrentSaveSlot = GetGameInstance<UPALGameInstance>()->MainPlayerCurrentSaveSlot;
		}
	}
	Super::PostLogin(NewPlayer);
}
