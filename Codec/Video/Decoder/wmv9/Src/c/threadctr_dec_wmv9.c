//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 1996, 1997, 1998  Microsoft Corporation

Module Name:

        vopmbEnc.cpp

Abstract:

        Encoder VOP composed of MB's

Author:

        Ming-Chieh Lee (mingcl@microsoft.com)
        Bruce Lin (blin@microsoft.com)
        Wei-ge Chen (wchen@microsoft.com)
                        
        20-March-1996

Revision History:
        Bruce Lin (blin@microsoft.com)
        Rewrite
                textureDecodePMB ();
        11/04/96


*************************************************************************/
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#ifdef macintosh
#include "cpudetect_mac.h"
#include "motioncomp_altivec.h"
#endif
#include "tables_wmv.h"

#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "idctdec_wmv2.h"
#include "repeatpad_wmv.h"
#include "postfilter_wmv.h"

#include "spatialpredictor_wmv.h"
#include "localhuffman_wmv.h"
#include "tables_wmv.h"
#include "wmvdec_function.h"

#include "limits.h"
//#include "windows.h"
#include "voWmvPort.h"
//#include <fstream.h>
//#include <iostream.h>

#define STOP_MALLOC_FIRST 0
// while many Target CPU's only come in single proc configuratiions (like all targets in WinCE)
// some housekeeping is doen in this tread specific code so one must call at least init and close


Void_WMV CloseThreads (tWMVDecInternalMember * pWMVDec){ 
    
    pWMVDec->m_bShutdownThreads = TRUE_WMV;

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO CloseThreads 1" );
#endif
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8RowFlagY0);
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x4RowFlagY0); 
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8RowFlagU0); //
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x4RowFlagU0);
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8RowFlagV0); //
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x4RowFlagV0);

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO CloseThreads 2" );
#endif
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8ColFlagY0);
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter4x8ColFlagY0);
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8ColFlagU0);
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter4x8ColFlagU0);
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter8x8ColFlagV0);
    FREE_PTR(pWMVDec, pWMVDec->m_puchFilter4x8ColFlagV0);

#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO CloseThreads 3" );
#endif
}
tWMVDecodeStatus InitThreads (tWMVDecInternalMember * pWMVDec){

//    I32_WMV iLoopFHeight;

	pWMVDec->m_iStartY0 = 0; pWMVDec->m_iEndY0 = pWMVDec->m_iHeightY / pWMVDec->m_uiNumProcessors;
	pWMVDec->m_iStartUV0 = 0; pWMVDec->m_iEndUV0 = pWMVDec->m_iHeightUV / pWMVDec->m_uiNumProcessors;
	//pWMVDec->m_uiMBStart0 = 0;
	pWMVDec->m_uiMBEnd0 = pWMVDec->m_uintNumMBY / pWMVDec->m_uiNumProcessors;
	pWMVDec->m_uiMBEndX0 = pWMVDec->m_uintNumMBX / pWMVDec->m_uiNumProcessors;

    // for update / interpolate
	pWMVDec->m_iOffsetPrevY0 = pWMVDec->m_iWidthPrevYXExpPlusExp;
	pWMVDec->m_iOffsetPrevUV0 = pWMVDec->m_iWidthPrevUVXExpPlusExp;

#if STOP_MALLOC_FIRST

    pWMVDec->m_puchFilter8x8RowFlagY0 = (U8_WMV*)wmvMalloc(pWMVDec, (pWMVDec->m_uiMBEnd0 * 2 - 1) * ((pWMVDec->m_iWidthY + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter8x4RowFlagY0 = (U8_WMV*)wmvMalloc(pWMVDec, (pWMVDec->m_uiMBEnd0 * 2) * ((pWMVDec->m_iWidthY + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter8x8RowFlagU0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uiMBEnd0 * 2 - 1) >> 1) * ((pWMVDec->m_iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x4RowFlagU0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uiMBEnd0 * 2) >> 1) * ((pWMVDec->m_iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x8RowFlagV0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uiMBEnd0 * 2 - 1) >> 1) * ((pWMVDec->m_iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x4RowFlagV0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uiMBEnd0 * 2) >> 1) * ((pWMVDec->m_iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    if (!pWMVDec->m_puchFilter8x8RowFlagY0 || !pWMVDec->m_puchFilter8x4RowFlagY0 || 
        !pWMVDec->m_puchFilter8x8RowFlagU0 || !pWMVDec->m_puchFilter8x4RowFlagU0 ||
        !pWMVDec->m_puchFilter8x8RowFlagV0 || !pWMVDec->m_puchFilter8x4RowFlagV0 )
        return WMV_BadMemory;
        
    
    iLoopFHeight  = pWMVDec->m_uiMBEnd0 * 16;
    pWMVDec->m_puchFilter8x8ColFlagY0 = (U8_WMV*)wmvMalloc(pWMVDec, (pWMVDec->m_uintNumMBX * 2 - 1) * ((iLoopFHeight + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter4x8ColFlagY0 = (U8_WMV*)wmvMalloc(pWMVDec, (pWMVDec->m_uintNumMBX * 2) * ((iLoopFHeight + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter8x8ColFlagU0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uintNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter4x8ColFlagU0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uintNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x8ColFlagV0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uintNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter4x8ColFlagV0 = (U8_WMV*)wmvMalloc(pWMVDec, ((pWMVDec->m_uintNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    if (!pWMVDec->m_puchFilter8x8ColFlagY0 || !pWMVDec->m_puchFilter4x8ColFlagY0 || 
        !pWMVDec->m_puchFilter8x8ColFlagU0 || !pWMVDec->m_puchFilter4x8ColFlagU0 ||
        !pWMVDec->m_puchFilter8x8ColFlagV0 || !pWMVDec->m_puchFilter4x8ColFlagV0 )
        return WMV_BadMemory;
#endif        
    return WMV_Succeeded;

}

#ifdef _MultiThread_Decode_

Void_WMV DecodeMultiThreads (tWMVDecInternalMember *pWMVDec, THREADTYPE_DEC iMultiThreadType)
{
    // set X8 intra coding mode (remember, framewise switch is possible!)
//pWMVDec->m_pQuantizeI = pWMVDec->m_bXintra8 ? quantizeI_X8 : quantizeI;

    pWMVDec->m_iMultiThreadType = iMultiThreadType;
    if (pWMVDec->m_uiNumProcessors == 1) {
        switch (pWMVDec->m_iMultiThreadType){
//            case DECODE:
//                decode0 ();
//		        SetEvent (pWMVDec->hDecDone0);
//                break;
            case RENDER:
		        RenderFrame0 (pWMVDec, pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV, pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV);
//		        process0 ();
                break;
            case DEBLOCK_RENDER:
                Deblock0 (pWMVDec, pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV);
		        RenderFrame0 (pWMVDec, pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliCurrQV, pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV);
//		        process0 ();
                break;
            case LOOPFILTER:
                loopfilter0 (pWMVDec);
                break;
            case DEBLOCK:
                 Deblock0 (pWMVDec, pWMVDec->m_ppxliPostQY, pWMVDec->m_ppxliPostQU, pWMVDec->m_ppxliPostQV);
                break;
            case REPEATPAD:
		        process0 (pWMVDec);
                break;
            case DEINTERLACE:
              //  (*pWMVDec->m_pDeinterlaceProc)(pWMVDec, pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliPostQY, 0, pWMVDec->m_iStartDeinterlace1);
                break;
        }
    }
#if !defined(NO_WINDOWS) && !defined(UNDER_CE) && 0
    else {
        if (pWMVDec->m_iMultiThreadType == DECODE){ // Reset Last Thread Ctrl of LpFltr
            ResetEvent(pWMVDec->hHorizontalLpFltrDone[pWMVDec->m_uiNumProcessors - 1]);
            ResetEvent(pWMVDec->hReconComplete);
        }

        //pWMVDec->m_iMultiThreadType = DECODE;
        if (pWMVDec->m_uiNumProcessors == 2){
            SetEvent (pWMVDec->hDecEvent0);
            SetEvent (pWMVDec->hDecEvent1);
            WaitForSingleObject (pWMVDec->hDecDone0, INFINITE);
            WaitForSingleObject (pWMVDec->hDecDone1, INFINITE);
        }
        else {
            SetEvent (pWMVDec->hDecEvent0);
            SetEvent (pWMVDec->hDecEvent1);
            SetEvent (pWMVDec->hDecEvent2);
            SetEvent (pWMVDec->hDecEvent3);
            WaitForSingleObject (pWMVDec->hDecDone0, INFINITE);
            WaitForSingleObject (pWMVDec->hDecDone1, INFINITE);
            WaitForSingleObject (pWMVDec->hDecDone2, INFINITE);
            WaitForSingleObject (pWMVDec->hDecDone3, INFINITE);
        }
    }
#endif
}

Void_WMV process0 (tWMVDecInternalMember *pWMVDec)
{
    (*g_pRepeatRef0Y ) (pWMVDec->m_ppxliRefYPreproc, pWMVDec->m_iStartY0, pWMVDec->m_iEndY0, pWMVDec->m_iOffsetPrevY0, TRUE_WMV, pWMVDec->m_uiNumProcessors == 1, pWMVDec->m_iWidthYRepeatPad, pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY);
    (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRefUPreproc, pWMVDec->m_ppxliRefVPreproc, pWMVDec->m_iStartUV0, pWMVDec->m_iEndUV0, pWMVDec->m_iOffsetPrevUV0, TRUE_WMV, pWMVDec->m_uiNumProcessors == 1,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
}

Void_WMV process1 (tWMVDecInternalMember *pWMVDec)
{
    (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRefYPreproc, pWMVDec->m_iStartY1, pWMVDec->m_iEndY1, pWMVDec->m_iOffsetPrevY1, FALSE_WMV, pWMVDec->m_uiNumProcessors == 2, pWMVDec->m_iWidthYRepeatPad, pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY);
    (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRefUPreproc, pWMVDec->m_ppxliRefVPreproc, pWMVDec->m_iStartUV1, pWMVDec->m_iEndUV1, pWMVDec->m_iOffsetPrevUV1, FALSE_WMV, pWMVDec->m_uiNumProcessors == 2,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
}

//Void_WMV tWMVDecInternalMember::process2 ()
//{
//    (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRefYPreproc, pWMVDec->m_iStartY2, pWMVDec->m_iEndY2, pWMVDec->m_iOffsetPrevY2, FALSE_WMV, FALSE_WMV, pWMVDec->m_iWidthYRepeatPad, pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY);
//    (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRefUPreproc, pWMVDec->m_ppxliRefVPreproc, pWMVDec->m_iStartUV2, pWMVDec->m_iEndUV2, pWMVDec->m_iOffsetPrevUV2, FALSE_WMV, FALSE_WMV,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
//}
//
//
//Void_WMV tWMVDecInternalMember::process3 ()
//{
//    (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRefYPreproc, pWMVDec->m_iStartY3, pWMVDec->m_iEndY3, pWMVDec->m_iOffsetPrevY3, FALSE_WMV, pWMVDec->m_uiNumProcessors == 4, pWMVDec->m_iWidthYRepeatPad, pWMVDec->m_iWidthYPlusExp, pWMVDec->m_iWidthPrevY);
//    (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRefUPreproc, pWMVDec->m_ppxliRefVPreproc, pWMVDec->m_iStartUV3, pWMVDec->m_iEndUV3, pWMVDec->m_iOffsetPrevUV3, FALSE_WMV, pWMVDec->m_uiNumProcessors == 4,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
//}


Void_WMV RenderFrame0 (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, 
    U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV)
{

    U8_WMV * ppxliRenderQY;
    U8_WMV * ppxliRenderQU;
    U8_WMV * ppxliRenderQV;
//#ifdef X9
    if (pWMVDec->m_bX9)
        pWMVDec->m_bDisplay_AllMB = TRUE_WMV;
//#endif // X9
    if (pWMVDec->m_bRenderFromPostBuffer){
        ppxliRenderQY = ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp; 
        ppxliRenderQU = ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
        ppxliRenderQV = ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
    }
    else{
        ppxliRenderQY = ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp; 
        ppxliRenderQU = ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
        ppxliRenderQV = ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
    }

#if 0 //WDZ needs more work here
    if (DeColorConvParams.g_fPackedDstBMP){
        if (pWMVDec->m_bDisplay_AllMB)
    		(this->*pWMVDec->m_pRenderFrame_AllMB_Packed_X1) (ppxliRenderQY, ppxliRenderQU, ppxliRenderQV, pWMVDec->m_pBMPBits, 0, pWMVDec->m_uiMBEnd0);
        else
            (this->*pWMVDec->m_pRenderFrame_NonSkipMB_Packed_X1) (ppxliRenderQY, ppxliRenderQU, ppxliRenderQV, pWMVDec->m_pBMPBits, 0, pWMVDec->m_uiMBEnd0);
    }
    else{
        U8* pBmpPtr = pWMVDec->m_pBMPBits; 
        U8* pBmpPtrU = pWMVDec->m_pBMPBits + DeColorConvParams.g_iBMPOffsetUDst0;
        U8* pBmpPtrV = pWMVDec->m_pBMPBits + DeColorConvParams.g_iBMPOffsetVDst0; 

		RenderFrame_Planar (ppxliRenderQY, ppxliRenderQU, ppxliRenderQV, pBmpPtr, pBmpPtrU, pBmpPtrV, 0, pWMVDec->m_uiMBEnd0);
    }

#endif

}

Void_WMV RenderFrame1 (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, 
    U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV)
{
    U8_WMV * ppxliRenderQY;
    U8_WMV * ppxliRenderQU;
    U8_WMV * ppxliRenderQV;
    if (pWMVDec->m_bRenderFromPostBuffer){
        ppxliRenderQY = ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrQYOffset1; 
        ppxliRenderQU = ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1; 
        ppxliRenderQV = ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1; 
    }
    else{
        ppxliRenderQY = ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrQYOffset1; 
        ppxliRenderQU = ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1; 
        ppxliRenderQV = ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1; 
    }


#if 0

    if (DeColorConvParams.g_fPackedDstBMP){
        U8 * pBmpPtr = pWMVDec->m_pBMPBits + pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstTimesMB; 
        //pWMVDec->m_pBMPBits = (U8 *) pDst + DeColorConvParams.g_iBMPOffsetDst0;
        assert((I32_WMV) pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstTimesMB == (DeColorConvParams.g_iBMPOffsetDst1 - DeColorConvParams.g_iBMPOffsetDst0));
        if (pWMVDec->m_bDisplay_AllMB)
    		(this->*pWMVDec->m_pRenderFrame_AllMB_Packed_X1) (ppxliRenderQY, ppxliRenderQU, ppxliRenderQV, pBmpPtr, pWMVDec->m_uiMBEnd0, pWMVDec->m_uiMBEnd1);
        else
            (this->*pWMVDec->m_pRenderFrame_NonSkipMB_Packed_X1) (ppxliRenderQY, ppxliRenderQU, ppxliRenderQV, pBmpPtr, pWMVDec->m_uiMBEnd0, pWMVDec->m_uiMBEnd1);

    }
    else{
        U8 * pBmpPtr = pWMVDec->m_pBMPBits + pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstTimesMB; 
        U8 * pBmpPtrU = pWMVDec->m_pBMPBits + DeColorConvParams.g_iBMPOffsetUDst0 + pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstUVTimesBlk;
        U8 * pBmpPtrV = pWMVDec->m_pBMPBits + DeColorConvParams.g_iBMPOffsetVDst0 + pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstUVTimesBlk; 
        assert((I32_WMV) pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstTimesMB == DeColorConvParams.g_iBMPOffsetDst1);
        assert(DeColorConvParams.g_iBMPOffsetUDst0 + (I32_WMV) pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstUVTimesBlk == DeColorConvParams.g_iBMPOffsetUDst1);
        assert(DeColorConvParams.g_iBMPOffsetVDst0 + (I32_WMV) pWMVDec->m_uiMBEnd0 * DeColorConvParams.g_iWidthBMPDstUVTimesBlk == DeColorConvParams.g_iBMPOffsetVDst1);

        RenderFrame_Planar (ppxliRenderQY, ppxliRenderQU, ppxliRenderQV, pBmpPtr, pBmpPtrU, pBmpPtrV, pWMVDec->m_uiMBEnd0, pWMVDec->m_uiMBEnd1);
    }
#endif

}

Void_WMV Deblock0 (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV)
{
#ifndef STINGER
    if (pWMVDec->m_bDisplay_AllMB)
        DeblockIFrame(pWMVDec,
            ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
            ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            0, pWMVDec->m_uiMBEnd0
        );
    else
        DeblockPFrame(pWMVDec,
            ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
            ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            0, pWMVDec->m_uiMBEnd0
        );
#endif
}

Void_WMV Deblock1 (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV)
{
#ifndef STINGER
    if (pWMVDec->m_bDisplay_AllMB)
        DeblockIFrame(pWMVDec,
            ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrQYOffset1,
            ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1,
            ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1,
            pWMVDec->m_uiMBEnd0, pWMVDec->m_uiMBEnd1
        );
    else
        DeblockPFrame(pWMVDec,
            ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrQYOffset1,
            ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1,
            ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrQUVOffset1,
            pWMVDec->m_uiMBEnd0, pWMVDec->m_uiMBEnd1
        );
#endif
}

//Void_WMV tWMVDecInternalMember::DeblockCurrToPost0 (U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV)
//{
//#ifndef STINGER
//    DeblockIFrameCurrToPost(
//        ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
//        ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
//        ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        0, pWMVDec->m_uiMBEnd0
//    );
//#endif
//}
//
//Void_WMV tWMVDecInternalMember::DeblockCurrToPost1 (U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV)
//{
//#ifndef STINGER
//    DeblockIFrameCurrToPost(
//        ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
//        ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
//        ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
//        pWMVDec->m_uiMBEnd0, pWMVDec->m_uiMBEnd1
//    );
//#endif
//}

Void_WMV loopfilter0 (tWMVDecInternalMember *pWMVDec)
{
#ifndef WMV9_SIMPLE_ONLY
    DeblockSLFrame (pWMVDec,
        pWMVDec->m_ppxliCurrQPlusExpY0, pWMVDec->m_ppxliCurrQPlusExpU0, pWMVDec->m_ppxliCurrQPlusExpV0, 
        pWMVDec->m_uiNumProcessors - 1, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uiMBEnd0); 
#endif // WMV9_SIMPLE_ONLY
}

Void_WMV loopfilter1 (tWMVDecInternalMember *pWMVDec)
{
#ifndef WMV9_SIMPLE_ONLY
    DeblockSLFrame (pWMVDec,
        pWMVDec->m_ppxliCurrQPlusExpY0, pWMVDec->m_ppxliCurrQPlusExpU0, pWMVDec->m_ppxliCurrQPlusExpV0,
        pWMVDec->m_uiNumProcessors - 1, 1, 1, pWMVDec->m_uintNumMBX, pWMVDec->m_uiMBEnd0, pWMVDec->m_uiMBEnd1);
#endif // WMV9_SIMPLE_ONLY
}


/*
Void_WMV tWMVDecInternalMember::loopfilter2 ()
{
#ifndef _OLD_DEBLOCK_
    (this->*pWMVDec->m_pDeblockSLFrame) (
        pWMVDec->m_ppxliCurrQPlusExpY0, pWMVDec->m_ppxliCurrQPlusExpU0, pWMVDec->m_ppxliCurrQPlusExpV0, g_rgiClapTabEnc,
        pWMVDec->m_uiNumProcessors - 1, 2, 1, pWMVDec->m_uintNumMBX, pWMVDec->m_uiMBEnd1, pWMVDec->m_uiMBEnd2);
#else
    (this->*pWMVDec->m_pDeblockSLFrame) (
        pWMVDec->m_ppxliCurrQPlusExpY0, pWMVDec->m_ppxliCurrQPlusExpU0, pWMVDec->m_ppxliCurrQPlusExpV0, g_rgiClapTabEnc,
        pWMVDec->m_uiNumProcessors - 1, 2, pWMVDec->m_uiMBEndX1, pWMVDec->m_uiMBEndX2, pWMVDec->m_uiMBEnd1, pWMVDec->m_uiMBEnd2);
#endif
}

Void_WMV tWMVDecInternalMember::loopfilter3 ()
{
#ifndef _OLD_DEBLOCK_
    (this->*pWMVDec->m_pDeblockSLFrame) (
        pWMVDec->m_ppxliCurrQPlusExpY0, pWMVDec->m_ppxliCurrQPlusExpU0, pWMVDec->m_ppxliCurrQPlusExpV0, g_rgiClapTabEnc,
        pWMVDec->m_uiNumProcessors - 1, 3, 1, pWMVDec->m_uintNumMBX, pWMVDec->m_uiMBEnd2, pWMVDec->m_uiMBEnd3);
#else
    (this->*pWMVDec->m_pDeblockSLFrame) (
        pWMVDec->m_ppxliCurrQPlusExpY0, pWMVDec->m_ppxliCurrQPlusExpU0, pWMVDec->m_ppxliCurrQPlusExpV0, g_rgiClapTabEnc,
        pWMVDec->m_uiNumProcessors - 1, 3, pWMVDec->m_uiMBEndX2, pWMVDec->m_uiMBEndX3, pWMVDec->m_uiMBEnd2, pWMVDec->m_uiMBEnd3);
#endif
}
*/

#endif // _MultiThread_Decode_

tWMVDecodeStatus initMultiThreadVars_Dec (tWMVDecInternalMember *pWMVDec)
{	
	// for encoding
//	I32_WMV result;
//	CRct rctMBY (0, 0, MB_SIZE, MB_SIZE);
//	CRct rctMBUV (0, 0, MB_SIZE >> 1, MB_SIZE >> 1);
	
	pWMVDec->m_iStartY0 = 0; pWMVDec->m_iEndY0 = pWMVDec->m_iHeightY / pWMVDec->m_uiNumProcessors;
	pWMVDec->m_iStartUV0 = 0; pWMVDec->m_iEndUV0 = pWMVDec->m_iHeightUV / pWMVDec->m_uiNumProcessors;
	//pWMVDec->m_uiMBStart0 = 0;
	pWMVDec->m_uiMBEnd0 = pWMVDec->m_uintNumMBY / pWMVDec->m_uiNumProcessors;
	pWMVDec->m_uiMBEndX0 = pWMVDec->m_uintNumMBX / pWMVDec->m_uiNumProcessors;

    // for update / interpolate
	pWMVDec->m_iOffsetPrevY0 = pWMVDec->m_iWidthPrevYXExpPlusExp;
	pWMVDec->m_iOffsetPrevUV0 = pWMVDec->m_iWidthPrevUVXExpPlusExp;

	if (pWMVDec->m_uiNumProcessors >= 2) {
		pWMVDec->m_iStartY1 = pWMVDec->m_iHeightY / pWMVDec->m_uiNumProcessors; pWMVDec->m_iEndY1 = pWMVDec->m_iHeightY / pWMVDec->m_uiNumProcessors * 2;
		pWMVDec->m_iStartUV1 = pWMVDec->m_iHeightUV / pWMVDec->m_uiNumProcessors; pWMVDec->m_iEndUV1 = pWMVDec->m_iHeightUV / pWMVDec->m_uiNumProcessors * 2;
		pWMVDec->m_iOffsetPrevY1 = pWMVDec->m_iWidthPrevYXExpPlusExp + (pWMVDec->m_iHeightY / pWMVDec->m_uiNumProcessors) * pWMVDec->m_iWidthPrevY;
		pWMVDec->m_iOffsetPrevUV1 = pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iHeightUV / pWMVDec->m_uiNumProcessors) * pWMVDec->m_iWidthPrevUV;
	}
    

	if (pWMVDec->m_uiNumProcessors >= 2) {
		pWMVDec->m_uiMBStart1 = pWMVDec->m_uintNumMBY / pWMVDec->m_uiNumProcessors;
        if (pWMVDec->m_uiNumProcessors == 4){
			pWMVDec->m_uiMBEnd1 = pWMVDec->m_uintNumMBY / pWMVDec->m_uiNumProcessors * 2;
        	pWMVDec->m_uiMBEndX1 = pWMVDec->m_uintNumMBX / pWMVDec->m_uiNumProcessors * 2;
        }
        else{
			pWMVDec->m_uiMBEnd1 = pWMVDec->m_uintNumMBY;
			pWMVDec->m_uiMBEndX1 = pWMVDec->m_uintNumMBX;
        }
		pWMVDec->m_iCurrQYOffset1 = pWMVDec->m_iWidthPrevY * MB_SIZE * pWMVDec->m_uiMBStart1;
		pWMVDec->m_iCurrQUVOffset1 = pWMVDec->m_iWidthPrevUV * BLOCK_SIZE * pWMVDec->m_uiMBStart1;
	}
	return WMV_Succeeded;
}

//Adjust thread-related variables and pointers whenever encoding size is changed by entry point header.
// Move memory allocation to AllocateMultiThreadBufs_Dec().
Void_WMV initMultiThreadVars_Dec_WMVA (tWMVDecInternalMember *pWMVDec)
{	
	// for encoding
//	I32_WMV result;
//	CRct rctMBY (0, 0, MB_SIZE, MB_SIZE);
//	CRct rctMBUV (0, 0, MB_SIZE >> 1, MB_SIZE >> 1);
	
	pWMVDec->m_iStartY0 = 0; pWMVDec->m_iEndY0 = pWMVDec->m_iHeightY / pWMVDec->m_uiNumProcessors;
	pWMVDec->m_iStartUV0 = 0; pWMVDec->m_iEndUV0 = pWMVDec->m_iHeightUV / pWMVDec->m_uiNumProcessors;
	//pWMVDec->m_uiMBStart0 = 0;
	pWMVDec->m_uiMBEnd0 = pWMVDec->m_uintNumMBY / pWMVDec->m_uiNumProcessors;
	pWMVDec->m_uiMBEndX0 = pWMVDec->m_uintNumMBX / pWMVDec->m_uiNumProcessors;

    // for update / interpolate
	pWMVDec->m_iOffsetPrevY0 = pWMVDec->m_iWidthPrevYXExpPlusExp;
	pWMVDec->m_iOffsetPrevUV0 = pWMVDec->m_iWidthPrevUVXExpPlusExp;

	
		pWMVDec->m_uiMBEnd1 = pWMVDec->m_uintNumMBY;          // needed for single thread render
   
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
        
	pWMVDec->m_ppxliCurrQPlusExpY0 = (U8_WMV*) (pWMVDec->m_ppxliCurrQY) + pWMVDec->m_iWidthPrevYXExpPlusExp;
	pWMVDec->m_ppxliCurrQPlusExpU0 = (U8_WMV*) (pWMVDec->m_ppxliCurrQU) + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->m_ppxliCurrQPlusExpV0 = (U8_WMV*) (pWMVDec->m_ppxliCurrQV) + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    
}

//allocate thread buffers according to max encoding size.
//This is only called when the max encoding size is changed by sequence header.
I32_WMV AllocateMultiThreadBufs_Dec_WMVA (tWMVDecInternalMember *pWMVDec, 
    I32_WMV iMaxEncWidth,
    I32_WMV iMaxEncHeight)
{	
	U32_WMV uiMaxMBEnd0;
    I32_WMV iWidthMBAligned = (iMaxEncWidth + 15) & ~15;
    I32_WMV iHeightMBAligned = (iMaxEncHeight + 15) & ~15;
    I32_WMV iWidthY = iWidthMBAligned;
    I32_WMV iWidthUV = iWidthMBAligned >> 1;
    I32_WMV iMaxNumMBX = iWidthMBAligned >> 4; // divided by 16, MB size
    I32_WMV iMaxNumMBY = iHeightMBAligned >> 4;
    I32_WMV iLoopFHeight;

    uiMaxMBEnd0 = iMaxNumMBY / pWMVDec->m_uiNumProcessors;

#ifdef XDM
	pWMVDec->m_puchFilter8x8RowFlagY0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += (uiMaxMBEnd0 * 2 - 1) * ((iWidthY + 31) >> 5);
	pWMVDec->m_puchFilter8x4RowFlagY0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff     += (uiMaxMBEnd0 * 2) * ((iWidthY + 31) >> 5);
	pWMVDec->m_puchFilter8x8RowFlagU0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += ((uiMaxMBEnd0 * 2 - 1) >> 1) * ((iWidthUV + 31) >> 5);
	pWMVDec->m_puchFilter8x4RowFlagU0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff     += ((uiMaxMBEnd0 * 2) >> 1) * ((iWidthUV + 31) >> 5);
	pWMVDec->m_puchFilter8x8RowFlagV0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += ((uiMaxMBEnd0 * 2 - 1) >> 1) * ((iWidthUV + 31) >> 5);
	pWMVDec->m_puchFilter8x4RowFlagV0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff     += ((uiMaxMBEnd0 * 2) >> 1) * ((iWidthUV + 31) >> 5);
    
    iLoopFHeight  = uiMaxMBEnd0 * 16;
	pWMVDec->m_puchFilter8x8ColFlagY0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += (iMaxNumMBX * 2 - 1) * ((iLoopFHeight + 31) >> 5);
	pWMVDec->m_puchFilter4x8ColFlagY0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += (iMaxNumMBX * 2) * ((iLoopFHeight + 31) >> 5);
	pWMVDec->m_puchFilter8x8ColFlagU0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += ((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5);
	pWMVDec->m_puchFilter4x8ColFlagU0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += ((iMaxNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5);
	pWMVDec->m_puchFilter8x8ColFlagV0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += ((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5);
	pWMVDec->m_puchFilter4x8ColFlagV0  = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff    += ((iMaxNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5);
#else
    pWMVDec->m_puchFilter8x8RowFlagY0 = (U8_WMV *)wmvMalloc(pWMVDec, (uiMaxMBEnd0 * 2 - 1) * ((iWidthY + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter8x4RowFlagY0 = (U8_WMV *)wmvMalloc(pWMVDec, (uiMaxMBEnd0 * 2) * ((iWidthY + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter8x8RowFlagU0 = (U8_WMV *)wmvMalloc(pWMVDec, ((uiMaxMBEnd0 * 2 - 1) >> 1) * ((iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x4RowFlagU0 = (U8_WMV *)wmvMalloc(pWMVDec, ((uiMaxMBEnd0 * 2) >> 1) * ((iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x8RowFlagV0 = (U8_WMV *)wmvMalloc(pWMVDec, ((uiMaxMBEnd0 * 2 - 1) >> 1) * ((iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x4RowFlagV0 = (U8_WMV *)wmvMalloc(pWMVDec, ((uiMaxMBEnd0 * 2) >> 1) * ((iWidthUV + 31) >> 5), DHEAP_STRUCT); 
    if (!pWMVDec->m_puchFilter8x8RowFlagY0 || !pWMVDec->m_puchFilter8x4RowFlagY0 || 
        !pWMVDec->m_puchFilter8x8RowFlagU0 || !pWMVDec->m_puchFilter8x4RowFlagU0 ||
        !pWMVDec->m_puchFilter8x8RowFlagV0 || !pWMVDec->m_puchFilter8x4RowFlagV0 )
        return ICERR_MEMORY;

    
    iLoopFHeight  = uiMaxMBEnd0 * 16;
    pWMVDec->m_puchFilter8x8ColFlagY0 = (U8_WMV *)wmvMalloc(pWMVDec, (iMaxNumMBX * 2 - 1) * ((iLoopFHeight + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter4x8ColFlagY0 = (U8_WMV *)wmvMalloc(pWMVDec, (iMaxNumMBX * 2) * ((iLoopFHeight + 31) >> 5), DHEAP_STRUCT);
    pWMVDec->m_puchFilter8x8ColFlagU0 = (U8_WMV *)wmvMalloc(pWMVDec, ((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter4x8ColFlagU0 = (U8_WMV *)wmvMalloc(pWMVDec, ((iMaxNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter8x8ColFlagV0 = (U8_WMV *)wmvMalloc(pWMVDec, ((iMaxNumMBX * 2 - 1) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    pWMVDec->m_puchFilter4x8ColFlagV0 = (U8_WMV *)wmvMalloc(pWMVDec, ((iMaxNumMBX * 2) >> 1) * (((iLoopFHeight >> 1) + 31) >> 5), DHEAP_STRUCT); 
    if (!pWMVDec->m_puchFilter8x8ColFlagY0 || !pWMVDec->m_puchFilter4x8ColFlagY0 || 
        !pWMVDec->m_puchFilter8x8ColFlagU0 || !pWMVDec->m_puchFilter4x8ColFlagU0 ||
        !pWMVDec->m_puchFilter8x8ColFlagV0 || !pWMVDec->m_puchFilter4x8ColFlagV0 )
        return ICERR_MEMORY;

#endif


	return ICERR_OK;
}
