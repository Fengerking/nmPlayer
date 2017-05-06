//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

    blkdec.cpp

Abstract:

    block decoding functions.

Author:

    Wei-ge Chen (wchen@microsoft.com) 4-Sept-1996
    Bruce Lin (blin@microsoft.com) 02-Nov-1996

Revision History:

*************************************************************************/

#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "wmvdec_api.h"

#include <math.h>
#ifdef _TI_C55X_
//#include <string.h>
#endif
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "codehead.h"
#include "strmdec_wmv.h"
#include "idctdec_wmv2.h"

#ifdef __MFC_
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW               
#endif // __MFC_
//#define MAXCODEWORDLNGIntraDCy 7
//#define MAXCODEWORDLNGIntraDCc 8

#ifdef WMV_OPT_DQUANT_ARM
        extern Void_WMV Pack32To16(void* pDst,void* pSrc);
        extern Void_WMV ResetCoefACPred1(void* pDst,void* pSrc);
        extern Void_WMV ResetCoefACPred2(void* pDst,void* pSrc);
#endif

#ifdef _XBOX
extern void CalcrgiCoef(short* pDct,int* CoefRecon,int iDoubleStepSize,int iStep);
#endif
#ifdef _WMV_TARGET_X86_
extern void Pack32To16(void* pDst,void* pSrc);
#endif


//#pragma warning(disable: 4799)  // emms warning disable
tWMVDecodeStatus decodeIntraBlockAcPred (
    tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
    CWMVMBMode*        pmbmd,
    U8_WMV   *ppxliTextureQMB,        //place to set the pixels
    I32_WMV             iOffsetToNextRowForDCT,
    I32_WMV             ib, 
    I16_WMV*            piQuanCoefACPred, 
    I16_WMV*            pRef, 
    Bool_WMV            bPredPattern,
    I32_WMV             iMBX
)
{
    tWMVDecodeStatus tWMVStatus;

    //Bool_WMV bPredPattern;
    U8_WMV* piZigzagInv; 
#ifdef _WMV_TARGET_X86_
    U8_WMV *plus1;
#endif
    FUNCTION_PROFILE(fpIDCT)
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeIntraBlockAcPred);
    assert(MB_SQUARE_SIZE - 4 <= UINT_MAX);
#ifndef _WMV_TARGET_X86_
    ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefReconPlus1, 0, (size_t)(MB_SQUARE_SIZE - 4));
#else
    if(!g_bSupportMMX_WMV)
        ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefReconPlus1, 0, (size_t)(MB_SQUARE_SIZE - 4));
    else {
        ALIGNED32_FASTMEMCLR_MMX (pWMVDec->m_rgiCoefReconPlus1, (size_t)(MB_SQUARE_SIZE - 32));
        plus1 = (U8_WMV *)pWMVDec->m_rgiCoefReconPlus1 + (MB_SQUARE_SIZE - 32);
        memset (plus1, 0, 28);
    }
#endif

    if ((pmbmd->m_rgbDCTCoefPredPattern2[ALLBLOCK])){
        if (bPredPattern != pWMVDec->m_bRotatedIdct){
#ifndef     WMV_OPT_DQUANT_ARM
            I32_WMV i;
            for (i = 1; i < 8; i++) {
                piQuanCoefACPred [i + BLOCK_SIZE] = 0;
                piQuanCoefACPred [i] = pRef [i] ;
            }
#else
            ResetCoefACPred1 (piQuanCoefACPred, pRef);
#endif
            //piZigzagInv = g_pHorizontalZigzagInv; Old Symmetric matrix, 
            if (pWMVDec->m_bRotatedIdct) //bPredPattern == 0, pWMVDec->m_bRotatedIdct == 1 
                piZigzagInv = (U8_WMV*) pWMVDec->m_pVerticalZigzagInvRotated;
            else //bPredPattern == 1, pWMVDec->m_bRotatedIdct == 0 
                piZigzagInv = (U8_WMV*) pWMVDec->m_pHorizontalZigzagInv;
        }
        else{
#ifndef     WMV_OPT_DQUANT_ARM
            I32_WMV i;
            for (i = 1; i < 8; i++) {
                piQuanCoefACPred [i + BLOCK_SIZE] = pRef [i + BLOCK_SIZE];
                piQuanCoefACPred [i] = 0;
            }
#else
            ResetCoefACPred2 (piQuanCoefACPred, pRef);
#endif
            // piZigzagInv = g_pVerticalZigzagInv; Old Symmetric matrix, 
            if (pWMVDec->m_bRotatedIdct) //bPredPattern == 1 , pWMVDec->m_bRotatedIdct == 1
                piZigzagInv = (U8_WMV*) pWMVDec->m_pHorizontalZigzagInvRotated;
            else //bPredPattern == 0, pWMVDec->m_bRotatedIdct == 0
                piZigzagInv = (U8_WMV*) pWMVDec->m_pVerticalZigzagInv;
        }
    }
    else{
        piQuanCoefACPred [1] = 0;
        piQuanCoefACPred [1 + BLOCK_SIZE] = 0;
        piQuanCoefACPred [2] = 0;
        piQuanCoefACPred [2 + BLOCK_SIZE] = 0;
        piQuanCoefACPred [3] = 0;
        piQuanCoefACPred [3 + BLOCK_SIZE] = 0;
        piQuanCoefACPred [4] = 0;
        piQuanCoefACPred [4 + BLOCK_SIZE] = 0;
        piQuanCoefACPred [5] = 0;
        piQuanCoefACPred [5 + BLOCK_SIZE] = 0;
        piQuanCoefACPred [6] = 0;
        piQuanCoefACPred [6 + BLOCK_SIZE] = 0;
        piQuanCoefACPred [7] = 0;
        piQuanCoefACPred [7 + BLOCK_SIZE] = 0;
        if (pWMVDec->m_bRotatedIdct)
            piZigzagInv = pWMVDec->m_pZigzagInvRotated_I;
        else
            piZigzagInv = pWMVDec->m_pZigzagInv_I;
    }

    // reverse ib because of the internal representation of pWMVDec->m_rgbCodedBlockPattern
    if (pWMVDec->m_cvCodecVersion >= WMV3)
        tWMVStatus = DecodeInverseIntraBlockQuantizeAcPred (
            pWMVDec,
            ppIntraDCTTableInfo_Dec, 
#ifndef _WMV9AP_
            (pmbmd->m_rgbCodedBlockPattern & (1<<(5-ib))), 
#else
            pmbmd->m_rgbCodedBlockPattern2 [ib], 
#endif
            piQuanCoefACPred, 
            piZigzagInv
        );//get the quantized block 
    else
		if (pWMVDec->m_cvCodecVersion != MP4S)
			tWMVStatus = DecodeInverseIntraBlockQuantizeAcPred (
				pWMVDec,
				ppIntraDCTTableInfo_Dec, 
#ifndef _WMV9AP_
				(pmbmd->m_rgbCodedBlockPattern & (1<<(5-ib))), 
#else
				pmbmd->m_rgbCodedBlockPattern2 [ib], 
#endif
				piQuanCoefACPred, 
				piZigzagInv
			);//get the quantized block 

#ifndef WMV789_ONLY
		else
			tWMVStatus = DecodeInverseIntraBlockQuantizeAcPred_MPEG4 (
				pWMVDec,
				ppIntraDCTTableInfo_Dec, 
#ifndef _WMV9AP_
				(pmbmd->m_rgbCodedBlockPattern & (1<<(5-ib))), 
#else
				pmbmd->m_rgbCodedBlockPattern2 [ib], 
#endif
				piQuanCoefACPred, 
				piZigzagInv
				);//get the quantized block 
#endif
	    
    if (WMV_Succeeded != tWMVStatus){
        return tWMVStatus;
    }
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE);
#ifndef _WMV9AP_
    if (pWMVDec->m_iOverlap){
#else
    if (pWMVDec->m_iOverlap) {
#endif

        I32_WMV     iWidth1;
        I16_WMV     *pOut;
        I32_WMV     ii;

		// cast to 16 bit
		{
#if defined(WMV_OPT_DQUANT_ARM)
			Pack32To16(pWMVDec->m_rgErrorBlock->i16, pWMVDec->m_rgiCoefRecon);
#elif defined(_WMV_TARGET_X86_)
			if (g_bSupportMMX_WMV)
				Pack32To16(pWMVDec->m_rgErrorBlock->i16, pWMVDec->m_rgiCoefRecon);
			else {
				for (ii = 0; ii < 64; ii++)
					pWMVDec->m_rgErrorBlock->i16[ii] = (I16_WMV) pWMVDec->m_rgiCoefRecon[ii];
			}
#else   
			for (ii = 0; ii < 64; ii++)
				pWMVDec->m_rgErrorBlock->i16[ii] = (I16_WMV) pWMVDec->m_rgiCoefRecon[ii];
#endif
		}
#ifdef _EMB_SSIMD_IDCT_
		if (pWMVDec->m_b16bitXform && pWMVDec->m_cvCodecVersion != WMVA){
			SignPatch(pWMVDec->m_rgErrorBlock->i16, 64*2);
		}
#endif

		//Intra Block IDCT
		(*pWMVDec->m_pIntraX9IDCT_Dec) (pWMVDec->m_rgErrorBlock, pWMVDec->m_rgErrorBlock, BLOCK_SIZE, 0xff);//pWMVDec->m_iDCTHorzFlags);

#ifdef _WMV9AP_
		if (pWMVDec->m_iOverlap == 1 || pWMVDec->m_iOverlap == 3 || pmbmd->m_bOverlapIMB) {
#endif

			// overlapped transform copy
				if (ib < 4){
					iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 4;
					pOut = pWMVDec->m_pIntraBlockRow0[ib & 2] + (iMBX << 4) + ((ib & 1) << 3);
				} else {
					iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
					pOut = (ib == 4) ? pWMVDec->m_pIntraMBRowU0[0] : pWMVDec->m_pIntraMBRowV0[0];
					pOut += iMBX << 3;
				}
#ifdef WMV_OPT_COMMON_ARM
			FASTCPY_8x16(pOut, pWMVDec->m_rgErrorBlock->i16, iWidth1 << 1, BLOCK_SIZE << 1);
#else

			for (ii = 0; ii < 8; ii++){
				memcpy(pOut + ii * iWidth1,
					   pWMVDec->m_rgErrorBlock->i16 + ii * BLOCK_SIZE, 8 * sizeof(I16_WMV));
			}
#endif
	    
#ifdef _WMV9AP_
		 } else {
			for ( ii = 0; ii < 8; ii++)
					memset (ppxliTextureQMB + ii * iOffsetToNextRowForDCT, 128, 8 * sizeof(U8_WMV));
					pWMVDec->m_pAddError (ppxliTextureQMB, ppxliTextureQMB, pWMVDec->m_rgErrorBlock->i16, iOffsetToNextRowForDCT, pWMVDec->m_pcClapTable);
		 }
	        
#endif
    } 
	else {

#ifdef _EMB_SSIMD_IDCT_
        if (pWMVDec->m_b16bitXform && pWMVDec->m_cvCodecVersion != WMVA){
            SignPatch32_Fun((I32_WMV *)pWMVDec->m_rgiCoefRecon, 128);
        }
#endif
        (*pWMVDec->m_pIntraIDCT_Dec) (ppxliTextureQMB, iOffsetToNextRowForDCT, pWMVDec->m_rgiCoefRecon);
    }
    FUNCTION_PROFILE_STOP(&fpIDCT);
    return WMV_Succeeded;
}
//#pragma warning(default: 4799)  // emms warning disable



//Use 0-5 in decoder but Y_BLOCK1-V_BLOCK in encoder. the mess needs to be fixed.
I16_WMV* decodeDiffDCTCoef(tWMVDecInternalMember *pWMVDec,
                           I16_WMV** piQuanCoefACPredTable, 
                           Bool_WMV bLeftMB, 
                           Bool_WMV bTopMB, 
                           Bool_WMV bLeftTopMB, 
                           Bool_WMV* pbPredPattern)
{
//  I16_WMV* pDct = piQuanCoefACPred;
    //I16_WMV* pCoefRecon = pWMVDec->m_rgiCoefRecon;
    I16_WMV *pDctLeft, *pDctTop, *pDctLeftTop;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDiffDCTCoef);

    pDctLeft = piQuanCoefACPredTable[bLeftMB];
    pDctTop = piQuanCoefACPredTable[2+bTopMB];
    pDctLeftTop = piQuanCoefACPredTable[4+bLeftTopMB];

    if (abs (*pDctLeftTop - *pDctTop) < abs (*pDctLeftTop - *pDctLeft) + pWMVDec->m_iDCPredCorrect){
        *pbPredPattern = 0;
        COVERAGE_DCPRED (pWMVDec, CV_DCPRED_LEFT);
        return pDctLeft;
    }
    else {
        *pbPredPattern = 1;
        COVERAGE_DCPRED (pWMVDec, CV_DCPRED_LEFT);
        return pDctTop;
    }
}
Void_WMV decodeBitsOfESCCode (tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeBitsOfESCCode);

    if (pWMVDec->m_iStepSize >= 8 
#ifdef _WMV9AP_
                && (((pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP) && !pWMVDec->m_bAdvancedProfile)|| !pWMVDec->m_bDQuantOn)
#else
                && (pWMVDec->m_tFrmPredType == IVOP || !pWMVDec->m_bDQuantOn)
#endif
        ){
        I32_WMV iBitsCnt = 0, iBit = 0;
        while (iBitsCnt < 6 && !iBit){
            iBit = BS_getBit ( pWMVDec->m_pbitstrmIn);
            iBitsCnt++;
        }
        if (iBit)
            pWMVDec->m_iNUMBITS_ESC_LEVEL = iBitsCnt + 1; // from 2-7
        else
            pWMVDec->m_iNUMBITS_ESC_LEVEL = 8; // from 2-7
    }
    else{
        pWMVDec->m_iNUMBITS_ESC_LEVEL = BS_getBits ( pWMVDec->m_pbitstrmIn, 3);  // 1-7, 0 is ESC
        if (!pWMVDec->m_iNUMBITS_ESC_LEVEL){
            if (pWMVDec->m_cvCodecVersion >= WMV3) {
                pWMVDec->m_iNUMBITS_ESC_LEVEL = 8 + BS_getBits ( pWMVDec->m_pbitstrmIn, 2);
            } else {
            pWMVDec->m_iNUMBITS_ESC_LEVEL = 8 + BS_getBit ( pWMVDec->m_pbitstrmIn);  // ESC + 0 == 8  ESC + 1 == 9
            }
        }
    }

    pWMVDec->m_iNUMBITS_ESC_RUN = 3 + BS_getBits ( pWMVDec->m_pbitstrmIn, 2);

    COVERAGE_BLOCK_NUMBITSESC (pWMVDec);
}



#define WMV_ESC_Decoding()                                                          \
    if (pWMVDec->m_bFirstEscCodeInFrame){                                                    \
        decodeBitsOfESCCode (pWMVDec);                                                     \
        pWMVDec->m_bFirstEscCodeInFrame = FALSE_WMV;                                             \
    }                                                                               \
    uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);                               \
    if (BS_getBit ( pWMVDec->m_pbitstrmIn)) /* escape decoding */                              \
        iLevel = -1 * BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                 \
    else                                                                            \
        iLevel = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                      



// Sridhar:  This function isn't called (unles MMX stream) so don't bother for now
//#pragma warning(disable: 4799)  // emms warning disable
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode
                                                        , DQuantDecParam *pDQ
)
{
    CDCTTableInfo_Dec* InterDCTTableInfo_Dec = *ppInterDCTTableInfo_Dec;
    Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec->hufDCTACDec;
    I8_WMV* rgLevelAtIndx = InterDCTTableInfo_Dec->pcLevelAtIndx;
    U8_WMV* rgRunAtIndx = InterDCTTableInfo_Dec->puiRunAtIndx;
//  Change iStartIndxOfLastRun to iStartIndxOfLastRunMinus1 and <= to <
//  iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1;
    U8_WMV* rgIfNotLastNumOfLevelAtRun = InterDCTTableInfo_Dec->puiNotLastNumOfLevelAtRun;
    U8_WMV* rgIfLastNumOfLevelAtRun = InterDCTTableInfo_Dec->puiLastNumOfLevelAtRun; 
    U8_WMV* rgIfNotLastNumOfRunAtLevel = InterDCTTableInfo_Dec->puiNotLastNumOfRunAtLevel;
    U8_WMV* rgIfLastNumOfRunAtLevel = InterDCTTableInfo_Dec->puiLastNumOfRunAtLevel;
    U32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
    I32_WMV iTCOEF_ESCAPE = InterDCTTableInfo_Dec->iTcoef_ESCAPE;
    Bool_WMV bIsLastRun = FALSE_WMV;
    U32_WMV uiRun; // = 0;
    I32_WMV iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    U8_WMV  lIndex;
    register I32_WMV iDCTHorzFlags = 0;
    I32_WMV iDoubleStepSize, iStepMinusStepIsEven, i2DoublePlusStepSize, i2DoublePlusStepSizeNeg;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseInterBlockQuantizeEscCode);
    
    if (pWMVDec->m_cvCodecVersion >= WMV3) {
        iDoubleStepSize = pDQ -> iDoubleStepSize;
        iStepMinusStepIsEven = pDQ -> iStepMinusStepIsEven;
        i2DoublePlusStepSize = pDQ -> i2DoublePlusStepSize;
        i2DoublePlusStepSizeNeg = pDQ ->i2DoublePlusStepSizeNeg;
    } else {
        iDoubleStepSize = pWMVDec -> m_iDoubleStepSize;
        iStepMinusStepIsEven = pWMVDec -> m_iStepMinusStepIsEven;
        i2DoublePlusStepSize = pWMVDec -> m_i2DoublePlusStepSize;
        i2DoublePlusStepSizeNeg = pWMVDec -> m_i2DoublePlusStepSizeNeg;
    }

    FUNCTION_PROFILE_DECL_START(fpDecode,DECINVIBQUANTESCCODE_PROFILE );    
#ifndef _WMV_TARGET_X86_
    ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefRecon, 0, (size_t) MB_SQUARE_SIZE);
#else
    if(!g_bSupportMMX_WMV)
        ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefRecon, 0, (size_t) MB_SQUARE_SIZE);
    else
        ALIGNED32_FASTMEMCLR_MMX (pWMVDec->m_rgiCoefRecon, (size_t) MB_SQUARE_SIZE);
#endif
    do {
        register I32_WMV iIndex;
        lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
        //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            FUNCTION_PROFILE_STOP(&fpDecode);
            return WMV_CorruptedBits;
        }
        //lIndex = m_pentrdecDCT->decodeSymbol();
        // if Huffman
        if (lIndex != iTCOEF_ESCAPE)    {
            if (lIndex >= iStartIndxOfLastRun)
                bIsLastRun = TRUE_WMV;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return WMV_CorruptedBits;
            }
            COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
        } 
        else {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
                // ESC + '1' + VLC
                COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC1VLC);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
                uiRun = rgRunAtIndx[lIndex];
                iLevel = rgLevelAtIndx[lIndex];
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE_WMV;
                    iLevel += (I8_WMV) rgIfLastNumOfLevelAtRun[uiRun];
                }
                else
                    iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRun[uiRun];
                if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
                COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
            }
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
                // ESC + '10' + VLC
                COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC01VLC);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
                uiRun = rgRunAtIndx[lIndex];
                iLevel = rgLevelAtIndx[lIndex];
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE_WMV;
                    uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
                COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
            }
            else{
                // ESC + '00' + FLC
                COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC00FLC);
                bIsLastRun = (Bool_WMV) BS_getBit ( pWMVDec->m_pbitstrmIn); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){
                    WMV_ESC_Decoding();
                    COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                }
                else{
                    uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);           
                    iLevel = (I8_WMV)BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
                }
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return WMV_CorruptedBits;
                }
            }
        }
        uiCoefCounter += uiRun;

        COVERAGE_ACPRED (pWMVDec, uiCoefCounter);

        if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
#if 0 //2008/01/18
			uiCoefCounter &= 63;
			if(!uiCoefCounter) {
				uiCoefCounter = 63;
			}
#else
            FUNCTION_PROFILE_STOP(&fpDecode);
            return WMV_CorruptedBits;
#endif
        }
        
        iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];

        // If this coefficient is not in the first column then set the flag which indicates
        // what row it is in. This flag field will be used by the IDCT to see if it can
        // shortcut the IDCT of the row if all coefficients are zero.
        if (iIndex & 0x7)
            iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

        if (iLevel == 1)
            pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = i2DoublePlusStepSize;
        else if (iLevel == -1)
            pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = i2DoublePlusStepSizeNeg;
        else if (iLevel > 0)
//          I32_WMV iCoefRecon = m_iStepSize * ((iLevel << 1) + 1) - m_bStepSizeIsEven;
            pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
        else
//          I32_WMV iCoefRecon = m_bStepSizeIsEven - m_iStepSize * (1 - (iLevel << 1));
            pWMVDec->m_rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;

        uiCoefCounter++;
    } while (!bIsLastRun);
    // Save the DCT row flags. This will be passed to the IDCT routine
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;

    FUNCTION_PROFILE_STOP(&fpDecode);
    return WMV_Succeeded;
}
//#pragma warning(default: 4799)  // emms warning disable


Void_WMV decodeIntraDCAcPredMSV (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCStepSize)
{
    U16_WMV lSzDiffIntraDC;
    lSzDiffIntraDC = (U16_WMV) Huffman_WMV_get (hufDCTDCDec, pWMVDec->m_pbitstrmIn);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeIntraDCAcPredMSV);

    if (BS_invalid ( pWMVDec->m_pbitstrmIn)){
        *pError = WMV_CorruptedBits;
        return;
    }
    if (lSzDiffIntraDC != iTCOEF_ESCAPE)    {
        if (lSzDiffIntraDC != 0) {
                        if(pWMVDec->m_bNewDCQuant) {
                if (iDCStepSize == 4){
                    lSzDiffIntraDC = (lSzDiffIntraDC<<1) + (U16_WMV) (BS_getBit ( pWMVDec->m_pbitstrmIn )) -1;
                    COVERAGE_DCDIFF (pWMVDec, 1);
                }
                if (iDCStepSize == 2){
                    lSzDiffIntraDC = (lSzDiffIntraDC<<2) + (U16_WMV) (BS_getBits ( pWMVDec->m_pbitstrmIn, 2)) -3;
                    COVERAGE_DCDIFF (pWMVDec, 2);
                }
            }
            pWMVDec->m_rgiCoefRecon [0] = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? -lSzDiffIntraDC : lSzDiffIntraDC;  //zou -->DCDifferential = (HWD16)vc1DECBIT_GetBits(pBitstream, BitsToGet);
        }
        else {
            pWMVDec->m_rgiCoefRecon [0] = 0;
            COVERAGE_DCDIFF (pWMVDec, 0);
        }
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            *pError = WMV_CorruptedBits;
            return;
        }
    }
    else {
        U32_WMV offset = 0;
        if(iDCStepSize <= 4 && pWMVDec->m_bNewDCQuant)
            offset =  3 - (iDCStepSize>>1);
        lSzDiffIntraDC = (U16_WMV) BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL + offset);
        COVERAGE_DCDIFF (pWMVDec, NUMBITS_ESC_LEVEL + offset);
        pWMVDec->m_rgiCoefRecon [0] = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? -lSzDiffIntraDC : lSzDiffIntraDC;
    }
    COVERAGE_DCDIFF_SIGN (pWMVDec, (pWMVDec->m_rgiCoefRecon [0]<0));
    *pError = WMV_Succeeded;
}

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred (
    tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
    Bool_WMV CBP,
    I16_WMV* pDct, 
    U8_WMV* piZigzagInv     
)
{

    CDCTTableInfo_Dec* IntraDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec[0]; 

    Huffman_WMV* hufDCTACDec = IntraDCTTableInfo_Dec->hufDCTACDec;
    I8_WMV* rgLevelAtIndx = IntraDCTTableInfo_Dec->pcLevelAtIndx;
    U8_WMV* rgRunAtIndx = IntraDCTTableInfo_Dec->puiRunAtIndx;
//  Change iStartIndxOfLastRun to iStartIndxOfLastRunMinus1 and <= to <
//  iStartIndxOfLastRun = IntraDCTTableInfo_Dec->iStartIndxOfLastRunMinus1;
    U8_WMV* rgIfNotLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec->puiNotLastNumOfLevelAtRun;
    U8_WMV* rgIfLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec->puiLastNumOfLevelAtRun; 
    U8_WMV* rgIfNotLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec->puiNotLastNumOfRunAtLevel;
    U8_WMV* rgIfLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec->puiLastNumOfRunAtLevel;
    U32_WMV iStartIndxOfLastRun = IntraDCTTableInfo_Dec->iStartIndxOfLastRunMinus1 + 1;
    I32_WMV iTCOEF_ESCAPE = IntraDCTTableInfo_Dec->iTcoef_ESCAPE;


    Bool_WMV bIsLastRun = FALSE_WMV;
    U32_WMV uiRun; // = 0;
    I32_WMV iLevel; // = 0;
    U32_WMV uiCoefCounter = 1;
    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;

#ifndef _NEW_INTRA_QUANT_RECON_
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
#else
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepSize - 1;
#endif
#if (defined(_XBOX) && defined(_DEBUG)) || !defined(_XBOX)
    I32_WMV i;
#endif

#   if !defined(_SH4_)
    FUNCTION_PROFILE_DECL_START(fpDecode,DECINVIBQUANTESCCODE_PROFILE );
#   endif
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseIntraBlockQuantizeAcPred);
    
    if (CBP) {
        //I32_WMV iSign = 0;

        do {
            lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
            //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
            //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT,decodeCodeTableDCT);
            assert(lIndex <= iTCOEF_ESCAPE);
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {

#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif

                               
                return WMV_CorruptedBits;
            }
            if (lIndex != iTCOEF_ESCAPE)    {
                bIsLastRun = (lIndex >= iStartIndxOfLastRun);
                uiRun = rgRunAtIndx[lIndex];
#if 0
                iLevel = rgLevelAtIndx[lIndex];
                iSign = BS_getBit(pWMVDec->m_pbitstrmIn);
#else
                iLevel = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                    rgLevelAtIndx[lIndex];
                COVERAGE_ACCOEFSIGN(pWMVDec, (iLevel<0));
#endif
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {

#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                    
                    return WMV_CorruptedBits;
                }
            }
            else {
                if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {


#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif

                        return WMV_CorruptedBits;
                    }
                    // ESC + '1' + VLC
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC1VLC);
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                    //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                        return WMV_CorruptedBits;
                    }
                    uiRun = rgRunAtIndx [lIndex];
                    iLevel = rgLevelAtIndx[lIndex];
                    if (lIndex >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE_WMV;
                        iLevel += (I8_WMV) rgIfLastNumOfLevelAtRunIntra[uiRun];
                    }
                    else
                        iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRunIntra[uiRun];
//#if 1
//                  iSign = BS_getBit(pWMVDec->m_pbitstrmIn);
//#else
                    if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
                    COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
//#endif
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                        return WMV_CorruptedBits;
                    }
                }
                else if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                        return WMV_CorruptedBits;
                    }
                    // ESC + '01' + VLC
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC01VLC);
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                    //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                        return WMV_CorruptedBits;
                    }
                    uiRun = rgRunAtIndx [lIndex];
                    iLevel = rgLevelAtIndx[lIndex];
                    if (lIndex >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE_WMV;
                        uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                    }
                    else
                        uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
//#if 1
//                  iSign = BS_getBit(pWMVDec->m_pbitstrmIn);
//#else
                    if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
                    COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
//#endif
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                        return WMV_CorruptedBits;
                    }
                }
                else{
                    // ESC + '00' + FLC
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC00FLC);
                    bIsLastRun = (Bool_WMV) BS_getBit ( pWMVDec->m_pbitstrmIn); // escape decoding
                    if (pWMVDec->m_cvCodecVersion >= WMV1){
                        WMV_ESC_Decoding();
                        COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                    }
                    else{
                        uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);           
                        iLevel = (I8_WMV)BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
                    }
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                        return WMV_CorruptedBits;
                    }
                    //iSign = ((U32_WMV) iLevel) >> 31;
                    //if(iLevel <0)
                    //    iLevel = -iLevel;
//                    I32_WMV iSign1 = iLevel >> 31;
 //                   iLevel = (iLevel^iSign1)-iSign1;

                }
            }
            //assert(uiRun<= 63);
            uiCoefCounter += uiRun;
            COVERAGE_ACPRED (pWMVDec, uiCoefCounter);

            if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
#if 1 //2008/01/18
				uiCoefCounter &= 63;
				if(!uiCoefCounter) {
					uiCoefCounter = 63;
				}
#else
#   if !defined(_SH4_)
                FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
                return WMV_CorruptedBits;
#endif
            }
            // Use 8x8 for each blcok to aVoid_WMV IFs. 
            if ((piZigzagInv[uiCoefCounter] & 0xfffffff8) == 0)  // cnt >= 0(1) && <=7
                pDct[piZigzagInv[uiCoefCounter]] = iLevel + pDct[piZigzagInv[uiCoefCounter]];
            else if ((piZigzagInv[uiCoefCounter] & 0x00000007) == 0) // cnt mod 8 == 0
                pDct[BLOCK_SIZE + (piZigzagInv[uiCoefCounter] >> 3)] = iLevel + pDct[BLOCK_SIZE + (piZigzagInv [uiCoefCounter] >> 3)];
            else {
                if (iLevel > 0)
//                  pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel * 2 + 1)) - pWMVDec->m_bStepSizeIsEven);
                    pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
                else // iLevel < 0 (note: iLevel != 0)
//                  pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = (PixelI32) ((pWMVDec->m_iStepSize * (iLevel * 2 - 1)) + pWMVDec->m_bStepSizeIsEven);
                    pWMVDec->m_rgiCoefRecon[piZigzagInv[uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;
            }

            uiCoefCounter++;

        } while (!bIsLastRun);
    }

#ifndef _XBOX
#if 0 // VOARMV7
	//Disable this v7 optimization now,because there is bug. For example,file mmsc.wmv .
	ARMV7_IntraDequantACPred(pDct,pWMVDec->m_rgiCoefRecon,iDoubleStepSize,iStepMinusStepIsEven);
#else
    for (i = 1; i < 8; i++){
        I32_WMV iLevel1 = pDct [i];

        // fast mask; want -1, 1, or 0
        int signMask = iLevel1 >> 31; // 0 or FFFFFFFF
        int zeroMask = (iLevel1 == 0)? 0x0 : ~0x0;
        pWMVDec->m_rgiCoefRecon[i] = zeroMask & (iDoubleStepSize * iLevel1 + (signMask ^ iStepMinusStepIsEven) - signMask);
    }

    for (i = 1; i < 8; i++) {
        int iLevel1 = pDct [i + BLOCK_SIZE];

        int signMask2 = iLevel1 >> 31; // 0 or FFFFFFFF
        int zeroMask2 = (iLevel1 == 0)? 0x0 : ~0x0;
        pWMVDec->m_rgiCoefRecon [i << 3] = zeroMask2 & (iDoubleStepSize * iLevel1 + (signMask2 ^ iStepMinusStepIsEven) - signMask2);
    }
#endif
#else
    CalcrgiCoef(pDct,pWMVDec->m_rgiCoefRecon,iDoubleStepSize,iStepMinusStepIsEven);
#ifdef _DEBUG
    for (i = 1; i < 8; i++){
        I32_WMV iLevel1 = pDct [i];

        // fast mask; want -1, 1, or 0
        int signMask = iLevel1 >> 31; // 0 or FFFFFFFF
        int zeroMask = (iLevel1 == 0)? 0x0 : ~0x0;
        assert(pWMVDec->m_rgiCoefRecon[i] == (zeroMask & (iDoubleStepSize * iLevel1 + (signMask ^ iStepMinusStepIsEven) - signMask)));
    }

    for (i = 1; i < 8; i++) {
        int iLevel1 = pDct [i + BLOCK_SIZE];

        int signMask2 = iLevel1 >> 31; // 0 or FFFFFFFF
        int zeroMask2 = (iLevel1 == 0)? 0x0 : ~0x0;
        assert(pWMVDec->m_rgiCoefRecon [i << 3] == (zeroMask2 & (iDoubleStepSize * iLevel1 + (signMask2 ^ iStepMinusStepIsEven) - signMask2)));
    }
#endif    
#endif    
#   if !defined(_SH4_)
    FUNCTION_PROFILE_STOP(&fpDecode);
#   endif
    return WMV_Succeeded;
}

//Use 0-5 in decoder but Y_BLOCK1-V_BLOCK in encoder. the mess needs to be fixed.
I16_WMV* decodeDiffDCTCoef_WMV (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliTextureQMB, I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB, Bool_WMV bLeftMBAndInter, Bool_WMV bTopMBAndInter, Bool_WMV bLeftTopMBAndInter, Bool_WMV* pbPredPattern, I32_WMV iWidthPrev, I32_WMV iStepSize)
{
    I16_WMV *pDctLeft, *pDctTop;
    //I16_WMV iDctLeftTop;
    I16_WMV *pAvgDct;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDiffDCTCoef_WMV);

    if (!*pbPredPattern){
        if (bLeftMBAndInter){
            pAvgDct = piQuanCoefACPredTable[1];
            if (1)//!pAvgDct[0])
                pAvgDct[0] = (I16_WMV) (*pWMVDec->m_pBlkAvgX8_MMX) (ppxliTextureQMB - BLOCK_SIZE, iWidthPrev, iStepSize);
            pWMVDec->m_pAvgQuanDctCoefDecLeft[0] = pAvgDct[0];
            pDctLeft = pWMVDec->m_pAvgQuanDctCoefDecLeft;
        }
        else{
            pDctLeft = piQuanCoefACPredTable[bLeftMB];
        }
        //*pbPredPattern = 0;
        COVERAGE_DCPRED (pWMVDec, CV_DCPRED_LEFT);
        return pDctLeft;
    }
    else{
        if (bTopMBAndInter){
            pAvgDct = piQuanCoefACPredTable[3];
            if (1)//!pAvgDct[0])
                pAvgDct[0] = (I16_WMV) (*pWMVDec->m_pBlkAvgX8_MMX) (ppxliTextureQMB - (iWidthPrev << 3) , iWidthPrev, iStepSize);
            pWMVDec->m_pAvgQuanDctCoefDecTop[0] = pAvgDct[0];
            pDctTop = pWMVDec->m_pAvgQuanDctCoefDecTop;
        }
        else{
            pDctTop = piQuanCoefACPredTable[2+bTopMB];
        }
        //*pbPredPattern = 1;
        COVERAGE_DCPRED (pWMVDec, CV_DCPRED_LEFT);
        return pDctTop;
    }
}

I16_WMV* decodeDiffDCTCoef_WMV_Y23(I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB, Bool_WMV* pbPredPattern)
{
    I16_WMV *pDctLeft, *pDctTop;//, *pDctLeftTop;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDiffDCTCoef_WMV_Y23);

    pDctLeft = piQuanCoefACPredTable[bLeftMB];
    pDctTop = piQuanCoefACPredTable[2+bTopMB];
    //pDctLeftTop = piQuanCoefACPredTable[4+bLeftTopMB];

    if (!*pbPredPattern)
        return pDctLeft;
    else
        return pDctTop;

}

#ifndef _EMB_WMV2_
// MB routines from Vopdec.cpp
tWMVDecodeStatus DecodePMB (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
//    const Bool_WMV* rgCBP, 
    U8_WMV* ppxlcCurrQYMB,
    U8_WMV* ppxlcCurrQUMB,
    U8_WMV* ppxlcCurrQVMB,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
)
{
    tWMVDecodeStatus tWMVStatus;
    Bool_WMV bTop, bBottom, bLeft, bRight;
#ifndef _WMV9AP_
    const Bool_WMV rgCBP = pmbmd->m_rgbCodedBlockPattern;
#else
    const U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
#endif
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];
    // Y
    UnionBuffer* ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
    U8_WMV* ppxlcRefY = pWMVDec->m_ppxliRef0Y + ((yRefY >> 1) + pWMVDec->EXPANDY_REFVOP) * pWMVDec->m_iWidthPrevY + (xRefY >> 1) + pWMVDec->EXPANDY_REFVOP;
    Bool_WMV bXEvenY = !(xRefY & 1); 
    Bool_WMV bYEvenY = !(yRefY & 1);

    // UV
    Bool_WMV bXEvenUV = !(xRefUV & 1); 
    Bool_WMV bYEvenUV = !(yRefUV & 1);
    I32_WMV iPxLoc = ((yRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (xRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP;
    const U8_WMV* ppxlcRefU = pWMVDec->m_ppxliRef0U + iPxLoc;
    const U8_WMV* ppxlcRefV = pWMVDec->m_ppxliRef0V + iPxLoc;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;

    FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodePMB);

    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    //CDCTTableInfo_Dec* pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec;
    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
        //pWMVDec->m_pIntraDCTTableInfo_Dec = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
    }
    else{
        ppInterDCTTableInfo_Dec = &pWMVDec->m_ppInterDCTTableInfo_Dec[0];
    }

    // Y-blocks first (4 blocks)

#ifdef _WMV9AP_
          if (rgCBP[5-0]) {
#else
    if (rgCBP & 32) {
#endif
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
 
            COVERAGE_PXFORM (pWMVDec, iXformType);
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ);//get the QP      
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }
        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);  // _XFORMSWITCHING_TODO either implement faster way to clear error buffer (MMX) or eliminate need to clear by implementing 8x4 and 4x8 AddError functions 
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }                

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
        (*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    } 
    else {
        (*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    ppxlcCurrQYMB += BLOCK_SIZE;
    ppxlcRefY += BLOCK_SIZE;
    
#ifdef _WMV9AP_
        if (rgCBP[5-1]) {
#else
    if (rgCBP & 16) {
#endif
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;

            COVERAGE_PXFORM (pWMVDec, iXformType);
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ);//get the QP       
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }

        (*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    else { 
        (*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY,  bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    ppxlcCurrQYMB += pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    ppxlcRefY += pWMVDec->m_iWidthPrevYxBlkMinusBlk;

#ifdef _WMV9AP_
    if (rgCBP[5-2]) {
#else
    if (rgCBP & 8) {
#endif
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;

            COVERAGE_PXFORM (pWMVDec, iXformType);
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ);//get the QP       
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP     
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }                

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
        (*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    else {
        (*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    ppxlcCurrQYMB += BLOCK_SIZE;
    ppxlcRefY += BLOCK_SIZE;

#ifdef _WMV9AP_
    if (rgCBP[5-3]) {
#else
    if (rgCBP & 4) {
#endif
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;

            COVERAGE_PXFORM (pWMVDec, iXformType);
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ);//get the QP       
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
        (*pWMVDec->m_pMotionCompAndAddError) (pWMVDec, ppxlcCurrQYMB, ppxliErrorQMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }
    else {
        (*pWMVDec->m_pMotionComp) (pWMVDec, ppxlcCurrQYMB, ppxlcRefY, pWMVDec->m_iWidthPrevY, bXEvenY, bYEvenY, pWMVDec->m_iMixedPelMV);
    }

    // UV

    // U-block

#ifdef _WMV9AP_
    if (rgCBP[5-4]) {
#else
    if (rgCBP & 2) {
#endif
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;

            COVERAGE_PXFORM (pWMVDec, iXformType);
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8, pDQ);//get the QP  
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }        

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }


        (*pWMVDec->m_pMotionCompAndAddErrorUV) (
            pWMVDec, ppxlcCurrQUMB, ppxliErrorQMB, ppxlcRefU, 
            pWMVDec->m_iWidthPrevUV, bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);
    } 
    else 
        (*pWMVDec->m_pMotionCompUV) (pWMVDec, ppxlcCurrQUMB, ppxlcRefU, pWMVDec->m_iWidthPrevUV,  bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);

    // V-block
#ifdef _WMV9AP_
    if (rgCBP[5-5]) {
#else
    if (rgCBP & 1) {
#endif
        if (pmbmd->m_bBlkXformSwitchOn)
        {
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;

            COVERAGE_PXFORM (pWMVDec, iXformType);
        }

        if (iXformType == XFORMMODE_8x8)
        {
            ppxliErrorQMB = pWMVDec->m_rgiCoefReconBuf;
            tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, XFORMMODE_8x8,pDQ);//get the QP  
            if (WMV_Succeeded != tWMVStatus) {
                FUNCTION_PROFILE_STOP(&fpDecode);
                return tWMVStatus;
            }

            (*pWMVDec->m_pInterIDCT_Dec) (ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_iDCTHorzFlags);
        }

        bTop = TRUE_WMV;
        bBottom = TRUE_WMV;
        if (iXformType == XFORMMODE_8x4)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bTop = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bBottom = FALSE_WMV;
            if (bTop)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bBottom)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, XFORMMODE_8x4, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter8x4IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            }
        }

        bLeft = TRUE_WMV;
        bRight = TRUE_WMV;
        if (iXformType == XFORMMODE_4x8)
        {
            ppxliErrorQMB = pWMVDec->m_ppxliErrorQ;
            ALIGNED32_MEMCLR_256(ppxliErrorQMB, 0, 256);
            if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 0)
                bLeft = FALSE_WMV;
            else if (BS_getBit ( pWMVDec->m_pbitstrmIn) == 1)
                bRight = FALSE_WMV;
            if (bLeft)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 0);
            }
            if (bRight)
            {
                tWMVStatus = (*pWMVDec->m_pDecodeInverseInterBlockQuantize)(pWMVDec, ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, XFORMMODE_4x8, pDQ);//get the QP      
                if (WMV_Succeeded != tWMVStatus) {
                    FUNCTION_PROFILE_STOP(&fpDecode);
                    return tWMVStatus;
                }

                (*pWMVDec->m_pInter4x8IDCT_Dec) (ppxliErrorQMB, BLOCK_SIZE, pWMVDec->m_rgiCoefReconBuf, 1);
            } 
        }
       (*pWMVDec->m_pMotionCompAndAddErrorUV) (
            pWMVDec, ppxlcCurrQVMB, ppxliErrorQMB, ppxlcRefV, 
            pWMVDec->m_iWidthPrevUV, bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);
    }
    else 
        (*pWMVDec->m_pMotionCompUV) (pWMVDec, ppxlcCurrQVMB, ppxlcRefV, pWMVDec->m_iWidthPrevUV, bXEvenUV, bYEvenUV, pWMVDec->m_iMixedPelMV);
            
    FUNCTION_PROFILE_STOP(&fpDecode);
    return WMV_Succeeded;
}
#endif //_EMB_WMV2_

#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119                           // see table.13/H.263
tWMVDecodeStatus DecodeIMBAcPred (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV*      ppxliTextureQMBY,
    U8_WMV*      ppxliTextureQMBU, 
    U8_WMV*      ppxliTextureQMBV, 
    I16_WMV*                piQuanCoefACPred, 
    I16_WMV**                piQuanCoefACPredTable, 
    Bool_WMV                bLeftMB, 
    Bool_WMV                bTopMB, 
    Bool_WMV                bLeftTopMB, I32_WMV imbX)
{
    tWMVDecodeStatus tWMVStatus;

    // Y-blocks first (4 blocks)
    U8_WMV * ppxliCodedBlkY = ppxliTextureQMBY;
    Bool_WMV bPredPattern;
    I16_WMV* pRef; 
    Bool_WMV bMBPattern[4]; 
    U32_WMV ib;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeIMBAcPred);

#ifdef _WMV9AP_
    if (pWMVDec->m_bAdvancedProfile) {
        I32_WMV iMBY = imbX >> 16; // hack-must be cleaned up
        imbX &= 0xffff;
        pWMVDec->m_pDiffMV->iIntra = TRUE;

        return DecodeMB_1MV_WMVA_Intra (pWMVDec,
            pmbmd, 
            ppxliTextureQMBY,
            ppxliTextureQMBU,
            ppxliTextureQMBV,
            imbX, iMBY,
            0,0,0,0);
    }
#endif


    bMBPattern[0] = bLeftTopMB;
    bMBPattern[1] = bTopMB; 
    bMBPattern[2] = bLeftMB; 
    bMBPattern[3] = bLeftTopMB;

    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
        ppIntraDCTTableInfo_Dec = &pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }


    for (ib = 0; ib < 4; ib++) {
        decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSize);

        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }

        pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bMBPattern[ib], &bPredPattern);
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
        //pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] << 3);
        pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);

        tWMVStatus = decodeIntraBlockAcPred (
            pWMVDec, 
            ppIntraDCTTableInfo_Dec,
            pmbmd, 
            ppxliCodedBlkY,
            pWMVDec->m_iWidthPrevY,
            ib,
            piQuanCoefACPred,
            pRef,
            bPredPattern,
            imbX);
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }
#ifndef   XBOXGPU
        ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
#else
        if(!pWMVDec->m_bXBOXUsingGPU)
        ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
        else
        ppxliCodedBlkY += (ib != 1) ? iBlockSize : iPitchY*BLOCK_SIZE-iBlockSize;
#endif
        
        piQuanCoefACPred += BLOCK_SIZE_TIMES2;
        piQuanCoefACPredTable += 6;
    }

    // U-blocks

    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSizeC);
    
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, &bPredPattern);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
//    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] << 3);
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

//#ifdef __MSV


    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
        ppInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBU,
#ifndef XBOXGPU        
        pWMVDec->m_iWidthPrevUV,
#else
        pWMVDec->m_bXBOXUsingGPU?iPitchUV:pWMVDec->m_iWidthPrevUV,
#endif
        
        4,
        piQuanCoefACPred,
        pRef,
        bPredPattern,
        imbX);

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    // V-blocks
//#ifdef __MSV

    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSizeC);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, &bPredPattern);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
//    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] << 3);
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
        ppInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBV,
#ifndef XBOXGPU        
        pWMVDec->m_iWidthPrevUV,
#else
        pWMVDec->m_bXBOXUsingGPU?iPitchUV:pWMVDec->m_iWidthPrevUV,
#endif
        5,
        piQuanCoefACPred,
        pRef,
        bPredPattern,
        imbX
        );

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    return WMV_Succeeded;
}


//#ifdef X9
//#include "alttables.h"



//#pragma warning(disable: 4799)  // emms warning disable
//This is not really an MMX routine but works along side the MMX idct
#ifdef _WMV9AP_

tWMVDecodeStatus DecodeInverseIntraBlockX9(tWMVDecInternalMember *pWMVDec,
                           CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                           I32_WMV iblk,      Bool_WMV bResidual,
                           I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                           I16_WMV   *pIntra, CWMVMBMode* pmbmd,
                           DQuantDecParam *pDQ, UnionBuffer *pOut, I32_WMV iOutStride
                           )
#else
tWMVDecodeStatus DecodeInverseIntraBlockX9(tWMVDecInternalMember *pWMVDec,
                           CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                           I32_WMV iblk,   U8_WMV *pZigzagInv,   Bool_WMV bResidual,
                           I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                           I16_WMV   *pIntra, CWMVMBMode* pmbmd,
                           DQuantDecParam *pDQ
                           )
#endif
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    //I32_WMV i2DoublePlusStepSize = pDQ->i2DoublePlusStepSize;
    //I32_WMV i2DoublePlusStepSizeNeg = pDQ->i2DoublePlusStepSizeNeg;
    I32_WMV iDCStepSize = pDQ->iDCStepSize;
#ifdef _WMV9AP_
    U8_WMV * pZigzagInv = pWMVDec->m_pZigzagScanOrder;
#endif
    //Align on cache line
    I16_WMV *rgiCoefRecon = (I16_WMV*) pWMVDec->m_rgiCoefRecon;
    I32_WMV i;
    I16_WMV pNonZeroCoeff[64];
    I16_WMV iNumNonZeroCoef = 0;
    //U32_WMV  uiRun = 0, uiCoefCounter = 0;
    I32_WMV   iLevel;

#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119                           // see table.13/H.263
    
    tWMVDecodeStatus result;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseIntraBlockX9);

#ifndef _WMV_TARGET_X86_
    ALIGNED32_FASTMEMCLR (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));//memset (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));
#else
    if(g_bSupportMMX_WMV)
        ALIGNED32_FASTMEMCLR_MMX (rgiCoefRecon, (I32_WMV)(BLOCK_SQUARE_SIZE_2>>1));
    else
        memset (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));
#endif
	
	//Double check for EMB code path!!!
    /** set scan pattern for advanced profile I frame (which doesn't follow the simple/main I frame code path) **/
    if (pWMVDec->m_bAdvancedProfile && (pWMVDec->m_tFrmType == IVOP || pWMVDec->m_tFrmType == BIVOP)) {
        /** this section of code occurs at the top of decodeIntraBlockAcPred (simple/main I frame path) as well **/
        if (pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK]) { // whether AC is predicted or not, read in headdec/bitplane
            // pWMVDec->m_bRotatedIdct is always TRUE for AP
            pZigzagInv = (U8_WMV*) (iDirection ? pWMVDec->m_pHorizontalZigzagInvRotated : pWMVDec->m_pVerticalZigzagInvRotated);
        }
        else
            pZigzagInv = pWMVDec->m_pZigzagInvRotated_I;
    }

    if (iblk < 4)
        decodeIntraDCAcPredMSV (pWMVDec, &result, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, iDCStepSize);
    else 
        decodeIntraDCAcPredMSV (pWMVDec, &result, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, iDCStepSize);
    // decodeIntraDCAcPredMSV sets pWMVDec->m_rgiCoefRecon[0] be to the DC component
    // but pWMVDec->m_rgiCoefRecon is I32_WMV array and we are using it as a 16-bit array
    // through rgiCoefRecon,  so we need to zero out rgiCoefRecon[1], also,
    // i set rgiCoefRecon[0] by typecasting just in case some other endian order
    // is used...  
    rgiCoefRecon[0] = (I16_WMV) pWMVDec->m_rgiCoefRecon[0];  //zou -->DCDifferential = (HWD16)vc1DECBIT_GetVLC(pBitstream, pTable);
    rgiCoefRecon[1] = 0;  
                        
    if (WMV_Succeeded != result) {
        return result;
    }

    if (bResidual) {
        CDCTTableInfo_Dec* IntraDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec[0];         
        Huffman_WMV* hufDCTACDec = IntraDCTTableInfo_Dec -> hufDCTACDec;
        I8_WMV* rgLevelAtIndx = IntraDCTTableInfo_Dec -> pcLevelAtIndx;
        U8_WMV* rgRunAtIndx = IntraDCTTableInfo_Dec -> puiRunAtIndx;
        U8_WMV* rgIfNotLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec -> puiNotLastNumOfLevelAtRun;
        U8_WMV* rgIfLastNumOfLevelAtRunIntra = IntraDCTTableInfo_Dec -> puiLastNumOfLevelAtRun; 
        U8_WMV* rgIfNotLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec -> puiNotLastNumOfRunAtLevel;
        U8_WMV* rgIfLastNumOfRunAtLevelIntra = IntraDCTTableInfo_Dec -> puiLastNumOfRunAtLevel;
        U32_WMV iStartIndxOfLastRun = IntraDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
        I32_WMV iTCOEF_ESCAPE = IntraDCTTableInfo_Dec -> iTcoef_ESCAPE;

        //U32_WMV iCurrIndexZone = 0;

        Bool_WMV bIsLastRun = FALSE_WMV;
        U32_WMV uiRun; // = 0;
        I32_WMV iLevel; // = 0;
        U32_WMV uiCoefCounter = 1;
        
        U8_WMV   lIndex;

        

//        FUNCTION_PROFILE_DECL_START(fpDecode,DECINVIBQUANTESCCODE_PROFILE );

        do
        {
            lIndex = (U8_WMV) (  Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn) );  //zou  --->index = vc1DECBIT_GetVLC(pBitstream, pHuffTable);
            assert(lIndex <= iTCOEF_ESCAPE);
            if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                
                return WMV_Failed;
            }
            if (lIndex != iTCOEF_ESCAPE)    {
                bIsLastRun = (lIndex >= iStartIndxOfLastRun);
                uiRun = rgRunAtIndx[lIndex];
                iLevel = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :  //zou --->sign = (BYTE8)vc1DECBIT_GetBits(pBitstream, 1);
                rgLevelAtIndx[lIndex];
                if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                    
                    return WMV_Failed;
                }
                COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
            }
            else {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                        
                        return WMV_Failed;
                    }
                    // ESC + '1' + VLC
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC1VLC);
                    lIndex = (U8_WMV) ( Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn) );
                    if (BS_invalid(pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        
                        return WMV_Failed;
                    }
                    uiRun = rgRunAtIndx [lIndex];
                    iLevel = rgLevelAtIndx[lIndex];
                    if (lIndex >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE_WMV;
                        iLevel += (I8_WMV) rgIfLastNumOfLevelAtRunIntra[uiRun];
                    }
                    else
                        iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRunIntra[uiRun];
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                        
                        return WMV_Failed;
                    }
                    COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                }
                else if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                        
                        return WMV_Failed;
                    }
                    // ESC + '01' + VLC
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC01VLC);
                    lIndex = (U8_WMV) ( Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn) );
                    if (BS_invalid(pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        
                        return WMV_Failed;
                    }
                    uiRun = rgRunAtIndx [lIndex];
                    iLevel = rgLevelAtIndx[lIndex];
                    if (lIndex >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE_WMV;
                        uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                    }
                    else
                        uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                        
                        return WMV_Failed;
                    }
                    COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                }
                else{
                    // ESC + '00' + FLC
                    bIsLastRun = (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC00FLC);
                    if (pWMVDec->m_cvCodecVersion >= WMV1){
                        WMV_ESC_Decoding();
                        COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                    }
                    else{
                        uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);          
                        iLevel = (I8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_LEVEL);
                    }
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                        
                        return WMV_Failed;
                    }
                    COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                }
            }
#if 0 //2008/01/18
            assert(uiRun<= 63);
#endif
            uiCoefCounter += uiRun;
            COVERAGE_ACPRED (pWMVDec, uiCoefCounter);

            if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
                
#if 1 //2008/01/18
				uiCoefCounter &= 63;
				if(!uiCoefCounter) {
					uiCoefCounter = 63;
				}
#else
                return WMV_Failed;
#endif
            }

            rgiCoefRecon [(pNonZeroCoeff[iNumNonZeroCoef]=pZigzagInv [uiCoefCounter])] = (I16_WMV) iLevel;
            uiCoefCounter++;
            iNumNonZeroCoef++;
        
#if 1	/* To avoid error when meeting corrupted bits. 2010.11.08 */
			if( iNumNonZeroCoef >= BLOCK_SQUARE_SIZE) {
				iNumNonZeroCoef &= 63;
				if(!iNumNonZeroCoef) { // // iNumNonZeroCoef can't be 0 because pixel 0 is DC whose value can't be overwrited.
					iNumNonZeroCoef = 1;
				}
			}
#endif
        } while (!bIsLastRun);
        
    }

    // add prediction of dct-ac
    if (pPred) {

        
       // I32_WMV  iFactor;
        I16_WMV *pTrans = pPred;

      

        if (iDirection) 
        {
            pPred += pWMVDec->m_iACRowPredOffset;
            pTrans += pWMVDec->m_iACColPredOffset;
        }    
        else { // left
            pPred += pWMVDec->m_iACColPredOffset;
            pTrans += pWMVDec->m_iACRowPredOffset;
        }    

#ifdef HIGHER_ORDER_DCAC_PRED
        // higher order terms to adjust DC
           iFactor = 2 * (pTrans[1] - pTrans[2] + pTrans[3] - pTrans[4]);
           if (pWMVDec->m_iStepSize > 8)
            iFactor *= 2;

            if (pWMVDec->m_bDQuantOn || pWMVDec->m_iStepSize < 4) 
            {
                     iFactor = 0;
            }

            rgiCoefRecon[0] -= (I16_WMV) iFactor;
#endif  // HIGHER_ORDER_DCAC_PRED

            rgiCoefRecon[0] += pPred[0];
            if (iShift != -1) {           
            I32_WMV i;
            for ( i = 1; i < 8; i++) {
				I32_WMV j;
#ifndef _WMV9AP_
                rgiCoefRecon[i << iShift] += pPred[i];
#else
                j = i<< iShift;
                if(rgiCoefRecon[j]) {
                    rgiCoefRecon[j] += pPred[i];
                }
                else {
                    rgiCoefRecon[(pNonZeroCoeff[iNumNonZeroCoef]=j)] = pPred[i];
                    iNumNonZeroCoef++;
#if 1			/* To avoid error when meeting corrupted bits. 2010.11.08 */
					if( iNumNonZeroCoef >= BLOCK_SQUARE_SIZE) {
						iNumNonZeroCoef &= 63;
						if(!iNumNonZeroCoef) { 
							iNumNonZeroCoef = 1;
						}
					}
#endif
                }
#endif
            }
        }
    }



    for ( i = 0; i < 8; i++) {
        pIntra[i] = rgiCoefRecon[i];
        pIntra[i + 8] = rgiCoefRecon[i << 3];
    }
    // Dequantize DCT-DC
    rgiCoefRecon[0] = (I16_WMV) (rgiCoefRecon[0] * iDCStepSize);

    // Dequantize DCT-AC
    for (i = 0; i < iNumNonZeroCoef; i++) {
        int signMask, j;
        j=pNonZeroCoeff[i];
        iLevel = rgiCoefRecon[j];
        if(iLevel) {
            signMask = iLevel >> 31; // 0 or FFFFFFFF
            rgiCoefRecon[j] = (I16_WMV) ((I32_WMV) iDoubleStepSize * iLevel + (signMask ^ iStepMinusStepIsEven) - signMask);
        }
    }

#ifdef _EMB_SSIMD_IDCT_
    if ( pWMVDec->m_b16bitXform) SignPatch(rgiCoefRecon, 128);
#endif
    // DCT flags are set to all on
    pWMVDec->m_iDCTHorzFlags = 0xff;

#ifdef _WMV9AP_
        (*pWMVDec->m_pIntraX9IDCT_Dec) ((UnionBuffer*)pOut, (UnionBuffer*)pOut, iOutStride, pWMVDec->m_iDCTHorzFlags);
#endif

    return WMV_Succeeded;
}
//#pragma warning(default: 4799)  // emms warning disable

//#endif // X9




tWMVDecodeStatus DecodeIMBAcPred_PFrame (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV*      ppxliTextureQMBY,
    U8_WMV*      ppxliTextureQMBU, 
    U8_WMV*      ppxliTextureQMBV, 
    I16_WMV*                piQuanCoefACPred, 
    I16_WMV**               piQuanCoefACPredTable, 
    Bool_WMV                bLeftMB, 
    Bool_WMV                bTopMB, 
    Bool_WMV                bLeftTopMB)
{
    Bool_WMV bLeftMBAndIntra, bLeftMBAndInter;
    Bool_WMV bTopMBAndIntra, bTopMBAndInter;
    Bool_WMV bLeftTopMBAndIntra, bLeftTopMBAndInter;
    Bool_WMV bMBPattern[4];
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;

    // Y
    tWMVDecodeStatus tWMVStatus;
    U8_WMV * ppxliCodedBlkY = ppxliTextureQMBY;
    Bool_WMV bPredPattern;
    I16_WMV* pRef; 
    U32_WMV ib;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeIMBAcPred_PFrame);

    if (bLeftMB){
        if ((pmbmd - 1)->m_dctMd == INTRA){
            bLeftMBAndIntra = TRUE_WMV;
            bLeftMBAndInter = FALSE_WMV;
        }
        else{
            bLeftMBAndIntra = FALSE_WMV;
            bLeftMBAndInter = TRUE_WMV;
        }
    }
    else{
        bLeftMBAndInter = bLeftMBAndIntra = FALSE_WMV;
    }

    if (bTopMB){
        if ((pmbmd - pWMVDec->m_uintNumMBX)->m_dctMd == INTRA){
            bTopMBAndIntra = TRUE_WMV;
            bTopMBAndInter = FALSE_WMV;
        }
        else{
            bTopMBAndIntra = FALSE_WMV;
            bTopMBAndInter = TRUE_WMV;
        }
    }
    else{
        bTopMBAndInter = bTopMBAndIntra = FALSE_WMV;
    }

    if (bLeftTopMB){
        if ((pmbmd - pWMVDec->m_uintNumMBX - 1)->m_dctMd == INTRA){
            bLeftTopMBAndIntra = TRUE_WMV;
            bLeftTopMBAndInter = FALSE_WMV;
        }
        else{
            bLeftTopMBAndIntra = FALSE_WMV;
            bLeftTopMBAndInter = TRUE_WMV;
        }
    }
    else{
        bLeftTopMBAndInter = bLeftTopMBAndIntra = FALSE_WMV;
    }
    // bMBPattern is not used anymore.

    bMBPattern[0] = bLeftTopMBAndIntra;
    bMBPattern[1] = bTopMBAndIntra;
    bMBPattern[2] = bLeftMBAndIntra;
    bMBPattern[3] = bLeftTopMBAndIntra;



    if (pWMVDec->m_bDCTTable_MB){
        ppInterDCTTableInfo_Dec = &pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
        ppIntraDCTTableInfo_Dec = &pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
    }
    else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }


    // Y-blocks first (4 blocks)


    // Y0 block first 
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSize);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    ib = 0;

    bPredPattern = pmbmd->m_rgbDCTCoefPredPattern2 [Y_BLOCK1];
    pRef = decodeDiffDCTCoef_WMV(pWMVDec, ppxliTextureQMBY, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, bLeftMBAndInter, bTopMBAndInter, bLeftTopMBAndInter, &bPredPattern, pWMVDec->m_iWidthPrevY, pWMVDec->m_iDCStepSize);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
        ppIntraDCTTableInfo_Dec,
        pmbmd, 
        ppxliCodedBlkY, 
        pWMVDec->m_iWidthPrevY,
        ib,
        piQuanCoefACPred,
        pRef,
        bPredPattern,
        0);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    for (ib = 1; ib < 3; ib++) {
        decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSize);
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }
        // Y_BLOCK1 = 1; Y_BLOCK2 = 2; Y_BLOCK3 = 3;

       // bPredPattern = (pmbmd->m_rgbDCTCoefPredPattern & (1<<(ib+1))) >> (ib+1);
        bPredPattern = pmbmd->m_rgbDCTCoefPredPattern2[ib + 1];;

        //pRef = decodeDiffDCTCoef(piQuanCoefACPredTable, bLeftMBAndIntra, bTopMBAndIntra, bMBPattern[ib], &bPredPattern);
        pRef = decodeDiffDCTCoef_WMV_Y23(piQuanCoefACPredTable, bLeftMBAndIntra, bTopMBAndIntra, bMBPattern[ib], &bPredPattern);
        COVERAGE_DCPRED (pWMVDec, bPredPattern);
        piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
        pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);
        tWMVStatus = decodeIntraBlockAcPred (
            pWMVDec, 
            ppIntraDCTTableInfo_Dec,
            pmbmd, 
            ppxliCodedBlkY, 
            pWMVDec->m_iWidthPrevY,
            ib,
            piQuanCoefACPred,
            pRef,
            bPredPattern,
            0);
        if (WMV_Succeeded != tWMVStatus) {
            return tWMVStatus;
        }
        ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
        piQuanCoefACPred += BLOCK_SIZE_TIMES2;
        piQuanCoefACPredTable += 6;
    }

    // Y4
    ib = 3;
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSize);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    pRef = decodeDiffDCTCoef(pWMVDec, piQuanCoefACPredTable, bLeftMBAndIntra, bTopMBAndIntra, bMBPattern[ib], &bPredPattern);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSize);
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
            ppIntraDCTTableInfo_Dec,
        pmbmd, 
        ppxliCodedBlkY, 
        pWMVDec->m_iWidthPrevY,
        ib,
        piQuanCoefACPred,
        pRef,
        bPredPattern,
        0);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    // U-blocks
//#ifdef __MSV
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSizeC);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    // bPredPattern = (pmbmd->m_rgbDCTCoefPredPattern & (1<<5))>>5;
    bPredPattern = pmbmd->m_rgbDCTCoefPredPattern2[U_BLOCK];
    pRef = decodeDiffDCTCoef_WMV(pWMVDec, ppxliTextureQMBU, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, bLeftMBAndInter, bTopMBAndInter, bLeftTopMBAndInter, &bPredPattern, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iDCStepSizeC);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

//#ifdef __MSV
    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
        ppInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBU,
        pWMVDec->m_iWidthPrevUV,
        4,
        piQuanCoefACPred,
        pRef,
        bPredPattern, 0
        );

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }
    piQuanCoefACPred += BLOCK_SIZE_TIMES2;
    piQuanCoefACPredTable += 6;

    // V-blocks
//#ifdef __MSV
    decodeIntraDCAcPredMSV (pWMVDec, &tWMVStatus, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, pWMVDec->m_iDCStepSizeC);
    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

  //    bPredPattern = (pmbmd->m_rgbDCTCoefPredPattern & (1<<6)) >> 6;
    bPredPattern = pmbmd->m_rgbDCTCoefPredPattern2[V_BLOCK];
    pRef = decodeDiffDCTCoef_WMV(pWMVDec, ppxliTextureQMBV, piQuanCoefACPredTable, bLeftMB, bTopMB, bLeftTopMB, bLeftMBAndInter, bTopMBAndInter, bLeftTopMBAndInter, &bPredPattern, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iDCStepSizeC);
    piQuanCoefACPred [0] = pWMVDec->m_rgiCoefRecon[0] + pRef [0];
    pWMVDec->m_rgiCoefRecon[0] = (piQuanCoefACPred [0] * pWMVDec->m_iDCStepSizeC);

    tWMVStatus = decodeIntraBlockAcPred (
        pWMVDec, 
        ppInterDCTTableInfo_Dec,
        pmbmd,
        ppxliTextureQMBV,
        pWMVDec->m_iWidthPrevUV,
        5,
        piQuanCoefACPred,
        pRef,
        bPredPattern, 0
        );

    if (WMV_Succeeded != tWMVStatus) {
        return tWMVStatus;
    }

    return WMV_Succeeded;
}


//#pragma warning(disable: 4799)  // emms warning disable
tWMVDecodeStatus DecodeInverseInterBlockQuantize16(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode
        , DQuantDecParam *pDQ
)
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
    I32_WMV signBit;
    I32_WMV value;
    register I32_WMV iIndex;
    //Align on cache line
    
    I16_WMV *rgiCoefRecon = (I16_WMV*)pWMVDec->m_rgiCoefRecon;

    I32_WMV iDCTHorzFlags = 0;
#ifdef _WMV9AP_
    U32_WMV uiMaxRun = 64;

    if(iXformMode == XFORMMODE_8x4 || iXformMode == XFORMMODE_4x8)
        uiMaxRun  = 32;
    else if(iXformMode == XFORMMODE_4x4)
        uiMaxRun  = 16;

#ifdef _WMV_TARGET_X86_
    if(g_bSupportMMX_WMV) {
        ALIGNED32_FASTMEMCLR_MMX(rgiCoefRecon, 64*sizeof(I16_WMV));
    }
    else
        memset (rgiCoefRecon, 0, 64*sizeof(I16_WMV));
#else
    memset (rgiCoefRecon, 0, 64*sizeof(I16_WMV));
#endif

#endif

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseInterBlockQuantize16);

#ifndef _WMV_TARGET_X86_
    ALIGNED32_FASTMEMCLR (rgiCoefRecon, 0, 64*sizeof(I16_WMV));
#else
    if(g_bSupportMMX_WMV)
        ALIGNED32_FASTMEMCLR_MMX (rgiCoefRecon, (I32_WMV)(64 * sizeof(I16_WMV)));
    else
        ALIGNED32_FASTMEMCLR (rgiCoefRecon, 0, 64*sizeof(I16_WMV));
#endif

    if (iXformMode == XFORMMODE_8x8) {
        do {
            I32_WMV iSign = 0;
            I32_WMV lIndex = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
            if (lIndex != iTCOEF_ESCAPE)    {
                if (lIndex >= iStartIndxOfLastRun)
                    bIsLastRun = TRUE;
                uiRun = rgRunAtIndx[lIndex];
#if 1 
                iLevel = rgLevelAtIndx[lIndex];
                iSign = BS_getBit(pWMVDec->m_pbitstrmIn);
                COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
#else
                iLevel = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
#endif
            } 
            else {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                    // ESC + '1' + VLC
                    I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC1VLC);
                    //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                    if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        return WMV_CorruptedBits;
                    }
                    uiRun = rgRunAtIndx[lIndex2];
                    iLevel = rgLevelAtIndx[lIndex2];
                    if (lIndex2 >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE;
                        iLevel += (I8_WMV) rgIfLastNumOfLevelAtRun[uiRun];
                    }
                    else
                        iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRun[uiRun];
#if 1
                    iSign = BS_getBit(pWMVDec->m_pbitstrmIn); 
                    COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
#else
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
#endif
                }
                else if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                    // ESC + '10' + VLC
                    I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC01VLC);
                    //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                    if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        return WMV_CorruptedBits;
                    }
                    uiRun = rgRunAtIndx[lIndex2];
                    iLevel = rgLevelAtIndx[lIndex2];
                    if (lIndex2 >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE;
                        uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                    }
                    else
                        uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
#if 1     
                    iSign = BS_getBit(pWMVDec->m_pbitstrmIn); 
                    COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
#else
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
#endif
                }
                else{
                    // ESC + '00' + FLC
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC00FLC);
                    bIsLastRun = (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
                    if (pWMVDec->m_cvCodecVersion >= WMV1){  // == WMV1 || WMV2
                        WMV_ESC_Decoding();
                        COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                    }
                    else{
                        uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);          
                        iLevel = (I8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_LEVEL);
                    }
                    iSign = ((U32_WMV) iLevel) >> 31;
                    if(iLevel <0)
                        iLevel = -iLevel;
                    COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
                   // I32_WMV iSign1 = iLevel >> 31;
//                    iLevel = (iLevel^iSign1)-iSign1;
                }
            }
            uiCoefCounter += uiRun;
            COVERAGE_ACPRED (pWMVDec, uiCoefCounter);

            if (BS_invalid(pWMVDec->m_pbitstrmIn) || uiCoefCounter >= 64) {
                
                return WMV_CorruptedBits;
            }

             iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];

            // If this coefficient is not in the first column then set the flag which indicates
            // what row it is in. This flag field will be used by the IDCT to see if it can
            // shortcut the IDCT of the row if all coefficients are zero.
            if (iIndex & 0x7)
                iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

            // generate sign mask 0xFFFFFFFF (s=0) or 0x00000000 (s=1)
//            int signBit = iLevel>>31;
             signBit = ~(iSign -1); // 0 or 0xffffffff
//            I32_WMV value = iDoubleStepSize * iLevel + ((iStepMinusStepIsEven ^ signBit) - signBit);
             value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;

            // we have a conditional negative depending on the sign bit. If 
            // sign == -1 or 1, we want 
            // output = sign * (iDoubleStepSize * iLevel + iStepMinusStepIsEven)
            // however, sign is 0 or 1.  note we essentially have a conditional negative.
            // recall -X = X' + 1 
            // we generate a mask from the sign bit
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = (I16_WMV)value;

            uiCoefCounter++;
        } while (!bIsLastRun);
        // Save the DCT row flags. This will be passed to the IDCT routine
        pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
#ifdef _EMB_SSIMD_IDCT_
    if ( pWMVDec->m_b16bitXform) SignPatch(rgiCoefRecon, 128);
#endif

    } else {
        do {
            I32_WMV lIndex = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
            I32_WMV iSign = 0;

            if (lIndex != iTCOEF_ESCAPE)    {
                if (lIndex >= iStartIndxOfLastRun)
                    bIsLastRun = TRUE;
                uiRun = rgRunAtIndx[lIndex];
#if 1
                iSign = BS_getBit(pWMVDec->m_pbitstrmIn);
                iLevel = rgLevelAtIndx[lIndex];
                COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
#else
                iLevel = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
#endif
            } 
            else {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                    // ESC + '1' + VLC
                    I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC1VLC);
                    //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                    if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        return WMV_CorruptedBits;
                    }
                    uiRun = rgRunAtIndx[lIndex2];
                    iLevel = rgLevelAtIndx[lIndex2];
                    if (lIndex2 >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE;
                        iLevel += (I8_WMV) rgIfLastNumOfLevelAtRun[uiRun];
                    }
                    else
                        iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRun[uiRun];
#if 1
                    iSign = BS_getBit(pWMVDec->m_pbitstrmIn); 
                    COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
#else
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
#endif

                }
                else if (BS_getBit(pWMVDec->m_pbitstrmIn)){

                    // ESC + '10' + VLC
                    I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC01VLC);
                    //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                    if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        return WMV_CorruptedBits;
                    }
                    uiRun = rgRunAtIndx[lIndex2];
                    iLevel = rgLevelAtIndx[lIndex2];
                    if (lIndex2 >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE;
                        uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                    }
                    else
                        uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
#if 1
                    iSign = BS_getBit(pWMVDec->m_pbitstrmIn); 
                    COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
#else
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
#endif
                    
                }
                else{
                    // ESC + '00' + FLC
                    COVERAGE_ACCOEFESC (pWMVDec, CV_ACCOEF_ESC00FLC);
                    bIsLastRun = (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
                    if (pWMVDec->m_cvCodecVersion >= WMV1){  // == WMV1 || WMV2
                        WMV_ESC_Decoding();
                        COVERAGE_ACCOEFSIGN (pWMVDec, (iLevel<0));
                    }
                    else{
                        uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);          
                        iLevel = (I8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_LEVEL);
                    }
                    iSign = ((U32_WMV) iLevel) >> 31;
                    if(iLevel <0)
                        iLevel = -iLevel;
                    COVERAGE_ACCOEFSIGN (pWMVDec, iSign);
//                    I32_WMV iSign1 = iLevel >> 31;
//                    iLevel = (iLevel^iSign1)-iSign1;
                }
            }
            uiCoefCounter += uiRun;

            COVERAGE_ACPRED (pWMVDec, uiCoefCounter);

            if (BS_invalid(pWMVDec->m_pbitstrmIn) || uiCoefCounter >= 32) {
                
                return WMV_CorruptedBits;
            }

            // generate sign mask 0xFFFFFFFF (s=0) or 0x00000000 (s=1)
//            int signBit = iLevel>>31;
             signBit = ~(iSign -1); // 0 or 0xffffffff
//            int value = iDoubleStepSize * iLevel + ((iStepMinusStepIsEven ^ signBit) - signBit);
             value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;

            // we have a conditional negative depending on the sign bit. If 
            // sign == -1 or 1, we want 
            // output = sign * (iDoubleStepSize * iLevel + iStepMinusStepIsEven)
            // however, sign is 0 or 1.  note we essentially have a conditional negative.
            // recall -X = X' + 1 
            // we generate a mask from the sign bit
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = (I16_WMV)value;


            uiCoefCounter++;
        } while (!bIsLastRun);
        // Save the DCT row flags. This will be passed to the IDCT routine
        pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
#ifdef _EMB_SSIMD_IDCT_
    if ( pWMVDec->m_b16bitXform) SignPatch(rgiCoefRecon, 64);
#endif
    }

    return WMV_Succeeded;
}



#ifndef _EMB_WMV2_
// _MIXEDPEL_

#define TWOPOINT(output, a, b)                  \
    output = rgiClapTab[(((I32_WMV)a + (I32_WMV)b + 1) >> 1)];

#define FOURPOINT(output, a, b, c, d)           \
    output = rgiClapTab[(I32_WMV)((9 * ((I32_WMV)b + (I32_WMV)c) - ((I32_WMV)a + (I32_WMV)d) + 8) >> 4)];

Void_WMV MotionCompMixed (
                        tWMVDecInternalMember *pWMVDec,
                        U8_WMV*              ppxlcPredMB,
                        const U8_WMV*        ppxlcRefMB,
                        I32_WMV                  iWidthPrev,
                        Bool_WMV                 bInterpolateX,
                        Bool_WMV                 bInterpolateY,
                        I32_WMV                  iMixedPelMV
                        )
{    

    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    U8_WMV* ppxlcPred = ppxlcPredMB;
    U8_WMV tempBlock[11 * 8 + 15];
    U8_WMV *alignedBlock = (U8_WMV*) (((DWORD_PTR)tempBlock + 15) & ~15);
    
    FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMPMIXED_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(MotionCompMixed);

    if (bInterpolateY) {
        if (bInterpolateX) {  //!bXSubPxl && !bYSubPxl
            I32_WMV iy;
            assert(iMixedPelMV == 0);
            for (iy = 0; iy < 8; iy++) {
                memcpy(ppxlcPredMB,ppxlcRefMB,8);                
                ppxlcRefMB += iWidthPrev;
                ppxlcPredMB += iWidthPrev;
            }
        }
        else {  //bXSubPxl && !bYSubPxl            
            U8_WMV* pRef = (U8_WMV*) (ppxlcRefMB + 1);
            
            if (iMixedPelMV == 0) {
                I32_WMV iy; 
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pRef[ix-2], pRef[ix-1], pRef[ix], pRef[ix+1]);
                    }
                    ppxlcPred += iWidthPrev;
                    pRef += iWidthPrev;
                } 
            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___
                I32_WMV iy;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pRef[ix-2], pRef[ix-1], pRef[ix], pRef[ix+1]);
                        TWOPOINT(ppxlcPred[ix], ppxlcPred[ix], pRef[ix]);
                    }

                    ppxlcPred  += iWidthPrev;
                    pRef += iWidthPrev;
                }
            }
        }
    }
    else {
        if (bInterpolateX) {  //!bXSubPxl&& bYSubPxl
            U8_WMV* pRef = (U8_WMV*) (ppxlcRefMB + iWidthPrev);           
            const U8_WMV* pLine4 = pRef + iWidthPrev;
            const U8_WMV* pLine3 = pRef;
            const U8_WMV* pLine2 = pLine3 - iWidthPrev;
            const U8_WMV* pLine1 = pLine2 - iWidthPrev;        
            
            if (iMixedPelMV == 0) {   
                I32_WMV iy;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix; 
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                    }
                    
                    ppxlcPred += iWidthPrev;
                    pLine1 += iWidthPrev;
                    pLine2 += iWidthPrev;
                    pLine3 += iWidthPrev;
                    pLine4 += iWidthPrev;
                }

            } else {
                //      H Q H' 
                //      I                        
                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 1], pLine[ix], pLine[ix + 1], pLine[ix + 2]);
                    }

                    pLine += iWidthPrev;
                    pBlock += 8;
                }
            
                pBlock = alignedBlock;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        I32_WMV iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);                        
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(ppxlcPred[ix], iDiagResult, iVertResult);
                    }

                    ppxlcPred += iWidthPrev;
                    pBlock += 8;
                    pLine1 += iWidthPrev;
                    pLine2 += iWidthPrev;
                    pLine3 += iWidthPrev;
                    pLine4 += iWidthPrev; 
                }     

            }
        }
        else { // bXSubPxl && bYSubPxl
            U8_WMV* pRef = (U8_WMV*) (ppxlcRefMB + iWidthPrev + 1);             
            const U8_WMV* pLine4 = pRef + iWidthPrev;
            const U8_WMV* pLine3 = pRef;
            const U8_WMV* pLine2 = pLine3 - iWidthPrev;
            const U8_WMV* pLine1 = pLine2 - iWidthPrev;                
            if (iMixedPelMV == 0) {

                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 2], pLine[ix - 1], pLine[ix], pLine[ix + 1]);
                    }
                    pLine += iWidthPrev;
                    pBlock += 8;
                }
            
                pBlock = alignedBlock;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(ppxlcPred[ix], pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                     }
                    ppxlcPred += iWidthPrev;
                    pBlock += 8;
                }   
            } else {
                // H Q H
                //     I

                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 2], pLine[ix - 1], pLine[ix], pLine[ix + 1]);
                    }
                    pLine += iWidthPrev;
                    pBlock += 8;
                }
            
                pBlock = alignedBlock;
                for (iy = 0; iy < 8; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        I32_WMV iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(ppxlcPred[ix], iDiagResult, iVertResult);
                     }
                    ppxlcPred += iWidthPrev;
                    pBlock += 8;
                    pLine1 += iWidthPrev;
                    pLine2 += iWidthPrev;
                    pLine3 += iWidthPrev;
                    pLine4 += iWidthPrev; 
                }  

            }
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}

Void_WMV MotionCompMixedAndAddError (
                                   tWMVDecInternalMember *pWMVDec,
                                   U8_WMV* ppxlcCurrQMB, 
                                   const UnionBuffer* ppxliErrorBuf, 
                                   const U8_WMV* ppxlcRef, 
                                   I32_WMV iWidthFrm, Bool_WMV bXEven, Bool_WMV bYEven, 
                                   I32_WMV iMixedPelMV
                                   )
{
    const U8_WMV* rgiClapTab = pWMVDec->m_rgiClapTabDec; 
    U8_WMV tempBlock[11 * 8 + 15];
    U8_WMV *alignedBlock = (U8_WMV*) (((DWORD_PTR)tempBlock + 15) & ~15);
    const PixelI32 *ppxliErrorQMB = ppxliErrorBuf->i32;

    FUNCTION_PROFILE_DECL_START(fpDecode,MOTIONCOMPMIXEDADDERROR_PROFILE);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(MotionCompMixedAndAddError);

    if (bYEven) {
        if (bXEven) {  //!bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        ppxlcCurrQMB [ix] = rgiClapTab [ppxliErrorQMB [ix] + ppxlcRef [ix]];
                    }
                    ppxlcRef += iWidthFrm;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }

            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //         ___                          
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, ppxlcRef[ix - 1], ppxlcRef[ix], ppxlcRef[ix + 1], ppxlcRef[ix + 2]);
                        TWOPOINT(iResult, iResult, ppxlcRef[ix]);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                    ppxlcRef += iWidthFrm;
                }    

            }
        }
        else {  //bXSubPxl && !bYSubPxl
            if (iMixedPelMV == 0) {
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {   
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, ppxlcRef[ix - 1], ppxlcRef[ix], ppxlcRef[ix + 1], ppxlcRef[ix + 2]);
                        ppxlcCurrQMB [ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    ppxlcRef += iWidthFrm;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }

            } else {
                // Handles the case in underline
                // I Q H Q I Q H Q I
                //     ___     ___               
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, ppxlcRef[ix - 1], ppxlcRef[ix], ppxlcRef[ix + 1], ppxlcRef[ix + 2]);
                        TWOPOINT(iResult, iResult, ppxlcRef[ix + 1]);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + 
                            iResult];   
                    }
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                    ppxlcRef += iWidthFrm;
                }   

            }
        }
    }
    else {
        if (bXEven) {  //!bXSubPxl&& bYSubPxl
            if (iMixedPelMV == 0) {

                const U8_WMV* pLine1 = ppxlcRef - iWidthFrm;
                const U8_WMV* pLine2 = ppxlcRef;
                const U8_WMV* pLine3 = pLine2 + iWidthFrm;
                const U8_WMV* pLine4 = pLine3 + iWidthFrm;   
                I32_WMV iy;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {  
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {   
                        I32_WMV iResult;
                        FOURPOINT(iResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        ppxlcCurrQMB [ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    pLine1 += iWidthFrm;
                    pLine2 += iWidthFrm;
                    pLine3 += iWidthFrm;
                    pLine4 += iWidthFrm;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }

            } else {
                //      H Q H' 
                //      I          

                U8_WMV* pRef = (U8_WMV*) (ppxlcRef + iWidthFrm);                   
                const U8_WMV* pLine4 = pRef + iWidthFrm; 
                const U8_WMV* pLine3 = pRef;
                const U8_WMV* pLine2 = pLine3 - iWidthFrm;
                const U8_WMV* pLine1 = pLine2 - iWidthFrm;
                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 1], pLine[ix], pLine[ix + 1], pLine[ix + 2]);
                    }
                    pLine += iWidthFrm;
                    pBlock += 8;
                }
                pBlock = alignedBlock;
                
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult, iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(iResult, iDiagResult, iVertResult);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    pBlock += 8;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                    pLine1 += iWidthFrm;
                    pLine2 += iWidthFrm;
                    pLine3 += iWidthFrm;
                    pLine4 += iWidthFrm;
                }   

            }
        }
        else { // bXSubPxl && bYSubPxl
            if (iMixedPelMV == 0) {

                U8_WMV* pRef = (U8_WMV*) ppxlcRef; 
                const U8_WMV* pLine1 = pRef - iWidthFrm;
                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix - 1], pLine[ix], pLine[ix + 1], pLine[ix + 2]);
                    }
                    pLine += iWidthFrm;
                    pBlock += 8;
                }
                pBlock = alignedBlock;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult;
                        FOURPOINT(iResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];
                    }
                    pBlock += 8;
                    ppxlcCurrQMB += iWidthFrm;
                    ppxliErrorQMB += BLOCK_SIZE;
                }      

            } else {
                // H Q H
                //     I
                //alignedBlock

                U8_WMV* pRef = (U8_WMV*) (ppxlcRef + iWidthFrm + 1); 
                const U8_WMV* pLine4 = pRef + iWidthFrm;
                const U8_WMV* pLine3 = pRef;
                const U8_WMV* pLine2 = pLine3 - iWidthFrm;
                const U8_WMV* pLine1 = pLine2 - iWidthFrm;

                const U8_WMV* pLine = pLine1;
                U8_WMV* pBlock = alignedBlock;
                I32_WMV iy;
                for (iy = 0; iy < 11; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < 8; ix++) {
                        FOURPOINT(pBlock[ix], pLine[ix- 2], pLine[ix - 1], pLine[ix], pLine[ix + 1]);
                    }
                    pLine += iWidthFrm;
                    pBlock += 8;
                }
                pBlock = alignedBlock;
                for (iy = 0; iy < BLOCK_SIZE; iy++) {
                    I32_WMV ix;
                    for (ix = 0; ix < BLOCK_SIZE; ix++) {
                        I32_WMV iResult, iDiagResult, iVertResult;
                        FOURPOINT(iDiagResult, pBlock[ix], pBlock[ix + 8], pBlock[ix + 16], pBlock[ix + 24]);
                        FOURPOINT(iVertResult, pLine1[ix], pLine2[ix], pLine3[ix], pLine4[ix]);
                        TWOPOINT(iResult, iDiagResult, iVertResult);
                        ppxlcCurrQMB[ix] = rgiClapTab [ppxliErrorQMB [ix] + iResult];

                    }
                    pBlock += 8;
                    ppxliErrorQMB += BLOCK_SIZE;
                    ppxlcCurrQMB += iWidthFrm;
                    pLine4 += iWidthFrm;
                    pLine3 += iWidthFrm;
                    pLine2 += iWidthFrm;
                    pLine1 += iWidthFrm;
                }      

            }
        }
    }
    FUNCTION_PROFILE_STOP(&fpDecode);
}
#endif //_EMB_WMV2_

// Interlace Intra Frame Dec

//MMX Bitstream Dequantization functions. For now they are mainly placeholders.



