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


#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119                           // see table.13/H.263

tWMVDecodeStatus decodeIInterlace411 (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus result;
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    U32_WMV imbY, imbX, iBlk;
    //Bool_WMV bFrameInPostBuf = FALSE;

    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set [pWMVDec->m_rgiDCTACIntraTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[1] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set [pWMVDec->m_rgiDCTACIntraTableIndx[1]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[2] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set [pWMVDec->m_rgiDCTACIntraTableIndx[2]];
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_ppInterDCTTableInfo_Dec[1] = pWMVDec->m_ppInterDCTTableInfo_Dec [2] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];

    HUFFMANGET_DBG_HEADER(":decodeI411",11);

    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {
        U8_WMV* ppxliCodedY = ppxliCurrQY;
        U8_WMV* ppxliCodedU = ppxliCurrQU;
        U8_WMV* ppxliCodedV = ppxliCurrQV;
        
        if(pWMVDec->m_bStartCode && pWMVDec->m_tFrmType != BVOP)
        {
            if(SliceStartCode(pWMVDec, imbY)!= WMV_Succeeded)
                                return WMV_Failed;
        }
     

        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            //set up intra flags
            for (iBlk = 0; iBlk < 6; iBlk++) {
                pmbmd->m_rgcIntraFlag [iBlk] = 1;
            }
            for (iBlk = 0; iBlk < 4; iBlk++) {
                pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
            }
            pmbmd->m_rgcBlockXformMode [4] = XFORMMODE_4x8;
            pmbmd->m_rgcBlockXformMode [5] = XFORMMODE_4x8;

            ClearFieldMBInterlace411 (pWMVDec->m_ppxliFieldMB, 2); // INTERLACE

            // decoding MV Mode
            if (pWMVDec->m_iMVSwitchCodingMode == SKIP_RAW) {
                pmbmd->m_chMBMode = (BS_getBit(pWMVDec->m_pbitstrmIn) ? MB_FIELD : MB_1MV);
            }

            if (pmbmd->m_chMBMode == MB_1MV) {                
                result = decodeFrameIMBOverheadInterlace411 (pWMVDec, pmbmd, imbX, imbY);
                if (WMV_Succeeded != result) {
                    return result;
                }
                result = DecodeFrameIMBInterlace411 (pWMVDec,
                    imbY, imbX, pmbmd, ppxliCodedY, ppxliCodedU, ppxliCodedV); 
                if (WMV_Succeeded != result) {
                    return result;
                }
            } else {
                result = decodeFieldIMBOverheadInterlace411 (pWMVDec, pmbmd, imbX, imbY);
                if (WMV_Succeeded != result) {
                    return result;
                }
                result = DecodeTopFieldIMBInterlace411 (pWMVDec, 
                    imbY, imbX, pmbmd, ppxliCodedY, ppxliCodedU, ppxliCodedV);
                if (WMV_Succeeded != result) {
                    return result;
                }
                result = DecodeBotFieldIMBInterlace411 (pWMVDec, 
                    imbY, imbX, pmbmd, ppxliCodedY, ppxliCodedU, ppxliCodedV);
                if (WMV_Succeeded != result) {
                    return result;
                }
            }
            ppxliCodedY += 16;
            ppxliCodedU += 4;
            ppxliCodedV += 4;
            pmbmd++;
        }
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY; 
        ppxliCurrQU += 16*pWMVDec->m_iWidthPrevUV ; 
        ppxliCurrQV += 16*pWMVDec->m_iWidthPrevUV ; 
    }

    if (pWMVDec->m_bLoopFilter) {
       LoopFilterIFrameInterlace411 (pWMVDec,
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp);
    }

   // if (!pWMVDec->m_bLoopFilter)
  //      memset (pWMVDec->m_rgchSkipPrevFrame, 0, (size_t) pWMVDec->m_uintNumMB);

//  pWMVDec->m_bColorSettingChanged = FALSE;
    //pWMVDec->m_bCopySkipMBToPostBuf = FALSE; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame
    //pWMVDec->m_iRefreshDisplay_AllMB_Cnt = 0;
    
    return WMV_Succeeded;
}

I32_WMV ComputePredCBPCY (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV iCBPCY) 
{
    CWMVMBMode* pmbmdTop = pmbmd - pWMVDec->m_uintNumMBX;
    CWMVMBMode* pmbmdLeftTop = pmbmdTop - 1;
    CWMVMBMode* pmbmdLeft = pmbmd - 1;
    I32_WMV iPredCBPY1, iPredCBPY2, iPredCBPY3, iPredCBPY4; 

    if (imbY == 0) {
        pmbmdTop = pWMVDec->m_pmbmdZeroCBPCY;
        pmbmdLeftTop = pWMVDec->m_pmbmdZeroCBPCY;
    }
    if (imbX == 0) {
        pmbmdLeft = pWMVDec->m_pmbmdZeroCBPCY;
        pmbmdLeftTop = pWMVDec->m_pmbmdZeroCBPCY;
    }

    iPredCBPY1 = (getCodedBlockPattern (pmbmdLeftTop , Y_BLOCK4) == 
                  getCodedBlockPattern (pmbmdTop , Y_BLOCK3))?
                  getCodedBlockPattern (pmbmdLeft , Y_BLOCK2):
                  getCodedBlockPattern (pmbmdTop , Y_BLOCK3);
    iPredCBPY1 ^= ((iCBPCY >> 5) & 0x00000001);
    iPredCBPY2 = (getCodedBlockPattern (pmbmdTop , Y_BLOCK3) == 
                  getCodedBlockPattern (pmbmdTop , Y_BLOCK4))?
                  iPredCBPY1 :
                  getCodedBlockPattern (pmbmdTop , Y_BLOCK4);
    iPredCBPY2 ^= ((iCBPCY >> 4) & 0x00000001);
    iPredCBPY3 = (getCodedBlockPattern (pmbmdLeft , Y_BLOCK2) == 
                  iPredCBPY1)?
                  getCodedBlockPattern (pmbmdLeft , Y_BLOCK4):
                  iPredCBPY1;
    iPredCBPY3 ^= ((iCBPCY >> 3) & 0x00000001);
    iPredCBPY4 = (iPredCBPY1 == iPredCBPY2)? iPredCBPY3 : iPredCBPY2;
    iPredCBPY4 ^= ((iCBPCY >> 2) & 0x00000001);

    setCodedBlockPattern (pmbmd, Y_BLOCK1, iPredCBPY1);
    setCodedBlockPattern (pmbmd, Y_BLOCK2, iPredCBPY2);
    setCodedBlockPattern (pmbmd, Y_BLOCK3, iPredCBPY3);
    setCodedBlockPattern (pmbmd, Y_BLOCK4, iPredCBPY4);
    setCodedBlockPattern (pmbmd, U_BLOCK, (iCBPCY >> 1) & 0x00000001);
    setCodedBlockPattern (pmbmd, V_BLOCK, iCBPCY & 0x00000001);

     iCBPCY = (((iPredCBPY1 | iPredCBPY2) | iPredCBPY3) | iPredCBPY4) | (iCBPCY & 0x00000003);

    return iCBPCY;
}

tWMVDecodeStatus decodeFrameIMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPCY; 
    U32_WMV iBlk;

    iCBPCY = Huffman_WMV_get(&pWMVDec->m_hufICBPCYDec, pWMVDec->m_pbitstrmIn);
    if (iCBPCY < 0 || iCBPCY > 63 || BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    iCBPCY = ComputePredCBPCY (pWMVDec, pmbmd, imbX, imbY, iCBPCY);

    pmbmd->m_bSkip = FALSE;
    pmbmd->m_dctMd = INTRA;

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT

    //pmbmd->m_iQP = pWMVDec->m_iStepSize; // starting QP - maybe changed by DQUANT
    
    // decode subblock 4x8 cbp
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
            } else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
            } else {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
            }
        }
    }

    
    pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBit(pWMVDec->m_pbitstrmIn);   

    if (pWMVDec->m_bDQuantOn) {
        if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
            return WMV_Failed;
        }
    }

    pmbmd->m_iDCTTable_MB_Index = 0;
    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn); 
#ifdef STABILITY
        if(iDCTMBTableIndex<0 || iDCTMBTableIndex > 2)
            return WMV_Failed;
#endif
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    
    return WMV_Succeeded;
}

tWMVDecodeStatus decodeFieldIMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPCY; 
    U32_WMV iBlk;

    iCBPCY = Huffman_WMV_get(&pWMVDec->m_hufICBPCYDec, pWMVDec->m_pbitstrmIn);
    if (iCBPCY < 0 || iCBPCY > 63 || BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }

    iCBPCY = ComputePredCBPCY (pWMVDec, pmbmd, imbX, imbY, iCBPCY);

    pmbmd->m_bSkip = FALSE;
    pmbmd->m_dctMd = INTRA;

        pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT

    //pmbmd->m_iQP = pWMVDec->m_iStepSize; // starting QP - maybe changed by DQUANT

    // decode subblock 4x8 cbp
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
            } else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
            } else {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
            }
        }
    }

    pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBit(pWMVDec->m_pbitstrmIn) << 1; // Left
    pmbmd->m_rgbDCTCoefPredPattern2 [0] |= (U8_WMV)BS_getBit(pWMVDec->m_pbitstrmIn); // Right

    if (pWMVDec->m_bDQuantOn) {
        if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
            return WMV_Failed;
        }
    }

    pmbmd->m_iDCTTable_MB_Index = 0;
    if (pWMVDec->m_bDCTTable_MB && iCBPCY){
        I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn); 
#ifdef STABILITY
        if(iDCTMBTableIndex<0 || iDCTMBTableIndex > 2)
            return WMV_Failed;
#endif
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }

    return WMV_Succeeded;
}

I32_WMV DecodeIntraBlockYInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode *pmbmd, 
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, DQuantDecParam *pDQ,
    Bool_WMV bAcPredOn)
{
    I32_WMV result;
    U8_WMV* piZigzagInv = (U8_WMV*) pWMVDec->m_pZigzagScanOrder;//pWMVDec->m_pZigzagInv /*pWMVDec->m_pZigzagInv_I*/;
    I32_WMV iDCStepSize = pDQ->iDCStepSize; // used for Y and UV DC decode
    I16_WMV* rgiCoefRecon = (I16_WMV*) pWMVDec->m_rgiCoefReconBuf;
    I16_WMV *ppxliDst;

    ALIGNED32_FASTMEMCLR (rgiCoefRecon, 0, 64 * sizeof(I16_WMV));

    // Decode DCT Coefs            
    result = decodeIntraDCInterlace411 (pWMVDec, &rgiCoefRecon [0], 
        pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, iDCStepSize);    

    if (WMV_Succeeded != result) {
        return result;
    }

    if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
        result = DecodeInverseIntraBlockQuantizeInterlace411 (pWMVDec, 
            ppIntraDCTTableInfo_Dec, piZigzagInv, 64);
        if (WMV_Succeeded != result) {
            return result;
        }
    }

    // AC/DC Prediction    
    decodeDCACPredYInterlace411 (pWMVDec, imbY, imbX, iBlk, pmbmd, bAcPredOn, rgiCoefRecon);
    StoreDCACPredCoefYInterlace411 (pWMVDec, imbY, imbX, iBlk, rgiCoefRecon); 

    // DeQuant
    inverseIntraBlockQuantizeInterlace411 (pWMVDec, rgiCoefRecon, 64, pDQ);

    // Idct
    ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64; 

    (*pWMVDec->m_pInterIDCT_Dec)((UnionBuffer*)ppxliDst, (UnionBuffer*) rgiCoefRecon, 8, 255); 
   
    (*pWMVDec->m_pFieldBlockAdd128) (ppxliDst, 64);
    return WMV_Succeeded;
}

I32_WMV DecodeIntraBlockUVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I32_WMV iSubblock, CWMVMBMode *pmbmd, 
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ,
    Bool_WMV bAcPredOn)
{
    I32_WMV result;
    I32_WMV iDCStepSize = pDQ->iDCStepSize; // used for Y and UV DC decode
    U8_WMV *piZigzagInv = (U8_WMV*) pWMVDec->m_p4x8ZigzagScanOrder;  
    I32_WMV iSubblockCBP = pmbmd->m_rgbCodedBlockPattern2 [iBlk];
    I16_WMV* rgiCoefRecon = (I16_WMV*) pWMVDec->m_rgiCoefReconBuf;
    I16_WMV *ppxliDst;
    Bool_WMV bSubblock [2];
   
    bSubblock [0] = (iSubblockCBP & 2) >> 1; // Left
    bSubblock [1] = iSubblockCBP & 1; // Right
    
    ALIGNED32_FASTMEMCLR (rgiCoefRecon, 0, 32 * sizeof(I16_WMV));

    // decode 1st 4x8 Block
    result = decodeIntraDCInterlace411 (pWMVDec, &rgiCoefRecon [0], 
        pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, iDCStepSize);
    if (WMV_Succeeded != result) {
        return result;
    }

    if (bSubblock[iSubblock]) {
        result = DecodeInverseIntraBlockQuantizeInterlace411 (pWMVDec, 
            ppInterDCTTableInfo_Dec, piZigzagInv, 32);
        if (WMV_Succeeded != result) {
            return result;
        }                
    }     

    // DC/AC Prediction    
    decodeDCACPredUVInterlace411 (pWMVDec, imbY, imbX, iBlk, iSubblock, pmbmd, bAcPredOn, rgiCoefRecon);            
    StoreDCACPredCoefUVInterlace411 (pWMVDec, imbY, imbX, iBlk, iSubblock, rgiCoefRecon);

    // Dequant                          
    inverseIntraBlockQuantizeInterlace411 (pWMVDec, rgiCoefRecon, 32, pDQ);          

    ppxliDst = pWMVDec->m_ppxliFieldMB + (iBlk << 6) + (iSubblock << 5); 

    (*pWMVDec->m_pInter4x8IDCT_Dec) ((UnionBuffer*) ppxliDst, 4, (UnionBuffer*) rgiCoefRecon, 0); 

    (*pWMVDec->m_pFieldBlockAdd128) (ppxliDst, 32);
    return WMV_Succeeded;
}


tWMVDecodeStatus DecodeFrameIMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV)
{
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    I32_WMV result;
    //I32_WMV *rgiCoefRecon = pWMVDec->m_rgiCoefRecon; 
    U32_WMV iBlk;

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    } else{
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }
            
    for (iBlk = 0; iBlk < 4; iBlk++) {
        Bool_WMV bAcPredOn = pmbmd->m_rgbDCTCoefPredPattern2 [0];
        result = DecodeIntraBlockYInterlace411 (pWMVDec, imbY, imbX, iBlk, pmbmd,
            ppIntraDCTTableInfo_Dec, pDQ, bAcPredOn);
        if (WMV_Succeeded != result) {
            return WMV_Failed;
        }
    }

    for (iBlk = 4; iBlk < 6; iBlk++) {
        Bool_WMV bAcPredOn = pmbmd->m_rgbDCTCoefPredPattern2 [0];
        I32_WMV iSubblock;
        for (iSubblock = 0; iSubblock < 2; iSubblock++) {
            DecodeIntraBlockUVInterlace411 (pWMVDec, imbY, imbX, iBlk, iSubblock, 
                pmbmd, ppInterDCTTableInfo_Dec, pDQ, bAcPredOn);
            if (WMV_Succeeded != result) {
                return WMV_Failed;
            }
        }
    }

    CopyIntraFieldMBtoFrameMBInterlace411 (pWMVDec->m_ppxliFieldMB, ppxliTextureQMBY, ppxliTextureQMBU, 
        ppxliTextureQMBV, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV, (pmbmd->m_chMBMode == MB_FIELD));

    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeTopFieldIMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV)
{
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    I32_WMV result;
    //I32_WMV *rgiCoefRecon = pWMVDec->m_rgiCoefRecon;          
    U32_WMV iBlk;

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    for (iBlk = 0; iBlk < 2; iBlk++) { 
        Bool_WMV bAcPredOn = pmbmd->m_rgbDCTCoefPredPattern2 [0] & 2;
        result = DecodeIntraBlockYInterlace411 (pWMVDec, imbY, imbX, iBlk, pmbmd,
            ppIntraDCTTableInfo_Dec, pDQ, bAcPredOn);
        if (WMV_Succeeded != result) {
            return WMV_Failed;
        }
    }

    for (iBlk = 4; iBlk < 6; iBlk++) {
        Bool_WMV bAcPredOn = pmbmd->m_rgbDCTCoefPredPattern2 [0] & 2;
        result = DecodeIntraBlockUVInterlace411 (pWMVDec, 
            imbY, imbX, iBlk, 0, pmbmd, ppInterDCTTableInfo_Dec, pDQ, bAcPredOn);
        if (WMV_Succeeded != result) {
            return WMV_Failed;
        }
    }

    CopyIntraFieldMBtoTopFieldInterlace411 (
        pWMVDec->m_ppxliFieldMB, ppxliTextureQMBY, ppxliTextureQMBU, 
        ppxliTextureQMBV, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);

    return WMV_Succeeded;
}


tWMVDecodeStatus DecodeBotFieldIMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV)
{
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];
    
    I32_WMV result;
    //I32_WMV *rgiCoefRecon = pWMVDec->m_rgiCoefRecon;          
    U32_WMV iBlk;

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
    }

    for (iBlk = 2; iBlk < 4; iBlk++) {   
        Bool_WMV bAcPredOn = pmbmd->m_rgbDCTCoefPredPattern2 [0] & 1;
        result = DecodeIntraBlockYInterlace411 (pWMVDec, imbY, imbX, iBlk, pmbmd,
            ppIntraDCTTableInfo_Dec, pDQ, bAcPredOn);
        if (WMV_Succeeded != result) {
            return WMV_Failed;
        }
    }

    // UV
    for (iBlk = 4; iBlk < 6; iBlk++) {
        Bool_WMV bAcPredOn = pmbmd->m_rgbDCTCoefPredPattern2 [0] & 1;
        result = DecodeIntraBlockUVInterlace411 (pWMVDec,
            imbY, imbX, iBlk, 1, pmbmd, ppInterDCTTableInfo_Dec, pDQ, bAcPredOn);
        if (WMV_Succeeded != result) {
            return WMV_Failed;
        }
    }

    CopyIntraFieldMBtoBotFieldInterlace411 (
        pWMVDec->m_ppxliFieldMB, ppxliTextureQMBY, ppxliTextureQMBU, 
        ppxliTextureQMBV, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);

    return WMV_Succeeded;
}


Void_WMV inverseIntraBlockQuantizeInterlace411 (
    tWMVDecInternalMember *pWMVDec,
    I16_WMV *piCoefRecon, I32_WMV iNumCoef, DQuantDecParam *pDQ)
{    
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    I32_WMV iDCStepSize = pDQ->iDCStepSize;
    I32_WMV i ;

    piCoefRecon[0] = (piCoefRecon[0] * iDCStepSize);
    for ( i = 1; i < iNumCoef; i++) {
        I32_WMV iLevel = piCoefRecon[i];
        if (iLevel)  {
            if (iLevel > 0)
                piCoefRecon[i] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
            else // iLevel < 0 (note: iLevel != 0)
                piCoefRecon[i] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;
        }
    }

    SignPatch(piCoefRecon, iNumCoef*2);

}

Void_WMV ScaleTopPredYForDQuantInterlace411 (
    tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    I32_WMV i;
    if (iBlk == 0 || iBlk == 1) {
        I32_WMV iTmp;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        pPredScaled [0] = 
            (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
        for (i = 1; i < 8; i++) {
            iTmp = pPred [i] * (pmbmd - pWMVDec->m_uintNumMBX)->m_iQP;
            pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
        }
    } else {
        for (i = 0; i < 8; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}

Void_WMV ScaleLeftPredYForDQuantInterlace411 (
    tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    I32_WMV i;
    if (iBlk == 0 || iBlk == 2) {
        I32_WMV iTmp;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        pPredScaled [0] = 
            (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
        for (i = 1; i < 8; i++) {
            iTmp = pPred [i] * (pmbmd - 1)->m_iQP;
            pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
        }
    } else {
        for (i = 0; i < 8; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}

Void_WMV ScaleDCPredYForDQuantInterlace411 (
    tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, CWMVMBMode* pmbmd, I32_WMV *piTopDC, I32_WMV *piLeftDC, I32_WMV *piTopLeftDC)
{

    I32_WMV iTopDC = *piTopDC; 
    I32_WMV iLeftDC = *piLeftDC;
    I32_WMV iTopLeftDC =*piTopLeftDC;
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
    } 

    *piTopDC =  iTopDC ; 
    *piLeftDC =  iLeftDC ;
    *piTopLeftDC =  iTopLeftDC;
}

Void_WMV ScaleTopPredUVForDQuantInterlace411 (
    tWMVDecInternalMember *pWMVDec,
    I32_WMV iSubblock, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{
    I32_WMV i;
    if (iSubblock == 0) {
        I32_WMV iTmp;
        iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - pWMVDec->m_uintNumMBX)->m_iQP].iDCStepSize;
        pPredScaled [0] = 
            (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
        for (i = 1; i < 4; i++) {
            iTmp = pPred [i] * (pmbmd - pWMVDec->m_uintNumMBX)->m_iQP;
            pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
        }
    } else {
        for (i = 0; i < 4; i++) {
            pPredScaled [i] = pPred [i];
        }
    }
}

Void_WMV ScaleLeftPredUVForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iSubblock, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd)
{    
    I32_WMV iTmp, i;
    iTmp = pPred [0] * pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
    pPredScaled [0] = 
        (I16_WMV)divroundnearest (iTmp, pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize);
    for (i = 1; i < 8; i++) {
        iTmp = pPred [i] * (pmbmd - 1)->m_iQP;
        pPredScaled [i] = (I16_WMV)divroundnearest (iTmp, pmbmd->m_iQP);
    }
}

Void_WMV ScaleDCPredUVForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iSubblock, CWMVMBMode* pmbmd, I32_WMV *piTopDC, I32_WMV *piLeftDC, I32_WMV *piTopLeftDC)
{
   I32_WMV iTopDC = *piTopDC; 
    I32_WMV iLeftDC = *piLeftDC;
    I32_WMV iTopLeftDC =*piTopLeftDC;
    if (iSubblock == 0) {
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
    } else { // iSubblock == 1
        I32_WMV iTmp;
        I32_WMV iTopLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        I32_WMV iLeftDCStepSize = pWMVDec->m_prgDQuantParam[(pmbmd - 1)->m_iQP].iDCStepSize;
        I32_WMV iCurrDCStepSize = pWMVDec->m_prgDQuantParam[pmbmd->m_iQP].iDCStepSize;
        
        iTmp = iTopLeftDC * iTopLeftDCStepSize;
        iTopLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
        iTmp = iLeftDC * iLeftDCStepSize;
        iLeftDC = divroundnearest (iTmp, iCurrDCStepSize);
    } 
    *piTopDC =  iTopDC ; 
    *piLeftDC =  iLeftDC ;
    *piTopLeftDC =  iTopLeftDC;
}

Void_WMV decodeDCACPredUVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I32_WMV iSubblock, CWMVMBMode* pmbmd, 
    Bool_WMV bAcPredOn, I16_WMV* piLevelBlk)
{
    // Store DCT-DCAC coefficients and subtract out prediction
    Bool_WMV bIsTopBlkBoundary = ((imbY == 0) && (iSubblock == 0));
    Bool_WMV bIsLeftBlkBoundary = (imbX == 0);
    Bool_WMV bIsTopBlockI, bIsLeftBlockI, bIsTopLeftBlockI;
    I16_WMV *pDCT, *pIntra, *pPredIntraL, *pPredIntraT;
    I32_WMV iWidth, iIndex;
    I32_WMV i;
    I16_WMV rgiPredCoef [8];

    // set bIsTopBlockI;
    if (!bIsTopBlkBoundary) {
        if (iSubblock == 0) { 
            bIsTopBlockI = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcIntraFlag [2]; // above MB's Y2
        } else { 
            bIsTopBlockI = pmbmd->m_rgcIntraFlag [0]; // same MB's Y0
        }
    } else {
        bIsTopBlockI = FALSE;
    }

    // set bIsLeftBlockI
    if (!bIsLeftBlkBoundary) {
        if (iSubblock == 0) {
            bIsLeftBlockI = (pmbmd - 1)->m_rgcIntraFlag [0];
        } else {
            bIsLeftBlockI = (pmbmd - 1)->m_rgcIntraFlag [2];
        }
    } else {
        bIsLeftBlockI = FALSE;
    }

    // set bIsTopLeftBlockI
    if (!bIsTopBlkBoundary && !bIsLeftBlkBoundary) {
        if (iSubblock == 0) {
            bIsTopLeftBlockI = (pmbmd - pWMVDec->m_uintNumMBX - 1)->m_rgcIntraFlag [2];
        } else {
            bIsTopLeftBlockI = (pmbmd - 1)->m_rgcIntraFlag [0];
        }
    } else {
        bIsTopLeftBlockI = FALSE;
    }

    pDCT = (iBlk == 4) ? pWMVDec->m_pX9dctU : pWMVDec->m_pX9dctV;
    iWidth = pWMVDec->m_uintNumMBX;
    iIndex = 2 * imbY * iWidth + imbX; // location of 1st 4x8 block
    
    if (iSubblock) { // 2nd 4x8 block
        iIndex += iWidth;
    }
    
    pIntra = pDCT + iIndex * 12; // 4 bytes row, 8 bytes col = 12 bytes       
    pPredIntraL = pIntra - 12 + 4; // point to column
    pPredIntraT = pIntra - iWidth * 12; // point to row      
    
    if (bIsLeftBlockI && bIsTopBlockI) {        
        // pick best direction
        I32_WMV iTopLeftDC = (bIsTopLeftBlockI) ? pPredIntraT[-12] : 0;
        I32_WMV iTopDC = pPredIntraT[0];
        I32_WMV iLeftDC = pPredIntraL[0];
        
        ScaleDCPredUVForDQuantInterlace411 (pWMVDec,
            iSubblock, pmbmd, &iTopDC,&iLeftDC, &iTopLeftDC);        
                
        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {

            ScaleTopPredUVForDQuantInterlace411 (pWMVDec,
                iSubblock, pPredIntraT, rgiPredCoef, pmbmd);

            piLevelBlk[0] += rgiPredCoef[0];
            if (bAcPredOn) {
                if (pWMVDec->m_bRotatedIdct) { 
                    for (i = 1; i < 4; i++) {
                        piLevelBlk [i << 3] += rgiPredCoef [i];
                    }
                } else {
                    for (i = 1; i < 4; i++) {
                        piLevelBlk [i] += rgiPredCoef [i];
                    }
                }
            }
        } else {
            ScaleLeftPredUVForDQuantInterlace411 (pWMVDec,
                iSubblock, pPredIntraL, rgiPredCoef, pmbmd);

            piLevelBlk[0] += rgiPredCoef [0];
            if (bAcPredOn) {
                if (pWMVDec->m_bRotatedIdct) { 
                    for (i = 1; i < 8; i++) {
                        piLevelBlk [i] += rgiPredCoef [i];
                    }
                } else {
                    for (i = 1; i < 8; i++) {
                        piLevelBlk [i << 2] += rgiPredCoef [i];
                    }
                }
            }            
        }                        
    } else if (bIsLeftBlockI) {   

        ScaleLeftPredUVForDQuantInterlace411 (pWMVDec,
            iSubblock, pPredIntraL, rgiPredCoef, pmbmd);

        piLevelBlk[0] += rgiPredCoef [0];
        if (bAcPredOn) {
            if (pWMVDec->m_bRotatedIdct) { 
                for (i = 1; i < 8; i++) {
                    piLevelBlk [i] += rgiPredCoef [i];
                }
            } else {
                for (i = 1; i < 8; i++) {
                    piLevelBlk [i << 2] += rgiPredCoef [i];
                }
            }
        }  
    } else if (bIsTopBlockI) {
        ScaleTopPredUVForDQuantInterlace411 (pWMVDec,
            iSubblock, pPredIntraT, rgiPredCoef, pmbmd);

        piLevelBlk[0] += rgiPredCoef[0];
        if (bAcPredOn) {
            if (pWMVDec->m_bRotatedIdct) { 
                for (i = 1; i < 4; i++) {
                    piLevelBlk [i << 3] += rgiPredCoef [i];
                }
            } else {
                for (i = 1; i < 4; i++) {
                    piLevelBlk [i] += rgiPredCoef [i];
                }
            }
        }
    }
}

Bool_WMV decodeDCACPredYInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode* pmbmd, Bool_WMV bAcPredOn, I16_WMV* piLevelBlk)
{
    I32_WMV iX = imbX * 2 + (iBlk & 1);
    I32_WMV iY = imbY * 2 + ((iBlk & 2) >> 1);    
    I16_WMV *pIntra = pWMVDec->m_pX9dct + (iX + iY * pWMVDec->m_uintNumMBX * 2) * 16;    

    Bool_WMV bIsTopBlkBoundary = ((imbY == 0) && (iBlk == 0 || iBlk == 1));
    Bool_WMV bIsLeftBlkBoundary = ((imbX == 0) && (iBlk == 0 || iBlk == 2));
    Bool_WMV bIsTopBlockI, bIsLeftBlockI, bIsTopLeftBlockI;

    I32_WMV iWidth, iIndex;
    I16_WMV *pPredIntraL, *pPredIntraT;
    I16_WMV rgiPredCoef [8]; // for scaling
    I32_WMV i;

    // set bIsTopBlockI;
    if (!bIsTopBlkBoundary) {
        if (iBlk < 2) { // iBlk = 0,1
            // 0's Top is above MB's 2 & 1's Top is above MB's 3 
            bIsTopBlockI = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcIntraFlag [iBlk + 2];
        } else { // iBlk = 2,3
            // 2's Top is same MB's 0 & 3's Top is same MB's 1
            bIsTopBlockI = pmbmd->m_rgcIntraFlag [iBlk - 2];
        }
    } else {
        bIsTopBlockI = FALSE;
    }

    // set bIsLeftBlockI
    if (!bIsLeftBlkBoundary) {
        if (iBlk == 0 || iBlk == 2) {
            bIsLeftBlockI = (pmbmd - 1)->m_rgcIntraFlag [iBlk + 1];
        } else {
            bIsLeftBlockI = pmbmd->m_rgcIntraFlag [iBlk - 1];
        }
    } else {
        bIsLeftBlockI = FALSE;
    }

    // set bIsTopLeftBlockI
    if (!bIsTopBlkBoundary && !bIsLeftBlkBoundary) {
        if (iBlk == 0) {
            bIsTopLeftBlockI = (pmbmd - pWMVDec->m_uintNumMBX - 1)->m_rgcIntraFlag [3];
        } else if (iBlk == 1) {
            bIsTopLeftBlockI = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcIntraFlag [2];
        } else if (iBlk == 2) {
            bIsTopLeftBlockI = (pmbmd - 1)->m_rgcIntraFlag [1];
        } else { // iBlk == 3
            bIsTopLeftBlockI = pmbmd->m_rgcIntraFlag [0];
        }
    } else {
        bIsTopLeftBlockI = FALSE;
    }

    iWidth = 2 * pWMVDec->m_uintNumMBX;
    iIndex = (imbY * 2 + (iBlk >> 1)) * iWidth + (imbX * 2 + (iBlk & 1));       
    
    pPredIntraL = pIntra - 16 + 8;
    pPredIntraT = pIntra - iWidth * 16; 

    if (bIsLeftBlockI && bIsTopBlockI) {              
        // pick best direction
        I32_WMV iTopLeftDC = (bIsTopLeftBlockI) ? pPredIntraT[-16 + 8] : 0;
        I32_WMV iTopDC = pPredIntraT[0];
        I32_WMV iLeftDC = pPredIntraL[0];
    
        ScaleDCPredYForDQuantInterlace411 (pWMVDec,
            iBlk, pmbmd, &iTopDC, &iLeftDC, &iTopLeftDC);

        if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {  

            ScaleTopPredYForDQuantInterlace411 (pWMVDec,
                iBlk, pPredIntraT, rgiPredCoef, pmbmd);

            piLevelBlk [0] += rgiPredCoef [0];
            if (bAcPredOn) { // row
                if (pWMVDec->m_bRotatedIdct) {
                    for (i = 1; i < 8; i++) {
                        piLevelBlk [i << 3] += rgiPredCoef [i];
                    }
                } else {
                    for (i = 1; i < 8; i++) {
                        piLevelBlk [i] += rgiPredCoef [i];
                    }
                }
            }             
        } else {

            ScaleLeftPredYForDQuantInterlace411 (pWMVDec,
                iBlk, pPredIntraL, rgiPredCoef, pmbmd);

            piLevelBlk [0] += rgiPredCoef [0];
            if (bAcPredOn) { // col
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
    } else if (bIsLeftBlockI) {
        ScaleLeftPredYForDQuantInterlace411 (pWMVDec,
            iBlk, pPredIntraL, rgiPredCoef, pmbmd);

        piLevelBlk [0] += rgiPredCoef [0];
        if (bAcPredOn) { // col
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
    } else if (bIsTopBlockI) {

        ScaleTopPredYForDQuantInterlace411 (pWMVDec,
            iBlk, pPredIntraT, rgiPredCoef, pmbmd);
    
        piLevelBlk [0] += rgiPredCoef [0];
        if (bAcPredOn) { // row
            if (pWMVDec->m_bRotatedIdct) {
                for (i = 1; i < 8; i++) {
                    piLevelBlk [i << 3] += rgiPredCoef [i];
                }
            } else {
                for (i = 1; i < 8; i++) {
                    piLevelBlk [i] += rgiPredCoef [i];
                }
            }
        }
    }
    return TRUE;
}

Void_WMV StoreDCACPredCoefYInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I16_WMV *piLevelBlk)
{
    I32_WMV iWidth = 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iIndex = (imbY * 2 + (iBlk >> 1)) * iWidth + (imbX * 2 + (iBlk & 1));
    I32_WMV i;
    I16_WMV *pIntra = pWMVDec->m_pX9dct + iIndex * 16;
   
    if (pWMVDec->m_bRotatedIdct) {
        for (i = 0; i < 8; i++) {
            pIntra[i] = piLevelBlk [i << 3];
            pIntra[i + 8] = piLevelBlk [i];
        }    
    } else {
        for (i = 0; i < 8; i++) {
            pIntra[i] = piLevelBlk [i];
            pIntra[i + 8] = piLevelBlk [i << 3];
        }    
    }
}

Void_WMV StoreDCACPredCoefUVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I32_WMV iSubblock, I16_WMV *piLevelBlk)                                                   
{
    I16_WMV *pDCT = (iBlk == 4) ? pWMVDec->m_pX9dctU : pWMVDec->m_pX9dctV;
    I32_WMV iWidth = pWMVDec->m_uintNumMBX;
    I32_WMV iIndex = 2 * imbY * iWidth + imbX; // location of 1st 4x8 block
    I32_WMV i;
    I16_WMV *pIntra;

    if (iSubblock) { // 2nd 4x8 block
        iIndex += iWidth;
    }
    
    pIntra = pDCT + iIndex * 12; // 4 bytes row, 8 bytes col = 12 bytes

    if (pWMVDec->m_bRotatedIdct) {
        for (i = 0; i < 4; i++) { // store row
            pIntra [i] = piLevelBlk [i << 3];
        }
        for (i = 0; i < 8; i++) { // store col
            pIntra [i + 4] = piLevelBlk [i];
        }
    } else {
        for (i = 0; i < 4; i++) { // store row
            pIntra [i] = piLevelBlk [i];
        }
        for (i = 0; i < 8; i++) { // store col
            pIntra [i + 4] = piLevelBlk [i << 2];
        }
    }
}

tWMVDecodeStatus decodeIntraDCInterlace411 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *piDC, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCStepSize)
{
    I32_WMV iDC;

    iDC = Huffman_WMV_get(hufDCTDCDec,pWMVDec->m_pbitstrmIn);

    if (iDC != iTCOEF_ESCAPE) {
        if (iDC != 0) {
            if (iDCStepSize == 4) {
                iDC = (iDC<<1) + (U16_WMV) (BS_getBit(pWMVDec->m_pbitstrmIn)) -1;
            } else if (iDCStepSize == 2){
                iDC = (iDC<<2) + (U16_WMV) (BS_getBits(pWMVDec->m_pbitstrmIn,2)) -3;
            }                 
            iDC = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? -iDC : iDC;
        } else
            iDC = 0;
    } else {
        U32_WMV offset = 0;
        if (iDCStepSize <= 4) 
            offset = 3 - (iDCStepSize >> 1);
        iDC = BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_LEVEL + offset);
        iDC = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? -iDC : iDC;
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }

    *piDC = (I16_WMV) iDC;
    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeInterlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, U8_WMV* piZigzagInv, U32_WMV uiNumCoef)
{
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
        
    Bool_WMV bIsLastRun = FALSE;
    U32_WMV uiRun; // = 0;
    I32_WMV iLevel; // = 0;
    U32_WMV uiCoefCounter = 1;
    U8_WMV   lIndex;    
    
    I16_WMV *rgiCoefRecon = (I16_WMV*) pWMVDec->m_rgiCoefReconBuf;

     

    do {
        lIndex = (U8_WMV) ( Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn) );
#ifdef STABILITY
        if(lIndex > iTCOEF_ESCAPE)
            return WMV_Failed;
#endif

        if (lIndex != iTCOEF_ESCAPE)    {
            bIsLastRun = (lIndex >= iStartIndxOfLastRun);
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
            rgLevelAtIndx[lIndex];

        }
        else {
            if (BS_getBit(pWMVDec->m_pbitstrmIn)){

                // ESC + '1' + VLC
                lIndex = (U8_WMV) (Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn) );
                if (lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return WMV_Failed;
                }
                uiRun = rgRunAtIndx [lIndex];
                iLevel = rgLevelAtIndx[lIndex];
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    iLevel += (I8_WMV) rgIfLastNumOfLevelAtRunIntra[uiRun];
                }
                else
                    iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRunIntra[uiRun];
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;

            }
            else if (BS_getBit(pWMVDec->m_pbitstrmIn)){

                // ESC + '01' + VLC
                lIndex = (U8_WMV) ( Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn) );
                if (lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return WMV_Failed;
                }
                uiRun = rgRunAtIndx [lIndex];
                iLevel = rgLevelAtIndx[lIndex];
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    uiRun += (rgIfLastNumOfRunAtLevelIntra[iLevel] + 1);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevelIntra[iLevel] + 1);
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;

            }
            else{
                // ESC + '00' + FLC
                bIsLastRun = (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){
                    // WMV_ESC_Decoding();
                    if (pWMVDec->m_bFirstEscCodeInFrame){
                        decodeBitsOfESCCode (pWMVDec);
                        pWMVDec->m_bFirstEscCodeInFrame = FALSE;
                    }                                                                               
                    uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_RUN);
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) /* escape decoding */
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
        
        if (BS_invalid(pWMVDec->m_pbitstrmIn) || uiCoefCounter >= uiNumCoef) {
            
            return WMV_CorruptedBits;
        }
        
        rgiCoefRecon[piZigzagInv[uiCoefCounter]] = (I16_WMV) iLevel;
        
        uiCoefCounter++;
    } while (!bIsLastRun);
    
     
    return WMV_Succeeded;
}

//
// Inter Codec 
//

tWMVDecodeStatus decodePInterlace411 (tWMVDecInternalMember *pWMVDec)
{
    tWMVDecodeStatus result;
    U8_WMV *ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV *ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefY = pWMVDec->m_ppxliRef0YPlusExp;
    U8_WMV *ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV *ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    U32_WMV imbY, imbX;

    //Bool_WMV bFrameInPostBuf = FALSE;

    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
       
    pWMVDec->m_iFilterType = FILTER_BICUBIC;
    
    HUFFMANGET_DBG_HEADER(":decodeP411",12);
    

    // SET_NEW_FRAME
    pWMVDec->m_iHalfPelMV = FALSE;
//#ifdef STABILITY
//    if(pWMVDec->m_pAltTables->m_iNContexts>11)
//        return -1;
//#endif
     //t_AltTablesSetNewFrame( pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);
    //pWMVDec->m_pAltTables->setNewFrame(pWMVDec->m_rgiCoefRecon, pWMVDec->m_iStepSize, 1);
   
    for (imbY = 0; imbY < pWMVDec->m_uintNumMBY; imbY++) {                
        U8_WMV *ppxliCurrQYMB = ppxliCurrQY;
        U8_WMV *ppxliCurrQUMB = ppxliCurrQU;
        U8_WMV *ppxliCurrQVMB = ppxliCurrQV;
        U8_WMV *ppxliRefYMB = ppxliRefY;
        U8_WMV *ppxliRefUMB = ppxliRefU;
        U8_WMV *ppxliRefVMB = ppxliRefV;
        U32_WMV iBlk;
                
        if(pWMVDec->m_bStartCode)
        {
            if(SliceStartCode(pWMVDec, imbY)!= WMV_Succeeded)
                return WMV_Failed;
        }
       
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {  
            I32_WMV iIntra;

            // these are always true
            pmbmd->m_rgcBlockXformMode [4] = XFORMMODE_4x8;
            pmbmd->m_rgcBlockXformMode [5] = XFORMMODE_4x8;

            ClearFieldMBInterlace411 (pWMVDec->m_ppxliFieldMB, 2); 

            // decoding MV Mode
            if (pWMVDec->m_iMVSwitchCodingMode == SKIP_RAW) {
                pmbmd->m_chMBMode = (BS_getBit(pWMVDec->m_pbitstrmIn) ? MB_FIELD : MB_1MV);
            }

            if (pmbmd->m_chMBMode == MB_1MV) {
                result = decodeFramePMBOverheadInterlace411 (pWMVDec,
                    pmbmd, imbX, imbY);
                if (WMV_Succeeded != result) {
                    return result;
                }
               
                iIntra = ComputeFrameMVFromDiffMVInterlace411 (pWMVDec,
                    imbX, imbY, pWMVDec->m_pDiffMV); 

                if (iIntra) {

                    for (iBlk = 0; iBlk < 4; iBlk++) {
                        pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    }
                    
                    result = DecodeFrameIMBInterlace411 (pWMVDec,
                        imbY, imbX, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else {                   
                    result = DecodeFramePMBInterlace411 (pWMVDec,
                        imbX, imbY, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB); 
                }

                if (WMV_Succeeded != result) {
                    return result;
                }
            } else {

                result = decodeFieldPMBOverheadInterlace411 (pWMVDec,
                    pmbmd, imbX, imbY);
                if (WMV_Succeeded != result) {
                    return result;
                }

                ComputeTopFieldMVFromDiffMVInterlace411 (pWMVDec, imbX, imbY, pWMVDec->m_pDiffMV);
                ComputeBotFieldMVFromDiffMVInterlace411 (pWMVDec, imbX, imbY, pWMVDec->m_pDiffMV + 2);

                if (pWMVDec->m_pDiffMV->iIntra) {

                    for (iBlk = 0; iBlk < 2; iBlk++) {
                        pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    }
                    
                    result = DecodeTopFieldIMBInterlace411 (pWMVDec, imbY, imbX, pmbmd, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else {
                    result = DecodeTopFieldPMBInterlace411 (pWMVDec,
                        imbX, imbY, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB);  
                }
                if (WMV_Succeeded != result) {
                    return result;
                }

                if ((pWMVDec->m_pDiffMV + 2)->iIntra) {

                    for (iBlk = 2; iBlk < 4; iBlk++) {
                        pmbmd->m_rgcBlockXformMode [iBlk] = XFORMMODE_8x8;
                    }

                    result = DecodeBotFieldIMBInterlace411 (pWMVDec, imbY, imbX, pmbmd, 
                        ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB);
                } else {

                    result = DecodeBotFieldPMBInterlace411 (pWMVDec, 
                        imbX, imbY, pmbmd, ppxliCurrQYMB, ppxliCurrQUMB, 
                        ppxliCurrQVMB, ppxliRefYMB, ppxliRefUMB, ppxliRefVMB); 
                }
                if (WMV_Succeeded != result) {
                    return result;
                }
            }
            

            {
            I32_WMV iTopLeftBlkIndex = 2*imbY*2*pWMVDec->m_uintNumMBX + imbX*2;
            I32_WMV iBotLeftBlkIndex = iTopLeftBlkIndex + 2*pWMVDec->m_uintNumMBX;
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pXMotion [iTopLeftBlkIndex];
            pWMVDec->m_pFieldMvX_Pbuf[iTopLeftBlkIndex + 1 ] = pWMVDec->m_pXMotion [iTopLeftBlkIndex + 1 ];
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex] = pWMVDec->m_pXMotion [iBotLeftBlkIndex];
            pWMVDec->m_pFieldMvX_Pbuf[iBotLeftBlkIndex + 1 ] = pWMVDec->m_pXMotion [iBotLeftBlkIndex + 1 ];
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex] = pWMVDec->m_pYMotion [iTopLeftBlkIndex];
            pWMVDec->m_pFieldMvY_Pbuf[iTopLeftBlkIndex + 1 ] = pWMVDec->m_pYMotion [iTopLeftBlkIndex + 1 ];
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex] = pWMVDec->m_pYMotion [iBotLeftBlkIndex];
            pWMVDec->m_pFieldMvY_Pbuf[iBotLeftBlkIndex + 1 ] = pWMVDec->m_pYMotion [iBotLeftBlkIndex + 1 ];
            }

            pmbmd->m_bSkip = FALSE;
            pmbmd++;
            ppxliCurrQYMB += 16;
            ppxliCurrQUMB += 4;
            ppxliCurrQVMB += 4;
            ppxliRefYMB += 16;
            ppxliRefUMB += 4;
            ppxliRefVMB += 4;
        }

        // point to the starting location of the first MB of each row
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliCurrQU += 16*pWMVDec->m_iWidthPrevUV;
        ppxliCurrQV += 16*pWMVDec->m_iWidthPrevUV;
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRefU += 16*pWMVDec->m_iWidthPrevUV;
        ppxliRefV += 16*pWMVDec->m_iWidthPrevUV;
       
    }

    if (pWMVDec->m_bLoopFilter) {
                LoopFilterPFrameDecInterlace411 (pWMVDec, 
            pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
            pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
            pWMVDec->m_rgmbmd);
        
    }

    //pWMVDec->m_bRenderFromPostBuffer = FALSE_WMV;

    return WMV_Succeeded;
}



tWMVDecodeStatus decodeFramePMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPCY = 0;
    U32_WMV iBlk;
    Bool_WMV bSendTableswitchMode = FALSE;
    Bool_WMV bSendXformswitchMode = FALSE;

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT

    // pmbmd->m_iQP = pWMVDec->m_iStepSize;
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;


    // init vars before skip
    pmbmd->m_rgbDCTCoefPredPattern2 [0] = 0;
    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgcIntraFlag [iBlk] = 0;
    }

    if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn) ;
    }
    
    if (pmbmd->m_bSkip) {
        memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));
        memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV));  

        return WMV_Succeeded;
    }

    // decode MV
    decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, pWMVDec->m_pDiffMV); 
    pWMVDec->m_pDiffMV->iHybrid = 0; 
    

    // decode intra info
    if (pWMVDec->m_pDiffMV->iIntra) { // Intra

        // set intra flags
        for (iBlk = 0; iBlk < 6; iBlk++) {
            pmbmd->m_rgcIntraFlag [iBlk] = 1;
        }

        // decode CBP
        if (pWMVDec->m_pDiffMV->iLast == 0) {
            pmbmd->m_bCBPAllZero = TRUE;
        } else {
            pmbmd->m_bCBPAllZero = FALSE;
        }

        if (pmbmd->m_bCBPAllZero) {
            // set cbp to 0
            for (iBlk = 0; iBlk < 6; iBlk ++) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 0;
            }        
        } else {
            // read CBP
            iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufNewPCBPCYDec,pWMVDec->m_pbitstrmIn);
            pmbmd->m_bCBPAllZero = FALSE;


            // set CBP
            for (iBlk = 0; iBlk < 6; iBlk ++) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = (iCBPCY & 0x1);
                iCBPCY >>= 1;
            }

            // decode subblock 4x8 cbp
            for (iBlk = 4; iBlk < 6; iBlk++) {
                if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
                    if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0) {
                        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
                    } else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
                    } else {
                        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
                    }
                }        
            }        
        }

        // get AC Pred Flag
        pmbmd->m_rgbDCTCoefPredPattern2 [0] = (U8_WMV)BS_getBit(pWMVDec->m_pbitstrmIn);

        if (pWMVDec->m_bDQuantOn) {
            if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                return WMV_Failed;
            }        
        }

        if (pmbmd->m_bCBPAllZero == FALSE) {
            bSendTableswitchMode = pWMVDec->m_bDCTTable_MB;

            if (bSendTableswitchMode) {
                I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn);
                if (iDCTMBTableIndex)
                    iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn); 
#ifdef STABILITY
                if(iDCTMBTableIndex < 0 || iDCTMBTableIndex > 2)
                    return WMV_Failed;
#endif
                pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
            }
        }
       
        return WMV_Succeeded;
    }
    
    // decode Inter info
    if (pWMVDec->m_pDiffMV->iLast == 0) {
        pmbmd->m_bCBPAllZero = TRUE;
        // set cbp to 0
        for (iBlk = 0; iBlk < 6; iBlk ++) {
            pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 0;
        }

    } else {

        bSendTableswitchMode = pWMVDec->m_bDCTTable_MB;
        bSendXformswitchMode = pWMVDec->m_bMBXformSwitching && !pWMVDec->m_pDiffMV->iIntra;

        // read CBP
        iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufNewPCBPCYDec,pWMVDec->m_pbitstrmIn);
        pmbmd->m_bCBPAllZero = FALSE;

        // set CBP
        for (iBlk = 0; iBlk < 6; iBlk ++) {
            pmbmd->m_rgbCodedBlockPattern2 [iBlk] = (iCBPCY & 0x1);
            iCBPCY >>= 1;
        }

        // decode subblock 4x8 cbp
        for (iBlk = 4; iBlk < 6; iBlk++) {
            if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0) {
                    pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
                } else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                    pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
                } else {
                    pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
                }
            }        
        }

        if (pWMVDec->m_bDQuantOn) {
            if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
                return WMV_Failed;
            }
        }

        // pmbmd->m_bCBPAllZero is always true here but just leave it here for now
        if (bSendTableswitchMode && !pmbmd->m_bCBPAllZero) {
            I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn);
            if (iDCTMBTableIndex)
                iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn); 
#ifdef STABILITY
            if(iDCTMBTableIndex < 0 || iDCTMBTableIndex > 2)
                return WMV_Failed;
#endif
            pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
        }

        if (bSendXformswitchMode) {
            if ((pmbmd->m_iMBXformMode = BS_getBits(pWMVDec->m_pbitstrmIn,2)) == 3) {
                pmbmd->m_bBlkXformSwitchOn = TRUE;
            }
        }

    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }

    return WMV_Succeeded;
}

tWMVDecodeStatus decodeFieldPMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY)
{
    I32_WMV iCBPCY = 0;
    U32_WMV iBlk;
    I32_WMV iCBPCY_V8, iTmpCBP;
    Bool_WMV bSendXformswitchMode = FALSE;

    pmbmd->m_iQP = (2 * pWMVDec->m_iStepSize - 1) + pWMVDec->m_bHalfStep; // starting QP - maybe changed by DQUANT

    //pmbmd->m_iQP = pWMVDec->m_iStepSize;
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE;
    
    // init vars before skip
    pmbmd->m_rgbDCTCoefPredPattern2 [0] = 0;
    
    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgcIntraFlag [iBlk] = 0;
    }


    if (pWMVDec->m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pWMVDec->m_pbitstrmIn);
    }

    if (pmbmd->m_bSkip) {
        memset (pmbmd->m_rgbCodedBlockPattern2, 0, 6 * sizeof (U8_WMV));        
        memset (pWMVDec->m_pDiffMV, 0, sizeof(CDiffMV) * 4); 
        return WMV_Succeeded;
    }

    // read CBP
    iCBPCY = Huffman_WMV_get(pWMVDec->m_pHufNewPCBPCYDec,pWMVDec->m_pbitstrmIn);
    pmbmd->m_bCBPAllZero = FALSE;

    // decode MV
    iCBPCY_V8 = iCBPCY & 0x3A;
    memset (pWMVDec->m_pDiffMV, 0, sizeof (CDiffMV) * 4);
    

    if (iCBPCY & 1) { // Top
        decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV + 0));  
    } 

    if (iCBPCY & 4) { // Bot 
        decodeMV_V9(pWMVDec, pWMVDec->m_pHufMVTable_V9, LH_MV0, (pWMVDec->m_pDiffMV + 2)); 

    }



    // Update and set CBP

    iCBPCY_V8 |= ((pWMVDec->m_pDiffMV + 0)->iLast << 0);
    iCBPCY_V8 |= ((pWMVDec->m_pDiffMV + 2)->iLast << 2);

    iTmpCBP = iCBPCY_V8;

    for (iBlk = 0; iBlk < 6; iBlk++) {
        pmbmd->m_rgbCodedBlockPattern2 [iBlk] = iTmpCBP & 0x1;
        iTmpCBP >>= 1;
    }

    // decode subblock 4x8 cbp
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (pmbmd->m_rgbCodedBlockPattern2 [iBlk]) {
            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 3; // Right
            } else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 2; // Left
            } else {
                pmbmd->m_rgbCodedBlockPattern2 [iBlk] = 1; // Both
            }
        }        
    }

    // get AC Pred Flag
    if (pWMVDec->m_pDiffMV->iIntra) {
        // set intra flags - UV not used in indication of intra
        for (iBlk = 0; iBlk < 2; iBlk++) {
            pmbmd->m_rgcIntraFlag [iBlk] = 1;
        }
        pmbmd->m_rgbDCTCoefPredPattern2 [0] |= BS_getBit(pWMVDec->m_pbitstrmIn) << 1; // Left
    }
    if ((pWMVDec->m_pDiffMV + 2)->iIntra) {
        // set intra flags
        for (iBlk = 2; iBlk < 4; iBlk++) {
            pmbmd->m_rgcIntraFlag [iBlk] = 1;
        }
        pmbmd->m_rgbDCTCoefPredPattern2 [0] |= BS_getBit(pWMVDec->m_pbitstrmIn); // Right
    }
    
    // Dquant
    if (pWMVDec->m_bDQuantOn && (pWMVDec->m_pDiffMV->iIntra || 
        (pWMVDec->m_pDiffMV + 2)->iIntra || (iCBPCY_V8 != 0))) {
        if (WMV_Succeeded != decodeDQuantParam (pWMVDec, pmbmd)) {
            return WMV_Failed;
        }
    }
    
    // Dct Tableswitch
    if (pWMVDec->m_bDCTTable_MB && iCBPCY_V8) {
        I32_WMV iDCTMBTableIndex = BS_getBit(pWMVDec->m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pWMVDec->m_pbitstrmIn); 
#ifdef STABILITY
        if(iDCTMBTableIndex<0 || iDCTMBTableIndex >2)
            return -1;
#endif
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }

    // XFormSwitchingMode    
    bSendXformswitchMode |= (pmbmd->m_rgbCodedBlockPattern2 [0] || 
                             pmbmd->m_rgbCodedBlockPattern2 [1]) && 
                             !(pWMVDec->m_pDiffMV->iIntra);

    bSendXformswitchMode |= (pmbmd->m_rgbCodedBlockPattern2 [2] || 
                             pmbmd->m_rgbCodedBlockPattern2 [3]) && 
                             !((pWMVDec->m_pDiffMV + 2)->iIntra);
    bSendXformswitchMode &= pWMVDec->m_bMBXformSwitching;
    if (bSendXformswitchMode) {
        if ((pmbmd->m_iMBXformMode = BS_getBits(pWMVDec->m_pbitstrmIn,2)) == 3) {
            pmbmd->m_bBlkXformSwitchOn = TRUE;
        }
    }

    if (BS_invalid(pWMVDec->m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }

    return WMV_Succeeded;
}


tWMVDecodeStatus DecodeInverseInterSubblockQuantizeInterlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, DQuantDecParam *pDQ)
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
        I32_WMV lIndex = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);

        if (lIndex != iTCOEF_ESCAPE)    {
            if (lIndex >= iStartIndxOfLastRun)
                bIsLastRun = TRUE;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
        } 
        else {
            if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                // ESC + '1' + VLC
                I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
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
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
            }
            else if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                // ESC + '10' + VLC
                I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return WMV_CorruptedBits;
                }
                uiRun = rgRunAtIndx[lIndex2];
                iLevel = rgLevelAtIndx[lIndex2];
                if (lIndex2 >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + 1);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + 1);
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
            }
            else{
                // ESC + '00' + FLC
                bIsLastRun = (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){  // == WMV1 || WMV2
                    if (pWMVDec->m_bFirstEscCodeInFrame){                                                    
                        decodeBitsOfESCCode (pWMVDec);                                                     
                        pWMVDec->m_bFirstEscCodeInFrame = FALSE;                                             
                    }                                                                               
                    uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_RUN);                              
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) /* escape decoding */                              
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
        if (uiCoefCounter >= 32 || BS_invalid(pWMVDec->m_pbitstrmIn)) {
            return WMV_CorruptedBits;
        }

        if (iLevel > 0)
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
        else
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;

        uiCoefCounter++;
    } while (!bIsLastRun);
    // Save the DCT row flags. This will be passed to the IDCT routine
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;

    SignPatch(rgiCoefRecon, 64);


    return WMV_Succeeded;
}

I32_WMV DecodeInverseInter8x8BlockQuantizeInterlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, DQuantDecParam *pDQ)
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

    I32_WMV iIndex ;
    Bool_WMV bIsLastRun = FALSE;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    //Align on cache line
    I16_WMV *rgiCoefRecon = (I16_WMV*)pWMVDec->m_rgiCoefRecon;
    I32_WMV iDCTHorzFlags = 0;

    do {
        I32_WMV lIndex = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);

        if (lIndex != iTCOEF_ESCAPE)    {
            if (lIndex >= iStartIndxOfLastRun)
                bIsLastRun = TRUE;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit(pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
        } 
        else {
            if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                // ESC + '1' + VLC
                I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
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
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
            }
            else if (BS_getBit(pWMVDec->m_pbitstrmIn)){
                // ESC + '10' + VLC
                I32_WMV lIndex2 = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = pWMVDec->m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (lIndex2 == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return WMV_CorruptedBits;
                }
                uiRun = rgRunAtIndx[lIndex2];
                iLevel = rgLevelAtIndx[lIndex2];
                if (lIndex2 >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + 1);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + 1);
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
            }
            else{
                // ESC + '00' + FLC
                bIsLastRun = (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){  // == WMV1 || WMV2
                    if (pWMVDec->m_bFirstEscCodeInFrame){                                                    
                        decodeBitsOfESCCode (pWMVDec);                                                     
                        pWMVDec->m_bFirstEscCodeInFrame = FALSE;                                             
                    }                                                                               
                    uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,pWMVDec->m_iNUMBITS_ESC_RUN);                              
                    if (BS_getBit(pWMVDec->m_pbitstrmIn)) /* escape decoding */                              
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
        if (uiCoefCounter >= 64 || BS_invalid(pWMVDec->m_pbitstrmIn)) {
            return WMV_CorruptedBits;
        }

        iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];

        // If this coefficient is not in the first column then set the flag which indicates
        // what row it is in. This flag field will be used by the IDCT to see if it can
        // shortcut the IDCT of the row if all coefficients are zero.
        if (iIndex & 0x7)
            iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

        if (iLevel > 0)
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
        else
            rgiCoefRecon [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;

        uiCoefCounter++;
    } while (!bIsLastRun);
    // Save the DCT row flags. This will be passed to the IDCT routine
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;

    SignPatch(rgiCoefRecon, 128);


    return WMV_Succeeded;
}


I32_WMV Decode8x8Interlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam* pDQ, I16_WMV *ppxliDst)
{    
    I32_WMV result;
    ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));

    result = DecodeInverseInter8x8BlockQuantizeInterlace411 (pWMVDec,
        ppInterDCTTableInfo_Dec, pWMVDec->m_pZigzagScanOrder, pDQ);


    (*pWMVDec->m_pInterIDCT_Dec) ((UnionBuffer *) ppxliDst, pWMVDec->m_rgiCoefReconBuf, 8, pWMVDec->m_iDCTHorzFlags);

    return result;
}

tWMVDecodeStatus Decode4x8Interlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam* pDQ, 
    I16_WMV *ppxliDst, U32_WMV iDstOffset)
{    
    tWMVDecodeStatus result;
    ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));

    result = DecodeInverseInterSubblockQuantizeInterlace411 (pWMVDec,
        ppInterDCTTableInfo_Dec, pWMVDec->m_p4x8ZigzagScanOrder, pDQ); 

    (*pWMVDec->m_pInter4x8IDCT_Dec) ((UnionBuffer *) ppxliDst, iDstOffset, pWMVDec->m_rgiCoefReconBuf, 0);
    return result;
}

I32_WMV Decode8x4Interlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam* pDQ, I16_WMV *ppxliDst)
{
    I32_WMV result; 
    ALIGNED32_FASTMEMCLR (pWMVDec->m_rgiCoefRecon, 0, 64*sizeof(I16_WMV));
    result = DecodeInverseInterSubblockQuantizeInterlace411  (pWMVDec,
        ppInterDCTTableInfo_Dec, pWMVDec->m_p8x4ZigzagScanOrder, pDQ);


    (*pWMVDec->m_pInter8x4IDCT_Dec) ((UnionBuffer *) ppxliDst, 8, pWMVDec->m_rgiCoefReconBuf, 0);
    return result;
}

tWMVDecodeStatus DecodeInterBlockYInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk, CWMVMBMode *pmbmd,
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ)
{
    I32_WMV result;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    Bool_WMV bResidual = rgCBP [iBlk];

    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;

    if (!bResidual) {
        pmbmd->m_rgcBlockXformMode[iBlk] = XFORMMODE_8x8;
    }

    if (bResidual) {
        if (pmbmd->m_bBlkXformSwitchOn) {
            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x8;
            else if (BS_getBit(pWMVDec->m_pbitstrmIn) == 0)
                iXformType = XFORMMODE_8x4;
            else
                iXformType = XFORMMODE_4x8;
        }
        pmbmd->m_rgcBlockXformMode[iBlk] = (I8_WMV)iXformType;

        if (iXformType == XFORMMODE_8x8) {
            I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
            result = Decode8x8Interlace411 (pWMVDec, ppInterDCTTableInfo_Dec, pDQ, ppxliDst);

        } else if (iXformType == XFORMMODE_8x4) {
            Bool_WMV bTop = TRUE;
            Bool_WMV bBottom = TRUE;                
            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                    bBottom = FALSE;
                } else {
                    bTop = FALSE;
                }
            }
            if (bTop) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                result = Decode8x4Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst);     
            }
            if (bBottom) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4 * 8;
                result = Decode8x4Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst);     
            }            
        } else if (iXformType == XFORMMODE_4x8) {
            Bool_WMV bLeft = TRUE;
            Bool_WMV bRight = TRUE;              
            if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                if (BS_getBit(pWMVDec->m_pbitstrmIn) == 1) {
                    bRight = FALSE;
                } else {
                    bLeft = FALSE;
                }
            }
            if (bLeft) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                result = Decode4x8Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 8);                         
            }
            if (bRight) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4;
                result = Decode4x8Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 8);                       
            } 
        }
    }
    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeTopFieldPMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB)
{
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result;
    U32_WMV iBlk;

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set [pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
    }
    
    for (iBlk = 0; iBlk < 2; iBlk++) {
        result = DecodeInterBlockYInterlace411 (pWMVDec,
            imbX, imbY, iBlk, pmbmd, ppInterDCTTableInfo_Dec, pDQ);
        if (WMV_Succeeded != result) {
            return result;
        }
    }

    // U0 V0
    for (iBlk = 4; iBlk < 6; iBlk++) {        
        if (rgCBP [iBlk]) {
            I32_WMV iSubblockCBP = pmbmd->m_rgbCodedBlockPattern2 [iBlk];
            Bool_WMV bTop = (iSubblockCBP & 2);            
            if (bTop) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                result = Decode4x8Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);   
            }
        }
    }

    MotionCompTopFieldInterlace411 (pWMVDec,
        imbX, imbY, ppxlcRefQYMB, ppxlcRefQUMB, ppxlcRefQVMB, 
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB);

    AddErrorTopFieldInterlace411 (pWMVDec,
        pWMVDec->m_ppxliFieldMB, ppxlcCurrQYMB, ppxlcCurrQUMB, 
        ppxlcCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);

    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeBotFieldPMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB)
{
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result;
    U32_WMV iBlk;

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set [pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
    }

    for (iBlk = 2; iBlk < 4; iBlk++) {
        result = DecodeInterBlockYInterlace411 (pWMVDec,
            imbX, imbY, iBlk, pmbmd, ppInterDCTTableInfo_Dec, pDQ);
        if (WMV_Succeeded != result) {
            return result;
        }
    }

    // U1 V1
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (rgCBP [iBlk]) {
            I32_WMV iSubblockCBP = pmbmd->m_rgbCodedBlockPattern2 [iBlk];
            Bool_WMV bBottom = iSubblockCBP & 1; 
            if (bBottom) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4*8;
                result = Decode4x8Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);           
            } 
        }
    }

    MotionCompBotFieldInterlace411 (pWMVDec,
        imbX, imbY, ppxlcRefQYMB, ppxlcRefQUMB, ppxlcRefQVMB, 
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB);

    AddErrorBotFieldInterlace411 (pWMVDec,
        pWMVDec->m_ppxliFieldMB, ppxlcCurrQYMB, ppxlcCurrQUMB, 
        ppxlcCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);


    return WMV_Succeeded;
}

tWMVDecodeStatus DecodeFramePMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB)
{
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec;
    DQuantDecParam *pDQ = &pWMVDec->m_prgDQuantParam [pmbmd->m_iQP];

    U8_WMV *rgCBP = pmbmd->m_rgbCodedBlockPattern2;
    tWMVDecodeStatus result;
    U32_WMV iBlk;

    if (pWMVDec->m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pWMVDec->m_pInterDCTTableInfo_Dec_Set [pmbmd->m_iDCTTable_MB_Index]);
    } else {
        ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
    }
    
    for (iBlk = 0; iBlk < 4; iBlk++) {
        result = DecodeInterBlockYInterlace411 (pWMVDec, 
            imbX, imbY, iBlk, pmbmd, ppInterDCTTableInfo_Dec, pDQ);
        if (WMV_Succeeded != result) {
            return result;
        }
    }
    
    for (iBlk = 4; iBlk < 6; iBlk++) {
        if (rgCBP [iBlk]) {
            I32_WMV iSubblockCBP = pmbmd->m_rgbCodedBlockPattern2 [iBlk];
            Bool_WMV bTop = (iSubblockCBP & 2); 
            Bool_WMV bBottom = iSubblockCBP & 1; 
            if (bTop) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64;
                result = Decode4x8Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);   
            }
            if (bBottom) {
                I16_WMV* ppxliDst = pWMVDec->m_ppxliFieldMB + iBlk * 64 + 4*8;
                result = Decode4x8Interlace411 (pWMVDec,
                    ppInterDCTTableInfo_Dec, pDQ, ppxliDst, 4);           
            } 
        }
    }

    MotionCompFrameInterlace411 (pWMVDec,
        imbX, imbY, ppxlcRefQYMB, ppxlcRefQUMB, ppxlcRefQVMB, 
        ppxlcCurrQYMB, ppxlcCurrQUMB, ppxlcCurrQVMB);
              
    AddErrorFrameInterlace411 (pWMVDec,
        pWMVDec->m_ppxliFieldMB, ppxlcCurrQYMB, ppxlcCurrQUMB, 
        ppxlcCurrQVMB, pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);

    return WMV_Succeeded;
}

I32_WMV ComputeFrameMVFromDiffMVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV iIBlock = 0;
 
    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k + iXBlocks]
                       = pWMVDec->m_pXMotion [k + iXBlocks + 1] = IBLOCKMV;
    } else  {       
        I32_WMV dX = imbX;  
        I32_WMV dY = imbY;
        Bool_WMV bTopBndry = (dY == 0);

        PredictFieldOneMV (pWMVDec, &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);

        pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + iXBlocks]
                           = pWMVDec->m_pXMotion [k + iXBlocks + 1]
                           = ((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
        pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = pWMVDec->m_pYMotion [k + iXBlocks]
                           = pWMVDec->m_pYMotion [k + iXBlocks + 1]
                           = ((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;

    }
    return iIBlock;
}

I32_WMV ComputeTopFieldMVFromDiffMVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + 2 * imbX ;
    I32_WMV iIBlock = 0;

    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + 1] = IBLOCKMV;
    } else  {
        I32_WMV dX = imbX;  
        I32_WMV dY = imbY;
        Bool_WMV bTopBndry = (dY == 0);

        PredictFieldTopMV (pWMVDec, &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);

        pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
            ((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
        pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
            ((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;
              
    }
    return iIBlock;
}

I32_WMV ComputeBotFieldMVFromDiffMVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV)
{
    I32_WMV iXBlocks = pWMVDec->m_uintNumMBX * 2;
    I32_WMV k = 2 * imbY * iXBlocks + iXBlocks + 2 * imbX ;
    I32_WMV iIBlock = 0;

    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        pWMVDec->m_pXMotion [k] = pWMVDec->m_pXMotion [k + 1] = IBLOCKMV;
    } else  {
        I32_WMV dX = imbX;  
        I32_WMV dY = imbY;
        Bool_WMV bTopBndry = (dY == 0);

        PredictFieldBotMV (pWMVDec, &dX, &dY, pWMVDec->m_pXMotion, pWMVDec->m_pYMotion, bTopBndry);

        pWMVDec->m_pXMotion [k + 1] = pWMVDec->m_pXMotion [k] = 
            ((pInDiffMV->iX + dX + pWMVDec->m_iXMVRange) & pWMVDec->m_iXMVFlag) - pWMVDec->m_iXMVRange;
        pWMVDec->m_pYMotion [k + 1] = pWMVDec->m_pYMotion [k] = 
            ((pInDiffMV->iY + dY + pWMVDec->m_iYMVRange) & pWMVDec->m_iYMVFlag) - pWMVDec->m_iYMVRange;

    }
    return iIBlock;
}

Void_WMV LoopFilterPFrameDecInterlace411 (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, 
    CWMVMBMode *pmbmd)
{
    U8_WMV* pCurrY, *pCurrU, *pCurrV;
    CWMVMBMode *pMBMode;
    U32_WMV imbX, imbY;    
    U32_WMV uiMBStart = 0;
    U32_WMV uiMBEnd = pWMVDec->m_uintNumMBY;

    ppxliY = ppxliY + 8 - 5;
    ppxliU = ppxliU + 4 - 5;
    ppxliV = ppxliV + 4 - 5;

    for (imbY = uiMBStart; imbY < uiMBEnd; imbY++) {  
        pCurrY = ppxliY + imbY * pWMVDec->m_iMBSizeXWidthPrevY; 
        pCurrU = ppxliU + imbY * 16*pWMVDec->m_iWidthPrevUV ; 
        pCurrV = ppxliV + imbY * 16*pWMVDec->m_iWidthPrevUV ; 
        pMBMode = pmbmd + imbY * pWMVDec->m_uintNumMBX;
        for (imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++) {
            // Filter Y - Block boundary within MB 
            if (pMBMode->m_chMBMode == MB_1MV) {
                U8_WMV *pbCBP = pMBMode->m_rgbCodedBlockPattern2;
                if (pbCBP [0] || pbCBP [1] || pMBMode->m_rgcIntraFlag [0]) {
                    (*pWMVDec->m_pFilterVerticalEdge)(pCurrY, pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);                
                } 
                if (pbCBP [2] || pbCBP [3] || pMBMode->m_rgcIntraFlag [2]) {
                    (*pWMVDec->m_pFilterVerticalEdge)(pCurrY + 8 * pWMVDec->m_iWidthPrevY, 
                        pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);                
                }                    
            } else {                 
                // top field
                U8_WMV *pbCBP = pMBMode->m_rgbCodedBlockPattern2;
                if (pbCBP [0] || pbCBP [1] || pMBMode->m_rgcIntraFlag [0]) {
                    (*pWMVDec->m_pFilterVerticalEdge)(pCurrY, pWMVDec->m_iWidthPrevY*2, pWMVDec->m_iStepSize, 8);                
                }                 
                // bot field
                if (pbCBP [2] || pbCBP [3] || pMBMode->m_rgcIntraFlag [2]) {
                    (*pWMVDec->m_pFilterVerticalEdge)(pCurrY + pWMVDec->m_iWidthPrevY,
                        pWMVDec->m_iWidthPrevY*2, pWMVDec->m_iStepSize, 8);                
                }                       
            }
            
            pCurrY += 8;
            // Filter Y,U,V - Block boundary across MB 
            if (imbX != pWMVDec->m_uintNumMBX - 1) {
                if (pMBMode->m_chMBMode != (pMBMode+1)->m_chMBMode) {
                    (*pWMVDec->m_pFilterVerticalEdge)(pCurrY, pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 16);
                    (*pWMVDec->m_pFilterVerticalEdge)(pCurrU, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 16);
                    (*pWMVDec->m_pFilterVerticalEdge)(pCurrV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 16);
                } else {
                    U8_WMV *pbCBP0 = (pMBMode)->m_rgbCodedBlockPattern2;
                    U8_WMV *pbCBP1 = (pMBMode + 1)->m_rgbCodedBlockPattern2;
                    I8_WMV *pcIntra0 = (pMBMode)->m_rgcIntraFlag;
                    I8_WMV *pcIntra1 = (pMBMode + 1)->m_rgcIntraFlag;
                    if (pMBMode->m_chMBMode == MB_1MV) { // Frame
                        if (pbCBP0 [1] || pbCBP1 [0] || pcIntra0 [0] || pcIntra1 [0]) {
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrY, pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);  
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrU, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);                
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);    
                        } 
                        if (pbCBP0 [3] || pbCBP1 [2] || pcIntra0 [2] || pcIntra1 [2]) {
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrY + (8 * pWMVDec->m_iWidthPrevY), pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);                
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrU + 8 * pWMVDec->m_iWidthPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);                
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrV + 8 * pWMVDec->m_iWidthPrevUV, pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);                
                        } 
                    } else { // Field                
                        if (pbCBP0 [1] || pbCBP1 [0] || pcIntra0 [0] || pcIntra1 [0]) {
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrY, 2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrU, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);                
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrV, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);   
                        } 
                        if (pbCBP0 [3] || pbCBP1 [2] || pcIntra0 [2] || pcIntra1 [2]) {
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrY + pWMVDec->m_iWidthPrevY, 2*pWMVDec->m_iWidthPrevY, pWMVDec->m_iStepSize, 8);                
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrU + pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);                
                            (*pWMVDec->m_pFilterVerticalEdge)(pCurrV + pWMVDec->m_iWidthPrevUV, 2*pWMVDec->m_iWidthPrevUV, pWMVDec->m_iStepSize, 8);                
                        } 
                    }
                }
            }
            pMBMode ++;
            pCurrY += 8;
            pCurrU += 4;
            pCurrV += 4;
        } 
    }
}

