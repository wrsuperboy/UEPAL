// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"
#include "Opl.h"

/**
 * 
 */
class PAL_API RixPlayer
{
public:
	RixPlayer(Opl* NewOpl);
	~RixPlayer();

	bool Load(IFileHandle* FileHandle, bool bMKF);
	bool Update(); // executes replay code for 1 tick
	void Rewind(int32 Subsong); // rewinds to specified subsong
	void RewindReInit(int32 Subsong, bool bReInit); /* For seamless continuous */
	float GetRefresh(); // returns needed timer refresh rate
	uint32 GetSubsongs();
	float SongLength(int32 Subsong);

protected:
	Opl* MyOpl;	// our OPL chip

	static const uint16	NoteTable[12];	// standard adlib note table
	static const uint8 OpTable[9];	// the 9 operators as expected by the OPL

	struct ADDT {
		uint8 v[14];
	};

	IFileHandle* FileHandle;
	bool bMKF;
	int32 Subsongs;
	uint8* RixBuf;  /* rix files' f_buffer */
	uint16 FBuffer[300];//9C0h-C18h
	uint16 A0b0Data2[11];
	uint8 A0b0Data3[18];
	uint8 A0b0Data4[18];
	uint8 A0b0Data5[96];
	uint8 AddressHead[96];
	uint16 InsBuf[28];
	uint16 Displace[11];
	ADDT RegBufs[18];
	uint32 Pos;
	uint32 Length;

	static const uint8 AdFlag[18];
	static const uint8 RegData[18];
	static const uint8 AdC0Offs[18];
	static const uint8 Modify[28];
	static const uint8 BdRegData[124];
	static uint8 For40Reg[18];
	static const uint16 MusTime;
	uint32 I, T;
	uint16 MusBlock;
	uint16 InsBlock;
	uint8 Rhythm;
	bool bMusicOn;
	bool bPause;
	uint16 Band;
	uint8 BandLow;
	uint16 E0RegFlag;
	uint8 BdModify;
	int32 Sustain;
	bool bPlayEnd;

	void Ad20Reg(uint16);              /**/
	void Ad40Reg(uint16);              /**/
	void Ad60Reg(uint16);              /**/
	void Ad80Reg(uint16);              /**/
	void AdA0b0Reg(uint16);            /**/
	void AdA0b0lReg(uint16, uint16, uint16); /**/
	void AdA0b0lReg_(uint16, uint16, uint16); /**/
	void AdBdReg();                  /**/
	void AdBop(uint16, uint16);                     /**/
	void AdC0Reg(uint16);              /**/
	void AdE0Reg(uint16);              /**/
	uint16 AdInitial();                 /**/
	void DataInitial();               /* done */
	void InsToReg(uint16, uint16*, uint16);  /**/
	void Int08hEntry();    /**/
	void MusicCtrl();                 /**/
	void Pause();                      /**/
	void PrepareA0b0(uint16, uint16);      /**/
	void Rix90Pro(uint16);             /**/
	void RixA0Pro(uint16, uint16);        /**/
	void RixB0Pro(uint16, uint16);        /**/
	void RixC0Pro(uint16, uint16);        /**/
	void RixGetIns();                /**/
	uint16 RixProc();                   /**/
	void SetNewInt();
	void SwitchAdBd(uint16);           /**/
};
