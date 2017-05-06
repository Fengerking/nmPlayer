//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (c) 1996  Microsoft Corporation

Module Name:

    vopmbEnc.cpp

Abstract:

    Encoder VOP composed of MB's

Author:

    Ming-Chieh Lee (mingcl@microsoft.com)
    Bruce Lin (blin@microsoft.com)
            
    20-March-1996

Revision History:

*************************************************************************/
#include "xplatform_wmv.h"
#include "voWmvPort.h"
#include <math.h>

#include "typedef.h"
#include "strmdec_wmv.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"

#ifdef _WMV9AP_

Bool_WMV getCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn) 
{ 
	return pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1];
}

Void_WMV  setCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) 
{
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1] =(U8_WMV) bisCoded;

#ifndef _WMV9AP_
    pmbmd->m_rgbCodedBlockPattern |= bisCoded << (blkn - 1);
#endif

}

Void_WMV  setCodedBlockPattern2 (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) 
{
	pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1] =(U8_WMV) bisCoded;
	//pmbmd->m_rgbCodedBlockPattern2 |= bisCoded << (blkn - 1);

}

#endif

Void_WMV decodePredictIntraCBP (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    U8_WMV iPredCBPY1, iPredCBPY2, iPredCBPY3, iPredCBPY4; 
    U8_WMV *pCBPTop, *pCBPTopLeft, *pCBPLeft, *pCBP;

    pCBPTop = pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern2;
    pCBPTopLeft = pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern2;
    pCBPLeft = pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern2;

   if (imbY != 0 && !pWMVDec->m_pbStartOfSliceRow[imbY]) {
        pCBPTop = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
    }
    if (imbX != 0) {
        pCBPLeft = (pmbmd - 1)->m_rgbCodedBlockPattern2;
    }
    if ((imbY != 0 && !pWMVDec->m_pbStartOfSliceRow[imbY]) && imbX != 0) {
        pCBPTopLeft = (pmbmd - pWMVDec->m_uintNumMBX - 1)->m_rgbCodedBlockPattern2;
    }
    pCBP = pmbmd->m_rgbCodedBlockPattern2;

    iPredCBPY1 = (pCBPTopLeft [3] == pCBPTop [2]) ? pCBPLeft [1] : pCBPTop [2];
    pCBP [0] = pCBP [0] ^ iPredCBPY1;
    iPredCBPY2 = (pCBPTop [2] == pCBPTop [3]) ? pCBP [0] : pCBPTop [3];
    pCBP [1] = pCBP [1] ^ iPredCBPY2;
    iPredCBPY3 = (pCBPLeft [1] == pCBP [0]) ? pCBPLeft [3] : pCBP [0];
    pCBP [2] = pCBP [2] ^ iPredCBPY3;
    iPredCBPY4 = (pCBP [0] == pCBP [1]) ? pCBP [2] : pCBP [1];
    pCBP [3] = pCBP [3] ^ iPredCBPY4;
}



//#include "alttables.h"

tWMVDecodeStatus decodeDQuantParam (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDQuantParam);
    assert (pWMVDec->m_bDQuantOn);
#if 1//def _NEWQP_
    if (pWMVDec->m_iPanning){
        if (pWMVDec->m_iPanning & pmbmd->m_bBoundary)
            pmbmd->m_iQP = 2 * pWMVDec->m_iDQuantBiLevelStepSize - 1;
        else
            pmbmd->m_iQP = 2 * pWMVDec->m_iStepSize - 1 + pWMVDec->m_bHalfStep;
    }
    else{
        if (pWMVDec->m_bDQuantBiLevel) {
            pmbmd->m_iQP = BS_getBits(pWMVDec->m_pbitstrmIn, 1) ? 2 * pWMVDec->m_iDQuantBiLevelStepSize - 1 
                         : 2 * pWMVDec->m_iStepSize - 1 + pWMVDec->m_bHalfStep;
        } else { /// This will not work with _NEWQP_ ... FIX!!!!
            pmbmd->m_iQP = decodeDQuantStepsize(pWMVDec, 0); //m_pbitstrmIn->getBits (5);
        }
    }
    
    if (pmbmd->m_iQP < 1 || pmbmd->m_iQP > 62) {
        return WMV_Failed;
    }
    
#else
    if (pWMVDec->m_iPanning){
        if (pWMVDec->m_iPanning & pmbmd->m_bBoundary)
            pmbmd->m_iQP = pWMVDec->m_iDQuantBiLevelStepSize;
        else
            pmbmd->m_iQP = pWMVDec->m_iStepSize;
    }
    else{
        if (pWMVDec->m_bDQuantBiLevel) {
            pmbmd->m_iQP = m_pbitstrmIn->getBits (1) ? m_iDQuantBiLevelStepSize 
                         : m_iStepSize;
            
        } else {
            pmbmd->m_iQP = decodeDQuantStepsize(pWMVDec, 0); //BS_getBits(pWMVDec->m_pbitstrmIn, 5);
        }
    }
    
    if (pmbmd->m_iQP < 1 || pmbmd->m_iQP > 31) {
        return WMV_Failed;
    }
#endif
    
    return WMV_Succeeded;
}

I32_WMV decodeDQuantStepsize(tWMVDecInternalMember *pWMVDec, I32_WMV iShift)
{
    I32_WMV iQP = BS_getBits (pWMVDec->m_pbitstrmIn, 3);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDQuantStepsize);

    if (iQP == 7)
        iQP = BS_getBits (pWMVDec->m_pbitstrmIn, 5);
    else
        iQP += pWMVDec->m_iStepSize + iShift;

    if (iShift == 0) // MB level Qp needs to be remapped
        return 2 * iQP - 1;
    else
    return iQP;
}



static I32_WMV s_iMBXformTypeLUT[16] = {
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
};
static I32_WMV s_iSubblkPatternLUT[16] = {0, 1, 2, 3, 1, 2, 3, 0, 0, 1, 2, 3, 1, 2, 3, 0};


tWMVDecodeStatus decodeMBOverheadOfPVOP_WMV3 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd,  I32_WMV iMBX, I32_WMV iMBY)
{
    I32_WMV iCBPCY = 0, dummy1, dummy2;//, iPos=0, iTemp=0;
    Bool_WMV bSendTableswitchMode = FALSE_WMV;
    Bool_WMV bSendXformswitchMode = FALSE_WMV;
    CDiffMV *pDiffMV = pWMVDec->m_pDiffMV;
    CInputBitStream_WMV *pbitstrmIn = pWMVDec->m_pbitstrmIn;
    I8_WMV chMBMode;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMBOverheadOfPVOP_WMV3);

    pmbmd->m_iQP = ((pWMVDec->m_iStepSize << 1) - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE_WMV;
    
    if (pWMVDec->m_iMVSwitchCodingMode == SKIP_RAW) {
        pmbmd->m_chMBMode = (BS_getBit(pbitstrmIn) ? MB_4MV : MB_1MV);
    }
    chMBMode = pmbmd->m_chMBMode;

    if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pbitstrmIn);
    }
    
    if (pmbmd->m_bSkip) {
#ifdef _WMV9AP_
                memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));
#else
        pmbmd->m_rgbCodedBlockPattern = 0;
#endif
        // handle hybrid MV (needed for skipbit too)
        if (chMBMode == MB_1MV) {
            memset (pDiffMV, 0, sizeof(CDiffMV));
            if (PredictHybridMV (pWMVDec, iMBX << 1, iMBY << 1, TRUE, &dummy1, &dummy2))
                pDiffMV->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);
            else
                pDiffMV->iHybrid = 0;
        } else {
            I32_WMV iblk;
            memset (pDiffMV, 0, sizeof(CDiffMV) << 2);
            for ( iblk = 0; iblk < 4; iblk++) {
                if (PredictHybridMV (pWMVDec, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1), FALSE, &dummy1, &dummy2))
                    (pDiffMV + iblk)->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);
                else
                    (pDiffMV + iblk)->iHybrid = 0;
                // hybrid MV - need to compute actual motion vectors too!
                ComputeMVFromDiffMV (pWMVDec, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1),
                    pDiffMV + iblk, FALSE_WMV);
            }
        }
        goto OneMVEnd;
    }
    
    if (chMBMode == MB_1MV) {
        // 1MV - read motion vector
        
        decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, pDiffMV);    
        
        // hybrid MV
        pDiffMV->iHybrid = 0;
        if (PredictHybridMV (pWMVDec, iMBX << 1, iMBY << 1, TRUE, &dummy1, &dummy2) && !pDiffMV->iIntra)
            pDiffMV->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);
        
        // skip to end if last = 0
        if (pDiffMV->iLast == 0) {
            pmbmd->m_bCBPAllZero = TRUE_WMV;
            
            if (pWMVDec->m_bDQuantOn && pDiffMV->iIntra) {
                if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                    return WMV_CorruptedBits; 
                }
            }
            // One MV Intra AC Prediction Flag
            if (pDiffMV->iIntra)
                pmbmd->m_rgbDCTCoefPredPattern2[ 0 ] = (U8_WMV)BS_getBit (pbitstrmIn);              
            
            goto End;
        }
        bSendTableswitchMode = pWMVDec->m_bDCTTable_MB;
        bSendXformswitchMode = pWMVDec->m_bMBXformSwitching && !pDiffMV->iIntra;
        
        // One MV Intra AC Prediction Flag
        if (pDiffMV->iIntra)
            pmbmd->m_rgbDCTCoefPredPattern2[0]  =  (U8_WMV)BS_getBit (pbitstrmIn);              
        
    }
    
    // read CBP
    iCBPCY = Huffman_WMV_get (pWMVDec->m_pHufNewPCBPCYDec, pbitstrmIn);
    
    if (pWMVDec->m_bDQuantOn && chMBMode == MB_1MV) {
        if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
            return WMV_CorruptedBits;
        }
    }
    
    pmbmd->m_bCBPAllZero = FALSE_WMV;
    
    if (chMBMode == MB_4MV) {
        // 4MV - read motion vectors
        I32_WMV iCBPCY_V8 = iCBPCY & 0x30;
        I32_WMV iIntraYUV = 0;
        I32_WMV iIntraChroma = 0;
        I32_WMV iIntraPred = 0;
        I32_WMV iblk;
        Bool_WMV bDecodeDQ = FALSE;
        
        for ( iblk = 0; iblk < 4; iblk++) {
            I32_WMV iIntra;
            
            if (iCBPCY & (1 << iblk)) {
                decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pDiffMV + iblk));    
            }
            else
                memset (pDiffMV + iblk, 0, sizeof (CDiffMV));
            // hybrid MV
            (pDiffMV + iblk)->iHybrid = 0;
            if (PredictHybridMV (pWMVDec, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1), FALSE, &dummy1, &dummy2) 
                && !((pDiffMV + iblk)->iIntra))
                (pDiffMV + iblk)->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);
            // hybrid MV - need to compute actual motion vectors too!
            iIntra = ComputeMVFromDiffMV (pWMVDec, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1),
                                          pDiffMV + iblk, FALSE_WMV);

            
            if (iIntra)
                iIntraPred |= IsIntraPredY(pWMVDec, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1));
            
            // enable transform switching if necessary
            iIntraChroma += iIntra;
            iIntraYUV |= (iIntra << iblk);
            iCBPCY_V8 |= ((pDiffMV + iblk)->iLast << iblk);
        }
        iIntraYUV |= (iIntraChroma < 3)? 0 : 0x30;
        bSendTableswitchMode = (pWMVDec->m_bDCTTable_MB && iCBPCY_V8);
        bSendXformswitchMode = (pWMVDec->m_bMBXformSwitching && (~iIntraYUV & iCBPCY_V8));
        bDecodeDQ |= (iCBPCY_V8 != 0) || (iIntraChroma != 0);
        if (iIntraChroma > 2) {
            iIntraPred |= IsIntraPredUV(pWMVDec, iMBX, iMBY);
            // iIntraPred |= IsIntraPredUV(pWMVDec, iMBX, iMBY);       // this line looks reduntant.
        }
        
        iCBPCY = iCBPCY_V8;
        if (pWMVDec->m_bDQuantOn && bDecodeDQ) {
            if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                return WMV_CorruptedBits;
            }
        }
        
        if (iIntraPred) 
            pmbmd->m_rgbDCTCoefPredPattern2[0]  = (U8_WMV)BS_getBit (pbitstrmIn);              
    }
    
    // 0 :  0
    // 1 : 10
    // 2 : 11
    if (bSendTableswitchMode) {
        I32_WMV iDCTMBTableIndex = BS_getBit(pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
End:
    if (bSendXformswitchMode) {
        I32_WMV iIndex = Huffman_WMV_get (pWMVDec->m_pHufMBXformTypeDec, pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        pmbmd->m_iMBXformMode = s_iMBXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = s_iSubblkPatternLUT[iIndex];
        pmbmd->m_bFirstBlockDecoded = FALSE;
    }
    
#ifdef _WMV9AP_
        setCodedBlockPattern2 (pmbmd, Y_BLOCK1, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, Y_BLOCK2, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, Y_BLOCK3, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, Y_BLOCK4, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, U_BLOCK, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, V_BLOCK, iCBPCY & 0x1);
#else
    pmbmd->m_rgbCodedBlockPattern = (U8_WMV) iCBPCY; // iCBPC info is already in iCBPCY. 
#endif

OneMVEnd:
    
    if (chMBMode == MB_1MV)
        ComputeMVFromDiffMV (pWMVDec, iMBX << 1, iMBY << 1, pDiffMV, TRUE);
    
    if (chMBMode == MB_1MV) {
        I32_WMV iStride = (I32_WMV) (pWMVDec->m_uintNumMBX << 1);
        I32_WMV iMVOffst = 2 * (iStride * iMBY + iMBX);
        I16_WMV *pXMotion = pWMVDec->m_pXMotion + iMVOffst;
        I16_WMV *pYMotion = pWMVDec->m_pYMotion + iMVOffst;
        I16_WMV XMotion = *pXMotion++;      // m_pXMotion[iMVOffset+1]
        I16_WMV YMotion = *pYMotion++;      // m_pYMotion[iMVOffset+1]
        *pXMotion   = XMotion;              // m_pXMotion[iMVOffset+1]
        *pYMotion   = YMotion;              // m_pYMotion[iMVOffset+1]
        pXMotion   += iStride;
        pYMotion   += iStride;
        *pXMotion-- = XMotion;              // m_pXMotion[iMVOffset+iStride+1]
        *pYMotion-- = YMotion;              // m_pYMotion[iMVOffset+iStride+1]
        *pXMotion   = XMotion;              // m_pXMotion[iMVOffset+iStride]
        *pYMotion   = YMotion;              // m_pYMotion[iMVOffset+iStride]
    }
    
    if (BS_invalid(pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    
    return WMV_Succeeded;
}

static I32_WMV gFineMV[] = {0,2,3,4,5,8};
static I32_WMV aMVOffsets[] = {0,1,3,7,15,31};

Void_WMV decodeMV_V9(tWMVDecInternalMember *pWMVDec, Huffman_WMV* pHufMVTable, I32_WMV iMode, CDiffMV *pDiffMV)
{
    I32_WMV iFine=0; 
    const I32_WMV  iNm1 = 5;
    I32_WMV iHalfpelMV;
    I32_WMV iIndex1;
    I32_WMV iSign;
    I32_WMV len;
    CInputBitStream_WMV *pbitstrmIn = pWMVDec->m_pbitstrmIn;
    I32_WMV iIndex = Huffman_WMV_get(pHufMVTable, pbitstrmIn);
    I32_WMV iTemp;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMV_V9);
    

    if (iMode == LH_MV0) {
        ++iIndex;
        // last symbol
        if (iIndex >= 37) {
            pDiffMV->iLast = 1;
            iIndex -= 37;
        }
        else
            pDiffMV->iLast = 0;
    }
    else
        pDiffMV->iLast = 1;
    
    pDiffMV->iIntra = 0;
    
    switch (iIndex) {
    case 0:
        pDiffMV->iX = pDiffMV->iY = 0;
        break;
    case 36: // intra
        pDiffMV->iIntra = 1;
        pDiffMV->iX = pDiffMV->iY = 0;
        break;
    case 35: // long MV
        iHalfpelMV = pWMVDec->m_pAltTables->m_iHalfPelMV;
        pDiffMV->iX = BS_getBits(pbitstrmIn, pWMVDec->m_iLogXRange - iHalfpelMV);
        pDiffMV->iY = BS_getBits(pbitstrmIn, pWMVDec->m_iLogYRange - iHalfpelMV);
        break;
    default:
        iTemp = pWMVDec->m_pAltTables->m_iHalfPelMV;
        iIndex1 = prvWMVModulus  ((I32_WMV)(iIndex), (I32_WMV)(iNm1 + 1));
        iHalfpelMV = iTemp && (iIndex1 == iNm1);
        len = gFineMV[iIndex1] - iHalfpelMV;
        if (len>0)
            iFine = BS_getBits(pbitstrmIn,len);
        else
            iFine = 0;
        iSign = - (iFine & 1);
        pDiffMV->iX = iSign ^ ((iFine >> 1) + aMVOffsets[iIndex1]);
        pDiffMV->iX -= iSign;
        
        iIndex = prvWMVDivision (iIndex , (iNm1 + 1));
        iHalfpelMV = iTemp && (iIndex == iNm1);
        len = gFineMV[iIndex] - iHalfpelMV;
        
        if(len>0)
            iFine = BS_getBits(pbitstrmIn,gFineMV[iIndex] - iHalfpelMV);
        else
            iFine = 0;
        iSign = - (iFine & 1);
        pDiffMV->iY = iSign ^ ((iFine >> 1) + aMVOffsets[iIndex]);
        pDiffMV->iY -= iSign;
    }
}

#ifdef _WMV9AP_

tWMVDecodeStatus decodeMBOverheadOfIVOP_MSV (tWMVDecInternalMember *pWMVDec,CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iBlk, iCBPCY;

    iCBPCY = Huffman_WMV_get (&pWMVDec->m_hufICBPCYDec, pWMVDec->m_pbitstrmIn);  //zou  --->vc1DECBIT_GetVLC(pBitstream, vc1DEC_I_Picture_CBPCY_VLC);
    
    if (iCBPCY < 0 || iCBPCY > 63 || BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = ((iCBPCY >> (5 - iBlk)) & 1);
    }
    decodePredictIntraCBP (pWMVDec, pmbmd, imbX, imbY);

    if (pWMVDec->m_cvCodecVersion != WMVA || (pWMVDec->m_cvCodecVersion == WMVA && 
        pWMVDec->m_iACPredIFrameCodingMode == SKIP_RAW)) {
    	pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBits( pWMVDec->m_pbitstrmIn, 1);  //zou --> vc1DECBITPL_ReadBitplaneBit(&pState->sPicParams.sBPPredictAC, pBitstream);
    }

	if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
		//;
		return WMV_CorruptedBits;
	}
	pmbmd->m_bSkip = FALSE;
	pmbmd->m_dctMd = INTRA;
    pmbmd->m_iDCTTable_MB_Index = 0;

    iCBPCY = 0;
    for (iBlk = 0; iBlk < 6; iBlk++) {
        // this is only for WMV2 code below / m_bDCTTable_MB && iCBPCY
        iCBPCY |= (pmbmd->m_rgbCodedBlockPattern2 [iBlk] << (5 - iBlk)); 
    }


    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
    if ((pWMVDec->m_iOverlap & 4) && pWMVDec->m_iOverlapIMBCodingMode == SKIP_RAW)  //zou ---> /* conditional overlap flags */
        pmbmd->m_bOverlapIMB = (BS_getBits( pWMVDec->m_pbitstrmIn, 1) ? MB_4MV : MB_1MV);  //zou -->Value = vc1DECBITPL_ReadBitplaneBit(&pState->sPicParams.sBPOverflags, pBitstream);
    
    if (pWMVDec->m_bAdvancedProfile) {
        pmbmd->m_iQP = 2 * pWMVDec->m_iStepSize - 1 + pWMVDec->m_bHalfStep;
        if (pWMVDec->m_bDQuantOn) {
            if (ICERR_OK != decodeDQuantParam (pWMVDec, pmbmd))
                return WMV_CorruptedBits;
        }
    }


    return WMV_Succeeded;
}


#else

tWMVDecodeStatus decodeMBOverheadOfIVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    CWMVMBMode* pmbmdTop = pmbmd - pWMVDec->m_uintNumMBX;
    CWMVMBMode* pmbmdLeftTop = pmbmdTop - 1;
    CWMVMBMode* pmbmdLeft = pmbmd - 1;
    I32_WMV iCBPCY;
    I32_WMV iPredCBPY1, iPredCBPY2, iPredCBPY3, iPredCBPY4; 
    I32_WMV iLeftTopCBPY4, iTopCBPY3, iTopCBPY4, iLeftCBPY2, iLeftCBPY4;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMBOverheadOfIVOP_MSV);
    
    if (imbY == 0) {
        pmbmdTop = pWMVDec->m_pmbmdZeroCBPCY;
        pmbmdLeftTop = pWMVDec->m_pmbmdZeroCBPCY;
    }
    if (imbX == 0) {
        pmbmdLeft = pWMVDec->m_pmbmdZeroCBPCY;
        pmbmdLeftTop = pWMVDec->m_pmbmdZeroCBPCY;
    }
    
    iCBPCY = Huffman_WMV_get (&pWMVDec->m_hufICBPCYDec, pWMVDec->m_pbitstrmIn);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
        return WMV_CorruptedBits;
    }
    if (iCBPCY < 0 || iCBPCY > 63) {
        return WMV_CorruptedBits;
    }
    COVERAGE_SPECIAL_ICBPCY(pWMVDec, imbX, imbY, iCBPCY);
    
    pmbmd->m_rgbDCTCoefPredPattern2[0] = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
        return WMV_CorruptedBits;
    }
    COVERAGE_ACPRED_FRAME(pWMVDec, imbX, imbY, pmbmd->m_rgbDCTCoefPredPattern2[0]);

    pmbmd->m_bSkip = FALSE_WMV;
    pmbmd->m_dctMd = INTRA;
    
    iLeftTopCBPY4 = (pmbmdLeftTop->m_rgbCodedBlockPattern & (1<<2)) >> 2;
    iTopCBPY3 = (pmbmdTop->m_rgbCodedBlockPattern & (1<<3)) >> 3;
    iTopCBPY4 = (pmbmdTop->m_rgbCodedBlockPattern & (1<<2)) >> 2;
    iLeftCBPY2 = (pmbmdLeft->m_rgbCodedBlockPattern & (1<<4)) >> 4;
    iLeftCBPY4 = (pmbmdLeft->m_rgbCodedBlockPattern & (1<<2)) >> 2;
    
    iPredCBPY1 = (iLeftTopCBPY4 == iTopCBPY3 ) ? iLeftCBPY2 : iTopCBPY3;
    iPredCBPY1 ^= ((iCBPCY >> 5) & 0x00000001);
    
    iPredCBPY2 = (iTopCBPY3 == iTopCBPY4)? iPredCBPY1 : iTopCBPY4;
    iPredCBPY2 ^= ((iCBPCY >> 4) & 0x00000001);
    
    iPredCBPY3 = (iLeftCBPY2 == iPredCBPY1) ? iLeftCBPY4 : iPredCBPY1;
    iPredCBPY3 ^= ((iCBPCY >> 3) & 0x00000001);
    
    iPredCBPY4 = (iPredCBPY1 == iPredCBPY2)? iPredCBPY3: iPredCBPY2;
    iPredCBPY4 ^= ((iCBPCY >> 2) & 0x00000001);
    
    pmbmd->m_rgbCodedBlockPattern = (iPredCBPY1<<5)  | (iPredCBPY2<<4) |
        (iPredCBPY3<<3) | (iPredCBPY4<<2) | (iCBPCY & 0x00000003);
    
    iCBPCY = iPredCBPY1 | iPredCBPY2 | iPredCBPY3 | iPredCBPY4 | (iCBPCY & 0x00000003);
    // 0 :  0
    // 1 : 10
    // 2 : 11
    pmbmd->m_iDCTTable_MB_Index = 0;
    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBit (pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit (pWMVDec->m_pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    COVERAGE_DCTTAB (pWMVDec, imbX, imbY, pmbmd->m_iDCTTable_MB_Index);
    
    return WMV_Succeeded;
}
#endif

tWMVDecodeStatus decodeMBOverheadOfPVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd)
{
    I32_WMV iCBPCY = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMBOverheadOfPVOP_MSV);
    
    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1);
        
    if (pWMVDec->m_bCODFlagOn) {
        if (pWMVDec->m_bSKIPBIT_CODING_ == FALSE_WMV) {
            pmbmd->m_bSkip = BS_getBit (pWMVDec->m_pbitstrmIn);
            if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
                return WMV_CorruptedBits;
            }
        }
        if (pmbmd->m_bSkip) {
#ifdef _WMV9AP_
            memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));
#else
            pmbmd->m_rgbCodedBlockPattern = 0;
#endif
            pmbmd->m_dctMd = INTER;
            return WMV_Succeeded;
        }
    }
    else
        pmbmd->m_bSkip = FALSE_WMV; // might be reset later
    
    if (pWMVDec->m_bNEW_PCBPCY_TABLE) {
        iCBPCY  = Huffman_WMV_get (pWMVDec->m_pHufNewPCBPCYDec, pWMVDec->m_pbitstrmIn);
    } else {
        iCBPCY  = Huffman_WMV_get (&pWMVDec->m_hufPCBPCYDec, pWMVDec->m_pbitstrmIn);
    }
    if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
        return WMV_CorruptedBits;
    }
    if (iCBPCY < 0 || iCBPCY > 127) {
        return WMV_CorruptedBits;
    }
    
    if (iCBPCY & 0x00000040){ // INTER MB iCBPCY == CBPCY + 64
        pmbmd->m_dctMd = INTER;
        iCBPCY ^= 0x00000040;
        pmbmd->m_bCBPAllZero = (iCBPCY == 0);
        //_HYBRID_MV_
        if (pWMVDec->m_bMBHybridMV)
            pmbmd->m_iMVPredDirection = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
        else
            pmbmd->m_iMVPredDirection = 2;
    }
    else{
        pmbmd->m_dctMd = INTRA;
        
        pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV) BS_getBit (pWMVDec->m_pbitstrmIn);
        
        // YU = 00 :0 
        // YU = 01 :110
        // YU = 10 :10
        // YU = 11 :111
        if (pWMVDec->m_bDCPred_IMBInPFrame){
            I32_WMV ibYUV_IPred;
            ibYUV_IPred = BS_getBit (pWMVDec->m_pbitstrmIn);
            if (!ibYUV_IPred){
                // [V_BLOCK] = [U_BLOCK] = [Y_BLOCK1] = 0
                // pmbmd->m_rgbDCTCoefPredPattern &= 0x9D;
                pmbmd->m_rgbDCTCoefPredPattern2 [Y_BLOCK1] = 0;
                pmbmd->m_rgbDCTCoefPredPattern2 [U_BLOCK]  = 0;
            }
            else{
                ibYUV_IPred = BS_getBit (pWMVDec->m_pbitstrmIn);
                if (!ibYUV_IPred){
                    // [Y_BLOCK1] = 1
                    // [V_BLOCK] = [U_BLOCK] = 0
                    //pmbmd->m_rgbDCTCoefPredPattern |= 0x02;
                    //pmbmd->m_rgbDCTCoefPredPattern &= 0x9F;
                    pmbmd->m_rgbDCTCoefPredPattern2 [Y_BLOCK1] = 1;
                    pmbmd->m_rgbDCTCoefPredPattern2 [U_BLOCK]  = 0;
                    
                }
                else if ((ibYUV_IPred = BS_getBit (pWMVDec->m_pbitstrmIn))){
                    // [V_BLOCK] = [U_BLOCK] = [Y_BLOCK1] = 1
                    // pmbmd->m_rgbDCTCoefPredPattern |= 0x62;
                    pmbmd->m_rgbDCTCoefPredPattern2 [Y_BLOCK1] = 1;
                    pmbmd->m_rgbDCTCoefPredPattern2 [U_BLOCK]  = 1;
                    
                }else{
                    // [Y_BLOCK1] = 0
                    // [V_BLOCK] = [U_BLOCK] = 1
                    //pmbmd->m_rgbDCTCoefPredPattern &= 0xFD;
                    //pmbmd->m_rgbDCTCoefPredPattern |= 0x60;
                    pmbmd->m_rgbDCTCoefPredPattern2 [Y_BLOCK1] = 0;
                    pmbmd->m_rgbDCTCoefPredPattern2 [U_BLOCK]  = 1;
                }
            }
            
            //pmbmd->m_rgbDCTCoefPredPattern &= 0xFB; // [Y_BLOCK2] = 0;
            //pmbmd->m_rgbDCTCoefPredPattern |= 0x08; // [Y_BLOCK3] = 1;
            pmbmd->m_rgbDCTCoefPredPattern2 [Y_BLOCK2] = 0;
            pmbmd->m_rgbDCTCoefPredPattern2 [Y_BLOCK3] = 1;
            pmbmd->m_rgbDCTCoefPredPattern2 [V_BLOCK]  = pmbmd->m_rgbDCTCoefPredPattern2 [U_BLOCK]; 
        }
        
        if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
            return WMV_CorruptedBits;
        }
    }
    
    // 0 :  0
    // 1 : 10
    // 2 : 11
    pmbmd->m_iDCTTable_MB_Index = 0;
    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBit (pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit (pWMVDec->m_pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
#ifdef _WMV9AP_
        setCodedBlockPattern2 (pmbmd, Y_BLOCK1, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, Y_BLOCK2, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, Y_BLOCK3, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, Y_BLOCK4, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, U_BLOCK, iCBPCY & 0x1); iCBPCY >>= 1;
    setCodedBlockPattern2 (pmbmd, V_BLOCK, iCBPCY & 0x1);
#else
    pmbmd->m_rgbCodedBlockPattern = (U8_WMV) iCBPCY; // iCBPC info is already in iCBPCY. 
#endif

    if (pWMVDec->m_bXformSwitch) {
        pmbmd->m_bBlkXformSwitchOn = FALSE_WMV;
        
        if (pWMVDec->m_bMBXformSwitching && !pmbmd->m_bCBPAllZero && pmbmd->m_dctMd == INTER) {
            if ( !(pmbmd->m_bBlkXformSwitchOn = BS_getBit (pWMVDec->m_pbitstrmIn)) ) {
                // Block-based transform-switching must not be enabled for this MB so Huffman_WMV_get the
                // transform-type that will be applied to all blocks in this MB
                if (BS_getBit (pWMVDec->m_pbitstrmIn) == 0)
                    pmbmd->m_iMBXformMode = XFORMMODE_8x8;
                else if (BS_getBit (pWMVDec->m_pbitstrmIn) == 0)
                    pmbmd->m_iMBXformMode = XFORMMODE_8x4;
                else
                    pmbmd->m_iMBXformMode = XFORMMODE_4x8;            
            }
        }
    }
    return WMV_Succeeded;
}


#define MVFLCSIZE 6
#define MV_ESCAPE 1099

I32_WMV ReadSliceHeaderWMVA (tWMVDecInternalMember *pWMVDec, I32_WMV imbY)
{
    I32_WMV imbYSlice;
    Bool_WMV bPicHeader;
    I32_WMV result = ICERR_OK;
    I32_WMV iFrameType;

    imbYSlice = BS_getBits(pWMVDec->m_pbitstrmIn, 9);
    if(pWMVDec->m_bCodecIsWVC1) {
		pWMVDec->SliceY = imbYSlice;//zou 330
    }

    if (pWMVDec->m_bInterlaceV2 && pWMVDec->m_bFieldMode && pWMVDec->m_iCurrentTemporalField == 1) {
        if (imbYSlice != (imbY + ((I32_WMV)pWMVDec->m_uintNumMBY))) {
			assert (0);
			return ICERR_ERROR;
		}
	} else {
        if (imbYSlice != imbY) {
			assert (0);
			return ICERR_ERROR;
		}
	}

    bPicHeader = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    if (!bPicHeader) {
        return ICERR_OK;
    }
    if (bPicHeader) {
#ifdef DUMPLOG
        {
        FILE *fLog = fopen("c:/wmva.log","a");
        fprintf(fLog, "PicHeader in Slice %d:\n", imbYSlice);
        fclose(fLog);
        }
#endif
        if (pWMVDec->m_bInterlacedSource) { 
            if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
                iFrameType = PROGRESSIVE;     // Progressive
            else {
                if (0 == BS_getBits(pWMVDec->m_pbitstrmIn, 1))
                    iFrameType = INTERLACEFRAME; // Frame Interlace
                else    
                    iFrameType = INTERLACEFIELD; // Field interlace
            }
        }
        else 
            iFrameType = PROGRESSIVE;    // Progressive
//        g_iWMVAFrameType = (I32_WMV) iFrameType;
        if (iFrameType == PROGRESSIVE) {
            pWMVDec->m_bInterlaceV2 = FALSE;
            pWMVDec->m_iCurrentField = 0;
           result = decodeVOPHeadProgressiveWMVA (pWMVDec);
           assert (pWMVDec->m_tFrmType != SKIPFRAME);
        } else if (iFrameType == INTERLACEFRAME){//Frame Interlace
            pWMVDec->m_bInterlaceV2 = TRUE;
            pWMVDec->m_iCurrentField = 0;
#ifdef PPCWMP
            return WMV_UnSupportedCompressedFormat;
#else
            result = decodeVOPHeadInterlaceV2 (pWMVDec);
#endif            
			assert (pWMVDec->m_tFrmType != SKIPFRAME);
        } else {
            pWMVDec->m_bInterlaceV2 = TRUE;
            pWMVDec->m_bFieldMode = TRUE;
#ifdef PPCWMP
            return WMV_UnSupportedCompressedFormat;
#else
            result = decodeVOPHeadFieldPicture (pWMVDec);
#endif
            if (result != ICERR_OK) {
                ;
                return result;
            }
            result = decodeFieldHeadFieldPicture (pWMVDec);
            assert (pWMVDec->m_tFrmType != SKIPFRAME);
        }

    }

    if (result != ICERR_OK) {
        ;
        return result;
    }
    return ICERR_OK;
}

tWMVDecodeStatus SliceStartCode(tWMVDecInternalMember *pWMVDec, I32_WMV imbY)
{
    Bool_WMV bNot1stRowAfterStartCode;    
    I32_WMV startcode, startcodeinfo;
    //    I32_WMV dummystartcodeinfo;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SliceStartCode);
    
    if(imbY != 0) {
        bNot1stRowAfterStartCode = BS_getBit(pWMVDec->m_pbitstrmIn);
        if(!bNot1stRowAfterStartCode) {                   
            I32_WMV BitsLeft = (BS_BitsLeft(pWMVDec->m_pbitstrmIn) & 7);
            /*I32_WMV Bits = */BS_getBits(pWMVDec->m_pbitstrmIn, BitsLeft);
            startcode = BS_getBits(pWMVDec->m_pbitstrmIn, LENSTARTCODE);
            if(startcode == STARTCODE1) {
                startcodeinfo = BS_getBits(pWMVDec->m_pbitstrmIn, 24);
                startcodeinfo = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            }
            else if(startcode == STARTCODE2) {
                startcodeinfo = BS_getBits(pWMVDec->m_pbitstrmIn, 24);
                startcodeinfo = BS_getBits(pWMVDec->m_pbitstrmIn, 24);
                startcodeinfo = BS_getBits(pWMVDec->m_pbitstrmIn, 24);
                startcodeinfo = BS_getBits(pWMVDec->m_pbitstrmIn, 16);
            }
            else 
                return WMV_Failed; // invalid startcode
        }
    }
    return WMV_Succeeded;
}
//#endif



#if !defined(WMV_OPT_COMMON_ARM)
#   ifdef _EMB_WMV3_
#       pragma code_seg (EMBSEC_PML)
#   endif

    I32_WMV medianof3 (I32_WMV a0, I32_WMV a1, I32_WMV a2)
    {
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT(medianof3);
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
#endif

//#ifdef _EMB_WMV3_
//#   pragma code_seg (EMBSEC_DEF)
//#endif

Void_WMV find16x16MVpred (tWMVDecInternalMember *pWMVDec, CVector* pvecPredHalfPel, const CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry) 
{
    CVector vctCandMVHalfPel0, vctCandMVHalfPel1, vctCandMVHalfPel2;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(find16x16MVpred);

    if (bLeftBndry)
        vctCandMVHalfPel0.x = vctCandMVHalfPel0.y = 0;
    else
        vctCandMVHalfPel0 = *(pmv - 1);

    if (bTopBndry) {
        *pvecPredHalfPel = vctCandMVHalfPel0;
        return;
    }
    else {
        vctCandMVHalfPel1 = *(pmv - pWMVDec->m_iOffsetToTopMB);
        if (bRightBndry)
            vctCandMVHalfPel2.x = vctCandMVHalfPel2.y = 0;
        else
            vctCandMVHalfPel2 = *(pmv - pWMVDec->m_iOffsetToTopMB + 1);
    }
    pvecPredHalfPel->x = (I8_WMV)medianof3 (vctCandMVHalfPel0.x, vctCandMVHalfPel1.x, vctCandMVHalfPel2.x);
    pvecPredHalfPel->y = (I8_WMV)medianof3 (vctCandMVHalfPel0.y, vctCandMVHalfPel1.y, vctCandMVHalfPel2.y);

    if (pmbmd->m_iMVPredDirection != 2) {
        *pvecPredHalfPel = (pmbmd->m_iMVPredDirection == 0)? vctCandMVHalfPel0 : vctCandMVHalfPel1;
    }
}


tWMVDecodeStatus decodeMVMSV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry, Huffman_WMV* pHufMVDec, U8_WMV* uXMvFromIndex, U8_WMV* uYMvFromIndex)
{
    CVector vctPred;
    I32_WMV lSymbolx, lSymboly, lJoinSymbol, iVLC;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMVMSV);
    
    find16x16MVpred (pWMVDec, &vctPred, pmv, pmbmd, bLeftBndry, bRightBndry, bTopBndry);
    lJoinSymbol = Huffman_WMV_get (pHufMVDec, pWMVDec->m_pbitstrmIn);
    if (BS_invalid ( pWMVDec->m_pbitstrmIn )) {
        return WMV_CorruptedBits;
    }
    
    if (lJoinSymbol != MV_ESCAPE){
        lSymbolx = uXMvFromIndex[lJoinSymbol];
        lSymboly = uYMvFromIndex[lJoinSymbol];
    }
    else {
        lSymbolx = BS_getBits (pWMVDec->m_pbitstrmIn, MVFLCSIZE);
        lSymboly = BS_getBits (pWMVDec->m_pbitstrmIn, MVFLCSIZE);
    }
    
    iVLC = lSymbolx - 32 + vctPred.x;
    if (iVLC > pWMVDec->m_iMVRightBound)
        pmv->x = iVLC - 64;
    else if (iVLC < pWMVDec->m_iMVLeftBound)
        pmv->x = iVLC + 64;
    else
        pmv->x = (I8_WMV) iVLC;
    
    iVLC = lSymboly - 32 + vctPred.y;
    if (iVLC > pWMVDec->m_iMVRightBound)
        pmv->y = iVLC - 64;
    else if (iVLC < pWMVDec->m_iMVLeftBound)
        pmv->y = iVLC + 64;
    else
        pmv->y = (I8_WMV) iVLC;
    
    pWMVDec->m_iMixedPelMV = 0;
    if (pWMVDec->m_iMvResolution == 1) {
        if ( ((pmv->x) & 1) != 0 || ((pmv->y) & 1) != 0) {
            pWMVDec->m_iMixedPelMV = BS_getBits (pWMVDec->m_pbitstrmIn, 1);
        } 
    }
    
    return WMV_Succeeded;
}


Bool_WMV decideHybridMVOn (tWMVDecInternalMember *pWMVDec, CoordI x, Bool_WMV bNot1stRowInSlice, const CVector* pmv)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decideHybridMVOn);

    if (pWMVDec->m_bFrmHybridMVOn && x != 0 && bNot1stRowInSlice && !pWMVDec->m_iMvResolution) {
        CVector vctMvLeft = *(pmv - 1);
        CVector vctMvUp = *(pmv - pWMVDec->m_iOffsetToTopMB);
        I32_WMV iMaxDeltaMV = max(abs(vctMvLeft.x - vctMvUp.x), abs(vctMvLeft.y - vctMvUp.y));;
        return (iMaxDeltaMV >= VAR_FOR_HYBRID_MV);
    } else
        return FALSE_WMV;
}

I32_WMV prvWMVModulus  (I32_WMV a, I32_WMV b)
{ 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(prvWMVModulus);

    do {
        a -= b;
    } while (a >= 0);
    a += b;
    return (a);
}

I32_WMV prvWMVDivision (I32_WMV a, I32_WMV b)
{
    I32_WMV result = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(prvWMVDivision);

    do {
        a -= b;
        result++;
    } while (a >= 0);
    return (result - 1);
}