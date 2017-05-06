//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    HuffDec.h

Abstract:

    Huffman decoder. Simplified from Sanjeevm's huffman.hpp

Author:

    Wei-ge Chen (wchen) 19-July-1999

Revision History:


*************************************************************************/

#ifndef _HUFFDEC_H
#define _HUFFDEC_H

#include "macros.h"
#include "strmdec_wma.h"

#define BITSTREAM_READ 1

#ifdef __cplusplus
extern "C"  // only need to export C interface if
            // used by C++ source code
#endif
typedef struct huffResult
{
	U32 state;
	U16 run;
	U16 level;	
} huffResult;

typedef struct huffVecResult
{
	U16 state;
	I16 iResult;
} huffVecResult;

WMARESULT huffDecGet(const U16 *pDecodeTable, CWMAInputBitStream *bitstream, U32* puBitCnt, U32 *puResult, U32* puSign);
WMARESULT huffDecGet_i(const U16 *pDecodeTable, CWMAInputBitStream *bs, U32 *puResult, U32* puSign);
WMARESULT huffDecGetMix(const U32 *pDecodeTable, CWMAInputBitStream *bs, huffResult *HResult, U32* puSign);
WMARESULT huffDecGetMIXVec(const U32 *pDecodeTable, CWMAInputBitStream *bs, huffVecResult *HVResult);
#ifdef __cplusplus
}
#endif

#endif //_HUFFDEC_H

