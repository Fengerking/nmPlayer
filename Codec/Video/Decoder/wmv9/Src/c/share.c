//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

        vopSes.cpp

Abstract:

        Base class for the encoder for one VOP session.

Author:

        Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
        Chuang Gu (chuanggu@microsoft.com) 10-December-1999

Revision History:

*************************************************************************/
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "voWmvPort.h"
#include "motioncomp_wmv.h"
#include "tables_wmv.h"
#include "repeatpad_wmv.h"


#ifdef _WMV_TARGET_X86_
#include "opcodes.h"
#include "cpudetect.h"
#endif 
#define phi1(a) ((U32_WMV)(a+iThr1) <= uThr2)

Bool_WMV bMin_Max_LE_2QP(I32_WMV a,I32_WMV b,I32_WMV c,I32_WMV d,I32_WMV e,I32_WMV f,I32_WMV g,I32_WMV h, I32_WMV i2Qp);
Bool_WMV bMin_Max_LE_2QP_Short (I32_WMV a,I32_WMV b,I32_WMV c,I32_WMV d,I32_WMV e,I32_WMV f,I32_WMV i2Qp);

int EXPANDY_REFVOP, EXPANDUV_REFVOP;

#ifdef UNDER_CE
    extern SetKMode ();
#endif

Bool_WMV IsProcessorFeaturePresentWMV (U32_WMV ProcessorFeature)
{
    Bool_WMV bIsProcessorFeaturePresent;
#ifndef ARM_C
#if defined(_ARM_)
#   ifndef __arm
    I32_WMV OldMode;
    OldMode = SetKMode (TRUE);
#   endif
    switch (ProcessorFeature) {
    case WM_PF_ARM_V4:
        bIsProcessorFeaturePresent = g_supportStrongARM();
        break;
    case WM_PF_ARM_V5:
        bIsProcessorFeaturePresent = g_supportARMv5();
        break;
    case WM_PF_ARM_INTEL_XSCALE:
        bIsProcessorFeaturePresent = g_supportXScale();
        break;
    case WM_PF_ARM_INTEL_WMMX:
        bIsProcessorFeaturePresent = g_supportWMMX();
        break;
    default: 
        bIsProcessorFeaturePresent = FALSE_WMV;
    }
#   ifndef __arm
    SetKMode (OldMode);
#   endif
#else
    bIsProcessorFeaturePresent = FALSE_WMV;
#endif
#endif
    return bIsProcessorFeaturePresent;
}

#if !defined( WMV_OPT_REPEATPAD_ARM ) 

#if !defined(_EMB_ASM_MIPS_REPEATPAD_)


Void_WMV g_RepeatRef0Y_32 (
    U8_WMV* ppxlcRef0Y,
    CoordI  iStart, 
    CoordI  iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthY,
    I32_WMV     iWidthYPlusExp,
    I32_WMV     iWidthPrevY,
    Bool_WMV bProgressive
)
{
    const U8_WMV* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
    const U8_WMV* ppxliOldRight = ppxliOldLeft + iWidthY - 1;
    const U8_WMV* ppxliOldTopLn = ppxliOldLeft - 32; //EXPANDY_REFVOP;
    U8_WMV* ppxliNew = (U8_WMV*) ppxliOldTopLn;
    U8_WMV* ppxliNewPlusWidth;
    I32_WMV iResidue = iWidthPrevY - iWidthYPlusExp;
    CoordI y;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_RepeatRef0Y);

#ifndef HITACHI 
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U32_WMV uipadValueLeft;
        U32_WMV uipadValueRight;
        I32_WMV x;
        uipadValueLeft = *ppxliOldLeft; 
        uipadValueRight = *ppxliOldRight;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueLeft |= (uipadValueLeft << 16);
        uipadValueRight |= (uipadValueRight << 16);
//      uipadValueLeft |= (uipadValueLeft << 32);
//      uipadValueRight |= (uipadValueRight << 32);
        ppxliNewPlusWidth = ppxliNew + iWidthYPlusExp;
        for (x = 0; x < 32 /*EXPANDY_REFVOP*/; x += 4) {
            *(U32_WMV *)(ppxliNew + x) = uipadValueLeft;
            *(U32_WMV *)(ppxliNewPlusWidth + x) = uipadValueRight;
        }
        ppxliNew += iWidthYPlusExp;
        ppxliNew += iResidue;
        ppxliOldLeft += iWidthPrevY;
        ppxliOldRight += iWidthPrevY;
    }
#else
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U64 uipadValueLeft;
        U64 uipadValueRight;
        I32_WMV x;
        uipadValueLeft.dwLo = (DWORD)*ppxliOldLeft; 
        uipadValueRight.dwLo = (DWORD)*ppxliOldRight;
        uipadValueLeft.dwLo |= (uipadValueLeft.dwLo << 8);
        uipadValueRight.dwLo |= (uipadValueRight.dwLo << 8);
        uipadValueLeft.dwLo |= (uipadValueLeft.dwLo << 16);
        uipadValueRight.dwLo |= (uipadValueRight.dwLo << 16);
        uipadValueLeft.dwHi = (uipadValueLeft.dwLo );
        uipadValueRight.dwHi = (uipadValueRight.dwLo );
        ppxliNewPlusWidth = ppxliNew + iWidthYPlusExp;
        for (x = 0; x < EXPANDY_REFVOP; x += 8) {
            *(U64 *)(ppxliNew + x) = uipadValueLeft;
            *(U64 *)(ppxliNewPlusWidth + x) = uipadValueRight;
        }
        ppxliNew += iWidthYPlusExp;
        ppxliNew += iResidue;
        ppxliOldLeft += iWidthPrevY;
        ppxliOldRight += iWidthPrevY;
    }
#endif
    {
#ifdef _TI_C55X_
        I32_WMV iWidthPrevYDiv8 = iWidthPrevY >> 2;
#else
        I32_WMV iWidthPrevYDiv8 = iWidthPrevY >> 3;
#endif
        U8_WMV* ppxliSrc;
        U8_WMV* ppxliDst;
        if (fTop) {
            U8_WMV* ppxliLeftTop = ppxlcRef0Y;
            for (y = 0; y < 32 /*EXPANDY_REFVOP*/; y++) {
                I32_WMV x;
                //memcpy (ppxliLeftTop, ppxliOldTopLn, iWidthPrevY);
                ppxliSrc = (U8_WMV*) ppxliOldTopLn;
                ppxliDst = (U8_WMV*) ppxliLeftTop;
                for (x = 0; x < iWidthPrevYDiv8; x++) {
#ifdef _TI_C55X_
                    *(U32_WMV *) ppxliDst = *(U32_WMV *) ppxliSrc;
#else
                    *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
#endif
                    ppxliSrc += 8;
                    ppxliDst += 8;
                }
                ppxliLeftTop += iWidthPrevY;
            }
        }
        if (fBottom) {
            const U8_WMV* ppxliOldBotLn = ppxliNew - iWidthPrevY;
            I32_WMV iHeightYPadded = (iEnd + 15) & ~15;
            I32_WMV iBotExtend = iHeightYPadded - iEnd + 32; //EXPANDY_REFVOP;
            for (y = 0; y < iBotExtend; y++) {
                I32_WMV x;
                //memcpy (ppxliNew, ppxliOldBotLn, iWidthPrevY);
                ppxliSrc = (U8_WMV*) ppxliOldBotLn;
                ppxliDst = (U8_WMV*) ppxliNew;
                for (x = 0; x < iWidthPrevYDiv8; x++) {
#ifdef _TI_C55X_
                    *(U32_WMV *) ppxliDst = *(U32_WMV *) ppxliSrc;
#else
                    *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
#endif                  
                    ppxliSrc += 8;
                    ppxliDst += 8;
                }
                ppxliNew += iWidthPrevY;
            }
        }
    }
}





#endif


Void_WMV g_RepeatRef0UV_16 (
    U8_WMV* ppxlcRef0U,
    U8_WMV* ppxlcRef0V,
    CoordI  iStart, CoordI iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV,
    Bool_WMV    bProgressive
)
{
    const U8_WMV* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
    const U8_WMV* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
    const U8_WMV* ppxliOldRightU = ppxliOldLeftU + iWidthUV - 1;
    const U8_WMV* ppxliOldRightV = ppxliOldLeftV + iWidthUV - 1;
    const U8_WMV* ppxliOldTopLnU = ppxliOldLeftU - 16; //EXPANDUV_REFVOP;
    const U8_WMV* ppxliOldTopLnV = ppxliOldLeftV - 16; //EXPANDUV_REFVOP;
    U8_WMV* ppxliNewU = (U8_WMV*) ppxliOldTopLnU;
    U8_WMV* ppxliNewV = (U8_WMV*) ppxliOldTopLnV;
    U8_WMV* ppxliNewUPlusWidth;
    U8_WMV* ppxliNewVPlusWidth;
    I32_WMV iResidue = iWidthPrevUV - iWidthUVPlusExp;
    CoordI y;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_RepeatRef0UV);

#ifndef HITACHI
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        I32_WMV x;
        U32_WMV uipadValueLeftU, uipadValueLeftV;
        U32_WMV uipadValueRightU, uipadValueRightV;
        uipadValueLeftU = *ppxliOldLeftU;
        uipadValueLeftV = *ppxliOldLeftV;
        uipadValueLeftU |= (uipadValueLeftU << 8);
        uipadValueLeftV |= (uipadValueLeftV << 8);
        uipadValueLeftU |= (uipadValueLeftU << 16);
        uipadValueLeftV |= (uipadValueLeftV << 16);
    //  uipadValueLeftU |= (uipadValueLeftU << 32);
    //  uipadValueLeftV |= (uipadValueLeftV << 32);
        uipadValueRightU = *ppxliOldRightU;
        uipadValueRightV = *ppxliOldRightV;
        uipadValueRightU |= (uipadValueRightU << 8);
        uipadValueRightV |= (uipadValueRightV << 8);
        uipadValueRightU |= (uipadValueRightU << 16);
        uipadValueRightV |= (uipadValueRightV << 16);
    //  uipadValueRightU |= (uipadValueRightU << 32);
    //  uipadValueRightV |= (uipadValueRightV << 32);
        ppxliNewUPlusWidth = ppxliNewU + iWidthUVPlusExp;
        ppxliNewVPlusWidth = ppxliNewV + iWidthUVPlusExp;
        for (x = 0; x < 16 /*EXPANDUV_REFVOP*/; x += 4) {
            *(U32_WMV *)(ppxliNewU + x) = uipadValueLeftU;
            *(U32_WMV *)(ppxliNewV + x) = uipadValueLeftV;
            *(U32_WMV *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
            *(U32_WMV *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
        }

        ppxliNewU += iWidthUVPlusExp;       
        ppxliNewV += iWidthUVPlusExp;
        ppxliNewU += iResidue;      
        ppxliNewV += iResidue;      
        ppxliOldLeftU += iWidthPrevUV;
        ppxliOldLeftV += iWidthPrevUV;
        ppxliOldRightU += iWidthPrevUV;
        ppxliOldRightV += iWidthPrevUV;
    }
#else
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        I32_WMV x;
        U64 uipadValueLeftU, uipadValueLeftV;
        U64 uipadValueRightU, uipadValueRightV;
        uipadValueLeftU.dwLo = (DWORD)*ppxliOldLeftU;
        uipadValueLeftV.dwLo = (DWORD)*ppxliOldLeftV;
        uipadValueLeftU.dwLo |= (uipadValueLeftU.dwLo << 8);
        uipadValueLeftV.dwLo |= (uipadValueLeftV.dwLo << 8);
        uipadValueLeftU.dwLo |= (uipadValueLeftU.dwLo << 16);
        uipadValueLeftV.dwLo |= (uipadValueLeftV.dwLo << 16);
        uipadValueLeftU.dwHi = (uipadValueLeftU.dwLo);
        uipadValueLeftV.dwHi = (uipadValueLeftV.dwLo);
        uipadValueRightU.dwLo = (DWORD)*ppxliOldRightU;
        uipadValueRightV.dwLo = (DWORD)*ppxliOldRightV;
        uipadValueRightU.dwLo |= (uipadValueRightU.dwLo << 8);
        uipadValueRightV.dwLo |= (uipadValueRightV.dwLo << 8);
        uipadValueRightU.dwLo |= (uipadValueRightU.dwLo << 16);
        uipadValueRightV.dwLo |= (uipadValueRightV.dwLo << 16);
        uipadValueRightU.dwHi = (uipadValueRightU.dwLo );
        uipadValueRightV.dwHi = (uipadValueRightV.dwLo);
        ppxliNewUPlusWidth = ppxliNewU + iWidthUVPlusExp;
        ppxliNewVPlusWidth = ppxliNewV + iWidthUVPlusExp;
        for (x = 0; x < EXPANDUV_REFVOP; x += 8) {
            *(U64 *)(ppxliNewU + x) = uipadValueLeftU;
            *(U64 *)(ppxliNewV + x) = uipadValueLeftV;
            *(U64 *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
            *(U64 *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
        }
        ppxliNewU += iWidthUVPlusExp;       
        ppxliNewV += iWidthUVPlusExp;
        ppxliNewU += iResidue;      
        ppxliNewV += iResidue;      
        ppxliOldLeftU += iWidthPrevUV;
        ppxliOldLeftV += iWidthPrevUV;
        ppxliOldRightU += iWidthPrevUV;
        ppxliOldRightV += iWidthPrevUV;
    }
#endif
    {
        I32_WMV iWidthPrevUVDiv4 = iWidthPrevUV >> 2;
        U8_WMV* ppxliSrcU;
        U8_WMV* ppxliSrcV;
        U8_WMV* ppxliDstU;
        U8_WMV* ppxliDstV;
        if (fTop) {
            U8_WMV* ppxliLeftTopU = ppxlcRef0U;
            U8_WMV* ppxliLeftTopV = ppxlcRef0V;
            for (y = 0; y < 16 /*EXPANDUV_REFVOP*/; y++) {
                I32_WMV x;
                ppxliSrcU = (U8_WMV*) ppxliOldTopLnU;
                ppxliSrcV = (U8_WMV*) ppxliOldTopLnV;
                ppxliDstU = ppxliLeftTopU;
                ppxliDstV = ppxliLeftTopV;
                for (x = 0; x < iWidthPrevUVDiv4; x++) {
                    *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                    ppxliSrcU += 4;
                    ppxliDstU += 4;
                    *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                    ppxliSrcV += 4;
                    ppxliDstV += 4;
                }
                ppxliLeftTopU += iWidthPrevUV;
                ppxliLeftTopV += iWidthPrevUV;
            }
        }
        if (fBottom) {
            const U8_WMV* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
            const U8_WMV* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;
            I32_WMV iHeightUVPadded = ((iEnd * 2 + 15) & ~15)/2;
            I32_WMV iBotExtend = iHeightUVPadded - iEnd + 16; //EXPANDUV_REFVOP;       
            for (y = 0; y < iBotExtend; y++) {
                I32_WMV x;
                ppxliSrcU = (U8_WMV*) ppxliOldBotLnU;
                ppxliSrcV = (U8_WMV*) ppxliOldBotLnV;
                ppxliDstU = ppxliNewU;
                ppxliDstV = ppxliNewV;
                for (x = 0; x < iWidthPrevUVDiv4; x++) {
                    *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                    ppxliSrcU += 4;
                    ppxliDstU += 4;
                    *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                    ppxliSrcV += 4;
                    ppxliDstV += 4;
                }
                ppxliNewU += iWidthPrevUV;
                ppxliNewV += iWidthPrevUV;
            }
        }
    }
}




Void_WMV g_RepeatRef0Y_24 (
    U8_WMV* ppxlcRef0Y,
    CoordI  iStart, 
    CoordI  iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthY,
    I32_WMV     iWidthYPlusExp,
    I32_WMV     iWidthPrevY,
    Bool_WMV    bProgressive
)
{
    const U8_WMV* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
    const U8_WMV* ppxliOldRight = ppxliOldLeft + iWidthY - 1;
    const U8_WMV* ppxliOldTopLn = ppxliOldLeft - 24 ;//EXPANDY_REFVOP;
    U8_WMV* ppxliNew = (U8_WMV*) ppxliOldTopLn;
    U8_WMV* ppxliNewPlusWidth;
    I32_WMV iResidue = iWidthPrevY - iWidthYPlusExp;
    CoordI y;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_RepeatRef0Y);

    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U32_WMV uipadValueLeft;
        U32_WMV uipadValueRight;
        I32_WMV x;
        uipadValueLeft = *ppxliOldLeft; 
        uipadValueRight = *ppxliOldRight;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueLeft |= (uipadValueLeft << 16);
        uipadValueRight |= (uipadValueRight << 16);
//      uipadValueLeft |= (uipadValueLeft << 32);
//      uipadValueRight |= (uipadValueRight << 32);
        ppxliNewPlusWidth = ppxliNew + iWidthYPlusExp;
        for (x = 0; x < 24 /*EXPANDY_REFVOP*/; x += 4) {
            *(U32_WMV *)(ppxliNew + x) = uipadValueLeft;
            *(U32_WMV *)(ppxliNewPlusWidth + x) = uipadValueRight;
        }
        ppxliNew += iWidthYPlusExp;
        ppxliNew += iResidue;
        ppxliOldLeft += iWidthPrevY;
        ppxliOldRight += iWidthPrevY;
    }

    {
        I32_WMV iWidthPrevYDiv8 = iWidthPrevY >> 3;
        U8_WMV* ppxliSrc;
        U8_WMV* ppxliDst;
        if (fTop) {
            U8_WMV* ppxliLeftTop = ppxlcRef0Y;
            for (y = 0; y < 24 /*EXPANDY_REFVOP*/; y++) {
                I32_WMV x;
                //memcpy (ppxliLeftTop, ppxliOldTopLn, iWidthPrevY);
                ppxliSrc = (U8_WMV*) ppxliOldTopLn;
                ppxliDst = (U8_WMV*) ppxliLeftTop;
                for (x = 0; x < iWidthPrevYDiv8; x++) {
                    *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
                    ppxliSrc += 8;
                    ppxliDst += 8;
                }
                ppxliLeftTop += iWidthPrevY;
            }
        }
        if (fBottom) {
            const U8_WMV* ppxliOldBotLn = ppxliNew - iWidthPrevY;
            I32_WMV iHeightYPadded = (iEnd + 15) & ~15;
            I32_WMV iBotExtend = iHeightYPadded - iEnd + 24; //EXPANDY_REFVOP;
            for (y = 0; y < iBotExtend; y++) {
                I32_WMV x;
                //memcpy (ppxliNew, ppxliOldBotLn, iWidthPrevY);
                ppxliSrc = (U8_WMV*) ppxliOldBotLn;
                ppxliDst = (U8_WMV*) ppxliNew;
                for (x = 0; x < iWidthPrevYDiv8; x++) {
                    *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
                    ppxliSrc += 8;
                    ppxliDst += 8;
                }
                ppxliNew += iWidthPrevY;
            }
        }
    }
}

Void_WMV g_RepeatRef0UV_12 (
    U8_WMV* ppxlcRef0U,
    U8_WMV* ppxlcRef0V,
    CoordI  iStart, CoordI iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV,
    Bool_WMV bProgressive
)
{
    const U8_WMV* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
    const U8_WMV* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
    const U8_WMV* ppxliOldRightU = ppxliOldLeftU + iWidthUV - 1;
    const U8_WMV* ppxliOldRightV = ppxliOldLeftV + iWidthUV - 1;
    const U8_WMV* ppxliOldTopLnU = ppxliOldLeftU - 12 ;//EXPANDUV_REFVOP;
    const U8_WMV* ppxliOldTopLnV = ppxliOldLeftV - 12 ;//EXPANDUV_REFVOP;
    U8_WMV* ppxliNewU = (U8_WMV*) ppxliOldTopLnU;
    U8_WMV* ppxliNewV = (U8_WMV*) ppxliOldTopLnV;
    U8_WMV* ppxliNewUPlusWidth;
    U8_WMV* ppxliNewVPlusWidth;
    I32_WMV iResidue = iWidthPrevUV - iWidthUVPlusExp;
    CoordI y;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_RepeatRef0UV);

    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        I32_WMV x;
        U32_WMV uipadValueLeftU, uipadValueLeftV;
        U32_WMV uipadValueRightU, uipadValueRightV;
        uipadValueLeftU = *ppxliOldLeftU;
        uipadValueLeftV = *ppxliOldLeftV;
        uipadValueLeftU |= (uipadValueLeftU << 8);
        uipadValueLeftV |= (uipadValueLeftV << 8);
        uipadValueLeftU |= (uipadValueLeftU << 16);
        uipadValueLeftV |= (uipadValueLeftV << 16);
        uipadValueRightU = *ppxliOldRightU;
        uipadValueRightV = *ppxliOldRightV;
        uipadValueRightU |= (uipadValueRightU << 8);
        uipadValueRightV |= (uipadValueRightV << 8);
        uipadValueRightU |= (uipadValueRightU << 16);
        uipadValueRightV |= (uipadValueRightV << 16);
        ppxliNewUPlusWidth = ppxliNewU + iWidthUVPlusExp;
        ppxliNewVPlusWidth = ppxliNewV + iWidthUVPlusExp;
        for (x = 0; x < 12 /*EXPANDUV_REFVOP*/; x += 4) {
            *(U32_WMV *)(ppxliNewU + x) = uipadValueLeftU;
            *(U32_WMV *)(ppxliNewV + x) = uipadValueLeftV;
            *(U32_WMV *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
            *(U32_WMV *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
        }

        ppxliNewU += iWidthUVPlusExp;       
        ppxliNewV += iWidthUVPlusExp;
        ppxliNewU += iResidue;      
        ppxliNewV += iResidue;      
        ppxliOldLeftU += iWidthPrevUV;
        ppxliOldLeftV += iWidthPrevUV;
        ppxliOldRightU += iWidthPrevUV;
        ppxliOldRightV += iWidthPrevUV;
    }

    {
        I32_WMV iWidthPrevUVDiv4 = iWidthPrevUV >> 2;
        U8_WMV* ppxliSrcU;
        U8_WMV* ppxliSrcV;
        U8_WMV* ppxliDstU;
        U8_WMV* ppxliDstV;
        if (fTop) {
            U8_WMV* ppxliLeftTopU = ppxlcRef0U;
            U8_WMV* ppxliLeftTopV = ppxlcRef0V;
            for (y = 0; y < 12 /*EXPANDUV_REFVOP*/; y++) {
                I32_WMV x;
                ppxliSrcU = (U8_WMV*) ppxliOldTopLnU;
                ppxliSrcV = (U8_WMV*) ppxliOldTopLnV;
                ppxliDstU = ppxliLeftTopU;
                ppxliDstV = ppxliLeftTopV;
                for (x = 0; x < iWidthPrevUVDiv4; x++) {
                    *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                    ppxliSrcU += 4;
                    ppxliDstU += 4;
                    *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                    ppxliSrcV += 4;
                    ppxliDstV += 4;
                }
                ppxliLeftTopU += iWidthPrevUV;
                ppxliLeftTopV += iWidthPrevUV;
            }
        }
        if (fBottom) {
            const U8_WMV* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
            const U8_WMV* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;
            I32_WMV iHeightUVPadded = ((iEnd * 2 + 15) & ~15)/2;
            I32_WMV iBotExtend = iHeightUVPadded - iEnd + 12 ;//EXPANDUV_REFVOP;       
            for (y = 0; y < iBotExtend; y++) {
                I32_WMV x;
                ppxliSrcU = (U8_WMV*) ppxliOldBotLnU;
                ppxliSrcV = (U8_WMV*) ppxliOldBotLnV;
                ppxliDstU = ppxliNewU;
                ppxliDstV = ppxliNewV;
                for (x = 0; x < iWidthPrevUVDiv4; x++) {
                    *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                    ppxliSrcU += 4;
                    ppxliDstU += 4;
                    *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                    ppxliSrcV += 4;
                    ppxliDstV += 4;
                }
                ppxliNewU += iWidthPrevUV;
                ppxliNewV += iWidthPrevUV;
            }
        }
    }
}

#endif //WMV_OPT_REPEATPAD_ARM

#if !(defined(WMV_OPT_LOOPFILTER_ARM)||defined(_MIPS_ASM_LOOPFILTER_OPT_)||defined(_SH4_ASM_LOOPFILTER_OPT_)||defined(SH3_ASM_LOOPFILTER)||defined(SH3_DSP_ASM_LOOPFILTER))
#ifndef WMV9_SIMPLE_ONLY
Void_WMV g_FilterHorizontalEdge_WMV(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                            I32_WMV iNumPixel)
{
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterHorizontalEdge_WMV);

    // Filter horizontal line        
    for (i = 0; i < iNumPixel; ++i) {                                                                                                                                                                                                    
        I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;
        I32_WMV v4_v5, a30, absA30;
        I32_WMV a, c;                
        U8_WMV *pVtmp = pV5;

        v5 = *pV5;
        pVtmp -= iPixelDistance;
        v4 = *pVtmp;

        v4_v5 = v4 - v5;
        c = v4_v5/2;

        if (c == 0) 
        {
            pV5 ++;
            continue;
        }

        pVtmp -= iPixelDistance;
        v3 = *pVtmp;
        pVtmp -= iPixelDistance;
        v2 = *pVtmp;
        pVtmp -= iPixelDistance;
        v1 = *pVtmp;
        pVtmp = pV5 + iPixelDistance;
        v6 = *pVtmp;        

        a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
        if (((c & 0x80000000) ^ (a30 & 0x80000000)) == 0)
        {
            pV5 ++;
            continue;  
        }

        pVtmp += iPixelDistance;
        v7 = *pVtmp;
        pVtmp += iPixelDistance;
        v8 = *pVtmp;

        absA30 = abs(a30);
        if (absA30 < iStepSize) {

            I32_WMV v2_v3 = v2 - v3;
            I32_WMV v6_v7 = v6 - v7;
            I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
            I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
            I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 

            if (iMina31_a32 < absA30){

                I32_WMV dA30 = absA30 - iMina31_a32;
                dA30 = (5*dA30) >> 3;
                a = min(dA30, abs(c));
                if (v4 < v5) a = -a;
                *(pV5 - iPixelDistance) = v4 - a;
                *(pV5) = v5 + a;
               
                /*if (0 < c) {
                    if (a30 < 0) {
                        I32_WMV dA30;
                        dA30 = absA30 - iMina31_a32 ;  // > 0
                        a = (5 * dA30) >> 3; // >= 0
                        if (a > c) a = c;
                        *(pV5 - iPixelDistance) = v4 - a;                                                                                         
                        *(pV5) = v5 + a;
                    } 
                } else {
                    if (a30 >= 0) {                       
                        I32_WMV dA30;
                        dA30 =  iMina31_a32 - absA30; // < 0
                        a = (5 * dA30 + 7) >> 3; // <= 0
                        if (a < c) a = c;
                        *(pV5 - iPixelDistance) = v4 - a;                                                                                         
                        *(pV5) = v5 + a;
                    }
                }*/
            }
        }        
        pV5 ++;
    }
}


Void_WMV g_FilterVerticalEdge_WMV(U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                          I32_WMV iNumPixel)
{
    U8_WMV *pVh;
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterVerticalEdge_WMV);

    // Filter vertical line
    for (i = 0; i < iNumPixel; ++i) {
        
        I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;  
        I32_WMV v4_v5, a30, absA30;
        I32_WMV a, c;                

        pVh = pVhstart;            

        v4 = pVh[4];
        v5 = pVh[5];

        v4_v5 = v4 - v5;
        c = v4_v5/2;

        if (c == 0) 
        {
            pVhstart += iPixelDistance;
            continue;
        }

        v3 = pVh[3]; 
        v6 = pVh[6]; 

        a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
        if (((c & 0x80000000) ^ (a30 & 0x80000000)) == 0)
        {
            pVhstart += iPixelDistance;
            continue;  
        }

        v1 = pVh[1]; 
        v2 = pVh[2];
        v7 = pVh[7]; 
        v8 = pVh[8]; 

        absA30 = abs(a30);
        if (absA30 < iStepSize) {

            I32_WMV v2_v3 = v2 - v3;
            I32_WMV v6_v7 = v6 - v7;
            I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
            I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
            I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 

            if (iMina31_a32 < absA30){

                I32_WMV dA30 = absA30 - iMina31_a32;
                dA30 = (5*dA30) >> 3;
                a = min(dA30, abs(c));
                if (v4 < v5) a = -a;
                pVh[4] = v4 - a;                                                                                         
                pVh[5] = v5 + a;
               
                /*if (0 < c) {
                    if (a30 < 0) {
                        I32_WMV dA30;
                        dA30 = absA30 - iMina31_a32 ;  // > 0
                        a = (5 * dA30) >> 3; // >= 0
                        if (a > c) a = c;
                        pVh[4] = v4 - a;                                                                                         
                        pVh[5] = v5 + a;
                    } 
                } else {
                    if (a30 >= 0) {                       
                        I32_WMV dA30;
                        dA30 =  iMina31_a32 - absA30; // < 0
                        a = (5 * dA30 + 7) >> 3; // <= 0
                        if (a < c) a = c;
                        pVh[4] = v4 - a;                                                                                         
                        pVh[5] = v5 + a;
                    }
                }*/
            }
        }       
        pVhstart += iPixelDistance;                
    }
}
#endif // WMV9_SIMPLE_ONLY
#endif // WMV_OPT_LOOPFILTER_ARM

#if 0
Void_WMV FilterEdgeShortTagMBRow(
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxlcCenter, 
    I32_WMV iPixelDistance, 
    I32_WMV iStepSize, 
    I32_WMV iMBsPerRow, 
    Bool_WMV bFirst, 
    Bool_WMV bLast
    )
{
    U8_WMV* pVhstart;
    U8_WMV* pV5 = ppxlcCenter;
    U8_WMV* pV5x = pV5 + 8*iPixelDistance;

    I32_WMV iVertSize = 16;
    I32_WMV iVertOffset = -4;
    I32_WMV imbX;

    assert(bFirst == FALSE);

    if (bLast)
        iVertSize += 4;

    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
    pV5 += 4;
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 4);    
    pV5x += 4;

    // Loop through the macroblocks in the row filtering horiz. line first then vertical
    pVhstart = ppxlcCenter + iVertOffset*iPixelDistance + 3;

    for (imbX = 0; imbX < iMBsPerRow - 1; imbX++) {
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 16);
        pV5 += 16;//iPixelDistance*8;          
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 16);
        pV5x += 16;

        (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
        (*pWMVDec->m_pFilterVerticalEdge)(pVhstart + 8, iPixelDistance, iStepSize, iVertSize);
        pVhstart += 16;
    }

    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 12);
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 12);
}

Void_WMV FilterEdgeShortTagBlockRow(tWMVDecInternalMember *pWMVDec, U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iBlocksPerRow, Bool_WMV bFirst, Bool_WMV bLast)
{    
    U8_WMV *pVhstart;
    U8_WMV *pV5 = ppxlcCenter;

    I32_WMV iVertSize = 8;
    I32_WMV iVertOffset = -4;
    I32_WMV iblkX;

    if (bFirst)
    {
        iVertSize = 12;
        iVertOffset = -8;
    }
    if (bLast)
        iVertSize += 4;

    pVhstart = ppxlcCenter + iPixelDistance*iVertOffset + 3;

    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
    pV5 += 4;

    // Loop through the blocks in the row filtering horiz line first then vertical
    for (iblkX = 0; iblkX < iBlocksPerRow - 1; iblkX++) {
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 8);
        pV5 += 8;
        (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize,  iVertSize);        
        pVhstart += 8;
    }

    // Filter last four pixels in the horizontal line
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);

}

#endif


#ifndef WMV9_SIMPLE_ONLY
Void_WMV DeblockSLFrame (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, 
    Bool_WMV bMultiThread, U32_WMV iThreadID, I32_WMV iMBStartX, I32_WMV iMBEndX, I32_WMV iMBStartY, I32_WMV iMBEndY
) // Frame-based deblocking
{
    
    U8_WMV * ppxliPost;
    Bool_WMV bOneMBRow;
    //I32_WMV iblkSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY>>1;  // / 2;
    I32_WMV iMBStartUV = iMBStartY;
    //I32_WMV iVertSize = (iMBEndY - iMBStartY) * MB_SIZE;
    I32_WMV  iStep = pWMVDec->m_iStepSize;
    I32_WMV iblkY;
    FUNCTION_PROFILE_DECL_START(fp,DEBLOCKSLFRAME_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockSLFrame);
    

    if (pWMVDec->m_bSliceWMVA) {
        // by slice segments
        U8_WMV __huge* ppxliPostY	= ppxliY;
        U8_WMV __huge* ppxliPostU	= ppxliU;
        U8_WMV __huge* ppxliPostV	= ppxliV;
        I32_WMV iMBY;

        if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1)
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig + pWMVDec->m_uintNumMBY;        
        else
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;

        for ( iMBY = iMBStartY - 1; iMBY < iMBEndY; iMBY ++) {
            Bool_WMV  bFlag0            = (iMBY == 0) || pWMVDec->m_pbStartOfSliceRow [ iMBY ];
            Bool_WMV  bFlag1            = ((iThreadID == pWMVDec->m_uiNumProcessors - 1) && ((iMBY + 1) == iMBEndY)) || pWMVDec->m_pbStartOfSliceRow[iMBY + 1];

            FilterEdgeShortTagMBRow    (pWMVDec, ppxliPostY, pWMVDec->m_iWidthPrevY,  iStep, iMBEndX, bFlag0, bFlag1);
            FilterEdgeShortTagBlockRow (pWMVDec, ppxliPostU, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, bFlag0, bFlag1);
            FilterEdgeShortTagBlockRow (pWMVDec, ppxliPostV, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, bFlag0, bFlag1);
            ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
            ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
            ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
        }

#if 0
        fpx = fopen ("c:\\junk\\dumpdec1.yuv", "ab");
        for (I32_WMV j = 0; j < 486; j ++) {
            fwrite (ppxliY + j * 784, 720, 1, fpx);
        }
        for ( j = 0; j < 243; j ++) {
            fwrite (ppxliU + j * 392, 360, 1, fpx);
        }
        for ( j = 0; j < 243; j ++) {
            fwrite (ppxliV + j * 392, 360, 1, fpx);
        }
        fclose (fpx);
#endif
    } else {

		if (iThreadID == pWMVDec->m_uiNumProcessors - 1)
		{
			iMBEndY--;
		}
		bOneMBRow = (iMBEndY == 0);

        // Deblock Y
        if (iThreadID == 0)
            FilterEdgeShortTagMBRow (pWMVDec,  ppxliY, pWMVDec->m_iWidthPrevY, iStep, iMBEndX, TRUE, bOneMBRow);
        //FilterEdgeShortTagMBRow (pWMVDec,  ppxliY, pWMVDec->m_iWidthPrevY, iStep, iMBEndX, TRUE, FALSE);
    
        ppxliPost = ppxliY + iMBStartY * pWMVDec->m_iMBSizeXWidthPrevY;
    
        for (iblkY = iMBStartY; iblkY < iMBEndY; iblkY++) {
            FilterEdgeShortTagMBRow(pWMVDec,  ppxliPost, pWMVDec->m_iWidthPrevY, iStep, iMBEndX, FALSE, FALSE);
            ppxliPost += pWMVDec->m_iMBSizeXWidthPrevY;
        }
    
        if ((iThreadID == pWMVDec->m_uiNumProcessors - 1) && !bOneMBRow)
            FilterEdgeShortTagMBRow(pWMVDec,  ppxliPost, pWMVDec->m_iWidthPrevY, iStep, iMBEndX, FALSE, TRUE);
    
        // Deblock U
        if (iThreadID == 0) 
            FilterEdgeShortTagBlockRow (pWMVDec, ppxliU, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, TRUE, bOneMBRow);
        // FilterEdgeShortTagBlockRow (pWMVDec, ppxliU, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, TRUE, FALSE);
    
        ppxliPost = ppxliU + iMBStartUV * pWMVDec->m_iBlkSizeXWidthPrevUV;
        for (iblkY = iMBStartUV; iblkY < iMBEndY; iblkY++) {
            FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, FALSE, FALSE);
            ppxliPost += pWMVDec->m_iBlkSizeXWidthPrevUV;
        }
    
        if ((iThreadID == pWMVDec->m_uiNumProcessors - 1) && !bOneMBRow)
            FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, FALSE, TRUE);
    
        // Deblock V
        if (iThreadID == 0) 
            FilterEdgeShortTagBlockRow (pWMVDec, ppxliV, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, TRUE,  bOneMBRow);
        //  FilterEdgeShortTagBlockRow (pWMVDec, ppxliV, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, TRUE, FALSE);
    
        ppxliPost = ppxliV + iMBStartUV * pWMVDec->m_iBlkSizeXWidthPrevUV;
        for (iblkY = iMBStartUV; iblkY < iMBEndY; iblkY++) {
            FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, FALSE, FALSE);
            ppxliPost += pWMVDec->m_iBlkSizeXWidthPrevUV;
        }
        // if (iThreadID == pWMVDec->m_uiNumProcessors - 1)
        if ((iThreadID == pWMVDec->m_uiNumProcessors - 1) && !bOneMBRow)
            FilterEdgeShortTagBlockRow(pWMVDec, ppxliPost, pWMVDec->m_iWidthPrevUV, iStep, iMBEndX, FALSE, TRUE);
        FUNCTION_PROFILE_STOP(&fp);
        }
}

Void_WMV DeblockSLFrame_V8 (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, 
    Bool_WMV bMultiThread, U32_WMV iThreadID, I32_WMV iMBStartX, I32_WMV iMBEndX, I32_WMV iMBStartY, I32_WMV iMBEndY
) // Frame-based deblocking
{
    U8_WMV __huge* ppxliPostY, *ppxliPostU, *ppxliPostV;
    I32_WMV iblkSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY>>1;  // / 2;
    

    I32_WMV iWidthPrevY = pWMVDec->m_iWidthPrevY;
    I32_WMV iWidthPrevUV = pWMVDec->m_iWidthPrevUV;

    I32_WMV iStepSize=pWMVDec->m_iStepSize;
    I32_WMV iWidthInternalUVTimesBlk=pWMVDec->m_iWidthInternalUVTimesBlk;
    //I32_WMV iMBEndX,  iMBEndY;

    U8_WMV * pVhstart = ppxliY + 3; 
    I32_WMV iblkX ;
    I32_WMV iMBsPerRow;
    I32_WMV imbY;
    I32_WMV iYVhstartOffset;
    U8_WMV* pUV5;
    U8_WMV* pVV5;
    I32_WMV iUVVhstartOffset;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockSLFrame_V8);

    iMBEndX=pWMVDec->m_uiNumMBX;
    iMBEndY=pWMVDec->m_uiNumMBY;
    iMBEndY--;        
    for ( iblkX = 0; iblkX < (2*iMBEndX - 1); iblkX++) 
    {        
        g_FilterVerticalEdge_WMV(pVhstart, iWidthPrevY, iStepSize, 4);
        pVhstart+=8;        
    }
  
    ppxliPostY = ppxliY;
    ppxliPostU = ppxliU;
    ppxliPostV = ppxliV;
    iMBsPerRow = iMBEndX;
    iYVhstartOffset = (-4)*iWidthPrevY - 1;
    for (imbY = 0; imbY < iMBEndY; imbY++) 
    {
        U8_WMV __huge* ppxliCodedPostY = ppxliPostY;
        U8_WMV __huge* ppxliCodedPostU = ppxliPostU;
        U8_WMV __huge* ppxliCodedPostV = ppxliPostV;

        U8_WMV* pYV5=ppxliPostY+iblkSizeXWidthPrevY;
        U8_WMV* pYV5x=ppxliPostY+8*iWidthPrevY+iblkSizeXWidthPrevY;
        I32_WMV iUVVertSize=8;
        I32_WMV iUVVertOffset = -4;
        I32_WMV imbX;
        U8_WMV* pYVhstart;

        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 4);
        pYV5 += 4;
        g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 4);
        pYV5x += 4;
        if(imbY==0)
        {
            iUVVertSize=12;
            iUVVertOffset = -8;
        }
        else if(imbY==(iMBEndY-1))
        {
            iUVVertSize=12;
            iUVVertOffset = -4;
        }

        //U
        pUV5=ppxliPostU+iWidthInternalUVTimesBlk;
        g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 4);
        pUV5 += 4;

        iUVVhstartOffset = iUVVertOffset*iWidthPrevUV - 1;

        //V
        pVV5=ppxliPostV + iWidthInternalUVTimesBlk;
        g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 4);
        pVV5 += 4;
        
        for (imbX = 0; imbX < (iMBsPerRow -1); imbX++) 
        {
            U8_WMV* pYVhstart = pYV5 + iYVhstartOffset;  
            U8_WMV* pUVhstart = pUV5 + iUVVhstartOffset; 
            U8_WMV* pVVhstart = pVV5 + iUVVhstartOffset;
            
            // Y
            g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 16);
            pYV5 += 16;
            g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 16);        
            pYV5x += 16;
            g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 16);
            //UV
            g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 8);
            pUV5 += 8;
            g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 8);
            pVV5 += 8;

            g_FilterVerticalEdge_WMV(pUVhstart, iWidthPrevUV, iStepSize, iUVVertSize);          
            g_FilterVerticalEdge_WMV(pVVhstart, iWidthPrevUV, iStepSize, iUVVertSize);
            g_FilterVerticalEdge_WMV(pYVhstart + 8, iWidthPrevY, iStepSize, 16);         

            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        //Y
        pYVhstart = pYV5 + iYVhstartOffset;  
        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 12);
        g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 12);              
        g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 16); 
        g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 4);
        g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 4);

        ppxliPostY += pWMVDec->m_iWidthInternalTimesMB;
        ppxliPostU += iWidthInternalUVTimesBlk;
        ppxliPostV += iWidthInternalUVTimesBlk;
    }

    //row iMBEndY-1
 
    {
        U8_WMV __huge* ppxliCodedPostY = ppxliPostY;
        U8_WMV __huge* ppxliCodedPostU = ppxliPostU;
        U8_WMV __huge* ppxliCodedPostV = ppxliPostV;
        I32_WMV imbX;

        U8_WMV* pYV5=ppxliPostY+iblkSizeXWidthPrevY;
        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 4);
        pYV5 += 4;
        for (imbX = 0; imbX < iMBsPerRow; imbX++) 
        {
            U8_WMV* pYVhstart = pYV5 + iYVhstartOffset;        
            I32_WMV hy_size=16;
            I32_WMV huv_size=8;
            if(imbX==(iMBsPerRow-1))
            {
                hy_size=12;
                huv_size=4;
            }

            g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, hy_size);
            pYV5 += 16;
            g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 12);
            if(imbX < (iMBsPerRow-1))
                g_FilterVerticalEdge_WMV(pYVhstart + 8, iWidthPrevY, iStepSize, 12);
            
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
    }
}
#endif // WMV9_SIMPLE_ONLY

#ifndef WMV9_SIMPLE_ONLY
// Filter 8x8 boundaries for a macroblock row
Void_WMV FilterEdgeShortTagMBRow(tWMVDecInternalMember *pWMVDec, 
                                                 U8_WMV* ppxlcCenter, 
                                                 I32_WMV iPixelDistance, 
                                                 I32_WMV iStepSize, 
                                                 I32_WMV iMBsPerRow, 
                                                 Bool_WMV bFirst, 
                                                 Bool_WMV bLast)
{
    U8_WMV* pVhstart, *pVhstart2, *pVhstart2x;
    U8_WMV* pV5 = ppxlcCenter + 8*iPixelDistance, *pV5x = pV5 + 8*iPixelDistance;
    U8_WMV* pV5_2 = pV5 - 4*iPixelDistance, *pV5x_2 = pV5_2 + 8*iPixelDistance;

    I32_WMV iVertSize = 16;
    I32_WMV iVertOffset = 4;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterEdgeShortTagMBRow);

    if (bFirst)
    {
        iVertSize = 20;
        iVertOffset = 0;
    }

    if (bLast)
            iVertSize -= 4;

    else
    {
#ifndef _LONGLOOPFILTER_
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 4);
#else
        g_FilterHorizontalEdge (pV5x, iPixelDistance, iStepSize, 4);
#endif
        pV5x += 4;
    }

#ifndef _LONGLOOPFILTER_
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
#else
    g_FilterHorizontalEdge (pV5, iPixelDistance, iStepSize, 4);
#endif
    pV5 += 4;

    // Loop through the macroblocks in the row filtering horiz. line first then vertical
    pVhstart = ppxlcCenter + iVertOffset*iPixelDistance + 3;
    pVhstart2 = ppxlcCenter - 1;
    pVhstart2x = pVhstart2 + 8*iPixelDistance;

    if (!bLast)
    {
        I32_WMV imbX ;
        for ( imbX = 0; imbX < iMBsPerRow - 1; imbX++) {
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 16);
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 16);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart + 8, iPixelDistance, iStepSize, iVertSize);
            pV5 += 16;
            pV5x += 16;
            pVhstart += 16;
            pV5_2 += 16;
            pV5x_2 += 16;
            pVhstart2 += 16;
            pVhstart2x += 16;
        }
    }
    else
    {
        I32_WMV imbX ;
        for ( imbX = 0; imbX < iMBsPerRow - 1; imbX++) {
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 16);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart + 8, iPixelDistance, iStepSize, iVertSize);
            pV5 += 16;
            pVhstart += 16;
            pV5_2 += 16;
            pV5x_2 += 16;
            pVhstart2 += 16;
            pVhstart2x += 16;
        }
    }

#ifndef _LONGLOOPFILTER_
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 12);
    if (!bLast)
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 12);
    (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
#else
    (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 8);
    g_FilterHorizontalEdge (pV5, iPixelDistance, iStepSize, 4);
    if (!bLast)
    {
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5x, iPixelDistance, iStepSize, 8);
        g_FilterHorizontalEdge (pV5x, iPixelDistance, iStepSize, 4);
    }
    (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize, iVertSize);
#endif
}

// Filter 8x8 boundaries for a block row
Void_WMV FilterEdgeShortTagBlockRow(tWMVDecInternalMember *pWMVDec, 
                                                U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                                                I32_WMV iBlocksPerRow, Bool_WMV bFirst, Bool_WMV bLast)
{    
    U8_WMV *pVhstart, *pVhstart2;
    U8_WMV *pV5 = ppxlcCenter + iPixelDistance*8, *pV5_2 = pV5 - iPixelDistance*4;

    I32_WMV iVertSize = 8;
    I32_WMV iVertOffset = 4;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterEdgeShortTagBlockRow);

    if (bFirst)
    {
        iVertSize = 12;
        iVertOffset = 0;
    }
    if (bLast)
        iVertSize -= 4;
    else
    {
#ifndef _LONGLOOPFILTER_
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
#else
        g_FilterHorizontalEdge (pV5, iPixelDistance, iStepSize, 4);
#endif
        pV5 += 4;
    }

    pVhstart = ppxlcCenter + iPixelDistance*iVertOffset + 3;
    pVhstart2 = ppxlcCenter - 1;

    // Loop through the blocks in the row filtering horiz line first then vertical
    if (!bLast)
    {
        I32_WMV iblkX;
        for ( iblkX = 0; iblkX < iBlocksPerRow - 1; iblkX++) 
        {
            (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 8);
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize,  iVertSize);
            pV5 += 8;
            pV5_2 += 8;
            pVhstart += 8;
            pVhstart2 += 8;
        }
    }
    else
    {
        I32_WMV iblkX;
        for ( iblkX = 0; iblkX < iBlocksPerRow - 1; iblkX++) 
        {
            (*pWMVDec->m_pFilterVerticalEdge)(pVhstart, iPixelDistance, iStepSize,  iVertSize);
            pV5_2 += 8;
            pVhstart += 8;
            pVhstart2 += 8;
        }
    }

    // Filter last four pixels in the horizontal line
    if (!bLast)
#ifndef _LONGLOOPFILTER_
        (*pWMVDec->m_pFilterHorizontalEdge)(pV5, iPixelDistance, iStepSize, 4);
#else
        g_FilterHorizontalEdge (pV5, iPixelDistance, iStepSize, 4);
#endif
}

#endif // WMV9_SIMPLE_ONLY

I32_WMV BlkAvgX8(const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize)
{
    I32_WMV iSum = 0, i, j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BlkAvgX8);

    for (i = 0; i < BLOCK_SIZE; i++){
        for (j = 0; j < BLOCK_SIZE; j++){
            iSum += ppxlcCurrRecnMB[i* iWidthPrev + j];
        }
    }
    return ((iSum + 4) >> 3) / iStepSize;  // iSum / 64 * 8
}


I32_WMV g_iDecGlobalVarCount  = 0;
U8_WMV  g_rgiClapTabDecArray [5120 * 2]; // clapping the reconstructed pixels
U8_WMV* g_rgiClapTabDec; // clapping the reconstructed pixels

#ifdef CLAPTAB_6BIT
U8_WMV  g_rgiClap6BitTabDecArray [512]; // clapping the reconstructed pixels
U8_WMV* g_rgiClap6BitTabDec;
#endif

U8_WMV *g_InitDecGlobalVars (Void_WMV)
{
    I32_WMV iClapTabCenter = 1024 * 5; // for clapping table
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_InitDecGlobalVars);

    g_iDecGlobalVarCount++;
    if (g_iDecGlobalVarCount != 1)
        return(g_rgiClapTabDec);

    

    g_rgiClapTabDec = &(g_rgiClapTabDecArray [iClapTabCenter]);
    for ( i = -iClapTabCenter; i < iClapTabCenter; i++)
        g_rgiClapTabDec [i] = (U8_WMV) ((i < 0) ? 0 : (i > 255) ? 255 : i);

#ifdef CLAPTAB_6BIT

    iClapTabCenter = 256; // for clapping table

    g_rgiClap6BitTabDec = &(g_rgiClap6BitTabDecArray [iClapTabCenter]);
    for (i = -iClapTabCenter; i < iClapTabCenter; i++)
        g_rgiClap6BitTabDec [i] = (U8_WMV) ((i < 0) ? 0 : (i > 63) ? 63 : i);
#endif

    return(g_rgiClapTabDec);
}

static const I32_WMV s_iInv[] = {
    0, 262144, 131072, 87381, 65536, 52429, 43691, 37449,
    32768, 29127, 26214, 23831, 21845, 20165, 18725, 17476,
    16384, 15420, 14564, 13797, 13107, 12483, 11916, 11398,
    10923, 10486, 10082, 9709, 9362, 9039, 8738, 8456 ,
    8192, 7944, 7710, 7490, 7282, 7085, 6899, 6722,
    6554, 6394, 6242, 6096, 5958, 5825, 5699, 5578, 
    5461, 5350, 5243, 5140, 5041, 4946, 4855, 4766, 
    4681, 4599, 4520, 4443, 4369, 4297, 4228, 4161};



I32_WMV divroundnearest(I32_WMV i, I32_WMV iDenom)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(divroundnearest);
    assert(iDenom > 0 && iDenom < 64);
    return ((i * s_iInv[iDenom] + 0x20000) >> 18);

}

Void_WMV SetDefaultDQuantSetting(tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetDefaultDQuantSetting);
    pWMVDec->m_bDQuantOn = FALSE_WMV;
    pWMVDec->m_bDQuantBiLevel = FALSE_WMV;
    pWMVDec->m_iPanning = 0;
    pWMVDec->m_iDQuantBiLevelStepSize = pWMVDec->m_iStepSize;
    return; 
}



I8_WMV s_iNextPixel[4] = {-2, 1, 2, 1};

#if !(defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
// take out these C-code for xbox
#ifndef WMV_OPT_LOOPFILTER_ARM
Void_WMV g_FilterHorizontalEdgeV9(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                                  I32_WMV iNumPixel)
{
    I32_WMV i, j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterHorizontalEdgeV9);
    // Filter horizontal line       
    for ( j = 0; j < iNumPixel >> 2; j ++) {                                                                                                                                                                                                    
        
        pV5 += 2;
        for ( i = 0; i < 4; i++) {                                                                                                                                                                                                  
            I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;
            I32_WMV v4_v5, a30, absA30;
            I32_WMV a, c;                
            
            U8_WMV *pVtmp = pV5;
            v5 = *pV5;
            pVtmp -= iPixelDistance;
            v4 = *pVtmp;
            pVtmp -= iPixelDistance;
            v3 = *pVtmp;
            pVtmp -= iPixelDistance;
            v2 = *pVtmp;
            pVtmp -= iPixelDistance;
            v1 = *pVtmp;
            pVtmp = pV5 + iPixelDistance;
            v6 = *pVtmp;        
            pVtmp += iPixelDistance;
            v7 = *pVtmp;
            pVtmp += iPixelDistance;
            v8 = *pVtmp;
            
            v4_v5 = v4 - v5;
            c = v4_v5/2;

            if (c == 0)
            {
                if (i == 0)
                {
                    pV5 += 2;
                    break;
                }
                else
                {
                    pV5 += s_iNextPixel[i];
                    continue;
                }
            }
            
            a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;

            absA30 = abs(a30);
            if (absA30 < iStepSize) {
                
                I32_WMV v2_v3 = v2 - v3;
                I32_WMV v6_v7 = v6 - v7;
                I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
                I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
                I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
                
                if (iMina31_a32 < absA30){

                    I32_WMV dA30;
                    
                    if (((c & 0x80000000) ^ (a30 & 0x80000000)) != 0)
                    {
                        dA30 = (5*(absA30 - iMina31_a32)) >>3;
                        a = min(dA30, abs(c));
                        if (v4 < v5) a = -a;
                        *(pV5 - iPixelDistance) = v4 - a;
                        *(pV5) = v5 + a;
                    }
                }
                else if (i == 0)
                {
                    pV5 += 2;
                    break;
                }
            }
            else if (i == 0)
            {
                pV5 += 2;
                break;
            }
            
            pV5 += s_iNextPixel[i];
        }
    }
}

#ifdef DEBLOCK_H_NEON_OPT
Void_WMV g_FilterHorizontalEdgeV9_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

#define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];    
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];
	I32_WMV  Is8x8Done = uchBitField & 0x03;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterHorizontalEdgeV9_armv7_C);

	if ( 0 == Is8x8Done )
		goto DEBLOCK_H_8x4;

	{ // Load 8 rows src data for 8x8 lines.
		U8_WMV *pVtmp1 = pV5 - (iPixelDistance << 2);
		U8_WMV *pVtmp2 = pVtmp1 + iPixelDistance;
		U8_WMV *pVtmp3 = pVtmp2 + iPixelDistance;
		U8_WMV *pVtmp4 = pVtmp3 + iPixelDistance;
		U8_WMV *pVtmp5 = pVtmp4 + iPixelDistance;
		U8_WMV *pVtmp6 = pVtmp5 + iPixelDistance;
		U8_WMV *pVtmp7 = pVtmp6 + iPixelDistance;
		U8_WMV *pVtmp8 = pVtmp7 + iPixelDistance;

		for ( i = 0; i < 8; i ++) {      
			v1[i] = pVtmp1[i];
			v2[i] = pVtmp2[i];
			v3[i] = pVtmp3[i];
			v4[i] = pVtmp4[i];
			v5[i] = pVtmp5[i];
			v6[i] = pVtmp6[i];
			v7[i] = pVtmp7[i];
			v8[i] = pVtmp8[i];
		}
	}

DEBLOCK_H_8x8:

	// Do filter.
	for ( i = 0; i < 8; i ++) {     

		I16_WMV a30;
		I16_WMV  v4_v5 = v4[i] - v5[i];
        c[i]      = abs(v4_v5)>>1;  
        a30    = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
        absA30[i] = abs(a30);
		{
			I16_WMV flag1 = v4_v5 & MASK_0x8000; //sign(v4_v5)
			I16_WMV flag2 = a30   & MASK_0x8000; //sign(a30)
			Flag[i] = (flag1 != flag2) ? 1 : 0;
		}

		{
            I16_WMV v2_v3 = v2[i] - v3[i];
            I16_WMV v6_v7 = v6[i] - v7[i];
            I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
            I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

            I16_WMV dA30;
            iMina31_a32[i] = min( abs(a31), abs(a32) ); 
            dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
            a[i] = min(dA30, c[i]);

			if (v4[i] < v5[i]) 
				a[i] = -a[i];
		}
	}


	// Calculate the flag of valid result.

	for ( i = 0; i < 8; i ++) {   
		validFlag[i] = (c[i] != 0) ? 1 : 0;
		validFlag[i] &= (absA30[i] < iStepSize);
		validFlag[i] &= (iMina31_a32[i] < absA30[i]);
	}

	{
		I16_WMV flag2 = validFlag[2];
		I16_WMV flag6 = validFlag[6];
		for ( i = 0; i < 4; i ++) {  // first segment
			validFlag[i] &= ((uchBitField & 0x02)>>1);
			validFlag[i] &= flag2; 
			validFlag[i] &= Flag[i];
		}
		for ( i = 4; i < 8; i ++) {  // second segment
			validFlag[i] &= (uchBitField & 0x01);
			validFlag[i] &= flag6;
			validFlag[i] &= Flag[i];
		}
	}

	// Pick out valid results and store them.
	{
		U8_WMV *pVtmp4 = pV5 - iPixelDistance;
		U8_WMV *pVtmp5 = pV5;
		for ( i = 0; i < 8; i ++) {   
			if (validFlag[i]) {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp4[i] = (U8_WMV)v4[i];
				pVtmp5[i] = (U8_WMV)v5[i];	
			}
		}
	}

DEBLOCK_H_8x4:

	uchBitField >>= 4;

	if ( 0 == (uchBitField & 0x03) )
		return;

	{ // Load 8 rows src data for 8x4 lines.
		U8_WMV *pVtmp1 = pV5 - (iPixelDistance << 3);
		U8_WMV *pVtmp2 = pVtmp1 + iPixelDistance;
		U8_WMV *pVtmp3 = pVtmp2 + iPixelDistance;
		U8_WMV *pVtmp4 = pVtmp3 + iPixelDistance;
		U8_WMV *pVtmp5 = pVtmp4 + iPixelDistance;
		U8_WMV *pVtmp6 = pVtmp5 + iPixelDistance;
		U8_WMV *pVtmp7 = pVtmp6 + iPixelDistance;
		U8_WMV *pVtmp8 = pVtmp7 + iPixelDistance;

		if ( 0 != Is8x8Done ) {
			for ( i = 0; i < 8; i ++) {      
				v5[i] = v1[i];
				v6[i] = v2[i];
				v7[i] = v3[i];
				v8[i] = v4[i];
			}
		}
		else {
			for ( i = 0; i < 8; i ++) {      
				v5[i] = pVtmp5[i];
				v6[i] = pVtmp6[i];
				v7[i] = pVtmp7[i];
				v8[i] = pVtmp8[i];
			}
		}
		for ( i = 0; i < 8; i ++) {      
			v1[i] = pVtmp1[i];
			v2[i] = pVtmp2[i];
			v3[i] = pVtmp3[i];
			v4[i] = pVtmp4[i];
		}
	}

	pV5 -= (iPixelDistance << 2);
	goto DEBLOCK_H_8x8;
}

Void_WMV g_FilterHorizontalEdgeV9Last8x4_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

#define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];    
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterHorizontalEdgeV9Last8x4_armv7_C);


	{ // Load 8 rows src data for 8x4 lines.
		U8_WMV *pVtmp1 = pV5 - (iPixelDistance << 2);
		U8_WMV *pVtmp2 = pVtmp1 + iPixelDistance;
		U8_WMV *pVtmp3 = pVtmp2 + iPixelDistance;
		U8_WMV *pVtmp4 = pVtmp3 + iPixelDistance;
		U8_WMV *pVtmp5 = pVtmp4 + iPixelDistance;
		U8_WMV *pVtmp6 = pVtmp5 + iPixelDistance;
		U8_WMV *pVtmp7 = pVtmp6 + iPixelDistance;
		U8_WMV *pVtmp8 = pVtmp7 + iPixelDistance;

		for ( i = 0; i < 8; i ++) {      
			v1[i] = pVtmp1[i];
			v2[i] = pVtmp2[i];
			v3[i] = pVtmp3[i];
			v4[i] = pVtmp4[i];
			v5[i] = pVtmp5[i];
			v6[i] = pVtmp6[i];
			v7[i] = pVtmp7[i];
			v8[i] = pVtmp8[i];
		}
	}

	// Do filter.
	for ( i = 0; i < 8; i ++) {     

		I16_WMV a30;
		I16_WMV  v4_v5 = v4[i] - v5[i];
        c[i]      = abs(v4_v5)>>1;  
        a30    = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
        absA30[i] = abs(a30);
		{
			I16_WMV flag1 = v4_v5 & MASK_0x8000; //sign(v4_v5)
			I16_WMV flag2 = a30   & MASK_0x8000; //sign(a30)
			Flag[i] = (flag1 != flag2) ? 1 : 0;
		}

		{
            I16_WMV v2_v3 = v2[i] - v3[i];
            I16_WMV v6_v7 = v6[i] - v7[i];
            I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
            I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

            I16_WMV dA30;
            iMina31_a32[i] = min( abs(a31), abs(a32) ); 
            dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
            a[i] = min(dA30, c[i]);

			if (v4[i] < v5[i]) 
				a[i] = -a[i];
		}
	}


	// Calculate the flag of valid result.

	for ( i = 0; i < 8; i ++) {   
		validFlag[i] = (c[i] != 0) ? 1 : 0;
		validFlag[i] &= (absA30[i] < iStepSize);
		validFlag[i] &= (iMina31_a32[i] < absA30[i]);
	}

	{
		I16_WMV flag2 = validFlag[2];
		I16_WMV flag6 = validFlag[6];
		for ( i = 0; i < 4; i ++) {  // first segment
			validFlag[i] &= ((uchBitField & 0x02)>>1);
			validFlag[i] &= flag2; 
			validFlag[i] &= Flag[i];
		}
		for ( i = 4; i < 8; i ++) {  // second segment
			validFlag[i] &= (uchBitField & 0x01);
			validFlag[i] &= flag6;
			validFlag[i] &= Flag[i];
		}
	}

	// Pick out valid results and store them.
	{
		U8_WMV *pVtmp4 = pV5 - iPixelDistance;
		U8_WMV *pVtmp5 = pV5;
		for ( i = 0; i < 8; i ++) {   
			if (validFlag[i]) {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp4[i] = (U8_WMV)v4[i];
				pVtmp5[i] = (U8_WMV)v5[i];	
			}
		}
	}
}
#endif  //DEBLOCK_H_NEON_OPT

#ifdef DEBLOCK_V_NEON_OPT
Void_WMV g_FilterVerticalEdgeV9_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

#define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];    
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];
	I32_WMV  Is8x8Done = uchBitField & 0x03;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterVerticalEdgeV9_armv7_C);

	if ( 0 == Is8x8Done )
		goto DEBLOCK_V_4x8;

	{ // Load 8 column src data for 8x8 lines.
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) { 
			v1[i] = pVtmp[0];
			v2[i] = pVtmp[1];
			v3[i] = pVtmp[2];
			v4[i] = pVtmp[3];
			v5[i] = pVtmp[4];
			v6[i] = pVtmp[5];
			v7[i] = pVtmp[6];
			v8[i] = pVtmp[7];
			pVtmp += iPixelDistance;
		}
	}

DEBLOCK_V_8x8:

	// Do filter.
	for ( i = 0; i < 8; i ++) {     

		I16_WMV a30;
		I16_WMV  v4_v5 = v4[i] - v5[i];
        c[i]      = abs(v4_v5)>>1;  
        a30    = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
        absA30[i] = abs(a30);
		{
			I16_WMV flag1 = v4_v5 & MASK_0x8000; //sign(v4_v5)
			I16_WMV flag2 = a30   & MASK_0x8000; //sign(a30)
			Flag[i] = (flag1 != flag2) ? 1 : 0;
		}

		{
            I16_WMV v2_v3 = v2[i] - v3[i];
            I16_WMV v6_v7 = v6[i] - v7[i];
            I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
            I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

            I16_WMV dA30;
            iMina31_a32[i] = min( abs(a31), abs(a32) ); 
            dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
            a[i] = min(dA30, c[i]);

			if (v4[i] < v5[i]) 
				a[i] = -a[i];
		}
	}


	// Calculate the flag of valid result.

	for ( i = 0; i < 8; i ++) {   
		validFlag[i] = (c[i] != 0) ? 1 : 0;
		validFlag[i] &= (absA30[i] < iStepSize);
		validFlag[i] &= (iMina31_a32[i] < absA30[i]);
	}

	{
		I16_WMV flag2 = validFlag[2];
		I16_WMV flag6 = validFlag[6];
		for ( i = 0; i < 4; i ++) {  // first segment
			validFlag[i] &= ((uchBitField & 0x02)>>1);
			validFlag[i] &= flag2; 
			validFlag[i] &= Flag[i];
		}
		for ( i = 4; i < 8; i ++) {  // second segment
			validFlag[i] &= (uchBitField & 0x01);
			validFlag[i] &= flag6;
			validFlag[i] &= Flag[i];
		}
	}

	// Pick out valid results and store them.
	{
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) {   
			if (validFlag[i]) {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp[3] = (U8_WMV)v4[i];
				pVtmp[4] = (U8_WMV)v5[i];	
			}
			pVtmp += iPixelDistance; 
		}
	}

DEBLOCK_V_4x8:

	uchBitField >>= 4;

	if ( 0 == (uchBitField & 0x03) )
		return;

	{ // Load 8 columns src data for 4x8 columns.
		U8_WMV *pVtmp = pV5 - 4;
#if 0
		for ( i = 0; i < 8; i ++) {      
			v1[i] = pVtmp[0];
			v2[i] = pVtmp[1];
			v3[i] = pVtmp[2];
			v4[i] = pVtmp[3];
			v5[i] = pVtmp[4];
			v6[i] = pVtmp[5];
			v7[i] = pVtmp[6];
			v8[i] = pVtmp[7];
			pVtmp += iPixelDistance;
		}
#else
		if ( 0 != Is8x8Done ) {
			for ( i = 0; i < 8; i ++) {      
				v5[i] = v1[i];
				v6[i] = v2[i];
				v7[i] = v3[i];
				v8[i] = v4[i];
			}
		}
		else {
			for ( i = 0; i < 8; i ++) {      
				v5[i] = pVtmp[4];
				v6[i] = pVtmp[5];
				v7[i] = pVtmp[6];
				v8[i] = pVtmp[7];
				pVtmp += iPixelDistance;
			}
		}
		pVtmp = pV5 - 4;
		for ( i = 0; i < 8; i ++) {      
			v1[i] = pVtmp[0];
			v2[i] = pVtmp[1];
			v3[i] = pVtmp[2];
			v4[i] = pVtmp[3];
			pVtmp += iPixelDistance;
		}
#endif
	}
	pV5 -= 4;
	goto DEBLOCK_V_8x8;
}

Void_WMV g_FilterVerticalEdgeV9Last4x8_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField)
{ // Simulator for Neon optimization.

#define MASK_0x8000  0x8000

    I32_WMV i;
    I16_WMV v1[8], v2[8], v3[8], v4[8], v5[8], v6[8], v7[8], v8[8];
    I16_WMV absA30[8];
    I16_WMV a[8], c[8];    
	I16_WMV iMina31_a32[8];
	I16_WMV  Flag[8],validFlag[8];
	I32_WMV  Is8x8Done = uchBitField & 0x03;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterVerticalEdgeV9Last4x8_armv7_C);


	{ // Load 8 column src data for 8x8 lines.
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) { 
			v1[i] = pVtmp[0];
			v2[i] = pVtmp[1];
			v3[i] = pVtmp[2];
			v4[i] = pVtmp[3];
			v5[i] = pVtmp[4];
			v6[i] = pVtmp[5];
			v7[i] = pVtmp[6];
			v8[i] = pVtmp[7];
			pVtmp += iPixelDistance;
		}
	}

	// Do filter.
	for ( i = 0; i < 8; i ++) {     

		I16_WMV a30;
		I16_WMV  v4_v5 = v4[i] - v5[i];
        c[i]      = abs(v4_v5)>>1;  
        a30    = (2*(v3[i] - v6[i]) - 5 * v4_v5 + 4) >> 3;
        absA30[i] = abs(a30);
		{
			I16_WMV flag1 = v4_v5 & MASK_0x8000; //sign(v4_v5)
			I16_WMV flag2 = a30   & MASK_0x8000; //sign(a30)
			Flag[i] = (flag1 != flag2) ? 1 : 0;
		}

		{
            I16_WMV v2_v3 = v2[i] - v3[i];
            I16_WMV v6_v7 = v6[i] - v7[i];
            I16_WMV a31   = (2 * (v1[i] - v4[i]) - 5 * v2_v3 + 4) >> 3;                                 
            I16_WMV a32   = (2 * (v5[i] - v8[i]) - 5 * v6_v7 + 4) >> 3;    

            I16_WMV dA30;
            iMina31_a32[i] = min( abs(a31), abs(a32) ); 
            dA30 = (5*(absA30[i] - iMina31_a32[i])) >> 3;
            a[i] = min(dA30, c[i]);

			if (v4[i] < v5[i]) 
				a[i] = -a[i];
		}
	}


	// Calculate the flag of valid result.

	for ( i = 0; i < 8; i ++) {   
		validFlag[i] = (c[i] != 0) ? 1 : 0;
		validFlag[i] &= (absA30[i] < iStepSize);
		validFlag[i] &= (iMina31_a32[i] < absA30[i]);
	}

	{
		I16_WMV flag2 = validFlag[2];
		I16_WMV flag6 = validFlag[6];
		for ( i = 0; i < 4; i ++) {  // first segment
			validFlag[i] &= ((uchBitField & 0x02)>>1);
			validFlag[i] &= flag2; 
			validFlag[i] &= Flag[i];
		}
		for ( i = 4; i < 8; i ++) {  // second segment
			validFlag[i] &= (uchBitField & 0x01);
			validFlag[i] &= flag6;
			validFlag[i] &= Flag[i];
		}
	}

	// Pick out valid results and store them.
	{
		U8_WMV *pVtmp = pV5;
		for ( i = 0; i < 8; i ++) {   
			if (validFlag[i]) {
				v4[i] -= a[i];
				v5[i] += a[i];
				pVtmp[3] = (U8_WMV)v4[i];
				pVtmp[4] = (U8_WMV)v5[i];	
			}
			pVtmp += iPixelDistance; 
		}
	}
}

#endif //DEBLOCK_V_NEON_OPT

Void_WMV g_FilterVerticalEdgeV9 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                          I32_WMV iNumPixel)
{
    U8_WMV *pVh;
    I32_WMV i, j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_FilterVerticalEdgeV9);
    
    for (j = 0; j < iNumPixel >> 2; j++)
    {
        // Filter vertical line
        pVhstart += (iPixelDistance*2);
        for (i = 0; i < 4; ++i) {
            
            
            I32_WMV v1, v2, v3, v4, v5, v6, v7, v8;       
            I32_WMV v4_v5, a30, absA30;
            I32_WMV a, c;                
 
            pVh = pVhstart;        
            
            v1 = pVh[1]; 
            v2 = pVh[2];
            v3 = pVh[3]; 
            v4 = pVh[4];
            v5 = pVh[5];
            v6 = pVh[6]; 
            v7 = pVh[7]; 
            v8 = pVh[8]; 
            
            v4_v5 = v4 - v5;
            c = v4_v5/2;
            
            if (c == 0)
            {
                if (i == 0)
                {
                    pVhstart += (2*iPixelDistance);
                    break;
                }
                else
                {
                    pVhstart += (s_iNextPixel[i]*iPixelDistance);
                    continue;
                }
            }

            a30 = (2*(v3-v6) - 5*v4_v5 + 4) >> 3;
            absA30 = abs(a30);
            if (absA30 < iStepSize) {
                
                I32_WMV v2_v3 = v2 - v3;
                I32_WMV v6_v7 = v6 - v7;
                I32_WMV a31 = (2 * (v1-v4) - 5 * v2_v3 + 4) >> 3;                                 
                I32_WMV a32 = (2 * (v5-v8) - 5 * v6_v7 + 4) >> 3;                                 
                I32_WMV iMina31_a32 = min(abs(a31),abs(a32)); 
                
                if (iMina31_a32 < absA30){

                    I32_WMV dA30;
                    if (((c & 0x80000000) ^ (a30 & 0x80000000)) != 0)
                    {
                        dA30 = (5*(absA30 - iMina31_a32)) >> 3;
                        a = min(dA30, abs(c));
                        if (v4 < v5) 
							a = -a;
                        pVh[4] = v4 - a;
                        pVh[5] = v5 + a;
                    }
                }
                else if (i == 0)
                {
                    pVhstart += (2*iPixelDistance);
                    break;
                }
            }
            else if (i == 0)
            {
                pVhstart += (2*iPixelDistance);
                break;
            }
            pVhstart += (s_iNextPixel[i]*iPixelDistance);                
        }
    }
}

#endif //WMV_OPT_LOOPFILTER_ARM
#endif //WMV9_SIMPLE_ONLY || WMV789_ONLY


#ifndef WMV9_SIMPLE_ONLY

#ifdef DEBLOCK_H_NEON_OPT
Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,I32_WMV iNumRows, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4
)
{
    I32_WMV iX, iRow;
    U8_WMV uchFlag, uchFlag8x4, *puchFilterFlag, *puchFilterFlag8x4;
    U8_WMV * pCurr;
	I32_WMV iWidthPrevYUV, iWidthYUV, iStepSize;
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockRows);

    // Filter Y boundary rows
 	iWidthPrevYUV = pWMVDec->m_iWidthPrevY;
	iWidthYUV     = pWMVDec->m_iWidthY;
	iStepSize     = pWMVDec->m_iStepSize;
    puchFilterFlag = puchFilterFlagY;
    puchFilterFlag8x4 = puchFilterFlagY8x4;
	iRow = 0;
    do  
    {
        pCurr = ppxliY;
		iX = (iWidthYUV + 31) >> 5;
		ppxliY += (iWidthPrevYUV << 3);
        do
        {
            uchFlag    = *puchFilterFlag ++;
            uchFlag8x4 = *puchFilterFlag8x4 ++;
            FilterRow_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevYUV, iStepSize);
            FilterRow_CtxA8 (pCurr + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += 32;
			iX --;
        } while ( iX != 0 );

		iRow ++;
    } while ( iRow < iNumRows );

	//Last 8x4 row of Y.
	iX = (iWidthYUV + 31) >> 5;
    pCurr = ppxliY - (iWidthPrevYUV << 2);
    do  
    {
		uchFlag8x4 = *puchFilterFlag8x4 ++;
		FilterRowLast8x4_CtxA8 (pCurr, (uchFlag8x4 >> 4), iWidthPrevYUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurr + 16, (uchFlag8x4 & 0xf), iWidthPrevYUV, iStepSize);
        pCurr += 32;
		iX --;
    } while ( iX != 0 );

    // Filter U boundary rows
 	iWidthPrevYUV = pWMVDec->m_iWidthPrevUV;
	iWidthYUV     = pWMVDec->m_iWidthUV;
    puchFilterFlag = puchFilterFlagU;
    puchFilterFlag8x4 = puchFilterFlagU8x4;
  	iRow = 0;
	if(iNumRows>1)  //zou 330,pWMVDec->m_bCodecIsWVC1
	{
		do
		{
			pCurr = ppxliU;
			iX = (iWidthYUV + 31) >> 5;
			ppxliU += (iWidthPrevYUV << 3);
			do
			{
				uchFlag = *puchFilterFlag ++;
				uchFlag8x4 = *puchFilterFlag8x4 ++;
				FilterRow_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevYUV, iStepSize);
				FilterRow_CtxA8 (pCurr + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevYUV, iStepSize);
				pCurr += 32;

				iX --;
			} while ( iX != 0 );

			iRow ++;
		} while ( iRow < (iNumRows >> 1) );
	}

	//Last 8x4 row of U.
	iX = (iWidthYUV + 31) >> 5;
    pCurr = ppxliU - (iWidthPrevYUV << 2);
    do  
    {
        uchFlag8x4 = *puchFilterFlag8x4 ++;
		FilterRowLast8x4_CtxA8 (pCurr, (uchFlag8x4 >> 4), iWidthPrevYUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurr + 16, (uchFlag8x4 & 0xf), iWidthPrevYUV, iStepSize);
        pCurr += 32;
		iX --;
    } while ( iX != 0 );

    // Filter V boundary rows
    puchFilterFlag = puchFilterFlagV;
    puchFilterFlag8x4 = puchFilterFlagV8x4;
  	iRow = 0;

  if(iNumRows>1)  //zou 330,pWMVDec->m_bCodecIsWVC1
  {
    do
    {
        pCurr = ppxliV;
		iX = (iWidthYUV + 31) >> 5;
		ppxliV += (iWidthPrevYUV << 3);
        do
        {
            uchFlag = *puchFilterFlag ++;
            uchFlag8x4 = *puchFilterFlag8x4 ++;
            FilterRow_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag8x4 >> 4), iWidthPrevYUV, iStepSize);
            FilterRow_CtxA8 (pCurr + 16, (uchFlag & 0xf), (uchFlag8x4 & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += 32;

			iX --;
        } while ( iX != 0 );
        
		iRow ++;
    } while ( iRow < (iNumRows >> 1) );
  }

	//Last 8x4 row of V.
	iX = (iWidthYUV + 31) >> 5;
    pCurr = ppxliV - (iWidthPrevYUV << 2);
    do  
    {
        uchFlag8x4 = *puchFilterFlag8x4 ++;
		FilterRowLast8x4_CtxA8 (pCurr, (uchFlag8x4 >> 4), iWidthPrevYUV, iStepSize);
		FilterRowLast8x4_CtxA8 (pCurr + 16, (uchFlag8x4 & 0xf), iWidthPrevYUV, iStepSize);
        pCurr += 32;
		iX --;
    } while ( iX != 0 );

}
#else
Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumRows, U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV
)
{
    I32_WMV iX, iRow;
    U8_WMV uchFlag, *puchFilterFlag;
    U8_WMV * pCurr;
	I32_WMV iWidthPrevYUV, iWidthYUV, iStepSize;
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockRows);

    // Filter Y boundary rows
 	iWidthPrevYUV = pWMVDec->m_iWidthPrevY;
	iWidthYUV     = pWMVDec->m_iWidthY;
	iStepSize     = pWMVDec->m_iStepSize;
    puchFilterFlag = puchFilterFlagY;
	iRow = 0;
    do  
    {
        pCurr = ppxliY + iRow * 8 * iWidthPrevYUV;
		iX = (iWidthYUV + 31) >> 5;
        do
        {
            uchFlag = *puchFilterFlag ++;
            FilterRow (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterRow (pWMVDec, pCurr + 16, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += 32;
			iX --;
        } while ( iX != 0 );

		iRow ++;
    } while ( iRow < iNumRows );

    // Filter U boundary rows
 	iWidthPrevYUV = pWMVDec->m_iWidthPrevUV;
	iWidthYUV     = pWMVDec->m_iWidthUV;
    puchFilterFlag = puchFilterFlagU;
  	iRow = 0;
	if(iNumRows>1)  //zou 330,pWMVDec->m_bCodecIsWVC1
	{
		do
		{
			pCurr = ppxliU + iRow * 8 * iWidthPrevYUV;
			iX = (iWidthYUV + 31) >> 5;
			do
			{
				uchFlag = *puchFilterFlag ++;
				FilterRow (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
				FilterRow (pWMVDec, pCurr + 16, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
				pCurr += 32;

				iX --;
			} while ( iX != 0 );

			iRow ++;
		} while ( iRow < (iNumRows >> 1) );
	}

    // Filter V boundary rows
    puchFilterFlag = puchFilterFlagV;
  	iRow = 0;
	if(iNumRows>1)  //zou 330,pWMVDec->m_bCodecIsWVC1
	{
		do
		{
			pCurr = ppxliV + iRow * 8 * iWidthPrevYUV;
			iX = (iWidthYUV + 31) >> 5;
			do
			{
				uchFlag = *puchFilterFlag ++;
				FilterRow (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
				FilterRow (pWMVDec, pCurr + 16, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
				pCurr += 32;

				iX --;
			} while ( iX != 0 );
	        
			iRow ++;
		} while ( iRow < (iNumRows >> 1) );
	}

}
#endif	//DEBLOCK_H_NEON_OPT

#define DEBLOCK_V_NEON_OPT_2_TEST


#ifdef DEBLOCK_V_NEON_OPT
#ifdef DEBLOCK_V_NEON_OPT_2_TEST

Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV
)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag, uchFlag4x8, *puchFilterFlag4x8;
    U8_WMV* pCurr;
	I32_WMV iWidthPrevYUV, iStepSize;
	I32_WMV iRow;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockColumns);

    ppxliY -= 4;
    ppxliU -= 4;
    ppxliV -= 4;

    // Filter Y boundary columns
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevY;
	iStepSize      = pWMVDec->m_iStepSize;
	iY = (iHeight + 31) >> 5;
	iRow = 0;
    do {
		puchFilterFlag    = puchFilterFlagY;
		puchFilterFlag4x8 = puchFilter4x8ColFlagY;
        pCurr = ppxliY;
		ppxliY += (iWidthPrevYUV << 4);
		iCol = 0;
        do {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
            FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
		    puchFilterFlag4x8 += iY;
		    puchFilterFlag    += iY;
            pCurr += 8;
			iCol ++;
        } while (iCol < iNumCols);

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevYUV, iStepSize);

		puchFilterFlag    = puchFilterFlagY;
		puchFilterFlag4x8 = puchFilter4x8ColFlagY;
        pCurr = ppxliY;
		ppxliY += (iWidthPrevYUV << 4);
		iCol = 0;
        do {
            uchFlag    = *puchFilterFlag;
            uchFlag4x8 = *puchFilterFlag4x8;
            FilterColumn_CtxA8 (pCurr, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevYUV, iStepSize);
		    puchFilterFlag4x8 += iY;
		    puchFilterFlag    += iY;
            pCurr += 8;
			iCol ++;
        } while (iCol < iNumCols);

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 & 0x0f), iWidthPrevYUV, iStepSize);

		puchFilterFlagY       ++;
		puchFilter4x8ColFlagY ++;
		iRow ++;
    } while (iRow < iY);


    // Filter U boundary columns
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevUV;
	iY = ((iHeight >> 1) + 31) >> 5;
	iRow = 0;
    do {
		puchFilterFlag = puchFilterFlagU;
		puchFilterFlag4x8 = puchFilter4x8ColFlagU;
        pCurr = ppxliU;
		ppxliU += (iWidthPrevYUV << 4);
		iCol = 0;
		if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
		{
			do {
				uchFlag    = *puchFilterFlag;
				uchFlag4x8 = *puchFilterFlag4x8;
				FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
				puchFilterFlag4x8 += iY;
				puchFilterFlag    += iY;
				pCurr += 8;
				iCol ++;
			} while (iCol < (iNumCols >> 1));
		}

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevYUV, iStepSize);

		puchFilterFlag    = puchFilterFlagU;
		puchFilterFlag4x8 = puchFilter4x8ColFlagU;
        pCurr = ppxliU;
		ppxliU += (iWidthPrevYUV << 4);
		iCol = 0;
		if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
		{
			do {
				uchFlag    = *puchFilterFlag;
				uchFlag4x8 = *puchFilterFlag4x8;
				FilterColumn_CtxA8 (pCurr, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevYUV, iStepSize);
				puchFilterFlag4x8 += iY;
				puchFilterFlag    += iY;
				pCurr += 8;
				iCol ++;
			} while (iCol < (iNumCols >> 1));
		}

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 & 0x0f), iWidthPrevYUV, iStepSize);

		puchFilterFlagU ++;
		puchFilter4x8ColFlagU ++;
		iRow ++;
    } while (iRow < iY);


    // Filter V boundary columns
	iRow = 0;
    do {
		puchFilterFlag = puchFilterFlagV;
		puchFilterFlag4x8 = puchFilter4x8ColFlagV;
        pCurr = ppxliV;
		ppxliV += (iWidthPrevYUV << 4);
		iCol = 0;
		if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
		{
			do {
				uchFlag    = *puchFilterFlag;
				uchFlag4x8 = *puchFilterFlag4x8;
				FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
				puchFilterFlag4x8 += iY;
				puchFilterFlag    += iY;
				pCurr += 8;
				iCol ++;
			} while (iCol < (iNumCols >> 1));
		}

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8) >> 4, iWidthPrevYUV, iStepSize);

		puchFilterFlag    = puchFilterFlagV;
		puchFilterFlag4x8 = puchFilter4x8ColFlagV;
        pCurr = ppxliV;
		ppxliV += (iWidthPrevYUV << 4);
		iCol = 0;
		if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
		{
			do {
				uchFlag    = *puchFilterFlag;
				uchFlag4x8 = *puchFilterFlag4x8;
				FilterColumn_CtxA8 (pCurr, (uchFlag & 0x0f), (uchFlag4x8 & 0x0f), iWidthPrevYUV, iStepSize);
				puchFilterFlag4x8 += iY;
				puchFilterFlag    += iY;
				pCurr += 8;
				iCol ++;
			} while (iCol < (iNumCols >> 1));
		}

		pCurr -= 4;
        uchFlag4x8 = *puchFilterFlag4x8;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 & 0x0f), iWidthPrevYUV, iStepSize);

		puchFilterFlagV ++;
		puchFilter4x8ColFlagV ++;
		iRow ++;
    } while (iRow < iY);

}

#else //DEBLOCK_V_NEON_OPT_2_TEST

Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV
)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag, uchFlag4x8, *puchFilterFlag4x8;
    U8_WMV* pCurr;
	I32_WMV iWidthPrevYUV, iStepSize;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockColumns);

    ppxliY -= 4;
    ppxliU -= 4;
    ppxliV -= 4;

    // Filter Y boundary columns
    puchFilterFlag4x8 = puchFilter4x8ColFlagY;
    puchFilterFlag = puchFilterFlagY;
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevY;
	iStepSize      = pWMVDec->m_iStepSize;
	iCol = 0;
    do {
        pCurr = ppxliY + iCol * 8;
		iY = (iHeight + 31) >> 5;
        do {
            uchFlag    = *puchFilterFlag ++;
            uchFlag4x8 = *puchFilterFlag4x8 ++;
            FilterColumn_CtxA8 (pCurr,(uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn_CtxA8 (pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), (uchFlag4x8 & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);
			iY --;
        } while (iY != 0);

		iCol ++;
    } while (iCol < iNumCols);

	//Last 4x8 col of Y.
	iY = (iHeight + 31) >> 5;
    pCurr = ppxliY + iCol * 8 - 4;
    do  
    {
        uchFlag4x8 = *puchFilterFlag4x8 ++;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
		FilterColumnLast4x8_CtxA8 (pCurr + 16 * iWidthPrevYUV, (uchFlag4x8 & 0xf), iWidthPrevYUV, iStepSize);
        pCurr += (32 * iWidthPrevYUV);
		iY --;
    } while ( iY != 0 );

    // Filter U boundary columns
    puchFilterFlag = puchFilterFlagU;
    puchFilterFlag4x8 = puchFilter4x8ColFlagU;
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevUV;
	iCol = 0; 
    do {
        pCurr = ppxliU + iCol * 8;
		iY = ((iHeight >> 1) + 31) >> 5;
        do {
            uchFlag = *puchFilterFlag ++;
            uchFlag4x8 = *puchFilterFlag4x8 ++;
            FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn_CtxA8 (pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), (uchFlag4x8 & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);
			iY --;
        } while (iY != 0);

		iCol ++;
    } while (iCol < (iNumCols >> 1));

	//Last 4x8 col of U.
	iY = ((iHeight >> 1) + 31) >> 5;
    pCurr = ppxliU + iCol * 8 - 4;
    do  
    {
        uchFlag4x8 = *puchFilterFlag4x8 ++;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
		FilterColumnLast4x8_CtxA8 (pCurr + 16 * iWidthPrevYUV, (uchFlag4x8 & 0xf), iWidthPrevYUV, iStepSize);
        pCurr += (32 * iWidthPrevYUV);
		iY --;
    } while ( iY != 0 );

    // Filter V boundary columns
    puchFilterFlag = puchFilterFlagV;
    puchFilterFlag4x8 = puchFilter4x8ColFlagV;
	iCol = 0; 
    do {
        pCurr = ppxliV + iCol * 8;
		iY = ((iHeight >> 1) + 31) >> 5;
        do {
            uchFlag = *puchFilterFlag ++;
            uchFlag4x8 = *puchFilterFlag4x8 ++;
            FilterColumn_CtxA8 (pCurr, (uchFlag >> 4), (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn_CtxA8 (pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), (uchFlag4x8 & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);
			iY --;
        } while (iY != 0);

		iCol ++;
    } while (iCol < (iNumCols >> 1));

	//Last 4x8 col of V.
	iY = ((iHeight >> 1) + 31) >> 5;
    pCurr = ppxliV + iCol * 8 - 4;
    do  
    {
        uchFlag4x8 = *puchFilterFlag4x8 ++;
		FilterColumnLast4x8_CtxA8 (pCurr, (uchFlag4x8 >> 4), iWidthPrevYUV, iStepSize);
		FilterColumnLast4x8_CtxA8 (pCurr + 16 * iWidthPrevYUV, (uchFlag4x8 & 0xf), iWidthPrevYUV, iStepSize);
        pCurr += (32 * iWidthPrevYUV);
		iY --;
    } while ( iY != 0 );

}


#endif //DEBLOCK_V_NEON_OPT_2_TEST

#else //DEBLOCK_V_NEON_OPT

Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV
)
{
    I32_WMV iY, iCol;
    U8_WMV uchFlag, *puchFilterFlag;
    U8_WMV* pCurr;
	I32_WMV iWidthPrevYUV, iStepSize;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockColumns);

    ppxliY -= 5;
    ppxliU -= 5;
    ppxliV -= 5;

    // Filter Y boundary columns
    puchFilterFlag = puchFilterFlagY;
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevY;
	iStepSize      = pWMVDec->m_iStepSize;
	iCol = 0;
    do {
        pCurr = ppxliY + iCol * 8;
		iY = (iHeight + 31) >> 5;
        do {
            uchFlag = *puchFilterFlag ++;
            FilterColumn (pWMVDec, pCurr,(uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn (pWMVDec, pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);

			iY --;
        } while (iY != 0);

		iCol ++;
    } while (iCol < iNumCols);

    // Filter U boundary columns
    puchFilterFlag = puchFilterFlagU;
	iWidthPrevYUV  = pWMVDec->m_iWidthPrevUV;
	iCol = 0; 
    do {
        pCurr = ppxliU + iCol * 8;
		iY = ((iHeight >> 1) + 31) >> 5;
	if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
	{
        do {
            uchFlag = *puchFilterFlag ++;
            FilterColumn (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn (pWMVDec, pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);
            
			iY --;
        } while (iY != 0);
	}

		iCol ++;
    } while (iCol < (iNumCols >> 1));

    // Filter V boundary columns
    puchFilterFlag = puchFilterFlagV;
	iCol = 0; 
    do {
        pCurr = ppxliV + iCol * 8;
		iY = ((iHeight >> 1) + 31) >> 5;
	if((iNumCols >> 1)>=1) //zou 401,pWMVDec->m_bCodecIsWVC1
	{
        do {
            uchFlag = *puchFilterFlag ++;
            FilterColumn (pWMVDec, pCurr, (uchFlag >> 4), iWidthPrevYUV, iStepSize);
            FilterColumn (pWMVDec, pCurr + 16 * iWidthPrevYUV, (uchFlag & 0xf), iWidthPrevYUV, iStepSize);
            pCurr += (32 * iWidthPrevYUV);
            
			iY --;
        } while (iY != 0);
	}

		iCol ++;
    } while (iCol < (iNumCols >> 1));
}
#endif	//DEBLOCK_V_NEON_OPT


#ifdef DEBLOCK_H_NEON_OPT
Void_WMV FilterRow_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize)
{
	U8_WMV BitField;
    assert (!(uchBitField & 0xf0));
    assert (!(uchBitField8x4 & 0xf0));

	//First 8 pixels.
	BitField = (uchBitField>>2) | ((uchBitField8x4>>2) << 4);
	if (BitField & 0x33) {
		g_FilterHorizontalEdgeV9_Fun(pRow, iPitch, iStepSize, BitField);
	}

	//Next 8 pixels.
	BitField = uchBitField | (uchBitField8x4 << 4);
	if (BitField & 0x33) {
		g_FilterHorizontalEdgeV9_Fun(pRow + 8, iPitch, iStepSize, BitField);
	}
}

Void_WMV FilterRowLast8x4_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize)
{
    assert (!(uchBitField8x4 & 0xf0));

	//First 8 pixels.
	if (uchBitField8x4 & 0x0C) {
		g_FilterHorizontalEdgeV9Last8x4_Fun(pRow, iPitch, iStepSize, uchBitField8x4 >> 2);
	}

	//Next 8 pixels.
	if (uchBitField8x4 & 0x03) {
		g_FilterHorizontalEdgeV9Last8x4_Fun(pRow + 8, iPitch, iStepSize, uchBitField8x4);
	}
}

#else

Void_WMV FilterRow (tWMVDecInternalMember *pWMVDec, U8_WMV* pRow, U8_WMV uchBitField, I32_WMV iPitch, I32_WMV iStepSize)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterRow);
    assert (!(uchBitField & 0xf0));
    switch (uchBitField)
    {
        case 0:
            break;
        case 1:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 2:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 4);
            break;
        case 3:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 8);
            break;
        case 4:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 4);
            break;
        case 5:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 6:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 8);
            break;
        case 7:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 4, iPitch, iStepSize, 12);
            break;
        case 8:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            break;
        case 9:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 10:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 4);
            break;
        case 11:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 8, iPitch, iStepSize, 8);
            break;
        case 12:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 8);
            break;
        case 13:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 8);
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow + 12, iPitch, iStepSize, 4);
            break;
        case 14:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 12);
            break;
        case 15:
            (*pWMVDec->m_pFilterHorizontalEdge)(pRow, iPitch, iStepSize, 16);
            break;
    }
}

#endif	//DEBLOCK_H_NEON_OPT

#ifdef DEBLOCK_V_NEON_OPT
Void_WMV FilterColumn_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize)
{
	U8_WMV BitField;
    assert (!(uchBitField & 0xf0));
    assert (!(uchBitField4x8 & 0xf0));

	//First 8 pixels.
	BitField = (uchBitField>>2) | ((uchBitField4x8>>2) << 4);
	if (BitField & 0x33) {
		g_FilterVerticalEdgeV9_Fun(pCol, iPitch, iStepSize, BitField);
	}

	//Next 8 pixels.
	BitField = uchBitField | (uchBitField4x8 << 4);
	if (BitField & 0x33) {
		g_FilterVerticalEdgeV9_Fun(pCol + (iPitch<<3), iPitch, iStepSize, BitField);
	}
}


Void_WMV FilterColumnLast4x8_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize)
{
    assert (!(uchBitField4x8 & 0xf0));

	//First 8 pixels.
	if (uchBitField4x8 & 0x0C) {
		g_FilterVerticalEdgeV9Last4x8_Fun(pCol, iPitch, iStepSize, uchBitField4x8 >> 2);
	}

	//Next 8 pixels.
	if (uchBitField4x8 & 0x03) {
		g_FilterVerticalEdgeV9Last4x8_Fun(pCol + (iPitch<<3), iPitch, iStepSize, uchBitField4x8);
	}
}

#else

Void_WMV FilterColumn (tWMVDecInternalMember *pWMVDec, U8_WMV* pCol, U8_WMV uchBitField, I32_WMV iPitch, I32_WMV iStepSize)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(FilterColumn);
    assert (!(uchBitField & 0xf0));
    switch (uchBitField)
    {
        case 0:
            break;
        case 1:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 2:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 4);
            break;
        case 3:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 8);
            break;
        case 4:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 4);
            break;
        case 5:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 6:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 8);
            break;
        case 7:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 4 * iPitch, iPitch, iStepSize, 12);
            break;
        case 8:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            break;
        case 9:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 10:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 4);
            break;
        case 11:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 4);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 8 * iPitch, iPitch, iStepSize, 8);
            break;
        case 12:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 8);
            break;
        case 13:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 8);
            (*pWMVDec->m_pFilterVerticalEdge)(pCol + 12 * iPitch, iPitch, iStepSize, 4);
            break;
        case 14:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 12);
            break;
        case 15:
            (*pWMVDec->m_pFilterVerticalEdge)(pCol, iPitch, iStepSize, 16);
            break;
    }
}
#endif	//DEBLOCK_V_NEON_OPT

#endif // WMV9_SIMPLE_ONLY 

#ifndef WMV9_SIMPLE_ONLY
Void_WMV DeblockSLFrame_V9 (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliCurrY, U8_WMV* ppxliCurrU, U8_WMV* ppxliCurrV,
    U8_WMV* puchFilter8x8RowFlagY, U8_WMV* puchFilter8x8RowFlagU, U8_WMV* puchFilter8x8RowFlagV,
    U8_WMV* puchFilter8x4RowFlagY, U8_WMV* puchFilter8x4RowFlagU, U8_WMV* puchFilter8x4RowFlagV,
    U8_WMV* puchFilter8x8ColFlagY, U8_WMV* puchFilter8x8ColFlagU, U8_WMV* puchFilter8x8ColFlagV,
    U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV,
    LOOPF_FLAG* pLoopFilterFlag, U32_WMV uiMBStart, U32_WMV uiMBEnd)
{
    I32_WMV i8x8OffsetY = 0;
    I32_WMV i8x8OffsetUV = 0;
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockSLFrame_V9);


    if (pWMVDec->m_bSliceWMVA || pWMVDec->m_bAdvancedProfile) { //zou fix,pWMVDec->m_bCodecIsWVC1
        I32_WMV  i8OffsetY	    = 0;
        I32_WMV  i8OffsetUV	    = 0;    

        //U32_WMV iMBX_2			= (pWMVDec->m_uintNumMBX + 1) >> 1;
        //U32_WMV iMBX			= iMBX_2 * 2;
        //U32_WMV iMBXUV			= (pWMVDec->m_uintNumMBX + 3) >> 2;
        //U32_WMV iMBY_2			= (pWMVDec->m_uintNumMBY + 1) >> 1;
        //U32_WMV iMBY			= iMBY_2 * 2;
        //U32_WMV iMBYUV			= (pWMVDec->m_uintNumMBY + 3) >> 2;
        U32_WMV iSliceHead		= uiMBStart;
        U32_WMV iSliceTail		= iSliceHead;
        U32_WMV iSliceDiff;

        if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1)
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig + pWMVDec->m_uintNumMBY;        
        else
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;

        
        if (iSliceTail < uiMBEnd)
            iSliceTail ++;
        while ((iSliceTail < uiMBEnd) && !pWMVDec->m_pbStartOfSliceRow[iSliceTail])
            iSliceTail ++;

        iSliceDiff		= iSliceTail - iSliceHead;

        while (iSliceDiff > 0) {
            if (iSliceHead == 0 || pWMVDec->m_pbStartOfSliceRow[iSliceHead]) {            
                i8OffsetY		= 8 * pWMVDec->m_iWidthPrevY;
                i8OffsetUV		= 8 * pWMVDec->m_iWidthPrevUV;
            } else {
                i8OffsetY		= 0;
                i8OffsetUV		= 0;
            }

            GenerateRowLoopFilterFlags (pWMVDec,
                puchFilter8x8RowFlagY, puchFilter8x8RowFlagU, puchFilter8x8RowFlagV,
                puchFilter8x4RowFlagY, puchFilter8x4RowFlagU, puchFilter8x4RowFlagV,
                pLoopFilterFlag, 0, iSliceDiff, (iSliceHead == 0) || pWMVDec->m_pbStartOfSliceRow[iSliceHead]);

            GenerateColumnLoopFilterFlags (pWMVDec,
                puchFilter8x8ColFlagY, puchFilter8x8ColFlagU, puchFilter8x8ColFlagV,
                puchFilter4x8ColFlagY, puchFilter4x8ColFlagU, puchFilter4x8ColFlagV,
                pLoopFilterFlag, iSliceDiff);

#ifdef DEBLOCK_H_NEON_OPT
            DeblockRows(pWMVDec,
                ppxliCurrY + i8OffsetY, 
                ppxliCurrU + i8OffsetUV, 
                ppxliCurrV + i8OffsetUV, 
                iSliceDiff * 2 - 1, 
                puchFilter8x8RowFlagY, 
                puchFilter8x8RowFlagU, 
				puchFilter8x8RowFlagV,
				puchFilter8x4RowFlagY, 
				puchFilter8x4RowFlagU, 
				puchFilter8x4RowFlagV);
#else
            {
            I32_WMV  i4OffsetY		= 4 * pWMVDec->m_iWidthPrevY;
            I32_WMV  i4OffsetUV		= 4 * pWMVDec->m_iWidthPrevUV;
            DeblockRows(pWMVDec,
                ppxliCurrY + i8OffsetY, 
                ppxliCurrU + i8OffsetUV, 
                ppxliCurrV + i8OffsetUV, 
                iSliceDiff * 2 - ((iSliceHead == 0) || pWMVDec->m_pbStartOfSliceRow[iSliceHead]), 
                puchFilter8x8RowFlagY, 
                puchFilter8x8RowFlagU, 
                puchFilter8x8RowFlagV);

            DeblockRows(pWMVDec,
                ppxliCurrY + i4OffsetY, 
                ppxliCurrU + i4OffsetUV, 
                ppxliCurrV + i4OffsetUV, 
                iSliceDiff * 2, 
                puchFilter8x4RowFlagY, 
                puchFilter8x4RowFlagU, 
                puchFilter8x4RowFlagV);
            }
#endif
#ifdef DEBLOCK_V_NEON_OPT
			DeblockColumns(pWMVDec,
				ppxliCurrY + 8, 
				ppxliCurrU + 8, 
				ppxliCurrV + 8,
				pWMVDec->m_uintNumMBX * 2 - 1, 
				iSliceDiff << 4, 
				puchFilter8x8ColFlagY, 
				puchFilter8x8ColFlagU, 
				puchFilter8x8ColFlagV,
				puchFilter4x8ColFlagY, 
				puchFilter4x8ColFlagU, 
				puchFilter4x8ColFlagV);
#else

            DeblockColumns(pWMVDec,
                ppxliCurrY + 8, 
                ppxliCurrU + 8, 
                ppxliCurrV + 8,
                pWMVDec->m_uintNumMBX * 2 - 1, 
                iSliceDiff << 4, 
                puchFilter8x8ColFlagY, 
                puchFilter8x8ColFlagU, 
                puchFilter8x8ColFlagV);

            DeblockColumns(pWMVDec,
                ppxliCurrY + 4, 
                ppxliCurrU + 4, 
                ppxliCurrV + 4,
                pWMVDec->m_uintNumMBX * 2, 
                iSliceDiff << 4, 
                puchFilter4x8ColFlagY, 
                puchFilter4x8ColFlagU, 
                puchFilter4x8ColFlagV);
#endif
            ppxliCurrY		+= iSliceDiff * pWMVDec->m_iWidthPrevY  * 16;
            ppxliCurrU		+= iSliceDiff * pWMVDec->m_iWidthPrevUV * 8 ;
            ppxliCurrV		+= iSliceDiff * pWMVDec->m_iWidthPrevUV * 8;

            pLoopFilterFlag	+= iSliceDiff * pWMVDec->m_uintNumMBX;

            iSliceHead		 = iSliceTail;
            if (iSliceTail < uiMBEnd)
                iSliceTail ++;
            while ((iSliceTail < uiMBEnd) && !pWMVDec->m_pbStartOfSliceRow[iSliceTail])
                iSliceTail ++;

            iSliceDiff		 = iSliceTail - iSliceHead;
        }

    } else {

        if (uiMBStart == 0)
        {
            i8x8OffsetY = 8 * pWMVDec->m_iWidthPrevY;
            i8x8OffsetUV = 8 * pWMVDec->m_iWidthPrevUV;
        
        }

        GenerateRowLoopFilterFlags (pWMVDec,
            puchFilter8x8RowFlagY, puchFilter8x8RowFlagU, puchFilter8x8RowFlagV,
            puchFilter8x4RowFlagY, puchFilter8x4RowFlagU, puchFilter8x4RowFlagV,
            pLoopFilterFlag, uiMBStart, uiMBEnd, uiMBStart == 0);


        GenerateColumnLoopFilterFlags (pWMVDec,
            puchFilter8x8ColFlagY, puchFilter8x8ColFlagU, puchFilter8x8ColFlagV,
            puchFilter4x8ColFlagY, puchFilter4x8ColFlagU, puchFilter4x8ColFlagV,
            pLoopFilterFlag, uiMBEnd - uiMBStart);

#ifdef DEBLOCK_H_NEON_OPT
        DeblockRows(pWMVDec,
            ppxliCurrY + i8x8OffsetY, 
            ppxliCurrU + i8x8OffsetUV, 
            ppxliCurrV + i8x8OffsetUV, 
            (uiMBEnd - uiMBStart) * 2 - 1, 
            puchFilter8x8RowFlagY, 
            puchFilter8x8RowFlagU, 
            puchFilter8x8RowFlagV,
            puchFilter8x4RowFlagY, 
            puchFilter8x4RowFlagU, 
            puchFilter8x4RowFlagV);
#else
        DeblockRows(pWMVDec,
            ppxliCurrY + i8x8OffsetY, 
            ppxliCurrU + i8x8OffsetUV, 
            ppxliCurrV + i8x8OffsetUV, 
            (uiMBEnd - uiMBStart) * 2 - (uiMBStart == 0), 
            puchFilter8x8RowFlagY, 
            puchFilter8x8RowFlagU, 
            puchFilter8x8RowFlagV);

        DeblockRows(pWMVDec,
            ppxliCurrY + (4 * pWMVDec->m_iWidthPrevY), 
            ppxliCurrU + (4 * pWMVDec->m_iWidthPrevUV), 
            ppxliCurrV + (4 * pWMVDec->m_iWidthPrevUV),
                
            (uiMBEnd - uiMBStart) * 2, 
            puchFilter8x4RowFlagY, 
            puchFilter8x4RowFlagU, 
            puchFilter8x4RowFlagV);
#endif

#ifdef DEBLOCK_V_NEON_OPT
        DeblockColumns(pWMVDec,
            ppxliCurrY + 8, 
            ppxliCurrU + 8, 
            ppxliCurrV + 8,
            pWMVDec->m_uintNumMBX * 2 - 1, 
            (uiMBEnd - uiMBStart) << 4, 
            puchFilter8x8ColFlagY, 
            puchFilter8x8ColFlagU, 
            puchFilter8x8ColFlagV,
            puchFilter4x8ColFlagY, 
            puchFilter4x8ColFlagU, 
            puchFilter4x8ColFlagV);
#else
        DeblockColumns(pWMVDec,
            ppxliCurrY + 8, 
            ppxliCurrU + 8, 
            ppxliCurrV + 8,
            pWMVDec->m_uintNumMBX * 2 - 1, 
            (uiMBEnd - uiMBStart) << 4, 
            puchFilter8x8ColFlagY, 
            puchFilter8x8ColFlagU, 
            puchFilter8x8ColFlagV);

        DeblockColumns(pWMVDec,
            ppxliCurrY + 4, 
            ppxliCurrU + 4, 
            ppxliCurrV + 4,
            pWMVDec->m_uintNumMBX * 2, 
            (uiMBEnd - uiMBStart) << 4, 
            puchFilter4x8ColFlagY, 
            puchFilter4x8ColFlagU, 
            puchFilter4x8ColFlagV);
#endif
    }
}

static I32_WMV LOOPFILTERFLAGS_WMV9[17][16] =
{
 {0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff},
 {0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xff,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff},
 {0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff},
 {0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff},
 {0x0f,0x5f,0x9f,0xdf,0x6f,0x7f,0xff,0xff,0xaf,0xff,0xbf,0xff,0xef,0xff,0xff,0xff}
};

static I32_WMV LEFTFLAGS[5][16] =
{{0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff}
};
static I32_WMV LEFTPREDFLAGS[5][16] =
{{0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xff,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
{0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff}
};
static I32_WMV TOPFLAGS[5][16] =
{{0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff}
};
static I32_WMV TOPPREDFLAGS[5][16] =
{{0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff}
};
static I32_WMV MIDDLEFLAGS[16] =
 {0x0f,0x5f,0x9f,0xdf,0x6f,0x7f,0xff,0xff,0xaf,0xff,0xbf,0xff,0xef,0xff,0xff,0xff};


#ifndef _WMV9AP_
Void_WMV ComputeMBLoopFilterFlags (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft)
{
    I32_WMV iBlk0 = iMBY * 4 * pWMVDec->m_uintNumMBX + iMBX * 2;
    I32_WMV iBlk2 = iBlk0 + 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iBlkUp = iBlk0 - 2 * pWMVDec->m_uintNumMBX;
    I8_WMV* pchLoopFilterFlag = pWMVDec->m_pLoopFilterFlags[iMBY*pWMVDec->m_uintNumMBX + iMBX].chFlag;
    
    I32_WMV iBlk;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeMBLoopFilterFlags);

    // in the interests of time and complexity, sub-block Xforms in B frames 
    // aren't internally loop filtered - besides "loop filtering" isn't really
    // necessary - postfiltering with equal visual quality can take care of it
    if (pWMVDec->m_tFrmType == BVOP) {
        for (iBlk = 0; iBlk < 6; iBlk++)
            pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
    } 
    else

    for (iBlk = 0; iBlk < 6; iBlk++) 
    {
        if (pBlockXformMode[iBlk] == XFORMMODE_8x8)
            pchLoopFilterFlag[iBlk] = 0x0f;
        else if (pBlockXformMode[iBlk] == XFORMMODE_8x4)
            pchLoopFilterFlag[iBlk] = (I8_WMV)0xcf;
        else if (pBlockXformMode[iBlk] == XFORMMODE_4x8)
            pchLoopFilterFlag[iBlk] = 0x3f;
        else if (pBlockXformMode[iBlk] == XFORMMODE_4x4)
            pchLoopFilterFlag[iBlk] = (I8_WMV)LOOPFILTERFLAGS_WMV9[16][pCBPCurr[iBlk]];


    }

    if (bTop && bLeft) 
    {  // top left
        pchLoopFilterFlag[0] &= 0xf0;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xf0;
        pchLoopFilterFlag[5] &= 0xf0;
    }
    else if (bTop) 
    {  // top row
        pchLoopFilterFlag[0] &= 0xf3;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[4] &= 0xf3;
        pchLoopFilterFlag[5] &= 0xf3;
    }
    else if (bLeft) 
    {  // left col
        
        pchLoopFilterFlag[pWMVDec->m_iBetaRTMMismatchIndex] &= 0xfc;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xfc;
        pchLoopFilterFlag[5] &= 0xfc;
    }


    if (pWMVDec->m_tFrmType  == BVOP)
        return;

    if (chMBMode != MB_1MV || pWMVDec->m_pXMotion[0] != IBLOCKMV)
    {
        I32_WMV iBlkC;
       // block 0 rules
        if (!bTop && (pWMVDec->m_pXMotion[iBlk0] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlkUp] == pWMVDec->m_pXMotion[iBlk0]) && 
            (pWMVDec->m_pYMotion[iBlkUp] == pWMVDec->m_pYMotion[iBlk0]))
            pchLoopFilterFlag[0] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+8][pCBPCurr[0]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[2]+12][pCBPTop[2]]);

        if (!bLeft && (pWMVDec->m_pXMotion[iBlk0] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0 - 1] == pWMVDec->m_pXMotion[iBlk0]) && 
            (pWMVDec->m_pYMotion[iBlk0 - 1] == pWMVDec->m_pYMotion[iBlk0]))
            pchLoopFilterFlag[0] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]][pCBPCurr[0]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[1]+4][pCBPLeft[1]]);
              
        // block 1 rules
        if (!bTop && (pWMVDec->m_pXMotion[iBlk0 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlkUp + 1] == pWMVDec->m_pXMotion[iBlk0 + 1]) && 
            (pWMVDec->m_pYMotion[iBlkUp + 1] == pWMVDec->m_pYMotion[iBlk0 + 1]))
            pchLoopFilterFlag[1] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+8][pCBPCurr[1]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[3]+12][pCBPTop[3]]);

        if ((pWMVDec->m_pXMotion[iBlk0 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0] == pWMVDec->m_pXMotion[iBlk0 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk0] == pWMVDec->m_pYMotion[iBlk0 + 1]))
            pchLoopFilterFlag[1] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]][pCBPCurr[1]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+4][pCBPCurr[0]]);        

        // block 2 rules
        if ((pWMVDec->m_pXMotion[iBlk2] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0] == pWMVDec->m_pXMotion[iBlk2]) && 
            (pWMVDec->m_pYMotion[iBlk0] == pWMVDec->m_pYMotion[iBlk2]))
            pchLoopFilterFlag[2] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]+8][pCBPCurr[2]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+12][pCBPCurr[0]]);

        if (!bLeft && (pWMVDec->m_pXMotion[iBlk2] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk2 - 1] == pWMVDec->m_pXMotion[iBlk2]) && 
            (pWMVDec->m_pYMotion[iBlk2 - 1] == pWMVDec->m_pYMotion[iBlk2]))
            pchLoopFilterFlag[2] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]][pCBPCurr[2]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[3]+4][pCBPLeft[3]]);

        // block 3 rules
        if ((pWMVDec->m_pXMotion[iBlk2 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0 + 1] == pWMVDec->m_pXMotion[iBlk2 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk0 + 1] == pWMVDec->m_pYMotion[iBlk2 + 1]))
            pchLoopFilterFlag[3] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]+8][pCBPCurr[3]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+12][pCBPCurr[1]]);

        if ((pWMVDec->m_pXMotion[iBlk2 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk2] == pWMVDec->m_pXMotion[iBlk2 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk2] == pWMVDec->m_pYMotion[iBlk2 + 1]))
            pchLoopFilterFlag[3] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]][pCBPCurr[3]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+4][pCBPCurr[1]]);

        // block 4 & 5 rules
        iBlkC = iMBY * pWMVDec->m_uintNumMBX + iMBX;
        if (!bTop && (pWMVDec->m_pXMotionC[iBlkC] != IBLOCKMV) && 
            (pWMVDec->m_pXMotionC[iBlkC - pWMVDec->m_uintNumMBX] == pWMVDec->m_pXMotionC[iBlkC]) && 
            (pWMVDec->m_pYMotionC[iBlkC - pWMVDec->m_uintNumMBX] == pWMVDec->m_pYMotionC[iBlkC])) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]+8][pCBPCurr[4]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[4]+12][pCBPTop[4]]);
            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]+8][pCBPCurr[5]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[5]+12][pCBPTop[5]]);
        }

        if (!bLeft && (pWMVDec->m_pXMotionC[iBlkC] != IBLOCKMV) && 
            (pWMVDec->m_pXMotionC[iBlkC - 1] == pWMVDec->m_pXMotionC[iBlkC]) && 
            (pWMVDec->m_pYMotionC[iBlkC - 1] == pWMVDec->m_pYMotionC[iBlkC])) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]][pCBPCurr[4]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[4]+4][pCBPLeft[4]]);
            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]][pCBPCurr[5]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[5]+4][pCBPLeft[5]]);
        }
    }
}
#else  //_WMV9AP_

Void_WMV ComputeMBLoopFilterFlags_WMV9 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft)
{
    I32_WMV iBlk0 = iMBY * 4 * pWMVDec->m_uintNumMBX + iMBX * 2;
    I32_WMV iBlk2 = iBlk0 + 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iBlkUp = iBlk0 - 2 * pWMVDec->m_uintNumMBX;
    I8_WMV* pchLoopFilterFlag = pWMVDec->m_pLoopFilterFlags[iMBY*pWMVDec->m_uintNumMBX + iMBX].chFlag;
    
    I32_WMV iBlk;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeMBLoopFilterFlags);

    // in the interests of time and complexity, sub-block Xforms in B frames 
    // aren't internally loop filtered - besides "loop filtering" isn't really
    // necessary - postfiltering with equal visual quality can take care of it
    if (pWMVDec->m_tFrmType == BVOP) {
        for (iBlk = 0; iBlk < 6; iBlk++)
            pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
    } 
    else {
        for (iBlk = 0; iBlk < 6; iBlk++) {
        if (pBlockXformMode[iBlk] == XFORMMODE_8x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
        else if (pBlockXformMode[iBlk] == XFORMMODE_8x4)
            pchLoopFilterFlag[iBlk] = (I8_WMV)0xcf;
        else if (pBlockXformMode[iBlk] == XFORMMODE_4x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x3f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x4)
                pchLoopFilterFlag[iBlk] = (I8_WMV)LOOPFILTERFLAGS_WMV9[16][pCBPCurr[iBlk]];
        }
    }

    if (bTop && bLeft) {  
        // top left
        pchLoopFilterFlag[0] &= 0xf0;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xf0;
        pchLoopFilterFlag[5] &= 0xf0;
    }
    else if (bTop) {  
        // top row
        pchLoopFilterFlag[0] &= 0xf3;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[4] &= 0xf3;
        pchLoopFilterFlag[5] &= 0xf3;
    }
    else if (bLeft) {  
        // left col
        pchLoopFilterFlag[pWMVDec->m_iBetaRTMMismatchIndex] &= 0xfc;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xfc;
        pchLoopFilterFlag[5] &= 0xfc;
    }
    if (pWMVDec->m_tFrmType == BVOP)
        return;

    if (chMBMode != MB_1MV || pWMVDec->m_pXMotion[0] != IBLOCKMV) {

        I32_WMV iBlkC ;
        // block 0 rules
        if (!bTop && (pWMVDec->m_pXMotion[iBlk0] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlkUp] == pWMVDec->m_pXMotion[iBlk0]) && 
            (pWMVDec->m_pYMotion[iBlkUp] == pWMVDec->m_pYMotion[iBlk0]))
            pchLoopFilterFlag[0] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+8][pCBPCurr[0]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[2]+12][pCBPTop[2]]);

        if (!bLeft && (pWMVDec->m_pXMotion[iBlk0] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0 - 1] == pWMVDec->m_pXMotion[iBlk0]) && 
            (pWMVDec->m_pYMotion[iBlk0 - 1] == pWMVDec->m_pYMotion[iBlk0]))
            pchLoopFilterFlag[0] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]][pCBPCurr[0]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[1]+4][pCBPLeft[1]]);
              
        // block 1 rules
        if (!bTop && (pWMVDec->m_pXMotion[iBlk0 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlkUp + 1] == pWMVDec->m_pXMotion[iBlk0 + 1]) && 
            (pWMVDec->m_pYMotion[iBlkUp + 1] == pWMVDec->m_pYMotion[iBlk0 + 1]))
            pchLoopFilterFlag[1] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+8][pCBPCurr[1]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[3]+12][pCBPTop[3]]);

        if ((pWMVDec->m_pXMotion[iBlk0 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0] == pWMVDec->m_pXMotion[iBlk0 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk0] == pWMVDec->m_pYMotion[iBlk0 + 1]))
            pchLoopFilterFlag[1] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]][pCBPCurr[1]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+4][pCBPCurr[0]]);        

        // block 2 rules
        if ((pWMVDec->m_pXMotion[iBlk2] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0] == pWMVDec->m_pXMotion[iBlk2]) && 
            (pWMVDec->m_pYMotion[iBlk0] == pWMVDec->m_pYMotion[iBlk2]))
            pchLoopFilterFlag[2] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]+8][pCBPCurr[2]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+12][pCBPCurr[0]]);

        if (!bLeft && (pWMVDec->m_pXMotion[iBlk2] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk2 - 1] == pWMVDec->m_pXMotion[iBlk2]) && 
            (pWMVDec->m_pYMotion[iBlk2 - 1] == pWMVDec->m_pYMotion[iBlk2]))
            pchLoopFilterFlag[2] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]][pCBPCurr[2]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[3]+4][pCBPLeft[3]]);

        // block 3 rules
        if ((pWMVDec->m_pXMotion[iBlk2 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0 + 1] == pWMVDec->m_pXMotion[iBlk2 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk0 + 1] == pWMVDec->m_pYMotion[iBlk2 + 1]))
            pchLoopFilterFlag[3] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]+8][pCBPCurr[3]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+12][pCBPCurr[1]]);

        if ((pWMVDec->m_pXMotion[iBlk2 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk2] == pWMVDec->m_pXMotion[iBlk2 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk2] == pWMVDec->m_pYMotion[iBlk2 + 1]))
            pchLoopFilterFlag[3] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]][pCBPCurr[3]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+4][pCBPCurr[1]]);

        // block 4 & 5 rules
        iBlkC = iMBY * pWMVDec->m_uintNumMBX + iMBX;
        if (!bTop && (pWMVDec->m_pXMotionC[iBlkC] != IBLOCKMV) && 
            (pWMVDec->m_pXMotionC[iBlkC - pWMVDec->m_uintNumMBX] == pWMVDec->m_pXMotionC[iBlkC]) && 
            (pWMVDec->m_pYMotionC[iBlkC - pWMVDec->m_uintNumMBX] == pWMVDec->m_pYMotionC[iBlkC])) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]+8][pCBPCurr[4]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[4]+12][pCBPTop[4]]);
            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]+8][pCBPCurr[5]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[5]+12][pCBPTop[5]]);
        }

        if (!bLeft && (pWMVDec->m_pXMotionC[iBlkC] != IBLOCKMV) && 
            (pWMVDec->m_pXMotionC[iBlkC - 1] == pWMVDec->m_pXMotionC[iBlkC]) && 
            (pWMVDec->m_pYMotionC[iBlkC - 1] == pWMVDec->m_pYMotionC[iBlkC])) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]][pCBPCurr[4]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[4]+4][pCBPLeft[4]]);
            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]][pCBPCurr[5]] | 
                LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[5]+4][pCBPLeft[5]]);
        }
    }
}

Void_WMV ComputeMBLoopFilterFlags_WMVA (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV* pCBPCurr, U8_WMV* pCBPTop, U8_WMV* pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft)
{
    I32_WMV iBlk0 = iMBY * 4 * pWMVDec->m_uintNumMBX + iMBX * 2;
    I32_WMV iBlk2 = iBlk0 + 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iBlkUp = iBlk0 - 2 * pWMVDec->m_uintNumMBX;
    I8_WMV* pchLoopFilterFlag = pWMVDec->m_pLoopFilterFlags[iMBY*pWMVDec->m_uintNumMBX + iMBX].chFlag;

    I32_WMV iBlk;

    if (!pWMVDec->m_bInterlaceV2 && pWMVDec->m_tFrmType == BVOP) {
        for (iBlk = 0; iBlk < 6; iBlk++)
            pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
    }
    else
    {

        for (iBlk = 0; iBlk < 6; iBlk++) {
            if (pBlockXformMode[iBlk] == XFORMMODE_8x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_8x4)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0xcf;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x3f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x4)
                pchLoopFilterFlag[iBlk] = (I8_WMV)MIDDLEFLAGS[pCBPCurr[iBlk]];
	    else
		pchLoopFilterFlag[iBlk] = 0; //pWMVDec->m_bCodecIsWVC1


    }

    }

    if (bTop && bLeft) {  
        // top left
        pchLoopFilterFlag[0] &= 0xf0;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xf0;
        pchLoopFilterFlag[5] &= 0xf0;
    }
    else if (bTop) {  
        pchLoopFilterFlag[0] &= 0xf3;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[4] &= 0xf3;
        pchLoopFilterFlag[5] &= 0xf3;
    }
    else if (bLeft) {  
        pchLoopFilterFlag[pWMVDec->m_iBetaRTMMismatchIndex] &= 0xfc;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xfc;
        pchLoopFilterFlag[5] &= 0xfc;
    }


    if (pWMVDec->m_tFrmType  == BVOP)
        return;

    if (chMBMode != MB_INTRA) {
        I32_WMV iBlkC;
       // block 0 rules
        if (!bTop && (pWMVDec->m_pXMotion[iBlk0] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlkUp] == pWMVDec->m_pXMotion[iBlk0]) && 
            (pWMVDec->m_pYMotion[iBlkUp] == pWMVDec->m_pYMotion[iBlk0]))
            pchLoopFilterFlag[0] &= 
                (TOPFLAGS[pBlockXformMode[0]][pCBPCurr[0]] | 
                TOPPREDFLAGS[pBlockXformModeTop[2]][pCBPTop[2]]);

        if (!bLeft && (pWMVDec->m_pXMotion[iBlk0] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0 - 1] == pWMVDec->m_pXMotion[iBlk0]) && 
            (pWMVDec->m_pYMotion[iBlk0 - 1] == pWMVDec->m_pYMotion[iBlk0]))
            pchLoopFilterFlag[0] &= 
                (LEFTFLAGS[pBlockXformMode[0]][pCBPCurr[0]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[1]][pCBPLeft[1]]);
              
        // block 1 rules
        if (!bTop && (pWMVDec->m_pXMotion[iBlk0 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlkUp + 1] == pWMVDec->m_pXMotion[iBlk0 + 1]) && 
            (pWMVDec->m_pYMotion[iBlkUp + 1] == pWMVDec->m_pYMotion[iBlk0 + 1]))
            pchLoopFilterFlag[1] &= 
                (TOPFLAGS[pBlockXformMode[1]][pCBPCurr[1]] | 
                TOPPREDFLAGS[pBlockXformModeTop[3]][pCBPTop[3]]);

        if ((pWMVDec->m_pXMotion[iBlk0 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0] == pWMVDec->m_pXMotion[iBlk0 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk0] == pWMVDec->m_pYMotion[iBlk0 + 1]))
            pchLoopFilterFlag[1] &= 
                (LEFTFLAGS[pBlockXformMode[1]][pCBPCurr[1]] | 
                LEFTPREDFLAGS[pBlockXformMode[0]][pCBPCurr[0]]);        

        // block 2 rules
        if ((pWMVDec->m_pXMotion[iBlk2] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0] == pWMVDec->m_pXMotion[iBlk2]) && 
            (pWMVDec->m_pYMotion[iBlk0] == pWMVDec->m_pYMotion[iBlk2]))
            pchLoopFilterFlag[2] &= 
                (TOPFLAGS[pBlockXformMode[2]][pCBPCurr[2]] | 
                TOPPREDFLAGS[pBlockXformMode[0]][pCBPCurr[0]]);

        if (!bLeft && (pWMVDec->m_pXMotion[iBlk2] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk2 - 1] == pWMVDec->m_pXMotion[iBlk2]) && 
            (pWMVDec->m_pYMotion[iBlk2 - 1] == pWMVDec->m_pYMotion[iBlk2]))
            pchLoopFilterFlag[2] &= 
                (LEFTFLAGS[pBlockXformMode[2]][pCBPCurr[2]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[3]][pCBPLeft[3]]);

        // block 3 rules
        if ((pWMVDec->m_pXMotion[iBlk2 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk0 + 1] == pWMVDec->m_pXMotion[iBlk2 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk0 + 1] == pWMVDec->m_pYMotion[iBlk2 + 1]))
            pchLoopFilterFlag[3] &= 
                (TOPFLAGS[pBlockXformMode[3]][pCBPCurr[3]] | 
                TOPPREDFLAGS[pBlockXformMode[1]][pCBPCurr[1]]);

        if ((pWMVDec->m_pXMotion[iBlk2 + 1] != IBLOCKMV) && 
            (pWMVDec->m_pXMotion[iBlk2] == pWMVDec->m_pXMotion[iBlk2 + 1]) && 
            (pWMVDec->m_pYMotion[iBlk2] == pWMVDec->m_pYMotion[iBlk2 + 1]))
            pchLoopFilterFlag[3] &= 
                (LEFTFLAGS[pBlockXformMode[3]][pCBPCurr[3]] | 
                LEFTPREDFLAGS[pBlockXformMode[2]][pCBPCurr[2]]);

        // block 4 & 5 rules
        iBlkC = iMBY * pWMVDec->m_uintNumMBX + iMBX;
        if (!bTop && (pWMVDec->m_pXMotionC[iBlkC] != IBLOCKMV) && 
            (pWMVDec->m_pXMotionC[iBlkC - pWMVDec->m_uintNumMBX] == pWMVDec->m_pXMotionC[iBlkC]) && 
            (pWMVDec->m_pYMotionC[iBlkC - pWMVDec->m_uintNumMBX] == pWMVDec->m_pYMotionC[iBlkC])) {
            pchLoopFilterFlag[4] &= 
                (TOPFLAGS[pBlockXformMode[4]][pCBPCurr[4]] | 
                TOPPREDFLAGS[pBlockXformModeTop[4]][pCBPTop[4]]);
            pchLoopFilterFlag[5] &= 
                (TOPFLAGS[pBlockXformMode[5]][pCBPCurr[5]] | 
                TOPPREDFLAGS[pBlockXformModeTop[5]][pCBPTop[5]]);
        }

        if (!bLeft && (pWMVDec->m_pXMotionC[iBlkC] != IBLOCKMV) && 
            (pWMVDec->m_pXMotionC[iBlkC - 1] == pWMVDec->m_pXMotionC[iBlkC]) && 
            (pWMVDec->m_pYMotionC[iBlkC - 1] == pWMVDec->m_pYMotionC[iBlkC])) {
            pchLoopFilterFlag[4] &= 
                (LEFTFLAGS[pBlockXformMode[4]][pCBPCurr[4]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[4]][pCBPLeft[4]]);
            pchLoopFilterFlag[5] &= 
                (LEFTFLAGS[pBlockXformMode[5]][pCBPCurr[5]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[5]][pCBPLeft[5]]);
        }



    }
}

Void_WMV m_InitLoopFilterFramebyFrame(tWMVDecInternalMember *pWMVDec)
{
    if (!pWMVDec->m_bX9)
        return;

    pWMVDec->m_pFilterHorizontalEdge = &g_FilterHorizontalEdgeV9;
    pWMVDec->m_pFilterVerticalEdge = &g_FilterVerticalEdgeV9;
#ifdef _WMV_TARGET_X86_
    if (g_bSupportMMX_WMV) {

        // Loop Filter Selection
        if (pWMVDec->m_bX9) {
            pWMVDec->m_pFilterHorizontalEdge = &g_FilterHorizontalEdgeV9_MMX;
            pWMVDec->m_pFilterVerticalEdge = &g_FilterVerticalEdgeV9_MMX;
#if !defined(WMV_DISABLE_SSE1) 
            if (g_SupportSSE1 ()) {
                pWMVDec->m_pFilterHorizontalEdge = &g_FilterHorizontalEdgeV9_KNI;
                pWMVDec->m_pFilterVerticalEdge = &g_FilterVerticalEdgeV9_KNI;
            }
#endif // SSE1
        }
        else {
            pWMVDec->m_pFilterHorizontalEdge = &g_FilterHorizontalEdgeV8_MMX;
            pWMVDec->m_pFilterVerticalEdge = &g_FilterVerticalEdgeV8_MMX;
        }
    }
#endif
}

Void_WMV g_InitRepeatRefInfo (tWMVDecInternalMember *pWMVDec, Bool_WMV bAdvancedProfile)
{
    
    g_pRepeatRef0Y = g_RepeatRef0Y; 
    g_pRepeatRef0UV = g_RepeatRef0UV;



    EXPANDY_REFVOP = pWMVDec->EXPANDY_REFVOP;
    EXPANDUV_REFVOP = pWMVDec->EXPANDUV_REFVOP;

    if (bAdvancedProfile) {
        g_pRepeatRef0Y = g_RepeatRef0Y_AP; 
        g_pRepeatRef0UV = g_RepeatRef0UV_AP;
        return;
    }

//#if (defined(WMV_TARGET_X86) || defined(_Embedded_x86)) && !defined(UNDER_CE)

}

Void_WMV g_RepeatRef0Y_AP (
    U8_WMV* ppxlcRef0Y,
    CoordI  iStart, 
    CoordI  iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthY,
    I32_WMV     iWidthYPlusExp,
    I32_WMV     iWidthPrevY,
    Bool_WMV    bProgressive
)
{
    const U8_WMV* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
    const U8_WMV* ppxliOldTopLn = ppxliOldLeft - EXPANDY_REFVOP;
    U8_WMV* ppxliOldRight = (U8_WMV *) ppxliOldLeft + iWidthY - 1;
    U8_WMV* ppxliNew = (U8_WMV*) ppxliOldTopLn;
    U8_WMV* ppxliNewPlusWidth;
    I32_WMV iTrueWidth = iWidthY + ((-iWidthY) & 0xf) + 2 * EXPANDY_REFVOP; // width + alignment + horizontal repeatpad
    I32_WMV iTrueWidthMinusExp = iTrueWidth - EXPANDY_REFVOP;
    I32_WMV iVertPad;
    CoordI y;
    I32_WMV iWidthPrevYDiv8 = iTrueWidth >> 3;
    U8_WMV* ppxliSrc;
    U8_WMV* ppxliDst;

    if (bProgressive)
        iVertPad = 36;  // 32 pixels of additional access + 4 pixels for interpolator (16 + 2 lines in interlace resolution)
    else
        iVertPad = 18;  // 16 pixels of additional access + 2 pixels for interpolator
    //printf ("%d %d  ", iWidthY, iEnd);
    /** disabled - interlace or not must be passed in externally!!!
    I32_WMV iLastVert = 0;  // Hack ... remove once fix is ready
    if (iWidthPrevY - iWidthYPlusExp != 32) {
        iVertPad = (EXPANDY_REFVOP >> 1) + 2; // Must be interlaced frame
        ppxlcRef0Y -= iWidthPrevY * 2;
    }
    //  Part of brutal hack. Remove once real fix is ready
    if (iEnd & 15)
        iLastVert = 16 - (iEnd & 15);
    **/

    
#ifndef HITACHI    
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U64 uipadValueLeft;
        U64 uipadValueRight;
        I32_WMV x;
        uipadValueLeft = *ppxliOldLeft;    
        uipadValueRight = *ppxliOldRight;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueLeft |= (uipadValueLeft << 16);
        uipadValueRight |= (uipadValueRight << 16);
        uipadValueLeft |= (uipadValueLeft << 32);
        uipadValueRight |= (uipadValueRight << 32);
        ppxliNewPlusWidth = ppxliNew + iTrueWidthMinusExp; //+ iWidthYPlusExp;
        /** tight overlap on right **/
        for ( x = 0; x < ((-iWidthY) & 0xf); x++)
            ppxliOldRight[x + 1] = ppxliOldRight[0];
        for (x = 0; x < EXPANDY_REFVOP; x += 8) {
            *(U64 *)(ppxliNew + x) = uipadValueLeft;
            *(U64 *)(ppxliNewPlusWidth + x) = uipadValueRight;
        }
        ppxliNew += iWidthPrevY;
        ppxliOldLeft += iWidthPrevY;
        ppxliOldRight += iWidthPrevY;
    }
#else
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U64 uipadValueLeft;
        U64 uipadValueRight;
        uipadValueLeft.dwLo = (DWORD)*ppxliOldLeft;    
        uipadValueRight.dwLo = (DWORD)*ppxliOldRight;
        uipadValueLeft.dwLo |= (uipadValueLeft.dwLo << 8);
        uipadValueRight.dwLo |= (uipadValueRight.dwLo << 8);
        uipadValueLeft.dwLo |= (uipadValueLeft.dwLo << 16);
        uipadValueRight.dwLo |= (uipadValueRight.dwLo << 16);
        uipadValueLeft.dwHi = (uipadValueLeft.dwLo );
        uipadValueRight.dwHi = (uipadValueRight.dwLo );
        ppxliNewPlusWidth = ppxliNew + iTrueWidthMinusExp; // + iWidthYPlusExp;
        /** tight overlap on right **/
        for (I32_WMV x = 0; x < ((-iWidthY) & 0xf); x++)
            ppxliOldRight[x + 1] = ppxliOldRight[0];
        for (x = 0; x < EXPANDY_REFVOP; x += 8) {
            *(U64 *)(ppxliNew + x) = uipadValueLeft;
            *(U64 *)(ppxliNewPlusWidth + x) = uipadValueRight;
        }
        ppxliNew += iWidthPrevY;
        ppxliOldLeft += iWidthPrevY;
        ppxliOldRight += iWidthPrevY;
    }
#endif
    
    
    if (fTop) {
        U8_WMV* ppxliLeftTop = (U8_WMV *) ppxliOldTopLn - iVertPad * iWidthPrevY;
        for (y = 0; y < iVertPad; y++) {
            I32_WMV x;
            //memcpy (ppxliLeftTop, ppxliOldTopLn, iWidthPrevY);
            ppxliSrc = (U8_WMV*) ppxliOldTopLn;
            ppxliDst = (U8_WMV*) ppxliLeftTop;
            for ( x = 0; x < iWidthPrevYDiv8; x++) {
                *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
                ppxliSrc += 8;
                ppxliDst += 8;
            }
            ppxliLeftTop += iWidthPrevY;
        }
    }
    if (fBottom) {
        const U8_WMV* ppxliOldBotLn = ppxliNew - iWidthPrevY;
        /** disabled
        iVertPad += iLastVert; // Part of hack ... remove when fix is ready.
        **/

        if (bProgressive)
            iVertPad += ((-iEnd) & 0x1f);
        else
            iVertPad += ((-iEnd) & 0xf);

//printf("b%d ", iVertPad);
        for (y = 0; y < iVertPad; y++) {
            I32_WMV x = 0;
            //memcpy (ppxliNew, ppxliOldBotLn, iWidthPrevY);
            ppxliSrc = (U8_WMV*) ppxliOldBotLn;
            ppxliDst = (U8_WMV*) ppxliNew;
            for ( x = 0; x < iWidthPrevYDiv8; x++) {
                *(U64 *) ppxliDst = *(U64 *) ppxliSrc;
                ppxliSrc += 8;
                ppxliDst += 8;
            }
            ppxliNew += iWidthPrevY;
        }
    }
}

Void_WMV g_RepeatRef0UV_AP (
    U8_WMV* ppxlcRef0U,
    U8_WMV* ppxlcRef0V,
    CoordI  iStart, CoordI iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV,
    Bool_WMV    bProgressive
)
{
    const U8_WMV* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
    const U8_WMV* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
    U8_WMV* ppxliOldRightU = (U8_WMV *) ppxliOldLeftU + iWidthUV - 1;
    U8_WMV* ppxliOldRightV = (U8_WMV *) ppxliOldLeftV + iWidthUV - 1;
    const U8_WMV* ppxliOldTopLnU = ppxliOldLeftU - EXPANDUV_REFVOP;
    const U8_WMV* ppxliOldTopLnV = ppxliOldLeftV - EXPANDUV_REFVOP;
    U8_WMV* ppxliNewU = (U8_WMV*) ppxliOldTopLnU;
    U8_WMV* ppxliNewV = (U8_WMV*) ppxliOldTopLnV;
    U8_WMV* ppxliNewUPlusWidth;
    U8_WMV* ppxliNewVPlusWidth;
    I32_WMV iTrueWidth = iWidthUV + ((-iWidthUV) & 7) + 2 * EXPANDUV_REFVOP; // width + alignment + horizontal repeatpad
    I32_WMV iTrueWidthMinusExp = iTrueWidth - EXPANDUV_REFVOP;
    I32_WMV iVertPad;
    CoordI y;

    I32_WMV iWidthPrevUVDiv8 = iTrueWidth >> 2;
    U8_WMV* ppxliSrcU;
    U8_WMV* ppxliSrcV;
    U8_WMV* ppxliDstU;
    U8_WMV* ppxliDstV;

    if (bProgressive)
        iVertPad = 18;  // 16 pixels of additional access + 2 pixels for interpolator (8 + 1 lines in interlace resolution)
    else
        iVertPad = 9;   // 8 pixels of additional access + 1 pixel for interpolator

    /** disabled - interlace or not must be passed in externally!!!
    I32_WMV iLastVert = 0; // Hack
    if ((iWidthPrevUV - iWidthUVPlusExp) != 16) {
        iVertPad = (EXPANDUV_REFVOP >> 1) + 1;    // Must be interlaced frame
        ppxlcRef0U -= iWidthPrevUV;
        ppxlcRef0V -= iWidthPrevUV;

        // Hack
        if (iEnd & 7)
            iLastVert = 8 - (iEnd & 7);
    }
    **/

    
#ifndef HITACHI
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U64 uipadValueLeftU, uipadValueLeftV;
        U64 uipadValueRightU, uipadValueRightV;
        I32_WMV x;
        uipadValueLeftU = *ppxliOldLeftU;
        uipadValueLeftV = *ppxliOldLeftV;
        uipadValueLeftU |= (uipadValueLeftU << 8);
        uipadValueLeftV |= (uipadValueLeftV << 8);
        uipadValueLeftU |= (uipadValueLeftU << 16);
        uipadValueLeftV |= (uipadValueLeftV << 16);
        uipadValueLeftU |= (uipadValueLeftU << 32);
        uipadValueLeftV |= (uipadValueLeftV << 32);
        uipadValueRightU = *ppxliOldRightU;
        uipadValueRightV = *ppxliOldRightV;
        uipadValueRightU |= (uipadValueRightU << 8);
        uipadValueRightV |= (uipadValueRightV << 8);
        uipadValueRightU |= (uipadValueRightU << 16);
        uipadValueRightV |= (uipadValueRightV << 16);
        uipadValueRightU |= (uipadValueRightU << 32);
        uipadValueRightV |= (uipadValueRightV << 32);
        ppxliNewUPlusWidth = ppxliNewU + iTrueWidthMinusExp; // + iWidthUVPlusExp;
        ppxliNewVPlusWidth = ppxliNewV + iTrueWidthMinusExp; //iWidthUVPlusExp;
        /** tight overlap on right **/
        for ( x = 0; x < ((-iWidthUV) & 0x7); x++) {
            ppxliOldRightU[x + 1] = ppxliOldRightU[0];
            ppxliOldRightV[x + 1] = ppxliOldRightV[0];
        }
        for (x = 0; x < EXPANDUV_REFVOP; x += 8) {
            *(U64 *)(ppxliNewU + x) = uipadValueLeftU;
            *(U64 *)(ppxliNewV + x) = uipadValueLeftV;
            *(U64 *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
            *(U64 *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
        }
        ppxliNewU += iWidthPrevUV;        
        ppxliNewV += iWidthPrevUV;
        ppxliOldLeftU += iWidthPrevUV;
        ppxliOldLeftV += iWidthPrevUV;
        ppxliOldRightU += iWidthPrevUV;
        ppxliOldRightV += iWidthPrevUV;
    }
#else
    for (y = iStart; y < iEnd; y++) { // x-direction interpolation
        U64 uipadValueLeftU, uipadValueLeftV;
        U64 uipadValueRightU, uipadValueRightV;
        uipadValueLeftU.dwLo = (DWORD)*ppxliOldLeftU;
        uipadValueLeftV.dwLo = (DWORD)*ppxliOldLeftV;
        uipadValueLeftU.dwLo |= (uipadValueLeftU.dwLo << 8);
        uipadValueLeftV.dwLo |= (uipadValueLeftV.dwLo << 8);
        uipadValueLeftU.dwLo |= (uipadValueLeftU.dwLo << 16);
        uipadValueLeftV.dwLo |= (uipadValueLeftV.dwLo << 16);
        uipadValueLeftU.dwHi = (uipadValueLeftU.dwLo);
        uipadValueLeftV.dwHi = (uipadValueLeftV.dwLo);
        uipadValueRightU.dwLo = (DWORD)*ppxliOldRightU;
        uipadValueRightV.dwLo = (DWORD)*ppxliOldRightV;
        uipadValueRightU.dwLo |= (uipadValueRightU.dwLo << 8);
        uipadValueRightV.dwLo |= (uipadValueRightV.dwLo << 8);
        uipadValueRightU.dwLo |= (uipadValueRightU.dwLo << 16);
        uipadValueRightV.dwLo |= (uipadValueRightV.dwLo << 16);
        uipadValueRightU.dwHi = (uipadValueRightU.dwLo );
        uipadValueRightV.dwHi = (uipadValueRightV.dwLo);
        ppxliNewUPlusWidth = ppxliNewU + iTrueWidthMinusExp; // + iWidthUVPlusExp;;
        ppxliNewVPlusWidth = ppxliNewV + iTrueWidthMinusExp; // + iWidthUVPlusExp;;
        /** tight overlap on right **/
        for (I32_WMV x = 0; x < ((-iWidthUV) & 0x7); x++) {
            ppxliOldRightU[x + 1] = ppxliOldRightU[0];
            ppxliOldRightV[x + 1] = ppxliOldRightV[0];
        }
        for (x = 0; x < EXPANDUV_REFVOP; x += 8) {
            *(U64 *)(ppxliNewU + x) = uipadValueLeftU;
            *(U64 *)(ppxliNewV + x) = uipadValueLeftV;
            *(U64 *)(ppxliNewUPlusWidth + x) = uipadValueRightU;
            *(U64 *)(ppxliNewVPlusWidth + x) = uipadValueRightV;
        }
        ppxliNewU += iWidthPrevUV;        
        ppxliNewV += iWidthPrevUV;
        ppxliOldLeftU += iWidthPrevUV;
        ppxliOldLeftV += iWidthPrevUV;
        ppxliOldRightU += iWidthPrevUV;
        ppxliOldRightV += iWidthPrevUV;
    }
#endif
    
    if (fTop) {
        U8_WMV* ppxliLeftTopU = (U8_WMV *) ppxliOldTopLnU - iVertPad * iWidthPrevUV;
        U8_WMV* ppxliLeftTopV = (U8_WMV *) ppxliOldTopLnV - iVertPad * iWidthPrevUV;
        for (y = 0; y < iVertPad; y++) {
            I32_WMV x;
            ppxliSrcU = (U8_WMV*) ppxliOldTopLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldTopLnV;
            ppxliDstU = ppxliLeftTopU;
            ppxliDstV = ppxliLeftTopV;
            for ( x = 0; x < iWidthPrevUVDiv8; x++) {
                *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                ppxliSrcU += 4;
                ppxliDstU += 4;
                *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                ppxliSrcV += 4;
                ppxliDstV += 4;
            }
            ppxliLeftTopU += iWidthPrevUV;
            ppxliLeftTopV += iWidthPrevUV;
        }
    }
    if (fBottom) {
        /** disabled
        iVertPad += iLastVert; // Part of hack ... remove when fix is ready.
        **/
        I32_WMV x;
        const U8_WMV* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
        const U8_WMV* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;

        if (bProgressive)
            iVertPad += ((-iEnd) & 0xf);
        else
            iVertPad += ((-iEnd) & 0x7);

        for (y = 0; y < iVertPad; y++) {
            ppxliSrcU = (U8_WMV*) ppxliOldBotLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldBotLnV;
            ppxliDstU = ppxliNewU;
            ppxliDstV = ppxliNewV;
            for ( x = 0; x < iWidthPrevUVDiv8; x++) {
                *(U32_WMV *) ppxliDstU = *(U32_WMV *) ppxliSrcU;
                ppxliSrcU += 4;
                ppxliDstU += 4;
                *(U32_WMV *) ppxliDstV = *(U32_WMV *) ppxliSrcV;
                ppxliSrcV += 4;
                ppxliDstV += 4;
            }
            ppxliNewU += iWidthPrevUV;
            ppxliNewV += iWidthPrevUV;
        }
    }
}


#endif  //_WMV9AP_


#define EXTRACT0_ROWX4(x) ((x & 0xc0) << 0)
#define EXTRACT1_ROWX4(x) ((x & 0xc0) >> 2)
#define EXTRACT2_ROWX4(x) ((x & 0xc0) >> 4)
#define EXTRACT3_ROWX4(x) ((x & 0xc0) >> 6)
#define EXTRACT0_ROWX8(x) ((x & 0xc) << 4) 
#define EXTRACT1_ROWX8(x) ((x & 0xc) << 2) 
#define EXTRACT2_ROWX8(x) ((x & 0xc) >> 0) 
#define EXTRACT3_ROWX8(x) ((x & 0xc) >> 2) 

#define EXTRACT0_COLX4(x) ((x & 0x30) << 2)
#define EXTRACT1_COLX4(x) ((x & 0x30) << 0)
#define EXTRACT2_COLX4(x) ((x & 0x30) >> 2)
#define EXTRACT3_COLX4(x) ((x & 0x30) >> 4)
#define EXTRACT0_COLX8(x) ((x & 0x3) << 6) 
#define EXTRACT1_COLX8(x) ((x & 0x3) << 4) 
#define EXTRACT2_COLX8(x) ((x & 0x3) << 2) 
#define EXTRACT3_COLX8(x) ((x & 0x3) >> 0)

Void_WMV GenerateRowLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterRowFlagY, U8_WMV* puchFilterRowFlagU, U8_WMV* puchFilterRowFlagV,
    U8_WMV* puchFilterRowFlagY2, U8_WMV* puchFilterRowFlagU2, U8_WMV* puchFilterRowFlagV2,
    LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBYStart, I32_WMV iMBYEnd, Bool_WMV bFirstRow)
{
    U8_WMV* puch8x4FilterRow0 = puchFilterRowFlagY2;
    U8_WMV* puch8x4FilterRow1 = puchFilterRowFlagY2 + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV* puch8x8FilterRow0 = puchFilterRowFlagY;
    U8_WMV* puch8x8FilterRow1 = puchFilterRowFlagY + ((pWMVDec->m_uintNumMBX + 1) >> 1);
    U8_WMV uch8x4Flag0, uch8x4Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch8x4FlagU, uch8x4FlagV;
    I32_WMV  i4MBX, iMBY;
    I32_WMV  iNumMBX = pWMVDec->m_uintNumMBX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(GenerateRowLoopFilterFlags);
    
    uch8x4Flag0 = 0, uch8x4Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch8x4FlagU = 0, uch8x4FlagV = 0;
    
    if (bFirstRow)
    {
        iMBYStart = 1;
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
            uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [1]);
            uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [3]);
            uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [3]);
            uch8x4FlagU |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [4]);
            uch8x4FlagV |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [5]);


            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;

            uch8x4Flag0 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x8Flag0 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [3]);
            uch8x4FlagU |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [4]);
            uch8x4FlagV |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [5]);
            uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [1]);
            uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [3]);
            uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [3]);
            uch8x4FlagU |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [4]);
            uch8x4FlagV |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [5]);


            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;


            *puchFilterRowFlagU2++ = uch8x4FlagU;
            *puchFilterRowFlagV2++ = uch8x4FlagV;
            pLoopFilterFlag += 4;
        } while ( ++i4MBX < iNumMBX >> 2);
      }

        if (iNumMBX & 3)
        {
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);

            pLoopFilterFlag ++;

            if ((iNumMBX & 2) == 2)
            {
                uch8x4Flag0 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;
            }
            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;

            if ((iNumMBX & 3) == 3)
            {
                uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;

                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
            }
            *puchFilterRowFlagU2++ = uch8x4FlagU;
            *puchFilterRowFlagV2++ = uch8x4FlagV;
        }
        puch8x8FilterRow1 = puch8x8FilterRow0 + ((iNumMBX + 1) >> 1);
        puch8x4FilterRow0 = puch8x4FilterRow1;
        puch8x4FilterRow1 = puch8x4FilterRow1 + ((iNumMBX + 1) >> 1);
    }

    for (iMBY = iMBYStart; iMBY < iMBYEnd; iMBY ++)
    {
		i4MBX = 0;
      if(iNumMBX>3)//zou 401,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag0  = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
            uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
            uch8x8FlagU = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [4]);
            uch8x8FlagV = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [5]);
            uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [1]);
            uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 1)->chFlag [3]);
            uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [1]);
            uch8x8Flag1 |= EXTRACT2_ROWX8((pLoopFilterFlag + 1)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT3_ROWX8((pLoopFilterFlag + 1)->chFlag [3]);
            uch8x4FlagU |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [4]);
            uch8x4FlagV |= EXTRACT1_ROWX4((pLoopFilterFlag + 1)->chFlag [5]);
            uch8x8FlagU |= EXTRACT1_ROWX8((pLoopFilterFlag + 1)->chFlag [4]);
            uch8x8FlagV |= EXTRACT1_ROWX8((pLoopFilterFlag + 1)->chFlag [5]);

            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;
            uch8x4Flag0 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [1]);
            uch8x8Flag0 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4((pLoopFilterFlag + 2)->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_ROWX8((pLoopFilterFlag + 2)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8((pLoopFilterFlag + 2)->chFlag [3]);
            uch8x4FlagU |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [4]);
            uch8x4FlagV |= EXTRACT2_ROWX4((pLoopFilterFlag + 2)->chFlag [5]);
            uch8x8FlagU |= EXTRACT2_ROWX8((pLoopFilterFlag + 2)->chFlag [4]);
            uch8x8FlagV |= EXTRACT2_ROWX8((pLoopFilterFlag + 2)->chFlag [5]);
            uch8x4Flag0 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [0]);
            uch8x4Flag0 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [1]);
            uch8x8Flag0 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [1]);
            uch8x4Flag1 |= EXTRACT2_ROWX4((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x4Flag1 |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [3]);
            uch8x8Flag1 |= EXTRACT2_ROWX8((pLoopFilterFlag + 3)->chFlag [2]);
            uch8x8Flag1 |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [3]);
            uch8x4FlagU |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [4]);
            uch8x4FlagV |= EXTRACT3_ROWX4((pLoopFilterFlag + 3)->chFlag [5]);
            uch8x8FlagU |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [4]);
            uch8x8FlagV |= EXTRACT3_ROWX8((pLoopFilterFlag + 3)->chFlag [5]);

            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;
            *puchFilterRowFlagU++ = uch8x8FlagU;
            *puchFilterRowFlagV++ = uch8x8FlagV;
            *puchFilterRowFlagU2++ = uch8x4FlagU;
            *puchFilterRowFlagV2++ = uch8x4FlagV;
            pLoopFilterFlag += 4;
        } while ( ++i4MBX < iNumMBX >> 2 );
       }

        if (iNumMBX & 3)
        {
            uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
            uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
            uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
            uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
            uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
            uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
            uch8x4FlagU = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [4]);
            uch8x4FlagV = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [5]);
            uch8x8FlagU = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [4]);
            uch8x8FlagV = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [5]);

                        pLoopFilterFlag++;


            if ((iNumMBX & 2) == 2)
            {

                uch8x4Flag0 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x8Flag0 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [0]);
                uch8x8Flag0 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT3_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag1 |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag1 |= EXTRACT3_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [5]);
                uch8x8FlagU |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [4]);
                uch8x8FlagV |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;

            }
            *puch8x4FilterRow0++ = uch8x4Flag0;
            *puch8x4FilterRow1++ = uch8x4Flag1;
            *puch8x8FilterRow0++ = uch8x8Flag0;
            *puch8x8FilterRow1++ = uch8x8Flag1;

            if ((iNumMBX & 3) == 3)
            {

                uch8x4Flag0 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [0]);
                uch8x4Flag0 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [1]);
                uch8x8Flag0 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [0]);
                uch8x8Flag0 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [1]);
                uch8x4Flag1 = EXTRACT0_ROWX4(pLoopFilterFlag->chFlag [2]);
                uch8x4Flag1 |= EXTRACT1_ROWX4(pLoopFilterFlag->chFlag [3]);
                uch8x8Flag1 = EXTRACT0_ROWX8(pLoopFilterFlag->chFlag [2]);
                uch8x8Flag1 |= EXTRACT1_ROWX8(pLoopFilterFlag->chFlag [3]);
                uch8x4FlagU |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [4]);
                uch8x4FlagV |= EXTRACT2_ROWX4(pLoopFilterFlag->chFlag [5]);
                uch8x8FlagU |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [4]);
                uch8x8FlagV |= EXTRACT2_ROWX8(pLoopFilterFlag->chFlag [5]);
                pLoopFilterFlag++;


                *puch8x4FilterRow0++ = uch8x4Flag0;
                *puch8x4FilterRow1++ = uch8x4Flag1;
                *puch8x8FilterRow0++ = uch8x8Flag0;
                *puch8x8FilterRow1++ = uch8x8Flag1;
            }
            *puchFilterRowFlagU++ = uch8x8FlagU;
            *puchFilterRowFlagV++ = uch8x8FlagV;
            *puchFilterRowFlagU2++ = uch8x4FlagU;
            *puchFilterRowFlagV2++ = uch8x4FlagV;
        }
        puch8x4FilterRow0 = puch8x4FilterRow1;
        puch8x4FilterRow1 = puch8x4FilterRow1 + ((iNumMBX + 1) >> 1);
        puch8x8FilterRow0 = puch8x8FilterRow1;
        puch8x8FilterRow1 = puch8x8FilterRow1 + ((iNumMBX + 1) >> 1);
    }
}

Void_WMV GenerateColumnLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* puchFilterColFlagY, U8_WMV* puchFilterColFlagU, U8_WMV* puchFilterColFlagV,
    U8_WMV* puchFilterColFlagY2, U8_WMV* puchFilterColFlagU2, U8_WMV* puchFilterColFlagV2,
    LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight)
{
    U8_WMV* puch4x8FilterCol0 = puchFilterColFlagY2;
    U8_WMV* puch4x8FilterCol1 = puchFilterColFlagY2 + ((iMBHeight + 1) >> 1);
    U8_WMV* puch8x8FilterCol0 = puchFilterColFlagY;
    U8_WMV* puch8x8FilterCol1;
    U8_WMV uch4x8Flag0, uch4x8Flag1, uch8x8Flag0, uch8x8Flag1;
    U8_WMV uch8x8FlagU, uch8x8FlagV, uch4x8FlagU, uch4x8FlagV;
    LOOPF_FLAG* pLoopFilterFlag2 = pLoopFilterFlag;
    I32_WMV iMBX;
    I32_WMV i4MBY;
	I32_WMV iNumMBX = pWMVDec->m_uintNumMBX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(GenerateColumnLoopFilterFlags);

    uch4x8Flag0 = 0, uch4x8Flag1 = 0, uch8x8Flag0 = 0, uch8x8Flag1 = 0;
    uch8x8FlagU = 0, uch8x8FlagV = 0, uch4x8FlagU = 0, uch4x8FlagV = 0;

	i4MBY = 0;

  if(iMBHeight>3) //zou 330,pWMVDec->m_bCodecIsWVC1
  {
    do
   {
        // Extract flag from pLoopFilterFlag
        
        uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);

        pLoopFilterFlag2 += iNumMBX;
        uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);


        *puch4x8FilterCol0++ = uch4x8Flag0;
        *puch4x8FilterCol1++ = uch4x8Flag1;
        *puch8x8FilterCol0++ = uch8x8Flag0;
        pLoopFilterFlag2 += iNumMBX;

        uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);

        uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
        pLoopFilterFlag2 += iNumMBX;
        uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [5]);
        pLoopFilterFlag2 += iNumMBX;


        *puch4x8FilterCol0++ = uch4x8Flag0;
        *puch4x8FilterCol1++ = uch4x8Flag1;
        *puch8x8FilterCol0++ = uch8x8Flag0;

        *puchFilterColFlagU2++ = uch4x8FlagU;
        *puchFilterColFlagV2++ = uch4x8FlagV;
    } while ( ++i4MBY < iMBHeight >> 2 );
   }

    if (iMBHeight & 3)
    {
        uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
        uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
        uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
        uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
        uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
        uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
        uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
        uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
        pLoopFilterFlag2 += iNumMBX;

        if ((iMBHeight & 2) == 2)
        {
            uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
        }
        *puch4x8FilterCol0++ = uch4x8Flag0;
        *puch4x8FilterCol1++ = uch4x8Flag1;
        *puch8x8FilterCol0++ = uch8x8Flag0;

        if ((iMBHeight & 3) == 3)
        {
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);

            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
        }
        *puchFilterColFlagU2++ = uch4x8FlagU;
        *puchFilterColFlagV2++ = uch4x8FlagV;

    }

    puch4x8FilterCol0 = puch4x8FilterCol1;
    puch4x8FilterCol1 = puch4x8FilterCol1 + ((iMBHeight + 1) >> 1);
    puch8x8FilterCol1 = puch8x8FilterCol0 + ((iMBHeight + 1) >> 1);
    pLoopFilterFlag ++;

    iMBX = 1;

  if(iNumMBX>1) //zou 401,pWMVDec->m_bCodecIsWVC1
  {
    do
    {
        pLoopFilterFlag2 = pLoopFilterFlag;
		i4MBY = 0;
      if(iMBHeight>3)   //zou 330,pWMVDec->m_bCodecIsWVC1
      {
        do
        {
            // Extract flag from pLoopFilterFlag
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [5]);

            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
            *puch8x8FilterCol1++ = uch8x8Flag1;
            pLoopFilterFlag2 += iNumMBX;
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;
            uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;

            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
            *puch8x8FilterCol1++ = uch8x8Flag1;
            *puchFilterColFlagU++ = uch8x8FlagU;
            *puchFilterColFlagV++ = uch8x8FlagV;
            *puchFilterColFlagU2++ = uch4x8FlagU;
            *puchFilterColFlagV2++ = uch4x8FlagV;
        } while ( ++i4MBY < iMBHeight >> 2 );
       }

        if (iMBHeight & 3)
        {
            uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
            uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
            uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
            uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
            uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
            uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
            uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
            uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
            uch4x8FlagU = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [4]);
            uch4x8FlagV = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [5]);
            uch8x8FlagU = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [4]);
            uch8x8FlagV = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [5]);
            pLoopFilterFlag2 += iNumMBX;

            if ((iMBHeight & 2) == 2)
            {
                uch4x8Flag0 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [0]);
                uch4x8Flag0 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [2]);
                uch8x8Flag0 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [0]);
                uch8x8Flag0 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [2]);
                uch4x8Flag1 |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [1]);
                uch4x8Flag1 |= EXTRACT3_COLX4(pLoopFilterFlag2->chFlag [3]);
                uch8x8Flag1 |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [1]);
                uch8x8Flag1 |= EXTRACT3_COLX8(pLoopFilterFlag2->chFlag [3]);
                uch4x8FlagU |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [4]);
                uch4x8FlagV |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [5]);
                uch8x8FlagU |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [4]);
                uch8x8FlagV |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [5]);
                pLoopFilterFlag2 += iNumMBX;
            }
            *puch4x8FilterCol0++ = uch4x8Flag0;
            *puch4x8FilterCol1++ = uch4x8Flag1;
            *puch8x8FilterCol0++ = uch8x8Flag0;
            *puch8x8FilterCol1++ = uch8x8Flag1;

            if ((iMBHeight & 3) == 3)
            {
                uch4x8Flag0 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [0]);
                uch4x8Flag0 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [2]);
                uch8x8Flag0 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [0]);
                uch8x8Flag0 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [2]);
                uch4x8Flag1 = EXTRACT0_COLX4(pLoopFilterFlag2->chFlag [1]);
                uch4x8Flag1 |= EXTRACT1_COLX4(pLoopFilterFlag2->chFlag [3]);
                uch8x8Flag1 = EXTRACT0_COLX8(pLoopFilterFlag2->chFlag [1]);
                uch8x8Flag1 |= EXTRACT1_COLX8(pLoopFilterFlag2->chFlag [3]);
                uch4x8FlagU |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [4]);
                uch4x8FlagV |= EXTRACT2_COLX4(pLoopFilterFlag2->chFlag [5]);
                uch8x8FlagU |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [4]);
                uch8x8FlagV |= EXTRACT2_COLX8(pLoopFilterFlag2->chFlag [5]);

                *puch4x8FilterCol0++ = uch4x8Flag0;
                *puch4x8FilterCol1++ = uch4x8Flag1;
                *puch8x8FilterCol0++ = uch8x8Flag0;
                *puch8x8FilterCol1++ = uch8x8Flag1;
            }
            *puchFilterColFlagU++ = uch8x8FlagU;
            *puchFilterColFlagV++ = uch8x8FlagV;
            *puchFilterColFlagU2++ = uch4x8FlagU;
            *puchFilterColFlagV2++ = uch4x8FlagV;
        }
        puch4x8FilterCol0 = puch4x8FilterCol1;
        puch4x8FilterCol1 = puch4x8FilterCol1 + ((iMBHeight + 1) >> 1);
        puch8x8FilterCol0 = puch8x8FilterCol1;
        puch8x8FilterCol1 = puch8x8FilterCol1 + ((iMBHeight + 1) >> 1);
        pLoopFilterFlag ++;
    } while ( ++iMBX < iNumMBX );
   }
}
#endif // WMV9_SIMPLE_ONLY


#ifdef WMV_OPT_INTENSITYCOMP_ARM
extern const U8_WMV g_iDownAdjustRecon [256];
extern const U8_WMV g_iUpAdjustRecon [256];
extern Void_WMV IntensityComp (U32_WMV* pScr, I32_WMV iFrameSize, const U8_WMV* pLUT);
#endif
// used to adjust recon


#ifndef WMV9_SIMPLE_ONLY

//#ifdef _EMB_3FRAMES_
               
Void_WMV AdjustReconRange2AltRef (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iFrmSrcAreaY = pWMVDec->m_iHeightPrevY*pWMVDec->m_iWidthPrevY;
    I32_WMV iFrmSrcAreaUV = pWMVDec->m_iHeightPrevUV*pWMVDec->m_iWidthPrevUV;

    U8_WMV *pRefY = pWMVDec->m_ppxliRef0Y;
    U8_WMV *pRefU = pWMVDec->m_ppxliRef0U;
    U8_WMV *pRefV = pWMVDec->m_ppxliRef0V;

    U8_WMV *pDestY = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane;
    U8_WMV *pDestU = pWMVDec->m_pfrmRef0AltQ->m_pucUPlane;
    U8_WMV *pDestV = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(AdjustReconRange);


    if (pWMVDec->m_iReconRangeState == 0) {
        if (pWMVDec->m_iRangeState == 1) {
            // reduce by 2
            I32_WMV i;

            pWMVDec->m_bRef0InRefAlt = TRUE_WMV;

            for (i = 0; i < iFrmSrcAreaY; i++) {
                pDestY [i] = ((pRefY [i] - 128) >> 1) + 128;
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                pDestU [i] = ((pRefU [i] - 128) >> 1) + 128;
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                pDestV [i] = ((pRefV [i] - 128) >> 1) + 128;
            }
            pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
        }
    } else if (pWMVDec->m_iReconRangeState == 1) {
        if (pWMVDec->m_iRangeState == 0) {
            // increase by 2
            I32_WMV i;

            pWMVDec->m_bRef0InRefAlt = TRUE_WMV;

            for (i = 0; i < iFrmSrcAreaY; i++) {
                I32_WMV iTmp = (((I32_WMV)pRefY [i] - 128) << 1) + 128;
                pDestY [i] = g_rgiClapTabDec [iTmp];
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                I32_WMV iTmp = (((I32_WMV)pRefU [i] - 128) << 1) + 128;
                pDestU [i] = g_rgiClapTabDec [iTmp];
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                I32_WMV iTmp = (((I32_WMV)pRefV [i] - 128) << 1) + 128;
                pDestV [i] = g_rgiClapTabDec [iTmp];
            }
            pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
        }
    }
}


//#endif


Void_WMV AdjustReconRange (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iFrmSrcAreaY = pWMVDec->m_iHeightPrevY*pWMVDec->m_iWidthPrevY;
    I32_WMV iFrmSrcAreaUV = pWMVDec->m_iHeightPrevUV*pWMVDec->m_iWidthPrevUV;

    U8_WMV *pRefY = pWMVDec->m_ppxliRef0Y;
    U8_WMV *pRefU = pWMVDec->m_ppxliRef0U;
    U8_WMV *pRefV = pWMVDec->m_ppxliRef0V;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(AdjustReconRange);

    if (pWMVDec->m_iReconRangeState == 0) {
        if (pWMVDec->m_iRangeState == 1) {
#ifdef WMV_OPT_INTENSITYCOMP_ARM
            IntensityComp ((U32_WMV*)pRefY, iFrmSrcAreaY >> 4, g_iDownAdjustRecon);
            IntensityComp ((U32_WMV*)pRefU, iFrmSrcAreaUV >> 4, g_iDownAdjustRecon);
            IntensityComp ((U32_WMV*)pRefV, iFrmSrcAreaUV >> 4, g_iDownAdjustRecon);
#else
            // reduce by 2
            I32_WMV i;
            for (i = 0; i < iFrmSrcAreaY; i++) {
                pRefY [i] = ((pRefY [i] - 128) >> 1) + 128;
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                pRefU [i] = ((pRefU [i] - 128) >> 1) + 128;
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                pRefV [i] = ((pRefV [i] - 128) >> 1) + 128;
            }
#endif
            pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
        }
    } else if (pWMVDec->m_iReconRangeState == 1) {
        if (pWMVDec->m_iRangeState == 0) {
            // increase by 2
#ifdef WMV_OPT_INTENSITYCOMP_ARM
            IntensityComp ((U32_WMV*)pRefY, iFrmSrcAreaY >> 4, g_iUpAdjustRecon);
            IntensityComp ((U32_WMV*)pRefU, iFrmSrcAreaUV >> 4, g_iUpAdjustRecon);
            IntensityComp ((U32_WMV*)pRefV, iFrmSrcAreaUV >> 4, g_iUpAdjustRecon);
#else
            I32_WMV i;
            for (i = 0; i < iFrmSrcAreaY; i++) {
                I32_WMV iTmp = (((I32_WMV)pRefY [i] - 128) << 1) + 128;
                pRefY [i] = g_rgiClapTabDec [iTmp];
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                I32_WMV iTmp = (((I32_WMV)pRefU [i] - 128) << 1) + 128;
                pRefU [i] = g_rgiClapTabDec [iTmp];
            }
            for (i = 0; i < iFrmSrcAreaUV; i++) {
                I32_WMV iTmp = (((I32_WMV)pRefV [i] - 128) << 1) + 128;
                pRefV [i] = g_rgiClapTabDec [iTmp];
            }
#endif
            pWMVDec->m_iReconRangeState = pWMVDec->m_iRangeState;
        }
    }
}


// used to adjust output
Void_WMV    AdjustDecRange(tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(AdjustDecRange);
   
    if (pWMVDec->m_iRangeState == 1){
        I32_WMV iFrmSrcAreaY = pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY;
        I32_WMV iFrmSrcAreaUV = pWMVDec->m_iWidthPrevUV * pWMVDec->m_iHeightPrevUV;
#if !defined(WMV_OPT_INTENSITYCOMP_ARM)
        I32_WMV i, iTmp;
#endif

//#ifdef _EMB_3FRAMES_

        if (pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bOutputOpEnabled == FALSE_WMV){
            RF_AdjustDecRangeCtl * pDecRangeCtl = (RF_AdjustDecRangeCtl *)&pWMVDec->m_pfrmPostQ->m_pOps[pWMVDec->m_pfrmPostQ->m_iNumOps++];

            pDecRangeCtl->m_iOpCode  = FRMOP_ADJUST_DEC_RANGE;

            pDecRangeCtl->m_pfrmSrc = pWMVDec->m_pfrmPostQ;
            pDecRangeCtl->m_iWidthPrevY = pWMVDec->m_iWidthPrevY;
            //pDecRangeCtl->m_iHeightPrevY = pWMVDec->m_iHeightPrevY;
            pDecRangeCtl->m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
            //pDecRangeCtl->m_iHeightPrevUV = pWMVDec->m_iHeightPrevUV;
            pDecRangeCtl->m_iRangeState = pWMVDec->m_iRangeState;

            pDecRangeCtl->m_iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevYXExpPlusExp;
            pDecRangeCtl->m_iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUVXExpPlusExp;
            pDecRangeCtl->m_uintNumMBY = pWMVDec->m_uintNumMBY ;

            return;
        }
//#endif

#ifdef WMV_OPT_INTENSITYCOMP_ARM
        IntensityComp((U32_WMV*)pWMVDec->m_ppxliPostQY, iFrmSrcAreaY >> 4, g_iUpAdjustRecon);
        IntensityComp((U32_WMV*)pWMVDec->m_ppxliPostQU, iFrmSrcAreaUV >> 4, g_iUpAdjustRecon);
        IntensityComp((U32_WMV*)pWMVDec->m_ppxliPostQV, iFrmSrcAreaUV >> 4, g_iUpAdjustRecon);
#else
        for (i = 0; i < iFrmSrcAreaY; i++){
            iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQY [i] - 128) << 1) + 128;
            pWMVDec->m_ppxliPostQY [i] = g_rgiClapTabDec [iTmp];
        }
        for (i = 0; i < iFrmSrcAreaUV; i++){
            iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQU [i] - 128) << 1) + 128;
            pWMVDec->m_ppxliPostQU [i] = g_rgiClapTabDec [iTmp];
        }
        for (i = 0; i < iFrmSrcAreaUV; i++){
            iTmp = (((I32_WMV)pWMVDec->m_ppxliPostQV [i] - 128) << 1) + 128;
            pWMVDec->m_ppxliPostQV [i] = g_rgiClapTabDec [iTmp];
        }
#endif
    }
}

#endif


Void_WMV CopyFramesMBRow (U8_WMV* ppxliDestQY, 
                            U8_WMV* ppxliDestQU, 
                            U8_WMV* ppxliDestQV, 
                            U8_WMV* ppxliSrcQY, 
                            U8_WMV* ppxliSrcQU, 
                            U8_WMV* ppxliSrcQV,
                             I32_WMV m_iMBSizeXWidthPrevY, 
                            I32_WMV m_iBlkSizeXWidthPrevUV
                            )
{
    ALIGNED32_FASTMEMCPY(ppxliDestQY, ppxliSrcQY, m_iMBSizeXWidthPrevY);
    ALIGNED32_FASTMEMCPY(ppxliDestQU, ppxliSrcQU, m_iBlkSizeXWidthPrevUV);
    ALIGNED32_FASTMEMCPY(ppxliDestQV, ppxliSrcQV, m_iBlkSizeXWidthPrevUV);
}

Void_WMV    AdjustDecRangeMBRow(U8_WMV* ppxliPostQY, 
                            U8_WMV* ppxliPostQU, 
                            U8_WMV* ppxliPostQV, 
                            I32_WMV m_iMBSizeXWidthPrevY, 
                            I32_WMV m_iBlkSizeXWidthPrevUV, 
                            I32_WMV m_iRangeState)
{

    if (m_iRangeState == 1){
        
        I32_WMV i, iTmp;
        
        for (i = 0; i < m_iMBSizeXWidthPrevY; i++)
        {
            iTmp = (((I32_WMV)ppxliPostQY [i] - 128) << 1) + 128;
            ppxliPostQY [i] = g_rgiClapTabDec [iTmp];
        }
        
        for (i = 0; i < m_iBlkSizeXWidthPrevUV; i++){
            iTmp = (((I32_WMV)ppxliPostQU [i] - 128) << 1) + 128;
            ppxliPostQU [i] = g_rgiClapTabDec [iTmp];
        }
        for (i = 0; i < m_iBlkSizeXWidthPrevUV; i++){
            iTmp = (((I32_WMV)ppxliPostQV [i] - 128) << 1) + 128;
            ppxliPostQV [i] = g_rgiClapTabDec [iTmp];
        }
        
    }


}

#if !defined( WMV_OPT_REPEATPAD_ARM )
Void_WMV g_memcpy(U8_WMV* ppxliLeftTop, U8_WMV* ppxliSrc, I32_WMV iWidthPrevY, I32_WMV iTrueWidth, I32_WMV iVertPad)
{
    I32_WMV a,b,c,d;
	I32_WMV iRowNum = 0;
	U8_WMV* ppxliDst;
   
    do
    {
		iRowNum = iVertPad;
    	ppxliDst = (U8_WMV*) ppxliLeftTop;	        

		a = *(U32_WMV *) &ppxliSrc[0];
        b = *(U32_WMV *) &ppxliSrc[4];
        c = *(U32_WMV *) &ppxliSrc[8];
        d = *(U32_WMV *) &ppxliSrc[12];	 

        do
        {
			*(U32_WMV *) &ppxliDst[0] = a;	  
			*(U32_WMV *) &ppxliDst[4] = b;
			*(U32_WMV *) &ppxliDst[8] = c;
			*(U32_WMV *) &ppxliDst[12] = d;		    		      	          	
			ppxliDst += iWidthPrevY;
        } while ( --iRowNum != 0 );

		ppxliSrc     += 16;
        ppxliLeftTop += 16;
		iTrueWidth   -= 16;
	} while ( iTrueWidth >= 16 );	

	if (iTrueWidth != 0) {	//last 8 columns.
		assert( iTrueWidth == 8 ); //iTrueWidth must be 8 here.
		iRowNum = iVertPad;
		ppxliDst = (U8_WMV*) ppxliLeftTop;	        
		a = *(U32_WMV *) &ppxliSrc[0];
		b = *(U32_WMV *) &ppxliSrc[4];
		do 
		{
			*(U32_WMV *) &ppxliDst[0] = a;	  
			*(U32_WMV *) &ppxliDst[4] = b;
			ppxliDst += iWidthPrevY;
		} while ( --iRowNum != 0 );	
	}
}

Void_WMV g_RepeatRef0Y_LeftRight ( 
	const U8_WMV* pLeft, 
	const U8_WMV* pRight, 
	U8_WMV* pDst, 
	I32_WMV iWidthPrevY,	
	I32_WMV iWidthYPlusExp,  
	I32_WMV iRowNum
)
{
	U32_WMV uipadValueLeft;
	U32_WMV uipadValueRight;
	U8_WMV * pDstPlusWidth = pDst + iWidthYPlusExp;

	do
	{ // x-direction interpolation

		uipadValueLeft  = *pLeft; 
		uipadValueRight = *pRight;
		uipadValueLeft  |= (uipadValueLeft << 8);
		uipadValueRight |= (uipadValueRight << 8);
		uipadValueLeft  |= (uipadValueLeft << 16);
		uipadValueRight |= (uipadValueRight << 16);

		// left Y
		((U32_WMV *)pDst)[0]  = uipadValueLeft;
		((U32_WMV *)pDst)[1]  = uipadValueLeft;
		((U32_WMV *)pDst)[2]  = uipadValueLeft;
		((U32_WMV *)pDst)[3] = uipadValueLeft;		    
		((U32_WMV *)pDst)[4] = uipadValueLeft;
		((U32_WMV *)pDst)[5] = uipadValueLeft;
		((U32_WMV *)pDst)[6] = uipadValueLeft;
		((U32_WMV *)pDst)[7] = uipadValueLeft;		
		// right Y
		((U32_WMV *)pDstPlusWidth)[0] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[1] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[2] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[3] = uipadValueRight;		    
		((U32_WMV *)pDstPlusWidth)[4] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[5] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[6] = uipadValueRight;
		((U32_WMV *)pDstPlusWidth)[7] = uipadValueRight;		    
			
		pDst		  += iWidthPrevY;
		pDstPlusWidth += iWidthPrevY;
		pLeft		  += iWidthPrevY;
		pRight		  += iWidthPrevY;
	} while ( --iRowNum != 0 );
}

Void_WMV g_RepeatRef0UV_LeftRight ( 
	const U8_WMV* pLeftU, 
	const U8_WMV* pRightU, 
	const U8_WMV* pLeftV, 
	const U8_WMV* pRightV, 
	U8_WMV* pDstU, 
	U8_WMV* pDstV, 
	I32_WMV iWidthPrevUV,	
	I32_WMV iWidthUVPlusExp,  
	I32_WMV iRowNum
)
{
	U32_WMV uipadValueLeft;
	U32_WMV uipadValueRight;
	U8_WMV * pDstUPlusWidth = pDstU + iWidthUVPlusExp;
	U8_WMV * pDstVPlusWidth = pDstV + iWidthUVPlusExp;

	do 
    { // x-direction interpolation
        uipadValueLeft = *pLeftU;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueLeft |= (uipadValueLeft << 16);

        uipadValueRight = *pRightU;
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueRight |= (uipadValueRight << 16);
		
		// left U
        ((U32_WMV *)pDstU)[0] = uipadValueLeft;
        ((U32_WMV *)pDstU)[1] = uipadValueLeft;
        ((U32_WMV *)pDstU)[2] = uipadValueLeft;
        ((U32_WMV *)pDstU)[3] = uipadValueLeft;            
		// right U
        ((U32_WMV *)pDstUPlusWidth)[0] = uipadValueRight;
        ((U32_WMV *)pDstUPlusWidth)[1] = uipadValueRight;
        ((U32_WMV *)pDstUPlusWidth)[2] = uipadValueRight;
        ((U32_WMV *)pDstUPlusWidth)[3] = uipadValueRight;  

        uipadValueLeft = *pLeftV;
        uipadValueLeft |= (uipadValueLeft << 8);
        uipadValueLeft |= (uipadValueLeft << 16);
		
        uipadValueRight = *pRightV;
        uipadValueRight |= (uipadValueRight << 8);
        uipadValueRight |= (uipadValueRight << 16);

		// left V
        ((U32_WMV *)pDstV)[0] = uipadValueLeft;
        ((U32_WMV *)pDstV)[1] = uipadValueLeft;
        ((U32_WMV *)pDstV)[2] = uipadValueLeft;
        ((U32_WMV *)pDstV)[3] = uipadValueLeft;            
		// right V
        ((U32_WMV *)pDstVPlusWidth)[0] = uipadValueRight;
        ((U32_WMV *)pDstVPlusWidth)[1] = uipadValueRight;
        ((U32_WMV *)pDstVPlusWidth)[2] = uipadValueRight;
        ((U32_WMV *)pDstVPlusWidth)[3] = uipadValueRight;            

		pDstU += iWidthPrevUV;       
        pDstV += iWidthPrevUV;      
		pDstUPlusWidth += iWidthPrevUV;       
        pDstVPlusWidth += iWidthPrevUV;      

        pLeftU += iWidthPrevUV;
        pLeftV += iWidthPrevUV;
        pRightU += iWidthPrevUV;
        pRightV += iWidthPrevUV;
    } while ( --iRowNum != 0 );
}
#endif


Void_WMV g_RepeatRef0Y (
    U8_WMV* ppxlcRef0Y,
    CoordI  iStart, 
    CoordI  iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthY,
    I32_WMV     iWidthYPlusExp,
    I32_WMV     iWidthPrevY,
    Bool_WMV    bProgressive
)
{
	const U8_WMV* ppxliOldLeft = ppxlcRef0Y + iOldLeftOffet;
	const U8_WMV* ppxliOldRight = ppxliOldLeft + iWidthY - 1;
	const U8_WMV* ppxliOldTopLn = ppxliOldLeft - EXPANDY_REFVOP;
	U8_WMV* ppxliNew = (U8_WMV*) ppxliOldTopLn;
	
	I32_WMV iVertPad = EXPANDY_REFVOP;
	I32_WMV iTrueWidth = iWidthPrevY;
	//I32_WMV iLastVert = 0;  // Hack ... remove once fix is ready

	
	CoordI y = iEnd - iStart;

#if 0 //It will cause bug when pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP. wshao.2009.12.01
    if (iWidthPrevY - iWidthYPlusExp != 32) {
         iVertPad = (EXPANDY_REFVOP >> 1) + 2; // Must be interlaced frame
         iTrueWidth = iWidthPrevY >> 1;
         ppxlcRef0Y -= iWidthPrevY * 2;
		
		//  Part of brutal hack. Remove once real fix is ready
		if (iEnd & 15)
			iLastVert = 16 - (iEnd & 15);
	}
#endif

	g_RepeatRef0Y_LeftRight_Fun(ppxliOldLeft, ppxliOldRight, ppxliNew, iWidthPrevY, iWidthYPlusExp, y);
	ppxliNew += y * iWidthPrevY;

    {
		//I32_WMV iWidthPrevYDiv8 = iTrueWidth >> 3;
		U8_WMV* ppxliSrc;

		if (fTop) 
		{
			U8_WMV* ppxliLeftTop = ppxlcRef0Y;
			ppxliSrc = (U8_WMV*) ppxliOldTopLn;		
			g_memcpy_Fun(ppxliLeftTop, ppxliSrc, iWidthPrevY, iTrueWidth, iVertPad);		        
		}

		if (fBottom) 
		{
			const U8_WMV* ppxliOldBotLn = ppxliNew - iWidthPrevY;

	//		iVertPad += iLastVert; // Part of hack ... remove when fix is ready.	                
	        ppxliSrc = (U8_WMV*) ppxliOldBotLn;
			g_memcpy_Fun(ppxliNew, ppxliSrc, iWidthPrevY, iTrueWidth, iVertPad);	    	    	
	     }
	}
}


Void_WMV g_RepeatRef0UV (
    U8_WMV* ppxlcRef0U,
    U8_WMV* ppxlcRef0V,
    CoordI  iStart, CoordI iEnd,
    I32_WMV     iOldLeftOffet,
    Bool_WMV    fTop, 
    Bool_WMV    fBottom,
    I32_WMV     iWidthUV,
    I32_WMV     iWidthUVPlusExp,
    I32_WMV     iWidthPrevUV,
    Bool_WMV    bProgressive

)
{
    const U8_WMV* ppxliOldLeftU = ppxlcRef0U + iOldLeftOffet;
    const U8_WMV* ppxliOldLeftV = ppxlcRef0V + iOldLeftOffet;
    const U8_WMV* ppxliOldRightU = ppxliOldLeftU + iWidthUV - 1;
    const U8_WMV* ppxliOldRightV = ppxliOldLeftV + iWidthUV - 1;
    const U8_WMV* ppxliOldTopLnU = ppxliOldLeftU - EXPANDUV_REFVOP;
    const U8_WMV* ppxliOldTopLnV = ppxliOldLeftV - EXPANDUV_REFVOP;
    U8_WMV* ppxliNewU = (U8_WMV*) ppxliOldTopLnU;
    U8_WMV* ppxliNewV = (U8_WMV*) ppxliOldTopLnV;
    
    CoordI y = iEnd - iStart;
    
    I32_WMV iLastVert = 0; // Hack


    I32_WMV iVertPad = EXPANDUV_REFVOP;
    I32_WMV iTrueWidth = iWidthPrevUV;
    
#if 0 //It will cause bug when pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP. wshao.2009.12.01
    if ((iWidthPrevUV - iWidthUVPlusExp) != 16) 
    {
        iVertPad = (EXPANDUV_REFVOP >> 1) + 1;    // Must be interlaced frame
        iTrueWidth = iWidthPrevUV >> 1;
        ppxlcRef0U -= iWidthPrevUV;
        ppxlcRef0V -= iWidthPrevUV;
                // Hack
        if (iEnd & 7)
            iLastVert = 8 - (iEnd & 7);
    }
#endif

	g_RepeatRef0UV_LeftRight_Fun ( ppxliOldLeftU, ppxliOldRightU, ppxliOldLeftV, ppxliOldRightV, 
							ppxliNewU, ppxliNewV, iWidthPrevUV,	iWidthUVPlusExp, y);
	ppxliNewU += y * iWidthPrevUV;
	ppxliNewV += y * iWidthPrevUV;

   {      
        //I32_WMV iWidthPrevUVDiv4 = iTrueWidth >> 2;

        U8_WMV* ppxliSrcU;
        U8_WMV* ppxliSrcV;
        if (fTop)
        {
            U8_WMV* ppxliLeftTopU = ppxlcRef0U;
            U8_WMV* ppxliLeftTopV = ppxlcRef0V;

            ppxliSrcU = (U8_WMV*) ppxliOldTopLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldTopLnV;
			g_memcpy_Fun(ppxliLeftTopU,ppxliSrcU, iWidthPrevUV, iTrueWidth, iVertPad);
			g_memcpy_Fun(ppxliLeftTopV,ppxliSrcV, iWidthPrevUV, iTrueWidth, iVertPad);			    	       
        }
        if (fBottom) 
        {
            const U8_WMV* ppxliOldBotLnU = ppxliNewU - iWidthPrevUV;
            const U8_WMV* ppxliOldBotLnV = ppxliNewV - iWidthPrevUV;

            iVertPad += iLastVert;  // Hack
            ppxliSrcU = (U8_WMV*) ppxliOldBotLnU;
            ppxliSrcV = (U8_WMV*) ppxliOldBotLnV;

			g_memcpy_Fun(ppxliNewU,ppxliSrcU, iWidthPrevUV, iTrueWidth, iVertPad);
			g_memcpy_Fun(ppxliNewV,ppxliSrcV, iWidthPrevUV, iTrueWidth, iVertPad);	
        }
   }
}

Void_WMV AdjustDecRangeWMVA (tWMVDecInternalMember *pWMVDec)
{
    
    I32_WMV iFrmSrcAreaY = pWMVDec->m_iWidthPrevY * pWMVDec->m_iHeightPrevY;
    I32_WMV iFrmSrcAreaUV = pWMVDec->m_iWidthPrevUV * pWMVDec->m_iHeightPrevUV;
    I32_WMV iTmp, i, iMul;
	
    assert (pWMVDec->m_cvCodecVersion == WMVA);

    if (pWMVDec->m_bRangeRedY_Flag) {
        assert (pWMVDec->m_iRangeRedY > 0 && pWMVDec->m_iRangeRedY < 9);
        iMul = pWMVDec->m_iRangeRedY + 8;
     if(pWMVDec->m_bCodecIsWVC1){
        for (i = 0; i < iFrmSrcAreaY; i++) {
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQY [i] - 128) * iMul) +4)>> 3) + 128;
            pWMVDec->m_ppxliPostQY [i] = g_rgiClapTabDec [iTmp];
        }
     }
     else {
       for (i = 0; i < iFrmSrcAreaY; i++) {
            iTmp = ((((I32_WMV)pWMVDec->m_ppxliPostQY [i] - 128) * iMul) >> 3) + 128;
            pWMVDec->m_ppxliPostQY [i] = g_rgiClapTabDec [iTmp];
        }
     }
    }

    if (pWMVDec->m_bRangeRedUV_Flag) {
        assert (pWMVDec->m_iRangeRedUV > 0 && pWMVDec->m_iRangeRedUV < 9);
        iMul = pWMVDec->m_iRangeRedUV + 8;
     if(pWMVDec->m_bCodecIsWVC1){
        for (i = 0; i < iFrmSrcAreaUV; i++) {
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQU [i] - 128) * iMul) +4)>> 3) + 128;
            pWMVDec->m_ppxliPostQU [i] = g_rgiClapTabDec [iTmp];
        }
     }
     else {
        for (i = 0; i < iFrmSrcAreaUV; i++) {
            iTmp = ((((I32_WMV)pWMVDec->m_ppxliPostQU [i] - 128) * iMul) >> 3) + 128;
            pWMVDec->m_ppxliPostQU [i] = g_rgiClapTabDec [iTmp];
        }
      }

     if(pWMVDec->m_bCodecIsWVC1){
        for (i = 0; i < iFrmSrcAreaUV; i++) {
            iTmp = (((((I32_WMV)pWMVDec->m_ppxliPostQV [i] - 128) * iMul ) +4)>> 3) + 128;
            pWMVDec->m_ppxliPostQV [i] = g_rgiClapTabDec [iTmp];
        }   
     }
     else {
       for (i = 0; i < iFrmSrcAreaUV; i++) {
            iTmp = ((((I32_WMV)pWMVDec->m_ppxliPostQV [i] - 128) * iMul ) >> 3) + 128;
            pWMVDec->m_ppxliPostQV [i] = g_rgiClapTabDec [iTmp];
        }   
      }
    }

}

#ifndef WMV9_SIMPLE_ONLY

Void_WMV SetMotionFieldBFrame (I16_WMV *pMvX, I16_WMV *pMvY,
                           I16_WMV *pFMvX, I16_WMV *pFMvY,
                           I16_WMV *pBMvX, I16_WMV *pBMvY,
                           I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX, I32_WMV iBotMvY,
                           I32_WMV iFTopMvX, I32_WMV iFTopMvY, I32_WMV iFBotMvX, I32_WMV iFBotMvY,
                           I32_WMV iBTopMvX, I32_WMV iBTopMvY, I32_WMV iBBotMvX, I32_WMV iBBotMvY,
                           I32_WMV iTopLeftBlkIndex, I32_WMV iBotLeftBlkIndex
                           )
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetMotionFieldBFrame);

    pMvX [iTopLeftBlkIndex] = (I16_WMV) iTopMvX;
    pMvY [iTopLeftBlkIndex] = (I16_WMV) iTopMvY;                    
    pMvX [iTopLeftBlkIndex + 1] = (I16_WMV) iTopMvX;
    pMvY [iTopLeftBlkIndex + 1] = (I16_WMV) iTopMvY;                    
    pMvX [iBotLeftBlkIndex] = (I16_WMV) iBotMvX;
    pMvY [iBotLeftBlkIndex] = (I16_WMV) iBotMvY;
    pMvX [iBotLeftBlkIndex + 1] = (I16_WMV) iBotMvX;
    pMvY [iBotLeftBlkIndex + 1] = (I16_WMV) iBotMvY;
    
    // backward
    pBMvX [iTopLeftBlkIndex] = (I16_WMV) iBTopMvX;
    pBMvY [iTopLeftBlkIndex] = (I16_WMV) iBTopMvY;                  
    pBMvX [iTopLeftBlkIndex + 1] = (I16_WMV) iBTopMvX;
    pBMvY [iTopLeftBlkIndex + 1] = (I16_WMV) iBTopMvY;                  
    pBMvX [iBotLeftBlkIndex] = (I16_WMV) iBBotMvX;
    pBMvY [iBotLeftBlkIndex] = (I16_WMV) iBBotMvY;
    pBMvX [iBotLeftBlkIndex + 1] = (I16_WMV) iBBotMvX;
    pBMvY [iBotLeftBlkIndex + 1] = (I16_WMV) iBBotMvY;
    
    // forward
    pFMvX [iTopLeftBlkIndex] = (I16_WMV) iFTopMvX;
    pFMvY [iTopLeftBlkIndex] = (I16_WMV) iFTopMvY;                  
    pFMvX [iTopLeftBlkIndex + 1] = (I16_WMV) iFTopMvX;
    pFMvY [iTopLeftBlkIndex + 1] = (I16_WMV) iFTopMvY;                  
    pFMvX [iBotLeftBlkIndex] = (I16_WMV) iFBotMvX;
    pFMvY [iBotLeftBlkIndex] = (I16_WMV) iFBotMvY;
    pFMvX [iBotLeftBlkIndex + 1] = (I16_WMV) iFBotMvX;
    pFMvY [iBotLeftBlkIndex + 1] = (I16_WMV) iFBotMvY;
    
}
#endif //WMV9_SIMPLE_ONLY

Void_WMV Repeatpad (tWMVDecInternalMember *pWMVDec)
{
    if (pWMVDec->m_bInterlaceV2 && !pWMVDec->m_bFieldMode) {
        // repeat pad the bottom field of the reference frame

        FUNCTION_PROFILE_DECL_START(fpRep,DECODEREPEATPAD_PROFILE)
        (*g_pRepeatRef0Y) (
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevY, 0, pWMVDec->m_iHeightYRepeatPad >> 1,
            pWMVDec->m_iWidthPrevYXExpPlusExp, TRUE, TRUE, pWMVDec->m_iWidthYRepeatPad,
            pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY << 1, !pWMVDec->m_bInterlaceV2);
        (*g_pRepeatRef0UV) (
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUV, pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUV,
            0, pWMVDec->m_iHeightUVRepeatPad >> 1, pWMVDec->m_iWidthPrevUVXExpPlusExp, TRUE, TRUE,
            pWMVDec->m_iWidthUVRepeatPad, pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV << 1, !pWMVDec->m_bInterlaceV2);
        FUNCTION_PROFILE_STOP(&fpRep)

        // repeat pad the top field of the reference frame

        FUNCTION_PROFILE_DECL_START(fpRep,DECODEREPEATPAD_PROFILE)
        (*g_pRepeatRef0Y) (
            pWMVDec->m_ppxliCurrQY, 0, pWMVDec->m_iHeightYRepeatPad >> 1,
            pWMVDec->m_iWidthPrevYXExpPlusExp, TRUE, TRUE, pWMVDec->m_iWidthYRepeatPad,
            pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY << 1, !pWMVDec->m_bInterlaceV2);
        (*g_pRepeatRef0UV) (
            pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV,
            0, pWMVDec->m_iHeightUVRepeatPad >> 1, pWMVDec->m_iWidthPrevUVXExpPlusExp, TRUE, TRUE,
            pWMVDec->m_iWidthUVRepeatPad, pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV << 1, !pWMVDec->m_bInterlaceV2);
        FUNCTION_PROFILE_STOP(&fpRep)
    }
    else {

        if (!pWMVDec->m_bInterlaceV2)
            pWMVDec->m_iCurrentField = 0;


    FUNCTION_PROFILE_DECL_START(fpRep,DECODEREPEATPAD_PROFILE)
        (*g_pRepeatRef0Y) (pWMVDec->m_ppxliCurrQY + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevY >> 1), 0, 
            pWMVDec->m_iHeightYRepeatPad, pWMVDec->m_iWidthPrevYXExpPlusExp,
            TRUE, TRUE, pWMVDec->m_iWidthYRepeatPad, pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY, !pWMVDec->m_bInterlaceV2);
        (*g_pRepeatRef0UV) (pWMVDec->m_ppxliCurrQU + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1),
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1), 0,
            pWMVDec->m_iHeightUVRepeatPad, pWMVDec->m_iWidthPrevUVXExpPlusExp,
            TRUE, TRUE, pWMVDec->m_iWidthUVRepeatPad, pWMVDec->m_iWidthUVPlusExp, pWMVDec->m_iWidthPrevUV, !pWMVDec->m_bInterlaceV2);
    FUNCTION_PROFILE_STOP(&fpRep)
    }
}
