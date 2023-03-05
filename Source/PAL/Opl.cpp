// Copyright (C) 2022 Meizhouxuanhan.


#include "Opl.h"

#define RSM_FRAC	10

enum EOplChannelType : uint8
{
	ch_2op = 0,
	ch_4op = 1,
	ch_4op2 = 2,
	ch_drum = 3
};

enum EEnvelopeKeyType : uint8
{
	egk_norm = 0x01,
	egk_drum = 0x02
};

// logsin table
static const uint16 LogSinRom[256] = {
	0x859, 0x6c3, 0x607, 0x58b, 0x52e, 0x4e4, 0x4a6, 0x471,
	0x443, 0x41a, 0x3f5, 0x3d3, 0x3b5, 0x398, 0x37e, 0x365,
	0x34e, 0x339, 0x324, 0x311, 0x2ff, 0x2ed, 0x2dc, 0x2cd,
	0x2bd, 0x2af, 0x2a0, 0x293, 0x286, 0x279, 0x26d, 0x261,
	0x256, 0x24b, 0x240, 0x236, 0x22c, 0x222, 0x218, 0x20f,
	0x206, 0x1fd, 0x1f5, 0x1ec, 0x1e4, 0x1dc, 0x1d4, 0x1cd,
	0x1c5, 0x1be, 0x1b7, 0x1b0, 0x1a9, 0x1a2, 0x19b, 0x195,
	0x18f, 0x188, 0x182, 0x17c, 0x177, 0x171, 0x16b, 0x166,
	0x160, 0x15b, 0x155, 0x150, 0x14b, 0x146, 0x141, 0x13c,
	0x137, 0x133, 0x12e, 0x129, 0x125, 0x121, 0x11c, 0x118,
	0x114, 0x10f, 0x10b, 0x107, 0x103, 0x0ff, 0x0fb, 0x0f8,
	0x0f4, 0x0f0, 0x0ec, 0x0e9, 0x0e5, 0x0e2, 0x0de, 0x0db,
	0x0d7, 0x0d4, 0x0d1, 0x0cd, 0x0ca, 0x0c7, 0x0c4, 0x0c1,
	0x0be, 0x0bb, 0x0b8, 0x0b5, 0x0b2, 0x0af, 0x0ac, 0x0a9,
	0x0a7, 0x0a4, 0x0a1, 0x09f, 0x09c, 0x099, 0x097, 0x094,
	0x092, 0x08f, 0x08d, 0x08a, 0x088, 0x086, 0x083, 0x081,
	0x07f, 0x07d, 0x07a, 0x078, 0x076, 0x074, 0x072, 0x070,
	0x06e, 0x06c, 0x06a, 0x068, 0x066, 0x064, 0x062, 0x060,
	0x05e, 0x05c, 0x05b, 0x059, 0x057, 0x055, 0x053, 0x052,
	0x050, 0x04e, 0x04d, 0x04b, 0x04a, 0x048, 0x046, 0x045,
	0x043, 0x042, 0x040, 0x03f, 0x03e, 0x03c, 0x03b, 0x039,
	0x038, 0x037, 0x035, 0x034, 0x033, 0x031, 0x030, 0x02f,
	0x02e, 0x02d, 0x02b, 0x02a, 0x029, 0x028, 0x027, 0x026,
	0x025, 0x024, 0x023, 0x022, 0x021, 0x020, 0x01f, 0x01e,
	0x01d, 0x01c, 0x01b, 0x01a, 0x019, 0x018, 0x017, 0x017,
	0x016, 0x015, 0x014, 0x014, 0x013, 0x012, 0x011, 0x011,
	0x010, 0x00f, 0x00f, 0x00e, 0x00d, 0x00d, 0x00c, 0x00c,
	0x00b, 0x00a, 0x00a, 0x009, 0x009, 0x008, 0x008, 0x007,
	0x007, 0x007, 0x006, 0x006, 0x005, 0x005, 0x005, 0x004,
	0x004, 0x004, 0x003, 0x003, 0x003, 0x002, 0x002, 0x002,
	0x002, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};

// exp table
static const uint16 ExpRom[256] = {
	0x7fa, 0x7f5, 0x7ef, 0x7ea, 0x7e4, 0x7df, 0x7da, 0x7d4,
	0x7cf, 0x7c9, 0x7c4, 0x7bf, 0x7b9, 0x7b4, 0x7ae, 0x7a9,
	0x7a4, 0x79f, 0x799, 0x794, 0x78f, 0x78a, 0x784, 0x77f,
	0x77a, 0x775, 0x770, 0x76a, 0x765, 0x760, 0x75b, 0x756,
	0x751, 0x74c, 0x747, 0x742, 0x73d, 0x738, 0x733, 0x72e,
	0x729, 0x724, 0x71f, 0x71a, 0x715, 0x710, 0x70b, 0x706,
	0x702, 0x6fd, 0x6f8, 0x6f3, 0x6ee, 0x6e9, 0x6e5, 0x6e0,
	0x6db, 0x6d6, 0x6d2, 0x6cd, 0x6c8, 0x6c4, 0x6bf, 0x6ba,
	0x6b5, 0x6b1, 0x6ac, 0x6a8, 0x6a3, 0x69e, 0x69a, 0x695,
	0x691, 0x68c, 0x688, 0x683, 0x67f, 0x67a, 0x676, 0x671,
	0x66d, 0x668, 0x664, 0x65f, 0x65b, 0x657, 0x652, 0x64e,
	0x649, 0x645, 0x641, 0x63c, 0x638, 0x634, 0x630, 0x62b,
	0x627, 0x623, 0x61e, 0x61a, 0x616, 0x612, 0x60e, 0x609,
	0x605, 0x601, 0x5fd, 0x5f9, 0x5f5, 0x5f0, 0x5ec, 0x5e8,
	0x5e4, 0x5e0, 0x5dc, 0x5d8, 0x5d4, 0x5d0, 0x5cc, 0x5c8,
	0x5c4, 0x5c0, 0x5bc, 0x5b8, 0x5b4, 0x5b0, 0x5ac, 0x5a8,
	0x5a4, 0x5a0, 0x59c, 0x599, 0x595, 0x591, 0x58d, 0x589,
	0x585, 0x581, 0x57e, 0x57a, 0x576, 0x572, 0x56f, 0x56b,
	0x567, 0x563, 0x560, 0x55c, 0x558, 0x554, 0x551, 0x54d,
	0x549, 0x546, 0x542, 0x53e, 0x53b, 0x537, 0x534, 0x530,
	0x52c, 0x529, 0x525, 0x522, 0x51e, 0x51b, 0x517, 0x514,
	0x510, 0x50c, 0x509, 0x506, 0x502, 0x4ff, 0x4fb, 0x4f8,
	0x4f4, 0x4f1, 0x4ed, 0x4ea, 0x4e7, 0x4e3, 0x4e0, 0x4dc,
	0x4d9, 0x4d6, 0x4d2, 0x4cf, 0x4cc, 0x4c8, 0x4c5, 0x4c2,
	0x4be, 0x4bb, 0x4b8, 0x4b5, 0x4b1, 0x4ae, 0x4ab, 0x4a8,
	0x4a4, 0x4a1, 0x49e, 0x49b, 0x498, 0x494, 0x491, 0x48e,
	0x48b, 0x488, 0x485, 0x482, 0x47e, 0x47b, 0x478, 0x475,
	0x472, 0x46f, 0x46c, 0x469, 0x466, 0x463, 0x460, 0x45d,
	0x45a, 0x457, 0x454, 0x451, 0x44e, 0x44b, 0x448, 0x445,
	0x442, 0x43f, 0x43c, 0x439, 0x436, 0x433, 0x430, 0x42d,
	0x42a, 0x428, 0x425, 0x422, 0x41f, 0x41c, 0x419, 0x416,
	0x414, 0x411, 0x40e, 0x40b, 0x408, 0x406, 0x403, 0x400
};

// freq mult table multiplied by 2
// 1/2, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 12, 12, 15, 15
static const uint8 MultTable[16] = {
	1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30
};

// Ksl table
static const uint8 KslRom[16] = {
	0, 32, 40, 45, 48, 51, 53, 55, 56, 58, 59, 60, 61, 62, 63, 64
};

static const uint8 KslShift[4] = {
	8, 1, 2, 0
};

// Envelope generator constants
static const uint8 EgIncStep[4][4] = {
	{ 0, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 1, 0, 1, 0 },
	{ 1, 1, 1, 0 }
};

// address decoding
static const int8 AdSlot[0x20] = {
	0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, 9, 10, 11, -1, -1,
	12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

static const int8 ChSlot[18] = {
	0, 1, 2, 6, 7, 8, 12, 13, 14, 18, 19, 20, 24, 25, 26, 30, 31, 32
};

// Envelope generator
typedef int16(*EnvelopeSinFunc)(uint16 Phase, uint16 Envelope);
typedef void(*EnvelopeGenFunc)(Opl3Slot* Slot);

static int16 Opl3EnvelopeCalcExp(uint32 Level)
{
	if (Level > 0x1fff)
	{
		Level = 0x1fff;
	}
	return (ExpRom[Level & 0xff] << 1) >> (Level >> 8);
}

static int16 Opl3EnvelopeCalcSin0(uint16 Phase, uint16 Envelope)
{
	uint16 Out = 0;
	uint16 Neg = 0;
	Phase &= 0x3ff;
	if (Phase & 0x200)
	{
		Neg = 0xffff;
	}
	if (Phase & 0x100)
	{
		Out = LogSinRom[(Phase & 0xff) ^ 0xff];
	}
	else
	{
		Out = LogSinRom[Phase & 0xff];
	}
	return Opl3EnvelopeCalcExp(Out + (Envelope << 3)) ^ Neg;
}

static int16 Opl3EnvelopeCalcSin1(uint16 Phase, uint16 Envelope)
{
	uint16 Out = 0;
	Phase &= 0x3ff;
	if (Phase & 0x200)
	{
		Out = 0x1000;
	}
	else if (Phase & 0x100)
	{
		Out = LogSinRom[(Phase & 0xff) ^ 0xff];
	}
	else
	{
		Out = LogSinRom[Phase & 0xff];
	}
	return Opl3EnvelopeCalcExp(Out + (Envelope << 3));
}

static int16 Opl3EnvelopeCalcSin2(uint16 Phase, uint16 Envelope)
{
	uint16 Out = 0;
	Phase &= 0x3ff;
	if (Phase & 0x100)
	{
		Out = LogSinRom[(Phase & 0xff) ^ 0xff];
	}
	else
	{
		Out = LogSinRom[Phase & 0xff];
	}
	return Opl3EnvelopeCalcExp(Out + (Envelope << 3));
}

static int16 Opl3EnvelopeCalcSin3(uint16 Phase, uint16 Envelope)
{
	uint16 Out = 0;
	Phase &= 0x3ff;
	if (Phase & 0x100)
	{
		Out = 0x1000;
	}
	else
	{
		Out = LogSinRom[Phase & 0xff];
	}
	return Opl3EnvelopeCalcExp(Out + (Envelope << 3));
}

static int16 Opl3EnvelopeCalcSin4(uint16 Phase, uint16 Envelope)
{
	uint16 Out = 0;
	uint16 Neg = 0;
	Phase &= 0x3ff;
	if ((Phase & 0x300) == 0x100)
	{
		Neg = 0xffff;
	}
	if (Phase & 0x200)
	{
		Out = 0x1000;
	}
	else if (Phase & 0x80)
	{
		Out = LogSinRom[((Phase ^ 0xff) << 1) & 0xff];
	}
	else
	{
		Out = LogSinRom[(Phase << 1) & 0xff];
	}
	return Opl3EnvelopeCalcExp(Out + (Envelope << 3)) ^ Neg;
}

static int16 Opl3EnvelopeCalcSin5(uint16 Phase, uint16 Envelope)
{
	uint16 Out = 0;
	Phase &= 0x3ff;
	if (Phase & 0x200)
	{
		Out = 0x1000;
	}
	else if (Phase & 0x80)
	{
		Out = LogSinRom[((Phase ^ 0xff) << 1) & 0xff];
	}
	else
	{
		Out = LogSinRom[(Phase << 1) & 0xff];
	}
	return Opl3EnvelopeCalcExp(Out + (Envelope << 3));
}

static int16 Opl3EnvelopeCalcSin6(uint16 Phase, uint16 Envelope)
{
	uint16 Neg = 0;
	Phase &= 0x3ff;
	if (Phase & 0x200)
	{
		Neg = 0xffff;
	}
	return Opl3EnvelopeCalcExp(Envelope << 3) ^ Neg;
}

static int16 Opl3EnvelopeCalcSin7(uint16 Phase, uint16 Envelope)
{
	uint16 Out = 0;
	uint16 Neg = 0;
	Phase &= 0x3ff;
	if (Phase & 0x200)
	{
		Neg = 0xffff;
		Phase = (Phase & 0x1ff) ^ 0x1ff;
	}
	Out = Phase << 3;
	return Opl3EnvelopeCalcExp(Out + (Envelope << 3)) ^ Neg;
}

static const EnvelopeSinFunc EnvelopeSin[8] = {
	Opl3EnvelopeCalcSin0,
	Opl3EnvelopeCalcSin1,
	Opl3EnvelopeCalcSin2,
	Opl3EnvelopeCalcSin3,
	Opl3EnvelopeCalcSin4,
	Opl3EnvelopeCalcSin5,
	Opl3EnvelopeCalcSin6,
	Opl3EnvelopeCalcSin7
};

enum EnvelopeGenNum : uint8
{
	envelope_gen_num_attack = 0,
	envelope_gen_num_decay = 1,
	envelope_gen_num_sustain = 2,
	envelope_gen_num_release = 3
};

static void Opl3EnvelopeUpdateKsl(Opl3Slot* Slot)
{
	int16 Ksl = (KslRom[Slot->Channel->FNum >> 6] << 2)
		- ((0x08 - Slot->Channel->Block) << 5);
	if (Ksl < 0)
	{
		Ksl = 0;
	}
	Slot->EgKsl = (uint8)Ksl;
}

static void Opl3EnvelopeCalc(Opl3Slot* Slot)
{
	uint8 Reset = 0;
	uint8 RegRate = 0;
	Slot->EgOut = Slot->EgRout + (Slot->RegTl << 2)
		+ (Slot->EgKsl >> KslShift[Slot->RegKsl]) + *Slot->Trem;
	if (Slot->Key && Slot->EgGen == envelope_gen_num_release)
	{
		Reset = 1;
		RegRate = Slot->RegAr;
	}
	else
	{
		switch (Slot->EgGen)
		{
		case envelope_gen_num_attack:
			RegRate = Slot->RegAr;
			break;
		case envelope_gen_num_decay:
			RegRate = Slot->RegDr;
			break;
		case envelope_gen_num_sustain:
			if (!Slot->RegType)
			{
				RegRate = Slot->RegRr;
			}
			break;
		case envelope_gen_num_release:
			RegRate = Slot->RegRr;
			break;
		}
	}
	Slot->PgReset = Reset;
	uint8 Ks = Slot->Channel->Ksv >> ((Slot->RegKsr ^ 1) << 1);
	uint8 Nonzero = (RegRate != 0);
	uint8 Rate = Ks + (RegRate << 2);
	uint8 RateHi = Rate >> 2;
	uint8 RateLo = Rate & 0x03;
	if (RateHi & 0x10)
	{
		RateHi = 0x0f;
	}
	uint8 EgShift = RateHi + Slot->Chip->EgAdd;
	uint8 Shift = 0;
	if (Nonzero)
	{
		if (RateHi < 12)
		{
			if (Slot->Chip->EgState)
			{
				switch (EgShift)
				{
				case 12:
					Shift = 1;
					break;
				case 13:
					Shift = (RateLo >> 1) & 0x01;
					break;
				case 14:
					Shift = RateLo & 0x01;
					break;
				default:
					break;
				}
			}
		}
		else
		{
			Shift = (RateHi & 0x03) + EgIncStep[RateLo][Slot->Chip->Timer & 0x03];
			if (Shift & 0x04)
			{
				Shift = 0x03;
			}
			if (!Shift)
			{
				Shift = Slot->Chip->EgState;
			}
		}
	}
	uint16 EgRout = Slot->EgRout;
	int16 EgInc = 0;
	uint8 EgOff = 0;
	// Instant attack
	if (Reset && RateHi == 0x0f)
	{
		EgRout = 0x00;
	}
	// Envelope off
	if ((Slot->EgRout & 0x1f8) == 0x1f8)
	{
		EgOff = 1;
	}
	if (Slot->EgGen != envelope_gen_num_attack && !Reset && EgOff)
	{
		EgRout = 0x1ff;
	}
	switch (Slot->EgGen)
	{
	case envelope_gen_num_attack:
		if (!Slot->EgRout)
		{
			Slot->EgGen = envelope_gen_num_decay;
		}
		else if (Slot->Key && Shift > 0 && RateHi != 0x0f)
		{
			EgInc = ((~Slot->EgRout) << Shift) >> 4;
		}
		break;
	case envelope_gen_num_decay:
		if ((Slot->EgRout >> 4) == Slot->RegSl)
		{
			Slot->EgGen = envelope_gen_num_sustain;
		}
		else if (!EgOff && !Reset && Shift > 0)
		{
			EgInc = 1 << (Shift - 1);
		}
		break;
	case envelope_gen_num_sustain:
	case envelope_gen_num_release:
		if (!EgOff && !Reset && Shift > 0)
		{
			EgInc = 1 << (Shift - 1);
		}
		break;
	}
	Slot->EgRout = (EgRout + EgInc) & 0x1ff;
	// Key off
	if (Reset)
	{
		Slot->EgGen = envelope_gen_num_attack;
	}
	if (!Slot->Key)
	{
		Slot->EgGen = envelope_gen_num_release;
	}
}

static void Opl3EnvelopeKeyOn(Opl3Slot* Slot, uint8 Type)
{
	Slot->Key |= Type;
}

static void Opl3EnvelopeKeyOff(Opl3Slot* Slot, uint8 Type)
{
	Slot->Key &= ~Type;
}

// Phase Generator
static void Opl3PhaseGenerate(Opl3Slot* Slot)
{
	Opl3Chip* Chip = Slot->Chip;
	uint16 FNum = Slot->Channel->FNum;
	if (Slot->RegVib)
	{
		int8 Range;
		uint8 VibPos;

		Range = (FNum >> 7) & 7;
		VibPos = Slot->Chip->VibPos;

		if (!(VibPos & 3))
		{
			Range = 0;
		}
		else if (VibPos & 1)
		{
			Range >>= 1;
		}
		Range >>= Slot->Chip->VibShift;

		if (VibPos & 4)
		{
			Range = -Range;
		}
		FNum += Range;
	}
	uint32 BaseFreq = (FNum << Slot->Channel->Block) >> 1;
	uint16 Phase = (uint16)(Slot->PgPhase >> 9);
	if (Slot->PgReset)
	{
		Slot->PgPhase = 0;
	}
	Slot->PgPhase += (BaseFreq * MultTable[Slot->RegMult]) >> 1;
	// Rhythm mode
	uint32 Noise = Chip->Noise;
	Slot->PgPhaseOut = Phase;
	if (Slot->SlotNum == 13) // hh
	{
		Chip->RmHhBit2 = (Phase >> 2) & 1;
		Chip->RmHhBit3 = (Phase >> 3) & 1;
		Chip->RmHhBit7 = (Phase >> 7) & 1;
		Chip->RmHhBit8 = (Phase >> 8) & 1;
	}
	if (Slot->SlotNum == 17 && (Chip->Rhy & 0x20)) // tc
	{
		Chip->RmTcBit3 = (Phase >> 3) & 1;
		Chip->RmTcBit5 = (Phase >> 5) & 1;
	}
	if (Chip->Rhy & 0x20)
	{
		uint8 RmXOR = (Chip->RmHhBit2 ^ Chip->RmHhBit7)
			| (Chip->RmHhBit3 ^ Chip->RmTcBit5)
			| (Chip->RmTcBit3 ^ Chip->RmTcBit5);
		switch (Slot->SlotNum)
		{
		case 13: // hh
			Slot->PgPhaseOut = RmXOR << 9;
			if (RmXOR ^ (Noise & 1))
			{
				Slot->PgPhaseOut |= 0xd0;
			}
			else
			{
				Slot->PgPhaseOut |= 0x34;
			}
			break;
		case 16: // sd
			Slot->PgPhaseOut = (Chip->RmHhBit8 << 9)
				| ((Chip->RmHhBit8 ^ (Noise & 1)) << 8);
			break;
		case 17: // tc
			Slot->PgPhaseOut = (RmXOR << 9) | 0x80;
			break;
		default:
			break;
		}
	}
	uint8 BitCount = ((Noise >> 14) ^ Noise) & 0x01;
	Chip->Noise = (Noise >> 1) | (BitCount << 22);
}

// Slot

static void Opl3SlotWrite20(Opl3Slot* Slot, uint8 Data)
{
	if ((Data >> 7) & 0x01)
	{
		Slot->Trem = &Slot->Chip->Tremolo;
	}
	else
	{
		Slot->Trem = (uint8*)&Slot->Chip->ZeroMod;
	}
	Slot->RegVib = (Data >> 6) & 0x01;
	Slot->RegType = (Data >> 5) & 0x01;
	Slot->RegKsr = (Data >> 4) & 0x01;
	Slot->RegMult = Data & 0x0f;
}

static void Opl3SlotWrite40(Opl3Slot* Slot, uint8 Data)
{
	Slot->RegKsl = (Data >> 6) & 0x03;
	Slot->RegTl = Data & 0x3f;
	Opl3EnvelopeUpdateKsl(Slot);
}

static void Opl3SlotWrite60(Opl3Slot* Slot, uint8 Data)
{
	Slot->RegAr = (Data >> 4) & 0x0f;
	Slot->RegDr = Data & 0x0f;
}

static void Opl3SlotWrite80(Opl3Slot* Slot, uint8 Data)
{
	Slot->RegSl = (Data >> 4) & 0x0f;
	if (Slot->RegSl == 0x0f)
	{
		Slot->RegSl = 0x1f;
	}
	Slot->RegAr = Data & 0x0f;
}

static void Opl3SlotWriteE0(Opl3Slot* Slot, uint8 Data)
{
	Slot->RegWf = Data & 0x07;
	if (Slot->Chip->NewM == 0x00)
	{
		Slot->RegWf &= 0x03;
	}
}

static void Opl3SlotGenerate(Opl3Slot* Slot)
{
	Slot->Out = EnvelopeSin[Slot->RegWf](Slot->PgPhaseOut + *Slot->Mod, Slot->EgOut);
}

static void Opl3SlotCalcFB(Opl3Slot* Slot)
{
	if (Slot->Channel->Fb != 0x00)
	{
		Slot->FbMod = (Slot->Prout + Slot->Out) >> (0x09 - Slot->Channel->Fb);
	}
	else
	{
		Slot->FbMod = 0;
	}
	Slot->Prout = Slot->Out;
}

// Channel

static void Opl3ChannelSetupAlg(Opl3Channel* Channel);

static void Opl3ChannelUpdateRhythm(Opl3Chip* Chip, uint8 Data)
{
	Chip->Rhy = Data & 0x3f;
	if (Chip->Rhy & 0x20)
	{
		Opl3Channel* Channel6 = &Chip->Channel[6];
		Opl3Channel* Channel7 = &Chip->Channel[7];
		Opl3Channel* Channel8 = &Chip->Channel[8];
		Channel6->Out[0] = &Channel6->Slots[1]->Out;
		Channel6->Out[1] = &Channel6->Slots[1]->Out;
		Channel6->Out[2] = &Chip->ZeroMod;
		Channel6->Out[3] = &Chip->ZeroMod;
		Channel7->Out[0] = &Channel7->Slots[0]->Out;
		Channel7->Out[1] = &Channel7->Slots[0]->Out;
		Channel7->Out[2] = &Channel7->Slots[1]->Out;
		Channel7->Out[3] = &Channel7->Slots[1]->Out;
		Channel8->Out[0] = &Channel8->Slots[0]->Out;
		Channel8->Out[1] = &Channel8->Slots[0]->Out;
		Channel8->Out[2] = &Channel8->Slots[1]->Out;
		Channel8->Out[3] = &Channel8->Slots[1]->Out;
		for (uint8 ChNum = 6; ChNum < 9; ChNum++)
		{
			Chip->Channel[ChNum].ChType = ch_drum;
		}
		Opl3ChannelSetupAlg(Channel6);
		Opl3ChannelSetupAlg(Channel7);
		Opl3ChannelSetupAlg(Channel8);
		//hh
		if (Chip->Rhy & 0x01)
		{
			Opl3EnvelopeKeyOn(Channel7->Slots[0], egk_drum);
		}
		else
		{
			Opl3EnvelopeKeyOff(Channel7->Slots[0], egk_drum);
		}
		//tc
		if (Chip->Rhy & 0x02)
		{
			Opl3EnvelopeKeyOn(Channel8->Slots[1], egk_drum);
		}
		else
		{
			Opl3EnvelopeKeyOff(Channel8->Slots[1], egk_drum);
		}
		//tom
		if (Chip->Rhy & 0x04)
		{
			Opl3EnvelopeKeyOn(Channel8->Slots[0], egk_drum);
		}
		else
		{
			Opl3EnvelopeKeyOff(Channel8->Slots[0], egk_drum);
		}
		//sd
		if (Chip->Rhy & 0x08)
		{
			Opl3EnvelopeKeyOn(Channel7->Slots[1], egk_drum);
		}
		else
		{
			Opl3EnvelopeKeyOff(Channel7->Slots[1], egk_drum);
		}
		//bd
		if (Chip->Rhy & 0x10)
		{
			Opl3EnvelopeKeyOn(Channel6->Slots[0], egk_drum);
			Opl3EnvelopeKeyOn(Channel6->Slots[1], egk_drum);
		}
		else
		{
			Opl3EnvelopeKeyOff(Channel6->Slots[0], egk_drum);
			Opl3EnvelopeKeyOff(Channel6->Slots[1], egk_drum);
		}
	}
	else
	{
		for (uint8 ChNum = 6; ChNum < 9; ChNum++)
		{
			Chip->Channel[ChNum].ChType = ch_2op;
			Opl3ChannelSetupAlg(&Chip->Channel[ChNum]);
			Opl3EnvelopeKeyOff(Chip->Channel[ChNum].Slots[0], egk_drum);
			Opl3EnvelopeKeyOff(Chip->Channel[ChNum].Slots[1], egk_drum);
		}
	}
}

static void Opl3ChannelWriteA0(Opl3Channel* Channel, uint8 Data)
{
	if (Channel->Chip->NewM && Channel->ChType == ch_4op2)
	{
		return;
	}
	Channel->FNum = (Channel->FNum & 0x300) | Data;
	Channel->Ksv = (Channel->Block << 1)
		| ((Channel->FNum >> (0x09 - Channel->Chip->Nts)) & 0x01);
	Opl3EnvelopeUpdateKsl(Channel->Slots[0]);
	Opl3EnvelopeUpdateKsl(Channel->Slots[1]);
	if (Channel->Chip->NewM && Channel->ChType == ch_4op)
	{
		Channel->Pair->FNum = Channel->FNum;
		Channel->Pair->Ksv = Channel->Ksv;
		Opl3EnvelopeUpdateKsl(Channel->Pair->Slots[0]);
		Opl3EnvelopeUpdateKsl(Channel->Pair->Slots[1]);
	}
}

static void Opl3ChannelWriteB0(Opl3Channel* Channel, uint8 Data)
{
	if (Channel->Chip->NewM && Channel->ChType == ch_4op2)
	{
		return;
	}
	Channel->FNum = (Channel->FNum & 0xff) | ((Data & 0x03) << 8);
	Channel->Block = (Data >> 2) & 0x07;
	Channel->Ksv = (Channel->Block << 1)
		| ((Channel->FNum >> (0x09 - Channel->Chip->Nts)) & 0x01);
	Opl3EnvelopeUpdateKsl(Channel->Slots[0]);
	Opl3EnvelopeUpdateKsl(Channel->Slots[1]);
	if (Channel->Chip->NewM && Channel->ChType == ch_4op)
	{
		Channel->Pair->FNum = Channel->FNum;
		Channel->Pair->Block = Channel->Block;
		Channel->Pair->Ksv = Channel->Ksv;
		Opl3EnvelopeUpdateKsl(Channel->Pair->Slots[0]);
		Opl3EnvelopeUpdateKsl(Channel->Pair->Slots[1]);
	}
}

static void Opl3ChannelSetupAlg(Opl3Channel* Channel)
{
	if (Channel->ChType == ch_drum)
	{
		if (Channel->ChNum == 7 || Channel->ChNum == 8)
		{
			Channel->Slots[0]->Mod = &Channel->Chip->ZeroMod;
			Channel->Slots[1]->Mod = &Channel->Chip->ZeroMod;
			return;
		}
		switch (Channel->Alg & 0x01)
		{
		case 0x00:
			Channel->Slots[0]->Mod = &Channel->Slots[0]->FbMod;
			Channel->Slots[1]->Mod = &Channel->Slots[0]->Out;
			break;
		case 0x01:
			Channel->Slots[0]->Mod = &Channel->Slots[0]->FbMod;
			Channel->Slots[1]->Mod = &Channel->Chip->ZeroMod;
			break;
		}
		return;
	}
	if (Channel->Alg & 0x08)
	{
		return;
	}
	if (Channel->Alg & 0x04)
	{
		Channel->Pair->Out[0] = &Channel->Chip->ZeroMod;
		Channel->Pair->Out[1] = &Channel->Chip->ZeroMod;
		Channel->Pair->Out[2] = &Channel->Chip->ZeroMod;
		Channel->Pair->Out[3] = &Channel->Chip->ZeroMod;
		switch (Channel->Alg & 0x03)
		{
		case 0x00:
			Channel->Pair->Slots[0]->Mod = &Channel->Pair->Slots[0]->FbMod;
			Channel->Pair->Slots[1]->Mod = &Channel->Pair->Slots[0]->Out;
			Channel->Slots[0]->Mod = &Channel->Pair->Slots[1]->Out;
			Channel->Slots[1]->Mod = &Channel->Slots[0]->Out;
			Channel->Out[0] = &Channel->Slots[1]->Out;
			Channel->Out[1] = &Channel->Chip->ZeroMod;
			Channel->Out[2] = &Channel->Chip->ZeroMod;
			Channel->Out[3] = &Channel->Chip->ZeroMod;
			break;
		case 0x01:
			Channel->Pair->Slots[0]->Mod = &Channel->Pair->Slots[0]->FbMod;
			Channel->Pair->Slots[1]->Mod = &Channel->Pair->Slots[0]->Out;
			Channel->Slots[0]->Mod = &Channel->Chip->ZeroMod;
			Channel->Slots[1]->Mod = &Channel->Slots[0]->Out;
			Channel->Out[0] = &Channel->Pair->Slots[1]->Out;
			Channel->Out[1] = &Channel->Slots[1]->Out;
			Channel->Out[2] = &Channel->Chip->ZeroMod;
			Channel->Out[3] = &Channel->Chip->ZeroMod;
			break;
		case 0x02:
			Channel->Pair->Slots[0]->Mod = &Channel->Pair->Slots[0]->FbMod;
			Channel->Pair->Slots[1]->Mod = &Channel->Chip->ZeroMod;
			Channel->Slots[0]->Mod = &Channel->Pair->Slots[1]->Out;
			Channel->Slots[1]->Mod = &Channel->Slots[0]->Out;
			Channel->Out[0] = &Channel->Pair->Slots[0]->Out;
			Channel->Out[1] = &Channel->Slots[1]->Out;
			Channel->Out[2] = &Channel->Chip->ZeroMod;
			Channel->Out[3] = &Channel->Chip->ZeroMod;
			break;
		case 0x03:
			Channel->Pair->Slots[0]->Mod = &Channel->Pair->Slots[0]->FbMod;
			Channel->Pair->Slots[1]->Mod = &Channel->Chip->ZeroMod;
			Channel->Slots[0]->Mod = &Channel->Pair->Slots[1]->Out;
			Channel->Slots[1]->Mod = &Channel->Chip->ZeroMod;
			Channel->Out[0] = &Channel->Pair->Slots[0]->Out;
			Channel->Out[1] = &Channel->Slots[0]->Out;
			Channel->Out[2] = &Channel->Slots[1]->Out;
			Channel->Out[3] = &Channel->Chip->ZeroMod;
			break;
		}
	}
	else
	{
		switch (Channel->Alg & 0x01)
		{
		case 0x00:
			Channel->Slots[0]->Mod = &Channel->Slots[0]->FbMod;
			Channel->Slots[1]->Mod = &Channel->Slots[0]->Out;
			Channel->Out[0] = &Channel->Slots[1]->Out;
			Channel->Out[1] = &Channel->Chip->ZeroMod;
			Channel->Out[2] = &Channel->Chip->ZeroMod;
			Channel->Out[3] = &Channel->Chip->ZeroMod;
			break;
		case 0x01:
			Channel->Slots[0]->Mod = &Channel->Slots[0]->FbMod;
			Channel->Slots[1]->Mod = &Channel->Chip->ZeroMod;
			Channel->Out[0] = &Channel->Slots[0]->Out;
			Channel->Out[1] = &Channel->Slots[1]->Out;
			Channel->Out[2] = &Channel->Chip->ZeroMod;
			Channel->Out[3] = &Channel->Chip->ZeroMod;
			break;
		}
	}
}

static void Opl3ChannelWriteC0(Opl3Channel* Channel, uint8 Data)
{
	Channel->Fb = (Data & 0x0e) >> 1;
	Channel->Con = Data & 0x01;
	Channel->Alg = Channel->Con;
	if (Channel->Chip->NewM)
	{
		if (Channel->ChType == ch_4op)
		{
			Channel->Pair->Alg = 0x04 | (Channel->Con << 1) | (Channel->Pair->Con);
			Channel->Alg = 0x08;
			Opl3ChannelSetupAlg(Channel->Pair);
		}
		else if (Channel->ChType == ch_4op2)
		{
			Channel->Alg = 0x04 | (Channel->Pair->Con << 1) | (Channel->Con);
			Channel->Pair->Alg = 0x08;
			Opl3ChannelSetupAlg(Channel);
		}
		else
		{
			Opl3ChannelSetupAlg(Channel);
		}
	}
	else
	{
		Opl3ChannelSetupAlg(Channel);
	}
	if (Channel->Chip->NewM)
	{
		Channel->Cha = ((Data >> 4) & 0x01) ? ~0 : 0;
		Channel->Chb = ((Data >> 5) & 0x01) ? ~0 : 0;
	}
	else
	{
		Channel->Cha = Channel->Chb = (uint16)~0;
	}
}

static void Opl3ChannelKeyOn(Opl3Channel* Channel)
{
	if (Channel->Chip->NewM)
	{
		if (Channel->ChType == ch_4op)
		{
			Opl3EnvelopeKeyOn(Channel->Slots[0], egk_norm);
			Opl3EnvelopeKeyOn(Channel->Slots[1], egk_norm);
			Opl3EnvelopeKeyOn(Channel->Pair->Slots[0], egk_norm);
			Opl3EnvelopeKeyOn(Channel->Pair->Slots[1], egk_norm);
		}
		else if (Channel->ChType == ch_2op || Channel->ChType == ch_drum)
		{
			Opl3EnvelopeKeyOn(Channel->Slots[0], egk_norm);
			Opl3EnvelopeKeyOn(Channel->Slots[1], egk_norm);
		}
	}
	else
	{
		Opl3EnvelopeKeyOn(Channel->Slots[0], egk_norm);
		Opl3EnvelopeKeyOn(Channel->Slots[1], egk_norm);
	}
}

static void Opl3ChannelKeyOff(Opl3Channel* Channel)
{
	if (Channel->Chip->NewM)
	{
		if (Channel->ChType == ch_4op)
		{
			Opl3EnvelopeKeyOff(Channel->Slots[0], egk_norm);
			Opl3EnvelopeKeyOff(Channel->Slots[1], egk_norm);
			Opl3EnvelopeKeyOff(Channel->Pair->Slots[0], egk_norm);
			Opl3EnvelopeKeyOff(Channel->Pair->Slots[1], egk_norm);
		}
		else if (Channel->ChType == ch_2op || Channel->ChType == ch_drum)
		{
			Opl3EnvelopeKeyOff(Channel->Slots[0], egk_norm);
			Opl3EnvelopeKeyOff(Channel->Slots[1], egk_norm);
		}
	}
	else
	{
		Opl3EnvelopeKeyOff(Channel->Slots[0], egk_norm);
		Opl3EnvelopeKeyOff(Channel->Slots[1], egk_norm);
	}
}

static void Opl3ChannelSet4Op(Opl3Chip* Chip, uint8 Data)
{
	for (uint8 Bit = 0; Bit < 6; Bit++)
	{
		uint8 ChNum = Bit;
		if (Bit >= 3)
		{
			ChNum += 9 - 3;
		}
		if ((Data >> Bit) & 0x01)
		{
			Chip->Channel[ChNum].ChType = ch_4op;
			Chip->Channel[ChNum + 3].ChType = ch_4op2;
		}
		else
		{
			Chip->Channel[ChNum].ChType = ch_2op;
			Chip->Channel[ChNum + 3].ChType = ch_2op;
		}
	}
}

static int16 Opl3ClipSample(int32 Sample)
{
	if (Sample > 32767)
	{
		Sample = 32767;
	}
	else if (Sample < -32768)
	{
		Sample = -32768;
	}
	return (int16)Sample;
}

void Opl3WriteReg(Opl3Chip* Chip, uint16 Reg, uint8 v)
{
	uint8 High = (Reg >> 8) & 0x01;
	uint8 RegM = Reg & 0xff;
	switch (RegM & 0xf0)
	{
	case 0x00:
		if (High)
		{
			switch (RegM & 0x0f)
			{
			case 0x04:
				Opl3ChannelSet4Op(Chip, v);
				break;
			case 0x05:
				Chip->NewM = v & 0x01;
				break;
			}
		}
		else
		{
			switch (RegM & 0x0f)
			{
			case 0x08:
				Chip->Nts = (v >> 6) & 0x01;
				break;
			}
		}
		break;
	case 0x20:
	case 0x30:
		if (AdSlot[RegM & 0x1f] >= 0)
		{
			Opl3SlotWrite20(&Chip->Slot[18 * High + AdSlot[RegM & 0x1f]], v);
		}
		break;
	case 0x40:
	case 0x50:
		if (AdSlot[RegM & 0x1f] >= 0)
		{
			Opl3SlotWrite40(&Chip->Slot[18 * High + AdSlot[RegM & 0x1f]], v);
		}
		break;
	case 0x60:
	case 0x70:
		if (AdSlot[RegM & 0x1f] >= 0)
		{
			Opl3SlotWrite60(&Chip->Slot[18 * High + AdSlot[RegM & 0x1f]], v);
		}
		break;
	case 0x80:
	case 0x90:
		if (AdSlot[RegM & 0x1f] >= 0)
		{
			Opl3SlotWrite80(&Chip->Slot[18 * High + AdSlot[RegM & 0x1f]], v);
		}
		break;
	case 0xe0:
	case 0xf0:
		if (AdSlot[RegM & 0x1f] >= 0)
		{
			Opl3SlotWriteE0(&Chip->Slot[18 * High + AdSlot[RegM & 0x1f]], v);
		}
		break;
	case 0xa0:
		if ((RegM & 0x0f) < 9)
		{
			Opl3ChannelWriteA0(&Chip->Channel[9 * High + (RegM & 0x0f)], v);
		}
		break;
	case 0xb0:
		if (RegM == 0xbd && !High)
		{
			Chip->TremoloShift = (((v >> 7) ^ 1) << 1) + 2;
			Chip->VibShift = ((v >> 6) & 0x01) ^ 1;
			Opl3ChannelUpdateRhythm(Chip, v);
		}
		else if ((RegM & 0x0f) < 9)
		{
			Opl3ChannelWriteB0(&Chip->Channel[9 * High + (RegM & 0x0f)], v);
			if (v & 0x20)
			{
				Opl3ChannelKeyOn(&Chip->Channel[9 * High + (RegM & 0x0f)]);
			}
			else
			{
				Opl3ChannelKeyOff(&Chip->Channel[9 * High + (RegM & 0x0f)]);
			}
		}
		break;
	case 0xc0:
		if ((RegM & 0x0f) < 9)
		{
			Opl3ChannelWriteC0(&Chip->Channel[9 * High + (RegM & 0x0f)], v);
		}
		break;
	}
}

void Opl3Generate(Opl3Chip* Chip, int16* Buf)
{
	uint8 ii;
	uint8 jj;
	int16 accm;
	uint8 Shift = 0;

	Buf[1] = Opl3ClipSample(Chip->MixBuff[1]);

	for (ii = 0; ii < 15; ii++)
	{
		Opl3SlotCalcFB(&Chip->Slot[ii]);
		Opl3EnvelopeCalc(&Chip->Slot[ii]);
		Opl3PhaseGenerate(&Chip->Slot[ii]);
		Opl3SlotGenerate(&Chip->Slot[ii]);
	}

	Chip->MixBuff[0] = 0;
	for (ii = 0; ii < 18; ii++)
	{
		accm = 0;
		for (jj = 0; jj < 4; jj++)
		{
			accm += *Chip->Channel[ii].Out[jj];
		}
		Chip->MixBuff[0] += (int16)(accm & Chip->Channel[ii].Cha);
	}

	for (ii = 15; ii < 18; ii++)
	{
		Opl3SlotCalcFB(&Chip->Slot[ii]);
		Opl3EnvelopeCalc(&Chip->Slot[ii]);
		Opl3PhaseGenerate(&Chip->Slot[ii]);
		Opl3SlotGenerate(&Chip->Slot[ii]);
	}

	Buf[0] = Opl3ClipSample(Chip->MixBuff[0]);

	for (ii = 18; ii < 33; ii++)
	{
		Opl3SlotCalcFB(&Chip->Slot[ii]);
		Opl3EnvelopeCalc(&Chip->Slot[ii]);
		Opl3PhaseGenerate(&Chip->Slot[ii]);
		Opl3SlotGenerate(&Chip->Slot[ii]);
	}

	Chip->MixBuff[1] = 0;
	for (ii = 0; ii < 18; ii++)
	{
		accm = 0;
		for (jj = 0; jj < 4; jj++)
		{
			accm += *Chip->Channel[ii].Out[jj];
		}
		Chip->MixBuff[1] += (int16)(accm & Chip->Channel[ii].Chb);
	}

	for (ii = 33; ii < 36; ii++)
	{
		Opl3SlotCalcFB(&Chip->Slot[ii]);
		Opl3EnvelopeCalc(&Chip->Slot[ii]);
		Opl3PhaseGenerate(&Chip->Slot[ii]);
		Opl3SlotGenerate(&Chip->Slot[ii]);
	}

	if ((Chip->Timer & 0x3f) == 0x3f)
	{
		Chip->TremoloPos = (Chip->TremoloPos + 1) % 210;
	}
	if (Chip->TremoloPos < 105)
	{
		Chip->Tremolo = Chip->TremoloPos >> Chip->TremoloShift;
	}
	else
	{
		Chip->Tremolo = (210 - Chip->TremoloPos) >> Chip->TremoloShift;
	}

	if ((Chip->Timer & 0x3ff) == 0x3ff)
	{
		Chip->VibPos = (Chip->VibPos + 1) & 7;
	}

	Chip->Timer++;

	Chip->EgAdd = 0;
	if (Chip->EgTimer)
	{
		while (Shift < 36 && ((Chip->EgTimer >> Shift) & 1) == 0)
		{
			Shift++;
		}
		if (Shift > 12)
		{
			Chip->EgAdd = 0;
		}
		else
		{
			Chip->EgAdd = Shift + 1;
		}
	}

	if (Chip->EgTimerRem || Chip->EgState)
	{
		if (Chip->EgTimer == 0xfffffffff)
		{
			Chip->EgTimer = 0;
			Chip->EgTimerRem = 1;
		}
		else
		{
			Chip->EgTimer++;
			Chip->EgTimerRem = 0;
		}
	}

	Chip->EgState ^= 1;

	while (Chip->WriteBuf[Chip->WriteBufCur].Time <= Chip->WriteBufSampleCnt)
	{
		if (!(Chip->WriteBuf[Chip->WriteBufCur].Reg & 0x200))
		{
			break;
		}
		Chip->WriteBuf[Chip->WriteBufCur].Reg &= 0x1ff;
		Opl3WriteReg(Chip, Chip->WriteBuf[Chip->WriteBufCur].Reg,
			Chip->WriteBuf[Chip->WriteBufCur].Data);
		Chip->WriteBufCur = (Chip->WriteBufCur + 1) % OPL_WRITEBUF_SIZE;
	}
	Chip->WriteBufSampleCnt++;
}

void Opl3GenerateResampled(Opl3Chip* Chip, int16* Buf)
{
	while (Chip->SampleCnt >= Chip->RateRatio)
	{
		Chip->OldSamples[0] = Chip->Samples[0];
		Chip->OldSamples[1] = Chip->Samples[1];
		Opl3Generate(Chip, Chip->Samples);
		Chip->SampleCnt -= Chip->RateRatio;
	}
	Buf[0] = (int16)((Chip->OldSamples[0] * (Chip->RateRatio - Chip->SampleCnt)
		+ Chip->Samples[0] * Chip->SampleCnt) / Chip->RateRatio);
	Buf[1] = (int16)((Chip->OldSamples[1] * (Chip->RateRatio - Chip->SampleCnt)
		+ Chip->Samples[1] * Chip->SampleCnt) / Chip->RateRatio);
	Chip->SampleCnt += 1 << RSM_FRAC;
}

void Opl3Reset(Opl3Chip* Chip, uint32 SampleRate)
{
	FMemory::Memzero(Chip, sizeof(Opl3Chip));
	for (uint8 SlotNum = 0; SlotNum < 36; SlotNum++)
	{
		Chip->Slot[SlotNum].Chip = Chip;
		Chip->Slot[SlotNum].Mod = &Chip->ZeroMod;
		Chip->Slot[SlotNum].EgRout = 0x1ff;
		Chip->Slot[SlotNum].EgOut = 0x1ff;
		Chip->Slot[SlotNum].EgGen = envelope_gen_num_release;
		Chip->Slot[SlotNum].Trem = (uint8*)&Chip->ZeroMod;
		Chip->Slot[SlotNum].SlotNum = SlotNum;
	}
	for (uint8 ChanNum = 0; ChanNum < 18; ChanNum++)
	{
		Chip->Channel[ChanNum].Slots[0] = &Chip->Slot[ChSlot[ChanNum]];
		Chip->Channel[ChanNum].Slots[1] = &Chip->Slot[ChSlot[ChanNum] + 3];
		Chip->Slot[ChSlot[ChanNum]].Channel = &Chip->Channel[ChanNum];
		Chip->Slot[ChSlot[ChanNum] + 3].Channel = &Chip->Channel[ChanNum];
		if ((ChanNum % 9) < 3)
		{
			Chip->Channel[ChanNum].Pair = &Chip->Channel[ChanNum + 3];
		}
		else if ((ChanNum % 9) < 6)
		{
			Chip->Channel[ChanNum].Pair = &Chip->Channel[ChanNum - 3];
		}
		Chip->Channel[ChanNum].Chip = Chip;
		Chip->Channel[ChanNum].Out[0] = &Chip->ZeroMod;
		Chip->Channel[ChanNum].Out[1] = &Chip->ZeroMod;
		Chip->Channel[ChanNum].Out[2] = &Chip->ZeroMod;
		Chip->Channel[ChanNum].Out[3] = &Chip->ZeroMod;
		Chip->Channel[ChanNum].ChType = ch_2op;
		Chip->Channel[ChanNum].Cha = 0xffff;
		Chip->Channel[ChanNum].Chb = 0xffff;
		Chip->Channel[ChanNum].ChNum = ChanNum;
		Opl3ChannelSetupAlg(&Chip->Channel[ChanNum]);
	}
	Chip->Noise = 1;
	Chip->RateRatio = (SampleRate << RSM_FRAC) / 49716;
	Chip->TremoloShift = 4;
	Chip->VibShift = 1;
}

void Opl3WriteRegBuffered(Opl3Chip* Chip, uint16 Reg, uint8 v)
{
	if (Chip->WriteBuf[Chip->WriteBufLast].Reg & 0x200)
	{
		Opl3WriteReg(Chip, Chip->WriteBuf[Chip->WriteBufLast].Reg & 0x1ff,
			Chip->WriteBuf[Chip->WriteBufLast].Data);

		Chip->WriteBufCur = (Chip->WriteBufLast + 1) % OPL_WRITEBUF_SIZE;
		Chip->WriteBufSampleCnt = Chip->WriteBuf[Chip->WriteBufLast].Time;
	}

	Chip->WriteBuf[Chip->WriteBufLast].Reg = Reg | 0x200;
	Chip->WriteBuf[Chip->WriteBufLast].Data = v;
	uint64 Time1 = Chip->WriteBufLastTime + OPL_WRITEBUF_DELAY;
	uint64 Time2 = Chip->WriteBufSampleCnt;

	if (Time1 < Time2)
	{
		Time1 = Time2;
	}

	Chip->WriteBuf[Chip->WriteBufLast].Time = Time1;
	Chip->WriteBufLastTime = Time1;
	Chip->WriteBufLast = (Chip->WriteBufLast + 1) % OPL_WRITEBUF_SIZE;
}

void Opl3GenerateStream(Opl3Chip* Chip, int16* SndPtr, uint32 SampleCount)
{
	for (uint32 i = 0; i < SampleCount; i++)
	{
		Opl3GenerateResampled(Chip, SndPtr);
		SndPtr += 2;
	}
}

NukedOpl3::NukedOpl3(uint32 SampleRate) : OplCore(SampleRate) {}

void NukedOpl3::Reset()
{
	Opl3Reset(&Chip, Rate);
}
void NukedOpl3::Write(uint32 Reg, uint8 Val) {
	if (Reg == OPL3_4OP_REGISTER || Reg == OPL3_MODE_REGISTER)
	{
		Opl3WriteReg(&Chip, (uint16)Reg, Val);
	}
	else
	{
		Opl3WriteRegBuffered(&Chip, (uint16)Reg, Val);
	}
}
void NukedOpl3::Generate(int16* Buf, int32 Samples)
{
	Opl3GenerateStream(&Chip, Buf, Samples);
}

OplCore* NukedOpl3::Duplicate()
{
	return new NukedOpl3(Rate);
}

Opl* EmuOpl::CreateEmuopl(EOplCoreType Core, EOplChipType Type, int32 Rate)
{
	switch (Core)
	{
	case EOplCoreType::OPLCORE_MAME:  return nullptr; //new EmuOpl(Type == Opl::TYPE_OPL3 ? (OplCore*)(new MAMEOPL3(Rate)) : (OplCore*)(new MAMEOPL2(rate)), Type);
	case EOplCoreType::OPLCORE_DBFLT: return nullptr; //new EmuOpl(Type == Opl::TYPE_OPL3 ? (OplCore*)(new DBFLTOPL3(Rate)) : (OplCore*)(new DBFLTOPL2(rate)), Type);
	case EOplCoreType::OPLCORE_DBINT: return nullptr; //new EmuOpl(Type == Opl::TYPE_OPL3 ? (OplCore*)(new DBINTOPL3(Rate)) : (OplCore*)(new DBINTOPL2(rate)), Type);
	case EOplCoreType::OPLCORE_NUKED: return new EmuOpl(new NukedOpl3(Rate), OPLCHIP_OPL3);
	default: return nullptr;
	}
}

EmuOpl::~EmuOpl()
{
	if (CurrType == OPLCHIP_DUAL_OPL2)
	{
		delete OplCores[1];
	}
	delete OplCores[0];
}

// Assumes a 16-bit, mono output sample buffer @ OPL2 mode
// Assumes a 16-bit, stereo output sample buffer @ OPL3/DUAL_OPL2 mode
void EmuOpl::Update(int16* Buf, int32 Samples)
{
	if (CurrType == OPLCHIP_DUAL_OPL2)
	{
		int16* LBuf = (int16*)FMemory::Malloc(sizeof(int16) * Samples);
		OplCores[0]->Generate(LBuf, Samples);
		OplCores[1]->Generate(Buf + Samples, Samples);
		for (int32 i = 0, j = 0; i < Samples; i++) {
			Buf[j++] = LBuf[i];
			Buf[j++] = Buf[i + Samples];
		}
		FMemory::Free(LBuf);
	}
	else {
		OplCores[0]->Generate(Buf, Samples);
	}
}

void EmuOpl::Write(int32 Reg, int32 Val)
{
	if (Reg == 0x105 && CurrType == OPLCHIP_OPL3)
	{
		bOpl3Mode = ((Val & 0x1) == 0x1);
	}
	else
	{
		Reg &= bOpl3Mode ? 0x1FF : 0xFF;
	}
	OplCores[CurrChip]->Write(Reg, (uint8)Val);
}

void EmuOpl::Init()
{
	OplCores[0]->Reset();
	if (CurrType == OPLCHIP_DUAL_OPL2)
	{
		OplCores[1]->Reset();
	}
	if (bOpl3Mode)
	{
		OplCores[0]->Write(0x105, 1);
	}
}

EmuOpl::EmuOpl(OplCore * Core, EOplChipType Type) : bOpl3Mode(false)
{
	OplCores[0] = Core;
	OplCores[1] = (Type == OPLCHIP_DUAL_OPL2) ? Core->Duplicate() : nullptr;
	Init();
}
