// Copyright (C) 2022 Meizhouxuanhan.


#include "PALScriptRunnerBase.h"
#include "PALGameInstance.h"
#include "PALGameState.h"

// Sets default values
APALScriptRunnerBase::APALScriptRunnerBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bInitialized = false;
	bCompleted = false;
	bSuccessful = true;
}

bool APALScriptRunnerBase::IsCompleted() const
{
	return bCompleted;
}

bool APALScriptRunnerBase::IsSuccessful() const
{
	if (IsCompleted())
	{
		return bSuccessful;
	}

	return false;
}

void APALScriptRunnerBase::MarkInitialized()
{
	bInitialized = true;
}

void APALScriptRunnerBase::MarkCompeted(bool bInSuccessful)
{
	bSuccessful = bInSuccessful;
	bCompleted = true;
	SetActorTickEnabled(false);
	Destroy();
}

void APALScriptRunnerBase::TickRun(float DeltaTime)
{

}

void APALScriptRunnerBase::BeginPlay()
{
	Super::BeginPlay();
	GameData = GetWorld()->GetGameInstance<UPALGameInstance>()->GetGameData();
	// could be a scene game mode or a battle game mode
	GameMode = GetWorld()->GetAuthGameMode<APALGameModeBase>();
	GameStateData = GetWorld()->GetGameState<APALGameState>()->GetGameStateData();
	PlayerController = Cast<APALPlayerController>(GetWorld()->GetGameInstance<UPALGameInstance>()
		->GetMainPlayer()->GetPlayerController(nullptr));
	PlayerStateData = PlayerController->GetPlayerState<APALPlayerState>()->GetPlayerStateData();
}

void APALScriptRunnerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bInitialized)
	{
		TickRun(DeltaTime);
	}
}