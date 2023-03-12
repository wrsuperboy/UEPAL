// Copyright (C) 2022 Meizhouxuanhan.


#include "RixPlayer.h"

const uint16 RixPlayer::NoteTable[12] =
{ 363, 385, 408, 432, 458, 485, 514, 544, 577, 611, 647, 686 };

const uint8 RixPlayer::OpTable[9] =
{ 0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0x10, 0x11, 0x12 };

const uint8 RixPlayer::AdFlag[] = { 0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1 };
const uint8 RixPlayer::RegData[] = { 0,1,2,3,4,5,8,9,10,11,12,13,16,17,18,19,20,21 };
const uint8 RixPlayer::AdC0Offs[] = { 0,1,2,0,1,2,3,4,5,3,4,5,6,7,8,6,7,8 };
const uint8 RixPlayer::Modify[] = { 0,3,1,4,2,5,6,9,7,10,8,11,12,15,13,16,14,17,12,15,16,0,14,0,17,0,13,0 };
const uint8 RixPlayer::BdRegData[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x08,0x04,0x02,0x01,
	0x00,0x01,0x01,0x03,0x0F,0x05,0x00,0x01,0x03,0x0F,0x00,
	0x00,0x00,0x01,0x00,0x00,0x01,0x01,0x0F,0x07,0x00,0x02,
	0x04,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x0A,
	0x04,0x00,0x08,0x0C,0x0B,0x00,0x00,0x00,0x01,0x00,0x00,
	0x00,0x00,0x0D,0x04,0x00,0x06,0x0F,0x00,0x00,0x00,0x00,
	0x01,0x00,0x00,0x0C,0x00,0x0F,0x0B,0x00,0x08,0x05,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x0F,0x0B,0x00,
	0x07,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,
	0x0F,0x0B,0x00,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x00,0x0F,0x0B,0x00,0x07,0x05,0x00,0x00,0x00,
	0x00,0x00,0x00 };
uint8 RixPlayer::For40Reg[] = { 0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F };
const uint16 RixPlayer::MusTime = 0x4268;

RixPlayer::RixPlayer(Opl* NewOpl)
	: MyOpl(NewOpl), FileHandle(nullptr), bMKF(false), RixBuf(nullptr)
{
	verify(FGenericPlatformProperties::IsLittleEndian());
}

RixPlayer::~RixPlayer()
{
	if (FileHandle)
	{
		delete FileHandle;
	}
	if (RixBuf)
	{
		FMemory::Free(RixBuf);
	}
}

bool RixPlayer::Load(IFileHandle* InFileHandle, bool bInMKF)
{
	FileHandle = InFileHandle;
	bMKF = bInMKF;
	if (bMKF)
	{
		FileHandle->Seek(0);
		int32 Offset;
		FileHandle->Read((uint8*)&Offset, sizeof(int32));
		FileHandle->Seek(Offset);
	}
	uint16 Signature;
	FileHandle->Read((uint8*)&Signature, sizeof(uint16));

	if (Signature != 0x55aa)
	{
		return false;
	}
	if (!bMKF)
	{
		Length = FileHandle->Size();
		RixBuf = (uint8*)FMemory::Realloc(RixBuf, Length);
		FileHandle->Seek(0);
		FileHandle->Read((uint8*)RixBuf, Length);
		Subsongs = 1;
	}
	else
	{
		FileHandle->Seek(0);
		FileHandle->Read((uint8*)&Subsongs, sizeof(int32));
		Subsongs /= 4;
	}
	Rewind(0);
	return true;
}

bool RixPlayer::Update()
{
	Int08hEntry();
	return !bPlayEnd;
}

void RixPlayer::Rewind(int32 Subsong)
{
	RewindReInit(Subsong, true);
}

void RixPlayer::RewindReInit(int32 Subsong, bool bReInit)
{
	bPlayEnd = false;
	Pos = 0;

	if (bReInit)
	{
		I = 0; T = 0;
		MusBlock = 0;
		InsBlock = 0;
		Rhythm = 0;
		bMusicOn = false;
		bPause = false;
		Band = 0;
		BandLow = 0;
		E0RegFlag = 0;
		BdModify = 0;
		Sustain = 0;

		FMemory::Memset(FBuffer, 0, sizeof(FBuffer));
		FMemory::Memset(A0b0Data2, 0, sizeof(A0b0Data2));
		FMemory::Memset(A0b0Data3, 0, sizeof(A0b0Data3));
		FMemory::Memset(A0b0Data4, 0, sizeof(A0b0Data4));
		FMemory::Memset(A0b0Data5, 0, sizeof(A0b0Data5));
		FMemory::Memset(AddressHead, 0, sizeof(AddressHead));
		FMemory::Memset(InsBuf, 0, sizeof(InsBuf));
		FMemory::Memset(Displace, 0, sizeof(Displace));
		FMemory::Memset(RegBufs, 0, sizeof(RegBufs));
		FMemory::Memset(For40Reg, 0x7F, sizeof(For40Reg));
	}

	if (bMKF)
	{
		int32 Index, Index2;
		FileHandle->Seek(Subsong * 4);
		FileHandle->Read((uint8*)&Index, sizeof(int32));
		FileHandle->Read((uint8*)&Index2, sizeof(int32));
		Length = Index2 - Index;
		if (Length == 0)
		{
			return;
		}
		FileHandle->Seek(Index);
		RixBuf = (uint8*)FMemory::Realloc(RixBuf, Length);
		FMemory::Memset(RixBuf, 0, Length);
		FileHandle->Read(RixBuf, Length);
	}

	if (bReInit)
	{
		MyOpl->Init();
		MyOpl->Write(1, 32);	// go to OPL2 mode
		SetNewInt();
		DataInitial();
	}
}

float RixPlayer::GetRefresh()
{
	return 70.0f;
}

uint32 RixPlayer::GetSubsongs()
{
	return Subsongs;
}

float RixPlayer::SongLength(int32 Subsong) {
	Opl TempOpl;
	Opl* SaveOpl = MyOpl;
	float Duration = 0.0f;

	// save original OPL from being overwritten
	MyOpl = &TempOpl;

	// get song length
	Rewind(Subsong);
	while (Update() && Duration < 600000)
	{
		// song length limit: 10 minutes
		Duration += 1000.0f / GetRefresh();
	}

	Rewind(Subsong);

	// restore original OPL and return
	MyOpl = SaveOpl;
	return Duration;
}

#define Ad08Reg() AdBop(8,0)

/*------------------Implemention----------------------------*/
FORCEINLINE void RixPlayer::SetNewInt()
{
	// if(!Adinitial()) exit(1);
	AdInitial();
}
/*----------------------------------------------------------*/
FORCEINLINE void RixPlayer::Pause()
{
	bPause = true;
	for (uint16 i = 0; i < 11; i++)
	{
		SwitchAdBd(i);
	}
}
/*----------------------------------------------------------*/
FORCEINLINE void RixPlayer::AdA0b0lReg_(uint16 Index, uint16 P2, uint16 P3)
{
	// uint16 i = P2+A0b0Data2[Index];
	A0b0Data4[Index] = P3;
	A0b0Data3[Index] = P2;
}

FORCEINLINE void RixPlayer::DataInitial()
{
	Rhythm = RixBuf[2];
	MusBlock = (RixBuf[0x0D] << 8) + RixBuf[0x0C];
	InsBlock = (RixBuf[0x09] << 8) + RixBuf[0x08];
	I = MusBlock + 1;
	if (Rhythm != 0)
	{
		AdA0b0Reg(6);
		AdA0b0Reg(7);
		AdA0b0Reg(8);
		AdA0b0lReg_(8, 0x18, 0);
		AdA0b0lReg_(7, 0x1F, 0);

		MyOpl->Write(0xa8, 87);
		MyOpl->Write(0xb8, 9);
		MyOpl->Write(0xa7, 3);
		MyOpl->Write(0xb7, 15/*10*/);	// Changed from 10 (original Value) to 15 for better quality
	}
	BdModify = 0;
	AdBdReg();
	Band = 0;
	bMusicOn = true;
}
/*----------------------------------------------------------*/
FORCEINLINE uint16 RixPlayer::AdInitial()
{
	for (uint16 i = 0; i < 25; i++)
	{
		uint32 Res = ((uint32)i * 24 + 10000) * 52088 / 250000 * 0x24000 / 0x1B503;
		FBuffer[i * 12] = ((uint16)Res + 4) >> 3;
		for (int32 t = 1; t < 12; t++)
		{
			Res *= 1.06;
			FBuffer[i * 12 + t] = ((uint16)Res + 4) >> 3;
		}
	}
	uint16 k = 0;
	for (uint16 i = 0; i < 8; i++)
	{
		for (uint16 j = 0; j < 12; j++)
		{
			A0b0Data5[k] = i;
			AddressHead[k] = j;
			k++;
		}
	}
	AdBdReg();
	Ad08Reg();
	for (uint16 i = 0; i < 9; i++)
	{
		AdA0b0Reg(i);
	}
	E0RegFlag = 0x20;
	for (uint16 i = 0; i < 18; i++)
	{
		AdBop(0xE0 + RegData[i], 0);
	}
	AdBop(1, E0RegFlag);
	return 1;//Adtest();
}
/*----------------------------------------------------------*/
FORCEINLINE void RixPlayer::AdBop(uint16 Reg, uint16 Value)
{
	//if(Reg == 2 || Reg == 3)
	//  AdPlug_LogWrite("switch OPL2/3 mode!\n");
	MyOpl->Write(Reg & 0xff, Value & 0xff);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::Int08hEntry()
{
	uint16 BandSus = 1;
	while (BandSus)
	{
		if (Sustain <= 0)
		{
			BandSus = RixProc();
			if (BandSus)
			{
				Sustain += BandSus;
			}
			else
			{
				bPlayEnd = true;
				break;
			}
		}
		else
		{
			if (BandSus)
			{
				Sustain -= 14; /* aging */
			}
			break;
		}
	}
}
/*--------------------------------------------------------------*/
FORCEINLINE uint16 RixPlayer::RixProc()
{
	uint8 Control = 0;
	if (!bMusicOn || bPause)
	{
		return 0;
	}
	Band = 0;
	while (I < Length && RixBuf[I] != 0x80)
	{
		BandLow = RixBuf[I - 1];
		Control = RixBuf[I];
		I += 2;
		switch (Control & 0xF0)
		{
		case 0x90:  RixGetIns(); Rix90Pro(Control & 0x0F); break;
		case 0xA0:  RixA0Pro(Control & 0x0F, ((uint16)BandLow) << 6); break;
		case 0xB0:  RixB0Pro(Control & 0x0F, BandLow); break;
		case 0xC0:  SwitchAdBd(Control & 0x0F);
			if (BandLow != 0)
			{
				RixC0Pro(Control & 0x0F, BandLow);
			}
			break;
		default:
			Band = (Control << 8) + BandLow;
			break;
		}
		if (Band != 0)
		{
			return Band;
		}
	}
	MusicCtrl();
	I = MusBlock + 1;
	Band = 0;
	bMusicOn = true;
	return 0;
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::RixGetIns()
{
	if (InsBlock + (BandLow << 6) + sizeof(InsBuf) >= Length)
	{
		return;
	}
	uint8* baddr = (&RixBuf[InsBlock]) + (BandLow << 6);

	for (int32 i = 0; i < 28; i++)
	{
		InsBuf[i] = (baddr[i * 2 + 1] << 8) + baddr[i * 2];
	}
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::Rix90Pro(uint16 CtrlL)
{
	if (CtrlL >= 11)
	{
		return;
	}
	if (Rhythm == 0 || CtrlL < 6)
	{
		InsToReg(Modify[CtrlL * 2], InsBuf, InsBuf[26]);
		InsToReg(Modify[CtrlL * 2 + 1], InsBuf + 13, InsBuf[27]);
	}
	else if (CtrlL > 6)
	{
		InsToReg(Modify[CtrlL * 2 + 6], InsBuf, InsBuf[26]);
	}
	else
	{
		InsToReg(12, InsBuf, InsBuf[26]);
		InsToReg(15, InsBuf + 13, InsBuf[27]);
	}
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::RixA0Pro(uint16 CtrlL, uint16 Index)
{
	if (Rhythm == 0 || CtrlL <= 6)
	{
		PrepareA0b0(CtrlL, Index > 0x3FFF ? 0x3FFF : Index);
		AdA0b0lReg(CtrlL, A0b0Data3[CtrlL], A0b0Data4[CtrlL]);
	}
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::PrepareA0b0(uint16 Index, uint16 v)  /* important !*/
{
	if (Index >= 11)
	{
		return;
	}
	int32 Res1 = (v - 0x2000) * 0x19;
	if (Res1 == (int32)0xff)
	{
		return;
	}
	int16 High = 0;
	int16 Low = Res1 / 0x2000;
	uint32 Res;
	if (Low < 0)
	{
		Low = 0x18 - Low; High = (int16)Low < 0 ? 0xFFFF : 0;
		Res = High; Res <<= 16; Res += Low;
		Low = ((int16)Res) / static_cast<int16>(0xFFE7);
		A0b0Data2[Index] = Low;
		Low = Res;
		Res = Low - 0x18;
		High = (int16)Res % 0x19;
		Low = (int16)Res / 0x19;
		if (High != 0)
		{
			Low = 0x19;
			Low = Low - High;
		}
	}
	else
	{
		Res = High = Low;
		Low = (int16)Res / (int16)0x19;
		A0b0Data2[Index] = Low;
		Res = High;
		Low = (int16)Res % (int16)0x19;
	}
	Low = (int16)Low * (int16)0x18;
	Displace[Index] = Low;
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::AdA0b0lReg(uint16 Index, uint16 P2, uint16 P3)
{
	uint16 i = P2 + A0b0Data2[Index];
	A0b0Data4[Index] = P3;
	A0b0Data3[Index] = P2;
	i = ((int16)i <= 0x5F ? i : 0x5F);
	i = ((int16)i >= 0 ? i : 0);
	uint16 Data = FBuffer[AddressHead[i] + Displace[Index] / 2];
	AdBop(0xA0 + Index, Data);
	Data = A0b0Data5[i] * 4 + (P3 < 1 ? 0 : 0x20) + ((Data >> 8) & 3);
	AdBop(0xB0 + Index, Data);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::RixB0Pro(uint16 CtrlL, uint16 Index)
{
	if (CtrlL >= 11)
	{
		return;
	}
	int32 Temp = 0;
	if (Rhythm == 0 || CtrlL < 6)
	{
		Temp = Modify[CtrlL * 2 + 1];
	}
	else
	{
		Temp = CtrlL > 6 ? CtrlL * 2 : CtrlL * 2 + 1;
		Temp = Modify[Temp + 6];
	}
	For40Reg[Temp] = Index > 0x7F ? 0x7F : Index;
	Ad40Reg(Temp);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::RixC0Pro(uint16 CtrlL, uint16 Index)
{
	uint16 i = Index >= 12 ? Index - 12 : 0;
	if (CtrlL < 6 || Rhythm == 0)
	{
		AdA0b0lReg(CtrlL, i, 1);
	}
	else
	{
		if (CtrlL != 6)
		{
			if (CtrlL == 8)
			{
				AdA0b0lReg(CtrlL, i, 0);
				AdA0b0lReg(7, i + 7, 0);
			}
		}
		else
		{
			AdA0b0lReg(CtrlL, i, 0);
		}
		BdModify |= BdRegData[CtrlL];
		AdBdReg();
	}
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::SwitchAdBd(uint16 Index)
{
	if (Rhythm == 0 || Index < 6)
	{
		AdA0b0lReg(Index, A0b0Data3[Index], 0);
	}
	else
	{
		BdModify &= (~BdRegData[Index]),
			AdBdReg();
	}
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::InsToReg(uint16 Index, uint16* insb, uint16 Value)
{
	for (uint16 i = 0; i < 13; i++)
	{
		RegBufs[Index].v[i] = insb[i];
	}
	RegBufs[Index].v[13] = Value & 3;
	AdBdReg(), Ad08Reg(),
		Ad40Reg(Index), AdC0Reg(Index), Ad60Reg(Index),
		Ad80Reg(Index), Ad20Reg(Index), AdE0Reg(Index);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::AdE0Reg(uint16 Index)
{
	uint16 Data = E0RegFlag == 0 ? 0 : (RegBufs[Index].v[13] & 3);
	AdBop(0xE0 + RegData[Index], Data);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::Ad20Reg(uint16 Index)
{
	uint16 Data = (RegBufs[Index].v[9] < 1 ? 0 : 0x80);
	Data += (RegBufs[Index].v[10] < 1 ? 0 : 0x40);
	Data += (RegBufs[Index].v[5] < 1 ? 0 : 0x20);
	Data += (RegBufs[Index].v[11] < 1 ? 0 : 0x10);
	Data += (RegBufs[Index].v[1] & 0x0F);
	AdBop(0x20 + RegData[Index], Data);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::Ad80Reg(uint16 Index)
{
	uint16 Data = (RegBufs[Index].v[7] & 0x0F), Temp = RegBufs[Index].v[4];
	Data |= (Temp << 4);
	AdBop(0x80 + RegData[Index], Data);
}
/*--------------------------------------------------------------*/
void RixPlayer::Ad60Reg(uint16 Index)
{
	uint16 Data = RegBufs[Index].v[6] & 0x0F, Temp = RegBufs[Index].v[3];
	Data |= (Temp << 4);
	AdBop(0x60 + RegData[Index], Data);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::AdC0Reg(uint16 Index)
{
	uint16 Data = RegBufs[Index].v[2];
	if (AdFlag[Index] == 1)
	{
		return;
	}
	Data *= 2,
		Data |= (RegBufs[Index].v[12] < 1 ? 1 : 0);
	AdBop(0xC0 + AdC0Offs[Index], Data);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::Ad40Reg(uint16 Index)
{
	uint32 Res = 0;
	uint16 Data = 0, Temp = RegBufs[Index].v[0];
	Data = 0x3F - (0x3F & RegBufs[Index].v[8]),
		Data *= For40Reg[Index],
		Data *= 2,
		Data += 0x7F,
		Res = Data;
	Data = Res / 0xFE,
		Data -= 0x3F,
		Data = -Data,
		Data |= Temp << 6;
	AdBop(0x40 + RegData[Index], Data);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::AdBdReg()
{
	uint16 Data = Rhythm < 1 ? 0 : 0x20;
	Data |= BdModify;
	AdBop(0xBD, Data);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::AdA0b0Reg(uint16 Index)
{
	AdBop(0xA0 + Index, 0);
	AdBop(0xB0 + Index, 0);
}
/*--------------------------------------------------------------*/
FORCEINLINE void RixPlayer::MusicCtrl()
{
	for (int32 i = 0; i < 11; i++)
	{
		SwitchAdBd(i);
	}
}
