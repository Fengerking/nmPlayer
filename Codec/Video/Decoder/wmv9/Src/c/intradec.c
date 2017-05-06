//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 2000  Microsoft Corporation

Module Name:
        Intradec.cpp

Abstract:
        New Intra decoder

Author:
        Sridhar Srinivasan (sridhsri@microsoft.com)
        28 July 2000

*************************************************************************/
#include "xplatform_wmv.h"
#include "limits.h"
#include "wmvdec_api.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "strmdec_wmv.h"
#include "idctdec_wmv2.h"
#include "localhuffman_wmv.h"

#include "spatialpredictor_wmv.h"
#include "tables_wmv.h"

#ifndef WMV9_SIMPLE_ONLY
extern U8_WMV  zigzagArray[];
extern U16_WMV aNullPredWts_WMV[];
static U8_WMV  *zigzagRedirectionArray[3];
static const I32_WMV iFlatOrients = 3;

extern Bool_WMV g_bSupportMMX_WMV;
//  Non-flat quantization : reconstruction levels
I16_WMV gaReconstructionLevels[] = {
    256,
    256, 256, 256, 256, 256, 259, 262, 265, 269, 272, 275, 278,
    282, 285, 288, 292, 295, 299, 303, 306, 310, 314, 317, 321,
    325, 329, 333, 337, 341, 345, 349, 353, 358, 362, 366, 371,
    375, 379, 384, 389, 393, 398, 403, 408, 413, 417, 422, 428,
    433, 438, 443, 448, 454, 459, 465, 470, 476, 482, 488, 493,
    499, 505, 511
};
static const I32_WMV i2to15 = 1 << 15;

extern I32_WMV LhdecDecodeSymbol (tWMVDecInternalMember *pWMVDec, t_CLocalHuffmanDecoder *pLhdec, CInputBitStream_WMV* pbitstrmIn,
                                  U32_WMV* puRun, I32_WMV* piLevel, I32_WMV* pbLast, tWMVDecodeStatus * pstatus);
extern I32_WMV LhdecDecodeOrient (tWMVDecInternalMember *pWMVDec, t_CLocalHuffmanDecoder *pLhdec, CInputBitStream_WMV* pbitstrmIn, tWMVDecodeStatus * pstatus);
extern I32_WMV LhdecDecodeJoint  (tWMVDecInternalMember *pWMVDec, t_CLocalHuffmanDecoder *pLhdec, CInputBitStream_WMV* pbitstrmIn, tWMVDecodeStatus * pstatus);
I32_WMV x8DequantizeDC(I32_WMV qVal, I32_WMV qLev) { return qLev * qVal;}
extern Void_WMV X8Deblock(U8_WMV *pCenter, I32_WMV iStride, I32_WMV iStepSize,
                          I32_WMV blockX, I32_WMV blockY, Bool_WMV bResidual, I32_WMV iOrient);

/****************************************************************************************
  performLifting (prior to IDCT)
****************************************************************************************/
static Void_WMV PerformLifting16 (I16_WMV *pDCTCoef, I32_WMV iOrient, I32_WMV dcCoef)
{
    I32_WMV  val;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PerformLifting16);

    if (iOrient == 0) {
        val = (3811 * dcCoef + i2to15) >> 16;
        pDCTCoef[1] -= (I16_WMV) val;  pDCTCoef[8] -= (I16_WMV) val;

        val = (487 * dcCoef + i2to15) >> 16;
        pDCTCoef[2] -= (I16_WMV) val;  pDCTCoef[16] -= (I16_WMV) val;

        val = (506 * dcCoef + i2to15) >> 16;
        pDCTCoef[3] -= (I16_WMV) val;  pDCTCoef[24] -= (I16_WMV) val;

        val = (135 * dcCoef + i2to15) >> 16;
        pDCTCoef[4] -= (I16_WMV) val;  pDCTCoef[32] -= (I16_WMV) val;
        pDCTCoef[10] += (I16_WMV) val;  pDCTCoef[11] += (I16_WMV) val;
        pDCTCoef[17] += (I16_WMV) val;  pDCTCoef[25] += (I16_WMV) val;

        val = (173 * dcCoef + i2to15) >> 16;
        pDCTCoef[5] -= (I16_WMV) val;  pDCTCoef[40] -= (I16_WMV) val;

        val = (61 * dcCoef + i2to15) >> 16;
        pDCTCoef[6] -= (I16_WMV) val;  pDCTCoef[48] -= (I16_WMV) val;
        pDCTCoef[13] += (I16_WMV) val; pDCTCoef[41] += (I16_WMV) val;
    
        val = (42 * dcCoef + i2to15) >> 16;
        pDCTCoef[7] -= (I16_WMV) val;  pDCTCoef[56] -= (I16_WMV) val;
        pDCTCoef[12] += (I16_WMV) val; pDCTCoef[36] += (I16_WMV) val;
        pDCTCoef[33] += (I16_WMV) val;

        val = (1084 * dcCoef + i2to15) >> 16;
        pDCTCoef[9] += (I16_WMV) val;
    }
    else {
        if (g_bSupportMMX_WMV ^ (iOrient == 2)) {
            val = 0;
        }
        else {
            val = 3;
        }
        pDCTCoef[1<<val] -= (6269 * dcCoef + i2to15) >> 16;
        pDCTCoef[3<<val] -= (708 * dcCoef + i2to15) >> 16;
        pDCTCoef[5<<val] -= (172 * dcCoef + i2to15) >> 16;
        pDCTCoef[7<<val] -= (73 * dcCoef + i2to15) >> 16;
    }
}

/****************************************************************************************
  Overall IDCT function (handles both C and MMX cases)
****************************************************************************************/
static I8_WMV aAppearance[] = { -1, 0, -1, -1, 1, 1, 0, 0, 0, 2, 2, 2, 1 };
static Bool_WMV m_bIsMMX= FALSE;

static Void_WMV IDCT_Dec (t_CSpatialPredictor *pSp,UnionBuffer *pBuf, I32_WMV iOrient, I32_WMV iLast, Bool_WMV b16bitXform)
{
    FUNCTION_PROFILE(fp)
 	I32_WMV   dcCoef = pBuf->i16[0];
    iOrient = aAppearance[iOrient+1];

    FUNCTION_PROFILE_START(&fp,DECODEIX8LIFTINGDCT_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(IDCT_Dec);

    if (dcCoef && iOrient >= 0) {
        // hack!!!!
        Bool_WMV  bIsMMX = m_bIsMMX;
        if (pSp->m_pIDCT_Dec == g_IDCTDec16_WMV3)
            m_bIsMMX = FALSE;          

        PerformLifting16 (pBuf->i16, iOrient, dcCoef);
        m_bIsMMX = bIsMMX;
    }

    if (iLast /* AC terms exist */ || (dcCoef && iOrient >= 0) /* did lifting */) {
        // compute IDCT
//#ifdef _WMV_TARGET_X86_
//            if (g_bSupportMMX_WMV)
//                g_IDCTDecMMX_WMV2_Inter (pBuf, pBuf, 8, 0xff);
//            else
//#endif // _WMV_TARGET_X86_

#ifdef _EMB_SSIMD_IDCT_
    if (b16bitXform) SignPatch((I16_WMV *)pBuf,128);
#endif
                pSp->m_pIDCT_Dec (pBuf, pBuf, 8, 0xff);

    }
    else {
        I32_WMV  *pBlock32;
        I32_WMV i;
        dcCoef = ((dcCoef + 4) >> 3) & 0xffff;
        dcCoef = dcCoef * 0x10001;
        pBlock32 = pBuf->i32;
        for (i = 0; i < 32; i++)
            *pBlock32++ = dcCoef;
    }
    FUNCTION_PROFILE_STOP(&fp);
}

/****************************************************************************************
  Decoder specific function for adjusting for flat condition
****************************************************************************************/
static Void_WMV AdjustFlatCondition(I32_WMV iDcValue,
                                    U8_WMV *pRef, I32_WMV iRefStride, I32_WMV val,
                                    I32_WMV iStep, I32_WMV iInvStep)
{
    U32_WMV val4, i;
    U32_WMV  *pIRef;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(AdjustFlatCondition);

    iDcValue <<= 3;       // convert to DC coefficient
    // adjust pWMVDec->m_aLevel[0], in quantized domain
    val += /* x8QuantizeDC(pWMVDec->m_iDcValue, iInvStep); */
        ((iDcValue * iInvStep + 0x8000) >> 16);

    // quantized reconstructed predictor
    val = (x8DequantizeDC(val, iStep) + 4) >> 3;
    // clamp value
    if (val < 0)
        val = 0;
    else if (val > 255)
        val = 255;

    // reset predictor, don't care about residual error
    val4 = val * 0x01010101;

    pIRef = (U32_WMV *)pRef;
    for (i = 0; i < 8; i++, pIRef += (iRefStride>>2)) {
     //   pIRef = (U32_WMV *)(pRef + i * iRefStride);
        pIRef[0] = pIRef[1] = val4;  // 4 pixels assigned at a time
    } 
}

/************************************************************************************************
  decodeI_X8block : block decoder for X8 blocks
************************************************************************************************/
#define SMALL_CODE_SIZE

I32_WMV decodeI_X8block (tWMVDecInternalMember *pWMVDec, t_CLocalHuffmanDecoder **pLhdec,
                                CInputBitStream_WMV *pBitstrm, I16_WMV *pReconBuf,
                                I32_WMV iStride, I32_WMV iDCStep,
                                I32_WMV blockX, I32_WMV blockY,
                                U8_WMV *pRecon, I32_WMV iEstRunMin, I32_WMV iDiffOrient,
                                I32_WMV iOrient, I32_WMV iInvDCStep, Bool_WMV bChroma, tWMVDecodeStatus * pstatus)
{
    // Set context for DC value, and read it
    I32_WMV iContext;
    I32_WMV idc0;

    I32_WMV iLast;
    I32_WMV iNumRun = 0;
    Bool_WMV  bResidual;
    I32_WMV   iZCoef;
    
    FUNCTION_PROFILE_DECL_START(fp,DECODEIX8BLOCK_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeI_X8block);

    iContext = (iEstRunMin == 0) ? LH_INTRAZ : LH_INTRANZ;
    if (bChroma) {
        iContext = LH_INTRAC0;
        blockX >>= 1;
        blockY >>= 1;
    }

    idc0 = LhdecDecodeJoint (pWMVDec, pLhdec[iContext], pBitstrm, pstatus);

	if((*pstatus)!=WMV_Succeeded) {
		return 1;
	}

    iLast = (idc0 & 1) ^ 1;
    idc0 >>= 1;

    if (iLast) {
        I32_WMV  iModeDefault, iNonflat = pWMVDec->m_iNonflatQuant;
        U8_WMV  *pZigzag;

        U32_WMV  iRun;
        I32_WMV   iLevel, iLast1 = 0, iCoef = 1;
        I32_WMV   iDequant;
        const I32_WMV iLevelSign = 15;
        const I32_WMV i64Flag = 0x3f;
        I32_WMV  iSmSe;
        I32_WMV  iDSS;

        bResidual = TRUE;
        // Decode DCT AC terms
        if (bChroma) {
            iContext = iModeDefault = LH_INTER;
        }
        else {
            if (iDiffOrient > 4) {
                iModeDefault = LH_INTER0;
                iEstRunMin = 0;
            }
            else {
                iModeDefault = LH_INTRAY;
            }
            iContext = LH_INTRAY0;
            if (iDiffOrient < iFlatOrients)
                iNonflat = 0;
        }

        // Get zigzag scan direction
        pZigzag = zigzagRedirectionArray[zigzagArray[iOrient]];

        iSmSe = pWMVDec->m_iStepMinusStepIsEven;
        iDSS = pWMVDec->m_iDoubleStepSize;

        do {
            if (++iNumRun >= iEstRunMin)
                iContext = iModeDefault;

            LhdecDecodeSymbol(pWMVDec, pLhdec[iContext], pBitstrm, &iRun, &iLevel, &iLast1, pstatus);
			if((*pstatus)!=WMV_Succeeded) {
				return 1;
			}

            iCoef += iRun;

            // find zigzagged coef
            iZCoef = pZigzag [iCoef & i64Flag];
            iLast1 |= (iCoef >> 6); // if iCoef >= 64 - safety check

            // Dequantize DCT AC
            iDequant = iLevel >> iLevelSign;
            iDequant = (iDequant ^ iSmSe) - iDequant;
            pReconBuf[iZCoef] = ((iDSS * iLevel + iDequant)
                    * gaReconstructionLevels[iCoef & iNonflat]) >> 8;
            iCoef++;
        }
        while (iLast1 == 0);
        // handle error if iCoef>=64
        if (iCoef > 64) {
            FUNCTION_PROFILE_STOP(&fp);
            return 1;
        }
    }
    else {  
        // bIsFlat = pWMVDec->m_pSp->isFlat() && (abs(idc0) < 2);
        if (pWMVDec->m_pSp->m_bFlat && ((U32_WMV)(idc0 + 1) < 3)) {
            // bIsFlat is true
            // prediction etc. done in adjustFlatCondition function
            AdjustFlatCondition(pWMVDec->m_pSp->m_iDcValue,
                pRecon, iStride, idc0, iDCStep, iInvDCStep);
            bResidual = TRUE;
            goto SkipChecks;
        }
        bResidual = idc0;
    }

    if (bResidual) {
        // Dequantize DC value
        I32_WMV iDir = iOrient;
        pReconBuf[0] = (I16_WMV) x8DequantizeDC(idc0, iDCStep);
        if (((blockX | blockY) == 0) || ((U32_WMV)(idc0 + 1) < 3))
            iDir = -1;

        // IDCT
        IDCT_Dec (pWMVDec->m_pSp, pWMVDec->m_rgiCoefReconBuf, iDir, iLast, pWMVDec->m_b16bitXform);
        //pWMVDec->m_pSp->IDCT_Dec (pBuffer,
        //    (((blockX | blockY) == 0) || (abs(idc0) < 2)) ? -1 : iOrient, iLast);
        // Spatial prediction: add predicted value and clean buffer

#ifdef SMALL_CODE_SIZE
    }
#endif // SMALL_CODE_SIZE
#ifdef  _WMV_TARGET_X86_
        if (g_bSupportMMX_WMV)
            predictDec_MMX (pWMVDec->m_pSp, iOrient, pRecon, iStride, pReconBuf, bResidual);
        else
#endif // _WMV_TARGET_X86_

            predictDec (pWMVDec->m_pSp, iOrient, pRecon, iStride, pReconBuf, bResidual);

#ifndef SMALL_CODE_SIZE
    }
    else {
        // Spatial prediction: add predicted value
        predict_0 (pWMVDec->m_pSp, iOrient, pRecon, iStride);
    }
#endif // !SMALL_CODE_SIZE

SkipChecks:
    // Update orientation and DCvalue/run contexts
    if (!bChroma)
        t_ContextPutDec (pWMVDec->m_pContext, blockX, blockY, iOrient, iNumRun);
    // Deblocking filter
    if (pWMVDec->m_bLoopFilter) {
        X8Deblock (pRecon, iStride, pWMVDec->m_iStepSize, blockX, blockY, bResidual, iOrient);
    }

    FUNCTION_PROFILE_STOP(&fp);
    return 0;
}

 
/************************************************************************************************
  SetupSpatialPredictor : setup function
************************************************************************************************/
static I32_WMV orderArray[]={0,8,4, 10,11, 2,6,9, 1,3,5,7};
static I32_WMV orderArrayH[]={8,0,4, 10,11, 1,7, 2,6,9, 3,5};
static I32_WMV orderArrayV[]={4,0,8, 11,10, 3,5, 2,6,9, 1,7};
#define EIGHT 8

static Bool_WMV SetupSpatialPredictor (t_CSpatialPredictor *pSp, const U8_WMV *pRef, I32_WMV iRefStride,
                               I32_WMV iX, I32_WMV iY, I32_WMV iBlocksInX, I32_WMV* piPredOrient,
                               I32_WMV** ppOrder, I32_WMV iStepSize, Bool_WMV bChroma)
{
    //const U8_WMV meanGrey = 128, *pPtr;
    const U8_WMV *pPtr;
    I32_WMV  i;
    I32_WMV iRange;

    FUNCTION_PROFILE_DECL_START(fp,DECODEIX8SETUP_PROFILE)
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetupSpatialPredictor);

    pSp->m_bFlat = FALSE_WMV;
    // initialize zero predictor setup
    pSp->m_bInitZeroPredictor = FALSE_WMV;

    // first initialize the neighbor pixel values
    if (iX == 0 && iY == 0) {
        U32_WMV  uMeanGrey = 0x80808080;
        U32_WMV  *pPtr;
        pSp->m_iRefStride = -1;
        pSp->m_pRef = pSp->m_pLeft + 1;

        // memset(pWMVDec->m_pNeighbors, meanGrey, sizeof(U8_WMV)*pWMVDec->m_iNeighbors);
        pPtr = (U32_WMV *) (pSp->m_pNeighbors + 12);
        for (i = 0; i < 13; i++)  // be sure this space is available
            pPtr[i] = uMeanGrey;
    }
    else if (iX == 0) {
        I32_WMV sum = 0;
        U32_WMV  uSet;
        U32_WMV *pDst;
        pSp->m_iRefStride = -1;
        pSp->m_pRef = pSp->m_pLeft + 1;

        pPtr = pRef - iRefStride;
        if (iX < iBlocksInX - 1) {
            U32_WMV  *pSrc = (U32_WMV *) pPtr;
            U32_WMV  *pDst = (U32_WMV *) pSp->m_pTop;
            pDst[0] = pSrc[0];
            pDst[1] = pSrc[1];
            pDst[2] = pSrc[2];
            pDst[3] = pSrc[3];
            pSrc = (U32_WMV *)(pPtr - iRefStride);
            pDst[4] = pSrc[0];
            pDst[5] = pSrc[1];
            // memcpy (pWMVDec->m_pTop, pPtr, pWMVDec->m_pSide * 2); // copy top row
            // memcpy (pWMVDec->m_pTop+pWMVDec->m_pSide*2, pPtr-iRefStride, pWMVDec->m_pSide*2);
        }
        else {
            const U32_WMV *pSrc0 = (const U32_WMV *) pPtr;
            const U32_WMV *pSrc1 = (const U32_WMV *)(pPtr - iRefStride);
            U32_WMV  *pDst = (U32_WMV *) pSp->m_pTop;
            const U32_WMV  uSet0 = (U32_WMV) pPtr[EIGHT - 1] * 0x01010101;
            //const U32_WMV  uSet1 = (U32_WMV) pPtr[EIGHT - 1 - iRefStride] * 0x01010101;

            pDst[0] = pSrc0[0];
            pDst[1] = pSrc0[1];
            pDst[2] = pDst[3] = uSet0;

            pDst[4] = pSrc1[0];
            pDst[5] = pSrc1[1];

            // memcpy (pWMVDec->m_pTop, pPtr, pWMVDec->m_pSide); // copy top row
            // memcpy (pWMVDec->m_pTop+pWMVDec->m_pSide*2, pPtr-iRefStride, pWMVDec->m_pSide);
            // memset (pWMVDec->m_pTop+pWMVDec->m_pSide, pPtr[pWMVDec->m_pSide-1], pWMVDec->m_pSide);
            // memset (pWMVDec->m_pTop+pWMVDec->m_pSide*3, pPtr[pWMVDec->m_pSide-1-iRefStride], pWMVDec->m_pSide);            
        }
        for (i = 0; i < EIGHT; i++)  sum += pSp->m_pTop[i];
        sum = (sum + (EIGHT >> 1)) / EIGHT;

        uSet = (U32_WMV) sum * 0x01010101; 
        pDst = (U32_WMV *) pSp->m_pTop;
        pDst -= 5;
        pDst[0] = pDst[1] = pDst[2] = pDst[3] = pDst[4] = uSet;

        // memset (pWMVDec->m_pNeighbors, sum, pWMVDec->m_pSide * 2 + 1);
    }
    else if (iY == 0) {
        I32_WMV sum = 0;
        U32_WMV  uSet;
        U32_WMV *pDst;
        pSp->m_pRef = pRef;
        pSp->m_iRefStride = iRefStride;

        pPtr = pRef - 1;
        for (i = 0; i < 8; i++)  {
            sum += (pSp->m_pLeft[- (i + 1)] = pPtr[i * iRefStride]);
            // pWMVDec->m_pLeft[- (i + 1) - 8] = pPtr[i * iRefStride - 1];
        }
        sum = (sum + 4) >> 3;  // pWMVDec->m_pSide = 8 assumed
        *pSp->m_pLeft = (U8_WMV) sum;

        uSet = (U32_WMV) sum * 0x01010101; 
        pDst = (U32_WMV *) pSp->m_pTop;
        pDst[0] = pDst[1] = pDst[2] = pDst[3] =
        pDst[4] = pDst[5] = uSet;

        // memset(pWMVDec->m_pLeft, sum, pWMVDec->m_pSide * 4 + 1);
    }
    else { // all neighbors exist
        U8_WMV  *pDst0;
        const U8_WMV *pPtrM1;
        U8_WMV  *pDst1;
        pSp->m_pRef = pRef;
        pSp->m_iRefStride = iRefStride;

        pPtr = pRef-iRefStride;

        if (iX < iBlocksInX - 1) {
            const U32_WMV  *pSrc0 = (const U32_WMV *) pPtr;
            U32_WMV  *pDst = (U32_WMV *) pSp->m_pTop;
            const U32_WMV  *pSrc1 = (const U32_WMV *)(pPtr - iRefStride);
            pDst[0] = pSrc0[0];
            pDst[1] = pSrc0[1];
            pDst[2] = pSrc0[2];
            pDst[3] = pSrc0[3];
            pDst[4] = pSrc1[0];
            pDst[5] = pSrc1[1];

            // memcpy (pWMVDec->m_pTop, pPtr, pWMVDec->m_pSide * 2); // copy top row
            // memcpy (pWMVDec->m_pTop + pWMVDec->m_pSide * 2, pPtr - iRefStride, pWMVDec->m_pSide * 2);
        }
        else {
            const U32_WMV *pSrc0 = (const U32_WMV *) pPtr;
            U32_WMV  *pDst = (U32_WMV *) pSp->m_pTop;
            const U32_WMV *pSrc1 = (const U32_WMV *)(pPtr - iRefStride);

            const U32_WMV uSet0 = (U32_WMV) pPtr[EIGHT - 1] * 0x01010101;
            //const U32_WMV uSet1 = (U32_WMV) pPtr[EIGHT - 1 - iRefStride] * 0x01010101;

            pDst[0] = pSrc0[0];
            pDst[1] = pSrc0[1];
            pDst[2] = pDst[3] = uSet0;

            pDst[4] = pSrc1[0];
            pDst[5] = pSrc1[1];

        }

        pPtr = pRef-1;
        pDst0 = pSp->m_pLeft - 4;
        pPtrM1 = pPtr + iRefStride * 4;
        pDst1 = pDst0 - 4;
        pSp->m_pLeft[0] = pPtr[-iRefStride];

        pDst0[3] = pPtr[0];
        pDst1[3] = pPtrM1[0];
        pPtr += iRefStride;
        pPtrM1 += iRefStride;
        pDst0[2] = pPtr[0];
        pDst1[2] = pPtrM1[0];
        pPtr += iRefStride;
        pPtrM1 += iRefStride;
        pDst0[1] = pPtr[0];
        pDst1[1] = pPtrM1[0];
        pPtr += iRefStride;
        pPtrM1 += iRefStride;
        pDst0[0] = pPtr[0];
        pDst1[0] = pPtrM1[0];
    }

/** so now we have the following data elements:

    pWMVDec->m_pLeft[0]   pWMVDec->m_pTop[0] pWMVDec->m_pTop[1] ... pWMVDec->m_pTop[15] pWMVDec->m_pTop[16] ... pWMVDec->m_pTop[31]
    pWMVDec->m_pLeft[-1]     xxx     xxx   ...   xxx
          :                ..
    pWMVDec->m_pLeft[-14]                \ ..
    pWMVDec->m_pLeft[-15]    xxx     xxx         xxx
**/

/** Find the DC value **/
    {
        I32_WMV iDC = pSp->m_pLeft[0];

#ifdef  _WMV_TARGET_X86_
        if (g_bSupportMMX_WMV)
            GetRange (pSp->m_pTop, pSp->m_pLeft - 8, &iRange, &iDC);
        else
#endif // _WMV_TARGET_X86_
        {
            const U8_WMV *pLeft = pSp->m_pLeft - 8;
            I32_WMV iMin = iRange = pSp->m_pTop[0];

            for (i = 0; i < 8; i++) {
                I32_WMV  iT = pSp->m_pTop[i];
                I32_WMV  iL = pLeft[i];

                iDC += iT + iL;
                if (iT > iRange) iRange = iT;
                else if (iT < iMin) iMin = iT;
                if (iL > iRange) iRange = iL;
                else if (iL < iMin) iMin = iL;
            }
            iRange -= iMin;  // range
        }


        if ((iRange < iStepSize) || (iRange < 3)) {
            // prefer zero orientation when information is small
            *piPredOrient = 0;
            if (iRange < 3) {
#ifndef _WMV_TARGET_X86_
                I32_WMV iAcc;
#endif // #ifdef _WMV_TARGET_X86_
                //iAcc;
                pSp->m_bFlat = TRUE_WMV;
                // compute best bin for DC value
                iDC += pSp->m_pTop[EIGHT] + (I32_WMV) pSp->m_pTop[EIGHT + 1] + EIGHT + 1;
                // pWMVDec->m_iDcValue /= (pWMVDec->m_pSide + pWMVDec->m_pSide + 3);
                // need 17 bit precision to divide exactly by 19
                // note 16 bit multiplies are good enough
#ifdef _WMV_TARGET_X86_
                pSp->m_iDcValue = (iDC * 6899) >> 17;
#else // #ifdef _WMV_TARGET_X86_

	            iAcc = (iDC << 2) - iDC;
                iAcc >>= 4;
                iAcc += (iDC << 4) - iDC;
                iAcc >>= 5;
                iAcc += iDC;
                iAcc >>= 2;
                iAcc += iDC;
                iAcc >>= 1;
                iAcc += iDC;
                iAcc >>= 5;
                pSp->m_iDcValue = iAcc;
#endif // _WMV_TARGET_X86_
            }
        }
    }

    if (bChroma) {
        *piPredOrient <<= 2;
        FUNCTION_PROFILE_STOP(&fp)
        return FALSE_WMV;
    }

    // choose order of prediction
    if (*piPredOrient == 2) *ppOrder = orderArrayH;
    else if (*piPredOrient == 1)  *ppOrder = orderArrayV;
    else *ppOrder = orderArray;

    // remap from 0-1-2 to 0-4-8
    (*piPredOrient) <<= 2;

    // almost flat: if horiz, change to decaying horiz, if vert, change to decaying vert
    iStepSize <<= 1;
    if (iRange < iStepSize) { 
        if (iX && iY) {     // only for non-edge blocks!!
            if (*piPredOrient == 4)  *piPredOrient = 11;
            else if (*piPredOrient == 8)  *piPredOrient = 10;
        }
        else
            *piPredOrient = 0;
    }

    FUNCTION_PROFILE_STOP(&fp)
    return (iRange < iStepSize) ? FALSE_WMV : TRUE_WMV;
}
#undef EIGHT //8
 
/************************************************************************************************
  decodeI_X8 : X_I32_WMVRA_8 decoder
************************************************************************************************/
tWMVDecodeStatus WMVideoDecDecodeI_X8 (tWMVDecInternalMember *pWMVDec)
{

    U8_WMV __huge* ppxliCurrY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV __huge* ppxliCurrU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV __huge* ppxliCurrV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    I32_WMV  iInvDCStep;
    I32_WMV  iInvDCStepC;
    tWMVDecodeStatus status = WMV_Succeeded;

    I32_WMV  blockX, blockY;
    // Set the dequantization offset
    I32_WMV iStep;
    //const I32_WMV i64Flag = 0x3f;
    t_CLocalHuffmanDecoder **pLhdec;
    CInputBitStream_WMV *pBitstrm;
    I16_WMV  *pReconBuf;

    FUNCTION_PROFILE(fpX8[2]);
    HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeX8",8);
    DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliCurrQY,NULL,NULL);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeI_X8);
    FUNCTION_PROFILE_START(&fpX8[1],DECODEIX8_PROFILE);
    FUNCTION_PROFILE_START(&fpX8[0],DECODEIX8INIT_PROFILE);

    // Read nonflat quantization bit

    pWMVDec->m_iNonflatQuant = BS_getBits ( pWMVDec->m_pbitstrmIn, 1);
    pWMVDec->m_iNonflatQuant = pWMVDec->m_iNonflatQuant * 0x3f;

    // Set image data poI32_WMVers

    if (pWMVDec->m_cvCodecVersion >= WMV3)
               UpdateDCStepSize (pWMVDec, pWMVDec->m_iStepSize );
    else {
        pWMVDec->m_bStepSizeIsEven       = ((pWMVDec->m_iStepSize & 1) == 0);
        pWMVDec->m_iStepMinusStepIsEven  = pWMVDec->m_iStepSize - pWMVDec->m_bStepSizeIsEven;
        pWMVDec->m_iDoubleStepSize       = pWMVDec->m_iStepSize << 1;
    }

    
    // SET_NEW_FRAME
    t_AltTablesSetNewFrame (pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1/*intra*/);

/** X8 I32_WMVra decoding starts here **/
    pWMVDec->m_iDCStepSize = pWMVDec->m_iDCStepSizeC = pWMVDec->m_iDoubleStepSize >>1;
    if (pWMVDec->m_iDCStepSizeC > 4)
        pWMVDec->m_iDCStepSizeC += (pWMVDec->m_iDCStepSizeC + 3) >> 3;

    iInvDCStep  = (65536 + (pWMVDec->m_iDCStepSize >> 1)) / pWMVDec->m_iDCStepSize;
    iInvDCStepC = (65536 + (pWMVDec->m_iDCStepSizeC >> 1)) / pWMVDec->m_iDCStepSizeC;

    // Set the dequantization offset
    iStep = pWMVDec->m_iStepMinusStepIsEven;

    memset(pWMVDec->m_rgiCoefReconBuf, 0, sizeof(UnionBuffer));  // zero out reconstruction
    if (g_bSupportMMX_WMV) {
        zigzagRedirectionArray[0] = pWMVDec->m_pZigzagInvRotated;
        zigzagRedirectionArray[1] = pWMVDec->m_pHorizontalZigzagInvRotated;
        zigzagRedirectionArray[2] = pWMVDec->m_pVerticalZigzagInvRotated;
    } else {
        zigzagRedirectionArray[0] = pWMVDec->m_pZigzagInv;
        zigzagRedirectionArray[1] = pWMVDec->m_pHorizontalZigzagInv;
        zigzagRedirectionArray[2] = pWMVDec->m_pVerticalZigzagInv;
    }

    if (pWMVDec->m_b16bitXform) {
        zigzagRedirectionArray[0] = pWMVDec->m_pZigzagInv;
        zigzagRedirectionArray[1] = pWMVDec->m_pHorizontalZigzagInv;
        zigzagRedirectionArray[2] = pWMVDec->m_pVerticalZigzagInv;
    }

    pWMVDec->m_pSp->m_pClampTable = pWMVDec->m_rgiClapTabDec;

    pLhdec = pWMVDec->m_pAltTables->m_paLH;
    pBitstrm = pWMVDec->m_pbitstrmIn;
    pReconBuf = pWMVDec->m_rgiCoefReconBuf->i16;

    FUNCTION_PROFILE_STOP(&fpX8[0])

    /* Loop */
    for (blockY = 0; blockY < (I32_WMV) pWMVDec->m_uintNumMBY << 1; blockY++) {
        U8_WMV __huge* ppxliCodedY = ppxliCurrY + (blockY * (pWMVDec->m_iWidthPrevY << 3));
        U8_WMV __huge* ppxliCodedU = ppxliCurrU + (blockY >> 1) * (pWMVDec->m_iWidthPrevUV << 3);
        U8_WMV __huge* ppxliCodedV = ppxliCurrV + (blockY >> 1) * (pWMVDec->m_iWidthPrevUV << 3);
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("BBY_LoopCount");

        for (blockX = 0; blockX < (I32_WMV) pWMVDec->m_uintNumMBX << 1; blockX++) {
            /* Get orientation context/prediction */
            I32_WMV predOrient, *order, iOrient, iDiffOrient, estRunMin;
            Bool_WMV bSendOrient;
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT("BBX_LoopCount");

            t_ContextGetDec (pWMVDec->m_pContext, blockX, blockY, pWMVDec->m_iStepSize, &predOrient, &estRunMin);

            // Figure out whether to get differential orient
            bSendOrient = SetupSpatialPredictor (pWMVDec->m_pSp, ppxliCodedY, pWMVDec->m_iWidthPrevY, blockX, blockY,
                               pWMVDec->m_uintNumMBX << 1, &predOrient, &order, pWMVDec->m_iDCStepSize, FALSE);

            // Decode differential orientation
            iDiffOrient = 0;
            iOrient = predOrient;
            if (bSendOrient) {
                iDiffOrient = LhdecDecodeOrient (pWMVDec, pLhdec[LH_ORIENT], pBitstrm, &status);
                if((status)!=WMV_Succeeded) goto Error;
                iOrient = order[iDiffOrient];
            }

            if (decodeI_X8block(pWMVDec, pLhdec, pBitstrm, pReconBuf, pWMVDec->m_iWidthPrevY,
                pWMVDec->m_iDCStepSize, blockX, blockY, ppxliCodedY,
                estRunMin, iDiffOrient, iOrient, iInvDCStep, FALSE, &status))
                goto Error;

            // Move output poI32_WMVer
            ppxliCodedY += 8;

            ////////////
            //  COLOR
            ////////////
            if (blockX & blockY & 1) {
                /* Find the prediction direction, and scan direction */
                I32_WMV iOrient0 = iOrient = t_ContextGetChromaDec (pWMVDec->m_pContext, blockX, blockY);

                SetupSpatialPredictor (pWMVDec->m_pSp, ppxliCodedU, pWMVDec->m_iWidthPrevUV, blockX>>1, blockY>>1,
                    pWMVDec->m_uintNumMBX, &iOrient, &order, pWMVDec->m_iDCStepSizeC, TRUE);
        
                if (decodeI_X8block(pWMVDec, pLhdec, pBitstrm, pReconBuf, pWMVDec->m_iWidthPrevUV,
                    pWMVDec->m_iDCStepSizeC, blockX, blockY, ppxliCodedU, 0xff, 0,
                    iOrient, iInvDCStepC, TRUE, &status))
                    goto Error;

                SetupSpatialPredictor (pWMVDec->m_pSp, ppxliCodedV, pWMVDec->m_iWidthPrevUV, blockX>>1, blockY>>1,
                    pWMVDec->m_uintNumMBX, &iOrient0, &order, pWMVDec->m_iDCStepSizeC, TRUE);

                if (decodeI_X8block(pWMVDec, pLhdec, pBitstrm, pReconBuf, pWMVDec->m_iWidthPrevUV,
                    pWMVDec->m_iDCStepSizeC, blockX, blockY, ppxliCodedV, 0xff, 0,
                    iOrient0, iInvDCStepC, TRUE, &status))
                    goto Error;

                // bump up poI32_WMVers
                ppxliCodedU += 8;
                ppxliCodedV += 8;
            }  // end of chroma block
        } // end of loop over blockX
    }  // end of loop over blockY
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT("BB_Loop_Done");

    // deblocking and color mapping operations
    if (pWMVDec->m_bDeblockOn) {
        pWMVDec->m_iRefreshDisplay_AllMB_Cnt = 0;
        pWMVDec->m_bCopySkipMBToPostBuf = FALSE_WMV;
    }
    else
        pWMVDec->m_bCopySkipMBToPostBuf = TRUE_WMV;


        pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;


#ifdef  _WMV_TARGET_X86_
    if (g_bSupportMMX_WMV) {
        _asm  emms
    }
#endif // _WMV_TARGET_X86_

    if (!pWMVDec->m_bLoopFilter)
        memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);


    FUNCTION_PROFILE_STOP(&fpX8[1]);
    return WMV_Succeeded;

Error:
#ifdef  _WMV_TARGET_X86_
    if (g_bSupportMMX_WMV) {
        _asm  emms
    }
#endif 

    FUNCTION_PROFILE_STOP(&fpX8[1]);

    if(status != WMV_Succeeded) 
        return status;

    return WMV_Failed;
}


/****************************************************************************************
  setupZeroPredictor_C_fast : sets up boundary arrays for zero prediction mode
  "un-mmx'd" C version
****************************************************************************************/

Void_WMV SetupZeroPredictor_C_fast_Part (U16_WMV *pu16TSum,U16_WMV *pu16LSum,U16_WMV *pi16S1,U16_WMV *pi16S3)
{
	int j;
    for( j=10; j>0; j-- ) {
        U16_WMV **ppu16XSum = (j&1) ? &pu16TSum : &pu16LSum;
        U16_WMV i16S1_0 = (U16_WMV)(*pi16S1++);
        U16_WMV i16S1_1 = (U16_WMV)(*pi16S1++);
        U16_WMV i16S3_0 = (U16_WMV)(*pi16S3++);
        U16_WMV i16S3_1 = (U16_WMV)(*pi16S3++);

        *(*ppu16XSum)++ = i16S1_1 + (i16S3_1 >> 1) + (((i16S1_0 + i16S3_0) * 181 + 128) >> 8);
        *(*ppu16XSum)++ = i16S3_0 + (i16S1_0 >> 1) + (((i16S1_1 + i16S3_1) * 181 + 128) >> 8);
    }
}

static Void_WMV SetupZeroPredictor_C_fast (t_CSpatialPredictor *pSp)
{
    // fewer temporaries to save/restor to stack, smaller code size
    // rename variables using Hungrian.
    // Note 46% of the X8 blocks in CE_320_240_06f_WMV2_X8 call this routine
    I16_WMV *pi16Top  = (pSp->m_pBuffer16 + 12);
    I16_WMV *pi16Left = pSp->m_pBuffer16;
    I32_WMV j;
    U32_WMV pi32aS1[10], pi32aS3[10];   // space for two at a time - aligned 32
    U32_WMV *pi32Top;
    U32_WMV *pi32Left;
    U32_WMV *pi32S1;   
    U32_WMV *pi32S3;
    U32_WMV iTopAcc, iLeftAcc;
    U16_WMV *pi16S1;                              // S1[0]
    U16_WMV *pi16S3;                              // S3[0]
    U16_WMV *pu16TSum;
    U16_WMV *pu16LSum;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetupZeroPredictor_C_fast);

	{
        U8_WMV *ppxlTop = pSp->m_pTop, *ppxlLeft = pSp->m_pLeft-1;
        I16_WMV    *pi16TopT = pi16Top, *pi16LeftT = pi16Left;

        for( j=8; j>0;  j-- ) {
            *pi16TopT++  = (I16_WMV) (*ppxlTop++);                   // [0,1,...,7]
            *pi16LeftT++ = (I16_WMV) (*ppxlLeft--);
        }
        for( j=4; j>0; j-- ) {
            *pi16TopT++  = (I16_WMV) (*ppxlTop++);                   // [8,9,10,11]
            *pi16LeftT++ = 0;
        }
    }

    pi32aS1[0] = pi32aS1[1] = 0;
    pi32Top  = (U32_WMV*)pi16Top;
    pi32Left = (U32_WMV*)pi16Left;
    pi32S1 = pi32aS1 + 2;   
    pi32S3 = pi32aS3 + 9;
    *pi32S1++ = iLeftAcc = ((*pi32Left++) << 4);                // 1[2] = f( L[0] )
    *pi32S1++ = iTopAcc  = ((*pi32Top++)  << 4);                // 1[3] = f( T[0] )

    for( j=3; j>0; j-- ) {
        iLeftAcc >>= 1;
        iTopAcc  >>= 1;
        *pi32S1++ = iLeftAcc += ((*pi32Left++) << 4);           // 1[4,6,8] = f( LeftAcc, L[1,2,3] )
        *pi32S1++ = iTopAcc  += ((*pi32Top++)  << 4);           // 1[5,7,9] = f( TopAcc,  T[1,2,3] )
    }

    *pi32S3-- = (pi32Top[0] << 4) + (pi32Top[1] << 3);          // 3[9] = f( T[4], T[5] )
    *pi32S3-- = 0;                                              // 3[8] = 0
     pi32Top--;
    *pi32S3-- = (iTopAcc = pi32Top[0] << 4) + (pi32Top[1] << 3);// 3[7] = f( T[3], T[4] )
    *pi32S3-- = iLeftAcc = ((*--pi32Left) << 4);                // 3[6] = f( L[3] )

    for( j=3; j>0; j-- ) {
        iLeftAcc >>= 1;
        iTopAcc  >>= 1;
        *pi32S3-- = iTopAcc  += ((*--pi32Top)  << 4);           // 3[5,3,1] = f( TopAcc,  T[2,1,0] )
        *pi32S3-- = iLeftAcc += ((*--pi32Left) << 4);           // 3[4,2,0] = f( LeftAcc, L[2,1,0] )
    }

    pi16S1 = (U16_WMV*)(pi32aS1);                              // S1[0]
    pi16S3 = (U16_WMV*)(pi32aS3);                              // S3[0]
    pu16TSum = pSp->m_pTopSum - 1;
    pu16LSum = pSp->m_pLeftSum - 1;

	/* shao wei 20090729. There is unknown bug with these loop code when optimization level is o3. Packing them into function can fix this problem. */
#if 1
	SetupZeroPredictor_C_fast_Part(pu16TSum,pu16LSum,pi16S1,pi16S3);
#else
    for( j=10; j>0; j-- ) {
        U16_WMV **ppu16XSum = (j&1) ? &pu16TSum : &pu16LSum;
        U32_WMV i16S1_0 = (U32_WMV)(*pi16S1++);
        U32_WMV i16S1_1 = (U32_WMV)(*pi16S1++);
        U32_WMV i16S3_0 = (U32_WMV)(*pi16S3++);
        U32_WMV i16S3_1 = (U32_WMV)(*pi16S3++);

		U32_WMV a = (i16S1_0 + i16S3_0) * 181 + 128;
		U32_WMV b = (i16S1_1 + i16S3_1) * 181 + 128;
		U32_WMV a2 = (U32_WMV)(a >> 8);
		U32_WMV b2 = (U32_WMV)(b >> 8);

        *(*ppu16XSum)++ = (U16_WMV)(i16S1_1 + (i16S3_1 >> 1) + a2);
        *(*ppu16XSum)++ = (U16_WMV)(i16S3_0 + (i16S1_0 >> 1) + b2);
    }
#endif

}

/****************************************************************************************
  Spatial prediction function
  The predictors are (being) optimized.  For more readable code, refer to
  spatialpredictor.cpp v5, in Scrunch\wmv2.
****************************************************************************************/
// for case 9
static const I8_WMV gaPredCase9Index[16] = { -2, -3, -4, -5, -6, -7, -8, -8,
        -8, -8, -8, -8, -8, -8, -8, -8};
// for case 1 
static const I8_WMV gaPredCase1Index[24] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 
        9, 10, 11, 12, 13, 14, 15, 15, 15, 15, 15, 15, 15, 15, 15};

Void_WMV predictDec(t_CSpatialPredictor *pSp, const I32_WMV iOrient,
                    U8_WMV *pRef, const I32_WMV iRefStride, I16_WMV *pDelta, Bool_WMV bClear)
{
    I32_WMV  i, j, k, iStridem8, tmp, iPred;
    const U8_WMV  *pVal;
    I16_WMV  *pDeltaOrig = pDelta;
    FUNCTION_PROFILE_DECL_START(fp,DECODEIX8PRED_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(predictDec);

    iStridem8 = iRefStride-8;

    if (pSp->m_bFlat) {      // 2% of the time take this case on CE_320_240_06f_WMV2_300kX8
        // FUNCTION_PROFILE_COUNT(DECODEIX8PREDDEC_FLAT_COUNT);
        const I32_WMV iDcValue = pSp->m_iDcValue;    // hI32_WMV compiler
        for( i=8; i>0; i-- ) {
            for( j=8; j>0; j-- ) {
                *pRef++ = pSp->m_pClampTable[iDcValue + *pDelta++]; 
            }
            pRef += iStridem8; 
        }
        if (bClear)
            goto Clear;
        else {
            FUNCTION_PROFILE_STOP(&fp);
            return;
        }
    }

    // FUNCTION_PROFILE_COUNT(DECODEIX8PREDDEC_CASE0_COUNT+iOrient);
    switch(iOrient) {
    case 0:         // 41% of the time take this case on CE_320_240_06f_WMV2_300kX8

		SetupZeroPredictor_C_fast (pSp);

        {
            U16_WMV *pWts = aNullPredWts_WMV;
            U16_WMV *pLeftSum = pSp->m_pLeftSum;
            for( i=8; i>0; i-- ) {
                U16_WMV u16LSum = *pLeftSum++;
                U16_WMV* pu16TSum = pSp->m_pTopSum;
                for( j=8; j>0; j--) {
                    // sophisticated smoothing prediction
                    iPred = (*pu16TSum++ * pWts[0] + u16LSum * pWts[1] + 0x8000) >> 16;
                    *pRef++ = pSp->m_pClampTable[iPred + *pDelta++]; 
                    pWts += 2;
                }
                pRef += iStridem8;
            }
        }
        break;
    case 8:         // 16% of the time take this case on CE_320_240_06f_WMV2_300kX8
        {
            U8_WMV *pLeft = pSp->m_pLeft-1;
            pVal = pSp->m_pRef - 2;
            for( i=8; i>0; i-- ) {
                iPred = (*pLeft-- + pVal[0] + 1) >> 1;
                for( j=8; j>0; j-- ) {
                    *pRef++ = pSp->m_pClampTable[iPred + *pDelta++];
                }
                pVal += pSp->m_iRefStride;
                pRef += iStridem8;
            }
        }
        break;
    case 4:         //  9% of the time take this case on CE_320_240_06f_WMV2_300kX8
        {
            U8_WMV tmp8[8], *ptmp8 = tmp8, *pTopj = pSp->m_pTop, *pTopj16 = pSp->m_pTop+16;
            for (j=8; j>0; j--)
                *ptmp8++ = (*pTopj++ + *pTopj16++ + 1) >> 1;
            for( i=8; i>0; i-- ) {
                pVal = tmp8;
                for( j=8; j>0; j-- ) {
                    *pRef++ = pSp->m_pClampTable[*pVal++ + *pDelta++]; 
                }
                pRef += iStridem8;
            }
        }
        break;
    case 10:         //  9% of the time take this case on CE_320_240_06f_WMV2_300kX8
        {   // decaying horizontal continuity
            I32_WMV  tmp8[8], *ptmp8 = tmp8;
            U8_WMV *pLeft = pSp->m_pLeft - 1, *pTopj = pSp->m_pTop;
            for (j = 0; j < 8; j++) {
                *ptmp8++ = (*pTopj++) * j + 4;
            }
            for( i=8; i>0; i-- ) {
                I32_WMV *pTmp8 = tmp8;
                I32_WMV iLeft = *pLeft--;
                I32_WMV iAcc = iLeft << 3;
                for( j=8; j>0; j-- ) {
                    iPred = (iAcc + *pTmp8++) >> 3;
                    iAcc -= iLeft;
                    *pRef++ = pSp->m_pClampTable[iPred + *pDelta++];
                }
                pRef += iStridem8;
            }
        }
        break;
    case 11:          //  9% of the time take this case on CE_320_240_06f_WMV2_300kX8
        {   // decaying vertical continuity
            U8_WMV  *pLeft = pSp->m_pLeft - 1, *pTop = pSp->m_pTop;
            I32_WMV pTmp[8], *pTmp8 = pTmp;
            for (j=8; j>0; j--) {
                *pTmp8++ = (*pTop++) << 3;
            }
            for (i = 0; i < 8; i++ ) {
                I32_WMV     *pTmp8 = pTmp;
                U8_WMV  *pTop  = pSp->m_pTop;
                tmp = (*pLeft--) * i + 4;
                for( j=8; j>0; j-- ) {
                    iPred     = (*pTmp8  + tmp) >> 3; 
                    *pTmp8++ -= *pTop++;
                    *pRef++   = pSp->m_pClampTable[iPred + *pDelta++]; 
                }                
                pRef += iStridem8;
            }
        }
        break;
    case 1:
        {
            const I8_WMV *piPredCase1Index = gaPredCase1Index + 2;
            for( i=8; i>0; i-- ) {
                for( j=8; j>0; j-- ) {
                    *pRef++ = pSp->m_pClampTable[pSp->m_pTop[*piPredCase1Index++] + *pDelta++];
                }
                pRef += iStridem8;
                piPredCase1Index -= 6;
            }
        }
        break;
    case 2:   
        for (i = 0; i < 8; i++) {
            pVal = pSp->m_pTop + i + 1;
            for( j=8; j>0; j-- ) {
                *pRef++ = pSp->m_pClampTable[*pVal++ + *pDelta++]; 
            }
            pRef += iStridem8;
        }
        break;
    case 3:
        for (i=0; i<8; i++) {
            pVal = pSp->m_pTop + ((i + 1) >> 1);
            for( j=8; j>0; j-- ) {
                *pRef++ = pSp->m_pClampTable[*pVal++ + *pDelta++]; 
            }
            pRef += iStridem8;
        }
        break;
    case 5:
        {
            U8_WMV *pTop;
            for( i=0; i<8; i++ ) {
                k = 1-i;
                for( j=8; k<=0; j--, k+=2 ) {
                    *pRef++ = pSp->m_pClampTable[pSp->m_pLeft[k] + *pDelta++]; 
                }
                pTop = pSp->m_pTop;
                for( ; j>0; j-- ) {
                    *pRef++ = pSp->m_pClampTable[*pTop++    + *pDelta++]; 
                }
                pRef += iStridem8;
            }
        }
        break;
    case 7:
        {
            U8_WMV  *pLeft, *pTop;
            I32_WMV iPrior, iThis;
            for (i = 0; i < 4; i++) {
                // TODO - Add an OPT_TINY_CODE_SIZE Option
                pLeft = pSp->m_pLeft - i;
                for( j = i; j>0; j-- ) {                                // i==1             i==2 
                    *pRef++ = pSp->m_pClampTable[*pLeft   + *pDelta++];      // R[0] = f(L[-i]); R[0,2] = f(L[-i,-i+1])
                    *pRef++ = pSp->m_pClampTable[*pLeft++ + *pDelta++];      // R[1] = f(L[-i]); R[1,3] = f(L[-i,-i+1])
                }
                *pRef++ = pSp->m_pClampTable[pSp->m_pLeft[0] + *pDelta++];        // R[2*i] = f(L[0])
                pTop = pSp->m_pTop - 1;
                iPrior = *pTop++;
                for( j = 8-1-(i<<1); j>0; j--, iPrior = iThis ) {
                    iPred = (iPrior + (iThis = *pTop++) + 1)>>1;
                    *pRef++ = pSp->m_pClampTable[iPred + *pDelta++];         // R[2*i+1,...] = f(T[-1],T[0]),f(T[0],T[1]),...
                }
                pRef += iStridem8;
            }
            for ( ; i < 8; i++) {
                pLeft = pSp->m_pLeft - i;
                for( j=4; j>0; j-- ) {
                    *pRef++ = pSp->m_pClampTable[*pLeft   + *pDelta++];
                    *pRef++ = pSp->m_pClampTable[*pLeft++ + *pDelta++];
                }
                pRef += iStridem8;
            }
        }
        break;
    case 6:
        for (i=0; i<8; i++) {
            pVal = pSp->m_pTop - i - 1;
            for( j=8; j>0; j-- ) {
                *pRef++ = pSp->m_pClampTable[*pVal++ + *pDelta++]; 
            }
            pRef += iStridem8;
        }
        break;
    case 9:
        for (i = 0; i < 8; i++) {
            const I8_WMV *piPredCase9Index = gaPredCase9Index + i;
            for( j=8; j>0; j-- ) {
                *pRef++ = pSp->m_pClampTable[pSp->m_pLeft[*piPredCase9Index++] + *pDelta++]; 
            }
            pRef += iStridem8;
        }
        break;
    }
    if (!bClear) {
        FUNCTION_PROFILE_STOP(&fp);
        return;
    }

Clear:
    ALIGNED32_MEMCLR_128U8(pDeltaOrig);
    FUNCTION_PROFILE_STOP(&fp);
    return;
}

/****************************************************************************************
  Spatial prediction function for decoder : no residual
****************************************************************************************/
#ifndef SMALL_CODE_SIZE
Void_WMV predict_0(t_CSpatialPredictor *pSp, const I32_WMV iOrient,
                   U8_WMV *pRef, const I32_WMV iRefStride)
{
    I32_WMV  i, j, k, tmp;
    const U8_WMV  *pVal;
    const I32_WMV iRefStridem8 = iRefStride - 8;
#   ifdef OPT_TINY_CODE_SIZE
        I32_WMV iPred;
#   endif
    FUNCTION_PROFILE_DECL_START(fp,DECODEIX8PRED0_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(predict_0);

    if (pSp->m_bFlat) {
        const U32_WMV u32Val = ALIGNED32_REPLICATE_4U8(pSp->m_iDcValue);
        // FUNCTION_PROFILE_COUNT(DECODEIX8PRED0_FLAT_COUNT);
        for( i=8; i>0; i-- ) {
            ALIGNED32_MEMSET_2U32( pRef, u32Val );
            pRef += iRefStride;
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    }

    // FUNCTION_PROFILE_COUNT(DECODEIX8PRED0_CASE0_COUNT+iOrient);
    switch(iOrient) {
    case 0:          //  5% of the time take this case on CE_320_240_06f_WMV2_300kX8
        {
#ifdef _WMV_TARGET_X86_
            if (g_bSupportMMX_WMV) {
                SetupZeroPredictor_MMX (pSp);
                predict_0_MMX (pSp->m_pTopSum, pSp->m_pLeftSum, aNullPredWts_WMV, pRef, iRefStride);
            }
            else
#endif // _WMV_TARGET_X86_
            {
                U16_WMV  iLSum, *pTSum;
                U16_WMV  *pWts = aNullPredWts_WMV;

                SetupZeroPredictor_C_fast (pSp);
                for (i=0; i<8; i++) {
                    iLSum = pSp->m_pLeftSum[i];
                    pTSum = pSp->m_pTopSum;
                    for (j=8; j>0; j--, pWts += 2) {
                        // sophisticated smoothing prediction
                        *pRef++ = (U8_WMV)(((*pTSum++) * pWts[0] + iLSum * pWts[1] + (1<<15)) >> 16);
                    }
                    pRef += iRefStridem8;
                }
                FUNCTION_PROFILE_STOP(&fp);
                return;
            }
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 8:
        {
            pVal = pSp->m_pRef - 2;
            U8_WMV  *pLeft = pSp->m_pLeft - 1;
            for (i=0; i<8; i++) {
                tmp = (*pLeft-- + pVal[0] + 1) >> 1;
                ALIGNED32_MEMSET_8U8( pRef, tmp );
                pRef += iRefStride;
                pVal += pSp->m_iRefStride;
            }
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 4:
        {
            U32_WMV    tmpU32[2];
            U8_WMV *tmp8 = (U8_WMV *) tmpU32;
            for (j=0; j<8; j++)
                tmp8[j] = (pSp->m_pTop[j] + pSp->m_pTop[j + 8 + 8] + 1) >> 1;
            for( j=8; j>0; j-- ) {
                ALIGNED32_MEMCPY8( pRef, tmp8 );
                pRef += iRefStride;
            }
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 10: // decaying horizontal continuity
        {
            I32_WMV  tmp8[8];
            for (j = 0; j < 8; j++)
                tmp8[j] = pSp->m_pTop[j] * j + 4;
            for (i=0; i<8; i++) {
                tmp = pSp->m_pLeft[- 1 - i];
                I32_WMV iAcc = tmp << 3;
                I32_WMV *pTmp = tmp8;
                for ( j=8; j>0; j-- ) {
                    *pRef++ = (iAcc + (*pTmp++)) >> 3;
                    iAcc -= tmp;
                }
                pRef += iRefStridem8;
            }
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 11: // decaying vertical continuity
        {
            U8_WMV  *pLeft = pSp->m_pLeft - 1;
            for (i = 0, k = 8; i < 8; i++, k--) {
                tmp = (*pLeft--) * i + 4;
                U8_WMV  *pTop = pSp->m_pTop;
                for( j=8; j>0; j-- ) {
                    *pRef++ = ((*pTop++) * k + tmp) >> 3;
                }
                pRef += iRefStridem8;
            }
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 1:
#       ifdef OPT_TINY_CODE_SIZE
            for (i=0; i<8; i++) {
                k = i + i + 2;
                for (j=0; j<8; j++, k++) {
                    *pRef++ = pSp->m_pTop[(k > 15) ? 15:k];
                }
                pRef += iRefStridem8;
            }
#       else   // OPT_TINY_CODE_SIZE
            for (i=0; i<8; i++) {
                k = i + i + 2;
                U8_WMV *pTop = pSp->m_pTop + (k>15?15:k);
                k = k<8 ? 0 : (k-7);
                for( j=8; j>k; j--) {
                    *pRef++ = *pTop++;
                }
                const U8_WMV cT = *pTop;
                for(    ; j>0; j-- ) {
                    *pRef++ = cT;
                }
                pRef += iRefStridem8;
            }
#       endif
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 2:
        for (i=0; i<8; i++) {
            pVal = pSp->m_pTop + i + 1;
#ifdef _WMV_TARGET_X86_  // unaligned access
                U32_WMV  *pSrc = (U32_WMV *)pVal;
                U32_WMV  *pDst = (U32_WMV *)(pRef + i * iRefStride);
                *pDst++ = *pSrc++;
                *pDst   = *pSrc;
#else
                for ( j=8; j>0; j-- ) {
                    *pRef++ = *pVal++;
                }
                pRef += iRefStridem8;
#endif // _WMV_TARGET_X86_
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 3:
        for (i=0; i<8; i++) {
            pVal = pSp->m_pTop + ((i + 1) >> 1);
            for (j=0; j<8; j++) {
                *pRef++ = *pVal++;
            }
            pRef += iRefStridem8;
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 5:
#ifdef OPT_TINY_CODE_SIZE
            for (i=0; i<8; i++) {
                for (j=0; j<8; j++) {
                    k = j-((i+1)>>1);
                    if (k>=0) iPred = pSp->m_pTop[k];
                    else iPred = pSp->m_pLeft[j+j-i+1];
                    *pRef++ = iPred;
                }
                pRef += iRefStridem8;
            }
#       else    // OPT_TINY_CODE_SIZE
            for (i=0; i<8; i++) {
                U8_WMV *pLT = pSp->m_pLeft-i+1;
                k = 8-((i+1)>>1);                // k=8-[0,-1,-1,-2,-2,-3,-3,-4]
                for (j=8; j>k; j--) {
                    *pRef++ = *pLT;  pLT += 2;
                }
                pLT = pSp->m_pTop;
                for (    ; j>0; j-- ) {
                    *pRef++ = *pLT++;
                }
                pRef += iRefStridem8;
            }
#       endif   // OPT_TINY_CODE_SIZE
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 6:
        for (i=0; i<8; i++) {
            pVal = pSp->m_pTop - i - 1;
#           ifdef _WMV_TARGET_X86_  // unaligned access
                U32_WMV  *pSrc = (U32_WMV *)pVal;
                U32_WMV  *pDst = (U32_WMV *)pRef;
                *pDst++ = *pSrc++;
                *pDst   = *pSrc;
                pRef += iRefStride;
#           else
                for ( j=8; j>0; j-- ) {
                    *pRef++ = *pVal++;
                }
                pRef += iRefStridem8;
#           endif // _WMV_TARGET_X86_
        }
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 7:
#       ifdef OPT_TINY_CODE_SIZE
            for (i=0; i<8; i++) {
                for (j=0; j<8; j++) {
                    k = j-i-i-2;
                    if (k>=-1) iPred = (pSp->m_pTop[k]+pSp->m_pTop[k+1]+1)>>1;
                    else iPred = pSp->m_pLeft[(j >> 1) - i];
                    *pRef++ = iPred;
                }
                pRef += iRefStridem8;
            }
#       else   // OPT_TINY_CODE_SIZE
            for (i=0; i<8; i++) {
                U8_WMV *pLeft = pSp->m_pLeft - i;
                k = i<4 ? 8-(i<<1) : 0;                 // k = 10+[ -2, -4, -6, -8, -10, -12, -14, -16]
                for( j=8; j>k; j-=2 ) {
                    *pRef++ = *pLeft;                               // j==even, k==-even
                    *pRef++ = *pLeft++;                             // j==odd,  k==-odd.
                }
                if ( j>0 ) {
                    *pRef++ = *pLeft;                               // k==-2
                    j--;
                    U8_WMV *pTop = pSp->m_pTop - 1;                      // k==-1
                    I32_WMV iTk0 = *pTop++, iTk1;
                    for (    ; j>0; j--, iTk0 = iTk1) {
                        *pRef++ = ( iTk0 + (iTk1=*pTop++) + 1 )>>1;
                    }
                }
                pRef += iRefStridem8;
            }
#       endif  //OPT_TINY_CODE_SIZE
        FUNCTION_PROFILE_STOP(&fp);
        return;
    case 9:
#       ifdef OPT_TINY_CODE_SIZE
            for (i = 0; i < 8; i++) {
                k = - i - 2;
                for (j=0; j<8; j++, k--) {
                    *pRef++ = pSp->m_pLeft[(k > -8) ? k : -8];
                }
                pRef += iRefStridem8;
            }
#       else   // OPT_TINY_CODE_SIZE
            for (i = 0; i < 8; i++) {
                U8_WMV *pLeft = pSp->m_pLeft -2 -i;
                k = 1 + i;  
                for ( j=8; j>k; j--) {
                    *pRef++ = *pLeft--;
                }
                const U8_WMV pxlL8 = pSp->m_pLeft[-8];
                for (   ; j>0; j-- ) {
                    *pRef++ = pxlL8;
                }
                pRef += iRefStridem8;
            }
#       endif  // OPT_TINY_CODE_SIZE
        FUNCTION_PROFILE_STOP(&fp);
        return;
    }
    FUNCTION_PROFILE_STOP(&fp);
}
#endif // SMALL_CODE_SIZE
#endif // WMV9_SIMPLE_ONLY

//#pragma warning(default: 4799)  // reset emms warning disable

/****************************************************************************************
  End of decoder specific functions
*****************************************************************************************/
