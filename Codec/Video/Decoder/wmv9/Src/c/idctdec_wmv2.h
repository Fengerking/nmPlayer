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

///////////////////////////////////////////////////////////////////////////
//
// Intra IDCT Functions
//
///////////////////////////////////////////////////////////////////////////

extern Void_WMV g_IDCTDec_WMV2_Intra (U8_WMV* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV* rgiCoefRecon);


#ifdef _WMV_TARGET_X86_
    Void_WMV g_IDCTDecMMX_WMV2(U8_WMV __huge* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon);
#endif //_WMV_TARGET_X86_

#ifdef	SH3_DSP_ASM_IDCT
    Void_WMV g_IDCTDec_WMV2_init ();
#endif

///////////////////////////////////////////////////////////////////////////
//
// Inter IDCT Functions
//
///////////////////////////////////////////////////////////////////////////


extern  Void_WMV g_IDCTDec16_WMV2 (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV  iDCTHorzFlags);

#if defined(VOARMV7)
Void_WMV ARMV7_g_IDCTDec_WMV3 (U8_WMV* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV* rgiCoefRecon);
#elif defined(VOARMV6)
Void_WMV ARMV6_g_IDCTDec_WMV3 (U8_WMV* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV* rgiCoefRecon);
#else
Void_WMV g_IDCTDec_WMV3 (U8_WMV* piDst, I32_WMV  iOffsetToNextRowForDCT, const I32_WMV* rgiCoefRecon);
#endif

Void_WMV g_IDCTDec16_WMV3 (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
Void_WMV g_8x4IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf);
Void_WMV g_4x8IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf);
Void_WMV g_4x4IDCTDec_WMV3 (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf);


#ifdef _EMB_IDCT_SSIMD64_

#define IDCT_INT   I64_WMV
#define UDCT_INT   U64_WMV

#elif defined(_EMB_IDCT_SSIMD32_)

#define IDCT_INT   I32_WMV
#define UDCT_INT   U32_WMV

#else

#define IDCT_INT   I32_WMV
#define UDCT_INT   U32_WMV

#endif

Void_WMV g_IDCTDec_WMV3_Pass1(const IDCT_INT  *piSrc0, IDCT_INT * blk32, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags);
Void_WMV g_IDCTDec_WMV3_Pass2(const IDCT_INT  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);
Void_WMV g_IDCTDec_WMV3_Pass3(const IDCT_INT  *piSrc0, IDCT_INT * blk32, const I32_WMV iNumLoops,  I32_WMV iDCTHorzFlags);
Void_WMV g_IDCTDec_WMV3_Pass4(const IDCT_INT  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);

Void_WMV g_IDCTDec_WMV3_Pass2_C(const I32_WMV  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);
Void_WMV g_IDCTDec_WMV3_Pass4_C(const IDCT_INT  *piSrc0, I16_WMV * blk16, const I32_WMV iNumLoops);


    Void_WMV g_IDCTDec_WMV2_Inter (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
//#ifndef _EMB_WMV2_
    Void_WMV g_8x4IDCTDec_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iHalf);
    Void_WMV g_4x8IDCTDec_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iHalf);
    Void_WMV g_4x4IDCTDec_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iQuadrant);
//#endif //_EMB_WMV2_

    Void_WMV g_8x4IDCTDec16_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iHalf);
    Void_WMV g_4x8IDCTDec16_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iHalf);
    Void_WMV g_4x4IDCTDec16_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iQuadrant);

#ifdef _WMV_TARGET_X86_
    Void_WMV g_8x4IDCTDec16MMX_WMV2 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalf);
    Void_WMV g_4x8IDCTDec16MMX_WMV2 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant);
    Void_WMV g_4x4IDCTDec16MMX_WMV2 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant);
#endif

#ifdef _WMV_TARGET_X86_
    Void_WMV g_IDCTDecMMX_WMV2_Inter(UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV  iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
    Void_WMV g_IntraIDCTDecMMX_WMV3 (U8_WMV __huge* piDst, I32_WMV iOffsetToNextRowForDCT, const I32_WMV __huge* rgiCoefRecon);
    Void_WMV g_InterIDCTDecMMX_WMV3 (UnionBuffer __huge* piDst, UnionBuffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
    Void_WMV g_IDCTDecMMX_WMV3 (UnionBuffer __huge* piDst, UnionBuffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
    Void_WMV g_8x4IDCTDecMMX_WMV3 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalf);
    Void_WMV g_4x8IDCTDecMMX_WMV3 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalf);
    Void_WMV g_4x4IDCTDecMMX_WMV3 (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalf);

#endif //_WMV_TARGET_X86_

#if defined(_WMV_TARGET_X86_) || defined(_Embedded_x86)
    Void_WMV g_8x4IDCTDecMMX_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iHalf);
    Void_WMV g_4x8IDCTDecMMX_WMV2 (UnionBuffer* piDstBuf, I32_WMV  iOffsetToNextRowForDCT, const UnionBuffer* rgiCoefReconBuf, I32_WMV  iHalf);
#endif //_WMV_TARGET_X86_

Void_WMV g_InitIDCTTablesDec (); 

#endif // __IDCTDEC_H_
