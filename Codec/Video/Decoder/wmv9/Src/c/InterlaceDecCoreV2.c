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
#include "localhuffman_wmv.h"
#include "codehead.h"

#ifndef PPCWMP

#ifdef _WMV_TARGET_X86_
#include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#include "repeatpad_wmv.h"
//
// Core
//
#define CLIP(x) ((U32_WMV) x <= 255 ? x : (x < 0 ? 0: 255))

static const I32_WMV s_RndTbl[] = { 0, 0, 0, 1 };
static const I32_WMV s_RndTblField [] = {0, 0, 1, 2, 4, 4, 5, 6, 
    2, 2, 3, 8, 6, 6, 7, 12};
static const I32_WMV g_iNegOffset [4] = {0, 3, 2, 1};



static Void_WMV CopyPixelIToPixelCBlock_8x8 (
    I16_WMV *pSrc, U8_WMV *pDst, I32_WMV iSrcOffset, I32_WMV iDstOffset)
{
    I32_WMV y, x;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {    
            pDst [x] = (U8_WMV) CLIP (pSrc [x]);            
        }
        pDst += iDstOffset;
        pSrc += iSrcOffset;
    }
}

static Void_WMV CopyPixelIToPixelIBlock_8x8 (
    I16_WMV *pSrc, I16_WMV *pDst, I32_WMV iSrcOffset, I32_WMV iDstOffset )
{
    I32_WMV y, x;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 4; x++) {    
            ((U32_WMV *)pDst) [x] = ((U32_WMV *)pSrc) [x];            
        }
        pDst += iDstOffset;
        pSrc += iSrcOffset;
    }
}

Void_WMV CopyIntraFieldMBtoFieldMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    I16_WMV *pSrc = ppxliFieldMB;
    // Y
    CopyPixelIToPixelCBlock_8x8 (pSrc, ppxliFrameMBY, 8, iOffsetY*2);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 64, ppxliFrameMBY + 8, 8, iOffsetY*2);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 2 * 64, ppxliFrameMBY + iOffsetY, 8, iOffsetY*2);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 3 * 64, ppxliFrameMBY + iOffsetY + 8 , 8, iOffsetY*2);
    // U, V
    CopyPixelIToPixelCBlock_8x8 (pSrc + 4 * 64, ppxliFrameMBU, 8, iOffsetUV);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 5 * 64, ppxliFrameMBV, 8, iOffsetUV);
}

Void_WMV CopyIntraFieldMBtoFrameMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    I16_WMV *pSrc = ppxliFieldMB;
    CopyPixelIToPixelCBlock_8x8 (pSrc, ppxliFrameMBY, 8, iOffsetY);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 64, ppxliFrameMBY + 8, 8, iOffsetY);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 2 * 64, ppxliFrameMBY + 8*iOffsetY, 8, iOffsetY);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 3 * 64, ppxliFrameMBY + 8*iOffsetY + 8, 8, iOffsetY);
    // U, V
    CopyPixelIToPixelCBlock_8x8 (pSrc + 4 * 64, ppxliFrameMBU, 8, iOffsetUV);
    CopyPixelIToPixelCBlock_8x8 (pSrc + 5 * 64, ppxliFrameMBV, 8, iOffsetUV);
} 

Void_WMV CopyIntraFieldMBtoOverlapFieldMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliMBY, I16_WMV *ppxliMBU, 
    I16_WMV *ppxliMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    I16_WMV *pSrc = ppxliFieldMB;
    // Y
    CopyPixelIToPixelIBlock_8x8 (pSrc, ppxliMBY, 8, iOffsetY * 2);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 64, ppxliMBY + 8, 8, iOffsetY * 2);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 2 * 64, ppxliMBY + iOffsetY, 8, iOffsetY * 2);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 3 * 64, ppxliMBY + iOffsetY + 8 , 8, iOffsetY * 2);
    // U, V
    CopyPixelIToPixelIBlock_8x8 (pSrc + 4 * 64, ppxliMBU, 8, iOffsetUV);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 5 * 64, ppxliMBV, 8, iOffsetUV);
}

Void_WMV CopyIntraFieldMBtoOverlapFrameMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliMBY, I16_WMV *ppxliMBU, 
    I16_WMV *ppxliMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    I16_WMV *pSrc = ppxliFieldMB;
    CopyPixelIToPixelIBlock_8x8 (pSrc, ppxliMBY, 8, iOffsetY);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 64, ppxliMBY + 8, 8, iOffsetY);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 2 * 64, ppxliMBY + 8*iOffsetY, 8, iOffsetY);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 3 * 64, ppxliMBY + 8*iOffsetY + 8, 8, iOffsetY);
    // U, V
    CopyPixelIToPixelIBlock_8x8 (pSrc + 4 * 64, ppxliMBU, 8, iOffsetUV);
    CopyPixelIToPixelIBlock_8x8 (pSrc + 5 * 64, ppxliMBV, 8, iOffsetUV);
} 

Void_WMV LoopFilterRowIFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    CWMVMBMode *pmbmd)
{
    U8_WMV* pCurrY, *pCurrU, *pCurrV;
    CWMVMBMode *pMBMode;
    U32_WMV imbX, imbY;    
    Bool_WMV bSliceEnd = FALSE;

    if (pWMVDec->m_bSliceWMVA) {
        if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1)
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig + pWMVDec->m_uintNumMBY;        
        else
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;
    }

    // horizontal
    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {  
        pCurrY = ppxliY + imbY * pWMVDec->m_iMBSizeXWidthPrevY; 
        pCurrU = ppxliU + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pCurrV = ppxliV + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pMBMode = pmbmd + imbY * pWMVDec->m_uintNumMBX;
        if (pWMVDec->m_bSliceWMVA) 
            bSliceEnd = (imbY >= pWMVDec->m_uintNumMBY - 1) || pWMVDec->m_pbStartOfSliceRow [ imbY + 1 ];
        else
            bSliceEnd = (imbY >= pWMVDec->m_uintNumMBY - 1);

        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            // Filter row boundary within MB             
            if (pMBMode->m_chFieldDctMode == FALSE) {
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 8 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                    pWMVDec->m_iStepSize, 16);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 9 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                    pWMVDec->m_iStepSize, 16);
            }
            // Filter row boundary across MB 
            if (!bSliceEnd) {
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 16 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                    pWMVDec->m_iStepSize, 16);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 17 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                    pWMVDec->m_iStepSize, 16);

                // UV
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrU + 8 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrU + 9 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrV + 8 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrV + 9 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
            }

            pMBMode ++;
            pCurrY += 16;
            pCurrU += 8;
            pCurrV += 8;
        } 
    }
}

Void_WMV LoopFilterColIFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    CWMVMBMode *pmbmd)
{
    U8_WMV* pCurrY, *pCurrU, *pCurrV;
    CWMVMBMode *pMBMode;
    U32_WMV imbX, imbY;    

    // vertical
    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {  
        pCurrY = ppxliY + imbY * pWMVDec->m_iMBSizeXWidthPrevY; 
        pCurrU = ppxliU + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pCurrV = ppxliV + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pMBMode = pmbmd + imbY * pWMVDec->m_uintNumMBX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            // Filter column boundary within MB 
            (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5, 2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
            (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5 + pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY,
                pWMVDec->m_iStepSize, 8);
            // Filter column boundary across MB 
            if (imbX != pWMVDec->m_uintNumMBX - 1) {
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5, 2*pWMVDec->m_iWidthPrevY, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5 + pWMVDec->m_iWidthPrevY, 
                    2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                
                // UV
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrU + 8 - 5, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 4);
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrU + 8 - 5 + pWMVDec->m_iWidthPrevUV, 
                    2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4);
                
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrV + 8 - 5, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 4); 
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrV + 8 - 5 + pWMVDec->m_iWidthPrevUV, 
                    2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 
            }

            pMBMode ++;
            pCurrY += 16;
            pCurrU += 8;
            pCurrV += 8;
        } 
    }
}

Void_WMV LoopFilterRowPFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, 
    CWMVMBMode *pmbmd)
{
    U8_WMV* pCurrY, *pCurrU, *pCurrV;
    CWMVMBMode *pMBMode;
    U32_WMV imbX, imbY;  
    Bool_WMV bSliceHead = FALSE;
    Bool_WMV bSliceEnd  = FALSE;

    if (pWMVDec->m_bSliceWMVA) {
        if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1)
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig + pWMVDec->m_uintNumMBY;        
        else
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;
    }


    // horizontal
    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {  
        pCurrY = ppxliY + imbY * pWMVDec->m_iMBSizeXWidthPrevY; 
        pCurrU = ppxliU + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pCurrV = ppxliV + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pMBMode = pmbmd + imbY * pWMVDec->m_uintNumMBX;
        if (pWMVDec->m_bSliceWMVA) { 
            bSliceHead = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [ imbY ];
            bSliceEnd  = (imbY >= pWMVDec->m_uintNumMBY - 1) || pWMVDec->m_pbStartOfSliceRow [ imbY + 1 ];
        } else {
            bSliceHead = (imbY == 0);
            bSliceEnd  = (imbY >= pWMVDec->m_uintNumMBY - 1);
        }
    

        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {

            I8_WMV *pBlockMode = pMBMode->m_rgcBlockXformMode;

            if (pMBMode->m_chFieldDctMode == FALSE) {
                // block 0 - 8x4 Bndry
                if (!bSliceHead && 
                    (pBlockMode [0] == XFORMMODE_8x4 || pBlockMode [0] == XFORMMODE_4x4)) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 4 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 8);
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 5 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 8);
                }

                // block 0 - 8x8 Bndry
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 8 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 9 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                    pWMVDec->m_iStepSize, 8);
                
                // block 1 - 8x4 Bndry
                
                if (!bSliceHead && 
                    (pBlockMode [1] == XFORMMODE_8x4 || pBlockMode [1] == XFORMMODE_4x4)) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 4 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 5 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }

                // block 1 - 8x8 Bndry
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 8 * pWMVDec->m_iWidthPrevY + 8, 
                    2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 9 * pWMVDec->m_iWidthPrevY + 8, 
                    2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);

                // block 2
            
                if ((!bSliceEnd) &&
                    (pBlockMode [2] == XFORMMODE_8x4 || pBlockMode [2] == XFORMMODE_4x4)) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 12 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 13 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
                
                if (!bSliceEnd) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 16 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 17 * pWMVDec->m_iWidthPrevY,
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }

                // block 3
               
                if ((!bSliceEnd) &&
                    (pBlockMode [3] == XFORMMODE_8x4 || pBlockMode [3] == XFORMMODE_4x4)) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 12 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 13 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
                
                if (!bSliceEnd) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 16 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 17 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
            } else {
                // block 0 - 8x4
                if (pBlockMode [0] == XFORMMODE_8x4 || pBlockMode [0] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 8 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 8);
                }
                if (!bSliceEnd) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 16 * pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 8);
                }

                // block 1 - 8x4
                if (pBlockMode [1] == XFORMMODE_8x4 || pBlockMode [1] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 8 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
                if (!bSliceEnd) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 16 * pWMVDec->m_iWidthPrevY + 8,
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }

                // block 2 - 8x4 
                if (pBlockMode [2] == XFORMMODE_8x4 || pBlockMode [2] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 9 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
                if (!bSliceEnd) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 17 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }

                // block 3 - 8x4
                if (pBlockMode [3] == XFORMMODE_8x4 || pBlockMode [3] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 9 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
                if (!bSliceEnd) {
                    (*pWMVDec->m_pFilterHorizontalEdge) (pCurrY + 17 * pWMVDec->m_iWidthPrevY + 8, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
            }

            // U
            if ((!bSliceHead) && (!bSliceEnd) &&   // ???????????????
                (pBlockMode [4] == XFORMMODE_8x4 || pBlockMode [4] == XFORMMODE_4x4)) {
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrU + 4 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrU + 5 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
            }

            if (!bSliceEnd) {
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrU + 8 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrU + 9 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
            }

            // V
            if (!bSliceHead && !bSliceEnd &&
                (pBlockMode [5] == XFORMMODE_8x4 || pBlockMode [5] == XFORMMODE_4x4)) {
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrV + 4 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrV + 5 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
            }

            if (!bSliceEnd) {
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrV + 8 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
                (*pWMVDec->m_pFilterHorizontalEdge) (pCurrV + 9 * pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, 
                    pWMVDec->m_iStepSize, 8);
            }
            pMBMode ++;
            pCurrY += 16;
            pCurrU += 8;
            pCurrV += 8;
        } 
    }
}

Void_WMV LoopFilterColPFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, 
    CWMVMBMode *pmbmd)
{
    U8_WMV* pCurrY, *pCurrU, *pCurrV;
    CWMVMBMode *pMBMode;
    U32_WMV imbX, imbY;    

    // vertical
    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {  
        pCurrY = ppxliY + imbY * pWMVDec->m_iMBSizeXWidthPrevY; 
        pCurrU = ppxliU + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pCurrV = ppxliV + imbY * pWMVDec->m_iBlkSizeXWidthPrevUV; 
        pMBMode = pmbmd + imbY * pWMVDec->m_uintNumMBX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            I8_WMV *pBlockMode = pMBMode->m_rgcBlockXformMode;

            if (pMBMode->m_chFieldDctMode == FALSE) {
                // Block 0 - 4x8 Bndry
                if (pBlockMode [0] == XFORMMODE_4x8 || pBlockMode [0] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 4 - 5, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 4);
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 4 - 5 + pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 4);
                }
                // Block 0 - 8x8 Bndry
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5, 2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);             
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5 + pWMVDec->m_iWidthPrevY, 
                    2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4); 

                // Block 1 - 4x8 Bndry
                if (pBlockMode [1] == XFORMMODE_4x8 || pBlockMode [1] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 12 - 5, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 4);
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 12 - 5 + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);

                }
                // Block 1 - 8x8 Bndry
                if (imbX != pWMVDec->m_uintNumMBX - 1) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 4);  
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5 + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);  
                }
                // Block 2 - 4x8 Bndry
                if (pBlockMode [2] == XFORMMODE_4x8 || pBlockMode [2] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 4 - 5 + 8 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 4 - 5 + 8 * pWMVDec->m_iWidthPrevY + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);
                }
                // Block 2 - 8x8 Bndry
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5 + 8 * pWMVDec->m_iWidthPrevY, 
                    2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);             
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5 + 8 * pWMVDec->m_iWidthPrevY + pWMVDec->m_iWidthPrevY, 
                    2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);    

                // Block 3 - 4x8 Bndry
                if (pBlockMode [3] == XFORMMODE_4x8 || pBlockMode [3] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 12 - 5 + 8 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 12 - 5 + 8 * pWMVDec->m_iWidthPrevY + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);
                }
                // Block 3 - 8x8 Bndry
                if (imbX != pWMVDec->m_uintNumMBX - 1) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5 + 8 * pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);             
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5 + 8 * pWMVDec->m_iWidthPrevY + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 4);             
                }
            } else {
                // Block 0 - 4x8 Bndry
                if (pBlockMode [0] == XFORMMODE_4x8 || pBlockMode [0] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 4 - 5, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 8);
                }
                // Block 0 - 8x8 Bndry
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5, 2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);

                // Block 1 - 4x8 Bndry
                if (pBlockMode [1] == XFORMMODE_4x8 || pBlockMode [1] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 12 - 5, 2*pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iStepSize, 8);
                }
                // Block 1 - 8x8 Bndry
                if (imbX != pWMVDec->m_uintNumMBX - 1) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5, 2*pWMVDec->m_iWidthPrevY,
                        pWMVDec->m_iStepSize, 8);            
                }
                // Block 2 - 4x8 Bndry
                if (pBlockMode [2] == XFORMMODE_4x8 || pBlockMode [2] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 4 - 5 + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
                // Block 2 - 8x8 Bndry
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 8 - 5 + pWMVDec->m_iWidthPrevY, 
                    2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);            

                // Block 3 - 4x8 Bndry
                if (pBlockMode [3] == XFORMMODE_4x8 || pBlockMode [3] == XFORMMODE_4x4) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 12 - 5 + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
                // Block 3 - 8x8 Bndry
                if (imbX != pWMVDec->m_uintNumMBX - 1) {
                    (*pWMVDec->m_pFilterVerticalEdge) (pCurrY + 16 - 5 + pWMVDec->m_iWidthPrevY, 
                        2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                }
            }
            // U
            if (pBlockMode [4] == XFORMMODE_4x8 || pBlockMode [4] == XFORMMODE_4x4) {
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrU + 4 - 5, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrU + 4 - 5 + pWMVDec->m_iWidthPrevUV, 
                    2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 

            }
            if (imbX != pWMVDec->m_uintNumMBX - 1) {
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrU + 8 - 5, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrU + 8 - 5 + pWMVDec->m_iWidthPrevUV, 
                    2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 

            }
            // V
            if (pBlockMode [5] == XFORMMODE_4x8 || pBlockMode [5] == XFORMMODE_4x4) {
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrV + 4 - 5, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrV + 4 - 5 + pWMVDec->m_iWidthPrevUV,
                    2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 
            }        
            if (imbX != pWMVDec->m_uintNumMBX - 1) {
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrV + 8 - 5, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 
                (*pWMVDec->m_pFilterVerticalEdge) (pCurrV + 8 - 5 + pWMVDec->m_iWidthPrevUV, 
                    2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 4); 
            }
            pMBMode ++;
            pCurrY += 16;
            pCurrU += 8;
            pCurrV += 8;
        } 
    }
}

// Inter Core

Void_WMV ChromaMVInterlaceV2 (
    I32_WMV imbX, I32_WMV imbY, I32_WMV iMvX, I32_WMV iMvY, I32_WMV *iChromaMvX, I32_WMV *iChromaMvY, 
    Bool_WMV bField)
{ 
    assert (iMvX != IBLOCKMV);  

    *iChromaMvX = (iMvX + s_RndTbl[iMvX & 3]) >> 1;

    if (bField) {
        *iChromaMvY = (iMvY >> 4)*8 + s_RndTblField [iMvY & 0xF];
    } else {
        *iChromaMvY = (iMvY + s_RndTbl[iMvY & 3]) >> 1;
    }
}

Bool_WMV PullBackFrameMBMotionVector (tWMVDecInternalMember *pWMVDec,
    I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iX = *iMvX;
    I32_WMV iY = *iMvY;
    I32_WMV iXCoord = imbX * 16 * 4; // q-pel unit
    I32_WMV iYCoord = imbY * 16 * 4;
    I32_WMV iWidth = pWMVDec->m_uintNumMBX * 16 * 4;
    I32_WMV iHeight = pWMVDec->m_uintNumMBY * 16 * 4;
    Bool_WMV bPullBack = FALSE;

    I32_WMV iPosX = iXCoord + iX; 
    I32_WMV iPosY = iYCoord + iY;

    if (iPosX < -16 * 4 - 3) { // pullback to -16.x
        *iMvX = -64 - iXCoord - g_iNegOffset [iX & 3];
        bPullBack = TRUE;
    } else if (iPosX > iWidth + 3) { // pullback to w.x
        *iMvX = iWidth - iXCoord + (iX & 3);
        bPullBack = TRUE;
    }

    if (iPosY < -16 * 4 - 3) { // pullback to -16.x
        *iMvY = -64 - iYCoord - g_iNegOffset [iY & 3];
        bPullBack = TRUE;
    } else if (iPosY > iHeight + 3) { // pullback to h.x
        *iMvY = iHeight - iYCoord + (iY & 3);
        bPullBack = TRUE;
    }

    return bPullBack;
}

Bool_WMV PullBackFrameBlockMotionVector (tWMVDecInternalMember *pWMVDec,
    I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk)
{
    I32_WMV iX = *iMvX;
    I32_WMV iY = *iMvY;
    I32_WMV iXCoord = imbX * 16 * 4 + (iBlk & 1) * 8 * 4; // q-pel unit
    I32_WMV iYCoord = imbY * 16 * 4 + ((iBlk & 2)>>1) * 8 * 4;
    I32_WMV iWidth = pWMVDec->m_uintNumMBX * 16 * 4;
    I32_WMV iHeight = pWMVDec->m_uintNumMBY * 16 * 4;
    Bool_WMV bPullBack = FALSE;
    I32_WMV iPosX = iXCoord + iX; 
    I32_WMV iPosY = iYCoord + iY;

    if (iPosX < -8 * 4 - 3) { // pullback to -8.x
        *iMvX = -32 - iXCoord - g_iNegOffset [iX & 3];
        bPullBack = TRUE;
    } else if (iPosX > iWidth + 3) { // pullback to w.x
        *iMvX = iWidth - iXCoord + (iX & 3);
        bPullBack = TRUE;
    }

    if (iPosY < -8 * 4 - 3) { // pullback to -8.x
        *iMvY = -32 - iYCoord - g_iNegOffset [iY & 3];
        bPullBack = TRUE;
    } else if (iPosY > iHeight + 3) { // pullback to h.x
        *iMvY = iHeight - iYCoord + (iY & 3);
        bPullBack = TRUE;
    }

    return bPullBack;
}

Bool_WMV PullBackFieldMBMotionVector (tWMVDecInternalMember *pWMVDec,
    I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iField)
{
    I32_WMV iX = *iMvX;
    I32_WMV iY = *iMvY;
    I32_WMV iXCoord = imbX * 16 * 4; // q-pel unit
    I32_WMV iYCoord = imbY * 16 * 4 + iField * 4;
    I32_WMV iWidth = pWMVDec->m_uintNumMBX * 16 * 4;
    I32_WMV iHeight = pWMVDec->m_uintNumMBY * 16 * 4;
    Bool_WMV bPullBack = FALSE;

    I32_WMV iPosX = iXCoord + iX; 
    I32_WMV iPosY = iYCoord + iY;

    if (iPosX < -16 * 4 - 3) { // pullback to -16.x
        *iMvX = -64 - iXCoord - g_iNegOffset [iX & 3];
        bPullBack = TRUE;
    } else if (iPosX > iWidth + 3) { // pullback to w.x
        *iMvX = iWidth - iXCoord + (iX & 3);
        bPullBack = TRUE;
    }

    if (iPosY & 4) { // odd
        if (iPosY < -16 * 4 - 3) { // pullback to -16.x for odd lines
            *iMvY = -64 - iYCoord - g_iNegOffset [iY & 3];
            bPullBack = TRUE;
        } else if (iPosY > iHeight + 4 + 3) { // pullback to h+1.x
            *iMvY = iHeight + 4 - iYCoord + (iY & 3);
            bPullBack = TRUE;
        }
    } else { // even
        if (iPosY < -17 * 4 - 3) { // pullback to -17.x for even lines
            *iMvY = -68 - iYCoord - g_iNegOffset [iY & 3];
            bPullBack = TRUE;
        } else if (iPosY > iHeight + 3) { // pullback to h.x
            *iMvY = iHeight - iYCoord + (iY & 3);
            bPullBack = TRUE;
        }
    }

    return bPullBack;
}

Bool_WMV PullBackFieldBlockMotionVector (tWMVDecInternalMember *pWMVDec,
    I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk)
{
    I32_WMV iX = *iMvX;
    I32_WMV iY = *iMvY;
    I32_WMV iXCoord = imbX * 16 * 4 + (iBlk & 1) * 8 * 4; // q-pel unit
    I32_WMV iYCoord = imbY * 16 * 4 + ((iBlk & 2)>>1) * 1 * 4;
    I32_WMV iWidth = pWMVDec->m_uintNumMBX * 16 * 4;
    I32_WMV iHeight = pWMVDec->m_uintNumMBY * 16 * 4;
    Bool_WMV bPullBack = FALSE;

    I32_WMV iPosX = iXCoord + iX; 
    I32_WMV iPosY = iYCoord + iY;

    if (iPosX < -8 * 4 - 3) { // pullback to -8.x
        *iMvX = -32 - iXCoord - g_iNegOffset [iX & 3];
        bPullBack = TRUE;
    } else if (iPosX > iWidth + 3) { // pullback to w.x
        *iMvX = iWidth - iXCoord + (iX & 3);
        bPullBack = TRUE;
    }

    if (iPosY & 4) { // odd
        if (iPosY < -8 * 4 - 3) { // pullback to -8.x for odd lines
            *iMvY = -32 - iYCoord - g_iNegOffset [iY & 3];
            bPullBack = TRUE;
        } else if (iPosY > iHeight + 4 + 3) { // pullback to h+1.x
            *iMvY = iHeight + 4 - iYCoord + (iY & 3);
            bPullBack = TRUE;
        }
    } else { // even
        if (iPosY < -9 * 4 - 3) { // pullback to -9.x for even lines
            *iMvY = -36 - iYCoord - g_iNegOffset [iY & 3];
            bPullBack = TRUE;
        } else if (iPosY > iHeight + 3) { // pullback to h.x
            *iMvY = iHeight - iYCoord + (iY & 3);
            bPullBack = TRUE;
        }
    }

    return bPullBack;
}


Void_WMV MotionCompFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    I32_WMV iMvX = pWMVDec->m_pXMotion [iMvIndex];
    I32_WMV iMvY = pWMVDec->m_pYMotion [iMvIndex];
    I32_WMV iChromaMvX, iChromaMvY, iOffset;
	int xindex=0,yindex=0;
	int IY,IX;

    // this is required for correct rounding in repeatpad area
    if(pWMVDec->m_bCodecIsWVC1){
		PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY); 
		xindex = iMvX;
		yindex = iMvY;
    }
    else {
       PullBackFrameMBMotionVector (pWMVDec, &iMvX, &iMvY, imbX, imbY);
    }

    
    // Luma
    if(pWMVDec->m_bCodecIsWVC1){
		Round_MV_Luam_FRAMEMV(pWMVDec, &IX, &IY,0, xindex, yindex);
		pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (pWMVDec->m_iWidthPrevY) + IX;

        pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, ppxliReconY, pWMVDec->m_iWidthPrevY, xindex, yindex, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
    }
    else {
      pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
      pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, ppxliReconY, pWMVDec->m_iWidthPrevY, iMvX, iMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
    }

    // Chroma
    if(pWMVDec->m_bCodecIsWVC1){
	 xindex = pWMVDec->m_pXMotion [iMvIndex];
     yindex = pWMVDec->m_pYMotion [iMvIndex];
        ChromaMVInterlaceV2 (imbX, imbY, xindex, yindex, &iChromaMvX, &iChromaMvY, FALSE);
		PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);
		Round_MV_Chroma_FRAMEMV(pWMVDec, &IX, &IY,0, iChromaMvX, iChromaMvY);
		pSrcU = pWMVDec->vo_ppxliRefU +IY * (pWMVDec->m_iWidthPrevUV) + IX;
		pSrcV = pWMVDec->vo_ppxliRefV +IY * (pWMVDec->m_iWidthPrevUV) + IX;
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, ppxliReconU, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);  
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, ppxliReconV, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8); 

    }
    else 
	{
       ChromaMVInterlaceV2 (imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY, FALSE);
	iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + iOffset;
    pSrcV = ppxliRefV + iOffset;
        
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, ppxliReconU, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);  
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, ppxliReconV, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8); 
}
}

Void_WMV MotionCompFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    U8_WMV *pDstY, *pDstU, *pDstV;
    I32_WMV iTopMvX, iTopMvY, iBotMvX, iBotMvY;
    I32_WMV iChromaMvX, iChromaMvY;
    I32_WMV iMvIndex, iOffset;

	int xindexT=0,yindexT=0,xindexB=0,yindexB=0;
	int IX,IY,F;

    iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    iTopMvX = pWMVDec->m_pXMotion [iMvIndex];
    iTopMvY = pWMVDec->m_pYMotion [iMvIndex];
    iMvIndex += pWMVDec->m_uintNumMBX * 2;
    iBotMvX = pWMVDec->m_pXMotion [iMvIndex];
    iBotMvY = pWMVDec->m_pYMotion [iMvIndex];

    // this is required for correct rounding in repeatpad area
    if(pWMVDec->m_bCodecIsWVC1){
	PullBackMotionVector ( pWMVDec, &iTopMvX, &iTopMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iBotMvX, &iBotMvY, imbX, imbY);

	xindexT = iTopMvX;
	yindexT = iTopMvY;
	xindexB = iBotMvX;
	yindexB = iBotMvY;
    }
    else {
    PullBackFieldMBMotionVector (pWMVDec, &iTopMvX, &iTopMvY, imbX, imbY, 0);
    PullBackFieldMBMotionVector (pWMVDec, &iBotMvX, &iBotMvY, imbX, imbY, 1);
    }

    // Luma
    // Top Field
    if(pWMVDec->m_bCodecIsWVC1){
	Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexT, yindexT);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, xindexT, yindexT, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);   
    }
    else {
    pSrcY = ppxliRefY + (iTopMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopMvX >> 2);
    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, iTopMvX, iTopMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);   
    }

    // Bot Field
    if(pWMVDec->m_bCodecIsWVC1){
	Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexB, yindexB);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp +pWMVDec->m_iWidthPrevY+IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, xindexB, yindexB, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);     
    }
    else {
    pSrcY = ppxliRefY + (iBotMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iBotMvX >> 2) + pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, iBotMvX, iBotMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);     
    }


    // Chroma
    // Top Field
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexT, yindexT, &iChromaMvX, &iChromaMvY, TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix 
	Round_MV_Chroma_FILEDMV(pWMVDec, &IX, &IY,&F, iChromaMvX, iChromaMvY);

	pSrcU = pWMVDec->vo_ppxliRefU+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcU += pWMVDec->m_iWidthPrevUV;
	pSrcV = pWMVDec->vo_ppxliRefV+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcV += pWMVDec->m_iWidthPrevUV;
    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iTopMvX, iTopMvY, &iChromaMvX, &iChromaMvY, TRUE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + iOffset;
    pSrcV = ppxliRefV + iOffset;
    }
    pDstU = ppxliReconU;
    pDstV = ppxliReconV;
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);

    // Bot Field
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexB, yindexB, &iChromaMvX, &iChromaMvY, TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix 
	Round_MV_Chroma_FILEDMV(pWMVDec, &IX, &IY,&F, iChromaMvX, iChromaMvY);

	pSrcU = pWMVDec->vo_ppxliRefU + pWMVDec->m_iWidthPrevUV+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcU += pWMVDec->m_iWidthPrevUV;
	pSrcV = pWMVDec->vo_ppxliRefV+ pWMVDec->m_iWidthPrevUV+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcV += pWMVDec->m_iWidthPrevUV;
    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iBotMvX, iBotMvY, &iChromaMvX, &iChromaMvY, TRUE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + pWMVDec->m_iWidthPrevUV + iOffset;
    pSrcV = ppxliRefV + pWMVDec->m_iWidthPrevUV + iOffset;
    }
    pDstU = ppxliReconU + pWMVDec->m_iWidthPrevUV;
    pDstV = ppxliReconV + pWMVDec->m_iWidthPrevUV;
    
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV,
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
}

#ifdef _WMV9AP_ 
Void_WMV MotionCompSingleFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV,
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX,
    I32_WMV iBotMvY, I32_WMV iBot)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    U8_WMV *pDstY, *pDstU, *pDstV;    
    I32_WMV iChromaMvX, iChromaMvY;
    I32_WMV iOffset;
	int xindexT=0,yindexT=0,xindexB=0,yindexB=0;
	int IX,IY,F;

    // this is required for correct rounding in repeatpad area
    if(pWMVDec->m_bCodecIsWVC1){
	PullBackMotionVector ( pWMVDec, &iTopMvX, &iTopMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iBotMvX, &iBotMvY, imbX, imbY);

	xindexT = iTopMvX;
	yindexT = iTopMvY;
	xindexB = iBotMvX;
	yindexB = iBotMvY;
    }
    else {
    PullBackFieldMBMotionVector (pWMVDec, &iTopMvX, &iTopMvY, imbX, imbY, 0);
    PullBackFieldMBMotionVector (pWMVDec, &iBotMvX, &iBotMvY, imbX, imbY, 1);
    }

    // Luma
    // Top Field
    if(pWMVDec->m_bCodecIsWVC1){
	Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexT, yindexT);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;

    pDstY = ppxliReconY;
	if (!iBot)
		pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, xindexT, yindexT, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);   
    }
    else {
    pSrcY = ppxliRefY + (iTopMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopMvX >> 2);
    pDstY = ppxliReconY;
	if (!iBot)
		pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, iTopMvX, iTopMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);   
    }

    // Bot Field
    if(pWMVDec->m_bCodecIsWVC1){
	Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexB, yindexB);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp + pWMVDec->m_iWidthPrevY+IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;

        pDstY = ppxliReconY + iReconStrideY;
	if (iBot)
		pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, xindexB, yindexB, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);     
    }
    else {
    pSrcY = ppxliRefY + (iBotMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iBotMvX >> 2) + pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY + iReconStrideY;
	if (iBot)
		pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, iBotMvX, iBotMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);     
    }

    // Chroma
    // Top Field
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexT, yindexT, &iChromaMvX, &iChromaMvY, TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);
    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iTopMvX, iTopMvY, &iChromaMvX, &iChromaMvY, TRUE);
    }    
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + iOffset;
    pSrcV = ppxliRefV + iOffset;
    pDstU = ppxliReconU;
    pDstV = ppxliReconV;

	if (!iBot)
	{
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*iReconStrideUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*iReconStrideUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
	}

    // Bot Field
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexB, yindexB, &iChromaMvX, &iChromaMvY, TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);
    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iBotMvX, iBotMvY, &iChromaMvX, &iChromaMvY, TRUE);
    }
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + pWMVDec->m_iWidthPrevUV + iOffset;
    pSrcV = ppxliRefV + pWMVDec->m_iWidthPrevUV + iOffset;
    pDstU = ppxliReconU + iReconStrideUV;
    pDstV = ppxliReconV + iReconStrideUV;
    
	if (iBot)
	{
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*iReconStrideUV,
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*iReconStrideUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
	}
}
Void_WMV MotionCompFrameInterlaceV2MV (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, 
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, I32_WMV iMvX, I32_WMV iMvY)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    I32_WMV iChromaMvX, iChromaMvY, iOffset;
	int xindex,yindex;
	int IX,IY,tmpx,tmpy;

    // this is required for correct rounding in repeatpad area
    if(pWMVDec->m_bCodecIsWVC1)
	{
		tmpx = iMvX;
		tmpy =iMvY;

		PullBackMotionVector ( pWMVDec, &iMvX, &iMvY, imbX, imbY);
		xindex = iMvX;
		yindex = iMvY;
	   
		// Luma
		Round_MV_Luam_FRAMEMV(pWMVDec, &IX, &IY,0, xindex, yindex);
		pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (pWMVDec->m_iWidthPrevY) + IX;

		pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, ppxliReconY, iReconStrideY, xindex, yindex, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);

		// Chroma
		xindex = tmpx;
		yindex = tmpy;

		ChromaMVInterlaceV2 (imbX, imbY, xindex, yindex, &iChromaMvX, &iChromaMvY, FALSE);
		PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);

		Round_MV_Chroma_FRAMEMV(pWMVDec, &IX, &IY,0, iChromaMvX, iChromaMvY);
		pSrcU = pWMVDec->vo_ppxliRefU +IY * (pWMVDec->m_iWidthPrevUV) + IX;
		pSrcV = pWMVDec->vo_ppxliRefV +IY * (pWMVDec->m_iWidthPrevUV) + IX;
	        
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, ppxliReconU, iReconStrideUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);  
		pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, ppxliReconV, iReconStrideUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8); 
    }
    else {
		PullBackFrameMBMotionVector (pWMVDec, &iMvX, &iMvY, imbX, imbY);
		// Luma
		pSrcY = ppxliRefY + (iMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iMvX >> 2);
		pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, ppxliReconY, iReconStrideY, iMvX, iMvY, 
			pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
		// Chroma
		ChromaMVInterlaceV2 (imbX, imbY, iMvX, iMvY, &iChromaMvX, &iChromaMvY, FALSE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + iOffset;
    pSrcV = ppxliRefV + iOffset;
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, ppxliReconU, iReconStrideUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);  
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, ppxliReconV, iReconStrideUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8); 
    }

}

Void_WMV MotionCompFieldInterlaceV2MV (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV,
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX,
    I32_WMV iBotMvY)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    U8_WMV *pDstY, *pDstU, *pDstV;    
    I32_WMV iChromaMvX, iChromaMvY;
    I32_WMV iOffset;
	int xindexT =0,yindexT=0,xindexB=0,yindexB=0;
	int IX,IY,F;


    // this is required for correct rounding in repeatpad area
    if(pWMVDec->m_bCodecIsWVC1){
		PullBackMotionVector ( pWMVDec, &iTopMvX, &iTopMvY, imbX, imbY);
		PullBackMotionVector ( pWMVDec, &iBotMvX, &iBotMvY, imbX, imbY);
		xindexT = iTopMvX;
		yindexT = iTopMvY;
		xindexB = iBotMvX;
		yindexB = iBotMvY;
    }
    else {
		PullBackFieldMBMotionVector (pWMVDec, &iTopMvX, &iTopMvY, imbX, imbY, 0);
		PullBackFieldMBMotionVector (pWMVDec, &iBotMvX, &iBotMvY, imbX, imbY, 1);
	}

    // Luma
    // Top Field
    if(pWMVDec->m_bCodecIsWVC1){
 	if(0)
	{
		Round_MV_Luam ( pWMVDec, &iTopMvX, &iTopMvY); 
		Round_MV_Luam ( pWMVDec, &iBotMvX, &iBotMvY);
		pSrcY = ppxliRefY + (iTopMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopMvX >> 2);
	}
	else
	{
		Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexT, yindexT);
		pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (2*pWMVDec->m_iWidthPrevY) + IX;
		if (F)
			pSrcY += pWMVDec->m_iWidthPrevY;
	}

    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, xindexT, yindexT, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);   
   }
    else {
    pSrcY = ppxliRefY + (iTopMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopMvX >> 2);
    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, iTopMvX, iTopMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);   
	}

    // Bot Field
    if(pWMVDec->m_bCodecIsWVC1){
	if(0)
	{
		pSrcY = ppxliRefY + (iBotMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iBotMvX >> 2) + pWMVDec->m_iWidthPrevY;
	}
	else
	{
		Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexB, yindexB);
		pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (2*pWMVDec->m_iWidthPrevY) + IX + pWMVDec->m_iWidthPrevY;
		if (F)
			pSrcY += pWMVDec->m_iWidthPrevY;
	}
    pDstY = ppxliReconY + iReconStrideY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, xindexB, yindexB, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);     
   }
    else {
    pSrcY = ppxliRefY + (iBotMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iBotMvX >> 2) + pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY + iReconStrideY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*iReconStrideY, iBotMvX, iBotMvY, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);     
    }

    // Chroma
    // Top Field
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexT, yindexT, &iChromaMvX, &iChromaMvY, TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix 
   }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iTopMvX, iTopMvY, &iChromaMvX, &iChromaMvY, TRUE);
    }
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + iOffset;
    pSrcV = ppxliRefV + iOffset;
    pDstU = ppxliReconU;
    pDstV = ppxliReconV;
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*iReconStrideUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*iReconStrideUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);

    // Bot Field
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexB, yindexB, &iChromaMvX, &iChromaMvY, TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix 
   }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iBotMvX, iBotMvY, &iChromaMvX, &iChromaMvY, TRUE);
    }
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + pWMVDec->m_iWidthPrevUV + iOffset;
    pSrcV = ppxliRefV + pWMVDec->m_iWidthPrevUV + iOffset;
    pDstU = ppxliReconU + iReconStrideUV;
    pDstV = ppxliReconV + iReconStrideUV;
    
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*iReconStrideUV,
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*iReconStrideUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
}
Void_WMV MotionCompFrameInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRef0Y, U8_WMV *ppxliRef0U, U8_WMV *ppxliRef0V,
	U8_WMV *ppxliRef1Y, U8_WMV *ppxliRef1U, U8_WMV *ppxliRef1V,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, 
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, MBType mbType)
{
    I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
	U8_WMV rgpxlcBlock [384 + 32];
	U8_WMV *pBlock = (U8_WMV*) (((U32_WMV)rgpxlcBlock + 31) & ~31);
	U8_WMV rgpxlcBlock1 [384 + 32];
	U8_WMV *pBlock1 = (U8_WMV*) (((U32_WMV)rgpxlcBlock1 + 31) & ~31);
	I32_WMV    iFMVx = 0, iFMVy = 0;
	I32_WMV    iBMVx = 0, iBMVy = 0;

	if (mbType == DIRECT)
	{
		I32_WMV iMvXdirect = pWMVDec->m_pFieldMvX_Pbuf [iMvIndex];
		I32_WMV iMvYdirect = pWMVDec->m_pFieldMvY_Pbuf [iMvIndex];
        DirectModeMV (pWMVDec, iMvXdirect, iMvYdirect, FALSE, imbX, imbY, &iFMVx, &iFMVy, &iBMVx, &iBMVy);
	}
	else if (mbType == INTERPOLATE)
	{
		iFMVx = pWMVDec->m_pXMotion[iMvIndex];
		iFMVy = pWMVDec->m_pYMotion[iMvIndex];
		iBMVx = pWMVDec->m_pB411InterpX[iMvIndex]; 
		iBMVy = pWMVDec->m_pB411InterpY[iMvIndex]; 
	}
	else
		assert(0);

    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef0YPlusExp;
	 pWMVDec->vo_ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->vo_ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    }

	MotionCompFrameInterlaceV2MV (pWMVDec, imbX, imbY, ppxliRef0Y, ppxliRef0U, ppxliRef0V, 
									pBlock, pBlock + 256, pBlock + 256 + 64, 
									16, 8, iFMVx, iFMVy);
    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef1YPlusExp;
	 pWMVDec->vo_ppxliRefU = pWMVDec->m_ppxliRef1U+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
	 pWMVDec->vo_ppxliRefV = pWMVDec->m_ppxliRef1V+ pWMVDec->m_iWidthPrevUVXExpPlusExp;
    }

	MotionCompFrameInterlaceV2MV (pWMVDec, imbX, imbY, ppxliRef1Y, ppxliRef1U, ppxliRef1V, 
									pBlock1, pBlock1 + 256, pBlock1 + 256 + 64, 
									16, 8, iBMVx, iBMVy);

    {
        I32_WMV ip;
      if(pWMVDec->m_bCodecIsWVC1){
	for ( ip = 0; ip < 384; ip++)
		pBlock[ip] = (pBlock[ip] + pBlock1[ip]+1) / 2;
      }
      else {
	for ( ip = 0; ip < 384; ip++)
		pBlock[ip] = (pBlock[ip] + pBlock1[ip]) / 2;
      }
    }


	pWMVDec->m_pInterpolateMBRow (pWMVDec, pBlock, 16, ppxliReconY, iReconStrideY, 0, 0, 
							pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 16);
	pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pBlock + 256, 8, ppxliReconU, iReconStrideUV, 
							0, 0, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);  
	pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pBlock + 256 + 64, 8, ppxliReconV, iReconStrideUV, 
							0, 0, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8); 
}

Void_WMV MotionCompFieldInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRef0Y, U8_WMV *ppxliRef0U, U8_WMV *ppxliRef0V,
	U8_WMV *ppxliRef1Y, U8_WMV *ppxliRef1U, U8_WMV *ppxliRef1V,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV,
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, MBType mbType)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;
    U8_WMV *pDstY, *pDstU, *pDstV;    

	I32_WMV iTopLeftBlkIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2 * pWMVDec->m_uintNumMBX;

	U8_WMV rgpxlcBlock [384 + 32];
	U8_WMV *pBlock = (U8_WMV*) (((U32_WMV)rgpxlcBlock + 31) & ~31);
	U8_WMV rgpxlcBlock1 [384 + 32];
	U8_WMV *pBlock1 = (U8_WMV*) (((U32_WMV)rgpxlcBlock1 + 31) & ~31);

	I32_WMV iFMVx=0, iFMVy=0, iBMVx=0, iBMVy=0;
	I32_WMV iFMV1x=0, iFMV1y=0, iBMV1x=0, iBMV1y=0;

	if (mbType == DIRECT)
	{
		I32_WMV iX = pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex];
		I32_WMV iY = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex];

		I32_WMV iX1 = pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex];
		I32_WMV iY1 = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex];

		DirectModeMV (pWMVDec, iX, iY, FALSE, imbX, imbY, &iFMVx, &iFMVy, &iBMVx, &iBMVy);
		DirectModeMV (pWMVDec, iX1, iY1, FALSE, imbX, imbY, &iFMV1x, &iFMV1y, &iBMV1x, &iBMV1y);
	}
	else if (mbType == INTERPOLATE)
	{
		iFMVx = pWMVDec->m_pXMotion[iTopLeftBlkIndex];
		iFMVy = pWMVDec->m_pYMotion[iTopLeftBlkIndex];
		iFMV1x = pWMVDec->m_pXMotion[iBotLeftBlkIndex];
		iFMV1y = pWMVDec->m_pYMotion[iBotLeftBlkIndex];
		iBMVx = pWMVDec->m_pB411InterpX[iTopLeftBlkIndex]; 
		iBMVy = pWMVDec->m_pB411InterpY[iTopLeftBlkIndex]; 
		iBMV1x = pWMVDec->m_pB411InterpX[iBotLeftBlkIndex]; 
		iBMV1y = pWMVDec->m_pB411InterpY[iBotLeftBlkIndex];
	}
	else if (mbType == FORWARD_BACKWARD)
	{
		I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
		I32_WMV iTopMvX = pWMVDec->m_pXMotion [iMvIndex];
		I32_WMV iTopMvY = pWMVDec->m_pYMotion [iMvIndex];
        I32_WMV iBotMvX;
        I32_WMV iBotMvY;

		iMvIndex += pWMVDec->m_uintNumMBX * 2;
	    iBotMvX = pWMVDec->m_pXMotion [iMvIndex];
	    iBotMvY = pWMVDec->m_pYMotion [iMvIndex];
    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef0YPlusExp;
    }
		MotionCompSingleFieldInterlaceV2 (pWMVDec, imbX, imbY, ppxliRef0Y, ppxliRef0U, ppxliRef0V, 
											ppxliReconY, ppxliReconU, ppxliReconV, 
											iReconStrideY, iReconStrideUV, iTopMvX, iTopMvY, 0, 0, 0);
    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef1YPlusExp;
    }
		MotionCompSingleFieldInterlaceV2 (pWMVDec, imbX, imbY, ppxliRef1Y, ppxliRef1U, ppxliRef1V, 
											ppxliReconY, ppxliReconU, ppxliReconV, 
											iReconStrideY, iReconStrideUV, 0, 0, iBotMvX, iBotMvY, 1);
		return;
	}
	else if (mbType == BACKWARD_FORWARD)
	{
		I32_WMV iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
		I32_WMV iTopMvX = pWMVDec->m_pXMotion [iMvIndex];
		I32_WMV iTopMvY = pWMVDec->m_pYMotion [iMvIndex];
I32_WMV iBotMvX;
I32_WMV iBotMvY;


		iMvIndex += pWMVDec->m_uintNumMBX * 2;
	    iBotMvX = pWMVDec->m_pXMotion [iMvIndex];
	    iBotMvY = pWMVDec->m_pYMotion [iMvIndex];
    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef1YPlusExp;
    }
		MotionCompSingleFieldInterlaceV2 (pWMVDec, imbX, imbY, ppxliRef1Y, ppxliRef1U, ppxliRef1V, 
											ppxliReconY, ppxliReconU, ppxliReconV, 
											iReconStrideY, iReconStrideUV, iTopMvX, iTopMvY, 0, 0, 0);
    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef0YPlusExp;
    }
		MotionCompSingleFieldInterlaceV2 (pWMVDec, imbX, imbY, ppxliRef0Y, ppxliRef0U, ppxliRef0V, 
											ppxliReconY, ppxliReconU, ppxliReconV, 
											iReconStrideY, iReconStrideUV, 0, 0, iBotMvX, iBotMvY, 1);
		return;
	}
	else
		assert(0);

    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef0YPlusExp;
    }
	MotionCompFieldInterlaceV2MV (pWMVDec, imbX, imbY, ppxliRef0Y, ppxliRef0U, ppxliRef0V, 
									pBlock, pBlock + 256, pBlock + 256 + 64, 
									16, 8, iFMVx, iFMVy, iFMV1x, iFMV1y);

    if(pWMVDec->m_bCodecIsWVC1){
	 pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef1YPlusExp;
    }
	MotionCompFieldInterlaceV2MV (pWMVDec, imbX, imbY, ppxliRef1Y, ppxliRef1U, ppxliRef1V, 
									pBlock1, pBlock1 + 256, pBlock1 + 256 + 64, 
									16, 8, iBMVx, iBMVy, iBMV1x, iBMV1y);

    {
        I32_WMV ip;
      if(pWMVDec->m_bCodecIsWVC1){
	for ( ip = 0; ip < 384; ip++)
		pBlock[ip] = (pBlock[ip] + pBlock1[ip] +1) / 2;  //zou fix
      }
      else {
	for ( ip = 0; ip < 384; ip++)
		pBlock[ip] = (pBlock[ip] + pBlock1[ip]) / 2;
      }
    }

    // Luma
    // Top Field
    pSrcY = pBlock;
    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*16, pDstY, 2*iReconStrideY, 0, 0, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);   

    // Bot Field
    pSrcY = pBlock + 16;
    pDstY = ppxliReconY + iReconStrideY;
    pWMVDec->m_pInterpolateMBRow (pWMVDec, pSrcY, 2*16, pDstY, 2*iReconStrideY, 0, 0, 
        pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);     

    pSrcU = pBlock + 256;
    pSrcV = pBlock + 256 + 64;
    pDstU = ppxliReconU;
    pDstV = ppxliReconV;
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*8, pDstU, 2*iReconStrideUV, 
        0, 0, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*8, pDstV, 2*iReconStrideUV, 
        0, 0, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);

    // Bot Field
    pSrcU = pSrcU + 8;
    pSrcV = pSrcV + 8;
    pDstU = ppxliReconU + iReconStrideUV;
    pDstV = ppxliReconV + iReconStrideUV;
    
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcU, 2*8, pDstU, 2*iReconStrideUV,
        0, 0, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolateBlockRowBilinear (pWMVDec, pSrcV, 2*8, pDstV, 2*iReconStrideUV, 
        0, 0, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
}
#endif

Void_WMV MotionCompFrame4MvInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;   
    U8_WMV *pDstY, *pDstU, *pDstV;
    I32_WMV iTopLeftMvX, iTopLeftMvY, iTopRightMvX, iTopRightMvY;
    I32_WMV iBotLeftMvX, iBotLeftMvY, iBotRightMvX, iBotRightMvY;
    I32_WMV iChromaMvX, iChromaMvY;
    I32_WMV iMvIndex, iOffset;
	int IX,IY;

	int xindexTL=0,yindexTL=0;
	int xindexTR=0,yindexTR=0;
	int xindexBL=0,yindexBL=0;
	int xindexBR=0,yindexBR=0;
  
    iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    iTopLeftMvX = pWMVDec->m_pXMotion [iMvIndex];
    iTopLeftMvY = pWMVDec->m_pYMotion [iMvIndex];
    iTopRightMvX = pWMVDec->m_pXMotion [iMvIndex + 1];
    iTopRightMvY = pWMVDec->m_pYMotion [iMvIndex + 1];    
    iMvIndex += 2 * pWMVDec->m_uintNumMBX;
    iBotLeftMvX = pWMVDec->m_pXMotion [iMvIndex];
    iBotLeftMvY = pWMVDec->m_pYMotion [iMvIndex];
    iBotRightMvX = pWMVDec->m_pXMotion [iMvIndex + 1];
    iBotRightMvY = pWMVDec->m_pYMotion [iMvIndex + 1];

    // this is required for correct rounding in repeatpad area
    if(pWMVDec->m_bCodecIsWVC1){
	PullBackMotionVector ( pWMVDec, &iTopLeftMvX, &iTopLeftMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iTopRightMvX, &iTopRightMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iBotLeftMvX, &iBotLeftMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iBotRightMvX, &iBotRightMvY, imbX, imbY);

	xindexTL = iTopLeftMvX;		
	yindexTL = iTopLeftMvY;
	xindexTR = iTopRightMvX;		
	yindexTR = iTopRightMvY;
	xindexBL = iBotLeftMvX;		
	yindexBL = iBotLeftMvY;
	xindexBR = iBotRightMvX;		
	yindexBR = iBotRightMvY;
    }
    else {
    PullBackFrameBlockMotionVector (pWMVDec, &iTopLeftMvX, &iTopLeftMvY, imbX, imbY, 0);
    PullBackFrameBlockMotionVector (pWMVDec, &iTopRightMvX, &iTopRightMvY, imbX, imbY, 1);
    PullBackFrameBlockMotionVector (pWMVDec, &iBotLeftMvX, &iBotLeftMvY, imbX, imbY, 2);
    PullBackFrameBlockMotionVector (pWMVDec, &iBotRightMvX, &iBotRightMvY, imbX, imbY, 3);
    }

    // Luma
    if(pWMVDec->m_bCodecIsWVC1){
    // Top Left Block 
	Round_MV_Luam_FRAMEMV(pWMVDec, &IX, &IY,0, xindexTL, yindexTL);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (pWMVDec->m_iWidthPrevY) + IX;

    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        xindexTL, yindexTL, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    
    // Top Right Block
	Round_MV_Luam_FRAMEMV(pWMVDec, &IX, &IY,1, xindexTR, yindexTR);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp /*+8*/+IY * (pWMVDec->m_iWidthPrevY) + IX;

    pDstY = ppxliReconY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        xindexTR, yindexTR, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

    // Bot Left Block
	Round_MV_Luam_FRAMEMV(pWMVDec, &IX, &IY,2, xindexBL, yindexBL);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp /*+ 8 * pWMVDec->m_iWidthPrevY*/+IY * (pWMVDec->m_iWidthPrevY) + IX;

    pDstY = ppxliReconY + 8 * pWMVDec->m_iWidthPrevY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        xindexBL, yindexBL, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    
    // Bot Right Block
	Round_MV_Luam_FRAMEMV(pWMVDec, &IX, &IY,3, xindexBR, yindexBR);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp + /*8 * pWMVDec->m_iWidthPrevY+8*/+IY * (pWMVDec->m_iWidthPrevY) + IX;

    pDstY = ppxliReconY + 8 * pWMVDec->m_iWidthPrevY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        xindexBR, yindexBR, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    }
    else {
    pSrcY = ppxliRefY + (iTopLeftMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopLeftMvX >> 2);
    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        iTopLeftMvX, iTopLeftMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    
    // Top Right Block
    pSrcY = ppxliRefY + 8 + (iTopRightMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopRightMvX >> 2);
    pDstY = ppxliReconY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        iTopRightMvX, iTopRightMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

    // Bot Left Block
    pSrcY = ppxliRefY + 8 * pWMVDec->m_iWidthPrevY + (iBotLeftMvY >> 2) * (pWMVDec->m_iWidthPrevY)
        + (iBotLeftMvX >> 2);
    pDstY = ppxliReconY + 8 * pWMVDec->m_iWidthPrevY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        iBotLeftMvX, iBotLeftMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    
    // Bot Right Block
    pSrcY = ppxliRefY + 8 * pWMVDec->m_iWidthPrevY + 8 + (iBotRightMvY >> 2) * (pWMVDec->m_iWidthPrevY) 
        + (iBotRightMvX >> 2);
    pDstY = ppxliReconY + 8 * pWMVDec->m_iWidthPrevY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, 
        iBotRightMvX, iBotRightMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    }

    // Chroma

    // Top Left
    if(pWMVDec->m_bCodecIsWVC1){
	iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    xindexTL = pWMVDec->m_pXMotion [iMvIndex];
    yindexTL = pWMVDec->m_pYMotion [iMvIndex];
    xindexTR = pWMVDec->m_pXMotion [iMvIndex + 1];
    yindexTR = pWMVDec->m_pYMotion [iMvIndex + 1];    
    iMvIndex += 2 * pWMVDec->m_uintNumMBX;
    xindexBL = pWMVDec->m_pXMotion [iMvIndex];
    yindexBL = pWMVDec->m_pYMotion [iMvIndex];
    xindexBR = pWMVDec->m_pXMotion [iMvIndex + 1];
    yindexBR = pWMVDec->m_pYMotion [iMvIndex + 1];

    ChromaMVInterlaceV2 (imbX, imbY, xindexTL, yindexTL, &iChromaMvX, &iChromaMvY,  FALSE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix

		Round_MV_Chroma_FRAMEMV(pWMVDec, &IX, &IY,0, iChromaMvX, iChromaMvY);
		pSrcU = pWMVDec->vo_ppxliRefU +IY * (pWMVDec->m_iWidthPrevUV) + IX;
		pSrcV = pWMVDec->vo_ppxliRefV +IY * (pWMVDec->m_iWidthPrevUV) + IX;

		/*iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
		pSrcU = ppxliRefU + iOffset;
		pSrcV = ppxliRefV + iOffset;*/

		pDstU = ppxliReconU;
		pDstV = ppxliReconV;
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    }
    else 
	{
		ChromaMVInterlaceV2 (imbX, imbY, iTopLeftMvX, iTopLeftMvY, &iChromaMvX, &iChromaMvY, FALSE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + iOffset;
    pSrcV = ppxliRefV + iOffset;
    pDstU = ppxliReconU;
    pDstV = ppxliReconV;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    }
    

    // Top Right
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexTR, yindexTR, &iChromaMvX, &iChromaMvY, FALSE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix

		Round_MV_Chroma_FRAMEMV(pWMVDec, &IX, &IY,0, iChromaMvX, iChromaMvY);
		pSrcU = pWMVDec->vo_ppxliRefU +4+IY * (pWMVDec->m_iWidthPrevUV) + IX;
		pSrcV = pWMVDec->vo_ppxliRefV +4+IY * (pWMVDec->m_iWidthPrevUV) + IX;

		/*iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
		pSrcU = ppxliRefU + 4 + iOffset;
		pSrcV = ppxliRefV + 4 + iOffset;*/
		pDstU = ppxliReconU + 4;
		pDstV = ppxliReconV + 4;
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    }
    else 
	{
		ChromaMVInterlaceV2 (imbX, imbY, iTopRightMvX, iTopRightMvY, &iChromaMvX, &iChromaMvY, FALSE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + 4 + iOffset;
    pSrcV = ppxliRefV + 4 + iOffset;
    pDstU = ppxliReconU + 4;
    pDstV = ppxliReconV + 4;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    }
    

    // Bot Left
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexBL, yindexBL, &iChromaMvX, &iChromaMvY, FALSE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix

		Round_MV_Chroma_FRAMEMV(pWMVDec, &IX, &IY,0, iChromaMvX, iChromaMvY);
		pSrcU = pWMVDec->vo_ppxliRefU +4 * pWMVDec->m_iWidthPrevUV+IY * (pWMVDec->m_iWidthPrevUV) + IX;
		pSrcV = pWMVDec->vo_ppxliRefV +4 * pWMVDec->m_iWidthPrevUV+IY * (pWMVDec->m_iWidthPrevUV) + IX;

		/*iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
		pSrcU = ppxliRefU + 4 * pWMVDec->m_iWidthPrevUV + iOffset;
		pSrcV = ppxliRefV + 4 * pWMVDec->m_iWidthPrevUV + iOffset;*/
		pDstU = ppxliReconU + 4 * pWMVDec->m_iWidthPrevUV;
		pDstV = ppxliReconV + 4 * pWMVDec->m_iWidthPrevUV;
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    }
    else 
	{
		ChromaMVInterlaceV2 (imbX, imbY, iBotLeftMvX, iBotLeftMvY, &iChromaMvX, &iChromaMvY,FALSE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + 4 * pWMVDec->m_iWidthPrevUV + iOffset;
    pSrcV = ppxliRefV + 4 * pWMVDec->m_iWidthPrevUV + iOffset;
    pDstU = ppxliReconU + 4 * pWMVDec->m_iWidthPrevUV;
    pDstV = ppxliReconV + 4 * pWMVDec->m_iWidthPrevUV;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    }
    

    // Bot Right
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexBR, yindexBR, &iChromaMvX, &iChromaMvY, FALSE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix

		Round_MV_Chroma_FRAMEMV(pWMVDec, &IX, &IY,0, iChromaMvX, iChromaMvY);
		pSrcU = pWMVDec->vo_ppxliRefU + 4 * pWMVDec->m_iWidthPrevUV + 4+IY * (pWMVDec->m_iWidthPrevUV) + IX;
		pSrcV = pWMVDec->vo_ppxliRefV + 4 * pWMVDec->m_iWidthPrevUV + 4+IY * (pWMVDec->m_iWidthPrevUV) + IX;

		/*iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
		pSrcU = ppxliRefU + 4 * pWMVDec->m_iWidthPrevUV + 4 + iOffset;
		pSrcV = ppxliRefV + 4 * pWMVDec->m_iWidthPrevUV + 4 + iOffset;*/

		pDstU = ppxliReconU + 4 * pWMVDec->m_iWidthPrevUV + 4;
		pDstV = ppxliReconV + 4 * pWMVDec->m_iWidthPrevUV + 4;
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
		pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
			iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    }
    else 
	{
		ChromaMVInterlaceV2 (imbX, imbY, iBotRightMvX, iBotRightMvY, &iChromaMvX, &iChromaMvY, FALSE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + 4 * pWMVDec->m_iWidthPrevUV + 4 + iOffset;
    pSrcV = ppxliRefV + 4 * pWMVDec->m_iWidthPrevUV + 4 + iOffset;
    pDstU = ppxliReconU + 4 * pWMVDec->m_iWidthPrevUV + 4;
    pDstV = ppxliReconV + 4 * pWMVDec->m_iWidthPrevUV + 4;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
}

}

Void_WMV MotionCompField4MvInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV)
{
    U8_WMV *pSrcY, *pSrcU, *pSrcV;   
    U8_WMV *pDstY, *pDstU, *pDstV;
    I32_WMV iTopLeftMvX, iTopLeftMvY, iTopRightMvX, iTopRightMvY;
    I32_WMV iBotLeftMvX, iBotLeftMvY, iBotRightMvX, iBotRightMvY;
    I32_WMV iChromaMvX, iChromaMvY;
    I32_WMV iMvIndex, iOffset;
  
	int xindexTL=0,yindexTL=0;
	int xindexTR=0,yindexTR=0;
	int xindexBL=0,yindexBL=0;
	int xindexBR=0,yindexBR=0;
	int F;

	int IX,IY;
  
    iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    iTopLeftMvX = pWMVDec->m_pXMotion [iMvIndex];
    iTopLeftMvY = pWMVDec->m_pYMotion [iMvIndex];
    iTopRightMvX = pWMVDec->m_pXMotion [iMvIndex + 1];
    iTopRightMvY = pWMVDec->m_pYMotion [iMvIndex + 1];    
    iMvIndex += 2 * pWMVDec->m_uintNumMBX;
    iBotLeftMvX = pWMVDec->m_pXMotion [iMvIndex];
    iBotLeftMvY = pWMVDec->m_pYMotion [iMvIndex];
    iBotRightMvX = pWMVDec->m_pXMotion [iMvIndex + 1];
    iBotRightMvY = pWMVDec->m_pYMotion [iMvIndex + 1];

    // this is required for correct rounding in repeatpad area
    if(pWMVDec->m_bCodecIsWVC1){
	PullBackMotionVector ( pWMVDec, &iTopLeftMvX, &iTopLeftMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iTopRightMvX, &iTopRightMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iBotLeftMvX, &iBotLeftMvY, imbX, imbY);
	PullBackMotionVector ( pWMVDec, &iBotRightMvX, &iBotRightMvY, imbX, imbY);

	xindexTL = iTopLeftMvX;		
	yindexTL = iTopLeftMvY;
	xindexTR = iTopRightMvX;		
	yindexTR = iTopRightMvY;
	xindexBL = iBotLeftMvX;		
	yindexBL = iBotLeftMvY;
	xindexBR = iBotRightMvX;		
	yindexBR = iBotRightMvY;	
    }
    else {
    PullBackFieldBlockMotionVector (pWMVDec, &iTopLeftMvX, &iTopLeftMvY, imbX, imbY,   0);
    PullBackFieldBlockMotionVector (pWMVDec, &iTopRightMvX, &iTopRightMvY, imbX, imbY, 1);
    PullBackFieldBlockMotionVector (pWMVDec, &iBotLeftMvX, &iBotLeftMvY, imbX, imbY,  2);
    PullBackFieldBlockMotionVector (pWMVDec, &iBotRightMvX, &iBotRightMvY, imbX, imbY, 3);
    }

    // Luma
    if(pWMVDec->m_bCodecIsWVC1){
	Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexTL, yindexTL);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp +IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
         xindexTL,yindexTL, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    
	// Top Right Block
	Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexTR, yindexTR);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp +8+IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
        xindexTR, yindexTR, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
	
    // Bot Left Block
	Round_MV_Luam_FILEDMV(pWMVDec, &IX,&IY,&F, xindexBL, yindexBL);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp + pWMVDec->m_iWidthPrevY+IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
        xindexBL, yindexBL, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
	
    // Bot Right Block
    Round_MV_Luam_FILEDMV(pWMVDec, &IX, &IY,&F, xindexBR, yindexBR);
	pSrcY = pWMVDec->vo_ppxliRefYPlusExp + pWMVDec->m_iWidthPrevY +8+IY * (2*pWMVDec->m_iWidthPrevY) + IX;
	if (F)
		pSrcY += pWMVDec->m_iWidthPrevY;
    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
        xindexBR, yindexBR, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    }
    else {
   // Top Left Block 
    pSrcY = ppxliRefY + (iTopLeftMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopLeftMvX >> 2);
    pDstY = ppxliReconY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
        iTopLeftMvX, iTopLeftMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    
    // Top Right Block
    pSrcY = ppxliRefY + 8 +(iTopRightMvY >> 2) * (pWMVDec->m_iWidthPrevY) + (iTopRightMvX >> 2);
    pDstY = ppxliReconY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
        iTopRightMvX, iTopRightMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);

    // Bot Left Block
    pSrcY = ppxliRefY + pWMVDec->m_iWidthPrevY + (iBotLeftMvY >> 2) * (pWMVDec->m_iWidthPrevY)
        + (iBotLeftMvX >> 2);
    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
        iBotLeftMvX, iBotLeftMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    
    // Bot Right Block
    pSrcY = ppxliRefY + pWMVDec->m_iWidthPrevY + 8 + (iBotRightMvY >> 2) * (pWMVDec->m_iWidthPrevY) 
        + (iBotRightMvX >> 2);
    pDstY = ppxliReconY + pWMVDec->m_iWidthPrevY + 8;
    pWMVDec->m_pInterpolateBlockRow (pWMVDec, pSrcY, 2*pWMVDec->m_iWidthPrevY, pDstY, 2*pWMVDec->m_iWidthPrevY, 
        iBotRightMvX, iBotRightMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 8);
    }



    // Chroma
    // Top Left
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexTL,yindexTL, &iChromaMvX, &iChromaMvY,TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix
	Round_MV_Chroma_FILEDMV(pWMVDec, &IX, &IY,&F, iChromaMvX, iChromaMvY);
	pSrcU = pWMVDec->vo_ppxliRefU +IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcU += pWMVDec->m_iWidthPrevUV;

	pSrcV = pWMVDec->vo_ppxliRefV +IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcV += pWMVDec->m_iWidthPrevUV;

    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iTopLeftMvX, iTopLeftMvY, &iChromaMvX, &iChromaMvY,
        TRUE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + iOffset;
    pSrcV = ppxliRefV + iOffset;
    }
    pDstU = ppxliReconU;
    pDstV = ppxliReconV;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);

    // Top Right
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY,xindexTR, yindexTR, &iChromaMvX, &iChromaMvY,
        TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix
	Round_MV_Chroma_FILEDMV(pWMVDec, &IX, &IY,&F, iChromaMvX, iChromaMvY);

	pSrcU = pWMVDec->vo_ppxliRefU+4+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcU += pWMVDec->m_iWidthPrevUV;
	pSrcV = pWMVDec->vo_ppxliRefV+4 +IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcV += pWMVDec->m_iWidthPrevUV;
    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iTopRightMvX, iTopRightMvY, &iChromaMvX, &iChromaMvY,
        TRUE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + 4 + iOffset;
    pSrcV = ppxliRefV + 4 + iOffset;
    }
    pDstU = ppxliReconU + 4;
    pDstV = ppxliReconV + 4;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);

    // Bot Left
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexBL, yindexBL, &iChromaMvX, &iChromaMvY, TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix
	Round_MV_Chroma_FILEDMV(pWMVDec, &IX, &IY,&F, iChromaMvX, iChromaMvY);
	pSrcU = pWMVDec->vo_ppxliRefU +pWMVDec->m_iWidthPrevUV+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcU += pWMVDec->m_iWidthPrevUV;
	pSrcV = pWMVDec->vo_ppxliRefV +pWMVDec->m_iWidthPrevUV +IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcV += pWMVDec->m_iWidthPrevUV;

    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iBotLeftMvX, iBotLeftMvY, &iChromaMvX, &iChromaMvY, 
        TRUE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + pWMVDec->m_iWidthPrevUV + iOffset;
    pSrcV = ppxliRefV + pWMVDec->m_iWidthPrevUV + iOffset;
    }
    pDstU = ppxliReconU + pWMVDec->m_iWidthPrevUV;
    pDstV = ppxliReconV + pWMVDec->m_iWidthPrevUV;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec,  pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);

    // Bot Right
    if(pWMVDec->m_bCodecIsWVC1){
    ChromaMVInterlaceV2 (imbX, imbY, xindexBR, yindexBR, &iChromaMvX, &iChromaMvY,  TRUE);
	PullBackMotionVector_UV (pWMVDec, &iChromaMvX, &iChromaMvY, imbX, imbY);  //zou inter_fix
	Round_MV_Chroma_FILEDMV(pWMVDec, &IX, &IY,&F, iChromaMvX, iChromaMvY);
	pSrcU = pWMVDec->vo_ppxliRefU +pWMVDec->m_iWidthPrevUV+4+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcU += pWMVDec->m_iWidthPrevUV;
	pSrcV = pWMVDec->vo_ppxliRefV +pWMVDec->m_iWidthPrevUV +4+IY * (2*pWMVDec->m_iWidthPrevUV) + IX;
	if (F)
		pSrcV += pWMVDec->m_iWidthPrevUV;
    }
    else {
    ChromaMVInterlaceV2 (imbX, imbY, iBotRightMvX, iBotRightMvY, &iChromaMvX, &iChromaMvY, 
        TRUE);
    iOffset = (iChromaMvY >> 2) * pWMVDec->m_iWidthPrevUV + (iChromaMvX >> 2);
    pSrcU = ppxliRefU + pWMVDec->m_iWidthPrevUV + 4 + iOffset;
    pSrcV = ppxliRefV + pWMVDec->m_iWidthPrevUV + 4 + iOffset;
    }
    pDstU = ppxliReconU + pWMVDec->m_iWidthPrevUV + 4;
    pDstV = ppxliReconV + pWMVDec->m_iWidthPrevUV + 4;
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcU, 2*pWMVDec->m_iWidthPrevUV, pDstU, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
    pWMVDec->m_pInterpolate4ByteRowBilinear (pWMVDec, pSrcV, 2*pWMVDec->m_iWidthPrevUV, pDstV, 2*pWMVDec->m_iWidthPrevUV, 
        iChromaMvX, iChromaMvY, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl, 4);
}

Void_WMV AddErrorFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    U8_WMV *pDst;
    I16_WMV *pSrc;

    // Y0
    pDst = ppxlcCurrY;
    pSrc = ppxliFieldMB;
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // Y1
    pDst = ppxlcCurrY + 8;
    pSrc = ppxliFieldMB + 64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // Y2
    pDst = ppxlcCurrY + 8 * iOffsetY;
    pSrc = ppxliFieldMB + 2*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // Y3
    pDst = ppxlcCurrY + 8 * iOffsetY + 8;
    pSrc = ppxliFieldMB + 3*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetY);

    // U
    pDst = ppxlcCurrU;
    pSrc = ppxliFieldMB + 4*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetUV);  

    // V
    pDst = ppxlcCurrV;
    pSrc = ppxliFieldMB + 5*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetUV);  
}

Void_WMV AddErrorFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV)
{
    U8_WMV *pDst;
    I16_WMV *pSrc;

    // Y0
    pDst = ppxlcCurrY;
    pSrc = ppxliFieldMB;
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // Y1
    pDst = ppxlcCurrY + 8;
    pSrc = ppxliFieldMB + 64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // Y2
    pDst = ppxlcCurrY + iOffsetY;
    pSrc = ppxliFieldMB + 2*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // Y3
    pDst = ppxlcCurrY + iOffsetY + 8;
    pSrc = ppxliFieldMB + 3*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, 2*iOffsetY);

    // U
    pDst = ppxlcCurrU;
    pSrc = ppxliFieldMB + 4*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetUV);  

    // V
    pDst = ppxlcCurrV;
    pSrc = ppxliFieldMB + 5*64;    
    pWMVDec->m_pFieldAddError8x8 (pSrc, pDst, iOffsetUV);  
}

Void_WMV CopyFrameMBtoInterFieldMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, 
    I16_WMV *ppxliFrameMBV)
{   
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY, ppxliFieldMB, 16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY + 8, ppxliFieldMB + 64, 16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY + 8*16, ppxliFieldMB + 2 * 64, 16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY + 8*16 + 8, ppxliFieldMB + 3 * 64, 16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBU, ppxliFieldMB + 4 * 64, 8, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBV, ppxliFieldMB + 5 * 64, 8, 8);
}

Void_WMV CopyFieldMBtoInterFieldMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, 
    I16_WMV *ppxliFrameMBV)
{        
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY, ppxliFieldMB, 2*16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY + 8, ppxliFieldMB + 64, 2*16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY + 16, ppxliFieldMB + 2 * 64, 2*16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBY + 16 + 8, ppxliFieldMB + 3 * 64, 2*16, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBU, ppxliFieldMB + 4 * 64, 8, 8);
    CopyPixelIToPixelIBlock_8x8 (ppxliFrameMBV, ppxliFieldMB + 5 * 64, 8, 8);
}

// Used in field-mode interace coding
I32_WMV PredictFieldModeMV (tWMVDecInternalMember *pWMVDec,
    I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry)
{
    // find motion predictor
    I32_WMV iBlkX = *iPredX;
    I32_WMV iBlkY = *iPredY;
    I32_WMV iX1 = iBlkX * 8 * 4;
    I32_WMV iY1 = iBlkY * 8 * 4;
    I32_WMV iX, iY;

    I32_WMV iIndex  = iBlkY * pWMVDec->m_uintNumMBX * 2 + iBlkX;  // Index to block
    I32_WMV iMinX = -60;  // -15 pixels
    I32_WMV iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * 4 - 4;  // x8 for block x4 for qpel
    I32_WMV iMinY, iMaxY;
    I32_WMV iSameFieldX [3], iSameFieldY [3];
    I32_WMV iOppFieldX [3], iOppFieldY [3];
    I32_WMV iSameFieldCount = 0, iOppFieldCount = 0;
    Bool_WMV bValidLeft = FALSE, bValidTop = FALSE, bValidTopRight = FALSE;

    iSameFieldX [0] = iSameFieldY [0] = 0;

    if (!bTopBndry) {
        
        I32_WMV iTopX, iTopY;
        I32_WMV iLeftX, iLeftY;
        I32_WMV iTopRightX, iTopRightY;
        Bool_WMV bIntraTop, bIntraLeft, bIntraTopRight;

        // left block
        if (iBlkX) {
            iLeftX = pFieldMvX [iIndex - 1];
            iLeftY = pFieldMvY [iIndex - 1];
            bValidLeft = TRUE;
        } else {
			// special case - 1MB wide frame
			if (pWMVDec->m_uintNumMBX == 1) {
				iX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX];
				iY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX];
				goto End;
			}
            iLeftX = 0;
            iLeftY = 0;
        }

        bIntraLeft = (iLeftX == IBLOCKMV);

        if (bIntraLeft) {
            iLeftX = iLeftY = 0;  // set it to zero for median
        } 

        // top block
        iTopX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX];
        iTopY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX];
        bValidTop = TRUE;

        bIntraTop = (iTopX == IBLOCKMV);

        if (bIntraTop) {
            iTopX = iTopY = 0;
        }
        
        // top right block
        if (b1MV) {
            if (iBlkX != (I32_WMV) pWMVDec->m_uintNumMBX * 2 - 2) {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + 2];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + 2];
            } else {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX - 2];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX - 2];
            }
        }
        else {
              if (iBlkX) {
                    I32_WMV iOffset = 1 - ((((iBlkX ^ iBlkY) & 1) == 0) || (iBlkX >= ((I32_WMV)pWMVDec->m_uintNumMBX * 2 - 1))) * 2;
                    iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + iOffset];
                    iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + iOffset];
            }
            else {
                iTopRightX = pFieldMvX [iIndex - 2 * pWMVDec->m_uintNumMBX + 1];
                iTopRightY = pFieldMvY [iIndex - 2 * pWMVDec->m_uintNumMBX + 1];
            }
        }

        bValidTopRight = TRUE;

        bIntraTopRight = (iTopRightX == IBLOCKMV);
        if (bIntraTopRight) {
            iTopRightX = iTopRightY = 0;
        } 

        if (bValidLeft + bValidTop + bValidTopRight == 0)
            iX = IBLOCKMV;
        else {
            if (bValidLeft && !bIntraLeft) {
                if (iLeftY & 4) {
                    iOppFieldX [iOppFieldCount] = iLeftX;
                    iOppFieldY [iOppFieldCount++] = iLeftY;
                }
                else {
                    iSameFieldX [iSameFieldCount] = iLeftX;
                    iSameFieldY [iSameFieldCount++] = iLeftY;
                }
            }
            if (bValidTop && !bIntraTop) {
                if (iTopY & 4) {
                    iOppFieldX [iOppFieldCount] = iTopX;
                    iOppFieldY [iOppFieldCount++] = iTopY;
                }
                else {
                    iSameFieldX [iSameFieldCount] = iTopX;
                    iSameFieldY [iSameFieldCount++] = iTopY;
                }
            }
            if (bValidTopRight && !bIntraTopRight) {
                if (iTopRightY & 4) {
                    iOppFieldX [iOppFieldCount] = iTopRightX;
                    iOppFieldY [iOppFieldCount++] = iTopRightY;
                }
                else {
                    iSameFieldX [iSameFieldCount] = iTopRightX;
                    iSameFieldY [iSameFieldCount++] = iTopRightY;
                }
            }
            if (iSameFieldCount == 3 || iOppFieldCount == 3) {
                iX = medianof3 (iLeftX, iTopX, iTopRightX);
                iY = medianof3 (iLeftY, iTopY, iTopRightY);
            }
            else if (iSameFieldCount > iOppFieldCount) {
                iX = iSameFieldX [0];
                iY = iSameFieldY [0];
            }
            else if (iSameFieldCount < iOppFieldCount) {
                iX = iOppFieldX [0];
                iY = iOppFieldY [0];
            }
            else {
                // Use same field as predictor
                iX = iSameFieldX [0];
                iY = iSameFieldY [0];
            }
        }

End:;
    } else if (iBlkX) {
        // left predictor
        iY = pFieldMvY[iIndex - 1];
        iX = pFieldMvX[iIndex - 1];   
    } else {
        iX = iY = 0;
    }

    if (iX == IBLOCKMV) {
        iX = iY = 0;

        *iPredX = iX; *iPredY = iY;
        return 1;
    }

    if (iY & 4) {
        iMinY = -124;
        iMaxY = pWMVDec->m_uintNumMBY * 2 * 8 * 4 - 8;
    }
    else {
        iMinY = -120;
        iMaxY = pWMVDec->m_uintNumMBY * 2 * 8 * 4 - 4;
    }

    // limit motion vector
    iX1 += iX;
    iY1 += iY;

    if (iX1 < iMinX)
        iX += iMinX - iX1;
    else if (iX1 > iMaxX)
        iX += iMaxX - iX1;

    if (iY1 < iMinY)
        iY += iMinY - iY1;
    else if (iY1 > iMaxY)
        iY += iMaxY - iY1;

    *iPredX = iX; *iPredY = iY;
    return 0;
}

Void_WMV SetFieldModeResolution (tWMVDecInternalMember *pWMVDec,Bool_WMV bHalfVert)
{
    if (bHalfVert) {
        // Set resolution for full horiz, half vert.
        I32_WMV iHeightPrevY = pWMVDec->m_iHeightPrevY >> 1; // pWMVDec->m_iHeightPrevY and UV are not calculated correctly in ResetParams so 
		I32_WMV iHeightPrevUV = pWMVDec->m_iHeightPrevUV >> 1; // lets calculate the correct values here and store them after the call to ResetParams
#ifndef WMV9_SIMPLE_ONLY
        ResetParams(pWMVDec,2);
#endif
        ResetDecoderParams (pWMVDec);
        pWMVDec->m_iWidthPrevYxBlkMinusBlk = pWMVDec->m_iWidthPrevY * 16 - 8;
        pWMVDec->m_iMBSizeXWidthPrevY = pWMVDec->m_iWidthPrevY * 16 * 2;
        pWMVDec->m_iBlkSizeXWidthPrevUV = pWMVDec->m_iWidthPrevUV * 8 * 2;
        pWMVDec->m_iWidthPrevY = pWMVDec->m_iWidthPrevY * 2;
        pWMVDec->m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV * 2;
        pWMVDec->m_iHeightPrevY = iHeightPrevY;
        pWMVDec->m_iHeightPrevUV = iHeightPrevUV;
    }
    else {
        // Set resolution for full horiz, full vert.
#ifndef WMV9_SIMPLE_ONLY
        ResetParams(pWMVDec, 0);
#endif
        ResetDecoderParams (pWMVDec);
    }
}

I32_WMV isThereStartCode( CInputBitStream_WMV *pbitstrmIn )
{
	U8_WMV *pBitStream = pbitstrmIn->m_pCurr;
	if( pBitStream[-2] == 0x00 && pBitStream[-1] == 0x00 && pBitStream[0] == 0x01 ) {
		return 1;
	}

	return 0;
}
I32_WMV isThereFieldStartCode( CInputBitStream_WMV *pbitstrmIn )
{
	U8_WMV *pBitStream = pbitstrmIn->m_pCurr;

	int i=0;
	for(i=0;i<pbitstrmIn->m_pLast-pbitstrmIn->m_pCurr;i++)
	{
		if(pBitStream[0] ==0x00 && pBitStream[1] == 0x00 && pBitStream[2] == 0x01 && pBitStream[3] == 0x0c)
		{
			pbitstrmIn->m_pCurr += (i +4);
			return 0;
		}
		else
			pBitStream++;

	}
	//if( pBitStream[-2] == 0x00 && pBitStream[-1] == 0x00 && pBitStream[0] == 0x01 ) {
		//return 1;
	//}

	return 0;
}


I32_WMV DecodeFieldModeFrame (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV result;  
    pWMVDec->m_bFirstEscCodeInFrame = TRUE;

    SetFieldModeResolution (pWMVDec, TRUE);

    pWMVDec->m_iCurrentTemporalField = 0;
    if (pWMVDec->m_t == 1) {
        if (pWMVDec->m_bTopFieldFirst)
            pWMVDec->m_iPrevSpatialField = 1;
        else
            pWMVDec->m_iPrevSpatialField = 0;
    }
    else
        pWMVDec->m_iPrevSpatialField = pWMVDec->m_iCurrentField ^ 1;
    if (pWMVDec->m_bTopFieldFirst)
        pWMVDec->m_iCurrentField = 0;
    else
        pWMVDec->m_iCurrentField = 1;
  
    // Decode top field
    pWMVDec->m_tFrmType = pWMVDec->m_FirstFieldType;

    if(pWMVDec->m_bCodecIsWVC1){
	if(pWMVDec->m_tFrmType == PVOP)
		pWMVDec->m_nPframes ++;

	if(pWMVDec->m_tFrmType == IVOP)
		pWMVDec->m_nIframes ++;

	if(pWMVDec->m_tFrmType == BVOP)
		pWMVDec->m_nBframes ++;
    }

    result = decodeFieldHeadFieldPicture (pWMVDec);
    if (pWMVDec->m_tFrmType == PVOP)
        SetupFieldPictureMVScaling (pWMVDec, pWMVDec->m_iRefFrameDistance);
    else if (pWMVDec->m_tFrmType == BVOP) {
        /*
        I32_WMV iBackwardRefDistance;
        double dBFrac = (double)pWMVDec->m_iBNumerator / (double)pWMVDec->m_iBDenominator;
        I32_WMV iForwardRefDistance = (I32_WMV)((pWMVDec->m_iRefFrameDistance + 1) * dBFrac + 0.5) - 1;
        if (iForwardRefDistance < 0)
            iForwardRefDistance = 0;
                */
        I32_WMV iForwardRefDistance = (pWMVDec->m_iBNumerator * pWMVDec->m_iRefFrameDistance * pWMVDec->m_iBFrameReciprocal) >> 8;
        I32_WMV iBackwardRefDistance = pWMVDec->m_iRefFrameDistance - iForwardRefDistance - 1;
        SetupForwardBFieldPictureMVScaling (pWMVDec, iForwardRefDistance);
        SetupBackwardBFieldPictureMVScaling (pWMVDec, iBackwardRefDistance);
    }


    if (pWMVDec->m_tFrmType == PVOP)
    {
        pWMVDec->m_iPrevIFrame = 0;
    }

    pWMVDec->m_bFirstEscCodeInFrame = TRUE;

    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        pWMVDec->m_fDecodedI = TRUE;
#ifdef _WMV9AP_ 
        if (pWMVDec->m_bBFrameOn) 
		{
            if (pWMVDec->m_iNumBFrames > 0) {
                if (pWMVDec->m_iBState == -3)
                {
                    assert (pWMVDec->m_tFrmType != BVOP); // otherwise we are screwed!
                    pWMVDec->m_iBState = 1;
                }
                else {
                    if (pWMVDec->m_iBState == 1) {
                        if (pWMVDec->m_tFrmType == BIVOP) {
                            pWMVDec->m_bSkipThisFrame = TRUE;
                            pWMVDec->m_iCurrentTemporalField = 1;
                            pWMVDec->m_iPrevSpatialField ^= 1;
                            SetFieldModeResolution (pWMVDec, FALSE);
                            return ICERR_OK;
                        }

                        pWMVDec->m_iBState = 0;
 #ifndef WMV9_SIMPLE_ONLY
                       swapCurrRef1Pointers (pWMVDec);
                        swapRef0Ref1Pointers(pWMVDec);
#endif
                        Repeatpad(pWMVDec);
                    }
#ifndef WMV9_SIMPLE_ONLY
                    else {
                        if (pWMVDec->m_bPrevAnchor) 
						{
                            swapCurrRef1Pointers (pWMVDec);
                            // repeat pad the bottom field of the reference frame

                        }
                        if ( pWMVDec->m_tFrmType != BIVOP)
							swapRef0Ref1Pointers(pWMVDec);                        
                    }
#endif
                    pWMVDec->m_iDroppedPFrame = 0;
                }
            }   // if (pWMVDec->m_iNumBFrames > 0)
            else
                WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);
		}
#endif
        pWMVDec->m_bMainProfileOn = (!pWMVDec->m_bDeblockOn && pWMVDec->m_bMBAligned); // set it again to be safe

#ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bMultiresEnabled)
            HandleResolutionChange (pWMVDec);
#endif

        result = WMVideoDecDecodeI (pWMVDec);
  
        pWMVDec->m_PredTypePrev = IVOP;
#ifdef _WMV9AP_
    if (pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_tFrmType != BIVOP) 
    {
        U32_WMV imbY, imbX;
    for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
		{
			I32_WMV iOffset = ((pWMVDec->m_iCurrentField == 0) ? 0 : (2 * pWMVDec->m_uintNumMBY * 2 * pWMVDec->m_uintNumMBX));
            I32_WMV iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2 + iOffset;
            I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX ;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
		}
     }
#endif
        if (ICERR_OK != result) {
            return result;
        }
    }
    else {
        WMVideoDecAssignMotionCompRoutines(pWMVDec);
        // If the previous frame is I-VOP, we have to use it as the reference
        // anyway even if it is bad.
#ifdef _WMV9AP_
        if ((!pWMVDec->m_bBFrameOn || pWMVDec->m_tFrmType == PVOP)){
            if (pWMVDec->m_iNumBFrames == 0) {
                WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);
                pWMVDec->m_PredTypePrev = PVOP;
            }
            else {
                if (pWMVDec->m_iBState == 1) {
                    pWMVDec->m_iBState=0;
#ifndef WMV9_SIMPLE_ONLY
                    swapCurrRef1Pointers (pWMVDec);
                    swapRef0Ref1Pointers(pWMVDec);
#endif
                }
#ifndef WMV9_SIMPLE_ONLY
                else
                {
                    if (pWMVDec->m_bPrevAnchor)
                    {
                        swapCurrRef1Pointers (pWMVDec);
                    }
                    swapRef0Ref1Pointers(pWMVDec);
                }
#endif
            }   // if not pWMVDec->m_iNumBFrames == 0

            pWMVDec->m_iDroppedPFrame = 0;
            pWMVDec->m_PredTypePrev = PVOP;
		}
        else if (pWMVDec->m_tFrmType == BVOP) {
            // B frame  
            if (pWMVDec->m_iBState == 1) {
                pWMVDec->m_bSkipThisFrame = TRUE;
                pWMVDec->m_iCurrentTemporalField = 1;
                pWMVDec->m_iPrevSpatialField ^= 1;
                SetFieldModeResolution (pWMVDec, FALSE);
                return ICERR_OK;
            }

#ifndef WMV9_SIMPLE_ONLY
            if (pWMVDec->m_bPrevAnchor) {
                swapCurrRef1Pointers (pWMVDec);
            }
#endif
            
            pWMVDec->m_PredTypePrev = PVOP;
		}
#else
        WMVideoDecSwapCurrAndRef (pWMVDec, !pWMVDec->m_iNumBFrames);
#endif	// _WMV9AP_


        pWMVDec->m_PredTypePrev = PVOP;
#ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bLuminanceWarpTop)
            IntensityCompensationFieldMode(pWMVDec, 0, pWMVDec->m_iLuminanceScaleTop, pWMVDec->m_iLuminanceShiftTop);
        if (pWMVDec->m_bLuminanceWarpBottom)
            IntensityCompensationFieldMode(pWMVDec, 1, pWMVDec->m_iLuminanceScaleBottom, pWMVDec->m_iLuminanceShiftBottom);
#endif
#ifdef _WMV9AP_
		if (pWMVDec->m_tFrmType == BVOP)
			result =  decodeBFieldMode (pWMVDec);
		else
#endif
			result =  decodePFieldMode (pWMVDec);
        if (result != ICERR_OK) {
            ;
            return result;
        }
    }

    if(pWMVDec->m_bCodecIsWVC1){
	pWMVDec->m_pfrmCurrQ->m_frameType =  pWMVDec->m_FirstFieldType;//zou 323
    }
    /** repeatpad **/
    //if (pWMVDec->m_tFrmType == PVOP || pWMVDec->m_tFrmType == IVOP)
        Repeatpad (pWMVDec);

    // Decode bottom field
    pWMVDec->m_iCurrentTemporalField = 1;
    pWMVDec->m_iCurrentField ^= 1;
    pWMVDec->m_iPrevSpatialField ^= 1;

    if(!pWMVDec->m_bCodecIsWVC1){
     pWMVDec->m_tFrmType = pWMVDec->m_SecondFieldType;
    }
    // Second field picture header is aligned to byte boundary
    BS_flush(pWMVDec->m_pbitstrmIn); 

#if 1
    if(pWMVDec->m_bCodecIsWVC1){
	isThereFieldStartCode(pWMVDec->m_pbitstrmIn); //zou 323
	pWMVDec->m_pbitstrmIn->m_uBitMask = 0;
	pWMVDec->m_pbitstrmIn->m_iBitsLeft =0;
	BS_flush16 (pWMVDec->m_pbitstrmIn, 16);
    }
#else
	if (pWMVDec->m_bCodecIsWVC1){
		if( isThereStartCode(pWMVDec->m_pbitstrmIn) ) { //Sometime there is a start code before the second filed,sometimes not.
			//ignore start code prefix
			I32_WMV iPrefix;
			iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
			assert(iPrefix == 0);
			iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
			assert(iPrefix == 0);
			iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
			assert(iPrefix == 1);
			iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
			assert(iPrefix == SC_FIELD);
			pWMVDec->m_iParseStartCodeLastType = iPrefix;
		}
	}
#endif

    result = decodeFieldHeadFieldPicture (pWMVDec);
    if (result != ICERR_OK) {
        return result;
    }


    if (pWMVDec->m_tFrmType == PVOP)
        SetupFieldPictureMVScaling (pWMVDec, pWMVDec->m_iRefFrameDistance);
    else if (pWMVDec->m_tFrmType == BVOP) {
        I32_WMV iBackwardRefDistance;
        I32_WMV iForwardRefDistance = (pWMVDec->m_iBNumerator * pWMVDec->m_iRefFrameDistance * pWMVDec->m_iBFrameReciprocal) >> 8;

        iBackwardRefDistance = pWMVDec->m_iRefFrameDistance - iForwardRefDistance - 1;
        SetupForwardBFieldPictureMVScaling (pWMVDec, iForwardRefDistance);
        SetupBackwardBFieldPictureMVScaling (pWMVDec, iBackwardRefDistance);
    }

    if (pWMVDec->m_tFrmType == PVOP)
    {
      //  pWMVDec->m_iRndCtrl ^= 0x01;
        pWMVDec->m_iPrevIFrame = 0;
    }
    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
    
    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        pWMVDec->m_bMainProfileOn = (!pWMVDec->m_bDeblockOn && pWMVDec->m_bMBAligned); // set it again to be safe

 #ifndef WMV9_SIMPLE_ONLY
       if (pWMVDec->m_bMultiresEnabled)
            HandleResolutionChange (pWMVDec);
#endif

        result = WMVideoDecDecodeI (pWMVDec);
    
        pWMVDec->m_PredTypePrev = IVOP;
#ifdef _WMV9AP_
            if (pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_tFrmType != BIVOP) 
        {U32_WMV imbY, imbX;
    for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
		{
			I32_WMV iOffset = ((pWMVDec->m_iCurrentField == 0) ? 0 : (2 * pWMVDec->m_uintNumMBY * 2 * pWMVDec->m_uintNumMBX));
            I32_WMV iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2 + iOffset;
            I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX ;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = IBLOCKMV;
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
		}
        }
#endif
        if (ICERR_OK != result) {
            ;
            return result;
        }
    }
    else {
        WMVideoDecAssignMotionCompRoutines(pWMVDec);

        pWMVDec->m_PredTypePrev = PVOP;
#ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bLuminanceWarpTop)
            IntensityCompensationFieldMode(pWMVDec, 0, pWMVDec->m_iLuminanceScaleTop, pWMVDec->m_iLuminanceShiftTop);
        if (pWMVDec->m_bLuminanceWarpBottom)
            IntensityCompensationFieldMode(pWMVDec, 1, pWMVDec->m_iLuminanceScaleBottom, pWMVDec->m_iLuminanceShiftBottom);
#endif
#ifdef _WMV9AP_
		if (pWMVDec->m_tFrmType == BVOP)
			result =  decodeBFieldMode (pWMVDec);
		else
#endif
			result =  decodePFieldMode (pWMVDec);
        if (result != ICERR_OK) {
            ;
            return result;
        }
    }

    /** repeatpad **/
    //if (pWMVDec->m_tFrmType == PVOP || pWMVDec->m_tFrmType == IVOP)
        Repeatpad (pWMVDec);
    pWMVDec->m_iCurrentField ^= 1;  // why is this here?


    SetFieldModeResolution (pWMVDec, FALSE);

    // auto mode
    if (pWMVDec->m_iNumBFrames > 0 && pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP)
    {
        U32_WMV imbY, imbX;
        for ( imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
	        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
		    {
			    I32_WMV iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
			    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.x = pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex];
			    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.y = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex];
                pWMVDec->m_rgmbmd1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_dctMd = INTER;
                if (pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] == IBLOCKMV)
                {
                    pWMVDec->m_rgmbmd1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_dctMd = INTRA;
			        pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.x = 0;
			        pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.y = 0;
                } 
		    }
    }


    return ICERR_OK;
}

Void_WMV AverageFieldMv (tWMVDecInternalMember *pWMVDec, 
    I32_WMV* iAvgMvX, I32_WMV* iAvgMvY, I16_WMV *pFieldMvX, I16_WMV *pFieldMvY, I32_WMV iCurrIndex,
    I32_WMV iColumn)
{
    I32_WMV iMvX0, iMvX1, iMvY0, iMvY1;

    iCurrIndex += iColumn;
    iMvX0 = pFieldMvX [iCurrIndex];
    iMvY0 = pFieldMvY [iCurrIndex];
    iCurrIndex += 2 * pWMVDec->m_uintNumMBX;
    iMvX1 = pFieldMvX [iCurrIndex];
    iMvY1 = pFieldMvY [iCurrIndex];
    *iAvgMvX = (iMvX0 + iMvX1 + 1) >> 1;
    *iAvgMvY = (iMvY0 + iMvY1 + 1) >> 1;
}

Void_WMV ComputeFieldMvPredictorFromNeighborMv (
    I32_WMV *piValidPredMvX, I32_WMV *piValidPredMvY, I32_WMV iTotalValidMv, I32_WMV *piMvX, I32_WMV *piMvY)
{
    I32_WMV i, iMvX, iMvY;
    I32_WMV iSameFieldX [3], iSameFieldY [3];
    I32_WMV iOppFieldX [3], iOppFieldY [3];
    I32_WMV iSameFieldCount = 0, iOppFieldCount = 0;
    memset (iSameFieldX, 0, 3*sizeof(I32_WMV));
    memset (iSameFieldY, 0, 3*sizeof(I32_WMV));
    memset (iOppFieldX, 0, 3*sizeof(I32_WMV));
    memset (iOppFieldY, 0, 3*sizeof(I32_WMV));

    // compute predictor
    iSameFieldCount = iOppFieldCount = 0;
    for (i = 0; i < iTotalValidMv; i++) {
        if (piValidPredMvY [i] & 4) {
            iOppFieldX [iOppFieldCount] = piValidPredMvX [i];
            iOppFieldY [iOppFieldCount++] = piValidPredMvY [i];
        } else {
            iSameFieldX [iSameFieldCount] = piValidPredMvX [i];
            iSameFieldY [iSameFieldCount++] = piValidPredMvY [i];
        }
    }

    if (iTotalValidMv == 3) {
        if (iSameFieldCount == 3 || iOppFieldCount == 3) {
            iMvX = medianof3 (piValidPredMvX [0], piValidPredMvX [1], piValidPredMvX [2]);
            iMvY = medianof3 (piValidPredMvY [0], piValidPredMvY [1], piValidPredMvY [2]);
        } else if (iSameFieldCount >= iOppFieldCount) {
            iMvX = iSameFieldX [0]; 
            iMvY = iSameFieldY [0];
        } else {
            iMvX = iOppFieldX [0];
            iMvY = iOppFieldY [0];
        }
    } else if (iTotalValidMv == 2) {
        if (iSameFieldCount >= iOppFieldCount) {
            iMvX = iSameFieldX [0];
            iMvY = iSameFieldY [0];
        } else {
            iMvX = iOppFieldX [0];
            iMvY = iOppFieldY [0];
        }
    } else if (iTotalValidMv == 1) {
        iMvX = piValidPredMvX [0];
        iMvY = piValidPredMvY [0];
    } else {
        iMvX = iMvY = 0;
    }

    *piMvX = iMvX;
    *piMvY = iMvY;
}

Void_WMV ComputeFrameMvPredictorFromNeighborMv (
    I32_WMV *piValidPredMvX, I32_WMV *piValidPredMvY, I32_WMV iTotalValidMv, I32_WMV *piMvX, I32_WMV *piMvY)
{
    I32_WMV iMvX, iMvY;
    if (iTotalValidMv >= 2) {
        iMvX = medianof3 (piValidPredMvX [0], piValidPredMvX [1], piValidPredMvX [2]);
        iMvY = medianof3 (piValidPredMvY [0], piValidPredMvY [1], piValidPredMvY [2]);
    } else if (iTotalValidMv == 1) {
        iMvX = piValidPredMvX [0];
        iMvY = piValidPredMvY [0];
    } else {
        iMvX = iMvY = 0;
    }
    *piMvX = iMvX;
    *piMvY = iMvY;
}

Void_WMV PredictFrameOneMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];
    I32_WMV iTotalValidMv;
    I32_WMV iMvX, iMvY;

    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    
    iTotalValidMv = iMvX = iMvY = 0;    

    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));
    
    // left
    if (imbX) { 
        pmbmdCurr = pmbmd - 1;
        iCurrIndex = iIndex - 2;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {               
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 1);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top
    if (!bTopBndry) {
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0; // for top right blocks
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1; // for top left blocks
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, i4MVOffset);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFrameMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);    
}

Void_WMV PredictFieldTopMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];    
    I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
     
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));

    // left
    if (imbX) { 
        pmbmdCurr = pmbmd - 1;
        iCurrIndex = iIndex - 2;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {               
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 1];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top
    if (!bTopBndry) {
        
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0; // for top right blocks
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {                        
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + i4MVOffset];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFieldMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);
}

Void_WMV PredictFieldBotMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];
       I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2; 
     
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));

    // left
    if (imbX) { 
        pmbmdCurr = pmbmd - 1;
        iCurrIndex = iIndex - 2;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {               
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top
    if (!bTopBndry) {
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0;
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {                        
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFieldMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);

}

Void_WMV PredictFrameTopLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];    
        I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));
    
    // left
    if (imbX) { 
        pmbmdCurr = pmbmd - 1;
        iCurrIndex = iIndex - 2;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {               
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 1);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top
    if (!bTopBndry) {
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0;
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {                        
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, i4MVOffset);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFrameMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);
}

Void_WMV PredictFrameTopRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];    
   
        I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));
    
    // left    
    iValidPredMvX [iTotalValidMv] = pFieldMvX [iIndex];
    iValidPredMvY [iTotalValidMv++] = pFieldMvY [iIndex];

    // top
    if (!bTopBndry) {
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 1);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0;
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {                      
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, i4MVOffset);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFrameMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);
}

Void_WMV PredictFrameBotLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];    

    I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
       
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));
    
    // left
    if (imbX) { 
        pmbmdCurr = pmbmd - 1;
        iCurrIndex = iIndex - 2;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {               
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 0);
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                AverageFieldMv (pWMVDec, &iMvX, &iMvY, pFieldMvX, pFieldMvY, iCurrIndex, 1);
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top    
    iValidPredMvX [iTotalValidMv] = pFieldMvX [iIndex];
    iValidPredMvY [iTotalValidMv++] = pFieldMvY [iIndex];

    // top right    
    iValidPredMvX [iTotalValidMv] = pFieldMvX [iIndex + 1];
    iValidPredMvY [iTotalValidMv++] = pFieldMvY [iIndex + 1];

    // compute predictor
    ComputeFrameMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);

}

Void_WMV PredictFrameBotRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    I32_WMV iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;

    *iPredX = medianof3 (pFieldMvX [iIndex], pFieldMvX [iIndex + 1],
        pFieldMvX [iIndex + 2 * pWMVDec->m_uintNumMBX]);
    *iPredY = medianof3 (pFieldMvY [iIndex], pFieldMvY [iIndex + 1], 
        pFieldMvY [iIndex + 2 * pWMVDec->m_uintNumMBX]);

}

Void_WMV PredictFieldTopLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];
    
    I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
      
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));

    // left
    if (imbX) { 
        pmbmdCurr = pmbmd - 1;
        iCurrIndex = iIndex - 2;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {               
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 1];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top
    if (!bTopBndry) {
       
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0;
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + i4MVOffset];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFieldMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);

}

Void_WMV PredictFieldTopRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];
        I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
    
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));

    // left
    iValidPredMvX [iTotalValidMv] = pFieldMvX [iIndex]; 
    iValidPredMvY [iTotalValidMv++]  = pFieldMvY [iIndex];

    // top
    if (!bTopBndry) {
        
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 1];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0;
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + i4MVOffset];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFieldMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);

}


Void_WMV PredictFieldBotLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];
    
    I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2; 
    
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));

    // left
    if (imbX) { 
        pmbmdCurr = pmbmd - 1;
        iCurrIndex = iIndex - 2;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {               
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top
    if (!bTopBndry) {
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0;
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFieldMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);

}


Void_WMV PredictFieldBotRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, 
    I16_WMV* pFieldMvY, Bool_WMV bTopBndry)
{
    CWMVMBMode* pmbmdCurr;
    I32_WMV iIndex, iCurrIndex;
    I32_WMV iValidPredMvX [3], iValidPredMvY [3];
    
    I32_WMV iTotalValidMv = 0, iMvX = 0, iMvY = 0;


    // init predictors
    iIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2; 
    
    memset (iValidPredMvX, 0, 3*sizeof(I32_WMV));
    memset (iValidPredMvY, 0, 3*sizeof(I32_WMV));

    // left
    iValidPredMvX [iTotalValidMv] = pFieldMvX [iIndex + 2 * pWMVDec->m_uintNumMBX];
    iValidPredMvY [iTotalValidMv++] = pFieldMvY [iIndex + 2 * pWMVDec->m_uintNumMBX];

    // top
    if (!bTopBndry) {
        pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX;
        iCurrIndex = iIndex -  4 * pWMVDec->m_uintNumMBX;
        if (!(pmbmdCurr->m_dctMd == INTRA)) {
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + 1];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // top right
    if (!bTopBndry && (pWMVDec->m_uintNumMBX != 1)) {
        I32_WMV i4MVOffset = 0;
        if (imbX != (I32_WMV) pWMVDec->m_uintNumMBX - 1) {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX + 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX + 1;
        } else {
            iCurrIndex = iIndex - 4 * pWMVDec->m_uintNumMBX - 2;
            pmbmdCurr = pmbmd - pWMVDec->m_uintNumMBX - 1;
            i4MVOffset = 1;
        }
        if (!(pmbmdCurr->m_dctMd == INTRA)) {            
            if (pmbmdCurr->m_chMBMode == MB_1MV) {        
                iMvX = pFieldMvX [iCurrIndex]; 
                iMvY = pFieldMvY [iCurrIndex];
            } else if (pmbmdCurr->m_chMBMode == MB_4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX];
            } else if (pmbmdCurr->m_chMBMode == MB_FIELD4MV) {
                iMvX = pFieldMvX [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset]; 
                iMvY = pFieldMvY [iCurrIndex + 2 * pWMVDec->m_uintNumMBX + i4MVOffset];
            }
            iValidPredMvX [iTotalValidMv] = iMvX;
            iValidPredMvY [iTotalValidMv++] = iMvY;
        }
    }

    // compute predictor
    ComputeFieldMvPredictorFromNeighborMv (
        iValidPredMvX, iValidPredMvY, iTotalValidMv, iPredX, iPredY);
}

/***********************************************************************
  PullBackFieldModeMotionVector : Pull motion vector into repeatpad range
***********************************************************************/
Bool_WMV PullBackFieldModeMotionVector (tWMVDecInternalMember *pWMVDec, 
    I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iX = *iMvX;
    I32_WMV iY = *iMvY;
    I32_WMV iXCoord = imbX * 16;
    I32_WMV iYCoord = imbY * 32;
    I32_WMV iMinY, iMaxY;
    Bool_WMV bPullBack = FALSE;
    I32_WMV iPosX ;
    I32_WMV iPosY ;


    if (pWMVDec->m_iCurrentField == 0) { 
        // We are coding top field
        if (iY & 4) {
            // MV points to bottom field
            iMinY = -33;
            iMaxY = pWMVDec->m_uintNumMBY * 32 + 1;
        }
        else {
            // MV points to top field
            iMinY = -34;
            iMaxY = pWMVDec->m_uintNumMBY * 32;
        }
    }
    else { 
        // We are coding bottom field
        if (iY & 4) {
            // MV points to top field
            iMinY = -35;
            iMaxY = pWMVDec->m_uintNumMBY * 32 - 1;
        }
        else {
            // MV points to bottom field
            iMinY = -34;
            iMaxY = pWMVDec->m_uintNumMBY * 32;
        }
    }

    // this is required for correct rounding in repeatpad area
    iPosX = iXCoord + (iX >> 2);
    iPosY = iYCoord + (iY >> 2);
    
    if (iPosX < -17) {
        iPosX = -17; 
        bPullBack = TRUE;
    } else if (iPosX > (I32_WMV) pWMVDec->m_uintNumMBX * 16) {
        iPosX = pWMVDec->m_uintNumMBX * 16;
        bPullBack = TRUE;
    }

    if (iPosY < iMinY) {
        iPosY = iMinY;
        bPullBack = TRUE;
    } else if (iPosY > iMaxY) {
        iPosY = iMaxY;
        bPullBack = TRUE;
    }

    if (bPullBack) {
        iX = ((iPosX - iXCoord) << 2) + (iX & 3);
        iY = ((iPosY - iYCoord) << 2) + (iY & 3);
        *iMvX = iX;
        *iMvY = iY;
    }

    return bPullBack;
}

#endif //PPCWMP
