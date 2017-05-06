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

    pWMVDec->m_bMBAligned = ( pWMVDec->m_iFrmWidthSrc == iWidthMBAligned && 
                                            pWMVDec->m_iFrmHeightSrc == iHeightMBAligned);
    // for block-based rendering
    //pWMVDec->m_iWidthPrevYTimes8Minus8 = pWMVDec->m_iWidthPrevY * 8 - 8;
    //pWMVDec->m_iWidthPrevUVTimes4Minus4 = pWMVDec->m_iWidthPrevUV * 4 - 4;
    pWMVDec->m_iWidthPrevYxBlkMinusBlk = (pWMVDec->m_iWidthPrevY * BLOCK_SIZE) - BLOCK_SIZE;
    //ReInitContext(pWMVDec->m_pContext, pWMVDec->m_uintNumMBX*2, 2);
    pWMVDec->m_uiMBEnd0 = pWMVDec->m_uintNumMBY;
}
