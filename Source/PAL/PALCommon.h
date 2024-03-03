// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Font.h"
#include "PALSprite.h"
#include "PALMKF.h"
#include "PALGameData.h"
#include "PAL.h"
#include "PALCommon.generated.h"

enum EPALCodePage : uint8
{
	CP_BIG5 = 0,
	CP_GBK = 1,
	//CP_SHIFTJIS = 2,
	//CP_JISX0208 = 3,
	CP_MAX = CP_GBK + 1,
	CP_UTF_8 = CP_MAX + 1,
	CP_UCS = CP_UTF_8 + 1,
};

enum EPALSoundFormat : uint8
{
	WAVE,
	VOC
};

/**
 * 
 */
UCLASS()
class PAL_API UPALCommon : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class UPALGameInstance;

	friend class UPALSprite;

private:
	FString GameResourcePath;

	UPROPERTY()
	TArray<FColor> DefaultPalette;

	EGameDistribution GameDistribution;

	UPROPERTY(VisibleAnywhere)
	UPALSprite* CachedUISprite;

	UPROPERTY(VisibleAnywhere)
	UPALSprite* CachedBattleEffectSprite;

	EPALCodePage CodePage;

	UPROPERTY()
	TArray<FString> WordBuffer;

	UPROPERTY()
	TArray<FString> MessageBuffer;

	UPROPERTY(VisibleAnywhere)
	UFont* UniFont;

	EPALSoundFormat SoundFormat;

public:
	EGameDistribution GetGameDistribution() const;

	UPALGameData* LoadGameData();

	UPALSprite* GetSprite(const SIZE_T SpriteNum);

	UPALSprite* GetUISprite();

	const TArray<FColor>& GetDefaultPalette() const;

	TArray<FColor> GetPalette(const SIZE_T PaletteNum, bool bNight) const;

	bool GetMap(const SIZE_T MapNum, uint32(&OutTiles)[128][64][2], UPALSprite*& OutTileSprite);

	FString GetWord(SIZE_T WordNum);

	FString GetMessage(SIZE_T MessageNum);

	UFont* GetDefaultFont();

	UTexture2D* GetBackgroundPicture(SIZE_T FPBNum);

	SIZE_T GetSavedTimes(SIZE_T SaveSlotNum);

	bool GetSavedGame(SIZE_T SaveSlotNum, FSavedGame& OutSavedGame);

	void PutSavedGame(SIZE_T SaveSlotNum, const FSavedGame& SavedGame);

	UTexture2D* GetCharacterFace(SIZE_T CharacterFaceNum);

	UTexture2D* GetItemImage(SIZE_T ItemImageNum);

	uint8* LoadSound(SIZE_T SoundNum, int32& OutSize);

	EPALSoundFormat GetSoundFormat() const;

	UPALSprite* GetBattleRoleSprite(const SIZE_T BattleRoleSpriteNum);

	UPALSprite* GetBattleEnemySprite(const SIZE_T BattleEnemySpriteNum);

	UPALSprite* GetBattleEffectSprite();

private:
	UPALMKF* LoadMKF(const FString& FileName) const;

	UTexture2D* CreateTextureFromRLE(const uint8* const RLE);

	void Init(const FString& InGameResourcePath);

	void SolveDistribution();

	void LoadDefaultPalette();

	EPALCodePage DetectCodePage(const FString& FileName) const;

	void DetectCodePage();

	void MultiByteToWideChar(const ANSICHAR* MultiByteStringPtr, int32 MultiByteStringLength, FString& OutString);

	void InitWord();

	void InitMessage();

	void InitText();

	void SolveSoundFormat();

};