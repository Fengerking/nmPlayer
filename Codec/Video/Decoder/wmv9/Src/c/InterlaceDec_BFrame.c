//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 2002  Microsoft Corporation

Module Name:

        InterlaceDec_BFrame.cpp

Abstract:

        Interlaced B frame decoding functions

Author:

        Kunal Mukerjee (kunalm@microsoft.com) 28-June-2002

Revision History:

*************************************************************************/

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

#ifndef WMV9_SIMPLE_ONLY


#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119                           // see table.13/H.263

tWMVDecodeStatus decodeBInterlace411 (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus result;
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefY = pWMVDec->m_ppxliRef0YPlusExp;
    U8_WMV *ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefY1 = pWMVDec->m_ppxliRef1YPlusExp;
    U8_WMV *ppxliRefU1 = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefV1 = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    U32_WMV imbY, imbX;

    Bool_WMV bFrameInPostBuf = FALSE;

    I32_WMV  iscaleFactor = pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal;

    HUFFMANGET_DBG_HEADER(":decodeB411",13);
    DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef1Y);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeBInterlace411);

    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
       
    pWMVDec->m_iFilterType = FILTER_BICUBIC;


    // SET_NEW_FRAME
    pWMVDec->m_pAltTables->m_iHalfPelMV = FALSE;
    t_AltTablesSetNewFrame(pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);
   
    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {                
        U8_WMV *ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV *ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV *ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV *ppxliRefYMB = ppxliRefY;
        U8_WMV *ppxliRefUMB = ppxliRefU;
        U8_WMV *ppxliRefVMB = ppxliRefV;
        U8_WMV *ppxliRefY1MB = ppxliRefY1;
        U8_WMV *ppxliRefU1MB = ppxliRefU1;
        U8_WMV *ppxliRefV1MB = ppxliRefV1;
        U32_WMV iBlk;
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBY_LoopCount");
                
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {  

            I32_WMV iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
            I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
            I32_WMV iIntra =0;
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBX_LoopCount");

            // these are always true
            pmbmd->m_rgcBlockXformMode [4] = XFORMMODE_4x8;
            pmbmd->m_rgcBlockXformMode [5] = XFORMMODE_4x8;

            ClearFieldMBInterlace411 (pWMVDec->m_ppxliFieldMB, 2); 
        //  pmbmd->m_mbType = DIRECT;

            // decoding MV Mode
            if (pWMVDec->m_iMVSwitchCodingMode == SKIP_RAW) {
                pmbmd->m_chMBMode = (BS_getBit(pWMVDec->m_pbitstrmIn ) ? MB_FIELD : MB_1MV);
            }

            if (pmbmd->m_chMBMode == MB_1MV) {
                result = decodeFrameBMBOverheadInterlace411 (pWMVDec,
                    pmbmd, imbX, imbY);
                if (WMV_Succeeded != result) {
                    return result;
                }
                if (pWMVDec->m_iPrevIFrame == 1 || pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] == IBLOCKMV)
                    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = 0;  

                if (pmbmd->m_mbType == DIRECT)
                {
                    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
                    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
                    I16_WMV iX, iY, iX1, iY1;
                        I32_WMV  iDFTopFieldMvX, iDFTopFieldMvY, iDFBotFieldMvX, iDFBotFieldMvY;
                    I32_WMV  iDBTopFieldMvX, iDBTopFieldMvY, iDBBotFieldMvX, iDBBotFieldMvY;

                        iIntra = 0;
                    pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k + iXBlocks]
                                       = pWMVDec->m_pXMotion [k + iXBlocks + 1] = 0;
                    pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k + iXBlocks]
                                       = pWMVDec->m_pYMotion [k + iXBlocks + 1] = 0;
                    
                    iX = pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex];
                    iY = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex];
                    iX1 = pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex];
                    iY1 = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex];
/*
                    I16_WMV iDFTopFieldMvX = (iX * iscaleFactor + 128) >> 8;
                    I16_WMV iDFTopFieldMvY = (iY * iscaleFactor) >> 8;

                    I16_WMV iDFBotFieldMvX = (iX1 * iscaleFactor + 128) >> 8;
                    I16_WMV iDFBotFieldMvY = (iY1 * iscaleFactor + 128) >> 8;

                    I16_WMV iDBTopFieldMvX = (iX * (iscaleFactor - 256) + 128) >> 8;
                    I16_WMV iDBTopFieldMvY = (iY * (iscaleFactor - 256) + 128) >> 8;

                    I16_WMV iDBBotFieldMvX = (iX1 * (iscaleFactor - 256) + 128) >> 8;
                    I16_WMV iDBBotFieldMvY = (iY1 * (iscaleFactor - 256) + 128) >> 8;
*/
                

                    DirectModeMV ( pWMVDec,  iX, iY, FALSE, imbX, imbY, &iDFTopFieldMvX, &iDFTopFieldMvY,
                        &iDBTopFieldMvX, &iDBTopFieldMvY);

                    DirectModeMV ( pWMVDec,  iX1, iY1, FALSE, imbX, imbY, &iDFBotFieldMvX, &iDFBotFieldMvY,
                        &iDBBotFieldMvX, &iDBBotFieldMvY);

                    SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                        pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                        pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                        0, 0, 0, 0,
                        iDFTopFieldMvX, iDFTopFieldMvY, iDFTopFieldMvX, iDFTopFieldMvY,                         // don't know these at decoder
                        iDBTopFieldMvX, iDBTopFieldMvY, iDBTopFieldMvX, iDBTopFieldMvY,
                        iTopLeftBlkIndex, iBotLeftBlkIndex);

                }
                else
                {
                    if (pmbmd->m_mbType == FORWARD)
                    {
                        iIntra = ComputeFrameMVFromDiffMVInterlace411_BFrame (pWMVDec,
                            imbX, imbY, pWMVDec->m_pDiffMV, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred);
                        SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                            pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                            pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                            0, 0, 0, 0,                         // don't know these at decoder
                            iTopLeftBlkIndex, iBotLeftBlkIndex);
                    }
                    else if (pmbmd->m_mbType == BACKWARD)
                    {
                        iIntra = ComputeFrameMVFromDiffMVInterlace411_BFrame (pWMVDec,
                            imbX, imbY, pWMVDec->m_pDiffMV, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred);
                        SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                            pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                            pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                            0, 0, 0, 0,                         // don't know these at decoder
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                            iTopLeftBlkIndex, iBotLeftBlkIndex);
                    }

                    else if (pmbmd->m_mbType == INTERPOLATE)
                    {
                        I32_WMV iMvIndex ;
                        I32_WMV iMvIndex1;

                            iIntra = ComputeFrameMVFromDiffMVInterlace411_BFrame (pWMVDec, imbX, imbY, 
                                            pWMVDec->m_pDiffMV, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred);
                            iIntra = ComputeFrameMVFromDiffMVInterlace411_BFrame (pWMVDec, imbX, imbY, 
                                            pWMVDec->m_pDiffMV + 1, pWMVDec->m_pB411InterpX, pWMVDec->m_pB411InterpY, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred);
                            SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                                pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                                pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                                pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],
                                pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex],                         // don't know these at decoder
                                pWMVDec->m_pB411InterpX [iTopLeftBlkIndex], pWMVDec->m_pB411InterpY [iTopLeftBlkIndex], pWMVDec->m_pB411InterpX [iTopLeftBlkIndex], pWMVDec->m_pB411InterpY [iTopLeftBlkIndex],
                                iTopLeftBlkIndex, iBotLeftBlkIndex);

                             iMvIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
                             iMvIndex1 = iMvIndex + pWMVDec->m_uintNumMBX * 2;
                            pWMVDec->m_pB411InterpX [iMvIndex1] = pWMVDec->m_pB411InterpX [iMvIndex];
                            pWMVDec->m_pB411InterpY [iMvIndex1] = pWMVDec->m_pB411InterpY [iMvIndex];

                    }

                }

                if (iIntra) {
                    for (iBlk = 0; iBlk < 4; iBlk++) {
                        pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    }
                    
                    result = DecodeFrameIMBInterlace411 (pWMVDec,
                        imbY, imbX, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else {                   
                    result = DecodeFrameBMBInterlace411 (pWMVDec,
                        imbX, imbY, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
                        ppxliRefY1MB, ppxliRefU1MB, ppxliRefV1MB, iscaleFactor); 
                }

                if (WMV_Succeeded != result) {
                    return result;
                }
            } else {

                result = decodeFieldBMBOverheadInterlace411 (pWMVDec, 
                    pmbmd, imbX, imbY);
                if (WMV_Succeeded != result) {
                    return result;
                }
                if (pWMVDec->m_iPrevIFrame == 1 || pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] == IBLOCKMV)
                    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = 0;
                if (pWMVDec->m_iPrevIFrame == 1 || pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] == IBLOCKMV)
                    pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = 0;

                if (pmbmd->m_mbType == DIRECT)
                {
                    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
                    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
                        I32_WMV  iDFTopFieldMvX, iDFTopFieldMvY, iDFBotFieldMvX, iDFBotFieldMvY;
                    I32_WMV  iDBTopFieldMvX, iDBTopFieldMvY, iDBBotFieldMvX, iDBBotFieldMvY;
                        I16_WMV iX, iY, iX1, iY1;

                    iIntra = 0;
                    pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k + iXBlocks]
                                       = pWMVDec->m_pXMotion [k + iXBlocks + 1] = 0;
                    pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k + iXBlocks]
                                       = pWMVDec->m_pYMotion [k + iXBlocks + 1] = 0;
                
                    iX = pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex];
                    iY = pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex];
                    iX1 = pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex];
                    iY1 = pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex];
/*
                    I16_WMV iDFTopFieldMvX = (iX * iscaleFactor + 128) >> 8;
                    I16_WMV iDFTopFieldMvY = (iY * iscaleFactor) >> 8;

                    I16_WMV iDFBotFieldMvX = (iX1 * iscaleFactor + 128) >> 8;
                    I16_WMV iDFBotFieldMvY = (iY1 * iscaleFactor + 128) >> 8;

                    I16_WMV iDBTopFieldMvX = (iX * (iscaleFactor - 256) + 128) >> 8;
                    I16_WMV iDBTopFieldMvY = (iY * (iscaleFactor - 256) + 128) >> 8;

                    I16_WMV iDBBotFieldMvX = (iX1 * (iscaleFactor - 256) + 128) >> 8;
                    I16_WMV iDBBotFieldMvY = (iY1 * (iscaleFactor - 256) + 128) >> 8;
*/
                

                    DirectModeMV ( pWMVDec, iX, iY, FALSE, imbX, imbY, &iDFTopFieldMvX, &iDFTopFieldMvY,
                        &iDBTopFieldMvX, &iDBTopFieldMvY);

                    DirectModeMV ( pWMVDec, iX1, iY1, FALSE, imbX, imbY, &iDFBotFieldMvX, &iDFBotFieldMvY,
                        &iDBBotFieldMvX, &iDBBotFieldMvY);

                    SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                        pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                        pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                        0, 0, 0, 0,
                        iDFTopFieldMvX, iDFTopFieldMvY, iDFBotFieldMvX, iDFBotFieldMvY,                         // don't know these at decoder
                        iDBTopFieldMvX, iDBTopFieldMvY, iDBBotFieldMvX, iDBBotFieldMvY,
                        iTopLeftBlkIndex, iBotLeftBlkIndex);
                }
                else
                {
                    if (pmbmd->m_mbType == FORWARD)
                    {
                        ComputeTopFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                            imbX, imbY, pWMVDec->m_pDiffMV, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred);
                        ComputeBotFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                            imbX, imbY, pWMVDec->m_pDiffMV + 2, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred);

                        SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                            pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                            pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                            0, 0, 0, 0,                         // don't know these at decoder
                            iTopLeftBlkIndex, iBotLeftBlkIndex);
                    }
                    else if (pmbmd->m_mbType == BACKWARD)
                    {
                        ComputeTopFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                            imbX, imbY, pWMVDec->m_pDiffMV, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred);
                        ComputeBotFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                            imbX, imbY, pWMVDec->m_pDiffMV + 2, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred);

                        SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                            pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                            pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                            0, 0, 0, 0,                         // don't know these at decoder
                            pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                            iTopLeftBlkIndex, iBotLeftBlkIndex);
                    }

                    else if (pmbmd->m_mbType == INTERPOLATE)
                    {
                            ComputeTopFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                                imbX, imbY, pWMVDec->m_pDiffMV, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred);
                            ComputeBotFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                                imbX, imbY, pWMVDec->m_pDiffMV + 2, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred);
                            ComputeTopFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                                imbX, imbY, pWMVDec->m_pDiffMV + 1, pWMVDec->m_pB411InterpX, pWMVDec->m_pB411InterpY, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred);
                            ComputeBotFieldMVFromDiffMVInterlace411_BFrame ( pWMVDec, 
                                imbX, imbY, pWMVDec->m_pDiffMV + 2 + 1, pWMVDec->m_pB411InterpX, pWMVDec->m_pB411InterpY, pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred);

                            SetMotionFieldBFrame (pWMVDec->m_pFieldMvX, pWMVDec->m_pFieldMvY,
                                pWMVDec->m_pFieldMvX_FPred, pWMVDec->m_pFieldMvY_FPred,
                                pWMVDec->m_pFieldMvX_BPred, pWMVDec->m_pFieldMvY_BPred,
                                pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],
                                pWMVDec->m_pXMotion [iTopLeftBlkIndex], pWMVDec->m_pYMotion [iTopLeftBlkIndex], pWMVDec->m_pXMotion [iBotLeftBlkIndex], pWMVDec->m_pYMotion [iBotLeftBlkIndex],                         // don't know these at decoder
                                pWMVDec->m_pB411InterpX [iTopLeftBlkIndex], pWMVDec->m_pB411InterpY [iTopLeftBlkIndex], pWMVDec->m_pB411InterpX [iBotLeftBlkIndex], pWMVDec->m_pB411InterpY [iBotLeftBlkIndex],
                                iTopLeftBlkIndex, iBotLeftBlkIndex);
                    }

                }

                if (pWMVDec->m_pDiffMV->iIntra) {

                    for (iBlk = 0; iBlk < 2; iBlk++) {
                        pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    }
                    
                    result = DecodeTopFieldIMBInterlace411 (pWMVDec, imbY, imbX, pmbmd, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else {
                    result = DecodeTopFieldBMBInterlace411 (pWMVDec, 
                        imbX, imbY, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
                        ppxliRefY1MB, ppxliRefU1MB, ppxliRefV1MB, iscaleFactor);  
                }
                if (WMV_Succeeded != result) {
                    return result;
                }

                if ((pWMVDec->m_pDiffMV + 2)->iIntra) {

                    for (iBlk = 2; iBlk < 4; iBlk++) {
                        pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    }

                    result = DecodeBotFieldIMBInterlace411 (pWMVDec, imbY, imbX, pmbmd, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else {

                    result = DecodeBotFieldBMBInterlace411 (pWMVDec,
                        imbX, imbY, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
                        ppxliRefY1MB, ppxliRefU1MB, ppxliRefV1MB, iscaleFactor); 
                }
                if (WMV_Succeeded != result) {
                    return result;
                }

            }
            
            pmbmd->m_bSkip = FALSE;
            pmbmd++;
            ppxliCurrQYMB += 16;
            ppxliCurrQUMB += 4;
            ppxliCurrQVMB += 4;
            ppxliRefYMB += 16;
            ppxliRefUMB += 4;
            ppxliRefVMB += 4;
            ppxliRefY1MB += 16;
            ppxliRefU1MB += 4;
            ppxliRefV1MB += 4;
        }

        // point to the starting location of the first MB of each row
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliCurrQU += pWMVDec->m_iWidthPrevUV << 4;
        ppxliCurrQV += pWMVDec->m_iWidthPrevUV << 4;
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRefU += pWMVDec->m_iWidthPrevUV << 4;
        ppxliRefV += pWMVDec->m_iWidthPrevUV << 4;
        ppxliRefY1 += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRefU1 += pWMVDec->m_iWidthPrevUV << 4;
        ppxliRefV1 += pWMVDec->m_iWidthPrevUV << 4;
    }
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MB_Loop_Done");

    if (pWMVDec->m_bLoopFilter) {
        LoopFilterPFrameDecInterlace411 (pWMVDec,
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_rgmbmd);
    }



    pWMVDec->m_bDisplay_AllMB = TRUE;

 


    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;


   // pWMVDec->m_bColorSettingChanged = FALSE;
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame

    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeFrameBMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB,
    U8_WMV *ppxlcRef1QYMB, U8_WMV *ppxlcRef1QUMB, U8_WMV *ppxlcRef1QVMB, I32_WMV iscaleFactor)
{
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result;
    U32_WMV iBlk;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeFrameBMBInterlace411);

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set [pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
    }
    
    for (iBlk = 0; iBlk < 4; iBlk++) {
        result = DecodeInterBlockYInterlace411 (pWMVDec,
            imbX, imbY, iBlk, pmbmd, ppInterDCTTableInfo_Dec, pDQ);
        if (WMV_Succeeded != result) {
            return result;
        }
    }
    
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (rgCBP [iBlk]) {
            I32_WMV iSubblockCBP = pmbmd->m_rgbCodedBlockPattern2 [iBlk];
            Bool_WMV bTop = (iSubblockCBP & 2); 
            Bool_WMV bBottom = iSubblockCBP & 1; 
            if (bTop) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                result = Decode4x8Interlace411 (pWMVDec, 
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);   
            }
            if (bBottom) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4*8;
                result = Decode4x8Interlace411 (pWMVDec, 
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);           
            } 
        }
    }

    MotionCompFrameInterlace411_BFrame (pWMVDec, 
        imbX, imbY, ppxlcRefQYMB, ppxlcRefQUMB, ppxlcRefQVMB, ppxlcRef1QYMB, ppxlcRef1QUMB, ppxlcRef1QVMB,
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB, pmbmd->m_mbType);
              
    AddErrorFrameInterlace411 (pWMVDec,
        pWMVDec->m_ppxliFieldMB, ppxlcCurrQYMB, ppxlcCurrQUMB, 
        ppxlcCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);

    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeTopFieldBMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB,
    U8_WMV *ppxlcRef1QYMB, U8_WMV *ppxlcRef1QUMB, U8_WMV *ppxlcRef1QVMB, I32_WMV iscaleFactor)
{
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result;
    U32_WMV iBlk;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeTopFieldBMBInterlace411);

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set [pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
    }
    
    for (iBlk = 0; iBlk < 2; iBlk++) {
        result = DecodeInterBlockYInterlace411 (pWMVDec, 
            imbX, imbY, iBlk, pmbmd, ppInterDCTTableInfo_Dec, pDQ);
        if (WMV_Succeeded != result) {
            return result;
        }
    }

    // U0 V0
    for (iBlk = 4; iBlk < 6; iBlk++) {        
        if (rgCBP [iBlk]) {
            I32_WMV iSubblockCBP = pmbmd->m_rgbCodedBlockPattern2 [iBlk];
            Bool_WMV bTop = (iSubblockCBP & 2);            
            if (bTop) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                result = Decode4x8Interlace411 (pWMVDec, 
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);   
            }
        }
    }

    MotionCompTopFieldInterlace411_BFrame (pWMVDec, 
        imbX, imbY, ppxlcRefQYMB, ppxlcRefQUMB, ppxlcRefQVMB, ppxlcRef1QYMB, ppxlcRef1QUMB, ppxlcRef1QVMB,
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB, pmbmd->m_mbType);

    AddErrorTopFieldInterlace411 (pWMVDec, 
        pWMVDec->m_ppxliFieldMB, ppxlcCurrQYMB, ppxlcCurrQUMB, 
        ppxlcCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);

    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeBotFieldBMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB,
    U8_WMV *ppxlcRef1QYMB, U8_WMV *ppxlcRef1QUMB, U8_WMV *ppxlcRef1QVMB, I32_WMV iscaleFactor)
{
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result;
    U32_WMV iBlk;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeBotFieldBMBInterlace411);

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set [pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
    }

    for (iBlk = 2; iBlk < 4; iBlk++) {
        result = DecodeInterBlockYInterlace411 (pWMVDec, 
            imbX, imbY, iBlk, pmbmd, ppInterDCTTableInfo_Dec, pDQ);
        if (WMV_Succeeded != result) {
            return result;
        }
    }

    // U1 V1
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (rgCBP [iBlk]) {
            I32_WMV iSubblockCBP = pmbmd->m_rgbCodedBlockPattern2 [iBlk];
            Bool_WMV bBottom = iSubblockCBP & 1; 
            if (bBottom) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4*8;
                result = Decode4x8Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);           
            } 
        }
    }

    MotionCompBotFieldInterlace411_BFrame (pWMVDec, 
        imbX, imbY, ppxlcRefQYMB, ppxlcRefQUMB, ppxlcRefQVMB, ppxlcRef1QYMB, ppxlcRef1QUMB, ppxlcRef1QVMB,
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB, pmbmd->m_mbType);

    AddErrorBotFieldInterlace411 (pWMVDec, 
        pWMVDec->m_ppxliFieldMB, ppxlcCurrQYMB, ppxlcCurrQUMB, 
        ppxlcCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);


    return WMV_Succeeded;
}

I32_WMV ComputeFrameMVFromDiffMVInterlace411_BFrame (tWMVDecInternalMember *pWMVDec, 
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV, I16_WMV *m_pXTrue, I16_WMV *m_pYTrue, I16_WMV *m_pXPred, I16_WMV *m_pYPred)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV iIBlock = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeFrameMVFromDiffMVInterlace411_BFrame);
 
    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        m_pXTrue [k] = m_pXTrue [k + 1] = m_pXTrue [k + iXBlocks]
                       = m_pXTrue [k + iXBlocks + 1] = IBLOCKMV;
        m_pYTrue [k] = m_pYTrue [k + 1] = m_pYTrue [k + iXBlocks]
                       = m_pYTrue [k + iXBlocks + 1] = 0;
        pWMVDec->m_pXMotionC[imbX + imbY * (I32_WMV) pWMVDec->m_uintNumMBX] = IBLOCKMV;
        pWMVDec->m_pYMotionC[imbX + imbY * (I32_WMV) pWMVDec->m_uintNumMBX] = 0;
    } else  {    

        I32_WMV dX = imbX;  
        I32_WMV dY = imbY;
        Bool_WMV bTopBndry = (dY == 0);

        PredictFieldOneMV (pWMVDec, &dX, &dY, m_pXPred, m_pYPred, bTopBndry);

        m_pXTrue [k + 1] = m_pXTrue [k] = m_pXTrue [k + iXBlocks]
                           = m_pXTrue [k + iXBlocks + 1]
                           = ((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
        m_pYTrue [k + 1] = m_pYTrue [k] = m_pYTrue [k + iXBlocks]
                           = m_pYTrue [k + iXBlocks + 1]
                           = ((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;

    }
    return iIBlock;
}

I32_WMV ComputeTopFieldMVFromDiffMVInterlace411_BFrame ( tWMVDecInternalMember * pWMVDec, 
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV, I16_WMV *m_pXTrue, I16_WMV *m_pYTrue, I16_WMV *m_pXPred, I16_WMV *m_pYPred)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV iIBlock = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeTopFieldMVFromDiffMVInterlace411_BFrame);

    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        m_pXTrue [k] = m_pXTrue [k + 1] = IBLOCKMV;
        m_pYTrue [k] = m_pYTrue [k + 1] = 0;
    } else  {
        I32_WMV dX = imbX;  
        I32_WMV dY = imbY;
        Bool_WMV bTopBndry = (dY == 0);

        PredictFieldTopMV (pWMVDec, &dX, &dY, m_pXPred, m_pYPred, bTopBndry);

        m_pXTrue [k + 1] = m_pXTrue [k] = 
            ((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
        m_pYTrue [k + 1] = m_pYTrue [k] = 
            ((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;
   
    }
    return iIBlock;
}

I32_WMV ComputeBotFieldMVFromDiffMVInterlace411_BFrame ( tWMVDecInternalMember *  pWMVDec, 
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV, I16_WMV *m_pXTrue, I16_WMV *m_pYTrue, I16_WMV *m_pXPred, I16_WMV *m_pYPred)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + iXBlocks + 2 * imbX ;
    I32_WMV iIBlock = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeBotFieldMVFromDiffMVInterlace411_BFrame);

    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        m_pXTrue [k] = m_pXTrue [k + 1] = IBLOCKMV;
        m_pYTrue [k] = m_pYTrue [k + 1] = 0;
    } else  {
        I32_WMV dX = imbX;  
        I32_WMV dY = imbY;
        Bool_WMV bTopBndry = (dY == 0);

        PredictFieldBotMV (pWMVDec, &dX, &dY, m_pXPred, m_pYPred, bTopBndry);

        m_pXTrue [k + 1] = m_pXTrue [k] = 
            ((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
        m_pYTrue [k + 1] = m_pYTrue [k] = 
            ((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;



    }
    return iIBlock;
}

tWMVDecodeStatus decodeFrameBMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPCY = 0;
    U32_WMV iBlk;
    Bool_WMV bSendTableswitchMode = FALSE;
    Bool_WMV bSendXformswitchMode = FALSE;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeFrameBMBOverheadInterlace411);

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;


    memset (pWMVDec->m_pDiffMV, 0, sizeof (CDiffMV) * 4);
    if (pWMVDec->m_iDirectCodingMode == SKIP_RAW) 
    {
        pmbmd->m_mbType = (MBType)BS_getBit(pWMVDec->m_pbitstrmIn );
    }


    if (pWMVDec->m_bBFrameOn)
        if (pWMVDec->m_tFrmType == BVOP)
        {
            if (pmbmd->m_mbType == (MBType)0)
            {
                if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 0)
                {
                    pmbmd->m_mbType =   BACKWARD;
                }
                else if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 0)
                {
                    pmbmd->m_mbType = FORWARD;
                }
                else
                {
                        pmbmd->m_mbType = INTERPOLATE;
                } 
            }
            assert(pWMVDec->m_iX9MVMode != MIXED_MV);
            pmbmd->m_chMBMode = MB_1MV;
        }



    // init vars before skip
    pmbmd->m_rgbDCTCoefPredPattern2 [0] = 0;
    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgcIntraFlag [iBlk] = 0;
    }

    if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn ) ;
    }
    
    if (pmbmd->m_bSkip) {
        memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));
        memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV));   



        return WMV_Succeeded;
    }

    // decode MV

    if (pWMVDec->m_tFrmType != BVOP || pmbmd->m_mbType != DIRECT)
    decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, pWMVDec->m_pDiffMV); 
    pWMVDec->m_pDiffMV->iHybrid = 0; 
    

    if (pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == DIRECT)
    {
        pWMVDec->m_pDiffMV->iIntra = 0;
        pWMVDec->m_pDiffMV->iLast = 1;
    }


    // decode intra info
    if (pWMVDec->m_pDiffMV->iIntra) { // Intra

        // set intra flags
        for (iBlk = 0; iBlk < 6; iBlk++) {
            pmbmd->m_rgcIntraFlag [iBlk] = 1;
        }

        // decode CBP
        if (pWMVDec->m_pDiffMV->iLast == 0) {
            pmbmd->m_bCBPAllZero = TRUE;
        } else {
            pmbmd->m_bCBPAllZero = FALSE;
        }

        if (pmbmd->m_bCBPAllZero) {
            // set cbp to 0
            for (iBlk = 0; iBlk < 6; iBlk ++) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 0;
            }        
        } else {
            // read CBP
            iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufNewPCBPCYDec, pWMVDec->m_pbitstrmIn);
            pmbmd->m_bCBPAllZero = FALSE;


            // set CBP
            for (iBlk = 0; iBlk < 6; iBlk ++) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = (iCBPCY & 0x1);
                iCBPCY >>= 1;
            }

            // decode subblock 4x8 cbp
            for (iBlk = 4; iBlk < 6; iBlk++) {
                if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 0) {
                        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
                    } else if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 1) {
                        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
                    } else {
                        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
                    }
                }        
            }        
        }

        // get AC Pred Flag
        pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBit(pWMVDec->m_pbitstrmIn );

        if (pWMVDec->m_bDQuantOn) {
            if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                return WMV_Failed;
            }        
        }

        if (pmbmd->m_bCBPAllZero == FALSE) {
            bSendTableswitchMode = pWMVDec->m_bDCTTable_MB;

            if (bSendTableswitchMode) {
                I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn );
                if (iDCTMBTableIndex)
                    iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn ); 
                assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
                pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
            }
        }
       
        return WMV_Succeeded;
    }
    
    // decode Inter info
    if (pWMVDec->m_pDiffMV->iLast == 0) {
        pmbmd->m_bCBPAllZero = TRUE;
        // set cbp to 0
        for (iBlk = 0; iBlk < 6; iBlk ++) {
            pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 0;
        }


    } else {

        if (pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == INTERPOLATE)
        {
            decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV + 1)); 
            (pWMVDec->m_pDiffMV + 1)->iHybrid = 0; 
        
            if ((pWMVDec->m_pDiffMV + 1)->iLast == 0) {
                pmbmd->m_bCBPAllZero = TRUE;
                // set cbp to 0
                for (iBlk = 0; iBlk < 6; iBlk ++) {
                    pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 0;
                }
                if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }

                return WMV_Succeeded;

            } 
        }   // if INTERPOLATE


        bSendTableswitchMode = pWMVDec->m_bDCTTable_MB;
        bSendXformswitchMode = pWMVDec->m_bMBXformSwitching && !pWMVDec->m_pDiffMV->iIntra;

        // read CBP
        iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufNewPCBPCYDec, pWMVDec->m_pbitstrmIn);
        pmbmd->m_bCBPAllZero = FALSE;


        // set CBP
        for (iBlk = 0; iBlk < 6; iBlk ++) {
            pmbmd->m_rgbCodedBlockPattern2 [iBlk] = (iCBPCY & 0x1);
            iCBPCY >>= 1;
        }

        // decode subblock 4x8 cbp
        for (iBlk = 4; iBlk < 6; iBlk++) {
            if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 0) {
                    pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
                } else if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 1) {
                    pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
                } else {
                    pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
                }
            }        
        }

        if (pWMVDec->m_bDQuantOn) {
            if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                return WMV_Failed;
            }
        }

        // pmbmd->m_bCBPAllZero is always true here but just leave it here for now
        if (bSendTableswitchMode && !pmbmd->m_bCBPAllZero) {
            I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn );
            if (iDCTMBTableIndex)
                iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn ); 
            assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
            pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
        }

        if (bSendXformswitchMode) {
            if ((pmbmd->m_iMBXformMode = BS_getBits(pWMVDec->m_pbitstrmIn, 2)) == 3) {
                pmbmd->m_bBlkXformSwitchOn = TRUE;
            }
        }

    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }

    return WMV_Succeeded;
}

tWMVDecodeStatus decodeFieldBMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPCY = 0;
    U32_WMV iBlk;
    I32_WMV iCBPCY_V8, iTmpCBP;
    Bool_WMV bSendXformswitchMode = FALSE;
    I32_WMV iL = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeFieldBMBOverheadInterlace411);

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;
    
    // init vars before skip
    pmbmd->m_rgbDCTCoefPredPattern2 [0] = 0;
    
    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgcIntraFlag [iBlk] = 0;
    }


    if (pWMVDec->m_iDirectCodingMode == SKIP_RAW) 
    {
        pmbmd->m_mbType = (MBType)BS_getBit(pWMVDec->m_pbitstrmIn );
    }


    if (pWMVDec->m_bBFrameOn)
        if (pWMVDec->m_tFrmType == BVOP)
        {
            if (pmbmd->m_mbType == (MBType)0)
            {
                if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 0)
                {
                    pmbmd->m_mbType =   BACKWARD;
                }
                else if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 0)
                {
                    pmbmd->m_mbType = FORWARD;
                }
                else
                {
                        pmbmd->m_mbType = INTERPOLATE;
                } 
            }
            assert(pWMVDec->m_iX9MVMode != MIXED_MV);
        }


    if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn );
    }

    if (pmbmd->m_bSkip) {
        memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));        
        memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV) * 4); 
        return WMV_Succeeded;
    }

    // read CBP
    iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufNewPCBPCYDec, pWMVDec->m_pbitstrmIn);
    pmbmd->m_bCBPAllZero = FALSE;

    // decode MV
    iCBPCY_V8 = iCBPCY & 0x3A;

    if (pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == DIRECT)
        iCBPCY_V8 = iCBPCY;

    memset (pWMVDec->m_pDiffMV, 0, sizeof (CDiffMV) * 4);
    

    if (pWMVDec->m_tFrmType != BVOP || pmbmd->m_mbType != DIRECT)
    {
        if (iCBPCY & 1) { // Top
            decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV + 0));  

            if (pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == INTERPOLATE && pWMVDec->m_pDiffMV->iLast)
                decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV + 1));

        } 
    }


    if (pWMVDec->m_tFrmType != BVOP || pmbmd->m_mbType != DIRECT)
    {
        if (iCBPCY & 4) { // Bot 
            decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV + 2)); 

            if (pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == INTERPOLATE && (pWMVDec->m_pDiffMV + 2)->iLast)
                decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV + 3));

        }
    }


    // Update and set CBP


    iL = ((pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == INTERPOLATE && pWMVDec->m_pDiffMV->iLast && (pWMVDec->m_pDiffMV + 1)->iLast)
                    || (pWMVDec->m_pDiffMV->iLast && (pWMVDec->m_tFrmType != BVOP || pmbmd->m_mbType != INTERPOLATE)));
    iCBPCY_V8 |= iL << 0;
    iL = ((pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == INTERPOLATE && (pWMVDec->m_pDiffMV + 2)->iLast && (pWMVDec->m_pDiffMV + 3)->iLast)
                    || ((pWMVDec->m_pDiffMV + 2)->iLast && (pWMVDec->m_tFrmType != BVOP || pmbmd->m_mbType != INTERPOLATE)));
    iCBPCY_V8 |= (iL << 2);

    iTmpCBP = iCBPCY_V8;

    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = iTmpCBP & 0x1;
        iTmpCBP >>= 1;
    }

    // decode subblock 4x8 cbp
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
            if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 0) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
            } else if (BS_getBit(pWMVDec->m_pbitstrmIn ) == 1) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
            } else {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
            }
        }        
    }

    // get AC Pred Flag
    if (pWMVDec->m_pDiffMV->iIntra) {
        // set intra flags - UV not used in indication of intra
        for (iBlk = 0; iBlk < 2; iBlk++) {
            pmbmd->m_rgcIntraFlag [iBlk] = 1;
        }
        pmbmd->m_rgbDCTCoefPredPattern2 [0] |= BS_getBit(pWMVDec->m_pbitstrmIn ) << 1; // Left
    }
    if ((pWMVDec->m_pDiffMV + 2)->iIntra) {
        // set intra flags
        for (iBlk = 2; iBlk < 4; iBlk++) {
            pmbmd->m_rgcIntraFlag [iBlk] = 1;
        }
        pmbmd->m_rgbDCTCoefPredPattern2 [0] |= BS_getBit(pWMVDec->m_pbitstrmIn ); // Right
    }
    
    // Dquant
    if (pWMVDec->m_bDQuantOn && (pWMVDec->m_pDiffMV->iIntra || 
        (pWMVDec->m_pDiffMV + 2)->iIntra || (iCBPCY_V8 != 0))) {
        if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
            return WMV_Failed;
        }
    }
    
    // Dct Tableswitch
    if (pWMVDec->m_bDCTTable_MB && iCBPCY_V8) {
        I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn );
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn ); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }

    // XFormSwitchingMode    
    bSendXformswitchMode |= (pmbmd->m_rgbCodedBlockPattern2 [0] || 
                             pmbmd->m_rgbCodedBlockPattern2 [1]) && 
                             !(pWMVDec->m_pDiffMV->iIntra);

    bSendXformswitchMode |= (pmbmd->m_rgbCodedBlockPattern2 [2] || 
                             pmbmd->m_rgbCodedBlockPattern2 [3]) && 
                             !((pWMVDec->m_pDiffMV + 2)->iIntra);
    bSendXformswitchMode &= pWMVDec->m_bMBXformSwitching;
    if (bSendXformswitchMode) {
        if ((pmbmd->m_iMBXformMode = BS_getBits(pWMVDec->m_pbitstrmIn, 2)) == 3) {
            pmbmd->m_bBlkXformSwitchOn = TRUE;
        }
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }

    return WMV_Succeeded;
}

#endif

#endif //PPCWMP
