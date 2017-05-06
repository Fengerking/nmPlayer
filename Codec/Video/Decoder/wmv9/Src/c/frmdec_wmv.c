//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#ifdef _TI_C55X_
#	include "voWmvPort.h"
#endif
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "strmdec_wmv.h"
#include "codehead.h"
#include "localhuffman_wmv.h"
#include "repeatpad_wmv.h"
#ifdef XBOXGPU
#   include "xboxdec.h"
#   include "xboxfunc.h"
#endif
#ifdef _WMV9AP_
I32_WMV g_iNumShortVLC[] = {1, 1, 2, 1, 3, 1, 2};
I32_WMV g_iDenShortVLC[] = {2, 3, 3, 4, 4, 5, 5};
I32_WMV g_iNumLongVLC[] =  {3, 4, 1, 5, 1, 2, 3, 4, 5, 6, 1, 3, 5, 7};
I32_WMV g_iDenLongVLC[] =  {5, 5, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8};
extern const I32_WMV g_iBInverse[];
#endif

// mingcl: don't delete these two.  These are for hacking the display of the video info in the player.
int g_iBitRate = 0;
int g_iFrameRate = 0;

Void_WMV (*g_pRepeatRef0Y) (RepeatRef0YArgs);
Void_WMV (*g_pRepeatRef0UV) (RepeatRef0UVArgs);

#ifndef WMV9_SIMPLE_ONLY
Void_WMV DecodeFrac(tWMVDecInternalMember *pWMVDec, const I32_WMV iVal, const Bool_WMV bLong);
#endif

// Global tables

// Mapping array for QP index to QP. Used for 5QP deadzone quantizer.
#ifdef _NEW_QPMAPPING_
    I32_WMV g_iStepRemap[31] = {
         1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  
         3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 
        13, 14, 15, 16, 18, 20, 22, 24, 26, 28, 31};
#else
    I32_WMV g_iStepRemap[31] = {
         1,  1,  1,  1,  2,  3,  4,  5,  6,  7,  
         8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 
        18, 19, 20, 21, 22, 23, 24, 25, 27, 29, 31};
#endif


Void_WMV WMVideoDecAssignMotionCompRoutines (tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecAssignMotionCompRoutines);
    if (pWMVDec->m_iMvResolution == 1) {
#       ifndef _EMB_WMV2_
            pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompMixed;
            pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompMixedAndAddError;
#       endif
    } else {
        if (pWMVDec->m_bRndCtrlOn){
            if (pWMVDec->m_iRndCtrl){
#               ifndef _EMB_WMV2_
                    pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompRndCtrlOn;
                    pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOn;
#               endif
#               ifdef _EMB_WMV2_
                    pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_ON;
#               endif
            }
            else{
#               ifndef _EMB_WMV2_
                    pWMVDec->m_pMotionComp = pWMVDec->m_pMotionCompRndCtrlOff;
                    pWMVDec->m_pMotionCompAndAddError = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff;
#               endif
#               ifdef _EMB_WMV2_
                    pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_OFF;
#               endif
            }
#           ifdef DYNAMIC_EDGEPAD
                pWMVDec->m_tEdgePad.m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;
#           endif
        }
    }
    
    if (pWMVDec->m_bRndCtrlOn){
        if (pWMVDec->m_iRndCtrl){
#           ifndef _EMB_WMV2_
                pWMVDec->m_pMotionCompUV = pWMVDec->m_pMotionCompRndCtrlOn;
                pWMVDec->m_pMotionCompAndAddErrorUV = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOn;
#           endif
#           ifdef _EMB_WMV2_
                pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_ON;
#           endif
        }
        else{
#           ifndef _EMB_WMV2_
                pWMVDec->m_pMotionCompUV = pWMVDec->m_pMotionCompRndCtrlOff;
                pWMVDec->m_pMotionCompAndAddErrorUV = pWMVDec->m_pMotionCompAndAddErrorRndCtrlOff;
#           endif
#           ifdef _EMB_WMV2_
                pWMVDec->m_iMotionCompRndCtrl=VECTORIZE_MOTIONCOMP_RNDCTRL_OFF;
#           endif
        }
        
#       ifdef DYNAMIC_EDGEPAD
            pWMVDec->m_tEdgePad.m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;
#       endif
    }
}

tWMVDecodeStatus WMVideoDecDecodeI (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus tWMVStatus;
    I32_WMV ii;
    U32_WMV imbY;

#ifdef _WMV9AP_
    Bool_WMV bTopRowOfSlice = TRUE;
    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevY >> 1);
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1);
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1);
#else
        U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
        U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
        U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
#endif
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    I16_WMV* piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    I16_WMV** piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;
    FUNCTION_PROFILE_DECL_START(fp,DECODEI_PROFILE);
    HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeI",1);
    DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliCurrQY,NULL,NULL);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeI);
    DEBUG_CACHE_START_FRAME;

#ifdef _WMV9AP_
    if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1)
        pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig + pWMVDec->m_uintNumMBY;        
    else
        pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;

    if (pWMVDec->m_iOverlap & 1) {
#else
    if (pWMVDec->m_iOverlap) {
#endif
        for ( ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMB << 2; ii++)
            pWMVDec->m_pXMotion[ii] = IBLOCKMV;
        for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMB; ii++)
            pWMVDec->m_pXMotionC[ii] = IBLOCKMV;
    }

    if (pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_ppInterDCTTableInfo_Dec[0]  
            = pWMVDec->m_ppInterDCTTableInfo_Dec[1]
            = pWMVDec->m_ppInterDCTTableInfo_Dec[2]
            = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
        pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACIntraTableIndx[0]];
        pWMVDec->m_ppIntraDCTTableInfo_Dec[1] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACIntraTableIndx[1]];
        pWMVDec->m_ppIntraDCTTableInfo_Dec[2] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACIntraTableIndx[2]];
        pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
        pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
    }
#   ifndef WMV789_ONLY
        else{
            pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[2];
            pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2];
        }
#   endif

    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);

    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
        U8_WMV* ppxliCodedY = ppxliCurrQY;
        U8_WMV* ppxliCodedU = ppxliCurrQU;
        U8_WMV* ppxliCodedV = ppxliCurrQV;
        Bool_WMV bNot1stRowInSlice;
        U32_WMV imbX;
        Bool_WMV bBottomRowOfSlice = (imbY == (pWMVDec->m_uintNumMBY - 1));

        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBY_LoopCount");

        if (!(imbY & 0x01)) { 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
        }
#       ifndef WMV9_SIMPLE_ONLY
            else if (pWMVDec->m_bMultiresEnabled) {
                I32_WMV iWidthY = pWMVDec->m_rMultiResParams[0].iWidthY;
                piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred + (iWidthY>>4)*BLOCK_SIZE_TIMES2_TIMES6;
                piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable + (iWidthY>>4)*36;
            }
#       endif //ifndef WMV9_SIMPLE_ONLY

        if (pWMVDec->m_cvCodecVersion >= WMV1 || pWMVDec->m_cvCodecVersion == MP4S) // THREE or STANDARD
            bNot1stRowInSlice = (imbY != 0);
        else 
            bNot1stRowInSlice = (prvWMVModulus ((I32_WMV)imbY, (I32_WMV)(pWMVDec->m_uintNumMBYSlice)) != 0);

        if (pWMVDec->m_bSliceWMVA) {

			if (pWMVDec->m_iCurrentTemporalField == 1 && imbY == 0) {
                pWMVDec->m_uiCurrSlice++;
            }

            //if (uiNumSlices > 1) {
                if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
#ifndef _TEST_DROP_SLICE_
                    I32_WMV result;
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush (pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
					if (result != ICERR_OK)
                        return WMV_Failed;
#else // _TEST_DROP_SLICE_
                    if (g_iDropSliceFrame == (I32_WMV) pWMVDec->m_t) {
                        pWMVDec->m_uiCurrSlice++;
                        if (!g_pbDropSlice[pWMVDec->m_uiCurrSlice - 1]) { // if prev slice is not dropped, flush
                            pWMVDec->m_pbitstrmIn->flush ();
                        }
                        if (g_pbDropSlice[pWMVDec->m_uiCurrSlice]) {
                            U32_WMV i = 0;
                            while (i < pWMVDec->m_puiNumBytesOfSliceWMVA[pWMVDec->m_uiCurrSlice]) {
                                BS_getBits(pWMVDec->m_pbitstrmIn, 8);
                                i++;
                            }

                            U32_WMV uiStartRow, uiEndRow;
                            uiStartRow = pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiCurrSlice];
                            if (pWMVDec->m_uiCurrSlice == pWMVDec->m_uiNumSlicesWMVA - 1) {
                                uiEndRow = pWMVDec->m_uintNumMBY;
                            } else { 
                                uiEndRow = pWMVDec->m_puiFirstMBRowOfSlice[pWMVDec->m_uiCurrSlice + 1];
                            }
                            if (pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1) {
                                uiStartRow -= pWMVDec->m_uintNumMBY;
                                uiEndRow -= pWMVDec->m_uintNumMBY;
                            }

                            U32_WMV uiRow;
                            for (uiRow = uiStartRow; uiRow < uiEndRow; uiRow++) {
                                ppxliCodedY += MB_SIZE * pWMVDec->m_uintNumMBX;
                                ppxliCodedU += BLOCK_SIZE * pWMVDec->m_uintNumMBX;
                                ppxliCodedV += BLOCK_SIZE * pWMVDec->m_uintNumMBX;
                                pmbmd += pWMVDec->m_uintNumMBX;
                                piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6 * pWMVDec->m_uintNumMBX;
                                piQuanCoefACPredTable += 36 * pWMVDec->m_uintNumMBX;
                                ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
                                ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV;
                                ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV;
                            }
                            imbY = uiRow - 1;
                            continue;
                        } else {
                            result = ReadSliceHeaderWMVA (imbY);
                            if (result != ICERR_OK)
                                return result;
                        }
                    } else {
                        pWMVDec->m_uiCurrSlice++;
                        pWMVDec->m_pbitstrmIn->flush ();
                        result = ReadSliceHeaderWMVA (imbY);
                        if (result != ICERR_OK)
                            return result;
                    }                   
#endif // _TEST_DROP_SLICE_
                }
           // }
           
            if (pWMVDec->m_pbStartOfSliceRow[imbY])
                bTopRowOfSlice = TRUE;

        }


        if (pWMVDec->m_bStartCode && pWMVDec->m_tFrmType != BIVOP) {
            if( SliceStartCode(pWMVDec, imbY)  != WMV_Succeeded)
                return WMV_Failed;
        }

        pWMVDec->m_rgiDCRef [0] = pWMVDec->m_rgiDCRef [1] = pWMVDec->m_rgiDCRef [2] = 128;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBX_LoopCount");
#           ifndef WMV789_ONLY
                if(pWMVDec->m_cvCodecVersion == MP4S) {
                    if(checkResyncMarker(pWMVDec)) {
                        decodeVideoPacketHeader(pWMVDec, &(pWMVDec->m_iStepSize));
                        uiNumMBFromSliceBdry = 0;
                    }
                }
#           endif

#ifdef _DEBUG
			if( imbY == 14 && imbX == 19) {
				imbX ++;
				imbX --;
			}
#endif

            uiNumMBFromSliceBdry++;
            tWMVStatus = (*pWMVDec->m_pDecodeMBOverheadOfIVOP) (pWMVDec, pmbmd, imbX, imbY); //sw:decodeMBOverheadOfIVOP_MSV() in headdec.c
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }

#ifdef _WMV9AP_
            if ((pWMVDec->m_iOverlap & 4) && !pmbmd->m_bOverlapIMB) {
                pWMVDec->m_iOverlap &= ~1;
                pWMVDec->m_pXMotion[imbY * (I32_WMV) pWMVDec->m_uintNumMBX * 4 + imbX * 2] =
                pWMVDec->m_pXMotion[imbY * (I32_WMV) pWMVDec->m_uintNumMBX * 4 + imbX * 2 + 1] =
                pWMVDec->m_pXMotion[(imbY * 2 + 1) * (I32_WMV) pWMVDec->m_uintNumMBX * 2 + imbX * 2] =
                pWMVDec->m_pXMotion[(imbY * 2 + 1) * (I32_WMV) pWMVDec->m_uintNumMBX * 2 + imbX * 2 + 1] =
                pWMVDec->m_pXMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] = 0;
            }
#endif

            {
                Bool_WMV bLeft = (imbX != 0) && (uiNumMBFromSliceBdry > 1);
                Bool_WMV bTop = bNot1stRowInSlice && (uiNumMBFromSliceBdry > pWMVDec->m_uintNumMBX);
                Bool_WMV bLeftTop = (imbX != 0) && bNot1stRowInSlice && (uiNumMBFromSliceBdry > (pWMVDec->m_uintNumMBX+1));

				//sw:DecodeIMBAcPred() in blkdec_wmv.c
                tWMVStatus = (*pWMVDec->m_pDecodeIMBAcPred) (
                                pWMVDec,
                                pmbmd, ppxliCodedY, ppxliCodedU, ppxliCodedV, 
                                piQuanCoefACPred, piQuanCoefACPredTable, 
#ifdef _WMV9AP_
                                bLeft, bTop, bLeftTop, (pWMVDec->m_bAdvancedProfile ? (imbX + (imbY << 16)) : imbX));  
#else
                                bLeft, bTop, bLeftTop, imbX);  
#endif
            }

#ifdef _WMV9AP_
            if (pWMVDec->m_iOverlap & 4)
                pWMVDec->m_iOverlap = 7;
#endif

            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }

            ppxliCodedY += MB_SIZE;
            ppxliCodedU += BLOCK_SIZE;
            ppxliCodedV += BLOCK_SIZE;
                

            pmbmd++;
            piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable += 36;

            DEBUG_CACHE_STEP_MB;
        } //for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
#ifdef _WMV9AP_
        if (pWMVDec->m_iOverlap & 1)
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, bTopRowOfSlice, FALSE);
        
        bTopRowOfSlice = FALSE;

         if (imbY < pWMVDec->m_uintNumMBY - 1 && pWMVDec->m_pbStartOfSliceRow[imbY + 1])
                bBottomRowOfSlice = TRUE;
#else
        if (pWMVDec->m_iOverlap)
            OverlapMBRowIFrame ( pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, pWMVDec->m_uintNumMBY, 0, 0);
#endif
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row

        if (bBottomRowOfSlice && (pWMVDec->m_iOverlap & 1))
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, FALSE, TRUE);


        DEBUG_CACHE_STEP_MB_ROW;
    } //for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MB_Loop_Done");
            
#   ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_cvCodecVersion >= WMV3) {

            if (pWMVDec->m_bLoopFilter)
#ifdef _WMV9AP_
        DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp  + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevY >> 1), 
                         		    pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1),
                          		    pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1), 
                                    FALSE_WMV, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#else
                DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                                         pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                                         pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,                            
                                         FALSE_WMV, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#endif

        }
        else {
#           ifndef WMV_OPT_COMBINE_LF_CC
                if (pWMVDec->m_bLoopFilter)
                    DeblockSLFrame_V8 (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                                                pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                                                pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                                                FALSE_WMV, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#           else
                if (pWMVDec->m_bLoopFilter)
                    pWMVDec->m_bNeedLoopFilterOnCurrFrm = TRUE_WMV;
#           endif
        }
#   endif //WMV9_SIMPLE_ONLY
    // render
    if (!pWMVDec->m_bLoopFilter)
        memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);

    pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;
    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}


Void_WMV reverseByteArray6(U8_WMV * p)
{
    U8_WMV tmp;
    
    tmp = p[0];
    p[0] = p[5];
    p[5] = tmp;

    tmp = p[1];
    p[1] = p[4];
    p[4] = tmp;

    tmp = p[2];
    p[2] = p[3];
    p[3] = tmp;

}
//***************************************************************************************************
tWMVDecodeStatus WMVideoDecDecodeP (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus tWMVStatus;
    CoordI x, y; // mvXUV, mvYUV; // mvXUV and mvYUV are zoomed UV MV
    U32_WMV imbx, imbY;

    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliRefY =  pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* rgchSkipPrevFrame = pWMVDec->m_rgchSkipPrevFrame;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    CVector* pmv = pWMVDec->m_rgmv;
    I16_WMV* piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    I16_WMV** piQuanCoefACPredTable = pWMVDec->m_rgiQuanCoefACPredTable;
    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;

    FUNCTION_PROFILE(fpDecP[2]);
    HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeP",2);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeP);
    FUNCTION_PROFILE_START(&fpDecP[1],DECODEP_PROFILE);

    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;

    if (pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion >= WMV1) {
        pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
        pWMVDec->m_ppInterDCTTableInfo_Dec[1] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
        pWMVDec->m_ppInterDCTTableInfo_Dec[2] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
        pWMVDec->m_ppIntraDCTTableInfo_Dec[0] 
            = pWMVDec->m_ppIntraDCTTableInfo_Dec[1]
            = pWMVDec->m_ppIntraDCTTableInfo_Dec[2]
            = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
        pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
        pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];


        pWMVDec->m_pHufMVDec = pWMVDec->m_pHufMVDec_Set[pWMVDec->m_iMVTable];
        pWMVDec->m_puXMvFromIndex = pWMVDec->m_puMvFromIndex_Set[pWMVDec->m_iMVTable<<1];
        pWMVDec->m_puYMvFromIndex = pWMVDec->m_puMvFromIndex_Set[(pWMVDec->m_iMVTable<<1) + 1];

    }
#   ifndef WMV789_ONLY
        else{
            pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[2];

            pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[2];
        }
#   endif

    // update all stepsize
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);
    pWMVDec->m_i2DoublePlusStepSize = pWMVDec->m_iDoubleStepSize + pWMVDec->m_iStepMinusStepIsEven;
    pWMVDec->m_i2DoublePlusStepSizeNeg = -1 * pWMVDec->m_i2DoublePlusStepSize;

#   ifdef _EMB_WMV2_
        InitEMB_DecodePShortcut(pWMVDec);
#   endif

#   ifdef DYNAMIC_EDGEPAD
        edgePadSetup(pWMVDec, &pWMVDec->m_tEdgePad);
#   endif

    DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliRef0Y, NULL);

    for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {
        U8_WMV* ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV* ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV* ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV* ppxliRefYMB = ppxliRefY;
        U8_WMV* ppxliRefUMB = ppxliRefU;
        U8_WMV* ppxliRefVMB = ppxliRefV;
        Bool_WMV bNot1stRowInPict = (imbY != 0);
        Bool_WMV bNot1stRowInSlice = (prvWMVModulus ((I32_WMV)imbY, (I32_WMV)(pWMVDec->m_uintNumMBYSlice)) != 0);
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBY_LoopCount");

        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;
        if (!(imbY & 0x01)){ 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
            pmv = pWMVDec->m_rgmv;
        }

        if (pWMVDec->m_cvCodecVersion == MP4S && imbY) 
            bNot1stRowInSlice = TRUE_WMV;

        for (x = 0, imbx = 0; imbx < pWMVDec->m_uintNumMBX; imbx++, x += MB_SIZE) {
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBX_LoopCount");

#if defined(DEBUG) || defined(_DEBUG)
			if(imbY == 0 && imbx == 25){
				imbx += 0;
			}
#endif
#           ifndef WMV789_ONLY
                if (pWMVDec->m_cvCodecVersion == MP4S) {
                    if (checkResyncMarker(pWMVDec)) {
                        decodeVideoPacketHeader(pWMVDec, &(pWMVDec->m_iStepSize));
                        uiNumMBFromSliceBdry = 0;
                    }
                }
#           endif
            uiNumMBFromSliceBdry ++;
            pWMVDec->m_bMBHybridMV = decideHybridMVOn (pWMVDec, x, bNot1stRowInSlice, pmv);
            tWMVStatus = (*pWMVDec->m_pDecodeMBOverheadOfPVOP) (pWMVDec, pmbmd);
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecP[1]);
                return tWMVStatus;
            }

            if (pmbmd->m_bSkip) {
SKIP_MB:
                pmv->x = pmv->y = 0;

                if (pWMVDec->m_bLoopFilter || *rgchSkipPrevFrame == 0) { // MB in the previous frame is not skipped
                    FUNCTION_PROFILE_START(&fpDecP[0],DECODEMBSKIP_PROFILE);
                    (*pWMVDec->m_pMotionCompZero) (
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                        ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
                        pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                }
                *rgchSkipPrevFrame = 1;
                goto NEXT_MB;
            }
            // no skip
            *rgchSkipPrevFrame = 0;
            if (pmbmd->m_dctMd == INTER) {
                I32_WMV iMV_X, iMV_Y, iMV_X_UV, iMV_Y_UV;
                FUNCTION_PROFILE_START(&fpDecP[0],DECODEMBMOTION_PROFILE);

                if (pWMVDec->m_cvCodecVersion >= WMV1 || pWMVDec->m_cvCodecVersion == MP43) 
                    //result = decodeMV (pmv, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice);
                    tWMVStatus = decodeMVMSV (pWMVDec, pmv, pmbmd, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice, pWMVDec->m_pHufMVDec, pWMVDec->m_puXMvFromIndex, pWMVDec->m_puYMvFromIndex);
#               ifndef WMV789_ONLY
                    else if (pWMVDec->m_cvCodecVersion == MP42 || pWMVDec->m_cvCodecVersion == MPG4)
                        tWMVStatus = decodeMV (pWMVDec, pmv, pmbmd, imbx == 0, imbx == pWMVDec->m_uiRightestMB, !bNot1stRowInSlice);
                    else if (pWMVDec->m_cvCodecVersion == MP4S)
                        tWMVStatus = decodeMV (pWMVDec, pmv, pmbmd, imbx == 0 || uiNumMBFromSliceBdry==1 , imbx == pWMVDec->m_uiRightestMB, (!bNot1stRowInSlice) || uiNumMBFromSliceBdry < (pWMVDec->m_uintNumMBX + 1));
#               endif
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
                pmbmd->m_bZeroMV = (pmv->x == 0 && pmv->y == 0);
                // no COD, so skip is decided by MV and CBP
                if (pmbmd->m_bCBPAllZero && pmbmd->m_bZeroMV) {
                    pmbmd->m_bSkip = TRUE_WMV;
                    FUNCTION_PROFILE_STOP(&fpDecP[0]);
                    goto SKIP_MB;
                }
                iMV_X = (x << 1) + pmv->x;
                iMV_Y = (y << 1) + pmv->y;
                iMV_X_UV = x + gmvUVtable4 [pmv->x];
                iMV_Y_UV = y + gmvUVtable4 [pmv->y];


                tWMVStatus = (*pWMVDec->m_pDecodePMB) ( //sw: DecodePMB_EMB() in vopdec_emb.c
                    pWMVDec, 
                    pmbmd,
                    ppxliCurrQYMB, 
                    ppxliCurrQUMB,
                    ppxliCurrQVMB, 
                    iMV_X, // Y MV 
                    iMV_Y,
                    iMV_X_UV, 
                    iMV_Y_UV);
                FUNCTION_PROFILE_STOP(&fpDecP[0]);
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
            }
            else {
                Bool_WMV bLeft, bTop, bLeftTop; 
                FUNCTION_PROFILE_START(&fpDecP[0],DECODEIMBINPFRAME_PROFILE);
                pmv->x = pmv->y = 0;                
                if (pWMVDec->m_cvCodecVersion >= WMV1){ 
                    bLeft = (imbx != 0);
                    bTop = bNot1stRowInPict;
                    bLeftTop = (imbx !=0 ) && bNot1stRowInPict;
                    if (pWMVDec->m_bDCPred_IMBInPFrame) {
#ifdef _WMV9AP_
                        reverseByteArray6(pmbmd->m_rgbCodedBlockPattern2 );
#endif
                        tWMVStatus = DecodeIMBAcPred_PFrame (
                            pWMVDec, 
                            pmbmd,
                            ppxliCurrQYMB,
                            ppxliCurrQUMB,
                            ppxliCurrQVMB,
                            piQuanCoefACPred, 
                            piQuanCoefACPredTable, 
                            bLeft,
                            bTop,
                            bLeftTop);
					}
                    else
                    {
#ifdef _WMV9AP_
                        reverseByteArray6(pmbmd->m_rgbCodedBlockPattern2 );
#endif
                        tWMVStatus = DecodeIMBAcPred (
                            pWMVDec, 
                            pmbmd,
                            ppxliCurrQYMB,
                            ppxliCurrQUMB,
                            ppxliCurrQVMB,
                            piQuanCoefACPred, 
                            piQuanCoefACPredTable, 
                            bLeft && ((pmbmd - 1)->m_dctMd == INTRA),
                            bTop && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA),
                            bLeftTop && ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA),
                            0
                            );
                    }
                }
#               ifndef WMV789_ONLY
                    else{
                        //if(pWMVDec->m_cvCodecVersion == MP43)
                            reverseByteArray6(pmbmd->m_rgbCodedBlockPattern2 );

                        bLeft = (imbx != 0) && ((pmbmd - 1)->m_dctMd == INTRA) &&
                            (uiNumMBFromSliceBdry > 1);
                        bTop = bNot1stRowInSlice && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA)  &&
                            (uiNumMBFromSliceBdry > pWMVDec->m_uintNumMBX);
                        bLeftTop = (imbx!=0 ) && bNot1stRowInSlice && ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA)  &&
                            (uiNumMBFromSliceBdry > (pWMVDec->m_uintNumMBX+1));
                        tWMVStatus = DecodeIMBAcPred_MP4X (
                            pWMVDec, 
                            pmbmd,
                            ppxliCurrQYMB,
                            ppxliCurrQUMB,
                            ppxliCurrQVMB,
                            piQuanCoefACPred, 
                            piQuanCoefACPredTable, 
                            bLeft,
                            bTop,
                            bLeftTop,
                            0
                            );
                    }
#               endif // WMV789_ONLY

                FUNCTION_PROFILE_STOP(&fpDecP[0]);
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecP[1]);
                    return tWMVStatus;
                }
            }
NEXT_MB:
            if (pmbmd->m_dctMd == INTER) {
                piQuanCoefACPred[0] = piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES4] =piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES5] = 0;
            }
            rgchSkipPrevFrame++;
            pmbmd++;
            pmv++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            ppxliRefYMB += MB_SIZE;
            ppxliRefUMB += BLOCK_SIZE;
            ppxliRefVMB += BLOCK_SIZE;
            piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable += 36;
        }

        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliRefU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRefV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
    }
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MB_Loop_Done");

#   ifndef WMV9_SIMPLE_ONLY 
#       ifndef WMV_OPT_COMBINE_LF_CC
            if (pWMVDec->m_bLoopFilter)
#ifdef _WMV9AP_
        DeblockSLFrame_V8 (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                                    pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                                    pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                                    FALSE_WMV, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#else
                DeblockSLFrame_V8 (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                                            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                                            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                                            FALSE_WMV, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#endif
#       else
            if (pWMVDec->m_bLoopFilter)
                pWMVDec->m_bNeedLoopFilterOnCurrFrm = TRUE_WMV;
#       endif 
#   endif // WMV9_SIMPLE_ONLY 
    
    FUNCTION_PROFILE_STOP(&fpDecP[1]);
    return WMV_Succeeded;
}

Void_WMV UpdateDCStepSize(tWMVDecInternalMember *pWMVDec, I32_WMV iStepSize) 
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(UpdateDCStepSize);
    //assert(pWMVDec->m_iStepSize == iStepSize);

    pWMVDec->m_iStepSize = iStepSize;

    if (pWMVDec->m_cvCodecVersion >= WMV3) {
        
        DQuantDecParam *pDQ;
        iStepSize = (2 * iStepSize - 1) + pWMVDec->m_bHalfStep;
        pDQ = &pWMVDec->m_prgDQuantParam [iStepSize];
        
        pWMVDec->m_bStepSizeIsEven = 0;
        pWMVDec->m_iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
        pWMVDec->m_iDoubleStepSize = pDQ->iDoubleStepSize;
		if (pDQ->iDCStepSize == 0)
			return;
		else
			pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = pDQ->iDCStepSize;
        
#ifdef _WMV9AP_
        if (pWMVDec->m_iOverlap & 1) {
#else
        if (pWMVDec->m_iOverlap) {
#endif
            pWMVDec->m_pAvgQuanDctCoefDec [0] = pWMVDec->m_pAvgQuanDctCoefDec [8] = 0;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[8] = 0;
        }
        else {     
            pWMVDec->m_pAvgQuanDctCoefDec [0] = pWMVDec->m_pAvgQuanDctCoefDec [8] = (1024 + (pWMVDec->m_iDCStepSize >> 1)) / pWMVDec->m_iDCStepSize;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[8] = (1024 + (pWMVDec->m_iDCStepSizeC >> 1)) / pWMVDec->m_iDCStepSizeC;
        }
        return;
    }

    pWMVDec->m_bStepSizeIsEven = ((pWMVDec->m_iStepSize & 1) == 0);
    pWMVDec->m_iStepMinusStepIsEven = pWMVDec->m_iStepSize - pWMVDec->m_bStepSizeIsEven;
    pWMVDec->m_iDoubleStepSize = pWMVDec->m_iStepSize << 1;
    pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = 8;

    if (pWMVDec->m_cvCodecVersion >= MP43 || pWMVDec->m_cvCodecVersion == MP4S) {
        if (pWMVDec->m_iStepSize <= 4)   {
            pWMVDec->m_iDCStepSize = 8;
            pWMVDec->m_iDCStepSizeC = 8;
            if(pWMVDec->m_bNewDCQuant && pWMVDec->m_iStepSize <=2)
                pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = pWMVDec->m_iStepSize << 1;
        }
        else if (pWMVDec->m_cvCodecVersion >= WMV1){
            pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize >> 1) + 6;
        }
        else if (pWMVDec->m_iStepSize <= 8)  {
            pWMVDec->m_iDCStepSize = 2 * pWMVDec->m_iStepSize;
            pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize + 13) >> 1;
        }
        else if (pWMVDec->m_iStepSize <= 24) {
            pWMVDec->m_iDCStepSize = pWMVDec->m_iStepSize + 8;
            pWMVDec->m_iDCStepSizeC = (pWMVDec->m_iStepSize + 13) >> 1;
        }
        else {
            pWMVDec->m_iDCStepSize = 2 * pWMVDec->m_iStepSize - 16;
            pWMVDec->m_iDCStepSizeC = pWMVDec->m_iStepSize - 6;
        }
        if (pWMVDec->m_cvCodecVersion >= MP43) {
            pWMVDec->m_pAvgQuanDctCoefDec [0] = pWMVDec->m_pAvgQuanDctCoefDec [8] = (1024 + (pWMVDec->m_iDCStepSize >> 1)) / pWMVDec->m_iDCStepSize;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[8] = (1024 + (pWMVDec->m_iDCStepSizeC >> 1)) / pWMVDec->m_iDCStepSizeC;
        }
    }
}


tWMVDecodeStatus WMVideoDecDecodeClipInfo (tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeClipInfo);
    if (pWMVDec->m_cvCodecVersion >= WMV2) {
        if (pWMVDec->m_bXintra8Switch)
            pWMVDec->m_bXintra8 = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    } else { // WMV1
        I32_WMV iFrameRate = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            pWMVDec->m_iFrameRate = 30;
            pWMVDec->m_iBitRate = 500; // to be conservative for the deblocking/deringing choice
            pWMVDec->m_bRndCtrlOn = FALSE_WMV;
            return WMV_Succeeded;
        }
        if (pWMVDec->m_iFrameRate == 0) // if the info is available from system (app), use it.
            pWMVDec->m_iFrameRate = iFrameRate;
        pWMVDec->m_iBitRate = BS_getBits (pWMVDec->m_pbitstrmIn, 11);

        g_iBitRate = pWMVDec->m_iBitRate;
        g_iFrameRate = pWMVDec->m_iFrameRate;

        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            pWMVDec->m_iFrameRate = 30;
            pWMVDec->m_iBitRate = 500; // to be conservative for the deblocking/deringing choice
            pWMVDec->m_bRndCtrlOn = FALSE_WMV;
            return WMV_Succeeded;
        }
        if (pWMVDec->m_cvCodecVersion != MP42) 
            pWMVDec->m_bRndCtrlOn = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    }
    return WMV_Succeeded;
}


tWMVDecodeStatus setSliceCode (tWMVDecInternalMember *pWMVDec, I32_WMV iSliceCode)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(setSliceCode);
    // setSliceCode 
    if (!pWMVDec->m_fPrepared) {
        if (pWMVDec->m_cvCodecVersion >= WMV2) {
            setRefreshPeriod(pWMVDec);
            if (pWMVDec->m_cvCodecVersion == WMV2) {
                if (iSliceCode == 0)
                    return WMV_Failed;
                pWMVDec->m_uintNumMBYSlice = prvWMVDivision (pWMVDec->m_uintNumMBY , iSliceCode);
            }

        }
        else if (pWMVDec->m_cvCodecVersion != MP4S) {
            if (pWMVDec->m_cvCodecVersion == WMV1) 
                setRefreshPeriod(pWMVDec);
            if (iSliceCode <= 22)
                return WMV_Failed;
         
            pWMVDec->m_uintNumMBYSlice = prvWMVDivision (pWMVDec->m_uintNumMBY , (iSliceCode - 22));
            
        }
        else // MP4S  Needs to be fixed
            pWMVDec->m_uintNumMBYSlice = pWMVDec->m_uintNumMBY;

        pWMVDec->m_bMainProfileOn = (!pWMVDec->m_bDeblockOn && pWMVDec->m_bMBAligned);
        pWMVDec->m_fPrepared = TRUE_WMV;
        return WMV_Succeeded;
    }
    else
        return WMV_Succeeded;
}


tWMVDecodeStatus decodeVOLHead (tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeVOLHead);
    assert (pWMVDec->m_cvCodecVersion == WMV2);
    pWMVDec->m_iFrameRate = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);
    g_iFrameRate = pWMVDec->m_iFrameRate;
    pWMVDec->m_iBitRate = BS_getBits  ( pWMVDec->m_pbitstrmIn, 11);
    g_iBitRate = pWMVDec->m_iBitRate;
    pWMVDec->m_bRndCtrlOn = TRUE_WMV;
    pWMVDec->m_bMixedPel = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bLoopFilter = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bXformSwitch = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bXintra8Switch = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    
    pWMVDec->m_bFrmHybridMVOn = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);

    // DCTTABLE S/W at MB level for WMV2.
    pWMVDec->m_bDCTTable_MB_ENABLED = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_iSliceCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE_WMV2);
    
#ifdef WIN32_DEG
	{
		I8_WMV cTmp[60];
		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nFrame Rate: ");
		sprintf(cTmp,"%d", pWMVDec->m_iFrameRate);
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);

		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nBit Rate: ");
		sprintf(cTmp,"%d", pWMVDec->m_iBitRate);
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);
	}
#endif

    return WMV_Succeeded;
}


#ifndef WMV789_ONLY
    tWMVDecodeStatus decodeVOPHead_MP4S (tWMVDecInternalMember *pWMVDec)
    {
    
        U32_WMV start, uiVopStartCode, iModuloInc, uiMarker;
        Time tCurrSec, tVopIncr;
        I32_WMV iIntraDcSwitchThr;
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeVOPHead_MP4S);
    
        start = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_START_CODE_PREFIX);
        if (start != START_CODE_PREFIX) {
            return WMV_CorruptedBits;
        }
    
        uiVopStartCode = BS_getBits  (pWMVDec->m_pbitstrmIn , NUMBITS_VOP_START_CODE);
        if (uiVopStartCode != VOP_START_CODE) {
            return WMV_CorruptedBits;
        }

#ifdef _WMV9AP_
        pWMVDec->m_tFrmType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_PRED_TYPE);
#else
        pWMVDec->m_tFrmPredType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_PRED_TYPE);
#endif
        // Time reference and VOP_pred_type
        iModuloInc = 0;
        while (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1) != 0)
            iModuloInc++;
        tCurrSec = iModuloInc + pWMVDec->m_tModuloBaseDecd;
        uiMarker = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
        //        assert(uiMarker == 1);
    
        tVopIncr = BS_getBits  ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNumBitsTimeIncr);
        uiMarker = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); // marker bit
        //        assert(uiMarker == 1);
    
        pWMVDec->m_tOldModuloBaseDecd = pWMVDec->m_tModuloBaseDecd;
        pWMVDec->m_tOldModuloBaseDisp = pWMVDec->m_tModuloBaseDisp;
        //(pWMVDec->m_tFrmType != BVOP)
        pWMVDec->m_tModuloBaseDisp = pWMVDec->m_tModuloBaseDecd;      //update most recently displayed time base
        pWMVDec->m_tModuloBaseDecd = tCurrSec;
    
        pWMVDec->m_t = tCurrSec * pWMVDec->m_iClockRate + tVopIncr;
    
        //  if ((pWMVDec->m_coded = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) == 0)         //vop_coded == false
        if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1) == 0)     {       //vop_coded == false
            //Bool_WMV bInterlace = FALSE_WMV; //wchen: temporary solution
            return WMV_CorruptedBits;
        }
#ifdef _WMV9AP_
        if (pWMVDec->m_tFrmType == PVOP)
#else
        if (pWMVDec->m_tFrmPredType == PVOP)
#endif
            pWMVDec->m_iRndCtrl = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1); //"VOP_Rounding_Type"
        else
            pWMVDec->m_iRndCtrl = 0;
    
        iIntraDcSwitchThr = BS_getBits  ( pWMVDec->m_pbitstrmIn, 3);
    
        // INTERLACE
        //        bInterlace = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
        //        I32_WMV UIQUANTPRECISION = 5;
#ifdef _WMV9AP_
        if (pWMVDec->m_tFrmType == IVOP)    {
#else
        if (pWMVDec->m_tFrmPredType == IVOP)    {
#endif
            pWMVDec->m_iStepSize = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);    //also assign intStep to be safe
            pWMVDec->uiFCode = 1;
        }
#ifdef _WMV9AP_
        else if (pWMVDec->m_tFrmType == PVOP) {
#else
        else if (pWMVDec->m_tFrmPredType == PVOP) {
#endif
            pWMVDec->m_iStepSize = BS_getBits  ( pWMVDec->m_pbitstrmIn, 5);
            pWMVDec->uiFCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_FCODE);
            pWMVDec->iScaleFactor = 1 << (pWMVDec->uiFCode - 1);
            pWMVDec->iRange = 16 << pWMVDec->uiFCode;
        }
    
#ifndef _WMV9AP_
        pWMVDec->m_tFrmType = pWMVDec->m_tFrmPredType; 
#endif

        return WMV_Succeeded;
    }
#endif


/********************************************************************************
  X9SkipbitDecode2 - used for norm2 and diff2 skipbit modes
********************************************************************************/
Void_WMV X9SkipbitDecode2 (CWMVMBMode* pmbmd, I32_WMV iNumMB,
                              CInputBitStream_WMV * pIn)
{
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(X9SkipbitDecode2);

    if (iNumMB & 1)
        (pmbmd++)->m_bSkip = BS_getBit(pIn);

    for ( i = iNumMB & 1; i < iNumMB; i += 2) {
        if (BS_getBit(pIn)) {
            if (BS_getBit(pIn))
                pmbmd->m_bSkip = (pmbmd + 1)->m_bSkip = 1;
            else {
                if (BS_getBit(pIn)) {
                    pmbmd->m_bSkip = 0;
                    (pmbmd + 1)->m_bSkip = 1;
                }
                else {
                    pmbmd->m_bSkip = 1;
                    (pmbmd + 1)->m_bSkip = 0;
                }
            }
        }
        else
            pmbmd->m_bSkip = (pmbmd + 1)->m_bSkip = 0;
        pmbmd += 2;
    }
}

/********************************************************************************
  DecodeNormDiff6Tile - used for norm6 and diff6 skipbit mode individual tiles
********************************************************************************/
tWMVDecodeStatus DecodeNormDiff6Tile (CInputBitStream_WMV * pIn, I32_WMV *a2On4Off,
                                       I32_WMV *aCount, I32_WMV *pi6)
{
    Bool_WMV bInvert = FALSE_WMV;
    I32_WMV  iCode;
    I32_WMV i6 = *pi6;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeNormDiff6Tile);

    // 4 stage decoding - read 1, 3, 1, 3 bits respectively
StartOfLocalFn6:
    // length 1 code
    if (BS_getBit(pIn)) {
        i6 = 0;
        goto EndOfLocalFn6;
    }
    iCode = BS_getBits(pIn,3);
    if (iCode > 1) {
        // length 4 codes
        i6 = 1 << (iCode - 2);
        goto EndOfLocalFn6;
    }
    if (iCode == 1) {
        // length 5 codes
        if (BS_getBit(pIn)) {
            // complementary codes - send to start
            if (bInvert)  // only one inversion possible
            {
                *pi6 = i6 ;
                return WMV_Failed;
            }
            bInvert = TRUE_WMV;
            goto StartOfLocalFn6;
        }
        else {
            // 3 on 3 off
            i6 = BS_getBits(pIn,5);
            iCode = aCount[i6];
            if (iCode == 2) {
                i6 |= 32;
            }
            else if (iCode == 3);  // do nothing
            else {
                *pi6 = i6 ;
                return WMV_Failed; // invalid symbol
            }
            goto EndOfLocalFn6;
        }
    }
    // length 8 codes
    iCode = BS_getBits(pIn,4);
    if (iCode == 0xff)  // non-existent codeword
    {
        *pi6 = i6 ;
        return WMV_Failed;
    }
    i6 = a2On4Off[iCode];

EndOfLocalFn6:
    if (bInvert)
        i6 ^= 0x3f;
    *pi6 = i6 ;
    return WMV_Succeeded;
}


/********************************************************************************
  DecodeNormDiff6 - used for norm6 and diff6 skipbit modes
********************************************************************************/
tWMVDecodeStatus DecodeNormDiff6 (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV  i, j;
    // a2On4Off are consecutive indices which have 2 1s and 4 0s in binary rep
    static I32_WMV  a2On4Off[] = { 3, 5, 6, 9, 10, 12, 17, 18, 20, 24, 33, 34, 36, 40, 48 };
    // aCount is the number of 1s
    static I32_WMV   aCount[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
                      1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5 };
    I32_WMV  iStart = 0, jStart = 0;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeNormDiff6);

    if ((prvWMVModulus ((I32_WMV)(pWMVDec->m_uintNumMBY), 3) == 0) && prvWMVModulus ((I32_WMV)(pWMVDec->m_uintNumMBX), 3)) {
        // vertically oriented 2 x 3 tiles
        jStart = pWMVDec->m_uintNumMBX & 1;
        for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i += 3) {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j += 2) {

                I32_WMV  l = i * pWMVDec->m_uintNumMBX + j, i6;
                if (DecodeNormDiff6Tile (pWMVDec->m_pbitstrmIn, a2On4Off, aCount, &i6)
                    != WMV_Succeeded) {
                    // printf ("ERROR ");
                    return WMV_Failed;
                }
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                l += pWMVDec->m_uintNumMBX;
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                l += pWMVDec->m_uintNumMBX;
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;
            }
        }
    }
    else {
        // horizontally oriented 3 x 2 tiles
        iStart = pWMVDec->m_uintNumMBY & 1;
        jStart = prvWMVModulus ((I32_WMV)(pWMVDec->m_uintNumMBX), 3);
        for (i = iStart; i < (I32_WMV) pWMVDec->m_uintNumMBY; i += 2) {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j += 3) {
                I32_WMV  l = i * pWMVDec->m_uintNumMBX + j, i6;
                if (DecodeNormDiff6Tile (pWMVDec->m_pbitstrmIn, a2On4Off, aCount, &i6)
                    != WMV_Succeeded) {
                //  printf ("ERROR ");
                    return WMV_Failed;
                }
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                (pmbmd + l + 2)->m_bSkip = i6 & 1;  i6 >>= 1;
                l += pWMVDec->m_uintNumMBX;
                (pmbmd + l)->m_bSkip = i6 & 1;      i6 >>= 1;
                (pmbmd + l + 1)->m_bSkip = i6 & 1;  i6 >>= 1;
                (pmbmd + l + 2)->m_bSkip = i6 & 1;
            }
        }
    }
    // left column(s)
    for (j = 0; j < jStart; j++) {
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
        }
        else
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = 0;
    }
    // top row
    if (iStart) {
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                (pmbmd + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
        }
        else {
            for (j = jStart; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                (pmbmd + j)->m_bSkip = 0;
        }
    }

    return WMV_Succeeded;
}


tWMVDecodeStatus DecodeSkipBitX9 (tWMVDecInternalMember *pWMVDec, I32_WMV iMode)
{
    I32_WMV   i, j;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    I32_WMV iCodingMode;

    // whether majority skip or nonskip
    Bool_WMV  bDiff = FALSE_WMV, bInvert = BS_getBit(pWMVDec->m_pbitstrmIn);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeSkipBitX9);

    switch (BS_getBits(pWMVDec->m_pbitstrmIn,2)) {
    case 2:
        // norm 2 coding
        iCodingMode = SKIP_NORM2;   
        X9SkipbitDecode2 (pmbmd, (I32_WMV) pWMVDec->m_uintNumMB, pWMVDec->m_pbitstrmIn);
        break;
    case 3:
        // norm 6 coding (not done yet)
        iCodingMode = SKIP_NORM6;
        if (DecodeNormDiff6 (pWMVDec) != WMV_Succeeded)
            return WMV_Failed;
        break;
    case 1:
        // row / col skip coding modes
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            // col skipbit mode
            iCodingMode = SKIP_COL;
            for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                    for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
                }
                else
                    for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = 0;
            }
        }
        else {
            // row skipbit mode
            iCodingMode = SKIP_ROW;
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                    for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
                }
                else
                    for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++)
                        (pmbmd + i * pWMVDec->m_uintNumMBX + j)->m_bSkip = 0;
            }
        }
        break;
    default:
        if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
            iCodingMode = SKIP_DIFF2;
            // diff 2 coding
            bDiff = TRUE_WMV;
            X9SkipbitDecode2 (pmbmd, (I32_WMV) pWMVDec->m_uintNumMB, pWMVDec->m_pbitstrmIn);
            goto EndOfLoop;
        }
        else {
            // diff6, raw
            if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                iCodingMode = SKIP_DIFF6;
                // diff6
                bDiff = TRUE_WMV;
                DecodeNormDiff6 (pWMVDec);
            }
            else {
                // raw
                iCodingMode = SKIP_RAW;
            }
        }
    }

EndOfLoop:
    if (bDiff) {
        CWMVMBMode* pmb1 = pWMVDec->m_rgmbmd;
        // undo differential coding
        for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++) {
            for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++) {
                I32_WMV  iPred;
                if (i + j == 0)
                    iPred = bInvert;
                else if (i == 0)
                    iPred = (pmb1 - 1)->m_bSkip;
                else if (j == 0)
                    iPred = (pmb1 - (I32_WMV) pWMVDec->m_uintNumMBX)->m_bSkip;
                else {
                    if ((pmb1 - 1)->m_bSkip == (pmb1 - (I32_WMV) pWMVDec->m_uintNumMBX)->m_bSkip)
                        iPred = (pmb1 - 1)->m_bSkip;
                    else
                        iPred = bInvert;
                }
                pmb1->m_bSkip ^= iPred;
                pmb1++;
            }
        }
    }
    else {
        // conditional inversion - only for nondifferential coding
        if (bInvert)
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++)
                (pmbmd + i)->m_bSkip ^= 1;
    }

    if (iMode == 0) {
        pWMVDec->m_iSkipbitCodingMode = iCodingMode;
    } else {
#ifdef _WMV9AP_
        if (iMode == 5) 
            pWMVDec->m_iOverlapIMBCodingMode = iCodingMode;
        else if (iMode == 4) 
            pWMVDec->m_iACPredIFrameCodingMode = iCodingMode;
        else if (iMode == 3)
                    pWMVDec->m_iDirectCodingMode = iCodingMode;
        else if (iMode == 2)
            pWMVDec->m_iFieldDctIFrameCodingMode = iCodingMode;
#else
        if (iMode == 3)
            pWMVDec->m_iDirectCodingMode = iCodingMode;
#endif
        else
        pWMVDec->m_iMVSwitchCodingMode = iCodingMode;
    }

    return WMV_Succeeded;
}



Void_WMV DecodeSkipBit(tWMVDecInternalMember *pWMVDec)
{
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    U32_WMV imbY, imbX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeSkipBit);

    switch(pWMVDec->m_SkipBitCodingMode) {
    case Normal:
        for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
            for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
                pmbmd [imbX].m_bSkip = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                //pmbmd++;
            }
            pmbmd += pWMVDec->m_uintNumMBX;
        }
        break;
    case RowPredict:
        for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
            if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { 
                // skip row
                for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
                    pmbmd->m_bSkip = 1;
                    pmbmd++;
                }
            } else { 
                for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
                    pmbmd->m_bSkip = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                    pmbmd++;
                }
            }
        }
        break;
    case ColPredict:
        {
            CWMVMBMode* pmbmdcol = pWMVDec->m_rgmbmd;
            for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
                pmbmd = pmbmdcol;
                if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { // skip column
                    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
                        pmbmd->m_bSkip = 1;
                        pmbmd += pWMVDec->m_uintNumMBX;
                    }
                } else {
                    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
                        pmbmd->m_bSkip = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                        pmbmd += pWMVDec->m_uintNumMBX;
                    }
                }
                pmbmdcol++;
            }
        }
        break;
    }
}

tWMVDecodeStatus WMVideoDecDecodeFrameHead (tWMVDecInternalMember *pWMVDec)
{
    Bool_WMV bNewRef = FALSE_WMV;
    I32_WMV stepDecoded;
#   ifndef WMV9_SIMPLE_ONLY
        I32_WMV i;
        I32_WMV iNumBitsFrameType;
#   endif
    tWMVDecodeStatus result;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeFrameHead);
    
#   ifndef WMV789_ONLY
        if (pWMVDec->m_cvCodecVersion == MP4S)
            return decodeVOPHead_MP4S (pWMVDec);
#   endif
    
    pWMVDec->m_bIsBChangedToI = FALSE;
    
    if (pWMVDec->m_cvCodecVersion == WMV3) { 
        U32_WMV iFrmCntMod4 ;
        if (pWMVDec->m_bSeqFrameInterpolation) {
            pWMVDec->m_bInterpolateCurrentFrame = BS_getBit(pWMVDec->m_pbitstrmIn);
            COVERAGE_CVSET (picture.m_bInterpolateCurrentFrame, pWMVDec->m_bInterpolateCurrentFrame);
        }
        iFrmCntMod4 = BS_getBits  ( pWMVDec->m_pbitstrmIn,2);
        COVERAGE_CVSET (picture.iFrmCntMod4, iFrmCntMod4);
                   
        
        if (pWMVDec->m_cvCodecVersion == WMV3 && pWMVDec->m_bPreProcRange) {
            pWMVDec->m_iRangeState = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            COVERAGE_CVSET (picture.m_iRangeState, pWMVDec->m_iRangeState);
            // g_iRangeRedux = m_iRangeState;
        }        
        
    }
    
    
    if (pWMVDec->m_cvCodecVersion == WMV3) {
        pWMVDec->m_iRefFrameNum = -1;
        if (BS_getBits  ( pWMVDec->m_pbitstrmIn,1) == 1)
#ifdef _WMV9AP_
            pWMVDec->m_tFrmType = PVOP;
#else
            pWMVDec->m_tFrmPredType = PVOP;
#endif
        else
        {
            if (pWMVDec->m_iNumBFrames == 0) {
                if (!pWMVDec->m_bRefLibEnabled)
#ifdef _WMV9AP_
                    pWMVDec->m_tFrmType = IVOP;
#else
                    pWMVDec->m_tFrmPredType = IVOP;
#endif
                else { // pWMVDec->m_bRefLibEnabled == TRUE
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                        pWMVDec->m_tFrmType = IVOP; // non-key IVOP
#else
                        pWMVDec->m_tFrmPredType = IVOP; // non-key IVOP
#endif
                    else {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) {
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = IVOP; //key IVOP
#else
                            pWMVDec->m_tFrmPredType = IVOP; //key IVOP
#endif
                            RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                        }
                        else {
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = PVOP;
#else
                            pWMVDec->m_tFrmPredType = PVOP;
#endif
                            bNewRef = TRUE;
                        }
                    }
                }
            }
#           ifndef WMV9_SIMPLE_ONLY
                else { // pWMVDec->m_bBFrameOn == TRUE
                    if (!pWMVDec->m_bRefLibEnabled) {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = IVOP;
                        else
                            pWMVDec->m_tFrmType = BVOP;
#else
                            pWMVDec->m_tFrmPredType = IVOP;
                        else
                            pWMVDec->m_tFrmPredType = BVOP;
#endif
                    }
                    else {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = BVOP;
#else
                            pWMVDec->m_tFrmPredType = BVOP;
#endif
                        else {
                            if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                                pWMVDec->m_tFrmType = IVOP; // non-key IVOP
#else
                                pWMVDec->m_tFrmPredType = IVOP; // non-key IVOP
#endif
                            else
                            {
                                if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) {
#ifdef _WMV9AP_
                                    pWMVDec->m_tFrmType = IVOP; // key IVOP
#else
                                    pWMVDec->m_tFrmPredType = IVOP; // key IVOP
#endif
                                    RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                                }
                                else {
#ifdef _WMV9AP_
                                    pWMVDec->m_tFrmType = PVOP;
#else
                                    pWMVDec->m_tFrmPredType = PVOP;
#endif
                                    bNewRef = TRUE;
                                }
                            }
                        }
                    }  

#ifdef _WMV9AP_
                    if (pWMVDec->m_tFrmType == BVOP) {
#else
                    if (pWMVDec->m_tFrmPredType == BVOP) {
#endif
                        I32_WMV iShort = 0, iLong = 0;
                        iShort = BS_getBits(pWMVDec->m_pbitstrmIn,3);
                        if (iShort == 0x7) {
                            iLong = BS_getBits(pWMVDec->m_pbitstrmIn,4);
                            if (iLong == 0xe)       // "hole" in VLC
                                return WMV_Failed;
                        
                            if (iLong == 0xf)
                                pWMVDec->m_bIsBChangedToI = TRUE;
                            else {
                                DecodeFrac(pWMVDec, iLong + 112, TRUE);
                                COVERAGE_CVSET(picture.m_BfractionLong, iLong);
                            }
                        }
                        else {
                            DecodeFrac(pWMVDec, iShort, FALSE);
                            COVERAGE_CVSET(picture.m_BfractionShort, iShort);
                        }
                    }
                }
#           endif //WMV9_SIMPLE_ONLY
        }
    }
#   ifndef WMV9_SIMPLE_ONLY
        else
        {
            iNumBitsFrameType = (pWMVDec->m_cvCodecVersion == WMV2) ? NUMBITS_VOP_PRED_TYPE_WMV2 :  NUMBITS_VOP_PRED_TYPE;
#ifdef _WMV9AP_
            pWMVDec->m_tFrmType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, iNumBitsFrameType);

#else
            pWMVDec->m_tFrmPredType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, iNumBitsFrameType);
#endif
        }
#   endif

    COVERAGE_CVSET (picture.m_bRefLibEnabled, pWMVDec->m_bRefLibEnabled);
    COVERAGE_CVSET (picture.m_tFrmPredType,   pWMVDec->m_tFrmPredType);
    COVERAGE_CVSET (picture.m_bIsBChangedToI, pWMVDec->m_bIsBChangedToI);
    
#ifndef _WMV9AP_
    pWMVDec->m_tFrmType = pWMVDec->m_tFrmPredType;
#endif

#   ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bIsBChangedToI)
#ifdef _WMV9AP_
            pWMVDec->m_tFrmType = BIVOP;
#else
            pWMVDec->m_tFrmPredType = IVOP;
        else if (pWMVDec->m_bBFrameOn && pWMVDec->m_tFrmType == BVOP)
            pWMVDec->m_tFrmPredType = PVOP;
#endif
#   endif
    

#ifdef _WMV9AP_
    if (pWMVDec->m_tFrmType  != IVOP && pWMVDec->m_tFrmType  != PVOP && pWMVDec->m_tFrmType  != BVOP && pWMVDec->m_tFrmType  != BIVOP)
#else
    if (pWMVDec->m_tFrmPredType  != IVOP && pWMVDec->m_tFrmPredType  != PVOP)
#endif
        return WMV_Failed;
    
#ifdef _WMV9AP_
    if (pWMVDec->m_cvCodecVersion >= WMV2 && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)) {
#else
    if (pWMVDec->m_cvCodecVersion >= WMV2 && pWMVDec->m_tFrmPredType == IVOP) {
#endif
        /*I32_WMV iBufferFullPercent =*/ BS_getBits  ( pWMVDec->m_pbitstrmIn, 7);
        COVERAGE_CVSET (picture.iBf, iBufferFullPercent);
        if (BS_invalid ( pWMVDec->m_pbitstrmIn))
            return WMV_CorruptedBits;
    }
    COVERAGE_SET_FRAMETYPE (pWMVDec);
    
    stepDecoded = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_QUANTIZER);
    //pWMVDec->m_iStepSize = pWMVDec->m_iStepSize = stepDecoded;
    if (BS_invalid ( pWMVDec->m_pbitstrmIn) ) {
        return WMV_CorruptedBits;
    }
    
    if (pWMVDec->m_cvCodecVersion >= WMV3)
    {
        pWMVDec->m_iQPIndex = stepDecoded;
        COVERAGE_CVSET (picture.m_iQPIndex, pWMVDec->m_iQPIndex);
        if (stepDecoded <= MAXHALFQP)
            pWMVDec->m_bHalfStep = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        else
            pWMVDec->m_bHalfStep = FALSE;
        COVERAGE_CVSET (picture.m_bHalfStep, pWMVDec->m_bHalfStep);
        
        if (pWMVDec->m_bExplicitFrameQuantizer)
            pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        
        if (!pWMVDec->m_bExplicitQuantizer) { // Implicit quantizer
            if (pWMVDec->m_iQPIndex <= MAX3QP) {
                pWMVDec->m_bUse3QPDZQuantizer = TRUE;
                stepDecoded = pWMVDec->m_iQPIndex;
            }
            else {
                pWMVDec->m_bUse3QPDZQuantizer = FALSE;
                stepDecoded = g_iStepRemap[pWMVDec->m_iQPIndex - 1];
            }
        }
        else // Explicit quantizer
            stepDecoded = pWMVDec->m_iQPIndex;

        COVERAGE_CVSET (picture.m_bUse3QPDZQuantizer, pWMVDec->m_bUse3QPDZQuantizer);
        
        pWMVDec->m_iStepSize = stepDecoded;
        
#ifdef _WMV9AP_
        pWMVDec->m_iOverlap = 0;
        if (pWMVDec->m_bSequenceOverlap && !(pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bIsBChangedToI) ) {
            if (pWMVDec->m_iStepSize >= 9)
                pWMVDec->m_iOverlap = 1;
            else if (pWMVDec->m_bAdvancedProfile && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP))
                pWMVDec->m_iOverlap = 7;
        }   // pWMVDec->m_iOverlap last 3 bits: [MB switch=1/frame switch=0][sent=1/implied=0][on=1/off=0]
#else
        pWMVDec->m_iOverlap = FALSE;
        if (pWMVDec->m_iStepSize >= 9) {
            pWMVDec->m_iOverlap = pWMVDec->m_bSequenceOverlap;
            if (pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bIsBChangedToI)
                pWMVDec->m_iOverlap = FALSE;
        }
#endif
#ifdef _WMV9AP_
        if (pWMVDec->m_iOverlap & 1) {
#else
        if (pWMVDec->m_iOverlap) {
#endif
            pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 0;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 0;
        }
        else {
            pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
            pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;
            
        }
        pWMVDec->m_prgDQuantParam = pWMVDec->m_bUse3QPDZQuantizer ? pWMVDec->m_rgDQuantParam3QPDeadzone : pWMVDec->m_rgDQuantParam5QPDeadzone;
    }
    else
        pWMVDec->m_prgDQuantParam = pWMVDec->m_rgDQuantParam5QPDeadzone;
    
    pWMVDec->m_iStepSize = stepDecoded;
    
    if ( pWMVDec->m_iStepSize <= 0 || pWMVDec->m_iStepSize > 31) {
        return WMV_CorruptedBits;
    }
    
    if (pWMVDec->m_iQPIndex <= MAXHIGHRATEQP && pWMVDec->m_cvCodecVersion == WMV3) {
        // If QP (picture quant) is <= MAXQP_3QPDEADZONE then we substitute the highrate
        // inter coeff table for the talking head table among the 3 possible tables that
        // can be used to code the coeff data
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_HghRate;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_HghRate;
    }
    
    if (bNewRef && pWMVDec->m_bRefLibEnabled)
        pWMVDec->m_iRefFrameNum = BS_getBits  ( pWMVDec->m_pbitstrmIn,1);
    COVERAGE_CVSET (picture.m_iRefFrameNum, pWMVDec->m_iRefFrameNum);
    
#   ifndef WMV9_SIMPLE_ONLY

#ifdef _WMV9AP_
        if (pWMVDec->m_bExtendedMvMode) {
#else
        if (pWMVDec->m_bExtendedMvMode) {
#endif
            pWMVDec->m_iMVRangeIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            if (pWMVDec->m_iMVRangeIndex)
                pWMVDec->m_iMVRangeIndex +=  BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            if (pWMVDec->m_iMVRangeIndex == 2)
                pWMVDec->m_iMVRangeIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            SetMVRangeFlag (pWMVDec, pWMVDec->m_iMVRangeIndex);
        }
        COVERAGE_CVSET(picture.m_MVRange, pWMVDec->m_iMVRangeIndex);
    
#ifndef _WMV9AP_
        if (pWMVDec->m_tFrmType != BVOP) {
            if (pWMVDec->m_bMultiresEnabled) {
#else
        if (pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP) {
            if (pWMVDec->m_cvCodecVersion == WMV3 && pWMVDec->m_bMultiresEnabled) {
#endif
                if (pWMVDec->m_bYUV411) {
                    pWMVDec->m_iResIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
                    COVERAGE_CVSET(picture.m_iResIndexInterlaced, pWMVDec->m_iResIndex);
                }
                else {
                    pWMVDec->m_iResIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 2);
                    COVERAGE_CVSET(picture.m_iResIndexProgressive, pWMVDec->m_iResIndex);
                }
            }
        }
    
        if (pWMVDec->m_iResIndex != 0) {
            ResetParams(pWMVDec, pWMVDec->m_iResIndex);
            ResetDecoderParams(pWMVDec);
        }
#   endif // WMV9_SIMPLE_ONLY
    
    //X9
#   ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bYUV411) {
            // get frame level INTERLACE flag   
            Bool_WMV bFieldCodingOn = BS_getBits  ( pWMVDec->m_pbitstrmIn,1);                
            COVERAGE_CVSET (picture.m_bFieldCodingOn, pWMVDec->m_bFieldCodingOn);
            if (bFieldCodingOn) {
                InitDeinterlace(pWMVDec);
                // get Interlace/Progress switch
#ifdef _WMV9AP_
                if (DecodeSkipBitX9(pWMVDec, 1) != WMV_Succeeded) 
#else
                if (DecodeSkipBitX9(pWMVDec, 2) != WMV_Succeeded) 
#endif
                    return WMV_Failed;     
                if (pWMVDec->m_iMVSwitchCodingMode != SKIP_RAW) {
                    for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                        pWMVDec->m_rgmbmd[i].m_chMBMode = (pWMVDec->m_rgmbmd[i].m_bSkip ? MB_FIELD : MB_1MV);
                        //   pWMVDec->m_rgmbmd[i].m_bFieldMode = pWMVDec->m_rgmbmd[i].m_bSkip;
                    }
                }
            } else { // set to MB_1MV to be safe for FORCE_PROGRESSIVE
                I32_WMV i;
                for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                    pWMVDec->m_rgmbmd[i].m_chMBMode = MB_1MV;
                }
            }
        }
#   endif //WMV9_SIMPLE_ONLY
    
#ifdef _WMV9AP_
        if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) 
        {
#else
    if (pWMVDec->m_tFrmPredType == IVOP) {
#endif
        tWMVDecodeStatus tWMVStatus;
        if (pWMVDec->m_cvCodecVersion < WMV2)        
            pWMVDec->m_iSliceCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE);
        tWMVStatus = setSliceCode (pWMVDec, pWMVDec->m_iSliceCode);
        //I32_WMV iSliceCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE);
        if (tWMVStatus != WMV_Succeeded) {
            return tWMVStatus;
        }
        if (pWMVDec->m_cvCodecVersion >= WMV1){
            tWMVStatus = WMVideoDecDecodeClipInfo (pWMVDec);
            if(BS_invalid ( pWMVDec->m_pbitstrmIn) || tWMVStatus != WMV_Succeeded) {
                return tWMVStatus;
            }
            if (pWMVDec->m_cvCodecVersion >= WMV2) {
                pWMVDec->m_bDCPred_IMBInPFrame = FALSE_WMV;
            } else {    // WMV1
                pWMVDec->m_bDCTTable_MB_ENABLED = (pWMVDec->m_iBitRate > MIN_BITRATE_MB_TABLE);
                pWMVDec->m_bDCPred_IMBInPFrame = (pWMVDec->m_iBitRate <= MAX_BITRATE_DCPred_IMBInPFrame && (pWMVDec->m_iFrmWidthSrc * pWMVDec->m_iFrmHeightSrc < 320 * 240));
            }
        }
        if (!pWMVDec->m_bXintra8 && pWMVDec->m_cvCodecVersion >= MP43) {
            //pWMVDec->m_cvCodecVersion == MP43 || pWMVDec->m_cvCodecVersion == WMV1(2)
            // If pWMVDec->m_bDCTTable_MB_ENABLED is on
            if (pWMVDec->m_bDCTTable_MB_ENABLED){
                pWMVDec->m_bDCTTable_MB = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
            }

            if (!pWMVDec->m_bDCTTable_MB)
            {
                // DCT Table swtiching, I and P index are coded separately.
                // Can be jointly coded using the following table. 
                // IP Index : Code
                // 00       : 00, 
                // 11       : 01, 
                // 01       : 100,
                // 10       : 101,
                // 02       : 1100,
                // 12       : 1101,
                // 20       : 1110, 
                // 21       : 11110
                // 22       : 11111
                
                pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
                    pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                }
                pWMVDec->m_rgiDCTACIntraTableIndx[0] = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                if (pWMVDec->m_rgiDCTACIntraTableIndx[0]){
                    pWMVDec->m_rgiDCTACIntraTableIndx[0] += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                }
                pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = pWMVDec->m_rgiDCTACIntraTableIndx[0];
            }
            pWMVDec->m_iIntraDCTDCTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
        }

#       ifndef WMV9_SIMPLE_ONLY
            // Interlace 411 I Frame Dquant
#ifdef _WMV9AP_
            if ((pWMVDec->m_bAdvancedProfile || pWMVDec->m_bYUV411) && pWMVDec->m_iDQuantCodingOn)
#else
            if (pWMVDec->m_bYUV411 && pWMVDec->m_iDQuantCodingOn)
#endif
                DecodeVOPDQuant (pWMVDec, FALSE_WMV);
            else
#       endif
        SetDefaultDQuantSetting(pWMVDec);
        
        pWMVDec->m_iRndCtrl = 1;
    }
    else 
    {
        if (pWMVDec->m_cvCodecVersion == WMV3) {
            result = decodeVOPHead_WMV3(pWMVDec);
            if(result !=  WMV_Succeeded)
                return result;
        }
#       ifndef WMV9_SIMPLE_ONLY
            else
                decodeVOPHead_WMV2(pWMVDec);
#       endif
                
        if (pWMVDec->m_cvCodecVersion >= MP43) {
            // MP43 || WMV1 (2) 
            if (pWMVDec->m_bDCTTable_MB_ENABLED){
                pWMVDec->m_bDCTTable_MB = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
            }
            
            if (!pWMVDec->m_bDCTTable_MB){
                pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
                    pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
                }
            }
            
            pWMVDec->m_rgiDCTACIntraTableIndx[0] = pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = 
                pWMVDec->m_rgiDCTACInterTableIndx[1] = pWMVDec->m_rgiDCTACInterTableIndx[2] = pWMVDec->m_rgiDCTACInterTableIndx[0];
            
            
            pWMVDec->m_iIntraDCTDCTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
            
            if (pWMVDec->m_cvCodecVersion < WMV3)
                pWMVDec->m_iMVTable = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
        }
        
        if (pWMVDec->m_tFrmType == PVOP)
            pWMVDec->m_iRndCtrl ^= 0x01;
    }
    
    // sanity check for B frames - see if it matches previously decoded P
    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    return WMV_Succeeded;
}

tWMVDecodeStatus WMVideoDecDecodeFrameHead2 (tWMVDecInternalMember *pWMVDec)
{
    Bool_WMV bNewRef = FALSE_WMV;
#   ifndef WMV9_SIMPLE_ONLY
        I32_WMV iNumBitsFrameType;
#   endif
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeFrameHead);
    
#   ifndef WMV789_ONLY
        if (pWMVDec->m_cvCodecVersion == MP4S)
            return decodeVOPHead_MP4S (pWMVDec);
#   endif
    
    pWMVDec->m_bIsBChangedToI = FALSE;
    
    if (pWMVDec->m_cvCodecVersion == WMV3) { 
        U32_WMV iFrmCntMod4 ;
        if (pWMVDec->m_bSeqFrameInterpolation) {
            pWMVDec->m_bInterpolateCurrentFrame = BS_getBit(pWMVDec->m_pbitstrmIn);
            COVERAGE_CVSET (picture.m_bInterpolateCurrentFrame, pWMVDec->m_bInterpolateCurrentFrame);
        }
        iFrmCntMod4 = BS_getBits  ( pWMVDec->m_pbitstrmIn,2);
        COVERAGE_CVSET (picture.iFrmCntMod4, iFrmCntMod4);
                   
        
        if (pWMVDec->m_cvCodecVersion == WMV3 && pWMVDec->m_bPreProcRange) {
            pWMVDec->m_iRangeState = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            COVERAGE_CVSET (picture.m_iRangeState, pWMVDec->m_iRangeState);
            // g_iRangeRedux = m_iRangeState;
        }        
        
    }
    
    
    if (pWMVDec->m_cvCodecVersion == WMV3) {
        pWMVDec->m_iRefFrameNum = -1;
        if (BS_getBits  ( pWMVDec->m_pbitstrmIn,1) == 1)
#ifdef _WMV9AP_
            pWMVDec->m_tFrmType = PVOP;
#else
            pWMVDec->m_tFrmPredType = PVOP;
#endif
        else
        {
            if (pWMVDec->m_iNumBFrames == 0) {
                if (!pWMVDec->m_bRefLibEnabled)
#ifdef _WMV9AP_
                    pWMVDec->m_tFrmType = IVOP;
#else
                    pWMVDec->m_tFrmPredType = IVOP;
#endif
                else { // pWMVDec->m_bRefLibEnabled == TRUE
                    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                        pWMVDec->m_tFrmType = IVOP; // non-key IVOP
#else
                        pWMVDec->m_tFrmPredType = IVOP; // non-key IVOP
#endif
                    else {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) {
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = IVOP; //key IVOP
#else
                            pWMVDec->m_tFrmPredType = IVOP; //key IVOP
#endif
                            RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                        }
                        else {
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = PVOP;
#else
                            pWMVDec->m_tFrmPredType = PVOP;
#endif
                            bNewRef = TRUE;
                        }
                    }
                }
            }
#           ifndef WMV9_SIMPLE_ONLY
                else { // pWMVDec->m_bBFrameOn == TRUE
                    if (!pWMVDec->m_bRefLibEnabled) {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = IVOP;
                        else
                            pWMVDec->m_tFrmType = BVOP;
#else
                            pWMVDec->m_tFrmPredType = IVOP;
                        else
                            pWMVDec->m_tFrmPredType = BVOP;
#endif
                    }
                    else {
                        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                            pWMVDec->m_tFrmType = BVOP;
#else
                            pWMVDec->m_tFrmPredType = BVOP;
#endif
                        else {
                            if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1)
#ifdef _WMV9AP_
                                pWMVDec->m_tFrmType = IVOP; // non-key IVOP
#else
                                pWMVDec->m_tFrmPredType = IVOP; // non-key IVOP
#endif
                            else
                            {
                                if (BS_getBits( pWMVDec->m_pbitstrmIn, 1) == 1) {
#ifdef _WMV9AP_
                                    pWMVDec->m_tFrmType = IVOP; // key IVOP
#else
                                    pWMVDec->m_tFrmPredType = IVOP; // key IVOP
#endif
                                    RL_ResetLibrary(pWMVDec-> m_cReferenceLibrary);
                                }
                                else {
#ifdef _WMV9AP_
                                    pWMVDec->m_tFrmType = PVOP;
#else
                                    pWMVDec->m_tFrmPredType = PVOP;
#endif
                                    bNewRef = TRUE;
                                }
                            }
                        }
                    }  
                }
#           endif //WMV9_SIMPLE_ONLY
        }
    }
#   ifndef WMV9_SIMPLE_ONLY
    else
    {
            iNumBitsFrameType = (pWMVDec->m_cvCodecVersion == WMV2) ? NUMBITS_VOP_PRED_TYPE_WMV2 :  NUMBITS_VOP_PRED_TYPE;
#ifdef _WMV9AP_
            pWMVDec->m_tFrmType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, iNumBitsFrameType);

#else
            pWMVDec->m_tFrmPredType = (tFrameType_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, iNumBitsFrameType);
#endif
     }
#   endif

    COVERAGE_CVSET (picture.m_bRefLibEnabled, pWMVDec->m_bRefLibEnabled);
    COVERAGE_CVSET (picture.m_tFrmPredType,   pWMVDec->m_tFrmPredType);
    COVERAGE_CVSET (picture.m_bIsBChangedToI, pWMVDec->m_bIsBChangedToI);
    
#ifndef _WMV9AP_
    pWMVDec->m_tFrmType = pWMVDec->m_tFrmPredType;
#endif

#   ifndef WMV9_SIMPLE_ONLY
        if (pWMVDec->m_bIsBChangedToI)
#ifdef _WMV9AP_
            pWMVDec->m_tFrmType = BIVOP;
#else
            pWMVDec->m_tFrmPredType = IVOP;
        else if (pWMVDec->m_bBFrameOn && pWMVDec->m_tFrmType == BVOP)
            pWMVDec->m_tFrmPredType = PVOP;
#endif
#   endif
    

#ifdef _WMV9AP_
    if (pWMVDec->m_tFrmType  != IVOP && pWMVDec->m_tFrmType  != PVOP && pWMVDec->m_tFrmType  != BVOP && pWMVDec->m_tFrmType  != BIVOP)
#else
    if (pWMVDec->m_tFrmPredType  != IVOP && pWMVDec->m_tFrmPredType  != PVOP)
#endif
        return WMV_Failed;
    
    COVERAGE_SET_FRAMETYPE (pWMVDec);
    
    return WMV_Succeeded;
}





Void_WMV WMVideoDecSwapCurrAndRef (tWMVDecInternalMember *pWMVDec, Bool_WMV bSwapPostBuffers)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmCurrQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecSwapCurrAndRef);

    pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmRef0Q;
    pWMVDec->m_pfrmRef0Q = pTmp;

    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;

    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;

    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

#ifdef _WMV9AP_
    pWMVDec->m_ppxliCurrQPlusExpY0 = (U8_WMV*) pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpU0 = (U8_WMV*) pWMVDec->m_ppxliCurrQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpV0 = (U8_WMV*) pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
#endif

    pWMVDec->m_ppxliRefYPreproc = pWMVDec->m_ppxliRef0Y;      
    pWMVDec->m_ppxliRefUPreproc = pWMVDec->m_ppxliRef0U;          
    pWMVDec->m_ppxliRefVPreproc = pWMVDec->m_ppxliRef0V;

    // swap frame pointers for post processing
    if (bSwapPostBuffers)
        SwapPostAndPostPrev(pWMVDec);
}


Void_WMV SwapPostAndPostPrev(tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SwapPostAndPostPrev);

    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmPostPrevQ;
    pWMVDec->m_pfrmPostPrevQ = pTmp;

    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;

    pWMVDec->m_ppxliPostPrevQY = pWMVDec->m_pfrmPostPrevQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostPrevQU = pWMVDec->m_pfrmPostPrevQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostPrevQV = pWMVDec->m_pfrmPostPrevQ->m_pucVPlane;
}


static I32_WMV s_pXformLUT[4] = {XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_4x8, XFORMMODE_4x4};


tWMVDecodeStatus decodeVOPHead_WMV3 (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus status;
    Bool_WMV bRepeat = FALSE_WMV;
    CWMVMBMode* pmbmd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeVOPHead_WMV3);
    
    pWMVDec->m_bLuminanceWarp = FALSE_WMV;
    
#ifdef _WMV9AP_
    pWMVDec->m_bLuminanceWarpTop = pWMVDec->m_bLuminanceWarpBottom = FALSE;

    if (pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) {   //zou  --->/* Motion vector mode 1 */
#else
    if (pWMVDec->m_tFrmType == BVOP) {
#endif
        if (pWMVDec->m_bYUV411) {
            pWMVDec->m_iX9MVMode = ALL_1MV;
        }
        else {
            pWMVDec->m_iX9MVMode = BS_getBit( pWMVDec->m_pbitstrmIn) ? ALL_1MV : ALL_1MV_HALFPEL_BILINEAR;
        }
    } else {
        if (pWMVDec->m_bYUV411) {
            pWMVDec->m_iX9MVMode = ALL_1MV;
            if (BS_getBit( pWMVDec->m_pbitstrmIn)) {
                pWMVDec->m_bLuminanceWarp = TRUE;
            }
        } else {
RepeatMVRead:
            if (pWMVDec->m_iStepSize > 12) {
                // low rate modes
                if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL;
                else {
#ifdef _WMV9AP_
                    if (pWMVDec->m_tFrmType == BVOP &&!pWMVDec->m_bFieldMode) {
#else
                    if (pWMVDec->m_tFrmType == BVOP) {
#endif
                        pWMVDec->m_iX9MVMode = MIXED_MV;
                    }
                    else if (bRepeat || !BS_getBit(pWMVDec->m_pbitstrmIn))
                            pWMVDec->m_iX9MVMode = MIXED_MV;
                    else {
                        bRepeat = TRUE_WMV;
                        pWMVDec->m_bLuminanceWarp = TRUE_WMV;
                        goto RepeatMVRead;
                    }
                }
            }
            else {
                // high rate modes  //zou -->vc1DECPIC_P_Picture_High_Rate_Motion_Vector_Mode_2
                if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = MIXED_MV;
                else if (BS_getBit(pWMVDec->m_pbitstrmIn))
                    pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL;
                else {
#ifdef _WMV9AP_
                    if (pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) {
#else
                    if (pWMVDec->m_tFrmType == BVOP) {
#endif
                        pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
                    }
                    else if (bRepeat || !BS_getBit(pWMVDec->m_pbitstrmIn))
                            pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
                    else {
                        bRepeat = TRUE_WMV;
                        pWMVDec->m_bLuminanceWarp = TRUE_WMV;
                        goto RepeatMVRead;
                    }
                }
            }
        
        }
        // reset variables that may not be read
        // pWMVDec->m_iGlobalXPad = 0;
        
        if ((pWMVDec->m_iWMV3Profile == WMV3_SIMPLE_PROFILE) && pWMVDec->m_bLuminanceWarp) {
            return WMV_Failed;
        }
        
#ifndef _WMV9AP_
        if (pWMVDec->m_bLuminanceWarp) {
            pWMVDec->m_iLuminanceScale = BS_getBits(pWMVDec->m_pbitstrmIn,6);
            pWMVDec->m_iLuminanceShift = BS_getBits(pWMVDec->m_pbitstrmIn,6);
        }
#endif
    }

#ifdef _WMV9AP_
    	      if (pWMVDec->m_bLuminanceWarp) {
            
            if (pWMVDec->m_bFieldMode) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                    // Both reference field remapped
                    pWMVDec->m_bLuminanceWarpTop = pWMVDec->m_bLuminanceWarpBottom = TRUE;
                    pWMVDec->m_iLuminanceScaleTop = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                    pWMVDec->m_iLuminanceShiftTop = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                    pWMVDec->m_iLuminanceScaleBottom = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                    pWMVDec->m_iLuminanceShiftBottom = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                }
                else if (BS_getBit(pWMVDec->m_pbitstrmIn)) {
                    // Bottom reference field remapped 
                    pWMVDec->m_bLuminanceWarpBottom = TRUE;
                    pWMVDec->m_iLuminanceScaleBottom = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                    pWMVDec->m_iLuminanceShiftBottom = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                }
                else {
                    // Top reference field remapped 
                    pWMVDec->m_bLuminanceWarpTop = TRUE;
                    pWMVDec->m_iLuminanceScaleTop = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                    pWMVDec->m_iLuminanceShiftTop = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                }
            }
            else {
                pWMVDec->m_iLuminanceScale = BS_getBits(pWMVDec->m_pbitstrmIn,6);
                pWMVDec->m_iLuminanceShift = BS_getBits(pWMVDec->m_pbitstrmIn,6);
            }
        }

    
#endif
    COVERAGE_CVSET (picture.m_bLuminanceWarp, pWMVDec->m_bLuminanceWarp);

    // if global warp exists, disable skip MB
    memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);
    
    // decode 1/4 MV flag
    pmbmd = pWMVDec->m_rgmbmd;
    if (pWMVDec->m_iX9MVMode == MIXED_MV) {
        I32_WMV i;
        if ((status=DecodeSkipBitX9(pWMVDec, 1)) != WMV_Succeeded)  // decoded into pWMVDec->m_bSkip bit
            return status;
        if (pWMVDec->m_iMVSwitchCodingMode != SKIP_RAW) {
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                (pmbmd + i) ->m_chMBMode = ((pmbmd + i) ->m_bSkip ? MB_4MV : MB_1MV);  // copied into pWMVDec->m_chMBMode
            }
        }
    }
    else {
        I32_WMV i;
        if (!pWMVDec->m_bYUV411) {
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                (pmbmd + i) ->m_chMBMode = MB_1MV;  // copied into pWMVDec->m_chMBMode
            }
        }
    }
    
    if (pWMVDec->m_bBFrameOn) {
        if (pWMVDec->m_tFrmType == BVOP) {
            if (DecodeSkipBitX9(pWMVDec,3) != WMV_Succeeded)   // decoded into m_bSkip bit
                return WMV_Failed;
            
            if (pWMVDec->m_iDirectCodingMode != SKIP_RAW) {
                I32_WMV i;
                pmbmd = pWMVDec->m_rgmbmd;
                for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                    if (pmbmd->m_bSkip)
                        pmbmd->m_mbType = DIRECT;
                    else
                        pmbmd->m_mbType = (MBType)0;
                    pmbmd++;
                }
            }
        }
    }
    
    // decode skip MB flag
    if ((status=DecodeSkipBitX9(pWMVDec, 0)) != WMV_Succeeded)  // decoded into pWMVDec->m_bSkip bit
        return status;  //zou -->/* Macroblock skip bitplane */
    
    pWMVDec->m_bCODFlagOn = 1;
    // read MV and CBP codetable indices

#ifndef _WMV9AP_
    t_AltTablesDecodeHeader(pWMVDec->m_pAltTables, pWMVDec->m_pbitstrmIn);
#endif
    
    pWMVDec->m_pHufMVTable_V9 = pWMVDec->m_pHufMVTables[BS_getBits(pWMVDec->m_pbitstrmIn,2)];  //zou   --->/* Motion vector huffman table */
    pWMVDec->m_pHufNewPCBPCYDec = pWMVDec->m_pHufPCBPCYTables[BS_getBits(pWMVDec->m_pbitstrmIn,2)]; //zou ---> /* Coded block pattern table */
    
    // 0: off
    // 100 : On All EdgeMB Profile
    // 101 : On 2D Panning Profile 
    // 110: On 1D Panning Profile
    // 1110:PerMB MQuant
    // 1111:Bilevel MQuant
    
    if (pWMVDec->m_iDQuantCodingOn)
        DecodeVOPDQuant (pWMVDec, TRUE_WMV); //zou --->vc1DECPIC_UnpackVOPDQUANTParams(pParams, pSeqParams, pBitstream);
    
    if (pWMVDec->m_bXformSwitch) {
        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
            pWMVDec->m_bMBXformSwitching = FALSE_WMV;
            pWMVDec->m_iFrameXformMode = s_pXformLUT[BS_getBits(pWMVDec->m_pbitstrmIn,2)];  //zou -->/* Macroblock level transform type flag */  ???
        }
        else
            pWMVDec->m_bMBXformSwitching = TRUE_WMV;
    }
    else
        pWMVDec->m_bMBXformSwitching = FALSE_WMV;

    return WMV_Succeeded;
}



Void_WMV DecodeVOPDQuant (tWMVDecInternalMember *pWMVDec, Bool_WMV bPFrame)
{
    //Bool_WMV bNewDQuantSetting = FALSE;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeVOPDQuant);
    
    if (pWMVDec->m_iDQuantCodingOn == 2){
        pWMVDec->m_bDQuantOn = TRUE;
        pWMVDec->m_iPanning = 0x0f;
        pWMVDec->m_iDQuantBiLevelStepSize = decodeDQuantStepsize(pWMVDec, 1);
        return; 
    }

    pWMVDec->m_bDQuantBiLevel = FALSE_WMV;
    pWMVDec->m_iPanning = 0;
    pWMVDec->m_bDQuantOn = BS_getBit ( pWMVDec->m_pbitstrmIn); // turn on DQuant
    if (pWMVDec->m_bDQuantOn) {
        U32_WMV iPanningProfile;
        U32_WMV iPanningCode;
        iPanningProfile = BS_getBits ( pWMVDec->m_pbitstrmIn, 2);
        if (iPanningProfile == 0){
            pWMVDec->m_iPanning = 0x0f;
        }
        else if (iPanningProfile == 1){
            iPanningCode = BS_getBits ( pWMVDec->m_pbitstrmIn, 2);
            if (iPanningCode != 3)
                pWMVDec->m_iPanning = (0x03 << iPanningCode);
            else 
                pWMVDec->m_iPanning = 9; // BL
        }
        else if (iPanningProfile == 2){ // 
            iPanningCode = BS_getBits ( pWMVDec->m_pbitstrmIn, 2);
            pWMVDec->m_iPanning = (1 << iPanningCode);
        }
        else if (iPanningProfile == 3){
            pWMVDec->m_bDQuantBiLevel = BS_getBit( pWMVDec->m_pbitstrmIn);
        }
        if (pWMVDec->m_bDQuantBiLevel || pWMVDec->m_iPanning) {
            //pWMVDec->m_iDQuantBiLevelStepSize = BS_getBits ( pWMVDec, 5);
            pWMVDec->m_iDQuantBiLevelStepSize = decodeDQuantStepsize(pWMVDec, 1);
        }
    }
}


const I32_WMV g_iBInverse[] = { 256, 128, 85, 64, 51, 43, 37, 32 };

tWMVDecodeStatus decodeSpriteVOLHead (tWMVDecInternalMember *pWMVDec)
{
    int m_iFrameRate;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeSpriteVOLHead);

    pWMVDec->m_iFrmWidthSrc  = BS_getBits(pWMVDec->m_pbitstrmIn, 11);
    pWMVDec->m_iFrmHeightSrc = BS_getBits(pWMVDec->m_pbitstrmIn, 11); 
    
    if(pWMVDec->m_iFrmWidthSrc == 0 || pWMVDec->m_iFrmHeightSrc == 0)
        return WMV_Failed;
    
    m_iFrameRate             = BS_getBits(pWMVDec->m_pbitstrmIn, 5);
    
    pWMVDec->m_bRndCtrlOn   = TRUE;
    pWMVDec->m_bMixedPel    = FALSE;
    pWMVDec->m_bXformSwitch = FALSE;
    pWMVDec->m_bLoopFilter  = FALSE;
    
    pWMVDec->m_bXintra8Switch       = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bDCTTable_MB_ENABLED = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_iSliceCode           = BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE_WMV2);
    
    return WMV_Succeeded;
}

#ifdef _WMV9AP_

I32_WMV decodeSequenceHead_Advanced (tWMVDecInternalMember *pWMVDec, U32_WMV uiFormatLength,I32_WMV *piPIC_HORIZ_SIZE, I32_WMV *piPIC_VERT_SIZE, I32_WMV *piDISP_HORIZ_SIZE, I32_WMV *piDISP_VERT_SIZE)
{
    I32_WMV iProfile ;
    //Bool_WMV bPIC_SIZE_FLAG;
    //I32_WMV iPIC_HORIZ_SIZE, iPIC_VERT_SIZE;
    
    I32_WMV iASPECT_RATIO=0, iASPECT_HORIZ_SIZE =0, iASPECT_VERT_SIZE=0;
    I32_WMV iFRAMERATENR, iFRAMERATEDR, iFRAMERATEEXP;
    I32_WMV iCOLOR_PRIM, iTRANSFER_CHAR, iMATRIX_COEF;
    I32_WMV ibit_rate_exponent, ibuffer_size_exponent;
    I32_WMV ihrd_rate_N, ihrd_buffer_N;
    I32_WMV i;
#if 1
    tWMVDimensions *tDimensions= &pWMVDec->tDimensions;
    tWMVHRD ptHRD[17];
	memset(tDimensions, 0, sizeof(tWMVDimensions));
	memset(ptHRD, 0, 17*sizeof(tWMVHRD));
#else
    tWMVDimensions tDimensions = { 0 };
    tWMVHRD ptHRD[9] = {0};
#endif
    pWMVDec->m_bRndCtrlOn = TRUE;
    pWMVDec->m_bRangeRedY_Flag = FALSE; // Resetting range red flags at the beginning of new advance sequence header
    pWMVDec->m_bRangeRedUV_Flag = FALSE;

    // Non-conditional sequence header fields
    iProfile = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
    assert(iProfile == 3);
        
    pWMVDec->m_iWMV3Profile = WMV3_ADVANCED_PROFILE;
    pWMVDec->m_iLevel = BS_getBits(pWMVDec->m_pbitstrmIn, 3);
    pWMVDec->m_iChromaFormat = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
    pWMVDec->m_dFrameRate = BS_getBits(pWMVDec->m_pbitstrmIn, 3);  // midpoint of bin
    pWMVDec->m_iBitRate = BS_getBits(pWMVDec->m_pbitstrmIn, 5);
    pWMVDec->m_bPostProcInfoPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);

    // Max coded size for sequence
    pWMVDec->m_iMaxFrmWidthSrc = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;
    pWMVDec->m_iMaxFrmHeightSrc = 2 * BS_getBits(pWMVDec->m_pbitstrmIn, 12) + 2;
	if (piPIC_HORIZ_SIZE != NULL && piPIC_VERT_SIZE != NULL) {
        *piPIC_HORIZ_SIZE  = pWMVDec->m_iMaxFrmWidthSrc;
        *piPIC_VERT_SIZE   = pWMVDec->m_iMaxFrmHeightSrc;
    } 

 
    // in PKAP there is such an explicit print. I don't think we should do that. chuanggu
    //printf("\nMax Enc Size %d x %d\n", pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);

#ifdef DUMPLOG
    if(pWMVDec->m_pFileLog) {
    fprintf(pWMVDec->m_pFileLog, "\nMaxEncSize %d %d\n", pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
    }
#endif //DUMPLOG
    
    pWMVDec->m_bBroadcastFlags = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bInterlacedSource = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bTemporalFrmCntr = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_bSeqFrameInterpolation = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    // beta=0/RTM=1 flag - no handling for now
    BS_getBits(pWMVDec->m_pbitstrmIn, 1);

    // Placeholders for values parsed from sequence header but not currently used in this implementation of the decoder
    

    // Display extension
    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
        I32_WMV iDISP_HORIZ_SIZE = 2*BS_getBits(pWMVDec->m_pbitstrmIn, 14) + 2;
        I32_WMV iDISP_VERT_SIZE = 2*BS_getBits(pWMVDec->m_pbitstrmIn, 14) + 2;
        if (piDISP_HORIZ_SIZE != NULL && piDISP_VERT_SIZE != NULL) {
            *piDISP_HORIZ_SIZE = iDISP_HORIZ_SIZE;
            *piDISP_VERT_SIZE  = iDISP_VERT_SIZE;
        }
        tDimensions->bDispSizeFlag = TRUE_WMV;
        tDimensions->iDispHorizSize = iDISP_HORIZ_SIZE ;
        tDimensions->iDispVertSize  = iDISP_VERT_SIZE ;
#ifdef DUMPLOG
        if(pWMVDec->m_pFileLog) {
        fprintf(pWMVDec->m_pFileLog,"DspSize %d %d\n", iDISP_HORIZ_SIZE, iDISP_VERT_SIZE);
		}
#endif //DUMPLOG

        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){
            iASPECT_RATIO = BS_getBits(pWMVDec->m_pbitstrmIn, 4);
            if (iASPECT_RATIO == 15) {
                iASPECT_HORIZ_SIZE  = BS_getBits(pWMVDec->m_pbitstrmIn, 8) + 1;
                iASPECT_VERT_SIZE  = BS_getBits(pWMVDec->m_pbitstrmIn, 8) + 1;
            }

            tDimensions->bAspectRatioFlag = TRUE_WMV;
            tDimensions->iAspectRatio = iASPECT_RATIO ;
            tDimensions->iAspect15HorizSize = iASPECT_HORIZ_SIZE;
            tDimensions->iAspect15VertSize = iASPECT_VERT_SIZE  ;

#ifdef DUMPLOG
            if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog,"Ratio %d %d\n", iASPECT_HORIZ_SIZE, iASPECT_VERT_SIZE);
			}
#endif //DUMPLOG
        }

        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){
            tDimensions->bFrameRateFlag = TRUE_WMV;
            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
                iFRAMERATEEXP = BS_getBits(pWMVDec->m_pbitstrmIn, 16) + 1;
                tDimensions->bFrameRateInd = TRUE_WMV;
                tDimensions->iFrameRateExp = iFRAMERATEEXP;
    #ifdef DUMPLOG
                if(pWMVDec->m_pFileLog) {
                fprintf(pWMVDec->m_pFileLog,"FRAMERATEEXP %d\n", iFRAMERATEEXP);
				}
    #endif
            }
            else {
                iFRAMERATENR = BS_getBits(pWMVDec->m_pbitstrmIn, 8);
                iFRAMERATEDR = BS_getBits(pWMVDec->m_pbitstrmIn, 4);

                tDimensions->iFrameRateNR = iFRAMERATENR ;
                tDimensions->iFrameRateDR = iFRAMERATEDR ;

    #ifdef DUMPLOG
                if(pWMVDec->m_pFileLog) {
                fprintf(pWMVDec->m_pFileLog,"FRAMERATENR %d FRAMERATEDR %d\n", iFRAMERATENR, iFRAMERATEDR);
				}
    #endif //DUMPLOG
            }
        }
        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){
            iCOLOR_PRIM = BS_getBits(pWMVDec->m_pbitstrmIn, 8);
            iTRANSFER_CHAR = BS_getBits(pWMVDec->m_pbitstrmIn, 8);
            iMATRIX_COEF = BS_getBits(pWMVDec->m_pbitstrmIn, 8);

            tDimensions->bColorFormatFlag = TRUE_WMV;
            tDimensions->iColorPrim = iCOLOR_PRIM ;
            tDimensions->iTransferChar = iTRANSFER_CHAR ;
            tDimensions->iMatrixCoef = iMATRIX_COEF ;

    #ifdef DUMPLOG
            if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog,"COLOR_PRIM %d\n", iCOLOR_PRIM);
            fprintf(pWMVDec->m_pFileLog,"TRANSFER_CHAR %d\n", iTRANSFER_CHAR);
            fprintf(pWMVDec->m_pFileLog,"MATRIX_COEF %d\n", iMATRIX_COEF);
			}
    #endif //DUMPLOG
        }
    }
    
    pWMVDec->m_bHRD_PARAM_FLAG = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    if (pWMVDec->m_bHRD_PARAM_FLAG) {
		if(pWMVDec->m_bCodecIsWVC1) {
			pWMVDec->m_ihrd_num_leaky_buckets = BS_getBits(pWMVDec->m_pbitstrmIn, 5);
		}
		else {
			pWMVDec->m_ihrd_num_leaky_buckets = BS_getBits(pWMVDec->m_pbitstrmIn, 5) + 1;
		}
        ibit_rate_exponent = BS_getBits(pWMVDec->m_pbitstrmIn, 4) + 6;
        ibuffer_size_exponent = BS_getBits(pWMVDec->m_pbitstrmIn, 4) + 4;

        tDimensions->iNumLeakyBuckets = pWMVDec->m_ihrd_num_leaky_buckets ;
        tDimensions->iBitRateExponent = ibit_rate_exponent ;
        tDimensions->iBufferSizeExponent = ibuffer_size_exponent ;
        tDimensions->pHRD = pWMVDec->m_ihrd_num_leaky_buckets>0 ? ptHRD : NULL;

#ifdef DUMPLOG
        if(pWMVDec->m_pFileLog) {
        fprintf(pWMVDec->m_pFileLog,"HRD %d %d %d ", pWMVDec->m_ihrd_num_leaky_buckets, ibit_rate_exponent, ibuffer_size_exponent);
		}
#endif //DUMPLOG

        for (i = 0; i < pWMVDec->m_ihrd_num_leaky_buckets; i++){
            ihrd_rate_N = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            ihrd_buffer_N = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            ptHRD[i].ui16HrdRate   = ihrd_rate_N;
            ptHRD[i].ui16HrdBuffer = ihrd_buffer_N;
#ifdef DUMPLOG
			if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog,"%d %d ", ihrd_rate_N, ihrd_buffer_N);
			}
#endif //DUMPLOG
        }

#ifdef DUMPLOG
		if(pWMVDec->m_pFileLog) {
        fprintf(pWMVDec->m_pFileLog, "\n");
		}
#endif //DUMPLOG
    }

#ifdef WIN32_DEG
	{
		I8_WMV cTmp[60];
		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nProfile: Advanced Profile(AP)");
		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nLevel: ");
		if(pWMVDec->m_iLevel <= 4) sprintf(cTmp,"L%d", pWMVDec->m_iLevel);
		else sprintf(cTmp,"SMPTE Resevered");
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);

		if(pWMVDec->m_iNumBFrames == 1)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nB frame: Contained");
		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nB frame: Not Contained");
	}
#endif

    if (BS_invalid (pWMVDec->m_pbitstrmIn)) {
#ifdef WIN32_DEG
		strcat(pWMVDec->m_SeqHeadInfoString, "\r\n*** Error:There is no enough bits for decoding sequence header. ***");
#endif
        return ICERR_ERROR;
    }

    // Set up variables based on sequence parameters
    pWMVDec->m_dFrameRate = 4 * pWMVDec->m_dFrameRate + 2;  // midpoint of bin
    pWMVDec->m_iFrameRate = (I32_WMV)pWMVDec->m_dFrameRate;
    pWMVDec->m_iBitRate = 64 * pWMVDec->m_iBitRate + 32;
    //g_iBframeDist = pWMVDec->m_iNumBFrames;
    pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iNumBFrames];
    //pWMVDec->m_bExplicitQuantizer = pWMVDec->m_bExplicitSeqQuantizer || pWMVDec->m_bExplicitFrameQuantizer;

    SetMVRangeFlag (pWMVDec, 0);

    //initAdaptPostLevel();   // re-init now that we know the frame rate
    //initBFOmission();       // init BF Omit logic
    
	// old/unused stuff
	pWMVDec->m_bMixedPel = pWMVDec->m_bFrmHybridMVOn = FALSE;
	pWMVDec->m_bYUV411 = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1); 
	pWMVDec->m_bSpriteMode = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_bXintra8Switch = pWMVDec->m_bXintra8 = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_bMultiresEnabled  = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_b16bitXform  = TRUE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	pWMVDec->m_bDCTTable_MB_ENABLED  = FALSE; //BS_getBits(pWMVDec->m_pbitstrmIn, 1);
	// may have to override transform function pointers
	//if (pWMVDec->m_b16bitXform) 
	decideMMXRoutines (pWMVDec);
	m_InitFncPtrAndZigzag(pWMVDec);

	pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;
	pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantize16;
	// The following bit determines wthe encoder used to generate the content:
	// 0: Beta 1: RTM. Due to bugs, each version produces an incompatible
	// bitstream. Parsing this value allows the decoder to properly decode the corresponding version.
	// NOTE: default at this point is: pWMVDec->m_bBetaContent = TRUE, pWMVDec->m_bRTMContent = FALSE
	// Depending on the detected encoder version, these values are overidden as follows:
	pWMVDec->m_iBetaRTMMismatchIndex = 1;
	pWMVDec->m_bRTMContent = TRUE;
	pWMVDec->m_bBetaContent = FALSE;
	pWMVDec->m_iBetaRTMMismatchIndex = 0;
	#ifdef _GENERATE_DXVA_DATA_
	if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
	{
		dxvaInitProtocol();
		dxvaOpen();
		dxvaReset(pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY, (I32_WMV)(pWMVDec->m_cvCodecVersion == WMV3),0);
	}
	#endif

#ifdef WIN32_DEG
	{
		I8_WMV cTmp[60];
		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nFrame Rate: ");
		sprintf(cTmp,"%d", pWMVDec->m_iFrameRate);
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);

		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nBit Rate: ");
		sprintf(cTmp,"%d", pWMVDec->m_iBitRate);
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);

#if !(defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
		if(pWMVDec->m_bSpriteMode || pWMVDec->m_bSpriteModeV2)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nSprite Mode: Yes");
		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nSprite Mode: No");
#endif
		if(pWMVDec->m_bMultiresEnabled == 0)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nMulti-Resolution: Disabled");
		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nMulti-Resolution: Enabled");

		if(pWMVDec->m_bYUV411 == 1)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nYUV411 Format: Yes");
	}
#endif

    return ICERR_OK;
}

#endif


#ifdef _WMV9AP_
tWMVDecodeStatus decodeVOLHead_WMV3 (tWMVDecInternalMember *pWMVDec, U32_WMV uiFormatLength, I32_WMV *piPIC_HORIZ_SIZE, I32_WMV *piPIC_VERT_SIZE , I32_WMV *piDISP_HORIZ_SIZE, I32_WMV *piDISP_VERT_SIZE)
#else
tWMVDecodeStatus decodeVOLHead_WMV3 (tWMVDecInternalMember *pWMVDec)
#endif
{
    // read WMV3 profile
#ifndef _WMV9AP_
    I32_WMV iProfile = BS_getBits(pWMVDec->m_pbitstrmIn, 2);                            // C0 00 00 00
    Bool_WMV bValidProfile = TRUE;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeVOLHead_WMV3);
#else

    I32_WMV iProfile;                         // C0 00 00 00
    Bool_WMV bValidProfile = TRUE;
    if (pWMVDec->m_cvCodecVersion == WMVA){
        I32_WMV iPrefix;
        I32_WMV result;
        //ignore start code prefix
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8); 
        if(iPrefix != 0) return WMV_Failed;
        assert(iPrefix == 0);
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8); 
        if(iPrefix != 0) return WMV_Failed;
        assert(iPrefix == 0);
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
        if(iPrefix != 1) return WMV_Failed;
        assert(iPrefix == 1);
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8); 
        if(iPrefix != SC_SEQ) return WMV_Failed;
        assert(iPrefix == SC_SEQ);


        result = decodeSequenceHead_Advanced(pWMVDec, uiFormatLength, piPIC_HORIZ_SIZE, piPIC_VERT_SIZE, piDISP_HORIZ_SIZE, piDISP_VERT_SIZE);
        if (result != ICERR_OK) 
            return WMV_Failed;
            
        if (pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc > pWMVDec->m_iMaxPrevSeqFrameArea) {
            //if max encoding size is greater than the display size in ASF header.
            pWMVDec->m_iMaxPrevSeqFrameArea = pWMVDec->m_iMaxFrmWidthSrc * pWMVDec->m_iMaxFrmHeightSrc;
            FreeFrameAreaDependentMemory(pWMVDec);
            result = initFrameAreaDependentMemory(pWMVDec, pWMVDec->m_iMaxFrmWidthSrc, pWMVDec->m_iMaxFrmHeightSrc);
            if (ICERR_OK != result)
                return WMV_Failed;
        }

        BS_flush (pWMVDec->m_pbitstrmIn);


        //ignore start code prefix
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
        assert(iPrefix == 0);
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
        assert(iPrefix == 0);
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
        assert(iPrefix == 1);
        iPrefix = BS_getBits(pWMVDec->m_pbitstrmIn, 8);  
        assert(iPrefix == SC_ENTRY);


        //DecodeEntryPointHeader(pWMVDec, NULL, NULL);

                //update buffer pointer if display size is different from encoding size.
        SwitchEntryPoint(pWMVDec);

        
#ifdef WMVA_NEW_ENTROPY_CODING
        if (pWMVDec->m_bEnableCBEntropyCoding) {
            //printf("Use new entropy coding.\n");
            I32_WMV result = InitTwoLayerEntropyDecoding(pWMVDec);
            if (result != ICERR_OK)  return result;
            result = InitTwoLayerEntropyDecodingHuffTables(pWMVDec);
            if (result != ICERR_OK)  return ICERR_MEMORY;
        }
#endif
        
        return WMV_Succeeded;
    }


    // read WMV3 profile

   // read WMV3 profile
    iProfile = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
    

#endif
    
    if (iProfile == 0) {
        pWMVDec->m_iWMV3Profile = WMV3_SIMPLE_PROFILE;
    } else if (iProfile == 1) {
        pWMVDec->m_iWMV3Profile = WMV3_MAIN_PROFILE;
    } else if (iProfile == 2) {
        pWMVDec->m_iWMV3Profile = WMV3_PC_PROFILE;
#ifdef _WMV9AP_
    }
#else
    } else
        bValidProfile = FALSE;

#   ifdef WMV9_SIMPLE_ONLY
        if(pWMVDec->m_iWMV3Profile != WMV3_SIMPLE_PROFILE)
            return WMV_UnSupportedCompressedFormat;
#   endif

#endif

    pWMVDec->m_bYUV411 = BS_getBits(pWMVDec->m_pbitstrmIn, 1);                // 20 00 00 00
    pWMVDec->m_bSpriteMode = BS_getBits(pWMVDec->m_pbitstrmIn,1);                       // 10 00 00 00
    
    pWMVDec->m_iFrameRate = BS_getBits(pWMVDec->m_pbitstrmIn,3);  // midpoint of bin    // 0E 00 00 00
    
    pWMVDec->m_iBitRate = BS_getBits(pWMVDec->m_pbitstrmIn,5);                          // 01 F0 00 00
    pWMVDec->m_iFrameRate = 4 * pWMVDec->m_iFrameRate + 2;  // midpoint of bin
    pWMVDec->m_iBitRate = 64 * pWMVDec->m_iBitRate + 32;
    
    pWMVDec->m_bRndCtrlOn = TRUE_WMV;
    pWMVDec->m_bLoopFilter = BS_getBits(pWMVDec->m_pbitstrmIn,1);                       // 00 08 00 00
    
#ifndef _WMV9AP_
    pWMVDec->m_bV9LoopFilter = pWMVDec->m_bLoopFilter;      
#endif
    
    pWMVDec->m_bXintra8Switch = BS_getBits(pWMVDec->m_pbitstrmIn,1);                    // 00 04 00 00
    
    pWMVDec->m_bMultiresEnabled = BS_getBits(pWMVDec->m_pbitstrmIn,1);                  // 00 02 00 00
    pWMVDec->m_b16bitXform = BS_getBits(pWMVDec->m_pbitstrmIn,1);                       // 00 01 00 00
    
    pWMVDec->m_bUVHpelBilinear = BS_getBits(pWMVDec->m_pbitstrmIn,1);                   // 00 00 80 00
    
#ifdef _WMV9AP_
    pWMVDec->m_bExtendedMvMode = BS_getBits(pWMVDec->m_pbitstrmIn,1); 
                 // 00 00 40 00
#else
    pWMVDec->m_bExtendedMvMode = BS_getBits(pWMVDec->m_pbitstrmIn,1);                    // 00 00 40 00
#endif
    pWMVDec->m_iDQuantCodingOn = BS_getBits(pWMVDec->m_pbitstrmIn,2);                   // 00 00 30 00
    
    // common to main, simple, interlace
    pWMVDec->m_bXformSwitch = BS_getBits(pWMVDec->m_pbitstrmIn,1);                      // 00 00 08 00
    pWMVDec->m_bDCTTable_MB_ENABLED = BS_getBits(pWMVDec->m_pbitstrmIn,1);              // 00 00 04 00    
    pWMVDec->m_bSequenceOverlap = BS_getBits( pWMVDec->m_pbitstrmIn, 1);                // 00 00 02 00
    pWMVDec->m_bStartCode = BS_getBits(pWMVDec->m_pbitstrmIn, 1);                       // 00 00 01 00      
    pWMVDec->m_bPreProcRange = BS_getBits( pWMVDec->m_pbitstrmIn, 1);                   // 00 00 00 80
    
    pWMVDec->m_iNumBFrames = BS_getBits( pWMVDec->m_pbitstrmIn, 3);                     // 00 00 00 70
    pWMVDec->m_bExplicitSeqQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);            // 00 00 00 08
    if (pWMVDec->m_bExplicitSeqQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);           // 00 00 00 04
    else 
        pWMVDec->m_bExplicitFrameQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1);      // 00 00 00 04
    pWMVDec->m_bExplicitQuantizer = pWMVDec->m_bExplicitSeqQuantizer || pWMVDec->m_bExplicitFrameQuantizer;
    
    pWMVDec->m_bSeqFrameInterpolation = BS_getBits(pWMVDec->m_pbitstrmIn, 1);           // 00 00 00 02
 
#ifdef WIN32_DEG
	{
		I8_WMV cTmp[60];
		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nProfile: ");
		if (iProfile == 0) {
			sprintf(cTmp,"Simple Profile(SP)");
		} else if (iProfile == 1) {
			sprintf(cTmp,"Main Profile(MP)");
		} else if (iProfile == 2) {
			sprintf(cTmp,"PC Profile(no feature restrictions for complex profile)");
		}
		else {
			sprintf(cTmp,"Unknown");
		}
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);

		if(pWMVDec->m_iNumBFrames == 1)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nB frame: Contained");
		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nB frame: Not Contained");

		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nFrame Rate: ");
		sprintf(cTmp,"%d", pWMVDec->m_iFrameRate);
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);

		strcat(pWMVDec->m_SeqHeadInfoString, "\r\nBit Rate: ");
		sprintf(cTmp,"%d", pWMVDec->m_iBitRate);
		strcat(pWMVDec->m_SeqHeadInfoString, cTmp);

#if !(defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
		if(pWMVDec->m_bSpriteMode || pWMVDec->m_bSpriteModeV2)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nSprite Mode: Yes");
		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nSprite Mode: No");
#endif
		if(pWMVDec->m_bMultiresEnabled == 0)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nMulti-Resolution: Disabled");
		else strcat(pWMVDec->m_SeqHeadInfoString, "\r\nMulti-Resolution: Enabled");

		if(pWMVDec->m_bYUV411 == 1)	strcat(pWMVDec->m_SeqHeadInfoString, "\r\nYUV411 Format: Yes");
	}
#endif

    // Verify Profile

#ifndef _WMV9AP_
#   ifdef WMV9_SIMPLE_ONLY
            bValidProfile = (pWMVDec->m_bXintra8Switch == FALSE) &&
                (pWMVDec->m_b16bitXform == TRUE) &&
                (pWMVDec->m_bUVHpelBilinear == TRUE) &&
                (pWMVDec->m_bStartCode == FALSE) &&
                (pWMVDec->m_bExtendedMvMode == FALSE) &&
                (pWMVDec->m_bV9LoopFilter == FALSE) &&
                (pWMVDec->m_bYUV411 == FALSE) &&
                (pWMVDec->m_bMultiresEnabled == FALSE) &&
                (pWMVDec->m_iDQuantCodingOn == 0) &&
                (pWMVDec->m_iNumBFrames == 0) &&
                (pWMVDec->m_bPreProcRange == FALSE);

        if (!bValidProfile)
            return WMV_UnSupportedCompressedFormat;
#   endif
    
#endif

    if (!pWMVDec->m_bSpriteMode) {
        if (pWMVDec->m_iWMV3Profile == WMV3_SIMPLE_PROFILE) {
            bValidProfile = (pWMVDec->m_bXintra8Switch == FALSE) &&
                (pWMVDec->m_b16bitXform == TRUE) &&
                (pWMVDec->m_bUVHpelBilinear == TRUE) &&
                (pWMVDec->m_bStartCode == FALSE) &&
#ifdef _WMV9AP_
               // (pWMVDec->m_bExtendedMvMode == FALSE) &&
                (pWMVDec->m_bLoopFilter == FALSE) &&
#else
                (pWMVDec->m_bExtendedMvMode == FALSE) &&
                (pWMVDec->m_bV9LoopFilter == FALSE) &&
#endif
                (pWMVDec->m_bYUV411 == FALSE) &&
                (pWMVDec->m_bMultiresEnabled == FALSE) &&
                (pWMVDec->m_iDQuantCodingOn == 0) &&
                (pWMVDec->m_iNumBFrames == 0) &&
                (pWMVDec->m_bPreProcRange == FALSE);
            
        } else if (pWMVDec->m_iWMV3Profile == WMV3_MAIN_PROFILE) {
            bValidProfile = (pWMVDec->m_bXintra8Switch == FALSE) &&
                (pWMVDec->m_b16bitXform == TRUE);
        } else if (pWMVDec->m_iWMV3Profile == WMV3_PC_PROFILE) {
            // no feature restrictions for complex profile.
        }
        
        if (!bValidProfile) {
#ifdef WIN32_DEG
			strcat(pWMVDec->m_SeqHeadInfoString, "\r\n*** Error: Invalid profile! There are corrupted bits with file or some features don't match with the profile.***");
#endif
            return WMV_UnSupportedCompressedFormat;
        }
    }
    
#   ifdef PPC_SUPPORT_PROFILES
        if (WMV3_SIMPLE_PROFILE == pWMVDec->m_iWMV3Profile) {
            PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,176,144,WMVSupportWMV9SPLL)
            PPC_TEST_SUPPORT_PROFILE(WMVSupportWMV9SPML);
        } else if (WMV3_MAIN_PROFILE == pWMVDec->m_iWMV3Profile) {
            if ( pWMVDec->m_bInterlaceCodingOn ) {
                PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,720,576,WMVSupportWMV9MPMLI)
                PPC_TEST_SUPPORT_PROFILE(WMVSupportWMV9MPHLI);
            } else if ( pWMVDec->m_iNumBFrames > 0 ) {
                PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,352,288,WMVSupportWMV9MPLLB)
                PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,720,576,WMVSupportWMV9MPMLB)
                PPC_TEST_SUPPORT_PROFILE(WMVSupportWMV9MPHLB);
            } else {
                PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,352,288,WMVSupportWMV9MPLL)
                PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,720,576,WMVSupportWMV9MPML)
                PPC_TEST_SUPPORT_PROFILE(WMVSupportWMV9MPHL);
            }
        } else if (WMV3_PC_PROFILE == pWMVDec->m_iWMV3Profile) {
            PPC_TEST_SUPPORT_PROFILE(WMVSupportWMV9PC);
        } 
#   endif

    pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iNumBFrames];
    
    //SetProfileVariablesWMV3 (pWMVDec);
    if (pWMVDec->m_bYUV411)
        pWMVDec->m_bXintra8Switch = pWMVDec->m_bXintra8 = FALSE;
    
    SetMVRangeFlag (pWMVDec, 0);
    
    // old/unused stuff
    pWMVDec->m_bMixedPel = pWMVDec->m_bFrmHybridMVOn = FALSE_WMV;
    
    // may have to override transform function pointers
    if (pWMVDec->m_b16bitXform) {
#ifdef _WMV9AP_
        m_InitIDCT_Dec (pWMVDec, TRUE);
        decideMMXRoutines (pWMVDec);
        m_InitFncPtrAndZigzag(pWMVDec);
#ifndef WMV9_SIMPLE_ONLY
		pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;
#endif
#else
        m_InitIDCT_Dec (pWMVDec,TRUE);
        m_InitFncPtrAndZigzag(pWMVDec);
        decideMMXRoutines (pWMVDec);
#endif
        
        // IW pWMVDec->m_pSp->m_pIDCT_Dec = pWMVDec->m_pIntraX9IDCT_Dec;
        pWMVDec->m_pDecodeInverseInterBlockQuantize = DecodeInverseInterBlockQuantize16;
    }
    
    if (pWMVDec->m_bSpriteMode) {
        if(!pWMVDec->m_bXintra8Switch   &&
            !pWMVDec->m_bDCTTable_MB_ENABLED  &&
            !pWMVDec->m_bYUV411 && 
#ifdef _WMV9AP_
            !pWMVDec->m_bLoopFilter &&
            !pWMVDec->m_bExtendedMvMode &&
#else
            !pWMVDec->m_bV9LoopFilter &&
            !pWMVDec->m_bExtendedMvMode &&
#endif
            !pWMVDec->m_bMultiresEnabled &&
            //pWMVDec->m_b16bitXform &&
            !pWMVDec->m_bUVHpelBilinear &&
            !pWMVDec->m_iDQuantCodingOn &&
            !pWMVDec->m_bXformSwitch &&
            !pWMVDec->m_bStartCode &&
            !pWMVDec->m_bPreProcRange &&
            !pWMVDec->m_bExplicitSeqQuantizer &&
            !pWMVDec->m_bUse3QPDZQuantizer &&
            !pWMVDec->m_bExplicitFrameQuantizer)
            return decodeSpriteVOLHead (pWMVDec);
        else
            return WMV_Failed;
    }
    
    // The following bit determines which encoder was used to generate the content:
    // 0: Beta 1: RTM. Due to bugs, each version produces an incompatible
    // bitstream. Parsing this value allows the decoder to properly decode the corresponding version.
    // NOTE: default at this point is: m_bBetaContent = TRUE, m_bRTMContent = FALSE
    // Depending on the detected encoder version, these values are overidden as follows:
    pWMVDec->m_iBetaRTMMismatchIndex = 1;
    
    if (BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)== 1 && !BS_invalid(pWMVDec->m_pbitstrmIn)) {    // 00 00 00 01
        // RTM content
        pWMVDec->m_bRTMContent = TRUE;
        pWMVDec->m_bBetaContent = FALSE;
        pWMVDec->m_iBetaRTMMismatchIndex = 0;
    }
    COVERAGE_SEQUENCE(pWMVDec);

#ifdef _WMV9AP_
        if (uiFormatLength == 5){
        Bool_WMV bVCMInfoPresent;

        pWMVDec->m_bPostProcInfoPresent = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        // VCM bit (34th bit) is reserved for VCM contents if VOL size > 4 bytes.
        bVCMInfoPresent = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
    }

#endif
    return WMV_Succeeded;
}

I32_WMV decodeVOPHeadProgressiveWMVA (tWMVDecInternalMember *pWMVDec)    
{
    I32_WMV stepDecoded;
    I32_WMV iMVRangeIndex = 0;
    I32_WMV iNumWindowsPresent = 0;
    CPanScanInfo sPanScanInfo;

    DecodeVOPType_WMVA (pWMVDec);

    if (pWMVDec->m_tFrmType == SKIPFRAME) {
        if (pWMVDec->m_bBroadcastFlags){
            if (pWMVDec->m_bInterlacedSource) {
                pWMVDec->m_bTopFieldFirst = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                pWMVDec->m_bRepeatFirstField = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            }
            else// Progressive source
                pWMVDec->m_iRepeatFrameCount = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
        }
        // Pan-scan parameters
        if (pWMVDec->m_bPanScanPresent) {
            DecodePanScanInfo (pWMVDec, &sPanScanInfo, &iNumWindowsPresent);
        }

        return ICERR_OK;
    }

    if (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != PVOP && pWMVDec->m_tFrmType != BIVOP && pWMVDec->m_tFrmType != BVOP)
        return ICERR_ERROR;

    if (pWMVDec->m_bTemporalFrmCntr)
    {
        /*I32_WMV iTime = */BS_getBits( pWMVDec->m_pbitstrmIn, 8);  //zou  -->VC1_BITS_TFCNTR
    }

    if (pWMVDec->m_bBroadcastFlags){
        if (pWMVDec->m_bInterlacedSource) {
            pWMVDec->m_bTopFieldFirst = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            pWMVDec->m_bRepeatFirstField = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
#ifdef DUMPLOG
            {
            FILE *fLog = fopen("c:/wmva.log","a");
            fprintf(fLog, "TFF %d RFF %d\n", m_bTopFieldFirst, m_bRepeatFirstField);
            fclose(fLog);
            }
#endif
        }
        else // Progressive source
        {
            pWMVDec->m_iRepeatFrameCount = BS_getBits( pWMVDec->m_pbitstrmIn, 2);
#ifdef DUMPLOG
            {
            FILE *fLog = fopen("c:/wmva.log","a");
            fprintf(fLog, "RFC %d\n", m_iRepeatFrameCount);
            fclose(fLog);
            }
#endif
        }
	}

 // Pan-scan parameters
    if (pWMVDec->m_bPanScanPresent) {
        DecodePanScanInfo (pWMVDec, &sPanScanInfo, &iNumWindowsPresent);
    }

    pWMVDec->m_iRndCtrl = BS_getBits( pWMVDec->m_pbitstrmIn, 1);

    if (pWMVDec->m_bInterlacedSource)
    {
        pWMVDec->m_bProgressive420 = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
#ifdef DUMPLOG
        {
        if(pWMVDec->m_pFileLog) {
        fprintf(pWMVDec->m_pFileLog, "UVSAMP %d\n", pWMVDec->m_bProgressive420);
        }
        }
#endif
    }


    if (pWMVDec->m_bSeqFrameInterpolation)
        pWMVDec->m_bInterpolateCurrentFrame = BS_getBits( pWMVDec->m_pbitstrmIn, 1 );   

    // Decode B frac
    if (pWMVDec->m_tFrmType == BVOP)
    {
        I32_WMV iShort = 0, iLong = 0;
        iShort = BS_getBits( pWMVDec->m_pbitstrmIn,  3);
        if (iShort == 0x7)
        {
            iLong = BS_getBits( pWMVDec->m_pbitstrmIn,  4);

            if (iLong == 0xe)       // "hole" in VLC
                return ICERR_ERROR;

#ifndef WMV9_SIMPLE_ONLY
            DecodeFrac(pWMVDec, iLong + 112, TRUE);
#endif
        }
#ifndef WMV9_SIMPLE_ONLY
        else
            DecodeFrac(pWMVDec, iShort, FALSE);
#endif
    }

   
// Check to see if this supposed to be in WMVA
//    if (pWMVDec->m_cvCodecVersion >= WMV2 && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)) {
//        I32_WMV iBufferFullPercent = BS_getBits( pWMVDec->m_pbitstrmIn, 7);
//        if (BS_invalid(pWMVDec->m_pbitstrmIn))
//            return ICERR_ERROR;
//    }
    
    stepDecoded = BS_getBits( pWMVDec->m_pbitstrmIn, NUMBITS_VOP_QUANTIZER);  //zou --->VC1_BITS_PQINDEX
    
    pWMVDec->m_iQPIndex = stepDecoded;

    if (stepDecoded <= MAXHALFQP)
        pWMVDec->m_bHalfStep = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
    else
        pWMVDec->m_bHalfStep = FALSE;

    if (pWMVDec->m_bExplicitFrameQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
    
    if (!pWMVDec->m_bExplicitQuantizer) { // Implicit quantizer   //zou difference
        if (pWMVDec->m_iQPIndex <= MAX3QP) {
            pWMVDec->m_bUse3QPDZQuantizer = TRUE;
            stepDecoded = pWMVDec->m_iQPIndex;
        }
        else {
            pWMVDec->m_bUse3QPDZQuantizer = FALSE;
            stepDecoded = g_iStepRemap[pWMVDec->m_iQPIndex - 1];
        }
    }
    else // Explicit quantizer
        stepDecoded = pWMVDec->m_iQPIndex;

    pWMVDec->m_iStepSize = stepDecoded;

    pWMVDec->m_iOverlap = 0;
    if (pWMVDec->m_bSequenceOverlap && pWMVDec->m_tFrmType != BVOP) {
        if (pWMVDec->m_iStepSize >= 9)
            pWMVDec->m_iOverlap = 1;
        else if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)
            pWMVDec->m_iOverlap = 7;
        // pWMVDec->m_iOverlap last 3 bits: [MB switch=1/frame switch=0][sent=1/implied=0][on=1/off=0]
    }

    if (pWMVDec->m_bPostProcInfoPresent)
        pWMVDec->m_iPostProcLevel = BS_getBits( pWMVDec->m_pbitstrmIn, 2);   //zou -->vc1_PostProcessingNone

    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        // AC Pred flag
        if (DecodeSkipBitX9 (pWMVDec, 4) == ICERR_ERROR) {  //zou --->eResult = vc1DECBITPL_ReadBitplane(pState, &pParams->sBPPredictAC, pBitstream);
            return ICERR_ERROR;
        }
        if (pWMVDec->m_iACPredIFrameCodingMode != SKIP_RAW) {
            I32_WMV i;
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                pWMVDec->m_rgmbmd[i].m_rgbDCTCoefPredPattern2 [0] = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE 
                    : FALSE);
            }
        }

        if (pWMVDec->m_iOverlap & 2) {
            if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0) // off
                pWMVDec->m_iOverlap = 0;
            else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0) // on
                pWMVDec->m_iOverlap = 1;
            else {// MB switch - decode bitplane
                if (DecodeSkipBitX9 (pWMVDec, 5) == ICERR_ERROR) 
                    return ICERR_ERROR;   
                if (pWMVDec->m_iOverlapIMBCodingMode != SKIP_RAW) {
                    I32_WMV i;
                    for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                        pWMVDec->m_rgmbmd[i].m_bOverlapIMB = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE : FALSE); 
                    }
                }
            }
        }
    }

    if (pWMVDec->m_iOverlap & 1) {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 0;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 0;
    }
    else {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;
    }

    pWMVDec->m_prgDQuantParam = pWMVDec->m_bUse3QPDZQuantizer ? pWMVDec->m_rgDQuantParam3QPDeadzone : pWMVDec->m_rgDQuantParam5QPDeadzone;

    //g_iStepsize = pWMVDec->m_iQPIndex * 2 + pWMVDec->m_bHalfStep;
    
    pWMVDec->m_iStepSize = stepDecoded;
    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }


    pWMVDec->m_iStepSize = stepDecoded;
    if (stepDecoded <= 0 || stepDecoded > 31)
            return ICERR_ERROR;

    /*
    if (pWMVDec->m_tFrmType == PVOP || pWMVDec->m_tFrmType == BVOP) {
        pWMVDec->m_vopmd.intStep = stepDecoded;
        if (pWMVDec->m_vopmd.intStep <= 0 || pWMVDec->m_vopmd.intStep > 31)
            return ICERR_ERROR;
    }
    else if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        pWMVDec->m_vopmd.intStepI = stepDecoded;
        if (pWMVDec->m_vopmd.intStepI <= 0 || pWMVDec->m_vopmd.intStepI > 31)
            return ICERR_ERROR;
    }
    else {
        BOE;
        return ICERR_UNSUPPORTED;
    }
    */

    if (pWMVDec->m_iQPIndex <= MAXHIGHRATEQP) {
        // If QP (picture quant) is <= MAXQP_3QPDEADZONE then we substitute the highrate
        // inter coeff table for the talking head table among the 3 possible tables that
        // can be used to code the coeff data
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_HghRate;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_HghRate;
    }
    
	if (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != BIVOP) {
		if (pWMVDec->m_bExtendedMvMode) {
			iMVRangeIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
			if (iMVRangeIndex)
				iMVRangeIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
			if (iMVRangeIndex == 2)
				iMVRangeIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        
		}
	}

    SetMVRangeFlag (pWMVDec, iMVRangeIndex);

    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        I32_WMV result = setSliceCode (pWMVDec, pWMVDec->m_iSliceCode);
        //I32_WMV iSliceCode = BS_getBits( pWMVDec->m_pbitstrmIn, NUMBITS_SLICE_SIZE);
        if (result == ICERR_ERROR) {
            return ICERR_ERROR;
        }
      //  result = DecodeClipInfo (pWMVDec);
        if(BS_invalid(pWMVDec->m_pbitstrmIn) || result != ICERR_OK) {
            
            return ICERR_ERROR;
        }
        pWMVDec->m_bDCPred_IMBInPFrame = FALSE;
 
        // If pWMVDec->m_bDCTTable_MB_ENABLED is on
        //if (pWMVDec->m_bDCTTable_MB_ENABLED){
        //    pWMVDec->m_bDCTTable_MB = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        //}

            
        // DCT Table swtiching, I and P index are coded separately.
        // Can be jointly coded using the following table. 
        // IP Index : Code
        // 00       : 00, 
        // 11       : 01, 
        // 01       : 100,
        // 10       : 101,
        // 02       : 1100,
        // 12       : 1101,
        // 20       : 1110, 
        // 21       : 11110
        // 22       : 11111
        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_rgiDCTACIntraTableIndx[0]){
            pWMVDec->m_rgiDCTACIntraTableIndx[0] += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = pWMVDec->m_rgiDCTACIntraTableIndx[0];
            
        pWMVDec->m_iIntraDCTDCTable = BS_getBits( pWMVDec->m_pbitstrmIn, 1);

        // I Frame Dquant
        if (pWMVDec->m_iDQuantCodingOn)
            DecodeVOPDQuant (pWMVDec, FALSE);
        else
            SetDefaultDQuantSetting(pWMVDec);
        
        
    }
    else {
        I32_WMV result;
        result = decodeVOPHead_WMV3(pWMVDec);

        if (result == ICERR_ERROR)
            return ICERR_ERROR;
        
        //if (pWMVDec->m_bDCTTable_MB_ENABLED){
        //    pWMVDec->m_bDCTTable_MB = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        //}

        //if (!pWMVDec->m_bDCTTable_MB){
        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits( pWMVDec->m_pbitstrmIn, 1);  //zou --->/* Frame level transform AC coding set index */
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits( pWMVDec->m_pbitstrmIn, 1);
            //   }
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = 
        pWMVDec->m_rgiDCTACInterTableIndx[1] = pWMVDec->m_rgiDCTACInterTableIndx[2] = pWMVDec->m_rgiDCTACInterTableIndx[0];
        pWMVDec->m_iIntraDCTDCTable = BS_getBits( pWMVDec->m_pbitstrmIn, 1);   //zou -->  /* Intra transform DC table */

     //   if (pWMVDec->m_tFrmType == PVOP && !pWMVDec->m_bFieldMode)
     //       pWMVDec->m_iRndCtrl ^= 0x01;            
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    return ICERR_OK;
}

I32_WMV DecodePanScanInfo (tWMVDecInternalMember *pWMVDec, CPanScanInfo *pPanScanInfo, I32_WMV *piNumWindowsPresent)
{
    I32_WMV iNumWindows = 0;
    I32_WMV i;

    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
        if (pWMVDec->m_bInterlacedSource) {
            if (pWMVDec->m_bBroadcastFlags)
                iNumWindows = 2 + pWMVDec->m_bRepeatFirstField;
            else
                iNumWindows = 2;      
        }
        else {
            if (pWMVDec->m_bBroadcastFlags)
                iNumWindows = 1 + pWMVDec->m_iRepeatFrameCount;
            else
                iNumWindows = 1; 
        }
        for (i = 0; i < iNumWindows; i++) {
            I32_WMV iBits;
            iBits = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            pPanScanInfo->sWindowInfo[i].iHorizOffset = (iBits << 2) | BS_getBits(pWMVDec->m_pbitstrmIn, 2);
            iBits = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            pPanScanInfo->sWindowInfo[i].iVertOffset = (iBits << 2) | BS_getBits(pWMVDec->m_pbitstrmIn, 2);
            pPanScanInfo->sWindowInfo[i].iWidth = BS_getBits(pWMVDec->m_pbitstrmIn, 14);
            pPanScanInfo->sWindowInfo[i].iHeight = BS_getBits(pWMVDec->m_pbitstrmIn, 14);
#ifdef DUMPLOG
		    if(pWMVDec->m_pFileLog) {
                fprintf(pWMVDec->m_pFileLog, "Pan[%d] %d %d %d %d\n", 
                    i,
                    pPanScanInfo->sWindowInfo[i].iHorizOffset,
                    pPanScanInfo->sWindowInfo[i].iVertOffset,
                    pPanScanInfo->sWindowInfo[i].iWidth,
                    pPanScanInfo->sWindowInfo[i].iHeight
                );
		    }
#endif

        }
    }

    *piNumWindowsPresent = iNumWindows;
    return ICERR_OK;
}

Void_WMV DecodeVOPType_WMVA (tWMVDecInternalMember *pWMVDec)
{
    
    // I        : 110
    // P        : 0
    // B        : 10
    // BI       : 1110
    // Skipped  : 1111
    
        if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = PVOP;
        else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = BVOP;
        else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = IVOP;
        else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
            pWMVDec->m_tFrmType = BIVOP;
        else
        {
            pWMVDec->m_tFrmType = SKIPFRAME;
            pWMVDec->m_iDroppedPFrame = 1;
            PreGetOutput(pWMVDec);
        }
}

Void_WMV DecodeVOPType_WMVA2 (tWMVDecInternalMember *pWMVDec)
{
    
    // I        : 110
    // P        : 0
    // B        : 10
    // BI       : 1110
    // Skipped  : 1111
    
    if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = PVOP;
    else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = BVOP;
    else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = IVOP;
    else if (BS_getBits( pWMVDec->m_pbitstrmIn, 1 ) == 0)
        pWMVDec->m_tFrmType = BIVOP;
    else
        pWMVDec->m_tFrmType = SKIPFRAME;
}


//check zigzag scan arrays before each frame
//used only in WMVA, where interlace and progressive modes have different scan arrays,
Void_WMV decideMMXRoutines_ZigzagOnly (tWMVDecInternalMember *pWMVDec)
{
    pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInv;
    pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInv;
    pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInv;
    pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInv;

    if (pWMVDec->m_b16bitXform)
    {
        pWMVDec->m_bRotatedIdct = 1;
        pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
        pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInvRotated;
        pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInvRotated;        
        pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInvRotated;   
    } 

#if defined(WMV_TARGET_X86) || defined(USE_AUTO_GENERATED_INTRINSICS)
    if (g_SupportMMX ()) {
        //All the New (C & MMX) IDCT's now use the transposed matrix.
        pWMVDec->m_bRotatedIdct = 1;
        pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
    }
#endif // WMA_TARGET_X86 

#if defined(_Embedded_x86)
    if (g_SupportMMX ()) {
#ifdef USE_NEW_MMX_IDCT
        pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
        if (pWMVDec->m_b16bitXform)
        {
            pWMVDec->m_p8x4ZigzagScanOrder = pWMVDec->m_p8x4ZigzagInvRotated;
            pWMVDec->m_p4x8ZigzagScanOrder = pWMVDec->m_p4x8ZigzagInvRotated;        
            pWMVDec->m_p4x4ZigzagScanOrder = pWMVDec->m_p4x4ZigzagInvRotated;        
        } 
#else        
        if (!pWMVDec->m_b16bitXform)
            pWMVDec->m_pZigzagScanOrder = pWMVDec->m_pZigzagInvRotated;
#endif
    }
#endif // _Embedded_x86
}



Void_WMV decodeVOPHead_WMV2 (tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeVOPHead_WMV2);
#   ifndef WMV9_SIMPLE_ONLY
        if (!pWMVDec->m_bSKIPBIT_CODING_){
            pWMVDec->m_bCODFlagOn = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    }
    else{
        I32_WMV iSkipBitCode = BS_getBits  ( pWMVDec->m_pbitstrmIn, 2);
        pWMVDec->m_bCODFlagOn = 1;
        if (iSkipBitCode == 0) {
            pWMVDec->m_bCODFlagOn = 0;
        } else {
            if (iSkipBitCode == 1) {
                pWMVDec->m_SkipBitCodingMode = Normal;
            } else if (iSkipBitCode == 2) {
                pWMVDec->m_SkipBitCodingMode = RowPredict;
            } else {
                pWMVDec->m_SkipBitCodingMode = ColPredict;
            }
            DecodeSkipBit(pWMVDec);
        }
    }
    
    // NEW_PCBPCY_TABLE
    if (pWMVDec->m_bNEW_PCBPCY_TABLE){
        if (pWMVDec->m_iStepSize <= 10) {
            if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //0 High
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_HighRate;
            } else if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //10 Low
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_LowRate;
            } else { //11 Mid
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_MidRate;
            }
        } else if (pWMVDec->m_iStepSize <= 20) {
            if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //0 Mid
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_MidRate;
            } else if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //10 High
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_HighRate;
            } else { //11 Low
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_LowRate;
            }
        } else {
            if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //0 Low
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_LowRate;
            } else if (!BS_getBits  ( pWMVDec->m_pbitstrmIn, 1)) { //10 Mid
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_MidRate;
            } else { //11 High
                pWMVDec->m_pHufNewPCBPCYDec = &pWMVDec->m_hufPCBPCYDec_HighRate;
            }
        }
    }
    
    //_MIXEDPEL_
    if (pWMVDec->m_bMixedPel)
        pWMVDec->m_iMvResolution = BS_getBits  ( pWMVDec->m_pbitstrmIn, 1);
    
    if (pWMVDec->m_bXformSwitch) {
        if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
        {
            pWMVDec->m_bMBXformSwitching = FALSE_WMV;
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                pWMVDec->m_iFrameXformMode = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                pWMVDec->m_iFrameXformMode = XFORMMODE_8x4;
            else
                pWMVDec->m_iFrameXformMode = XFORMMODE_4x8;
        }
        else
            pWMVDec->m_bMBXformSwitching = TRUE_WMV;
    }
#endif    
}


Void_WMV setRefreshPeriod(tWMVDecInternalMember *pWMVDec) 
{
#   ifdef _SUPPORT_POST_FILTERS_
        I32_WMV iWidthTimesHeight  = pWMVDec->m_iWidthY * pWMVDec->m_iHeightY;
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(setRefreshPeriod);
        if (pWMVDec->m_iBitRate > 300 || iWidthTimesHeight > 320 * 240)
            pWMVDec->m_bRefreshDisplay_AllMB_Enable = FALSE_WMV;
        else if (iWidthTimesHeight > 240 * 176) {
#           ifdef  _WMV_TARGET_X86_
                if (g_bSupportMMX_WMV)
                //pWMVDec->m_bRefreshDisplay_AllMB_Enable = FALSE_WMV;
                pWMVDec->m_iRefreshDisplay_AllMB_Period = 2;
            else
#           endif //  _WMV_TARGET_X86_
            pWMVDec->m_iRefreshDisplay_AllMB_Period = 4;
            //pWMVDec->m_iRefreshDisplay_AllMB_Period = 3;
        }
        else {
            pWMVDec->m_iRefreshDisplay_AllMB_Period = 1;
        }
#   endif
}


Void_WMV SwapRefAndPost(tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SwapRefAndPost);

    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmRef0Q;
    pWMVDec->m_pfrmRef0Q = pTmp;

    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;

    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
}


Void_WMV SwapCurrAndPost(tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SwapCurrAndPost);

    pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmCurrQ;
    pWMVDec->m_pfrmCurrQ = pTmp;

    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;

    pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
    pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
    pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
}


#ifndef WMV9_SIMPLE_ONLY
    Void_WMV SwapMultiresAndPost(tWMVDecInternalMember *pWMVDec)
    {
        tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmPostQ;
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SwapMultiresAndPost);

        pWMVDec->m_pfrmPostQ = pWMVDec->m_pfrmMultiresQ;
        pWMVDec->m_pfrmMultiresQ = pTmp;

        pWMVDec->m_ppxliMultiresY = pWMVDec->m_pfrmMultiresQ->m_pucYPlane;
        pWMVDec->m_ppxliMultiresU = pWMVDec->m_pfrmMultiresQ->m_pucUPlane;
        pWMVDec->m_ppxliMultiresV = pWMVDec->m_pfrmMultiresQ->m_pucVPlane;

        pWMVDec->m_ppxliPostQY = pWMVDec->m_pfrmPostQ->m_pucYPlane ;
        pWMVDec->m_ppxliPostQU = pWMVDec->m_pfrmPostQ->m_pucUPlane ;
        pWMVDec->m_ppxliPostQV = pWMVDec->m_pfrmPostQ->m_pucVPlane;
    }
#endif // WMV9_SIMPLE_ONLY


#ifndef WMV9_SIMPLE_ONLY
#ifndef _WMV9AP_
    I32_WMV g_iNumShortVLC[] = {1, 1, 2, 1, 3, 1, 2};
    I32_WMV g_iDenShortVLC[] = {2, 3, 3, 4, 4, 5, 5};
    I32_WMV g_iNumLongVLC[] =  {3, 4, 1, 5, 1, 2, 3, 4, 5, 6, 1, 3, 5, 7};
    I32_WMV g_iDenLongVLC[] =  {5, 5, 6, 6, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8};
#endif
    extern const I32_WMV g_iBInverse[];

    Void_WMV DecodeFrac(tWMVDecInternalMember *pWMVDec, const I32_WMV iVal, const Bool_WMV bLong)
    {
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeFrac);
        if (bLong)
        {
            assert (iVal > 111 && iVal < 126);
            pWMVDec->m_iBNumerator   = g_iNumLongVLC[iVal - 112];
            pWMVDec->m_iBDenominator = g_iDenLongVLC[iVal - 112];
        }
        else
        {
            assert (iVal >= 0 && iVal < 7);
            pWMVDec->m_iBNumerator   = g_iNumShortVLC[iVal];
            pWMVDec->m_iBDenominator = g_iDenShortVLC[iVal];
        }

        pWMVDec->m_iBFrameReciprocal = g_iBInverse[pWMVDec->m_iBDenominator - 1]; // this is messy, but
                                                                // seems like "the" place
                                                                // to reset this
    }
#endif //WMV9_SIMPLE_ONLY

/****************************************************************************************************
  decodeSkipP : callback for doing the right things when skipped P is encountered
****************************************************************************************************/

tWMVDecodeStatus decodeSkipP (tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeSkipP);
    // Can't do anything unless we've seen an I-Picture go by
    if (!pWMVDec->m_fDecodedI) {
        return WMV_Failed;
    }
        
#ifdef _WMV9AP_
    pWMVDec->m_tFrmType  = pWMVDec->m_PredTypePrev = PVOP;      // render the last P frame, this is the right time
#else
    pWMVDec->m_tFrmType  = pWMVDec->m_tFrmPredType = PVOP;      // render the last P frame, this is the right time
#endif
    pWMVDec->m_iDroppedPFrame = 1;       // need this for rendering - buffer swaps
    
    return WMV_Succeeded;
}

/****************************************************************************************************
  CopyRefToPrevPost : backup contents of Ref0 to prev post (for rendering)
****************************************************************************************************/

#ifndef WMV9_SIMPLE_ONLY
    Void_WMV CopyRefToPrevPost (tWMVDecInternalMember *pWMVDec)
    {
        if (pWMVDec->m_bPrevRefInPrevPost || pWMVDec->m_bRenderFromPostBuffer)
            return; // ref already in prev post
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(CopyRefToPrevPost);
        // Copy the reference frame because the decode might change the reference frame and we may use the unchanged for output.. 
         pWMVDec->m_bPrevRefInPrevPost = TRUE;

        if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bOutputOpEnabled == FALSE_WMV)
        {
            RF_CpyCtl * pCopyCtl = (RF_CpyCtl *)&pWMVDec->m_pfrmPostPrevQ->m_pOps[pWMVDec->m_pfrmPostPrevQ->m_iNumOps++];

            assert(pWMVDec->m_pfrmPostPrevQ->m_iNumOps == 1);

          //  pCopyCtl->m_iOpCode  = FRMOP_COPY_REF2PREVPOST;

            pCopyCtl->m_iOpCode  = FRMOP_COPY_CURR2POST;

            pCopyCtl->m_pfrmSrc = pWMVDec->m_pfrmRef0Q;



             pCopyCtl->m_iWidthPrevY = pWMVDec->m_rMultiResParams[0].iWidthPrevY ;
            pCopyCtl->m_iWidthPrevUV = pWMVDec->m_rMultiResParams[0].iWidthPrevUV;
           // pCopyCtl->m_iHeightUV = pWMVDec->m_iHeightUV;

            pCopyCtl->m_uintNumMBY = pWMVDec->m_rMultiResParams[0].uiNumMBY;

            pCopyCtl->m_iWidthPrevYXExpPlusExp = pWMVDec->m_rMultiResParams[0].iWidthPrevYXExpPlusExp;
            pCopyCtl->m_iWidthPrevUVXExpPlusExp = pWMVDec->m_rMultiResParams[0].iWidthPrevUVXExpPlusExp;

            return;
        }
   
        ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliPostPrevQY, pWMVDec->m_ppxliRef0Y, pWMVDec->m_rMultiResParams[0].iWidthPrevY * pWMVDec->m_rMultiResParams[0].iHeightPrevY);
        ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliPostPrevQU, pWMVDec->m_ppxliRef0U, (pWMVDec->m_rMultiResParams[0].iWidthPrevY * pWMVDec->m_rMultiResParams[0].iHeightPrevY) >> 2);
        ALIGNED32_FASTMEMCPY (pWMVDec->m_ppxliPostPrevQV, pWMVDec->m_ppxliRef0V, (pWMVDec->m_rMultiResParams[0].iWidthPrevY * pWMVDec->m_rMultiResParams[0].iHeightPrevY) >> 2);
        DEBUG_SHOW_FRAME_COPY_BUFFER(pWMVDec->m_ppxliPostPrevQY,pWMVDec->m_ppxliRef0Y);
    }

    Void_WMV HandleResolutionChange (tWMVDecInternalMember *pWMVDec)
    {
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(HandleResolutionChange);
        if (pWMVDec->m_iResIndex != pWMVDec->m_iResIndexPrev) {
        
            if (pWMVDec->m_iNumBFrames && pWMVDec->m_iResIndexPrev == 0)
                CopyRefToPrevPost (pWMVDec);
        
#ifdef _WMV9AP_
        if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {

              pWMVDec->m_ppxliCurrQPlusExpY0 = (U8_WMV*) pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
            pWMVDec->m_ppxliCurrQPlusExpU0 = (U8_WMV*) pWMVDec->m_ppxliCurrQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
            pWMVDec->m_ppxliCurrQPlusExpV0 = (U8_WMV*) pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

        }
        if (pWMVDec->m_iNumBFrames || pWMVDec->m_tFrmType == PVOP || pWMVDec->m_tFrmType == BVOP) {
#else
            if (pWMVDec->m_iNumBFrames || pWMVDec->m_tFrmPredType == PVOP) {
#endif
            
                I32_WMV iWidthPrevYXExpPlusExp = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevYXExpPlusExp;
                I32_WMV iWidthPrevUVXExpPlusExp = pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUVXExpPlusExp;
                if (pWMVDec->m_iResIndex > pWMVDec->m_iResIndexPrev) {
                    if (!pWMVDec->m_bMBAligned) {
                        // If the reference frame needs to be downsampled (due to a resolution change) then we
                        // need to pad the reference frame prior to downsampling if the downsampled horiz. or
                        // vert. Y dimension are not multiples of 16.
                        (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y, 0, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iHeightY, iWidthPrevYXExpPlusExp, TRUE, TRUE,
                            pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthY, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthYPlusExp,
#ifdef _WMV9AP_
                            pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevY, !pWMVDec->m_bInterlaceV2);
#else
                            pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevY);
#endif
                        (*g_pRepeatRef0UV) (pWMVDec->m_ppxliRef0U, pWMVDec->m_ppxliRef0V, 0, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iHeightUV, 
                            iWidthPrevUVXExpPlusExp, TRUE, TRUE, pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthUV,
                            pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthUVPlusExp,
#ifdef _WMV9AP_       
                                                    pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUV, !pWMVDec->m_bInterlaceV2);
#else
                            pWMVDec->m_rMultiResParams[pWMVDec->m_iResIndexPrev].iWidthPrevUV);
#endif
                    }
                
                    DownsampleRefFrame(pWMVDec,
                        pWMVDec->m_ppxliRef0Y + iWidthPrevYXExpPlusExp, 
                        pWMVDec->m_ppxliRef0U + iWidthPrevUVXExpPlusExp,
                        pWMVDec->m_ppxliRef0V + iWidthPrevUVXExpPlusExp,                    
                        pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
                        pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                        pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp);
                }
                else
                    UpsampleRefFrame(pWMVDec,
                        pWMVDec->m_ppxliRef0Y + iWidthPrevYXExpPlusExp, 
                        pWMVDec->m_ppxliRef0U + iWidthPrevUVXExpPlusExp,
                        pWMVDec->m_ppxliRef0V + iWidthPrevUVXExpPlusExp,                   
                        pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp,
                        pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                        pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp);
            
                memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);
                WMVideoDecSwapCurrAndRef (pWMVDec, FALSE);
            
                // Peform repeat pad for new resized reference frame
#               ifdef  _MultiThread_Decode_ 
                    DecodeMultiThreads (REPEATPAD);
#               else
                    FUNCTION_PROFILE_DECL_START(fpRep,DECODEREPEATPAD_PROFILE)
#ifdef _WMV9AP_
                    (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY, !pWMVDec->m_bInterlaceV2);
                    (*g_pRepeatRef0UV)  (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV, !pWMVDec->m_bInterlaceV2);

#else
                    (*g_pRepeatRef0Y) (pWMVDec->m_ppxliRef0Y,0,pWMVDec->m_iHeightYRepeatPad,pWMVDec->m_iWidthPrevYXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthYRepeatPad,pWMVDec->m_iWidthYPlusExp,pWMVDec->m_iWidthPrevY);
                    (*g_pRepeatRef0UV)  (pWMVDec->m_ppxliRef0U,pWMVDec->m_ppxliRef0V,0,pWMVDec->m_iHeightUVRepeatPad,pWMVDec->m_iWidthPrevUVXExpPlusExp,TRUE,TRUE,pWMVDec->m_iWidthUVRepeatPad,pWMVDec->m_iWidthUVPlusExp,pWMVDec->m_iWidthPrevUV);
#endif
                    FUNCTION_PROFILE_STOP(&fpRep)
#               endif
                
                if (!pWMVDec->m_bRenderFromPostBuffer && pWMVDec->m_iResIndexPrev == 0) {
                    // Need this so that GetPrevOutput gets the correct frame
                    SwapCurrAndPost (pWMVDec);
                    pWMVDec->m_bRenderFromPostBuffer = TRUE;
                }
            }
        }      
        pWMVDec->m_iResIndexPrev = pWMVDec->m_iResIndex;
    }

    Void_WMV ResetBState(tWMVDecInternalMember *pWMVDec) 
    { 
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ResetBState);
            pWMVDec->m_tFrmType = IVOP; 
            pWMVDec->m_iBState = -3; 
            pWMVDec->m_iPrevIFrame = 0;
            pWMVDec->m_bPrevAnchor = FALSE; 

            if (pWMVDec->m_bSpriteMode)
                pWMVDec->m_bBFrameOn = FALSE;
            else
                pWMVDec->m_bBFrameOn = TRUE;

            pWMVDec->m_iDroppedPFrame = 0; pWMVDec->m_bIsBChangedToI = FALSE;
            // sils 20020727 
            // Some discontinuities are due to user controlled seeks and some are from the player being behind and skipping to the next key frame.
            // without knowning, we can;t count this as a skipped frame as it will adapt the post processing
            // ideally, we could know the difference and count only the player behind ones...
            // pWMVDec->m_iNumFrameDropped++;
    }

#else

    Void_WMV ResetBState(tWMVDecInternalMember *pWMVDec) 
    {}

#endif //WMV9_SIMPLE_ONLY

Void_WMV GetPerSampleInterlaceMode (tWMVDecInternalMember *pWMVDec, 
    VideoInterlaceMetaData * peInterlaceMode, Bool_WMV * pbLowerField)
{
    VideoInterlaceMetaData eInterlaceMode = Interlace_Progressive;
    Bool_WMV bLowerField = FALSE;
    if (pWMVDec->m_cvCodecVersion == WMV3) {
        if (pWMVDec->m_bYUV411) {
            eInterlaceMode = Interlace_FieldInterleavedUpperFirst; // assume this first
        }
    } else if (pWMVDec->m_cvCodecVersion == WMVA) {
        if (pWMVDec->m_bInterlaceV2 || pWMVDec->m_bFieldMode) {
             eInterlaceMode = Interlace_FieldInterleavedUpperFirst;
        }
    }   

    *peInterlaceMode = eInterlaceMode;
    *pbLowerField = bLowerField;

    return;
}





#if defined(PPC_SUPPORT_PROFILES)

    I32_WMV dbgWMVReadPPCRegistry (void) 
    {
        I32_WMV iRtnVal = -1;
        HKEY hkey1, hkey2, hkey3, hkey4;
        DWORD Type;      // address of buffer for value type
        BYTE  Data[80];       // address of data buffer
        DWORD cbData = 80;    // address of data buffer size);
        LONG result = -1;

        result = RegOpenKeyEx (HKEY_CURRENT_USER, TEXT("SOFTWARE"), 0, 0, &hkey1);

        if (result != ERROR_SUCCESS)
            return iRtnVal;
    
        result = RegOpenKeyEx (hkey1, TEXT("Microsoft"), 0, 0, &hkey2);

        if(result != ERROR_SUCCESS) {
            RegCloseKey(hkey1);
            return iRtnVal;
        }

        result = RegOpenKeyEx (hkey2, TEXT("Scrunch"), 0, 0, &hkey3);

        if(result != ERROR_SUCCESS) {
            RegCloseKey(hkey2);
            RegCloseKey(hkey1);
            return iRtnVal;
        }

        result = RegOpenKeyEx (hkey3, TEXT("Video"), 0, 0, &hkey4);

        if(result != ERROR_SUCCESS) {
            RegCloseKey(hkey3);
            RegCloseKey(hkey2);
            RegCloseKey(hkey1);
            return iRtnVal;
        }

        // at this point, hkey4 is created and result == ERROR_SUCCESS

        result = RegQueryValueEx (hkey4, TEXT("Support Profiles"), 0, &Type,   &(Data[0]), &cbData);
        if (result == ERROR_SUCCESS) {
            if (Type == REG_DWORD)
                iRtnVal = (I32_WMV)(*((DWORD *)&(Data[0])));
        }

        // if any of the calls does not return ERROR_SUCCESS, set iRtnVal to -1 which supports all profiles
        if(result != ERROR_SUCCESS) 
            iRtnVal = -1;

        RegCloseKey(hkey4);
        RegCloseKey(hkey3);
        RegCloseKey(hkey2);
        RegCloseKey(hkey1);
        return iRtnVal;
    }
#   pragma message(  __FILE__ "(2552) : Warning OK to test but do not checkin : PPC_SUPPORT_PROFILES" ) 
#endif  // 
