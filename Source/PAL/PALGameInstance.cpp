// Copyright (C) 2022 Meizhouxuanhan.


#include "PALGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "PAL.h"
#include "PALCommon.h"

EGameDistribution UPALGameInstance::GetGameDistribution() const
{
	return GetSubsystem<UPALCommon>()->GetGameDistribution();
}

UPALGameData* UPALGameInstance::GetGameData() const
{
	return GameData;
}

UPALGameStateData* UPALGameInstance::GetGameStateData() const
{
	return GameStateData;
}

void UPALGameInstance::SetGameResourcePath(const FString& InGameResourcePath)
{
	GameResourcePath = InGameResourcePath;
	if (!GameResourcePath.IsEmpty() && !GameResourcePath.EndsWith(FGenericPlatformMisc::GetDefaultPathSeparator()))
	{
		GameResourcePath.Append(FGenericPlatformMisc::GetDefaultPathSeparator());
	}
}

FString UPALGameInstance::GetGameResourcePath() const
{
	return GameResourcePath;
}

void UPALGameInstance::SetGameStateData(UPALGameStateData* NewGameStateData)
{
	GameStateData = NewGameStateData;
}

UPALPlayerStateData* UPALGameInstance::GetMainPlayerStateData() const
{
	return MainPlayerStateData;
}

void UPALGameInstance::SetMainPlayerStateData(UPALPlayerStateData* NewMainPlayerStateData)
{
	MainPlayerStateData = NewMainPlayerStateData;
}

ULocalPlayer* UPALGameInstance::GetMainPlayer() const
{
	// Only count local players with an actual PC as part of the indexing
	for (ULocalPlayer* LocalPlayer : GetLocalPlayers())
	{
		
		APlayerController* PlayerController = LocalPlayer->GetPlayerController(nullptr);
		if (!PlayerController)
		{
			continue;
		}

		APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
		if (!PlayerState)
		{
			continue;
		}

		if (MainPlayerUniqueId == PlayerState->GetUniqueId())
		{
			return LocalPlayer;
		}
	}
	return nullptr;
}

bool UPALGameInstance::IsMainPlayerSet() const {
	return MainPlayerUniqueId.IsValid();
}

void UPALGameInstance::SetMainPlayerByUniqueId(const FUniqueNetIdRepl& UniqueId)
{
	MainPlayerUniqueId = UniqueId;
}

void UPALGameInstance::InitGameData()
{
	GetSubsystem<UPALCommon>()->Init(GameResourcePath);
	GameData = GetSubsystem<UPALCommon>()->LoadGameData();
}
