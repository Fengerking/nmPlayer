//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_

#ifndef WMV9_SIMPLE_ONLY



#define CLIP(x) ((U32_WMV) x <= 255 ? x : (x < 0 ? 0: 255))

// Declare local functions
Void_WMV g_HorizDownsampleFilter9(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_HorizDownsampleFilter5(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_VertDownsampleFilter9(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_VertDownsampleFilter5(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_HorizUpsampleFilter7(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_HorizUpsampleFilter3(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_VertUpsampleFilter7(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_VertUpsampleFilter3(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf);
Void_WMV g_HorizDownsampleFilter6(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_HorizUpsampleFilter10(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_HorizUpsampleFilter10(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_VertDownsampleFilter6(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_VertUpsampleFilter10(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);

#ifdef _WMV_TARGET_X86_
extern Void_WMV g_DownsampleWFilterLine5(I32_WMV *x, I32_WMV size);
extern Void_WMV g_HorizDownsampleFilter9_KNI(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
extern Void_WMV g_VertDownsampleFilter9_KNI(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
extern Void_WMV g_HorizUpsampleFilter7_KNI(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
extern Void_WMV g_VertUpsampleFilter7_KNI(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
extern Void_WMV g_HorizDownsampleFilter6_MMX(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_VertDownsampleFilter6_MMX(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_VertUpsampleFilter10_MMX(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_HorizUpsampleFilter10_KNI(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
#endif

Void_WMV InitMultires(tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitMultires);
    pWMVDec->m_pHorizDownsampleFilter9 = &g_HorizDownsampleFilter9;
    pWMVDec->m_pVertDownsampleFilter9 = &g_VertDownsampleFilter9;
    pWMVDec->m_pHorizDownsampleFilter5 = &g_HorizDownsampleFilter5;
    pWMVDec->m_pVertDownsampleFilter5 = &g_VertDownsampleFilter5;
    pWMVDec->m_pHorizUpsampleFilter7 = &g_HorizUpsampleFilter7;
    pWMVDec->m_pVertUpsampleFilter7 = &g_VertUpsampleFilter7;
    pWMVDec->m_pHorizUpsampleFilter3 = &g_HorizUpsampleFilter3;
    pWMVDec->m_pVertUpsampleFilter3 = &g_VertUpsampleFilter3;
    pWMVDec->m_pHorizDownsampleFilter6 = &g_HorizDownsampleFilter6;
    pWMVDec->m_pVertDownsampleFilter6 = &g_VertDownsampleFilter6;
    pWMVDec->m_pHorizUpsampleFilter10 = &g_HorizUpsampleFilter10;
    pWMVDec->m_pVertUpsampleFilter10 = &g_VertUpsampleFilter10;


#if defined(_WMV_TARGET_X86_)
#ifndef WMV_DISABLE_SSE1
    if (g_SupportSSE1 ()) {
#ifndef WMV_DISABLE_SSE1
        pWMVDec->m_pHorizUpsampleFilter10 = &g_HorizUpsampleFilter10_KNI;
#endif
        //g_HorizDownsampleFilter6_MMX basically call to SSE1 functions. only set when SSE1 is enabled.
        pWMVDec->m_pHorizDownsampleFilter6 = &g_HorizDownsampleFilter6_MMX;
    }
#endif

#ifndef WMV_DISABLE_MMX
    if (g_SupportMMX ()) {
        pWMVDec->m_pVertUpsampleFilter10 = &g_VertUpsampleFilter10_MMX;
        pWMVDec->m_pVertDownsampleFilter6 = &g_VertDownsampleFilter6_MMX;
    }
#endif

/*
    if (g_SupportSSE1 ()) {
        pWMVDec->m_pHorizDownsampleFilter9 = &g_HorizDownsampleFilter9_KNI;
        pWMVDec->m_pVertDownsampleFilter9 = &g_VertDownsampleFilter9_KNI;
        pWMVDec->m_pHorizUpsampleFilter7   = &g_HorizUpsampleFilter7_KNI;
        pWMVDec->m_pVertUpsampleFilter7    = &g_VertUpsampleFilter7_KNI;
        pWMVDec->m_pHorizDownsampleFilter6 = &g_HorizDownsampleFilter6_KNI;
    }
*/

#endif

 //   if (pWMVDec->m_iMultiresFilter == 0)
    {
#if 0   
        pWMVDec->m_pHorizDownsampleFilter = pWMVDec->m_pHorizDownsampleFilter9;
        pWMVDec->m_pVertDownsampleFilter = pWMVDec->m_pVertDownsampleFilter9;
        pWMVDec->m_pHorizUpsampleFilter = pWMVDec->m_pHorizUpsampleFilter7;
        pWMVDec->m_pVertUpsampleFilter = pWMVDec->m_pVertUpsampleFilter7;
#else  
        pWMVDec->m_pHorizDownsampleFilter = pWMVDec->m_pHorizDownsampleFilter6;
        pWMVDec->m_pVertDownsampleFilter = pWMVDec->m_pVertDownsampleFilter6;
        pWMVDec->m_pHorizUpsampleFilter = pWMVDec->m_pHorizUpsampleFilter10;
        pWMVDec->m_pVertUpsampleFilter = pWMVDec->m_pVertUpsampleFilter10;
#endif

    }
	/*
    else
    {
        pWMVDec->m_pHorizDownsampleFilter = pWMVDec->m_pHorizDownsampleFilter5;
        pWMVDec->m_pVertDownsampleFilter = pWMVDec->m_pVertDownsampleFilter5;
        pWMVDec->m_pHorizUpsampleFilter = pWMVDec->m_pHorizUpsampleFilter3;
        pWMVDec->m_pVertUpsampleFilter = pWMVDec->m_pVertUpsampleFilter3;    
    }
	*/
}

/*
Void_WMV ChangeMultiresFilter(tWMVDecInternalMember *pWMVDec, I32_WMV iFilterIndex)
{
    if (iFilterIndex == 0)
    {
        pWMVDec->m_pHorizDownsampleFilter = pWMVDec->m_pHorizDownsampleFilter9;
        pWMVDec->m_pVertDownsampleFilter = pWMVDec->m_pVertDownsampleFilter9;
        pWMVDec->m_pHorizUpsampleFilter = pWMVDec->m_pHorizUpsampleFilter7;
        pWMVDec->m_pVertUpsampleFilter = pWMVDec->m_pVertUpsampleFilter7;
    }
    else
    {
        pWMVDec->m_pHorizDownsampleFilter = pWMVDec->m_pHorizDownsampleFilter5;
        pWMVDec->m_pVertDownsampleFilter = pWMVDec->m_pVertDownsampleFilter5;
        pWMVDec->m_pHorizUpsampleFilter = pWMVDec->m_pHorizUpsampleFilter3;
        pWMVDec->m_pVertUpsampleFilter = pWMVDec->m_pVertUpsampleFilter3;    
    }
}
*/

Void_WMV SetupMultiResParams(tWMVDecInternalMember *pWMVDec)
{
    // Calculate half res params
    I32_WMV iWidthY = ((pWMVDec->m_iWidthY >> 1) + 15) & ~15;
    I32_WMV iWidthUV = iWidthY >> 1;
    I32_WMV iHeightY = ((pWMVDec->m_iHeightY >> 1) + 15) & ~15;
    I32_WMV iHeightUV = iHeightY >> 1;
    
    I32_WMV iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc >> 1;
    I32_WMV iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc >> 1;
    I32_WMV iWidthYPlusExp = iWidthY + pWMVDec->EXPANDY_REFVOP;
    I32_WMV iWidthUVPlusExp;
    U32_WMV uiNumMBX = iWidthY >> 4;
    U32_WMV uiNumMBY = iHeightY >> 4;
    U32_WMV uiRightestMB = uiNumMBX - 1;
    I32_WMV iWidthPrevY ;
    I32_WMV iWidthPrevUV ;
    I32_WMV iHeightPrevY ;
    I32_WMV iHeightPrevUV ;
    I32_WMV iWidthPrevYXExpPlusExp ;
    I32_WMV iWidthPrevUVXExpPlusExp ;
    I32_WMV iMBSizeXWidthPrevY ;
    I32_WMV iBlkSizeXWidthPrevUV ;
    
    // Save parameters for full res (index = 0)
    MULTIRES_PARAMS* pMultiResParams = &pWMVDec->m_rMultiResParams[0];

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetupMultiResParams);
    
    if (pWMVDec->m_bYUV411) {
        iWidthUV = iWidthY >> 2;
        iHeightUV = iHeightY >> 2;
    }
    
    iWidthUVPlusExp = iWidthUV + pWMVDec->EXPANDUV_REFVOP;
#ifdef VIDEO_MEM_SHARE
	if( pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP ) 
	{
		iWidthPrevY = pWMVDec->m_frameBufStrideY;
		iWidthPrevUV = pWMVDec->m_frameBufStrideUV;
	}
	else
#endif
	{
		iWidthPrevY = iWidthY + (pWMVDec->EXPANDY_REFVOP << 1);
		iWidthPrevUV = iWidthUV + (pWMVDec->EXPANDUV_REFVOP << 1);
	}
    iHeightPrevY = iHeightY + (pWMVDec->EXPANDY_REFVOP << 1);
    iHeightPrevUV = iHeightUV + (pWMVDec->EXPANDUV_REFVOP << 1);
    iWidthPrevYXExpPlusExp = iWidthPrevY * pWMVDec->EXPANDY_REFVOP + pWMVDec->EXPANDY_REFVOP;
    iWidthPrevUVXExpPlusExp = iWidthPrevUV * pWMVDec->EXPANDUV_REFVOP + pWMVDec->EXPANDUV_REFVOP;
    iMBSizeXWidthPrevY = iWidthPrevY << 4;
    iBlkSizeXWidthPrevUV = iWidthPrevUV << 3;
    
    
    
    pMultiResParams->iWidthY = pWMVDec->m_iWidthY;
    pMultiResParams->iWidthUV = pWMVDec->m_iWidthUV;
    pMultiResParams->iHeightY = pWMVDec->m_iHeightY;
    pMultiResParams->iHeightUV = pWMVDec->m_iHeightUV;
    pMultiResParams->iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
    pMultiResParams->iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;
    pMultiResParams->iWidthYPlusExp = pWMVDec->m_iWidthYPlusExp;
    pMultiResParams->iWidthUVPlusExp = pWMVDec->m_iWidthUVPlusExp;
    //pMultiResParams->iAreaY = pWMVDec->m_iAreaY;
    pMultiResParams->bMBAligned = pWMVDec->m_bMBAligned;
    pMultiResParams->uiNumMBX = pWMVDec->m_uintNumMBX;
    pMultiResParams->uiNumMBY = pWMVDec->m_uintNumMBY;
    pMultiResParams->uiNumMB = pWMVDec->m_uintNumMB;
    pMultiResParams->uiRightestMB = pWMVDec->m_uiRightestMB;
    pMultiResParams->iWidthPrevY = pWMVDec->m_iWidthPrevY;
    pMultiResParams->iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
    pMultiResParams->iHeightPrevY = pWMVDec->m_iHeightPrevY;
    pMultiResParams->iHeightPrevUV = pWMVDec->m_iHeightPrevUV;
    pMultiResParams->iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevYXExpPlusExp;
    pMultiResParams->iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pMultiResParams->iMBSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY;
    pMultiResParams->iBlkSizeXWidthPrevUV = pWMVDec->m_iBlkSizeXWidthPrevUV;
    
    // Save parameters for half-horizontal, full-vertical res (index = 1)
    pMultiResParams = &pWMVDec->m_rMultiResParams[1];
    pMultiResParams->iWidthY = iWidthY;
    pMultiResParams->iWidthUV = iWidthUV;
    pMultiResParams->iHeightY = pWMVDec->m_iHeightY;
    pMultiResParams->iHeightUV = pWMVDec->m_iHeightUV;
    pMultiResParams->iFrmWidthSrc = iFrmWidthSrc;
    pMultiResParams->iFrmHeightSrc = pWMVDec->m_iFrmHeightSrc;
    pMultiResParams->iWidthYPlusExp = iWidthYPlusExp;
    pMultiResParams->iWidthUVPlusExp = iWidthUVPlusExp;
    //    pMultiResParams->iAreaY = iWidthY * pWMVDec->m_iHeightY;
    pMultiResParams->bMBAligned = (iWidthY == iFrmWidthSrc && pWMVDec->m_iHeightY == pWMVDec->m_iFrmHeightSrc);;
    pMultiResParams->uiNumMBX = uiNumMBX;
    pMultiResParams->uiNumMBY = pWMVDec->m_uintNumMBY;
    pMultiResParams->uiNumMB = uiNumMBX * pWMVDec->m_uintNumMBY;
    pMultiResParams->uiRightestMB = uiRightestMB;
    pMultiResParams->iWidthPrevY = iWidthPrevY;
    pMultiResParams->iWidthPrevUV = iWidthPrevUV;
    pMultiResParams->iHeightPrevY = pWMVDec->m_iHeightPrevY;
    pMultiResParams->iHeightPrevUV = pWMVDec->m_iHeightPrevUV;
    pMultiResParams->iWidthPrevYXExpPlusExp = iWidthPrevYXExpPlusExp;
    pMultiResParams->iWidthPrevUVXExpPlusExp = iWidthPrevUVXExpPlusExp;
    pMultiResParams->iMBSizeXWidthPrevY = iMBSizeXWidthPrevY;
    pMultiResParams->iBlkSizeXWidthPrevUV = iBlkSizeXWidthPrevUV;
    
    // Save parameters for full-horizontal, half-vertical res (index = 2)
    pMultiResParams = &pWMVDec->m_rMultiResParams[2];
    pMultiResParams->iWidthY = pWMVDec->m_iWidthY;
    pMultiResParams->iWidthUV = pWMVDec->m_iWidthUV;
    pMultiResParams->iHeightY = iHeightY;
    pMultiResParams->iHeightUV = iHeightUV;
    pMultiResParams->iFrmWidthSrc = pWMVDec->m_iFrmWidthSrc;
    pMultiResParams->iFrmHeightSrc = iFrmHeightSrc;
    pMultiResParams->iWidthYPlusExp = pWMVDec->m_iWidthYPlusExp;
    pMultiResParams->iWidthUVPlusExp = pWMVDec->m_iWidthUVPlusExp;
    //    pMultiResParams->iAreaY = pWMVDec->m_iWidthY * iHeightY;
    pMultiResParams->bMBAligned = (pWMVDec->m_iWidthY == pWMVDec->m_iFrmWidthSrc && iHeightY == iFrmHeightSrc);;
    pMultiResParams->uiNumMBX = pWMVDec->m_uintNumMBX;
    pMultiResParams->uiNumMBY = uiNumMBY;
    pMultiResParams->uiNumMB = pWMVDec->m_uintNumMBX * uiNumMBY;
    pMultiResParams->uiRightestMB = pWMVDec->m_uiRightestMB;
    pMultiResParams->iWidthPrevY = pWMVDec->m_iWidthPrevY;
    pMultiResParams->iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
    pMultiResParams->iHeightPrevY = iHeightPrevY;
    pMultiResParams->iHeightPrevUV = iHeightPrevUV;
    pMultiResParams->iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevYXExpPlusExp;
    pMultiResParams->iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pMultiResParams->iMBSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY;
    pMultiResParams->iBlkSizeXWidthPrevUV = pWMVDec->m_iBlkSizeXWidthPrevUV;
    
    // Save parameters for half-horizontal, half-vertical res (index = 3)
    pMultiResParams = &pWMVDec->m_rMultiResParams[3];
    pMultiResParams->iWidthY = iWidthY;
    pMultiResParams->iWidthUV = iWidthUV;
    pMultiResParams->iHeightY = iHeightY;
    pMultiResParams->iHeightUV = iHeightUV;
    pMultiResParams->iFrmWidthSrc = iFrmWidthSrc;
    pMultiResParams->iFrmHeightSrc = iFrmHeightSrc;
    pMultiResParams->iWidthYPlusExp = iWidthYPlusExp;
    pMultiResParams->iWidthUVPlusExp = iWidthUVPlusExp;
    //    pMultiResParams->iAreaY = iWidthY * iHeightY;
    pMultiResParams->bMBAligned = (iWidthY == iFrmWidthSrc && iHeightY == iFrmHeightSrc);;
    pMultiResParams->uiNumMBX = uiNumMBX;
    pMultiResParams->uiNumMBY = uiNumMBY;
    pMultiResParams->uiNumMB = uiNumMBX * uiNumMBY;
    pMultiResParams->uiRightestMB = uiRightestMB;
    pMultiResParams->iWidthPrevY = iWidthPrevY;
    pMultiResParams->iWidthPrevUV = iWidthPrevUV;
    pMultiResParams->iHeightPrevY = iHeightPrevY;
    pMultiResParams->iHeightPrevUV = iHeightPrevUV;
    pMultiResParams->iWidthPrevYXExpPlusExp = iWidthPrevYXExpPlusExp;
    pMultiResParams->iWidthPrevUVXExpPlusExp = iWidthPrevUVXExpPlusExp;
    pMultiResParams->iMBSizeXWidthPrevY = iMBSizeXWidthPrevY;
    pMultiResParams->iBlkSizeXWidthPrevUV = iBlkSizeXWidthPrevUV;
}

Void_WMV ResetParams(tWMVDecInternalMember *pWMVDec, I32_WMV iResIndex)
{
    MULTIRES_PARAMS* pMultiResParams = &pWMVDec->m_rMultiResParams[iResIndex];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ResetParams);
 
    pWMVDec->m_iWidthY = pMultiResParams->iWidthY;
    pWMVDec->m_iWidthUV = pMultiResParams->iWidthUV;
    pWMVDec->m_iHeightY = pMultiResParams->iHeightY;
    pWMVDec->m_iHeightUV = pMultiResParams->iHeightUV;
    pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iWidthY;
    pWMVDec->m_iWidthUVRepeatPad = pWMVDec->m_iWidthUV;
    pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iHeightY;
    pWMVDec->m_iHeightUVRepeatPad = pWMVDec->m_iHeightUV;
    pWMVDec->m_iFrmWidthSrc = pMultiResParams->iFrmWidthSrc;
    pWMVDec->m_iFrmHeightSrc = pMultiResParams->iFrmHeightSrc;
    if (pWMVDec->m_bAdvancedProfile) {
        pWMVDec->m_iWidthUVRepeatPad = (pWMVDec->m_iFrmWidthSrc + 1) >> 1;
        pWMVDec->m_iWidthYRepeatPad = pWMVDec->m_iWidthUVRepeatPad * 2;
        pWMVDec->m_iHeightUVRepeatPad = (pWMVDec->m_iFrmHeightSrc + 1) >> 1;
        pWMVDec->m_iHeightYRepeatPad = pWMVDec->m_iHeightUVRepeatPad * 2;
    }

    pWMVDec->m_iWidthYPlusExp = pMultiResParams->iWidthYPlusExp;
    pWMVDec->m_iWidthUVPlusExp = pMultiResParams->iWidthUVPlusExp;
 //   pWMVDec->m_iAreaY = pMultiResParams->iAreaY;
    pWMVDec->m_bMBAligned = pMultiResParams->bMBAligned;
    pWMVDec->m_uintNumMBX = pMultiResParams->uiNumMBX;
    pWMVDec->m_uintNumMBY = pMultiResParams->uiNumMBY;
    pWMVDec->m_uintNumMB = pMultiResParams->uiNumMB;
    pWMVDec->m_uiRightestMB = pMultiResParams->uiRightestMB;
    pWMVDec->m_iWidthPrevY = pMultiResParams->iWidthPrevY;
    pWMVDec->m_iWidthPrevUV = pMultiResParams->iWidthPrevUV;
    pWMVDec->m_iHeightPrevY = pMultiResParams->iHeightPrevY;
    pWMVDec->m_iHeightPrevUV = pMultiResParams->iHeightPrevUV;
    pWMVDec->m_iWidthPrevYXExpPlusExp = pMultiResParams->iWidthPrevYXExpPlusExp;
    pWMVDec->m_iWidthPrevUVXExpPlusExp = pMultiResParams->iWidthPrevUVXExpPlusExp;
//    pWMVDec->m_iNumOfTotalMVPerRow = pWMVDec->m_uintNumMBX; 
	pWMVDec->m_iMBSizeXWidthPrevY = pMultiResParams->iMBSizeXWidthPrevY;
	pWMVDec->m_iBlkSizeXWidthPrevUV = pMultiResParams->iBlkSizeXWidthPrevUV;

    // Add these to param struct (calculated for now)
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp; 
}

static const I32_WMV s_iRowIncTbl[6] = {0, 1, 2, 2, 2, 1};
static const I32_WMV s_iColIncTbl[6] = {0, 2, 1, 2, 1, 2};


Void_WMV DownsampleRefFrame(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV)
{
    I32_WMV iWidth = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthY;
    I32_WMV iHeight = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iHeightY;
    I32_WMV iSrcPitchY = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevY;
    I32_WMV iSrcPitchUV = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUV;
    I32_WMV iRowInc = s_iRowIncTbl[pWMVDec->m_iResIndex + pWMVDec->m_iResIndexPrev];
    I32_WMV iColInc = s_iColIncTbl[pWMVDec->m_iResIndex + pWMVDec->m_iResIndexPrev];
    I32_WMV i, j, k;
    I32_WMV iWidthUV;
    I32_WMV iHeightUV;
    U8_WMV* pSrcFiltY = pSrcY;
    U8_WMV* pSrcFiltU = pSrcU;
    U8_WMV* pSrcFiltV = pSrcV;
    U8_WMV* pDstFiltY = pDstY;
    U8_WMV* pDstFiltU = pDstU;
    U8_WMV* pDstFiltV = pDstV;
    U8_WMV* pSrc ;
    U8_WMV* pDst ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DownsampleRefFrame);

    // If we are downsampling in either dimension, the downsampled dimension must be
    // a multiple of 16.
    if (iColInc == 2)
        iWidth = (iWidth + 31) & ~31;
    if (iRowInc == 2)
        iHeight = (iHeight + 31) & ~31;


    if (pWMVDec->m_bYUV411) {
        iWidthUV = iWidth >> 2;
        iHeightUV = iHeight;
    }
    else {
        iWidthUV = iWidth >> 1;
        iHeightUV = iHeight >> 1;
    }



    if (iColInc == 2)
    {
         

        (*pWMVDec->m_pHorizDownsampleFilter) (pSrcFiltY, pSrcFiltU, pSrcFiltV, pDstFiltY, pDstFiltU, pDstFiltV, iWidth, iHeight, iWidthUV, iHeightUV, iSrcPitchY, iSrcPitchUV, pWMVDec->m_pAlignedMultiresBuffer);
        if (iRowInc == 2)
        {
            pSrcFiltY = pDstFiltY;
            pSrcFiltU = pDstFiltU;
            pSrcFiltV = pDstFiltV;
            pDstFiltY = pSrcY;
            pDstFiltU = pSrcU;
            pDstFiltV = pSrcV;
        }
    }

    if (iRowInc == 2)
        (*pWMVDec->m_pVertDownsampleFilter) (pSrcFiltY, pSrcFiltU, pSrcFiltV, pDstFiltY, pDstFiltU, pDstFiltV, iWidth, iHeight,  iWidthUV, iHeightUV, iSrcPitchY, iSrcPitchUV, pWMVDec->m_pAlignedMultiresBuffer);

     pSrc = pDstFiltY;
     pDst = pDstY;
    for (i = 0; i < iHeight; i += iRowInc)
    {
        for (j = 0, k = 0; j < iWidth; j += iColInc, k++)
            pDst[k] = pSrc[j];
        pSrc += iSrcPitchY*iRowInc;
        pDst += pWMVDec->m_iWidthPrevY;
    }

    pSrc = pDstFiltU;
    pDst = pDstU;
    for (i = 0; i < iHeightUV ; i += iRowInc)
    {
        for (j = 0, k = 0; j < (iWidthUV); j += iColInc, k++)
            pDst[k] = pSrc[j];
        pSrc += iSrcPitchUV*iRowInc;
        pDst += pWMVDec->m_iWidthPrevUV;
    }

    pSrc = pDstFiltV;
    pDst = pDstV;
    for (i = 0; i < iHeightUV; i += iRowInc)
    {
        for (j = 0, k = 0; j < (iWidthUV); j += iColInc, k++)
            pDst[k] = pSrc[j];
        pSrc += iSrcPitchUV*iRowInc;
        pDst += pWMVDec->m_iWidthPrevUV;
    }
}


#define WT1 -406  // 2^8  * -1.58613
#define WT2 -217  // 2^12 * -0.05298
#define WT3 226   // 2^8  * 0.882911
#define WT4 227   // 2^9  * 0.443506
#define NORM1 26  // 2^5  * (1.1496/sqrt(2))
#define NORM2 315 // 2^8


//////////////////////////////////////////////////////////////////////////////////////
//
// This function is a refferance function for the 9 tap filter which calculates 
// the error in the exact calculation and the one produced by the optimized version
// a floating point version of this filter would be ideal here 
//
// This code is not thread safe and is only for verification purposes
//
//To use this code to check the optimized versions you must:
//#define _DEVATION_
//
//////////////////////////////////////////////////////////////////////////////////////
I32_WMV g_DownsampleWFilterLine9_ErrorSum=0;
I32_WMV g_DownsampleWFilterLine9_ErrorSqSum=0;
I32_WMV g_DownsampleWFilterLine9_ErrorCnt=0;
Float_WMV g_DownsampleWFilterLine9_Error=0;
Void_WMV g_DownsampleWFilterLine9_FullPressitionRef(U8_WMV *pSrc, U8_WMV *pDst, I32_WMV size)
{
	I32_WMV j;
    I32_WMV x[4096];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DownsampleWFilterLine9_FullPressitionRef);
    for (j = 1; j < size-1; j += 2) 
		x[j] = ((pSrc[j]<<8) + WT1*(pSrc[j-1]+pSrc[j+1])); // shift 8 bshift 0 resshift 8
	x[size-1] = (pSrc[size-1]<<8) + WT1*(pSrc[size-2]<<1);
	
	x[0] = ((pSrc[0]<<20) + WT2*(x[1]<<1))>>12; // shift20 bshift12 res 8
	for (j = 2; j < size; j += 2)
		x[j] = (pSrc[j]<<8) + ((WT2*((x[j-1]+x[j+1])))>>12); // res 8

	for ( j = 1; j < size-1; j += 2)
		x[j] =((x[j]<<8) + WT3*(x[j-1]+x[j+1]))>>8; // shift 8 bshift8 res 8
	x[size-1] = ((x[size-1]<<8) + WT3*(x[size-2]<<1))>>8; // shift 8 bshift 4 res  8

	x[0] = ((x[0]<<9) + WT4*(x[1]<<1))>>9; // shift 9 bshift 9 res shift 8
	for (j = 2; j < size; j += 2)
		x[j] = ((x[j]<<9)+ WT4*(x[j-1]+x[j+1]))>>9; // shift 10 bshift 9 res8


	for (j = 0; j < size; j += 2) {
		x[j] = ((x[j]*NORM1)>>5);
		x[j] = (x[j]+128)>>8; // wrong rounding if x is less than -0.5, but that case is anyway handled by clipping
	    x[j] = CLIP(x[j]);
    }

    //Calculate squared error and here for reporting on quality
    g_DownsampleWFilterLine9_ErrorSqSum += (pDst[j]-x[j])*(pDst[j]-x[j]);
    g_DownsampleWFilterLine9_ErrorSum += (pDst[j]-x[j]);
    g_DownsampleWFilterLine9_ErrorCnt++;
    g_DownsampleWFilterLine9_Error = ((float)g_DownsampleWFilterLine9_ErrorSqSum/
                                            (float)g_DownsampleWFilterLine9_ErrorCnt);
}


//
// Scaled-down version that keeps intermediate results in 16 bits and matches MMX implementation
//
Void_WMV g_DownsampleWFilterLine9 (U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch)
{
#if 1

  U8_WMV *p;
  I32_WMV j;
   I32_WMV t;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DownsampleWFilterLine9);

  p = pSrc;
  for (j = 1; j < size-1; j += 2)
  {
    x[j] = (p[iPitch]<<4) + ((WT1*(p[0]+p[iPitch*2])) >> 4);
    p += iPitch * 2;
  }
  x[size-1] = (p[iPitch]<<4) + ((WT1*p[0]) >> 3);

  p = pSrc;
  x[0] = (p[0]<<5) + ((WT2*x[1])>>10);
  for (j = 2; j < size; j += 2)
  {
    pSrc += iPitch*2;
    x[j] = (pSrc[0]<<5) + ((WT2*(x[j-1]+x[j+1]))>>11);
  }


  for ( j = 1; j < size-1; j += 2)
    x[j] += (WT3*(x[j-1]+x[j+1]))>>9;
  x[size-1] += (WT3*x[size-2])>>8;


  p = pDst;
  t = x[1];
  for (j = 0; j < size; j += 2)
  {
    t = x[j] + ((WT4*(t + x[j+1]))>>8); // shift 10 bshift 9 res8
    t = ((t + (512 + NORM1/2)/NORM1) * NORM1) >> 10;
    if ((U32_WMV) t > 255)
    {
      if (t < 0)
        t = 0;
      else
        t = 255;
    }
    p[0] = (U8_WMV) t;
    p[iPitch] = 0;
    p += iPitch * 2;
    t = x[j+1];
  }

#else

  //
  // memoryless version used by vertical MMX version. size should be at least 6.
  //

  I32_WMV j, x1, x2, x3, x4, x5;

  x3 = (pSrc[iPitch]<<4) + ((WT1*(pSrc[0]+pSrc[iPitch*2])) >> 4); // shift 8 bshift 0 resshift 8
  x4 = (pSrc[0]<<5)      + ((WT2*x3)>>10); // shift20 bshift12 res 8
  pSrc += iPitch * 2;

  x1 = (pSrc[iPitch]<<4) + ((WT1*(pSrc[0]+pSrc[iPitch*2])) >> 4);
  x2 = (pSrc[0]<<5)      + ((WT2*(x3 + x1)) >> 11);
  pSrc += iPitch * 2;

  x3 += ((WT3/2)*(x4 + x2)) >> 8;
  x5 = x3;
  x4 += (WT4    *(x5 + x3)) >> 8;

  x4 = ((x4 + (512 + NORM1/2)/NORM1) * NORM1) >> 10;
  if ((U32_WMV) x4 > 255)
  {
    if (x4 > 255)
      x4 = 255;
    else
      x4 = 0;
  }
  pDst[0] = (U8_WMV) x4;
  pDst[iPitch] = 0;
  pDst += iPitch * 2;

  x5 = x3;
  x4 = x2;
  x3 = x1;

  for (j = 6; j < size; j += 2)
  {
    x1 = (pSrc[iPitch]<<4) + ((WT1*(pSrc[0]+pSrc[iPitch*2])) >> 4);
    x2 = (pSrc[0]<<5)      + ((WT2*(x3 + x1)) >> 11);
    pSrc += iPitch * 2;

    x3 += ((WT3/2)*(x4 + x2)) >> 8;
    x4 += (WT4    *(x5 + x3)) >> 8;

    x4 = ((x4 + (512 + NORM1/2)/NORM1) * NORM1) >> 10;
    if ((U32_WMV) x4 > 255)
    {
     if (x4 > 255)
       x4 = 255;
     else
       x4 = 0;
    }
    pDst[0] = (U8_WMV) x4;
    pDst[iPitch] = 0;
    pDst += iPitch * 2;

    x5 = x3;
    x4 = x2;
    x3 = x1;
  }

  x1 = (pSrc[iPitch]<<4) + ((WT1*pSrc[0]) >> 3);
  x2 = (pSrc[0]<<5)      + ((WT2*(x3 + x1 )) >> 11);

  x3 += ((WT3/2)*(x4 + x2)) >> 8;
  x4 += (WT4    *(x5 + x3)) >> 8;

  x4 = ((x4 + (512 + NORM1/2)/NORM1) * NORM1) >> 10;
  if ((U32_WMV) x4 > 255)
  {
    if (x4 > 255)
      x4 = 255;
    else
      x4 = 0;
  }
  pDst[0] = (U8_WMV) x4;
  pDst[iPitch] = 0;
  pDst += iPitch * 2;

  x1 += (WT3*x2) >> 8;
  x2 += (WT4*(x3+x1)) >> 8;

  x2 = ((x2 + (512 + NORM1/2)/NORM1) * NORM1) >> 10;
  if ((U32_WMV) x2 > 255)
  {
    if (x2 > 255)
      x2 = 255;
    else
      x2 = 0;
  }
  pDst[0] = (U8_WMV) x2;
  pDst[iPitch] = 0;

#endif
}


Void_WMV g_DownsampleWFilterLine5(I32_WMV *x, I32_WMV size)
{

	I32_WMV j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DownsampleWFilterLine5);
	for (j = 1; j < size-1; j += 2)
		x[j] =((x[j]<<8) - ((x[j-1]+x[j+1])<<7)); // shift 8 bshift 0 resshift 8
	x[size-1] = (x[size-1]<<8) - (x[size-2]<<8);
	

	x[0] = (x[0]<<8) + (x[1]>>1); // shift20 bshift12 res 8
	for (j = 2; j < size; j += 2)
		x[j] = (x[j]<<8) + ((x[j-1]+x[j+1])>>2); // res 8

	for (j = 0; j < size; j += 2) {
		x[j] = ((x[j]+128)>>8); // wrong rounding if x is less than -0.5, but that case is anyway handled by clipping
	    x[j] = CLIP(x[j]);
		x[j+1] = 0; // zeroing high freq coeffs.
	}

}

Void_WMV g_HorizDownsampleFilter9(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i;

    U8_WMV* pSrc = pSrcY;
    U8_WMV* pDst = pDstY;

    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizDownsampleFilter9);

    for (i = 0; i < iHeight; i++)
    {
        g_DownsampleWFilterLine9(pDst, pSrc, buffer, iWidth, 1);
        pSrc += iPitchY;
        pDst += iPitchY;
    }
    pSrc = pSrcU;
    pDst = pDstU;
    for (i = 0; i < iHeightUV; i++)
    {
        g_DownsampleWFilterLine9(pDst, pSrc, buffer, iWidthUV, 1);
        pSrc += iPitchUV;
        pDst += iPitchUV;
    }
    pSrc = pSrcV;
    pDst = pDstV;
    for (i = 0; i < iHeightUV; i++)
    {
        g_DownsampleWFilterLine9(pDst, pSrc, buffer, iWidthUV, 1);
        pSrc += iPitchUV;
        pDst += iPitchUV;
    }
}

/////////////////////////////////////////////////////////////////////////////////
// SIMD version of DownsampleWFilterLine9 eliminates need for intermediate
// buffer by calculating each coefficent multiplication 1 itteration ahead of
// the next so that the output of each step can be used ad the input for the
// next step.
// 
// note: in this code it is ok for pDst to be the same pointer as pSrc 
//
// To Do: Implement this code with 4 at a time MMX. It already uses 16 bit
//        math so this should be fairly trivial.
//
//////////////////////////////////////////////////////////////////////////


Void_WMV g_HorizDownsampleFilter5(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i, j;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    U8_WMV* pDst = pDstY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizDownsampleFilter5);
    
    for (i = 0; i < iHeight; i++)
    {
        for (j = 0; j < iWidth; j ++)
            buffer[j] = (I32_WMV) pSrc[j];
        g_DownsampleWFilterLine5(buffer, iWidth);
        for (j=0; j < iWidth; j ++)
            pDst[j] = (U8_WMV) buffer[j];
        pSrc += iPitchY;
        pDst += iPitchY;
    }
    pSrc = pSrcU;
    pDst = pDstU;
    for (i = 0; i < iHeightUV; i++)
    {
        for (j = 0; j < iWidthUV; j ++)
            buffer[j] = (I32_WMV) pSrc[j];
        g_DownsampleWFilterLine5(buffer, iWidthUV);
        for (j=0; j < iWidthUV; j ++)
            pDst[j] = (U8_WMV) buffer[j];
        pSrc += iPitchUV;
        pDst += iPitchUV;
    }
    pSrc = pSrcV;
    pDst = pDstV;
    for (i = 0; i < iHeightUV; i++)
    {
        for (j = 0; j < iWidthUV; j ++)
            buffer[j] = (I32_WMV) pSrc[j];
        g_DownsampleWFilterLine5(buffer, iWidthUV);
        for (j=0; j < iWidthUV; j ++)
            pDst[j] = (U8_WMV) buffer[j];
        pSrc += iPitchUV;
        pDst += iPitchUV;
    }
    
}

// This version of vertical upsampling uses column by column conversion; is memory intensive.
// To do: A low memory vertical upsampler.
Void_WMV g_VertDownsampleFilter9(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV j;
    U8_WMV* pSrc = pSrcY;
    U8_WMV* pDst = pDstY;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertDownsampleFilter9);

    for (j = 0; j < iWidth; j++)
        g_DownsampleWFilterLine9(pDst + j, pSrc + j, buffer, iHeight, iPitchY);
    pSrc = pSrcU;
    pDst = pDstU;
    for (j = 0; j < iWidthUV; j++)
        g_DownsampleWFilterLine9(pDst + j, pSrc + j, buffer, iHeightUV, iPitchUV);
    pSrc = pSrcV;
    pDst = pDstV;
    for (j = 0; j < iWidthUV; j++)
        g_DownsampleWFilterLine9(pDst + j, pSrc + j, buffer, iHeightUV, iPitchUV);
}

Void_WMV g_VertDownsampleFilter5(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i, j;
    U8_WMV* pSrc = pSrcY;
	U8_WMV* pDst = pDstY;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertDownsampleFilter5);
    
	for (j = 0; j < iWidth; j++) {
		for (i = 0; i < iHeight; i ++)
			buffer[i] = (I32_WMV) pSrc[i*iPitchY+j];
		g_DownsampleWFilterLine5(buffer, iHeight);
		for (i = 0; i < iHeight; i++)
			pDst[i*iPitchY+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcU;
	pDst = pDstU;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i ++)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_DownsampleWFilterLine5(buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pDst[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcV;
	pDst = pDstV;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i ++)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_DownsampleWFilterLine5(buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pDst[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }

}

Void_WMV UpsampleRefFrame(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV) 
{

    I32_WMV iWidthPrevY = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevY;
    I32_WMV iWidthPrevUV = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUV;
    I32_WMV iRowInc = s_iRowIncTbl[pWMVDec->m_iResIndex + pWMVDec->m_iResIndexPrev];
    I32_WMV iColInc = s_iColIncTbl[pWMVDec->m_iResIndex + pWMVDec->m_iResIndexPrev];
    I32_WMV i, j, k;
    U8_WMV* pSrc = pSrcY;
    U8_WMV* pDst = pDstY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(UpsampleRefFrame);

    for (i = 0; i < pWMVDec->m_iHeightY; i += iRowInc)
    {
        for (j = 0, k = 0; j < pWMVDec->m_iWidthY; j += iColInc, k++)
            pDst[j] = pSrc[k];
        pSrc += iWidthPrevY;
        pDst += pWMVDec->m_iWidthPrevY*iRowInc;
    }
    pSrc = pSrcU;
    pDst = pDstU;
    for (i = 0; i < pWMVDec->m_iHeightUV; i += iRowInc)
    {
        for (j = 0, k = 0; j < pWMVDec->m_iWidthUV; j += iColInc, k++)
            pDst[j] = pSrc[k];
        pSrc += iWidthPrevUV;
        pDst += pWMVDec->m_iWidthPrevUV*iRowInc;
    }
    pSrc = pSrcV;
    pDst = pDstV;
    for (i = 0; i < pWMVDec->m_iHeightUV; i += iRowInc)
    {
        for (j = 0, k = 0; j < pWMVDec->m_iWidthUV; j += iColInc, k++)
            pDst[j] = pSrc[k];
        pSrc += iWidthPrevUV;
        pDst += pWMVDec->m_iWidthPrevUV*iRowInc;
    }

    if (iColInc == 2)
        (*pWMVDec->m_pHorizUpsampleFilter)(pDstY, pDstU, pDstV, iRowInc, pWMVDec->m_iWidthY, pWMVDec->m_iHeightY, pWMVDec->m_iWidthUV, pWMVDec->m_iHeightUV, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pAlignedMultiresBuffer);
    if (iRowInc == 2)
        (*pWMVDec->m_pVertUpsampleFilter)(pDstY, pDstU, pDstV, pWMVDec->m_iWidthY, pWMVDec->m_iHeightY, pWMVDec->m_iWidthUV, pWMVDec->m_iHeightUV, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pAlignedMultiresBuffer);
}

Void_WMV g_UpsampleWFilterLine10_Horiz_MBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV size);
Void_WMV g_HorizUpsampleFilter10_MBRow(
    U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV,
    I32_WMV iDstWidth, I32_WMV iDstWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV, 
    I32_WMV iDstPitchY, I32_WMV iDstPitchUV);
Void_WMV g_UpsampleWFilterLine10_Vert_MBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst, I32_WMV size);
Void_WMV g_UpsampleWFilterLine10_Vert_FirstMBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst,  I32_WMV size);
Void_WMV g_UpsampleWFilterLine10_Vert_LastMBRow(U8_WMV *pDst, U8_WMV *pSrc,  I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst,  I32_WMV size);
Void_WMV g_VertUpsampleFilter10_FirstMBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV);
Void_WMV g_VertUpsampleFilter10_LastMBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV, I32_WMV iLastMBOffset);
Void_WMV g_VertUpsampleFilter10_MBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV);



#define UWT1 -406 // 2^8
#define UWT2 -217 // 2^12
#define UWT3 226 // 2^8
#define UNORM2 315 // 2^8

#define AKADATCH 1

#if AKADATCH

Void_WMV g_UpsampleWFilterLine7 (U8_WMV *pDst, U8_WMV *pSrc, I32_WMV iSize, I32_WMV *x, I32_WMV iPitch)
{
#if 1
  I32_WMV j;
  U8_WMV *p;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine7);

  x += 1;	// reserve space

  p = pSrc;
  for (j = 0; j < iSize; j += 2)
  {
    x[j] = (p[0]*UNORM2) >> 4;
    p += iPitch * 2;
  }

  x[iSize] = x[iSize-2];
  for (j = 1; j < iSize+1; j += 2)
    x[j] = (UWT3*(x[j-1] + x[j+1])) >> 8;

  x[-1] = x[1];
  for (j = 0; j < iSize; j += 2)
    x[j] -= ((-UWT2)*(x[j-1] + x[j+1])) >> 12;

  x[iSize] = x[iSize-2];
  for(j = 1; j < iSize+1; j += 2)
    x[j] = (((-UWT1)*(x[j-1] + x[j+1])) >> 8) - x[j];

  p = pDst;
  for(j = 0; j < iSize; j++)
  {
    I32_WMV t = x[j] >> 4; // AKadatch: no rounding here to counter-balance errors made by initial scaling in first loop
    if ((U32_WMV) t > 255)
    {
      if (t > 0)
        t = 255;
      else
        t = 0;
    }
    p[0] = (U8_WMV) t;
    p += iPitch;
  }

#else

//
// memoryless version used by vertical transform. iSize should be at least 4
//

#define EMIT(p,v) do { \
  I32_WMV t = (v) >> 4;    \
  if ((U32_WMV) t > 255)  \
  {                    \
    if (t > 0)         \
      t = 255;         \
    else               \
      t = 0;           \
  }                    \
  (p) = t;             \
} while (0)

  I32_WMV j, x0, x1, x2, x3, x4;
  U8_WMV *pSrc, *pDst;

  pSrc = pSrc0;
  pDst = pDst0;

  x0 = (pSrc[0]*315) >> 4;	// (n*315) >> 4 = (n * 19) + ((n * 11) >> 4);
  pSrc += iPitch*2;

  x2 = x0;

  for (j = 2; j < iSize; j += 2)
  {
    x0 = (pSrc[0]*315) >> 4;	// (n*315) >> 4 = (n * 19) + ((n * 11) >> 4);
    pSrc += iPitch*2;

    x1 = (226*(x2 + x0))>>8;
    if (j > 2)
    {
      x2 -= ((217)*(x3 + x1))>>12;
      EMIT (pDst[iPitch], x2);

      x3 = (((406)*(x4 + x2))>>8) - x3;
      EMIT (pDst[0], x3);

      pDst += iPitch*2;
    }
    else
    {
      x2 -= ((217)*(x1*2))>>12;
      EMIT (pDst[0], x2);

      pDst += iPitch;
    }

    x4 = x2;
    x3 = x1;
    x2 = x0;
  }

  x1 = (226*(x2*2))>>8;

  x2 -= ((217) * (x3 + x1)) >> 12;
  EMIT (pDst[iPitch], x2);

  x3 = (((406)*(x4 + x2))>>8) - x3;
  EMIT (pDst[0], x3);

  x1 = (((406)*(x2*2))>>8) - x1;
  EMIT (pDst[iPitch*2], x1);

#undef EMIT
#endif
}


#else


Void_WMV g_UpsampleWFilterLine7(I32_WMV *x, I32_WMV size)
{
	I32_WMV j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine7);
    
	for (j = 0; j < size; j += 2)
		x[j] = (x[j]*UNORM2); // shift 8 bshift 0 res 8

	for (j = 1; j < size-1; j += 2)
		x[j] =((-UWT3)*(x[j-1]+x[j+1]))>>8; // shift 8 bshift 8 res 8
	x[size-1] =  ((-UWT3)*(x[size-2]<<1))>>8; // res 8
	
	x[0] = ((x[0]<<12) + (-UWT2)*(x[1]<<1))>>12; // shift 12 bshift 12 res 8
	for (j = 2; j < size; j += 2)
		x[j] = ((x[j]<<12) + (-UWT2)*(x[j-1]+x[j+1]))>>12; // res 8
	
	for(j = 1; j < size-1; j += 2)
		x[j] =((x[j]<<8) + (-UWT1)*(x[j-1]+x[j+1]))>>8; // shift 8 bshift 8 res8 
	x[size-1] = ((x[size-1]<<8) + (-UWT1)*(x[size-2]<<1))>>8;
	
	for(j = 0; j < size; j++) {
      x[j] = (x[j]+128)>>8; // rounding incorrect for negative values, but clipping takes care of that.
      x[j] = CLIP(x[j]);
	}
}

#endif


Void_WMV g_UpsampleWFilterLine3(I32_WMV *x, I32_WMV size)
{

	I32_WMV j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine3);
	for (j = 1; j < size-1; j += 2)
		x[j] =((x[j-1]+x[j+1])>>1);
    x[size-1] = x[size-2];
	
}


#if AKADATCH

Void_WMV g_HorizUpsampleFilter7(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizUpsampleFilter7);

    for (i = 0; i < iHeight; i += iVScale)
    {
        g_UpsampleWFilterLine7 (pSrc, pSrc, iWidth, buffer, 1);
        pSrc += iPitchY*iVScale;
    }
    pSrc = pSrcU;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        g_UpsampleWFilterLine7 (pSrc, pSrc, iWidthUV, buffer, 1);
        pSrc += iPitchUV*iVScale;
    }
    pSrc = pSrcV;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        g_UpsampleWFilterLine7 (pSrc, pSrc, iWidthUV, buffer, 1);
        pSrc += iPitchUV*iVScale;
    }
}


#else


Void_WMV g_HorizUpsampleFilter7(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i, j;
    I32_WMV *buffer;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizUpsampleFilter7);

    for (i = 0; i < iHeight; i += iVScale)
    {
        for (j = 0; j < iWidth; j+= 2)
            buffer[j] = (I32_WMV) pSrc[j];
        g_UpsampleWFilterLine7(buffer, iWidth);
        for (j = 0; j < iWidth; j++)
            pSrc[j] = (U8_WMV) buffer[j];
        pSrc += iPitchY*iVScale;
    }
    pSrc = pSrcU;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        for (j = 0; j < iWidthUV; j+= 2)
            buffer[j] = (I32_WMV) pSrc[j];
        g_UpsampleWFilterLine7(buffer, iWidthUV);
        for (j = 0; j < iWidthUV; j++)
            pSrc[j] = (U8_WMV) buffer[j];
        pSrc += iPitchUV*iVScale;
    }
    pSrc = pSrcV;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        for (j = 0; j < iWidthUV; j+= 2)
            buffer[j] = (I32_WMV) pSrc[j];
        g_UpsampleWFilterLine7(buffer, iWidthUV);
        for (j = 0; j < iWidthUV; j++)
            pSrc[j] = (U8_WMV) buffer[j];    
        pSrc += iPitchUV*iVScale;
    }
   
}

#endif

Void_WMV g_HorizUpsampleFilter3(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i, j;
    U8_WMV* pSrc = pSrcY;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizUpsampleFilter3);

    for (i = 0; i < iHeight; i += iVScale)
    {
        for (j = 0; j < iWidth; j+= 2)
            buffer[j] = (I32_WMV) pSrc[j];
        g_UpsampleWFilterLine3(buffer, iWidth);
        for (j = 0; j < iWidth; j++)
            pSrc[j] = (U8_WMV) buffer[j];
        pSrc += iPitchY*iVScale;
    }
    pSrc = pSrcU;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        for (j = 0; j < iWidthUV; j+= 2)
            buffer[j] = (I32_WMV) pSrc[j];
        g_UpsampleWFilterLine3(buffer, iWidthUV);
        for (j = 0; j < iWidthUV; j++)
            pSrc[j] = (U8_WMV) buffer[j];
        pSrc += iPitchUV*iVScale;
    }
    pSrc = pSrcV;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        for (j = 0; j < iWidthUV; j+= 2)
            buffer[j] = (I32_WMV) pSrc[j];
        g_UpsampleWFilterLine3(buffer, iWidthUV);
        for (j = 0; j < iWidthUV; j++)
            pSrc[j] = (U8_WMV) buffer[j];    
        pSrc += iPitchUV*iVScale;
    }
  
}

#if AKADATCH

Void_WMV g_VertUpsampleFilter7 (U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter7);

    for (i = 0; i < iWidth; ++i)
        g_UpsampleWFilterLine7 (pSrc + i, pSrc + i, iHeight, buffer, iPitchY);
    pSrc = pSrcU;
    for (i = 0; i < iWidthUV; ++i)
        g_UpsampleWFilterLine7 (pSrc + i, pSrc + i, iHeightUV, buffer, iPitchUV);
    pSrc = pSrcV;
    for (i = 0; i < iWidthUV; ++i)
        g_UpsampleWFilterLine7 (pSrc + i, pSrc + i, iHeightUV, buffer, iPitchUV);
}


#else

// This version of vertical upsampling uses column by column conversion; is memory intensive.
// To do: A low memory vertical upsampler.
Void_WMV g_VertUpsampleFilter7(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV)
{
    I32_WMV i, j;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter7);

    for (j = 0; j < iWidth; j++) {
		for (i = 0; i < iHeight; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchY+j];
		g_UpsampleWFilterLine7(buffer, iHeight);
		for (i = 0; i < iHeight; i++)
			pSrc[i*iPitchY+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcU;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_UpsampleWFilterLine7(buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pSrc[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcV;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_UpsampleWFilterLine7(buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pSrc[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }
	
}

// This version just calls the KNI horizantal filter TODO: a real vert upsample 7 filter
Void_WMV g_VertUpsampleFilter7_KNI(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV)
{
    I32_WMV i, j;
    U8_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter7_KNI);

    for (j = 0; j < iWidth; j++) {
		for (i = 0; i < iHeight; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchY+j];
		g_UpsampleWFilterLine7_KNI(buffer, buffer, iHeight);
		for (i = 0; i < iHeight; i++)
			pSrc[i*iPitchY+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcU;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_UpsampleWFilterLine7_KNI(buffer, buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pSrc[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcV;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_UpsampleWFilterLine7_KNI(buffer, buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pSrc[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }
	
}

#endif


Void_WMV g_VertUpsampleFilter3(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV  iWidthUV,  I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV * pTempBuf)
{
    I32_WMV i, j;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter3);

	for (j = 0; j < iWidth; j++) {
		for (i = 0; i < iHeight; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchY+j];
		g_UpsampleWFilterLine3(buffer, iHeight);
		for (i = 0; i < iHeight; i++)
			pSrc[i*iPitchY+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcU;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_UpsampleWFilterLine3(buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pSrc[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }
    pSrc = pSrcV;
    for (j = 0; j < iWidthUV; j++) {
		for (i = 0; i < iHeightUV; i += 2)
			buffer[i] = (I32_WMV) pSrc[i*iPitchUV+j];
		g_UpsampleWFilterLine3(buffer, iHeightUV);
		for (i = 0; i < iHeightUV; i++)
			pSrc[i*iPitchUV+j] = (U8_WMV) buffer[i];
    }

}

#define AW1 70
#define AW2 5
#define AW3 -11

#ifndef WMV_OPT_MULTIRES_ARM
Void_WMV g_DownsampleWFilterLine6_Vert(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch)
{
    I32_WMV j ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DownsampleWFilterLine6_Vert);

    x[0] = (I32_WMV) (((pSrc[0] + pSrc[iPitch]) * AW1 + (pSrc[2*iPitch] + pSrc[0]) * AW2 + (pSrc[3*iPitch] + pSrc[iPitch]) * AW3+ 63) >> 7);
    for( j = 2; j < size -2; j += 2) {
        x[j] =  (I32_WMV) (((pSrc[j*iPitch] + pSrc[(j+1)*iPitch]) * AW1 + (pSrc[(j-1)*iPitch] + pSrc[(j+2)*iPitch]) * AW2 + (pSrc[(j-2)*iPitch] + pSrc[(j+3)*iPitch]) * AW3 + 63) >> 7);
    }
    x[size-2] =  (I32_WMV) (((pSrc[(size-2)*iPitch] + pSrc[(size-1)*iPitch]) * AW1 + (pSrc[(size-3)*iPitch] + pSrc[(size-1)*iPitch]) * AW2 + (pSrc[(size-4)*iPitch] + pSrc[(size-2)*iPitch]) * AW3 + 63) >> 7);

    for(j = 0; j < size; j+=2) {
        pDst[j*iPitch] = (U8_WMV) CLIP(x[j]);
        pDst[(j+1)*iPitch] = 0;
    }
}

Void_WMV g_DownsampleWFilterLine6_Horiz(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size)
{
    I32_WMV j ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_DownsampleWFilterLine6_Horiz);

    x[0] = (I32_WMV) (((pSrc[0] + pSrc[1]) * AW1 + (pSrc[2] + pSrc[0]) * AW2 + (pSrc[3] + pSrc[1]) * AW3+ 64) >> 7);

    for( j = 2; j < size -2; j += 2) {
        x[j] =  (I32_WMV) (((pSrc[j] + pSrc[j+1]) * AW1 + (pSrc[j-1] + pSrc[j+2]) * AW2 + (pSrc[j-2] + pSrc[j+3]) * AW3 + 64) >> 7);
    }

    x[size-2] =  (I32_WMV) (((pSrc[size-2] + pSrc[size-1]) * AW1 + (pSrc[size-3] + pSrc[size-1]) * AW2 + (pSrc[size-4] + pSrc[size-2]) * AW3 + 64) >> 7);
    
    for(j = 0; j < size; j+=2) {
        pDst[j] = (U8_WMV) CLIP(x[j]);
        pDst[j+1] = 0;
    }

}
#endif // WMV_OPT_MULTIRES_ARM

Void_WMV g_HorizDownsampleFilter6(
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, 
    I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf)
{
    I32_WMV i;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    U8_WMV* pDst = pDstY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizDownsampleFilter6);

    for (i = 0; i < iHeight; i++)
    {
        g_DownsampleWFilterLine6_Horiz(pDst, pSrc, buffer, iWidth);
        pSrc += iPitchY;
        pDst += iPitchY;
    }
    pSrc = pSrcU;
    pDst = pDstU;
    for (i = 0; i < iHeightUV; i++)
    {
        g_DownsampleWFilterLine6_Horiz(pDst, pSrc, buffer, iWidthUV);
        pSrc += iPitchUV;
        pDst += iPitchUV;
    }
    pSrc = pSrcV;
    pDst = pDstV;
    for (i = 0; i < iHeightUV; i++)
    {
        g_DownsampleWFilterLine6_Horiz(pDst, pSrc, buffer, iWidthUV);
        pSrc += iPitchUV;
        pDst += iPitchUV;
    }
}

Void_WMV g_VertDownsampleFilter6(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf)
{
    I32_WMV j;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
	U8_WMV* pDst = pDstY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertDownsampleFilter6);

    for (j = 0; j < iWidth; j++) {
		g_DownsampleWFilterLine6_Vert(pDst+j, pSrc+j, buffer, iHeight, iPitchY);
    }
    pSrc = pSrcU;
	pDst = pDstU;
    for (j = 0; j < iWidthUV; j++) {
		g_DownsampleWFilterLine6_Vert(pDst+j, pSrc+j, buffer, iHeightUV, iPitchUV);
    }
    pSrc = pSrcV;
	pDst = pDstV;
    for (j = 0; j < iWidthUV; j++) {
		g_DownsampleWFilterLine6_Vert(pDst+j, pSrc+j, buffer, iHeightUV, iPitchUV);
    }
}

#define SW1 28
#define SW2  6 
#define SW3  -3
//#define SW4  1 
//#define SW5  3
//#define SW5   4
#ifndef WMV_OPT_MULTIRES_ARM
Void_WMV g_UpsampleWFilterLine10_Vert(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch)
{
    I32_WMV j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Vert);

    x[0] = (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[2*iPitch] * SW3 + pSrc[4*iPitch]  + 16) >> 5);
    x[1] = (I32_WMV) ((pSrc[0] * SW1 + pSrc[2*iPitch] * SW2 + pSrc[0] * SW3 + pSrc[2*iPitch]  + 16) >> 5);
    x[2] = (I32_WMV) ((pSrc[2*iPitch] * SW1 + pSrc[0] * SW2 + pSrc[4*iPitch] * SW3 + pSrc[6*iPitch]  + 16) >> 5);
    x[3] = (I32_WMV) ((pSrc[2*iPitch] * SW1 + pSrc[4*iPitch] * SW2 + pSrc[0] * SW3 + pSrc[0]  + 16) >> 5);
 
    for( j = 4; j < size - 4; j += 2) {
        x[j] = (I32_WMV) ((pSrc[j*iPitch] * SW1 + pSrc[(j-2)*iPitch] * SW2 + pSrc[(j+2)*iPitch] * SW3 + pSrc[(j+4)*iPitch]  + 16) >> 5);
        x[j+1] = (I32_WMV) ((pSrc[j*iPitch] * SW1 + pSrc[(j+2)*iPitch] * SW2 + pSrc[(j-2)*iPitch] * SW3 + pSrc[(j-4)*iPitch]  + 16) >> 5);
    }


    x[size-4] = (I32_WMV) ((pSrc[(size-4)*iPitch] * SW1 + pSrc[(size-6)*iPitch] * SW2 + pSrc[(size-2)*iPitch] * SW3 + pSrc[(size-2)*iPitch]  + 16) >> 5);
    x[size-3] = (I32_WMV) ((pSrc[(size-4)*iPitch] * SW1 + pSrc[(size-2)*iPitch] * SW2 + pSrc[(size-6)*iPitch] * SW3 + pSrc[(size-8)*iPitch]  + 16) >> 5);
    x[size-2] = (I32_WMV) ((pSrc[(size-2)*iPitch] * SW1 + pSrc[(size-4)*iPitch] * SW2 + pSrc[(size-2)*iPitch] * SW3 + pSrc[(size-4)*iPitch]  + 16) >> 5);
    x[size-1] = (I32_WMV) ((pSrc[(size-2)*iPitch] * SW1 + pSrc[(size-2)*iPitch] * SW2 + pSrc[(size-4)*iPitch] * SW3 + pSrc[(size-6)*iPitch]  + 16) >> 5);

    for(j = 0; j < size; j++)
        pDst[j*iPitch] = (U8_WMV) CLIP(x[j]);

}

Void_WMV g_UpsampleWFilterLine10_Horiz(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size)
{
    I32_WMV j;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Horiz);

    x[0] = (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[2] * SW3 + pSrc[4] + 15) >> 5);
    x[1] = (I32_WMV) ((pSrc[0] * SW1 + pSrc[2] * SW2 + pSrc[0] * SW3 + pSrc[2] + 15) >> 5);
    x[2] = (I32_WMV) ((pSrc[2] * SW1 + pSrc[0] * SW2 + pSrc[4] * SW3 + pSrc[6] + 15) >> 5);
    x[3] = (I32_WMV) ((pSrc[2] * SW1 + pSrc[4] * SW2 + pSrc[0] * SW3 + pSrc[0] + 15) >> 5);

    for( j = 4; j < size - 4; j += 2) {
        x[j] = (I32_WMV) ((pSrc[j] * SW1 + pSrc[j-2] * SW2 + pSrc[j+2] * SW3 + pSrc[j+4] + 15) >> 5);
        x[j+1] = (I32_WMV) ((pSrc[j] * SW1 + pSrc[j+2] * SW2 + pSrc[j-2] * SW3 + pSrc[j-4] + 15) >> 5);
    }

    x[size-4] = (I32_WMV) ((pSrc[size-4] * SW1 + pSrc[size-6] * SW2 + pSrc[size-2] * SW3 + pSrc[size-2] + 15) >> 5);
    x[size-3] = (I32_WMV) ((pSrc[size-4] * SW1 + pSrc[size-2] * SW2 + pSrc[size-6] * SW3 + pSrc[size-8] + 15) >> 5);
    x[size-2] = (I32_WMV) ((pSrc[size-2] * SW1 + pSrc[size-4] * SW2 + pSrc[size-2] * SW3 + pSrc[size-4] + 15) >> 5);
    x[size-1] = (I32_WMV) ((pSrc[size-2] * SW1 + pSrc[size-2] * SW2 + pSrc[size-4] * SW3 + pSrc[size-6] + 15) >> 5);

    for(j = 0; j < size; j++)
        pDst[j] = (U8_WMV) CLIP(x[j]);

}
#endif //WMV_OPT_MULTIRES_ARM 

Void_WMV g_HorizUpsampleFilter10(
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, 
    I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV,
    I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf)
{
    I32_WMV i;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizUpsampleFilter10);

    for (i = 0; i < iHeight; i += iVScale)
    {
        g_UpsampleWFilterLine10_Horiz(pSrc, pSrc, buffer, iWidth);
        pSrc += iPitchY*iVScale;
    }
    pSrc = pSrcU;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        g_UpsampleWFilterLine10_Horiz(pSrc, pSrc, buffer, iWidthUV);
        pSrc += iPitchUV*iVScale;
    }
    pSrc = pSrcV;
    for (i = 0; i < iHeightUV; i += iVScale)
    {
        g_UpsampleWFilterLine10_Horiz(pSrc, pSrc, buffer, iWidthUV);    
        pSrc += iPitchUV*iVScale;
    }
}

Void_WMV g_VertUpsampleFilter10(
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV,
    I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf)
{
    I32_WMV j;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter10);

    for (j = 0; j < iWidth; j++) {
		g_UpsampleWFilterLine10_Vert(pSrc+j, pSrc+j, buffer, iHeight, iPitchY);
    }
    pSrc = pSrcU;
    for (j = 0; j < iWidthUV; j++) {
		g_UpsampleWFilterLine10_Vert(pSrc+j, pSrc+j, buffer, iHeightUV, iPitchUV);
    }
    pSrc = pSrcV;
    for (j = 0; j < iWidthUV; j++) {
		g_UpsampleWFilterLine10_Vert(pSrc+j, pSrc+j, buffer, iHeightUV, iPitchUV);
    }
}



Void_WMV g_UpsampleWFilterLine10_Horiz_MBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV size)
{


    I32_WMV i, j, x;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Horiz_MBRow);

    x = (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[1] * SW3 + pSrc[2] + 15) >> 5);
    pDst[0] = (U8_WMV) CLIP(x);
    x = (I32_WMV) ((pSrc[0] * SW1 + pSrc[1] * SW2 + pSrc[0] * SW3 + pSrc[1] + 15) >> 5);
    pDst[1] = (U8_WMV) CLIP(x);
    x = (I32_WMV) ((pSrc[1] * SW1 + pSrc[0] * SW2 + pSrc[2] * SW3 + pSrc[3] + 15) >> 5);
    pDst[2] = (U8_WMV) CLIP(x);
    x = (I32_WMV) ((pSrc[1] * SW1 + pSrc[2] * SW2 + pSrc[0] * SW3 + pSrc[0] + 15) >> 5);
    pDst[3] = (U8_WMV) CLIP(x);

    for(i = 2,  j = 4; j < size - 4; j += 2, i++) {
        x = (I32_WMV) ((pSrc[i] * SW1 + pSrc[i-1] * SW2 + pSrc[i+1] * SW3 + pSrc[i+2] + 15) >> 5);
        pDst[j] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i] * SW1 + pSrc[i+1] * SW2 + pSrc[i-1] * SW3 + pSrc[i-2] + 15) >> 5);
        pDst[j+1] = (U8_WMV) CLIP(x);
    }

    x = (I32_WMV) ((pSrc[(size-4)>>1] * SW1 + pSrc[(size-6)>>1] * SW2 + pSrc[(size-2)>>1] * SW3 + pSrc[(size-2)>>1] + 15) >> 5);
    pDst[size-4] = (U8_WMV) CLIP(x);
    x = (I32_WMV) ((pSrc[(size-4)>>1] * SW1 + pSrc[(size-2)>>1] * SW2 + pSrc[(size-6)>>1] * SW3 + pSrc[(size-8)>>1] + 15) >> 5);
    pDst[size-3] = (U8_WMV) CLIP(x);
    x = (I32_WMV) ((pSrc[(size-2)>>1] * SW1 + pSrc[(size-4)>>1] * SW2 + pSrc[(size-2)>>1] * SW3 + pSrc[(size-4)>>1] + 15) >> 5);
    pDst[size-2] = (U8_WMV) CLIP(x);
    x = (I32_WMV) ((pSrc[(size-2)>>1] * SW1 + pSrc[(size-2)>>1] * SW2 + pSrc[(size-4)>>1] * SW3 + pSrc[(size-6)>>1] + 15) >> 5);
    pDst[size-1] = (U8_WMV) CLIP(x);
/*
    for(j = 0; j < size; j++)
        pDst[j] = (U8_WMV) CLIP(x[j]);
        */

}



Void_WMV g_UpsampleWFilterLine10_Vert_MBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst, I32_WMV size)
{
    I32_WMV j, k,i,x;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Vert_MBRow);
    size = size *2;

    for(k = 0; k < width; k++, pDst++, pSrc++)
    {
 
      //  for(i = 2,  j = 0; j < size ; j += 2, i++) {
         for(i = 0,  j = 0; j < size ; j += 2, i++) {
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i+1)*iPitch] * SW2 + pSrc[(i-1)*iPitch] * SW3 + pSrc[(i-2)*iPitch]  + 16) >> 5);
            pDst[(j+1)*iPitchDst] = (U8_WMV) CLIP(x);
        }


    }

}

/*
Void_WMV g_UpsampleWFilterLine10_Vert_MBRow16(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst)
{
    I32_WMV j, k,i,x;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Vert_MBRow);
    size = size *2;

    for(k = 0; k < width; k++, pDst++, pSrc++)
    {
#if 0
        for(i = 2,  j = 0; j < size ; j += 2, i++) {
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i+1)*iPitch] * SW2 + pSrc[(i-1)*iPitch] * SW3 + pSrc[(i-2)*iPitch]  + 16) >> 5);
            pDst[(j+1)*iPitchDst] = (U8_WMV) CLIP(x);
        }
#else
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);

        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);

        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);

        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);


         x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);

        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);

        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);

        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);




#endif

    }

}

Void_WMV g_UpsampleWFilterLine10_Vert_MBRow8(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst, I32_WMV size)
{
    I32_WMV j, k,i,x;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Vert_MBRow);
    size = size *2;

    for(k = 0; k < width; k++, pDst++, pSrc++)
    {
 
        for(i = 2,  j = 0; j < size ; j += 2, i++) {
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i+1)*iPitch] * SW2 + pSrc[(i-1)*iPitch] * SW3 + pSrc[(i-2)*iPitch]  + 16) >> 5);
            pDst[(j+1)*iPitchDst] = (U8_WMV) CLIP(x);
        }


    }

}

  */

Void_WMV g_UpsampleWFilterLine10_Vert_FirstMBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst,  I32_WMV size)
{
    I32_WMV j,k,i,x;
    I32_WMV iMBSize = size*2;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Vert_FirstMBRow);
    

    for(k = 0; k < width; k++, pSrc++, pDst++)
    {

        x = (I32_WMV) ((pSrc[0] * SW1 + pSrc[0] * SW2 + pSrc[iPitch] * SW3 + pSrc[2*iPitch]  + 16) >> 5);
        pDst[0*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[0] * SW1 + pSrc[iPitch] * SW2 + pSrc[0] * SW3 + pSrc[iPitch]  + 16) >> 5);
        pDst[1*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[iPitch] * SW1 + pSrc[0] * SW2 + pSrc[2*iPitch] * SW3 + pSrc[3*iPitch]  + 16) >> 5);
        pDst[2*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[iPitch] * SW1 + pSrc[2*iPitch] * SW2 + pSrc[0] * SW3 + pSrc[0]  + 16) >> 5);
        pDst[3*iPitchDst] = (U8_WMV) CLIP(x);
 
        for(i = 2,  j = 4; j < iMBSize; j += 2, i++) {
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i+1)*iPitch] * SW2 + pSrc[(i-1)*iPitch] * SW3 + pSrc[(i-2)*iPitch]  + 16) >> 5);
            pDst[(j+1)*iPitchDst] = (U8_WMV) CLIP(x);
        }
    }
/*
    for(j = 0; j < iMBSize; j++)
        pDst[j*iPitchDst] = (U8_WMV) CLIP(x[j]);

    }
    */

}

Void_WMV g_UpsampleWFilterLine10_Vert_LastMBRow(U8_WMV *pDst, U8_WMV *pSrc,  I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst,  I32_WMV size)
{
    I32_WMV j,i,k,x;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_UpsampleWFilterLine10_Vert);

    size = size *2;
 
    for(k = 0; k < width; k++,pSrc++, pDst++)
    {
        for(i = 0,  j = 0; j < size - 4; j += 2, i++) {
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i-1)*iPitch] * SW2 + pSrc[(i+1)*iPitch] * SW3 + pSrc[(i+2)*iPitch]  + 16) >> 5);
            pDst[j*iPitchDst] = (U8_WMV) CLIP(x);
            x = (I32_WMV) ((pSrc[i*iPitch] * SW1 + pSrc[(i+1)*iPitch] * SW2 + pSrc[(i-1)*iPitch] * SW3 + pSrc[(i-2)*iPitch]  + 16) >> 5);
            pDst[(j+1)*iPitchDst] = (U8_WMV) CLIP(x);
        }


        x = (I32_WMV) ((pSrc[((size-4)>>1)*iPitch] * SW1 + pSrc[((size-6)>>1)*iPitch] * SW2 + pSrc[((size-2)>>1)*iPitch] * SW3 + pSrc[((size-2)>>1)*iPitch]  + 16) >> 5);
        pDst[(size-4)*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[((size-4)>>1)*iPitch] * SW1 + pSrc[((size-2)>>1)*iPitch] * SW2 + pSrc[((size-6)>>1)*iPitch] * SW3 + pSrc[((size-8)>>1)*iPitch]  + 16) >> 5);
        pDst[(size-3)*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[((size-2)>>1)*iPitch] * SW1 + pSrc[((size-4)>>1)*iPitch] * SW2 + pSrc[((size-2)>>1)*iPitch] * SW3 + pSrc[((size-4)>>1)*iPitch]  + 16) >> 5);
        pDst[(size-2)*iPitchDst] = (U8_WMV) CLIP(x);
        x = (I32_WMV) ((pSrc[((size-2)>>1)*iPitch] * SW1 + pSrc[((size-2)>>1)*iPitch] * SW2 + pSrc[((size-4)>>1)*iPitch] * SW3 + pSrc[((size-6)>>1)*iPitch]  + 16) >> 5);
        pDst[(size-1)*iPitchDst] = (U8_WMV) CLIP(x);

    }

}


Void_WMV g_HorizUpsampleFilter10_MBRow(
    U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV,
    I32_WMV iDstWidth, I32_WMV iDstWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV, 
    I32_WMV iDstPitchY, I32_WMV iDstPitchUV)
{
    I32_WMV i;
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_HorizUpsampleFilter10_MBRow);

    for (i = 0; i < 16; i ++)
    {
        g_UpsampleWFilterLine10_Horiz_MBRow(pDstY, pSrcY, iDstWidth);
        pDstY += iDstPitchY;
        pSrcY += iSrcPitchY;
    }
    
    for (i = 0; i < 8; i++)
    {
        g_UpsampleWFilterLine10_Horiz_MBRow(pDstU, pSrcU, iDstWidthUV);
        pDstU += iDstPitchUV;
        pSrcU += iSrcPitchUV;

        g_UpsampleWFilterLine10_Horiz_MBRow(pDstV, pSrcV, iDstWidthUV);
        pDstV += iDstPitchUV;
        pSrcV += iSrcPitchUV;
    }
}

Void_WMV g_VertUpsampleFilter10_FirstMBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV)
{
   
 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter10_FirstMBRow);

    g_UpsampleWFilterLine10_Vert_FirstMBRow(pDstY, pSrcY,  iWidth,  iSrcPitchY,  iPitchY, 14 /*16*/);
    g_UpsampleWFilterLine10_Vert_FirstMBRow(pDstU, pSrcU,  iWidthUV,  iSrcPitchUV,  iPitchUV, 6 /*8*/);
    g_UpsampleWFilterLine10_Vert_FirstMBRow(pDstV, pSrcV, iWidthUV,  iSrcPitchUV,  iPitchUV, 6/*8*/);

}

Void_WMV g_VertUpsampleFilter10_LastMBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV, I32_WMV iLastMBOffset)
{
    
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter10_LastMBRow);

    g_UpsampleWFilterLine10_Vert_LastMBRow(pDstY, pSrcY,  iWidth,  iSrcPitchY,  iPitchY, 18 - iLastMBOffset/*16*/);
    g_UpsampleWFilterLine10_Vert_LastMBRow(pDstU, pSrcU,  iWidthUV,  iSrcPitchUV,  iPitchUV, 10 - ( iLastMBOffset>>1)/*8*/);
    g_UpsampleWFilterLine10_Vert_LastMBRow(pDstV, pSrcV,  iWidthUV,  iSrcPitchUV,  iPitchUV, 10 - (iLastMBOffset>>1)/*8*/);

}

Void_WMV g_VertUpsampleFilter10_MBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV)
{
   
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_VertUpsampleFilter10_MBRow);

    g_UpsampleWFilterLine10_Vert_MBRow(pDstY, pSrcY, iWidth,  iSrcPitchY,  iPitchY, 16);
    g_UpsampleWFilterLine10_Vert_MBRow(pDstU, pSrcU, iWidthUV,  iSrcPitchUV,  iPitchUV, 8);
    g_UpsampleWFilterLine10_Vert_MBRow(pDstV, pSrcV, iWidthUV,  iSrcPitchUV,  iPitchUV, 8);

}

#endif // WMV9_SIMPLE_ONLY



