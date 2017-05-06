//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) Microsoft Corporation 1996 - 1999

Module Name:

    typedef.h

Abstract:

    Basic types:
        CVector2D, CPixel, CMotionVector, 

Author:

    Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
    Wei-ge Chen (wchen@microsoft.com) 20-March-1996
    Bruce Lin   (blin@microsoft.com) 14-April-1996
    Chuang Gu (chuanggu@microsoft.com) 5-June-1996

Revision History:


*************************************************************************/


#ifndef __TYPEDEF_HPP_
#define __TYPEDEF_HPP_

#include "voWmvPort.h"
#include "wmvdec_api.h"

#include "macros_wmv.h"
/////////////////////////////////////////////
//
//  Typedefs for basic types
//
/////////////////////////////////////////////
#define Class       class

#define __int64 long long

#ifdef macintosh
typedef unsigned long DWORD;
#endif


//#if defined(LINUX_ANDROID) && !defined(RVDS)
//typedef long long __int64;
//#endif


typedef unsigned long long U64; 
typedef U64 U64_WMV;

typedef I32_WMV CoordI;
typedef I32_WMV Wide;
typedef U32_WMV UWide;
typedef I32_WMV PixelI32;

typedef long long Time;


// WMVA Start-code suffixs
#define SC_SEQ          0x0F
#define SC_ENTRY        0x0E
#define SC_FRAME        0x0D
#define SC_FIELD        0x0C
#define SC_SLICE        0x0B
#define SC_ENDOFSEQ     0x0A
#define SC_SEQ_DATA     0x1F
#define SC_ENTRY_DATA   0x1E
#define SC_FRAME_DATA   0x1D
#define SC_FIELD_DATA   0x1C
#define SC_SLICE_DATA   0x1B

typedef enum WMVFrameType {PROGRESSIVE = 0, INTERLACEFIELD, INTERLACEFRAME} WMVFrameType;
typedef enum {MP4S, MPG4, MP42, MP43, WMV1, WMV2, WMV3, WMVA} CodecVersion;

typedef enum {DIRECT = 1, INTERPOLATE, BACKWARD, FORWARD, INTRAMBT, FORWARD_BACKWARD, BACKWARD_FORWARD} MBType; // define MB type

enum {XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_4x8, XFORMMODE_MBSWITCH/* pseudo-mode */, XFORMMODE_4x4};


typedef enum BlockNum {
    ALL_Y_BLOCKS    = 0,
    Y_BLOCK1        = 1,
    Y_BLOCK2        = 2,
    Y_BLOCK3        = 3,
    Y_BLOCK4        = 4,
    U_BLOCK         = 5,
    V_BLOCK         = 6,
    A_BLOCK1        = 7,
    A_BLOCK2        = 8,
    A_BLOCK3        = 9,
    A_BLOCK4        = 10,
    ALL_A_BLOCKS    = 11
} BlockNum;


/////////////////////////////////////////////
// Control flag for varius compiler
//
/////////////////////////////////////////////

//some platforms might not support inline assemb
#undef _SCRUNCH_INLINEASM


/////////////////////////////////////////////
//
//  Motion Vectors
//
/////////////////////////////////////////////

/*
union Buffer {
    I16_WMV i16[64];
    I32_WMV i32[64];
} ;
*/
typedef union _Buffer 
{
    I16_WMV i16[64];
    I32_WMV i32[64];
}UnionBuffer;


typedef struct tagCVector
{
    I8_WMV x;
    I8_WMV y;
} CVector;

typedef struct tagCWMVMBMode // MacroBlock Mode
{
    // Some extra data member
    U32_WMV m_bSkip : 1;    // is the Macroblock skiped. = COD in H.263
    U32_WMV m_bCBPAllZero : 1;  // are the CBP for 6 blocks all zero?
    U32_WMV m_bZeroMV : 1; //whether the MB has four motion vectors (for forward vectors)
    U32_WMV m_bBlkXformSwitchOn : 1;   // Boolean which indicates whether the MB or block-based transform switching is chosen
    U32_WMV m_bFirstBlockDecoded : 1;  // Boolean which indicates whether the first coded block in a MB has been decoded
    U32_WMV m_iMBXformMode : 3;         // Transform type for the macroblock
    U32_WMV m_iDCTTable_MB_Index : 2;
    U32_WMV m_iSubblockPattern : 2;        // Subblock pattern for 8x4 or 4x8 subblock
    U32_WMV m_iMVPredDirection : 2; // 2: use original pred direction; 0: from left MB; 1: from up MB.
    U32_WMV m_dctMd : 1; // is the Macroblock inter- or intra- coded    

    I8_WMV m_rgcBlockXformMode [NUM_BLOCKS];       // Transform type for each block: 8x8, 8x4 or 4x8
    I8_WMV m_rgcIntraFlag [NUM_BLOCKS];
    U8_WMV m_rgbCodedBlockPattern2 [NUM_BLOCKS];
    U8_WMV m_rgbDCTCoefPredPattern2[1];// [NUM_BLOCKS +1];
#ifdef _MULTIREF_
    I32_WMV iRefFrame;
#endif
    I8_WMV  m_chMBMode;
    MBType m_mbType; // sils this is not a bitfield but could be: FORWARD ;
    I32_WMV  m_iQP; // for DQUANT    
    Bool_WMV m_bIntra;
    I32_WMV m_bBoundary; // for DQUANT
    U8_WMV m_chFieldDctMode;
    Bool_WMV m_bOverlapIMB;  
} CWMVMBMode;

typedef struct CWMVMBMODEBACK {
    I8_WMV   pBackUpFormType[6];
    I32_WMV   pBackUpLoopMask[6];
    I32_WMV   pBackUpLoopTop[6];

    UnionBuffer  pBackUpErrorInter[6][4];
#if defined(VOARMV7)||defined(NEW_C)
	I32_WMV  pBackUpDCTRunFlags[6][4];
#else
    I32_WMV  pBackUpDCTHorzFlagsInter[6][4];	
    U8_WMV pBackUpIDCTClearOffset[4][4];
#endif
}CWMVMBModeBack;

typedef struct tagPackHuffmanCode_WMV
{
    U32_WMV code   : 26;
    U32_WMV length : 6;
} tPackHuffmanCode_WMV;
#endif // __BASIC_HPP_