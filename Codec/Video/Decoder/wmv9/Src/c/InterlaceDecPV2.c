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
//
// Inter Codec 
//
static I32_WMV s_iMBXformTypeLUT[16] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};
static I32_WMV s_iSubblkPatternLUT[16] = {0, 1, 2, 3, 1, 2, 3, 0, 0, 1, 2, 3, 1, 2, 3, 0};

static I32_WMV s_iBlkXformTypeLUT[8] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};

static I32_WMV s_pXformLUT[4] = {XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_4x8, XFORMMODE_4x4};

enum {FRAMEMV = 0, FRAMEMV_FIELD, FRAME4MV, FRAME4MV_FIELD, 
      FIELDMV, FIELDMV_FRAME, FIELD4MV, FIELD4MV_FRAME,
      INTRA_FRAME, INTRA_FIELD};

// Externs
extern I32_WMV g_iStepRemap[31];
//extern  I32_WMV g_iStepsize;
extern  I32_WMV g_iMultiResOn;
extern  I32_WMV g_iRangeRedux;
//extern  I32_WMV g_iFading;

#ifndef ICERR_UNSUPPORTED
#define ICERR_UNSUPPORTED      -1L
#endif

tWMVDecodeStatus decodePInterlaceV2 (tWMVDecInternalMember * pWMVDec)
{
    I32_WMV result = ICERR_OK;
    //U32_WMV m_uiCurrSlice = 0;
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefY = pWMVDec->m_ppxliRef0YPlusExp;
    U8_WMV *ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    I16_WMV ppxliErrorY [16*16], ppxliErrorU [64], ppxliErrorV [64];

    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    U32_WMV imbY, imbX;

    Bool_WMV bFrameInPostBuf = FALSE;

    if(pWMVDec->m_bCodecIsWVC1){
	pWMVDec->vo_ppxliRefYPlusExp = pWMVDec->m_ppxliRef0YPlusExp;
	pWMVDec->vo_ppxliRefU = pWMVDec->m_ppxliRef0U +pWMVDec->m_iWidthPrevUVXExpPlusExp;
	pWMVDec->vo_ppxliRefV = pWMVDec->m_ppxliRef0V +pWMVDec->m_iWidthPrevUVXExpPlusExp;
    }
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
       
    pWMVDec->m_iFilterType = FILTER_BICUBIC;
    
    HUFFMANGET_DBG_HEADER(":decodeP411",12);
    //DECODE_FRAME_TYPE_COUNT_CPU_CYCLES(12);

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
    {
        dxvaStartFrame(pWMVDec->m_bLoopFilter, 0, FALSE);
    }
#endif

    // Set transform type huffman pointers
    SetTransformTypeHuffmanTable (pWMVDec, pWMVDec->m_iStepSize);
   
    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {                

        U8_WMV *ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV *ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV *ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV *ppxliRefYMB = ppxliRefY;
        U8_WMV *ppxliRefUMB = ppxliRefU;
        U8_WMV *ppxliRefVMB = ppxliRefV;
        U32_WMV iBlk;

        if(pWMVDec->m_bCodecIsWVC1){
			pWMVDec->m_nrows = imbY;
        }

       if (pWMVDec->m_bSliceWMVA) {
                if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush(pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
					if (result != ICERR_OK)
                        return WMV_Failed;
                }
        }

        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {  
            Bool_WMV bCBPPresent, b1MVPresent;
            I32_WMV iTopLeftBlkIndex;
            I32_WMV iBotLeftBlkIndex;

            if(pWMVDec->m_bCodecIsWVC1){
		pWMVDec->m_ncols = imbX;
            }

            pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT            
            pmbmd->m_bBlkXformSwitchOn = FALSE;            
            pmbmd->m_rgbDCTCoefPredPattern2 [0] = 0;
            pmbmd->m_dctMd = INTER;
            pmbmd->m_chFieldDctMode = FALSE; 
#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
            {
                dxvaNextMB();
                dxvaSetMotionForward();
            }
#endif

            memset (pWMVDec->m_ppxliFieldMB, 0, 6 * 8 * 8 * sizeof(I16_WMV));

            if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
                pmbmd->m_bSkip = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            }
            
            DecodePMBModeInterlaceV2 (pWMVDec,pmbmd, &bCBPPresent, &b1MVPresent);

               if(pWMVDec->m_bCodecIsWVC1){
			if(pmbmd->m_chMBMode == MB_INTRA)
			{  
				if(pWMVDec->m_iStepSize >= 9)  //zou 321
				{
					pWMVDec->m_iOverlap  = pWMVDec->m_bSequenceOverlap;
					pmbmd->m_bOverlapIMB = 1;
				}
				else
				{
					pWMVDec->m_iOverlap = 0;
					pmbmd->m_bOverlapIMB = 0;
				}
			}

			for (iBlk = 0; iBlk < 6; iBlk ++) {
			    pmbmd -> m_rgcIntraFlag[iBlk] = INTER;//zou 321
                        }
               }

            if (pmbmd->m_chMBMode == MB_INTRA) {
                I32_WMV iCBPCY;

                pmbmd->m_dctMd = INTRA;   
                pmbmd->m_chFieldDctMode = (I8_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                bCBPPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                if (bCBPPresent) {
                    iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufICBPCYDec, pWMVDec->m_pbitstrmIn) + 1;  
                } else {
                    iCBPCY = 0;
                }
                for (iBlk = 0; iBlk < 6; iBlk ++) { // set CBP
                    pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    pmbmd->m_rgbCodedBlockPattern2 [5 - iBlk] = (iCBPCY & 0x1);
                    iCBPCY >>= 1;
                }
               if(pWMVDec->m_bCodecIsWVC1){
                   pmbmd -> m_rgcIntraFlag[0] = pmbmd -> m_rgcIntraFlag[1] = 
                   pmbmd -> m_rgcIntraFlag[2] = pmbmd -> m_rgcIntraFlag[3] = 
                   pmbmd -> m_rgcIntraFlag[4] = pmbmd -> m_rgcIntraFlag[5] = INTRA;
               }

                // get AC Pred Flag
                pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);

                if (pWMVDec->m_bDQuantOn) {
                    if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                        return WMV_Failed;
                    }   
                }   

                result = DecodeIMBInterlaceV2 (pWMVDec, 
                    imbY, imbX, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                    ppxliErrorY, ppxliErrorU, ppxliErrorV);
                if (ICERR_OK != result) {
                    return WMV_Failed;
                }
            } else {
                result = decodePMBOverheadInterlaceV2 (pWMVDec, 
                    pmbmd, imbX, imbY, bCBPPresent, b1MVPresent);
                if (ICERR_OK != result) {
                    return WMV_Failed;
                }

                result = DecodePMBInterlaceV2 (pWMVDec, imbX, imbY, pmbmd); 
                if (ICERR_OK != result) {
                    return WMV_Failed;
                }

                if (pmbmd->m_chMBMode == MB_1MV) {
                    ComputeFrameMVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV); 
                    MotionCompFrameInterlaceV2 (pWMVDec, 
                        imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else if (pmbmd->m_chMBMode == MB_4MV) {                       
                    ComputeFrame4MVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompFrame4MvInterlaceV2 (pWMVDec, 
                        imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else if (pmbmd->m_chMBMode == MB_FIELD) {
                    ComputeFieldMVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompFieldInterlaceV2 (pWMVDec,
                        imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else { //if (pmbmd->m_chMBMode == MB_FIELD4MV) {
                    ComputeField4MVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompField4MvInterlaceV2 (pWMVDec,
                        imbX, imbY, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                }

                if (pmbmd->m_chFieldDctMode == FALSE) {
                    AddErrorFrameInterlaceV2 (pWMVDec,
                        pWMVDec->m_ppxliFieldMB, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                } else {
                    AddErrorFieldInterlaceV2 (pWMVDec,
                        pWMVDec->m_ppxliFieldMB, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                        pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                }
            }
                        
            iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
            iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
            if (pWMVDec->m_iNumBFrames > 0)
            {
                pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pXMotion [iTopLeftBlkIndex];
                pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = pWMVDec->m_pXMotion [iTopLeftBlkIndex + 1 ];
                pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = pWMVDec->m_pXMotion [iBotLeftBlkIndex];
                pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = pWMVDec->m_pXMotion [iBotLeftBlkIndex + 1 ];
                pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pYMotion [iTopLeftBlkIndex];
                pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = pWMVDec->m_pYMotion [iTopLeftBlkIndex + 1 ];
                pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = pWMVDec->m_pYMotion [iBotLeftBlkIndex];
                pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = pWMVDec->m_pYMotion [iBotLeftBlkIndex + 1 ];
                pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.x = pWMVDec->m_pXMotion [iTopLeftBlkIndex];
                pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.y = pWMVDec->m_pYMotion [iTopLeftBlkIndex];
                pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.x = pWMVDec->m_pXMotion [iTopLeftBlkIndex];
                pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.y = pWMVDec->m_pYMotion [iTopLeftBlkIndex];

                if (pmbmd->m_chMBMode == MB_INTRA)
			    {
				    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = 
				    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = 
				    pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = 
				    pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
                    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.x = 0;                    
                    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.y = 0;                
                    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.x = 0;                    
                    pWMVDec->m_rgmv1[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].m_vctTrueHalfPel.y = 0;

                }
            }
            
            
            pmbmd->m_bSkip = FALSE;
            pmbmd++;
            ppxliCurrQYMB += 16;
            ppxliCurrQUMB += 8;
            ppxliCurrQVMB += 8;
            ppxliRefYMB += 16;
            ppxliRefUMB += 8;
            ppxliRefVMB += 8;
        }

        // point to the starting location of the first MB of each row
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRefU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRefV += pWMVDec->m_iBlkSizeXWidthPrevUV;
    }

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
    {
        dxvaEndFrame(pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY, FALSE);
    }
#endif

#if 0
	if(0)
	{
		unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp; 
		unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

		if(pWMVDec->m_nPframes ==13&& pWMVDec->m_tFrmType == PVOP)
		{
			int mb_cols =0, mb_rows=0;
			for(mb_rows=0;mb_rows<pWMVDec->m_uintNumMBY;mb_rows++)
			{
				unsigned char* Y = Yplane + mb_rows* pWMVDec->m_iMBSizeXWidthPrevY ;
				unsigned char *U = Uplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;
				unsigned char *V = Vplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;

				for(mb_cols=0;mb_cols<pWMVDec->m_uintNumMBX;mb_cols++)
				{
					FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump.txt","a");
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
    if (pWMVDec->m_bLoopFilter
#ifdef _GENERATE_DXVA_DATA_
        && !pWMVDec->m_pDMO
#endif        
        ) {

        LoopFilterRowPFrameInterlaceV2 (pWMVDec,
            0, pWMVDec->m_uintNumMBY,
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_rgmbmd);

        LoopFilterColPFrameInterlaceV2 (pWMVDec,
            0, pWMVDec->m_uintNumMBY,
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_rgmbmd);

    }

    // if the destination bitmap pointer is NULL, don't do color conversion
  

    pWMVDec->m_bDisplay_AllMB = TRUE;

    // Deinterlace
//    if (pWMVDec->m_bHostDeinterlace){
////#ifdef _MultiThread_Decode_
////        DecodeMultiThreads(DEINTERLACE);
////#else    
//        (*pWMVDec->m_pDeinterlaceProc)(pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliPostQY, 0, pWMVDec->m_iHeightPrevY, pWMVDec->m_iWidthPrevY, pWMVDec->m_iHeightPrevY);
////#endif
//        (*pWMVDec->m_pDeinterlaceProc)(pWMVDec->m_ppxliCurrQU, pWMVDec->m_ppxliPostQU, 0, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iHeightPrevUV);
//        (*pWMVDec->m_pDeinterlaceProc)(pWMVDec->m_ppxliCurrQV, pWMVDec->m_ppxliPostQV, 0, pWMVDec->m_iHeightPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iHeightPrevUV);
//
//        bFrameInPostBuf = TRUE;
//    }

    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;

    
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame

    return WMV_Succeeded;
}

I32_WMV DecodePMBModeInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    CWMVMBMode* pmbmd, Bool_WMV *pbCBPPresent, Bool_WMV *pb1MVPresent)
{
    I32_WMV result = ICERR_OK;
    I32_WMV iIndex;
    
    pmbmd->m_bOverlapIMB = FALSE;

    if (pmbmd->m_bSkip == TRUE) {
        pmbmd->m_chMBMode = MB_1MV;
        pmbmd->m_chFieldDctMode = FALSE;
        *pb1MVPresent = FALSE;
        *pbCBPPresent = FALSE;
        return result;
    }

    iIndex = Huffman_WMV_get(pWMVDec->m_pInterlaceFrameMBMode, pWMVDec->m_pbitstrmIn);

    if (pWMVDec->m_iX9MVMode == ALL_1MV) {
        if (iIndex == 8) { // map to INTRA
            iIndex = 14;
        }
    }

    if (iIndex == 0) {
        pmbmd->m_chMBMode = MB_1MV;     
        *pb1MVPresent = TRUE;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = FALSE;           
    } else if (iIndex == 1) {
        pmbmd->m_chMBMode = MB_1MV;
        *pb1MVPresent = TRUE;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = TRUE;
    } else if (iIndex == 2) {
        pmbmd->m_chMBMode = MB_1MV;
        *pb1MVPresent = TRUE;            
        *pbCBPPresent = FALSE;
        pmbmd->m_chFieldDctMode = FALSE; // default for loopfiltering                       
    } else if (iIndex == 3) {
        pmbmd->m_chMBMode = MB_1MV;
        *pb1MVPresent = FALSE; 
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = FALSE;
    } else if (iIndex == 4) {
        pmbmd->m_chMBMode = MB_1MV;
        *pb1MVPresent = FALSE; 
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = TRUE;
    } else if (iIndex == 5) {
        pmbmd->m_chMBMode = MB_FIELD;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = FALSE;
    } else if (iIndex == 6) {
        pmbmd->m_chMBMode = MB_FIELD;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = TRUE;
    } else if (iIndex == 7) {
        pmbmd->m_chMBMode = MB_FIELD;
        *pbCBPPresent = FALSE;
        pmbmd->m_chFieldDctMode = TRUE; // default for loopfiltering
    } else if (iIndex == 8) {
        pmbmd->m_chMBMode = MB_4MV;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = FALSE;
    } else if (iIndex == 9) {
        pmbmd->m_chMBMode = MB_4MV;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = TRUE;
    } else if (iIndex == 10) {
        pmbmd->m_chMBMode = MB_4MV;
        *pbCBPPresent = FALSE;
        pmbmd->m_chFieldDctMode = FALSE; // default for loopfiltering
    } else if (iIndex == 11) {
        pmbmd->m_chMBMode = MB_FIELD4MV;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = FALSE;
    } else if (iIndex == 12) {
        pmbmd->m_chMBMode = MB_FIELD4MV;
        *pbCBPPresent = TRUE;
        pmbmd->m_chFieldDctMode = TRUE;
    } else if (iIndex == 13) {
        pmbmd->m_chMBMode = MB_FIELD4MV;
        *pbCBPPresent = FALSE;
        pmbmd->m_chFieldDctMode = TRUE; // default for loopfiltering
    } else if (iIndex == 14) {
        pmbmd->m_chMBMode = MB_INTRA;
                
        if (pWMVDec->m_iOverlap) {
            pmbmd->m_bOverlapIMB = TRUE;
        }

    } else {
        assert (0);
        return ICERR_ERROR;
    }

    return result;
}

I32_WMV decodePMBOverheadInterlaceV2 (tWMVDecInternalMember * pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, Bool_WMV bCBPPresent, Bool_WMV b1MVPresent)
{
    I32_WMV iCBPCY = 0;
    U32_WMV iBlk;
    I32_WMV iTmpCBPCY;

    memset (pWMVDec->m_pDiffMV, 0, sizeof (CDiffMV) * 4);
    memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));        
    
    if (pmbmd->m_bSkip) {
        return ICERR_OK;
    }

    if (bCBPPresent) {
        iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufPCBPCYDec, pWMVDec->m_pbitstrmIn) + 1;
    } else {
        iCBPCY = 0;
    }

    // set CBP
    iTmpCBPCY = iCBPCY;
    for (iBlk = 0; iBlk < 6; iBlk ++) {
        pmbmd->m_rgbCodedBlockPattern2 [5-iBlk] = (iTmpCBPCY & 0x1);
        iTmpCBPCY >>= 1;
    }

    if (pmbmd->m_chMBMode == MB_1MV) {
        if (b1MVPresent) {
            decodeMV_Progressive(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
        }
    } else if (pmbmd->m_chMBMode == MB_FIELD) {
        I32_WMV iMV = Huffman_WMV_get(pWMVDec->m_p2MVBP, pWMVDec->m_pbitstrmIn);
        if (iMV & 2) {
            decodeMV_Progressive(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
        }
        if (iMV & 1) {
            decodeMV_Progressive(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + 2);
        }
    } else {
        I32_WMV i, iMV;
        iMV = Huffman_WMV_get(pWMVDec->m_p4MVBP, pWMVDec->m_pbitstrmIn);        
        for (i = 0; i < 4; i++) { 
            if (iMV & (1 << (3 - i))) {
                decodeMV_Progressive(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + i);
            }
        }        
    }

    if (pWMVDec->m_bDQuantOn && iCBPCY) {
        if (ICERR_OK != decodeDQuantParam (pWMVDec, pmbmd)) {
            return ICERR_ERROR;
        }
    }

    if (pWMVDec->m_bMBXformSwitching && iCBPCY) {
        I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufMBXformTypeDec, pWMVDec->m_pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        pmbmd->m_iMBXformMode = s_iMBXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        pmbmd->m_bFirstBlockDecoded = FALSE;
    }
    
    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    return ICERR_OK;
}

I32_WMV DecodeInverseInterBlockQuantizeInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, DQuantDecParam *pDQ, U32_WMV uiNumCoef)
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    //I32_WMV i2DoublePlusStepSize = pDQ->i2DoublePlusStepSize;
    //I32_WMV i2DoublePlusStepSizeNeg = pDQ->i2DoublePlusStepSizeNeg;
    CDCTTableInfo_Dec* InterDCTTableInfo_Dec = ppInterDCTTableInfo_Dec[0];
    Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec -> hufDCTACDec;
    I8_WMV* rgLevelAtIndx = InterDCTTableInfo_Dec -> pcLevelAtIndx;
    U8_WMV* rgRunAtIndx = InterDCTTableInfo_Dec -> puiRunAtIndx;
    U8_WMV* rgIfNotLastNumOfLevelAtRun = InterDCTTableInfo_Dec -> puiNotLastNumOfLevelAtRun;
    U8_WMV* rgIfLastNumOfLevelAtRun = InterDCTTableInfo_Dec -> puiLastNumOfLevelAtRun; 
    U8_WMV* rgIfNotLastNumOfRunAtLevel = InterDCTTableInfo_Dec -> puiNotLastNumOfRunAtLevel;
    U8_WMV* rgIfLastNumOfRunAtLevel = InterDCTTableInfo_Dec -> puiLastNumOfRunAtLevel;
    I32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
    I32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;


    Bool_WMV bIsLastRun = FALSE;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    //Align on cache line
    I16_WMV *rgiCoefRecon = (I16_WMV*)pWMVDec->m_rgiCoefRecon;
    I32_WMV iDCTHorzFlags = 0;

    do {
        I32_WMV lIndex = Huffman_WMV_get(hufDCTACDec , pWMVDec->m_pbitstrmIn);
        I32_WMV iIndex;

        if (lIndex != iTCOEF_ESCAPE)    {
            if (lIndex >= iStartIndxOfLastRun)
                bIsLastRun = TRUE;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
        } 
        else {
            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){
                // ESC + '1' + VLC
                I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec , pWMVDec->m_pbitstrmIn);
                //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return ICERR_ERROR;
                }
                uiRun = rgRunAtIndx[lIndex2];
                iLevel = rgLevelAtIndx[lIndex2];
                if (lIndex2 >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    iLevel += (I8_WMV) rgIfLastNumOfLevelAtRun[uiRun];
                }
                else
                    iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRun[uiRun];
                if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) 
                    iLevel = -iLevel;
            }
            else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){
                // ESC + '10' + VLC
                I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec , pWMVDec->m_pbitstrmIn);
                //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return ICERR_ERROR;
                }
                uiRun = rgRunAtIndx[lIndex2];
                iLevel = rgLevelAtIndx[lIndex2];
                if (lIndex2 >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) 
                    iLevel = -iLevel;
            }
            else{
                // ESC + '00' + FLC
                bIsLastRun = (Bool_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){  // == WMV1 || WMV2
                    if (pWMVDec->m_bFirstEscCodeInFrame){                                                    
                        decodeBitsOfESCCode (pWMVDec);                                                     
                        pWMVDec->m_bFirstEscCodeInFrame = FALSE;                                             
                    }                                                                               
                    uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_RUN);                                
                    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) /* escape decoding */                              
                        iLevel = -1 * BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_LEVEL);                 
                    else                                                                            
                        iLevel = BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_LEVEL);         
                }
                else{
                    uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);            
                    iLevel = (I8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_LEVEL);
                }
            }
        }
        uiCoefCounter += uiRun;
        if (uiCoefCounter >= uiNumCoef || BS_invalid(pWMVDec->m_pbitstrmIn)) {
            return ICERR_ERROR;
        }

        iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];

        if (iIndex & 0x7)
            iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

        if (iLevel > 0)
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = (I16_WMV)(iDoubleStepSize * iLevel + iStepMinusStepIsEven);
        else
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = (I16_WMV)(iDoubleStepSize * iLevel - iStepMinusStepIsEven);

        uiCoefCounter++;
    } while (!bIsLastRun);

    // Save the DCT row flags. This will be passed to the IDCT routine
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
    return ICERR_OK;
}

I32_WMV DecodePBlockInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk, CWMVMBMode *pmbmd,
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ,
    Bool_WMV *pbFirstBlock)
{
    Bool_WMV bFirstBlock = *pbFirstBlock;
    I32_WMV result;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    I32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    U8_WMV bResidual = rgCBP [iBlk];

    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    if (!bResidual) {
        pmbmd->m_rgcBlockXformMode[iBlk] = XFORMMODE_8x8;
    }

    if (bResidual) {       
        if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
            I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec , pWMVDec->m_pbitstrmIn);
            iXformType = s_iBlkXformTypeLUT[iIndex];
            iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        }

        pmbmd->m_rgcBlockXformMode[iBlk] = (I8_WMV)iXformType;

        if (iXformType == XFORMMODE_8x8) {
            I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;

            memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
            result = DecodeInverseInterBlockQuantizeInterlaceV2 (pWMVDec,
                ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, pDQ, 64);
#ifdef _EMB_SSIMD_IDCT_
			if ( pWMVDec->m_b16bitXform) SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);
#endif
            (*pWMVDec->m_pInterIDCT_Dec) ((UnionBuffer *) ppxliDst, pWMVDec->m_rgiCoefReconBuf, 8, 
                pWMVDec->m_iDCTHorzFlags);

        } else if (iXformType == XFORMMODE_8x4) {
            Bool_WMV bTop = TRUE;
            Bool_WMV bBottom = TRUE;  
            if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                rgCBP[iBlk] = iSubblockPattern;
                bTop = (rgCBP[iBlk] & 2);
                bBottom = (rgCBP[iBlk] & 1);
            }
            else {
                if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
                        bBottom = FALSE;
                    else
                        bTop = FALSE;
                }
                rgCBP[iBlk] = (U8_WMV)((bTop << 1) | bBottom);
            }             
            if (bTop) {

                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
                result = DecodeInverseInterBlockQuantizeInterlaceV2  (pWMVDec,
                    ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, pDQ, 32);
#ifdef _EMB_SSIMD_IDCT_
				if ( pWMVDec->m_b16bitXform) SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);
#endif                
				(*pWMVDec->m_pInter8x4IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4 * 8;
                
                memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
                result = DecodeInverseInterBlockQuantizeInterlaceV2  (pWMVDec,
                    ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, pDQ, 32);
#ifdef _EMB_SSIMD_IDCT_
				if ( pWMVDec->m_b16bitXform) SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);
#endif
                (*pWMVDec->m_pInter8x4IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, pWMVDec->m_rgiCoefReconBuf, 0);

            }            
        } else if (iXformType == XFORMMODE_4x8) {
            Bool_WMV bLeft = TRUE;
            Bool_WMV bRight = TRUE;    
            if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                rgCBP[iBlk] = iSubblockPattern;
                bLeft = rgCBP[iBlk] & 2;
                bRight = rgCBP[iBlk] & 1;
            }
            else {
                if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
                        bRight = FALSE;
                    else
                        bLeft = FALSE;
                }
                rgCBP[iBlk] = (U8_WMV)((bLeft << 1) | bRight);
            }

            if (bLeft) {

                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
                result = DecodeInverseInterBlockQuantizeInterlaceV2 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, pDQ, 32); 
#ifdef _EMB_SSIMD_IDCT_
				if ( pWMVDec->m_b16bitXform) SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);
#endif
                (*pWMVDec->m_pInter4x8IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, 
                    pWMVDec->m_rgiCoefReconBuf, 0);

            }
            if (bRight) {

                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4;
                memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
                result = DecodeInverseInterBlockQuantizeInterlaceV2 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, pDQ, 32); 
#ifdef _EMB_SSIMD_IDCT_
				if ( pWMVDec->m_b16bitXform) SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);
#endif
                (*pWMVDec->m_pInter4x8IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, 
                    pWMVDec->m_rgiCoefReconBuf, 0);

            } 
        } else if (iXformType == XFORMMODE_4x4) {
            I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
            I32_WMV i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec , pWMVDec->m_pbitstrmIn) + 1;
            
            I32_WMV i;

            rgCBP[iBlk] = i4x4Pattern;
            for (i = 0; i < 4; i++) {
                if (i4x4Pattern & (1 <<(3 - i))) {

                    memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
                    result = DecodeInverseInterBlockQuantizeInterlaceV2  (pWMVDec,
                        ppInterDCTTableInfo_Dec, pWMVDec->m_p4x4ZigzagScanOrder, pDQ, 16);
#ifdef _EMB_SSIMD_IDCT_
					if ( pWMVDec->m_b16bitXform) SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);
#endif
                    (*pWMVDec->m_pInter4x4IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, pWMVDec->m_rgiCoefReconBuf, i);
                }
            }
        }
        bFirstBlock = FALSE;
    }

    *pbFirstBlock = bFirstBlock;
    return ICERR_OK;
}

I32_WMV DecodePMBInterlaceV2 (tWMVDecInternalMember * pWMVDec, I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd)
{
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];
    //U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    U32_WMV iBlk;
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
        
    for (iBlk = 0; iBlk < 6; iBlk++) {
        I32_WMV result;
        result = DecodePBlockInterlaceV2 (pWMVDec,
            imbX, imbY, iBlk, pmbmd, pWMVDec->m_ppInterDCTTableInfo_Dec, pDQ,
            &bFirstBlock);
        if (ICERR_OK != result) {
            return result;
        }
    }

    return ICERR_OK;
}

Void_WMV ComputeFrameMVFromDiffMVInterlaceV2 (tWMVDecInternalMember * pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV dX, dY; 
    Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];

    PredictFrameOneMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, 
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);

    pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + iXBlocks]
        = pWMVDec->m_pXMotion [k + iXBlocks + 1]
        = (I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + iXBlocks]
        = pWMVDec->m_pYMotion [k + iXBlocks + 1]
        = (I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
        dxvaSetMV(pWMVDec->m_pXMotion [k], pWMVDec->m_pYMotion [k]);
#endif
}

Void_WMV ComputeFieldMVFromDiffMVInterlaceV2 (tWMVDecInternalMember * pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV dX, dY; 
    Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];

    PredictFieldTopMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);

    pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
        (I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
        (I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

    k += iXBlocks;

    PredictFieldBotMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, 
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);

    pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
        (I16_WMV)((((pInDiffMV + 2)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
        (I16_WMV)((((pInDiffMV + 2)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
}

Void_WMV ComputeFrame4MVFromDiffMVInterlaceV2 (tWMVDecInternalMember * pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV dX, dY; 
    Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];

    PredictFrameTopLeftMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k] = (I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k] = (I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

    PredictFrameTopRightMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k + 1] = (I16_WMV)((((pInDiffMV + 1)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k + 1] = (I16_WMV)((((pInDiffMV + 1)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

    k += iXBlocks;

    PredictFrameBotLeftMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k] = (I16_WMV)((((pInDiffMV + 2)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k] = (I16_WMV)((((pInDiffMV + 2)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

    PredictFrameBotRightMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k + 1] = (I16_WMV)((((pInDiffMV + 3)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k + 1] = (I16_WMV)((((pInDiffMV + 3)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);
}

Void_WMV ComputeField4MVFromDiffMVInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV dX, dY; 
    Bool_WMV bTopBndry = (imbY == 0) || pWMVDec->m_pbStartOfSliceRow [imbY];

    PredictFieldTopLeftMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k] = (I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k] = (I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

    PredictFieldTopRightMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k + 1] = (I16_WMV)((((pInDiffMV + 1)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k + 1] = (I16_WMV)((((pInDiffMV + 1)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

    k += iXBlocks;

    PredictFieldBotLeftMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k] = (I16_WMV)((((pInDiffMV + 2)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k] = (I16_WMV)((((pInDiffMV + 2)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

    PredictFieldBotRightMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY,
        &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);
    pWMVDec->m_pXMotion [k + 1] = (I16_WMV)((((pInDiffMV + 3)->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
    pWMVDec->m_pYMotion [k + 1] = (I16_WMV)((((pInDiffMV + 3)->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

}

/****************************************************************************************
  decodePFieldMode : Field Picture P frame decoding
****************************************************************************************/
I32_WMV decodePFieldMode (tWMVDecInternalMember * pWMVDec)
{
    I16_WMV MVx[4], MVy[4];
    I32_WMV result;

        U8_WMV __huge* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevY >> 1);
    U8_WMV __huge* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1);
    U8_WMV __huge* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1);
    U8_WMV __huge *ppxliSameRefY, *ppxliSameRefU, *ppxliSameRefV, *ppxliOppRefY, *ppxliOppRefU, *ppxliOppRefV;
    U8_WMV __huge *ppxliRefYMB, *ppxliRefUMB, *ppxliRefVMB;
    U8_WMV __huge* ppxliCurrQYMB, *ppxliCurrQUMB, *ppxliCurrQVMB;
    U8_WMV __huge* ppxliSameRefYMB, *ppxliSameRefUMB, *ppxliSameRefVMB;
    U8_WMV __huge* ppxliOppRefYMB, *ppxliOppRefUMB, *ppxliOppRefVMB;

    I32_WMV iOffset;
     CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
     Bool_WMV bFrameInPostBuf = FALSE;
        Bool_WMV bTopRowOfSlice = TRUE;


    I32_WMV x, y;
    U32_WMV imbX, imbY;

    U32_WMV uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;
    Bool_WMV bHalfPelMode = (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);


     //   if (pWMVDec->m_bSliceWMVA) {
        if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1)
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig + pWMVDec->m_uintNumMBY;        
        else {
            pWMVDec->m_pbStartOfSliceRow = pWMVDec->m_pbStartOfSliceRowOrig;
        }
  //  }




    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;
#ifdef _ANALYZEMC_
    if (pWMVDec->m_t == g_iFrame)
        g_bAddError = FALSE;
    else
        g_bAddError = TRUE;
#endif

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
        dxvaStartFrame(pWMVDec->m_bLoopFilter, 0, FALSE);
#endif
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];

    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];

    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;

#ifdef SIXTAP
    pWMVDec->m_iFilterType = FILTER_6TAP;
#endif // SIXTAP
//printf ("QP:%d\n", pWMVDec->m_iStepSize);

    HUFFMANGET_DBG_HEADER(":decodeP_X9",9);
    //DECODE_FRAME_TYPE_COUNT_CPU_CYCLES(9);

    // update overlap related variables
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);

    // Set transform type huffman pointers
    SetTransformTypeHuffmanTable (pWMVDec, pWMVDec->m_iStepSize);

        ppxliSameRefY = pWMVDec->m_ppxliRef0YPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
    ppxliSameRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * pWMVDec->m_iCurrentField;
    ppxliSameRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * pWMVDec->m_iCurrentField;;
    if (pWMVDec->m_iCurrentTemporalField == 0) {//top filed
        ppxliOppRefY = pWMVDec->m_ppxliRef0YPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
        if(pWMVDec->m_bCodecIsWVC1){
        ppxliOppRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField);// ^ 1);
        ppxliOppRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField );//^ 1);
        }
        else {
        ppxliOppRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
        ppxliOppRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
        }
    }
    else {
          ppxliOppRefY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + (pWMVDec->m_iWidthPrevY >> 1) * pWMVDec->m_iCurrentField;
            ppxliOppRefU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
            ppxliOppRefV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + (pWMVDec->m_iWidthPrevUV >> 1) * (pWMVDec->m_iCurrentField ^ 1);
    }
    if (pWMVDec->m_iCurrentField == 0)
        iOffset = 4;
    else
        iOffset = 2;
    
    if (bHalfPelMode)
        iOffset += (1 - 2 * pWMVDec->m_iCurrentField);

   
    for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {
                Bool_WMV bBottomRowOfSlice = (imbY == (pWMVDec->m_uintNumMBY - 1));
        FUNCTION_PROFILE_DECL_START(fpInit,DECODEPMAININIT_PROFILE);
        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;

        ppxliCurrQYMB = ppxliCurrQY + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliCurrQUMB = ppxliCurrQU + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliCurrQVMB = ppxliCurrQV + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliSameRefYMB = ppxliSameRefY + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliSameRefUMB = ppxliSameRefU + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliSameRefVMB = ppxliSameRefV + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliOppRefYMB = ppxliOppRefY + imbY * 16 * pWMVDec->m_iWidthPrevY;
        ppxliOppRefUMB = ppxliOppRefU + imbY * 8 * pWMVDec->m_iWidthPrevUV;
        ppxliOppRefVMB = ppxliOppRefV + imbY * 8 * pWMVDec->m_iWidthPrevUV;

        
        if (pWMVDec->m_bSliceWMVA) {

            if (pWMVDec->m_iCurrentTemporalField == 1 && imbY == 0) {
                pWMVDec->m_uiCurrSlice++;
            }
                if (pWMVDec->m_pbStartOfSliceRow[imbY]) {
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush(pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
                    if (result != ICERR_OK)
                        return result;
                }            
            if (pWMVDec->m_pbStartOfSliceRow[imbY])
                bTopRowOfSlice = TRUE;
        }

        FUNCTION_PROFILE_STOP(&fpInit);
        for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE) {
            Bool_WMV b1MV ;
            I32_WMV iOffsetB ;
            I32_WMV iTopLeftBlkIndex;
            I32_WMV iBotLeftBlkIndex;

            FUNCTION_PROFILE_DECL_START(fpPMBMode, DECODEMBMODE_PROFILE)
            uiNumMBFromSliceBdry ++;
            pWMVDec->m_bMBHybridMV = FALSE;

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaNextMB();
#endif
                       
            
            result = decodeMBOverheadOfPVOP_FieldPicture (pWMVDec, pmbmd, imbX, imbY, bHalfPelMode);

            pWMVDec->m_pXMotionC [imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] = 0;
            pWMVDec->m_pYMotionC [imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX] = 0;

            if (ICERR_OK != result) {
                FUNCTION_PROFILE_STOP(&fp);
                return result;
            }

            b1MV = (pmbmd->m_chMBMode == MB_1MV || pmbmd->m_chMBMode == MB_INTRA);

            if (b1MV) {
                I32_WMV iMVOffst = 4 * imbY * (I32_WMV) pWMVDec->m_uintNumMBX + 2 * imbX;
                I32_WMV iBotLeftBlkIndex;
                I32_WMV iIntra ;
                if (pmbmd->m_chMBMode == MB_INTRA) {
                    pWMVDec->m_pXMotion[iMVOffst] = pWMVDec->m_pXMotion[iMVOffst + 1] = 
                    pWMVDec->m_pXMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX] = pWMVDec->m_pXMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX + 1] = MVx[0] = IBLOCKMV;
                    pWMVDec->m_pYMotion[iMVOffst] = pWMVDec->m_pYMotion[iMVOffst + 1] = 
                    pWMVDec->m_pYMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX] = pWMVDec->m_pYMotion[iMVOffst + 2 * pWMVDec->m_uintNumMBX + 1] = MVy[0] = IBLOCKMV;
                }
                else {
                    if (bHalfPelMode) {
                        I32_WMV iMVY;
                        MVx[0] = pWMVDec->m_pXMotion[iMVOffst] << 1;
                        iMVY = pWMVDec->m_pYMotion[iMVOffst];
                        if (iMVY & 1) {
                            iMVY = iMVY * 2 - iOffset;
                            iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
                        }
                        else {
                            iMVY = iMVY * 2;
                            iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
                        }
                        MVy[0] = (I16_WMV) iMVY;
                    }
                    else {
                              if(pWMVDec->m_bCodecIsWVC1){
						MVx[0] = pWMVDec->m_pXMotion[iMVOffst];
						MVy[0] = pWMVDec->m_pYMotion[iMVOffst];
                              }
                              else {
						I32_WMV iMVY;
						MVx[0] = pWMVDec->m_pXMotion[iMVOffst];
						iMVY = pWMVDec->m_pYMotion[iMVOffst];
						if (iMVY & 1) {
							iMVY -= iOffset;
							iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
						}
						else
							iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
						MVy[0] = (I16_WMV) iMVY;
                               }
                    }
                }


#ifdef INTERLACEV2_B	// need this for B's direct mode ...
			    iBotLeftBlkIndex = iMVOffst + 2 * pWMVDec->m_uintNumMBX;
				pWMVDec->m_pXMotion[iBotLeftBlkIndex] = pWMVDec->m_pXMotion[iBotLeftBlkIndex + 1] = pWMVDec->m_pXMotion[iMVOffst + 1] = pWMVDec->m_pXMotion[iMVOffst];
				pWMVDec->m_pYMotion[iBotLeftBlkIndex] = pWMVDec->m_pYMotion[iBotLeftBlkIndex + 1] = pWMVDec->m_pYMotion[iMVOffst + 1] = pWMVDec->m_pYMotion[iMVOffst];
#endif
           {
                I16_WMV MV_y = MVy[0];
                if(pWMVDec->m_bCodecIsWVC1){
                   MV_y >>= 1;
                }
                if (MV_y & 4) {
                    ppxliRefYMB = ppxliOppRefYMB;
                    ppxliRefUMB = ppxliOppRefUMB;
                    ppxliRefVMB = ppxliOppRefVMB;
                }
                else {
                    ppxliRefYMB = ppxliSameRefYMB;
                    ppxliRefUMB = ppxliSameRefUMB;
                    ppxliRefVMB = ppxliSameRefVMB;
                }
            }
                iIntra = (MVx[0] == IBLOCKMV);
                pmbmd ->m_rgcIntraFlag[0] = pmbmd ->m_rgcIntraFlag[1] = pmbmd ->m_rgcIntraFlag[2] = pmbmd ->m_rgcIntraFlag[3] =  (U8_WMV) iIntra;
                if (pmbmd->m_bCBPAllZero == TRUE && !iIntra)
                {
                    pmbmd->m_rgcBlockXformMode[0] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[1] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[2] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[3] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[4] = XFORMMODE_8x8;
                    pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;

                    if (!pWMVDec->m_pXMotion[iMVOffst] && !pWMVDec->m_pYMotion[iMVOffst]) {
                        // zero motion : always within bounds by definition
                        (*pWMVDec->m_pMotionCompZero) (
                            ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                            ppxliSameRefYMB, ppxliSameRefUMB, ppxliSameRefVMB,
                            pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                        pmbmd->m_bSkip = TRUE;
                        pmbmd->m_dctMd = INTER;
                    }
                    else
                    {
                        MotionCompFieldMode1MV (pWMVDec,
                            ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                            ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
                            imbX, imbY, MVx, MVy);
                        pmbmd->m_bSkip = FALSE;
                    }
                }
                else
                    result = DecodeMBFieldMode1MV (pWMVDec,
                        pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
                        ppxliRefYMB, ppxliRefUMB, ppxliRefVMB,
                        imbX, imbY, MVx, MVy); 
            }
            else {  // 4MV
                I32_WMV iBlockIndex = imbY * pWMVDec->m_uintNumMBX * 4 + imbX * 2;
                I32_WMV iMBIndex = imbY * pWMVDec->m_uintNumMBX + imbX;
                I32_WMV iblk, iCMVx, iCMVy;
                pmbmd ->m_rgcIntraFlag[0] = (U8_WMV) pWMVDec->m_pDiffMV  -> iIntra;
                pmbmd ->m_rgcIntraFlag[1] = (U8_WMV)(pWMVDec->m_pDiffMV + 1) -> iIntra;
                pmbmd ->m_rgcIntraFlag[2] = (U8_WMV)(pWMVDec->m_pDiffMV + 2) -> iIntra;
                pmbmd ->m_rgcIntraFlag[3] = (U8_WMV)(pWMVDec->m_pDiffMV + 3) -> iIntra;

                // Decode chroma MVs
                for (iblk = 0; iblk < 4; iblk ++) {
                    I32_WMV iMVY; 
                    MVx[iblk] = pWMVDec->m_pXMotion[iBlockIndex + (iblk & 2) * pWMVDec->m_uintNumMBX + (iblk & 1)];
                    if(pWMVDec->m_bCodecIsWVC1){
			MVy[iblk] = pWMVDec->m_pYMotion[iBlockIndex + (iblk & 2) * pWMVDec->m_uintNumMBX + (iblk & 1)];
                    } else {
                    iMVY = pWMVDec->m_pYMotion[iBlockIndex + (iblk & 2) * pWMVDec->m_uintNumMBX + (iblk & 1)];
                    if (iMVY & 1) {
                        iMVY -= iOffset;
                        iMVY = (iMVY & ~7) | 4 | ((iMVY & 7) >> 1);
                    }
                    else
                        iMVY = (iMVY & ~7) | ((iMVY & 7) >> 1);
                    MVy[iblk] = (I16_WMV) iMVY;
                    }
                }
                ChromaMVFieldMode (pWMVDec,MVx, MVy, imbX, imbY, b1MV, &iCMVx, &iCMVy);
                pWMVDec->m_pXMotionC[iMBIndex] = (I16_WMV)iCMVx;
                pWMVDec->m_pYMotionC[iMBIndex] = (I16_WMV)iCMVy;

                result = DecodeFieldMode4MV (pWMVDec,
                    pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
                    ppxliSameRefYMB, ppxliSameRefUMB, ppxliSameRefVMB,
                    ppxliOppRefYMB, ppxliOppRefUMB, ppxliOppRefVMB,
                    imbX, imbY); 
            }

            if (ICERR_OK != result) {
                return result;
            }
#ifdef INTERLACEV2_B
           	if (pWMVDec->m_iNumBFrames > 0)
			{
            iOffsetB = ((pWMVDec->m_iCurrentField == 0) ? 0 : (2 * pWMVDec->m_uintNumMBY * 2 * pWMVDec->m_uintNumMBX));
            iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
            iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + iOffsetB] = pWMVDec->m_pXMotion [iTopLeftBlkIndex];
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 + iOffsetB ] = pWMVDec->m_pXMotion [iTopLeftBlkIndex + 1 ];
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + iOffsetB] = pWMVDec->m_pXMotion [iBotLeftBlkIndex];
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 + iOffsetB ] = pWMVDec->m_pXMotion [iBotLeftBlkIndex + 1 ];
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + iOffsetB] = pWMVDec->m_pYMotion [iTopLeftBlkIndex];
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 + iOffsetB ] = pWMVDec->m_pYMotion [iTopLeftBlkIndex + 1 ];
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + iOffsetB] = pWMVDec->m_pYMotion [iBotLeftBlkIndex];
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 + iOffsetB ] = pWMVDec->m_pYMotion [iBotLeftBlkIndex + 1 ];
            }
#endif
			//zou 325
#if 0
			if(pWMVDec->m_nPframes == 1 && pWMVDec->m_tFrmType == PVOP && 0)
			{
				FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump.txt","a");
				int i,j;
				fprintf(fp,"-----------------------%d %d -----------\n",imbY,imbX);
				for(i=0;i<16;i++)
				{
					for(j=0;j<16;j++)
					{
						fprintf(fp,"%x   ",ppxliCurrQYMB[i*pWMVDec->m_iWidthInternal*2+j]);
					}
					fprintf(fp,"\n");
				}
				for(i=0;i<8;i++)
				{
					for(j=0;j<8;j++)
					{
						fprintf(fp,"%x   ",ppxliCurrQUMB[i*pWMVDec->m_iWidthInternalUV*2+j]);
					}
					fprintf(fp,"\n");
				}
				for(i=0;i<8;i++)
				{
					for(j=0;j<8;j++)
					{
						fprintf(fp,"%x   ",ppxliCurrQVMB[i*pWMVDec->m_iWidthInternalUV*2+j]);
					}
					fprintf(fp,"\n");
				}
				fclose(fp);
			}
#endif

            pmbmd++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            ppxliSameRefYMB += MB_SIZE;
            ppxliSameRefUMB += BLOCK_SIZE;
            ppxliSameRefVMB += BLOCK_SIZE;
            ppxliOppRefYMB += MB_SIZE;
            ppxliOppRefUMB += BLOCK_SIZE;
            ppxliOppRefVMB += BLOCK_SIZE;
        }
        if (pWMVDec->m_iOverlap & 1)
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQYMB - pWMVDec->m_iWidthY, ppxliCurrQUMB - pWMVDec->m_iWidthUV, 
                ppxliCurrQVMB - pWMVDec->m_iWidthUV, bTopRowOfSlice, FALSE);

        if (imbY < pWMVDec->m_uintNumMBY - 1 && pWMVDec->m_pbStartOfSliceRow[imbY + 1])
                bBottomRowOfSlice = TRUE;

        bTopRowOfSlice = FALSE;
        if (bBottomRowOfSlice && (pWMVDec->m_iOverlap & 1))
            OverlapMBRow (pWMVDec, imbY, ppxliCurrQYMB - pWMVDec->m_iWidthY + 16 * pWMVDec->m_iWidthPrevY,
                ppxliCurrQUMB - pWMVDec->m_iWidthUV + 8 * pWMVDec->m_iWidthPrevUV, 
                ppxliCurrQVMB - pWMVDec->m_iWidthUV + 8 * pWMVDec->m_iWidthPrevUV,
                FALSE, TRUE);
    }

#ifdef _DISPLAY_MBTYPE_
    if (g_bDisplayVideoInfoBMP) 
        QueryMBType();
#endif

    



#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
        dxvaEndFrame(pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY, FALSE);
#endif

    
#ifndef WMV9_SIMPLE_ONLY
    if (pWMVDec->m_bLoopFilter)
    {

        if (pWMVDec->m_bLoopFilter) {
            ComputeLoopFilterFlags_WMVA(pWMVDec);
            DeblockSLFrame_V9 (pWMVDec,
                pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevY >> 1), 
                pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1), 
                pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp + pWMVDec->m_iCurrentField * (pWMVDec->m_iWidthPrevUV >> 1),
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

    if (pWMVDec->m_bBFrameOn)
    {   // A little post processing to line things up ...
        CWMVMBMode* pmbmd1 = pWMVDec->m_rgmbmd1;
        I32_WMV i,j;
        for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMBY; i++) 
            for (j = 0; j < (I32_WMV) pWMVDec->m_uintNumMBX; j++) 
            {
                //I32_WMV iMVX=0, iMVY=0;
                I16_WMV iX = pWMVDec->m_pXMotionC[j + i * (I32_WMV) pWMVDec->m_uintNumMBX];
                if (iX == IBLOCKMV)
                {
                    pmbmd1->m_dctMd = INTRA;
                }
                else
                {
                    pmbmd1->m_dctMd = INTER;
                }
                pmbmd1++;
            }
    }   // post-processing ends

    pWMVDec->m_bDisplay_AllMB = (pWMVDec->m_bLoopFilter ||  pWMVDec->m_iResIndex != 0 || pWMVDec->m_iRefFrameNum != -1);

    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;
   
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame

//#endif  // ifdef _GENERATE_DXVA_DATA_
    return ICERR_OK;
}


I32_WMV decodeFieldHeadFieldPicture (tWMVDecInternalMember * pWMVDec)    
{ 
    I32_WMV stepDecoded;
    
    stepDecoded = BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_VOP_QUANTIZER);  //zou -->vc1DECBIT_GetBits(pBitstream, VC1_BITS_PQINDEX);
    
    pWMVDec->m_iQPIndex = stepDecoded;

    if (stepDecoded <= MAXHALFQP)
        pWMVDec->m_bHalfStep = BS_getBits(pWMVDec->m_pbitstrmIn,1);  //zou -->vc1DECBIT_GetBits(pBitstream, VC1_BITS_HALFQP);
    else
        pWMVDec->m_bHalfStep = FALSE;

    if (pWMVDec->m_bExplicitFrameQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn,1);
    
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

    pWMVDec->m_iStepSize = stepDecoded;

    pWMVDec->m_iOverlap = 0;
    if (pWMVDec->m_bSequenceOverlap && !(pWMVDec->m_tFrmType == BVOP )) {
        if (pWMVDec->m_iStepSize >= 9)
            pWMVDec->m_iOverlap = 1;
        else if ( 
            (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP))
            pWMVDec->m_iOverlap = 7;
    }

    pWMVDec->m_prgDQuantParam = pWMVDec->m_bUse3QPDZQuantizer ? pWMVDec->m_rgDQuantParam3QPDeadzone : pWMVDec->m_rgDQuantParam5QPDeadzone;

    //g_iStepsize = pWMVDec->m_iQPIndex * 2 + pWMVDec->m_bHalfStep;
    
    pWMVDec->m_iStepSize = stepDecoded;
    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }
    if (pWMVDec->m_tFrmType == PVOP || pWMVDec->m_tFrmType == BVOP) {
        pWMVDec->m_iStepSize = stepDecoded;
        if (pWMVDec->m_iStepSize <= 0 || pWMVDec->m_iStepSize > 31)
            return ICERR_ERROR;
    }
    else if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        pWMVDec->m_iStepSize = stepDecoded;
        if (pWMVDec->m_iStepSize <= 0 || pWMVDec->m_iStepSize > 31)
            return ICERR_ERROR;
    }
    else {
        ;
        return ICERR_UNSUPPORTED;
    }

    if (pWMVDec->m_iQPIndex <= MAXHIGHRATEQP)
    {
        // If QP (picture quant) is <= MAXQP_3QPDEADZONE then we substitute the highrate
        // inter coeff table for the talking head table among the 3 possible tables that
        // can be used to code the coeff data
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_HghRate;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_HghRate;
    }
    else {
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_Talking;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_Talking;
    }
        
    if (pWMVDec->m_bPostProcInfoPresent)
        pWMVDec->m_iPostProcLevel = BS_getBits(pWMVDec->m_pbitstrmIn,2);  //zou -->vc1DECBIT_GetBits(pBitstream, VC1_BITS_POSTPROC);

    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {
        
        // AC Pred flag
        if (DecodeSkipBitX9 (pWMVDec,4) == ICERR_ERROR) {  //zou -->/* AC prediction bitplane */  vc1DECBITPL_ReadBitplane(pState, &pParams->sBPPredictAC, pBitstream);
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
            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 0) // off
                pWMVDec->m_iOverlap = 0;
            else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 0) // on
                pWMVDec->m_iOverlap = 1;
            else { // MB switch - decode bitplane
                if (DecodeSkipBitX9 (pWMVDec,5) == ICERR_ERROR) 
                    return ICERR_ERROR;   
                if (pWMVDec->m_iOverlapIMBCodingMode != SKIP_RAW) {
                I32_WMV i;
                for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                    pWMVDec->m_rgmbmd[i].m_bOverlapIMB = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE : FALSE);  
                }

                }
            }
        }


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
        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits(pWMVDec->m_pbitstrmIn,1);  //zou --->vc1DECBIT_GetBits(pBitstream, VC1_BITS_TRANSACFRM_1);
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits(pWMVDec->m_pbitstrmIn,1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = BS_getBits(pWMVDec->m_pbitstrmIn,1);//zou ---->alue = vc1DECBIT_GetBits(pBitstream, VC1_BITS_TRANSACFRM2_1);
        if (pWMVDec->m_rgiDCTACIntraTableIndx[0]){
            pWMVDec->m_rgiDCTACIntraTableIndx[0] += BS_getBits(pWMVDec->m_pbitstrmIn,1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = pWMVDec->m_rgiDCTACIntraTableIndx[0];           
        pWMVDec->m_iIntraDCTDCTable = BS_getBits(pWMVDec->m_pbitstrmIn,1);  //zou --->vc1DECBIT_GetBits(pBitstream, VC1_BITS_TRANSDCTAB);

        if (pWMVDec->m_iDQuantCodingOn)
            DecodeVOPDQuant (pWMVDec, FALSE);
        else
            SetDefaultDQuantSetting(pWMVDec);
        
        //pWMVDec->m_iRndCtrl = 1;
    }
    else {

        Bool_WMV bRepeat = FALSE;
		I32_WMV iMVRangeIndex = 0;

        if (pWMVDec->m_tFrmType  != BVOP)
            pWMVDec->m_bTwoRefPictures = BS_getBits(pWMVDec->m_pbitstrmIn, 1);  //zou  Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_NUMREF);
        else
            pWMVDec->m_bTwoRefPictures = TRUE;

        if (!pWMVDec->m_bTwoRefPictures)
            pWMVDec->m_bUseMostRecentFieldForRef = !BS_getBits(pWMVDec->m_pbitstrmIn, 1); //zou --->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_REFFIELD);

        // Set MV tables pointers
        if (pWMVDec->m_bTwoRefPictures) {
            // interlace MV tables
            pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_Interlace_MV1;
            pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_Interlace_MV2;
            pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_Interlace_MV3;
            pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_Interlace_MV4;
            pWMVDec->m_pHufMVTables[4] = &pWMVDec->m_HufMVTable_Interlace_MV5;
            pWMVDec->m_pHufMVTables[5] = &pWMVDec->m_HufMVTable_Interlace_MV6;
            pWMVDec->m_pHufMVTables[6] = &pWMVDec->m_HufMVTable_Interlace_MV7;
            pWMVDec->m_pHufMVTables[7] = &pWMVDec->m_HufMVTable_Interlace_MV8;
        }
        else {
            // progressive MV tables
            pWMVDec->m_pHufMVTables[0] = &pWMVDec->m_HufMVTable_Progressive_MV1;
            pWMVDec->m_pHufMVTables[1] = &pWMVDec->m_HufMVTable_Progressive_MV2;
            pWMVDec->m_pHufMVTables[2] = &pWMVDec->m_HufMVTable_Progressive_MV3;
            pWMVDec->m_pHufMVTables[3] = &pWMVDec->m_HufMVTable_Progressive_MV4;
        }



        if (pWMVDec->m_bExtendedMvMode) {
            iMVRangeIndex = BS_getBits(pWMVDec->m_pbitstrmIn,1);
            if (iMVRangeIndex)
                iMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn,1);
            if (iMVRangeIndex == 2)
                iMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn,1);
        }
            
            
        SetMVRangeFlag (pWMVDec, iMVRangeIndex);
        if (!pWMVDec->m_bTwoRefPictures) {
            pWMVDec->m_iYMVRange <<= 1;
            pWMVDec->m_iYMVFlag = pWMVDec->m_iYMVFlag * 2 + 1;

        }

        if (pWMVDec->m_bExtendedDeltaMvMode) {
            pWMVDec->m_iDeltaMVRangeIndex = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            if (pWMVDec->m_iDeltaMVRangeIndex)
                pWMVDec->m_iDeltaMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            if (pWMVDec->m_iDeltaMVRangeIndex == 2)
                pWMVDec->m_iDeltaMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            pWMVDec->m_iExtendedDMVX = pWMVDec->m_iDeltaMVRangeIndex & 1;
            pWMVDec->m_iExtendedDMVY = (pWMVDec->m_iDeltaMVRangeIndex & 2) >> 1;
        }


        pWMVDec->m_bLuminanceWarp = FALSE;
        pWMVDec->m_bLuminanceWarpTop = pWMVDec->m_bLuminanceWarpBottom = FALSE;
        
		if (0){ //pWMVDec->m_tFrmType == BVOP) {
			pWMVDec->m_iX9MVMode = ALL_1MV; //BS_getBits(pWMVDec->m_pbitstrmIn, 1) ? ALL_1MV : ALL_1MV_HALFPEL_BILINEAR;
		} else {

RepeatMVRead:
			if (pWMVDec->m_iStepSize > 12) {
				// low rate modes
				if (BS_getBits(pWMVDec->m_pbitstrmIn, 1))
					pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
				else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1))
					pWMVDec->m_iX9MVMode = ALL_1MV;
				else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1))
					pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL;
				else {
					if (pWMVDec->m_tFrmType == BVOP) {
						pWMVDec->m_iX9MVMode = MIXED_MV;
					}
					else
					if (bRepeat || !BS_getBits(pWMVDec->m_pbitstrmIn, 1))
						pWMVDec->m_iX9MVMode = MIXED_MV;
					else {
						bRepeat = TRUE;
						pWMVDec->m_bLuminanceWarp = TRUE;
						goto RepeatMVRead;
					}
				}
			}
			else {
				// high rate modes
				if (BS_getBits(pWMVDec->m_pbitstrmIn, 1))
					pWMVDec->m_iX9MVMode = ALL_1MV;
				else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1))
					pWMVDec->m_iX9MVMode = MIXED_MV;
				else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1))
					pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL;
				else {
					if (pWMVDec->m_tFrmType == BVOP) {
						pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
					}
					else
					if (bRepeat || !BS_getBits(pWMVDec->m_pbitstrmIn, 1))
						pWMVDec->m_iX9MVMode = ALL_1MV_HALFPEL_BILINEAR;
					else {
						bRepeat = TRUE;
						pWMVDec->m_bLuminanceWarp = TRUE;
						goto RepeatMVRead;
					}
				}
			}
			if (pWMVDec->m_bLuminanceWarp) {
			//	g_iFading = 1;
				if (pWMVDec->m_bFieldMode) {
					if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
						// Both reference field remapped
						pWMVDec->m_bLuminanceWarpTop = pWMVDec->m_bLuminanceWarpBottom = TRUE;
						pWMVDec->m_iLuminanceScaleTop = BS_getBits(pWMVDec->m_pbitstrmIn,6);
						pWMVDec->m_iLuminanceShiftTop = BS_getBits(pWMVDec->m_pbitstrmIn,6);
						pWMVDec->m_iLuminanceScaleBottom = BS_getBits(pWMVDec->m_pbitstrmIn,6);
						pWMVDec->m_iLuminanceShiftBottom = BS_getBits(pWMVDec->m_pbitstrmIn,6);
					}
					else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
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
		}
		// if global warp exists, disable skip MB
		memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);

		if (pWMVDec->m_bBFrameOn)
		{
			if (pWMVDec->m_tFrmType == BVOP)
			{
				if (DecodeSkipBitX9 (pWMVDec,3) == ICERR_ERROR)   // decoded into pWMVDec->m_bSkip bit
					return ICERR_ERROR;

				if (pWMVDec->m_iDirectCodingMode != SKIP_RAW) 
				{
					CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
                    I32_WMV i;
					for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
						if (pmbmd->m_bSkip)
							pmbmd->m_mbType = FORWARD;
						else
							pmbmd->m_mbType = (MBType)0;
						pmbmd++;
					}
				}
			}
		}
        pWMVDec->m_bCODFlagOn = 1;
        // read MV and CBP codetable indices
        if (pWMVDec->m_iX9MVMode == MIXED_MV)
            pWMVDec->m_pHufMBMode = pWMVDec->m_pHufMBModeMixedMVTables[BS_getBits(pWMVDec->m_pbitstrmIn,3)];//zou --> Value = vc1DECBIT_GetBits(pBitstream, 3);  /*-3 bit MBMODETAB */
        else
            pWMVDec->m_pHufMBMode = pWMVDec->m_pHufMBMode1MVTables[BS_getBits(pWMVDec->m_pbitstrmIn,3)];;
        if (pWMVDec->m_bTwoRefPictures)
            pWMVDec->m_pHufMVTable_V9 = pWMVDec->m_pHufMVTables[BS_getBits(pWMVDec->m_pbitstrmIn,3)];
        else
            pWMVDec->m_pHufMVTable_V9 = pWMVDec->m_pHufMVTables[BS_getBits(pWMVDec->m_pbitstrmIn,2)];
        
        pWMVDec->m_pHufPCBPCYDec = pWMVDec->m_pHufInterlaceCBPCYTables[BS_getBits(pWMVDec->m_pbitstrmIn,3)];//zou-->Value = vc1DECBIT_GetBits(pBitstream, 2);
        pWMVDec->m_pHufICBPCYDec = pWMVDec->m_pHufPCBPCYDec;
        if (pWMVDec->m_iX9MVMode == MIXED_MV)
            pWMVDec->m_p4MVBP = pWMVDec->m_p4MVBPTables[BS_getBits(pWMVDec->m_pbitstrmIn,2)];//zou --> Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_4MVBPTAB);

        if (pWMVDec->m_iDQuantCodingOn)
	        DecodeVOPDQuant (pWMVDec,TRUE);

        if (pWMVDec->m_bXformSwitch) {
	        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
	        {
		        pWMVDec->m_bMBXformSwitching = FALSE;
		        pWMVDec->m_iFrameXformMode = s_pXformLUT[BS_getBits(pWMVDec->m_pbitstrmIn,2)];
	        }
	        else
		        pWMVDec->m_bMBXformSwitching = TRUE;
        }
		else
            pWMVDec->m_bMBXformSwitching = FALSE;

        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits(pWMVDec->m_pbitstrmIn,1); //zou ---> Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_TRANSACFRM_1);
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits(pWMVDec->m_pbitstrmIn,1);
        }
        
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = 
        pWMVDec->m_rgiDCTACInterTableIndx[1] = pWMVDec->m_rgiDCTACInterTableIndx[2] = pWMVDec->m_rgiDCTACInterTableIndx[0];
        pWMVDec->m_iIntraDCTDCTable = BS_getBits(pWMVDec->m_pbitstrmIn,1); //zou -->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_TRANSDCTAB);
          
    }

    if (pWMVDec->m_iOverlap & 1) {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 0;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 0;
    }
    else {
        pWMVDec->m_pAvgQuanDctCoefDec[0] = pWMVDec->m_pAvgQuanDctCoefDec[BLOCK_SIZE] = 128;
        pWMVDec->m_pAvgQuanDctCoefDecC[0] = pWMVDec->m_pAvgQuanDctCoefDecC[BLOCK_SIZE] = 128;
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }
    return ICERR_OK;
}

I32_WMV decodeMBOverheadOfPVOP_FieldPicture (tWMVDecInternalMember * pWMVDec, CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY, Bool_WMV bHalfPelMode)
{
    I32_WMV iCBPCY = 0;
    I32_WMV iPredX, iPredY;
    
    Bool_WMV bCBPPresent = FALSE, bMVPresent = FALSE;

    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;

    // Decode MB mode
    if (ICERR_OK != DecodeMBModeFieldPicture (pWMVDec, pmbmd, &bMVPresent, &bCBPPresent))
        return ICERR_ERROR;;

        pmbmd->m_bCBPAllZero = !bCBPPresent;

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT

    if (pmbmd->m_chMBMode == MB_INTRA) {
        pWMVDec->m_pDiffMV->iIntra = 1;
        
            if (pWMVDec->m_bDQuantOn)
                if (ICERR_OK != decodeDQuantParam (pWMVDec, pmbmd))
                    return ICERR_ERROR;
        if (bCBPPresent) {
            pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);

            iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufICBPCYDec, pWMVDec->m_pbitstrmIn) + 1;

        }
        else {
            iCBPCY = 0;
            pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        }
    }
    else {
        // Inter-coded MB
        I32_WMV iStride, iMVOffst;
        if (pmbmd->m_chMBMode == MB_1MV) {
            // 1MV MB
            pWMVDec->m_pDiffMV->iIntra = 0;
            if (bMVPresent)
            {
                if (pWMVDec->m_bTwoRefPictures)
                    decodeMV_Interlace(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
                else {
                    decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
                    pWMVDec->m_pDiffMV->iY <<= 1;
                }
            }
            else
                memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV));
            // hybrid MV
            PredictHybridMVFieldPicture (pWMVDec, iMBX * 2, iMBY * 2, TRUE, bHalfPelMode, &iPredX, &iPredY, pWMVDec->m_pDiffMV->iY & 1);
            ComputeMVFromDiffMVFieldPicture (pWMVDec, iMBX * 2, iMBY * 2, iPredX, iPredY, pWMVDec->m_pDiffMV);

            iStride = (I32_WMV) pWMVDec->m_uintNumMBX * 2;
            iMVOffst = 2 * (iStride * iMBY + iMBX);
            pWMVDec->m_pXMotion[iMVOffst + 1] = pWMVDec->m_pXMotion[iMVOffst + iStride + 1] = pWMVDec->m_pXMotion[iMVOffst + iStride] = pWMVDec->m_pXMotion[iMVOffst];
	        pWMVDec->m_pYMotion[iMVOffst + 1] = pWMVDec->m_pYMotion[iMVOffst + iStride + 1] = pWMVDec->m_pYMotion[iMVOffst + iStride] = pWMVDec->m_pYMotion[iMVOffst];
        }
        else {
            // 4MV MB
            I32_WMV iMV;
            I32_WMV iblk;
            memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV) * 4);
            iMV = Huffman_WMV_get(pWMVDec->m_p4MVBP, pWMVDec->m_pbitstrmIn);
            for ( iblk = 0; iblk < 4; iblk++) {
                I32_WMV iBlkX;
                I32_WMV iBlkY;

                (pWMVDec->m_pDiffMV + iblk)->iIntra = 0;
               
                if (iMV & (1 << (3 - iblk))) {
                    if (pWMVDec->m_bTwoRefPictures)
                        decodeMV_Interlace(pWMVDec,pWMVDec->m_pHufMVTable_V9, (pWMVDec->m_pDiffMV + iblk));
                    else {
                        decodeMV_Progressive(pWMVDec, pWMVDec->m_pHufMVTable_V9, (pWMVDec->m_pDiffMV + iblk));
                        (pWMVDec->m_pDiffMV + iblk)->iY <<= 1;
                    }
                }
                else
                    memset (pWMVDec->m_pDiffMV + iblk, 0, sizeof (CDiffMV));
                // hybrid MV
                iBlkX = iMBX * 2 + (iblk & 1);
                iBlkY = iMBY * 2 + (iblk >> 1);
                PredictHybridMVFieldPicture (pWMVDec, iBlkX, iBlkY, FALSE, FALSE, &iPredX, &iPredY, (pWMVDec->m_pDiffMV + iblk)->iY & 1);
                ComputeMVFromDiffMVFieldPicture (pWMVDec, iBlkX, iBlkY, iPredX, iPredY, pWMVDec->m_pDiffMV + iblk);
            }
        }

        // Decode DCT CBP
        if (bCBPPresent)
            iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufPCBPCYDec, pWMVDec->m_pbitstrmIn) + 1; //zou-->Value = vc1DECBIT_GetVLC(pBitstream, pState->sPicParams.pCodedBlockPatternTable);//10/
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
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufMBXformTypeDec, pWMVDec->m_pbitstrmIn);
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

    return ICERR_OK;
}

I32_WMV DecodeMBModeFieldPicture (tWMVDecInternalMember * pWMVDec, CWMVMBMode* pmbmd, Bool_WMV * pbMVPresent, Bool_WMV * pbCBPPresent)
{

    Bool_WMV bMVPresent = * pbMVPresent;
    Bool_WMV bCBPPresent = * pbCBPPresent;
    I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufMBMode , pWMVDec->m_pbitstrmIn); //zou--->Value = vc1DECBIT_GetVLC(pBitstream, pState->sPicParams.pMBModeTable);
    if (iIndex < 0 || iIndex > 9)
    {
        * pbMVPresent = bMVPresent;
        * pbCBPPresent = bCBPPresent;
        return ICERR_OK;
    }
    else {
        if (iIndex < 2) {
            // Intra
            pmbmd->m_chMBMode = MB_INTRA;
            bMVPresent = FALSE;
            if (iIndex == 0)
                bCBPPresent = FALSE;
            else
                bCBPPresent = TRUE;
        }
        else if (iIndex < 6) {
            // 1MV
            pmbmd->m_chMBMode = MB_1MV;
            bCBPPresent = ((iIndex - 2) & 2) == 2;
            bMVPresent = ((iIndex - 2) & 1) == 1;
        }
        else {
            // 4MV
            pmbmd->m_chMBMode = MB_4MV;
            bCBPPresent = (iIndex - 6);
            bMVPresent = TRUE;
        }
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
		return ICERR_ERROR;
	}

    * pbMVPresent = bMVPresent;
        * pbCBPPresent = bCBPPresent;
    return ICERR_OK;
}

static I32_WMV gFineMV[] = {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7};
static I32_WMV aMVOffsets[] = {0,1,2,4,8,16,32,64,128};
static I32_WMV aMVOffsets2[] = {0,1,3,7,15,31,63,127,255};
#define XBINS   9
#define YBINS   14
#define MV_ESCAPE_INTERLACE     125
#define MV_ESCAPE_PROGRESSIVE   71

Void_WMV decodeMV_Interlace(tWMVDecInternalMember * pWMVDec,Huffman_WMV* pHufMVTable, CDiffMV *pDiffMV)
{
    I32_WMV iFine=0; 
    I32_WMV iIndex1, iSign;
    I32_WMV iIndex = Huffman_WMV_get(pHufMVTable, pWMVDec->m_pbitstrmIn);
    
    if (iIndex == 0) {
        if (pWMVDec->m_iExtendedDMVX) {
            iFine = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
            iSign = - (iFine & 1);
            pDiffMV->iX = iSign ^ ((iFine >> 1) + 1);
            pDiffMV->iX -= iSign;
        }
        else {
            pDiffMV->iX = 1 - 2 * (BS_getBits(pWMVDec->m_pbitstrmIn,1));
        }
        pDiffMV->iY = 0;
    }
    else if (iIndex == MV_ESCAPE_INTERLACE) {
        pDiffMV->iX = BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iLogXRange /*- pWMVDec->m_pAltTables->m_iHalfPelMV*/);
        pDiffMV->iY = BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iLogYRange /*- pWMVDec->m_pAltTables->m_iHalfPelMV*/);
    }
    else {
        iIndex1 = (iIndex + 1) % XBINS;
//        iHalfpelMV = pWMVDec->m_pAltTables->m_iHalfPelMV && (iIndex1 == iNm1);  // TODO ... how are we handling half-pel ?????
//        iFine = BS_getBits(pWMVDec->m_pbitstrmIn,gFineMV[iIndex1] - iHalfpelMV);
        if (iIndex1) {
            if (pWMVDec->m_iExtendedDMVX) {
                iFine = BS_getBits(pWMVDec->m_pbitstrmIn, iIndex1 + 1);
                iSign = - (iFine & 1);
                pDiffMV->iX = iSign ^ ((iFine >> 1) + aMVOffsets2[iIndex1]);
                pDiffMV->iX -= iSign;
            }
            else {

        iFine = BS_getBits(pWMVDec->m_pbitstrmIn,iIndex1);
        iSign = - (iFine & 1);
        pDiffMV->iX = iSign ^ ((iFine >> 1) + aMVOffsets[iIndex1]);
        pDiffMV->iX -= iSign;
                    }
        }
        else 
            pDiffMV->iX = 0;


        iIndex1 = (iIndex + 1) / XBINS;
//        iHalfpelMV = pWMVDec->m_pAltTables->m_iHalfPelMV && (iIndex1 == iNm1);  // TODO ... how are we handling half-pel ?????
//        iFine = BS_getBits(pWMVDec->m_pbitstrmIn,gFineMV[iIndex1] - iHalfpelMV);
        if (iIndex1) {
            if (pWMVDec->m_iExtendedDMVY) {
                if (iIndex1 == 0 || iIndex1 == 1) {
                    pDiffMV->iY = 0 - (iIndex1 & 1);
                }
                else {
                    iFine = BS_getBits(pWMVDec->m_pbitstrmIn, gFineMV[iIndex1 + 2]);
                    iSign = - (iFine & 1);
                    pDiffMV->iY = iSign ^ ((iFine >> 1) + aMVOffsets2[iIndex1 >> 1]);
                    pDiffMV->iY -= iSign;
                    pDiffMV->iY = pDiffMV->iY * 2 - (iIndex1 & 1);
                }
            }
            else {

        iFine = BS_getBits(pWMVDec->m_pbitstrmIn,gFineMV[iIndex1]);
        iSign = - (iFine & 1);
        pDiffMV->iY = iSign ^ ((iFine >> 1) + aMVOffsets[iIndex1 >> 1]);
        pDiffMV->iY -= iSign;
        pDiffMV->iY = pDiffMV->iY * 2 - (iIndex1 & 1);
                }
            }
                else
            pDiffMV->iY = 0;
    }
}


Void_WMV decodeMV_Progressive(tWMVDecInternalMember * pWMVDec, Huffman_WMV* pHufMVTable, CDiffMV *pDiffMV)
{
    I32_WMV iFine=0; 
    I32_WMV iIndex1, iSign;
    I32_WMV iIndex = Huffman_WMV_get(pHufMVTable, pWMVDec->m_pbitstrmIn);
    
    if (iIndex == 0) {
        if (pWMVDec->m_iExtendedDMVX) {
            iFine = BS_getBits(pWMVDec->m_pbitstrmIn, 2);
            iSign = - (iFine & 1);
            pDiffMV->iX = iSign ^ ((iFine >> 1) + 1);
            pDiffMV->iX -= iSign;
        }
        else
            pDiffMV->iX = 1 - 2 * (BS_getBits(pWMVDec->m_pbitstrmIn, 1));
        pDiffMV->iY = 0;
    }
    else if (iIndex == MV_ESCAPE_PROGRESSIVE) {
        pDiffMV->iX = BS_getBits(pWMVDec->m_pbitstrmIn, pWMVDec->m_iLogXRange /*- m_pAltTables->m_iHalfPelMV*/);
        pDiffMV->iY = BS_getBits(pWMVDec->m_pbitstrmIn, pWMVDec->m_iLogYRange /*- m_pAltTables->m_iHalfPelMV*/);
    }
    else {
        iIndex1 = (iIndex + 1) % XBINS; //zou -->Index1 = (Index + 1) % 9; //7/
        if (pWMVDec->m_iExtendedDMVX) {
            if (iIndex1) {
                iFine = BS_getBits(pWMVDec->m_pbitstrmIn,iIndex1 + 1);
                iSign = - (iFine & 1);
                pDiffMV->iX = iSign ^ ((iFine >> 1) + aMVOffsets2[iIndex1]);
                pDiffMV->iX -= iSign;
            }
            else 
                pDiffMV->iX = 0;
        }
        else {
            iFine = BS_getBits(pWMVDec->m_pbitstrmIn, iIndex1); //zou -->Value = vc1DECBIT_GetBits(pBitstream, (UBYTE8)(Index1 + ExtendX));//8/
            iSign = - (iFine & 1);
            pDiffMV->iX = iSign ^ ((iFine >> 1) + aMVOffsets[iIndex1]);
            pDiffMV->iX -= iSign;//zou-->DMV_X = DMV_X - Sign;
        }

        iIndex1 = (iIndex + 1) / XBINS;
        if (pWMVDec->m_iExtendedDMVY) {
            if (iIndex1) {
                iFine = BS_getBits(pWMVDec->m_pbitstrmIn, iIndex1 + 1);
                iSign = - (iFine & 1);
                pDiffMV->iY = iSign ^ ((iFine >> 1) + aMVOffsets2[iIndex1]);
                pDiffMV->iY -= iSign;
            }
            else
                pDiffMV->iY = 0;
        }
        else {
            iFine = BS_getBits(pWMVDec->m_pbitstrmIn, iIndex1);// Value = vc1DECBIT_GetBits(pBitstream, (UBYTE8)(Index1 + ExtendY));//9/
            iSign = - (iFine & 1);
            pDiffMV->iY = iSign ^ ((iFine >> 1) + aMVOffsets[iIndex1]);
            pDiffMV->iY -= iSign;
        }
    }
}


Void_WMV PredictHybridMVFieldPicture (tWMVDecInternalMember * pWMVDec,I32_WMV iX, I32_WMV iY, Bool_WMV b1MV, Bool_WMV bHalfPelMode,
                                I32_WMV *iPredX, I32_WMV *iPredY, I32_WMV iMVPolarity)
{
    CMVPred cMVPred;
    I32_WMV iHybridMV;
    I32_WMV iPredMVX, iPredMVY;

    Bool_WMV bTopBndry = FALSE;
    if (!(iY & 1)) {
        bTopBndry = ((iY == 0) || pWMVDec->m_pbStartOfSliceRow [iY >> 1]);
    }

    PredictFieldModeMVEx (pWMVDec, &cMVPred, iX, iY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, b1MV, bTopBndry, bHalfPelMode);

    if (iMVPolarity == 1) {
        // Use non-dominant field reference
        if (cMVPred.iPolarity == 0) {
            // Dominant predictor is same field - so use opposite field
            if (cMVPred.bOppFieldHybridMV) {
                // Bit indicates which MV predictor to use
                iHybridMV = 1 + (I8_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                if (iHybridMV == 1) {
                    iPredMVX = cMVPred.iPredOppX1;
                    iPredMVY = cMVPred.iPredOppY1;
                }
                else {
                    iPredMVX = cMVPred.iPredOppX2;
                    iPredMVY = cMVPred.iPredOppY2;
                }
            }
            else {
                iPredMVX = cMVPred.iPredOppX1;
                iPredMVY = cMVPred.iPredOppY1;
            }
        }
        else {
            // Dominant predictor is opposite field - so use same field
            if (cMVPred.bSameFieldHybridMV) {
                // Bit indicates which MV predictor to use
                iHybridMV = 1 + (I8_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                if (iHybridMV == 1) {
                    iPredMVX = cMVPred.iPredSameX1;
                    iPredMVY = cMVPred.iPredSameY1;
                }
                else {
                    iPredMVX = cMVPred.iPredSameX2;
                    iPredMVY = cMVPred.iPredSameY2;
                }
            }
            else {
                iPredMVX = cMVPred.iPredSameX1;
                iPredMVY = cMVPred.iPredSameY1;
            }
        }
        iPredMVY ++;
    }
    else {
        // Use dominant field reference
        if (cMVPred.iPolarity == 0) {
            // Dominant predictor is same field - so use same field
            if (cMVPred.bSameFieldHybridMV) {
                // Bit indicates which MV predictor to use
                iHybridMV = 1 + (I8_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                if (iHybridMV == 1) {
                    iPredMVX = cMVPred.iPredSameX1;
                    iPredMVY = cMVPred.iPredSameY1;
                }
                else {
                    iPredMVX = cMVPred.iPredSameX2;
                    iPredMVY = cMVPred.iPredSameY2;
                }
            }
            else {
                iPredMVX = cMVPred.iPredSameX1;
                iPredMVY = cMVPred.iPredSameY1;
            }
        }
        else {
            // Dominant predictor is opposite field - so use opposite field
            if (cMVPred.bOppFieldHybridMV) {
                // Bit indicates which MV predictor to use
                iHybridMV = 1 + (I8_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                if (iHybridMV == 1) {
                    iPredMVX = cMVPred.iPredOppX1;
                    iPredMVY = cMVPred.iPredOppY1;
                }
                else {
                    iPredMVX = cMVPred.iPredOppX2;
                    iPredMVY = cMVPred.iPredOppY2;
                }
            }
            else {
                iPredMVX = cMVPred.iPredOppX1;
                iPredMVY = cMVPred.iPredOppY1;
            }
        }
    }
    *iPredX = iPredMVX;
    *iPredY = iPredMVY;
}

typedef struct _CMVScale {
I32_WMV iMaxZone1ScaledFarMVX;
I32_WMV iMaxZone1ScaledFarMVY;
I32_WMV iZone1OffsetScaledFarMVX;
I32_WMV iZone1OffsetScaledFarMVY;
I32_WMV iFarFieldScale1;
I32_WMV iFarFieldScale2;
I32_WMV iNearFieldScale;
} CMVScale;

// P field scaling
static CMVScale s_sMVScaleValuesFirstField [4] =
{{32, 8, 37, 10, 512, 219, 128}, 
 {48, 12, 20, 5, 341, 236, 192},
 {53, 13, 14, 4, 307, 242, 213}, 
 {56, 14, 11, 3, 293, 245, 224}};
static CMVScale s_sMVScaleValuesSecondField [4] =
{{32, 8, 37, 10, 512, 219, 128}, 
 {16, 4, 52, 13, 1024, 204, 64},
 {11, 3, 56, 14, 1536, 200, 43}, 
 { 8, 2, 58, 15, 2048, 198, 32}};

// B field scaling
static CMVScale s_sMVScaleValuesFirstFieldB [4] =
{{32, 8, 37, 10, 512, 219, 128}, 
 {48, 12, 20, 5, 341, 236, 192},
 {53, 13, 14, 4, 307, 242, 213}, 
 {56, 14, 11, 3, 293, 245, 224}};
static CMVScale s_sMVScaleValuesSecondFieldB [4] =
{{43, 11, 26, 7, 384, 230, 171}, 
 {51, 13, 17, 4, 320, 239, 205}, 
 {55, 14, 12, 3, 299, 244, 219}, 
 {57, 14, 10, 3, 288, 246, 228}};

Void_WMV SetupFieldPictureMVScaling (tWMVDecInternalMember * pWMVDec, I32_WMV iRefFrameDistance)
{
    
    if (iRefFrameDistance > 3)
        iRefFrameDistance = 3;
    
    if (pWMVDec->m_iCurrentTemporalField == 0) {
        pWMVDec->m_iMaxZone1ScaledFarMVX = s_sMVScaleValuesFirstField[iRefFrameDistance].iMaxZone1ScaledFarMVX;
        pWMVDec->m_iMaxZone1ScaledFarMVY = s_sMVScaleValuesFirstField[iRefFrameDistance].iMaxZone1ScaledFarMVY;
        pWMVDec->m_iZone1OffsetScaledFarMVX = s_sMVScaleValuesFirstField[iRefFrameDistance].iZone1OffsetScaledFarMVX;
        pWMVDec->m_iZone1OffsetScaledFarMVY = s_sMVScaleValuesFirstField[iRefFrameDistance].iZone1OffsetScaledFarMVY;
        pWMVDec->m_iFarFieldScale1 = s_sMVScaleValuesFirstField[iRefFrameDistance].iFarFieldScale1;
        pWMVDec->m_iFarFieldScale2 = s_sMVScaleValuesFirstField[iRefFrameDistance].iFarFieldScale2;
        pWMVDec->m_iNearFieldScale = s_sMVScaleValuesFirstField[iRefFrameDistance].iNearFieldScale;
    }
    else {
        pWMVDec->m_iMaxZone1ScaledFarMVX = s_sMVScaleValuesSecondField[iRefFrameDistance].iMaxZone1ScaledFarMVX;
        pWMVDec->m_iMaxZone1ScaledFarMVY = s_sMVScaleValuesSecondField[iRefFrameDistance].iMaxZone1ScaledFarMVY;
        pWMVDec->m_iZone1OffsetScaledFarMVX = s_sMVScaleValuesSecondField[iRefFrameDistance].iZone1OffsetScaledFarMVX;
        pWMVDec->m_iZone1OffsetScaledFarMVY = s_sMVScaleValuesSecondField[iRefFrameDistance].iZone1OffsetScaledFarMVY;
        pWMVDec->m_iFarFieldScale1 = s_sMVScaleValuesSecondField[iRefFrameDistance].iFarFieldScale1;
        pWMVDec->m_iFarFieldScale2 = s_sMVScaleValuesSecondField[iRefFrameDistance].iFarFieldScale2;
        pWMVDec->m_iNearFieldScale = s_sMVScaleValuesSecondField[iRefFrameDistance].iNearFieldScale;
    }
}

Void_WMV SetupForwardBFieldPictureMVScaling (tWMVDecInternalMember * pWMVDec, I32_WMV iRefFrameDistance)
{
    if (iRefFrameDistance > 3)
        iRefFrameDistance = 3;
    if (pWMVDec->m_iCurrentTemporalField == 0) {
        pWMVDec->m_iMaxZone1ScaledFarMVX = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVX;
        pWMVDec->m_iMaxZone1ScaledFarMVY = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVY;
        pWMVDec->m_iZone1OffsetScaledFarMVX = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVX;
        pWMVDec->m_iZone1OffsetScaledFarMVY = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVY;
        pWMVDec->m_iFarFieldScale1 = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iFarFieldScale1;
        pWMVDec->m_iFarFieldScale2 = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iFarFieldScale2;
        pWMVDec->m_iNearFieldScale = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iNearFieldScale;
    }
    else {
        pWMVDec->m_iMaxZone1ScaledFarMVX = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVX;
        pWMVDec->m_iMaxZone1ScaledFarMVY = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVY;
        pWMVDec->m_iZone1OffsetScaledFarMVX = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVX;
        pWMVDec->m_iZone1OffsetScaledFarMVY = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVY;
        pWMVDec->m_iFarFieldScale1 = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iFarFieldScale1;
        pWMVDec->m_iFarFieldScale2 = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iFarFieldScale2;
        pWMVDec->m_iNearFieldScale = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iNearFieldScale;
    }
}

Void_WMV SetupBackwardBFieldPictureMVScaling (tWMVDecInternalMember * pWMVDec, I32_WMV iRefFrameDistance)
{
    if (iRefFrameDistance > 3)
        iRefFrameDistance = 3;
    if (pWMVDec->m_iCurrentTemporalField == 0) {
        pWMVDec->m_iMaxZone1ScaledFarBackMVX = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVX;
        pWMVDec->m_iMaxZone1ScaledFarBackMVY = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVY;
        pWMVDec->m_iZone1OffsetScaledFarBackMVX = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVX;
        pWMVDec->m_iZone1OffsetScaledFarBackMVY = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVY;
        pWMVDec->m_iFarFieldScaleBack1 = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iFarFieldScale1;
        pWMVDec->m_iFarFieldScaleBack2 = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iFarFieldScale2;
        pWMVDec->m_iNearFieldScaleBack = s_sMVScaleValuesFirstFieldB[iRefFrameDistance].iNearFieldScale;
    }
    else {
        pWMVDec->m_iMaxZone1ScaledFarBackMVX = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVX;
        pWMVDec->m_iMaxZone1ScaledFarBackMVY = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iMaxZone1ScaledFarMVY;
        pWMVDec->m_iZone1OffsetScaledFarBackMVX = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVX;
        pWMVDec->m_iZone1OffsetScaledFarBackMVY = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iZone1OffsetScaledFarMVY;
        pWMVDec->m_iFarFieldScaleBack1 = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iFarFieldScale1;
        pWMVDec->m_iFarFieldScaleBack2 = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iFarFieldScale2;
        pWMVDec->m_iNearFieldScaleBack = s_sMVScaleValuesSecondFieldB[iRefFrameDistance].iNearFieldScale;
    }
}


Void_WMV ScaleFarPred(tWMVDecInternalMember * pWMVDec, I32_WMV iXO, I32_WMV iYO, I32_WMV *piXS, I32_WMV * piYS, Bool_WMV bScaleForSameField)
{
    I32_WMV iYFO;
    I32_WMV iXS = *piXS;
    I32_WMV iYS = *piYS;

    if (abs(iXO) < pWMVDec->m_iMaxZone1ScaledFarMVX)
        iXS = iXO * pWMVDec->m_iFarFieldScale1 >> 8;
    else {
        if (iXO < 0)
            iXS = -pWMVDec->m_iZone1OffsetScaledFarMVX + ((iXO * pWMVDec->m_iFarFieldScale2) >> 8);
        else
            iXS = pWMVDec->m_iZone1OffsetScaledFarMVX + ((iXO * pWMVDec->m_iFarFieldScale2) >> 8);
    }
    if (bScaleForSameField) {
        if (pWMVDec->m_iCurrentField == 0) {
            iYFO = (iYO - 1) >> 1;
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2;
            }
        }
        else {
            iYFO = (iYO + 1) >> 1;
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2;
            }
        }
        if (iYS > pWMVDec->m_iYMVRange - 2)                          
            iYS = pWMVDec->m_iYMVRange - 2;                          
        if (iYS < -pWMVDec->m_iYMVRange)                             
            iYS = -pWMVDec->m_iYMVRange;

    }
    else { // scale for opposite field
        iYFO = iYO  >> 1;
        if (pWMVDec->m_iCurrentField == 0) {
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2 + 1;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2 + 1;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2 + 1;
            }
        }
        else {
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2 - 1;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2 - 1;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2 - 1;
            }
        }
        if (iYS > pWMVDec->m_iYMVRange - 1)                          
            iYS = pWMVDec->m_iYMVRange - 1;                          
        if (iYS < -pWMVDec->m_iYMVRange + 1)                             
            iYS = -pWMVDec->m_iYMVRange + 1;

    }
                       
    if (iXS > pWMVDec->m_iXMVRange - 1)                          
        iXS = pWMVDec->m_iXMVRange - 1;                          
    if (iXS < -pWMVDec->m_iXMVRange)                             
        iXS = -pWMVDec->m_iXMVRange;                             
    

    *piXS =  iXS;
    *piYS =  iYS;

}

Void_WMV ScaleNearPred(tWMVDecInternalMember * pWMVDec, I32_WMV iXS, I32_WMV iYS, I32_WMV* piXO, I32_WMV * piYO, Bool_WMV bScaleForOppField)
{
    I32_WMV iYFS;
    I32_WMV iXO = *piXO;
    I32_WMV iYO = *piYO;
    iXO = iXS * pWMVDec->m_iNearFieldScale >> 8;
    if (bScaleForOppField) {
        iYFS = iYS  >> 1;
        if (pWMVDec->m_iCurrentField == 0)              
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2 + 1;          
        else                                   
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2 - 1;
    }
    else {
        if (pWMVDec->m_iCurrentField == 0) {
            iYFS = (iYS - 1)  >> 1;
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2;
        }          
        else {                                   
            iYFS = (iYS + 1)  >> 1;
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2;
        }
    }
    *piXO =  iXO;
    *piYO =  iYO;
}

Void_WMV ScaleFarPredForward(tWMVDecInternalMember * pWMVDec, I32_WMV iXO, I32_WMV iYO, I32_WMV *piXS, I32_WMV * piYS, Bool_WMV bScaleForSameField)
{
    I32_WMV iYFO;
    I32_WMV iXS = *piXS;
    I32_WMV iYS = *piYS;
    if (abs(iXO) < pWMVDec->m_iMaxZone1ScaledFarMVX)
        iXS = iXO * pWMVDec->m_iFarFieldScale1 >> 8;
    else {
        if (iXO < 0)
            iXS = -pWMVDec->m_iZone1OffsetScaledFarMVX + ((iXO * pWMVDec->m_iFarFieldScale2) >> 8);
        else
            iXS = pWMVDec->m_iZone1OffsetScaledFarMVX + ((iXO * pWMVDec->m_iFarFieldScale2) >> 8);
    }
    if (bScaleForSameField) {
        if (pWMVDec->m_iCurrentField == 0) {
            iYFO = (iYO - 1) >> 1;
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2;
            }
        }
        else {
            iYFO = (iYO + 1) >> 1;
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2;
            }
        }
        if (iYS > pWMVDec->m_iYMVRange - 2)                          
            iYS = pWMVDec->m_iYMVRange - 2;                          
        if (iYS < -pWMVDec->m_iYMVRange)                             
            iYS = -pWMVDec->m_iYMVRange;

    }
    else {
        iYFO = iYO >> 1;
        if (pWMVDec->m_iCurrentField == 0) {
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2 + 1;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2 + 1;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2 + 1;
            }
        }
        else {
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScale1) >> 8) * 2 - 1;
            else {
                if (iYFO < 0)
                    iYS =  (((iYFO * pWMVDec->m_iFarFieldScale2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarMVY) * 2 - 1;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarMVY + ((iYFO * pWMVDec->m_iFarFieldScale2) >> 8)) * 2 - 1;
            }
        }
        if (iYS > pWMVDec->m_iYMVRange - 1)                          
            iYS = pWMVDec->m_iYMVRange - 1;                          
        if (iYS < -pWMVDec->m_iYMVRange + 1)                             
            iYS = -pWMVDec->m_iYMVRange + 1;
    }
                      
    if (iXS > pWMVDec->m_iXMVRange - 1)                          
        iXS = pWMVDec->m_iXMVRange - 1;                          
    if (iXS < -pWMVDec->m_iXMVRange)                             
        iXS = -pWMVDec->m_iXMVRange;                             
    

    *piXS =  iXS;
    *piYS =  iYS;
}

Void_WMV ScaleNearPredForward(tWMVDecInternalMember * pWMVDec, I32_WMV iXS, I32_WMV iYS, I32_WMV* piXO, I32_WMV * piYO, Bool_WMV bScaleForOppField)
{
    I32_WMV iYFS;
    I32_WMV iXO = *piXO;
    I32_WMV iYO = *piYO;
    iXO = iXS * pWMVDec->m_iNearFieldScale >> 8;
    if (bScaleForOppField) {
        iYFS = iYS  >> 1;
        if (pWMVDec->m_iCurrentField == 0)              
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2 + 1;          
        else                                   
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2 - 1;
    }
    else {
        if (pWMVDec->m_iCurrentField == 0) {
            iYFS = (iYS - 1)  >> 1;             
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2;
        }          
        else {
            iYFS = (iYS + 1)  >> 1;             
            iYO = (iYFS * pWMVDec->m_iNearFieldScale >> 8) * 2;
        }
    }

    *piXO =  iXO;
    *piYO =  iYO;
}

Void_WMV ScaleFarPredBackward(tWMVDecInternalMember * pWMVDec, I32_WMV iXO, I32_WMV iYO, I32_WMV *piXS, I32_WMV * piYS, Bool_WMV bScaleForSameField)
{
    I32_WMV iYFO;
    I32_WMV iXS = *piXS;
    I32_WMV iYS = *piYS;
    if (abs(iXO) < pWMVDec->m_iMaxZone1ScaledFarBackMVX)
        iXS = iXO * pWMVDec->m_iFarFieldScale1 >> 8;
    else {
        if (iXO < 0)
            iXS = -pWMVDec->m_iZone1OffsetScaledFarBackMVX + ((iXO * pWMVDec->m_iFarFieldScaleBack2) >> 8);
        else
            iXS = pWMVDec->m_iZone1OffsetScaledFarBackMVX + ((iXO * pWMVDec->m_iFarFieldScaleBack2) >> 8);
    }
    if (bScaleForSameField) {
        if (pWMVDec->m_iCurrentField == 0) {
            iYFO = (iYO - 1) >> 1;
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarBackMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScaleBack1) >> 8) * 2;
            else {
                if (iYFO < 0)
                    iYS = (((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarBackMVY) * 2;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarBackMVY + ((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8)) * 2;
            }
        }
        else {
            iYFO = (iYO + 1) >> 1;
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarBackMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScaleBack1) >> 8) * 2;
            else {
                if (iYFO < 0)
                    iYS = (((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarBackMVY) * 2;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarBackMVY + ((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8)) * 2;
            }
        }
        if (iYS > pWMVDec->m_iYMVRange - 2)                          
            iYS = pWMVDec->m_iYMVRange - 2;                          
        if (iYS < -pWMVDec->m_iYMVRange)                             
            iYS = -pWMVDec->m_iYMVRange;

    }
    else {
        iYFO = iYO >> 1;
        if (pWMVDec->m_iCurrentField == 0) {
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarBackMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScaleBack1) >> 8) * 2 + 1;
            else {
                if (iYFO < 0)
                    iYS = (((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarBackMVY) * 2 + 1;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarBackMVY + ((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8)) * 2 + 1;
            }
        }
        else {
            if (abs (iYFO) < pWMVDec->m_iMaxZone1ScaledFarBackMVY)
                iYS = ((iYFO * pWMVDec->m_iFarFieldScaleBack1) >> 8) * 2 - 1;
            else {
                if (iYFO < 0)
                    iYS = (((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8) - pWMVDec->m_iZone1OffsetScaledFarBackMVY) * 2 - 1;
                else
                    iYS = (pWMVDec->m_iZone1OffsetScaledFarBackMVY + ((iYFO * pWMVDec->m_iFarFieldScaleBack2) >> 8)) * 2 - 1;
            }
        }
        if (iYS > pWMVDec->m_iYMVRange - 1)                          
            iYS = pWMVDec->m_iYMVRange - 1;                          
        if (iYS < -pWMVDec->m_iYMVRange + 1)                             
            iYS = -pWMVDec->m_iYMVRange + 1;
    }
                    
    if (iXS > pWMVDec->m_iXMVRange - 1)                          
        iXS = pWMVDec->m_iXMVRange - 1;                          
    if (iXS < -pWMVDec->m_iXMVRange)                             
        iXS = -pWMVDec->m_iXMVRange;                             
    
    *piXS =  iXS;
    *piYS =  iYS;
}

Void_WMV ScaleNearPredBackward(tWMVDecInternalMember * pWMVDec, I32_WMV iXS, I32_WMV iYS, I32_WMV* piXO, I32_WMV * piYO, Bool_WMV bScaleForOppField)
{
    I32_WMV iYFS;
    I32_WMV iXO = *piXO;
    I32_WMV iYO = *piYO;
    iXO = iXS * pWMVDec->m_iNearFieldScaleBack >> 8;
    if (bScaleForOppField) {
        iYFS = iYS  >> 1;
        if (pWMVDec->m_iCurrentField == 0)              
            iYO = ((iYS >> 1) * pWMVDec->m_iNearFieldScaleBack >> 8) * 2 + 1;          
        else                                   
            iYO = ((iYS >> 1) * pWMVDec->m_iNearFieldScaleBack >> 8) * 2 - 1;
    }
    else {
        if (pWMVDec->m_iCurrentField == 0) {
            iYFS = (iYS - 1)  >> 1;
            iYO = ((iYS >> 1) * pWMVDec->m_iNearFieldScaleBack >> 8) * 2;
        }          
        else {
            iYFS = (iYS + 1)  >> 1;
            iYO = ((iYS >> 1) * pWMVDec->m_iNearFieldScaleBack >> 8) * 2;
        }
    }

    *piXO =  iXO;
    *piYO =  iYO;
}


Void_WMV PredictFieldModeMVEx (tWMVDecInternalMember * pWMVDec,
    CMVPred *pMVPred,
    I32_WMV iBlkX, I32_WMV iBlkY,
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry, Bool_WMV bHalfPelMode)
{
    // find motion predictor
    //I32_WMV iX1 = iBlkX * 8 * 4;
    //I32_WMV iY1 = iBlkY * 16 * 4 + pWMVDec->m_iCurrentField * 4;
    I32_WMV iX, iY;
    I32_WMV iXS = 0, iYS = 0, iXO = 0, iYO = 1 - 2 * pWMVDec->m_iCurrentField; //zou 328 delete

    I32_WMV iRes;
   
    I32_WMV iIndex  = iBlkY * pWMVDec->m_uintNumMBX * 2 + iBlkX;  // Index to block
    I32_WMV iMinX ;  // -15 pixels
    I32_WMV iMaxX ;  // x8 for block x4 for qpel
    I32_WMV iSameFieldX [3] = {0}, iSameFieldY [3] = {0};
    I32_WMV iOppFieldX [3] = {0}, iOppFieldY [3] = {0};
    I32_WMV iSameFieldCount = 0, iOppFieldCount = 0, iCount = 0;
    Bool_WMV bValidLeft = FALSE, bValidTop = FALSE, bValidTopRight = FALSE;
    I32_WMV iLeftSameHybridX =0, iLeftSameHybridY=0, iTopSameHybridX=0, iTopSameHybridY=0;
    I32_WMV iLeftOppHybridX=0, iLeftOppHybridY=0, iTopOppHybridX=0, iTopOppHybridY=0;
    Bool_WMV bSameFieldHybridMV = FALSE, bOppFieldHybridMV = FALSE;

     if (bHalfPelMode)
        iRes = 2;
    else
        iRes = 4;

    iMinX = -15 * iRes;
    iMaxX = pWMVDec->m_uintNumMBX * 2 * 8 * iRes - iRes;

    iOppFieldY [2] = 1 - 2 * pWMVDec->m_iCurrentField;   // In case we only have 2 valid opposite field Y predictors we need to set the 
                                                // 3rd one to a default odd value. Otherwise the median operation could produce an even value.
                                              
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
                if (iX != IBLOCKMV) {
                    if (iY & 1) {
                        iXO = iX;
                        iYO = iY;
                        iOppFieldCount ++;
                    }
                    else {
                        iXS = iX;
                        iYS = iY;
                        iSameFieldCount ++;
                    }
                }
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

        if (bIntraLeft + bIntraTop + bIntraTopRight != 3) {

            if (bValidLeft && !bIntraLeft) {
                if (iLeftY & 1) {
                    iLeftOppHybridX = iOppFieldX [iCount] = iLeftX;
                    iLeftOppHybridY = iOppFieldY [iCount] = iLeftY;
					ScaleFarPred (pWMVDec, iLeftX, iLeftY, &iXS, &iYS, TRUE);
                    iLeftSameHybridX = iSameFieldX [iCount] = iXS;
                    iLeftSameHybridY = iSameFieldY [iCount++] = iYS;
                    iOppFieldCount++;
                }
                else {
                    iLeftSameHybridX = iSameFieldX [iCount] = iLeftX;
                    iLeftSameHybridY = iSameFieldY [iCount] = iLeftY;
					ScaleNearPred (pWMVDec, iLeftX, iLeftY, &iXO, &iYO, TRUE);
                    iLeftOppHybridX = iOppFieldX [iCount] = iXO;
                    iLeftOppHybridY = iOppFieldY [iCount++] = iYO;
                    iSameFieldCount++;
                }
            }
            if (bValidTop && !bIntraTop) {
                if (iTopY & 1) {
                    iTopOppHybridX = iOppFieldX [iCount] = iTopX;
                    iTopOppHybridY = iOppFieldY [iCount] = iTopY;
					ScaleFarPred (pWMVDec, iTopX, iTopY, &iXS, &iYS, TRUE);
                    iTopSameHybridX = iSameFieldX [iCount] = iXS;
                    iTopSameHybridY = iSameFieldY [iCount++] = iYS;
                    iOppFieldCount++;
                }
                else {
                    iTopSameHybridX = iSameFieldX [iCount] = iTopX;
                    iTopSameHybridY = iSameFieldY [iCount] = iTopY;
					ScaleNearPred (pWMVDec, iTopX, iTopY, &iXO, &iYO, TRUE);
                    iTopOppHybridX = iOppFieldX [iCount] = iXO;
                    iTopOppHybridY = iOppFieldY [iCount++] = iYO;
                    iSameFieldCount++;
                }
            }
            if (bValidTopRight && !bIntraTopRight) {
                if (iTopRightY & 1) {
                    iOppFieldX [iCount] = iTopRightX;
                    iOppFieldY [iCount] = iTopRightY;
					ScaleFarPred (pWMVDec, iTopRightX, iTopRightY, &iXS, &iYS, TRUE);
                    iSameFieldX [iCount] = iXS;
                    iSameFieldY [iCount++] = iYS;
                    iOppFieldCount++;
                }
                else {
                    iSameFieldX [iCount] = iTopRightX;
                    iSameFieldY [iCount] = iTopRightY;
					ScaleNearPred (pWMVDec, iTopRightX, iTopRightY, &iXO, &iYO, TRUE);
                    iOppFieldX [iCount] = iXO;
                    iOppFieldY [iCount++] = iYO;
                    iSameFieldCount++;
                }
            }
            if (iCount > 1) {
                iXS = medianof3 (iSameFieldX [0], iSameFieldX [1], iSameFieldX [2]);
                iYS = medianof3 (iSameFieldY [0], iSameFieldY [1], iSameFieldY [2]);
                iXO = medianof3 (iOppFieldX [0], iOppFieldX [1], iOppFieldX [2]);
                iYO = medianof3 (iOppFieldY [0], iOppFieldY [1], iOppFieldY [2]);
            }
            else if (iCount == 1) {
                iXS = iSameFieldX [0];
                iYS = iSameFieldY [0];
                iXO = iOppFieldX [0];
                iYO = iOppFieldY [0];
            }
            else {
                iXS = 0;
                iYS = 0;
                iXO = 0;
                iYO = 1 - 2 * pWMVDec->m_iCurrentField;
            }
            // Decide whether to use hybrid predictor
            if (pWMVDec->m_tFrmType != BVOP && bValidLeft && !bIntraLeft && bValidTop && !bIntraTop) {
               if (!bHalfPelMode) {    // Qpel resolution
                    // Decide for same field
                    if ((abs (iXS - iLeftSameHybridX) + abs ((iYS - iLeftSameHybridY) >> 1)) > 32)
                        bSameFieldHybridMV = TRUE;
                    else if ((abs (iXS - iTopSameHybridX) + abs ((iYS - iTopSameHybridY) >> 1)) > 32)
                        bSameFieldHybridMV = TRUE;

                    // Decide for opposite field
                    if ((abs (iXO - iLeftOppHybridX) + abs ((iYO - iLeftOppHybridY) >> 1)) > 32)
                        bOppFieldHybridMV = TRUE;
                    else if ((abs (iXO - iTopOppHybridX) + abs ((iYO - iTopOppHybridY) >> 1)) > 32)
                        bOppFieldHybridMV = TRUE;
                }
                else {  // HPel resolution
                    // Decide for same field
                    if ((abs ((iXS - iLeftSameHybridX) << 1) + abs (iYS - iLeftSameHybridY)) > 32)
                        bSameFieldHybridMV = TRUE;
                    else if ((abs ((iXS - iTopSameHybridX) << 1) + abs (iYS - iTopSameHybridY)) > 32)
                        bSameFieldHybridMV = TRUE;

                    // Decide for opposite field
                    if (((abs (iXO - iLeftOppHybridX) << 1) + abs (iYO - iLeftOppHybridY)) > 32)
                        bOppFieldHybridMV = TRUE;
                    else if (((abs (iXO - iTopOppHybridX) << 1) + abs (iYO - iTopOppHybridY)) > 32)
                        bOppFieldHybridMV = TRUE;
                }

            }
        }

End:;
    } else if (iBlkX) {
        // left predictor
        iY = pFieldMvY[iIndex - 1];
        iX = pFieldMvX[iIndex - 1];

        if (iX != IBLOCKMV) {
            if (iY & 1) {
                iXO = iX;
                iYO = iY;

                ScaleFarPred (pWMVDec, iXO, iYO, &iXS, &iYS, TRUE);

                iOppFieldCount ++;
            }
            else {
                iXS = iX;
                iYS = iY;

                ScaleNearPred (pWMVDec, iXS, iYS, &iXO, &iYO, TRUE);

                iSameFieldCount ++;
            }
        }
    } 

    pMVPred->bSameFieldHybridMV = bSameFieldHybridMV;
    pMVPred->bOppFieldHybridMV = bOppFieldHybridMV;
    if (bSameFieldHybridMV) {
        pMVPred->iPredSameX1 = iLeftSameHybridX;
        pMVPred->iPredSameY1 = iLeftSameHybridY;
        pMVPred->iPredSameX2 = iTopSameHybridX;
        pMVPred->iPredSameY2 = iTopSameHybridY;
    }
    else {
        pMVPred->iPredSameX1 = iXS;
        pMVPred->iPredSameY1 = iYS;
    }
    if (bOppFieldHybridMV) {
        pMVPred->iPredOppX1 = iLeftOppHybridX;
        pMVPred->iPredOppY1 = iLeftOppHybridY;
        pMVPred->iPredOppX2 = iTopOppHybridX;
        pMVPred->iPredOppY2 = iTopOppHybridY;
    }
    else {
        pMVPred->iPredOppX1 = iXO;
        pMVPred->iPredOppY1 = iYO;
    }
    if (pWMVDec->m_bTwoRefPictures) {
        if (iSameFieldCount > iOppFieldCount)
            pMVPred->iPolarity = 0;
        else
            pMVPred->iPolarity = 1;
    }
        else {
        if (pWMVDec->m_bUseOppFieldForRef)
            pMVPred->iPolarity = 1;
        else
            pMVPred->iPolarity = 0;
    }
}


/****************************************************************************************
  ComputeMVFromDiffMVFieldPicture : compute motion vector from differential MV
****************************************************************************************/
Void_WMV ComputeMVFromDiffMVFieldPicture (tWMVDecInternalMember * pWMVDec,
    I32_WMV  iBlockX, I32_WMV iBlockY, I32_WMV iPredMVX, I32_WMV iPredMVY, CDiffMV *pInDiffMV)
{
    I32_WMV  iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV  k = iBlockY * iXBlocks + iBlockX;

	/*
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR) {
        pInDiffMV->iX <<= 1;
        pInDiffMV->iY <<= 1;
    }
	*/

    pWMVDec->m_pXMotion[k] = (I16_WMV)(((pInDiffMV->iX + iPredMVX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange); //zou 325  /* (dmv_y + predictor_y) smod range_y */
    pWMVDec->m_pYMotion[k] = (I16_WMV)(((pInDiffMV->iY + iPredMVY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange); //zou 325

}

/****************************************************************************************
  MotionCompFieldMode1MV : macroblock motion compensation for 1MV field mode
****************************************************************************************/
Void_WMV MotionCompFieldMode1MV (tWMVDecInternalMember * pWMVDec,
    U8_WMV *pDstY, U8_WMV *pDstU, U8_WMV *pDstV,
    U8_WMV *pRefY, U8_WMV *pRefU, U8_WMV *pRefV,
    I32_WMV imbX, I32_WMV imbY, I16_WMV *MVx, I16_WMV* MVy)
{
    if (
        (pWMVDec->m_tFrmType == BVOP && !pWMVDec->m_bFieldMode) ||  // only case where this is called for B frame is when IMB
        MVx[0] == IBLOCKMV) {  // if I block

        I32_WMV ii;

        for ( ii = 0; ii < 8; ii++) {
            memset (pDstY, 128, 16);
            memset (pDstY + pWMVDec->m_iWidthPrevY, 128, 16);
            pDstY += pWMVDec->m_iWidthPrevY * 2;
            memset (pDstU, 128, 8);
            memset (pDstV, 128, 8);
            pDstU += pWMVDec->m_iWidthPrevUV;
            pDstV += pWMVDec->m_iWidthPrevUV;
        }
        pWMVDec->m_pXMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = IBLOCKMV;
        pWMVDec->m_pYMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = IBLOCKMV;
    }
    else {  // Inter MB
        I32_WMV  iShiftX = MVx[0];
        I32_WMV  iShiftY = MVy[0];

		U8_WMV *pSourY,*pSourU,*pSourV ;//= pRefY -12;

        PullBackFieldModeMotionVector (pWMVDec, &iShiftX, &iShiftY, imbX, imbY);
      if(pWMVDec->m_bCodecIsWVC1){
		iShiftY = (iShiftY>>1);// (iShiftY>>1) + (iShiftY&1);//(iShiftY+1)/2;  //zou 325 pWMVDec->vo_ppxliRefYPlusExp
		pSourY = pRefY + (pWMVDec->m_iWidthPrevY) * (iShiftY >> 2) + (iShiftX >> 2);  //zou 325
		InterpolateMB (pWMVDec, pSourY,
           pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, iShiftX & 3, iShiftY& 3, pWMVDec->m_iFilterType);
      } else {
        InterpolateMB (pWMVDec, pRefY + (pWMVDec->m_iWidthPrevY >> 1) * (iShiftY >> 2) + (iShiftX >> 2),
            pWMVDec->m_iWidthPrevY, pDstY, pWMVDec->m_iWidthPrevY, iShiftX & 3, iShiftY & 3, pWMVDec->m_iFilterType);
      }
        // Chroma channels        
        ChromaMVFieldMode (pWMVDec,MVx, MVy, imbX, imbY, TRUE, &iShiftX, &iShiftY);
        pWMVDec->m_pXMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = (I16_WMV)iShiftX;
        pWMVDec->m_pYMotionC[imbY * pWMVDec->m_uintNumMBX + imbX] = (I16_WMV)iShiftY;

      if(pWMVDec->m_bCodecIsWVC1){
		pSourU = pRefU + pWMVDec->m_iWidthPrevUV * (iShiftY >> 2) + (iShiftX >> 2);
        InterpolateBlock(pWMVDec,pSourU , 
            pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);

		pSourV = pRefV + pWMVDec->m_iWidthPrevUV * (iShiftY >> 2) + (iShiftX >> 2);
        InterpolateBlock(pWMVDec, pSourV, 
            pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);
      } else {
        InterpolateBlock(pWMVDec, pRefU + pWMVDec->m_iWidthPrevUV * (iShiftY >> 2) + (iShiftX >> 2), 
            pWMVDec->m_iWidthPrevUV, pDstU, pWMVDec->m_iWidthPrevUV, iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);
        InterpolateBlock(pWMVDec, pRefV + pWMVDec->m_iWidthPrevUV * (iShiftY >> 2) + (iShiftX >> 2), 
            pWMVDec->m_iWidthPrevUV, pDstV, pWMVDec->m_iWidthPrevUV, iShiftX & 3, iShiftY & 3, FILTER_BILINEAR);
      }
    }
}

static I32_WMV s_RndTbl[] = { 0, 0, 0, 1 };

I32_WMV ChromaMVFieldMode (tWMVDecInternalMember * pWMVDec,
    I16_WMV* piMVx, I16_WMV* piMVy, 
    I32_WMV imbX, I32_WMV imbY, Bool_WMV b1MV, 
    I32_WMV * piMVx2, I32_WMV *piMVy2)
{
    I32_WMV  iX0 =0, iY0 =0, iX =0, iY =0;
    I32_WMV iMVx = *piMVx2;
    I32_WMV iMVy = *piMVy2;

    if (b1MV) {
        // 1MV case
        iX = piMVx[0];
      if(pWMVDec->m_bCodecIsWVC1){
        iY = (piMVy[0])>>1; //zou 325
      } else {
        iY = piMVy[0];
      }
    }
    else {
        // 4MV case
        // count I blocks
        I32_WMV  iLMVx[4], iLMVy[4], iType[4], iIntraBlocks = 0, iSameFieldCount = 0, iOppFieldCount, i;

      if(pWMVDec->m_bCodecIsWVC1){
        for (i = 0; i < 4; i++) {
            iLMVx[i] = piMVx[i];
            iLMVy[i] = piMVy[i]>>1;
            if (iLMVx[i] == IBLOCKMV) {
                iIntraBlocks ++;  //zou 328
                iType[i] = 1;
            }
            else
                iType[i] = ((iLMVy[i]>>1) & 4); //zou  328
        }
      } else {
        for (i = 0; i < 4; i++) {
            iLMVx[i] = piMVx[i];
            iLMVy[i] = piMVy[i];
            if (iLMVx[i] == IBLOCKMV) {
                iIntraBlocks ++;
                iType[i] = 1;
            }
            else
                iType[i] = (iLMVy[i] & 4);
        }
       }
          
        if (iIntraBlocks > 2) {
            iX = IBLOCKMV;
            iY = IBLOCKMV;
        }
        else {
            // Use the motion vectors from the most prevalent field
            for (i = 0; i < 4; i++) {
                if (iType[i] == 0)
                    iSameFieldCount ++;
            }
            iOppFieldCount = 4 - iSameFieldCount - iIntraBlocks;

            if (iOppFieldCount > iSameFieldCount) {
                // Use opposite field as chroma reference
                for (i = 0; i < 4; i++) {
                    if (iType[i] == 0) {
                        iLMVx[i] = IBLOCKMV;
                        iIntraBlocks ++;
                    }
                }
            }
            else {
                // Use same field as chroma reference
                for (i = 0; i < 4; i++) {
                    if (iType[i] == 4) {
                        iLMVx[i] = IBLOCKMV;
                        iIntraBlocks ++;
                    }
                }
            }

            if (iIntraBlocks == 1) {
                if (iLMVx[0] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[3], iLMVx[1], iLMVx[2]);
                    iY = medianof3 (iLMVy[3], iLMVy[1], iLMVy[2]);
                }
                else if (iLMVx[1] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[3], iLMVx[0], iLMVx[2]);
                    iY = medianof3 (iLMVy[3], iLMVy[0], iLMVy[2]);
                }
                else if (iLMVx[2] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[3], iLMVx[1], iLMVx[0]);
                    iY = medianof3 (iLMVy[3], iLMVy[1], iLMVy[0]);
                }
                else if (iLMVx[3] == IBLOCKMV) {
                    iX = medianof3 (iLMVx[0], iLMVx[1], iLMVx[2]);
                    iY = medianof3 (iLMVy[0], iLMVy[1], iLMVy[2]);
                }
            }
            else if (iIntraBlocks == 2) {
                iX = iY = 0;
                if (iLMVx[0] != IBLOCKMV) {
                    iX += iLMVx[0];
                    iY += iLMVy[0];
                }
                if (iLMVx[1] != IBLOCKMV) {
                    iX += iLMVx[1];
                    iY += iLMVy[1];
                }
                if (iLMVx[2] != IBLOCKMV) {
                    iX += iLMVx[2];
                    iY += iLMVy[2];
                }
                if (iLMVx[3] != IBLOCKMV) {
                    iX += iLMVx[3];
                    iY += iLMVy[3];
                }
                iX /= 2;
                iY /= 2;
            }
            else if (iIntraBlocks == 3) {
                if (iLMVx[0] != IBLOCKMV) {
                    iX = iLMVx[0];
                    iY = iLMVy[0];
                }
                else if (iLMVx[1] != IBLOCKMV) {
                    iX = iLMVx[1];
                    iY = iLMVy[1];
                }
                else if (iLMVx[2] != IBLOCKMV) {
                    iX = iLMVx[2];
                    iY = iLMVy[2];
                }
                else {
                    iX = iLMVx[3];
                    iY = iLMVy[3];
                }
            }
            else {
                iX = medianof4 (iLMVx[0], iLMVx[1], iLMVx[2], iLMVx[3]);
                iY = medianof4 (iLMVy[0], iLMVy[1], iLMVy[2], iLMVy[3]);
            }
        }
    }

    if (iX != IBLOCKMV) {

      if(!pWMVDec->m_bCodecIsWVC1){
        if (iY & 4) {
            if (pWMVDec->m_iCurrentField == 0)
                iY = (((iY - 4) >> 3) * 4) | (iY & 3);
            else
                iY = (((iY + 4) >> 3) * 4) | (iY & 3);
        }
        else 
            iY = ((iY >> 3) * 4) | (iY & 3);
       }

        iX = (iX + s_RndTbl[iX & 3]) >> 1;
        iY = (iY + s_RndTbl[iY & 3]) >> 1;
        if (pWMVDec->m_bUVHpelBilinear)
        {
            if (iX & 1) {
                if (iX > 0) iX--;
                else iX++;
            }
            if (iY & 1) {
                if (iY > 0) iY--;
                else iY++;
            }
        }

        iX0 = imbX * 8 + (iX >> 2);
        iY0 = imbY * 8 + (iY >> 2);
        if (iX0 < -8)
            iX -= (iX0 + 8) * 4;
        else if (iX0 > (I32_WMV) pWMVDec->m_uintNumMBX * 8)
            iX -= (iX0 - (I32_WMV) pWMVDec->m_uintNumMBX * 8) * 4;
        if (iY0 < -8)
            iY -= (iY0 + 8) * 4;
        else if (iY0 > (I32_WMV) pWMVDec->m_uintNumMBY * 8)
            iY -= (iY0 - (I32_WMV) pWMVDec->m_uintNumMBY * 8) * 4;
    }

    iMVx = iX;
    iMVy = iY;




    *piMVx2 =  iMVx;
    *piMVy2 =  iMVy;

    return (iMVx == IBLOCKMV);
}

/****************************************************************************************************
  DecodeMBFieldMode1MV : 1MV Field Mode macroblock decode
****************************************************************************************************/
I32_WMV DecodeMBFieldMode1MV (tWMVDecInternalMember * pWMVDec,
    CWMVMBMode* pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB, U8_WMV __huge* ppxlcCurrQUMB, U8_WMV __huge* ppxlcCurrQVMB,
    U8_WMV __huge* ppxlcRefYMB, U8_WMV __huge* ppxlcRefUMB, U8_WMV __huge* ppxlcRefVMB,
    I32_WMV imbX, I32_WMV imbY, I16_WMV* MVx, I16_WMV* MVy
)
{
    //FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);
    U8_WMV* rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    Bool_WMV  bTop, bBottom, bLeft, bRight;
    DQuantDecParam *pDQ;
    
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;

    I32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    UnionBuffer __huge* ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    I32_WMV result;

    // Y-blocks first (4 blocks)

    I32_WMV iShift, iblk, iDirection;
    I16_WMV  *pIntra, pPredScaled [16];

    Bool_WMV bIntra = (pmbmd->m_chMBMode == MB_INTRA);
    I32_WMV iX = imbX, iY = imbY;
    Bool_WMV bSkipMB = !(imbX || imbY || bIntra);

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

    

    

    // motion comp
    MotionCompFieldMode1MV (pWMVDec,
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB,
        ppxlcRefYMB, ppxlcRefUMB, ppxlcRefVMB,
        imbX, imbY, MVx, MVy);

    for (iblk = 0; iblk < 4; iblk++) {
        Bool_WMV bResidual = rgCBP[iblk];
        iX = imbX * 2 + (iblk & 1);
        iY = imbY * 2 + ((iblk & 2) >> 1);
        
        bSkipMB &= !bResidual;

        if (bIntra) {
            Bool_WMV bDCACPredOn;
            I16_WMV *pPred = NULL;
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;

            pIntra = pWMVDec->m_pX9dct + (iX + iY * pWMVDec->m_uintNumMBX * 2) * 16;
            bDCACPredOn = decodeDCTPredictionY (pWMVDec, pmbmd, pIntra, iblk, iX, iY, 
                &iShift, &iDirection, pPredScaled);

            
            if (bDCACPredOn) pPred = pPredScaled;
            result = DecodeInverseIntraBlockX9 (pWMVDec, ppIntraDCTTableInfo_Dec, iblk, bResidual, pPred,
                iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE); //get the inverse transformed block

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
            if (g_bAddError)
#endif
            // overlapped transform copy
            if (pWMVDec->m_iOverlap & 1) {
                I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX * 16;
                I32_WMV i1;
                for ( i1 = 0; i1 < 8; i1++)
                    memcpy (pWMVDec->m_pIntraBlockRow0[iblk & 2] + imbX * 16 + (iblk & 1) * 8 + i1 * iWidth1,
                        ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
            }
            pWMVDec->m_pAddError (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
        }
        else if (rgCBP[iblk]) {
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec , pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }

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
                memset(ppxliErrorQMB, 0, 256);
                if (pWMVDec->m_bRTMContent) {
                    if (bFirstBlock || pmbmd->m_bBlkXformSwitchOn) {
                        rgCBP[iblk] = iSubblockPattern;
                        bTop = (rgCBP[iblk] & 2);
                        bBottom = (rgCBP[iblk] & 1);
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
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
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
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
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iblk] = (U8_WMV )((bLeft << 1) | bRight);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iblk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP[iblk] & 2;
                        bRight = rgCBP[iblk] & 1;
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
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
                I32_WMV i4x4Pattern;
                ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
                memset(ppxliErrorQMB, 0, 256);

                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec , pWMVDec->m_pbitstrmIn) + 1;
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

            if (pWMVDec->m_pInterIDCT_Dec == g_IDCTDec_WMV2_Inter) {
                I32_WMV j1;
                for( j1= 0; j1 <64; j1++)
                    ppxliErrorQMB->i16[j1] = (I16_WMV) ppxliErrorQMB->i32[j1];
            }

#ifdef _GENERATE_DXVA_DATA_
            if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
                dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
            if (g_bAddError)
#endif
            pWMVDec->m_pAddError (ppxlcCurrQYMB, ppxlcCurrQYMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevY, pWMVDec->m_pcClapTable);

            bFirstBlock = FALSE;
        }
        else
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;

        ppxlcCurrQYMB += ((iblk & 1) ? pWMVDec->m_iWidthPrevYxBlkMinusBlk : BLOCK_SIZE);
    }

    // UV
    pmbmd->m_rgcBlockXformMode[4] = pmbmd->m_rgcBlockXformMode[5] = XFORMMODE_8x8;

    iX = imbX;
    iY = imbY;

    // U-block
    if (bIntra) {
        I16_WMV *pPred = NULL;
        
        ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
        bSkipMB = FALSE;

        pIntra = pWMVDec->m_pX9dctU + (imbX + imbY * pWMVDec->m_uintNumMBX) * 16;

        if (decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, imbX, imbY, &iShift, &iDirection, pPredScaled))
            pPred = pPredScaled;

        result = DecodeInverseIntraBlockX9 (pWMVDec, ppInterDCTTableInfo_Dec, 4, rgCBP[4], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE);
        
        // overlapped transform copy
        if (pWMVDec->m_iOverlap & 1) {
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
            I32_WMV i1;
            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraMBRowU0[0] + imbX * 8 + i1 * iWidth1,
                    ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
#ifdef _GENERATE_DXVA_DATA_
        if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
            dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
        if (g_bAddError)
#endif
        pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);

        pIntra = pWMVDec->m_pX9dctV + (imbX + imbY * pWMVDec->m_uintNumMBX) * 16;

        pPred = (decodeDCTPredictionUV(pWMVDec, pmbmd, pIntra, imbX, imbY, &iShift, &iDirection, pPredScaled)) ?
            pPredScaled : NULL;

        result = DecodeInverseIntraBlockX9 (pWMVDec, ppInterDCTTableInfo_Dec, 5, rgCBP[5], pPred,
                    iDirection, iShift, pIntra, pmbmd, pDQ, ppxliErrorQMB, BLOCK_SIZE);

        // overlapped transform copy
        if (pWMVDec->m_iOverlap & 1) {
            I32_WMV  iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
            I32_WMV i1 ;
            for ( i1 = 0; i1 < 8; i1++)
                memcpy (pWMVDec->m_pIntraMBRowV0[0] + imbX * 8 + i1 * iWidth1,
                    ppxliErrorQMB->i16 + i1 * BLOCK_SIZE, 8 * sizeof(I16_WMV));
        }
#ifdef _GENERATE_DXVA_DATA_
        if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
            dxvaDumpBlockedResiduals(ppxliErrorQMB);
#endif
#ifdef _ANALYZEMC_
        if (g_bAddError)
#endif
        pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
    }
    else {
        I32_WMV iBlk;
        for ( iBlk = 4; iBlk < 6; iBlk++) if (rgCBP[iBlk]) {
            bSkipMB = FALSE;
            if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
                I32_WMV iIndex = Huffman_WMV_get(pWMVDec->m_pHufBlkXformTypeDec , pWMVDec->m_pbitstrmIn);
                iXformType = s_iBlkXformTypeLUT[iIndex];
                iSubblockPattern = s_iSubblkPatternLUT[iIndex];
            }

            pmbmd->m_rgcBlockXformMode[iBlk] =(I8_WMV)iXformType;

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
                        rgCBP[iBlk] = iSubblockPattern;
                        bTop = (rgCBP[iBlk] & 2);
                        bBottom = (rgCBP[iBlk] & 1);
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bTop << 1) | bBottom);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iBlk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bTop = (rgCBP[iBlk] & 2);
                        bBottom = (rgCBP[iBlk] & 1);
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
                                bBottom = FALSE;
                            else
                                bTop = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bTop << 1) | bBottom);
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
                        rgCBP[iBlk] = iSubblockPattern;
                        bLeft = rgCBP[iBlk] & 2;
                        bRight = rgCBP[iBlk] & 1;
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bLeft << 1) | bRight);
                    }
                }
                else {
                    if (bFirstBlock) {
                        rgCBP[iBlk] = (U8_WMV)pmbmd->m_iSubblockPattern;
                        bLeft = rgCBP[iBlk] & 2;
                        bRight = rgCBP[iBlk] & 1;
                    }
                    else {
                        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
                                bRight = FALSE;
                            else
                                bLeft = FALSE;
                        }
                        rgCBP[iBlk] = (U8_WMV)((bLeft << 1) | bRight);
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

                i4x4Pattern = Huffman_WMV_get(pWMVDec->m_pHuf4x4PatternDec , pWMVDec->m_pbitstrmIn) + 1;
                rgCBP[iBlk] = i4x4Pattern;

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
#ifdef _ANALYZEMC_
            if (g_bAddError) {
                if (iBlk == 4)
                    pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
                else
                    pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
            }
#else
            if (iBlk == 4)
                pWMVDec->m_pAddError(ppxlcCurrQUMB, ppxlcCurrQUMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
            else
                pWMVDec->m_pAddError(ppxlcCurrQVMB, ppxlcCurrQVMB, ppxliErrorQMB->i16, pWMVDec->m_iWidthPrevUV, pWMVDec->m_pcClapTable);
#endif
            bFirstBlock = FALSE;
        }
    }

    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = bSkipMB;

    return ICERR_OK;
}

#endif //PPCWMP

