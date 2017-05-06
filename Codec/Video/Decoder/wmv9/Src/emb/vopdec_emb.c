//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//codecs/Scrunch/wmv2/decoder/vopdec_ce.cpp#12 - edit change 23357 (text)
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "constants_wmv.h"
#include "tables_wmv.h"
#include "strmdec_wmv.h"
#include "codehead.h"
#include "voWmvPort.h"

#ifdef WMV_OPT_SPRITE
#include "postfilter_wmv.h"
#endif

#ifdef _EMB_WMV2_


tWMVDecodeStatus g_CoefDecoderAndIdctDec8x8(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec, I32_WMV iXFormMode);
tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4or4x8(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec, I32_WMV iXFormMode);
tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4Table0x(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4Table10(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4Table11(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
tWMVDecodeStatus g_CoefDecoderAndIdctDec4x8Table0x(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
tWMVDecodeStatus g_CoefDecoderAndIdctDec4x8Table10(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);
tWMVDecodeStatus g_CoefDecoderAndIdctDec4x8Table11(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec);


Void_WMV InitEMB_DecodePShortcut(tWMVDecInternalMember *pWMVDec)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitEMB_DecodePShortcut);
    pWMVDec->m_EMBDecodePShortCut.pVideoDecoderObject=pWMVDec;
    pWMVDec->m_EMBDecodePShortCut.m_rgiCoefRecon=pWMVDec->m_rgiCoefRecon;
    pWMVDec->m_EMBDecodePShortCut.m_pbitstrmIn=pWMVDec->m_pbitstrmIn;
    pWMVDec->m_EMBDecodePShortCut.m_i2DoublePlusStepSize=pWMVDec->m_i2DoublePlusStepSize;
    pWMVDec->m_EMBDecodePShortCut.m_iDoubleStepSize=pWMVDec->m_iDoubleStepSize;
    pWMVDec->m_EMBDecodePShortCut.m_iStepMinusStepIsEven=pWMVDec->m_iStepMinusStepIsEven;

}


Void_WMV InitEMB_FuncTable(tWMVDecInternalMember *pWMVDec)
{

    I32_WMV iMvResolution;
    I32_WMV index;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitEMB_FuncTable);

    // MotionCompMixed functions

    iMvResolution=1;

    index=0x0;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed000;

    index=0x1;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed001;

    index=0x2;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed010;

    index=0x3;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed011;

    index=0x4;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed100;

    index=0x5;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed101;

    index=0x6;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed110;

    index=0x7;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed111;

    // MotionComp functions

    iMvResolution=0;

    index=0x0;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompWAddError00;

    index=0x1;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompWAddError01;

    index=0x2;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompWAddError10;

    index=0x3;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed011;

    index=0x4;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompWAddError00;

    index=0x5;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompWAddError01;

    index=0x6;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompWAddError10;

    index=0x7;
    pWMVDec->m_MotionCompAndAddErrorTable[index|(iMvResolution<<3)]=MotionCompMixed011;
    //m_CoefDecoderAndIdctDec* () init

        pWMVDec->m_CoefDecoderAndIdctDecTable[XFORMMODE_8x8]=g_CoefDecoderAndIdctDec8x8;
        pWMVDec->m_CoefDecoderAndIdctDecTable[XFORMMODE_8x4]=g_CoefDecoderAndIdctDec8x4or4x8;
        pWMVDec->m_CoefDecoderAndIdctDecTable[XFORMMODE_4x8]=g_CoefDecoderAndIdctDec8x4or4x8;


        //xFormMode==8x4
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_8x4&1)<<2)|0x0]=g_CoefDecoderAndIdctDec8x4Table0x;
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_8x4&1)<<2)|0x1]=g_CoefDecoderAndIdctDec8x4Table0x;
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_8x4&1)<<2)|0x2]=g_CoefDecoderAndIdctDec8x4Table10;
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_8x4&1)<<2)|0x3]=g_CoefDecoderAndIdctDec8x4Table11;
        //xFormMode==4x8
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_4x8&1)<<2)|0x0]=g_CoefDecoderAndIdctDec4x8Table0x;
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_4x8&1)<<2)|0x1]=g_CoefDecoderAndIdctDec4x8Table0x;
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_4x8&1)<<2)|0x2]=g_CoefDecoderAndIdctDec4x8Table10;
        pWMVDec->m_pCoefDecoderAndIdctDec8x4or4x8Table[((XFORMMODE_4x8&1)<<2)|0x3]=g_CoefDecoderAndIdctDec4x8Table11;
#       ifdef _XFORMSWITCHING2_
//            m_CoefDecoderAndIdctDecTable[XFORMMODE_4x4]=g_CoefDecoderAndIdctDec4x4;
#       endif

}





tWMVDecodeStatus g_CoefDecoderAndIdctDec8x8(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec, I32_WMV iXFormMode)
{
    FUNCTION_PROFILE(fpIDCT)
    
    tWMVDecodeStatus result=WMV_Succeeded;
    EMB_DecodeP_ShortCut * pShortCut=&pThis->m_EMBDecodePShortCut;
    pShortCut->XFormMode=XFORMMODE_8x8;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec8x8);

    if (pThis->m_cvCodecVersion != MP4S) 
        result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis, pInterDCTTableInfo_Dec, pThis->m_pZigzagScanOrder, pShortCut); //get the quantized block      
    else {
#if !(defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
#ifndef IDCTDEC_32BIT_INPUT
        result=DecodeInverseInterBlockQuantizeEscCode_MPEG4_EMB(pThis, pInterDCTTableInfo_Dec, pThis->m_pZigzagScanOrder, XFORMMODE_8x8);
#else
        result=DecodeInverseInterBlockQuantizeEscCode_MPEG4(pThis, &pInterDCTTableInfo_Dec, pThis->m_pZigzagScanOrder, XFORMMODE_8x8, NULL);
#endif
#endif
        pShortCut->m_iDCTHorzFlags = pThis->m_iDCTHorzFlags;
    }
    
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)

#ifdef IDCTDEC_16BIT_3ARGS
    g_IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, pShortCut->m_iDCTHorzFlags);
#else
    g_IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, BLOCK_SIZE, pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    return result;
}

tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4or4x8(tWMVDecInternalMember* pThis,CDCTTableInfo_Dec* pInterDCTTableInfo_Dec, I32_WMV XFormMode)
{

    tWMVDecodeStatus result=WMV_Succeeded;
    I32_WMV top_bottom=BS_peekBits(pThis->m_pbitstrmIn, 2);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec8x4or4x8);

#ifdef _EMB_WMV3_
    BS_adjustBits(pThis->m_pbitstrmIn, (top_bottom>>1)+1);
#else
    BS_getBits (pThis->m_pbitstrmIn, (top_bottom>>1)+1);
#endif
                
    pThis->m_pCoefDecoderAndIdctDec8x4or4x8Table[top_bottom|((XFormMode&1)<<2)](pThis, pInterDCTTableInfo_Dec);

    return result;

}

// top = false and bottom =true
tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4Table0x(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec)
{
    FUNCTION_PROFILE(fpIDCT)

    tWMVDecodeStatus result=WMV_Succeeded;
    EMB_DecodeP_ShortCut * pShortCut=&pThis->m_EMBDecodePShortCut;

#ifdef _64BIT_
    //I64_WMV * pBuff=(pThis->m_ppxliErrorQ->i32);
    I64_WMV * pBuff=(I64_WMV *)(pThis->m_ppxliErrorQ->i32);

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec8x4Table0x);

    pBuff[0]=0;
    pBuff[2]=0;
    pBuff[4]=0;
    pBuff[6]=0;
    pBuff[16]=0;
    pBuff[18]=0;
    pBuff[20]=0;
    pBuff[22]=0;
    
#else

    I32_WMV * pBuff=(pThis->m_ppxliErrorQ->i32);
    I32_WMV i;      // SH4 does not like this declared inside for loop
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec8x4Table0x);

    for(i=0;i<16;i+=4)
    {
        pBuff[i]=0;
        pBuff[i+1]=0;
        pBuff[i+32]=0;
        pBuff[i+32+1]=0;
    }


#endif
    pShortCut->XFormMode=XFORMMODE_8x4;

    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis,pInterDCTTableInfo_Dec, pThis->m_p8x4ZigzagScanOrder, pShortCut); //get the quantized block      
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)

#ifdef IDCTDEC_16BIT_3ARGS
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#else
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    return result;
}

// top = true and bottom =false
tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4Table11(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec)
{
    FUNCTION_PROFILE(fpIDCT)


    tWMVDecodeStatus result=WMV_Succeeded;
    EMB_DecodeP_ShortCut * pShortCut=&pThis->m_EMBDecodePShortCut;
    
#if defined( _64BIT_)

    //I64_WMV * pBuff=(pThis->m_ppxliErrorQ->i32+16);
    I64_WMV * pBuff=(I64_WMV *)(pThis->m_ppxliErrorQ->i32+16);

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec8x4Table11);

    pBuff[0]=0;
    pBuff[2]=0;
    pBuff[4]=0;
    pBuff[6]=0;
    pBuff[16]=0;
    pBuff[18]=0;
    pBuff[20]=0;
    pBuff[22]=0;


#else

    I32_WMV * pBuff=(pThis->m_ppxliErrorQ->i32+16);
    I32_WMV i;      // SH4 does not like this declared inside for loop
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec8x4Table11);

    for(i=0;i<16;i+=4)
    {
        pBuff[i]=0;
        pBuff[i+1]=0;
        pBuff[i+32]=0;
        pBuff[i+32+1]=0;
    }
#endif

    pShortCut->XFormMode=XFORMMODE_8x4;

    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis,pInterDCTTableInfo_Dec, pThis->m_p8x4ZigzagScanOrder, pShortCut); //get the quantized block      
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)
#ifdef IDCTDEC_16BIT_3ARGS
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, pShortCut-> m_iDCTHorzFlags);
#else
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, pShortCut-> m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    return result;
}

//top=true and bottom=true
tWMVDecodeStatus g_CoefDecoderAndIdctDec8x4Table10(tWMVDecInternalMember* pThis,CDCTTableInfo_Dec* pInterDCTTableInfo_Dec)
{
    FUNCTION_PROFILE(fpIDCT)

    
    tWMVDecodeStatus result=WMV_Succeeded;
    EMB_DecodeP_ShortCut * pShortCut=&pThis->m_EMBDecodePShortCut;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec8x4Table10);

    pShortCut->XFormMode=XFORMMODE_8x4;
    
    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis, pInterDCTTableInfo_Dec, pThis->m_p8x4ZigzagScanOrder, pShortCut); //get the quantized block      
    if(result!=WMV_Succeeded)
        return result;

    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)
#ifdef IDCTDEC_16BIT_3ARGS
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, pShortCut->m_iDCTHorzFlags);
#else
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis, pInterDCTTableInfo_Dec, pThis->m_p8x4ZigzagScanOrder, pShortCut); //get the quantized block      
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)
#ifdef IDCTDEC_16BIT_3ARGS
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#else
    g_8x4IDCTDec_WMV2_16bit (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    return result;
}

// top = false and bottom =true
tWMVDecodeStatus g_CoefDecoderAndIdctDec4x8Table0x(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec)
{
    FUNCTION_PROFILE(fpIDCT)

    I32_WMV * pBuff=(pThis->m_ppxliErrorQ->i32);
    I32_WMV i;      // SH4 does not like this declared inside for loop

    tWMVDecodeStatus result=WMV_Succeeded;

    EMB_DecodeP_ShortCut * pShortCut=&pThis->m_EMBDecodePShortCut;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec4x8Table0x);

    pShortCut->XFormMode=XFORMMODE_4x8;


    for(i=0;i<32;i+=4)
    {
        pBuff[i]=0;
        pBuff[i+32]=0;
    }

    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis, pInterDCTTableInfo_Dec, pThis->m_p4x8ZigzagScanOrder, pShortCut); //get the quantized block      
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)
#ifdef IDCTDEC_16BIT_3ARGS
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#else
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    return result;
}

// top = true and bottom =false
tWMVDecodeStatus g_CoefDecoderAndIdctDec4x8Table11(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec)
{
    FUNCTION_PROFILE(fpIDCT)

    I32_WMV * pBuff=(pThis->m_ppxliErrorQ->i32+1);
    I32_WMV i;      // SH4 does not like this declared inside for loop

    tWMVDecodeStatus result=WMV_Succeeded;

    EMB_DecodeP_ShortCut * pShortCut=&pThis->m_EMBDecodePShortCut;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec4x8Table11);

    pShortCut->XFormMode=XFORMMODE_4x8;

    for(i=0;i<32;i+=4)
    {
        pBuff[i]=0;
        pBuff[i+32]=0;
    }

    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis, pInterDCTTableInfo_Dec, pThis->m_p4x8ZigzagScanOrder, pShortCut); //get the quantized block      
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)
#ifdef IDCTDEC_16BIT_3ARGS
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, pShortCut->m_iDCTHorzFlags);
#else
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    return result;
}

//top=true and bottom=true
tWMVDecodeStatus g_CoefDecoderAndIdctDec4x8Table10(tWMVDecInternalMember* pThis, CDCTTableInfo_Dec* pInterDCTTableInfo_Dec)
{
    FUNCTION_PROFILE(fpIDCT)

    tWMVDecodeStatus result=WMV_Succeeded;

    EMB_DecodeP_ShortCut * pShortCut=&pThis->m_EMBDecodePShortCut;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_CoefDecoderAndIdctDec4x8Table10);

    pShortCut->XFormMode=XFORMMODE_4x8;
    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis, pInterDCTTableInfo_Dec, pThis->m_p4x8ZigzagScanOrder, pShortCut); //get the quantized block 
    if(result!=WMV_Succeeded)
        return result;

    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)
#ifdef IDCTDEC_16BIT_3ARGS
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, pShortCut->m_iDCTHorzFlags);
#else
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    result=DecodeInverseInterBlockQuantizeEscCodeEMB(pThis, pInterDCTTableInfo_Dec, pThis->m_p4x8ZigzagScanOrder, pShortCut); //get the quantized block      
    FUNCTION_PROFILE_START(&fpIDCT,IDCT_PROFILE)
#ifdef IDCTDEC_16BIT_3ARGS
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#else
    g_4x8IDCTDec_WMV2_16bit  (pThis->m_ppxliErrorQ, BLOCK_SIZE, pThis->m_rgiCoefReconBuf, 0x80000000+pShortCut->m_iDCTHorzFlags);
#endif
    FUNCTION_PROFILE_STOP(&fpIDCT)

    return result;
}

#ifdef DYNAMIC_EDGEPAD

#define EDGE_WIDTH_LEFT 2
#define EDGE_WIDTH_RIGHT 2
#define BLOCKSIZE_PLUS_2EDGES (BLOCK_SIZE + (EDGE_WIDTH_LEFT+EDGE_WIDTH_RIGHT))

extern  Void_WMV edgePadding(tWMVEdgePad * pEdgePad,  EdgePad_Y_UV_Data * pData, EdgePad_Blk_Data * pBlkData );

#endif

tWMVDecodeStatus DecodePMB_EMB (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
//    const Bool* rgCBP, 
    U8_WMV* ppxlcCurrQYMB,
    U8_WMV* ppxlcCurrQUMB,
    U8_WMV* ppxlcCurrQVMB,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
)
{
#ifdef _WMV9AP_
    const U8_WMV * rgCBP = pmbmd->m_rgbCodedBlockPattern2;
#else
    const U8_WMV rgCBP = pmbmd->m_rgbCodedBlockPattern;
#endif
    tWMVDecodeStatus result=WMV_Succeeded;
    I32_WMV iXformType = pWMVDec->m_iFrameXformMode;
    I32_WMV * ppxliErrorQ32=pWMVDec->m_ppxliErrorQ->i32;
    Bool_WMV bXEvenY = !(xRefY & 1), bYEvenY = !(yRefY & 1);
    I32_WMV iMotionCompAndAddErrorIndexY;
    I32_WMV iMotionCompAndAddErrorIndexUV;
    I32_WMV i;      // SH4 does not like this declared inside for loop

    U8_WMV picker=32;
    // UV
    Bool_WMV bXEvenUV = !(xRefUV & 1), bYEvenUV = !(yRefUV & 1);
    const U8_WMV riXformTypeTable[4]={XFORMMODE_8x8,XFORMMODE_8x8,XFORMMODE_8x4,XFORMMODE_4x8};

    CDCTTableInfo_Dec* pInterDCTTableInfo_Dec;

#ifndef DYNAMIC_EDGEPAD

    U8_WMV* ppxlcRefY ;
    I32_WMV iPxLoc ;
    U8_WMV* ppxlcRefU ;
    U32_WMV iWidthPrevY ;
    U32_WMV iWidthPrevUV ;
    I32_WMV diff_currUV;
    I32_WMV diff_refUV;

    I32_WMV iWidthInc;
#else
    tWMVEdgePad * pEdgePad ;
    EdgePad_Blk_Data * pBlkData ;
    I32_WMV offset_y ;
    I32_WMV offset_uv ;

   // pEdgePad->m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;

    EdgePad_Y_UV_Data * pData;
    I32_WMV iYInsideBlock;
    U8_WMV * ppxlcRef;
#endif

#ifndef DYNAMIC_EDGEPAD

    U8_WMV * pCurrUV, * pRefUV;
#endif

    FUNCTION_PROFILE(fpPMB[2]);     // must keep embedded profiles in stack order (decreasing)
    FUNCTION_PROFILE_START(&fpPMB[1],DECODEPMB_PROFILE)
    FUNCTION_PROFILE_START(&fpPMB[0],DECODEPMBINIT_PROFILE)
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodePMB_EMB);


    if (pWMVDec->m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;


    
    if (pWMVDec->m_bDCTTable_MB){
        pInterDCTTableInfo_Dec = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index];
    }
    else{
        pInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec[0];
    }


    iMotionCompAndAddErrorIndexY=(bYEvenY<<1)|bXEvenY|(pWMVDec->m_iMixedPelMV<<2)|(pWMVDec->m_iMvResolution<<3);

    iMotionCompAndAddErrorIndexUV=(bYEvenUV<<1)|bXEvenUV;


    


    FUNCTION_PROFILE_STOP(&fpPMB[0])



#ifndef DYNAMIC_EDGEPAD

#if 1
	{ /* wshao 2011.3.18.
	   * pWMVDec->EXPANDY_REFVOP == 32; pWMVDec->EXPANDUV_REFVOP ==16;
	   * Should make sure ((yRefY >> 1) + pWMVDec->EXPANDY_REFVOP) >= 1.So,should yRefY >= -62.It is the boundary. 
	   * Otherwise ppxlcRefY will points to invalid address with "pLine = ppxlcRefMB - iWidthPrevRef;" in MC functions,such as MotionCompMixedAlignBlock().
	   * As well as yRefUV >= -30.
	   */
		if( yRefY < -62 ) {
			yRefY = -62;   
		}
		if( yRefUV < -30 ) {
			yRefUV = -30;   
		}
	}
#endif
    ppxlcRefY = pWMVDec->m_ppxliRef0Y + ((yRefY >> 1) + pWMVDec->EXPANDY_REFVOP) * pWMVDec->m_iWidthPrevY + (xRefY >> 1) + pWMVDec->EXPANDY_REFVOP;
    iPxLoc = ((yRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (xRefUV >> 1) + pWMVDec->EXPANDUV_REFVOP;
    ppxlcRefU = pWMVDec->m_ppxliRef0U + iPxLoc;
    iWidthPrevY = pWMVDec->m_iWidthPrevY + pWMVDec->m_iMotionCompRndCtrl;
    iWidthPrevUV = pWMVDec->m_iWidthPrevUV + pWMVDec->m_iMotionCompRndCtrl;
    diff_currUV=(I32_WMV)(((U32_WMV)ppxlcCurrQVMB)-((U32_WMV)ppxlcCurrQUMB));
    diff_refUV=(I32_WMV)(((U32_WMV)pWMVDec->m_ppxliRef0V)-((U32_WMV)pWMVDec->m_ppxliRef0U));
#else
    pEdgePad = &pWMVDec->m_tEdgePad;

    xRefUV>>=1;
    xRefY>>=1;
    yRefUV>>=1;
    yRefY>>=1;

    xRefY += EXPANDY_REFVOP ;
    yRefY += EXPANDY_REFVOP ;

    xRefUV += EXPANDUV_REFVOP ;
    yRefUV += EXPANDUV_REFVOP ;

    pBlkData = &pEdgePad->blk_data[0];

    offset_y = (yRefY ) * pWMVDec->m_iWidthPrevY + (xRefY ) ;

    offset_uv = (yRefUV ) * pWMVDec->m_iWidthPrevUV + (xRefUV ) ;


    xRefY -= EDGE_WIDTH_LEFT ;
    yRefY -= EDGE_WIDTH_LEFT ;
    xRefUV -= EDGE_WIDTH_LEFT ;
    yRefUV -= EDGE_WIDTH_LEFT ;

   // pEdgePad->m_iround_ctrl=pWMVDec->m_iMotionCompRndCtrl;

    pData=&pEdgePad->y_uv_data[1];

    iYInsideBlock=((U32_WMV)(pEdgePad->m_iWidthYMinusAllEdges - xRefY))|((U32_WMV)xRefY)|((U32_WMV)yRefY)
                 |((U32_WMV)(pEdgePad->m_iHeightYMinusAllEdges - yRefY));
    iYInsideBlock >>= 31;

    if(!iYInsideBlock)
    {
        
        ppxlcRef = pWMVDec->m_ppxliRef0Y + offset_y;
        pEdgePad->m_iWidthCombine = pWMVDec->m_iWidthPrevY  | ( pWMVDec->m_iWidthPrevY  << 17 ) | pWMVDec->m_iMotionCompRndCtrl;

    }

#endif

    for(i=0;i<4;i++, picker>>=1  )
    {

#ifdef DYNAMIC_EDGEPAD

        if(!iYInsideBlock)
        {
            pEdgePad->ppxlcRef  = ppxlcRef      + pBlkData->m_BlkInc;
            pEdgePad->ppxlcCurr = ppxlcCurrQYMB + pBlkData->m_BlkInc;
        }
        else
        {

            pBlkData->m_ppxliRef  = pWMVDec->m_ppxliRef0Y + offset_y;
            pBlkData->m_ppxliCurr = ppxlcCurrQYMB;

            pEdgePad->m_iPBMV_x = xRefY + ((1&i)<<3) ;
            pEdgePad->m_iPBMV_y = yRefY + ((2&i)<<2) ;

            edgePadding(pEdgePad, pData, pBlkData);
        }

        pBlkData++;
#define ppxlcCurrQYMB pEdgePad->ppxlcCurr
#define ppxlcRefY     pEdgePad->ppxlcRef
#define iWidthPrevY   pEdgePad->m_iWidthCombine

#endif
#ifndef _WMV9AP_
        if (rgCBP & picker) 
#else
        if (rgCBP [5 - i]) 
#endif
        {
            if (pmbmd->m_bBlkXformSwitchOn)
            {
                iXformType=BS_peekBits(pWMVDec->m_pbitstrmIn, 2);
#ifdef _EMB_WMV3_
                BS_adjustBits(pWMVDec->m_pbitstrmIn, (iXformType>>1)+1);
#else
                BS_getBits (pWMVDec->m_pbitstrmIn, (iXformType>>1)+1);
#endif
                
                iXformType=riXformTypeTable[iXformType];

            }


            result=(pWMVDec->m_CoefDecoderAndIdctDecTable[iXformType])(pWMVDec, pInterDCTTableInfo_Dec,iXformType);

            if(result!=WMV_Succeeded)
                goto bailout;

            FUNCTION_PROFILE_START(&fpPMB[0],MOTIONCOMPMIXED_PROFILE)
            pWMVDec->m_MotionCompAndAddErrorTable[iMotionCompAndAddErrorIndexY](ppxlcCurrQYMB,ppxlcRefY, iWidthPrevY, ppxliErrorQ32);
            FUNCTION_PROFILE_STOP(&fpPMB[0])
        } 
        else {
            FUNCTION_PROFILE_START(&fpPMB[0],MOTIONCOMPMIXED_PROFILE)
            pWMVDec->m_MotionCompAndAddErrorTable[iMotionCompAndAddErrorIndexY](ppxlcCurrQYMB, ppxlcRefY, iWidthPrevY, NULL);
            FUNCTION_PROFILE_STOP(&fpPMB[0])
        }

#ifndef DYNAMIC_EDGEPAD
        if(picker&0x28)
        {

            iWidthInc=BLOCK_SIZE;
        }
        else
            iWidthInc=pWMVDec->m_iWidthPrevYxBlkMinusBlk;


        ppxlcCurrQYMB+=iWidthInc;
        ppxlcRefY += iWidthInc;
#endif

    }

    
#ifndef DYNAMIC_EDGEPAD

    pCurrUV=ppxlcCurrQUMB;
    pRefUV=ppxlcRefU;

#else

#undef ppxlcCurrQYMB 
#undef ppxlcRefY     
#undef iWidthPrevY   

    pEdgePad->m_iPBMV_x = xRefUV;
    pEdgePad->m_iPBMV_y = yRefUV;
    pData--;

    pBlkData->m_ppxliCurr = ppxlcCurrQUMB;
    pBlkData++;
    pBlkData->m_ppxliCurr = ppxlcCurrQVMB;
    pBlkData --;

#endif
    for(;i<6;i++, picker>>=1 )
    {
#ifdef  DYNAMIC_EDGEPAD

        pBlkData->m_ppxliRef = pBlkData->m_ppxliRef0 + offset_uv;
        edgePadding(pEdgePad,  pData, pBlkData);
        pBlkData++;
#define pCurrUV pEdgePad->ppxlcCurr
#define pRefUV    pEdgePad->ppxlcRef
#define iWidthPrevUV   pEdgePad->m_iWidthCombine

#endif

#ifndef _WMV9AP_
        if (rgCBP & picker) 
#else
        if (rgCBP[5 -i]) 
#endif
        {
            if (pmbmd->m_bBlkXformSwitchOn)
            {

                iXformType=BS_peekBits(pWMVDec->m_pbitstrmIn, 2);
#ifdef _EMB_WMV3_
                BS_adjustBits(pWMVDec->m_pbitstrmIn, (iXformType>>1)+1);
#else
                BS_getBits (pWMVDec->m_pbitstrmIn, (iXformType>>1)+1);
#endif
                
                iXformType=riXformTypeTable[iXformType];
            }


            result=(pWMVDec->m_CoefDecoderAndIdctDecTable[iXformType])(pWMVDec, pInterDCTTableInfo_Dec,iXformType);

            if(result!=WMV_Succeeded)
                goto bailout;


            FUNCTION_PROFILE_START(&fpPMB[0],MOTIONCOMPMIXED_PROFILE)
#ifndef OUTPUT_GRAY4
                pWMVDec->m_MotionCompAndAddErrorTable[iMotionCompAndAddErrorIndexUV](pCurrUV,pRefUV, iWidthPrevUV,ppxliErrorQ32);
#endif
            FUNCTION_PROFILE_STOP(&fpPMB[0])
        } 
        else {
            FUNCTION_PROFILE_START(&fpPMB[0],MOTIONCOMPMIXED_PROFILE)
#ifndef OUTPUT_GRAY4
                pWMVDec->m_MotionCompAndAddErrorTable[iMotionCompAndAddErrorIndexUV](pCurrUV,pRefUV, iWidthPrevUV,NULL);
#endif
            FUNCTION_PROFILE_STOP(&fpPMB[0])
        }

#ifndef DYNAMIC_EDGEPAD
        pCurrUV += diff_currUV;
        pRefUV  += diff_refUV;
#endif

    }
#ifdef DYNAMIC_EDGEPAD
#undef pCurrUV 
#undef pRefUV    
#undef iWidthPrevUV   
#endif
    
bailout:

    FUNCTION_PROFILE_STOP(&fpPMB[1])
    return result;
}

tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MPEG4_EMB(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode)
{
    I16_WMV* rgiCoefRecon16bit=(I16_WMV*)pWMVDec->m_rgiCoefRecon;
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
    I32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec->iTcoef_ESCAPE;

    Bool_WMV bIsLastRun = FALSE_WMV;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    I32_WMV iMaxAC;
    register I32_WMV iIndex;
    U8_WMV   lIndex;
    I32_WMV iDoubleStepSize = pWMVDec->m_iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pWMVDec->m_iStepMinusStepIsEven;
    register I32_WMV iDCTHorzFlags = 0;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseInterBlockQuantizeEscCode_MPEG4_EMB);

#if !defined(_WIN32) && !defined(_Embedded_x86) && !defined(_APOGEE_) && !defined(__arm) && !defined(macintosh) && !defined(_MAC_OS) && !defined(_IOS)
    assert(BLOCK_SQUARE_SIZE_2 <= UINT_MAX);
#endif
    memset (rgiCoefRecon16bit, 0, (size_t) MB_SQUARE_SIZE);
    iMaxAC = (1<<11) -1; 
    do {
        //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
        lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
        //lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
        if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
            return WMV_CorruptedBits;
        }
        //lIndex = pWMVDec->m_pentrdecDCT->decodeSymbol();
        // if Huffman
        if (lIndex != iTCOEF_ESCAPE)    {
            if (lIndex >= iStartIndxOfLastRun)
                bIsLastRun = TRUE_WMV;
            uiRun = rgRunAtIndx[lIndex];
            iLevel = (BS_getBit (pWMVDec->m_pbitstrmIn)) ? - rgLevelAtIndx[lIndex] :
                rgLevelAtIndx[lIndex];
            if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                return WMV_CorruptedBits;
            }
        } 
        else {
            if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
                // ESC + '0' + VLC
                //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
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
                if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
            }
            else if (BS_getBit (pWMVDec->m_pbitstrmIn)==0){
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
                // ESC + '10' + VLC
                //lIndex = (U8_WMV) hufDCTACDec->get (pWMVDec->m_pbitstrmIn);
                lIndex = (U8_WMV) Huffman_WMV_get (hufDCTACDec, pWMVDec->m_pbitstrmIn);
                //lIndex = BS_getMaxBits ( pWMVDec->m_pbitstrmIn, MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//pWMVDec->m_iMaxCodeLngth, pWMVDec->m_rgCodeSymSize);
                if (BS_invalid ( pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    return WMV_CorruptedBits;
                }
                uiRun = rgRunAtIndx[lIndex];
                iLevel = rgLevelAtIndx[lIndex];
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE_WMV;
                    uiRun += (rgIfLastNumOfRunAtLevel[iLevel]+1);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel]+1);
                if (BS_getBit (pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
            }
            else{
                // ESC + '11' + FLC
                bIsLastRun = (Bool_WMV) BS_getBit (pWMVDec->m_pbitstrmIn); // escape decoding
                uiRun = BS_getBits (pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);         
                BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
                iLevel = (I32_WMV) BS_getBits  ( pWMVDec->m_pbitstrmIn, 12);
                assert (iLevel != iMaxAC +1);
                if(iLevel > iMaxAC)
                    iLevel -= (1<<12);
                BS_getBit (pWMVDec->m_pbitstrmIn); // marker bit
                if (BS_invalid ( pWMVDec->m_pbitstrmIn)) {
                    return WMV_CorruptedBits;
                }
            }
        }
        uiCoefCounter += uiRun;
//         fprintf(fp_head,"(%d %d)", uiRun,iLevel);

        if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
#if 0 //2008/01/18
			uiCoefCounter &= 63;
			if(!uiCoefCounter) {
				uiCoefCounter = 63;
			}
#else
            return WMV_CorruptedBits;
#endif
        }

        iIndex = pWMVDec->m_pZigzagInv [uiCoefCounter];
        if (iIndex & 0x7)
            iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));

        if (iLevel == 1)
            rgiCoefRecon16bit [pZigzagInv [uiCoefCounter]] = pWMVDec->m_i2DoublePlusStepSize;
        else if (iLevel == -1)
            rgiCoefRecon16bit [pZigzagInv [uiCoefCounter]] = pWMVDec->m_i2DoublePlusStepSizeNeg;
        else if (iLevel > 0)
//          I32_WMV iCoefRecon = pWMVDec->m_iStepSize * ((iLevel << 1) + 1) - pWMVDec->m_bStepSizeIsEven;
            rgiCoefRecon16bit [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel + iStepMinusStepIsEven;
        else
//          I32_WMV iCoefRecon = pWMVDec->m_bStepSizeIsEven - pWMVDec->m_iStepSize * (1 - (iLevel << 1));
            rgiCoefRecon16bit [pZigzagInv [uiCoefCounter]] = iDoubleStepSize * iLevel - iStepMinusStepIsEven;

        uiCoefCounter++;
    } while (!bIsLastRun);
    pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
#ifdef __MACVIDEO__
    pWMVDec->m_iDCTHorzFlags = 0xffffffff;
#endif
    return WMV_Succeeded;
}

#ifdef DYNAMIC_EDGEPAD

Void_WMV (*EdgePadFuncTable[16])(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);

#ifndef WMV_OPT_DYNAMICPAD_ARM
Void_WMV edgePadding(tWMVEdgePad * pEdgePad,  EdgePad_Y_UV_Data * pData, EdgePad_Blk_Data * pBlkData )
{
    I32_WMV index=0;


    I32_WMV outer_x=pEdgePad->m_iPBMV_x;
    I32_WMV outer_y=pEdgePad->m_iPBMV_y;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgePadding);

    index = ((pData->m_iPMBMV_limit_x - outer_x ) >> 28) & 0x8;
    index |= (((U32_WMV)outer_x)>>29)&0x4;
    index |= ((pData->m_iPMBMV_limit_y - outer_y ) >> 30) & 0x2;
    index |= ((U32_WMV)outer_y)>>31;

    if(!index)
    {
        
        pEdgePad->ppxlcRef = pBlkData->m_ppxliRef + pBlkData->m_BlkInc;
        pEdgePad->m_iWidthCombine=pData->m_iWidth;
    }
    else
    {
        U8_WMV * pSrc, * pDst;
        I32_WMV i, k;
        U8_WMV * pSrc2;
        I32_WMV length_middle;

        EdgePadFuncTable[index](pEdgePad, pData);

        pSrc = pBlkData->m_ppxliRef0 + pEdgePad->src_offset;
        pDst = &pEdgePad->buffer[0];



        length_middle=BLOCKSIZE_PLUS_2EDGES  - pEdgePad->x_off_right;

        for(i=pEdgePad->m_iPBMV_y; i< ( pEdgePad->m_iPBMV_y + BLOCKSIZE_PLUS_2EDGES ); i++)
        {
            
            for(k=0; k< pEdgePad->x_off_left ; k++) *pDst++ = pSrc[0];
            pSrc2 = pSrc;
            for( ; k < length_middle; k++) *pDst++ = *pSrc2++;
            for( ; k< BLOCKSIZE_PLUS_2EDGES ; k++) *pDst++ = pSrc2[-1];
            if((i>=0)&&(i<(pData->m_iHeight-1)))
                pSrc+=pData->m_iWidth;
        }

        pEdgePad->ppxlcRef=pEdgePad->ppxlcRefBufferOrig;

        pEdgePad->m_iWidthCombine=BLOCKSIZE_PLUS_2EDGES;
    }

    pEdgePad->m_iWidthCombine = pData->m_iWidth | (pEdgePad->m_iWidthCombine<<17) | pEdgePad->m_iround_ctrl;

    pEdgePad->ppxlcCurr = pBlkData->m_ppxliCurr + pBlkData->m_BlkInc;

}

#endif

/********************************************
*
*  table index mapping: 
*       0<x<width-delta  ---> 0
*       x<0              ---> 1
*       x> width-delta   ---> 2
*
*      11|01|21
*      ---------
*      10|00|20
*      ---------
*      12|02|22
*  
*
*********************************************
*/
Void_WMV edgepad_tableY01(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);
Void_WMV edgepad_tableY11(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);
Void_WMV edgepad_tableY21(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);
Void_WMV edgepad_tableY10(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);
Void_WMV edgepad_tableY20(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);
Void_WMV edgepad_tableY02(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);
Void_WMV edgepad_tableY12(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);
Void_WMV edgepad_tableY22(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData);

Void_WMV edgepad_tableY01(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY01);

   assert(pEdgePad->m_iPBMV_y<0);
   assert(pEdgePad->m_iPBMV_x>=0);
   pEdgePad->src_offset=pEdgePad->m_iPBMV_x;
   pEdgePad->x_off_left=0;
   pEdgePad->x_off_right=0;
    
}

Void_WMV edgepad_tableY11(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY11);

   assert(pEdgePad->m_iPBMV_y<0);
   assert(pEdgePad->m_iPBMV_x<0);
   pEdgePad->src_offset=0;
   pEdgePad->x_off_left=-pEdgePad->m_iPBMV_x;
   if(pEdgePad->x_off_left>BLOCKSIZE_PLUS_2EDGES)
       pEdgePad->x_off_left=BLOCKSIZE_PLUS_2EDGES;
   pEdgePad->x_off_right=0;

}

Void_WMV edgepad_tableY21(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY21);

        assert(pEdgePad->m_iPBMV_y<0);
        assert(pEdgePad->m_iPBMV_x>pData->m_iPMBMV_limit_x);
        pEdgePad->src_offset=pEdgePad->m_iPBMV_x;
        pEdgePad->x_off_left=0;
        pEdgePad->x_off_right=pEdgePad->m_iPBMV_x-pData->m_iPMBMV_limit_x;
        if(pEdgePad->x_off_right>BLOCKSIZE_PLUS_2EDGES)
        {
            pEdgePad->x_off_right=BLOCKSIZE_PLUS_2EDGES;
            pEdgePad->src_offset=pData->m_iWidth;
        }
}
Void_WMV edgepad_tableY10(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY10);

        assert(pEdgePad->m_iPBMV_y>=0);
        assert(pEdgePad->m_iPBMV_x<0);
        pEdgePad->src_offset=pEdgePad->m_iPBMV_y*pData->m_iWidth;
        pEdgePad->x_off_left=-pEdgePad->m_iPBMV_x;
        if(pEdgePad->x_off_left>BLOCKSIZE_PLUS_2EDGES)
            pEdgePad->x_off_left=BLOCKSIZE_PLUS_2EDGES;
        pEdgePad->x_off_right=0;

}

Void_WMV edgepad_tableY20(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY20);

        assert(pEdgePad->m_iPBMV_y>=0);
        assert(pEdgePad->m_iPBMV_x>pData->m_iPMBMV_limit_x);
        pEdgePad->src_offset = pEdgePad->m_iPBMV_x + pEdgePad->m_iPBMV_y * pData->m_iWidth;
        pEdgePad->x_off_left = 0;
        pEdgePad->x_off_right = pEdgePad->m_iPBMV_x - pData->m_iPMBMV_limit_x;
        if(pEdgePad->x_off_right > BLOCKSIZE_PLUS_2EDGES)
        {
            pEdgePad->x_off_right = BLOCKSIZE_PLUS_2EDGES;
            pEdgePad->src_offset = ( 1 + pEdgePad->m_iPBMV_y ) * pData->m_iWidth;
        }

}

Void_WMV edgepad_tableY12(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY12);

        assert(pEdgePad->m_iPBMV_y>pData->m_iPMBMV_limit_y);
        assert(pEdgePad->m_iPBMV_x<0);

        if(pEdgePad->m_iPBMV_y>=pData->m_iHeight)
            pEdgePad->src_offset=pData->m_iStartOfLastLine;
        else
            pEdgePad->src_offset=pEdgePad->m_iPBMV_y*pData->m_iWidth;
        pEdgePad->x_off_left=-pEdgePad->m_iPBMV_x;
        if(pEdgePad->x_off_left>BLOCKSIZE_PLUS_2EDGES)
            pEdgePad->x_off_left=BLOCKSIZE_PLUS_2EDGES;
        pEdgePad->x_off_right=0;

}

Void_WMV edgepad_tableY02(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY02);
        assert(pEdgePad->m_iPBMV_y>pData->m_iPMBMV_limit_y);
        assert(pEdgePad->m_iPBMV_x>=0);

        if(pEdgePad->m_iPBMV_y>=pData->m_iHeight)
            pEdgePad->src_offset=pEdgePad->m_iPBMV_x + pData->m_iStartOfLastLine;
        else
            pEdgePad->src_offset=pEdgePad->m_iPBMV_x + pEdgePad->m_iPBMV_y*pData->m_iWidth;
        pEdgePad->x_off_left=0;
        pEdgePad->x_off_right=0;

}

Void_WMV edgepad_tableY22(tWMVEdgePad * pEdgePad, EdgePad_Y_UV_Data * pData)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgepad_tableY22);
        assert(pEdgePad->m_iPBMV_y>pData->m_iPMBMV_limit_y);
        assert(pEdgePad->m_iPBMV_x>pData->m_iPMBMV_limit_x);
        if(pEdgePad->m_iPBMV_y>=pData->m_iHeight)
            pEdgePad->src_offset=pEdgePad->m_iPBMV_x + pData->m_iStartOfLastLine;
        else
            pEdgePad->src_offset=pEdgePad->m_iPBMV_x + pEdgePad->m_iPBMV_y*pData->m_iWidth;
        
        pEdgePad->x_off_left=0;
        pEdgePad->x_off_right = pEdgePad->m_iPBMV_x - pData->m_iPMBMV_limit_x;
        if(pEdgePad->x_off_right>BLOCKSIZE_PLUS_2EDGES)
        {
            pEdgePad->src_offset -= pEdgePad->x_off_right - BLOCKSIZE_PLUS_2EDGES;
            pEdgePad->x_off_right = BLOCKSIZE_PLUS_2EDGES;
        }

}

tWMVDecodeStatus edgePadInit(tWMVDecInternalMember *pWMVDec, tWMVEdgePad * pEdgePad )
{
    EdgePad_Y_UV_Data * pData;
    U32_WMV offset;
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgePadInit);


    // Weidong commented that EdgePadFuncTable[] is used only for 11 entries, and
    // we never touch the ones we did not initialize.  It is okay for some of them
    // to be assigned with NULL.  (Tung-chiang Yang, Feb. 18, 2002)

    EdgePadFuncTable[0x0]=NULL; 
    EdgePadFuncTable[0x1]=edgepad_tableY01; 
    EdgePadFuncTable[0x2]=edgepad_tableY02; 
    EdgePadFuncTable[0x3]=NULL; 
    EdgePadFuncTable[0x4]=edgepad_tableY10; 
    EdgePadFuncTable[0x5]=edgepad_tableY11; 
    EdgePadFuncTable[0x6]=edgepad_tableY12; 
    EdgePadFuncTable[0x7]=NULL; 
    EdgePadFuncTable[0x8]=edgepad_tableY20; 
    EdgePadFuncTable[0x9]=edgepad_tableY21; 
    EdgePadFuncTable[0xa]=edgepad_tableY22; 


    

    pEdgePad->m_iWidthYMinusAllEdges = pWMVDec->m_iWidthPrevY - BLOCKSIZE_PLUS_2EDGES - BLOCK_SIZE;  
    pEdgePad->m_iHeightYMinusAllEdges = pWMVDec->m_iHeightPrevY - BLOCKSIZE_PLUS_2EDGES - BLOCK_SIZE;  
    pData=&pEdgePad->y_uv_data[0];

    pData->m_iWidth = pWMVDec->m_iWidthPrevUV;
    pData->m_iPMBMV_limit_x = pData->m_iWidth - BLOCKSIZE_PLUS_2EDGES;
    pData->m_iHeight = pWMVDec->m_iHeightPrevUV;
    pData->m_iPMBMV_limit_y = pData->m_iHeight - BLOCKSIZE_PLUS_2EDGES;
    pData->m_iStartOfLastLine = (pData->m_iHeight-1)*pData->m_iWidth;

    pData++;

    pData->m_iWidth = pWMVDec->m_iWidthPrevY;
    pData->m_iPMBMV_limit_x = pData->m_iWidth - BLOCKSIZE_PLUS_2EDGES;
    pData->m_iHeight = pWMVDec->m_iHeightPrevY;
    pData->m_iPMBMV_limit_y = pData->m_iHeight - BLOCKSIZE_PLUS_2EDGES;
    pData->m_iStartOfLastLine = (pData->m_iHeight-1)*pData->m_iWidth;

    pEdgePad->blk_data[0].m_BlkInc = 0;
    pEdgePad->blk_data[1].m_BlkInc = pEdgePad->blk_data[0].m_BlkInc + BLOCK_SIZE;
    pEdgePad->blk_data[2].m_BlkInc = pEdgePad->blk_data[1].m_BlkInc + pWMVDec->m_iWidthPrevYxBlkMinusBlk;
    pEdgePad->blk_data[3].m_BlkInc = pEdgePad->blk_data[2].m_BlkInc + BLOCK_SIZE;
    pEdgePad->blk_data[4].m_BlkInc = 0;
    pEdgePad->blk_data[5].m_BlkInc = 0;

#ifdef XDM
	pEdgePad->buffer = (U8_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += (BLOCKSIZE_PLUS_2EDGES*BLOCKSIZE_PLUS_2EDGES)+32;
#else
    pEdgePad->buffer=(U8_WMV *)wmvMalloc(pWMVDec, (BLOCKSIZE_PLUS_2EDGES*BLOCKSIZE_PLUS_2EDGES)+32, DHEAP_STRUCT);
    if (pEdgePad->buffer == NULL_WMV) {
        return  WMV_BadMemory;
    }
#endif 


    offset=((U32_WMV)pEdgePad->buffer)&0x1f;
    
    pEdgePad->buffer=(U8_WMV *)((((U32_WMV)pEdgePad->buffer)^offset)+32);

    pEdgePad->buffer[-1]=(U8_WMV)offset;




    pEdgePad->ppxlcRefBufferOrig=pEdgePad->buffer + BLOCKSIZE_PLUS_2EDGES*EDGE_WIDTH_LEFT+EDGE_WIDTH_LEFT;

    return WMV_Succeeded;

}

Void_WMV edgePadSetup(tWMVDecInternalMember *pWMVDec, tWMVEdgePad * pEdgePad )
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgePadSetup);

    EdgePad_Blk_Data * pBlkData = &pEdgePad->blk_data[0];

    (pBlkData++)->m_ppxliRef0 = pWMVDec->m_ppxliRef0Y;
    (pBlkData++)->m_ppxliRef0 = pWMVDec->m_ppxliRef0Y;
    (pBlkData++)->m_ppxliRef0 = pWMVDec->m_ppxliRef0Y;
    (pBlkData++)->m_ppxliRef0 = pWMVDec->m_ppxliRef0Y;
    (pBlkData++)->m_ppxliRef0 = pWMVDec->m_ppxliRef0U;
    (pBlkData++)->m_ppxliRef0 = pWMVDec->m_ppxliRef0V;

}

Void_WMV edgePadDestroy(tWMVEdgePad * pEdgePad)
{
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(edgePadDestroy);
    if (pEdgePad != NULL_WMV && pEdgePad->buffer != NULL_WMV ) {
        U8_WMV offset=pEdgePad->buffer[-1];
        assert((((U32_WMV)pEdgePad->buffer)&0x1f)==0);
        wmvFree(pWMVDec, pEdgePad->buffer + offset -32);
        pEdgePad->buffer = NULL_WMV;
    }
}

#endif //DYNAMIC_EDGEPAD


tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCodeEMB2(tWMVDecInternalMember *pWMVDec,CDCTTableInfo_Dec* InterDCTTableInfo_Dec, 
                                                                U32_WMV  *puiRun, 
                                                                I32_WMV * piLevel, 
                                                                Bool_WMV * pbIsLastRun );


#if !defined(WMV_OPT_BLKDEC_ARM) && !defined(_EMB_ASM_MIPS_BLKDEC_)
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCodeEMB(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, EMB_DecodeP_ShortCut * pShortCut)
{



    I16_WMV * rgLevelRunAtIndx = InterDCTTableInfo_Dec -> combined_levelrun;
    U32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
    U32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;

    tWMVDecodeStatus result=WMV_Succeeded;
    Bool_WMV bIsLastRun = FALSE;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    U32_WMV   lIndex;
    register I32_WMV iIndex;

 //   I32_WMV iDoubleStepSize = m_iDoubleStepSize;
//    I32_WMV iStepMinusStepIsEven = m_iStepMinusStepIsEven;

    register I32_WMV iDCTHorzFlags = 0;



#if !defined(IDCTDEC_32BIT_INPUT)

    I16_WMV *   rgiCoefRecon16bit=(I16_WMV*)pShortCut->m_rgiCoefRecon;
#define rgiCoefRecon2 rgiCoefRecon16bit
#else

    I32_WMV *   rgiCoefRecon32bit=(I32_WMV*)pShortCut->m_rgiCoefRecon;
#define rgiCoefRecon2 rgiCoefRecon32bit
#endif


    U32_WMV ptr=(U32_WMV)pShortCut->m_rgiCoefRecon;
    U32_WMV ptrmax=ptr+256;

 FUNCTION_PROFILE_DECL_START(fpDecodeIQ,DECINVIBQUANTESCCODE_PROFILE );
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseInterBlockQuantizeEscCodeEMB);
#if !defined(_WIN32) && !defined(_APOGEE_) && !defined(__arm) && !defined(macintosh) && !defined(_MAC_OS) && !defined(_IOS)
    assert(BLOCK_SQUARE_SIZE_2 <= UINT_MAX);
#endif
    if(pShortCut->XFormMode!=XFORMMODE_8x8)
    {
        ptrmax-=128;
    }
    
    for(;ptr<ptrmax;ptr+=32)
    {

       *(U32_WMV *)(ptr+0)=0;
       *(U32_WMV *)(ptr+4)=0;
       *(U32_WMV *)(ptr+8)=0;
       *(U32_WMV *)(ptr+12)=0;
       *(U32_WMV *)(ptr+16)=0;
       *(U32_WMV *)(ptr+20)=0;
       *(U32_WMV *)(ptr+24)=0;
       *(U32_WMV *)(ptr+28)=0;


    }
    

    do {        

        lIndex = Huffman_WMV_get (InterDCTTableInfo_Dec -> hufDCTACDec, pShortCut->m_pbitstrmIn);
        
  //       lIndex = hufDCTACDec -> get (m_pbitstrmIn);

        //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
        
        if (lIndex != iTCOEF_ESCAPE)    
        {
            
            if (lIndex >= iStartIndxOfLastRun)
                bIsLastRun = TRUE;
            uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
            iLevel= rgLevelRunAtIndx[lIndex] >>8;

            if (iLevel == 1)
              iLevel= pShortCut->m_i2DoublePlusStepSize;
            else
              iLevel = pShortCut->m_iDoubleStepSize * iLevel + pShortCut->m_iStepMinusStepIsEven;
            if(BS_getBit(pShortCut->m_pbitstrmIn))
                iLevel=-iLevel;

            

        } 
        else 
        {


           result=DecodeInverseInterBlockQuantizeEscCodeEMB2(pWMVDec,InterDCTTableInfo_Dec, &uiRun, &iLevel, &bIsLastRun);
        
           if(result!=WMV_Succeeded)
                goto bailout;

        }
        uiCoefCounter += uiRun;

        if (uiCoefCounter >= BLOCK_SQUARE_SIZE) {
            
#if 0 //2008/01/18
			uiCoefCounter &= 63;
			if(!uiCoefCounter) {
				uiCoefCounter = 63;
			}
#else
            result= WMV_CorruptedBits;
            goto bailout;
#endif
        }



        iIndex = pZigzagInv [uiCoefCounter];

        // If this coefficient is not in the first column then set the flag which indicates
        // what row it is in. This flag field will be used by the IDCT to see if it can
        // shortcut the IDCT of the row if all coefficients are zero.
        if (iIndex & 0x7)
            iDCTHorzFlags |= (1<<((iIndex & 0x38)>>3));



        rgiCoefRecon2[iIndex]=iLevel;

        uiCoefCounter++;
    } while (!bIsLastRun);

    if (BS_invalid (pShortCut->m_pbitstrmIn)) 
    {
                    result= WMV_CorruptedBits;
                    goto bailout;
    }


    // Save the DCT row flags. This will be passed to the IDCT routine
    pShortCut->m_iDCTHorzFlags = iDCTHorzFlags;


    
bailout:

#undef rgiCoefRecon2 

    FUNCTION_PROFILE_STOP(&fpDecodeIQ);
    return result;
}

#endif //#if !defined (WMV_OPT_BLKDEC_ARM

#define WMV_ESC_Decoding()                                                          \
    if (pWMVDec->m_bFirstEscCodeInFrame){                                                    \
        decodeBitsOfESCCode (pWMVDec);                                                     \
        pWMVDec->m_bFirstEscCodeInFrame = FALSE;                                             \
    }                                                                               \
    uiRun = BS_getBits  (pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);                               \
    if (BS_getBit (pWMVDec->m_pbitstrmIn)) /* escape decoding */                              \
        iLevel = -1 * BS_getBits  (pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                 \
    else                                                                            \
        iLevel = BS_getBits  (pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);         

tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCodeEMB2(tWMVDecInternalMember *pWMVDec,CDCTTableInfo_Dec* InterDCTTableInfo_Dec, 
                                                                U32_WMV  *puiRun, 
                                                                I32_WMV * piLevel, 
                                                                Bool_WMV * pbIsLastRun )
{


   
    U32_WMV  uiRun= *puiRun;
    I32_WMV iLevel=* piLevel; 
    Bool_WMV bIsLastRun= 0;
    tWMVDecodeStatus result=WMV_Succeeded;

    I16_WMV * rgLevelRunAtIndx = InterDCTTableInfo_Dec -> combined_levelrun;

//  Change iStartIndxOfLastRun to iStartIndxOfLastRunMinus1 and <= to <
//  iStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1;

    U32_WMV iStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
    U32_WMV  iTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;
    
    U8_WMV lIndex;
     FUNCTION_PROFILE_DECL_START(fpDecodeIQ2,DECINVIBQUANTESCCODE_PROFILE );
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseInterBlockQuantizeEscCodeEMB2);

            if (BS_getBit (pWMVDec->m_pbitstrmIn))
            {
                U8_WMV* rgIfNotLastNumOfLevelAtRun = InterDCTTableInfo_Dec -> puiNotLastNumOfLevelAtRun;
                U8_WMV* rgIfLastNumOfLevelAtRun = InterDCTTableInfo_Dec -> puiLastNumOfLevelAtRun; 

                // ESC + '1' + VLC

                lIndex = (U8_WMV)Huffman_WMV_get (InterDCTTableInfo_Dec -> hufDCTACDec, pWMVDec->m_pbitstrmIn);

          //      lIndex = hufDCTACDec -> get (m_pbitstrmIn);

                //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);



                 if (lIndex == iTCOEF_ESCAPE) 
                { // ESC + 1 + ESC :  bits corrupted
                    result= WMV_CorruptedBits;
                    goto bailout;
                    
                }

            uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
            iLevel= rgLevelRunAtIndx[lIndex] >>8;
                
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    iLevel += (I8_WMV) rgIfLastNumOfLevelAtRun[uiRun];
                }
                else
                    iLevel += (I8_WMV) rgIfNotLastNumOfLevelAtRun[uiRun];

                iLevel = pWMVDec->m_iDoubleStepSize * iLevel + pWMVDec->m_iStepMinusStepIsEven;
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;

            }
            else if (BS_getBit (pWMVDec->m_pbitstrmIn))
            {
                U8_WMV* rgIfNotLastNumOfRunAtLevel = InterDCTTableInfo_Dec -> puiNotLastNumOfRunAtLevel;
                U8_WMV* rgIfLastNumOfRunAtLevel = InterDCTTableInfo_Dec -> puiLastNumOfRunAtLevel;

                // ESC + '10' + VLC

                lIndex = (U8_WMV)Huffman_WMV_get (InterDCTTableInfo_Dec -> hufDCTACDec, pWMVDec->m_pbitstrmIn);               
                // lIndex = hufDCTACDec -> get (m_pbitstrmIn);

                //lIndex = m_pbitstrmIn->getMaxBits(MAXCODEWORDLNGDCT,gDecodeCodeTableDCT);//m_iMaxCodeLngth, m_rgCodeSymSize);
                if (BS_invalid (pWMVDec->m_pbitstrmIn) || lIndex == iTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
                    result= WMV_CorruptedBits;
                    goto bailout;
                }


            uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
            iLevel= rgLevelRunAtIndx[lIndex] >>8;
                
                if (lIndex >= iStartIndxOfLastRun){
                    bIsLastRun = TRUE;
                    uiRun += (rgIfLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                }
                else
                    uiRun += (rgIfNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
                iLevel = pWMVDec->m_iDoubleStepSize * iLevel + pWMVDec->m_iStepMinusStepIsEven;
                if (BS_getBit(pWMVDec->m_pbitstrmIn)) 
                    iLevel = -iLevel;
            


            }
            else
            {
                // ESC + '00' + FLC
                bIsLastRun = (Bool_WMV) BS_getBit (pWMVDec->m_pbitstrmIn); // escape decoding
                if (pWMVDec->m_cvCodecVersion >= WMV1){
                    WMV_ESC_Decoding();
                }
                else{
                    uiRun = BS_getBits  (pWMVDec->m_pbitstrmIn, NUMBITS_ESC_RUN);           
                    iLevel = (I8_WMV)BS_getBits (pWMVDec->m_pbitstrmIn, NUMBITS_ESC_LEVEL);
                }
        
                if (iLevel > 0)
                    iLevel = pWMVDec->m_iDoubleStepSize * iLevel + pWMVDec->m_iStepMinusStepIsEven;
                else
                    iLevel= pWMVDec->m_iDoubleStepSize * iLevel - pWMVDec->m_iStepMinusStepIsEven;


            }




    
bailout:

    *puiRun= uiRun;
    * piLevel=iLevel; 
    * pbIsLastRun= bIsLastRun ;
    FUNCTION_PROFILE_STOP(&fpDecodeIQ2);
    return result;
}
#endif //EMB_WMV2

#ifdef WMV_OPT_COMBINE_LF_CC

Void_WMV g_FilterHorizontalEdge_WMV(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdge_WMV(U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);

tWMVDecodeStatus CombineLoopfilterRenderingPacked (
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* pucDecodedOutput
)
{
    U8_WMV __huge* ppxliPostY, *ppxliPostU, *ppxliPostV;
    I32_WMV iblkSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY>>1;  // / 2;
    
    U8_WMV* ppxliY = pWMVDec->m_pvopcRenderThis->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliU = pWMVDec->m_pvopcRenderThis->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliV = pWMVDec->m_pvopcRenderThis->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    I32_WMV iWidthPrevY=pWMVDec->m_iWidthPrevY;
    I32_WMV iWidthPrevUV=pWMVDec->m_iWidthPrevUV;
    I32_WMV iWidthBMP = pWMVDec->m_iWidthBMP;
    I32_WMV iStepSize=pWMVDec->m_iStepSize;
    I32_WMV iWidthInternalUVTimesBlk=pWMVDec->m_iWidthInternalUVTimesBlk;
    I32_WMV iMBEndX,  iMBEndY;

    U8_WMV __huge* pBmpPtr = pucDecodedOutput + pWMVDec->m_iBMPPointerStart;
    U8_WMV * pVhstart = ppxliY + 3; 
    I32_WMV iblkX ;
    Bool_WMV bMbAligned;
    I32_WMV iMBsPerRow;
    I32_WMV imbY;
    I32_WMV iYVhstartOffset;
#ifndef OUTPUT_GRAY4
    U8_WMV* pUV5;
    U8_WMV* pVV5;
    I32_WMV iUVVhstartOffset;
#endif

    FUNCTION_PROFILE_DECL_START(fp,LOOPFILTERANDCOLORCONV_PROFILE);
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(CombineLoopfilterRenderingPacked);

#if defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined(OUTPUT_GRAY4) || defined(OUTPUT_RGB16)
#ifdef _ARM_
    pWMVDec->m_iXscalePrefechDistY = 32;
    pWMVDec->m_iXscalePrefechDistUV = 32;
#endif 
#endif 

    iMBEndX=pWMVDec->m_uiNumMBX;
    iMBEndY=pWMVDec->m_uiNumMBY;
    iMBEndY--;        
    for ( iblkX = 0; iblkX < (2*iMBEndX - 1); iblkX++) 
    {        
        g_FilterVerticalEdge_WMV(pVhstart, iWidthPrevY, iStepSize, 4);
        pVhstart+=8;        
    }
   
    if(((pWMVDec -> m_iWidthY - pWMVDec -> m_iFrmWidthSrc)==0)&&
        (abs(pWMVDec -> m_iHeightY) - abs(pWMVDec -> m_iFrmHeightSrc))==0)
    {
        bMbAligned=TRUE_WMV;
    }
    else
        bMbAligned=FALSE_WMV;
  
    ppxliPostY = ppxliY;
    ppxliPostU = ppxliU;
    ppxliPostV = ppxliV;
    iMBsPerRow = iMBEndX;
    iYVhstartOffset = (-4)*iWidthPrevY - 1;
    for (imbY = 0; imbY < iMBEndY; imbY++) 
    {

        U8_WMV __huge* ppxliCodedPostY = ppxliPostY;
        U8_WMV __huge* ppxliCodedPostU = ppxliPostU;
        U8_WMV __huge* ppxliCodedPostV = ppxliPostV;
        U8_WMV __huge* pBmpPtrMB = pBmpPtr;

        U8_WMV* pYV5=ppxliPostY+iblkSizeXWidthPrevY;
        U8_WMV* pYV5x=ppxliPostY+8*iWidthPrevY+iblkSizeXWidthPrevY;
        I32_WMV iUVVertSize=8;
        I32_WMV iUVVertOffset = -4;
        I32_WMV imbX;
        U8_WMV* pYVhstart;

        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 4);
        pYV5 += 4;
        g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 4);
        pYV5x += 4;
        if(imbY==0)
        {
            iUVVertSize=12;
            iUVVertOffset = -8;
        }
        else if(imbY==(iMBEndY-1))
        {
            iUVVertSize=12;
            iUVVertOffset = -4;
        }

#ifndef OUTPUT_GRAY4
            //U
            pUV5=ppxliPostU+iWidthInternalUVTimesBlk;
            g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 4);
            pUV5 += 4;

            iUVVhstartOffset = iUVVertOffset*iWidthPrevUV - 1;

            //V
            pVV5=ppxliPostV + iWidthInternalUVTimesBlk;
            g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 4);
            pVV5 += 4;
#endif
        
        for (imbX = 0; imbX < (iMBsPerRow -1); imbX++) 
        {
            //Y
            U8_WMV* pYVhstart = pYV5 + iYVhstartOffset;  
            g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 16);
            pYV5 += 16;
            g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 16);        
            pYV5x += 16;
            g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 16);

#ifndef OUTPUT_GRAY4
            {
                U8_WMV* pUVhstart = pUV5 + iUVVhstartOffset; 
                U8_WMV* pVVhstart = pVV5 + iUVVhstartOffset; 
            
                g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 8);
                pUV5 += 8;

                g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 8);
                pVV5 += 8;

                g_FilterVerticalEdge_WMV(pUVhstart, iWidthPrevUV, iStepSize, iUVVertSize);          
                g_FilterVerticalEdge_WMV(pVVhstart, iWidthPrevUV, iStepSize, iUVVertSize);
            }
#endif

            g_FilterVerticalEdge_WMV(pYVhstart + 8, iWidthPrevY, iStepSize, 16);         
            pWMVDec->m_pWMVideoDecUpdateDstMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP);

            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
            pBmpPtrMB += pWMVDec->m_iBMPMBIncrement;
        }
                   //Y
        pYVhstart = pYV5 + iYVhstartOffset;  
        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 12);
        g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 12);              
        g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 16);
       
#ifndef OUTPUT_GRAY4
            g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 4);
            g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 4);
#endif

        if(bMbAligned==TRUE_WMV)
        {
             pWMVDec->m_pWMVideoDecUpdateDstMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP);
        }
        else
        {
            I32_WMV iValidWidth = 
                        (MB_SIZE - (pWMVDec -> m_iWidthY - pWMVDec -> m_iFrmWidthSrc));
            I32_WMV iValidHeight = 
                        MB_SIZE;
            pWMVDec->m_pWMVideoDecUpdateDstPartialMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iValidWidth, iValidHeight);
        }
        ppxliPostY += pWMVDec->m_iWidthInternalTimesMB;
        ppxliPostU += iWidthInternalUVTimesBlk;
        ppxliPostV += iWidthInternalUVTimesBlk;
        pBmpPtr += pWMVDec->m_iBMPMBHeightIncrement;
    }

    //row iMBEndY-1
 
    {
        U8_WMV __huge* ppxliCodedPostY = ppxliPostY;
        U8_WMV __huge* ppxliCodedPostU = ppxliPostU;
        U8_WMV __huge* ppxliCodedPostV = ppxliPostV;
        U8_WMV __huge* pBmpPtrMB = pBmpPtr;
        I32_WMV imbX;

        U8_WMV* pYV5=ppxliPostY+iblkSizeXWidthPrevY;
        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 4);
        pYV5 += 4;
        for (imbX = 0; imbX < iMBsPerRow; imbX++) 
        {
            U8_WMV* pYVhstart = pYV5 + iYVhstartOffset;        
            I32_WMV hy_size=16;
            I32_WMV huv_size=8;
            if(imbX==(iMBsPerRow-1))
            {
                hy_size=12;
                huv_size=4;
            }

            g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, hy_size);
            pYV5 += 16;
            g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 12);
            if(imbX < (iMBsPerRow-1))
                g_FilterVerticalEdge_WMV(pYVhstart + 8, iWidthPrevY, iStepSize, 12);
            
            if(bMbAligned==TRUE_WMV)
            {
                pWMVDec->m_pWMVideoDecUpdateDstMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP);
            }
            else
            {
                I32_WMV iValidWidth = (imbX != ( iMBEndX - 1)) ? MB_SIZE :
                        (MB_SIZE - (pWMVDec -> m_iWidthY - pWMVDec -> m_iFrmWidthSrc));

                I32_WMV iValidHeight = 
                        (imbY != iMBEndY) ? MB_SIZE :
                        (MB_SIZE - (abs(pWMVDec -> m_iHeightY) - abs(pWMVDec -> m_iFrmHeightSrc)));

                pWMVDec->m_pWMVideoDecUpdateDstPartialMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iValidWidth, iValidHeight);
            
            }
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
            pBmpPtrMB += pWMVDec->m_iBMPMBIncrement;
        }
    }

    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}

tWMVDecodeStatus CombineLoopfilterRenderingPlanar (
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* pucDecodedOutput
)
{   
    U8_WMV __huge* ppxliPostY, *ppxliPostU, *ppxliPostV;
    I32_WMV iblkSizeXWidthPrevY = pWMVDec->m_iMBSizeXWidthPrevY>>1;  // / 2;
    
//    U8_WMV* ppxliY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
//    U8_WMV* ppxliU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
//    U8_WMV* ppxliV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    U8_WMV* ppxliY = pWMVDec->m_pvopcRenderThis->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliU = pWMVDec->m_pvopcRenderThis->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliV = pWMVDec->m_pvopcRenderThis->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    I32_WMV iWidthPrevY = pWMVDec->m_iWidthPrevY;
    I32_WMV iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
    I32_WMV iWidthBMP = pWMVDec->m_iWidthBMP;
    I32_WMV iWidthBMPUV = pWMVDec->m_iWidthBMPUV;

    I32_WMV iStepSize=pWMVDec->m_iStepSize;
    I32_WMV iWidthInternalUVTimesBlk=pWMVDec->m_iWidthInternalUVTimesBlk;
    I32_WMV iMBEndX,  iMBEndY;

    //U8_WMV __huge* pBmpPtr = pucDecodedOutput + pWMVDec->m_iBMPPointerStart;
    
    U8_WMV __huge* pBmpPtrY = pucDecodedOutput + pWMVDec->m_iBMPPointerStart;
    U8_WMV __huge* pBmpPtrU = pBmpPtrY + pWMVDec->m_iDstBMPOffsetU;
    U8_WMV __huge* pBmpPtrV = pBmpPtrY + pWMVDec->m_iDstBMPOffsetV;

    U8_WMV * pVhstart = ppxliY + 3; 
    I32_WMV iblkX ;
    Bool_WMV bMbAligned;
    I32_WMV iMBsPerRow;
    I32_WMV imbY;
    I32_WMV iYVhstartOffset;
#ifndef OUTPUT_GRAY4
    U8_WMV* pUV5;
    U8_WMV* pVV5;
    I32_WMV iUVVhstartOffset;
#endif

    FUNCTION_PROFILE_DECL_START(fp,LOOPFILTERANDCOLORCONV_PROFILE);
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(CombineLoopfilterRenderingPlanar);

   iMBEndX=pWMVDec->m_uiNumMBX;
    iMBEndY=pWMVDec->m_uiNumMBY;
    iMBEndY--;        
    for ( iblkX = 0; iblkX < (2*iMBEndX - 1); iblkX++) 
    {        
        g_FilterVerticalEdge_WMV(pVhstart, iWidthPrevY, iStepSize, 4);
        pVhstart+=8;        
    }
   
    if(((pWMVDec -> m_iWidthY - pWMVDec -> m_iFrmWidthSrc)==0)&&
        (abs(pWMVDec -> m_iHeightY) - abs(pWMVDec -> m_iFrmHeightSrc))==0)
    {
        bMbAligned=TRUE_WMV;
    }
    else
        bMbAligned=FALSE_WMV;
  
    ppxliPostY = ppxliY;
    ppxliPostU = ppxliU;
    ppxliPostV = ppxliV;
    iMBsPerRow = iMBEndX;
    iYVhstartOffset = (-4)*iWidthPrevY - 1;
    for (imbY = 0; imbY < iMBEndY; imbY++) 
    {
        U8_WMV __huge* ppxliCodedPostY = ppxliPostY;
        U8_WMV __huge* ppxliCodedPostU = ppxliPostU;
        U8_WMV __huge* ppxliCodedPostV = ppxliPostV;
        //U8_WMV __huge* pBmpPtrMB = pBmpPtr;
        U8_WMV __huge* pBmpPtrYMB = pBmpPtrY;
        U8_WMV __huge* pBmpPtrUMB = pBmpPtrU;
        U8_WMV __huge* pBmpPtrVMB = pBmpPtrV;

        U8_WMV* pYV5=ppxliPostY+iblkSizeXWidthPrevY;
        U8_WMV* pYV5x=ppxliPostY+8*iWidthPrevY+iblkSizeXWidthPrevY;
        I32_WMV iUVVertSize=8;
        I32_WMV iUVVertOffset = -4;
        I32_WMV imbX;
        U8_WMV* pYVhstart;

        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 4);
        pYV5 += 4;
        g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 4);
        pYV5x += 4;
        if(imbY==0)
        {
            iUVVertSize=12;
            iUVVertOffset = -8;
        }
        else if(imbY==(iMBEndY-1))
        {
            iUVVertSize=12;
            iUVVertOffset = -4;
        }

#ifndef OUTPUT_GRAY4
            //U
            pUV5=ppxliPostU+iWidthInternalUVTimesBlk;
            g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 4);
            pUV5 += 4;

            iUVVhstartOffset = iUVVertOffset*iWidthPrevUV - 1;

            //V
            pVV5=ppxliPostV + iWidthInternalUVTimesBlk;
            g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 4);
            pVV5 += 4;
#endif
        
        for (imbX = 0; imbX < (iMBsPerRow -1); imbX++) 
        {
            //Y
            U8_WMV* pYVhstart = pYV5 + iYVhstartOffset;  
            g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 16);
            pYV5 += 16;
            g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 16);        
            pYV5x += 16;
            g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 16);

#ifndef OUTPUT_GRAY4
            {
                U8_WMV* pUVhstart = pUV5 + iUVVhstartOffset; 
                U8_WMV* pVVhstart = pVV5 + iUVVhstartOffset; 
            
                g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 8);
                pUV5 += 8;

                g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 8);
                pVV5 += 8;

                g_FilterVerticalEdge_WMV(pUVhstart, iWidthPrevUV, iStepSize, iUVVertSize);          
                g_FilterVerticalEdge_WMV(pVVhstart, iWidthPrevUV, iStepSize, iUVVertSize);
            }
#endif

            g_FilterVerticalEdge_WMV(pYVhstart + 8, iWidthPrevY, iStepSize, 16);         
            pWMVDec->m_pWMVideoDecUpdateDstYUVMB (
                        pWMVDec,
                        pBmpPtrYMB, pBmpPtrUMB, pBmpPtrVMB,
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP, iWidthBMPUV);

            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
            pBmpPtrYMB += MB_SIZE;
            pBmpPtrUMB += BLOCK_SIZE;
            pBmpPtrVMB += BLOCK_SIZE;
        }
                   //Y
        pYVhstart = pYV5 + iYVhstartOffset;  
        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 12);
        g_FilterHorizontalEdge_WMV(pYV5x, iWidthPrevY, iStepSize, 12);              
        g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 16);
       
#ifndef OUTPUT_GRAY4
            g_FilterHorizontalEdge_WMV(pUV5, iWidthPrevUV, iStepSize, 4);
            g_FilterHorizontalEdge_WMV(pVV5, iWidthPrevUV, iStepSize, 4);
#endif

        if(bMbAligned==TRUE_WMV)
        {
             pWMVDec->m_pWMVideoDecUpdateDstYUVMB (
                        pWMVDec,
                        pBmpPtrYMB, pBmpPtrUMB, pBmpPtrVMB,
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP, iWidthBMPUV);
        }
        else
        {
            I32_WMV iValidWidth = 
                        (MB_SIZE - (pWMVDec -> m_iWidthY - pWMVDec -> m_iFrmWidthSrc));
            I32_WMV iValidHeight = 
                        MB_SIZE;
            pWMVDec->m_pWMVideoDecUpdateDstPartialYUVMB (
                        pWMVDec,
                        pBmpPtrYMB, pBmpPtrUMB, pBmpPtrVMB,
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP, iWidthBMPUV,
                        iValidWidth, iValidHeight);
        }
        ppxliPostY += pWMVDec->m_iWidthInternalTimesMB;
        ppxliPostU += iWidthInternalUVTimesBlk;
        ppxliPostV += iWidthInternalUVTimesBlk;
        pBmpPtrY += pWMVDec->m_iBMPMBHeightIncrement;
        pBmpPtrU += pWMVDec->m_iBMPMBHeightIncrementUV;
        pBmpPtrV += pWMVDec->m_iBMPMBHeightIncrementUV;
    }

    //row iMBEndY-1
 
    {
        U8_WMV __huge* ppxliCodedPostY = ppxliPostY;
        U8_WMV __huge* ppxliCodedPostU = ppxliPostU;
        U8_WMV __huge* ppxliCodedPostV = ppxliPostV;
        U8_WMV __huge* pBmpPtrYMB = pBmpPtrY;
        U8_WMV __huge* pBmpPtrUMB = pBmpPtrU;
        U8_WMV __huge* pBmpPtrVMB = pBmpPtrV;
        I32_WMV imbX;

        U8_WMV* pYV5=ppxliPostY+iblkSizeXWidthPrevY;
        g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, 4);
        pYV5 += 4;
        for (imbX = 0; imbX < iMBsPerRow; imbX++) 
        {
            U8_WMV* pYVhstart = pYV5 + iYVhstartOffset;        
            I32_WMV hy_size=16;
            I32_WMV huv_size=8;
            if(imbX==(iMBsPerRow-1))
            {
                hy_size=12;
                huv_size=4;
            }

            g_FilterHorizontalEdge_WMV(pYV5, iWidthPrevY, iStepSize, hy_size);
            pYV5 += 16;
            g_FilterVerticalEdge_WMV(pYVhstart, iWidthPrevY, iStepSize, 12);
            if(imbX < (iMBsPerRow-1))
                g_FilterVerticalEdge_WMV(pYVhstart + 8, iWidthPrevY, iStepSize, 12);
            
            if(bMbAligned==TRUE_WMV)
            {
                pWMVDec->m_pWMVideoDecUpdateDstYUVMB (
                        pWMVDec,
                        pBmpPtrYMB, pBmpPtrUMB, pBmpPtrVMB,
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP, iWidthBMPUV);
            }
            else
            {
                I32_WMV iValidWidth = (imbX != ( iMBEndX - 1)) ? MB_SIZE :
                        (MB_SIZE - (pWMVDec -> m_iWidthY - pWMVDec -> m_iFrmWidthSrc));

                I32_WMV iValidHeight = 
                        (imbY != iMBEndY) ? MB_SIZE :
                        (MB_SIZE - (abs(pWMVDec -> m_iHeightY) - abs(pWMVDec -> m_iFrmHeightSrc)));

                pWMVDec->m_pWMVideoDecUpdateDstPartialYUVMB (
                        pWMVDec,
                        pBmpPtrYMB, pBmpPtrUMB, pBmpPtrVMB,
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        iWidthPrevY, iWidthPrevUV,
                        iWidthBMP, iWidthBMPUV,
                        iValidWidth, iValidHeight);
            
            }
            ppxliCodedPostY += MB_SIZE;
            ppxliCodedPostU += BLOCK_SIZE;
            ppxliCodedPostV += BLOCK_SIZE;
            pBmpPtrYMB += MB_SIZE;
            pBmpPtrUMB += BLOCK_SIZE;
            pBmpPtrVMB += BLOCK_SIZE;
        }
    }

    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}
#endif // combine loop + rendering


#ifdef  WMV_COMBINE_PPL_COLORCONV

#include "postfilter_wmv.h"

/*
 * Combine PostFilter and Color Conversion. Will benefit small cache devices.
 * Currently, only combine deblocking(PPL1-2) and ColorConversionPacked.
 */

extern Void_WMV Full_DeblockMB (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV        *ppxliY,
    U8_WMV        *ppxliU,
    U8_WMV        *ppxliV,
    Bool_WMV                  bDoRight,
    Bool_WMV                  bDoRightHalfEdge,
    Bool_WMV                  bDoBottom,
    Bool_WMV                  bDoBottomHalfEdge,
    Bool_WMV                  bDoMiddle,
    I32_WMV                   iStepSize,
    I32_WMV                   iWidthPrevY,
    I32_WMV                   iWidthPrevUV
);

#if 1

/* 
 * The following algorithm demonstrates Combined Color Conversion and PPL without changing buffers.
 * Comparing old algorithm, this will enable color conversion and PPL applied to the same MacroBlock.
 * Perf gain for SA1110 is invisible. However, it didn't slow down the perf for any reason.
 */

FORCEINLINE extern Void_WMV FilterEdge_Short (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, I32_WMV iPixelIncrement, I32_WMV iEdgeLength, I32_WMV iStepSize
);

tWMVDecodeStatus CombinePPLColorConvertPacked (tWMVDecInternalMember *pWMVDec, U8_WMV* pucDecodedOutput)
{

    U8_WMV* ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;

    U8_WMV* ppxliPostY = pWMVDec->m_ppxliPostQY  + pWMVDec->m_iWidthPrevYXExpPlusExp;
    U8_WMV* ppxliPostU = pWMVDec->m_ppxliPostQU  + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    U8_WMV* ppxliPostV = pWMVDec->m_ppxliPostQV  + pWMVDec->m_iWidthPrevUVXExpPlusExp;

	U8_WMV* ppxliCodedPostY = ppxliPostY;
	U8_WMV* ppxliCodedPostU = ppxliPostU;
	U8_WMV* ppxliCodedPostV = ppxliPostV;


    U32_WMV uiNumMBX = pWMVDec->m_uiNumMBX;
    U32_WMV uiNumMBY = pWMVDec->m_uiNumMBY;

	U32_WMV iStepSize;
    I32_WMV iValidWidth;

	U32_WMV widthY, widthUV;
	I32_WMV imbY;
    I32_WMV i;

	//used in Color Conversion
    U8_WMV* pBmpPtr;
   DEBUG_PROFILE_FRAME_FUNCTION_COUNT(CombinePPLColorConvertPacked);

	widthY = pWMVDec->m_iWidthPrevY;
	widthUV = pWMVDec->m_iWidthPrevUV;

	assert(pWMVDec->m_uiNumMBX == pWMVDec->m_uintNumMBX);
	assert(pWMVDec->m_uiNumMBY == pWMVDec->m_uintNumMBY);
	assert(widthY == (U32_WMV)pWMVDec->m_iWidthInternal);
	assert(widthUV == (U32_WMV)pWMVDec->m_iWidthInternalUV);

#ifdef _ARM_
    pWMVDec->m_iXscalePrefechDistY = 32;
    pWMVDec->m_iXscalePrefechDistUV = 32;
#endif    

	//Prepare PostFilter Buffer
#if 1
	for (i = pWMVDec->m_iHeightUV; i > 0; i--) {
        ALIGNED32_FASTMEMCPY(ppxliCodedPostU, ppxliCurrQU, pWMVDec->m_iWidthUV * sizeof(I8_WMV));
        ppxliCodedPostU += widthUV;
        ppxliCurrQU += widthUV;
        ALIGNED32_FASTMEMCPY(ppxliCodedPostV, ppxliCurrQV, pWMVDec->m_iWidthUV * sizeof(I8_WMV));
        ppxliCodedPostV += widthUV;
        ppxliCurrQV += widthUV;
        ALIGNED32_FASTMEMCPY(ppxliCodedPostY, ppxliCurrQY, pWMVDec->m_iWidthY * sizeof(I8_WMV));
        ppxliCodedPostY += widthY;
        ppxliCurrQY += widthY;
        ALIGNED32_FASTMEMCPY(ppxliCodedPostY, ppxliCurrQY, pWMVDec->m_iWidthY * sizeof(I8_WMV));
        ppxliCodedPostY += widthY;
        ppxliCurrQY += widthY;
    }
#else
	for (i = pWMVDec->m_iHeightUV; i > 0; i--) {
        memcpy(ppxliCodedPostU, ppxliCurrQU, pWMVDec->m_iWidthUV * sizeof(I8_WMV));
        ppxliCodedPostU += widthUV;
        ppxliCurrQU += widthUV;
        memcpy(ppxliCodedPostV, ppxliCurrQV, pWMVDec->m_iWidthUV * sizeof(I8_WMV));
        ppxliCodedPostV += widthUV;
        ppxliCurrQV += widthUV;
        memcpy(ppxliCodedPostY, ppxliCurrQY, pWMVDec->m_iWidthY * sizeof(I8_WMV));
        ppxliCodedPostY += widthY;
        ppxliCurrQY += widthY;
        memcpy(ppxliCodedPostY, ppxliCurrQY, pWMVDec->m_iWidthY * sizeof(I8_WMV));
        ppxliCodedPostY += widthY;
        ppxliCurrQY += widthY;
    }

#endif
    assert (pWMVDec->m_bDeblockOn);


	// Prepare Color Conversion.

#ifdef NO_VIDEO_SEEK
    Bool_WMV bWMV2OrPostfilter = TRUE_WMV;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    if (pWMVDec->m_iPostProcessMode <= 0 || (pWMVDec->m_tFrmType == IVOP && pWMVDec->m_bXintra8)) {
        bWMV2OrPostfilter = m_bLoopFilter;
    }
#endif
    FUNCTION_PROFILE_DECL_START(fp,COLORCONVERTANDRENDER_PROFILE);

    pBmpPtr = pucDecodedOutput + pWMVDec->m_iBMPPointerStart;
	iStepSize = pWMVDec->m_iStepSize;
	iValidWidth = MB_SIZE - (widthY - pWMVDec->m_iFrmWidthSrc);

	// Do the Macro blocks for deblocking and color coonversion.
	for (imbY = uiNumMBY-1; imbY > 0; imbY--) 
	{
        U8_WMV* pBmpPtrMB = pBmpPtr;
		I32_WMV imbX;

		ppxliCodedPostY = ppxliPostY;
		ppxliCodedPostU = ppxliPostU;
		ppxliCodedPostV = ppxliPostV;


        // Filter the middle horizontal Y edge
        FilterEdge_Short(pWMVDec, ppxliCodedPostY+widthY * 8,widthY,1,MB_SIZE, iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge_Short(pWMVDec, ppxliCodedPostY + 8,1,widthY,MB_SIZE,iStepSize);     

		for (imbX = uiNumMBX-1; imbX > 0; imbX--) 
		{            

			FilterEdge_Short(pWMVDec, ppxliCodedPostY+widthY * 8+MB_SIZE,widthY,1,MB_SIZE,iStepSize);
			FilterEdge_Short(pWMVDec, ppxliCodedPostY + 8 + MB_SIZE,1,widthY,MB_SIZE,iStepSize);     
			FilterEdge_Short(pWMVDec, ppxliCodedPostY+MB_SIZE,1,widthY,MB_SIZE,iStepSize);
			FilterEdge_Short(pWMVDec, ppxliCodedPostY + (widthY * MB_SIZE),widthY,1,MB_SIZE,iStepSize);


			FilterEdge_Short(pWMVDec, ppxliCodedPostU+BLOCK_SIZE,1,widthUV,BLOCK_SIZE,iStepSize);
			FilterEdge_Short (pWMVDec, ppxliCodedPostU + (widthUV * BLOCK_SIZE), widthUV,1,BLOCK_SIZE,iStepSize);

			FilterEdge_Short(pWMVDec, ppxliCodedPostV+BLOCK_SIZE,1,widthUV,BLOCK_SIZE,iStepSize);
			FilterEdge_Short (pWMVDec, ppxliCodedPostV + (widthUV * BLOCK_SIZE), widthUV,1,BLOCK_SIZE,iStepSize);

#ifdef NO_VIDEO_SEEK
            if ((pmbmd->m_bSkip == FALSE_WMV) || bWMV2OrPostfilter) {
#endif
                    pWMVDec->m_pWMVideoDecUpdateDstMB (
                        pWMVDec,
                        pBmpPtrMB, 
                        ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
                        widthY, widthUV, 
                        pWMVDec->m_iWidthBMP);

#ifdef NO_VIDEO_SEEK
            }
            pmbmd++;
#endif
            pBmpPtrMB += pWMVDec->m_iBMPMBIncrement;

			ppxliCodedPostY += MB_SIZE;
			ppxliCodedPostU += BLOCK_SIZE;
			ppxliCodedPostV += BLOCK_SIZE;
		}

		// Filter the bottom Y, U and V edges.
		FilterEdge_Short (pWMVDec, ppxliCodedPostY + (widthY * MB_SIZE),widthY,1,MB_SIZE,iStepSize);
		FilterEdge_Short (pWMVDec, ppxliCodedPostU + (widthUV * BLOCK_SIZE), widthUV,1,BLOCK_SIZE,iStepSize);
		FilterEdge_Short (pWMVDec, ppxliCodedPostV + (widthUV * BLOCK_SIZE), widthUV,1,BLOCK_SIZE,iStepSize);

#ifdef NO_VIDEO_SEEK
            if ((pmbmd->m_bSkip == FALSE_WMV) || bWMV2OrPostfilter) {
#endif
        pWMVDec->m_pWMVideoDecUpdateDstPartialMB (
            pWMVDec,
            pBmpPtrMB, 
            ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
            widthY, widthUV, 
            iValidWidth, MB_SIZE);
#ifdef NO_VIDEO_SEEK
            }
            pmbmd++;
#endif

        ppxliPostY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliPostU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliPostV += pWMVDec->m_iBlkSizeXWidthPrevUV;

        pBmpPtr += pWMVDec->m_iBMPMBHeightIncrement;
	}

	{
        U8_WMV* pBmpPtrMB = pBmpPtr;
        I32_WMV iValidHeight = MB_SIZE - (abs(pWMVDec->m_iHeightY) - abs(pWMVDec ->m_iFrmHeightSrc));

		I32_WMV imbX;
		ppxliCodedPostY = ppxliPostY;
		ppxliCodedPostU = ppxliPostU;
		ppxliCodedPostV = ppxliPostV;

        // Filter the middle horizontal Y edge
        FilterEdge_Short(pWMVDec, ppxliCodedPostY+widthY * 8,widthY,1,MB_SIZE, iStepSize);

        // Filter the middle vertical Y edge
        FilterEdge_Short(pWMVDec, ppxliCodedPostY + 8,1,widthY,MB_SIZE,iStepSize);     

		for (imbX = uiNumMBX-1; imbX > 0; imbX--) 
		{            
			// Filter the middle horizontal Y edge
			FilterEdge_Short(pWMVDec, ppxliCodedPostY+widthY * 8+MB_SIZE,widthY,1,MB_SIZE,iStepSize);
			FilterEdge_Short(pWMVDec, ppxliCodedPostY + 8 + MB_SIZE,1,widthY,MB_SIZE,iStepSize);     
			FilterEdge_Short(pWMVDec, ppxliCodedPostY+MB_SIZE,1,widthY,MB_SIZE,iStepSize);

			FilterEdge_Short(pWMVDec, ppxliCodedPostU+BLOCK_SIZE,1,widthUV,BLOCK_SIZE,iStepSize);
			FilterEdge_Short(pWMVDec, ppxliCodedPostV+BLOCK_SIZE,1,widthUV,BLOCK_SIZE,iStepSize);


#ifdef NO_VIDEO_SEEK
            if ((pmbmd->m_bSkip == FALSE_WMV) || bWMV2OrPostfilter) {
#endif
        pWMVDec->m_pWMVideoDecUpdateDstPartialMB (
            pWMVDec,
            pBmpPtrMB, 
            ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
            widthY, widthUV, 
            MB_SIZE, iValidHeight);
#ifdef NO_VIDEO_SEEK
            }
            pmbmd++;
#endif

            pBmpPtrMB += pWMVDec->m_iBMPMBIncrement;

			ppxliCodedPostY += MB_SIZE;
			ppxliCodedPostU += BLOCK_SIZE;
			ppxliCodedPostV += BLOCK_SIZE;
		}

#ifdef NO_VIDEO_SEEK
            if ((pmbmd->m_bSkip == FALSE_WMV) || bWMV2OrPostfilter) {
#endif
        pWMVDec->m_pWMVideoDecUpdateDstPartialMB (
            pWMVDec,
            pBmpPtrMB, 
            ppxliCodedPostY, ppxliCodedPostU, ppxliCodedPostV,
            widthY, widthUV, 
            iValidWidth, iValidHeight);
#ifdef NO_VIDEO_SEEK
            }
            pmbmd++;
#endif
	}


    FUNCTION_PROFILE_STOP(&fp);
    return WMV_Succeeded;
}
#endif

#endif // combine PostFilter + ColorConversion

