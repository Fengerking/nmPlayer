//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#include "xplatform_wmv.h"
#include "limits.h"
#include "voWmvPort.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "postfilter_wmv.h"
#include <math.h>
#include "tables_wmv.h"
#if !defined(__arm) && !defined(_ARM_) && !defined(_MIPS_)
//#   include "cpudetect.h"
#endif
#ifdef macintosh
#   include "cpudetect_mac.h"
    extern Bool_WMV g_bSupportAltiVec_WMV;
#endif
#if defined(_SUPPORT_POST_FILTERS_) && defined(_WMV_TARGET_X86_)
#   include <tchar.h>
#endif



Void_WMV CopyCurrToPost (tWMVDecInternalMember *pWMVDec)
{    
    I32_WMV i;
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliPostQU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliPostQV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(CopyCurrToPost);

    if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bOutputOpEnabled == FALSE_WMV)
    {
        RF_CpyCtl * pCopyCtl = (RF_CpyCtl *)&pWMVDec->m_pfrmPostQ->m_pOps[pWMVDec->m_pfrmPostQ->m_iNumOps++];

        assert(pWMVDec->m_pfrmPostQ->m_iNumOps == 1);

        pCopyCtl->m_iOpCode  = FRMOP_COPY_CURR2POST;

        pCopyCtl->m_pfrmSrc = pWMVDec->m_pfrmCurrQ;
        pCopyCtl->m_iWidthPrevY = pWMVDec->m_iWidthPrevY ;
        pCopyCtl->m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
        pCopyCtl->m_uintNumMBY = pWMVDec->m_uintNumMBY;
        pCopyCtl->m_iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevYXExpPlusExp;
        pCopyCtl->m_iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUVXExpPlusExp;

        return;
    }

    for ( i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostQU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(U8_WMV));
        ppxliPostQV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        ALIGNED32_FASTMEMCPY(ppxliPostQY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(U8_WMV));
        ppxliPostQY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }
}

#ifdef _MultiThread_Decode_

Void_WMV DeblockIFrame (tWMVDecInternalMember *pWMVDec,
    U8_WMV * ppxliY,
    U8_WMV * ppxliU,
    U8_WMV * ppxliV,
    U32_WMV iMBStartY, U32_WMV iMBEndY
    )
{ 
    
    U8_WMV * ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV * ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV * ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U32_WMV imbY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockIFrame);

    assert (pWMVDec->m_bDeblockOn);

    for ( imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV * ppxliCodedPostY = ppxliPostY;
        U8_WMV * ppxliCodedPostU = ppxliPostU;
        U8_WMV * ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
#if 0
                g_pDeblockMB(pWMVDec,
                    ppxliCodedPostY, 
                    ppxliCodedPostU, 
                    ppxliCodedPostV, 
                    imbX,                       // left
                    FALSE_WMV,                  // right half-edge
                    imbY,                       // top
                    FALSE_WMV,                  // bottom half-edge
                    TRUE_WMV,                   // middle
                    pWMVDec->m_iStepSize,
                    pWMVDec->m_iWidthPrevY,
                    pWMVDec->m_iWidthPrevUV);
#endif
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }

    if (pWMVDec->m_bDeringOn == FALSE_WMV)
        return;

    ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV * ppxliCodedPostY = ppxliPostY;
        U8_WMV * ppxliCodedPostU = ppxliPostU;
        U8_WMV * ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            
#if 0
                if (!((imbX == 0)            ||
                     (imbX == pWMVDec->m_uintNumMBX-1)||
                     (imbY == 0)             ||
                     (imbY == pWMVDec->m_uintNumMBY-1))) 
                     (*g_pDeringMB)(ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,pWMVDec->m_iStepSize,pWMVDec->m_iWidthPrevY,pWMVDec->m_iWidthPrevUV);
#endif
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }
}



Void_WMV DeblockPFrame (tWMVDecInternalMember *pWMVDec,
    U8_WMV * ppxliY,
    U8_WMV * ppxliU,
    U8_WMV * ppxliV,
    U32_WMV iMBStartY, U32_WMV iMBEndY
    )
{   
    
    U8_WMV * ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV * ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV * ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    const CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd + iMBStartY * pWMVDec->m_uintNumMBX;
    U32_WMV imbY;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockPFrame);
    assert (pWMVDec->m_bDeblockOn);

    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV * ppxliCodedPostY = ppxliPostY;
        U8_WMV * ppxliCodedPostU = ppxliPostU;
        U8_WMV * ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;

        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, pmbmd++) {
            if (pmbmd->m_bSkip == FALSE_WMV) {
                Bool_WMV bdoBottomHalfEdge = FALSE_WMV, bdoRightHalfEdge = FALSE_WMV;
                if (imbX != (pWMVDec->m_uintNumMBX - 1)) {
                    bdoRightHalfEdge = (pmbmd + 1)->m_bSkip;
                }
                if (imbY != (pWMVDec->m_uintNumMBY - 1)) {
                    bdoBottomHalfEdge = (pmbmd + pWMVDec->m_uintNumMBX)->m_bSkip;
                }
#if 0
                g_pDeblockMB(pWMVDec,
                    ppxliCodedPostY, 
                    ppxliCodedPostU, 
                    ppxliCodedPostV, 
                    imbX,                           // left
                    bdoRightHalfEdge,               // right half-edge
                    imbY,                           // top
                    bdoBottomHalfEdge,              // bottom half-edge
                    TRUE_WMV,                       // middle
                    pWMVDec->m_iStepSize,
                    pWMVDec->m_iWidthPrevY,
                    pWMVDec->m_iWidthPrevUV);
#endif
            }

            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY; 
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }

    if (pWMVDec->m_bDeringOn == FALSE_WMV)
        return;

    ppxliPostY = ppxliY; //pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliPostU = ppxliU; //pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliPostV = ppxliV; //pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pmbmd = pWMVDec->m_rgmbmd + iMBStartY * pWMVDec->m_uintNumMBX;
    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV * ppxliCodedPostY = ppxliPostY;
        U8_WMV * ppxliCodedPostU = ppxliPostU;
        U8_WMV * ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;

        for ( imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, pmbmd++) {
            if (pmbmd->m_bSkip == FALSE_WMV) {
           
#if 0
                if (!((imbX == 0)            ||
                     (imbX == pWMVDec->m_uintNumMBX-1)||
                     (imbY == 0)             ||
                     (imbY == pWMVDec->m_uintNumMBY-1))) 
                     (*g_pDeringMB)(ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,pWMVDec->m_iStepSize,pWMVDec->m_iWidthPrevY,pWMVDec->m_iWidthPrevUV);
#endif
            }
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY; 
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }
}

#endif // _MultiThread_Decode_

Void_WMV DeblockFrame (
    tWMVDecInternalMember *pWMVDec,
    U32_WMV iMBStartY, U32_WMV iMBEndY
){ 
    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliPostY = pWMVDec->m_ppxliPostQY  + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliPostU = pWMVDec->m_ppxliPostQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliPostV = pWMVDec->m_ppxliPostQV  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U32_WMV imbY;
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockFrame);

    if(pWMVDec->m_bEMB3FrmEnabled && !pWMVDec->m_bOutputOpEnabled)
    {
        RF_DeBlockCtl * pDeBlockCtl = (RF_DeBlockCtl *)&pWMVDec->m_pfrmPostQ->m_pOps[pWMVDec->m_pfrmPostQ->m_iNumOps++];

        assert(pWMVDec->m_pfrmPostQ->m_iNumOps == 1);
        pDeBlockCtl->m_iOpCode  = FRMOP_DEBLOCK;

        pDeBlockCtl->m_pfrmSrc = pWMVDec->m_pfrmCurrQ;
        pDeBlockCtl->m_iHeightUV = pWMVDec->m_iHeightUV;
        pDeBlockCtl->m_iWidthPrevY = pWMVDec->m_iWidthPrevY;
        pDeBlockCtl->m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
        pDeBlockCtl->m_iWidthPrevYXExpPlusExp = pWMVDec->m_iWidthPrevYXExpPlusExp;
        pDeBlockCtl->m_iWidthPrevUVXExpPlusExp = pWMVDec->m_iWidthPrevUVXExpPlusExp;
        pDeBlockCtl->m_uintNumMBX = pWMVDec->m_uintNumMBX;
        pDeBlockCtl->m_uintNumMBY = pWMVDec->m_uintNumMBY;
        pDeBlockCtl->m_iStepSize  = pWMVDec->m_iStepSize;
        pDeBlockCtl->m_iMBSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY;
        pDeBlockCtl->m_iBlkSizeXWidthPrevUV = pWMVDec->m_iBlkSizeXWidthPrevUV;
        pDeBlockCtl->m_bDeringOn = pWMVDec->m_bDeringOn;

        pDeBlockCtl->iMBStartY = iMBStartY;
        pDeBlockCtl->iMBEndY = iMBEndY;
        return;

    }

    //Should be pWMVDec->m_iWidthUV and pWMVDec->m_iWidthY
    for (i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(I8_WMV));
        ppxliPostU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(I8_WMV));
        ppxliPostV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(I8_WMV));
        ppxliPostY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
        ALIGNED32_FASTMEMCPY(ppxliPostY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(I8_WMV));
        ppxliPostY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }

    assert (pWMVDec->m_bDeblockOn);
    ppxliPostY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliPostU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliPostV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV* ppxliCodedPostY = ppxliPostY;
        U8_WMV* ppxliCodedPostU = ppxliPostU;
        U8_WMV* ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {            
                g_pDeblockMB(
                    pWMVDec,
                    ppxliCodedPostY, 
                    ppxliCodedPostU, 
                    ppxliCodedPostV, 
                    imbX,                       // left
                    FALSE,                      // right half-edge
                    imbY,                       // top
                    FALSE,                      // bottom half-edge
                    TRUE_WMV,                   // middle
                    pWMVDec->m_iStepSize,
                    pWMVDec->m_iWidthPrevY,
                    pWMVDec->m_iWidthPrevUV);

            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }

    if (pWMVDec->m_bDeringOn != FALSE) {

        ppxliPostY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
        ppxliPostU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
        ppxliPostV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

        for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
            U8_WMV* ppxliCodedPostY = ppxliPostY;
            U8_WMV* ppxliCodedPostU = ppxliPostU;
            U8_WMV* ppxliCodedPostV = ppxliPostV;
            U32_WMV imbX;
            for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {  
                    if (!((imbX == 0)            ||
                         (imbX == pWMVDec->m_uintNumMBX-1)||
                         (imbY == 0)             ||
                         (imbY == pWMVDec->m_uintNumMBY-1))) 
                         g_pDeringMB(ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,pWMVDec->m_iStepSize,pWMVDec->m_iWidthPrevY,pWMVDec->m_iWidthPrevUV);
      
                ppxliCodedPostY += MB_SIZE;
                ppxliCodedPostU += BLOCK_SIZE;
                ppxliCodedPostV += BLOCK_SIZE;
            }
            ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
            ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
            ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;
        }
    }

// Use this to create a visual block on the screen to indicate Post Filtering level.  
// there is a better version in wmvdec.c which only works for RGB565.
//#   define DEBUG_SHOW_POST_LEVEL_ON_SCREEN
#   ifdef DEBUG_SHOW_POST_LEVEL_ON_SCREEN
        {
            // we don't come here on deblock 0, so no change is made to the screen in that case.
            DWORD* pdwY = (DWORD*)(pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp  + MB_SIZE*(pWMVDec->m_uintNumMBX>>1));
            DWORD* pdwU = (DWORD*)(pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + BLOCK_SIZE*(pWMVDec->m_uintNumMBX>>1));
            DWORD* pdwV = (DWORD*)(pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + BLOCK_SIZE*(pWMVDec->m_uintNumMBX>>1));
            // 0 = Black (0,0,0), 1 = Red (255,0,0), 2 = Green (0,255,0), 3 = Blue (0,0,255), 4 = Yellow (255,255,0)
            U32_WMV pYColorCode[] = { 0, 0x4c4c4c4c, 0x96969696, 0x1d1d1d1d, 0xe2e2e2e2 };
            U32_WMV pUColorCode[] = { 0,          0, 0x6f6f6f6f,          0,          0 };
            U32_WMV pVColorCode[] = { 0, 0x9d9d9d9d,          0,          0, 0x1a1a1a1a };
            ///U32_WMV pYColorCode[] = { 0, 0x40404040, 0x80808080, 0xc0c0c0c0, 0xffffffff };
            ///U32_WMV pUColorCode[] = { 0,          0,          0,          0, 0 };
            ///U32_WMV pVColorCode[] = { 0,          0,          0,          0, 0 };
            I32_WMV iY;
            for( iY = 0; iY < 4; iY++ ) {
                *(pdwY)   = pYColorCode[pWMVDec->m_iPostProcessMode];
                *(pdwY+1) = pYColorCode[pWMVDec->m_iPostProcessMode];
                pdwY += pWMVDec->m_iWidthPrevY>>2;
                *(pdwY)   = pYColorCode[pWMVDec->m_iPostProcessMode];
                *(pdwY+1) = pYColorCode[pWMVDec->m_iPostProcessMode];
                pdwY += pWMVDec->m_iWidthPrevY>>2;
                *(pdwU)   = pUColorCode[pWMVDec->m_iPostProcessMode];
                pdwU += pWMVDec->m_iWidthPrevUV>>2;
                *(pdwV)   = pVColorCode[pWMVDec->m_iPostProcessMode];
                pdwV += pWMVDec->m_iWidthPrevUV>>2;
            }
        }
#       pragma message(__FILE__ "(358) : Warning - build with DEBUG_SHOW_POST_LEVEL_ON_SCREEN - do not checkin.")
#   endif // DEBUG_SHOW_POST_LEVEL_ON_SCREEN
}

Void_WMV DeblockFrameMBRow (tWMVDecInternalMember *pWMVDec, 
                            U8_WMV* ppxliCurrQY, 
                            U8_WMV* ppxliCurrQU, 
                            U8_WMV* ppxliCurrQV, 
                            U8_WMV* ppxliPostY, 
                            U8_WMV* ppxliPostU, 
                            U8_WMV* ppxliPostV, 
                            I32_WMV imbY,
                            I32_WMV m_iBlkSizeXWidthPrevUV,
                            I32_WMV m_iMBSizeXWidthPrevY,
                            I32_WMV m_uintNumMBX,
                            I32_WMV m_uintNumMBY,
                            I32_WMV m_iStepSize,
                            I32_WMV m_iWidthPrevY,
                            I32_WMV m_iWidthPrevUV,
                            I32_WMV m_bDeringOn
                            )
{ 
    
    U8_WMV* ppxliCodedPostY = ppxliPostY;
    U8_WMV* ppxliCodedPostU = ppxliPostU;
    U8_WMV* ppxliCodedPostV = ppxliPostV;
    I32_WMV imbX;
    
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockFrameMBRow);
    
   
    
    ALIGNED32_FASTMEMCPY(ppxliPostU, ppxliCurrQU, m_iBlkSizeXWidthPrevUV * sizeof(I8_WMV));
    ALIGNED32_FASTMEMCPY(ppxliPostV, ppxliCurrQV, m_iBlkSizeXWidthPrevUV * sizeof(I8_WMV));
    ALIGNED32_FASTMEMCPY(ppxliPostY, ppxliCurrQY, m_iMBSizeXWidthPrevY * sizeof(I8_WMV));
    for (imbX = 0; imbX < m_uintNumMBX; imbX++) {            
        g_pDeblockMB(
            pWMVDec,
            ppxliCodedPostY, 
            ppxliCodedPostU, 
            ppxliCodedPostV, 
            imbX,                       // left
            FALSE,                      // right half-edge
            imbY,                       // top
            FALSE,                      // bottom half-edge
            TRUE_WMV,                   // middle
            m_iStepSize,
            m_iWidthPrevY,
            m_iWidthPrevUV);
        
        ppxliCodedPostY += MB_SIZE;
        ppxliCodedPostU += BLOCK_SIZE;
        ppxliCodedPostV += BLOCK_SIZE;
    }
    
    
    if (m_bDeringOn == FALSE)
        return;
    
    
    for (imbX = 0; imbX < m_uintNumMBX; imbX++) {  
        if (!((imbX == 0)            ||
            (imbX == m_uintNumMBX-1)||
            (imbY == 0)             ||
            (imbY == m_uintNumMBY-1))) 
            g_pDeringMB(ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,m_iStepSize,m_iWidthPrevY,m_iWidthPrevUV);
        
        ppxliCodedPostY += MB_SIZE;
        ppxliCodedPostU += BLOCK_SIZE;
        ppxliCodedPostV += BLOCK_SIZE;
    }

}


Void_WMV SetupPostFilterMode (tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetupPostFilterMode);
    if (pWMVDec->m_iPostProcessMode < 0 || pWMVDec->m_iPostProcessMode > _TOP_POST_FILTER_LEVEL_) 
        return;

    if (pWMVDec->m_iPostProcessMode == 4 && _TOP_POST_FILTER_LEVEL_>=4) {
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = TRUE_WMV;
        g_InitPostFilter (FALSE, pWMVDec->m_bYUV411);
    }
    else if (pWMVDec->m_iPostProcessMode == 3 && _TOP_POST_FILTER_LEVEL_>=3) {
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = TRUE_WMV;
        g_InitPostFilter (TRUE_WMV, pWMVDec->m_bYUV411);
    }
    else if (pWMVDec->m_iPostProcessMode == 2 && _TOP_POST_FILTER_LEVEL_>=2) {
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = FALSE_WMV;
        g_InitPostFilter (FALSE, pWMVDec->m_bYUV411);
    }
    else if (pWMVDec->m_iPostProcessMode == 1 && _TOP_POST_FILTER_LEVEL_>=1) {
        pWMVDec->m_bDeblockOn = TRUE_WMV;
        pWMVDec->m_bDeringOn = FALSE_WMV;
        g_InitPostFilter (TRUE_WMV, pWMVDec->m_bYUV411);
    }
    else {
        pWMVDec->m_bDeblockOn = FALSE_WMV;
        pWMVDec->m_bDeringOn = FALSE_WMV;
    }
}


#ifndef WMV9_SIMPLE_ONLY

Void_WMV DeblockInterlaceFrame (
    tWMVDecInternalMember *pWMVDec,
    U32_WMV iMBStartY, U32_WMV iMBEndY)
{
    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliPostY = pWMVDec->m_ppxliPostQY  + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliPostU = pWMVDec->m_ppxliPostQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliPostV = pWMVDec->m_ppxliPostQV  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U32_WMV imbY;
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DeblockInterlaceFrame);

    assert (pWMVDec->m_bDeblockOn);

    for (i = 0; i < pWMVDec->m_iHeightUV; i++) {
        ALIGNED32_FASTMEMCPY(ppxliPostU, ppxliCurrQU, pWMVDec->m_iWidthPrevUV * sizeof(I8_WMV));
        ppxliPostU += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostV, ppxliCurrQV, pWMVDec->m_iWidthPrevUV * sizeof(I8_WMV));
        ppxliPostV += pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV;
        ALIGNED32_FASTMEMCPY(ppxliPostY, ppxliCurrQY, pWMVDec->m_iWidthPrevY * sizeof(I8_WMV));
        ppxliPostY += pWMVDec->m_iWidthPrevY;
        ppxliCurrQY += pWMVDec->m_iWidthPrevY;
    }

    ppxliPostY = pWMVDec->m_ppxliPostQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliPostU = pWMVDec->m_ppxliPostQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliPostV = pWMVDec->m_ppxliPostQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    for (imbY = iMBStartY; imbY < iMBEndY; imbY++) {
        U8_WMV* ppxliCodedPostY = ppxliPostY;
        U8_WMV* ppxliCodedPostU = ppxliPostU;
        U8_WMV* ppxliCodedPostV = ppxliPostV;
        U32_WMV imbX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {            
                g_pDeblockMB(
                    pWMVDec,
                    ppxliCodedPostY, 
                    ppxliCodedPostU, 
                    ppxliCodedPostV, 
                    imbX,                       // left
                    FALSE,                      // right half-edge
                    imbY,                       // top
                    FALSE,                      // bottom half-edge
                    TRUE_WMV,                   // middle
                    pWMVDec->m_iStepSize,
                    pWMVDec->m_iWidthPrevY,
                    pWMVDec->m_iWidthPrevUV);

            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += 4;
            ppxliCodedPostV += 4;
        }
        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliPostU += 16*pWMVDec->m_iWidthPrevUV;
        ppxliPostV += 16*pWMVDec->m_iWidthPrevUV;
    }

}
#endif // WMV9_SIMPLE_ONLY


#ifdef _SUPPORT_POST_FILTERS_

I32_WMV prvWMVAccessRegistry (enum KEYS k, char rw, I32_WMV iData) 
{
    I32_WMV iRtnVal = -1;
#   if !(defined(_XBOX) || defined(macintosh))//
        HKEY hkey1, hkey2, hkey3, hkey4;
        DWORD Type;      // address of buffer for value type
        BYTE  Data[80];       // address of data buffer
        DWORD DataOut = 30;
        DWORD cbData = 80;    // address of data buffer size);
        LONG result = -1;

#       if !( defined(_WMV_TARGET_X86_) || defined(_ARM_) || !defined(UNDER_CE) )
            // Note - Writting the registry on some small CE devices is not allowed since their registry is in ROM.
            // assume writting is available on CEPC but not on other target processors.
            // 20031229 - CE folks say there is always a RW area of the registry, but it may not persist over reboots.
            if ( rw != 'r' )
                return -1;
#       endif // defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)

        result = RegOpenKeyEx (HKEY_CURRENT_USER, TEXT("SOFTWARE"), 0, 0, &hkey1);
        if (result != ERROR_SUCCESS) {
            result = RegOpenKeyEx (HKEY_CURRENT_USER, TEXT("Software"), 0, 0, &hkey1);
        }

        if (result == ERROR_SUCCESS)
            result = RegOpenKeyEx (hkey1, TEXT("Microsoft"), 0, 0, &hkey2);

        if (result != ERROR_SUCCESS) {
            RegCloseKey(hkey1);
            return -1;
        }
  
        if (result == ERROR_SUCCESS) {
            result = RegOpenKeyEx (hkey2, TEXT("Scrunch"), 0, 0, &hkey3);
        };

        if (result == ERROR_FILE_NOT_FOUND ) {
#           if defined(_WMV_TARGET_X86_) || defined(_ARM_) || !defined(UNDER_CE)
                DWORD dwDisposition;
                result = RegCreateKeyEx (hkey2, TEXT("Scrunch"), 0, TEXT(""), 0, 0, NULL, &hkey3, &dwDisposition);
#           endif // defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
        }

        if(result != ERROR_SUCCESS) {
            RegCloseKey(hkey2);
            RegCloseKey(hkey1);
            return -1;
        }

        // at this point, hkey3 is created and result == ERROR_SUCCESS

        switch (k) 
        {   case FPPM :
                if (rw == 'r') {
                    result = RegQueryValueEx (hkey3, TEXT("Force Post Process Mode"), 0, &Type,   &(Data[0]), &cbData);
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    }
                    else
                        iRtnVal = -1;
                }
                break;
            case CPPM :
                if (rw == 'r') {
                    result = RegQueryValueEx (hkey3, TEXT("Current Post Process Mode"), 0, &Type,   &(Data[0]), &cbData);
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    }
                    else 
                        iRtnVal = -1;
                }
#               if defined(_WMV_TARGET_X86_) || defined(_ARM_) || !defined(UNDER_CE)
                    else 
                        result = RegSetValueEx(  hkey3, TEXT("Current Post Process Mode"), 0, REG_DWORD, (BYTE *)(&iData), sizeof(DWORD));
#               endif //defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
            break;
            case PPM :
                if (rw == 'r') {
                    result = RegQueryValueEx (hkey3, TEXT("Post Process Mode"), 0, &Type,   &(Data[0]), &cbData);
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    };
                }
#               if defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                    else 
                        result = RegSetValueEx(  hkey3, TEXT("Post Process Mode"), 0, REG_DWORD, (BYTE *)(&iData), sizeof(DWORD));
#               endif //defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                break;
            case  RES :
                result = RegOpenKeyEx (hkey3, TEXT("Video"), 0, 0, &hkey4);
                if (result == ERROR_FILE_NOT_FOUND ) {
#                   if defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                        DWORD dwDisposition;
                        result = RegCreateKeyEx (hkey3, TEXT("Video"), 0, TEXT(""), 0, 0, NULL, &hkey4, &dwDisposition);
#                   endif // defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                }
                if (result == ERROR_SUCCESS) {
                    if (rw == 'r') {
                        result = RegQueryValueEx (hkey4, TEXT("Resolution"), 0, &Type,   &(Data[0]), &cbData);
                        if (result == ERROR_SUCCESS) {
                            if (Type == REG_DWORD)
                                iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                        }
                    }
                    RegCloseKey(hkey4);
                }
                break;
            case  BITRATE :
                result = RegOpenKeyEx (hkey3, TEXT("Video"), 0, 0, &hkey4);
                if (result == ERROR_FILE_NOT_FOUND ) {
#                   if defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                        DWORD dwDisposition;
                        result = RegCreateKeyEx (hkey3, TEXT("Video"), 0, TEXT(""), 0, 0, NULL, &hkey4, &dwDisposition);
#                   endif // defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                }
                if (result == ERROR_SUCCESS) {
                    if (rw == 'r') {
                        result = RegQueryValueEx (hkey4, TEXT("BitRate"), 0, &Type,   &(Data[0]), &cbData);
                        if (result == ERROR_SUCCESS) {
                            if (Type == REG_DWORD)
                                iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                        }
                    }
#                   if defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                        else
                            result = RegSetValueEx(hkey4, TEXT("BitRate")  , 0, REG_DWORD, (BYTE *)(&iData), sizeof(DWORD));
#                   endif //defined(_WMV_TARGET_X86_) || !defined(UNDER_CE)
                    RegCloseKey(hkey4);
                }
                break;
            case ADAPTPPM :
                if (rw == 'r') {
                    if (result == ERROR_SUCCESS) {
                        result = RegQueryValueEx (hkey3, TEXT("Adapt Post Process Mode"), 0, &Type,   &(Data[0]), &cbData);
                    }
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    }
                    else 
                        iRtnVal = -1;
                } 
                break;
            case ADAPTMHZ :
                if (rw == 'r') {
                    if (result == ERROR_SUCCESS) {
                        result = RegQueryValueEx (hkey3, TEXT("Adapt MHz"), 0, &Type,   &(Data[0]), &cbData);
                    }
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    }
                    else 
                        iRtnVal = -1;
                } 
#               if defined(_WMV_TARGET_X86_) || defined(_ARM_) || !defined(UNDER_CE)
                    else 
                        result = RegSetValueEx(  hkey3, TEXT("Adapt MHz"), 0, REG_DWORD, (BYTE *)(&iData), sizeof(DWORD));
#               endif // defined(_WMV_TARGET_X86_) || defined(_ARM_) || !defined(UNDER_CE)
                break;
            case QPCLEVEL1 :
            case QPCLEVEL2 :
            case QPCLEVEL3 :
            case QPCLEVEL4 :
                {
                    TCHAR strValue[128] = TEXT("Adapt QPC Level 0");
                    strValue[16] = TEXT('0') + k+1-QPCLEVEL1;	//Insert appropriate QPC level into the string.

                    if (rw == 'r') {
                        if (result == ERROR_SUCCESS) {
                            result = RegQueryValueEx (hkey3, strValue, 0, &Type,   &(Data[0]), &cbData);
                        }
                        if (result == ERROR_SUCCESS) {
                            iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                        }
                        else 
                            iRtnVal = -1;
                    }
#                   if defined(_WMV_TARGET_X86_) || defined(_ARM_)|| !defined(UNDER_CE)
                        else
                            result = RegSetValueEx(hkey3, strValue, 0, REG_DWORD, (BYTE *)(&iData), sizeof(DWORD));
#                   endif // defined(_WMV_TARGET_X86_) || defined(_ARM_)|| !defined(UNDER_CE)
                }
                break;
            case OMITBFM :
                if (rw == 'r') {
                    if (result == ERROR_SUCCESS) {
                        result = RegQueryValueEx (hkey3, TEXT("Omit BF Mode"), 0, &Type,   &(Data[0]), &cbData);
                    }
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    }
                    else 
                        iRtnVal = -1;
                }
                break;
            case FAKEBEHIND :
                if (rw == 'r') {
                    if (result == ERROR_SUCCESS) {
                        result = RegQueryValueEx (hkey3, TEXT("Fake Player Behind"), 0, &Type,   &(Data[0]), &cbData);
                    }
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    }
                    else 
                        iRtnVal = -1;
                }
                break;

            case COUNTCPUCYCLES :
                if (rw == 'r') {
                    if (result == ERROR_SUCCESS) {
                        result = RegQueryValueEx (hkey3, TEXT("Count CPU Cycles"), 0, &Type,   &(Data[0]), &cbData);
                    }
                    if (result == ERROR_SUCCESS) {
                        iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
                    }
                    else 
                        iRtnVal = -1;
                }
                break;
            default:
                iRtnVal = -1;
                break;
        }

        // if any of the calls does not return ERROR_SUCCESS, set iRtnVal to -1
        if(result != ERROR_SUCCESS) 
            iRtnVal = -1;

        RegCloseKey(hkey3);
        RegCloseKey(hkey2);
        RegCloseKey(hkey1);
#   endif // !(defined(_XBOX))
    return iRtnVal;
}


tWMVDecodeStatus WMVDecideDeblockLevel (tWMVDecInternalMember *pWMVDec, I32_WMV iWidth, I32_WMV iHeight)
{
    I32_WMV iWritePostProcessMode = 0;
#   if (defined(UNDER_CE) || defined(_XBOX)) && defined(_WMV_TARGET_X86_)
        Bool_WMV bPostFilterForCEandXbox_x86 = TRUE;
#   else
        Bool_WMV bPostFilterForCEandXbox_x86 = FALSE;
#   endif
    I32_WMV iRegForceDeblock = prvWMVAccessRegistry (FPPM, 'r', 0);
    if (0 <= pWMVDec->m_iPostFilterLevel && pWMVDec->m_iPostFilterLevel <= _TOP_POST_FILTER_LEVEL_)
        pWMVDec->m_iPostProcessMode = pWMVDec->m_iPostFilterLevel;
    else if (0 <= iRegForceDeblock && iRegForceDeblock <= _TOP_POST_FILTER_LEVEL_ && !bPostFilterForCEandXbox_x86 ) {
        pWMVDec->m_iPostProcessMode = iRegForceDeblock;
    }
    else {
        //Float_WMV fltRatio;
        I32_WMV ratio;
        I32_WMV iVideoRes;
        I32_WMV iCPUMHz = 100;

#       if defined(UNDER_CE) || defined(_XBOX)  || defined(macintosh)
            // derate CE because their hardware, drivers and kernel may be less well tuned than a desktop systems
            // derate XBox (when enabled) because they use a smaller P2 cache than most other 733 PIII 
            // and because they want more cycles for other functions
            // derating factor is a rough estimate.  sils 20011202
            const I32_WMV iNumerator =  5000;
#       else
            const I32_WMV iNumerator = 10000;
#       endif // defined(UNDER_CE) || defined(_XBOX)

        // protect from naive application parameters - choose conservative values
        I32_WMV iFrameRate = pWMVDec->m_iFrameRate <= 0 ? 30 : pWMVDec->m_iFrameRate;
        I32_WMV iBitRate   = pWMVDec->m_iBitRate <= 0 ? 500 : pWMVDec->m_iBitRate;

#       if defined(WMV_TARGET_X86) || defined(_Embedded_x86) || defined(_XBOX) || defined(WMV_TARGET_AMD64) || defined(WMV_TARGET_IA64)
            if (g_bSupportMMX_WMV)
                _asm emms;              //Make sure floating point calc works for building the table
            iCPUMHz = g_CPUClock ();
#       elif defined(macintosh)
            iCPUMHz = g_MacCPUClock();
#       else
            iCPUMHz = prvWMVAccessRegistry(ADAPTMHZ, 'r', 0);
            if ( iCPUMHz < 100 || 32000 < iCPUMHz ) {
#               if defined(_XSC_) && defined(WMV_OPT_COMMON_ARM)
                    iCPUMHz =  IsProcessorFeaturePresentWMV(WM_PF_ARM_INTEL_XSCALE) ? 400 : ( IsProcessorFeaturePresentWMV(WM_PF_ARM_V4) ? 206 : 100 );
#               elif defined(WMV_OPT_COMMON_ARM)
                    iCPUMHz = IsProcessorFeaturePresentWMV(WM_PF_ARM_V4)  ? 206 : 100;
#               endif
            }
            if ( iCPUMHz < 100 || 100000 < iCPUMHz ) {
                // If we don't know the CPU speed, default to no post processing but let adaption change this later
                pWMVDec->m_iPostProcessMode = 0;
                return WMV_Succeeded;
            }
#       endif

        iVideoRes = (I32_WMV)((iWidth * iHeight * iFrameRate) * ((Float_WMV) sqrt (iBitRate)) / 1000);

        ratio =((iCPUMHz * iNumerator + (iVideoRes>>1)) / iVideoRes);
        //ratio = (I32_WMV) (fltRatio + .5);

        if (!pWMVDec->m_bYUVDstBMP)
            ratio -= 50;

#       if defined(_WMV_TARGET_X86_)
            if (!g_bSupportMMX_WMV)
                ratio -= 100;
#       elif defined(macintosh)
            if (!g_bSupportAltiVec_WMV)
                ratio = 0;
#       else
            ratio -= 100;   // none of the other processors support MMX (yet)
#       endif

        if (ratio >= 120 && _TOP_POST_FILTER_LEVEL_ >= 4)
            pWMVDec->m_iPostProcessMode = 4;
        else if (ratio >= 90 && _TOP_POST_FILTER_LEVEL_ >= 3)
            pWMVDec->m_iPostProcessMode = 3;
        else if (ratio >= 65 && _TOP_POST_FILTER_LEVEL_ >= 2)
            pWMVDec->m_iPostProcessMode = 2;
        else if (ratio >= 42 && _TOP_POST_FILTER_LEVEL_ >= 1)
            pWMVDec->m_iPostProcessMode = 1;
        else
            pWMVDec->m_iPostProcessMode = 0;
    }
    // g_iWMVPostLevel = pWMVDec->m_iPostProcessMode;
#   ifndef UNDER_CE
        // do not write the registry for WinCE
        iWritePostProcessMode = prvWMVAccessRegistry (CPPM, 'w', pWMVDec->m_iPostProcessMode); 
        iWritePostProcessMode = prvWMVAccessRegistry (PPM, 'w', pWMVDec->m_iPostProcessMode); 
        //  even if iWritePostProcessMode == -1, there is no harm here.    
#   endif // UNDER_CE

    return WMV_Succeeded;
}

#endif // _SUPPORT_POST_FILTERS_
