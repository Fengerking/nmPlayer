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

#ifdef _EMB_WMV3_

/*
FORCEINLINE 	Bool_WMV getCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn) 
{ 
	return pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1];
}
*/

FORCEINLINE Void_WMV  setCodedBlockPattern_EMB (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) 
{
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1] =(U8_WMV) bisCoded;
    //pmbmd->m_rgbCodedBlockPattern |= bisCoded << (blkn - 1);

}

/****************************************************************************************
  decodeB_Deblock : B frame stuff
****************************************************************************************/
////#pragma code_seg (EMBSEC_BML)

tWMVDecodeStatus decodeB_EMB (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus result;
    
    U8_WMV __huge* ppxliCurrQY;
    U8_WMV __huge* ppxliCurrQU;
    U8_WMV __huge* ppxliCurrQV;
    
    U8_WMV __huge* ppxliRef1Y ;
    U8_WMV __huge* ppxliRef1U;
    U8_WMV __huge* ppxliRef1V ;
    
    U8_WMV __huge* ppxliRef0Y ;
    U8_WMV __huge* ppxliRef0U;
    U8_WMV __huge* ppxliRef0V ;
    I32_WMV  iPixliY,  iPixliUV ;
    
    U8_WMV* rgchSkipPrevFrame ;
    
    CWMVMBMode* pmbmd ;
    CWMVMBMode* pmbmd1 ;
    
   // CVector_X9* pmv ;
    CMotionVector_X9_EMB* pmv1 ;
    
    
    CoordI x, y; 
    I32_WMV  imbX, imbY, iMB;
    I16_WMV* piQuanCoefACPred ;
    I16_WMV** piQuanCoefACPredTable ;
    //U8_WMV** piQuanCoef_IMBInPFrame_DCPredTable ;
    U32_WMV uiNumMBFromSliceBdry ;
    EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop ;
    EMB_PBMainLoop * pMainLoop = (EMB_PBMainLoop *)pBMainLoop ;
    
    
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
    
    //pmv = pWMVDec->m_rgmv_X9;
    pmv1 = pWMVDec->m_rgmv1_EMB;
    
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
    // memset (pWMVDec->m_pX9dct, 0, pWMVDec->m_uintNumMB * 16 * 6 * sizeof(I16_WMV));
    
   //ALIGNED32_FASTMEMCLR (pWMVDec->m_pX9dct, 0, pWMVDec->m_uintNumMB * 32 * 6 * sizeof(I16_WMV));

     SetupMVDecTable_EMB(pWMVDec);
    
    SetupEMBBMainLoopCtl(pWMVDec);

    iPixliY = 0;
    iPixliUV = 0;

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

        //Bool_WMV bTopBndry = (imbY == 0);
        //Bool_WMV bNot1stRowInPict = (imbY != 0);
       // Bool_WMV bNot1stRowInSlice = (prvWMVModulus ((I32_WMV)(imbY), (I32_WMV)(pWMVDec->m_uintNumMBY)) != 0);
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBY_LoopCount");

        pWMVDec->m_EMB_BMainLoop.PB.m_iFrmMBOffset[0] = iPixliY;
        pWMVDec->m_EMB_BMainLoop.PB.m_iFrmMBOffset[1] = iPixliUV;
        
        pWMVDec->m_EMB_BMainLoop.PB.m_iBlkMBBase[X_INDEX] = 0;
      //  pWMVDec->m_EMB_BMainLoop.PB.m_PredictMVLUT.U.U.m_pBOffset = pWMVDec->m_EMB_BMainLoop.PB.m_PredictMVLUT.m_rgcBOffset;
     
        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;
        if (!(imbY & 0x01)){ 
            piQuanCoefACPred = pWMVDec->m_rgiQuanCoefACPred;
            piQuanCoefACPredTable= pWMVDec->m_rgiQuanCoefACPredTable;
            //pmv = pWMVDec->m_rgmv_X9;
        }
        
        
        pWMVDec->m_rgiDCRef [0] = pWMVDec->m_rgiDCRef [1] = pWMVDec->m_rgiDCRef [2] = 128;
        
        
        for (x = 0, imbX = 0; imbX < (I32_WMV) pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE, iMB++) {
           
           
           // DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBX_LoopCount");
            //pmv->x = pmv->y = 0;
            
            
            if (pWMVDec->m_iPrevIFrame == 1)
                pmv1->m_vctTrueHalfPel.I32 = 0;
                //pmv1->m_vctTrueHalfPel.x = pmv1->m_vctTrueHalfPel.y = 0;
            
            uiNumMBFromSliceBdry ++;
            pmbmd->m_bCBPAllZero = FALSE;
            
            // if skip set diff MV to zero
            memset (pMainLoop->m_pDiffMV_EMB, 0, 2 * sizeof(CDiffMV_EMB));
            
            result = decodeMBOverheadOfBVOP_WMV3_EMB (pBMainLoop, pmbmd, imbX, imbY);
            
          //  if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR) {
            if(pMainLoop->m_bHalfPelMV)
            {
                pMainLoop->m_pDiffMV_EMB->Diff.I16[0] <<= 1;
                pMainLoop->m_pDiffMV_EMB->Diff.I16[1] <<= 1;
                (pMainLoop->m_pDiffMV_EMB+1)->Diff.I16[0] <<= 1;
                (pMainLoop->m_pDiffMV_EMB+1)->Diff.I16[1] <<= 1;
            }
            
            if (WMV_Succeeded != result) {
                FUNCTION_PROFILE_STOP(&fp);
                return result;
            }
            
            *rgchSkipPrevFrame = 0;
            pmbmd->m_chMBMode = MB_1MV;
            
            if (pMainLoop->m_pDiffMV_EMB->iIntra == 0)
            {
                UMotion_EMB defPred[2];

                if (pmbmd->m_bCBPAllZero == TRUE)
                {
                    memset(pmbmd->m_rgcBlockXformMode, XFORMMODE_8x8, 6);
                }



             //   DirectModeMV_EMB ( pBMainLoop, pmv1->m_vctTrueHalfPel.x, pmv1->m_vctTrueHalfPel.y, (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR),  defPred);

                {
                    I32_WMV  iScaleFactor = pBMainLoop->m_iScaleFactor;  // can be pulled into frame level
                    I32_WMV iXMotion ;
                    I32_WMV iYMotion ;
                    
                     iXMotion = pmv1->m_vctTrueHalfPel.I16[X_INDEX];
                     iYMotion = pmv1->m_vctTrueHalfPel.I16[Y_INDEX];
                    
                    
                    // check if not intra (later)
                    if (pBMainLoop->m_bHalfPelBiLinear) {
                        defPred[0].I16[X_INDEX] = 2 * ((iXMotion * iScaleFactor + 255) >> 9);
                        defPred[0].I16[Y_INDEX] = 2 * ((iYMotion * iScaleFactor + 255) >> 9); 
                        defPred[1].I16[X_INDEX] = 2 * ((iXMotion * (iScaleFactor - 256) + 255) >> 9);
                        defPred[1].I16[Y_INDEX] = 2 * ((iYMotion * (iScaleFactor - 256) + 255) >> 9);
                    }
                    else {
                        defPred[0].I16[X_INDEX] = (iXMotion * iScaleFactor + 128) >> 8;
                        defPred[0].I16[Y_INDEX] = (iYMotion * iScaleFactor + 128) >> 8; 
                        defPred[1].I16[X_INDEX] = (iXMotion * (iScaleFactor - 256) + 128) >> 8;
                        defPred[1].I16[Y_INDEX] = (iYMotion * (iScaleFactor - 256) + 128) >> 8;
                    }
                    
                    
                    defPred[0].I32 = PredictBMVPullBackCheck_EMB(pBMainLoop,  2, defPred[0].I32);
                    defPred[1].I32 = PredictBMVPullBackCheck_EMB(pBMainLoop,  2, defPred[1].I32);
                    
                    
                }

                
                if (pmbmd->m_mbType != DIRECT)
                {
                    I32_WMV iMask = FORWARD + 1 - pmbmd->m_mbType;
                    I32_WMV i, k;
                    UMotion_EMB pred[2];


                    if(pmbmd->m_mbType != INTERPOLATE)
                    {
                        pred[0].I32 = pMainLoop->m_pDiffMV_EMB[0].Diff.I32;
                        pred[1].I32 = pMainLoop->m_pDiffMV_EMB[1].Diff.I32 ;
                    }else
                    {
                        pred[1].I32 = pMainLoop->m_pDiffMV_EMB[0].Diff.I32;
                        pred[0].I32 = pMainLoop->m_pDiffMV_EMB[1].Diff.I32;
                    }


                    for(i = 0, k=0; i < 2; i++, iMask >>=1)
                    {
                        if(!(iMask & 1))
                        {
                            //pMainLoop->m_pDiffMV_EMB[i].Diff.I32 = 0;
                            pBMainLoop->m_pMotionArray[i][iMB].I32 = defPred[i].I32;
                            continue;
                        }

                        pBMainLoop->m_pMotionCurr = pBMainLoop->m_pMotionArray[i];

                        (*pBMainLoop->m_ppPredictMV_EMB[0]) (pBMainLoop, pMainLoop->m_iBlkMBBase[X_INDEX] ,  TRUE_WMV, 0);

                        
                         if (!pmbmd->m_bSkip)
                        {
                            pMainLoop->m_pDiffMV_EMB[i].Diff.I16[X_INDEX] = ((pMainLoop->m_iPred.I16[X_INDEX] + pred[k].I16[X_INDEX] + pMainLoop->m_iXMVRange) & pMainLoop->m_iXMVFlag) - pMainLoop->m_iXMVRange;
                            pMainLoop->m_pDiffMV_EMB[i].Diff.I16[Y_INDEX] = ((pMainLoop->m_iPred.I16[Y_INDEX] + pred[k].I16[Y_INDEX] + pMainLoop->m_iYMVRange) & pMainLoop->m_iYMVFlag) - pMainLoop->m_iYMVRange;
                        }
                        else
                        {
                            pMainLoop->m_pDiffMV_EMB[i].Diff.I32 = pMainLoop->m_iPred.I32;
                        }


                        pBMainLoop->m_pMotionArray[i][iMB].I32 = pMainLoop->m_pDiffMV_EMB[i].Diff.I32;
                        pMainLoop->m_pDiffMV_EMB[i].Diff.I32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 0, pMainLoop->m_pDiffMV_EMB[i].Diff.I32);


                        k++;

                    }

                }
                else
                {
                   
                    pBMainLoop->m_pMotionArray[0][iMB].I32 = defPred[0].I32;
                    pBMainLoop->m_pMotionArray[1][iMB].I32 = defPred[1].I32;
                    pMainLoop->m_pDiffMV_EMB[0].Diff.I32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 0, defPred[0].I32);
                    pMainLoop->m_pDiffMV_EMB[1].Diff.I32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 0, defPred[1].I32);

                }

                 result = DecodeBMB_EMB_Fast(pBMainLoop,  pmbmd,  imbX, imbY);

  
                if (WMV_Succeeded != result) {
                    FUNCTION_PROFILE_STOP(&fp);
                    
                    return result;
                }
            }
            else {      // INTRA
                
                 pMainLoop->m_pMotion[iMB].I32 = IBLOCKMV;
                 pMainLoop->m_pMotionC[iMB].I32 = IBLOCKMV;
                      
                
            result =  DecodeBMB_EMB_Intra( pBMainLoop, pmbmd, imbX, imbY );

                if (WMV_Succeeded != result) {
                    FUNCTION_PROFILE_STOP(&fp);
                    
                    return result;
                }
            }
            
            //UPDATE_BMP:
            // If loopfilter is on, color convert and copying to Post waits till loopfilter is complete
            if (pWMVDec->m_bLoopFilter) 
                goto NEXT_MB;
            
            
NEXT_MB:
            if (pmbmd->m_dctMd == INTER) {
                piQuanCoefACPred[0] = piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES4] =piQuanCoefACPred[BLOCK_SIZE_TIMES2_TIMES5] = 0;
            }

           pMainLoop->m_rgiCurrBaseBK[0] += 2;
           pMainLoop->m_rgiCurrBaseBK[1] ++;
         
           pMainLoop->m_iBlkMBBase[X_INDEX] += 2;
            
           pMainLoop->m_iFrmMBOffset[0] += MB_SIZE;
           pMainLoop->m_iFrmMBOffset[1] += BLOCK_SIZE;           
       //     pWMVDec->m_EMB_BMainLoop.PB.m_PredictMVLUT.U.U.m_pBOffset = pWMVDec->m_EMB_BMainLoop.PB.m_PredictMVLUT.m_rgcBOffset + 5;
            
        //    if(pWMVDec->m_EMB_BMainLoop.PB.m_iBlkMBBase[X_INDEX] == (pWMVDec->m_EMB_BMainLoop.PB.m_iNumBlockX - 2))
        //        pWMVDec->m_EMB_BMainLoop.PB.m_PredictMVLUT.U.U.m_pBOffset = pWMVDec->m_EMB_BMainLoop.PB.m_PredictMVLUT.m_rgcBOffset + 10;
            
            rgchSkipPrevFrame++;
            pmbmd++;
          //  pmv++;
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
        

        pBMainLoop->m_ppPredictMV_EMB = pBMainLoop->m_rgfPredictMV_EMB + 1;

        
        pMainLoop->m_iBlkMBBase[Y_INDEX] += 2;
        pMainLoop->m_rgiCurrBaseBK[0] += pWMVDec->m_uintNumMBX<<1;
        iPixliY  += pWMVDec->m_iMBSizeXWidthPrevY;
        iPixliUV += pWMVDec->m_iBlkSizeXWidthPrevUV;

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
    //DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MB_Loop_Done");
    
    
#ifndef WMV9_SIMPLE_ONLY
    
    if (pWMVDec->m_bLoopFilter)
    {
        
#ifdef _WMV9AP_
        if (pWMVDec->m_bLoopFilter) {
            ComputeLoopFilterFlags_WMV9(pWMVDec);
#else
        if (pWMVDec->m_bV9LoopFilter) {
            ComputeLoopFilterFlags(pWMVDec);
#endif
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
        else
            DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            FALSE, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
        
    }    
#endif

    pWMVDec->m_bDisplay_AllMB = TRUE; 
    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;    
    
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame
    pWMVDec->m_iFilterType = FILTER_BICUBIC;
    
    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}

//#pragma code_seg (EMBSEC_BML2)
Bool_WMV decodeDCTPredictionB_EMB (EMB_BMainLoop * pBMainLoop,
                                   CWMVMBMode *pmbmd, I16_WMV *pIntra, I32_WMV iblk,
                                   I32_WMV iX, I32_WMV iY, I32_WMV *piShift, I32_WMV *iDirection, I16_WMV *pPredScaled)

{
    
    Bool_WMV bACPredOn = TRUE_WMV;
    I32_WMV iUV = iblk >>2;
    //EMB_PBMainLoop * pMainLoop = ( EMB_PBMainLoop *) pBMainLoop;
    I32_WMV iStride = pBMainLoop->PB.m_iNumBlockX >> iUV;
    Bool_WMV   iShift = 0; //= *piShift;
    I32_WMV  iWidth  = pBMainLoop->PB.m_iNumBlockX >> 1 ;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    UMotion_EMB *pMotion = pBMainLoop->PB.m_pMotion;
   // I32_WMV iIndex = iX + (iY * pWMVDec->m_uintNumMBX << 1);
    I32_WMV iIndex = pBMainLoop->PB.m_rgiCurrBaseBK[1];

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDCTPrediction_EMB);

    //assert(pWMVDec->m_tFrmType != BVOP);
 
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;

    if(iUV) pMotion = pBMainLoop->PB.m_pMotionC;
    
 //   if ((iY && (pWMVDec->m_pMotion[iIndex - iWidth].I16[X_INDEX] == IBLOCKMV))) 
     if ((iY & !iUV)||(iY && (pMotion[iIndex - iWidth].I32 == IBLOCKMV))) 
    {    
        pPred = pPredIntraTop = pIntra - 16 * iStride;// + pWMVDec->m_iACRowPredOffset;
        iShift = pBMainLoop->PB.m_iACRowPredShift;
    }
    
     if(iblk > 4) iblk = 4;
  //  if ((iX && (pWMVDec->m_pMotion[iIndex - 1].I16[X_INDEX] == IBLOCKMV))) 
     if ((iX & !iUV)||(iX && (pMotion[iIndex - 1].I32 == IBLOCKMV))) 
    {
        pPred = pPredIntraLeft = pIntra - 16;//pWMVDec->m_iACColPredOffset;
        iShift = pBMainLoop->PB.m_iACColPredShift;
    }
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
        I32_WMV iDiff0, iDiff1;
        if (((iX | iY) & !iUV) || (pMotion[(iIndex - iWidth - 1)].I32== IBLOCKMV) ) {                
            iTopLeftDC = pPredIntraTop[pBMainLoop->PB.m_iACRowPredOffset - 16];
        }
        iTopDC = pPredIntraTop[pBMainLoop->PB.m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pBMainLoop->PB.m_iACColPredOffset];                                   
        ScaleDCPredForDQuant (pBMainLoop->PB.m_pWMVDec, iblk, pmbmd, &iTopDC, &iLeftDC, &iTopLeftDC);
        
        iDiff0 = iTopLeftDC - iLeftDC;
        iDiff1 = iTopLeftDC - iTopDC;
        if (abs (iDiff0) < abs (iDiff1)) {
            pPred = pPredIntraTop;
            iShift = pBMainLoop->PB.m_iACRowPredShift;
        }
    }
    
    // preferred direction for 4MV
 //   assert(pWMVDec->m_bFieldCodingOn == 0);
    
    //  if (pPred != NULL && !(pmbmd->m_rgbDCTCoefPredPattern2[0]))
   //    bACPredOn = FALSE_WMV;
    
    if (pPred != NULL_WMV) {

        if(!pmbmd->m_rgbDCTCoefPredPattern2[0])
            bACPredOn = FALSE_WMV;

        bDCACPred = TRUE_WMV;
        if (iShift == pBMainLoop->PB.m_iACColPredShift) {
            ScaleLeftPredForDQuant (pBMainLoop->PB.m_pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        } else {
            ScaleTopPredForDQuant (pBMainLoop->PB.m_pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
    *iDirection = (pPred == pPredIntraTop);
    *piShift =  iShift ;
    return bDCACPred;
}

//#pragma code_seg (EMBSEC_BML2)
tWMVDecodeStatus DecodeBMB_EMB_Intra(EMB_BMainLoop  * pBMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY
                                   )
{
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    UnionBuffer * ppxliErrorQMB;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  iShift, iblk, iDirection;
    //Bool_WMV bSkipMB = TRUE_WMV;
    I16_WMV  *pIntra, pPredScaled[16];
   // I32_WMV iCurrMB;
    U8_WMV * pDst;
    I32_WMV iUV;
   

    //I32_WMV iXformType = pBMainLoop->PB.m_iFrameXformMode;
    DQuantDecParam *pDQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeMB_X9_Fast);
    
    pDQ = &pBMainLoop->PB.m_prgDQuantParam [pmbmd->m_iQP ];
    
    //    FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);   
    
    if (pBMainLoop->PB.m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pBMainLoop->PB.m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pBMainLoop->PB.m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else {
        ppInterDCTTableInfo_Dec = pBMainLoop->PB.m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pBMainLoop->PB.m_ppIntraDCTTableInfo_Dec;
    }
    

    for (iblk = 0; iblk < 6; iblk++) {
        Bool_WMV bResidual = rgCBP2[iblk];
        iUV = iblk >> 2;    
        
        pDst = pBMainLoop->PB.m_ppxliCurrBlkBase[iblk] + pBMainLoop->PB.m_iFrmMBOffset[iUV];
      
        {
            I32_WMV iX;
            I32_WMV iY;

            Bool_WMV bDCACPredOn;
            I16_WMV *pPred = NULL_WMV;
            CDCTTableInfo_Dec** ppDCTTableInfo_Dec;
            
            if(!iUV) {
                iX = (iMBX << 1) + (iblk & 1);
                iY = (iMBY << 1) + ((iblk & 2) >> 1);
                
                pIntra = pBMainLoop->PB.m_pX9dct + ((pBMainLoop->PB.m_rgiCurrBaseBK[0] + pBMainLoop->PB.m_rgiBlkIdx[iblk]) << 4);
                ppDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec;
            }
            else
            {
                iX = iMBX; iY = iMBY;
                
                pIntra = pBMainLoop->PB.m_pX9dctUV[iblk-4] + ((pBMainLoop->PB.m_rgiCurrBaseBK[1]) << 4);               
                ppDCTTableInfo_Dec = ppInterDCTTableInfo_Dec;
            }
            
            bDCACPredOn = decodeDCTPredictionB_EMB (pBMainLoop, pmbmd, pIntra, iblk, iX, iY, 
                    &iShift, &iDirection, pPredScaled);
            
            ppxliErrorQMB = pBMainLoop->PB.m_rgiCoefReconBuf;
            if (bDCACPredOn) pPred = pPredScaled;
            result = (tWMVDecodeStatus)(result | DecodeInverseIntraBlockX9_EMB (pBMainLoop->PB.m_pWMVDec,
                ppDCTTableInfo_Dec,
                iblk,
                pBMainLoop->PB.m_pZigzagScanOrder, bResidual, pPred,
                iDirection, iShift, pIntra, pmbmd, pDQ)); //get the quantized block
            
            pBMainLoop->PB.m_iDCTHorzFlags= pBMainLoop->PB.m_pWMVDec->m_iDCTHorzFlags;

#ifdef _EMB_SSIMD_B_
            
            assert(pBMainLoop->PB.m_pWMVDec->m_pIntraX9IDCT_Dec == g_IDCTDec16_WMV3);
            
            g_IDCTDec16_WMV3_Fun ( ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pBMainLoop->PB.m_iDCTHorzFlags );

            /*Should deal with the same situation here when in SSIMDxxx */
#           ifdef _EMB_SSIMD64_
                {
                    I64_WMV ii;
                    U64_WMV  * pData64 = (U64_WMV  *) pBMainLoop->m_rgIntraMotionCompBuffer_EMB;
                    for ( ii = 0; ii < 8; ii++) {
                        pData64[0] = pData64[3] = 0x0080008000800080;
                        pData64 += 6;
                    }
                }
#           endif

            g_AddError_SSIMD_Fun(pDst, pBMainLoop->m_rgIntraMotionCompBuffer_EMB, ppxliErrorQMB->i32, pBMainLoop->PB.m_iWidthPrev[iUV]);

#else
            //TBD
            assert(0);
            
#endif //_EMB_SSIMD_B_
            
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
            
        }
    }

    pmbmd->m_bSkip = FALSE_WMV;

    return result ;
}

//static int g_mc_cnt[4][4];
//#pragma code_seg (EMBSEC_BML)
Void_WMV g_MotionCompB_EMB (EMB_BMainLoop *pBMainLoop,  U8_WMV * pDest,  I32_WMV iRefIdx, I32_WMV iYUV)
{
    I32_WMV iUV = iYUV>0;
    Bool_WMV b1MV = !iUV;
    I32_WMV  iFilterType = pBMainLoop->PB.m_iFilterType[iUV];
    U8_WMV  *pRef;
    CDiffMV_EMB * pDiffMV = pBMainLoop->PB.m_pDiffMV_EMB + iRefIdx + (iYUV<<1);
    // I32_WMV  k = (iBlockY * pWMVDec->m_uintNumMBX << 1) + iBlockX;
    I32_WMV  iShiftX = pDiffMV->Diff.I16[X_INDEX];
    I32_WMV  iShiftY = pDiffMV->Diff.I16[Y_INDEX];

    I32_WMV iXFrac, iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_MotionCompB_EMB);

    pRef = pBMainLoop->PB.m_ppxliRefBlkBase[iRefIdx + (iYUV<<1)] + pBMainLoop->PB.m_iFrmMBOffset[iUV];

    pRef = pRef + (iShiftY >> 2) * pBMainLoop->PB.m_iWidthPrev[iUV] + (iShiftX >> 2);
    
    iXFrac = iShiftX&3;
    iYFrac = iShiftY&3;
    
    //g_dbg_cnt++;

 //   g_mc_cnt[iXFrac][iYFrac]++;

    if(iFilterType == FILTER_BICUBIC)
    {
        I_SIMD ret;

        ret = (*pBMainLoop->PB.m_pInterpolateBicubic[iXFrac][iYFrac]) 
                (pRef, pBMainLoop->PB.m_iWidthPrev[iUV], pDest, (EMB_PBMainLoop *)pBMainLoop, iXFrac, iYFrac, b1MV, pBMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV]);

        if(ret) {
            g_InterpolateBlockBicubicOverflow_EMB(pRef, pBMainLoop->PB.m_iWidthPrev[iUV], pDest,  iXFrac,  iYFrac,  pBMainLoop->PB.m_iRndCtrl, b1MV);
        }
    }
    else
    {
        if(iXFrac|iYFrac)
            (*pBMainLoop->PB.m_pInterpolateBlockBilinear[iXFrac][iYFrac])
                ( pRef, pBMainLoop->PB.m_iWidthPrev[iUV], pDest, iXFrac, iYFrac, pBMainLoop->PB.m_iRndCtrl, b1MV);
        else
#           if defined(_EMB_SSIMD32_) || defined(_EMB_SSIMD64_)
                g_InterpolateBlock_00_SSIMD_Fun(pRef, pBMainLoop->PB.m_iWidthPrev[iUV], pDest, (EMB_PBMainLoop *)pBMainLoop, iXFrac, iYFrac,  b1MV, 0);
#           else // defined(_EMB_SSIMD32_) || defined(_EMB_SSIMD64_)
                g_InterpolateBlock_00_C(pRef, pBMainLoop->PB.m_iWidthPrev[iUV], pDest, (EMB_PBMainLoop *)pBMainLoop, iXFrac, iYFrac,  b1MV, 0);
#           endif //defined(_EMB_SSIMD32_) || defined(_EMB_SSIMD64_)
    }

}

//#pragma code_seg (EMBSEC_BML)
tWMVDecodeStatus DecodeBMB_EMB_Fast(EMB_BMainLoop  * pBMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY
                                   )
{
    //U8_WMV rgCBP = pmbmd->m_rgbCodedBlockPattern;
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    UnionBuffer * ppxliErrorQMB;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV  iblk;
    UMotion_EMB iPred;
    U8_WMV * pDst;
    I32_WMV iUV;
    //CDiffMV_EMB * pDiffMV = pBMainLoop->PB.m_pDiffMV_EMB;
    Bool_WMV bFirstBlock =  pBMainLoop->PB.m_bMBXformSwitching;
    U32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;

	
#ifdef _EMB_SSIMD64_
    U32_WMV ppxliInterpolate0YMB[192+96], ppxliInterpolate1YMB[192+96];
#else
    U32_WMV ppxliInterpolate0YMB[160+80], ppxliInterpolate1YMB[160+80];
#endif

    I32_WMV iXformType = pBMainLoop->PB.m_iFrameXformMode;
    DQuantDecParam *pDQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeMB_X9_Fast);
    
    pDQ = &pBMainLoop->PB.m_prgDQuantParam [pmbmd->m_iQP ];
    
    //    FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);
    
    if (pBMainLoop->PB.m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;
    
    
    if (pBMainLoop->PB.m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pBMainLoop->PB.m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pBMainLoop->PB.m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else {
        ppInterDCTTableInfo_Dec = pBMainLoop->PB.m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pBMainLoop->PB.m_ppIntraDCTTableInfo_Dec;
    }
    
    // ppxliErrorQMB = pBMainLoop->PB.m_rgiCoefReconBuf;

    pBMainLoop->PB.m_pSSIMD_DstOffSet = pBMainLoop->m_SSIMD_DstLUT_B;

    pBMainLoop->m_pMotionCompBuffer_EMB[FORWARD_EMB] = ppxliInterpolate0YMB;
    pBMainLoop->m_pMotionCompBuffer_EMB[BACKWARD_EMB] = ppxliInterpolate1YMB;

    {

       I32_WMV iMask = pBMainLoop->m_iMBTypeMask[pmbmd->m_mbType-1];
       int i;
       U8_WMV * pTbl = pBMainLoop->m_RndTbl;
       for(i = 0; i < 2; i++, iMask>>=1)
        {
           U32_WMV * pDest;
           I32_WMV iX, iY;
           

           if(!(iMask &1))
               continue;
           
            pDest = pBMainLoop->m_pMotionCompBuffer_EMB[i];
            g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)pDest, i, Y_EMB);
        

            iX = pBMainLoop->PB.m_pDiffMV_EMB[ i + (Y_EMB<<1)].Diff.I16[X_INDEX];
            iY = pBMainLoop->PB.m_pDiffMV_EMB[ i + (Y_EMB<<1)].Diff.I16[Y_INDEX];
            
            iX = (iX + pTbl[iX & 3]) >> 1;
            iY = (iY + pTbl[iY & 3]) >> 1;
    
            if (pBMainLoop->PB.m_bUVHpelBilinear) {
                if (iX > 0) iX -= iX & 1 ;
                else iX+=  iX & 1;
                if (iY > 0) iY -= iY & 1 ;
                else iY+=  iY & 1;
            }

            iPred.I16[X_INDEX] = iX;
            iPred.I16[Y_INDEX] = iY;

            
            pBMainLoop->PB.m_pDiffMV_EMB[ i + (U_EMB<<1)].Diff.U32 =
            pBMainLoop->PB.m_pDiffMV_EMB[ i + (V_EMB<<1)].Diff.U32 = iPred.U32;

#		ifdef _EMB_SSIMD64_
            g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)(pDest + 192), i, U_EMB);
            g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)(pDest + 194), i, V_EMB);
#		else
            g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)(pDest + 160), i, U_EMB);
            g_MotionCompB_EMB (pBMainLoop,  (U8_WMV *)(pDest + 162), i, V_EMB);
#		endif
        }

    }



    for (iblk = 0; iblk < 6; iblk++) {
        Bool_WMV bResidual = rgCBP2[iblk];
        iUV = iblk >> 2;
        pDst = pBMainLoop->PB.m_ppxliCurrBlkBase[iblk] + pBMainLoop->PB.m_iFrmMBOffset[iUV];
        
        if(bResidual)
        {
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pBMainLoop->PB.m_pHufBlkXformTypeDec, pBMainLoop->PB.m_pbitstrmIn);
                iXformType = pBMainLoop->PB.m_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = pBMainLoop->PB.m_iSubblkPatternLUT[iIndex];
            }
            
            pmbmd->m_rgcBlockXformMode[iblk] =(I8_WMV)iXformType;
            
            if (iXformType == XFORMMODE_8x8) {
                ppxliErrorQMB = pBMainLoop->PB.m_rgiCoefReconBuf;
#ifdef _EMB_SSIMD_B_
                result = (tWMVDecodeStatus)(result | g_DecodeInterError8x8_SSIMD_Fun((EMB_PBMainLoop  * )pBMainLoop,  ppInterDCTTableInfo_Dec, pDQ));
#else
                //TBD
                assert(0);
#endif
                
                   
            } else {
                I32_WMV i, iLoopTop, iLoopMask, iPattern;
                //U8_WMV* pZigzag;
                I32_WMV iIdx;
                
                // Void_WMV (*p_XxYIDCTDec_WMV3_EMB) (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf, I32_WMV iDCTHorzFlags);
                I32_WMV (*p_DecodeInterError)(EMB_PBMainLoop  * pBMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
                
                ppxliErrorQMB = pBMainLoop->PB.m_ppxliErrorQ;
                //    ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  
                
                if (iXformType == XFORMMODE_8x4 || iXformType == XFORMMODE_4x8) {
                    if (pmbmd->m_bBlkXformSwitchOn || bFirstBlock) {
                        iPattern = iSubblockPattern;
                    }
                    else {
                        if (BS_getBit(pBMainLoop->PB.m_pbitstrmIn) == 1) {
                            iPattern = BS_getBit(pBMainLoop->PB.m_pbitstrmIn) + 1;
                        } else
                            iPattern = 3;
                    }
                    iLoopTop = 2;
                    iLoopMask = 2;
                    
                }
                else {
                    iPattern = Huffman_WMV_get(pBMainLoop->PB.m_pHuf4x4PatternDec, pBMainLoop->PB.m_pbitstrmIn) + 1;
                    iLoopTop = 4;
                    iLoopMask = 8;
                }
                
                iIdx = iXformType-1;
                rgCBP2[iblk] = (U8_WMV)iPattern;
                //pZigzag =  pBMainLoop->PB.m_pZigzag[iIdx];
                //p_XxYIDCTDec_WMV3_EMB = pBMainLoop->PB.m_pXxYIDCTDec_WMV3_EMB[iIdx];
                
                p_DecodeInterError = pBMainLoop->PB.m_pDecodeInterError[iIdx];
                
                
                for( i=0; i<iLoopTop; i++ ) {
                    if ( iPattern & (iLoopMask>>i) ) {
                        //     result |= DecodeInverseInterBlockQuantize16_EMB(pBMainLoop->PB.m_pWMVDec, ppInterDCTTableInfo_Dec, pZigzag, iXformType, pDQ);
                        //     (*p_XxYIDCTDec_WMV3_EMB) (ppxliErrorQMB, BLOCK_SIZE, pBMainLoop->PB.m_rgiCoefReconBuf, i, pBMainLoop->PB.m_pWMVDec->m_iDCTHorzFlags);
                        result = (tWMVDecodeStatus)(result | (*p_DecodeInterError)((EMB_PBMainLoop  *)pBMainLoop, ppInterDCTTableInfo_Dec, pDQ,  i));
                    }
                    else
                    {
                        g_SubBlkIDCTClear_EMB(ppxliErrorQMB, pBMainLoop->PB.m_uiIDCTClearOffset[iIdx][i]);
                        
                    }
                }
            }
            
#ifdef _EMB_SSIMD_B_
            if(pmbmd->m_mbType == DIRECT || pmbmd->m_mbType == INTERPOLATE)
                g_AddErrorB_SSIMD(pDst, pBMainLoop->m_pMotionCompBuffer_EMB[FORWARD_EMB],  pBMainLoop->m_pMotionCompBuffer_EMB[BACKWARD_EMB], ppxliErrorQMB->i32, pBMainLoop->PB.m_iWidthPrev[iUV], pBMainLoop->PB.m_pSSIMD_DstOffSet[iblk]);
            else
                g_AddError_SSIMD_Fun(pDst, pBMainLoop->m_pMotionCompBuffer_EMB[FORWARD - pmbmd->m_mbType ] + pBMainLoop->PB.m_pSSIMD_DstOffSet[iblk], ppxliErrorQMB->i32, pBMainLoop->PB.m_iWidthPrev[iUV]);
#else
            //TBD
            assert(0);
#endif
            bFirstBlock = FALSE;
        }
        else {
           
#ifdef _EMB_SSIMD_B_
            if(pmbmd->m_mbType == DIRECT || pmbmd->m_mbType == INTERPOLATE)
                g_AddNullB_SSIMD(pDst, pBMainLoop->m_pMotionCompBuffer_EMB[FORWARD_EMB],  pBMainLoop->m_pMotionCompBuffer_EMB[BACKWARD_EMB], pBMainLoop->PB.m_pSSIMD_DstOffSet[iblk], pBMainLoop->PB.m_iWidthPrev[iUV]);
            else
                g_AddNull_SSIMD_Fun(pDst, pBMainLoop->m_pMotionCompBuffer_EMB[FORWARD - pmbmd->m_mbType] + pBMainLoop->PB.m_pSSIMD_DstOffSet[iblk], pBMainLoop->PB.m_iWidthPrev[iUV]);
#else
            //TBD
            assert(0);
#endif
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
        }
        
    }


    
   
    
    return result;
}

/****************************************************************************************
  DecodeBMB : B macroblock level decoding
****************************************************************************************/


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

//#pragma code_seg (EMBSEC_BML)

tWMVDecodeStatus decodeMBOverheadOfBVOP_WMV3_EMB (EMB_BMainLoop * pBMainLoop,CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY)
{
    I32_WMV iCBPCY = 0;//, iPos=0, iTemp=0;
    Bool_WMV bSendTableswitchMode = FALSE;
    Bool_WMV bSendXformswitchMode = FALSE;
    //EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop.PB;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMBOverheadOfBVOP_WMV3_EMB);
    
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;
    pmbmd->m_chMBMode = MB_1MV; // always for B
    
    pmbmd->m_iQP = pBMainLoop->PB.m_iQP; //(2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT
    
   
    
    if (pBMainLoop->m_iDirectCodingMode == SKIP_RAW) 
        pmbmd->m_mbType = (MBType)BS_getBit(pBMainLoop->PB.m_pbitstrmIn);
    
    if (pBMainLoop->PB.m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pBMainLoop->PB.m_pbitstrmIn);
    }
    
    bSendTableswitchMode = FALSE;
    bSendXformswitchMode = FALSE;
    
    // 1MV - read motion vector
    
    if (pmbmd->m_mbType == DIRECT)
        pBMainLoop->PB.m_pDiffMV_EMB->iLast = 1;		// else would have skipped
    else {
        if (!pmbmd->m_bSkip)
            decodeMV_V9_EMB((EMB_PBMainLoop *)pBMainLoop, pBMainLoop->PB.m_pHufMVTable_V9,  pBMainLoop->PB.m_pDiffMV_EMB);    
        
        if (pmbmd->m_mbType == (MBType)0)
        {
            if (pBMainLoop->PB.m_pDiffMV_EMB->iIntra)
                pmbmd->m_mbType = BACKWARD;
            else if (BS_getBit(pBMainLoop->PB.m_pbitstrmIn) == 0)
                pmbmd->m_mbType = pBMainLoop->m_mbtShortBSymbol;
            else if (BS_getBit(pBMainLoop->PB.m_pbitstrmIn) == 0)
                pmbmd->m_mbType = pBMainLoop->m_mbtLongBSymbol;
            else
                pmbmd->m_mbType = INTERPOLATE;
        }
    }
    
    if (pmbmd->m_bSkip) {
        memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));
        return WMV_Succeeded;
    }
    
    // skip to end if last = 0
    if (pBMainLoop->PB.m_pDiffMV_EMB->iLast == 0) {
        pmbmd->m_bCBPAllZero = TRUE;
        if (pBMainLoop->PB.m_bDQuantOn && pBMainLoop->PB.m_pDiffMV_EMB->iIntra) {
            if (WMV_Succeeded != decodeDQuantParam (pBMainLoop->PB.m_pWMVDec, pmbmd)) {
                return WMV_Failed;
            }
        }
        // One MV Intra AC Prediction Flag
        if (pBMainLoop->PB.m_pDiffMV_EMB->iIntra)
            pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBit(pBMainLoop->PB.m_pbitstrmIn);              
        goto End;
    }
    
    if (pmbmd->m_mbType == INTERPOLATE)
    {
        decodeMV_V9_EMB((EMB_PBMainLoop *)pBMainLoop, pBMainLoop->PB.m_pHufMVTable_V9,  (pBMainLoop->PB.m_pDiffMV_EMB+1));    
        
        // INTRA is invalid in INTERPOLATE mode
        if ((pBMainLoop->PB.m_pDiffMV_EMB + 1)->iIntra || pBMainLoop->PB.m_pDiffMV_EMB->iIntra)
            return WMV_Failed;
        
        // skip to end if last = 0
        if ((pBMainLoop->PB.m_pDiffMV_EMB + 1)->iLast == 0) {
            pmbmd->m_bCBPAllZero = TRUE;
            goto End;
        }
    }
    
    bSendTableswitchMode = pBMainLoop->PB.m_bDCTTable_MB;
    bSendXformswitchMode = pBMainLoop->PB.m_bMBXformSwitching && !pBMainLoop->PB.m_pDiffMV_EMB->iIntra;
    // One MV Intra AC Prediction Flag
    if (pBMainLoop->PB.m_pDiffMV_EMB->iIntra)
        pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBit(pBMainLoop->PB.m_pbitstrmIn);              
    
    // read CBP
    iCBPCY = Huffman_WMV_get(pBMainLoop->PB.m_pHufNewPCBPCYDec, pBMainLoop->PB.m_pbitstrmIn);
    
    // decode DQUANT
    if (pBMainLoop->PB.m_bDQuantOn && WMV_Succeeded != decodeDQuantParam (pBMainLoop->PB.m_pWMVDec, pmbmd))
        return WMV_Failed;
    
    pmbmd->m_bCBPAllZero = FALSE;
    
    if (bSendTableswitchMode) {
        I32_WMV iDCTMBTableIndex = BS_getBit(pBMainLoop->PB.m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pBMainLoop->PB.m_pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
End:
    if (bSendXformswitchMode) {
        I32_WMV iIndex = Huffman_WMV_get(pBMainLoop->PB.m_pHufMBXformTypeDec, pBMainLoop->PB.m_pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        pmbmd->m_iMBXformMode = s_iMBXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        pmbmd->m_bFirstBlockDecoded = FALSE;
    }
    
#ifdef _WMV9AP_
    memset(pmbmd->m_rgbCodedBlockPattern2, 0, 6*sizeof(U8_WMV));
#else
    pmbmd->m_rgbCodedBlockPattern = 0;
#endif
    setCodedBlockPattern_EMB (pmbmd, Y_BLOCK1, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern_EMB (pmbmd, Y_BLOCK2, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern_EMB (pmbmd, Y_BLOCK3, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern_EMB (pmbmd, Y_BLOCK4, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern_EMB (pmbmd, U_BLOCK, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern_EMB (pmbmd, V_BLOCK, iCBPCY & 0x1);
    
    return WMV_Succeeded;
}

//#pragma code_seg (EMBSEC_BML)
Void_WMV PredictMV_EMB (EMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk)
{
//    I32_WMV sum, i;
    I32_WMV iNumBlockX = pBMainLoop->PB.m_iNumBlockX;
    I32_WMV iIndex2 = pBMainLoop->PB.m_rgiCurrBaseBK[1] + pBMainLoop->PB.m_rgiBlkIdx[iblk];  //iY * iNumBlockX + iX;
    UMotion_EMB * pMotion = pBMainLoop->m_pMotionCurr;
    UMotion_EMB iVa, iVb, iVc, iPred, uiPred1;
    I32_WMV  iBlocks;
    I32_WMV iIndex = iIndex2 - 1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictMV_EMB);
    
    assert(pBMainLoop->PB.m_rgiBlkIdx[iblk] == 0);

    iX >>=1;

    iNumBlockX = iNumBlockX >>1;
    // find motion predictor
    iVc.U32 = 0;
    if (iX) 
    {
        // left block
        iVc.U32 = pMotion[iIndex].U32;
    }
    
    iIndex = iIndex2 - (iNumBlockX);
    
    
    // top block
    iVa.U32 = pMotion[iIndex].U32;
    
    //iIndex += pBMainLoop->PB.m_PredictMVLUT.U.U.m_pBOffset[(b1MV<<2)+iblk];

    iIndex += ((iX < (iNumBlockX - 1)) << 1) - 1;
    
    iVb.U32 = pMotion[iIndex].U32;


    iBlocks = 0;
    if(iVa.U32 == IBLOCKMV) 
        iBlocks ++;
    if(iVb.U32 == IBLOCKMV)
        iBlocks ++;
    if(iVc.U32 == IBLOCKMV)
        iBlocks ++;


    if(iBlocks > 0)
        goto nonZeroIBlocks;

nonZero1IBlocksRet:

        iPred.U32 = medianof3 (iVa.I32, iVb.I32,iVc.I32)>>16; 
        
        /*
        iVa.U32 <<= 16;
        iVb.U32 <<= 16;
        iVc.U32 <<= 16;
        */
        
        uiPred1.U32 = medianof3 (iVa.I32<<16, iVb.I32<<16, iVc.I32<<16); 
        
        iPred.U32 <<= 16;
        
        iPred.U32 |= ((U32_WMV)uiPred1.U32) >>16;
        
        pBMainLoop->PB.m_iPred.I32 = iPred.U32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 1, iPred.I32);
        
        return;
    

nonZeroIBlocks:

  
    if (iBlocks == 1) {
            if (iVa.U32 == IBLOCKMV)
                iVa.U32 = 0;
            else if (iVb.U32== IBLOCKMV)
                iVb.U32= 0;
            else if (iVc.U32 == IBLOCKMV)
                iVc.U32= 0;
            goto nonZero1IBlocksRet;
    }
    else
    {
         pBMainLoop->PB.m_iPred.U32 = iPred.U32  = 0;
    }
    
}



//#pragma code_seg (EMBSEC_DEF)
I32_WMV PredictBMVPullBack_EMB(EMB_BMainLoop  * pBMainLoop,  I32_WMV bBfrm, I32_WMV iPred)
{
    
    I32_WMV iX1, iY1;
    I32_WMV iBFrm2 = (bBfrm == 2);
    I32_WMV iXPred = (iPred<<16)>>16;
    I32_WMV iYPred = iPred >>16;
    I32_WMV iX, iY;
    I32_WMV iNumBlockX ;
    I32_WMV iNumBlockY ;
    I32_WMV iMinCoordinate, iMaxX, iMaxY;
    Bool_WMV bUV = 0;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictBMVPullBack_EMB);

    bBfrm = bBfrm &1;

    iNumBlockX = pBMainLoop->PB.m_iNumBlockX >>bBfrm;
    iNumBlockY = pBMainLoop->PB.m_iNumBlockY >>bBfrm;

    if(bBfrm == 0)
        bUV = 1;
    
    
    iX = pBMainLoop->PB.m_iBlkMBBase[X_INDEX]>>bBfrm;
    iY = pBMainLoop->PB.m_iBlkMBBase[Y_INDEX]>>bBfrm ;
    
    iX1 = (iX << 5) + iXPred;
    iY1 = (iY << 5 ) + iYPred;
    iMinCoordinate = ((-28 -bUV*4)<<bUV);  // -15 or -7 pixels
    iMaxX = (iNumBlockX << 5) - 4 + bUV*4;  // x8 for block x4 for qpel
    iMaxY = (iNumBlockY << 5) - 4 + bUV*4;
    
    if(iBFrm2 > 0)
    {
        iMinCoordinate += 4;
        iMaxX -= 4;
        iMaxY -= 4;
    }
    else
    {
        iX1 &= ~(bUV*((2<<1)-1));
        iY1 &= ~(bUV*((2<<1)-1));
    }

    
    if (iX1 < iMinCoordinate)
    {
        iXPred += iMinCoordinate - iX1;
    }
    else if (iX1 > iMaxX)
    {
        iXPred += iMaxX - iX1;
    }
    
    if (iY1 < iMinCoordinate)
    {
        iYPred += iMinCoordinate - iY1;
    }
    else if (iY1 > iMaxY)
    {

        iYPred += iMaxY - iY1;
    }
    
    iPred = (iXPred&0xffff) |  (iYPred <<16);
    
    return iPred;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictBMVPullBackCheck_EMB(EMB_BMainLoop  * pBMainLoop,  I32_WMV bBfrm, I32_WMV iPred)
{
    I32_WMV iShift = 5 - (bBfrm&1);
    I32_WMV iCord = (I32_WMV)(pBMainLoop->PB.m_iBlkMBBase[0] | (pBMainLoop->PB.m_iBlkMBBase[1]<<16));//(*(I32_WMV *)(pBMainLoop->PB.m_iBlkMBBase));
    I32_WMV  iV1 = (iCord <<iShift) + (pBMainLoop->m_iMVMin[bBfrm]) + iPred;
    I32_WMV iV2 = pBMainLoop->m_iMVMax[bBfrm]  - (iCord <<iShift) -  iPred;
    I32_WMV iSign = (iPred &0x8000)<<1;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictBMVPullBackCheck_EMB);

    iV1 -= iSign;
    
    if((iV1|iV2)&0x80008000)
    {
        iPred = PredictBMVPullBack_EMB(pBMainLoop, bBfrm, iPred);
    }
    
    return iPred;
    
}


//#pragma code_seg (EMBSEC_BML)
Void_WMV PredictMVTop_EMB (EMB_BMainLoop  * pBMainLoop, I32_WMV iX,  Bool_WMV b1MV,  I32_WMV iblk)
{
    
    
    UMotion_EMB * pMotion = pBMainLoop->m_pMotionCurr;
    UMotion_EMB  iPred;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictMVTop_EMB);
    
    iPred.U32 = 0;
    
    if (iX != 0)
    {
        
        I32_WMV iIndex = pBMainLoop->PB.m_rgiCurrBaseBK[1] + pBMainLoop->PB.m_rgiBlkIdx[iblk] -1;  //iY * iNumBlockX + iX;
        I32_WMV iTmp = pMotion[iIndex].I32;
        
        if(iTmp != IBLOCKMV)
        {
            
            iPred.U32 = PredictBMVPullBackCheck_EMB(pBMainLoop, 1,  iTmp);
        }
        
    }
    
    
    pBMainLoop->PB.m_iPred.I32 = iPred.I32;
    
}

extern const I16_WMV gaSubsampleBicubic[];
//#pragma code_seg (EMBSEC_DEF)
Void_WMV InitEMBBMainLoopCtl(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV i,j;
    EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitEMBBMainLoopCtl);
    
    pBMainLoop->PB.m_pWMVDec = pWMVDec;
    
    pBMainLoop->PB.m_pDiffMV_EMB = pWMVDec->m_pDiffMV_EMB;
    pBMainLoop->PB.m_pbitstrmIn = pWMVDec->m_pbitstrmIn;
    
   // pBMainLoop->PB.m_pXMotion = pWMVDec->m_pXMotion;
    pBMainLoop->PB.m_pMotion = pWMVDec->m_pMotion;
    pBMainLoop->PB.m_pMotionC = pWMVDec->m_pMotionC;

    pBMainLoop->m_pMotionArray[0] = pWMVDec->m_pMotion;
    pBMainLoop->m_pMotionArray[1] = pWMVDec->m_pMotionC;

   

    pBMainLoop->PB.m_rgmv1_EMB = pWMVDec->m_rgmv1_EMB;
    
    pBMainLoop->PB.m_iNumBlockX = pWMVDec->m_uintNumMBX<<1;
    pBMainLoop->PB.m_iNumBlockY = pWMVDec->m_uintNumMBY<<1;
    pBMainLoop->PB.m_rgiBlkIdx[0] = 0;
    pBMainLoop->PB.m_rgiBlkIdx[1] = 1;
    pBMainLoop->PB.m_rgiBlkIdx[2] = pBMainLoop->PB.m_iNumBlockX;
    pBMainLoop->PB.m_rgiBlkIdx[3] = pBMainLoop->PB.m_iNumBlockX+1;
    
    pBMainLoop->PB.m_iWidthY = pBMainLoop->PB.m_iNumBlockX<<3;
    pBMainLoop->PB.m_iHeightY = pBMainLoop->PB.m_iNumBlockY<<3;
    pBMainLoop->PB.m_iWidthUV = pBMainLoop->PB.m_iNumBlockX<<2;
    pBMainLoop->PB.m_iHeightUV = pBMainLoop->PB.m_iNumBlockY<<2;
    
    pBMainLoop->PB.m_iWidthPrevY = pWMVDec->m_iWidthPrevY;
    pBMainLoop->PB.m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
    pBMainLoop->PB.m_iHeightPrevY = pWMVDec->m_iHeightPrevY;
    pBMainLoop->PB.m_iHeightPrevUV = pWMVDec->m_iHeightPrevUV;
    
    pBMainLoop->PB.m_iWidthPrev[0] = pBMainLoop->PB.m_iWidthPrevY;
    pBMainLoop->PB.m_iWidthPrev[1] = pBMainLoop->PB.m_iWidthPrevUV;
    
//    pBMainLoop->PB.m_rgMotionCompBuffer_EMB = pWMVDec->m_rgMotionCompBuffer_EMB;
    
    pBMainLoop->PB.m_rgiCoefReconBuf = pWMVDec->m_rgiCoefReconBuf;
    pBMainLoop->PB.m_pX9dct = pWMVDec->m_pX9dct;
    pBMainLoop->PB.m_pX9dctUV[0] = pWMVDec->m_pX9dctU;
    pBMainLoop->PB.m_pX9dctUV[1]= pWMVDec->m_pX9dctV;

   
    pBMainLoop->m_iMBTypeMask[FORWARD-1] = 0x1;
    pBMainLoop->m_iMBTypeMask[BACKWARD-1] = 0x2;
    
    pBMainLoop->m_iMBTypeMask[INTERPOLATE-1] = 0x3;
    pBMainLoop->m_iMBTypeMask[DIRECT-1] = 0x3;
    /*
    {
        I32_WMV iX, iblk, b1MV, iOffset, iX2;
        
        for(iX=0; iX < 3; iX++)
        {
            for(b1MV = 0; b1MV < 2; b1MV++)
            {
                for(iblk=0; iblk<4; iblk++)
                {
                    if(b1MV == 0) 
                    {
                        iX2 = iX + (iblk&1);
                        if (iX2)
                            iOffset = 1 - (((iblk == 0 || iblk == 3) || (iX == 2 && iblk == 1)) << 1);
                        else // First block column blocks use upper right predictor for B
                            iOffset = 1;
                        pBMainLoop->PB.m_PredictMVLUT.m_rgcBOffset[iX*5 + b1MV*4 + iblk] = iOffset;
                    }
                    else
                    {
                        iOffset = (iX < (2)) * 3 - 1;            
                        pBMainLoop->PB.m_PredictMVLUT.m_rgcBOffset[iX*5 + 4] = iOffset;
                    }
                }
            }
        }
    }
    */
    /*
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[0].m_uiMax  = (pBMainLoop->PB.m_iNumBlockX << 5) - 4 + (((pBMainLoop->PB.m_iNumBlockY << 5) - 4)<<16);
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[1].m_uiMax  = (pBMainLoop->PB.m_iNumBlockX << 5) - 4 + (((pBMainLoop->PB.m_iNumBlockY << 5) - 4)<<16);
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[2].m_uiMax  = (pBMainLoop->PB.m_iNumBlockX << 5)  + (((pBMainLoop->PB.m_iNumBlockY << 5))<<16);
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[4].m_uiMax  = (pBMainLoop->PB.m_iNumBlockX << 4)  + (((pBMainLoop->PB.m_iNumBlockY << 4))<<16);
    */
    pBMainLoop->PB.m_ppxliErrorQ = pWMVDec->m_ppxliErrorQ;
    
#ifdef _EMB_SSIMD_B_
    pBMainLoop->PB.m_pDecodeInterError[XFORMMODE_8x4 -1] = g_DecodeInterError8x4_SSIMD_Fun;
    pBMainLoop->PB.m_pDecodeInterError[XFORMMODE_4x8 -1] = g_DecodeInterError4x8_SSIMD_Fun;
    pBMainLoop->PB.m_pDecodeInterError[XFORMMODE_4x4 -1] = g_DecodeInterError4x4_SSIMD_Fun;
#else
    //TBD
    assert(0);
#endif
    
#ifdef _EMB_SSIMD_MC_
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pBMainLoop->PB.m_pInterpolateBlockBilinear [i][j] = g_InterpolateBlockBilinear_SSIMD_Fun;
        }
    }
#endif

#if defined(_ARM_) && defined(_EMB_SSIMD_MC_)
    pBMainLoop->PB.m_pInterpolateBlockBilinear [0][2] = g_InterpolateBlockBilinear_SSIMD_01_Fun;
    pBMainLoop->PB.m_pInterpolateBlockBilinear [2][0] = g_InterpolateBlockBilinear_SSIMD_10_Fun;
    pBMainLoop->PB.m_pInterpolateBlockBilinear [2][2] = g_InterpolateBlockBilinear_SSIMD_11_Fun;
#endif

    pBMainLoop->PB.m_iFrmMBOffset[0] = 0;
    pBMainLoop->PB.m_iFrmMBOffset[1] = 0;
    
    for(i = 0; i< 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            if(i == (XFORMMODE_8x4-1))
            {
                pBMainLoop->PB.m_uiIDCTClearOffset[i][j] = (j<<3);
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x8-1))
            {
                pBMainLoop->PB.m_uiIDCTClearOffset[i][j] = j + 0xc0;
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x4-1))
            {
                pBMainLoop->PB.m_uiIDCTClearOffset[i][j] =  ((j&2)<<2) + (j&1)+ 0x40;
                
            }
        }
    }
    
    /*
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[0].m_uiMin = 28 + (28<<16);
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[1].m_uiMin = 60 + (60<<16);
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[2].m_uiMin = 64 + (64<<16);
    pBMainLoop->PB.m_PredictMVLUT.U.m_PullBack[4].m_uiMin = 32 + (32<<16);
    
    pBMainLoop->PB.m_PredictMVLUT.U.U.m_RndTbl[0] = 0;
    pBMainLoop->PB.m_PredictMVLUT.U.U.m_RndTbl[1] = 0;
    pBMainLoop->PB.m_PredictMVLUT.U.U.m_RndTbl[2] = 0;
    pBMainLoop->PB.m_PredictMVLUT.U.U.m_RndTbl[3] = 1;
    */

#if    defined(_EMB_SSIMD32_)
            {
                I32_WMV ii;
                U32_WMV  * pData32 = pBMainLoop->m_rgIntraMotionCompBuffer_EMB;
                for ( ii = 0; ii < 8; ii++)
                {
                    pData32[0] = pData32[1]  = pData32[5] = pData32[6]  = 0x00800080;
                    pData32 += 10;
                }
            }
#endif            

    pBMainLoop->m_RndTbl[0] = 0;
    pBMainLoop->m_RndTbl[1] = 0;
    pBMainLoop->m_RndTbl[2] = 0;
    pBMainLoop->m_RndTbl[3] = 1;

#ifdef _EMB_SSIMD64_
    pBMainLoop->m_SSIMD_DstLUT_B[0] = 0;
    pBMainLoop->m_SSIMD_DstLUT_B[1] = 2;
    pBMainLoop->m_SSIMD_DstLUT_B[2] = 96;
    pBMainLoop->m_SSIMD_DstLUT_B[3] = 96+2;
    pBMainLoop->m_SSIMD_DstLUT_B[4] = 192;
    pBMainLoop->m_SSIMD_DstLUT_B[5] = 192+2;

#elif defined(_EMB_SSIMD32_)
    pBMainLoop->m_SSIMD_DstLUT_B[0] = 0;
    pBMainLoop->m_SSIMD_DstLUT_B[1] = 2;
    pBMainLoop->m_SSIMD_DstLUT_B[2] = 80;
    pBMainLoop->m_SSIMD_DstLUT_B[3] = 80+2;
    pBMainLoop->m_SSIMD_DstLUT_B[4] = 160;
    pBMainLoop->m_SSIMD_DstLUT_B[5] = 160+2;

#endif


    pBMainLoop->PB.m_iBlkXformTypeLUT[0] =  XFORMMODE_8x8, 
    pBMainLoop->PB.m_iBlkXformTypeLUT[1] = pBMainLoop->PB.m_iBlkXformTypeLUT[2] = pBMainLoop->PB.m_iBlkXformTypeLUT[3] =  XFORMMODE_8x4;
    pBMainLoop->PB.m_iBlkXformTypeLUT[4] = pBMainLoop->PB.m_iBlkXformTypeLUT[5] = pBMainLoop->PB.m_iBlkXformTypeLUT[6]= XFORMMODE_4x8;
    pBMainLoop->PB.m_iBlkXformTypeLUT[7] =   XFORMMODE_4x4;
    
    pBMainLoop->PB.m_iSubblkPatternLUT[0] = 0;
    pBMainLoop->PB.m_iSubblkPatternLUT[1] = 1;
    pBMainLoop->PB.m_iSubblkPatternLUT[2] = 2;
    pBMainLoop->PB.m_iSubblkPatternLUT[3] = 3;
    pBMainLoop->PB.m_iSubblkPatternLUT[4] = 1;
    pBMainLoop->PB.m_iSubblkPatternLUT[5] = 2;
    pBMainLoop->PB.m_iSubblkPatternLUT[6] = 3;
    pBMainLoop->PB.m_iSubblkPatternLUT[7] = 0;

/*
#ifdef _EMB_SSIMD64_
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[0] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[1] = 2;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[2] = 96;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[3] = 96+2;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[4] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[5] = 0;
#elif defined(_EMB_SSIMD32_)
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[0] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[1] = 2;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[2] = 80;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[3] = 80+2;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[4] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_1MV[5] = 0;
*/
 
/*
#endif
    
    pBMainLoop->PB.m_SSIMD_DstLUT_BK[0] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_BK[1] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_BK[2] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_BK[3] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_BK[4] = 0;
    pBMainLoop->PB.m_SSIMD_DstLUT_BK[5] = 0;
*/
    
    pBMainLoop->PB.m_iNumCoefTbl[0] = 64;
    pBMainLoop->PB.m_iNumCoefTbl[1] = 32;
    pBMainLoop->PB.m_iNumCoefTbl[2] = 32;
    pBMainLoop->PB.m_iNumCoefTbl[3] = 0;
    pBMainLoop->PB.m_iNumCoefTbl[4] = 16;

    for (i=0; i<64; i++)
    {
        if (i < 8)
            pBMainLoop->PB.m_IDCTShortCutLUT[i] = 0;
        else
            pBMainLoop->PB.m_IDCTShortCutLUT[i] = 1 << (i & 0x7);
    }

         //memcpy(pBMainLoop->PB.m_rgiSubsampleBicubic, gaSubsampleBicubic, sizeof(I16_WMV)*16);
    for(i = 0; i< 16; i++)
    {
        pBMainLoop->PB.m_rgiSubsampleBicubic[i] = (I8_WMV)gaSubsampleBicubic[i];
    }
    
    // *(U32_WMV *)pBMainLoop->PB.m_rgiSubsampleBicubic = 0x00000800 + 12 + 2*32 + ((0x00001000 + 12 + 4*32)<<16);
    //  *(U32_WMV *)(pBMainLoop->PB.m_rgiSubsampleBicubic + 2) =  0x00000800 + 12 + 3*32 + ((0x00001000 + 12 + 5*32)<<16);
    pBMainLoop->PB.m_rgiSubsampleBicubic[0] = 0x00;
    pBMainLoop->PB.m_rgiSubsampleBicubic[1] = 0x80;
    pBMainLoop->PB.m_rgiSubsampleBicubic[2] = 0x00;
    pBMainLoop->PB.m_rgiSubsampleBicubic[3] = 0x80; 
    //*(U32_WMV *)pBMainLoop->PB.m_rgiSubsampleBicubic = 0x80008000;

#if 0 // it is only used in g_NewVertFilter0LongNoGlblTbl()    
    pBMainLoop->PB.m_rgiSSIMD_00Tbl[0] = 0x00000800 + 12 + 2*32 + ((0x00001000 + 12 + 4*32)<<16);
    pBMainLoop->PB.m_rgiSSIMD_00Tbl[1] = 0x00000800 + 12 + 3*32 + ((0x00001000 + 12 + 5*32)<<16);
#endif
    pBMainLoop->PB. m_uiffff07ff =  0xffff07ff;
    pBMainLoop->PB. m_uiffff7fff =  0xffff7fff;
    pBMainLoop->PB. m_uiffff1fff =  0xffff1fff;;
    pBMainLoop->PB. m_uic000c000 = 0xc000c000;
    //     pBMainLoop->PB. m_ui80008000 = 0x80008000;
    pBMainLoop->PB. m_uif000f000 = 0xf000f000;;
    pBMainLoop->PB. m_ui00ff00ff = 0x00ff00ff;;

     pBMainLoop->m_iMVMin[0] = 64 + (64<<16);
     pBMainLoop->m_iMVMax[0] = (pBMainLoop->PB.m_iNumBlockX << 5)  + (((pBMainLoop->PB.m_iNumBlockY << 5))<<16);;
     pBMainLoop->m_iMVMin[2] = 60 + (60<<16);
     pBMainLoop->m_iMVMax[2] = (pBMainLoop->PB.m_iNumBlockX << 5) -4 + (((pBMainLoop->PB.m_iNumBlockY << 5)-4)<<16);


     pBMainLoop->m_iMVMin[1] = 28 + (28<<16);
     pBMainLoop->m_iMVMax[1] = (pBMainLoop->PB.m_iNumBlockX << 4) - 4 + (((pBMainLoop->PB.m_iNumBlockY << 4) - 4)<<16);

    {
        I32_WMV iUV, b1MV;

    for(iUV=0; iUV<2; iUV++)
        for(b1MV=0; b1MV<2; b1MV++)
        {
            I32_WMV iSrcStride = pBMainLoop->PB.m_iWidthPrevY;
#           ifdef _EMB_SSIMD64_
                I32_WMV iParam1 = 48, iParam2 = 24, iParam3 = 7;
#           else
                I32_WMV iParam1 = 40, iParam2 = 20, iParam3 = 3;
#           endif

            if(iUV == 1)
                iSrcStride = pBMainLoop->PB.m_iWidthPrevUV;

            pBMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][0] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) - 1 ;
            pBMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][1] = (iParam1 <<(3+b1MV)) - iParam2;
            pBMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][2] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) - iParam3 ;
            pBMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][3] = (iParam1 <<(3+b1MV)) + 16;
        }
    }
}


//#pragma code_seg (EMBSEC_DEF)
Void_WMV SetupEMBBMainLoopCtl(tWMVDecInternalMember *pWMVDec)
{
     EMB_BMainLoop * pBMainLoop = &pWMVDec->m_EMB_BMainLoop;
     DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetupEMBBMainLoopCtl);

     InitEMBBMainLoopCtl(pWMVDec);

     pBMainLoop->PB.m_rgiMVDecLUT_EMB = pWMVDec->m_rgiMVDecLUT_EMB;
     pBMainLoop->PB.m_iQP= ((pWMVDec->m_iStepSize << 1) - 1) + pWMVDec->m_bHalfStep;
 //    pBMainLoop->PB.m_iMVSwitchCodingMode = pWMVDec->m_iMVSwitchCodingMode;
     pBMainLoop->PB.m_iSkipbitCodingMode = pWMVDec->m_iSkipbitCodingMode;
     pBMainLoop->PB.m_pHufMVTable_V9 = pWMVDec->m_pHufMVTable_V9;
     pBMainLoop->PB.m_bDQuantOn = pWMVDec->m_bDQuantOn;
     pBMainLoop->PB.m_bDCTTable_MB = pWMVDec->m_bDCTTable_MB;
     pBMainLoop->PB.m_bMBXformSwitching = pWMVDec->m_bMBXformSwitching;
     pBMainLoop->PB.m_bHalfPelMV = 
         (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL) | (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);

     
     pBMainLoop->PB.m_pHufNewPCBPCYDec = pWMVDec->m_pHufNewPCBPCYDec;
     pBMainLoop->PB.m_pHufMBXformTypeDec = pWMVDec->m_pHufMBXformTypeDec;
     pBMainLoop->PB.m_rgiCurrBaseBK[0] = 0;
     pBMainLoop->PB.m_rgiCurrBaseBK[1] = 0;
     pBMainLoop->PB.m_rgiBlkIdx[4] =
     pBMainLoop->PB.m_rgiBlkIdx[5] = 0;
     
     pBMainLoop->PB.m_iXMVRange = pWMVDec->m_iXMVRange;
     pBMainLoop->PB.m_iYMVRange = pWMVDec->m_iYMVRange;
     pBMainLoop->PB.m_iXMVFlag = pWMVDec->m_iXMVFlag;
     pBMainLoop->PB.m_iYMVFlag = pWMVDec->m_iYMVFlag;
     pBMainLoop->PB.m_iLogXRange = pWMVDec->m_iLogXRange;
     pBMainLoop->PB.m_iLogYRange = pWMVDec->m_iLogYRange;
     
     pBMainLoop->PB.m_iNumBFrames = pWMVDec->m_iNumBFrames;
     pBMainLoop->PB.m_bUVHpelBilinear = pWMVDec->m_bUVHpelBilinear;
     
     pBMainLoop->PB.m_iFrameXformMode = pWMVDec->m_iFrameXformMode;
     pBMainLoop->PB.m_prgDQuantParam = pWMVDec->m_prgDQuantParam;
     pBMainLoop->PB.m_pInterDCTTableInfo_Dec_Set = pWMVDec->m_pInterDCTTableInfo_Dec_Set;
     pBMainLoop->PB.m_pIntraDCTTableInfo_Dec_Set = pWMVDec->m_pIntraDCTTableInfo_Dec_Set;
     pBMainLoop->PB.m_ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
     pBMainLoop->PB.m_ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
     pBMainLoop->PB.m_iDCTHorzFlags= pWMVDec->m_iDCTHorzFlags;
     pBMainLoop->PB.m_iOverlap= pWMVDec->m_iOverlap;
     
     pBMainLoop->PB.m_pZigzagScanOrder= pWMVDec->m_pZigzagScanOrder;
     pBMainLoop->PB.m_iFilterType[0] =  pWMVDec->m_iFilterType;
     pBMainLoop->PB.m_iFilterType[1] = FILTER_BILINEAR;

     pBMainLoop->PB.m_iRndCtrl  = (pWMVDec->m_iRndCtrl <<16)|pWMVDec->m_iRndCtrl ;

     
     pBMainLoop->PB.m_ppxliRefBlkBase[0] = pWMVDec->m_ppxliRef0YPlusExp;
     pBMainLoop->PB.m_ppxliRefBlkBase[1] = pWMVDec->m_ppxliRef1Y + pWMVDec->m_iWidthPrevYXExpPlusExp;;
     pBMainLoop->PB.m_ppxliRefBlkBase[2] = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pBMainLoop->PB.m_ppxliRefBlkBase[3] = pWMVDec->m_ppxliRef1U + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pBMainLoop->PB.m_ppxliRefBlkBase[4] = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pBMainLoop->PB.m_ppxliRefBlkBase[5] = pWMVDec->m_ppxliRef1V + pWMVDec->m_iWidthPrevUVXExpPlusExp; 

//#ifdef _EMB_3FRAMES_
     if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled)
     {
                if(pWMVDec->m_bRef0InRefAlt == TRUE_WMV)
                {
                    pBMainLoop->PB.m_ppxliRefBlkBase[0] = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp; 
                    pBMainLoop->PB.m_ppxliRefBlkBase[2] = pWMVDec->m_pfrmRef0AltQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
                    pBMainLoop->PB.m_ppxliRefBlkBase[4] = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
                }
     }
//#endif
     
     pBMainLoop->PB.m_ppxliCurrBlkBase[0] = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
     pBMainLoop->PB.m_ppxliCurrBlkBase[1] = pBMainLoop->PB.m_ppxliCurrBlkBase[0] + 8;
     pBMainLoop->PB.m_ppxliCurrBlkBase[2] = pBMainLoop->PB.m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8;
     pBMainLoop->PB.m_ppxliCurrBlkBase[3] = pBMainLoop->PB.m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8 + 8;
     pBMainLoop->PB.m_ppxliCurrBlkBase[4] = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pBMainLoop->PB.m_ppxliCurrBlkBase[5] = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp; 

  
     pBMainLoop->m_bHalfPelBiLinear = pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR;
     pBMainLoop->m_iDirectCodingMode = pWMVDec->m_iDirectCodingMode;
     pBMainLoop->m_mbtShortBSymbol  = pWMVDec->m_mbtShortBSymbol;
     pBMainLoop->m_mbtLongBSymbol =  pWMVDec->m_mbtLongBSymbol;
     pBMainLoop->m_iScaleFactor = pWMVDec->m_iBNumerator * pWMVDec->m_iBFrameReciprocal;   //new

     pBMainLoop->m_rgfPredictMV_EMB[0] = PredictMVTop_EMB;
     pBMainLoop->m_rgfPredictMV_EMB[1] = PredictMV_EMB;
     pBMainLoop->m_rgfPredictMV_EMB[2] = PredictMV_EMB;
     pBMainLoop->m_ppPredictMV_EMB = pBMainLoop->m_rgfPredictMV_EMB;

     pBMainLoop->PB.m_iBlkMBBase[X_INDEX] = pBMainLoop->PB.m_iBlkMBBase[Y_INDEX] = 0;
     
     pBMainLoop->PB.m_pIntraBlockRow[0] = pBMainLoop->PB.m_pIntraBlockRow[1]  = pWMVDec->m_pIntraBlockRow0[0];
     pBMainLoop->PB.m_pIntraBlockRow[2] = pBMainLoop->PB.m_pIntraBlockRow[3]  = pWMVDec->m_pIntraBlockRow0[2];
     pBMainLoop->PB.m_pIntraBlockRow[4]  = pWMVDec->m_pIntraMBRowU0[0];
     pBMainLoop->PB.m_pIntraBlockRow[5]  = pWMVDec->m_pIntraMBRowV0[0];
     
     pBMainLoop->PB.m_iACColPredShift =  pWMVDec->m_iACColPredShift;
     pBMainLoop->PB.m_iACRowPredShift =  pWMVDec->m_iACRowPredShift;
     pBMainLoop->PB.m_iACColPredOffset =  pWMVDec->m_iACColPredOffset;
     pBMainLoop->PB.m_iACRowPredOffset =  pWMVDec->m_iACRowPredOffset;

     pBMainLoop->PB.m_pHufBlkXformTypeDec = pWMVDec->m_pHufBlkXformTypeDec;
     pBMainLoop->PB.m_pHuf4x4PatternDec = pWMVDec->m_pHuf4x4PatternDec;
     
#ifdef _EMB_SSIMD_B_
     pBMainLoop->PB.m_pZigzag[ XFORMMODE_8x4 -1] = pWMVDec->m_p8x4ZigzagSSIMD;
     pBMainLoop->PB.m_pZigzag[ XFORMMODE_4x8 -1] = pWMVDec->m_p4x8ZigzagScanOrder;
     pBMainLoop->PB.m_pZigzag[ XFORMMODE_4x4 -1] = pWMVDec->m_p4x4ZigzagSSIMD;
#endif

     pBMainLoop->PB. m_ui001f001fPiRndCtl = 0x001f001f + pBMainLoop->PB.m_iRndCtrl;
     pBMainLoop->PB. m_ui000f000fPiRndCtl = 0x000f000f + pBMainLoop->PB.m_iRndCtrl;
     pBMainLoop->PB. m_ui00070007PiRndCtl = 0x00070007 + pBMainLoop->PB.m_iRndCtrl;
     pBMainLoop->PB. m_ui00030003PiRndCtl = 0x00030003 + pBMainLoop->PB.m_iRndCtrl;
     pBMainLoop->PB. m_ui00200020MiRndCtl = 0x00200020 - pBMainLoop->PB.m_iRndCtrl;
     pBMainLoop->PB. m_ui00400040MiRndCtl = 0x00400040 - pBMainLoop->PB.m_iRndCtrl;
     pBMainLoop->PB. m_ui00080008MiRndCtl = 0x00080008 - pBMainLoop->PB.m_iRndCtrl;
}
#endif



