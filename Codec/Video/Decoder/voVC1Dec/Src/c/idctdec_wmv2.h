//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) 1996 -- 1998  Microsoft Corporation

Module Name:

	idctdec_wmv2.h

Abstract:

	Inverse DCT routines used by WMV2 decoder

Author:

	Marc Holder (marchold@microsoft.com)    05/98
    Bruce Lin (blin@microsoft.com)          06/98
    Ming-Chieh Lee (mingcl@microsoft.com)   06/98

Revision History:

*************************************************************************/

#ifndef __IDCTDEC_WMV2_H_
#define __IDCTDEC_WMV2_H_

#include "wmvdec_api.h"
#define IDCT_INT   I32_WMV
#define UDCT_INT   U32_WMV

Void_WMV ARMV7_g_IDCTDec_WMV3 (U8_WMV* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV* rgiCoefRecon);

Void_WMV ARMV6_g_IDCTDec_WMV3 (U8_WMV* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV* rgiCoefRecon);
Void_WMV g_IDCTDec_WMV3_Pass1_ARMV4(const IDCT_INT  *piSrc0, IDCT_INT * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags);
Void_WMV g_IDCTDec_WMV3_Pass2_ARMV4(const IDCT_INT  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);
Void_WMV g_IDCTDec_WMV3_Pass3_ARMV4(const IDCT_INT  *piSrc0, IDCT_INT * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags);
Void_WMV g_IDCTDec_WMV3_Pass4_ARMV4(const IDCT_INT  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);

Void_WMV g_IDCTDec_WMV3 (U8_WMV* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV* rgiCoefRecon);

Void_WMV g_IDCTDec16_WMV3 (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
Void_WMV g_8x4IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf);
Void_WMV g_4x8IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf);
Void_WMV g_4x4IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf);

Void_WMV g_IDCTDec_WMV3_Pass1(const IDCT_INT  *piSrc0, IDCT_INT * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags);
Void_WMV g_IDCTDec_WMV3_Pass2(const IDCT_INT  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);
Void_WMV g_IDCTDec_WMV3_Pass3(const IDCT_INT  *piSrc0, IDCT_INT * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags);
Void_WMV g_IDCTDec_WMV3_Pass4(const IDCT_INT  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);

Void_WMV g_InitIDCTTablesDec (); 

#endif // __IDCTDEC_H_
