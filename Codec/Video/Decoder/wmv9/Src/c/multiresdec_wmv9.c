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

#include "spatialpredictor_wmv.h"

Void_WMV ResetDecoderParams (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iWidthMBAligned = (pWMVDec->m_iFrmWidthSrc + 15) & ~15;
    I32_WMV iHeightMBAligned = (pWMVDec->m_iFrmHeightSrc + 15) & ~15;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ResetDecoderParams);

    pWMVDec->m_bMBAligned = (
        pWMVDec->m_iFrmWidthSrc == iWidthMBAligned && 
        pWMVDec->m_iFrmHeightSrc == iHeightMBAligned);


    // for block-based rendering
    pWMVDec->m_iWidthPrevYTimes8Minus8 = pWMVDec->m_iWidthPrevY * 8 - 8;
    pWMVDec->m_iWidthPrevUVTimes4Minus4 = pWMVDec->m_iWidthPrevUV * 4 - 4;

//    pWMVDec->m_uintNumMBYSlice = pWMVDec->m_uintNumMBY / pWMVDec->m_iSliceCode;

    pWMVDec->m_iWidthPrevYxBlkMinusBlk = (pWMVDec->m_iWidthPrevY * BLOCK_SIZE) - BLOCK_SIZE;

    ReInitContext(pWMVDec->m_pContext, pWMVDec->m_uintNumMBX*2, 2);

        // Multithread params
    pWMVDec->m_iEndY0 = pWMVDec->m_iHeightY / pWMVDec->m_uiNumProcessors;
    pWMVDec->m_iEndUV0 = pWMVDec->m_iHeightUV / pWMVDec->m_uiNumProcessors;
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

}


Void_WMV ReInitContext(t_CContextWMV        *pContext, I32_WMV iCol, I32_WMV iRow)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ReInitContext);
    pContext->m_iRow = iRow;
    pContext->m_iCol = iCol;
    pContext->m_iColGt2 = (pContext->m_iCol > 2) ? 2 : 1;
}
