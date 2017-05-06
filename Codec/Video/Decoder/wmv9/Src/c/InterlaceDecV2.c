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
#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119                            // see table.13/H.263

// Mapping array for QP index to QP. Used for 5QP deadzone quantizer.
extern I32_WMV g_iStepRemap[31];
//extern  I32_WMV g_iStepsize;
//extern  I32_WMV g_iFading;

Void_WMV g_inverseIntraBlockQuantizeInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I16_WMV *piCoefRecon, I32_WMV iNumCoef, DQuantDecParam *pDQ);

#define MAXHALFQP           8
#define MAXHIGHRATEQP       8
#define MAX3QP              8
static I32_WMV s_pXformLUT[4] = {XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_4x8, XFORMMODE_4x4};

tWMVDecodeStatus decodeIInterlaceV2 (tWMVDecInternalMember * pWMVDec)
{
    I32_WMV result;
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    I16_WMV ppxliErrorY [16*16], ppxliErrorU [64], ppxliErrorV [64];
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    U32_WMV imbY, imbX, iBlk;
    Bool_WMV bFrameInPostBuf = FALSE;

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
    {
        pWMVDec->m_iDXVAOn = 1;
        dxvaStartFrame(pWMVDec->m_bLoopFilter, 0,TRUE);
    }
#endif

    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set [pWMVDec->m_rgiDCTACIntraTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[1] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set [pWMVDec->m_rgiDCTACIntraTableIndx[1]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[2] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set [pWMVDec->m_rgiDCTACIntraTableIndx[2]];
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_ppInterDCTTableInfo_Dec[1] = pWMVDec->m_ppInterDCTTableInfo_Dec [2] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];

    HUFFMANGET_DBG_HEADER(":decodeI411",11);
//    DECODE_FRAME_TYPE_COUNT_CPU_CYCLES(11);

    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
        U8_WMV* ppxliCodedY = ppxliCurrQY;
        U8_WMV* ppxliCodedU = ppxliCurrQU;
        U8_WMV* ppxliCodedV = ppxliCurrQV;

		if (pWMVDec->m_bSliceWMVA) {
           // if (pWMVDec->m_uiNumSlicesWMVA > 1 && pWMVDec->m_uiCurrSlice < pWMVDec->m_uiNumSlicesWMVA - 1) {
                if ( pWMVDec->m_pbStartOfSliceRow[imbY]) {
                    pWMVDec->m_uiCurrSlice++;
                    BS_flush(pWMVDec->m_pbitstrmIn);
                    result = ReadSliceHeaderWMVA (pWMVDec, imbY);
                    pWMVDec->m_bFirstEscCodeInFrame = TRUE;
					if (result != ICERR_OK)
                        return WMV_Failed;
                }
        }
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {

            //set up intra flags
            pmbmd->m_dctMd = INTRA;
            for (iBlk = 0; iBlk < 6; iBlk++) {
                pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
            }

            result = decodeIMBOverheadInterlaceV2 (pWMVDec,pmbmd, imbX, imbY);
            if (ICERR_OK != result) {
                return WMV_Failed;
            }

            result = DecodeIMBInterlaceV2 (pWMVDec,
                imbY, imbX, pmbmd, ppxliCodedY, ppxliCodedU, ppxliCodedV,
                ppxliErrorY, ppxliErrorU, ppxliErrorV); 
            if (ICERR_OK != result) {
                return WMV_Failed;
            }
#if 0
			if(0){
				//if(pWMVDec->m_nframes ==3)
				if(pWMVDec->m_nIframes == 2 && pWMVDec->m_tFrmType == IVOP)
				{
					FILE*fp=fopen("E:/MyResource/Video/clips/wmv test files/interfiled/vo_dump.txt","a");
					int i,j;
					fprintf(fp,"-----------------------%d %d -----------\n",imbY,imbX);
					for(i=0;i<16;i++)
					{
						for(j=0;j<16;j++)
						{
							fprintf(fp,"%x   ",ppxliCodedY[i*pWMVDec->m_iWidthInternal+j]);
						}
						fprintf(fp,"\n");
					}

					for(i=0;i<8;i++)
					{
						for(j=0;j<8;j++)
						{
							fprintf(fp,"%x   ",ppxliCodedU[i*pWMVDec->m_iWidthInternalUV+j]);
						}
						fprintf(fp,"\n");
					}

					for(i=0;i<8;i++)
					{
						for(j=0;j<8;j++)
						{
							fprintf(fp,"%x   ",ppxliCodedV[i*pWMVDec->m_iWidthInternalUV+j]);
						}
						fprintf(fp,"\n");
					}
					fclose(fp);
				}
			}
#endif

            ppxliCodedY += 16;
            ppxliCodedU += 8;
            ppxliCodedV += 8;
            pmbmd++;
        }
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; 
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV; 
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV; 
    }

#ifdef _GENERATE_DXVA_DATA_
    // Write out DXVA frame data to file 
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
    {
        dxvaEndFrame(pWMVDec->m_uintNumMBX, pWMVDec->m_uintNumMBY, TRUE);
        dxvaDumpIFrame();
        pWMVDec->m_iDXVAOn = 0;
    }
#endif

    if (pWMVDec->m_bLoopFilter
#ifdef _GENERATE_DXVA_DATA_
        && !pWMVDec->m_pDMO
#endif
        ) {
#if 0 //def _MultiThread_Decode_ 
        DecodeMultiThreads (LOOPFILTER_IFRAME_ROW_INTERLACEV2);
        DecodeMultiThreads (LOOPFILTER_IFRAME_COL_INTERLACEV2);
#else
        LoopFilterRowIFrameInterlaceV2 (pWMVDec,
            0, pWMVDec->m_uintNumMBY, 
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            pWMVDec->m_rgmbmd);

        LoopFilterColIFrameInterlaceV2 (pWMVDec,
            0, pWMVDec->m_uintNumMBY, 
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
            pWMVDec->m_rgmbmd);

#endif
    }

    if (!pWMVDec->m_bLoopFilter)
        memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);
   
    pWMVDec->m_bCopySkipMBToPostBuf = FALSE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame
    pWMVDec->m_iRefreshDisplay_AllMB_Cnt = 0;

    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;
    return WMV_Succeeded;
}

I32_WMV decodeIMBOverheadInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPCY; 
    //I32_WMV iPredCBP = 0;
    I32_WMV iBlk;

    if (pWMVDec->m_iFieldDctIFrameCodingMode == SKIP_RAW) {
        pmbmd->m_chFieldDctMode = (BS_getBits(pWMVDec->m_pbitstrmIn, 1) ? TRUE : FALSE);
    }

    iCBPCY = Huffman_WMV_get(&pWMVDec->m_hufICBPCYDec,pWMVDec->m_pbitstrmIn);
    if (iCBPCY < 0 || iCBPCY > 63 || BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }
    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = ((iCBPCY >> (5 - iBlk)) & 1);
    }
    decodePredictIntraCBP (pWMVDec, pmbmd, imbX, imbY);

    pmbmd->m_bSkip = FALSE;
    pmbmd->m_dctMd = INTRA;
    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT

    if (pWMVDec->m_iACPredIFrameCodingMode == SKIP_RAW) {
        pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    }

    if ((pWMVDec->m_iOverlap & 4) && pWMVDec->m_iOverlapIMBCodingMode == SKIP_RAW) {
        pmbmd->m_bOverlapIMB = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    }

    if (pWMVDec->m_bDQuantOn) {
        if (ICERR_OK != decodeDQuantParam (pWMVDec, pmbmd)) {
            return ICERR_ERROR;
        }
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }
    
    return ICERR_OK;
}

Void_WMV PerformACPredictionInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV iBlk, CWMVMBMode *pmbmd, I16_WMV *piLevelBlk, I16_WMV *rgiPredCoef,
    DCACPREDDIR iPredDir)
{
    I32_WMV i;
    if (iPredDir == TOP) {
        if (pWMVDec->m_bRotatedIdct) {
            for (i = 1; i < 8; i++) {
                piLevelBlk [i << 3] += rgiPredCoef [i];
            }
        } else {
            for (i = 1; i < 8; i++) {
                piLevelBlk [i] += rgiPredCoef [i];
            }
        }
    } else if (iPredDir == LEFT) {
        if (pWMVDec->m_bRotatedIdct) {
            for (i = 1; i < 8; i++) {
                piLevelBlk [i] += rgiPredCoef [i];
            }
        } else {
            for (i = 1; i < 8; i++) {
                piLevelBlk [i << 3] += rgiPredCoef [i];
            }
        }
    }
}

I32_WMV DecodeIntraBlockInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode *pmbmd, 
    CDCTTableInfo_Dec** ppDCTTableInfo_Dec, DQuantDecParam *pDQ,
    Bool_WMV bAcPredOn)
{
    I32_WMV result;
    I32_WMV iDCStepSize = pDQ->iDCStepSize; // used for Y and UV DC decode
    I16_WMV* rgiCoefRecon = (I16_WMV*) pWMVDec->m_rgiCoefReconBuf;
    I16_WMV *ppxliDst;
    DCACPREDDIR iPredDir = NONE;
    I16_WMV rgiPredCoef [8]; // for scaling
    U8_WMV* piZigzagInv = (U8_WMV*) pWMVDec->m_pZigzagScanOrder;

    memset (rgiCoefRecon, 0, 64 * sizeof(I16_WMV));

    // Decode DCT Coefs  
    if (iBlk < 4) {
        result = decodeIntraDCInterlaceV2 (pWMVDec, &rgiCoefRecon [0], 
            pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, iDCStepSize);    
    } else {
        result = decodeIntraDCInterlaceV2 (pWMVDec, &rgiCoefRecon [0], 
            pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, iDCStepSize);
    }

    if (ICERR_OK != result) {
        return result;
    }

    // AC/DC Prediction 
    decodeDCACPredInterlaceV2 (pWMVDec, imbY, imbX, iBlk, pmbmd,
        &iPredDir, rgiPredCoef);
    
    if (bAcPredOn) {
        if (iPredDir == TOP) {
            piZigzagInv = pWMVDec->m_pHorizontalZigzagScanOrder;
        } else if (iPredDir == LEFT) {
            piZigzagInv = pWMVDec->m_pVerticalZigzagScanOrder;
        } 
    }

    // for Y Blocks, Intra table is used.
    // for U,V Blocks, Inter table is used;
    if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
        result = DecodeInverseIntraBlockQuantizeInterlaceV2 (pWMVDec, 
            ppDCTTableInfo_Dec, piZigzagInv, rgiCoefRecon, 64);
    }
    if (ICERR_OK != result) {
        return result;
    }
    
    rgiCoefRecon [0] += rgiPredCoef [0];
    if (bAcPredOn) { // row
        PerformACPredictionInterlaceV2 (pWMVDec,
            iBlk, pmbmd, rgiCoefRecon, rgiPredCoef, iPredDir);
    } 
    StoreDCACPredCoefInterlaceV2 (pWMVDec, imbY, imbX, iBlk, rgiCoefRecon); 

    g_inverseIntraBlockQuantizeInterlaceV2 (pWMVDec, rgiCoefRecon, 64, pDQ);
    ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64; 

#ifdef _EMB_SSIMD_IDCT_
    if ( pWMVDec->m_b16bitXform) SignPatch(rgiCoefRecon, 128);
#endif

    (*pWMVDec->m_pInterIDCT_Dec)((UnionBuffer*)ppxliDst, (UnionBuffer*) rgiCoefRecon, 8, 255); 

    pWMVDec->m_pFieldBlockAdd128 (ppxliDst, 64);
    return ICERR_OK;
}

static Void_WMV g_InvOverlapVerticalEdge (I16_WMV *pOverlapBuffer, I32_WMV iOverlapBufferStride, I32_WMV iNumPixels)
{
    I32_WMV ii;
    I32_WMV iRnd = 1;
    for (ii = 0; ii < iNumPixels; ii++) {
        I16_WMV v0 = pOverlapBuffer[-2];
        I16_WMV v1 = pOverlapBuffer[-1];
        I16_WMV v2 = pOverlapBuffer[0];
        I16_WMV v3 = pOverlapBuffer[1];

        pOverlapBuffer[-2] = (I16_WMV) ((7 * v0 + v3 + iRnd + 3) >> 3);
        pOverlapBuffer[-1] = (I16_WMV) ((7 * v1 - v0 + v2 + v3 + 4 - iRnd) >> 3);
        pOverlapBuffer[0] = (I16_WMV) ((7 * v2 - v3 + v1 + v0 + iRnd + 3) >> 3);
        pOverlapBuffer[1] = (I16_WMV) ((7 * v3 + v0 + 4 - iRnd) >> 3);

        iRnd ^= 1;
        pOverlapBuffer += iOverlapBufferStride;
    }
}

Void_WMV InvOverlapInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode* pmbmd, 
    U8_WMV* ppxliCurrRecnMBY, U8_WMV* ppxliCurrRecnMBU, U8_WMV* ppxliCurrRecnMBV,
    I16_WMV *ppxliErrorQY, I16_WMV *ppxliErrorQU, I16_WMV *ppxliErrorQV,
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliIntraRowY, I16_WMV *ppxliIntraRowU, 
    I16_WMV *ppxliIntraRowV)
{
    // save I16_WMV macroblock
    I16_WMV *pOverlapBuffer;
    I32_WMV iOverlapBufferStride;
    I32_WMV ii;
    I32_WMV iOffset = 0;
    //I32_WMV iNumEdge = 0;
    //I32_WMV iRnd;
    I32_WMV jj;


    if (pmbmd->m_chFieldDctMode == FALSE) {
        CopyIntraFieldMBtoOverlapFrameMBInterlaceV2 (ppxliFieldMB, 
            ppxliErrorQY, ppxliErrorQU, ppxliErrorQV, 16, 8);
    } else {
        CopyIntraFieldMBtoOverlapFieldMBInterlaceV2 (ppxliFieldMB, 
            ppxliErrorQY, ppxliErrorQU, ppxliErrorQV, 16, 8);
    }

    
    iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 16;
    pOverlapBuffer = ppxliIntraRowY + imbX * 16;
    for (ii = 0; ii < 8; ii++) {
        memcpy (pOverlapBuffer + ii * iOverlapBufferStride,
            ppxliErrorQY + ii * 16, 16 * sizeof(I16_WMV));
    }
    pOverlapBuffer = ppxliIntraRowY + imbX * 16 + iOverlapBufferStride * 8;
    for (ii = 0; ii < 8; ii++) {
        memcpy (pOverlapBuffer + ii * iOverlapBufferStride,
            ppxliErrorQY + ii * 16 + 8*16, 16 * sizeof(I16_WMV));
    }

    iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 8;

    pOverlapBuffer = ppxliIntraRowU + imbX * 8;
    for (ii = 0; ii < 8; ii++) {
        memcpy (pOverlapBuffer + ii * iOverlapBufferStride,
            ppxliErrorQU + ii * 8, 8 * sizeof(I16_WMV));
    }
    pOverlapBuffer = ppxliIntraRowV + imbX * 8;
    for (ii = 0; ii < 8; ii++) {
        memcpy (pOverlapBuffer + ii * iOverlapBufferStride,
            ppxliErrorQV + ii * 8, 8 * sizeof(I16_WMV));
    }
    // ---------------------------

    // overlap smooth vert. edge
    
    if(pWMVDec->m_bCodecIsWVC1){
	if(pWMVDec->m_tFrmType == PVOP)//zou 321
	{
		if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) 
		{ // left MB boundary

			iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 16;
			pOverlapBuffer = ppxliIntraRowY + imbX * 16;
			if((pmbmd-1)->m_rgcIntraFlag[1] ==INTRA && (pmbmd)->m_rgcIntraFlag[0] ==INTRA)
			{				
				g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8);
			}
			if((pmbmd-1)->m_rgcIntraFlag[3] ==INTRA && (pmbmd)->m_rgcIntraFlag[2] ==INTRA)
			{
				g_InvOverlapVerticalEdge (pOverlapBuffer+8*iOverlapBufferStride, iOverlapBufferStride, 8);
			}

			//UV
			iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
			if((pmbmd-1)->m_rgcIntraFlag[4] ==INTRA && (pmbmd)->m_rgcIntraFlag[4] ==INTRA)
			{	
				pOverlapBuffer = ppxliIntraRowU + imbX * 8;
				g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8); 
			}
			if((pmbmd-1)->m_rgcIntraFlag[5] ==INTRA && (pmbmd)->m_rgcIntraFlag[5] ==INTRA)
			{
				pOverlapBuffer = ppxliIntraRowV + imbX * 8;
				g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8);
			}
		}

		if(pmbmd->m_bOverlapIMB)
		{
			iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 16;
			pOverlapBuffer = ppxliIntraRowY + imbX * 16 + 8;

			if(pmbmd->m_rgcIntraFlag[0] ==INTRA && pmbmd->m_rgcIntraFlag[1] ==INTRA)
			{
				g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8);  //zou 321  
			}

			if(pmbmd->m_rgcIntraFlag[2] ==INTRA && pmbmd->m_rgcIntraFlag[3] ==INTRA)
			{
				g_InvOverlapVerticalEdge (pOverlapBuffer+8*iOverlapBufferStride, iOverlapBufferStride, 8); 
			}
		}
	}
	else
	{
		iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 16; 
		if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) { // left MB boundary
			pOverlapBuffer = ppxliIntraRowY + imbX * 16;
			g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8);  //zou 321
			g_InvOverlapVerticalEdge (pOverlapBuffer+8*iOverlapBufferStride, iOverlapBufferStride, 8);
		}

		pOverlapBuffer = ppxliIntraRowY + imbX * 16 + 8;
		g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8);  //zou 321
		g_InvOverlapVerticalEdge (pOverlapBuffer+8*iOverlapBufferStride, iOverlapBufferStride, 8);   
	
		// U,V
		iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
		if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) {
			pOverlapBuffer = ppxliIntraRowU + imbX * 8;
			g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8); 
		}

		if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) {
			pOverlapBuffer = ppxliIntraRowV + imbX * 8;
			g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8);
		}        
	}
    }
    else {
    iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 16;
    if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) { // left MB boundary
       // I32_WMV ii;
        pOverlapBuffer = ppxliIntraRowY + imbX * 16;

        g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 16);
    }

    pOverlapBuffer = ppxliIntraRowY + imbX * 16 + 8;
    g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 16);   



    // U,V
    iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
    if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) {
        pOverlapBuffer = ppxliIntraRowU + imbX * 8;
        g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8); 
    }

    if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) {
        pOverlapBuffer = ppxliIntraRowV + imbX * 8;
        g_InvOverlapVerticalEdge (pOverlapBuffer, iOverlapBufferStride, 8);

    }       
    } 

    // copy to recon
#define CLIP(x) ((U32_WMV) x <= 255 ? x : (x < 0 ? 0: 255))
    iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 16;
    
    iOffset = 0;
    if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) {
        iOffset = -2;
    }
    pOverlapBuffer = ppxliIntraRowY + imbX * 16;
    for (ii = 0; ii < 16; ii++) {
        for (jj = iOffset; jj < 16; jj++) { 
            ppxliCurrRecnMBY [ii * pWMVDec->m_iWidthPrevY + jj] = 
                CLIP (pOverlapBuffer [ii * iOverlapBufferStride + jj]);
        }
    }

    //U,V
    iOffset = 0;
    if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) {
        iOffset = -2;
    }
    iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
    pOverlapBuffer = ppxliIntraRowU + imbX * 8;
    for (ii = 0; ii < 8; ii++) {
        for (jj = iOffset; jj < 8; jj++) { 
            ppxliCurrRecnMBU [ii * pWMVDec->m_iWidthPrevUV + jj] = 
                CLIP (pOverlapBuffer [ii * iOverlapBufferStride + jj]);
        }
    }


    iOffset = 0;
    if (imbX != 0 && (pmbmd-1)->m_bOverlapIMB) {
        iOffset = -2;
    }
    iOverlapBufferStride = (I32_WMV) pWMVDec->m_uintNumMBX * 8;
    pOverlapBuffer = ppxliIntraRowV + imbX * 8;
    for (ii = 0; ii < 8; ii++) {
        for (jj = iOffset; jj < 8; jj++) { 
            ppxliCurrRecnMBV [ii * pWMVDec->m_iWidthPrevUV + jj] = 
                CLIP (pOverlapBuffer [ii * iOverlapBufferStride + jj]);
        }
    }
}

I32_WMV DecodeIMBInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV, I16_WMV *ppxliErrorY,
    I16_WMV *ppxliErrorU, I16_WMV *ppxliErrorV)
{
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];
    I32_WMV result;
    //I32_WMV *rgiCoefRecon = pWMVDec->m_rgiCoefRecon; 
    U32_WMV iBlk;
    Bool_WMV bAcPredOn = pmbmd->m_rgbDCTCoefPredPattern2 [0]; 
    
    memset (pWMVDec->m_ppxliFieldMB, 0, 6 * 8 * 8 * sizeof(I16_WMV)); // clear field mb
           
    for (iBlk = 0; iBlk < 4; iBlk++) {
        result = DecodeIntraBlockInterlaceV2 (pWMVDec,imbY, imbX, iBlk, pmbmd,
            pWMVDec->m_ppIntraDCTTableInfo_Dec, pDQ, bAcPredOn);
        if (ICERR_OK != result) {
            return ICERR_ERROR;
        }
    }

    for (iBlk = 4; iBlk < 6; iBlk++) {
        DecodeIntraBlockInterlaceV2 (pWMVDec,imbY, imbX, iBlk, 
            pmbmd, pWMVDec->m_ppInterDCTTableInfo_Dec, pDQ, bAcPredOn);
        if (ICERR_OK != result) {
            return ICERR_ERROR;
        }        
    }

#ifdef _GENERATE_DXVA_DATA_
    if (pWMVDec->m_pDMO || pWMVDec->m_bDxvaOffLine)
    {
        if (!pWMVDec->m_iDXVAOn)
            dxvaDumpFieldIMB(pWMVDec->m_ppxliFieldMB, 2);
    }
#endif

    if ((pWMVDec->m_iOverlap & 1) && pmbmd->m_bOverlapIMB)  //  m_bOverlapIMB ×´Ì¬³ö´í  321
	{
        InvOverlapInterlaceV2 (pWMVDec,
            imbY, imbX, pmbmd, ppxliTextureQMBY, ppxliTextureQMBU, ppxliTextureQMBV,
            ppxliErrorY, ppxliErrorU, ppxliErrorV, pWMVDec->m_ppxliFieldMB, pWMVDec->m_ppxliIntraRowY,
            pWMVDec->m_ppxliIntraRowU, pWMVDec->m_ppxliIntraRowV);
    } else {
        if (pmbmd->m_chFieldDctMode == FALSE) {
            CopyIntraFieldMBtoFrameMBInterlaceV2 (
                pWMVDec->m_ppxliFieldMB, ppxliTextureQMBY, ppxliTextureQMBU, 
                ppxliTextureQMBV, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
        } else {
            CopyIntraFieldMBtoFieldMBInterlaceV2 (
                pWMVDec->m_ppxliFieldMB, ppxliTextureQMBY, ppxliTextureQMBU, 
                ppxliTextureQMBV, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
        }
    }


    return ICERR_OK;
}

Void_WMV g_inverseIntraBlockQuantizeInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I16_WMV *piCoefRecon, I32_WMV iNumCoef, DQuantDecParam *pDQ)
{    
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    I32_WMV iDCStepSize = pDQ->iDCStepSize;
    I32_WMV i;

    piCoefRecon[0] = (I16_WMV)((piCoefRecon[0] * iDCStepSize));
    for ( i = 1; i < iNumCoef; i++) {
        I32_WMV iLevel = piCoefRecon[i];
        if (iLevel)  {
            if (iLevel > 0)
                piCoefRecon[i] = (I16_WMV)(iDoubleStepSize * iLevel + iStepMinusStepIsEven);
            else // iLevel < 0 (note: iLevel != 0)
                piCoefRecon[i] = (I16_WMV)(iDoubleStepSize * iLevel - iStepMinusStepIsEven);
        }
    }
}

Void_WMV ScaleTopPredForDQuantInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    I32_WMV i;
    if (iBlk == 0 || iBlk == 1) {
        I32_WMV iTmp;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        pPredScaled [0] = 
            divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
        for (i = 1; i < 8; i++) {
            iTmp = pPred [i] * (pmbmd - pWMVDec->m_uintNumMBX)->m_iQP;
            pPredScaled [i] = divroundnearest (iTmp, pmbmd->m_iQP);
        }
    } else if (iBlk == 4 || iBlk == 5) {
        I32_WMV iTmp;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        pPredScaled [0] = 
            divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
        for (i = 1; i < 8; i++) {
            iTmp = pPred [i] * (pmbmd - pWMVDec->m_uintNumMBX)->m_iQP;
            pPredScaled [i] = divroundnearest (iTmp, pmbmd->m_iQP);
        }
    } else {
        for (i = 0; i < 8; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}

Void_WMV ScaleLeftPredForDQuantInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    I32_WMV i;
    if (iBlk == 0 || iBlk == 2) {
        I32_WMV iTmp;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        pPredScaled [0] = 
            divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
        for (i = 1; i < 8; i++) {
            iTmp = pPred [i] * (pmbmd - 1)->m_iQP;
            pPredScaled [i] = divroundnearest (iTmp, pmbmd->m_iQP);
        }
    } else if (iBlk == 4 || iBlk == 5) {
        I32_WMV iTmp;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        pPredScaled [0] = 
            divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
        for (i = 1; i < 8; i++) {
            iTmp = pPred [i] * (pmbmd - 1)->m_iQP;
            pPredScaled [i] = divroundnearest (iTmp, pmbmd->m_iQP);
        }
    } else {
        for (i = 0; i < 8; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}

Void_WMV ScaleDCPredForDQuantInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV iBlk, CWMVMBMode* pmbmd, I32_WMV *piTopDC, I32_WMV *piLeftDC, I32_WMV *piTopLeftDC)
{
    I32_WMV iTopDC = *piTopDC;
    I32_WMV iTopLeftDC = *piTopLeftDC;
    I32_WMV iLeftDC = *piLeftDC;
    if (iBlk == 0) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX - 1)->m_iQP].iDCStepSize;
        I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize;
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iTopDC * iTopDCStepSize;
        iTopDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    } else if (iBlk == 1) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize;
        
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iTopDC * iTopDCStepSize;
        iTopDC = divroundnearest (iTmp, iCurrDCStepSize);
    } else if (iBlk == 2) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize;
        
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    } else if (iBlk == 4 || iBlk == 5) {
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX - 1)->m_iQP].iDCStepSize;
        I32_WMV iTopDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize;
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iTopDC * iTopDCStepSize;
        iTopDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    }

    *piTopDC =  iTopDC ;
    *piTopLeftDC =  iTopLeftDC;
    *piLeftDC =iLeftDC ;
}

Void_WMV SetIBlockBoundaryForDCPred (tWMVDecInternalMember * pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode* pmbmd, Bool_WMV *pbIsTopBlockI,
    Bool_WMV *pbIsLeftBlockI, Bool_WMV *pbIsTopLeftBlockI)
{
    Bool_WMV bIsTopBlkBoundary, bIsLeftBlkBoundary;
    Bool_WMV bIsTopBlockI, bIsLeftBlockI, bIsTopLeftBlockI;

    if (iBlk < 4) {
        bIsTopBlkBoundary = (((imbY == 0) || pWMVDec->m_pbStartOfSliceRow[imbY]) && (iBlk == 0 || iBlk == 1));
        bIsLeftBlkBoundary = ((imbX == 0) && (iBlk == 0 || iBlk == 2));
    } else {
        bIsTopBlkBoundary = ((imbY == 0) || pWMVDec->m_pbStartOfSliceRow[imbY]);
        bIsLeftBlkBoundary = (imbX == 0);
    }

    // set bIsTopBlockI;
    if (!bIsTopBlkBoundary) {
        if (iBlk < 2) { // iBlk = 0,1
            // 0's Top is above MB's 2 & 1's Top is above MB's 3 
            bIsTopBlockI = ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA);
        } else if (iBlk < 4) { // iBlk = 2,3
            // 2's Top is same MB's 0 & 3's Top is same MB's 1
            bIsTopBlockI = (pmbmd->m_dctMd == INTRA);
        } else { // U,V
            bIsTopBlockI = ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA);
        }
    } else {
        bIsTopBlockI = FALSE;
    }

    // set bIsLeftBlockI
    if (!bIsLeftBlkBoundary) {
        if (iBlk == 0 || iBlk == 2) {
            bIsLeftBlockI = ((pmbmd - 1)->m_dctMd == INTRA);
        } else if (iBlk == 1 || iBlk == 3) {
            bIsLeftBlockI = (pmbmd->m_dctMd == INTRA);
        } else { // U,V
            bIsLeftBlockI = ((pmbmd - 1)->m_dctMd == INTRA);
        }
    } else {
        bIsLeftBlockI = FALSE;
    }

    // set bIsTopLeftBlockI
    if (!bIsTopBlkBoundary && !bIsLeftBlkBoundary) {
        if (iBlk == 0) {
            bIsTopLeftBlockI = ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA);
        } else if (iBlk == 1) {
            bIsTopLeftBlockI = ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA);
        } else if (iBlk == 2) {
            bIsTopLeftBlockI = ((pmbmd - 1)->m_dctMd == INTRA);
        } else if (iBlk == 3) {
            bIsTopLeftBlockI = (pmbmd->m_dctMd == INTRA);
        } else { // U,V
            bIsTopLeftBlockI = ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA);
        }
    } else {
        bIsTopLeftBlockI = FALSE;
    }

    *pbIsTopBlockI = bIsTopBlockI;
    *pbIsLeftBlockI = bIsLeftBlockI;
    *pbIsTopLeftBlockI = bIsTopLeftBlockI;

}

Void_WMV decodeDCACPredInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode* pmbmd,
    DCACPREDDIR *iPredDir, I16_WMV *rgiPredCoef)
{
    I32_WMV iWidth, iIndex;
    I16_WMV *pIntra, *pPredIntraL, *pPredIntraT;
    Bool_WMV bIsTopBlockI, bIsLeftBlockI, bIsTopLeftBlockI;

    SetIBlockBoundaryForDCPred (pWMVDec, imbY, imbX, iBlk, pmbmd, &bIsTopBlockI,
        &bIsLeftBlockI, &bIsTopLeftBlockI);

    if (iBlk < 4) {
        iWidth = 2 * pWMVDec->m_uintNumMBX;
        iIndex = (imbY * 2 + (iBlk >> 1)) * iWidth + (imbX * 2 + (iBlk & 1));
        pIntra = pWMVDec->m_pX9dct + iIndex * 16;
    } else {
        I32_WMV iIndex ;
        iWidth = pWMVDec->m_uintNumMBX;
        iIndex = imbY * iWidth + imbX;
        if (iBlk == 4) {
            pIntra = pWMVDec->m_pX9dctU + iIndex * 16;
        } else {
            pIntra = pWMVDec->m_pX9dctV + iIndex * 16;
        }
    }
    pPredIntraL = pIntra - 16 + 8;
    pPredIntraT = pIntra - iWidth * 16;

    if (bIsLeftBlockI && bIsTopBlockI) {              
        // pick best direction
        I32_WMV iTopLeftDC = (bIsTopLeftBlockI) ? pPredIntraT [-16] : 0;
        I32_WMV iTopDC = pPredIntraT [0];
        I32_WMV iLeftDC = pPredIntraL [0];
    
        ScaleDCPredForDQuantInterlaceV2 (pWMVDec, iBlk, pmbmd, &iTopDC, &iLeftDC, &iTopLeftDC);

        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {  
            ScaleTopPredForDQuantInterlaceV2 (pWMVDec, iBlk, pPredIntraT, rgiPredCoef, pmbmd);
            *iPredDir = TOP;
        } else {
            ScaleLeftPredForDQuantInterlaceV2 (pWMVDec, iBlk, pPredIntraL, rgiPredCoef, pmbmd);
            *iPredDir = LEFT;
        }          
    } 
	else if (bIsLeftBlockI) {
        ScaleLeftPredForDQuantInterlaceV2 (pWMVDec, iBlk, pPredIntraL, rgiPredCoef, pmbmd);
        *iPredDir = LEFT;
    } else if (bIsTopBlockI) 
	{
        ScaleTopPredForDQuantInterlaceV2 (pWMVDec, iBlk, pPredIntraT, rgiPredCoef, pmbmd);
        *iPredDir = TOP;    
    } 
#if 1
	else 
	{
		rgiPredCoef [0] = rgiPredCoef [1] = rgiPredCoef [2] = rgiPredCoef [3] = 
			rgiPredCoef [4] = rgiPredCoef [5] = rgiPredCoef [6] = rgiPredCoef [7] = 0;
    }
#else
	else 
	{
        I32_WMV i;
        *iPredDir = NONE;
        for (i = 0; i < 8; i++) {
            rgiPredCoef [i] = 0;
        }
    }
#endif
}

Void_WMV StoreDCACPredCoefInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I16_WMV *piLevelBlk)
{
    I16_WMV *pIntra;
    I32_WMV i;

    if (iBlk < 4) {
        I32_WMV iWidth = 2 * pWMVDec->m_uintNumMBX;
        I32_WMV iIndex = (imbY * 2 + (iBlk >> 1)) * iWidth + (imbX * 2 + (iBlk & 1));
        pIntra = pWMVDec->m_pX9dct + iIndex * 16;
    } else if (iBlk == 4) {
        I32_WMV iWidth = pWMVDec->m_uintNumMBX;
        I32_WMV iIndex = imbY * iWidth + imbX;
        pIntra = pWMVDec->m_pX9dctU + iIndex * 16;
    } else {
        I32_WMV iWidth = pWMVDec->m_uintNumMBX;
        I32_WMV iIndex = imbY * iWidth + imbX;
        pIntra = pWMVDec->m_pX9dctV + iIndex * 16;
    }
    
    if (pWMVDec->m_bRotatedIdct) {
        for (i = 0; i < 8; i++) {
            pIntra[i] = (I16_WMV) piLevelBlk [i << 3];
            pIntra[i + 8] = (I16_WMV) piLevelBlk [i];
        }    
    } else {
        for (i = 0; i < 8; i++) {
            pIntra[i] = (I16_WMV) piLevelBlk [i];
            pIntra[i + 8] = (I16_WMV) piLevelBlk [i << 3];
        }    
    }
}

I32_WMV decodeIntraDCInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    I16_WMV *piDC, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCStepSize)
{
    I32_WMV iDC;
    iDC = Huffman_WMV_get(hufDCTDCDec,pWMVDec->m_pbitstrmIn);

    if (iDC != iTCOEF_ESCAPE) {
        if (iDC != 0) {
            if (iDCStepSize == 4) {
                iDC = (iDC<<1) + (U16_WMV) (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) -1;
            } else if (iDCStepSize == 2){
                iDC = (iDC<<2) + (U16_WMV) (BS_getBits(pWMVDec->m_pbitstrmIn, 2)) -3;
            }                 
            iDC = (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) ? -iDC : iDC;
        } else
            iDC = 0;
    } else {
        U32_WMV offset = 0;
        if (iDCStepSize <= 4) 
            offset = 3 - (iDCStepSize >> 1);
        iDC = BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL + offset);
        iDC = (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) ? -iDC : iDC;
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    *piDC = (I16_WMV) iDC;
    return ICERR_OK;
}

I32_WMV DecodeInverseIntraBlockQuantizeInterlaceV2 (tWMVDecInternalMember * pWMVDec,
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, U8_WMV* piZigzagInv, 
    I16_WMV *rgiCoefRecon, U32_WMV uiNumCoef)
{
    CDCTTableInfo_Dec* IntraDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec[0]; 
    
    //FUNCTION_PROFILE_DECL_START(fpDecode,DECINVIBQUANTESCCODE_PROFILE );
    Huffman_WMV* hufDCTACDec = IntraDCTTableInfo_Dec -> hufDCTACDec;
    I8_WMV* rgLevelAtIndx = IntraDCTTableInfo_Dec -> pcLevelAtIndx;
    U8_WMV* rgRunAtIndx = IntraDCTTableInfo_Dec -> puiRunAtIndx;
    U8_WMV* rgIfNotLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec -> puiNotLastNumOfLevelAtRun;
    U8_WMV* rgIfLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec -> puiLastNumOfLevelAtRun; 
    U8_WMV* rgIfNotLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec -> puiNotLastNumOfRunAtLevel;
    U8_WMV* rgIfLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec -> puiLastNumOfRunAtLevel;
    U32_WMV iStartIndxOfLastRun = IntraDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
    I32_WMV iTCOEF_ESCAPE = IntraDCTTableInfo_Dec -> iTcoef_ESCAPE;
        
    Bool_WMV bIsLastRun = FALSE;
    U32_WMV uiRun; // = 0;
    I32_WMV iLevel; // = 0;
    U32_WMV uiCoefCounter = 1;
    U8_WMV   lIndex;    
    
    do {
        lIndex = (U8_WMV) ( Huffman_WMV_get(hufDCTACDec,pWMVDec->m_pbitstrmIn) );
        assert(lIndex <= iTCOEF_ESCAPE);

        if (lIndex != iTCOEF_ESCAPE)    {
            bIsLastRun = (lIndex >= iStartIndxOfLastRun);
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) ? - rgLevelAtIndx[lIndex] :
            rgLevelAtIndx[lIndex];

        }
        else {
            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){

                // ESC + '1' + VLC
                lIndex = (U8_WMV) (Huffman_WMV_get(hufDCTACDec,pWMVDec->m_pbitstrmIn) );
                if (lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return ICERR_ERROR;
                }
                uiRun = rgRunAtIndx [lIndex];
                iLevel = rgLevelAtIndx[lIndex];
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    iLevel += (I8_WMV) rgIfLastNumOfLevelAtRunIntra[uiRun];
                }
                else
                    iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRunIntra[uiRun];
                if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) 
                    iLevel = -iLevel;

            }
            else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)){

                // ESC + '01' + VLC
                lIndex = (U8_WMV) ( Huffman_WMV_get(hufDCTACDec,pWMVDec->m_pbitstrmIn) );
                if (lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return ICERR_ERROR;
                }
                uiRun = rgRunAtIndx [lIndex];
                iLevel = rgLevelAtIndx[lIndex];
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) 
                    iLevel = -iLevel;

            }
            else{
                // ESC + '00' + FLC
                bIsLastRun = (Bool_WMV) BS_getBits(pWMVDec->m_pbitstrmIn, 1); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){
                    // WMV_ESC_Decoding();
                    if (pWMVDec->m_bFirstEscCodeInFrame){
                        decodeBitsOfESCCode (pWMVDec);
                        pWMVDec->m_bFirstEscCodeInFrame = FALSE;
                    }                                                                               
                    uiRun = BS_getBits(pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);
                    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) /* escape decoding */
                        iLevel = -1 * BS_getBits(pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);
                    else                                                                            
                    iLevel = BS_getBits(pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);
                }
                else{
                    uiRun = BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);            
                    iLevel = (I8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
                }
            }
        }
        uiCoefCounter += uiRun;
        
        if (BS_invalid(pWMVDec->m_pbitstrmIn) || uiCoefCounter >= uiNumCoef) {
            ;
            return ICERR_ERROR;
        }
        
        rgiCoefRecon[piZigzagInv[uiCoefCounter]] = (I16_WMV) iLevel;       
        uiCoefCounter++;

    } while (!bIsLastRun);
    
    FUNCTION_PROFILE_STOP(&fpDecode);
    return ICERR_OK;
}




I32_WMV decodeVOPType (tWMVDecInternalMember * pWMVDec)
{
    

    if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
        pWMVDec->m_tFrmType = PVOP;
    } else {
        if (pWMVDec->m_iNumBFrames == 0) {
            pWMVDec->m_tFrmType = IVOP;
        } else { // pWMVDec->m_bBFrameOn == TRUE

            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1) {
                pWMVDec->m_tFrmType = IVOP;
            } else {
                pWMVDec->m_tFrmType = BVOP;
            }
              
            if (pWMVDec->m_tFrmType == BVOP)
            {
                I32_WMV iShort = 0, iLong = 0;
                iShort = BS_getBits(pWMVDec->m_pbitstrmIn, 3);
                if (iShort == 0x7)
                {
                    iLong = BS_getBits(pWMVDec->m_pbitstrmIn, 4);

                    if (iLong == 0xe)       // "hole" in VLC
                        return ICERR_ERROR;

                    if (iLong == 0xf)
                        pWMVDec->m_bIsBChangedToI = TRUE;
#ifndef WMV9_SIMPLE_ONLY
                    else
                    {
                        DecodeFrac(pWMVDec, iLong + 112, TRUE);
                    }
#endif
                }
#ifndef WMV9_SIMPLE_ONLY
                else
                    DecodeFrac(pWMVDec, iShort, FALSE);
#endif
            }
        }
    }

    
    if (pWMVDec->m_bIsBChangedToI)
        pWMVDec->m_tFrmType = BIVOP;
    if (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != PVOP && pWMVDec->m_tFrmType != BVOP && pWMVDec->m_tFrmType != BIVOP)
        return ICERR_ERROR;


    return ICERR_OK;
}

I32_WMV decodeVOPHeadInterlaceV2 (tWMVDecInternalMember * pWMVDec)    
{
    
    I32_WMV stepDecoded;
    CPanScanInfo sPanScanInfo;
    I32_WMV iNumWindowsPresent = 0;
   
    //g_iFading = 0;
    
    DecodeVOPType_WMVA (pWMVDec); //zou-->Value = vc1DECBIT_GetVLC(pBitstream, vc1DECPIC_Picture_Type_Table);

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
    else if (pWMVDec->m_tFrmType != IVOP && pWMVDec->m_tFrmType != PVOP && pWMVDec->m_tFrmType != BIVOP && pWMVDec->m_tFrmType != BVOP)   
        return ICERR_ERROR;
    
    if (pWMVDec->m_bTemporalFrmCntr) //zou-->FrameCounterFlag
    {
        /*I32_WMV iTime = */BS_getBits( pWMVDec->m_pbitstrmIn,  8);  //zou --->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_TFCNTR);
    }

    if (pWMVDec->m_bBroadcastFlags){ //zou --> PullDownFlag	
        if (pWMVDec->m_bInterlacedSource) {
            pWMVDec->m_bTopFieldFirst = BS_getBits( pWMVDec->m_pbitstrmIn,  1);  // zou -->TopFieldFirst
            pWMVDec->m_bRepeatFirstField = BS_getBits( pWMVDec->m_pbitstrmIn,  1);
#ifdef DUMPLOG
            {
            if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog, "TFF %d RFF %d\n", pWMVDec->m_bTopFieldFirst, pWMVDec->m_bRepeatFirstField);
            }
            }
#endif
        }
        else // Progressive source
        {
            pWMVDec->m_iRepeatFrameCount = BS_getBits( pWMVDec->m_pbitstrmIn,  2);
#ifdef DUMPLOG
            {
            if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog, "RFC %d\n", pWMVDec->m_iRepeatFrameCount);
            }
            }
#endif
        }
    }


    if (pWMVDec->m_bPanScanPresent) {  //zou --->PanScanFlag
        DecodePanScanInfo (pWMVDec, &sPanScanInfo, &iNumWindowsPresent);  //zou--->vc1DECPIC_UnpackPanScanParams
    }
        
    pWMVDec->m_iRndCtrl = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //pParams->sInterpolate.RndCtrl = (FLAG)Value;

    if (pWMVDec->m_bInterlacedSource) {
        pWMVDec->m_bProgressive420 = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //zou--->pParams->UVSampleMode = (FLAG)Value;

#ifdef DUMPLOG
    {
    if(pWMVDec->m_pFileLog) {
    fprintf(pWMVDec->m_pFileLog, "UVSAMP %d\n", pWMVDec->m_bProgressive420);
    }
    }
#endif
    }
        
    stepDecoded = BS_getBits(pWMVDec->m_pbitstrmIn, NUMBITS_VOP_QUANTIZER);  //zou --->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_PQINDEX);
    
    pWMVDec->m_iQPIndex = stepDecoded;

    if (stepDecoded <= MAXHALFQP)
        pWMVDec->m_bHalfStep = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //zou ---->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_HALFQP);
    else
        pWMVDec->m_bHalfStep = FALSE;

    if (pWMVDec->m_bExplicitFrameQuantizer)
        pWMVDec->m_bUse3QPDZQuantizer = BS_getBits(pWMVDec->m_pbitstrmIn, 1); //zou --->

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

  if(!pWMVDec->m_bCodecIsWVC1){//zou 321
    pWMVDec->m_iOverlap = 0;
    if (pWMVDec->m_bSequenceOverlap && !(pWMVDec->m_tFrmType == BVOP )) {
        if (pWMVDec->m_iStepSize >= 9)
            pWMVDec->m_iOverlap = 1;
        else if ((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP))
            pWMVDec->m_iOverlap = 7;
    }
   }

    pWMVDec->m_prgDQuantParam = pWMVDec->m_bUse3QPDZQuantizer ? pWMVDec->m_rgDQuantParam3QPDeadzone :
        pWMVDec->m_rgDQuantParam5QPDeadzone;

   // g_iStepsize = pWMVDec->m_iQPIndex * 2 + pWMVDec->m_bHalfStep;
    
    pWMVDec->m_iStepSize = stepDecoded;
    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    if (pWMVDec->m_iStepSize <= 0 || pWMVDec->m_iStepSize > 31)
            return ICERR_ERROR;

    

    if (pWMVDec->m_bPostProcInfoPresent)
        pWMVDec->m_iPostProcLevel = BS_getBits(pWMVDec->m_pbitstrmIn, 2); //zou ---->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_POSTPROC);

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



    if (pWMVDec->m_iQPIndex <= MAXHIGHRATEQP)
    {
        // If QP (picture quant) is <= MAXQP_3QPDEADZONE then we substitute the highrate
        // inter coeff table for the talking head table among the 3 possible tables that
        // can be used to code the coeff data
        pWMVDec->m_pInterDCTTableInfo_Dec_Set[0] = &pWMVDec->InterDCTTableInfo_Dec_HghRate;
        pWMVDec->m_pIntraDCTTableInfo_Dec_Set[0] = &pWMVDec->IntraDCTTableInfo_Dec_HghRate;
    }
        
    if (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) {

        // Field Transform Mode
        if (DecodeSkipBitX9(pWMVDec,2) == ICERR_ERROR)   //zou ---> /* Field TX bitplane */
            return ICERR_ERROR;   
        if (pWMVDec->m_iFieldDctIFrameCodingMode != SKIP_RAW) {
            I32_WMV i;
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                pWMVDec->m_rgmbmd[i].m_chFieldDctMode = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE : FALSE);
            }
        }

        // AC Pred flag
        if (DecodeSkipBitX9(pWMVDec,4) == ICERR_ERROR) {  //zou --->DEBUG0(vc1DEBUG_PIC, "ACPRED: Bitplane\n");
            return ICERR_ERROR;
        }
        if (pWMVDec->m_iACPredIFrameCodingMode != SKIP_RAW) {
            I32_WMV i;
            for (i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                pWMVDec->m_rgmbmd[i].m_rgbDCTCoefPredPattern2 [0] = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE 
                    : FALSE);
            }
        }

        // Conditional Overlap

    if(pWMVDec->m_bCodecIsWVC1){
			I32_WMV i;
			if(pWMVDec->m_bSequenceOverlap)
			{
				if (pWMVDec->m_iStepSize <=8)
				{
					if(BS_getBits(pWMVDec->m_pbitstrmIn, 1)==0) //SPEC 117 case b    0b
					{
						 pWMVDec->m_iOverlap = 0;
						 for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) 
							 pWMVDec->m_rgmbmd[i].m_bOverlapIMB = FALSE;  
					}
					else if(BS_getBits(pWMVDec->m_pbitstrmIn, 1)==0) //SPEC 117 case c; 10b
					{
						pWMVDec->m_iOverlap = 1;
						for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) 
							pWMVDec->m_rgmbmd[i].m_bOverlapIMB = TRUE;  
					}	
					else   //SPEC 117 case c; 11b
					{
						pWMVDec->m_iOverlap = 1;
						 if (DecodeSkipBitX9 (pWMVDec, 5) == ICERR_ERROR) 
							return ICERR_ERROR;   
						if (pWMVDec->m_iOverlapIMBCodingMode != SKIP_RAW) 
						{
							for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) 
								pWMVDec->m_rgmbmd[i].m_bOverlapIMB = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE : FALSE);                 
						}
					}
				}
				else //vc1_CondOverAll
				{
					pWMVDec->m_iOverlap = 1;
					for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) 
						pWMVDec->m_rgmbmd[i].m_bOverlapIMB = TRUE;
				}
			}
			else //vc1_CondOverNone
			{
				pWMVDec->m_iOverlap = 0;
				 for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) 
                    pWMVDec->m_rgmbmd[i].m_bOverlapIMB = FALSE;
			}
    }
    else {

        if (pWMVDec->m_iOverlap & 2) {//zou --->  conditional overlap flags    vc1decpic.c line 2242
            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 0) {// off
                I32_WMV i;
                 pWMVDec->m_iOverlap = 0;
                for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                    pWMVDec->m_rgmbmd[i].m_bOverlapIMB = FALSE;  
                }
            } else if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 0) {// on
                I32_WMV i;
                pWMVDec->m_iOverlap = 1;
                for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                    pWMVDec->m_rgmbmd[i].m_bOverlapIMB = TRUE;  
                }
            } else { // MB switch - decode bitplane
                if (DecodeSkipBitX9 (pWMVDec, 5) == ICERR_ERROR) 
                 return ICERR_ERROR;   
                if (pWMVDec->m_iOverlapIMBCodingMode != SKIP_RAW) {
                    I32_WMV i;
                    for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                        pWMVDec->m_rgmbmd[i].m_bOverlapIMB = (pWMVDec->m_rgmbmd[i].m_bSkip ? TRUE : FALSE);  
                    }
                
                }
            }
        } else { // no conditional overlap
            I32_WMV i = 0;
            for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                pWMVDec->m_rgmbmd[i].m_bOverlapIMB = FALSE;  
            
            }
        }
      }

        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_rgiDCTACIntraTableIndx[0]){
            pWMVDec->m_rgiDCTACIntraTableIndx[0] += BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = pWMVDec->m_rgiDCTACIntraTableIndx[0];

        pWMVDec->m_iIntraDCTDCTable = BS_getBits(pWMVDec->m_pbitstrmIn, 1);

        if (pWMVDec->m_iDQuantCodingOn)
            DecodeVOPDQuant (pWMVDec, FALSE);
        else
            SetDefaultDQuantSetting (pWMVDec);

       
    }
    else 
    {   
        I32_WMV iMBModeIndex;
        I32_WMV iMVRangeIndex = 0;


        if (pWMVDec->m_bExtendedMvMode) {
            iMVRangeIndex = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            if (iMVRangeIndex)
                iMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            if (iMVRangeIndex == 2)
                iMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            
        }
        SetMVRangeFlag (pWMVDec, iMVRangeIndex);
    
        if (pWMVDec->m_bExtendedDeltaMvMode) {
            pWMVDec->m_iDeltaMVRangeIndex = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
            if (pWMVDec->m_iDeltaMVRangeIndex)
                pWMVDec->m_iDeltaMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn,1);
            if (pWMVDec->m_iDeltaMVRangeIndex == 2)
                pWMVDec->m_iDeltaMVRangeIndex += BS_getBits(pWMVDec->m_pbitstrmIn,1);
            pWMVDec->m_iExtendedDMVX = pWMVDec->m_iDeltaMVRangeIndex & 1;
            pWMVDec->m_iExtendedDMVY = (pWMVDec->m_iDeltaMVRangeIndex & 2) >> 1;
        }

        if (pWMVDec->m_tFrmType == BVOP)
            pWMVDec->m_iX9MVMode = ALL_1MV;
        else
            pWMVDec->m_iX9MVMode = (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) ? MIXED_MV : ALL_1MV;
            
        pWMVDec->m_bLuminanceWarp = FALSE;
        if (BS_getBits(pWMVDec->m_pbitstrmIn, 1)) {
            pWMVDec->m_bLuminanceWarp = TRUE;
           // g_iFading = 1;
        }
        if (pWMVDec->m_bLuminanceWarp) {
            pWMVDec->m_iLuminanceScale = BS_getBits(pWMVDec->m_pbitstrmIn, 6);
            pWMVDec->m_iLuminanceShift = BS_getBits(pWMVDec->m_pbitstrmIn, 6);
        }

        // if global warp exists, disable skip MB
        memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);
#ifdef  INTERLACEV2_B
        if (pWMVDec->m_tFrmType == BVOP)
        {
            if (DecodeSkipBitX9(pWMVDec,3) == ICERR_ERROR)   // decoded into pWMVDec->m_bSkip bit
                return ICERR_ERROR;

            if (pWMVDec->m_iDirectCodingMode != SKIP_RAW) 
            {
                CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
                I32_WMV i;
                for ( i = 0; i < (I32_WMV) pWMVDec->m_uintNumMB; i++) {
                    if (pmbmd->m_bSkip)
                        pmbmd->m_mbType = DIRECT;
                    else
                        pmbmd->m_mbType = (MBType)0;
                    pmbmd++;
                }
            }
        }
#endif
        // decode skip MB flag
        if (DecodeSkipBitX9(pWMVDec,0) == ICERR_ERROR)
            return ICERR_ERROR;
        pWMVDec->m_bCODFlagOn = 1;

        iMBModeIndex = BS_getBits(pWMVDec->m_pbitstrmIn, 2);

        if (pWMVDec->m_iX9MVMode == MIXED_MV) {
            pWMVDec->m_pInterlaceFrameMBMode = pWMVDec->m_pInterlaceFrame4MvMBModeTables [iMBModeIndex];
        } else {
            pWMVDec->m_pInterlaceFrameMBMode = pWMVDec->m_pInterlaceFrame1MvMBModeTables [iMBModeIndex];
        }

        // read MV and CBP codetable indices
        pWMVDec->m_pHufMVTable_V9 = pWMVDec->m_pHufMVTables[BS_getBits(pWMVDec->m_pbitstrmIn, 2)];
        pWMVDec->m_pHufPCBPCYDec = pWMVDec->m_pHufInterlaceCBPCYTables[BS_getBits(pWMVDec->m_pbitstrmIn,3)];
        pWMVDec->m_pHufICBPCYDec = pWMVDec->m_pHufPCBPCYDec;

   
        pWMVDec->m_p2MVBP = pWMVDec->m_p2MVBPTables[BS_getBits(pWMVDec->m_pbitstrmIn, 2)];

        if (pWMVDec->m_iX9MVMode == MIXED_MV || pWMVDec->m_tFrmType == BVOP)
            pWMVDec->m_p4MVBP = pWMVDec->m_p4MVBPTables[BS_getBits(pWMVDec->m_pbitstrmIn, 2)];

        if (pWMVDec->m_iDQuantCodingOn)
            DecodeVOPDQuant (pWMVDec, TRUE);

        if (pWMVDec->m_bXformSwitch) {
            if (BS_getBits(pWMVDec->m_pbitstrmIn, 1) == 1)
            {
                pWMVDec->m_bMBXformSwitching = FALSE;
                pWMVDec->m_iFrameXformMode = s_pXformLUT[BS_getBits(pWMVDec->m_pbitstrmIn, 2)];
            }
            else
                pWMVDec->m_bMBXformSwitching = TRUE;
        }
        else
            pWMVDec->m_bMBXformSwitching = FALSE;

        if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
            return ICERR_ERROR;
        }        
        

        pWMVDec->m_rgiDCTACInterTableIndx[0] = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        if (pWMVDec->m_rgiDCTACInterTableIndx[0]){
            pWMVDec->m_rgiDCTACInterTableIndx[0] += BS_getBits(pWMVDec->m_pbitstrmIn, 1);
        }
        pWMVDec->m_rgiDCTACIntraTableIndx[0] = pWMVDec->m_rgiDCTACIntraTableIndx[1] = pWMVDec->m_rgiDCTACIntraTableIndx[2] = 
            pWMVDec->m_rgiDCTACInterTableIndx[1] = pWMVDec->m_rgiDCTACInterTableIndx[2] = pWMVDec->m_rgiDCTACInterTableIndx[0];
        

        pWMVDec->m_iIntraDCTDCTable = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
          
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    return ICERR_OK;
}

I32_WMV decodeVOPHeadFieldPicture (tWMVDecInternalMember * pWMVDec)    
{
    CPanScanInfo sPanScanInfo;
    I32_WMV iNumWindowsPresent = 0;

    DecodeVOPTypeFieldPicture ( pWMVDec);

    if (pWMVDec->m_bTemporalFrmCntr)
    {
        /*I32_WMV iTime = */BS_getBits( pWMVDec->m_pbitstrmIn,  8); 
    }

    if (pWMVDec->m_bBroadcastFlags){
        if (pWMVDec->m_bInterlacedSource) {
            pWMVDec->m_bTopFieldFirst = BS_getBits( pWMVDec->m_pbitstrmIn,  1);
            pWMVDec->m_bRepeatFirstField = BS_getBits( pWMVDec->m_pbitstrmIn,  1);
#ifdef DUMPLOG
            {
            if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog, "TFF %d RFF %d\n", pWMVDec->m_bTopFieldFirst, pWMVDec->m_bRepeatFirstField);
            }
            }
#endif
        }
        else // Progressive source
        {
            pWMVDec->m_iRepeatFrameCount = BS_getBits( pWMVDec->m_pbitstrmIn,  2);
#ifdef DUMPLOG
            {
            if(pWMVDec->m_pFileLog) {
            fprintf(pWMVDec->m_pFileLog, "RFC %d\n", pWMVDec->m_iRepeatFrameCount);
            }
            }
#endif
        }
	}

    if (pWMVDec->m_bPanScanPresent) {
        DecodePanScanInfo (pWMVDec, &sPanScanInfo, &iNumWindowsPresent);
    }

    pWMVDec->m_iRndCtrl = BS_getBits( pWMVDec->m_pbitstrmIn,  1); //zou --->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_RNDCTRL);
    pWMVDec->m_bProgressive420 = BS_getBits( pWMVDec->m_pbitstrmIn,  1);

#ifdef DUMPLOG
    {
    if(pWMVDec->m_pFileLog) {
    fprintf(pWMVDec->m_pFileLog, "UVSAMP %d\n", pWMVDec->m_bProgressive420);
    }
    }
#endif
    if (pWMVDec->m_bRefDistPresent && (pWMVDec->m_FirstFieldType == IVOP || pWMVDec->m_FirstFieldType == PVOP)) {
        pWMVDec->m_iRefFrameDistance = BS_getBits( pWMVDec->m_pbitstrmIn,  2);
        if (pWMVDec->m_iRefFrameDistance == 3) {
            while (BS_getBits( pWMVDec->m_pbitstrmIn,1)) 
                pWMVDec->m_iRefFrameDistance ++;
        }
    }

    // Decode B frac
    if (pWMVDec->m_FirstFieldType == BVOP || pWMVDec->m_FirstFieldType == BIVOP)
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

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return ICERR_ERROR;
    }

    return ICERR_OK;
}

static tFrameType_WMV s_vopFirstFieldType [8] = {IVOP, IVOP, PVOP, PVOP, BVOP, BVOP, BIVOP, BIVOP};
static tFrameType_WMV s_vopSecondFieldType [8] = {IVOP, PVOP, IVOP, PVOP, BVOP, BIVOP, BVOP, BIVOP};
Void_WMV DecodeVOPTypeFieldPicture (tWMVDecInternalMember * pWMVDec)
{
    // I/I:   000
    // I/P:   001
    // P/I:   010
    // P/P:   011
    // B/B:   100
    // B/BI:  101
    // BI/B:  110
    // BI/BI: 111
    I32_WMV iCode =  BS_getBits( pWMVDec->m_pbitstrmIn,  3);  //zou--->Value = vc1DECBIT_GetBits(pBitstream, VC1_BITS_FPTYPE);
    pWMVDec->m_FirstFieldType = s_vopFirstFieldType [iCode];
    pWMVDec->m_SecondFieldType = s_vopSecondFieldType [iCode];
}

#endif //PPCWMP
