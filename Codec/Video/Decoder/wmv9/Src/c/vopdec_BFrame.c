//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************


#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
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


#ifndef WMV9_SIMPLE_ONLY // no B frames
/*
FORCEINLINE 	Bool_WMV getCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn) 
{ 
	return pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1];
}

FORCEINLINE Void_WMV  setCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) 
{
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1] =(U8_WMV) bisCoded;
#ifndef _WMV9AP_
    pmbmd->m_rgbCodedBlockPattern |= bisCoded << (blkn - 1);
#endif

}
*/
/****************************************************************************************
  decodeB_Deblock : B frame stuff
****************************************************************************************/

tWMVDecodeStatus decodeB_Deblock (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV          result;
    tWMVDecodeStatus tWMVStatus = WMV_Succeeded;
    
    U8_WMV __huge* ppxliCurrQY;
    U8_WMV __huge* ppxliCurrQU;
    U8_WMV __huge* ppxliCurrQV;
    U8_WMV __huge* ppxliRef1Y=NULL_WMV;
    U8_WMV __huge* ppxliRef1U=NULL_WMV;
    U8_WMV __huge* ppxliRef1V=NULL_WMV;
    
    U8_WMV __huge* ppxliRef0Y=NULL_WMV;
    U8_WMV __huge* ppxliRef0U=NULL_WMV;
    U8_WMV __huge* ppxliRef0V=NULL_WMV;
    
    
    U8_WMV* rgchSkipPrevFrame ;
    
    CWMVMBMode* pmbmd ;
    CWMVMBMode* pmbmd1 ;
    
    CVector_X9* pmv ;
    CMotionVector_X9* pmv1 ;
    CVector_X9 mvInterp;
    
    CoordI x, y; 
    I32_WMV  imbX, imbY, iMB;
    I16_WMV* piQuanCoefACPred ;
    I16_WMV** piQuanCoefACPredTable ;
    //U8_WMV** piQuanCoef_IMBInPFrame_DCPredTable ;
    U32_WMV uiNumMBFromSliceBdry ;
    
    
    Bool_WMV bFrameInPostBuf = FALSE;
    
    
    HUFFMANGET_DBG_HEADER(":decodeB",3);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeB_Deblock);
    FUNCTION_PROFILE_DECL_START(fp,DECODEBDEBLOCK_PROFILE);
    
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
    
    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;
    
    if ((pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal) >> 7) {
        pWMVDec->m_mbtShortBSymbol = BACKWARD;
        pWMVDec->m_mbtLongBSymbol = FORWARD;
    }
    else {
        pWMVDec->m_mbtShortBSymbol = FORWARD;
        pWMVDec->m_mbtLongBSymbol = BACKWARD;
    }
    
    // SET_NEW_FRAME
    pWMVDec->m_pAltTables->m_iHalfPelMV =
        (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
    t_AltTablesSetNewFrame(pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);
    
    
    // Set transform type huffman pointers
    if (pWMVDec->m_iStepSize < 5) {
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternLowQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeLowQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeLowQPDec;
    }
    else if (pWMVDec->m_iStepSize < 13) { // Mid rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternMidQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeMidQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeMidQPDec;
    }
    else { // Low rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternHighQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeHighQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeHighQPDec;
    }
    
    ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    
    ppxliRef1Y = pWMVDec->m_ppxliRef1YPlusExp; 
    ppxliRef1U = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliRef1V = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    
    ppxliRef0Y = pWMVDec->m_ppxliRef0YPlusExp; 
    ppxliRef0U = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliRef0V = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;

//#ifdef _EMB_3FRAMES_
     if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled)
     {
                if(pWMVDec->m_bRef0InRefAlt == TRUE_WMV)
                {
                    ppxliRef0Y = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp; 
                    ppxliRef0U = pWMVDec->m_pfrmRef0AltQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
                    ppxliRef0V = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
                }
     }
//#endif
    
    DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliRef0Y, pWMVDec->m_ppxliRef1Y);
    
    rgchSkipPrevFrame = pWMVDec->m_rgchSkipPrevFrame;
    
    pmbmd = pWMVDec->m_rgmbmd;
    pmbmd1 = pWMVDec->m_rgmbmd1;
    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;
    
    pmv = pWMVDec->m_rgmv_X9;
    pmv1 = pWMVDec->m_rgmv1;
    
    piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
    piQuanCoefACPredTable = pWMVDec->m_rgiQuanCoefACPredTable;
    //     piQuanCoef_IMBInPFrame_DCPredTable = (ppxliCurrQY == pWMVDec->m_rgiQuanCoef_IMBInPFrame_DCPredTable0[0])? pWMVDec->m_rgiQuanCoef_IMBInPFrame_DCPredTable0 : pWMVDec->m_rgiQuanCoef_IMBInPFrame_DCPredTable1;
    uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;
    pWMVDec->m_pAltTables->m_iHalfPelMV =
        (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
    // SET_NEW_FRAME
    t_AltTablesSetNewFrame(pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);
    // reset intra context
    memset (pWMVDec->m_pX9dct, 0, pWMVDec->m_uintNumMB * 16 * 6 * sizeof(I16_WMV));   
    
    for (imbY = iMB = 0, y = 0; imbY < (I32_WMV) pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {
        U8_WMV __huge* ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV __huge* ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV __huge* ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV __huge* ppxliRef0YMB = ppxliRef0Y;
        U8_WMV __huge* ppxliRef0UMB = ppxliRef0U;
        U8_WMV __huge* ppxliRef0VMB = ppxliRef0V;
        
        U8_WMV __huge* ppxliRef1YMB = ppxliRef1Y;
        U8_WMV __huge* ppxliRef1UMB = ppxliRef1U;
        U8_WMV __huge* ppxliRef1VMB = ppxliRef1V;
        //Bool_WMV bNot1stRowInPict = (imbY != 0);
        //Bool_WMV bNot1stRowInSlice = ((imbY % pWMVDec->m_uintNumMBY) != 0);
                
        Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];
    
        
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBY_LoopCount");

        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;
        if (!(imbY & 0x01)){ 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
            pmv = pWMVDec->m_rgmv_X9;
        }
        
        if (pWMVDec->m_bSliceWMVA) {
                if ( pWMVDec->m_pbStartOfSliceRow[imbY]) {
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush(pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
                    if (result != ICERR_OK)
                        return WMV_Failed;
                }
        }        
        pWMVDec->m_rgiDCRef [0] = pWMVDec->m_rgiDCRef [1] = pWMVDec->m_rgiDCRef [2] = 128;
        
        
        for (x = 0, imbX = 0; imbX < (I32_WMV) pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE, iMB++) {
            I32_WMV  iPredx, iPredy;
            //I32_WMV iStride = (I32_WMV) pWMVDec->m_uintNumMBX << 1;
            //I32_WMV iMVOffst = 2 * (iStride * imbY + imbX);
            //I32_WMV k1 = imbY * pWMVDec->m_uintNumMBX + imbX;
            I32_WMV idfx, idfy, idbx, idby;
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBX_LoopCount");
            
            mvInterp.x = 0;
            mvInterp.y = 0;
            pmv->x = pmv->y = 0;
            
            
#ifdef _WMV9AP_
                        if (pWMVDec->m_iPrevIFrame == 1 || pmbmd1->m_dctMd == INTRA || pmv1->m_vctTrueHalfPel.x == IBLOCKMV)
#else
            if (pWMVDec->m_iPrevIFrame == 1 || pmbmd1->m_dctMd == INTRA)
#endif
                pmv1->m_vctTrueHalfPel.x = pmv1->m_vctTrueHalfPel.y = 0;
            
            uiNumMBFromSliceBdry ++;
            pmbmd->m_bCBPAllZero = FALSE;
            
            // if skip set diff MV to zero
            memset (pWMVDec->m_pDiffMV, 0, 2 * sizeof(CDiffMV));
            
            tWMVStatus = decodeMBOverheadOfBVOP_WMV3 (pWMVDec, pmbmd, imbX, imbY);
            
            if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR) {
                pWMVDec->m_pDiffMV->iX <<= 1;
                pWMVDec->m_pDiffMV->iY <<= 1;
                (pWMVDec->m_pDiffMV+1)->iX <<= 1;
                (pWMVDec->m_pDiffMV+1)->iY <<= 1;
            }
            
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fp);
                return tWMVStatus;
            }
            
            *rgchSkipPrevFrame = 0;
            pmbmd->m_chMBMode = MB_1MV;
            
            if (pWMVDec->m_pDiffMV->iIntra == 0)
            {
				pmbmd -> m_rgcIntraFlag[0] = pmbmd -> m_rgcIntraFlag[1] = pmbmd -> m_rgcIntraFlag[2] = pmbmd -> m_rgcIntraFlag[3] =  0;
                if (pmbmd->m_bCBPAllZero == TRUE)
                {
                    pmbmd->m_rgcBlockXformMode[0] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[1] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[2] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[3] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
                }
                if (pmbmd->m_mbType != DIRECT)
                {
                    I32_WMV iIsInterpolated = ((pmbmd->m_mbType == INTERPOLATE)?1:0);
                    I16_WMV  *pMVx, *pMVy;
                    if (iIsInterpolated == 1)
                        pmbmd->m_mbType = BACKWARD;
                    
                    if (pmbmd->m_mbType == FORWARD) {
                        pMVx = pWMVDec->m_pXMotion; // more space than necessary
                        pMVy = pWMVDec->m_pYMotion;
                    }
                    else {
                        pMVx = pWMVDec->m_pXMotionC;
                        pMVy = pWMVDec->m_pYMotionC;
                    }

#ifdef _WMV9AP_
                    if (pWMVDec->m_cvCodecVersion >= WMVA)
						PredictMVB (pWMVDec, imbX, imbY, 2, pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY,
							pMVx, pMVy, &iPredx, &iPredy, bTopBndry);
					else
#endif
                    PredictMV (pWMVDec, imbX, imbY, 2, pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY,
                        pMVx, pMVy, &iPredx, &iPredy, bTopBndry);
                    
                    if (!pmbmd->m_bSkip)
                    {
                        pmv->x = ((iPredx + pWMVDec->m_pDiffMV->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;  //zou -->DMV_X = pMB->sBlk[eBlk].u.sInter.sMotion[Backwards].sDMV.X;  //
                        pmv->y = ((iPredy + pWMVDec->m_pDiffMV->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;
                    }
                    else
                    {
                        pmv->x = (I16_WMV)iPredx;
                        pmv->y = (I16_WMV)iPredy;
                    }
                    
                    
                    if (iIsInterpolated == 1)
                    {
                        I16_WMV imvTempx = pmv->x;
                        I16_WMV imvTempy = pmv->y;
                        
                        pmbmd->m_mbType = FORWARD;                        
                        
                        pmv->x = pmv->y =0;
#ifdef _WMV9AP_
						if (pWMVDec->m_cvCodecVersion >= WMVA)
							PredictMVB (pWMVDec, imbX, imbY, 2, pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY,
								pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, &iPredx, &iPredy, bTopBndry);
						else
#endif
                        PredictMV (pWMVDec, imbX, imbY, 2, pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY,
                            pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, &iPredx, &iPredy, bTopBndry);
                        
                        if (!pmbmd->m_bSkip && pWMVDec->m_pDiffMV->iLast)
                        {
                            pmv->x = ((iPredx + (pWMVDec->m_pDiffMV + 1)->iX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
                            pmv->y = ((iPredy + (pWMVDec->m_pDiffMV + 1)->iY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;
                        }
                        else {
                            pmv->x = (I16_WMV)iPredx; pmv->y =(I16_WMV) iPredy;
                        }
                        
                        mvInterp.x = pmv->x;
                        mvInterp.y = pmv->y;
                        pmv->x = imvTempx;
                        pmv->y = imvTempy; 
                        
                        pmbmd->m_mbType = INTERPOLATE;
                        
                    } // end if interpolated
                    
                } // if no skip and no direct
                
                DirectModeMV (pWMVDec, pmv1->m_vctTrueHalfPel.x, pmv1->m_vctTrueHalfPel.y,
                    (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR),
                    imbX, imbY, &idfx, &idfy, &idbx, &idby);
                
                if (pmbmd->m_mbType == DIRECT) {
                    tWMVStatus = DecodeBMB (pWMVDec,
                        imbX, imbY,
                        pmbmd,
                        ppxliCurrQYMB, 
                        ppxliCurrQUMB,
                        ppxliCurrQVMB, x, y,
                        pmv1->m_vctTrueHalfPel.x, pmv1->m_vctTrueHalfPel.y, 0, 0);
                    
                    pWMVDec->m_pXMotion[iMB] = (I16_WMV)idfx;
                    pWMVDec->m_pYMotion[iMB] = (I16_WMV)idfy;
                    pWMVDec->m_pXMotionC[iMB] = (I16_WMV)idbx;
                    pWMVDec->m_pYMotionC[iMB] = (I16_WMV)idby;
                }
                else if (pmbmd->m_mbType == INTERPOLATE) {
                    tWMVStatus = DecodeBMB (pWMVDec,
                        imbX, imbY,
                        pmbmd,
                        ppxliCurrQYMB, 
                        ppxliCurrQUMB,
                        ppxliCurrQVMB, x, y, mvInterp.x, mvInterp.y, pmv->x, pmv->y );
                    
                    pWMVDec->m_pXMotion[iMB] = mvInterp.x;
                    pWMVDec->m_pYMotion[iMB] = mvInterp.y;
                    pWMVDec->m_pXMotionC[iMB] = pmv->x;
                    pWMVDec->m_pYMotionC[iMB] = pmv->y;
                }
                else {
                    tWMVStatus = DecodeBMB (pWMVDec,
                        imbX, imbY,
                        pmbmd,
                        ppxliCurrQYMB, 
                        ppxliCurrQUMB,
                        ppxliCurrQVMB, 
                        x, y, pmv->x, pmv->y, 0, 0); 
                    
                    if (pmbmd->m_mbType == FORWARD)
                    {
                        pWMVDec->m_pXMotion[iMB] = pmv->x;
                        pWMVDec->m_pYMotion[iMB] = pmv->y;
                        pWMVDec->m_pXMotionC[iMB] = (I16_WMV)idbx;
                        pWMVDec->m_pYMotionC[iMB] = (I16_WMV)idby;
                    }
                    else
                    {
                        pWMVDec->m_pXMotion[iMB] =(I16_WMV) idfx;
                        pWMVDec->m_pYMotion[iMB] =(I16_WMV) idfy;
                        pWMVDec->m_pXMotionC[iMB] = pmv->x;
                        pWMVDec->m_pYMotionC[iMB] = pmv->y;
                        
                    }
                    
                }
                
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fp);
                    
                    return tWMVStatus;
                }
            }
            else {      // INTRA
                //                pmv->x = pmv->y = 0;
                //I32_WMV iStride = (I32_WMV) pWMVDec->m_uintNumMBX << 1;
                //I32_WMV k1 = imbY * pWMVDec->m_uintNumMBX + imbX;
                //I32_WMV iMVOffst = 2 * (iStride * imbY + imbX);
                
				pmbmd -> m_rgcIntraFlag[0] = pmbmd -> m_rgcIntraFlag[1] = pmbmd -> m_rgcIntraFlag[2] = pmbmd -> m_rgcIntraFlag[3] =  1;

                pWMVDec->m_pXMotion[iMB] = pWMVDec->m_pYMotion[iMB] =
                    pWMVDec->m_pXMotionC[iMB] = pWMVDec->m_pYMotionC[iMB] = IBLOCKMV;
                
                tWMVStatus = DecodeMB_1MV (pWMVDec, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                    imbX, imbY, x << 1, y << 1, x, y);                
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fp);
                    
                    return tWMVStatus;
                }
            }
#if 0            
			if(0){
				if(pWMVDec->m_nBframes == 12)
				{
					FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump.txt","a");
					int i,j;
					fprintf(fp,"-----------------------%d %d -----------\n",imbY,imbX);
					for(i=0;i<16;i++)
					{
						for(j=0;j<16;j++)
						{
							fprintf(fp,"%x   ",ppxliCurrQYMB[i*pWMVDec->m_iWidthInternal+j]);
						}
						fprintf(fp,"\n");
					}

					for(i=0;i<8;i++)
					{
						for(j=0;j<8;j++)
						{
							fprintf(fp,"%x   ",ppxliCurrQUMB[i*pWMVDec->m_iWidthInternalUV+j]);
						}
						fprintf(fp,"\n");
					}

					for(i=0;i<8;i++)
					{
						for(j=0;j<8;j++)
						{
							fprintf(fp,"%x   ",ppxliCurrQVMB[i*pWMVDec->m_iWidthInternalUV+j]);
						}
						fprintf(fp,"\n");
					}
					fclose(fp);
				}
			}
#endif            
            //UPDATE_BMP:
            // If loopfilter is on, color convert and copying to Post waits till loopfilter is complete
            if (pWMVDec->m_bLoopFilter) 
                goto NEXT_MB;
            
NEXT_MB:
            if (pmbmd->m_dctMd == INTER) {
                piQuanCoefACPred[0] = piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES4] =piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES5] = 0;
            }
            rgchSkipPrevFrame++;
            pmbmd++;
            pmv++;
            pmbmd1++;
            pmv1++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            ppxliRef0YMB += MB_SIZE;
            ppxliRef0UMB += BLOCK_SIZE;
            ppxliRef0VMB += BLOCK_SIZE;
            ppxliRef1YMB += MB_SIZE;
            ppxliRef1UMB += BLOCK_SIZE;
            ppxliRef1VMB += BLOCK_SIZE;
            
            piQuanCoefACPred += BLOCK_SIZE_TIMES2_TIMES6;
            piQuanCoefACPredTable += 36;
            //  piQuanCoef_IMBInPFrame_DCPredTable += 9;
        }
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        
        ppxliRef0Y += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliRef0U += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRef0V += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRef1Y += pWMVDec->m_iMBSizeXWidthPrevY; // point to the starting location of the first MB of each row
        ppxliRef1U += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row
        ppxliRef1V += pWMVDec->m_iBlkSizeXWidthPrevUV; // point to the starting location of the first MB of each row        
        
    }
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MB_Loop_Done");
        
    if (pWMVDec->m_bLoopFilter)
    {
        
#ifdef _WMV9AP_
        if (pWMVDec->m_bLoopFilter) {
            if (pWMVDec->m_cvCodecVersion == WMVA) {
                ComputeLoopFilterFlags_WMVA(pWMVDec);
            } else {
                ComputeLoopFilterFlags_WMV9(pWMVDec);
            }

            DeblockSLFrame_V9 (pWMVDec,
                pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                pWMVDec->m_puchFilter8x8RowFlagY0, pWMVDec->m_puchFilter8x8RowFlagU0, pWMVDec->m_puchFilter8x8RowFlagV0,
                pWMVDec->m_puchFilter8x4RowFlagY0, pWMVDec->m_puchFilter8x4RowFlagU0, pWMVDec->m_puchFilter8x4RowFlagV0,
                pWMVDec->m_puchFilter8x8ColFlagY0, pWMVDec->m_puchFilter8x8ColFlagU0, pWMVDec->m_puchFilter8x8ColFlagV0,
                pWMVDec->m_puchFilter4x8ColFlagY0, pWMVDec->m_puchFilter4x8ColFlagU0, pWMVDec->m_puchFilter4x8ColFlagV0,
                pWMVDec->m_pLoopFilterFlags, 0, pWMVDec->m_uiMBEnd0);
        }
#else
        if (pWMVDec->m_bV9LoopFilter) {
            ComputeLoopFilterFlags(pWMVDec);
            DeblockSLFrame_V9 (pWMVDec,
                pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
                pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
                pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
                pWMVDec->m_puchFilter8x8RowFlagY0, pWMVDec->m_puchFilter8x8RowFlagU0, pWMVDec->m_puchFilter8x8RowFlagV0,
                pWMVDec->m_puchFilter8x4RowFlagY0, pWMVDec->m_puchFilter8x4RowFlagU0, pWMVDec->m_puchFilter8x4RowFlagV0,
                pWMVDec->m_puchFilter8x8ColFlagY0, pWMVDec->m_puchFilter8x8ColFlagU0, pWMVDec->m_puchFilter8x8ColFlagV0,
                pWMVDec->m_puchFilter4x8ColFlagY0, pWMVDec->m_puchFilter4x8ColFlagU0, pWMVDec->m_puchFilter4x8ColFlagV0,
                pWMVDec->m_pLoopFilterFlags, 0, pWMVDec->m_uiMBEnd0);
        }
#endif
        else
            DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
        
    }        
    
    
    pWMVDec->m_bDisplay_AllMB = TRUE;
    
    
    
    
    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;
    
    
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame
    pWMVDec->m_iFilterType = FILTER_BICUBIC;
    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}

static I32_WMV s_iBlkXformTypeLUT[8] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};

static I32_WMV s_iSubblkPatternBlkLUT[8] = {0, 1, 2, 3, 1, 2, 3, 0};

/****************************************************************************************
  DecodeBMB : B macroblock level decoding
****************************************************************************************/
tWMVDecodeStatus DecodeBMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY,
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    CoordI xIndex, CoordI yIndex,
    CoordI xMv, CoordI yMv,
    CoordI xMv2, CoordI yMv2
)
{
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
    U8_WMV* rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    Bool_WMV bTop, bBottom, bLeft, bRight;
    
    DQuantDecParam *pDQ;
    
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    
    
    I32_WMV i4x4Pattern;
    I32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    
    UnionBuffer __huge* ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    tWMVDecodeStatus result;
    
    // Y-blocks first (4 blocks)
    
    I32_WMV iDirecty, iDirectx;

    U8_WMV ppxliInterpolate0YMB[MB_SIZE * MB_SIZE], ppxliInterpolate1YMB[MB_SIZE * MB_SIZE];
    
    U8_WMV __huge *pSrcY0 = pWMVDec->m_ppxliRef0Y + (yIndex + pWMVDec->EXPANDY_REFVOP) * pWMVDec->m_iWidthPrevY + xIndex + pWMVDec->EXPANDY_REFVOP;
    U8_WMV __huge *pSrcY1 = pWMVDec->m_ppxliRef1Y + (yIndex + pWMVDec->EXPANDY_REFVOP) * pWMVDec->m_iWidthPrevY + xIndex + pWMVDec->EXPANDY_REFVOP;
    // UV
    I32_WMV iPxLoc = ((yIndex >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (xIndex >> 1) + pWMVDec->EXPANDUV_REFVOP;
    U8_WMV __huge* ppxlcRef0U = pWMVDec->m_ppxliRef0U + iPxLoc;
    U8_WMV __huge* ppxlcRef0V = pWMVDec->m_ppxliRef0V + iPxLoc;
    U8_WMV __huge* ppxlcRef1U = pWMVDec->m_ppxliRef1U + iPxLoc;
    U8_WMV __huge* ppxlcRef1V = pWMVDec->m_ppxliRef1V + iPxLoc;
    
    U8_WMV ppxliInterpolate0UMB[BLOCK_SIZE * BLOCK_SIZE],ppxliInterpolate0VMB[BLOCK_SIZE * BLOCK_SIZE];
    U8_WMV ppxliInterpolate1UMB[BLOCK_SIZE * BLOCK_SIZE],ppxliInterpolate1VMB[BLOCK_SIZE * BLOCK_SIZE];
    
    U8_WMV tempCurrYMB[MB_SIZE*MB_SIZE];
    U8_WMV __huge *ppxlcRefY = ppxliInterpolate0YMB;
    U8_WMV __huge *ptempCurrYMB = tempCurrYMB;
    U8_WMV tempCurrUMB[BLOCK_SIZE * BLOCK_SIZE], tempCurrVMB[BLOCK_SIZE * BLOCK_SIZE];
    U8_WMV __huge *ppxlcRefU = ppxliInterpolate0UMB;
    U8_WMV __huge *ppxlcRefV = ppxliInterpolate0VMB;
    U8_WMV __huge *ptempCurrUMB = tempCurrUMB;
    U8_WMV __huge *ptempCurrVMB = tempCurrVMB;
    I32_WMV iBlock;
    I32_WMV iOffst;
    I32_WMV i,j;
    U8_WMV __huge *ppxlcSrc,*ppxlcDst;
    
    
    FUNCTION_PROFILE_DECL_START(fpDecode,DECODEBMB_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeBMB);
    
    pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];
    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;
    
    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }
    
    if (pmbmd->m_mbType == DIRECT || pmbmd->m_mbType == INTERPOLATE) {
        I32_WMV idfx, idfy, idbx, idby;
        if (pmbmd->m_mbType == INTERPOLATE) {
            idfx = xMv;
            idfy = yMv;
            idbx = xMv2;
            idby = yMv2;
        }
        else {
            DirectModeMV (pWMVDec, xMv, yMv, (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR),
                imbX, imbY, &idfx, &idfy, &idbx, &idby);
        }
        
        PullBackMotionVector (pWMVDec, &idfx, &idfy, imbX, imbY);
        PullBackMotionVector (pWMVDec, &idbx, &idby, imbX, imbY);
        
    if(pWMVDec->m_bCodecIsWVC1){
		if( (idfx>>2) + xIndex <= -32)  //zou fix
			idfx =  (-32-xIndex)<<2;
		else	if( (idfx>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			idfx = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		if((idfy>>2) + yIndex <= -32)
			idfy =  (-32-yIndex)<<2;
		else	if((idfy>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			idfy = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;

		if( (idbx>>2) + xIndex <= -32)  //zou fix
			idbx =  (-32-xIndex)<<2;
		else	if( (idbx>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			idbx = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		if((idby>>2) + yIndex <= -32)
			idby =  (-32-yIndex)<<2;
		else	if((idby>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			idby = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;
    }

        // Y channel
        InterpolateYMB_X9 (pWMVDec, pSrcY0, pWMVDec->m_iWidthPrevY, ppxliInterpolate0YMB,                                       
                           MB_SIZE, idfx, idfy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl & 0x1);
        InterpolateYMB_X9 (pWMVDec, pSrcY1, pWMVDec->m_iWidthPrevY, ppxliInterpolate1YMB,                                 
                           MB_SIZE, idbx, idby, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl & 0x1);
        pWMVDec->m_pPixelMean (ppxliInterpolate0YMB, MB_SIZE, ppxliInterpolate1YMB, MB_SIZE,
                               ppxliInterpolate0YMB, MB_SIZE, MB_SIZE, MB_SIZE);
        
        // U, V motion vectors
        ChromaMV_B (pWMVDec, &idfx, &idfy);
        ChromaMV_B (pWMVDec, &idbx, &idby);
    if(pWMVDec->m_bCodecIsWVC1){
		if( (idfx>>2) + xIndex/2 <= -16)  //zou fix
			idfx =  (-16-xIndex/2)<<2;
		else	if( (idfx>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			idfx = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		if((idfy>>2) + yIndex/2 <= -16)
			idfy =  (-16-yIndex/2)<<2;
		else	if((idfy>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
			idfy = (pWMVDec->m_iHeightPrevUV+16 -yIndex/2)<<2;

		if( (idbx>>2) + xIndex/2 <= -16)  //zou fix
			idbx =  (-16-xIndex/2)<<2;
		else	if( (idbx>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			idbx = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		if((idby>>2) + yIndex/2 <= -16)
			idby =  (-16-yIndex/2)<<2;
		else	if((idby>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
			idby = (pWMVDec->m_iHeightPrevUV+16 -yIndex/2)<<2;
    }
        // U channel
        InterpolateBlock (pWMVDec, ppxlcRef0U, pWMVDec->m_iWidthPrevUV, ppxliInterpolate0UMB,      
                          BLOCK_SIZE, idfx, idfy, FILTER_BILINEAR);
        // BLOCK_SIZE, idfx / 2, idfy / 2, pWMVDec->m_iFilterType);
        InterpolateBlock (pWMVDec, ppxlcRef1U, pWMVDec->m_iWidthPrevUV, ppxliInterpolate1UMB,     
                          BLOCK_SIZE, idbx, idby, FILTER_BILINEAR);
        // BLOCK_SIZE, idbx / 2, idby / 2, pWMVDec->m_iFilterType);
        pWMVDec->m_pPixelMean (ppxliInterpolate0UMB, BLOCK_SIZE, ppxliInterpolate1UMB, BLOCK_SIZE,
                               ppxliInterpolate0UMB, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE); 

        // V channel
        InterpolateBlock (pWMVDec, ppxlcRef0V, pWMVDec->m_iWidthPrevUV, ppxliInterpolate0VMB,        
                          BLOCK_SIZE, idfx, idfy, FILTER_BILINEAR);
        //  BLOCK_SIZE, idfx / 2, idfy / 2, pWMVDec->m_iFilterType);
        InterpolateBlock (pWMVDec, ppxlcRef1V, pWMVDec->m_iWidthPrevUV, ppxliInterpolate1VMB,
                          BLOCK_SIZE, idbx, idby, FILTER_BILINEAR);
        //   BLOCK_SIZE, idbx / 2, idby / 2, pWMVDec->m_iFilterType);
        pWMVDec->m_pPixelMean (ppxliInterpolate0VMB, BLOCK_SIZE, ppxliInterpolate1VMB, BLOCK_SIZE,
                               ppxliInterpolate0VMB, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
        
    }
    else if (pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD)
    {
        I32_WMV idx = xMv;
        I32_WMV idy = yMv;
        U8_WMV  *pSrcY, *pSrcU, *pSrcV;
        
        PullBackMotionVector (pWMVDec, &idx, &idy, imbX, imbY);
        if (pmbmd->m_mbType == FORWARD) {
            pSrcY = pSrcY0;
            pSrcU = ppxlcRef0U;
            pSrcV = ppxlcRef0V;
        }
        else {
            pSrcY = pSrcY1;
            pSrcU = ppxlcRef1U;
            pSrcV = ppxlcRef1V;
        }

        if(pWMVDec->m_bCodecIsWVC1){
		if( (idx>>2) + xIndex <= -32)  //zou fix
			idx =  (-32-xIndex)<<2;
		else	if( (idx>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			idx = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		if((idy>>2) + yIndex <= -32)
			idy =  (-32-yIndex)<<2;
		else	if((idy>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			idy = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;
        }

        InterpolateYMB_X9 (pWMVDec, pSrcY, pWMVDec->m_iWidthPrevY, ppxliInterpolate0YMB,                                       
                           MB_SIZE, idx, idy, pWMVDec->m_iFilterType, pWMVDec->m_iRndCtrl & 0x1);
        
        // U, V motion vectors
        ChromaMV_B (pWMVDec, &idx, &idy);

        if(pWMVDec->m_bCodecIsWVC1){
		if( (idx>>2) + xIndex/2 <= -16)   //zou fix
			idx =  (-16-xIndex/2)<<2;
		else	if( (idx>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			idx = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		if((idy>>2) + yIndex/2 <= -16)
			idy =  (-16-yIndex/2)<<2;
		else	if((idy>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
			idy = (pWMVDec->m_iHeightPrevUV+16 -yIndex/2)<<2;
        }

        InterpolateBlock (pWMVDec, pSrcU, pWMVDec->m_iWidthPrevUV, ppxliInterpolate0UMB,     
                          BLOCK_SIZE, idx, idy, FILTER_BILINEAR);        
        // BLOCK_SIZE, idx / 2, idy / 2, pWMVDec->m_iFilterType);
        InterpolateBlock (pWMVDec, pSrcV, pWMVDec->m_iWidthPrevUV, ppxliInterpolate0VMB,        
                          BLOCK_SIZE, idx, idy, FILTER_BILINEAR);
        //  BLOCK_SIZE, idx / 2, idy / 2, pWMVDec->m_iFilterType);
    }
    ppxlcRefY = ppxliInterpolate0YMB;
    ptempCurrYMB = tempCurrYMB;
    ppxlcRefU = ppxliInterpolate0UMB;
    ppxlcRefV = ppxliInterpolate0VMB;
    ptempCurrUMB = tempCurrUMB;
    ptempCurrVMB = tempCurrVMB;
    
    for ( iBlock = 0; iBlock < 6; iBlock++) {
        if (rgCBP [iBlock]) {
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec, pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternBlkLUT[iIndex];
            }
            pmbmd->m_rgcBlockXformMode[iBlock] =(I8_WMV)iXformType;

            if (iXformType == XFORMMODE_8x8) {

                ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;           
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block      
                if (WMV_Succeeded != result) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return result;
                }            
                
                (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
            }
            else if (iXformType == XFORMMODE_8x4) {
                bTop = TRUE;
                bBottom = TRUE;
                
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions   // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[iBlock] = (U8_WMV) iSubblockPattern;
                        bTop = (rgCBP[iBlock] & 2);
                        bBottom = (rgCBP[iBlock] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iBlock] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                } 
                else {
                    if (bFirstBlock) {
                        rgCBP[iBlock] = (U8_WMV) pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP[iBlock] & 2);
                        bBottom = (rgCBP[iBlock] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iBlock] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }
                
                if (bTop)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    if (WMV_Succeeded != result) {
                        FUNCTION_PROFILE_STOP(&fpDecode);
                        return result;
                    }               
                    
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bBottom)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    if (WMV_Succeeded != result) {
                        FUNCTION_PROFILE_STOP(&fpDecode);
                        return result;
                    }
                    
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }
            else if (iXformType == XFORMMODE_4x8) {
                bLeft = TRUE;
                bRight = TRUE;
                
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[iBlock] = (U8_WMV) iSubblockPattern;
                        bLeft = rgCBP[iBlock] & 2;
                        bRight = rgCBP[iBlock] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iBlock] =(U8_WMV) ( (bLeft << 1) | bRight);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iBlock] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP[iBlock] & 2;
                        bRight = rgCBP[iBlock] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iBlock] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }
                
                if (bLeft) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    if (WMV_Succeeded != result) {
                        FUNCTION_PROFILE_STOP(&fpDecode);
                        return result;
                    }
                    
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bRight) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    if (WMV_Succeeded != result) {
                        FUNCTION_PROFILE_STOP(&fpDecode);
                        return result;
                    }
                    
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                } 
            }
            else if (iXformType == XFORMMODE_4x4) {
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec, pWMVDec->m_pbitstrmIn) +1;
                
                rgCBP[iBlock] = (U8_WMV)i4x4Pattern;
                
                if (i4x4Pattern & 8) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (i4x4Pattern & 4) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
                if (i4x4Pattern & 2) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
                }
                if (i4x4Pattern & 1) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
                }      
            }

            if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                I32_WMV j1;
                for( j1= 0; j1 <64; j1++)
                    ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
            }
            
            bFirstBlock = FALSE;
            if (iBlock < 4) {
                ppxlcSrc = ppxlcRefY;
                ppxlcDst = ptempCurrYMB;
                iOffst = MB_SIZE;
            }
            else {
                ppxlcSrc = (iBlock == 4) ?  ppxlcRefU : ppxlcRefV;
                ppxlcDst = (iBlock == 4) ? ptempCurrUMB : ptempCurrVMB;
                iOffst = BLOCK_SIZE;
            }
            
            for ( i = 0; i < 8; i++) {
                for ( j = 0; j < 8; j++) {
                    I32_WMV k = ppxlcSrc[i * iOffst + j] + ppxliErrorQMB->i16[i * 8 + j];
                    if (k < 0)  k = 0;
                    else if (k > 255)  k = 255;
                    ppxlcDst[i * iOffst + j] = (U8_WMV)k;
                }
            }
        }
        else {      
            // no residual
            U8_WMV __huge *ppxlcSrc,*ppxlcDst;
            I32_WMV iOffst;
            
            pmbmd->m_rgcBlockXformMode[iBlock] = XFORMMODE_8x8;
            
            if (iBlock < 4) {
                ppxlcSrc = ppxlcRefY;
                ppxlcDst = ptempCurrYMB;
                iOffst = MB_SIZE;
            }
            else {
                ppxlcSrc = (iBlock == 4) ?  ppxlcRefU : ppxlcRefV;
                ppxlcDst = (iBlock == 4) ? ptempCurrUMB : ptempCurrVMB;
                iOffst = BLOCK_SIZE;
            }
            
            for ( i = 0; i < 8; i++) {
                for ( j = 0; j < 8; j++) {
                    I32_WMV k = ppxlcSrc[i * iOffst + j];
                    if (k < 0)  k = 0;
                    else if (k > 255)  k = 255;
                    ppxlcDst[i * iOffst + j] = (U8_WMV)k;
                }
            }
        }
        if (iBlock < 4) {
            if (iBlock == 1) {
                ptempCurrYMB += MB_SIZE*BLOCK_SIZE - BLOCK_SIZE;
                ppxlcRefY += MB_SIZE*BLOCK_SIZE - BLOCK_SIZE;
                pSrcY0 += pWMVDec->m_iWidthPrevYxBlkMinusBlk;
                pSrcY1 += pWMVDec->m_iWidthPrevYxBlkMinusBlk;
                
            }
            else {
                ptempCurrYMB += BLOCK_SIZE; 
                ppxlcRefY += BLOCK_SIZE;
                
                pSrcY0 += BLOCK_SIZE;
                pSrcY1 += BLOCK_SIZE;
            }
            
        } // if (iBlock < 4)
            
    }   // for (iBlock < 6)
    for (iDirecty=0; iDirecty<MB_SIZE; iDirecty++)
        for (iDirectx=0; iDirectx<MB_SIZE; iDirectx++)
            ppxlcCurrQYMB[iDirecty * pWMVDec->m_iWidthPrevY + iDirectx] = tempCurrYMB[iDirecty * MB_SIZE + iDirectx];
        
    for (iDirecty=0; iDirecty<BLOCK_SIZE; iDirecty++)
        for (iDirectx=0; iDirectx<BLOCK_SIZE; iDirectx++) {
            ppxlcCurrQUMB[iDirecty * pWMVDec->m_iWidthPrevUV + iDirectx] = tempCurrUMB[iDirecty * BLOCK_SIZE + iDirectx];
            ppxlcCurrQVMB[iDirecty * pWMVDec->m_iWidthPrevUV + iDirectx] = tempCurrVMB[iDirecty * BLOCK_SIZE + iDirectx];
        }
    FUNCTION_PROFILE_STOP(&fpDecode);
    return WMV_Succeeded;
}

static I32_WMV s_iMBXformTypeLUT[16] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};
static I32_WMV s_iSubblkPatternLUT[16] = {0, 1, 2, 3, 1, 2, 3, 0, 0, 1, 2, 3, 1, 2, 3, 0};

/********************************************************************************
 decodeMBOverheadOfPVOP_WMV3 : WMV3 macroblock header decoding for B frames
********************************************************************************/
tWMVDecodeStatus decodeMBOverheadOfBVOP_WMV3 (tWMVDecInternalMember *pWMVDec,CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY)
{
    I32_WMV iCBPCY = 0;//, iPos=0, iTemp=0;
    Bool_WMV bSendTableswitchMode = FALSE;
    Bool_WMV bSendXformswitchMode = FALSE;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMBOverheadOfBVOP_WMV3);
    
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;
    pmbmd->m_chMBMode = MB_1MV; // always for B
    
    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT
    
    pmbmd->m_rgcBlockXformMode[0] = XFORMMODE_8x8;
    pmbmd->m_rgcBlockXformMode[1] = XFORMMODE_8x8;
    pmbmd->m_rgcBlockXformMode[2] = XFORMMODE_8x8;
    pmbmd->m_rgcBlockXformMode[3] = XFORMMODE_8x8;
    pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
    pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
    
    
    if (pWMVDec->m_iDirectCodingMode == SKIP_RAW) 
        pmbmd->m_mbType = (MBType)BS_getBit(pWMVDec->m_pbitstrmIn);
    
    if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
    }
    
    bSendTableswitchMode = FALSE;
    bSendXformswitchMode = FALSE;
    
    // 1MV - read motion vector
    
    if (pmbmd->m_mbType == DIRECT)
        pWMVDec->m_pDiffMV->iLast = 1;		// else would have skipped
    else {
        if (!pmbmd->m_bSkip)
            decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, pWMVDec->m_pDiffMV);    
        
        if (pmbmd->m_mbType == (MBType)0)
        {
            if (pWMVDec->m_pDiffMV->iIntra)
                pmbmd->m_mbType = BACKWARD;
            else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0)
                pmbmd->m_mbType = pWMVDec->m_mbtShortBSymbol;
            else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0)
                pmbmd->m_mbType = pWMVDec->m_mbtLongBSymbol;
            else
                pmbmd->m_mbType = INTERPOLATE;
        }
    }
    
    if (pmbmd->m_bSkip) {
        memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));
#ifndef _WMV9AP_
        pmbmd->m_rgbCodedBlockPattern = 0;
#endif
        setCodedBlockPattern (pmbmd, Y_BLOCK1, 0); 
        setCodedBlockPattern (pmbmd, Y_BLOCK2, 0);
        setCodedBlockPattern (pmbmd, Y_BLOCK3, 0); 
        setCodedBlockPattern (pmbmd, Y_BLOCK4, 0);
        setCodedBlockPattern (pmbmd, U_BLOCK, 0); 
        setCodedBlockPattern (pmbmd, V_BLOCK, 0);
        return WMV_Succeeded;
    }
    
    // skip to end if last = 0
    if (pWMVDec->m_pDiffMV->iLast == 0) {
        pmbmd->m_bCBPAllZero = TRUE;
        if (pWMVDec->m_bDQuantOn && pWMVDec->m_pDiffMV->iIntra) {
            if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                return WMV_Failed;
            }
        }
        // One MV Intra AC Prediction Flag
        if (pWMVDec->m_pDiffMV->iIntra)
            pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBit(pWMVDec->m_pbitstrmIn);              
        goto End;
    }
    
    if (pmbmd->m_mbType == INTERPOLATE)
    {
        decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV+1));    
        
        // INTRA is invalid in INTERPOLATE mode
        if ((pWMVDec->m_pDiffMV + 1)->iIntra || pWMVDec->m_pDiffMV->iIntra)
            return WMV_Failed;
        
        // skip to end if last = 0
        if ((pWMVDec->m_pDiffMV + 1)->iLast == 0) {
            pmbmd->m_bCBPAllZero = TRUE;
            goto End;
        }
    }
    
    bSendTableswitchMode = pWMVDec->m_bDCTTable_MB;
    bSendXformswitchMode = pWMVDec->m_bMBXformSwitching && !pWMVDec->m_pDiffMV->iIntra;
    // One MV Intra AC Prediction Flag
    if (pWMVDec->m_pDiffMV->iIntra)
        pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBit(pWMVDec->m_pbitstrmIn);              
    
    // read CBP
    iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufNewPCBPCYDec, pWMVDec->m_pbitstrmIn);
    
    // decode DQUANT
    if (pWMVDec->m_bDQuantOn && WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd))
        return WMV_Failed;
    
    pmbmd->m_bCBPAllZero = FALSE;
    
    if (bSendTableswitchMode) {
        I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
End:
    if (bSendXformswitchMode) {
        I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufMBXformTypeDec, pWMVDec->m_pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        pmbmd->m_iMBXformMode = s_iMBXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        pmbmd->m_bFirstBlockDecoded = FALSE;
    }
    
#ifndef _WMV9AP_
    pmbmd->m_rgbCodedBlockPattern = 0;
#endif
    setCodedBlockPattern (pmbmd, Y_BLOCK1, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, Y_BLOCK2, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, Y_BLOCK3, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, Y_BLOCK4, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, U_BLOCK, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, V_BLOCK, iCBPCY & 0x1);
    
    return WMV_Succeeded;
}

/********************************************************************************
 session stuff (should be moved out to sesdec later)
********************************************************************************/


Void_WMV swapCurrRef1Pointers (tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmCurrQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(swapCurrRef1Pointers);

    pWMVDec->m_pfrmCurrQ = pWMVDec->m_pfrmRef1Q;
    pWMVDec->m_pfrmRef1Q = pTmp;

    pWMVDec->m_ppxliCurrQY = pWMVDec->m_pfrmCurrQ->m_pucYPlane;
    pWMVDec->m_ppxliCurrQU = pWMVDec->m_pfrmCurrQ->m_pucUPlane;
    pWMVDec->m_ppxliCurrQV = pWMVDec->m_pfrmCurrQ->m_pucVPlane;

    pWMVDec->m_ppxliRef1Y = pWMVDec->m_pfrmRef1Q->m_pucYPlane;
    pWMVDec->m_ppxliRef1U = pWMVDec->m_pfrmRef1Q->m_pucUPlane;
    pWMVDec->m_ppxliRef1V = pWMVDec->m_pfrmRef1Q->m_pucVPlane;

    pWMVDec->m_ppxliRef1YPlusExp = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;

    pWMVDec->m_ppxliCurrQPlusExpY0 = (U8_WMV*) pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpU0 = (U8_WMV*) pWMVDec->m_ppxliCurrQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    pWMVDec->m_ppxliCurrQPlusExpV0 = (U8_WMV*) pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    pWMVDec->m_ppxliRefYPreproc = pWMVDec->m_ppxliRef1Y;      
    pWMVDec->m_ppxliRefUPreproc = pWMVDec->m_ppxliRef1U;          
    pWMVDec->m_ppxliRefVPreproc = pWMVDec->m_ppxliRef1V;

}

Void_WMV swapRef0Ref1Pointers (tWMVDecInternalMember *pWMVDec)
{
    tYUV420Frame_WMV* pTmp = pWMVDec->m_pfrmRef0Q;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(swapRef0Ref1Pointers);

    pWMVDec->m_pfrmRef0Q = pWMVDec->m_pfrmRef1Q;
    pWMVDec->m_pfrmRef1Q = pTmp;
    
    pWMVDec->m_ppxliRef0Y = pWMVDec->m_pfrmRef0Q->m_pucYPlane;
    pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Q->m_pucUPlane;
    pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Q->m_pucVPlane;
    
    pWMVDec->m_ppxliRef1Y = pWMVDec->m_pfrmRef1Q->m_pucYPlane;
    pWMVDec->m_ppxliRef1U = pWMVDec->m_pfrmRef1Q->m_pucUPlane;
    pWMVDec->m_ppxliRef1V = pWMVDec->m_pfrmRef1Q->m_pucVPlane;
    
    
    pWMVDec->m_ppxliRef1YPlusExp = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    
    
    pWMVDec->m_ppxliRef0YPlusExp = pWMVDec->m_ppxliRef0Y + pWMVDec->m_iWidthPrevYXExpPlusExp;
    
    pWMVDec->m_ppxliRefYPreproc = pWMVDec->m_ppxliRef0Y;      
    pWMVDec->m_ppxliRefUPreproc = pWMVDec->m_ppxliRef0U;          
    pWMVDec->m_ppxliRefVPreproc = pWMVDec->m_ppxliRef0V;
    
}

FORCEINLINE I16_WMV medianof3_X9 (I16_WMV a0, I16_WMV a1, I16_WMV a2)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(medianof3_X9);
	if (a0 > a1) {
		if (a1 > a2)
			return a1;
		else if (a0 > a2)
			return a2;
		else
			return a0;
	}
	else if (a0 > a2)
		return a0;
	else if (a1 > a2)
		return a2;
	else
		return a1;
}


Void_WMV find16x16MVpred_X9 (tWMVDecInternalMember *pWMVDec, CVector_X9 * pvecPredHalfPel, const CVector_X9* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry) 
{
	CVector_X9 vctCandMVHalfPel0, vctCandMVHalfPel1, vctCandMVHalfPel2;

    MBType wrongType;
    CVector_X9  vecPredHalfPel =  *pvecPredHalfPel;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(find16x16MVpred_X9);

    assert(pWMVDec->m_tFrmType != BVOP || pmbmd->m_mbType == BACKWARD || pmbmd->m_mbType == FORWARD);
    wrongType = ((pmbmd->m_mbType == BACKWARD)?FORWARD:BACKWARD);

	if (bLeftBndry)
		vctCandMVHalfPel0.x = vctCandMVHalfPel0.y = 0;
    else{
		if (pWMVDec->m_tFrmType == BVOP && ((pmbmd - 1)->m_dctMd == INTRA/*  || (pmbmd - 1)->m_mbType == wrongType*/))
			vctCandMVHalfPel0.x = vctCandMVHalfPel0.y = 0;
		else
        {
		    vctCandMVHalfPel0.x = (pmv - 1)->x;
            vctCandMVHalfPel0.y = (pmv - 1)->y;
        }
    }

	if (bTopBndry) {
		vecPredHalfPel.x = vctCandMVHalfPel0.x;
        vecPredHalfPel.y = vctCandMVHalfPel0.y;

        *pvecPredHalfPel = vecPredHalfPel ;

		return;
	}
	else {
#ifdef _WMV9AP_
            if ((pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA/*  || (pmbmd - pWMVDec->m_uintNumMBX)->m_mbType == wrongType*/))
#else
		    if (pWMVDec->m_tFrmType == BVOP && ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA/*  || (pmbmd - pWMVDec->m_uintNumMBX)->m_mbType == wrongType*/))
#endif
                vctCandMVHalfPel1.x = vctCandMVHalfPel1.y = 0;
            else 
#ifdef _WMV9AP_
                if (pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode)
#else
                if (pWMVDec->m_tFrmType == BVOP)
#endif
                {
                    vctCandMVHalfPel1.x = (pmv - pWMVDec->m_uintNumMBX)->x;
                    vctCandMVHalfPel1.y = (pmv - pWMVDec->m_uintNumMBX)->y;
                }
                else
                {
                    vctCandMVHalfPel1.x = (pmv - pWMVDec->m_iOffsetToTopMB)->x;
                    vctCandMVHalfPel1.y = (pmv - pWMVDec->m_iOffsetToTopMB)->y;
                }

		if (bRightBndry)
			vctCandMVHalfPel2.x = vctCandMVHalfPel2.y = 0;
        else{
#ifdef _WMV9AP_
                if ((pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) && ((pmbmd - pWMVDec->m_uintNumMBX + 1)->m_dctMd == INTRA/*  || (pmbmd - pWMVDec->m_uintNumMBX + 1)->m_mbType == wrongType*/))
#else
			    if (pWMVDec->m_tFrmType == BVOP && ((pmbmd - pWMVDec->m_uintNumMBX + 1)->m_dctMd == INTRA/*  || (pmbmd - pWMVDec->m_uintNumMBX + 1)->m_mbType == wrongType*/))
#endif
                    vctCandMVHalfPel2.x = vctCandMVHalfPel2.y = 0;
			    else
#ifdef _WMV9AP_
                    if (pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode)
#else
                    if (pWMVDec->m_tFrmType == BVOP)
#endif
                    {
                        vctCandMVHalfPel2.x = (pmv - pWMVDec->m_uintNumMBX + 1)->x;
                        vctCandMVHalfPel2.y = (pmv - pWMVDec->m_uintNumMBX + 1)->y;
                    }
                    else
                    {
                        vctCandMVHalfPel2.x = (pmv - pWMVDec->m_iOffsetToTopMB + 1)->x;
                        vctCandMVHalfPel2.y = (pmv - pWMVDec->m_iOffsetToTopMB + 1)->y;
                    }
        }
	}
	vecPredHalfPel.x = medianof3_X9 (vctCandMVHalfPel0.x, vctCandMVHalfPel1.x, vctCandMVHalfPel2.x);
	vecPredHalfPel.y = medianof3_X9 (vctCandMVHalfPel0.y, vctCandMVHalfPel1.y, vctCandMVHalfPel2.y);


     *pvecPredHalfPel = vecPredHalfPel ;
}

I32_WMV decodeMBOverheadOfBVOP_FieldPicture (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY)
{
#ifndef PPCWMP
    I32_WMV iCBPCY = 0;
   // I32_WMV iPredX, iPredY;
    Bool_WMV bCBPPresent, bMVPresent;
    I32_WMV iInterpMV = 0;

    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;

    // Decode MB mode
    if (ICERR_OK != DecodeMBModeFieldPicture (pWMVDec, pmbmd, &bMVPresent, &bCBPPresent))
        return ICERR_ERROR;;

	
    if (pmbmd->m_chMBMode == MB_INTRA)
            pmbmd->m_mbType = BACKWARD; 
	else
	{
		if (pWMVDec->m_iDirectCodingMode == SKIP_RAW) 
		{
			if (BS_getBits(pWMVDec->m_pbitstrmIn, 1))
				pmbmd->m_mbType = FORWARD;
			else
				pmbmd->m_mbType = (MBType)0;
		}

		if (pmbmd->m_mbType == (MBType)0)
		{
			if (pmbmd->m_chMBMode == MB_4MV || BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 0)
				pmbmd->m_mbType = BACKWARD;
			else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 0)
				pmbmd->m_mbType = DIRECT;
			else
			{
				pmbmd->m_mbType = INTERPOLATE; 
				iInterpMV = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
			}
		}
	}

    pmbmd->m_bCBPAllZero = !bCBPPresent;

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT

    if (pmbmd->m_chMBMode == MB_INTRA) {
        pWMVDec->m_pDiffMV->iIntra = 1;
        
            if (pWMVDec->m_bDQuantOn)
                if (ICERR_OK != decodeDQuantParam (pWMVDec, pmbmd))
                    return ICERR_ERROR;
            if (bCBPPresent) {
            pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);

            iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufICBPCYDec,pWMVDec->m_pbitstrmIn) + 1;
		}
        else {
			iCBPCY = 0;
            pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        }
    }
    else {
        // Inter-coded MB
       // I32_WMV iStride;
        if (pmbmd->m_chMBMode == MB_1MV) {
            // 1MV MB
            pWMVDec->m_pDiffMV->iIntra = 0;
            if ((pWMVDec->m_tFrmType != BVOP || pmbmd->m_mbType != DIRECT) && bMVPresent) {
                if (pWMVDec->m_bTwoRefPictures)
                    decodeMV_Interlace(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
                else {
                    decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
                    pWMVDec->m_pDiffMV->iY <<= 1;
                }
            }
            else
                memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV));
            // hybrid MV
            pWMVDec->m_pDiffMV->iHybrid = 0;
            if (pWMVDec->m_tFrmType == BVOP && pmbmd->m_mbType == INTERPOLATE && iInterpMV)
            {
                if (pWMVDec->m_bTwoRefPictures)
                    decodeMV_Interlace(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + 1);
                else {
                    decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + 1);
                    (pWMVDec->m_pDiffMV+1)->iY <<= 1;
                }

                (pWMVDec->m_pDiffMV + 1)->iHybrid = 0;
            }
            
        }
        else {
            I32_WMV iMV;
            I32_WMV iblk;
            // 4MV MB
			if (pWMVDec->m_tFrmType == BVOP)
				assert(pmbmd->m_mbType == FORWARD || pmbmd->m_mbType == BACKWARD);
            
            memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV) * 4);
            iMV = Huffman_WMV_get(pWMVDec->m_p4MVBP,pWMVDec->m_pbitstrmIn);

            for ( iblk = 0; iblk < 4; iblk++) {
                (pWMVDec->m_pDiffMV + iblk)->iIntra = 0;
                if (iMV & (1 << (3 - iblk))) {
                    if (pWMVDec->m_bTwoRefPictures)
                         decodeMV_Interlace(pWMVDec, pWMVDec->m_pHufMVTable_V9, (pWMVDec->m_pDiffMV + iblk));
                    else {
                        decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, (pWMVDec->m_pDiffMV + iblk));
                        (pWMVDec->m_pDiffMV + iblk)->iY <<= 1;
                    }
                }
                else
                    memset (pWMVDec->m_pDiffMV + iblk, 0, sizeof (CDiffMV));
                // hybrid MV
                (pWMVDec->m_pDiffMV + iblk)->iHybrid = 0;
            
            }
        }

        // Decode DCT CBP
        if (bCBPPresent)
            iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufPCBPCYDec,pWMVDec->m_pbitstrmIn) + 1;
        else
            iCBPCY = 0;

        if (iCBPCY) {
            // DQuant
            if (pWMVDec->m_bDQuantOn) {
                if (ICERR_OK != decodeDQuantParam (pWMVDec, pmbmd))
                    return ICERR_ERROR;
            }

           

            // MB transform type
            if (pWMVDec->m_bMBXformSwitching) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufMBXformTypeDec,pWMVDec->m_pbitstrmIn);
                pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
                pmbmd->m_iMBXformMode = s_iMBXformTypeLUT[iIndex];
                pmbmd->m_iSubblockPattern = s_iSubblkPatternLUT[iIndex];
                pmbmd->m_bFirstBlockDecoded = FALSE;
            }
        }
    }

    setCodedBlockPattern (pmbmd, V_BLOCK, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, U_BLOCK, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, Y_BLOCK4, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, Y_BLOCK3, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, Y_BLOCK2, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern (pmbmd, Y_BLOCK1, iCBPCY & 0x1);

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
		return ICERR_ERROR;
	}

#endif //PPCWMP

    return ICERR_OK;
}


#else
tWMVDecodeStatus decodeB_Deblock (tWMVDecInternalMember *pWMVDec)
{  return WMV_Failed; }
tWMVDecodeStatus DecodeBMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY,
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    CoordI xIndex, CoordI yIndex,
    CoordI xMv, CoordI yMv,
    CoordI xMv2, CoordI yMv2
)
{  return WMV_Failed; }
#endif //WMV9_SIMPLE_ONLY 

