// Copyright (C) 2022 Meizhouxuanhan.


#include "PALSceneGameMode.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"
#include "PALGameInstance.h"
#include "PALGameState.h"
#include "PALAudioManager.h"
#include "PALMapManager.h"
#include "PALEventObjectActor.h"
#include "PALRolePawn.h"
#include "PALPlayerCameraManager.h"
#include "PALPlayerState.h"
#include "PALCommon.h"
#include "PALScriptManager.h"
#include "PALTriggerScriptActor.h"
#include "PALScreenWaver.h"
#include "PALCameraShake.h"
#include "PAL.h"

APALSceneGameMode::APALSceneGameMode() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	bLoadGlobalData = false;
	bLoadScene = false;
	bLoadRoleSprites = false;
	MainPlayerStatePrivate = nullptr;
	ScreenWaveProgression = 0;
	bScreenWaving = false;
	bScreenShaking = false;
}

void APALSceneGameMode::LoadGlobalData()
{
	bLoadGlobalData = true;
}

void APALSceneGameMode::LoadScene()
{
	bLoadScene = true;
	for (TActorIterator<APALEventObjectActor> It(GetWorld(), APALEventObjectActor::StaticClass()); It; ++It)
	{
		if (*It)
		{
			(*It)->SetActorTickEnabled(false);
		}
	}
	for (TActorIterator<APALRolePawn> It(GetWorld(), APALRolePawn::StaticClass()); It; ++It)
	{
		if (*It)
		{
			(*It)->SetActorTickEnabled(false);
		}
	}
}

void APALSceneGameMode::LoadRoleSprites()
{
	bLoadRoleSprites = true;
}

FORCEINLINE void APALSceneGameMode::LoadResources()
{
	if (bLoadGlobalData)
	{
		GetGameState<APALGameState>()->InitGameStateData(MainPlayerStatePrivate->CurrentSaveSlot);
		UPALPlayerStateData* MainPlayerStateData = MainPlayerStatePrivate->InitPlayerStateData(MainPlayerStatePrivate->CurrentSaveSlot);
		GetGameInstance<UPALGameInstance>()->SetMainPlayerStateData(MainPlayerStateData);
		MainPlayerControllerPrivate->ReloadRoles();
		UPALGameStateData* GameStateData = Cast<APALGameState>(GameState)->GetGameStateData();
		SetLight(GameStateData->PaletteNum, GameStateData->bNightPalette);
		GetWorld()->GetSubsystem<UPALAudioManager>()->PlayMusic(GetGameState<APALGameState>()->GetGameStateData()->MusicNum, true, 1);
		bLoadGlobalData = false;
	}

	if (bLoadScene)
	{
		bool bTriggerExists = false;
		for (TActorIterator<AActor> It(GetWorld(), APALTriggerScriptActor::StaticClass()); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && Actor->IsValidLowLevel())
			{
				bTriggerExists = true;
				break;
			}
		}
		
		if (!bTriggerExists && !Cast<APALPlayerCameraManager>(MainPlayerControllerPrivate->PlayerCameraManager)->IsFadingOut())
		{
			UPALGameStateData* GameStateData = Cast<APALGameState>(GameState)->GetGameStateData();
			// Load map
			GetWorld()->GetSubsystem<UPALMapManager>()->LoadMap(GameStateData->SceneNum, GetWorld());
			StopScreenWave();
			for (TActorIterator<APALRolePawn> It(GetWorld(), APALRolePawn::StaticClass()); It; ++It)
			{
				if (*It)
				{
					(*It)->SetActorTickEnabled(true);
				}
			}
			bLoadScene = false;

			MainPlayerControllerPrivate->CameraRestoreNormal();
			// Run map entering event script
			// Record the scene index, because it may change
			const SIZE_T i = GameStateData->SceneNum - 1;
			GetWorld()->GetSubsystem<UPALScriptManager>()->RunTriggerScript(GameStateData->Scenes[i].ScriptOnEnter, UPALScriptManager::LastTriggeredEventObjectId, true);
		}
	}

	if (bLoadRoleSprites)
	{
		MainPlayerControllerPrivate->ReloadRoleSprites();
		bLoadRoleSprites = false;
	}
}

void APALSceneGameMode::ReloadInNextTick(const SIZE_T SaveSlot)
{
	GetGameInstance<UPALGameInstance>()->MainPlayerCurrentSaveSlot = MainPlayerStatePrivate->CurrentSaveSlot = SaveSlot;
	LoadGlobalData();
	LoadScene();
	LoadRoleSprites();
}

void APALSceneGameMode::SaveGame(const SIZE_T SaveSlot)
{
	FSavedGame SavedGame;
	GetGameState<APALGameState>()->SaveGameStateData(SavedGame);
	MainPlayerStatePrivate->SavePlayerStateData(SavedGame);
	UPALCommon* Common = GetGameInstance()->GetSubsystem<UPALCommon>();
	SIZE_T MaxSavedTimes = 0;
	for (SIZE_T SaveSlotNum = 1; SaveSlotNum <= 5; SaveSlotNum++)
	{
		MaxSavedTimes = FMath::Max<SIZE_T>(Common->GetSavedTimes(SaveSlotNum), MaxSavedTimes);
	}
	MaxSavedTimes = FMath::Max<SIZE_T>(Common->GetSavedTimes(SaveSlot), MaxSavedTimes);
	SavedGame.SavedTimes = MaxSavedTimes + 1;
	Common->PutSavedGame(SaveSlot, SavedGame);
}

EPALBattleResult APALSceneGameMode::StartBattle(const SIZE_T EnenyTeam, const bool bIsBoss)
{
	// TODO
	return EPALBattleResult::Won;
}

void APALSceneGameMode::SetLight(SIZE_T PaletteNum, bool bNightPalette)
{
	ADirectionalLight* DirectionalLight = nullptr;
	for (TActorIterator<ADirectionalLight> It(GetWorld(), ADirectionalLight::StaticClass()); It; ++It)
	{
		DirectionalLight = *It;
		break;
	}
	if (DirectionalLight)
	{
		const TArray<FColor>& DefaultPalette = GetGameInstance()->GetSubsystem<UPALCommon>()->GetDefaultPalette();
		const TArray<FColor>& NewPalette = GetGameInstance()->GetSubsystem<UPALCommon>()->GetPalette(PaletteNum, bNightPalette);
		
		FLinearColor DefaultColor(EForceInit::ForceInitToZero);
		for (const FColor& Color : DefaultPalette)
		{
			DefaultColor += Color;
		}
		DefaultColor /= DefaultPalette.Num();
		FLinearColor NewColor(EForceInit::ForceInitToZero);
		for (const FColor& Color : NewPalette)
		{
			NewColor += Color;
		}
		NewColor /= NewPalette.Num();

		FLinearColor NewLightColor = FLinearColor::White * NewColor / DefaultColor;
		check(!NewLightColor.IsAlmostBlack());
		DirectionalLight->SetLightColor(NewLightColor);
		DirectionalLight->SetBrightness(100
			* NewLightColor.GetLuminance() / FLinearColor::White.GetLuminance()
			* NewColor.GetLuminance() / DefaultColor.GetLuminance());
	}
}

void APALSceneGameMode::WaveScreen(uint32 ScreenWave, float InScreenWaveProgression)
{
	ScreenWaveProgression = InScreenWaveProgression;
	Cast<APALGameState>(GameState)->GetGameStateData()->ScreenWave = ScreenWave;
	if (ScreenWave != 0)
	{
		bScreenWaving = true;
		APALScreenWaver* ScreenWaver = nullptr;
		for (TActorIterator<APALScreenWaver> It(GetWorld(), APALScreenWaver::StaticClass()); It; ++It)
		{
			if ((*It)->IsValidLowLevel())
			{
				ScreenWaver = *It;
				break;
			}
		}
		if (!ScreenWaver)
		{
			ScreenWaver = GetWorld()->SpawnActor<APALScreenWaver>();
		}
	}
}

void APALSceneGameMode::StopScreenWave()
{
	Cast<APALGameState>(GameState)->GetGameStateData()->ScreenWave = 0;
	ScreenWaveProgression = 0;
	bScreenWaving = false;
	for (TActorIterator<APALScreenWaver> It(GetWorld(), APALScreenWaver::StaticClass()); It; ++It)
	{
		if ((*It)->IsValidLowLevel())
		{
			(*It)->Destroy();
		}
	}
}

void APALSceneGameMode::ShakeScreen(float ShakeTime, uint16 ShakeLevel)
{
	ScreenShakeEndTime = GetWorld()->GetTimeSeconds() + ShakeTime;
	if (!bScreenShaking)
	{
		MainPlayerControllerPrivate->PlayerCameraManager->StartCameraShake(UPALCameraShake::StaticClass(), ShakeLevel);
		bScreenShaking = true;
	}	
}

void APALSceneGameMode::RestartPlayer(AController* NewPlayer)
{
	// Do Nothing
}

void APALSceneGameMode::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
	checkNoEntry();
}

void APALSceneGameMode::RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	checkNoEntry();
}

void APALSceneGameMode::TryRestoreGame()
{
	// Restore roles
	MainPlayerControllerPrivate->ReloadRoles();
	// Restore screen wave
	WaveScreen(Cast<APALGameState>(GameState)->GetGameStateData()->ScreenWave, 0.);
	// Restore map & event objects
	GetWorld()->GetSubsystem<UPALMapManager>()->LoadMap(GetGameState<APALGameState>()->GetGameStateData()->SceneNum, GetWorld());
}
void APALSceneGameMode::ScreenEffects(const float DeltaTime)
{
	UPALGameStateData* GameStateData = Cast<APALGameState>(GameState)->GetGameStateData();
	if (GameStateData->ScreenWave == 0 || GameStateData->ScreenWave >= 256)
	{
		// No need to wave the screen
		if (bScreenWaving)
		{
			StopScreenWave();
		}
	}
	else
	{
		GameStateData->ScreenWave += FMath::CeilToInt32(ScreenWaveProgression * DeltaTime);
	}

	if (bScreenShaking)
	{
		if (GetWorld()->GetTimeSeconds() > ScreenShakeEndTime)
		{
			MainPlayerControllerPrivate->PlayerCameraManager->StopAllInstancesOfCameraShake(UPALCameraShake::StaticClass());
			bScreenShaking = false;
		}
	}
}

void APALSceneGameMode::StartPlay()
{
	UPlayer* MainPlayer = GetGameInstance<UPALGameInstance>()->GetMainPlayer();
	check(MainPlayer);
	MainPlayerControllerPrivate = Cast<APALPlayerController>(MainPlayer->GetPlayerController(nullptr));
	check(MainPlayerControllerPrivate);
	MainPlayerStatePrivate = MainPlayerControllerPrivate->GetPlayerState<APALPlayerState>();
	check(MainPlayerStatePrivate);

	if (GetGameState<APALGameState>()->IsInMainGame())
	{
		// Restore things destroyed on world travel
		TryRestoreGame();
	}
	else
	{
		ReloadInNextTick(MainPlayerStatePrivate->CurrentSaveSlot);
	}
	GameState->HandleBeginPlay();
}

void APALSceneGameMode::Tick(float DeltaTime)
{
	LoadResources();
	ScreenEffects(DeltaTime);
}