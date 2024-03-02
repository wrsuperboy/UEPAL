// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "PALGameModeBase.h"
#include "PALRolePawn.h"
#include "PALPlayerState.h"
#include "PALScenePlayerController.h"
#include "PALSceneGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PAL_API APALSceneGameMode : public APALGameModeBase
{
	GENERATED_BODY()

public:
	APALSceneGameMode();

private:
	bool bLoadGlobalData;

	bool bLoadScene;

	bool bLoadRoleSprites;

	UPROPERTY(VisibleAnywhere)
	APALScenePlayerController* MainPlayerControllerPrivate;

	UPROPERTY(VisibleAnywhere)
	APALPlayerState* MainPlayerStatePrivate;

	bool bScreenWaving;

	float ScreenWaveProgression;

	bool bScreenShaking;

	float ScreenShakeEndTime;

public:
	void LoadGlobalData();

	void LoadScene();

	void LoadRoleSprites();

	void ReloadInNextTick(const SIZE_T SaveSlot);

	void SaveGame(const SIZE_T SaveSlot);

	EPALBattleResult StartBattle(const SIZE_T EnenyTeamNum, const bool bIsBoss);

	void SetDayNight(bool bNight);

	void SetTone(SIZE_T PaletteNum);

	void WaveScreen(uint32 ScreenWave, float InScreenWaveProgression);

	void StopScreenWave();

	void ShakeScreen(float ShakeTime, uint16 ShakeLevel);

private:
	void LoadResources();

	void TryRestoreGame();

	void ScreenEffects(const float DeltaTime);

	void SetLight(SIZE_T PaletteNum, bool bNightPalette);
	
public:
	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	
	virtual void RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform) override;

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

};
