//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

sbc_tables.h

Abstract:

SBC types head file.

Author:

Witten Wen 29-December-2009

Revision History:

*************************************************************************/


#define SHIFT_15(a) voASR(a, 15)//15
#define SHIFT_16(a) voASR(a, 16)//16

static const int voSBR_4_Proto[20] = {
	SHIFT_15(0x02cb3e8c), SHIFT_15(0x22b63dc0), 
	SHIFT_15(0x002329cc), SHIFT_15(0x053b7548),
	SHIFT_15(0x31eab940), SHIFT_15(0xec1f5e60), 
	SHIFT_15(0xff3773a8), SHIFT_15(0x0061c5a7),
	SHIFT_15(0x07646680), SHIFT_15(0x3f239480), 
	SHIFT_15(0xf89f23a8), SHIFT_15(0x007a4737),
	SHIFT_15(0x00b32807), SHIFT_15(0x083ddc80), 
	SHIFT_15(0x4825e480), SHIFT_15(0x0191e578),
	SHIFT_15(0x00ff11ca), SHIFT_15(0x00fb7991), 
	SHIFT_15(0x069fdc58), SHIFT_15(0x4b584000)
};

static const int voSBR_8_Proto[40] = {
	SHIFT_15(0x02e5cd20), SHIFT_15(0x22d0c200), 
	SHIFT_15(0x006bfe27), SHIFT_15(0x07808930),
	SHIFT_15(0x3f1c8800), SHIFT_15(0xf8810d70), 
	SHIFT_15(0x002cfdc6), SHIFT_15(0x055acf28),
	SHIFT_15(0x31f566c0), SHIFT_15(0xebfe57e0), 
	SHIFT_15(0xff27c437), SHIFT_15(0x001485cc),
	SHIFT_15(0x041c6e58), SHIFT_15(0x2a7cfa80), 
	SHIFT_15(0xe4c4a240), SHIFT_15(0xfe359e4c),
	SHIFT_15(0x0048b1f8), SHIFT_15(0x0686ce30), 
	SHIFT_15(0x38eec5c0), SHIFT_15(0xf2a1b9f0),
	SHIFT_15(0xffe8904a), SHIFT_15(0x0095698a), 
	SHIFT_15(0x0824a480), SHIFT_15(0x443b3c00),
	SHIFT_15(0xfd7badc8), SHIFT_15(0x00d3e2d9), 
	SHIFT_15(0x00c183d2), SHIFT_15(0x084e1950),
	SHIFT_15(0x4810d800), SHIFT_15(0x017f43fe), 
	SHIFT_15(0x01056dd8), SHIFT_15(0x00e9cb9f),
	SHIFT_15(0x07d7d090), SHIFT_15(0x4a708980), 
	SHIFT_15(0x0488fae8), SHIFT_15(0x0113bd20),
	SHIFT_15(0x0107b1a8), SHIFT_15(0x069fb3c0), 
	SHIFT_15(0x4b3db200), SHIFT_15(0x00763f48)
};

static const int voSBR_M4_ANA[4] = {
	SHIFT_16(0x2d413cc0), SHIFT_16(0x3b20d780), 
	SHIFT_16(0x40000000), SHIFT_16(0x187de2a0)
};

static const int voSBR_M8_ANA[8] = {
	SHIFT_16(0x3b20d780), SHIFT_16(0x187de2a0), 
	SHIFT_16(0x3ec52f80), SHIFT_16(0x3536cc40),
	SHIFT_16(0x238e7680), SHIFT_16(0x0c7c5c20), 
	SHIFT_16(0x2d413cc0), SHIFT_16(0x40000000)
};
