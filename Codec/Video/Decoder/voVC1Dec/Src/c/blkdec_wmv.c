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
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "codehead.h"
#include "strmdec_wmv.h"
#include "idctdec_wmv2.h"

#ifdef __MFC_
#define new DEBUG_NEW               
#endif // __MFC_

#ifdef WMV_OPT_DQUANT_ARM
        extern Void_WMV Pack32To16_ARMV4(void* pDst,void* pSrc);
        extern Void_WMV ResetCoefACPred1_ARMV4(void* pDst,void* pSrc);
        extern Void_WMV ResetCoefACPred2_ARMV4(void* pDst,void* pSrc);
#endif

//////#pragma warning(disable: 4799)  // emms warning disable
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

    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop; 
    EMB_PBMainLoop *pMainPB = &pPMainLoop->PB[0];
    U8_WMV* piZigzagInv; 

    ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefReconPlus1, 0, (size_t)(MB_SQUARE_SIZE - 4));


    if (pmbmd->m_rgbDCTCoefPredPattern2[ALLBLOCK])
    {
        if (bPredPattern != pWMVDec->m_bRotatedIdct)
        {
#ifndef     WMV_OPT_DQUANT_ARM
            I32_WMV i;
            for (i = 1; i < 8; i++) {
                piQuanCoefACPred [i + BLOCK_SIZE] = 0;
                piQuanCoefACPred [i] = pRef [i] ;
            }
#else
            ResetCoefACPred1_ARMV4 (piQuanCoefACPred, pRef);
#endif
            //piZigzagInv = g_pHorizontalZigzagInv; Old Symmetric matrix, 
            if (pWMVDec->m_bRotatedIdct) //bPredPattern == 0, pWMVDec->m_bRotatedIdct == 1 
                piZigzagInv = (U8_WMV*) pWMVDec->m_pVerticalZigzagInvRotated;
            else //bPredPattern == 1, pWMVDec->m_bRotatedIdct == 0 
                piZigzagInv = (U8_WMV*) pWMVDec->m_pHorizontalZigzagInv;
        }
        else
        {
#ifndef     WMV_OPT_DQUANT_ARM
            I32_WMV i;
            for (i = 1; i < 8; i++) {
                piQuanCoefACPred [i + BLOCK_SIZE] = pRef [i + BLOCK_SIZE];
                piQuanCoefACPred [i] = 0;
            }
#else
            ResetCoefACPred2_ARMV4 (piQuanCoefACPred, pRef);
#endif
            // piZigzagInv = g_pVerticalZigzagInv; Old Symmetric matrix, 
            if (pWMVDec->m_bRotatedIdct) //bPredPattern == 1 , pWMVDec->m_bRotatedIdct == 1
                piZigzagInv = (U8_WMV*) pWMVDec->m_pHorizontalZigzagInvRotated;
            else //bPredPattern == 0, pWMVDec->m_bRotatedIdct == 0
                piZigzagInv = (U8_WMV*) pWMVDec->m_pVerticalZigzagInv;
        }
    }
    else
    {
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

            pmbmd->m_rgbCodedBlockPattern2 [ib], 
            piQuanCoefACPred, 
            piZigzagInv
        );//get the quantized block 
    else
	//	if (pWMVDec->m_cvCodecVersion != MP4S) 
			tWMVStatus = DecodeInverseIntraBlockQuantizeAcPred ( 
				pWMVDec,
				ppIntraDCTTableInfo_Dec, 

				pmbmd->m_rgbCodedBlockPattern2 [ib], 
				piQuanCoefACPred, 
				piZigzagInv
			);//get the quantized block  
	    
    if (WMV_Succeeded != tWMVStatus){
        return tWMVStatus;
    }

    if (pWMVDec->m_iOverlap) {

        I32_WMV     iWidth1;
        I16_WMV     *pOut;
        I32_WMV     ii;

		// cast to 16 bit
		{
#if defined(WMV_OPT_DQUANT_ARM)
			Pack32To16_ARMV4(pWMVDec->m_rgErrorBlock->i16, pWMVDec->m_rgiCoefRecon);
#else   
			for (ii = 0; ii < 64; ii++)
				pWMVDec->m_rgErrorBlock->i16[ii] = (I16_WMV) pWMVDec->m_rgiCoefRecon[ii];
#endif
		}
		if (pWMVDec->m_cvCodecVersion != WMVA){
			SignPatch(pWMVDec->m_rgErrorBlock->i16, 64*2);
		}

		//Intra Block IDCT
		(*pWMVDec->m_pIntraX9IDCT_Dec) (pWMVDec->m_rgErrorBlock, pWMVDec->m_rgErrorBlock, BLOCK_SIZE, 0xff);//pWMVDec->m_iDCTHorzFlags);

		if (pWMVDec->m_iOverlap == 1 || pWMVDec->m_iOverlap == 3 || pmbmd->m_bOverlapIMB) {

			// overlapped transform copy
				if (ib < 4){
					iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 4;
                    pOut = pMainPB->pThreadIntraBlockRow0[ib & 2] + (iMBX << 4) + ((ib & 1) << 3);
				} else {
					iWidth1 = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
                    pOut = (ib == 4) ? pMainPB->pThreadIntraMBRowU0[0] : pMainPB->pThreadIntraMBRowV0[0];
					pOut += iMBX << 3;
				}
#ifdef WMV_OPT_COMMON_ARM
			FASTCPY_8x16_ARMV4(pOut, pWMVDec->m_rgErrorBlock->i16, iWidth1 << 1, BLOCK_SIZE << 1);
#else

			for (ii = 0; ii < 8; ii++){
				memcpy(pOut + ii * iWidth1,
					   pWMVDec->m_rgErrorBlock->i16 + ii * BLOCK_SIZE, 8 * sizeof(I16_WMV));
			}
#endif
	    
		 } else {
			for ( ii = 0; ii < 8; ii++)
					memset (ppxliTextureQMB + ii * iOffsetToNextRowForDCT, 128, 8 * sizeof(U8_WMV));
					pWMVDec->m_pAddError (ppxliTextureQMB, ppxliTextureQMB, pWMVDec->m_rgErrorBlock->i16, iOffsetToNextRowForDCT );
		 }
    } 
	else {
        if ( pWMVDec->m_cvCodecVersion != WMVA){
            SignPatch32_Fun((I32_WMV *)pWMVDec->m_rgiCoefRecon, 128);
        }
        (*pWMVDec->m_pIntraIDCT_Dec) (ppxliTextureQMB, iOffsetToNextRowForDCT, pWMVDec->m_rgiCoefRecon);
    }
    return WMV_Succeeded;
}

////#pragma warning(default: 4799)  // emms warning disable

//Use 0-5 in decoder but Y_BLOCK1-V_BLOCK in encoder. the mess needs to be fixed.
I16_WMV* decodeDiffDCTCoef(tWMVDecInternalMember *pWMVDec,
                           I16_WMV** piQuanCoefACPredTable, 
                           Bool_WMV bLeftMB, 
                           Bool_WMV bTopMB, 
                           Bool_WMV bLeftTopMB, 
                           Bool_WMV* pbPredPattern)
{
    I16_WMV *pDctLeft, *pDctTop, *pDctLeftTop;

    pDctLeft = piQuanCoefACPredTable[bLeftMB];
    pDctTop = piQuanCoefACPredTable[2+bTopMB];
    pDctLeftTop = piQuanCoefACPredTable[4+bLeftTopMB];

    if (abs (*pDctLeftTop - *pDctTop) < abs (*pDctLeftTop - *pDctLeft) + 1){
        *pbPredPattern = 0;
        return pDctLeft;
    }
    else {
        *pbPredPattern = 1;
        return pDctTop;
    }
}
Void_WMV decodeBitsOfESCCode (tWMVDecInternalMember *pWMVDec)
{

    if (pWMVDec->m_iStepSize >= 8 
		&& (((pWMVDec->m_tFrmType == IVOP|| pWMVDec->m_tFrmType == BIVOP) && !pWMVDec->m_bAdvancedProfile)
		|| !pWMVDec->m_bDQuantOn))
	{
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

////#pragma warning(default: 4799)  // emms warning disable


Void_WMV decodeIntraDCAcPredMSV (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCStepSize)
{
    U16_WMV lSzDiffIntraDC;
    lSzDiffIntraDC = (U16_WMV) Huffman_WMV_get (hufDCTDCDec, pWMVDec->m_pbitstrmIn);

    if (BS_invalid ( pWMVDec->m_pbitstrmIn)){
        *pError = WMV_CorruptedBits;
        return;
    }
    if (lSzDiffIntraDC != iTCOEF_ESCAPE) {
        if (lSzDiffIntraDC != 0) {
            //if(pWMVDec->m_bNewDCQuant) 
			{
                if (iDCStepSize == 4)
                    lSzDiffIntraDC = (lSzDiffIntraDC<<1) + (U16_WMV) (BS_getBit ( pWMVDec->m_pbitstrmIn )) -1;
                else if (iDCStepSize == 2)
                    lSzDiffIntraDC = (lSzDiffIntraDC<<2) + (U16_WMV) (BS_getBits ( pWMVDec->m_pbitstrmIn, 2)) -3;
            }
            pWMVDec->m_rgiCoefRecon [0] = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? -lSzDiffIntraDC : lSzDiffIntraDC;  //zou -->DCDifferential = (HWD16)vc1DECBIT_GetBits(pBitstream, BitsToGet);
        }
        else {
            pWMVDec->m_rgiCoefRecon [0] = 0;
        }

        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            *pError = WMV_CorruptedBits;
            return;
        }
    }
    else {
        U32_WMV offset = 0;
        if(iDCStepSize <= 4/* && pWMVDec->m_bNewDCQuant*/)
            offset =  3 - (iDCStepSize>>1);
        lSzDiffIntraDC = (U16_WMV) BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL + offset);
        pWMVDec->m_rgiCoefRecon [0] = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? -lSzDiffIntraDC : lSzDiffIntraDC;
    }
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

    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;

    I32_WMV i;

    
    if (CBP) {
        //I32_WMV iSign = 0;

        do {
            lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
#ifdef STABILITY
            if(lIndex > iTCOEF_ESCAPE )
                return WMV_CorruptedBits;
#endif
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {                               
                return WMV_CorruptedBits;
            }
            if (lIndex != iTCOEF_ESCAPE)    {
                bIsLastRun = (lIndex >= iStartIndxOfLastRun);
                uiRun = rgRunAtIndx[lIndex];

                iLevel = (BS_getBit ( pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                    rgLevelAtIndx[lIndex];
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {

                    
                    return WMV_CorruptedBits;
                }
            }
            else {
                if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {

                        return WMV_CorruptedBits;
                    }
                    // ESC + '1' + VLC
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                    //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
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

                    if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;

                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                        return WMV_CorruptedBits;
                    }
                }
                else if (BS_getBit ( pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                        return WMV_CorruptedBits;
                    }
                    // ESC + '01' + VLC
                    lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                    //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                    //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits (iMAXCODEWORDLNGDCT, decodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        return WMV_CorruptedBits;
                    }
                    uiRun = rgRunAtIndx [lIndex];
                    iLevel = rgLevelAtIndx[lIndex];
                    if (lIndex >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE_WMV;
                        uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel] + 1);
                    }
                    else
                        uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel] + 1);

                    if (BS_getBit ( pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;

                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                        return WMV_CorruptedBits;
                    }
                }
                else{
                    // ESC + '00' + FLC
                    bIsLastRun = (Bool_WMV) BS_getBit ( pWMVDec->m_pbitstrmIn); // escape decoding
                    if (pWMVDec->m_cvCodecVersion >= WMV1){
                        WMV_ESC_Decoding();
                    }
                    else{
                        uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);           
                        iLevel = (I8_WMV)BS_getBits ( pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
                    }
                    if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {

                        return WMV_CorruptedBits;
                    }
                }
            }
            uiCoefCounter += uiRun;

            if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {

				uiCoefCounter &= 63;
				if(!uiCoefCounter) {
					uiCoefCounter = 63;
				}
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
  
    return WMV_Succeeded;
}

#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119                           // see table.13/H.263
tWMVDecodeStatus DecodeIMBAcPred ( tWMVDecInternalMember *pWMVDec,
                                                    CWMVMBMode* pmbmd, 
                                                    CWMVMBMode* pmbmd_lastrow, 
                                                    U8_WMV*      ppxliTextureQMBY,
                                                    U8_WMV*      ppxliTextureQMBU, 
                                                    U8_WMV*      ppxliTextureQMBV, 
                                                    I16_WMV*                piQuanCoefACPred, 
                                                    I16_WMV**                piQuanCoefACPredTable, 
                                                    Bool_WMV                bLeftMB, 
                                                    Bool_WMV                bTopMB, 
                                                    Bool_WMV                bLeftTopMB, I32_WMV imbX)
{
    tWMVDecodeStatus tWMVStatus =0;
    // Y-blocks first (4 blocks)
    U8_WMV * ppxliCodedBlkY = ppxliTextureQMBY;
    Bool_WMV bPredPattern;
    I16_WMV* pRef; 
    Bool_WMV bMBPattern[4]; 
    U32_WMV ib;
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;

#ifdef STABILITY
        if(pmbmd->m_iDCTTable_MB_Index>2)
            return  -1;
#endif

    if (pWMVDec->m_bAdvancedProfile) {
        I32_WMV iMBY = imbX >> 16; // hack-must be cleaned up
        imbX &= 0xffff;
        pWMVDec->m_pDiffMV->iIntra = TRUE;

        return DecodeMB_1MV_WMVA_Intra (pWMVDec, 
                                                        pmbmd, 
                                                        pmbmd_lastrow,
                                                        ppxliTextureQMBY,
                                                        ppxliTextureQMBU,
                                                        ppxliTextureQMBV,
                                                        imbX, iMBY);
    }


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
        ppxliCodedBlkY += (ib != 1) ? BLOCK_SIZE : pWMVDec->m_iWidthPrevYxBlkMinusBlk;
        
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
        pWMVDec->m_iWidthPrevUV,        
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
        pWMVDec->m_iWidthPrevUV,
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



////#pragma warning(disable: 4799)  // emms warning disable
//This is not really an MMX routine but works along side the MMX idct
tWMVDecodeStatus DecodeInverseIntraBlockX9(tWMVDecInternalMember *pWMVDec,
                                                               CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                                               I32_WMV iblk,      Bool_WMV bResidual,
                                                               I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                                                               I16_WMV   *pIntra, CWMVMBMode* pmbmd,
                                                               DQuantDecParam *pDQ )
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    //I32_WMV i2DoublePlusStepSize = pDQ->i2DoublePlusStepSize;
    //I32_WMV i2DoublePlusStepSizeNeg = pDQ->i2DoublePlusStepSizeNeg;
    I32_WMV iDCStepSize = pDQ->iDCStepSize;
    U8_WMV * pZigzagInv = pWMVDec->m_pZigzagScanOrder;

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

    ALIGNED32_FASTMEMCLR (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));//memset (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));

	
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
// 
        do {
            lIndex = (U8_WMV) (  Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn) );  //zou  --->index = vc1DECBIT_GetVLC(pBitstream, pHuffTable);
#ifdef STABILITY
            if(lIndex > iTCOEF_ESCAPE)
                return WMV_Failed;
#endif
            if (BS_invalid(pWMVDec->m_pbitstrmIn)) {                
                return WMV_Failed;
            }
            if (lIndex != iTCOEF_ESCAPE)    {
                bIsLastRun = (lIndex >= iStartIndxOfLastRun);
                uiRun = rgRunAtIndx[lIndex];//
                iLevel = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :  //zou --->sign = (BYTE8)vc1DECBIT_GetBits(pBitstream, 1);
                rgLevelAtIndx[lIndex];
                if (BS_invalid(pWMVDec->m_pbitstrmIn)) {                    
                    return WMV_Failed;
                }
            }
            else {
                if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {                        
                        return WMV_Failed;
                    }
                    // ESC + '1' + VLC
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
                }
                else if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                        
                        return WMV_Failed;
                    }
                    // ESC + '01' + VLC
                    lIndex = (U8_WMV) ( Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn) );
                    if (BS_invalid(pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                        
                        return WMV_Failed;
                    }
                    uiRun = rgRunAtIndx [lIndex];
                    iLevel = rgLevelAtIndx[lIndex];
                    if (lIndex >= iStartIndxOfLastRun){
                        bIsLastRun = TRUE_WMV;
                        uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel] + 1);
                    }
                    else
                        uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel] + 1);
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                        iLevel = -iLevel;
                    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
                        
                        return WMV_Failed;
                    }
                }
                else{
                    // ESC + '00' + FLC
                    bIsLastRun = (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
                    WMV_ESC_Decoding();
                    if (BS_invalid(pWMVDec->m_pbitstrmIn))                        
                        return WMV_Failed;
                }
            }
            uiCoefCounter += uiRun;

            if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
                
 //2008/01/18
				uiCoefCounter &= 63;
				if(!uiCoefCounter) {
					uiCoefCounter = 63;
				}
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

            rgiCoefRecon[0] += pPred[0];
            if (iShift != -1) {           
            I32_WMV i;
            for ( i = 1; i < 8; i++) {
				I32_WMV j;
                j = i<< iShift;
                if(rgiCoefRecon[j]) {
                    rgiCoefRecon[j] += pPred[i];
                }
                else {
                    rgiCoefRecon[(pNonZeroCoeff[iNumNonZeroCoef]=j)] = pPred[i];
                    iNumNonZeroCoef++;
		            /* To avoid error when meeting corrupted bits. 2010.11.08 */
					if( iNumNonZeroCoef >= BLOCK_SQUARE_SIZE) {
						iNumNonZeroCoef &= 63;
						if(!iNumNonZeroCoef) { 
							iNumNonZeroCoef = 1;
						}
					}
                }
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

    SignPatch(rgiCoefRecon, 128);
    // DCT flags are set to all on
    pWMVDec->m_iDCTHorzFlags = 0xff;

    //(*pWMVDec->m_pIntraX9IDCT_Dec) ((UnionBuffer*)pOut, (UnionBuffer*)pOut, iOutStride, pWMVDec->m_iDCTHorzFlags);


    return WMV_Succeeded;
}

//MMX Bitstream Dequantization functions. For now they are mainly placeholders.



