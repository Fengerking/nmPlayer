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

#ifdef WIN32
void voDumpInterlanceYuv(tWMVDecInternalMember *pWMVDec,int nframes)
{
	unsigned char* Yplane = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
	unsigned char* Uplane = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
	unsigned char* Vplane = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    char filename[250];

    sprintf(filename,"E:/MyResource/Video/clips/WMV/new/%d.yuv",nframes);


	if(pWMVDec->m_nPframes == nframes && pWMVDec->m_tFrmType == PVOP)
	{
		U32_WMV mb_cols =0, mb_rows=0;
		for(mb_rows=0;mb_rows<pWMVDec->m_uintNumMBY;mb_rows++)
		{
			unsigned char* Y = Yplane + mb_rows* pWMVDec->m_iMBSizeXWidthPrevY ;
			unsigned char *U = Uplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;
			unsigned char *V = Vplane + mb_rows* pWMVDec->m_iBlkSizeXWidthPrevUV ;

			for(mb_cols=0;mb_cols<pWMVDec->m_uintNumMBX;mb_cols++)
			{
				FILE*fp=fopen(filename,"a");
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

//static I32_WMV s_pXformLUT[4] = {XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_4x8, XFORMMODE_4x4};

enum {FRAMEMV = 0, FRAMEMV_FIELD, FRAME4MV, FRAME4MV_FIELD, 
      FIELDMV, FIELDMV_FRAME, FIELD4MV, FIELD4MV_FRAME,
      INTRA_FRAME, INTRA_FIELD};

// Externs
extern const I32_WMV g_iStepRemap[31];
//extern  I32_WMV g_iStepsize;
//extern  I32_WMV g_iMultiResOn;
//extern  I32_WMV g_iRangeRedux;
//extern  I32_WMV g_iFading;

#ifndef ICERR_UNSUPPORTED
#define ICERR_UNSUPPORTED      -1L
#endif

tWMVDecodeStatus decodePInterlaceV2 (tWMVDecInternalMember * pWMVDec)
{
    I32_WMV result = ICERR_OK;
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

	//20130118
	if(pWMVDec->bUseRef0Process == 1 && pWMVDec->CpuNumber > 1)
	{
		ppxliRefY = pWMVDec->m_pfrmRef0Process->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
		ppxliRefU = pWMVDec->m_pfrmRef0Process->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		ppxliRefV = pWMVDec->m_pfrmRef0Process->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
		pWMVDec->m_ppxliRef0YPlusExp = ppxliRefY;
		pWMVDec->m_ppxliRef0U = pWMVDec->m_pfrmRef0Process->m_pucUPlane;
		pWMVDec->m_ppxliRef0V = pWMVDec->m_pfrmRef0Process->m_pucVPlane;
	 }

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

    // Set transform type huffman pointers
    SetTransformTypeHuffmanTable (pWMVDec, pWMVDec->m_iStepSize);
   
    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++)
    {
        U8_WMV *ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV *ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV *ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV *ppxliRefYMB = ppxliRefY;
        U8_WMV *ppxliRefUMB = ppxliRefU;
        U8_WMV *ppxliRefVMB = ppxliRefV;
        U32_WMV iBlk;

       // i/f(pWMVDec->m_bCodecIsWVC1)
		//	pWMVDec->m_nrows = imbY;

       if (pWMVDec->m_bSliceWMVA)
       {
            if (pWMVDec->m_pbStartOfSliceRow[imbY]) 
            {
                pWMVDec->m_uiCurrSlice++;
                if(BS_flush(pWMVDec->m_pbitstrmIn))
                    return WMV_Failed;
                result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                pWMVDec->m_bFirstEscCodeInFrame = TRUE;
				if (result != ICERR_OK)
                    return WMV_Failed;
            }
        }

        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++)
        {  
            Bool_WMV bCBPPresent, b1MVPresent;
            I32_WMV iTopLeftBlkIndex;
            I32_WMV iBotLeftBlkIndex;

            //if(pWMVDec->m_bCodecIsWVC1)
            //    pWMVDec->m_ncols = imbX;
                        
            DecodePMBModeInterlaceV2 (pWMVDec,pmbmd, &bCBPPresent, &b1MVPresent);

            if(pWMVDec->m_bCodecIsWVC1)
            {
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

            if (pmbmd->m_chMBMode == MB_INTRA) 
            {
                I32_WMV iCBPCY;
                pmbmd->m_dctMd = INTRA;   
                pmbmd->m_chFieldDctMode = (I8_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1);
                bCBPPresent = BS_getBits(pWMVDec->m_pbitstrmIn, 1);

                if (bCBPPresent) 
                    iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufICBPCYDec, pWMVDec->m_pbitstrmIn) + 1;  
                else
                    iCBPCY = 0;

                for (iBlk = 0; iBlk < 6; iBlk ++) 
                { // set CBP
                    pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    pmbmd->m_rgbCodedBlockPattern2 [5 - iBlk] = (iCBPCY & 0x1);
                    iCBPCY >>= 1;
                }

               if(pWMVDec->m_bCodecIsWVC1)
               {
                   pmbmd -> m_rgcIntraFlag[0] = pmbmd -> m_rgcIntraFlag[1] = 
                   pmbmd -> m_rgcIntraFlag[2] = pmbmd -> m_rgcIntraFlag[3] = 
                   pmbmd -> m_rgcIntraFlag[4] = pmbmd -> m_rgcIntraFlag[5] = INTRA;
               }
                // get AC Pred Flag
                pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);

                if (pWMVDec->m_bDQuantOn)
                    if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd))
                        return WMV_Failed;

                result = DecodeIMBInterlaceV2 (pWMVDec, imbY, imbX, pmbmd, 
                                                          ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                                                          ppxliErrorY, ppxliErrorU, ppxliErrorV);
                if (ICERR_OK != result) 
                    return WMV_Failed;
            }
            else 
            {
                result = decodePMBOverheadInterlaceV2 (pWMVDec, 
                                                                        pmbmd, imbX, imbY,
                                                                        bCBPPresent, b1MVPresent);
                if (ICERR_OK != result)
                    return WMV_Failed;

                result = DecodePMBInterlaceV2 (pWMVDec, imbX, imbY, pmbmd); 
                if (ICERR_OK != result)
                    return WMV_Failed;

                if (pmbmd->m_chMBMode == MB_1MV) 
                {
                    ComputeFrameMVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV); 
                    MotionCompFrameInterlaceV2 (pWMVDec, imbX, imbY, 
                                                            ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                                                            ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } 
                else if (pmbmd->m_chMBMode == MB_4MV)
                {                       
                    ComputeFrame4MVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompFrame4MvInterlaceV2 (pWMVDec, imbX, imbY, 
                                                                ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                                                                ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                }
                else if (pmbmd->m_chMBMode == MB_FIELD) 
                {
                    ComputeFieldMVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompFieldInterlaceV2 (pWMVDec, imbX, imbY, 
                                                           ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                                                            ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } 
                else  //MB_FIELD4MV
                {
                    ComputeField4MVFromDiffMVInterlaceV2 (pWMVDec, pmbmd, imbX, imbY, pWMVDec->m_pDiffMV);
                    MotionCompField4MvInterlaceV2 (pWMVDec, imbX, imbY, 
                                                                ppxliRefYMB, ppxliRefUMB, ppxliRefVMB, 
                                                                ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                }

                if (pmbmd->m_chFieldDctMode == FALSE) 
                {
                    AddErrorFrameInterlaceV2 (pWMVDec, 
                                                        pWMVDec->m_ppxliFieldMB, 
                                                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB, 
                                                        pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                }
                else 
                {
                    AddErrorFieldInterlaceV2 (pWMVDec,     
                                                      pWMVDec->m_ppxliFieldMB, 
                                                      ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
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


                if (pmbmd->m_chMBMode == MB_INTRA)
			    {
				    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = 
				    pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = 
				    pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = 
				    pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = IBLOCKMV;
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

	//voDumpInterlanceYuv(pWMVDec,pWMVDec->m_nPframes);

    if (pWMVDec->m_bLoopFilter ) {

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
 
    //pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;
    return WMV_Succeeded;
}

I32_WMV DecodePMBModeInterlaceV2 (tWMVDecInternalMember * pWMVDec,
                                  CWMVMBMode* pmbmd, Bool_WMV *pbCBPPresent, Bool_WMV *pb1MVPresent)
{
    I32_WMV result = ICERR_OK;
    I32_WMV iIndex;
    
    pmbmd->m_bOverlapIMB = FALSE;

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT            
    pmbmd->m_bBlkXformSwitchOn = FALSE;            
    pmbmd->m_rgbDCTCoefPredPattern2 [0] = 0;
    pmbmd->m_dctMd = INTER;
    pmbmd->m_chFieldDctMode = FALSE; 
    memset (pWMVDec->m_ppxliFieldMB, 0, 6 * 8 * 8 * sizeof(I16_WMV));
    if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    }

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
        //assert (0);
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

    if (pmbmd->m_chMBMode == MB_1MV) {//1 MV
        if (b1MVPresent) {
            decodeMV_Progressive(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
        }
    } else if (pmbmd->m_chMBMode == MB_FIELD) { //FIELD 1MV
        I32_WMV iMV = Huffman_WMV_get(pWMVDec->m_p2MVBP, pWMVDec->m_pbitstrmIn);
        if (iMV & 2) {
            decodeMV_Progressive(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV);
        }
        if (iMV & 1) {
            decodeMV_Progressive(pWMVDec,pWMVDec->m_pHufMVTable_V9, pWMVDec->m_pDiffMV + 2);
        }
    } else {  // 4 MV  || FIELD 4 MV
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
                    uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + 1);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + 1);
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
            result = DecodeInverseInterBlockQuantizeInterlaceV2 (pWMVDec, ppInterDCTTableInfo_Dec,
                                                                                   pWMVDec->m_pZigzagScanOrder, pDQ, 64);

            SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);

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

				SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);
             
				(*pWMVDec->m_pInter8x4IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4 * 8;
                
                memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
                result = DecodeInverseInterBlockQuantizeInterlaceV2  (pWMVDec,
                    ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, pDQ, 32);

				SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);

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

				SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);

                (*pWMVDec->m_pInter4x8IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, 
                    pWMVDec->m_rgiCoefReconBuf, 0);

            }
            if (bRight) {

                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4;
                memset (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
                result = DecodeInverseInterBlockQuantizeInterlaceV2 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, pDQ, 32); 

				SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);

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

					SignPatch((I16_WMV*)pWMVDec->m_rgiCoefReconBuf, 128);

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
    U32_WMV iBlk;
    Bool_WMV bFirstBlock = TRUE && pWMVDec->m_bMBXformSwitching;
        
    for (iBlk = 0; iBlk < 6; iBlk++) {
        I32_WMV result;
        result = DecodePBlockInterlaceV2 (pWMVDec,
                                                    imbX, imbY, iBlk, pmbmd,
                                                    pWMVDec->m_ppInterDCTTableInfo_Dec, 
                                                    pDQ, &bFirstBlock);
        if (ICERR_OK != result)
            return result;
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
                                            &dX, &dY, pWMVDec->m_pXMotion, 
                                            pWMVDec->m_pYMotion, bTopBndry);

    pWMVDec->m_pXMotion [k] = 
    pWMVDec->m_pXMotion [k + 1] = 
    pWMVDec->m_pXMotion [k + iXBlocks] = 
    pWMVDec->m_pXMotion [k + iXBlocks + 1] = (I16_WMV)(((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange);
   
    pWMVDec->m_pYMotion [k] = 
    pWMVDec->m_pYMotion [k + 1] = 
    pWMVDec->m_pYMotion [k + iXBlocks] = 
    pWMVDec->m_pYMotion [k + iXBlocks + 1] = (I16_WMV)(((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange);

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


//static I32_WMV gFineMV[] = {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7};
static I32_WMV aMVOffsets[] = {0,1,2,4,8,16,32,64,128};
static I32_WMV aMVOffsets2[] = {0,1,3,7,15,31,63,127,255};
#define XBINS   9
#define YBINS   14
#define MV_ESCAPE_INTERLACE     125
#define MV_ESCAPE_PROGRESSIVE   71

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
        pDiffMV->iX = BS_getBits(pWMVDec->m_pbitstrmIn, pWMVDec->m_iLogXRange );
        pDiffMV->iY = BS_getBits(pWMVDec->m_pbitstrmIn, pWMVDec->m_iLogYRange );
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

//static I32_WMV s_RndTbl[] = { 0, 0, 0, 1 };

