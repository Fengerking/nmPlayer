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

#define CLIP(x) ((U32_WMV) x <= 255 ? x : (x < 0 ? 0: 255))

Void_WMV g_HorizDownsampleFilter6(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_HorizUpsampleFilter10(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_HorizUpsampleFilter10(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_VertDownsampleFilter6(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);
Void_WMV g_VertUpsampleFilter10(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf);


Void_WMV InitMultires(tWMVDecInternalMember *pWMVDec)
{ 
    pWMVDec->m_pHorizDownsampleFilter =  &g_HorizDownsampleFilter6;//pWMVDec->m_pHorizDownsampleFilter6;
    pWMVDec->m_pVertDownsampleFilter =  &g_VertDownsampleFilter6;//pWMVDec->m_pVertDownsampleFilter6;
    pWMVDec->m_pHorizUpsampleFilter = &g_HorizUpsampleFilter10;//pWMVDec->m_pHorizUpsampleFilter10;
    pWMVDec->m_pVertUpsampleFilter = &g_VertUpsampleFilter10;//pWMVDec->m_pVertUpsampleFilter10;
}

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

    
    if (pWMVDec->m_bYUV411) {
        iWidthUV = iWidthY >> 2;
        iHeightUV = iHeightY >> 2;
    }
    
    iWidthUVPlusExp = iWidthUV + pWMVDec->EXPANDUV_REFVOP;
	if( pWMVDec->m_MemOperator.Type == VIDEO_MEM_SHARE_OP ) 
	{
		iWidthPrevY = pWMVDec->m_frameBufStrideY;
		iWidthPrevUV = pWMVDec->m_frameBufStrideUV;
	}
	else
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

Void_WMV UpsampleRefFrame(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV) 
{

    I32_WMV iWidthPrevY = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevY;
    I32_WMV iWidthPrevUV = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUV;
    I32_WMV iRowInc = s_iRowIncTbl[pWMVDec->m_iResIndex + pWMVDec->m_iResIndexPrev];
    I32_WMV iColInc = s_iColIncTbl[pWMVDec->m_iResIndex + pWMVDec->m_iResIndexPrev];
    I32_WMV i, j, k;
    U8_WMV* pSrc = pSrcY;
    U8_WMV* pDst = pDstY;

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



#define AW1 70
#define AW2 5
#define AW3 -11

Void_WMV g_DownsampleWFilterLine6_Vert_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch)
{
    I32_WMV j ;

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

Void_WMV g_DownsampleWFilterLine6_Horiz_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size)
{
    I32_WMV j ;

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

Void_WMV g_HorizDownsampleFilter6(
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV, 
    I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf)
{
    I32_WMV i;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;
    U8_WMV* pDst = pDstY;

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

Void_WMV g_UpsampleWFilterLine10_Vert_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch)
{
    I32_WMV j;

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

Void_WMV g_UpsampleWFilterLine10_Horiz_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size)
{
    I32_WMV j;

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


Void_WMV g_HorizUpsampleFilter10(
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale, 
    I32_WMV iWidth, I32_WMV iHeight, I32_WMV iWidthUV, I32_WMV iHeightUV,
    I32_WMV iPitchY, I32_WMV iPitchUV, I8_WMV* pTempBuf)
{
    I32_WMV i;
    I32_WMV *buffer = (I32_WMV *) pTempBuf;
    U8_WMV* pSrc = pSrcY;

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




