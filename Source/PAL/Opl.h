// Copyright (C) 2022 Meizhouxuanhan.

#pragma once

#include "CoreMinimal.h"

enum EOplCoreType : uint8
{
	OPLCORE_MAME,
	OPLCORE_DBFLT,
	OPLCORE_DBINT,
	OPLCORE_NUKED,
};

enum EOplChipType : uint8
{
	OPLCHIP_OPL2,
	OPLCHIP_OPL3,

	OPLCHIP_DUAL_OPL2,
};

constexpr uint32 OPL3_EXTREG_BASE = 0x100;
constexpr uint32 OPL3_4OP_REGISTER = 0x104;
constexpr uint32 OPL3_MODE_REGISTER = 0x105;

struct Opl3WriteBuf {
	uint64 Time;
	uint16 Reg;
	uint8 Data;
};

constexpr uint32 OPL_WRITEBUF_SIZE = 1024;
constexpr uint64 OPL_WRITEBUF_DELAY = 2;

typedef struct _Opl3Channel Opl3Channel;
typedef struct _Opl3Slot Opl3Slot;
typedef struct _Opl3Chip Opl3Chip;

struct _Opl3Slot {
	Opl3Channel* Channel;
	Opl3Chip* Chip;
	int16 Out;
	int16 FbMod;
	int16* Mod;
	int16 Prout;
	uint16 EgRout;
	uint16 EgOut;
	uint8 EgInc;
	uint8 EgGen;
	uint8 EgRate;
	uint8 EgKsl;
	uint8* Trem;
	uint8 RegVib;
	uint8 RegType;
	uint8 RegKsr;
	uint8 RegMult;
	uint8 RegKsl;
	uint8 RegTl;
	uint8 RegAr;
	uint8 RegDr;
	uint8 RegSl;
	uint8 RegRr;
	uint8 RegWf;
	uint8 Key;
	uint32 PgReset;
	uint32 PgPhase;
	uint16 PgPhaseOut;
	uint8 SlotNum;
};

struct _Opl3Channel {
	Opl3Slot* Slots[2];
	Opl3Channel* Pair;
	Opl3Chip* Chip;
	int16* Out[4];
	uint8 ChType;
	uint16 FNum;
	uint8 Block;
	uint8 Fb;
	uint8 Con;
	uint8 Alg;
	uint8 Ksv;
	uint16 Cha, Chb;
	uint8 ChNum;
};

struct _Opl3Chip {
	Opl3Channel Channel[18];
	Opl3Slot Slot[36];
	uint16 Timer;
	uint64 EgTimer;
	uint8 EgTimerRem;
	uint8 EgState;
	uint8 EgAdd;
	uint8 NewM;
	uint8 Nts;
	uint8 Rhy;
	uint8 VibPos;
	uint8 VibShift;
	uint8 Tremolo;
	uint8 TremoloPos;
	uint8 TremoloShift;
	uint32 Noise;
	int16 ZeroMod;
	int32 MixBuff[2];
	uint8 RmHhBit2;
	uint8 RmHhBit3;
	uint8 RmHhBit7;
	uint8 RmHhBit8;
	uint8 RmTcBit3;
	uint8 RmTcBit5;
	//OPL3L
	int32 RateRatio;
	int32 SampleCnt;
	int16 OldSamples[2];
	int16 Samples[2];

	uint64 WriteBufSampleCnt;
	uint32 WriteBufCur;
	uint32 WriteBufLast;
	uint64 WriteBufLastTime;
	Opl3WriteBuf WriteBuf[OPL_WRITEBUF_SIZE];
};


/**
 * 
 */
class PAL_API Opl
{
public:
	Opl() : CurrChip(0), CurrType(OPLCHIP_OPL2) {};
	Opl(EOplChipType Type) : CurrChip(0), CurrType(OPLCHIP_OPL2) {};

	virtual ~Opl() {}

	virtual void Init() {}; // reinitialize OPL chip(s)
	virtual void Write(int32 Reg, int32 Val) {}; // combined register select + data write
	virtual void Update(int16* Buf, int32 Samples) {}; // Emulation only: fill buffer

	virtual void SetChip(int32 n)
	{
		// select OPL chip
		CurrChip = (n < 2 && CurrType == OPLCHIP_DUAL_OPL2) ? n : 0;
	}

	
	virtual int32 GetChip()
	{
		// returns current OPL chip
		return CurrChip;
	}

	EOplChipType GetType() {
		// return this OPL chip's type
		return CurrType;
	}

	virtual bool IsStereo() {	// return if this OPL chip output stereo
		return CurrType == OPLCHIP_OPL3 || CurrType == OPLCHIP_DUAL_OPL2;
	}

protected:
	int32 CurrChip;		// currently selected OPL chip number
	EOplChipType CurrType;		// this OPL chip's type
};

class PAL_API OplCore {
public:
	OplCore(uint32 InRate) : Rate(InRate) {}
	virtual ~OplCore() {}

	virtual void Reset() {};
	virtual void Write(uint32 Reg, uint8 Val) {};
	virtual void Generate(int16* Buf, int32 Samples) {};
	virtual OplCore* Duplicate() { return nullptr; };

protected:
	uint32 Rate;
};

class NukedOpl3 : public OplCore
{
public:
	NukedOpl3(uint32 SampleRate);

	virtual void Reset() override;
	virtual void Write(uint32 Reg, uint8 Val) override;
	virtual void Generate(int16* Buf, int32 Samples) override;
	virtual OplCore* Duplicate() override;

private:
	Opl3Chip Chip;
};

class PAL_API EmuOpl : public Opl {
public:
	static Opl* CreateEmuopl(EOplCoreType Core, EOplChipType Type, int32 Rate);

	~EmuOpl();

	// Assumes a 16-bit, mono output sample buffer @ OPL2 mode
	// Assumes a 16-bit, stereo output sample buffer @ OPL3/DUAL_OPL2 mode
	virtual void Update(int16* Buf, int32 Samples) override;

	virtual void Write(int32 Reg, int32 Val) override;

	void Init();

protected:
	EmuOpl(OplCore* Core, EOplChipType Type);

	OplCore* OplCores[2];
	bool bOpl3Mode;
};