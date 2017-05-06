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

Bool_WMV getCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn) 
{ 
	return pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1];
}

Void_WMV  setCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) 
{
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1] =(U8_WMV) bisCoded;


}

Void_WMV  setCodedBlockPattern2 (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) 
{
	//pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) blkn - 1] =(U8_WMV) bisCoded;
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) Y_BLOCK1 - 1] =(U8_WMV) (bisCoded & 0x1);
    bisCoded >>= 1;
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) Y_BLOCK2 - 1] =(U8_WMV) (bisCoded & 0x1);
    bisCoded >>= 1;
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) Y_BLOCK3 - 1] =(U8_WMV) (bisCoded & 0x1);
    bisCoded >>= 1;
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) Y_BLOCK4 - 1] =(U8_WMV) (bisCoded & 0x1);
    bisCoded >>= 1;
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) U_BLOCK - 1] =(U8_WMV) (bisCoded & 0x1);
    bisCoded >>= 1;
    pmbmd->m_rgbCodedBlockPattern2[(U32_WMV) V_BLOCK - 1] =(U8_WMV) (bisCoded & 0x1);
}

Void_WMV decodePredictIntraCBP (tWMVDecInternalMember *pWMVDec,
                                            CWMVMBMode* pmbmd, 
                                            CWMVMBMode* pmbmd_lastrow,
                                            I32_WMV imbX, 
                                            I32_WMV imbY)
{
    U8_WMV iPredCBPY1, iPredCBPY2, iPredCBPY3, iPredCBPY4; 
    U8_WMV *pCBPTop, *pCBPTopLeft, *pCBPLeft, *pCBP;

    pCBPTop = pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern2;
    pCBPTopLeft = pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern2;
    pCBPLeft = pWMVDec->m_pmbmdZeroCBPCY->m_rgbCodedBlockPattern2;

   if (imbY != 0 && !pWMVDec->m_pbStartOfSliceRow[imbY]) 
        pCBPTop = pmbmd_lastrow->m_rgbCodedBlockPattern2;
    
    if (imbX != 0) 
        pCBPLeft = (pmbmd - 1)->m_rgbCodedBlockPattern2;
    
    if ((imbY != 0 && !pWMVDec->m_pbStartOfSliceRow[imbY]) && imbX != 0) 
        pCBPTopLeft = (pmbmd_lastrow - 1)->m_rgbCodedBlockPattern2;
    
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
    if(pWMVDec->m_bDQuantOn == 0)
         return WMV_Failed;

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

    
    return WMV_Succeeded;
}

I32_WMV decodeDQuantStepsize(tWMVDecInternalMember *pWMVDec, I32_WMV iShift)
{
    I32_WMV iQP = BS_getBits (pWMVDec->m_pbitstrmIn, 3);

    if (iQP == 7)
        iQP = BS_getBits (pWMVDec->m_pbitstrmIn, 5);
    else
        iQP += pWMVDec->m_iStepSize + iShift;

    if (iShift == 0) // MB level Qp needs to be remapped
        return 2 * iQP - 1;
    else
    return iQP;
}



//static I32_WMV s_iMBXformTypeLUT[16] = {
//    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
//    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
//    XFORMMODE_8x8, XFORMMODE_8x4, XFORMMODE_8x4, XFORMMODE_8x4,
//    XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x8, XFORMMODE_4x4,
//};
//static I32_WMV s_iSubblkPatternLUT[16] = {0, 1, 2, 3, 1, 2, 3, 0, 0, 1, 2, 3, 1, 2, 3, 0};

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
        iHalfpelMV = pWMVDec->m_iHalfPelMV;//m_pAltTables->m_iHalfPelMV;
        pDiffMV->iX = BS_getBits(pbitstrmIn, pWMVDec->m_iLogXRange - iHalfpelMV);
        pDiffMV->iY = BS_getBits(pbitstrmIn, pWMVDec->m_iLogYRange - iHalfpelMV);
        break;
    default:
        iTemp = pWMVDec->m_iHalfPelMV;//m_pAltTables->m_iHalfPelMV;
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

tWMVDecodeStatus decodeMBOverheadOfIVOP_MSV (tWMVDecInternalMember *pWMVDec,
                                                                        CWMVMBMode* pmbmd, 
                                                                        CWMVMBMode* pmbmd_lastrow, 
                                                                        I32_WMV imbX,
                                                                        I32_WMV imbY)
{
    I32_WMV iBlk, iCBPCY;

    iCBPCY = Huffman_WMV_get (&pWMVDec->m_hufICBPCYDec, pWMVDec->m_pbitstrmIn);  //zou  --->vc1DECBIT_GetVLC(pBitstream, vc1DEC_I_Picture_CBPCY_VLC);
    
    if (iCBPCY < 0 || iCBPCY > 63 || BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    for (iBlk = 0; iBlk < 6; iBlk++) 
        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = ((iCBPCY >> (5 - iBlk)) & 1);
    
    decodePredictIntraCBP (pWMVDec, pmbmd, pmbmd_lastrow,imbX, imbY);

    if (pWMVDec->m_cvCodecVersion != WMVA || (pWMVDec->m_cvCodecVersion == WMVA && 
        pWMVDec->m_iACPredIFrameCodingMode == SKIP_RAW)) {
    	pmbmd->m_rgbDCTCoefPredPattern2 [ALLBLOCK] = (U8_WMV)BS_getBits( pWMVDec->m_pbitstrmIn, 1);  //zou --> vc1DECBITPL_ReadBitplaneBit(&pState->sPicParams.sBPPredictAC, pBitstream);
    }

	if (BS_invalid(pWMVDec->m_pbitstrmIn)) 
		return WMV_CorruptedBits;

	pmbmd->m_bSkip = FALSE;
	pmbmd->m_dctMd = INTRA;
    pmbmd->m_iDCTTable_MB_Index = 0;

    iCBPCY = 0;
    for (iBlk = 0; iBlk < 6; iBlk++) {
        iCBPCY |= (pmbmd->m_rgbCodedBlockPattern2 [iBlk] << (5 - iBlk)); 
    }

    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBits( pWMVDec->m_pbitstrmIn, 1);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBits( pWMVDec->m_pbitstrmIn, 1); 
#ifdef STABILITY
        if(iDCTMBTableIndex <0 || iDCTMBTableIndex >2)
            return WMV_CorruptedBits;
#endif
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
#define MVFLCSIZE 6
#define MV_ESCAPE 1099

I32_WMV ReadSliceHeaderWMVA (tWMVDecInternalMember *pWMVDec, I32_WMV imbY)
{
    I32_WMV imbYSlice;
    Bool_WMV bPicHeader;
    I32_WMV result = ICERR_OK;
    I32_WMV iFrameType;

    imbYSlice = BS_getBits(pWMVDec->m_pbitstrmIn, 9);
    if (imbYSlice != imbY) {
		return ICERR_ERROR;
	}

    bPicHeader = BS_getBits(pWMVDec->m_pbitstrmIn, 1);
    if (!bPicHeader) {
        return ICERR_OK;
    }
    if (bPicHeader) {
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
        if (iFrameType == PROGRESSIVE)
        {
            pWMVDec->m_bInterlaceV2 = FALSE;
           result = decodeVOPHeadProgressiveWMVA (pWMVDec);
           if(pWMVDec->m_tFrmType == SKIPFRAME)
               return ICERR_ERROR;
        } 
        else if (iFrameType == INTERLACEFRAME)
        {//Frame Interlace
            pWMVDec->m_bInterlaceV2 = TRUE;
            result = decodeVOPHeadInterlaceV2 (pWMVDec);            
            if(pWMVDec->m_tFrmType == SKIPFRAME)
               return ICERR_ERROR;
        } 
        else 
        {
            return ICERR_ERROR;
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
    
    if(imbY != 0) {
        bNot1stRowAfterStartCode = BS_getBit(pWMVDec->m_pbitstrmIn);
        if(!bNot1stRowAfterStartCode) {                   
            I32_WMV BitsLeft = (BS_BitsLeft(pWMVDec->m_pbitstrmIn) & 7);
            //I32_WMV Bits = BS_getBits(pWMVDec->m_pbitstrmIn, BitsLeft);
            BS_getBits(pWMVDec->m_pbitstrmIn, BitsLeft);
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

//#pragma code_seg (EMBSEC_PML)
I32_WMV medianof3_C (I32_WMV a0, I32_WMV a1, I32_WMV a2)
{
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

//#   pragma code_seg (EMBSEC_DEF)

I32_WMV prvWMVModulus  (I32_WMV a, I32_WMV b)
{ 

    do {
        a -= b;
    } while (a >= 0);
    a += b;
    return (a);
}

I32_WMV prvWMVDivision (I32_WMV a, I32_WMV b)
{
    I32_WMV result = 0;

    do {
        a -= b;
        result++;
    } while (a >= 0);
    return (result - 1);
}