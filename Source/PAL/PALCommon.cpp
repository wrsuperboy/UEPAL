// Copyright (C) 2022 Meizhouxuanhan.


#include "PALCommon.h"
#include "Misc/CString.h"
#include "PALGameInstance.h"
#include "CodePage.h"
#include "PAL.h"

EGameDistribution UPALCommon::GetGameDistribution() const
{
	return GameDistribution;
}

#define	LOAD_DATA(Ptr, Count, ChunkNum, MKF) \
{ \
	SIZE_T ChunkSize = MKF->GetChunkSize(ChunkNum); \
	Ptr = (decltype(Ptr))FMemory::Malloc(ChunkSize); \
	Count = ChunkSize / sizeof(decltype(*Ptr)); \
	MKF->ReadChunk((uint8*)Ptr, ChunkSize, ChunkNum); \
} \

UPALGameData* UPALCommon::LoadGameData()
{
	UPALGameData* GameData = NewObject<UPALGameData>();
	UPALMKF* SSSMKF = LoadMKF("SSS");
	LOAD_DATA(GameData->ScriptEntries, GameData->ScriptEntryCount, 4, SSSMKF);
	UPALMKF* DATAMKF = LoadMKF("DATA");
	LOAD_DATA(GameData->Stores, GameData->StoreCount, 0, DATAMKF);
	LOAD_DATA(GameData->Enemies, GameData->EnemyCount, 1, DATAMKF);
	LOAD_DATA(GameData->EnemyTeams, GameData->EnemyTeamCount, 2, DATAMKF);
	LOAD_DATA(GameData->Magics, GameData->MagicCount, 4, DATAMKF);
	LOAD_DATA(GameData->BattleFields, GameData->BattleFieldCount, 5, DATAMKF);
	LOAD_DATA(GameData->LevelUpMagicAlls, GameData->LevelUpMagicAllCount, 6, DATAMKF);
	DATAMKF->ReadChunk((uint8*)GameData->BattleEffectIndex, sizeof(GameData->BattleEffectIndex), 11);
	DATAMKF->ReadChunk((uint8*)&GameData->EnemyPos, sizeof(GameData->EnemyPos), 13);
	DATAMKF->ReadChunk((uint8*)GameData->LevelUpExps, sizeof(GameData->LevelUpExps), 14);


	// Always load the default data from the game data files.
	LOAD_DATA(GameData->_EventObjects, GameData->EventObjectCount, 0, SSSMKF);
	SSSMKF->ReadChunk((uint8*)GameData->_Scenes, sizeof(GameData->_Scenes), 1);
	if (GameDistribution == EGameDistribution::DOS)
	{
		FObjectDOS Objects[MAX_OBJECTS];
		SSSMKF->ReadChunk((uint8*)Objects, sizeof(Objects), 2);

		// Convert the DOS-style data structure to WIN-style data structure
		for (SIZE_T i = 0; i < MAX_OBJECTS; i++)
		{
			FMemory::Memcpy(&GameData->_Objects[i], &Objects[i], sizeof(FObjectDOS));
			GameData->_Objects[i].Data[6] = Objects[i].Data[5];     // wFlags
			GameData->_Objects[i].Data[5] = 0;                      // wScriptDesc or wReserved2
		}
	}
	else
	{
		SSSMKF->ReadChunk((uint8*)GameData->_Objects, sizeof(GameData->_Objects), 2);
	}

	DATAMKF->ReadChunk((uint8*)&GameData->_PlayerRoles, sizeof(FPlayerRoles), 3);
	return GameData;
}

UPALSprite* UPALCommon::GetSprite(const SIZE_T SpriteNum)
{
	UPALMKF* MKF = LoadMKF(FString(TEXT("MGO")));
	SIZE_T Size = MKF->GetDecompressedSize(SpriteNum);
	uint8* Buffer = (uint8*)FMemory::Malloc(Size);
	MKF->DecompressChunk(Buffer, Size, SpriteNum);
	return UPALSprite::Create(Buffer, this);
}

UPALSprite* UPALCommon::GetUISprite()
{
	if (!CachedUISprite)
	{
		UPALMKF* MKF = LoadMKF(FString(TEXT("DATA")));
		SIZE_T Size = MKF->GetChunkSize(9);
		uint8* Buffer = (uint8*)FMemory::Malloc(Size);
		MKF->ReadChunk(Buffer, Size, 9);
		CachedUISprite =  UPALSprite::Create(Buffer, this);
	}

	return CachedUISprite;
}

const TArray<FColor>& UPALCommon::GetDefaultPalette() const
{
	return DefaultPalette;
}

TArray<FColor> UPALCommon::GetPalette(const SIZE_T PaletteNum, bool bNight) const
{
	TArray<FColor> Palette;
	Palette.Init(FColor(), 256);
	uint8* Buffer = (uint8*)FMemory::Malloc(1536);

	// Read the palette data from the pat.mkf file
	UPALMKF* MKF = LoadMKF("pat");
	MKF->ReadChunk(Buffer, 1536, PaletteNum);

	for (SIZE_T i = 0; i < 256; i++)
	{
		Palette[i].R = Buffer[(bNight ? 256 * 3 : 0) + i * 3] << 2;
		Palette[i].G = Buffer[(bNight ? 256 * 3 : 0) + i * 3 + 1] << 2;
		Palette[i].B = Buffer[(bNight ? 256 * 3 : 0) + i * 3 + 2] << 2;
		Palette[i].A = 255;
	}

	FMemory::Free(Buffer);
	return Palette;
}

bool UPALCommon::GetMap(const SIZE_T MapNum, uint32(&OutTiles)[128][64][2], UPALSprite*& OutTileSprite)
{
	UPALMKF* MAPMKF = LoadMKF(TEXT("MAP"));
	UPALMKF* GOPMKF = LoadMKF(TEXT("GOP"));

	// Check for invalid map number.
	if (MapNum >= MAPMKF->GetChunkCount() || MapNum >= GOPMKF->GetChunkCount())
	{
		PAL_DebugMsg(FString(TEXT("Invalid map number ")).Append(FString::FromInt(MapNum)));
		return false;
	}

	PAL_DebugMsg(TEXT("Setting map tiles"));

	SIZE_T MapSize = MAPMKF->GetChunkSize(MapNum);
	if (MapSize == 0)
	{
		PAL_DebugMsg(FString(TEXT("Cannot read from MAP.mkf for map number ")).Append(FString::FromInt(MapNum)));
		return false;
	}

	// Decompress the tile data.
	MAPMKF->DecompressChunk((uint8*)OutTiles, sizeof(OutTiles), MapNum);

	// Load the tile bitmaps.
	SIZE_T GopSize = GOPMKF->GetChunkSize(MapNum);
	if (GopSize == 0)
	{
		PAL_DebugMsg(FString(TEXT("Cannot read from GOP.mkf for map number ")).Append(FString::FromInt(MapNum)));
		return false;
	}
	uint8* TileSpriteBuffer = (uint8*)FMemory::Malloc(GopSize);
	GOPMKF->ReadChunk(TileSpriteBuffer, GopSize, MapNum);
	OutTileSprite = UPALSprite::Create(TileSpriteBuffer, this);
	return true;
}

FString UPALCommon::GetWord(SIZE_T WordNum)
{
	if (!WordBuffer.IsValidIndex(WordNum))
	{
		return "???";
	}

	return WordBuffer[WordNum];
}

FString UPALCommon::GetMessage(SIZE_T MessageNum)
{
	if (!MessageBuffer.IsValidIndex(MessageNum))
	{
		return "???";
	}

	return MessageBuffer[MessageNum];
}

UFont* UPALCommon::GetDefaultFont()
{
	return UniFont;
}

UTexture2D* UPALCommon::GetBackgroundPicture(SIZE_T FPBNum)
{
	UPALMKF* FBPMKF = LoadMKF("FBP");
	SIZE_T Size = FBPMKF->GetDecompressedSize(FPBNum);
	uint8* Buffer = (uint8*)FMemory::Malloc(Size);
	FBPMKF->DecompressChunk(Buffer, Size, FPBNum);

	const SIZE_T Width = 320;
	const SIZE_T Height = 200;
	TArray<FColor> Data;
	Data.SetNumUninitialized(Width * Height);
	for (SIZE_T y = 0; y < Height; y++)
	{
		for (SIZE_T x = 0; x < Width; x++)
		{
			Data[y * Width + x] = DefaultPalette[Buffer[y * Width + x]];
		}
	}

	UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	void* TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Data.GetData(), 4 * Width * Height);
	NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	NewTexture->UpdateResource();
	FMemory::Free(Buffer);

	return NewTexture;
}

SIZE_T UPALCommon::GetSavedTimes(SIZE_T SaveSlotNum)
{
	FString FileFullPath = FString(GameResourcePath).Append(TEXT("SAVE")).Append(FGenericPlatformMisc::GetDefaultPathSeparator()).Append(FString::FromInt(SaveSlotNum)).Append(TEXT(".rpg"));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* SaveFileHanlde = PlatformFile.OpenRead(*FileFullPath);
	uint16 SavedTimes = 0;
	if (SaveFileHanlde)
	{
		SaveFileHanlde->Read((uint8*)&SavedTimes, sizeof(uint16));
	}
	delete SaveFileHanlde;
	return SavedTimes;
}

bool UPALCommon::GetSavedGame(SIZE_T SaveSlotNum, FSavedGame& SavedGame)
{
	FString FileFullPath = FString(GameResourcePath).Append(TEXT("SAVE")).Append(FGenericPlatformMisc::GetDefaultPathSeparator()).Append(FString::FromInt(SaveSlotNum)).Append(".rpg");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* SaveFileHanlde = PlatformFile.OpenRead(*FileFullPath);
	if (!SaveFileHanlde)
	{
		return false;
	}

	if (GameDistribution == EGameDistribution::DOS)
	{
		FSavedGameDOS* s = (FSavedGameDOS*)FMemory::Malloc(sizeof(FSavedGameDOS));
		FMemory::Memzero(s->EventObject, sizeof(s->EventObject));
		SIZE_T FileSize = SaveFileHanlde->Size();
		bool bSuccess = SaveFileHanlde->Read((uint8*)s, FMath::Min<SIZE_T>(sizeof(FSavedGameDOS), FileSize));
		if (!bSuccess && FileSize < sizeof(FSavedGameDOS) - sizeof(FEventObject) * MAX_EVENT_OBJECTS)
		{
			delete SaveFileHanlde;
			FMemory::Free(s);
			return false;
		}
		SavedGame.SavedTimes = s->SavedTimes;
		SavedGame.ViewportX = s->ViewportX;
		SavedGame.ViewportY = s->ViewportY;
		SavedGame.PartyMember = s->PartyMember;
		SavedGame.NumScene = s->NumScene;
		SavedGame.PaletteOffset = s->PaletteOffset;
		SavedGame.PartyDirection = s->PartyDirection;
		SavedGame.NumMusic = s->NumMusic;
		SavedGame.NumBattleMusic = s->NumBattleMusic;
		SavedGame.NumBattleField = s->NumBattleField;
		SavedGame.ScreenWave = s->ScreenWave;
		SavedGame.BattleSpeed = s->BattleSpeed;
		SavedGame.CollectValue = s->CollectValue;
		SavedGame.Layer = s->Layer;
		SavedGame.ChaseRange = s->ChaseRange;
		SavedGame.ChasespeedChangeCycles = s->ChasespeedChangeCycles;
		SavedGame.Follower = s->Follower;
		FMemory::Memcpy(SavedGame.Reserved2, s->Reserved2, sizeof(SavedGame.Reserved2));
		SavedGame.Cash = s->Cash;
		FMemory::Memcpy(SavedGame.Party, s->Party, sizeof(SavedGame.Party));
		FMemory::Memcpy(SavedGame.Trail, s->Trail, sizeof(SavedGame.Trail));
		FMemory::Memcpy(&SavedGame.Exp, &s->Exp, sizeof(SavedGame.Exp));
		FMemory::Memcpy(&SavedGame.PlayerRoles, &s->PlayerRoles, sizeof(SavedGame.PlayerRoles));
		FMemory::Memcpy(SavedGame.PoisonStatus, s->PoisonStatus, sizeof(SavedGame.PoisonStatus));
		FMemory::Memcpy(SavedGame.Inventory, s->Inventory, sizeof(SavedGame.Inventory));
		FMemory::Memcpy(SavedGame.Scene, s->Scene, sizeof(SavedGame.Scene));
		for (SIZE_T i = 0; i < MAX_OBJECTS; i++)
		{
			FMemory::Memcpy(&SavedGame.Object[i], &s->Object[i], sizeof(FObjectDOS));
			SavedGame.Object[i].Data[6] = s->Object[i].Data[5]; // wFlags
			SavedGame.Object[i].Data[5] = 0;
		}
		FMemory::Memcpy(SavedGame.EventObject, s->EventObject, sizeof(SavedGame.EventObject));
		FMemory::Free(s);
	}
	else
	{
		SIZE_T FileSize = SaveFileHanlde->Size();
		if (FileSize < sizeof(FSavedGame) - sizeof(FEventObject) * MAX_EVENT_OBJECTS)
		{
			return false;
		}
		FMemory::Memzero(SavedGame.EventObject, sizeof(SavedGame.EventObject));
		return SaveFileHanlde->Read((uint8*)&SavedGame, FMath::Min<SIZE_T>(sizeof(FSavedGame), FileSize));
	}
	delete SaveFileHanlde;
	return true;
}

void UPALCommon::PutSavedGame(SIZE_T SaveSlotNum, const FSavedGame& SavedGame)
{
	FString FileFullPath = FString(GameResourcePath).Append(TEXT("SAVE")).Append(FGenericPlatformMisc::GetDefaultPathSeparator()).Append(FString::FromInt(SaveSlotNum)).Append(".rpg");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* SaveFileHanlde = PlatformFile.OpenWrite(*FileFullPath);
	if (!SaveFileHanlde && PlatformFile.DeleteFile(*FileFullPath))
	{
		SaveFileHanlde = PlatformFile.OpenWrite(*FileFullPath);
	}
	if (!SaveFileHanlde)
	{
		PAL_DebugMsg(FString("Open write saved file failed ").Append(FileFullPath));
		return;
	}

	if (GameDistribution == EGameDistribution::DOS)
	{
		FSavedGameDOS* s = (FSavedGameDOS*)FMemory::Malloc(sizeof(FSavedGameDOS));
		s->SavedTimes = SavedGame.SavedTimes;
		s->ViewportX = SavedGame.ViewportX;
		s->ViewportY =SavedGame.ViewportY;
		s->PartyMember = SavedGame.PartyMember;
		s->NumScene = SavedGame.NumScene;
		s->PaletteOffset = SavedGame.PaletteOffset;
		s->PartyDirection = SavedGame.PartyDirection;
		s->NumMusic = SavedGame.NumMusic;
		s->NumBattleMusic = SavedGame.NumBattleMusic;
		s->NumBattleField = SavedGame.NumBattleField;
		s->ScreenWave = SavedGame.ScreenWave;
		s->BattleSpeed = SavedGame.BattleSpeed;
		s->CollectValue = SavedGame.CollectValue;
		s->Layer = SavedGame.Layer;
		s->ChaseRange = SavedGame.ChaseRange;
		s->ChasespeedChangeCycles = SavedGame.ChasespeedChangeCycles;
		s->Follower = SavedGame.Follower;
		FMemory::Memcpy(s->Reserved2, SavedGame.Reserved2, sizeof(s->Reserved2));
		s->Cash = SavedGame.Cash;
		FMemory::Memcpy(s->Party, SavedGame.Party, sizeof(s->Party));
		FMemory::Memcpy(s->Trail, SavedGame.Trail, sizeof(s->Trail));
		FMemory::Memcpy(&s->Exp, &SavedGame.Exp, sizeof(s->Exp));
		FMemory::Memcpy(&s->PlayerRoles, &SavedGame.PlayerRoles, sizeof(s->PlayerRoles));
		FMemory::Memcpy(s->PoisonStatus, SavedGame.PoisonStatus, sizeof(s->PoisonStatus));
		FMemory::Memcpy(s->Inventory, SavedGame.Inventory, sizeof(s->Inventory));
		FMemory::Memcpy(s->Scene, SavedGame.Scene, sizeof(s->Scene));
		for (SIZE_T i = 0; i < MAX_OBJECTS; i++)
		{
			FMemory::Memcpy(&s->Object[i], &SavedGame.Object[i], sizeof(FObjectDOS));
			s->Object[i].Data[5] = SavedGame.Object[i].Data[6]; // wFlags
		}
		FMemory::Memcpy(s->EventObject, SavedGame.EventObject, sizeof(s->EventObject));
		
		bool bSuccess = SaveFileHanlde->Write((uint8*)s, sizeof(FSavedGameDOS));
		if (!bSuccess)
		{
			PAL_DebugMsg(FString("Save file failed ").Append(FileFullPath));
		}
		FMemory::Free(s);
	}
	else
	{
		bool bSuccess = SaveFileHanlde->Write((uint8*)&SavedGame, sizeof(FSavedGame));
		if (!bSuccess)
		{
			PAL_DebugMsg(FString("Save file failed ").Append(FileFullPath));
		}
	}
	SaveFileHanlde->Flush();
	delete SaveFileHanlde;
}

UTexture2D* UPALCommon::GetCharacterFace(SIZE_T CharacterFaceNum)
{
	UPALMKF* RGMMKF = LoadMKF("RGM");
	SIZE_T Size = RGMMKF->GetChunkSize(CharacterFaceNum);
	uint8* Buffer = (uint8*)FMemory::Malloc(Size);
	UTexture2D* CharacterFace = nullptr;
	if (RGMMKF->ReadChunk(Buffer, Size, CharacterFaceNum) > 0)
	{
		CharacterFace = CreateTextureFromRLE(Buffer);
	}
	FMemory::Free(Buffer);
	return CharacterFace;
}

UTexture2D* UPALCommon::GetItemImage(SIZE_T ItemImageNum)
{
	UPALMKF* BALLMKF = LoadMKF("BALL");
	SIZE_T Size = BALLMKF->GetChunkSize(ItemImageNum);
	uint8* Buffer = (uint8*)FMemory::Malloc(Size);
	UTexture2D* ItemImage = nullptr;
	if (BALLMKF->ReadChunk(Buffer, Size, ItemImageNum) > 0)
	{
		ItemImage = CreateTextureFromRLE(Buffer);
	}
	FMemory::Free(Buffer);
	return ItemImage;
}

IFileHandle* UPALCommon::OpenRixSoundFile()
{
	FString FileFullPath = FString(GameResourcePath).Append(TEXT("MUS.mkf"));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	return PlatformFile.OpenRead(*FileFullPath);
}

uint8* UPALCommon::LoadSound(SIZE_T SoundNum, int32& OutSize)
{
	UPALMKF* SoundMKF;
	if (SoundFormat == EPALSoundFormat::VOC)
	{
		SoundMKF = LoadMKF("VOC");
	}
	else
	{
		SoundMKF = LoadMKF("Sounds");
	}
	OutSize = SoundMKF->GetChunkSize(SoundNum);
	if (OutSize == 0)
	{
		return nullptr;
	}
	uint8* Data = (uint8*)FMemory::Malloc(OutSize);
	SoundMKF->ReadChunk(Data, OutSize, SoundNum);
	return Data;
}

EPALSoundFormat UPALCommon::GetSoundFormat() const
{
	return SoundFormat;
}

UPALMKF* UPALCommon::LoadMKF(const FString& FileName) const
{
	FString FileFullPath = FString(GameResourcePath).Append(FileName).Append(".mkf");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* FileHandle = PlatformFile.OpenRead(*FileFullPath);
	check(FileHandle != nullptr);
	return UPALMKF::Create(FileHandle, GameDistribution);
}

UTexture2D* UPALCommon::CreateTextureFromRLE(const uint8* const RLE)
{
	const uint8* PtrBitmapRLE = RLE;

	// Skip the 0x00000002 in the file header.
	if (PtrBitmapRLE[0] == 0x02 && PtrBitmapRLE[1] == 0x00 &&
		PtrBitmapRLE[2] == 0x00 && PtrBitmapRLE[3] == 0x00)
	{
		PtrBitmapRLE += 4;
	}

	// Get the width and height of the bitmap.
	int32 Width = PtrBitmapRLE[0] | (PtrBitmapRLE[1] << 8);
	int32 Height = PtrBitmapRLE[2] | (PtrBitmapRLE[3] << 8);

	if (Width == 0 || Height == 0)
	{
		return nullptr;
	}

	// Calculate the total length of the bitmap.
	int32 Length = Width * Height;
	TArray<FColor> Data;
	Data.Init(FColor::Transparent, Length);
	TArray<FColor>::SizeType Index = 0;

	// Start decoding and blitting the bitmap.
	const TArray<FColor>& Colors = GetDefaultPalette();
	PtrBitmapRLE += 4;
	int32 SrcX = 0;
	int32 DX = 0;
	int32 DY = 0;
	for (int32 i = 0; i < Length;)
	{
		uint8 T = *PtrBitmapRLE++;
		if ((T & 0x80) && T <= 0x80 + Width)
		{
			i += T - 0x80;
			SrcX += T - 0x80;
			if (SrcX >= Width)
			{
				SrcX -= Width;
				DY++;
			}
		}
		else
		{
			// Prepare coordinates.
			int32 j = 0;
			int32 SX = SrcX;
			int32 X = DX + SrcX;
			int32 Y = DY;

			while (j < T)
			{
				// Put the pixels in row onto the texture
				int32 k = FGenericPlatformMath::Min(T - j, Width - SX);
				SX += k;
				Index = Y * Width;
				for (; k != 0; k--)
				{
					FColor color = Colors[PtrBitmapRLE[j]];
					Data[Index + X] = color;
					j++;
					X++;
				}

				if (SX >= Width)
				{
					SX -= Width;
					X -= Width;
					Y++;
				}
			}
			PtrBitmapRLE += T;
			i += T;
			SrcX += T;
			while (SrcX >= Width)
			{
				SrcX -= Width;
				DY++;
			}
		}
	}

	UTexture2D* NewTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	void* TextureData = NewTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, Data.GetData(), 4 * Width * Height);
	NewTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	NewTexture->UpdateResource();

	return NewTexture;
}

void UPALCommon::SolveDistribution()
{
	GameDistribution = EGameDistribution::Win95;
	TArray<UPALMKF*> Files{ LoadMKF("ABC"), LoadMKF("MAP"), LoadMKF("F"), LoadMKF("FBP"), LoadMKF("FIRE"), LoadMKF("MGO") };
	uint8* Data = nullptr;
	SIZE_T DataSize = 0;
	int32 DOSScore = 0, WinScore = 0;

	for (UPALMKF* MKF : Files)
	{
		// Find the first non-empty sub-file
		SIZE_T Count = MKF->GetChunkCount();
		SIZE_T j = 0;
		SIZE_T Size = 0;
		while (j < Count && (Size = MKF->GetChunkSize(j)) < 4)
		{
			j++;
		}
		if (j >= Count)
		{
			return;
		}

		/*
		// Read the content and check the compression signature
		// Note that this check is not 100% correct, however in incorrect situations,
		// the sub-file will be over 784MB if uncompressed, which is highly unlikely.
		*/
		if (DataSize < Size)
		{
			Data = (uint8*)FMemory::Realloc(Data, Size);
			DataSize = Size;
		}
		MKF->ReadChunk(Data, DataSize, j);

		if (Data[0] == 'Y' && Data[1] == 'J' && Data[2] == '_' && Data[3] == '1')
		{
			verify(WinScore <= 0);
			DOSScore++;
		}
		else
		{
			verify(DOSScore <= 0)
			WinScore++;
		}
	}

	// Finally check the size of object definition
	UPALMKF* SSSMKF = LoadMKF("SSS");
	DataSize = SSSMKF->GetChunkSize(2);
	verify(!(DataSize % sizeof(FObjectWin) == 0 && DataSize % sizeof(FObjectDOS) != 0 && DOSScore > 0));
	verify(!(DataSize % sizeof(FObjectDOS) == 0 && DataSize % sizeof(FObjectWin) != 0 && WinScore > 0));

	GameDistribution = (WinScore == Files.Num()) ? EGameDistribution::Win95 : EGameDistribution::DOS;
}

void UPALCommon::LoadDefaultPalette()
{
	DefaultPalette.Init(FColor(), 256);
	uint8* Buffer = (uint8*)FMemory::Malloc(1536);

	// Read the palette data from the pat.mkf file
	UPALMKF* MKF = LoadMKF("pat");
	MKF->ReadChunk(Buffer, 1536, 0);

	for (SIZE_T i = 0; i < 256; i++)
	{
		DefaultPalette[i].R = Buffer[i * 3] << 2;
		DefaultPalette[i].G = Buffer[i * 3 + 1] << 2;
		DefaultPalette[i].B = Buffer[i * 3 + 2] << 2;
		DefaultPalette[i].A = 255;
	}

	FMemory::Free(Buffer);
}

void MultiByteToWideCharCP(EPALCodePage CodePage, const ANSICHAR* MultiByteStringPtr, int32 MultiByteStringLength, FString& OutString)
{
	if (MultiByteStringLength < 0)
	{
		MultiByteStringLength = TCString<ANSICHAR>::Strlen(MultiByteStringPtr);
	}

	TArray<TCHAR> StringBuffer;
	TCHAR InvalidChar;
	int32 State = 0;
	switch (CodePage)
	{
		//case CP_SHIFTJIS:
		//	InvalidChar = 0x30fb;
		//	for (i = 0; i < MultiByteStringLength && Length < wcslength && MultiByteStringPtr[i]; i++)
		//	{
		//		if (State == 0)
		//		{
		//			if ((uint8)MultiByteStringPtr[i] <= 0x80)
		//				wcs[Length++] = MultiByteStringPtr[i];
		//			else if ((uint8)MultiByteStringPtr[i] >= 0xa0 && (uint8)MultiByteStringPtr[i] <= 0xdf)
		//				wcs[Length++] = cptbl_jis_half[(uint8)MultiByteStringPtr[i] - 0xa0];
		//			else if ((uint8)MultiByteStringPtr[i] == 0xfd)
		//				wcs[Length++] = 0xf8f1;
		//			else if ((uint8)MultiByteStringPtr[i] == 0xfe)
		//				wcs[Length++] = 0xf8f2;
		//			else if ((uint8)MultiByteStringPtr[i] == 0xff)
		//				wcs[Length++] = 0xf8f3;
		//			else
		//				State = 1;
		//		}
		//		else
		//		{
		//			if ((uint8)MultiByteStringPtr[i] < 0x40)
		//				wcs[Length++] = 0x30fb;
		//			else if ((uint8)MultiByteStringPtr[i - 1] < 0xa0)
		//				wcs[Length++] = cptbl_jis[(uint8)MultiByteStringPtr[i - 1] - 0x81][(uint8)MultiByteStringPtr[i] - 0x40];
		//			else
		//				wcs[Length++] = cptbl_jis[(uint8)MultiByteStringPtr[i - 1] - 0xc1][(uint8)MultiByteStringPtr[i] - 0x40];
		//			State = 0;
		//		}
		//	}
		//	break;
	case CP_GBK:
		InvalidChar = 0x3f;
		for (SIZE_T i = 0; i < MultiByteStringLength && MultiByteStringPtr[i]; i++)
		{
			if (State == 0)
			{
				if ((uint8)MultiByteStringPtr[i] < 0x80)
					StringBuffer.Add(MultiByteStringPtr[i]);
				else if ((uint8)MultiByteStringPtr[i] == 0x80)
					StringBuffer.Add(0x20ac);
				else if ((uint8)MultiByteStringPtr[i] == 0xff)
					StringBuffer.Add(0xf8f5);
				else
					State = 1;
			}
			else
			{
				if ((uint8)MultiByteStringPtr[i] < 0x40)
					StringBuffer.Add(InvalidChar);
				else
					StringBuffer.Add(CodePageTableGBK[(uint8)MultiByteStringPtr[i - 1] - 0x81][(uint8)MultiByteStringPtr[i] - 0x40]);
				State = 0;
			}
		}
		break;
	case CP_BIG5:
		InvalidChar = 0x3f;
		for (SIZE_T i = 0; i < MultiByteStringLength && MultiByteStringPtr[i]; i++)
		{
			if (State == 0)
			{
				if ((uint8)MultiByteStringPtr[i] <= 0x80)
					StringBuffer.Add(MultiByteStringPtr[i]);
				else if ((uint8)MultiByteStringPtr[i] == 0xff)
					StringBuffer.Add(0xf8f8);
				else
					State = 1;
			}
			else
			{
				if ((uint8)MultiByteStringPtr[i] < 0x40 || ((uint8)MultiByteStringPtr[i] >= 0x7f && (uint8)MultiByteStringPtr[i] <= 0xa0))
					StringBuffer.Add(InvalidChar);
				else if ((uint8)MultiByteStringPtr[i] <= 0x7e)
					StringBuffer.Add(CodePageTableBig5[(uint8)MultiByteStringPtr[i - 1] - 0x81][(uint8)MultiByteStringPtr[i] - 0x40]);
				else
					StringBuffer.Add(CodePageTableBig5[(uint8)MultiByteStringPtr[i - 1] - 0x81][(uint8)MultiByteStringPtr[i] - 0x60]);
				State = 0;
			}
		}
		break;
	case CP_UTF_8:
		InvalidChar = 0x3f;
		for (SIZE_T i = 0; i < MultiByteStringLength && MultiByteStringPtr[i]; i++)
		{
			if (State == 0)
			{
				if ((uint8)MultiByteStringPtr[i] >= 0x80)
				{
					uint8 s = (uint8)MultiByteStringPtr[i] << 1;
					while (s >= 0x80) { State++; s <<= 1; }
					if (State < 1 || State > 3)
					{
						State = 0;
						StringBuffer.Add(InvalidChar);
					}
					else
					{
						StringBuffer.Last() = s >> (State + 1);
					}
				}
				else
					StringBuffer.Add(MultiByteStringPtr[i]);
			}
			else
			{
				if ((uint8)MultiByteStringPtr[i] >= 0x80 && (uint8)MultiByteStringPtr[i] < 0xc0)
				{
					StringBuffer.Last() <<= 6;
					StringBuffer.Last() |= (uint8)MultiByteStringPtr[i] & 0x3f;
					if (--State == 0) StringBuffer.AddDefaulted();
				}
				else
				{
					State = 0;
					StringBuffer.Add(InvalidChar);
				}
			}
		}
		break;
	case CP_UCS:
		for (SIZE_T i = 0; i < MultiByteStringLength; i += 2) {
			StringBuffer.AddDefaulted();
			uint8* ptr = (uint8*)&StringBuffer.Last();
			*(ptr + 1) = MultiByteStringPtr[i];
			*ptr = MultiByteStringPtr[i + 1];
		}
		break;
	default:
		checkNoEntry();
	}
	if (State != 0)
	{
		StringBuffer.Add(InvalidChar);
	}
	OutString = FString(StringBuffer);
}

EPALCodePage DetectCodePageForString(const ANSICHAR* Text, SIZE_T TextLength, EPALCodePage DefaultCodePage, int32& OutProbability)
{
	// Try to convert the content of word.dat with different codepages,
	// and use the codepage with minimal inconvertible characters
	// Works fine currently for detecting Simplified Chinese & Traditional Chinese.
	// Since we're using language files to support additional languages, this detection
	// should be fine for us now.
	int32 MinInvalids = TNumericLimits<int32>::Max();

	if (Text && TextLength > 0)
	{
		// The file to be detected should not contain characters outside these ranges
		const static int32 ValidRanges[][2] = {
			{ 0x4E00, 0x9FFF }, // CJK Unified Ideographs
			{ 0x3400, 0x4DBF }, // CJK Unified Ideographs Extension A
			{ 0xF900, 0xFAFF }, // CJK Compatibility Ideographs
			{ 0x0020, 0x007E }, // Basic ASCII
			{ 0x3000, 0x301E }, // CJK Symbols
			{ 0xFF01, 0xFF5E }, // Fullwidth Forms
		};

		for (EPALCodePage i = CP_BIG5; i <= CP_GBK; i = static_cast<EPALCodePage>(i + 1))
		{
			FString Buffer;
			MultiByteToWideCharCP(i, Text, TextLength, Buffer);
			int32 Invalids = 0;
			SIZE_T Length = Buffer.Len();
			for (SIZE_T j = 0; j < Length; j++)
			{
				int32 Score = 1;
				for (SIZE_T k = 0; k < sizeof(ValidRanges) / sizeof(ValidRanges[0]); k++)
				{
					if (Buffer[j] >= ValidRanges[k][0] && Buffer[j] <= ValidRanges[k][1])
					{
						Score = 0;
						break;
					}
				}
				Invalids += Score;
			}
			// code page with less invalid chars wins
			if (Invalids < MinInvalids)
			{
				MinInvalids = Invalids;
				DefaultCodePage = i;
			}
		}
	}

	if (MinInvalids < TextLength / 2)
	{
		OutProbability = (TextLength / 2 - MinInvalids) * 200 / TextLength;
	}
	else
	{
		OutProbability = 0;
	}

	return DefaultCodePage;
}

EPALCodePage UPALCommon::DetectCodePage(const FString& FileName) const
{
	EPALCodePage DetectedCodePage = EPALCodePage::CP_BIG5;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString WordFilePath = FString(GameResourcePath).Append(FileName);
	IFileHandle* WordFileHandle = PlatformFile.OpenRead(*WordFilePath);

	WordFileHandle->SeekFromEnd(0);
	SIZE_T WordLength = WordFileHandle->Tell();
	ANSICHAR* Buffer = (ANSICHAR*)FMemory::Malloc(WordLength);
	WordFileHandle->SeekFromEnd(0);
	WordFileHandle->Seek(0);
	WordFileHandle->Read((uint8*)Buffer, WordLength);

	// Eliminates null characters so that MultiByteToWideChar works properly
	for (ANSICHAR* Ptr = Buffer; Ptr < Buffer + WordLength; Ptr++)
	{
		if (!*Ptr)
		{
			*Ptr = ' ';
		}
	}

	int32 Probability;
	DetectedCodePage = DetectCodePageForString(Buffer, WordLength, DetectedCodePage, Probability);

	FMemory::Free(Buffer);
	delete WordFileHandle;

	if (Probability == 100)
	{
		UE_LOG(LogInit, Log, TEXT("DetectCodePage detected code page '%s' for %s"), DetectedCodePage ? TEXT("GBK") : TEXT("BIG5"), *FileName);
	}
	else
	{
		UE_LOG(LogInit, Warning, TEXT("DetectCodePage detected the most possible (%d) code page '%s' for %s"), Probability, DetectedCodePage ? TEXT("GBK") : TEXT("BIG5"), *FileName);
	}

	return DetectedCodePage;
}

void UPALCommon::DetectCodePage()
{
	CodePage = DetectCodePage("word.dat");
}

void UPALCommon::MultiByteToWideChar(const ANSICHAR* MultiByteStringPtr, int32 MultiByteStringLength, FString& OutString)
{
	MultiByteToWideCharCP(CodePage, MultiByteStringPtr, MultiByteStringLength, OutString);
}

void UPALCommon::InitWord()
{
	// Open the message and word data files.
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString WordFilePath = FString(GameResourcePath).Append("word.dat");
	IFileHandle* WordFileHandle = PlatformFile.OpenRead(*WordFilePath);

	// See how many words we have
	WordFileHandle->SeekFromEnd(0);
	int64 FileLength = WordFileHandle->Tell();
	check(FileLength > 0);

	// Each word has 10 bytes
	const uint8 WordLength = 10;
	SIZE_T WordCount = (FileLength - 1) / WordLength + 1;
	WordCount = (WordCount < MAX_OBJECTS + 13) ? MAX_OBJECTS + 13 : WordCount;

	// Read the words
	uint8* Temp = (uint8*)FMemory::Malloc(WordLength * WordCount);
	WordFileHandle->Seek(0);
	bool bSucesss = WordFileHandle->Read(Temp, FileLength);
	check(bSucesss);
	FMemory::Memset(Temp + FileLength, 0, WordLength * WordCount - FileLength);

	// Split the words and do code page conversion
	for (SIZE_T i = 0; i < WordCount; i++)
	{
		SIZE_T Base = i * WordLength;
		SIZE_T Pos = Base + WordLength - 1;
		while (Pos >= Base && Temp[Pos] == ' ')
		{
			Temp[Pos--] = 0;
		}
	}
	WordBuffer.SetNum(WordCount);

	for (SIZE_T i = 0; i < WordCount; i++)
	{
		MultiByteToWideChar((ANSICHAR*)Temp + i * WordLength, WordLength, WordBuffer[i]);
		WordBuffer[i].RemoveFromEnd("1");
	}
	FMemory::Free(Temp);
	delete WordFileHandle;
}

void UPALCommon::InitMessage()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString MsgFilePath = FString(GameResourcePath).Append("m.msg");
	IFileHandle* MsgFileHandle = PlatformFile.OpenRead(*MsgFilePath);

	// Read the message offsets. The message offsets are in SSS.MKF #3
	UPALMKF* SSSMKF = LoadMKF("SSS");
	const SIZE_T OffsetCount = SSSMKF->GetChunkSize(3) / sizeof(uint32);
	const SIZE_T MessageCount = OffsetCount - 1;
	uint32* Offsets = (uint32*)FMemory::Malloc(OffsetCount * sizeof(uint32));
	SSSMKF->ReadChunk((uint8*)(Offsets), OffsetCount * sizeof(uint32), 3);

	// Read the messages.
	MsgFileHandle->SeekFromEnd();
	SIZE_T FileSize = MsgFileHandle->Tell();
	uint8* Temp = (uint8*)FMemory::Malloc(FileSize);
	MsgFileHandle->Seek(0);
	bool bSuccess = MsgFileHandle->Read(Temp, FileSize);
	check(bSuccess);

	delete MsgFileHandle;

	// Split messages and do code page conversion here
	MessageBuffer.SetNum(MessageCount);
	for (SIZE_T i = 0, Length = 0; i < MessageCount; i++)
	{
		MultiByteToWideChar((ANSICHAR*)Temp + Offsets[i], Offsets[i + 1] - Offsets[i], MessageBuffer[i]);
	}

	FMemory::Free(Temp);
	FMemory::Free(Offsets);
}

void UPALCommon::InitText()
{
	DetectCodePage();
	InitWord();
	InitMessage();
}

void UPALCommon::SolveSoundFormat()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (GameDistribution == EGameDistribution::Win95)
	{
		FString FileFullPath = FString(GameResourcePath).Append("Sounds.mkf");
		SoundFormat = PlatformFile.FileExists(*FileFullPath) ? EPALSoundFormat::WAVE : EPALSoundFormat::VOC;
	}
	else
	{
		FString FileFullPath = FString(GameResourcePath).Append("VOC.mkf");
		SoundFormat = PlatformFile.FileExists(*FileFullPath) ? EPALSoundFormat::VOC : EPALSoundFormat::WAVE;
	}
}

void UPALCommon::Initialize(FSubsystemCollectionBase& Collection)
{
	verify(FGenericPlatformProperties::IsLittleEndian());
	if (!GameResourcePath.IsEmpty() && !GameResourcePath.EndsWith(FGenericPlatformMisc::GetDefaultPathSeparator()))
	{
		GameResourcePath.Append(FGenericPlatformMisc::GetDefaultPathSeparator());
	}
	SolveDistribution();
	LoadDefaultPalette();
	InitText();
	UniFont = LoadObject<UFont>(nullptr, TEXT("/Script/Engine.Font'/Game/unifont_Font.unifont_Font'"));
	SolveSoundFormat();
}
