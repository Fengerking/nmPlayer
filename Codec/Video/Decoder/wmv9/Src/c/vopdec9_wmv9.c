//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 2001  Microsoft Corporation

Module Name:

        vopenc9.cpp

Abstract:

        X9 encoding functions

Author:

        Sridhar Srinivasan (sridhsri@microsoft.com) 10-September-2001

Revision History:

*************************************************************************/
//#ifdef X9

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



/****************************************************************************************
  decodeP_X9 : P frame decoding with X9 mode
****************************************************************************************/


tWMVDecodeStatus WMVideoDecDecodeP_X9 (tWMVDecInternalMember *pWMVDec)
{
    U8_WMV * ppxliCurrQY;
    U8_WMV * ppxliCurrQU;
    U8_WMV * ppxliCurrQV;
    U8_WMV * ppxliRefY ;
    U8_WMV * ppxliRefU ;
    U8_WMV * ppxliRefV ;
    U32_WMV imbX, imbY;
    I32_WMV x, y;
    U32_WMV uiNumMBFromSliceBdry;
    Bool_WMV bFrameInPostBuf;
    Bool_WMV bTopRowOfSlice = TRUE;
    CWMVMBMode* pmbmd;
    I32_WMV result = 0;

    HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeP_X9",9);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeP_X9);
    
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;
#   ifdef SIXTAP
        pWMVDec->m_iFilterType = FILTER_6TAP;
#   endif // SIXTAP
    
    
    // update overlap related variables
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);
        
    // SET_NEW_FRAME
    pWMVDec->m_pAltTables->m_iHalfPelMV =
            (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
    t_AltTablesSetNewFrame(pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);
    
    SetTransformTypeHuffmanTable (pWMVDec, pWMVDec->m_iStepSize);

    ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliRefY = pWMVDec->m_ppxliRef0YPlusExp;
    ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;

//#ifdef _EMB_3FRAMES_
     if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled)
     {
        if(pWMVDec->m_bRef0InRefAlt == TRUE_WMV)
        {
            ppxliRefY = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp; 
            ppxliRefU = pWMVDec->m_pfrmRef0AltQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
            ppxliRefV = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
        }
     }
//#endif
    DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliRef0Y, NULL);
    
    pmbmd = pWMVDec->m_rgmbmd;
    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;
    
    uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;
    
    for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {        
        U8_WMV * ppxliCurrQYMB;
        U8_WMV * ppxliCurrQUMB;
        U8_WMV * ppxliCurrQVMB;
        U8_WMV * ppxliRefYMB;
        U8_WMV * ppxliRefUMB;
        U8_WMV * ppxliRefVMB;

        Bool_WMV bBottomRowOfSlice = (imbY == pWMVDec->m_uintNumMBY - 1);
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBY_LoopCount");

        if(pWMVDec->m_bCodecIsWVC1){
		pWMVDec->m_nrows = imbY;
        }
        ppxliCurrQYMB = ppxliCurrQY;
        ppxliCurrQUMB = ppxliCurrQU;
        ppxliCurrQVMB = ppxliCurrQV;
       
        ppxliRefYMB = ppxliRefY;
        ppxliRefUMB = ppxliRefU;
        ppxliRefVMB = ppxliRefV;
        
        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;
        

       if (pWMVDec->m_bSliceWMVA) {
                if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush(pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
                    if (result != ICERR_OK)
                        return WMV_Failed;
		   }
            if (pWMVDec->m_pbStartOfSliceRow[imbY])
                bTopRowOfSlice = TRUE;

        }
        
        if(pWMVDec->m_bStartCode) {
            if(SliceStartCode(pWMVDec, imbY)!= WMV_Succeeded) {
                return WMV_Failed;
            }

        }
        
        for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE) {
            Bool_WMV b1MV;
			tWMVDecodeStatus result = WMV_Succeeded;
            //  I32_WMV iX, iY ;
            //  FUNCTION_PROFILE_DECL_START(fpPMBMode, DECODEMBMODE_PROFILE)
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBX_LoopCount");

			if(pWMVDec->m_bCodecIsWVC1) {
				pWMVDec->m_ncols = imbX;
			}

            uiNumMBFromSliceBdry ++;
            pWMVDec->m_bMBHybridMV = FALSE_WMV;

            result = decodeMBOverheadOfPVOP_WMV3 (pWMVDec, pmbmd, imbX, imbY);
            if (WMV_Succeeded != result) {
                // FUNCTION_PROFILE_STOP(&fp);
                return result;
            }
            
            b1MV = (pmbmd->m_chMBMode == MB_1MV);

            ChromaMV(pWMVDec, imbX, imbY, b1MV);  // populate pWMVDec->m_pXMotionC, pWMVDec->m_pYMotionC
            
            //#ifdef SKIP_SPEEDUP
            if (b1MV) {
                I32_WMV iIntra = (pWMVDec->m_pXMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] == IBLOCKMV);
                
                pmbmd -> m_rgcIntraFlag[0] = pmbmd -> m_rgcIntraFlag[1] = pmbmd -> m_rgcIntraFlag[2] = pmbmd -> m_rgcIntraFlag[3] =  (U8_WMV) iIntra;
                
                if (pmbmd->m_bCBPAllZero == TRUE && !iIntra) {
                    U8_WMV *ppxlcRefY;
                    I32_WMV iUVLoc ;
                    I32_WMV iMVOffst;
                    
                        memset(pmbmd->m_rgbCodedBlockPattern2, 0, 6);
                        pmbmd->m_rgcBlockXformMode[0] = XFORMMODE_8x8;
                        pmbmd->m_rgcBlockXformMode[1] = XFORMMODE_8x8;
                        pmbmd->m_rgcBlockXformMode[2] = XFORMMODE_8x8;
                        pmbmd->m_rgcBlockXformMode[3] = XFORMMODE_8x8;
                        pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
                        pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
                  
                    ppxlcRefY = pWMVDec->m_ppxliRef0YPlusExp + pWMVDec->m_iWidthPrevY * y + x;                    
                    iUVLoc = ((y >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (x >> 1) + pWMVDec->EXPANDUV_REFVOP;
                    

                    iMVOffst = 4 * imbY * (I32_WMV) pWMVDec->m_uintNumMBX + 2 * imbX;
                    if (!pWMVDec->m_pXMotion[iMVOffst] && !pWMVDec->m_pYMotion[iMVOffst]) {
                        
//#ifdef _EMB_3FRAMES_
                        {
                            if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled && pWMVDec->m_bRef0InRefAlt == TRUE_WMV)
                            {
                                ppxlcRefY = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iWidthPrevY * y + x;
                                iUVLoc = ((y >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (x >> 1) + pWMVDec->EXPANDUV_REFVOP;
                            
                                (*pWMVDec->m_pMotionCompZero) (
                                    ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                                    ppxlcRefY, pWMVDec->m_pfrmRef0AltQ->m_pucUPlane + iUVLoc, pWMVDec->m_pfrmRef0AltQ->m_pucVPlane + iUVLoc,
                                    pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                            
                            }
                            else
//#endif
                             (*pWMVDec->m_pMotionCompZero) (
                            ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                            ppxlcRefY, pWMVDec->m_ppxliRef0U + iUVLoc, pWMVDec->m_ppxliRef0V + iUVLoc,
                            pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                        }
                        
                        pmbmd->m_bSkip = TRUE_WMV;
                        pmbmd->m_dctMd = INTER;
                        result = WMV_Succeeded;
                        goto LOOP_END;
                    }
                    else
                    {
                        U8_WMV *ppxlcCurrQYMB = ppxliCurrQYMB;                  
                        MotionCompMB_WMV3(pWMVDec, ppxlcCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, imbX, imbY);
                        pmbmd->m_bSkip = FALSE_WMV;
                        result = WMV_Succeeded;
                        goto LOOP_END;
                    }
                }
                    result = DecodeMB_1MV (pWMVDec, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, imbX, imbY, 
                                           x << 1, y << 1, x, y);
            }
            //#endif  // SKIP_SPEEDUP
            else {  // 4MV
                
                pmbmd -> m_rgcIntraFlag[0] =  (I8_WMV) pWMVDec->m_pDiffMV  -> iIntra;
                pmbmd -> m_rgcIntraFlag[1] = (I8_WMV)(pWMVDec->m_pDiffMV + 1) -> iIntra;
                pmbmd -> m_rgcIntraFlag[2] = (I8_WMV)(pWMVDec->m_pDiffMV + 2) -> iIntra;
                pmbmd -> m_rgcIntraFlag[3] = (I8_WMV)(pWMVDec->m_pDiffMV + 3) -> iIntra;
                
                    result = DecodeMB_X9 (pWMVDec, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, imbX, imbY, 
                                          x << 1, y << 1, x, y);

            }
            
            if (WMV_Succeeded != result) {
                return result;
            }
LOOP_END:
            pmbmd++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            
            ppxliRefYMB += MB_SIZE;
            ppxliRefUMB += BLOCK_SIZE;
            ppxliRefVMB += BLOCK_SIZE;
            
        }
#ifdef _WMV9AP_
        if (pWMVDec->m_iOverlap & 1)
#else
        if (pWMVDec->m_iOverlap)
#endif
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, bTopRowOfSlice, FALSE);
        bTopRowOfSlice = FALSE;

         if (imbY < pWMVDec->m_uintNumMBY - 1 && pWMVDec->m_pbStartOfSliceRow[imbY + 1])
                bBottomRowOfSlice = TRUE;
        
        // point to the starting location of the first MB of each row
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRefU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRefV += pWMVDec->m_iBlkSizeXWidthPrevUV;

        if (bBottomRowOfSlice && (pWMVDec->m_iOverlap & 1))
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, FALSE, TRUE);
#if 0
		if(0)
		{
			unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
			unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
			unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
			unsigned char* Y = Yplane + imbY* pWMVDec->m_iMBSizeXWidthPrevY ;
			unsigned char *U = Uplane + imbY* pWMVDec->m_iBlkSizeXWidthPrevUV ;
			unsigned char *V = Vplane + imbY* pWMVDec->m_iBlkSizeXWidthPrevUV ;
			if(pWMVDec->m_nPframes ==1 && pWMVDec->m_tFrmType == PVOP)
			{
				int mb_cols =0;
				for(mb_cols=0;mb_cols<pWMVDec->m_uintNumMBX;mb_cols++)
				{
					FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump_before.txt","a");  //progressive
					int i,j;
					fprintf(fp,"-----------------------%d %d -----------\n",imbY,mb_cols);
					for(i=0;i<16;i++)
					{
						for(j=0;j<16;j++)
						{
							fprintf(fp,"%x   ",Y[i*pWMVDec->m_iWidthInternal+j]);
						}
						fprintf(fp,"\n");
					}

					for(i=0;i<8;i++)
					{
						for(j=0;j<8;j++)
						{
							fprintf(fp,"%x   ",U[i*pWMVDec->m_iWidthInternalUV+j]);
						}
						fprintf(fp,"\n");
					}

					for(i=0;i<8;i++)
					{
						for(j=0;j<8;j++)
						{
							fprintf(fp,"%x   ",V[i*pWMVDec->m_iWidthInternalUV+j]);
						}
						fprintf(fp,"\n");
					}

					Y +=MB_SIZE;
					U +=BLOCK_SIZE;
					V +=BLOCK_SIZE;
					fclose(fp);
				}
			}
		}
#endif
        
    }
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MB_Loop_Done");
    
    bFrameInPostBuf = FALSE_WMV;
#ifndef WMV9_SIMPLE_ONLY // no LoopFilter for V9_SIMPLE
    if (pWMVDec->m_bLoopFilter ) {
#ifndef _WMV9AP_
        if (pWMVDec->m_bV9LoopFilter)
            ComputeLoopFilterFlags(pWMVDec);
        if (pWMVDec->m_bV9LoopFilter)
            DeblockSLFrame_V9 (pWMVDec, 
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            pWMVDec->m_puchFilter8x8RowFlagY0, pWMVDec->m_puchFilter8x8RowFlagU0, pWMVDec->m_puchFilter8x8RowFlagV0,
            pWMVDec->m_puchFilter8x4RowFlagY0, pWMVDec->m_puchFilter8x4RowFlagU0, pWMVDec->m_puchFilter8x4RowFlagV0,
            pWMVDec->m_puchFilter8x8ColFlagY0, pWMVDec->m_puchFilter8x8ColFlagU0, pWMVDec->m_puchFilter8x8ColFlagV0,
            pWMVDec->m_puchFilter4x8ColFlagY0, pWMVDec->m_puchFilter4x8ColFlagU0, pWMVDec->m_puchFilter4x8ColFlagV0,
            pWMVDec->m_pLoopFilterFlags, 0, pWMVDec->m_uiMBEnd0);            
        else
            DeblockSLFrame (pWMVDec, pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            FALSE_WMV, 0, 1, pWMVDec->m_uintNumMBX, 1, pWMVDec->m_uintNumMBY);
#else
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
#endif
    }
#endif // WMV9_SIMPLE_ONLY
        
    if (pWMVDec->m_bBFrameOn) {
        // A little post processing to line things up ...
        CWMVMBMode* pmbmd1 = pWMVDec->m_rgmbmd1;
        I32_WMV i,j;
        for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++) 
            for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++) {
                //I32_WMV iMVX=0, iMVY=0;
                I16_WMV iX = pWMVDec->m_pXMotionC[j + i * (I32_WMV) pWMVDec->m_uintNumMBX];
                if (iX == IBLOCKMV) {
                    pmbmd1->m_dctMd = INTRA;
                } else {
                    pmbmd1->m_dctMd = INTER;
                }
                pmbmd1++;
            }
    }   // post-processing ends
    
    pWMVDec->m_bDisplay_AllMB = (pWMVDec->m_bLoopFilter || pWMVDec->m_iResIndex != 0 || pWMVDec->m_iRefFrameNum != -1);
            
    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;
    
    //    pWMVDec->m_bColorSettingChanged = FALSE_WMV;
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE_WMV; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame
 #if 0  
	if(0)
		{
			unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
			unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
			unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

			if(pWMVDec->m_nPframes ==1 && pWMVDec->m_tFrmType == PVOP)
			{
				int mb_cols =0, mb_rows=0;
				for(mb_rows=0;mb_rows<pWMVDec->m_uintNumMBY;mb_rows++)
				{
					unsigned char* Y = Yplane + mb_rows* pWMVDec->m_iMBSizeXWidthPrevY ;
					unsigned char *U = Uplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;
					unsigned char *V = Vplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;
					for(mb_cols=0;mb_cols<pWMVDec->m_uintNumMBX;mb_cols++)
					{
						FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump_after.txt","a");
						int i,j;
						fprintf(fp,"-----------------------%d %d -----------\n",mb_rows,mb_cols);
						for(i=0;i<16;i++)
						{
							for(j=0;j<16;j++)
							{
								fprintf(fp,"%x   ",Y[i*pWMVDec->m_iWidthInternal+j]);
							}
							fprintf(fp,"\n");
						}

						for(i=0;i<8;i++)
						{
							for(j=0;j<8;j++)
							{
								fprintf(fp,"%x   ",U[i*pWMVDec->m_iWidthInternalUV+j]);
							}
							fprintf(fp,"\n");
						}

						for(i=0;i<8;i++)
						{
							for(j=0;j<8;j++)
							{
								fprintf(fp,"%x   ",V[i*pWMVDec->m_iWidthInternalUV+j]);
							}
							fprintf(fp,"\n");
						}

						Y +=MB_SIZE;
						U +=BLOCK_SIZE;
						V +=BLOCK_SIZE;
						fclose(fp);
					}
				}
			}
		}
#endif

    return WMV_Succeeded;
}

/****************************************************************************************
  SetTransformTypeHuffmanTable
****************************************************************************************/
Void_WMV SetTransformTypeHuffmanTable (tWMVDecInternalMember *pWMVDec, I32_WMV iStep)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetTransformTypeHuffmanTable);
    if (iStep < 5) {
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternLowQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeLowQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeLowQPDec;
    }
    else if (iStep < 13) { // Mid rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternMidQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeMidQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeMidQPDec;
    }
    else { // Low rate
        pWMVDec->m_pHuf4x4PatternDec = &pWMVDec->m_huf4x4PatternHighQPDec;
        pWMVDec->m_pHufMBXformTypeDec = &pWMVDec->m_hufMBXformTypeHighQPDec;
        pWMVDec->m_pHufBlkXformTypeDec = &pWMVDec->m_hufBlkXformTypeHighQPDec;
    }
}




/****************************************************************************************
  ComputeMVFromDiffMV : compute motion vector from differential MV
****************************************************************************************/
I32_WMV ComputeMVFromDiffMV (tWMVDecInternalMember *pWMVDec, I32_WMV  iBlockX, I32_WMV iBlockY,
                             CDiffMV *pInDiffMV, Bool_WMV b1MV )
{
    I32_WMV  iXBlocks = pWMVDec->m_uintNumMBX << 1;
    I32_WMV  k = iBlockY * iXBlocks + iBlockX;
    I32_WMV  dX, dY, iIBlock = 0;
    I16_WMV  *pXMotion = pWMVDec->m_pXMotion;
    I16_WMV  *pYMotion = pWMVDec->m_pYMotion;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeMVFromDiffMV);

    // 1MV non-anchor blocks get assigned anchor block MV
    if (b1MV && ((iBlockX | iBlockY) & 1)) {
        I32_WMV l = (iBlockY & 0xfffe) * iXBlocks + (iBlockX & 0xfffe);
        pXMotion[k] = pXMotion[l];
        pYMotion[k] = pYMotion[l];
        return (pXMotion[k] == IBLOCKMV);
    }

    // find prediction, and add

    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        pXMotion[k] = IBLOCKMV;
    }
    else  {
        if (pInDiffMV->iHybrid == 0) {
            Bool_WMV bTopBndry = FALSE;
            if (!(iBlockY & 1)) {
                bTopBndry = (iBlockY == 0) || pWMVDec->m_pbStartOfSliceRow [iBlockY >> 1];
            }

#ifdef _WMV9AP_
            if (pWMVDec->m_bFieldMode) {
                dX = iBlockX;
                dY = iBlockY;
#ifndef PPCWMP
                PredictFieldModeMV (pWMVDec, &dX, &dY, 
                    pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, b1MV, bTopBndry);
#endif
            }
            else
                PredictMV (pWMVDec, iBlockX, iBlockY, (I32_WMV) b1MV, iXBlocks, pWMVDec->m_uintNumMBY * 2,
                pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, &dX, &dY, bTopBndry);
#else
            PredictMV (pWMVDec, iBlockX, iBlockY, (I32_WMV) b1MV, iXBlocks, pWMVDec->m_uintNumMBY << 1,
                       pXMotion, pYMotion, &dX, &dY, bTopBndry);
#endif
        }
          else {
            if (pInDiffMV->iHybrid == 1) {
                dX = pXMotion[k - 1];
                dY = pYMotion[k - 1];              
            }
            else {
                dX = pXMotion[k - iXBlocks];
                dY = pYMotion[k - iXBlocks];             
            }

            if (dX == IBLOCKMV) {
                dX = dY = 0;
            }
        }

        if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR) {
            pInDiffMV->iX <<= 1;
            pInDiffMV->iY <<= 1;
        }

        pXMotion[k] = ((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
        pYMotion[k] = ((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;     

    }

    return iIBlock;
}

/****************************************************************************************
  MotionComp_X9 : motion compensation
****************************************************************************************/
Void_WMV MotionComp_X9 (tWMVDecInternalMember *pWMVDec, U8_WMV *pDst, UnionBuffer *pError,
                                           I32_WMV iBlockX, I32_WMV iBlockY, U8_WMV *pSrc)
{
    I32_WMV  iFilterType = pWMVDec->m_iFilterType;    
    I32_WMV ii;
    I32_WMV  k = (iBlockY * pWMVDec->m_uintNumMBX << 1) + iBlockX;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(MotionComp_X9);
    
    if (pWMVDec->m_pXMotion[k] == IBLOCKMV) {  // if I block
        for ( ii = 0; ii < 8; ii++)
            memset (pDst + ii * pWMVDec->m_iWidthPrevY, 128, 8);
        
    }
    else {        
        U8_WMV  *pRef;    
        I32_WMV  iShiftX = pWMVDec->m_pXMotion[k];
        I32_WMV  iShiftY = pWMVDec->m_pYMotion[k];
        I32_WMV  iCoordX;
        I32_WMV  iCoordY;
        
        PullBackMotionVector (pWMVDec, &iShiftX, &iShiftY, iBlockX >> 1, iBlockY >> 1);
      
    if(pWMVDec->m_bCodecIsWVC1){
		int xIndex = pWMVDec->m_ncols<<4;
		int yIndex = pWMVDec->m_nrows<<4;
		if( (iShiftX>>2) + xIndex <= -32)  //zou fix
			iShiftX =  (-32-xIndex)<<2;
		else	if( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			iShiftX = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		if((iShiftY>>2) + yIndex <= -32)
			iShiftY =  (-32-yIndex)<<2;
		else	if((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			iShiftY = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;
    }
        iCoordX = (iBlockX << 3) + (iShiftX >> 2);
        iCoordY = (iBlockY << 3) + (iShiftY >> 2);
    if(pWMVDec->m_bCodecIsWVC1){
		if(iCoordY< -32)  //zou fix
			iCoordY = -32;
		else if(iCoordY>32 + pWMVDec->m_iHeightPrevY)
			iCoordY = 32 + pWMVDec->m_iHeightPrevY;

		if(iCoordX< -32)
			iCoordX = -32;
		else if(iCoordX>32+pWMVDec->m_iWidthPrevY)
			iCoordX = 32+pWMVDec->m_iWidthPrevY;
    }
        pRef = pWMVDec->m_ppxliRef0YPlusExp + pWMVDec->m_iWidthPrevY * iCoordY + iCoordX;
        InterpolateBlock(pWMVDec, pRef, pWMVDec->m_iWidthPrevY, pDst, pWMVDec->m_iWidthPrevY, iShiftX & 3, 
            iShiftY & 3, iFilterType);        
    }
    
    if (pError) {
        I16_WMV *pBuf = pError->i16;
        pWMVDec->m_pAddError(pDst, pDst, pBuf, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);
        
    }
    
}


/***********************************************************************
  InterpolateMB : Interpolate reference block 
***********************************************************************/
Void_WMV InterpolateMB (tWMVDecInternalMember *pWMVDec,
                        const U8_WMV *pSrc, I32_WMV iSrcStride,
                        U8_WMV *pDst, I32_WMV iDstStride,
                        I32_WMV iDX, I32_WMV iDY, I32_WMV iFilterType)
{
    const U8_WMV *pSrc2 = pSrc + (iDY >> 2) * iSrcStride + (iDX >> 2);
    U8_WMV *pDst2 = pDst;
    I32_WMV iXFrac = iDX & 3;
    I32_WMV iYFrac = iDY & 3;
    I32_WMV i ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InterpolateMB);

    if (iXFrac == 0 && iYFrac == 0)
    { // pDst is aligned, pSrc is also word aligned.
        for ( i = 0; i < 16; i++) {
#ifdef _WMV_TARGET_X86_
            ((__int64 *) pDst2)[0] = ((__int64 *) pSrc2)[0];
            ((__int64 *) pDst2)[1] = ((__int64 *) pSrc2)[1];
#else // _WMV_TARGET_X86_
            ALIGNEDEST_FASTMEMCPY16(pDst2, pSrc2, 16);
#endif // _WMV_TARGET_X86_
            pDst2 += iDstStride; pSrc2 += iSrcStride;
        }
    }
    else
    {
        if (iFilterType == FILTER_BICUBIC) {
            (*pWMVDec->m_pInterpolateBlockBicubic) (pSrc2, iSrcStride, pDst, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
            (*pWMVDec->m_pInterpolateBlockBicubic) (pSrc2 + 8, iSrcStride, pDst + 8, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
            (*pWMVDec->m_pInterpolateBlockBicubic) (pSrc2 + 8 * iSrcStride, iSrcStride, pDst + 8 * iDstStride, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
            (*pWMVDec->m_pInterpolateBlockBicubic) (pSrc2 + 8 * (iSrcStride + 1), iSrcStride, pDst + 8 * (iDstStride + 1), iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
        }
        else {
            (*pWMVDec->m_pInterpolateBlockBilinear) (pSrc2, iSrcStride, pDst, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
            (*pWMVDec->m_pInterpolateBlockBilinear) (pSrc2 + 8, iSrcStride, pDst + 8, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
            (*pWMVDec->m_pInterpolateBlockBilinear) (pSrc2 + 8 * iSrcStride, iSrcStride, pDst + 8 * iDstStride, iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
            (*pWMVDec->m_pInterpolateBlockBilinear) (pSrc2 + 8 * (iSrcStride + 1), iSrcStride, pDst + 8 * (iDstStride + 1), iDstStride, iXFrac, iYFrac, pWMVDec->m_iRndCtrl);
        }
    }
}


/****************************************************************************************
  MotionCompMB_WMV3 : macroblock motion compensation (don't add error)
****************************************************************************************/
Void_WMV MotionCompMB_WMV3 (tWMVDecInternalMember *pWMVDec, U8_WMV *pDst, U8_WMV *pDstU, U8_WMV *pDstV,
                            I32_WMV iMBX,  I32_WMV iMBY)
{
    I32_WMV ii;
    I32_WMV  k = (iMBY * pWMVDec->m_uintNumMBX << 2) + (iMBX << 1);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(MotionCompMB_WMV3);

#ifdef _WMV9AP_
    if((pWMVDec->m_tFrmType == BVOP  && !pWMVDec->m_bFieldMode) || // only case where this is called for B frame is when IMB
#else
    if (pWMVDec->m_tFrmType == BVOP || // only case where this is called for B frame is when IMB
#endif
       // pWMVDec->m_pXMotion[k] == IBLOCKMV) 
       pWMVDec->m_pDiffMV->iIntra == TRUE)
    {  // if I block
           
        for ( ii = 0; ii < 8; ii++) {
            memset (pDst, 128, 16);
            memset (pDst + pWMVDec->m_iWidthPrevY, 128, 16);
            pDst += pWMVDec->m_iWidthPrevY << 1;
            memset (pDstU, 128, 8);
            memset (pDstV, 128, 8);
            pDstU += pWMVDec->m_iWidthPrevUV;
            pDstV += pWMVDec->m_iWidthPrevUV;
        }
                
    }
    else {  // Inter MB
        I32_WMV iCoordX , iCoordY;
        I32_WMV iIndex;
        I32_WMV iUVLoc;
        I32_WMV iShiftX = pWMVDec->m_pXMotion[k];
        I32_WMV iShiftY = pWMVDec->m_pYMotion[k];           
		int xIndex = pWMVDec->m_ncols<<4;
		int yIndex = pWMVDec->m_nrows<<4;
        
        assert(pWMVDec->m_pXMotion[k] == pWMVDec->m_pXMotion[k+1]);
        assert(pWMVDec->m_pXMotion[k] == pWMVDec->m_pXMotion[k+pWMVDec->m_uintNumMBX*2]);
        assert(pWMVDec->m_pXMotion[k] == pWMVDec->m_pXMotion[k+pWMVDec->m_uintNumMBX*2 +1]);
        
        PullBackMotionVector (pWMVDec, &iShiftX, &iShiftY, iMBX, iMBY);
     
    if(pWMVDec->m_bCodecIsWVC1){
		if( (iShiftX>>2) + xIndex <= -32)  //zou fix
			iShiftX =  (-32-xIndex)<<2;
		else	if( (iShiftX>>2) +xIndex >= pWMVDec->m_iWidthPrevY+32)
			iShiftX = (pWMVDec->m_iWidthPrevY+32 -xIndex)<<2;

		if((iShiftY>>2) + yIndex <= -32)
			iShiftY =  (-32-yIndex)<<2;
		else	if((iShiftY>>2) +yIndex >= pWMVDec->m_iHeightPrevY+32)
			iShiftY = (pWMVDec->m_iHeightPrevY+32 -yIndex)<<2;
    }
   
		iCoordX = (iMBX << 4) + (iShiftX >> 2);   //zou --->vc1CROPMV_LumaPullBack(pPosition, &sMV);   IX = (IX * 16) + (X >> 2);
        iCoordY = (iMBY << 4) + (iShiftY >> 2);
     if(pWMVDec->m_bCodecIsWVC1){
		if(iCoordY< -32)
			iCoordY = -32;
		else if(iCoordY>32 + pWMVDec->m_iHeightPrevY)
			iCoordY = 32 + pWMVDec->m_iHeightPrevY;

		if(iCoordX< -32)
			iCoordX = -32;
		else if(iCoordX>32+pWMVDec->m_iWidthPrevY)
			iCoordX = 32+pWMVDec->m_iWidthPrevY;
    }
       InterpolateMB (pWMVDec, pWMVDec->m_ppxliRef0YPlusExp + pWMVDec->m_iWidthPrevY * iCoordY + iCoordX,
                       pWMVDec->m_iWidthPrevY, pDst, pWMVDec->m_iWidthPrevY, iShiftX & 3, iShiftY & 3, pWMVDec->m_iFilterType);
        
        // Chroma channels
        iIndex = iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX;
        iShiftX = pWMVDec->m_pXMotionC[iIndex];
        iShiftY = pWMVDec->m_pYMotionC[iIndex];
        
     if(pWMVDec->m_bCodecIsWVC1){
		PullBackMotionVector_UV (pWMVDec, &iShiftX, &iShiftY, iMBX, iMBY);  //zou fix 

		if( (iShiftX>>2) + xIndex/2 <= -16)   //zou fix
			iShiftX =  (-16-xIndex/2)<<2;
		else	if( (iShiftX>>2) +xIndex/2 >= pWMVDec->m_iWidthPrevUV+16)
			iShiftX = (pWMVDec->m_iWidthPrevUV+16 -xIndex/2)<<2;

		if((iShiftY>>2) + yIndex/2 <= -16)
			iShiftY =  (-16-yIndex/2)<<2;
		else	if((iShiftY>>2) +yIndex/2 >= pWMVDec->m_iHeightPrevUV+16)
			iShiftY = (pWMVDec->m_iHeightPrevUV+16 -yIndex/2)<<2;
    }
        iCoordX = (iMBX << 3) + (iShiftX >> 2);
        iCoordY = (iMBY << 3) + (iShiftY >> 2);
     if(pWMVDec->m_bCodecIsWVC1){
		if(iCoordY< -16)   //zou fix 
			iCoordY = -16;
		else if(iCoordY>16 + pWMVDec->m_iHeightPrevUV)
			iCoordY = 16 + pWMVDec->m_iHeightPrevUV;

		if(iCoordX< -16)
			iCoordX = -16;
		else if(iCoordX>16+pWMVDec->m_iWidthPrevUV)
			iCoordX = 16+pWMVDec->m_iWidthPrevUV;
    }
        iUVLoc = (iCoordY + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + iCoordX + pWMVDec->EXPANDUV_REFVOP;
          
        InterpolateBlock(pWMVDec, pWMVDec->m_ppxliRef0U + iUVLoc, pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV,
                         iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);
        InterpolateBlock(pWMVDec, pWMVDec->m_ppxliRef0V + iUVLoc, pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV,
                         iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);        
    }
}


/****************************************************************************************
  MotionCompChroma_X9 : chrominance motion compensation
****************************************************************************************/
Void_WMV MotionCompChroma_X9 (tWMVDecInternalMember *pWMVDec, U8_WMV *pDst, UnionBuffer *pError,
                              I32_WMV iMBindex, U8_WMV *pSrc)

{
    I32_WMV  dX , dY ;
    I32_WMV ii;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(MotionCompChroma_X9);
    
    if (pWMVDec->m_pXMotionC[iMBindex] == IBLOCKMV) {  // I block
        for ( ii = 0; ii < 8; ii++)
            memset (pDst + ii * pWMVDec->m_iWidthPrevUV, 128, 8);
        
    }
    else {
		int xIndex = pWMVDec->m_ncols<<3;
		int yIndex = pWMVDec->m_nrows<<3;
        dX = pWMVDec->m_pXMotionC[iMBindex];
        dY = pWMVDec->m_pYMotionC[iMBindex];
    if(pWMVDec->m_bCodecIsWVC1){
		PullBackMotionVector_UV (pWMVDec, &dX, &dY, pWMVDec->m_ncols, pWMVDec->m_nrows);  //zou fix 

		if( (dX>>2) + xIndex <= -16)   //zou fix
			dX =  (-16-xIndex)<<2;
		else	if( (dX>>2) +xIndex >= pWMVDec->m_iWidthPrevUV+16)
			dX = (pWMVDec->m_iWidthPrevUV+16 -xIndex)<<2;

		if((dY>>2) + yIndex <= -16)
			dY =  (-16-yIndex)<<2;
		else	if((dY>>2) +yIndex >= pWMVDec->m_iHeightPrevUV+16)
			dY = (pWMVDec->m_iHeightPrevUV+16 -yIndex)<<2;
    }
        InterpolateBlock(pWMVDec, pSrc, pWMVDec->m_iWidthPrevUV, pDst, pWMVDec->m_iWidthPrevUV, dX, dY, FILTER_BILINEAR);
        
    }
    
    if (pError) {
        I16_WMV *pBuf = pError->i16;
        pWMVDec->m_pAddError(pDst, pDst, pBuf, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
        
    }
}
           
/****************************************************************************************
  Interlace stuff
****************************************************************************************/

/****************************************************************************************
  Interlace stuff
****************************************************************************************/

//#endif // INTERLACE

/****************************************************************************************************
  DecodeMB_X9 : decode macroblock in X9 mode
****************************************************************************************************/

static I32_WMV s_iBlkXformTypeLUT[8] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};
  
static I32_WMV s_iSubblkPatternLUT[8] = {0, 1, 2, 3, 1, 2, 3, 0};




tWMVDecodeStatus DecodeMB_X9 (
                    tWMVDecInternalMember *pWMVDec,
                    CWMVMBMode* pmbmd, 
                    U8_WMV * ppxlcCurrQYMB,
                    U8_WMV * ppxlcCurrQUMB,
                    U8_WMV * ppxlcCurrQVMB,
                    I32_WMV iMBX, I32_WMV iMBY,
                    I32_WMV xRefY, I32_WMV yRefY,
                    I32_WMV xRefUV, I32_WMV yRefUV
)
{
#ifndef _WMV9AP_
    U8_WMV rgCBP = pmbmd->m_rgbCodedBlockPattern;
#endif
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    Bool_WMV  bTop, bBottom, bLeft, bRight;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    UnionBuffer * ppxliErrorQMB;
    tWMVDecodeStatus result;
    U8_WMV * ppxlcRefY ;
    I32_WMV iIntra = 0, iShift, iblk, iDirection;
    Bool_WMV bSkipMB = TRUE_WMV;
    I16_WMV  *pIntra, pPredScaled[16];
    I32_WMV    iOffset ;
    I32_WMV iPxLoc ;
    U8_WMV * ppxlcRefU ;
    U8_WMV * ppxlcRefV ;
    I32_WMV iMBindex ;
    I32_WMV i1;
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
    U32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    I32_WMV  iX ;
    I32_WMV  iY ;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    DQuantDecParam *pDQ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeMB_X9);

    pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP ];

//    FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);

    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    
    // Y-blocks first (4 blocks)
    ppxlcRefY = pWMVDec->m_ppxliRef0Y + ((yRefY >> 1) + pWMVDec->EXPANDY_REFVOP) * pWMVDec->m_iWidthPrevY + (xRefY >> 1) + pWMVDec->EXPANDY_REFVOP;
    iMBindex = iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX;

    for (iblk = 0; iblk < 4; iblk++) {
        I32_WMV iX = (iMBX << 1) + (iblk & 1);
        I32_WMV iY = (iMBY << 1) + ((iblk & 2) >> 1);

#ifndef _WMV9AP_
        Bool_WMV bResidual = (rgCBP>>iblk)&0x1;
        rgCBP2[iblk] =  (rgCBP>>iblk)&0x1 ;
#else
        Bool_WMV bResidual = rgCBP2[iblk];
#endif

        iIntra = (pWMVDec->m_pDiffMV + iblk)->iIntra; //ComputeMVFromDiffMV (iX, iY, pWMVDec->m_pDiffMV + iblk);  // duplication!

        if (iX || iY || iIntra || bResidual)
            bSkipMB = FALSE_WMV;

        if (iIntra) {
            Bool_WMV bDCACPredOn;
            I16_WMV *pPred = NULL_WMV;

            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            pIntra = pWMVDec->m_pX9dct + ((iX + iY * (pWMVDec->m_uintNumMBX << 1)) << 4);
          
            bDCACPredOn = decodeDCTPredictionY (pWMVDec, pmbmd, pIntra, iblk, iX, iY, 
                                                &iShift, &iDirection, pPredScaled);
           
            if (bDCACPredOn) pPred = pPredScaled;

#ifndef _WMV9AP_
            result = DecodeInverseIntraBlockX9 (pWMVDec,
                        ppIntraDCTTableInfo_Dec,
                        iblk,
                        pWMVDec->m_pZigzagScanOrder, bResidual, pPred,
                        iDirection, iShift, pIntra, pmbmd, pDQ); //get the quantized block
            (*pWMVDec->m_pIntraX9IDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
          
#else
            result = DecodeInverseIntraBlockX9 (pWMVDec,
                        ppIntraDCTTableInfo_Dec,
                iblk, bResidual, pPred,
                iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

#endif
             MotionComp_X9 (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, iX, iY, ppxlcRefY);
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
            
#ifdef _WMV9AP_
            if (pWMVDec->m_iOverlap & 1) {
#else
            if (pWMVDec->m_iOverlap) {
#endif
                // overlapped transform copy
                I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 4;
                for ( i1 = 0; i1 < 8; i1++)
                    memcpy (pWMVDec->m_pIntraBlockRow0[iblk & 2] + (iMBX << 4) + ((iblk & 1) << 3) + i1 * iWidth1,
                        ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
            }
        }
#ifdef _WMV9AP_
        else if (rgCBP2[iblk]) {
#else
        else if ((rgCBP>>iblk)&0x1) {
#endif
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec, pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }
            
            pmbmd->m_rgcBlockXformMode[iblk] =(I8_WMV)iXformType;

            if (iXformType == XFORMMODE_8x8) {
                ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8,pDQ);//get the QP      
                (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
            }
            else if (iXformType == XFORMMODE_8x4) {
                bTop = TRUE_WMV;
                bBottom = TRUE_WMV;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions   // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                        bTop = (rgCBP2[iblk] & 2);
                        bBottom = (rgCBP2[iblk] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP2[iblk] =(U8_WMV)( (bTop << 1) | bBottom);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP2[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP2[iblk] & 2);
                        bBottom = (rgCBP2[iblk] & 1);
                    }
                    else {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP2[iblk] = (U8_WMV) ((bTop << 1) | bBottom);
                    }
                }

                if (bTop) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4,pDQ);//get the QP      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bBottom) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4,pDQ);//get the QP      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }
            else if (iXformType == XFORMMODE_4x8) {
                bLeft = TRUE_WMV;
                bRight = TRUE_WMV;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                        bLeft = rgCBP2[iblk] & 2;
                        bRight = rgCBP2[iblk] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP2[iblk] = (U8_WMV)((bLeft << 1) | bRight);                
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP2[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP2[iblk] & 2;
                        bRight = rgCBP2[iblk] & 1;
                    }
                    else {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP2[iblk] = (U8_WMV)((bLeft << 1) | bRight);                
                    }
                }

                if (bLeft) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8,pDQ);//get the QP      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bRight) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8,pDQ);//get the QP      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                } 
            }
            else if (iXformType == XFORMMODE_4x4) {
                I32_WMV i4x4Pattern;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 

                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec, pWMVDec->m_pbitstrmIn) + 1;
                rgCBP2[iblk] = (U8_WMV)i4x4Pattern;

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
            MotionComp_X9 (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, iX, iY, ppxlcRefY);
            bFirstBlock = FALSE;

        }
        else {
            MotionComp_X9 (pWMVDec, ppxlcCurrQYMB, NULL_WMV, iX, iY, ppxlcRefY);
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
        }
        ppxlcCurrQYMB += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);
        ppxlcRefY += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);
	}

    // UV
    iOffset = ((iMBY << 3) + 16) * pWMVDec->m_iWidthPrevUV + (iMBX << 3) + 16;
    iPxLoc = ((yRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (xRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP;
    ppxlcRefU = pWMVDec->m_ppxliRef0U + iPxLoc;
    ppxlcRefV = pWMVDec->m_ppxliRef0V + iPxLoc;
    iX = iMBX;
    iY = iMBY;

#ifndef _WMV9AP_
    rgCBP2[4] =  (rgCBP>>4)&0x1 ;
    rgCBP2[5] =  (rgCBP>>5)&0x1 ;
#endif

    iIntra = (pWMVDec->m_pXMotionC[iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX] == IBLOCKMV);


    // U-block
    if (iIntra) {
        Bool_WMV bDCACPredOn ;
        I16_WMV *pPred = NULL_WMV;
        
        pIntra = pWMVDec->m_pX9dctU + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);
        
        bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
                            &iShift, &iDirection, pPredScaled);
        
        if (bDCACPredOn) pPred = pPredScaled;
        
        
        bSkipMB = FALSE_WMV;
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
        
#ifndef _WMV9AP_
        result = DecodeInverseIntraBlockX9 (pWMVDec,
                    ppInterDCTTableInfo_Dec,
                    iblk, pWMVDec->m_pZigzagScanOrder, (rgCBP>>4)&0x1, pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ);      
        (*pWMVDec->m_pIntraX9IDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
#else
        result = DecodeInverseIntraBlockX9 (pWMVDec,
                    ppInterDCTTableInfo_Dec,
                    iblk,  rgCBP2[4], pPred,   
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

#endif

        MotionCompChroma_X9 (pWMVDec, ppxlcCurrQUMB, ppxliErrorQMB, iMBindex, ppxlcRefU);
      
        pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;

#ifdef _WMV9AP_
        if (pWMVDec->m_iOverlap  & 1) {
#else
        if (pWMVDec->m_iOverlap) {
#endif
            // overlapped transform copy
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraMBRowU0[0] + (iMBX << 3) + i1 * iWidth1,
                ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
    }
#ifdef _WMV9AP_
    else if (rgCBP2[4]) {
#else
    else if ((rgCBP>>4)&0x1) {
#endif
         bSkipMB = FALSE_WMV;

        if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
            I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec, pWMVDec->m_pbitstrmIn);
            iXformType = s_iBlkXformTypeLUT[iIndex];
            iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        }
        
       pmbmd->m_rgcBlockXformMode[4] = (I8_WMV)iXformType;


        if (iXformType == XFORMMODE_8x8) {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8,pDQ);//get the QP 
            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);          
        }

        else if (iXformType == XFORMMODE_8x4) {
            bTop = TRUE_WMV;
            bBottom = TRUE_WMV;
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            if (pWMVDec->m_bRTMContent) {
                if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                    rgCBP2[4] = (U8_WMV) iSubblockPattern;
                    bTop = (rgCBP2[4] & 2);
                    bBottom = (rgCBP2[4] & 1);
                }
                else {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                            bBottom = FALSE;
                        else
                            bTop = FALSE;
                    }
                    rgCBP2[4] = (U8_WMV)((bTop << 1) | bBottom);            
                }
            }
            else {
                if (bFirstBlock) {
                    rgCBP2[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                    bTop = (rgCBP2[iblk] & 2);
                    bBottom = (rgCBP2[iblk] & 1);
                }
                else {
                    if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                            bBottom = FALSE;
                        else
                            bTop = FALSE;
                    }
                    rgCBP2[4] = (U8_WMV)((bTop << 1) | bBottom);            
                }
            }
            
            if (bTop) {

                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4,pDQ);//get the QP    
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom) {

                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4,pDQ);//get the QP    
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }
        

        if (iXformType == XFORMMODE_4x8) {
            bLeft = TRUE_WMV;
            bRight = TRUE_WMV;
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            if (pWMVDec->m_bRTMContent) {
                if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                    rgCBP2[4] = (U8_WMV) iSubblockPattern;
                    bLeft = rgCBP2[4] & 2;
                    bRight = rgCBP2[4] & 1;
                }
                else {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                            bRight = FALSE;
                        else
                            bLeft = FALSE;
                    }
                    rgCBP2[4]= (U8_WMV)((bLeft << 1) | bRight);
                }
            }
            else {
                
                if (bFirstBlock) {
                    rgCBP2[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                    bLeft = rgCBP2[iblk] & 2;
                    bRight = rgCBP2[iblk] & 1;
                }
                else {
                    if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                            bRight = FALSE;
                        else
                            bLeft = FALSE;
                    }
                    rgCBP2[4] =  (U8_WMV)((bLeft << 1) | bRight);
                }
            }
            
            if (bLeft) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8,pDQ);//get the QP      
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8,pDQ);//get the QP      
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }


        else if (iXformType == XFORMMODE_4x4) {
            I32_WMV i4x4Pattern;
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            
            i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec, pWMVDec->m_pbitstrmIn) + 1;
            rgCBP2[4] = (U8_WMV)i4x4Pattern;
            
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
        MotionCompChroma_X9 (pWMVDec, ppxlcCurrQUMB, ppxliErrorQMB, iMBindex, ppxlcRefU);
      
        bFirstBlock = FALSE;
    }
    else {
        MotionCompChroma_X9 (pWMVDec, ppxlcCurrQUMB, NULL, iMBindex, ppxlcRefU);
        pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
    }

    iblk++; //iblk == 5 now


    // V-block
    if (iIntra) {
        Bool_WMV bDCACPredOn;
        I16_WMV *pPred = NULL_WMV;
        
        pIntra = pWMVDec->m_pX9dctV + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);

        bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
                                            &iShift, &iDirection, pPredScaled);
        
        if (bDCACPredOn) 
            pPred = pPredScaled;
        
        bSkipMB = FALSE_WMV;
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;

#ifndef _WMV9AP_
        
        result = DecodeInverseIntraBlockX9 (pWMVDec, 
                    ppInterDCTTableInfo_Dec,
                    iblk,
                    pWMVDec->m_pZigzagScanOrder, (rgCBP>>5)&0x1, pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ);
        
        (*pWMVDec->m_pIntraX9IDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
#else
        result = DecodeInverseIntraBlockX9 (pWMVDec, 
                    ppInterDCTTableInfo_Dec,
                    iblk, rgCBP2[5], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block
#endif
        MotionCompChroma_X9 (pWMVDec, ppxlcCurrQVMB, ppxliErrorQMB, iMBindex, ppxlcRefV);
       
        pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;

#ifdef _WMV9AP_
        if (pWMVDec->m_iOverlap & 1) {
#else
        if (pWMVDec->m_iOverlap) {
#endif
            // overlapped transform copy
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraMBRowV0[0] + (iMBX << 3) + i1 * iWidth1,
                ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
        
    }
#ifdef _WMV9AP_
    else if (rgCBP2[5]) {
#else
    else if ((rgCBP>>5)&0x1) {
#endif
        bSkipMB = FALSE_WMV;

        if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
            I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec, pWMVDec->m_pbitstrmIn);
            iXformType = s_iBlkXformTypeLUT[iIndex];
            iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        }
       
        pmbmd->m_rgcBlockXformMode[5] = (I8_WMV)iXformType;


        if (iXformType == XFORMMODE_8x8) {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8,pDQ);//get the QP  
            if (WMV_Succeeded != result) {
//              FUNCTION_PROFILE_STOP(&fpDecode);
                return result;
            }

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        if (iXformType == XFORMMODE_8x4) {
            bTop = TRUE_WMV;
            bBottom = TRUE_WMV;
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            if (pWMVDec->m_bRTMContent) {
                if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                    rgCBP2[5] = (U8_WMV) iSubblockPattern;
                    bTop = (rgCBP2[5] & 2);
                    bBottom = (rgCBP2[5] & 1);
                }
                else {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                            bBottom = FALSE;
                        else
                            bTop = FALSE;
                    }
                    rgCBP2[5] = (U8_WMV)((bTop << 1) | bBottom);            
                }
            }
            else {
                if (bFirstBlock) {
                    rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                    bTop = (rgCBP2[iblk] & 2);
                    bBottom = (rgCBP2[iblk] & 1);
                }
                else {
                    if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                            bBottom = FALSE;
                        else
                            bTop = FALSE;
                    }
                    rgCBP2[5] = (U8_WMV) ((bTop << 1) | bBottom);            
                }
            }
            
            if (bTop) {

                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4,pDQ);//get the QP
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4,pDQ);//get the QP    
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        if (iXformType == XFORMMODE_4x8) {
            bLeft = TRUE_WMV;
            bRight = TRUE_WMV;
            
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            if (pWMVDec->m_bRTMContent) {
                if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                    rgCBP2[5] = (U8_WMV) iSubblockPattern;
                    bLeft = rgCBP2[5] & 2;
                    bRight = rgCBP2[5] & 1;
                }
                else {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                            bRight = FALSE;
                        else
                            bLeft = FALSE;
                    }
                    rgCBP2[5]= (U8_WMV)((bLeft << 1) | bRight);
                }
            }
            else {
                if (bFirstBlock) {
                    rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                    bLeft = rgCBP2[iblk] & 2;
                    bRight = rgCBP2[iblk] & 1;
                }
                else {
                    if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                            bRight = FALSE;
                        else
                            bLeft = FALSE;
                    }
                    rgCBP2[5] = (U8_WMV)((bLeft << 1) | bRight);
                }
            }
            
            if (bLeft) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8,pDQ);//get the QP      
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8,pDQ);//get the QP      
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }
        else if (iXformType == XFORMMODE_4x4) {
            I32_WMV i4x4Pattern;
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            
            i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec, pWMVDec->m_pbitstrmIn) + 1;
            rgCBP2[5] = (U8_WMV)i4x4Pattern;
            
            if (i4x4Pattern & 8)
            {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (i4x4Pattern & 4)
            {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
            if (i4x4Pattern & 2)
            {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
            }
            if (i4x4Pattern & 1)
            {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
            }
        }


        if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
            I32_WMV j1;
            for( j1= 0; j1 <64; j1++)
                ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
        }

        MotionCompChroma_X9 (pWMVDec, ppxlcCurrQVMB, ppxliErrorQMB, iMBindex, ppxlcRefV);
      
        bFirstBlock =  FALSE;
    }
    else {
        MotionCompChroma_X9 (pWMVDec, ppxlcCurrQVMB, NULL, iMBindex, ppxlcRefV);

        pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
    }
    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = bSkipMB;

    return WMV_Succeeded;
}



/****************************************************************************************************
  DecodeMB_1MV : WMV3 1MV macroblock decode
****************************************************************************************************/

tWMVDecodeStatus DecodeMB_1MV_WMVA_Intra (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    I32_WMV iMBX, I32_WMV iMBY,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
)
{
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    // Bool_WMV  bTop, bBottom, bLeft, bRight;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    tWMVDecodeStatus result;
    UnionBuffer * ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    Bool_WMV bXEvenY, bYEvenY;
    I32_WMV iShift, iblk, iDirection;
    I16_WMV  *pIntra, pPredScaled [16], *pPredPtr;
    // I32_WMV iIntra;
    I32_WMV iX, iY;
    Bool_WMV bResidual;
    DQuantDecParam *pDQ;
	Bool_WMV bDCACPredOn;
	I16_WMV *pPred = NULL;
	Bool_WMV bNoDQandNoSlice;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeMB_1MV_WMVA_Intra);
    
	bNoDQandNoSlice = (!pWMVDec->m_bDQuantOn) && (!pWMVDec->m_bSliceWMVA);

    pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP ];
    
    // FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);
    
    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    // Y-blocks first (4 blocks)
    bXEvenY = !(xRefY & 1); bYEvenY = !(yRefY & 1);
    
    iX = iMBX;
    iY = iMBY;
    
    for (iblk = 0; iblk < 4; iblk++) {

		pPred = NULL;
        iX = (iMBX << 1) + (iblk & 1);
        iY = (iMBY << 1) + ((iblk & 2) >> 1);

        bResidual = rgCBP2[iblk];

        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
        
        pIntra = pWMVDec->m_pX9dct + ((iX + iY * (pWMVDec->m_uintNumMBX << 1)) << 4);
		if (bNoDQandNoSlice) {
			bDCACPredOn = decodeDCTPredictionY_IFrameNoDQuantAndNoSlice (pWMVDec, pmbmd, pIntra, iblk, iX, iY, 
				&iShift, &iDirection, &pPredPtr);

			result = DecodeInverseIntraBlockX9 (pWMVDec, 
				ppIntraDCTTableInfo_Dec, iblk,
				bResidual, pPredPtr,
				iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

		} else {
			bDCACPredOn = decodeDCTPredictionY (pWMVDec, pmbmd, pIntra, iblk, iX, iY, 
				&iShift, &iDirection, pPredScaled);

			if (bDCACPredOn) 
				pPred = pPredScaled;

			result = DecodeInverseIntraBlockX9 (pWMVDec, 
				ppIntraDCTTableInfo_Dec, iblk,
				bResidual, pPred,
				iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block
		}
        


        // overlapped transform copy

        if (pWMVDec->m_iOverlap & 1) {
            I32_WMV i1;
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 4;
            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraBlockRow0[iblk & 2] + (iMBX << 4) + ((iblk & 1) << 3) + i1 * iWidth1,
                ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
     
        pWMVDec->m_pAddErrorIntra (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);        
        pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
       
        ppxlcCurrQYMB += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);   
    }

    // U-block
    pPred = NULL;
    pIntra = pWMVDec->m_pX9dctU + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);
	if (bNoDQandNoSlice) {
		bDCACPredOn = decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice (pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
											&iShift, &iDirection, &pPredPtr);
		ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
		result = DecodeInverseIntraBlockX9 (pWMVDec, 
			ppInterDCTTableInfo_Dec,
					iblk,  rgCBP2[4], pPredPtr,
					iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block
	} else {
		bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
											&iShift, &iDirection, pPredScaled);

		if (bDCACPredOn) 
			pPred = pPredScaled;
    
		ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    
		result = DecodeInverseIntraBlockX9 (pWMVDec, 
			ppInterDCTTableInfo_Dec,
					iblk,  rgCBP2[4], pPred,
					iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block
	}
    


    // overlapped transform copy

    if (pWMVDec->m_iOverlap & 1) {

        I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
        I32_WMV i1;
        for ( i1 = 0; i1 < 8; i1++)
            memcpy (pWMVDec->m_pIntraMBRowU0[0] + (iMBX << 3) + i1 * iWidth1,
            ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
    }
       
    pWMVDec->m_pAddErrorIntra(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
    
    pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
   
   iblk++;
   
   // V-block
   pPred = NULL; 
   pIntra = pWMVDec->m_pX9dctV + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);
   if (bNoDQandNoSlice) {
	   bDCACPredOn = decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice (pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
		   &iShift, &iDirection, &pPredPtr);

	   ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
   
	   result = DecodeInverseIntraBlockX9 (pWMVDec,
		   ppInterDCTTableInfo_Dec, iblk,
					rgCBP2[5], pPredPtr,
					iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

   } else {
	   bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
		   &iShift, &iDirection, pPredScaled);
	   
	   if (bDCACPredOn) pPred = pPredScaled;
   
	   ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
   
	   result = DecodeInverseIntraBlockX9 (pWMVDec,
		   ppInterDCTTableInfo_Dec, iblk,
					rgCBP2[5], pPred,
					iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block
   }
   

    if (pWMVDec->m_iOverlap & 1) {

       I32_WMV i1 ;
       I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
       for ( i1 = 0; i1 < 8; i1++)
           memcpy (pWMVDec->m_pIntraMBRowV0[0] + (iMBX << 3) + i1 * iWidth1,
           ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
   }
     
   pWMVDec->m_pAddErrorIntra(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);

   pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;

    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = FALSE_WMV;
    
    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeMB_1MV (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    I32_WMV iMBX, I32_WMV iMBY,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
)
{
#ifndef _WMV9AP_
    U8_WMV rgCBP = pmbmd->m_rgbCodedBlockPattern;
#endif
    U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
    Bool_WMV  bTop, bBottom, bLeft, bRight;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    tWMVDecodeStatus result;
    UnionBuffer * ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    //U8_WMV __huge* ppxlcRefY;
    Bool_WMV bXEvenY, bYEvenY;
    I32_WMV iShift, iblk, iDirection;
    I16_WMV  *pIntra, pPredScaled [16];
    I32_WMV iIntra, iX, iY;
    Bool_WMV bSkipMB;
    Bool_WMV bResidual;
    I32_WMV j1;
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
    I32_WMV    iOffset;
    I32_WMV iPxLoc ;
    //U8_WMV __huge* ppxlcRefU ;
    //U8_WMV __huge* ppxlcRefV ;
    I32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    DQuantDecParam *pDQ;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeMB_1MV);
    
    pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP ];
    
    // FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);
    
    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    // Y-blocks first (4 blocks)
 
    //ppxlcRefY = pWMVDec->m_ppxliRef0Y + ((yRefY >> 1) + pWMVDec->EXPANDY_REFVOP) * pWMVDec->m_iWidthPrevY + (xRefY >> 1) + pWMVDec->EXPANDY_REFVOP;
 
    bXEvenY = !(xRefY & 1); bYEvenY = !(yRefY & 1);

    iIntra = pWMVDec->m_pDiffMV->iIntra;
    
    iX = iMBX;
    iY = iMBY;
    bSkipMB = !(iMBX || iMBY || iIntra);
    
    // motion comp
    MotionCompMB_WMV3 (pWMVDec, ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB, iMBX, iMBY);
    
    for (iblk = 0; iblk < 4; iblk++) {
        iX = (iMBX << 1) + (iblk & 1);
        iY = (iMBY << 1) + ((iblk & 2) >> 1);
#ifndef _WMV9AP_
        bResidual = (rgCBP>>iblk)&0x1; //rgCBP2[iblk];
        rgCBP2[iblk] =  (rgCBP>>iblk)&0x1 ;
#else
        bResidual = rgCBP2[iblk];
#endif
        bSkipMB &= !bResidual;

        if (iIntra) {
            Bool_WMV bDCACPredOn ;
            I16_WMV *pPred = NULL;

            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            
            pIntra = pWMVDec->m_pX9dct + ((iX + iY * (pWMVDec->m_uintNumMBX << 1)) << 4);
            bDCACPredOn = decodeDCTPredictionY (pWMVDec, pmbmd, pIntra, iblk, iX, iY, 
                &iShift, &iDirection, pPredScaled);
            
            if (bDCACPredOn) 
                pPred = pPredScaled;

#ifndef _WMV9AP_
            result = DecodeInverseIntraBlockX9 (pWMVDec, 
                ppIntraDCTTableInfo_Dec, iblk,
                pWMVDec->m_pZigzagScanOrder, bResidual, pPred,
                iDirection, iShift, pIntra, pmbmd, pDQ); //get the quantized block
            
            (*pWMVDec->m_pIntraX9IDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
#else
            result = DecodeInverseIntraBlockX9 (pWMVDec, 
                ppIntraDCTTableInfo_Dec, iblk,
                bResidual, pPred,
                iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

#endif
			if(result != WMV_Succeeded)
				return result;

            // overlapped transform copy

#ifdef _WMV9AP_
            if (pWMVDec->m_iOverlap & 1) {
#else
            if (pWMVDec->m_iOverlap) {
#endif
                I32_WMV i1;
                I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 4;
                for ( i1 = 0; i1 < 8; i1++)
                    memcpy (pWMVDec->m_pIntraBlockRow0[iblk & 2] + (iMBX << 4) + ((iblk & 1) << 3) + i1 * iWidth1,
                    ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
            }
         
            pWMVDec->m_pAddError (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);
         
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
        }
#ifdef _WMV9AP_
        else if (rgCBP2[iblk]) {
#else
        else if ((rgCBP>>iblk)&0x1) {
#endif
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec, pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }
            
            //pWMVDec->m_pchXformType[pWMVDec->m_iModeXforpWMVDec->m_index + iblk] = (I8_WMV)iXformType;
            pmbmd->m_rgcBlockXformMode[iblk] =(I8_WMV)iXformType;

            if (iXformType == XFORMMODE_8x8) {
                ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block      
                (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
            }
            else if (iXformType == XFORMMODE_8x4) {
                bTop = TRUE;
                bBottom = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions   // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                        bTop = (rgCBP2[iblk] & 2);
                        bBottom = (rgCBP2[iblk] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP2[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }
                else {
                    
                    if (bFirstBlock) {
                        rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP2[iblk] & 2);
                        bBottom = (rgCBP2[iblk] & 1);
                    }
                    else {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP2[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }
                if (bTop) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bBottom) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block
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
                        rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                        bLeft = rgCBP2[iblk] & 2;
                        bRight = rgCBP2[iblk] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP2[iblk]= (U8_WMV)((bLeft << 1) | bRight);
                    }
                }
                else {
                    
                    if (bFirstBlock) {
                        rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP2[iblk] & 2;
                        bRight = rgCBP2[iblk] & 1;
                    }
                    else {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP2[iblk] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }
                
                if (bLeft) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bRight) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                } 
            }
            else if (iXformType == XFORMMODE_4x4)
            {
                I32_WMV i4x4Pattern;
                
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                
                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec, pWMVDec->m_pbitstrmIn) + 1;
                rgCBP2[iblk] = (U8_WMV)i4x4Pattern;
                
                if (i4x4Pattern & 8)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (i4x4Pattern & 4)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
                if (i4x4Pattern & 2)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
                }
                if (i4x4Pattern & 1)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
                }
            }
            
            if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                for( j1= 0; j1 <64; j1++)
                    ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
            }
         
            pWMVDec->m_pAddError (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);
          
            bFirstBlock = FALSE;
        }
        else
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
      
        ppxlcCurrQYMB += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);
        //ppxlcRefY += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);
	}

    // UV
    iOffset = ((iMBY << 3) + 16) * pWMVDec->m_iWidthPrevUV + (iMBX << 3) + 16;
    iPxLoc = ((yRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (xRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP;
    //ppxlcRefU = pWMVDec->m_ppxliRef0U + iPxLoc;
    //ppxlcRefV = pWMVDec->m_ppxliRef0V + iPxLoc;
    
    iX = iMBX;
    iY = iMBY;

#ifndef _WMV9AP_
    rgCBP2[4] =  (rgCBP>>4)&0x1 ;
    rgCBP2[5] =  (rgCBP>>5)&0x1 ;
#endif

    // U-block
    if (iIntra) {
        Bool_WMV bDCACPredOn;
        I16_WMV *pPred = NULL;

        
        pIntra = pWMVDec->m_pX9dctU + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);
        
        bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
                                            &iShift, &iDirection, pPredScaled);
        
        if (bDCACPredOn) 
            pPred = pPredScaled;
        
        bSkipMB = FALSE;
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
        
#ifndef _WMV9AP_
        result = DecodeInverseIntraBlockX9 (pWMVDec, 
            ppInterDCTTableInfo_Dec,
            iblk, pWMVDec->m_pZigzagScanOrder, (rgCBP>>4)&0x1, pPred,
            iDirection, iShift, pIntra, pmbmd, pDQ);      
        
        (*pWMVDec->m_pIntraX9IDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
#else
        result = DecodeInverseIntraBlockX9 (pWMVDec, 
            ppInterDCTTableInfo_Dec,
                    iblk,  rgCBP2[4], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

#endif
        // overlapped transform copy

#ifdef _WMV9AP_
        if (pWMVDec->m_iOverlap & 1) {
#else
        if (pWMVDec->m_iOverlap) {
#endif
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
            I32_WMV i1;
            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraMBRowU0[0] + (iMBX << 3) + i1 * iWidth1,
                ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
      
        pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
        
        pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
    }
#ifdef _WMV9AP_
    else if (rgCBP2[4]) {
#else
    else if ((rgCBP>>4)&0x1) {
#endif
        bSkipMB = FALSE;
        
        if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
            I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec, pWMVDec->m_pbitstrmIn);
            iXformType = s_iBlkXformTypeLUT[iIndex];
            iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        }
        
        // pWMVDec->m_pchXformType[pWMVDec->m_iModeXforpWMVDec->m_index + 4] = (I8_WMV)iXformType;
        pmbmd->m_rgcBlockXformMode[4] =(I8_WMV)iXformType;

        if (iXformType == XFORMMODE_8x8) {
            
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block 
            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);          
        }
        else if (iXformType == XFORMMODE_8x4) {
            bTop = TRUE;
            bBottom = TRUE;
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            if (pWMVDec->m_bRTMContent) {
                if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                    rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                    bTop = (rgCBP2[iblk] & 2);
                    bBottom = (rgCBP2[iblk] & 1);
                }
                else {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                            bBottom = FALSE;
                        else
                            bTop = FALSE;
                    }
                    rgCBP2[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                }
            }
            else {
                if (bFirstBlock) {
                    rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                    bTop = (rgCBP2[iblk] & 2);
                    bBottom = (rgCBP2[iblk] & 1);
                }
                else {
                    if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                            bBottom = FALSE;
                        else
                            bTop = FALSE;
                    }
                    rgCBP2[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                }
            }
            if (bTop) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
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
                    rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                    bLeft = rgCBP2[iblk] & 2;
                    bRight = rgCBP2[iblk] & 1;
                }
                else {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                            bRight = FALSE;
                        else
                            bLeft = FALSE;
                    }
                    rgCBP2[iblk]= (U8_WMV)((bLeft << 1) | bRight);
                }
            }
            else {
                
                if (bFirstBlock) {
                    rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                    bLeft = rgCBP2[iblk] & 2;
                    bRight = rgCBP2[iblk] & 1;
                }
                else {
                    if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                        if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                            bRight = FALSE;
                        else
                            bLeft = FALSE;
                    }
                    rgCBP2[iblk] = (U8_WMV)((bLeft << 1) | bRight);
                }
            }
            
            if (bLeft) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight) {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                
                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }
        
        else if (iXformType == XFORMMODE_4x4) {
            I32_WMV i4x4Pattern;
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            
            i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec, pWMVDec->m_pbitstrmIn) + 1;
            rgCBP2[iblk] = (U8_WMV)i4x4Pattern;
            
            if (i4x4Pattern & 8)
            {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (i4x4Pattern & 4)
            {

                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
            if (i4x4Pattern & 2)
            {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
            }
            if (i4x4Pattern & 1)
            {
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
            }
        }
        
        if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
            for( j1= 0; j1 <64; j1++)
                ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
        }
           
        pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
        
        bFirstBlock = FALSE;
        
   }
   else
       pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
   
   iblk++;
   
   // V-block
   if (iIntra) {
       Bool_WMV bDCACPredOn;
       I16_WMV *pPred = NULL;
       
       pIntra = pWMVDec->m_pX9dctV + ((iMBX + iMBY * pWMVDec->m_uintNumMBX) << 4);
       
       bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
           &iShift, &iDirection, pPredScaled);
       
       if (bDCACPredOn) pPred = pPredScaled;
       
       bSkipMB = FALSE;
       ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
       
#ifndef _WMV9AP_
       result = DecodeInverseIntraBlockX9 (pWMVDec,
           ppInterDCTTableInfo_Dec, iblk,
           pWMVDec->m_pZigzagScanOrder, (rgCBP>>5)&0x1, pPred,
           iDirection, iShift, pIntra, pmbmd, pDQ);
       
       (*pWMVDec->m_pIntraX9IDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);

       // overlapped transform copy
       if (pWMVDec->m_iOverlap) {
#else
       result = DecodeInverseIntraBlockX9 (pWMVDec,
           ppInterDCTTableInfo_Dec, iblk,
                    rgCBP2[5], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

        if (pWMVDec->m_iOverlap & 1) {
#endif
           I32_WMV i1 ;
           I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
           for ( i1 = 0; i1 < 8; i1++)
               memcpy (pWMVDec->m_pIntraMBRowV0[0] + (iMBX << 3) + i1 * iWidth1,
               ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
       }
           
       pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
       
       pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
   }
#ifdef _WMV9AP_
    else if (rgCBP2[5]) {
#else
   else if ((rgCBP>>5)&0x1) {
#endif
       bSkipMB = FALSE;
       
       if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
           I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec, pWMVDec->m_pbitstrmIn);
           iXformType = s_iBlkXformTypeLUT[iIndex];
           iSubblockPattern = s_iSubblkPatternLUT[iIndex];
       }
       // pWMVDec->m_pchXformType[pWMVDec->m_iModeXforpWMVDec->m_index + 5] = (I8_WMV)iXformType;
       pmbmd->m_rgcBlockXformMode[5] =(I8_WMV)iXformType;


       if (iXformType == XFORMMODE_8x8) {
           ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
           result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block  
           if (WMV_Succeeded != result) {
               //                FUNCTION_PROFILE_STOP(&fpDecode);
               return result;
           }
           (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
       }
       
       else if (iXformType == XFORMMODE_8x4) {
           bTop = TRUE;
           bBottom = TRUE;
           ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
           ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
           if (pWMVDec->m_bRTMContent) {
               if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                   rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                   bTop = (rgCBP2[iblk] & 2);
                   bBottom = (rgCBP2[iblk] & 1);
               }
               else {
                   if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                       if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                           bBottom = FALSE;
                       else
                           bTop = FALSE;
                   }
                   rgCBP2[iblk] = (U8_WMV)((bTop << 1) | bBottom);
               }
           }
           else {
               if (bFirstBlock) {
                   rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                   bTop = (rgCBP2[iblk] & 2);
                   bBottom = (rgCBP2[iblk] & 1);
               }
               else {
                   if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                       if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                           bBottom = FALSE;
                       else
                           bTop = FALSE;
                   }
                   rgCBP2[iblk] = (U8_WMV)((bTop << 1) | bBottom);
               }
           }
           
           if (bTop) {
               result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block
               
               (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
               
           }
           if (bBottom) {
               result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block    
               
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
                   rgCBP2[iblk] = (U8_WMV) iSubblockPattern;
                   bLeft = rgCBP2[iblk] & 2;
                   bRight = rgCBP2[iblk] & 1;
               }
               else {
                   if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                       if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                           bRight = FALSE;
                       else
                           bLeft = FALSE;
                   }
                   rgCBP2[iblk]= (U8_WMV)((bLeft << 1) | bRight);
               }
           }
           else {
               
               if (bFirstBlock) {
                   rgCBP2[iblk] = (U8_WMV) pmbmd->m_iSubblockPattern;
                   bLeft = rgCBP2[iblk] & 2;
                   bRight = rgCBP2[iblk] & 1;
               }
               else {
                   if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1) {
                       if (BS_getBit (pWMVDec->m_pbitstrmIn) == 1)
                           bRight = FALSE;
                       else
                           bLeft = FALSE;
                   }
                   rgCBP2[iblk] = (U8_WMV)((bLeft << 1) | bRight);
               }
           }
           
           if (bLeft) {
               result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
               
               (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
           }
           if (bRight) {
               result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
               
               (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
           }
       }
       
       else if (iXformType == XFORMMODE_4x4)
       {
           I32_WMV i4x4Pattern;
           ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
           ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
           
           i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec, pWMVDec->m_pbitstrmIn) + 1;
           rgCBP2[iblk] = (U8_WMV)i4x4Pattern;
           
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
           for( j1= 0; j1 <64; j1++)
               ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
       }
      
       pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);

       bFirstBlock = FALSE;
       
    }
    else
        pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
    
    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = bSkipMB;
    
    return WMV_Succeeded;
}

#ifdef _WMV9AP_
/****************************************************************************************
  MotionCompBlockFieldMode : block motion compensation for field-picture blocks
****************************************************************************************/
Void_WMV MotionCompBlockFieldMode (tWMVDecInternalMember *pWMVDec, 
    U8_WMV *pDst, UnionBuffer *pError, U8_WMV *pSrc,
    I32_WMV iBlockX, I32_WMV iBlockY, I32_WMV iMVx, I32_WMV iMVy)
{
    I32_WMV  iFilterType = pWMVDec->m_iFilterType;
    //I32_WMV  k = iBlockY * pWMVDec->m_uintNumMBX * 2 + iBlockX;
    I32_WMV  iCoordX;
    I32_WMV  iCoordY;

 
    PullBackMotionVector (pWMVDec, &iMVx, &iMVy, iBlockX >> 1, iBlockY >> 1);
    iCoordX = iBlockX * 8 + (iMVx >> 2);
    iCoordY = iBlockY * 8 + (iMVy >> 2);
    InterpolateBlock(pWMVDec, pSrc, pWMVDec->m_iWidthPrevY, pDst, pWMVDec->m_iWidthPrevY, iMVx & 3, 
        iMVy & 3, iFilterType);

#ifdef _ANALYZEMC_
    if (pError && g_bAddError) {
#else
    if (pError) {
#endif
        I16_WMV *pBuf = pError->i16;
        pWMVDec->m_pAddError(pDst, pDst, pBuf, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);
    }
}

/****************************************************************************************
  MotionCompChromaBlockFieldMode : chrominance motion compensation for field-pictures
****************************************************************************************/
Void_WMV MotionCompChromaBlockFieldMode (tWMVDecInternalMember *pWMVDec, 
    U8_WMV *pDst, UnionBuffer *pError, U8_WMV *pSrc,
    I32_WMV iMVx, I32_WMV iMVy)
{
    InterpolateBlock(pWMVDec, pSrc, pWMVDec->m_iWidthPrevUV, pDst, pWMVDec->m_iWidthPrevUV, iMVx & 3, iMVy & 3, FILTER_BILINEAR);

#ifdef _ANALYZEMC_
    if (pError && g_bAddError) {
#else
    if (pError) {
#endif
        I16_WMV *pBuf = pError->i16;
        pWMVDec->m_pAddError(pDst, pDst, pBuf, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
    }
}

    
/****************************************************************************************************
  DecodeFieldMode4MV : decode field-picture coded 4MV macroblock
****************************************************************************************************/
I32_WMV DecodeFieldMode4MV (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    U8_WMV __huge* ppxlcSameRefY,
    U8_WMV __huge* ppxlcSameRefU,
    U8_WMV __huge* ppxlcSameRefV,
    U8_WMV __huge* ppxlcOppRefY,
    U8_WMV __huge* ppxlcOppRefU,
    U8_WMV __huge* ppxlcOppRefV,
    I32_WMV iMBX, I32_WMV iMBY
)
{
#ifndef PPCWMP

    U8_WMV* rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    Bool_WMV  bTop, bBottom, bLeft, bRight;
    DQuantDecParam *pDQ;
    
    Bool_WMV bFirstBlock = pWMVDec->m_bMBXformSwitching;
    I32_WMV iMVx, iMVy;
    I32_WMV iSameFieldCount = 0, iOppFieldCount = 0;

    I32_WMV iSubblockPattern = (U8_WMV)pmbmd->m_iSubblockPattern;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    UnionBuffer __huge* ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    I32_WMV result;
    U8_WMV __huge* ppxlcRefU;
    U8_WMV __huge* ppxlcRefV;
    I32_WMV    iOffset;

    //I32_WMV  iX = iMBX;
    //I32_WMV  iY = iMBY;

    // Y-blocks first (4 blocks)
    U8_WMV __huge* ppxlcRefY;

    //I32_WMV iMBindex = iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX;
    I32_WMV iIntra = 0, iShift, iblk, iDirection;
    Bool_WMV bSkipMB = TRUE;
    I16_WMV  *pIntra, pPredScaled [16];

    
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;

    FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);

    pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    

    for (iblk = 0; iblk < 4; iblk++) {
        I32_WMV iX = iMBX * 2 + (iblk & 1);
        I32_WMV iY = iMBY * 2 + ((iblk & 2) >> 1);
        Bool_WMV bResidual = rgCBP[iblk];

        iIntra = (pWMVDec->m_pDiffMV + iblk)->iIntra; //ComputeMVFromDiffMV (iX, iY, pWMVDec->m_pDiffMV + iblk);  // duplication!
        if (iX || iY || iIntra || bResidual)
        bSkipMB = FALSE;

        if (iIntra) {
            Bool_WMV bDCACPredOn ;
            I16_WMV *pPred = NULL;
            I32_WMV ii;

            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;

            pIntra = pWMVDec->m_pX9dct + (iX + iY * pWMVDec->m_uintNumMBX * 2) * 16;
            bDCACPredOn = decodeDCTPredictionY (pWMVDec, pmbmd, pIntra, iblk, iX, iY, 
                &iShift, &iDirection, pPredScaled);

            
            if (bDCACPredOn) pPred = pPredScaled;
            result = DecodeInverseIntraBlockX9 (pWMVDec, 
                ppIntraDCTTableInfo_Dec, iblk,
                bResidual, pPred,
                iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block


            for ( ii = 0; ii < 8; ii++)
                memset (ppxlcCurrQYMB + ii * pWMVDec->m_iWidthPrevY, 128, 8);
            pWMVDec->m_pAddError(ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;

            // overlapped transform copy
            if (pWMVDec->m_iOverlap & 1) {
                I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX * 16;
                I32_WMV i1;
                for ( i1 = 0; i1 < 8; i1++)
                    memcpy (pWMVDec->m_pIntraBlockRow0[iblk & 2] + iMBX * 16 + (iblk & 1) * 8 + i1 * iWidth1,
                        ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
            }
        }
        else {
            I32_WMV iBlkIndex = (iMBY * 4 + (iblk & 2)) * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX * 2 + (iblk & 1);
            I32_WMV iOffset;
            if (pWMVDec->m_iCurrentField == 0)
                iOffset = 4;
            else
                iOffset = 2;
            iMVx = pWMVDec->m_pXMotion[iBlkIndex];
            iMVy = pWMVDec->m_pYMotion[iBlkIndex];
         if(!pWMVDec->m_bCodecIsWVC1){
            if (iMVy & 1) {//zou 328
                iMVy -= iOffset;
                iMVy = (iMVy & ~7) | 4 | ((iMVy & 7) >> 1);
            }
            else
                iMVy = (iMVy & ~7) | ((iMVy & 7) >> 1);
         }

            PullBackFieldModeMotionVector (pWMVDec, &iMVx, &iMVy, iX >> 1, iY >> 1);

         if(pWMVDec->m_bCodecIsWVC1){
	    iMVy = iMVy>>1; //zou 328

            if (iMVy & 4) {
                ppxlcRefY = ppxlcOppRefY   + (iblk & 2) * 4 * pWMVDec->m_iWidthPrevY + (iblk & 1) * 8 + (iMVy >> 2) * (pWMVDec->m_iWidthPrevY /*>> 1*/) + (iMVx >> 2);
                iOppFieldCount ++;
            }
            else {
                ppxlcRefY = ppxlcSameRefY + (iblk & 2) * 4 * pWMVDec->m_iWidthPrevY  + (iblk & 1) * 8 + (iMVy >> 2) * (pWMVDec->m_iWidthPrevY /*>> 1*/) + (iMVx >> 2);
                iSameFieldCount ++;
            }
         }
         else {
           if (iMVy & 4) {
                ppxlcRefY = ppxlcOppRefY + (iblk & 2) * 4 * pWMVDec->m_iWidthPrevY + (iblk & 1) * 8 + (iMVy >> 2) * (pWMVDec->m_iWidthPrevY >> 1) + (iMVx >> 2);
                iOppFieldCount ++;
            }
            else {
                ppxlcRefY = ppxlcSameRefY + (iblk & 2) * 4 * pWMVDec->m_iWidthPrevY  + (iblk & 1) * 8 + (iMVy >> 2) * (pWMVDec->m_iWidthPrevY >> 1) + (iMVx >> 2);
                iSameFieldCount ++;
            }
         }

            if (rgCBP[iblk]) {
                if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                    I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec,pWMVDec->m_pbitstrmIn);
                    iXformType = s_iBlkXformTypeLUT[iIndex];
                    iSubblockPattern = s_iSubblkPatternLUT[iIndex];
                }

                pmbmd->m_rgcBlockXformMode[iblk] = (I8_WMV)iXformType;

                if (iXformType == XFORMMODE_8x8) {

                    ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
                }

                else if (iXformType == XFORMMODE_8x4) {

                    bTop = TRUE;
                    bBottom = TRUE;
                    ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                    memset(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
                    if (pWMVDec->m_bRTMContent) {
                        if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                            rgCBP[iblk] = iSubblockPattern;
                            bTop = (rgCBP[iblk] & 2);
                            bBottom = (rgCBP[iblk] & 1);
                        }
                        else {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                                if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                    bBottom = FALSE;
                                else
                                    bTop = FALSE;
                            }
                            rgCBP[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                        }
                    }
                    else {
                        if (bFirstBlock) {
                            rgCBP[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                            bTop = (rgCBP[iblk] & 2);
                            bBottom = (rgCBP[iblk] & 1);
                        }
                        else {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                                if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                    bBottom = FALSE;
                                else
                                    bTop = FALSE;
                            }
                            rgCBP[iblk] = (U8_WMV)((bTop << 1) | bBottom);
                        }
                    }

                    if (bTop) {
                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                        (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                    }
                    if (bBottom) {

                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block
                        (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                    }
                }

                else if (iXformType == XFORMMODE_4x8) {

                    bLeft = TRUE;
                    bRight = TRUE;
                    ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                    memset(ppxliErrorQMB, 0, 256);
                    if (pWMVDec->m_bRTMContent) {
                        if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                            rgCBP[iblk] = iSubblockPattern;
                            bLeft = rgCBP[iblk] & 2;
                            bRight = rgCBP[iblk] & 1;
                        }
                        else {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                                if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                    bRight = FALSE;
                                else
                                    bLeft = FALSE;
                            }
                            rgCBP[iblk] = (U8_WMV)((bLeft << 1) | bRight);                
                        }
                    }
                    else {
                        if (bFirstBlock) {
                            rgCBP[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                            bLeft = rgCBP[iblk] & 2;
                            bRight = rgCBP[iblk] & 1;
                        }
                        else {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                                if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                    bRight = FALSE;
                                else
                                    bLeft = FALSE;
                            }
                            rgCBP[iblk] = (U8_WMV)((bLeft << 1) | bRight);                
                        }
                    }

                    if (bLeft) {
                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                        (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                    }
                    if (bRight) {
                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                        (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                    } 
                }

                else if (iXformType == XFORMMODE_4x4)
                {
                    I32_WMV i4x4Pattern ;
                    ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                    memset(ppxliErrorQMB, 0, 256);

                    i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec,pWMVDec->m_pbitstrmIn) + 1;
                    rgCBP[iblk] = i4x4Pattern;

                    if (i4x4Pattern & 8)
                    {
                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                        (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                    }
                    if (i4x4Pattern & 4)
                    {
                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                        (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                    }
                    if (i4x4Pattern & 2)
                    {
                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                        (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
                    }
                    if (i4x4Pattern & 1)
                    {
                        result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                        (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
                    }
                }

                if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                    I32_WMV j1;
                    for( j1= 0; j1 <64; j1++)
                        ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
                }

#ifdef _GENERATE_DXVA_DATA_
                if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                    dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
                MotionCompBlockFieldMode (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, iX, iY, iMVx, iMVy);
                bFirstBlock = FALSE;
            }
            else {
                MotionCompBlockFieldMode (pWMVDec, ppxlcCurrQYMB, NULL, ppxlcRefY, iX, iY, iMVx, iMVy);
                pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
            }
        }

        ppxlcCurrQYMB += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);
    }

    // UV
    iOffset = (iMBY * 8 + 16) * pWMVDec->m_iWidthPrevUV + iMBX * 8 + 16;
    
    iMVx = pWMVDec->m_pXMotionC[iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX];
    iMVy = pWMVDec->m_pYMotionC[iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX];
    iIntra = (iMVx == IBLOCKMV);

    // U-block
    if (iIntra) {
        Bool_WMV bDCACPredOn;
        I16_WMV *pPred = NULL;
        I32_WMV ii;
        pIntra = pWMVDec->m_pX9dctU + (iMBX + iMBY * pWMVDec->m_uintNumMBX) * 16;

        bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
            &iShift, &iDirection, pPredScaled);
        
        if (bDCACPredOn) pPred = pPredScaled;

        bSkipMB = FALSE;
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;

        result = DecodeInverseIntraBlockX9 (pWMVDec, 
                    ppInterDCTTableInfo_Dec,
                    4, rgCBP[4], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE);      

        for ( ii = 0; ii < 8; ii++)
            memset (ppxlcCurrQUMB + ii * pWMVDec->m_iWidthPrevUV, 128, 8);
        pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
        pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
        if (pWMVDec->m_iOverlap & 1) {
            // overlapped transform copy
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
            I32_WMV i1;
            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraMBRowU0[0] + iMBX * 8 + i1 * iWidth1,
                ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
    }
    else {

        if (iOppFieldCount > iSameFieldCount) 
            ppxlcRefU = ppxlcOppRefU + (iMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iMVx >> 2);
        else
            ppxlcRefU = ppxlcSameRefU + (iMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iMVx >> 2);

        if (rgCBP [4]) {
            bSkipMB = FALSE;
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec,pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }

            pmbmd->m_rgcBlockXformMode[4] = (I8_WMV)iXformType;

            if (iXformType == XFORMMODE_8x8) {
                ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block 
                (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);          
            }

            else if (iXformType == XFORMMODE_8x4) {

                bTop = TRUE;
                bBottom = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);

                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[4] = iSubblockPattern;
                        bTop = (rgCBP[4] & 2);
                        bBottom = (rgCBP[4] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[4] = (U8_WMV)((bTop << 1) | bBottom);            
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[4] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP[4] & 2);
                        bBottom = (rgCBP[4] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[4] = (U8_WMV)((bTop << 1) | bBottom);            
                    }
                }

                if (bTop) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bBottom) {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }

            else if (iXformType == XFORMMODE_4x8) {
                bLeft = TRUE;
                bRight = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[4] = iSubblockPattern;
                        bLeft = rgCBP[4] & 2;
                        bRight = rgCBP[4] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[4] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[4] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP[4] & 2;
                        bRight = rgCBP[4] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[4] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }

                if (bLeft) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bRight) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }

            else if (iXformType == XFORMMODE_4x4)
            {
                I32_WMV i4x4Pattern;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);

                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec,pWMVDec->m_pbitstrmIn) + 1;
                rgCBP[4] = i4x4Pattern;

                if (i4x4Pattern & 8)
                {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (i4x4Pattern & 4)
                {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
                if (i4x4Pattern & 2)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
                }
                if (i4x4Pattern & 1)
                {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
                }
            }

            if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                I32_WMV j1;
                for( j1= 0; j1 <64; j1++)
                    ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
            }

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
            MotionCompChromaBlockFieldMode (pWMVDec, ppxlcCurrQUMB, ppxliErrorQMB, ppxlcRefU, iMVx, iMVy);
            bFirstBlock = FALSE;
        }
        else {
            MotionCompChromaBlockFieldMode (pWMVDec, ppxlcCurrQUMB, NULL, ppxlcRefU, iMVx, iMVy);
            pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
        }
    }


    // V-block
    if (iIntra) {
        Bool_WMV bDCACPredOn;
        I16_WMV *pPred = NULL;
        I32_WMV ii ;

        pIntra = pWMVDec->m_pX9dctV + (iMBX + iMBY * pWMVDec->m_uintNumMBX) * 16;

        bDCACPredOn = decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, iMBX, iMBY, 
            &iShift, &iDirection, pPredScaled);
        
        if (bDCACPredOn) pPred = pPredScaled;

        bSkipMB = FALSE;
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;

        result = DecodeInverseIntraBlockX9 (pWMVDec, 
                    ppInterDCTTableInfo_Dec, 5,
                    rgCBP[5], pPred,
                   iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE);

        for ( ii = 0; ii < 8; ii++)
            memset (ppxlcCurrQVMB + ii * pWMVDec->m_iWidthPrevUV, 128, 8);
        pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
        pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
        if (pWMVDec->m_iOverlap & 1) {
            // overlapped transform copy
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
            I32_WMV i1 ;

            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraMBRowV0[0] + iMBX * 8 + i1 * iWidth1,
                ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
    }
    else {

        if (iOppFieldCount > iSameFieldCount) 
            ppxlcRefV = ppxlcOppRefV + (iMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iMVx >> 2);
        else
            ppxlcRefV = ppxlcSameRefV + (iMVy >> 2) * pWMVDec->m_iWidthPrevUV + (iMVx >> 2);

        if (rgCBP [5]) {
            bSkipMB = FALSE;
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec,pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }

            pmbmd->m_rgcBlockXformMode[5] = (I8_WMV)iXformType;

            if (iXformType == XFORMMODE_8x8) {
                ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
                result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ); //get the quantized block  
                if (ICERR_OK != result) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    //BOE;
                    return result;
                }

                (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
            }

            else if (iXformType == XFORMMODE_8x4) {

                bTop = TRUE;
                bBottom = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[5] = iSubblockPattern;
                        bTop = (rgCBP[5] & 2);
                        bBottom = (rgCBP[5] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[5] = (U8_WMV )((bTop << 1) | bBottom);            
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[5] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP[5] & 2);
                        bBottom = (rgCBP[5] & 1);
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[5] = (U8_WMV )((bTop << 1) | bBottom);            
                    }
                }

                if (bTop) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);

                }
                if (bBottom) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ); //get the quantized block    
                    (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }

            else if (iXformType == XFORMMODE_4x8) {

                bLeft = TRUE;
                bRight = TRUE;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[5] = iSubblockPattern;
                        bLeft = rgCBP[5] & 2;
                        bRight = rgCBP[5] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[5] = (U8_WMV )((bLeft << 1) | bRight);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[5] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP[5] & 2;
                        bRight = rgCBP[5] & 1;
                    }
                    else {
                        if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[5] = (U8_WMV )((bLeft << 1) | bRight);
                    }
                }

                if (bLeft) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (bRight) {
                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
            }

            else if (iXformType == XFORMMODE_4x4)
            {
                I32_WMV i4x4Pattern;

                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);

                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec,pWMVDec->m_pbitstrmIn) + 1;
                rgCBP[5] = i4x4Pattern;

                if (i4x4Pattern & 8)
                {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
                }
                if (i4x4Pattern & 4)
                {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
                }
                if (i4x4Pattern & 2)
                {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 2);
                }
                if (i4x4Pattern & 1)
                {

                    result = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, XFORMMODE_4x4, pDQ); //get the quantized block      
                    (*pWMVDec->m_pInter4x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 3);
                }
            }

            if(pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                I32_WMV j1;
                for( j1= 0; j1 <64; j1++)
                    ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
            }

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
            MotionCompChromaBlockFieldMode (pWMVDec, ppxlcCurrQVMB, ppxliErrorQMB, ppxlcRefV, iMVx, iMVy);
            bFirstBlock =  FALSE;
        }
        else {
            MotionCompChromaBlockFieldMode (pWMVDec, ppxlcCurrQVMB, NULL, ppxlcRefV, iMVx, iMVy);
            pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;
        }
    }
    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = bSkipMB;

#endif //PPCWMP

    return ICERR_OK;
}

#endif

Void_WMV ScaleTopPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, I16_WMV *pPred,
                                I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ScaleTopPredForDQuant);
    if (iblk == 0 || iblk == 1 || iblk == 4 || iblk == 5) {
        I32_WMV iTmp;
        I32_WMV i ;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;
        pPredScaled [0] = (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize);
        for ( i = 1; i < 16; i++) {
            iTmp = pPred [i] * (pmbmd - pWMVDec->m_uintNumMBX)->m_iQP;
            pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
        }
        pPredScaled[8] = pPredScaled[0];
    } else {
        I32_WMV i ;
        for ( i = 0; i < 16; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}


Void_WMV ScaleLeftPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, I16_WMV *pPred,
    I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ScaleLeftPredForDQuant);
    if (iblk == 0 || iblk == 2 || iblk == 4 || iblk == 5) {
        I32_WMV iTmp;
        I32_WMV i ;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
        pPredScaled [0] = (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize);
        for ( i = 1; i < 16; i++) {
            iTmp = pPred [i] * (pmbmd - 1)->m_iQP;
            pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
        }
        pPredScaled[8] = pPredScaled[0];
    } else {
        I32_WMV i;
        for ( i = 0; i < 16; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}


Void_WMV ScaleDCPredForDQuant (tWMVDecInternalMember *pWMVDec, I32_WMV iblk, CWMVMBMode* pmbmd, 
    I32_WMV *piTopDC, I32_WMV *piLeftDC, I32_WMV *piTopLeftDC)
{
    I32_WMV iTopDC = *piTopDC;
    I32_WMV iLeftDC = *piLeftDC;
    I32_WMV iTopLeftDC = *piTopLeftDC;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ScaleDCPredForDQuant);
    
    if (iblk == 0 || iblk == 4 || iblk == 5) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX - 1)->m_iQP ].iDCStepSize;
        I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;
        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize;
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iTopDC * iTopDCStepSize;
        iTopDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    } else if (iblk == 1) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;
        I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP ].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize;
        
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iTopDC * iTopDCStepSize;
        iTopDC = divroundnearest (iTmp, iCurrDCStepSize);
    } else if (iblk == 2) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP ].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP ].iDCStepSize;
        
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    } 
    
    *piTopDC = iTopDC ;
    *piLeftDC =   iLeftDC ;
    *piTopLeftDC =  iTopLeftDC ;
}

/****************************************************************************************************
  decodeDCTPredictionY,UV : find DCT-DCAC predictions for Y and U,V IMBs
****************************************************************************************************/
Bool_WMV decodeDCTPredictionY (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode *pmbmd, I16_WMV *pIntra, I32_WMV iblk,
    I32_WMV iX, I32_WMV iY, I32_WMV *piShift, I32_WMV *iDirection, I16_WMV *pPredScaled)
{
    Bool_WMV bACPredOn = TRUE_WMV;
    I32_WMV iStride = pWMVDec->m_uintNumMBX << 1;
    Bool_WMV   iShift = *piShift;
    I32_WMV  iWidth = iStride;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    I32_WMV iIndex = iX + (iY * pWMVDec->m_uintNumMBX << 1);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDCTPredictionY);

	iShift = 0;
#ifdef _WMV9AP_
    if (pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) {
#else
    if (pWMVDec->m_tFrmType == BVOP) {
#endif
        // if B frame, use MB packed motion vectors
        iIndex = (iX >> 1) + (iY >> 1) * (I32_WMV) pWMVDec->m_uintNumMBX;
        iWidth = (I32_WMV) pWMVDec->m_uintNumMBX;
    }
    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
    
    if (((pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) && (iY & 1)) ||
        (iY && ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) || pWMVDec->m_pXMotion[iIndex - iWidth] == IBLOCKMV))) {    

        if (((iY & 1) == 1) || !pWMVDec->m_pbStartOfSliceRow[iY>>1]) {
            pPred = pPredIntraTop = pIntra - 16 * iStride;// + pWMVDec->m_iACRowPredOffset;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
#ifdef _WMV9AP_
   if (((pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) && (iX & 1)) ||
        (iX && ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) ||pWMVDec->m_pXMotion[iIndex - 1] == IBLOCKMV))) 
#else
    if ((pWMVDec->m_tFrmType == BVOP && (iX & 1)) ||
        (iX && (pWMVDec->m_pXMotion[iIndex - 1] == IBLOCKMV))) 
#endif
    {
        pPred = pPredIntraLeft = pIntra - 16;//pWMVDec->m_iACColPredOffset;
        iShift = pWMVDec->m_iACColPredShift;
        
    }
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
#ifdef _WMV9AP_
        if (((pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) && ((iX | iY) & 1)) ||
            ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) || pWMVDec->m_pXMotion[iIndex - iWidth - 1] == IBLOCKMV)) {
#else
        if ( (pWMVDec->m_tFrmType == BVOP && ((iX | iY) & 1)) ||
            (pWMVDec->m_pXMotion[(iIndex - iWidth - 1)] == IBLOCKMV)  ) {   
#endif
                           
            iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        }
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];                                   
        ScaleDCPredForDQuant (pWMVDec, iblk, pmbmd, &iTopDC, &iLeftDC, &iTopLeftDC);
        
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (pPred != NULL && !(pmbmd->m_rgbDCTCoefPredPattern2[0]))
        bACPredOn = FALSE_WMV;
    
    if (pPred != NULL_WMV) {
        bDCACPred = TRUE_WMV;
        if (iShift == pWMVDec->m_iACColPredShift) {
            ScaleLeftPredForDQuant (pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        } else {
            ScaleTopPredForDQuant (pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    *piShift =  iShift ;
    return bDCACPred;
}

Bool_WMV decodeDCTPredictionY_IFrameNoDQuantAndNoSlice (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode *pmbmd, I16_WMV *pIntra, I32_WMV iblk,
    I32_WMV iX, I32_WMV iY, I32_WMV *piShift, I32_WMV *iDirection, I16_WMV **pPredScaled)
{
    Bool_WMV bACPredOn = TRUE_WMV;
    I32_WMV iStride = pWMVDec->m_uintNumMBX << 1;
    Bool_WMV iShift;
    //I32_WMV  iWidth = iStride;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDCTPredictionY);

	iShift = 0;
    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
    

    if (iY) {
        pPred = pPredIntraTop = pIntra - 16 * iStride;// + pWMVDec->m_iACRowPredOffset;
        iShift = pWMVDec->m_iACRowPredShift;
    }

	if (iX)
    {
        pPred = pPredIntraLeft = pIntra - 16;//pWMVDec->m_iACColPredOffset;
        iShift = pWMVDec->m_iACColPredShift;
        
    }
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
                           
        iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];                                          
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (pPred != NULL && !(pmbmd->m_rgbDCTCoefPredPattern2[0]))
        bACPredOn = FALSE_WMV;
    
    if (pPred != NULL_WMV) {
        bDCACPred = TRUE_WMV;
    }
    *pPredScaled = pPred;
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
	*piShift = iShift;
    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    return bDCACPred;
}

Bool_WMV decodeDCTPredictionUV(tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I16_WMV*     pIntra, 
    I32_WMV imbX,     I32_WMV imbY, 
    I32_WMV     *piShift,
    I32_WMV *iDirection, I16_WMV *pPredScaled)

{
    Bool_WMV bACPredOn = TRUE_WMV;
    Bool_WMV     iShift=  *piShift;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDCTPredictionUV);
    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
    iShift = 0;

   if (imbY && !pWMVDec->m_pbStartOfSliceRow[imbY]) {
#ifdef _WMV9AP_
        if (pWMVDec->m_pXMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] == IBLOCKMV || ( pWMVDec->m_tFrmType  == IVOP || pWMVDec->m_tFrmType  == BIVOP)) {
#else
        if (pWMVDec->m_pXMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] == IBLOCKMV) {
#endif
            pPred = pPredIntraTop = pIntra - 16 * pWMVDec->m_uintNumMBX;// + pWMVDec->m_iACRowPredOffset;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    if (imbX) {
#ifdef _WMV9AP_
        if (pWMVDec->m_pXMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1] == IBLOCKMV || (pWMVDec->m_tFrmType  == IVOP || pWMVDec->m_tFrmType  == BIVOP)) {
#else
        if (pWMVDec->m_pXMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1] == IBLOCKMV) {
#endif
            pPred = pPredIntraLeft = pIntra - 16;// - pWMVDec->m_iACColPredOffset;
            iShift = pWMVDec->m_iACColPredShift;
        }
    }
    
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV    iTopLeftDC = 0;
        I32_WMV iTopDC;
        I32_WMV iLeftDC;
        
#ifdef _WMV9AP_
        if (pWMVDec->m_pXMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1] == IBLOCKMV || (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType  == BIVOP))
#else
        if (pWMVDec->m_pXMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1] == IBLOCKMV)
#endif
            iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];                                        
        ScaleDCPredForDQuant (pWMVDec, 4, pmbmd, &iTopDC, &iLeftDC, &iTopLeftDC);
        
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (  pPred != NULL_WMV && ! (pmbmd->m_rgbDCTCoefPredPattern2[0] )) {
        bACPredOn = FALSE_WMV;
        //        iShift = -1; // no AC Pred
    }
    
    if (pPred != NULL_WMV) 
        bDCACPred = TRUE_WMV;
    
    if (bDCACPred) {
        if (iShift == pWMVDec->m_iACColPredShift) {
            ScaleLeftPredForDQuant (pWMVDec, 4, pPred, pPredScaled, pmbmd);
        } else {
            ScaleTopPredForDQuant (pWMVDec, 4, pPred, pPredScaled, pmbmd);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    *piShift =  iShift;
    
    return bDCACPred;
}

Bool_WMV decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice(tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I16_WMV*     pIntra, 
    I32_WMV imbX,     I32_WMV imbY, 
    I32_WMV *piShift,
    I32_WMV *iDirection, I16_WMV **pPredScaled)

{
    Bool_WMV bACPredOn = TRUE_WMV;
    Bool_WMV iShift;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDCTPredictionUV);
    
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
    iShift = 0;

	if (imbY) {
        pPred = pPredIntraTop = pIntra - 16 * pWMVDec->m_uintNumMBX;// + pWMVDec->m_iACRowPredOffset;
        iShift = pWMVDec->m_iACRowPredShift;
    }
    if (imbX) {
        pPred = pPredIntraLeft = pIntra - 16;// - pWMVDec->m_iACColPredOffset;
        iShift = pWMVDec->m_iACColPredShift;
    }
    
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC;
        I32_WMV iLeftDC;
        
        iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
        iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];                                        
        
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
            pPred = pPredIntraTop;
            iShift = pWMVDec->m_iACRowPredShift;
        }
    }
    
    if (  pPred != NULL_WMV && ! (pmbmd->m_rgbDCTCoefPredPattern2[0] )) {
        bACPredOn = FALSE_WMV;
        //        iShift = -1; // no AC Pred
    }
    
    if (pPred != NULL_WMV) 
        bDCACPred = TRUE_WMV;
    
	*pPredScaled = pPred;
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;

    *iDirection = (iShift == pWMVDec->m_iACRowPredShift);
    *piShift =  iShift;
    
    return bDCACPred;
}

I32_WMV  IsIntraPredY (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY)
{
    I32_WMV iWidth = pWMVDec->m_uintNumMBX << 1;
    I32_WMV iIndex = iX + (iY * pWMVDec->m_uintNumMBX << 1);
    I32_WMV iPred = 0;
    Bool_WMV bTopBndry = FALSE;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(IsIntraPredY);
    
    if (!(iY & 1)) {
        bTopBndry = (iY == 0) || pWMVDec->m_pbStartOfSliceRow [iY >> 1];
    }

    if (!bTopBndry && (pWMVDec->m_pXMotion[(iIndex - iWidth)] == IBLOCKMV))             
        iPred = 1;

    if (iX && (pWMVDec->m_pXMotion[(iIndex - 1)] == IBLOCKMV))             
        iPred = 1;

    return iPred;
}

I32_WMV IsIntraPredUV (tWMVDecInternalMember *pWMVDec, I32_WMV iMBX, I32_WMV iMBY)
{
    I32_WMV iPred = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(IsIntraPredUV);

    if (iMBY && !pWMVDec->m_pbStartOfSliceRow[iMBY]) {

        if (pWMVDec->m_pXMotionC[(iMBY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX] == IBLOCKMV)
            iPred = 1;
    }
    if (iMBX) {

        if (pWMVDec->m_pXMotionC[iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX - 1] == IBLOCKMV)
            iPred = 1;
    }

    return iPred;
}

//#endif // X9


Void_WMV ComputeDQuantDecParam (tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iStepSize;
    I32_WMV iDCStepSize ;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeDQuantDecParam);
   
    for (iStepSize = 1; iStepSize < 63; iStepSize++) {
        DQuantDecParam *pDQ = &pWMVDec->m_rgDQuantParam3QPDeadzone [iStepSize];
        I32_WMV iDoubleStepSize = (iStepSize + 1);
        //I32_WMV iHalfStepSize = (iStepSize >> 1);

        pDQ->iDoubleStepSize = iDoubleStepSize;
        pDQ->iStepMinusStepIsEven = 0;
        pDQ->i2DoublePlusStepSize = iDoubleStepSize;
        pDQ->i2DoublePlusStepSizeNeg = -1 * pDQ->i2DoublePlusStepSize;

        iDCStepSize = (iStepSize + 1) >> 1;
        if (iDCStepSize <= 4) {
            pDQ->iDCStepSize = 8;
            if(pWMVDec->m_bNewDCQuant && iDCStepSize <= 2) {
                pDQ->iDCStepSize = 2 * iDCStepSize;
            }
        } else {  
            pDQ->iDCStepSize = (iDCStepSize >> 1) + 6;
        }
    }
    
    for (iStepSize = 1; iStepSize < 63; iStepSize++) {
        DQuantDecParam *pDQ = &pWMVDec->m_rgDQuantParam5QPDeadzone [iStepSize];
        I32_WMV iDoubleStepSize;
        I32_WMV iHalfStepSize;

        iDoubleStepSize = (iStepSize + 1);
        iHalfStepSize = (iStepSize >> 1);

        pDQ->iDoubleStepSize = iDoubleStepSize;

        if (pWMVDec->m_cvCodecVersion >= WMV3) {
            pDQ->iStepMinusStepIsEven = (iStepSize + 1) >> 1;
           
            pDQ->i2DoublePlusStepSize = iDoubleStepSize + pDQ->iStepMinusStepIsEven;
        } else {
            I32_WMV iStepSize2 = (iStepSize + 1) >> 1;
            pDQ->iStepMinusStepIsEven = iStepSize2 - ((iStepSize2 & 1) == 0);
            pDQ->i2DoublePlusStepSize = iDoubleStepSize + pDQ->iStepMinusStepIsEven;      
        }
            
        pDQ->i2DoublePlusStepSizeNeg = -1 * pDQ->i2DoublePlusStepSize;

        iDCStepSize = (iStepSize + 1) >> 1;
        if (iDCStepSize <= 4) {
            pDQ->iDCStepSize = 8;
            if(pWMVDec->m_bNewDCQuant && iDCStepSize <= 2) 
                pDQ->iDCStepSize = 2 * iDCStepSize;

        } else {  
            pDQ->iDCStepSize = (iDCStepSize >> 1) + 6;
        }
    }           
}


#ifndef WMV9_SIMPLE_ONLY

#ifdef _WMV9AP_
Void_WMV ComputeLoopFilterFlags_WMV9(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iMBX, iMBY;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeLoopFilterFlags);

    if (pWMVDec->m_bSliceWMVA) {
        for (iMBY = 0; iMBY < (I32_WMV) pWMVDec->m_uintNumMBY; iMBY++) 
        {
            for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++) 
            {
                U8_WMV* pbCBP = pmbmd->m_rgbCodedBlockPattern2;
                U8_WMV* pbCBPTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
                U8_WMV* pbCBPLeft = (pmbmd - 1)->m_rgbCodedBlockPattern2;
                I8_WMV* pBlockXformMode = pmbmd->m_rgcBlockXformMode;
                I8_WMV* pBlockXformModeTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
                I8_WMV* pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;

                ComputeMBLoopFilterFlags_WMVA (pWMVDec, 
                    iMBX, iMBY, pmbmd->m_chMBMode,
                    pBlockXformMode, pBlockXformModeTop, pBlockXformModeLeft,
                    pbCBP, pbCBPTop, pbCBPLeft, 
                    iMBY == 0 || pWMVDec->m_pbStartOfSliceRow[iMBY], iMBX == 0);

                pmbmd ++;
            }
        }
    } else {
        for (iMBY = 0; iMBY < (I32_WMV) pWMVDec->m_uintNumMBY; iMBY++) 
        {
            for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++) 
            {
                U8_WMV *pCBP = pmbmd->m_rgbCodedBlockPattern2;
                U8_WMV * pCBPTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
                U8_WMV *pCBPLeft = (pmbmd - 1)->m_rgbCodedBlockPattern2;
                I8_WMV* pBlockXformMode = pmbmd->m_rgcBlockXformMode;
                I8_WMV* pBlockXformModeTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
                I8_WMV* pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;

                ComputeMBLoopFilterFlags_WMV9 (pWMVDec, 
                                    iMBX, iMBY, pmbmd->m_chMBMode,
                                    pBlockXformMode, pBlockXformModeTop, pBlockXformModeLeft,
                    pCBP, pCBPTop, pCBPLeft, 
                    iMBY == 0, iMBX == 0);

                pmbmd ++;  
            }
        }          
    } 
}

#else
Void_WMV ComputeLoopFilterFlags(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iMBX, iMBY;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeLoopFilterFlags);

    for (iMBY = 0; iMBY < (I32_WMV) pWMVDec->m_uintNumMBY; iMBY++) 
    {
        for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++) 
        {
            U8_WMV *pCBP = pmbmd->m_rgbCodedBlockPattern2;
            U8_WMV * pCBPTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPLeft = (pmbmd - 1)->m_rgbCodedBlockPattern2;
            I8_WMV* pBlockXformMode = pmbmd->m_rgcBlockXformMode;
            I8_WMV* pBlockXformModeTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
            I8_WMV* pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;

            ComputeMBLoopFilterFlags (pWMVDec, 
                                iMBX, iMBY, pmbmd->m_chMBMode,
                                pBlockXformMode, pBlockXformModeTop, pBlockXformModeLeft,
                pCBP, pCBPTop, pCBPLeft, 
                iMBY == 0, iMBX == 0);

            pmbmd ++;  
        }
    }           
}
#endif
#endif // WMV9_SIMPLE_ONLY

Void_WMV LoopFilterMultiThreads_V9 ()
{

}



#ifdef _WMV9AP_
#ifndef WMV9_SIMPLE_ONLY
Void_WMV ComputeLoopFilterFlags_WMVA(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iMBX, iMBY;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;

    for (iMBY = 0; iMBY < (I32_WMV) pWMVDec->m_uintNumMBY; iMBY++) 
    {
        for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++) 
        {
            U8_WMV* pCBP = pmbmd->m_rgbCodedBlockPattern2;
            U8_WMV* pCBPTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
            U8_WMV* pCBPLeft = (pmbmd - 1)->m_rgbCodedBlockPattern2;
            I8_WMV* pBlockXformMode = pmbmd->m_rgcBlockXformMode;
            I8_WMV* pBlockXformModeTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
            I8_WMV* pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;

        if(pWMVDec->m_bCodecIsWVC1){
            ComputeMBLoopFilterFlags_WMVA (pWMVDec, 
                                iMBX, iMBY, pmbmd->m_chMBMode,
                                pBlockXformMode, pBlockXformModeTop, pBlockXformModeLeft,
                pCBP, pCBPTop, pCBPLeft, 
                iMBY == 0 || pWMVDec->m_pbStartOfSliceRow[iMBY], iMBX == 0); //zou 330
				//iMBY == 0 || pWMVDec->m_pbStartOfSliceRow[iMBY+1]==1, iMBX == 0); //zou 330
        }
        else {
            ComputeMBLoopFilterFlags_WMVA (pWMVDec, 
                                iMBX, iMBY, pmbmd->m_chMBMode,
                                pBlockXformMode, pBlockXformModeTop, pBlockXformModeLeft,
                pCBP, pCBPTop, pCBPLeft, 
                iMBY == 0, iMBX == 0);
        }
        pmbmd ++;  
    }




    }           
}
#endif
#endif

