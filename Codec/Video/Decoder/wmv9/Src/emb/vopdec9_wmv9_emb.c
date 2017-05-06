//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "limits.h"
#include "typedef.h"
#include "wmvdec_member.h"
#ifdef _WMV_TARGET_X86_
//#include "cpudetect.h"
#endif // _WMV_TARGET_X86_
#ifdef macintosh
#include "cpudetect_mac.h"
#include "motioncomp_altivec.h"
#endif
#include "tables_wmv.h"

#include "strmdec_wmv.h"
#include "motioncomp_wmv.h"
#include "idctdec_wmv2.h"
#include "repeatpad_wmv.h"
#include "postfilter_wmv.h"

#include "spatialpredictor_wmv.h"
#include "localhuffman_wmv.h"
#include "tables_wmv.h"
#include "wmvdec_function.h"

//#define DUMP_YUV_TEST

#ifdef DUMP_YUV_TEST
extern int frameCount;
extern int g_IsDumpFlag;
extern FILE *g_DumpYUVFile;
extern FILE *g_DumpYUVFile2;
extern int DumpOneFrame( VO_VIDEO_BUFFER *par, VO_VIDEO_OUTPUTINFO* outFormat, FILE *dumpYUVFile );
#endif

#ifdef _EMB_WMV3_

#ifdef VOARMV7
Void_WMV ARMV7_IntraBlockDequant8x8(I32_WMV iDCStepSize, I32_WMV iDoubleStepSize, I32_WMV iStepMinusStepIsEven, I16_WMV *rgiCoefRecon);
#endif

#define WMV_ESC_Decoding()                                                                 \
    if (pWMVDec->m_bFirstEscCodeInFrame){                                                  \
    decodeBitsOfESCCode (pWMVDec);                                                     \
    pWMVDec->m_bFirstEscCodeInFrame = FALSE_WMV;                                       \
    }                                                                                      \
    uiRun = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_RUN);              \
    if (BS_getBit ( pWMVDec->m_pbitstrmIn)) /* escape decoding */                          \
    iLevel = -1 * BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);  \
    else                                                                                   \
iLevel = BS_getBits ( pWMVDec->m_pbitstrmIn, pWMVDec->m_iNUMBITS_ESC_LEVEL);                      

#ifdef _EMB_SSIMD_IDCT_
//#pragma code_seg (EMBSEC_DEF)
Void_WMV EMB_InitZigZagTable_SSIMD(U8_WMV * pZigzag, U8_WMV * pNewZigZag, I32_WMV iNumEntries)
{
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(EMB_InitZigZagTable_SSIMD);
    
    if(pZigzag != NULL)
    {
        for(i = 0; i< iNumEntries; i++)
        {
            U8_WMV iLoc = pZigzag[i];
            
            iLoc = ((iLoc&~3)<<1)|(iLoc&3);
            
            pNewZigZag[i] = iLoc;
        }
    }
}

#endif

/*
static int  g_dbg_cnt0;
static int  g_dbg_cnt1;
static int  g_dbg_cnt2;
static int  g_dbg_cnt3;
*/

//U8_WMV g_iNumCoefTbl[] = { 64, 32, 32, 0, 16};

#if !defined(WMV_OPT_DQUANT_ARM)
Void_WMV ResetCoefBufferNull(I32_WMV * pBuffer)
{
    
}


//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer8x8(I32_WMV * pBuffer)
{

    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ResetCoefBuffer8x8);
    for( i = 0; i<32;i += 4)
    {
        pBuffer[i] = 0;
        pBuffer[i+1] = 0;
        pBuffer[i+2] = 0;
        pBuffer[i+3] = 0;
    }

}

//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer8x4(I32_WMV * pBuffer)
{
    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ResetCoefBuffer8x4);
    for( i = 0; i<32;i += 4)
    {
        pBuffer[i] = 0;
        pBuffer[i+1] = 0;
    }
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer4x8(I32_WMV * pBuffer)
{

    I32_WMV i;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ResetCoefBuffer4x8);
    for( i = 0; i<16;i += 4)
    {
        pBuffer[i] = 0;
        pBuffer[i+1] = 0;
        pBuffer[i+2] = 0;
        pBuffer[i+3] = 0;
    }

}
//#pragma code_seg (EMBSEC_PML)
Void_WMV ResetCoefBuffer4x4(I32_WMV * pBuffer)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ResetCoefBuffer4x4);
    pBuffer[0] = 0;
    pBuffer[0+1] = 0;
    pBuffer[0+4] = 0;
    pBuffer[0+5] = 0;
    pBuffer[0+8] = 0;
    pBuffer[0+1+8] = 0;
    pBuffer[0+4+8] = 0;
    pBuffer[0+5+8] = 0;
}


Void_WMV (*g_ResetCoefBuffer[5])(I32_WMV * pBuffer)
= { ResetCoefBuffer8x8, ResetCoefBuffer8x4, ResetCoefBuffer4x8, ResetCoefBufferNull, ResetCoefBuffer4x4};
#endif //WMV_OPT_DQUANT_ARM

U8_WMV g_IDCTShortCutLUT[] =
{
    0,0,0,0,0,0,0,0,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7,
        
};


//#pragma code_seg (EMBSEC_DEF)
tWMVDecodeStatus DecodeInverseInterBlockQuantize16_level2_EMB(
                                                              tWMVDecInternalMember *pWMVDec, 
                                                              CDCTTableInfo_Dec* InterDCTTableInfo_Dec,
                                                              U32_WMV uiNumCoefs,
                                                              U32_WMV * puiCoefCounter,
                                                              I32_WMV * pvalue,
                                                              I32_WMV * pSignBit,
                                                              U32_WMV * plIndex
                                                              )
{    
    
    Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec -> hufDCTACDec;
    U32_WMV uiTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;
    U32_WMV uiStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;

    I16_WMV * rgLevelRunAtIndx = InterDCTTableInfo_Dec -> combined_levelrun;
    U32_WMV lIndex;
    I32_WMV signBit = *pSignBit;
    U32_WMV uiRun;
    I32_WMV iLevel;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseInterBlockQuantize16_level2_EMB);
    
    if(signBit != 0) {
        // ESC + '1' + VLC
        lIndex = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
        if (lIndex == uiTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
            return WMV_CorruptedBits;
        }
        
        
        uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
        iLevel= rgLevelRunAtIndx[lIndex] >>8;
        
        if (lIndex >= uiStartIndxOfLastRun) {
            iLevel += (I8_WMV) InterDCTTableInfo_Dec -> puiLastNumOfLevelAtRun[uiRun];
        }
        else
            iLevel += (I8_WMV) InterDCTTableInfo_Dec -> puiNotLastNumOfLevelAtRun[uiRun];
        
        signBit = -  (I32_WMV)BS_getBit(pWMVDec->m_pbitstrmIn); 
    }
    else if (BS_getBit(pWMVDec->m_pbitstrmIn)){
        
        // ESC + '10' + VLC
        lIndex = Huffman_WMV_get(hufDCTACDec, pWMVDec->m_pbitstrmIn);
        if (lIndex == uiTCOEF_ESCAPE) { // ESC + 1 + ESC :  bits corrupted
            return WMV_CorruptedBits;
        }
        
        uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
        iLevel= rgLevelRunAtIndx[lIndex] >>8;
        if (lIndex >= uiStartIndxOfLastRun) {
            uiRun += (InterDCTTableInfo_Dec -> puiLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
        }
        else
            uiRun += (InterDCTTableInfo_Dec -> puiNotLastNumOfRunAtLevel[iLevel] + pWMVDec->m_iEscRunDiffV2V3);
        
        signBit = -  (I32_WMV)BS_getBit(pWMVDec->m_pbitstrmIn); 
    }
    else{
        // ESC + '00' + FLC
        //                    g_dbg_cnt3++;
        lIndex = uiStartIndxOfLastRun -1 + (Bool_WMV) BS_getBit(pWMVDec->m_pbitstrmIn); // escape decoding
        
        if (pWMVDec->m_cvCodecVersion >= WMV1) {  // == WMV1 || WMV2
            WMV_ESC_Decoding();
        }
        else {
            uiRun = BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_RUN);          
            iLevel = (I8_WMV)BS_getBits(pWMVDec->m_pbitstrmIn,NUMBITS_ESC_LEVEL);
        }
        
        signBit = ((I32_WMV) iLevel) >> 31;
        
        if(iLevel <0)
            iLevel = -iLevel;
    }
    
    (*puiCoefCounter) += uiRun;
    
    if (BS_invalid(pWMVDec->m_pbitstrmIn) || ((*puiCoefCounter) >= uiNumCoefs)) {
        return WMV_CorruptedBits;
    }
    
    *pvalue = iLevel;
    *pSignBit = signBit ;
    
    *plIndex = lIndex;
    
    return WMV_Succeeded;
}


//#pragma code_seg (EMBSEC_PML2)
I32_WMV DecodeIntra16_level1_EMB(
                                 Huffman_WMV* hufDCTACDec, 
                                 CInputBitStream_WMV * pBitStrm, 
                                 I16_WMV * rgLevelRunAtIndx,
                                 U8_WMV * pZigzagInv,
                                 I16_WMV * rgiCoefRecon,
                                 I32_WMV uiTCOEF_ESCAPE,
                                 U32_WMV uiNumCoefs,
                                 I32_WMV uiStartIndxOfLastRun,
                                 U32_WMV * puiCoefCounter,
                                 I32_WMV * pvalue,
                                 U32_WMV  * puiRun,
                                 I32_WMV * pSignBit,
                                 I32_WMV *piDCTHorzFlags
                                 )
{
    
    I32_WMV lIndex;
    Bool_WMV bDone;
    U32_WMV uiRun;
    I32_WMV iLevel;
    I32_WMV signBit;
    U32_WMV uiCoefCounter = *puiCoefCounter;
    I32_WMV iDCTHorzFlags = *piDCTHorzFlags;
    I32_WMV value;
    I32_WMV iIndex;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeIntra16_level1_EMB);
    
    do {
        //lIndex = Huffman_WMV_get_peek(hufDCTACDec, pBitStrm, &signBit);
        lIndex = Huffman_WMV_get(hufDCTACDec, pBitStrm);
        signBit = - (I32_WMV)BS_getBit(pBitStrm);
        
        bDone = ( lIndex == uiTCOEF_ESCAPE);
        
        
        uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
        iLevel= rgLevelRunAtIndx[lIndex] >>8;
        
        uiCoefCounter += uiRun;
        
        bDone |= (uiCoefCounter >= uiNumCoefs);
        // bDone |= BS_invalid(pWMVDec->m_pbitstrmIn) ;
        bDone |= ( lIndex >= uiStartIndxOfLastRun);
        iIndex =  pZigzagInv [uiCoefCounter];
        
        iDCTHorzFlags |=  g_IDCTShortCutLUT[iIndex];
        
        value = ( iLevel ^ signBit) - signBit;
        
        ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;
        
        uiCoefCounter++;
    } while (bDone == 0) ; //!bIsLastRun);
    
    
    *puiCoefCounter = uiCoefCounter;
    *pvalue  = value;
    *puiRun = uiRun;
    *pSignBit = signBit;
    *piDCTHorzFlags = iDCTHorzFlags;
    
    return lIndex;
}




/*
static int g_dbg_cnt5;
static int g_dbg_cnt6;
*/
#if !defined(WMV_OPT_DQUANT_ARM)
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus DecodeInverseInterBlockQuantize16_EMB(EMB_PBMainLoop* pMainLoop, 
                                                       CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, 
                                                       I32_WMV XFormType,
                                                       DQuantDecParam *pDQ)
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    CDCTTableInfo_Dec* InterDCTTableInfo_Dec = ppInterDCTTableInfo_Dec[0];
    Huffman_WMV* hufDCTACDec = InterDCTTableInfo_Dec -> hufDCTACDec;
    I16_WMV * rgLevelRunAtIndx = InterDCTTableInfo_Dec -> combined_levelrun;
    // I8_WMV* rgLevelAtIndx = InterDCTTableInfo_Dec -> pcLevelAtIndx;
    // U8_WMV* rgRunAtIndx = InterDCTTableInfo_Dec -> puiRunAtIndx;
    I32_WMV uiStartIndxOfLastRun = InterDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
    I32_WMV  uiTCOEF_ESCAPE = InterDCTTableInfo_Dec -> iTcoef_ESCAPE;
    U32_WMV uiRun; // = 0;
    I32_WMV  iLevel; // = 0;
    U32_WMV uiCoefCounter = 0;
    I32_WMV signBit;
    I32_WMV value;
    I32_WMV iDCTHorzFlags;
    I32_WMV iIndex;
    I16_WMV *rgiCoefRecon = (I16_WMV*)pMainLoop->m_rgiCoefReconBuf;
    I32_WMV lIndex;
    U32_WMV uiNumCoefs = pMainLoop->m_iNumCoefTbl[XFormType];    
    Bool_WMV bNotDone;
    tWMVDecodeStatus result;
    U8_WMV *pZigzagInv; 
 
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseInterBlockQuantize16_EMB);

    //uiNumCoefs = 64;

    if (XFormType == XFORMMODE_8x8)
        pZigzagInv = pMainLoop->m_pZigzagScanOrder;
    else
        pZigzagInv = pMainLoop->m_pZigzag[XFormType-1];
    
    g_ResetCoefBuffer[XFormType] ((I32_WMV *)rgiCoefRecon);
    
    // memset (rgiCoefRecon, 0, 128*sizeof(I16_WMV));
    iDCTHorzFlags = 0;
    
    do {
        
        do {
            lIndex = Huffman_WMV_get(hufDCTACDec, pMainLoop->m_pbitstrmIn);
            signBit = - (I32_WMV)BS_getBit(pMainLoop->m_pbitstrmIn); 
            //lIndex = Huffman_WMV_get_peek(hufDCTACDec, pWMVDec->m_pbitstrmIn, &signBit);
            
            bNotDone = ( lIndex != uiTCOEF_ESCAPE);
            //   uiRun = rgRunAtIndx[lIndex];
            //    iLevel = rgLevelAtIndx[lIndex];
            
            uiRun= (U8_WMV)rgLevelRunAtIndx[lIndex];
            iLevel= rgLevelRunAtIndx[lIndex] >>8;
            
            uiCoefCounter += uiRun;
            
            bNotDone &= (uiCoefCounter < uiNumCoefs);
            // bDone |= BS_invalid(pWMVDec->m_pbitstrmIn) ;
            bNotDone &= ( lIndex < uiStartIndxOfLastRun);
            iIndex =  pZigzagInv [uiCoefCounter];
            
            iDCTHorzFlags |=  pMainLoop->m_IDCTShortCutLUT[iIndex];
            
            value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;

#ifdef _EMB_IDCT_SSIMD64_
            {
                I64_WMV rem = iIndex & 3;
                *(I64_WMV *)( rgiCoefRecon + (iIndex-rem)) += (I64_WMV)value << (rem<<4);
            }

#elif defined(_EMB_IDCT_SSIMD32_) && !defined(_EMB_HYBRID_16_32_IDCT_)
            if((iIndex&1)== 0) {
                *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value;
            }
            else {
#ifndef _BIG_ENDIAN_
                ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;
//                *(I32_WMV *)( rgiCoefRecon + (iIndex-1)) += (I32_WMV)value << 16;
#else
                //*(I32_WMV *)(rgiCoefRecon + (iIndex>>1)) += (I32_WMV)value;
                *(I32_WMV *)( rgiCoefRecon + (iIndex-1)) += (I32_WMV)value << 16;
#endif //_BIG_ENDIAN_
            }
#else
            ( *(I16_WMV *)( rgiCoefRecon + iIndex)) = (I16_WMV)value;
#endif
            
            uiCoefCounter++;
        } while (bNotDone) ; //!bIsLastRun);
        
        uiCoefCounter--;
        
        if (BS_invalid(pMainLoop->m_pbitstrmIn) | (uiCoefCounter >= uiNumCoefs)) {
            return WMV_CorruptedBits;
        }
        if(lIndex != uiTCOEF_ESCAPE) {
            assert(lIndex >=  uiStartIndxOfLastRun);
            break;
        }
        
        //undo the damage first
        
        if (XFormType < 8)
        {
#ifdef _EMB_IDCT_SSIMD64_
            I64_WMV rem;

            iIndex = pZigzagInv [uiCoefCounter&63];
            rem = iIndex & 3;
            (*(I64_WMV *)(rgiCoefRecon + iIndex - rem)) -= (I64_WMV)value << (rem << 4);
#elif defined(_EMB_IDCT_SSIMD32_) && !defined(_EMB_HYBRID_16_32_IDCT_)
//        if(XFormType < 8 && (pZigzagInv [uiCoefCounter]&1)== 0) {
            if((pZigzagInv [uiCoefCounter]&1)== 0) {
                *(I32_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] ) -= (I32_WMV)value;
            }
            else {
#ifndef _BIG_ENDIAN_            
                ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value;
#else
                ( *(I32_WMV *)( rgiCoefRecon + (pZigzagInv [uiCoefCounter&63]-1) )) -= (I32_WMV)value << 16;
#endif //_BIG_ENDIAN_
            }
#else
        ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)value;
#endif
        }
       
        uiCoefCounter -= (U8_WMV)rgLevelRunAtIndx[lIndex];
        
        //uiCoefCounter -= uiRun;
        //g_dbg_cnt6++;
        // assert(signBit ==( -(value < 0)));
        // signBit = -(value < 0);
        
        result = DecodeInverseInterBlockQuantize16_level2_EMB(pMainLoop->m_pWMVDec, 
            InterDCTTableInfo_Dec,
            uiNumCoefs,
            &uiCoefCounter,
            &iLevel,
            &signBit,
            (U32_WMV*)&lIndex
            );
        
        if(result != WMV_Succeeded)
            return result;
        
        // generate sign mask 0xFFFFFFFF (s=0) or 0x00000000 (s=1)
        //            int signBit = iLevel>>31;
        // signBit = ~(iSign -1); // 0 or 0xffffffff
        //            I32_WMV value = iDoubleStepSize * iLevel + ((iStepMinusStepIsEven ^ signBit) - signBit);
        
        iIndex =  pZigzagInv [uiCoefCounter];
        iDCTHorzFlags |=  pMainLoop->m_IDCTShortCutLUT[iIndex];
        
        value = ((iDoubleStepSize * iLevel + iStepMinusStepIsEven) ^ signBit) - signBit;
        
        // we have a conditional negative depending on the sign bit. If 
        // sign == -1 or 1, we want 
        // output = sign * (iDoubleStepSize * iLevel + iStepMinusStepIsEven)
        // however, sign is 0 or 1.  note we essentially have a conditional negative.
        // recall -X = X' + 1 
        // we generate a mask from the sign bit
        
#ifdef _EMB_IDCT_SSIMD64_
        {
            I64_WMV rem = iIndex & 3;
            *(I64_WMV *)( rgiCoefRecon + (iIndex-rem)) += (I64_WMV)value << (rem<<4);
        }
#elif defined(_EMB_IDCT_SSIMD32_) && !defined(_EMB_HYBRID_16_32_IDCT_)
        if((iIndex&1)== 0) {
            *(I32_WMV *)( rgiCoefRecon + iIndex ) += (I32_WMV)value;
        }
        else {
#ifndef _BIG_ENDIAN_
            ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)value;
#else
            *(I32_WMV *)( rgiCoefRecon + (iIndex-1)) += (I32_WMV)value << 16;
#endif  //_BIG_ENDIAN_
        }
#else
        ( *(I16_WMV *)( rgiCoefRecon + iIndex)) = (I16_WMV)value;
#endif

        uiCoefCounter++;
    } while (lIndex < uiStartIndxOfLastRun) ; //!bIsLastRun);
    
    // Save the DCT row flags. This will be passed to the IDCT routine
    pMainLoop->m_iDCTHorzFlags = iDCTHorzFlags;
    
    assert(pMainLoop->m_pWMVDec->m_b16bitXform == TRUE_WMV);

    return WMV_Succeeded;
}
#endif //WMV_OPT_DQUANT_ARM

// static int g_dbg_cnt_error;
// static int g_dbg_cnt_noerror;
//static int g_dbg_cnt;
//#pragma code_seg (EMBSEC_PML)
#ifdef COMBINE_2MC_TO_1MC
Void_WMV g_MotionComp_X9_EMB (EMB_PMainLoop *pPMainLoop,  I32_WMV iblk, Bool_WMV b1MV, Bool_WMV bForce4MV)
#else
Void_WMV g_MotionComp_X9_EMB (EMB_PMainLoop *pPMainLoop,  I32_WMV iblk, Bool_WMV b1MV)
#endif
{
    I32_WMV iUV = iblk>>2;
    I32_WMV  iFilterType = pPMainLoop->PB.m_iFilterType[iUV];
    U8_WMV  *pRef;
    CDiffMV_EMB * pDiffMV = pPMainLoop->PB.m_pDiffMV_EMB + iblk;
    // I32_WMV  k = (iBlockY * pWMVDec->m_uintNumMBX << 1) + iBlockX;
    I32_WMV  iShiftX;
    I32_WMV  iShiftY;

    I32_WMV iXFrac, iYFrac;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(g_MotionComp_X9_EMB);

#ifdef COMBINE_2MC_TO_1MC
	if( bForce4MV && !iUV ) {  //In fact it is 1mv,but we take it as 4mv.
		pDiffMV -= iblk;
		b1MV = 0;
	}
#endif

	iShiftX = pDiffMV->Diff.I16[X_INDEX];
	iShiftY = pDiffMV->Diff.I16[Y_INDEX];

    pRef = pPMainLoop->PB.m_ppxliRefBlkBase[iblk] + pPMainLoop->PB.m_iFrmMBOffset[iUV];

    pRef = pRef + (iShiftY >> 2) * pPMainLoop->PB.m_iWidthPrev[iUV] + (iShiftX >> 2);
    
    iXFrac = iShiftX&3;
    iYFrac = iShiftY&3;
    
    //g_dbg_cnt++;

    if(iFilterType == FILTER_BICUBIC)
    {
        I_SIMD ret;

		//sw: src:pRef, dst:m_rgMotionCompBuffer_EMB
	#ifdef COMBINE_2MC_TO_1MC
		if( b1MV ) {
			ret = (*pPMainLoop->PB.m_pInterpolateBicubic[iXFrac][iYFrac]) 
					(pRef, pPMainLoop->PB.m_iWidthPrev[iUV], (U8_WMV *)pPMainLoop->m_rgMotionCompBuffer_EMB, (EMB_PBMainLoop *)pPMainLoop, iXFrac, iYFrac, b1MV, pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV]);
		}
		else {
			ret = (*pPMainLoop->PB.m_pInterpolateBicubic[iXFrac][iYFrac]) 
					(pRef, pPMainLoop->PB.m_iWidthPrev[iUV], (U8_WMV *)pPMainLoop->m_rgMotionCompBuffer_EMB, (EMB_PBMainLoop *)pPMainLoop, iXFrac, iYFrac, b1MV, pPMainLoop->PB.m_rgiNewVertTbl_2[iUV][b1MV]);
		}
	#else
        ret = (*pPMainLoop->PB.m_pInterpolateBicubic[iXFrac][iYFrac]) 
                (pRef, pPMainLoop->PB.m_iWidthPrev[iUV], (U8_WMV *)pPMainLoop->m_rgMotionCompBuffer_EMB, (EMB_PBMainLoop *)pPMainLoop, iXFrac, iYFrac, b1MV, pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV]);
	#endif
#       ifdef _EMB_SSIMD_MC_
            if(ret) {
                g_InterpolateBlockBicubicOverflow_EMB(pRef, pPMainLoop->PB.m_iWidthPrev[iUV], (U8_WMV *)pPMainLoop->m_rgMotionCompBuffer_EMB,  iXFrac,  iYFrac,  pPMainLoop->PB.m_iRndCtrl, b1MV);
            }
#       endif
    }
    else
    {
        if(iXFrac|iYFrac) {
            pPMainLoop->PB.m_pInterpolateBlockBilinear[iXFrac][iYFrac]( pRef, pPMainLoop->PB.m_iWidthPrev[iUV], (U8_WMV *)pPMainLoop->m_rgMotionCompBuffer_EMB, iXFrac, iYFrac, pPMainLoop->PB.m_iRndCtrl, b1MV);         
        }
        else
            pPMainLoop->PB.m_pInterpolateBlock_00(pRef, pPMainLoop->PB.m_iWidthPrev[iUV], (U8_WMV *)pPMainLoop->m_rgMotionCompBuffer_EMB, (EMB_PBMainLoop *)pPMainLoop, iXFrac, iYFrac,  b1MV, 0);
    }

}



#define INTRADCYTCOEF_ESCAPE_MSV 119                            // see table.13/H.263
#define INTRADCUVTCOEF_ESCAPE_MSV 119    
// see table.13/H.263

//#pragma code_seg (EMBSEC_PML2)
//This is not really an MMX routine but works along side the MMX idct
tWMVDecodeStatus DecodeInverseIntraBlockX9_EMB(tWMVDecInternalMember *pWMVDec,
                                               CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                               I32_WMV iblk,   U8_WMV *pZigzagInv,   Bool_WMV bResidual,
                                               I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                                               I16_WMV   *pIntra, CWMVMBMode* pmbmd,
                                               DQuantDecParam *pDQ
                                               )
{
    I32_WMV iDoubleStepSize = pDQ->iDoubleStepSize;
    I32_WMV iStepMinusStepIsEven = pDQ->iStepMinusStepIsEven;
    I32_WMV iDCStepSize = pDQ->iDCStepSize;
    I32_WMV iValue0, iValue1;
    //Align on cache line
    I16_WMV *rgiCoefRecon = (I16_WMV*) pWMVDec->m_rgiCoefRecon;
    I32_WMV i;
    tWMVDecodeStatus result;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeInverseIntraBlockX9_EMB);
    
    memset (rgiCoefRecon, 0, (size_t) (BLOCK_SQUARE_SIZE_2>>1));
    
    // g_dbg_cnt_intra_total++;
    
    if (iblk < 4)
        decodeIntraDCAcPredMSV (pWMVDec, &result, pWMVDec->m_pHufDCTDCyDec, INTRADCYTCOEF_ESCAPE_MSV, iDCStepSize);
    else 
        decodeIntraDCAcPredMSV (pWMVDec, &result, pWMVDec->m_pHufDCTDCcDec, INTRADCUVTCOEF_ESCAPE_MSV, iDCStepSize);
    // decodeIntraDCAcPredMSV sets pWMVDec->m_rgiCoefRecon[0] be to the DC component
    // but pWMVDec->m_rgiCoefRecon is I32_WMV array and we are using it as a 16-bit array
    // through rgiCoefRecon,  so we need to zero out rgiCoefRecon[1], also,
    // i set rgiCoefRecon[0] by typecasting just in case some other endian order   
    // is used...  
    
    pWMVDec->m_iDCTHorzFlags = 0;
    
    rgiCoefRecon[0] = (I16_WMV) pWMVDec->m_rgiCoefRecon[0];
    rgiCoefRecon[1] = 0;  
    
    if (WMV_Succeeded != result) {
        return result;
    }
    
    if (bResidual) {
        CDCTTableInfo_Dec* IntraDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec[0];         
        Huffman_WMV* hufDCTACDec = IntraDCTTableInfo_Dec -> hufDCTACDec;
        
        I16_WMV * rgLevelRunAtIndx = IntraDCTTableInfo_Dec -> combined_levelrun;
        
        U32_WMV uiStartIndxOfLastRun = IntraDCTTableInfo_Dec -> iStartIndxOfLastRunMinus1 + 1;
        U32_WMV uiTCOEF_ESCAPE = IntraDCTTableInfo_Dec -> iTcoef_ESCAPE;
        U32_WMV uiNumCoefs = 64;
        U32_WMV uiRun; // = 0;
        I32_WMV iLevel; // = 0;
        U32_WMV uiCoefCounter = 1;
        I32_WMV iDCTHorzFlags = 0;
        I32_WMV signBit;
        I32_WMV iIndex;
        U32_WMV lIndex;
        
        // g_dbg_cnt_intra_residual++;
        //        FUNCTION_PROFILE_DECL_START(fpDecode,DECINVIBQUANTESCCODE_PROFILE );
        
        do {
            lIndex = DecodeIntra16_level1_EMB( hufDCTACDec, 
                pWMVDec->m_pbitstrmIn, 
                rgLevelRunAtIndx, 
                pZigzagInv,
                rgiCoefRecon,
                uiTCOEF_ESCAPE,
                uiNumCoefs,
                uiStartIndxOfLastRun,
                &uiCoefCounter,
                &iLevel,
                &uiRun,
                &signBit,
                &iDCTHorzFlags
                );
            
            uiCoefCounter--;
            
            if (BS_invalid(pWMVDec->m_pbitstrmIn) | (uiCoefCounter >= uiNumCoefs)) {
                return WMV_CorruptedBits;
            }
            if(lIndex != uiTCOEF_ESCAPE) {
                assert(lIndex >=  uiStartIndxOfLastRun);
                break;
            }
            
            //undo the damage first
            
            ( *(I16_WMV *)( rgiCoefRecon + pZigzagInv [uiCoefCounter&63] )) -= (I16_WMV)iLevel;
            
            uiCoefCounter -= uiRun;
            
            //g_dbg_cnt6++;
            
            result = DecodeInverseInterBlockQuantize16_level2_EMB(pWMVDec, 
                IntraDCTTableInfo_Dec,
                uiNumCoefs,
                &uiCoefCounter,
                &iLevel,
                &signBit,
                &lIndex
                );
            
            if(result != WMV_Succeeded)
                return result;
            
            // generate sign mask 0xFFFFFFFF (s=0) or 0x00000000 (s=1)
            //            int signBit = iLevel>>31;
            // signBit = ~(iSign -1); // 0 or 0xffffffff
            //            I32_WMV value = iDoubleStepSize * iLevel + ((iStepMinusStepIsEven ^ signBit) - signBit);
            
            iIndex =  pZigzagInv [uiCoefCounter];
            iDCTHorzFlags |=  g_IDCTShortCutLUT[iIndex];
            
            iLevel = ((iLevel ) ^ signBit) - signBit;
            
            // we have a conditional negative depending on the sign bit. If 
            // sign == -1 or 1, we want 
            // output = sign * (iDoubleStepSize * iLevel + iStepMinusStepIsEven)
            // however, sign is 0 or 1.  note we essentially have a conditional negative.
            // recall -X = X' + 1 
            // we generate a mask from the sign bit
            
            ( *(I16_WMV *)( rgiCoefRecon + iIndex)) += (I16_WMV)iLevel;
            
            uiCoefCounter++;
        } while (lIndex < uiStartIndxOfLastRun) ; //!bIsLastRun);
        // Save the DCT row flags. This will be passed to the IDCT routine
        pWMVDec->m_iDCTHorzFlags = iDCTHorzFlags;
        
        /*
        {
        int num =  numNonZeroDBits(iDCTHorzFlags);
        
          assert(num <= 4);
          g_dbg_idctSC_intra += 4 - num;
          g_dbg_idctNSC_intra += num;
          }
        */
    }
    
    // add prediction of dct-ac
    if (pPred) {
        // g_dbg_cnt_intra_pred++;
        
        if (iDirection) {
            pPred += pWMVDec->m_iACRowPredOffset;
        }    
        else { // left
            pPred += pWMVDec->m_iACColPredOffset;
        }    
        
        rgiCoefRecon[0] += pPred[0];
        if (iShift != -1) {           
            I32_WMV i;
            
            assert(iShift == 3 || iShift == 0);
            for ( i = 1; i < 8; i++)
                rgiCoefRecon[i << iShift] += pPred[i];
            
            pWMVDec->m_iDCTHorzFlags |= 1;
        }
    }
    
    for ( i = 0; i < 8; i++) {
        pIntra[i] = rgiCoefRecon[i];
        pIntra[i + 8] = rgiCoefRecon[i << 3];
    }
    
    // Dequantize DCT-AC
    assert( pWMVDec->m_b16bitXform != 0);
    
    // Dequantize DCT-DC
#if 0  // VOARMV7
	//Disable this v7 optimization now,because there is bug. For example,file mml0057_352x288.rcv .
	ARMV7_IntraBlockDequant8x8(iDCStepSize,iDoubleStepSize,iStepMinusStepIsEven,rgiCoefRecon);
#else    
	{
		iValue0 = (rgiCoefRecon[0] * iDCStepSize);
		iValue1 = rgiCoefRecon[1];
		if(iValue1) {
			int signMask = iValue1 >> 31;
			iValue1 = iDoubleStepSize * iValue1+ (signMask ^ iStepMinusStepIsEven) - signMask;
		}
	#ifdef _EMB_IDCT_SSIMD64_
		{
			I64_WMV iValue;
			iValue = iValue0 + (iValue1<<16);
	    
			for (i = 2; i < 64; i += 2) {
				iValue0 = rgiCoefRecon[i];
				if (iValue0) {
					int signMask = iValue0 >> 31; // 0 or FFFFFFFF
					iValue0 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue0 + (signMask ^ iStepMinusStepIsEven) - signMask);
				}
				iValue1 = rgiCoefRecon[i+1];
				if (iValue1) {
					int signMask = iValue1 >> 31; // 0 or FFFFFFFF
					iValue1 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue1 + (signMask ^ iStepMinusStepIsEven) - signMask);
				}
	        
				if (i & 3)
					*(I64_WMV *) (rgiCoefRecon + (i-2)) = iValue + ((I64_WMV)iValue0<<32) + ((I64_WMV)iValue1<<48);
				else
					iValue = iValue0 + (iValue1<<16);
			}  
		}
	#elif defined(_EMB_IDCT_SSIMD32_) && !defined(_EMB_HYBRID_16_32_IDCT_)
		*(I32_WMV *) rgiCoefRecon = iValue0 + (iValue1<<16);
		for (i = 2; i < 64; i += 2) {
			iValue0 = rgiCoefRecon[i];
			if (iValue0) {
				int signMask = iValue0 >> 31; // 0 or FFFFFFFF
				iValue0 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue0 + (signMask ^ iStepMinusStepIsEven) - signMask);
			}
			iValue1 = rgiCoefRecon[i+1];
			if (iValue1) {
				int signMask = iValue1 >> 31; // 0 or FFFFFFFF
				iValue1 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue1 + (signMask ^ iStepMinusStepIsEven) - signMask);
			}
	        
			*(I32_WMV *) (rgiCoefRecon + i) = iValue0 + (iValue1<<16);
		}

	#else
		rgiCoefRecon[0] = iValue0;
		rgiCoefRecon[1] = iValue1;

		for (i = 2; i < 64; i++) {
			iValue0 = rgiCoefRecon[i];
			if (iValue0) {
				int signMask = iValue0 >> 31; // 0 or FFFFFFFF
				iValue0 = (I16_WMV) ((I32_WMV) iDoubleStepSize * iValue0 + (signMask ^ iStepMinusStepIsEven) - signMask);
			}
			rgiCoefRecon[i] = iValue0;
		}

	#endif
	}
#endif	//VOARMV7
    // DCT flags are set to all on
    //pWMVDec->m_iDCTHorzFlags = 0xff;
    
    return WMV_Succeeded;
}

//static int g_dbg_cnt = 0;

// static int g_dbg_cnt_total;
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus DecodeMB_X9_Fast (EMB_PMainLoop  * pPMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY,
                                   Bool_WMV b1MV
                                   )
{   
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec;
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec;
    tWMVDecodeStatus result = WMV_Succeeded;
    I32_WMV iblk;
    Bool_WMV bSkipMB = TRUE_WMV;
    const I32_WMV iCurrMB = pPMainLoop->PB.m_rgiCurrBaseBK[1];
    I32_WMV i1MVCnt = b1MV;
    CDiffMV_EMB * pDiffMV = pPMainLoop->PB.m_pDiffMV_EMB;
    Bool_WMV bFirstBlock =  pPMainLoop->PB.m_bMBXformSwitching;
    U32_WMV iSubblockPattern = pmbmd->m_iSubblockPattern;
    I32_WMV iXformType = pPMainLoop->PB.m_iFrameXformMode;
    DQuantDecParam *pDQ = &pPMainLoop->PB.m_prgDQuantParam [pmbmd->m_iQP ];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(DecodeMB_X9_Fast);
    
    //    FUNCTION_PROFILE_DECL_START(fpDecode,DECODEPMB_PROFILE);
    
    if (pPMainLoop->PB.m_bMBXformSwitching)
        iXformType = pmbmd->m_iMBXformMode;
    if (pPMainLoop->PB.m_bDCTTable_MB) {
        ppInterDCTTableInfo_Dec = &(pPMainLoop->PB.m_pInterDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
        ppIntraDCTTableInfo_Dec = &(pPMainLoop->PB.m_pIntraDCTTableInfo_Dec_Set[pmbmd->m_iDCTTable_MB_Index]);
    }
    else {
        ppInterDCTTableInfo_Dec = pPMainLoop->PB.m_ppInterDCTTableInfo_Dec;
        ppIntraDCTTableInfo_Dec = pPMainLoop->PB.m_ppIntraDCTTableInfo_Dec;
    }

    pPMainLoop->PB.m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_BK;

#ifndef COMBINE_2MC_TO_1MC
    if( !pDiffMV->iIntra && i1MVCnt == 1) {
        g_MotionComp_X9_EMB (pPMainLoop,  0, 1);
        i1MVCnt += b1MV;
        pPMainLoop->PB.m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_1MV;
    }
#endif

    for (iblk = 0; iblk < 6; iblk++) {
#ifndef _WMV9AP_
        U8_WMV rgCBP = pmbmd->m_rgbCodedBlockPattern;
#endif
        U8_WMV * rgCBP2 = pmbmd->m_rgbCodedBlockPattern2;
        UnionBuffer * ppxliErrorQMB;
        I32_WMV iIntra = 0, iShift, iDirection;
        I16_WMV  *pIntra, pPredScaled[16];
        U8_WMV * pDst;
        I32_WMV iUV;
#ifndef _WMV9AP_
        Bool_WMV bResidual = (rgCBP>>iblk)&0x1;
        rgCBP2[iblk] =  (U8_WMV)bResidual ;
#else
        Bool_WMV bResidual = rgCBP2[iblk];
#endif

		iUV = iblk >> 2;
#ifndef COMBINE_2MC_TO_1MC
        i1MVCnt -= iUV;
#endif

        iIntra = pDiffMV->iIntra;
        pDiffMV++;
        pDst = pPMainLoop->PB.m_ppxliCurrBlkBase[iblk] + pPMainLoop->PB.m_iFrmMBOffset[iUV];
        if (!iUV &&(iCurrMB||iblk || iIntra || bResidual ))
            bSkipMB = FALSE_WMV;
        
        if (iIntra) {
            I32_WMV iX, iY;
            Bool_WMV bDCACPredOn;
            I16_WMV *pPred = NULL_WMV;
            CDCTTableInfo_Dec** ppDCTTableInfo_Dec;
            
            if(!iUV)
            {
                iX = (iMBX << 1) + (iblk & 1);
                iY = (iMBY << 1) + ((iblk & 2) >> 1);
                pIntra = pPMainLoop->PB.m_pX9dct + ((pPMainLoop->PB.m_rgiCurrBaseBK[0] + pPMainLoop->PB.m_rgiBlkIdx[iblk]) << 4);
                ppDCTTableInfo_Dec = ppIntraDCTTableInfo_Dec;
            }
            else
            {
                bSkipMB = FALSE_WMV;
                iX = iMBX; iY = iMBY;
                pIntra = pPMainLoop->PB.m_pX9dctUV[iblk-4] + ((pPMainLoop->PB.m_rgiCurrBaseBK[1]) << 4);
                ppDCTTableInfo_Dec = ppInterDCTTableInfo_Dec;
            }
            
            bDCACPredOn = decodeDCTPrediction_EMB (pPMainLoop, pmbmd, pIntra, iblk, iX, iY, &iShift, &iDirection, pPredScaled);
            
            ppxliErrorQMB = pPMainLoop->PB.m_rgiCoefReconBuf;
            if (bDCACPredOn) 
                pPred = pPredScaled;
            result = (tWMVDecodeStatus)(result | DecodeInverseIntraBlockX9_EMB (pPMainLoop->PB.m_pWMVDec,
                                        ppDCTTableInfo_Dec,
                                        iblk,
                                        pPMainLoop->PB.m_pZigzagScanOrder, bResidual, pPred,
                                        iDirection, iShift, pIntra, pmbmd, pDQ)); //get the quantized block
            pPMainLoop->PB.m_iDCTHorzFlags= pPMainLoop->PB.m_pWMVDec->m_iDCTHorzFlags;
            assert(pPMainLoop->PB.m_pWMVDec->m_pIntraX9IDCT_Dec == g_IDCTDec16_WMV3);
            pPMainLoop->PB.m_pIDCTDec16_WMV3( ppxliErrorQMB, ppxliErrorQMB, BLOCK_SIZE, pPMainLoop->PB.m_iDCTHorzFlags );

#ifdef _EMB_SSIMD_MC_
/*Should deal with the same situation here when in SSIMDxxx */
#ifdef _EMB_SSIMD64_
            {
                I64_WMV ii;
                U64_WMV  * pData64 = (U64_WMV  *) pPMainLoop->m_rgMotionCompBuffer_EMB;
                for ( ii = 0; ii < 8; ii++)
                {
//                    pData64[0] = pData64[1]  = pData64[3] = pData64[4]  = 0x0080008000800080;
                    pData64[0] = pData64[3] = 0x0080008000800080;
                    pData64 += 6;
                }
            }

#elif  defined(_EMB_SSIMD32_)
            {
                U32_WMV  * pData32 = pPMainLoop->m_rgMotionCompBuffer_EMB;
                const U32_WMV uiData = 0x00800080;
                pData32[0]  = pData32[1]   = pData32[5]  = pData32[6]   = uiData;
                pData32[10] = pData32[11]  = pData32[15] = pData32[16]  = uiData;
                pData32[20] = pData32[21]  = pData32[25] = pData32[26]  = uiData;
                pData32[30] = pData32[31]  = pData32[35] = pData32[36]  = uiData;
                pData32[40] = pData32[41]  = pData32[45] = pData32[46]  = uiData;
                pData32[50] = pData32[51]  = pData32[55] = pData32[56]  = uiData;
                pData32[60] = pData32[61]  = pData32[65] = pData32[66]  = uiData;
                pData32[70] = pData32[71]  = pData32[75] = pData32[76]  = uiData;
                DEBUG_CACHE_WRITE_BYTES(pData32,80);
            }
#endif
#else
            {
                I32_WMV ii;
                U32_WMV  * pData32 = pPMainLoop->m_rgMotionCompBuffer_EMB;
                for ( ii = 0; ii < 8; ii++)
                {
                    pData32[0] = pData32[1]  = pData32[2] = pData32[3]  = 0x00800080;
                    DEBUG_CACHE_WRITE_BYTES(pData32,16);
                    pData32 += 10;
                }
            }
#endif //_EMB_SSIMD_MC_

            pPMainLoop->PB.m_pAddError(pDst, pPMainLoop->m_rgMotionCompBuffer_EMB + pPMainLoop->PB.m_pSSIMD_DstOffSet[iblk], ppxliErrorQMB->i32, pPMainLoop->PB.m_iWidthPrev[iUV]);
            if (pPMainLoop->PB.m_iOverlap) {
                I32_WMV  iWidth1 = (I32_WMV) pPMainLoop->PB.m_iWidthY>>iUV;
                U16_WMV * pDst1 = (U16_WMV*) (pPMainLoop->PB.m_pIntraBlockRow[iblk] + ((((iMBX << 4) + ((iblk & 1) << 3))>>iUV)&~0x7));
                U16_WMV * pSrc = (U16_WMV*)ppxliErrorQMB->i16;

#if defined(_EMB_SSIMD_MC_) && defined (WMV_OPT_COMMON_ARM)
                memcpyOddEven_arm (pDst1, pSrc, iWidth1);
#elif defined (_EMB_SSIMD_MC_)
                I32_WMV i1;
                DEBUG_CACHE_READ_BYTES(pSrc,64);
                for ( i1 = 0; i1 < 8; i1++) {
                    pDst1[0] = pSrc[0];
                    pDst1[2] = pSrc[1];
                    pDst1[4] = pSrc[2];
                    pDst1[6] = pSrc[3];
                    pDst1[1] = pSrc[32];
                    pDst1[3] = pSrc[33];
                    pDst1[5] = pSrc[34];
                    pDst1[7] = pSrc[35];
                    DEBUG_CACHE_WRITE_BYTES(pDst1,8);
                    pSrc += 4;
                    pDst1 += iWidth1;
                }
#else
                I32_WMV i1;
                for ( i1 = 0; i1 < 8; i1++) {
                    memcpy (pDst1, pSrc, 8 * sizeof(U16_WMV));
                    DEBUG_CACHE_READ_BYTES(pSrc,16);
                    DEBUG_CACHE_WRITE_BYTES(pDst1,16);
                    pSrc += 8;
                    pDst1 += iWidth1;
                }
#endif //N _EMB_SSIMD_MC_
            }
            
            pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
            
        }
		else {

#ifdef COMBINE_2MC_TO_1MC
		//	if((i1MVCnt < 2)) { 
				Bool_WMV bForce4MV = 0;
				if(b1MV) {
					bForce4MV = 1;
				}
				g_MotionComp_X9_EMB(pPMainLoop, iblk, 0, bForce4MV);
		//	}
#endif

#ifndef _WMV9AP_
			if ((rgCBP>>iblk)&0x1) {
#else
			if (rgCBP2[iblk]) {
#endif
				if (pmbmd->m_bBlkXformSwitchOn && !bFirstBlock) {
					I32_WMV iIndex = Huffman_WMV_get(pPMainLoop->PB.m_pHufBlkXformTypeDec, pPMainLoop->PB.m_pbitstrmIn);
					iXformType = pPMainLoop->PB.m_iBlkXformTypeLUT[iIndex];
					iSubblockPattern = pPMainLoop->PB.m_iSubblkPatternLUT[iIndex];
				}
				pmbmd->m_rgcBlockXformMode[iblk] =(I8_WMV)iXformType;
				if (iXformType == XFORMMODE_8x8) {
					ppxliErrorQMB = pPMainLoop->PB.m_rgiCoefReconBuf;
					result = (tWMVDecodeStatus)(result | pPMainLoop->PB.m_pDecodeInterError8x8((EMB_PBMainLoop *)pPMainLoop,  ppInterDCTTableInfo_Dec, pDQ));              
				} else {
					I32_WMV i, iLoopTop, iLoopMask, iPattern;
					I32_WMV iIdx;
	                
					I32_WMV (*p_DecodeInterError)(EMB_PBMainLoop  *pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
					ppxliErrorQMB = pPMainLoop->PB.m_ppxliErrorQ;
					if (iXformType == XFORMMODE_8x4 || iXformType == XFORMMODE_4x8) {
						if (pmbmd->m_bBlkXformSwitchOn || bFirstBlock) {
							iPattern = iSubblockPattern;
						}
						else {
							if (BS_getBit(pPMainLoop->PB.m_pbitstrmIn) == 1) {
								iPattern = BS_getBit(pPMainLoop->PB.m_pbitstrmIn) + 1;
							} else
								iPattern = 3;
						}
						iLoopTop = 2;
						iLoopMask = 2;
	                    
					}
					else {
						iPattern = Huffman_WMV_get(pPMainLoop->PB.m_pHuf4x4PatternDec, pPMainLoop->PB.m_pbitstrmIn) + 1;
						iLoopTop = 4;
						iLoopMask = 8;
					}
	                
					iIdx = iXformType-1;
					rgCBP2[iblk] = (U8_WMV)iPattern;
					p_DecodeInterError = pPMainLoop->PB.m_pDecodeInterError[iIdx];
					for( i=0; i<iLoopTop; i++ ) {
						if ( iPattern & (iLoopMask>>i) ) {
							result = (tWMVDecodeStatus)(result | (*p_DecodeInterError)((EMB_PBMainLoop *)pPMainLoop, ppInterDCTTableInfo_Dec, pDQ,  i));
						}
						else
						{
							pPMainLoop->PB.m_pSubBlkIDCTClear(ppxliErrorQMB, pPMainLoop->PB.m_uiIDCTClearOffset[iIdx][i]);
						}
					}
				}

#ifdef COMBINE_2MC_TO_1MC
				pPMainLoop->PB.m_pAddError(pDst, pPMainLoop->m_rgMotionCompBuffer_EMB, ppxliErrorQMB->i32, pPMainLoop->PB.m_iWidthPrev[iUV]);
#else
				if((i1MVCnt < 2)) { 
					g_MotionComp_X9_EMB(pPMainLoop, iblk, 0);
				}

				pPMainLoop->PB.m_pAddError(pDst, pPMainLoop->m_rgMotionCompBuffer_EMB + pPMainLoop->PB.m_pSSIMD_DstOffSet[iblk], ppxliErrorQMB->i32, pPMainLoop->PB.m_iWidthPrev[iUV]);
#endif
				bFirstBlock = FALSE;
			}
			else {
#ifdef COMBINE_2MC_TO_1MC
				pPMainLoop->PB.m_pAddNull(pDst, pPMainLoop->m_rgMotionCompBuffer_EMB, pPMainLoop->PB.m_iWidthPrev[iUV]);
#else
				if( (i1MVCnt < 2)) {
					g_MotionComp_X9_EMB(pPMainLoop, iblk, 0);
				}
				pPMainLoop->PB.m_pAddNull(pDst, pPMainLoop->m_rgMotionCompBuffer_EMB + pPMainLoop->PB.m_pSSIMD_DstOffSet[iblk], pPMainLoop->PB.m_iWidthPrev[iUV]);
#endif
				pmbmd->m_rgcBlockXformMode[iblk] = XFORMMODE_8x8;
			}
		}
        
    }
    // reset pmbmd->m_bSkip
    pmbmd->m_bSkip = bSkipMB;
    
    return result;
}


//static int g_dbg_cnt20, g_dbg_cnt21, g_dbg_cnt22, g_dbg_cnt23;
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus decodeMBOverheadOfPVOP_WMV3_EMB (EMB_PMainLoop * pPMainLoop,  CWMVMBMode* pmbmd,  I32_WMV iMBX, I32_WMV iMBY)
{
    //EMB_PMainLoop * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    I32_WMV iCBPCY = 0;//,  iPos=0, iTemp=0;
    Bool_WMV bSendTableswitchMode = FALSE_WMV;
    Bool_WMV bSendXformswitchMode = FALSE_WMV;
    CDiffMV_EMB *pDiffMV = pPMainLoop->PB.m_pDiffMV_EMB;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMBOverheadOfPVOP_WMV3_EMB);
    
    pmbmd->m_iQP = pPMainLoop ->PB.m_iQP; // starting QP - maybe changed by DQUANT
    pmbmd->m_dctMd = INTER;
    pmbmd->m_iDCTTable_MB_Index = 0;
    pmbmd->m_bBlkXformSwitchOn = FALSE_WMV;
    
    if (pPMainLoop->PB.m_iMVSwitchCodingMode == SKIP_RAW) {
        pmbmd->m_chMBMode = (BS_getBit(pPMainLoop->PB.m_pbitstrmIn) ? MB_4MV : MB_1MV);
    }
    
    if (pPMainLoop->PB.m_iSkipbitCodingMode == SKIP_RAW) {
        pmbmd->m_bSkip = BS_getBit(pPMainLoop->PB.m_pbitstrmIn);
    }
    
    if (pmbmd->m_bSkip) {
     //   g_dbg_cnt20++;
#ifdef _WMV9AP_
        memset(pmbmd->m_rgbCodedBlockPattern2, 0, 6*sizeof(U8_WMV));
#else
        pmbmd->m_rgbCodedBlockPattern = 0;
#endif
        // handle hybrid MV (needed for skipbit too)
        if (pmbmd->m_chMBMode == MB_1MV) {
            memset (pDiffMV, 0, sizeof(CDiffMV_EMB));
            if ((*pPMainLoop->m_ppPredictHybridMV_EMB[0]) (pPMainLoop, pPMainLoop->PB.m_iBlkMBBase[X_INDEX] + 0,  TRUE, 0))
                pDiffMV->iHybrid = 1 + (I8_WMV) BS_getBit(pPMainLoop->PB.m_pbitstrmIn);
            else
                pDiffMV->iHybrid = 0;
        } else {
            I32_WMV iblk;
			I16_WMV *iBlkMBBase = pPMainLoop->PB.m_iBlkMBBase;
			CInputBitStream_WMV* pbitstrmIn = pPMainLoop->PB.m_pbitstrmIn;
            memset (pDiffMV, 0, sizeof(CDiffMV_EMB) << 2);
            for ( iblk = 0; iblk < 4; iblk++) {
                if ((*pPMainLoop->m_ppPredictHybridMV_EMB[iblk>>1]) (pPMainLoop, iBlkMBBase[X_INDEX] + (iblk & 1),  FALSE, iblk))
                    //        PredictHybridMV_EMB (pPMainLoop, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1), FALSE, &dummy1, &dummy2))
                    (pDiffMV + iblk)->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);
                else
                    (pDiffMV + iblk)->iHybrid = 0;
                // hybrid MV - need to compute actual motion vectors too!
                ComputeMVFromDiffMV_EMB (pPMainLoop, iblk);
            }
        }
        goto OneMVEnd;
    }
    
    if (pmbmd->m_chMBMode == MB_1MV) {
        // 1MV - read motion vector
        //g_dbg_cnt21++;
        decodeMV_V9_EMB((EMB_PBMainLoop *)pPMainLoop, pPMainLoop->PB.m_pHufMVTable_V9, pDiffMV);    
 
        // hybrid MV
        pDiffMV->iHybrid = 0;
        if (
            (*pPMainLoop->m_ppPredictHybridMV_EMB[0]) (pPMainLoop, pPMainLoop->PB.m_iBlkMBBase[X_INDEX] + 0,  TRUE, 0)
            && !pDiffMV->iIntra)
            pDiffMV->iHybrid = 1 + (I8_WMV) BS_getBit(pPMainLoop->PB.m_pbitstrmIn);
        
        // skip to end if last = 0
        if (pDiffMV->iLast == 0) {
            pmbmd->m_bCBPAllZero = TRUE_WMV;
            //g_dbg_cnt22++;
            if (pPMainLoop->PB.m_bDQuantOn && pDiffMV->iIntra) {
                if (WMV_Succeeded != decodeDQuantParam (pPMainLoop->PB.m_pWMVDec, pmbmd)) {
                    return WMV_CorruptedBits; 
                }
            }
            // One MV Intra AC Prediction Flag
            if (pDiffMV->iIntra)
                pmbmd->m_rgbDCTCoefPredPattern2[ 0 ] = (U8_WMV)BS_getBit (pPMainLoop->PB.m_pbitstrmIn);              
            
            goto End;
        }
        bSendTableswitchMode = pPMainLoop->PB.m_bDCTTable_MB;
        bSendXformswitchMode = pPMainLoop->PB.m_bMBXformSwitching && !pDiffMV->iIntra;
        
        // One MV Intra AC Prediction Flag
        if (pDiffMV->iIntra)
            pmbmd->m_rgbDCTCoefPredPattern2[0]  =  (U8_WMV)BS_getBit (pPMainLoop->PB.m_pbitstrmIn);              
        
    }
    
    // read CBP
    iCBPCY = Huffman_WMV_get (pPMainLoop->PB.m_pHufNewPCBPCYDec, pPMainLoop->PB.m_pbitstrmIn);
    
    if (pPMainLoop->PB.m_bDQuantOn && pmbmd->m_chMBMode == MB_1MV) {
        if (WMV_Succeeded != decodeDQuantParam (pPMainLoop->PB.m_pWMVDec, pmbmd)) {
            return WMV_CorruptedBits;
        }
    }
    
    pmbmd->m_bCBPAllZero = FALSE_WMV;
    
    if (pmbmd->m_chMBMode == MB_4MV) {
        
        
        // 4MV - read motion vectors
        I32_WMV iCBPCY_V8 = iCBPCY & 0x30;
        I32_WMV iIntraYUV = 0;
        I32_WMV iIntraChroma = 0;
        I32_WMV iIntraPred = 0;
        I32_WMV iblk;
        Bool_WMV bDecodeDQ = FALSE;
        I16_WMV *iBlkMBBase = pPMainLoop->PB.m_iBlkMBBase;
		CInputBitStream_WMV* pbitstrmIn = pPMainLoop->PB.m_pbitstrmIn;
        //g_dbg_cnt23++;
        for ( iblk = 0; iblk < 4; iblk++) {
            I32_WMV iIntra;
            
            if (iCBPCY & (1 << iblk)) {
                decodeMV_V9_EMB((EMB_PBMainLoop *)pPMainLoop, pPMainLoop->PB.m_pHufMVTable_V9, (pDiffMV + iblk));    
            }
            else
                memset (pDiffMV + iblk, 0, sizeof (CDiffMV_EMB));
            // hybrid MV
            (pDiffMV + iblk)->iHybrid = 0;
            if ((*pPMainLoop->m_ppPredictHybridMV_EMB[iblk>>1]) (pPMainLoop, iBlkMBBase[X_INDEX] + (iblk & 1),  FALSE, iblk)
                //   PredictHybridMV_EMB (pPMainLoop, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1), FALSE, &dummy1, &dummy2) 
                && !((pDiffMV + iblk)->iIntra))
                (pDiffMV + iblk)->iHybrid = 1 + (I8_WMV) BS_getBit(pbitstrmIn);
            // hybrid MV - need to compute actual motion vectors too!
            iIntra = ComputeMVFromDiffMV_EMB (pPMainLoop, iblk);
            if (iIntra)
                iIntraPred |= IsIntraPredY_EMB(pPMainLoop->PB.m_pWMVDec, (iMBX << 1) + (iblk & 1), (iMBY << 1) + (iblk >> 1));
            
            // enable transform switching if necessary
            iIntraChroma += iIntra;
            iIntraYUV |= (iIntra << iblk);
            iCBPCY_V8 |= ((pDiffMV + iblk)->iLast << iblk);
        }
        iIntraYUV |= (iIntraChroma < 3)? 0 : 0x30;
        bSendTableswitchMode = (pPMainLoop->PB.m_bDCTTable_MB && iCBPCY_V8);
        bSendXformswitchMode = (pPMainLoop->PB.m_bMBXformSwitching && (~iIntraYUV & iCBPCY_V8));
        bDecodeDQ |= (iCBPCY_V8 != 0) || (iIntraChroma != 0);
        if (iIntraChroma > 2) {
            iIntraPred |= IsIntraPredUV_EMB(pPMainLoop->PB.m_pWMVDec, iMBX, iMBY);
            // iIntraPred |= IsIntraPredUV(pPMainLoop, iMBX, iMBY);       // this line looks reduntant.
        }
        
        iCBPCY = iCBPCY_V8;
        if (pPMainLoop->PB.m_bDQuantOn && bDecodeDQ) {
            if (WMV_Succeeded != decodeDQuantParam (pPMainLoop->PB.m_pWMVDec, pmbmd)) {
                return WMV_CorruptedBits;
            }
        }
        
        if (iIntraPred) 
            pmbmd->m_rgbDCTCoefPredPattern2[0]  = (U8_WMV)BS_getBit (pPMainLoop->PB.m_pbitstrmIn);              
    }
    
    // 0 :  0
    // 1 : 10
    // 2 : 11
    if (bSendTableswitchMode) {
        I32_WMV iDCTMBTableIndex = BS_getBit(pPMainLoop->PB.m_pbitstrmIn);
        if (iDCTMBTableIndex)
            iDCTMBTableIndex += BS_getBit(pPMainLoop->PB.m_pbitstrmIn); 
        assert(iDCTMBTableIndex>=0 && iDCTMBTableIndex<=2);
        pmbmd->m_iDCTTable_MB_Index = iDCTMBTableIndex;
    }
    
End:
    if (bSendXformswitchMode) {
        I32_WMV iIndex = Huffman_WMV_get (pPMainLoop->PB.m_pHufMBXformTypeDec, pPMainLoop->PB.m_pbitstrmIn);
        pmbmd->m_bBlkXformSwitchOn = (iIndex < 8);
        iIndex &= 0x7;
        pmbmd->m_iMBXformMode = pPMainLoop->PB.m_iBlkXformTypeLUT[iIndex];
        pmbmd->m_iSubblockPattern = pPMainLoop->PB.m_iSubblkPatternLUT[iIndex];
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
    
    if (pmbmd->m_chMBMode == MB_1MV)
    {
       // I32_WMV  k;
        CDiffMV_EMB * pInDiffMV = pPMainLoop->PB.m_pDiffMV_EMB;
        UMotion_EMB * pMotion;
        I32_WMV iData;
        
        ComputeMVFromDiffMV_EMB (pPMainLoop, 0);

        pMotion = pPMainLoop->PB.m_pMotion + pPMainLoop->PB.m_rgiCurrBaseBK[0];

        iData = pMotion->I32;
        pMotion++;
        pMotion->I32 = iData;
        pMotion += pPMainLoop->PB.m_iNumBlockX;
        pMotion->I32 = iData;
        pMotion--;
        pMotion->I32 = iData;
        
        (pInDiffMV+3)->iIntra = (pInDiffMV+2)->iIntra = (pInDiffMV+1)->iIntra = (pInDiffMV)->iIntra ;

    }   
    
    if (BS_invalid(pPMainLoop->PB.m_pbitstrmIn)) {
        return WMV_CorruptedBits;
    }
    
    return WMV_Succeeded;
}


//#pragma code_seg (EMBSEC_DEF)
I32_WMV PredictMVPullBack_EMB(EMB_PMainLoop  * pPMainLoop,  I32_WMV b1MV, I32_WMV iblk, I32_WMV iPred)
{
    
    I32_WMV iX1, iY1;
    
    I32_WMV iXPred = (iPred<<16)>>16;
    I32_WMV iYPred = iPred >>16;
    I32_WMV iX, iY;
    I32_WMV iNumBlockX = pPMainLoop->PB.m_iNumBlockX;
    I32_WMV iNumBlockY = pPMainLoop->PB.m_iNumBlockY;
    I32_WMV iMinCoordinate, iMaxX, iMaxY;
    Bool_WMV bUV = 0;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictMVPullBack_EMB);
    
    if(b1MV == 4)
    {
        iXPred <<= 1;
        iYPred <<= 1;
        bUV = 1;
    }
    
    if(b1MV == 2)
    {
        bUV = 1;
    }
    
    iX = pPMainLoop->PB.m_iBlkMBBase[X_INDEX] + (iblk&1);
    iY = pPMainLoop->PB.m_iBlkMBBase[Y_INDEX] + ((iblk&2)>>1);
    
    iX1 = (iX << 5) + iXPred;
    iY1 = (iY << 5 ) + iYPred;
    iMinCoordinate = (b1MV == 1) ? ((-60 -bUV*4)<<bUV) : ((-28 -bUV*4)<<bUV);  // -15 or -7 pixels
    iMaxX = (iNumBlockX << 5) - 4 + bUV*4;  // x8 for block x4 for qpel
    iMaxY = (iNumBlockY << 5) - 4 + bUV*4;
    
    /*
    iX1 &= ~(bUV*7);
    iY1 &= ~(bUV*7);
    */
    iX1 &= ~(bUV*((b1MV<<1)-1));
    iY1 &= ~(bUV*((b1MV<<1)-1));

    
    if (iX1 < iMinCoordinate)
    {
        iXPred += iMinCoordinate - iX1;
    }
    else if (iX1 > iMaxX)
    {
        iXPred += iMaxX - iX1;
    }
    
    if (iY1 < iMinCoordinate)
    {
        iYPred += iMinCoordinate - iY1;
    }
    else if (iY1 > iMaxY)
    {
        iYPred += iMaxY - iY1;
    }
    
    if(b1MV == 4)
    {
        iXPred >>= 1;
        iYPred >>= 1;
    }
    
    iPred = (iXPred&0xffff) |  (iYPred <<16);
    
    return iPred;
}


//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictMVPullBackCheck_EMB(EMB_PMainLoop  * pPMainLoop,  I32_WMV b1MV, I32_WMV iblk, I32_WMV iPred)
{

    I32_WMV iShift = 5 - (b1MV>>2);
    //I32_WMV iCord = (*(I32_WMV *)(pPMainLoop->PB.m_iBlkMBBase))+ (iblk&1) + ((iblk&2)<<15);
    I32_WMV iCord = (I32_WMV)(pPMainLoop->PB.m_iBlkMBBase[0] | (pPMainLoop->PB.m_iBlkMBBase[1]<<16))+ (iblk&1) + ((iblk&2)<<15);
    I32_WMV  iV1 = (iCord <<iShift) + (pPMainLoop->m_PredictMVLUT.U.m_PullBack[b1MV].m_uiMin) + iPred;
    I32_WMV iV2 = pPMainLoop->m_PredictMVLUT.U.m_PullBack[b1MV].m_uiMax  - (iCord <<iShift) -  iPred;
    I32_WMV iSign = (iPred &0x8000)<<1;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictMVPullBackCheck_EMB);

    iV1 -= iSign;
    
    if((iV1|iV2)&0x80008000)
    {
        iPred = PredictMVPullBack_EMB(pPMainLoop, b1MV, iblk, iPred);
    }
    
    return iPred;
    
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictHybridMV_EMB (EMB_PMainLoop  * pPMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk)
{
//    I32_WMV sum, i;
    I32_WMV iNumBlockX = pPMainLoop->PB.m_iNumBlockX;
    I32_WMV iIndex2 = pPMainLoop->PB.m_rgiCurrBaseBK[0] + pPMainLoop->PB.m_rgiBlkIdx[iblk];  //iY * iNumBlockX + iX;
    UMotion_EMB * pMotion = pPMainLoop->PB.m_pMotion;
    UMotion_EMB iVa, iVb, iVc, iPred, uiPred1, uiRef;
    I32_WMV  iBlocks;
    I32_WMV iIndex = iIndex2 - 1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictHybridMV_EMB);
    
    // find motion predictor
    iVc.U32 = 0;
    if (iX) 
    {
        // left block
        iVc.U32 = pMotion[iIndex].U32;
    }
    
    iIndex = iIndex2 - iNumBlockX;
    
    
    // top block
    iVa.U32 = pMotion[iIndex].U32;
    
    iIndex += pPMainLoop->m_PredictMVLUT.U.U.m_pBOffset[(b1MV<<2)+iblk];
    
    iVb.U32 = pMotion[iIndex].U32;


    iBlocks = 0;
    if(iVa.U32 == IBLOCKMV) 
        iBlocks ++;
    if(iVb.U32 == IBLOCKMV)
        iBlocks ++;
    if(iVc.U32 == IBLOCKMV)
        iBlocks ++;


    if(iBlocks > 0)
        goto nonZeroIBlocks;

nonZero1IBlocksRet:

        iPred.U32 = medianof3 (iVa.I32, iVb.I32,iVc.I32)>>16; 
        
        /*
        iVa.U32 <<= 16;
        iVb.U32 <<= 16;
        iVc.U32 <<= 16;
        */
        
        uiPred1.U32 = medianof3 (iVa.I32<<16, iVb.I32<<16, iVc.I32<<16); 
        
        iPred.U32 <<= 16;
        
        iPred.U32 |= ((U32_WMV)uiPred1.U32) >>16;
        
        
        pPMainLoop->PB.m_iPred.I32 = iPred.U32 = PredictMVPullBackCheck_EMB(pPMainLoop, b1MV, iblk, iPred.I32);
        
nonZero2IBlocksRet:
    
    
    if (iX == 0)
        return 0;

    {
        I32_WMV dx, dy;
        I32_WMV sum;

       // dx1 = dx = pPMainLoop->PB.m_iPred.I16[X_INDEX];
      //  dy1 = dy = pPMainLoop->PB.m_iPred.I16[Y_INDEX] ;
        dx = iPred.I16[X_INDEX];
        dy = iPred.I16[Y_INDEX];
        uiRef.I32 = pMotion[iIndex2 - 1].I32;
        if(uiRef.I32 != IBLOCKMV) 
        {
            dx -=uiRef.I16[X_INDEX];
            dy -= uiRef.I16[Y_INDEX];
        }
        
        sum = abs (dx) + abs (dy);
        if (sum > 32)
            return 1;

        uiRef.I32 = pMotion[iIndex2 - iNumBlockX].I32;
        if(uiRef.I32 != IBLOCKMV) 
        {
            dx = iPred.I16[X_INDEX] - uiRef.I16[X_INDEX];
            dy = iPred.I16[Y_INDEX] - uiRef.I16[Y_INDEX];
        }
        else
        {
            dx = iPred.I16[X_INDEX]; 
            dy = iPred.I16[Y_INDEX];
        }
        sum = abs (dx) + abs (dy);
        if (sum > 32)
            return 1;

        return 0;

    }
    
    

nonZeroIBlocks:

  
    if (iBlocks == 1) {
            if (iVa.U32 == IBLOCKMV)
                iVa.U32 = 0;
            else if (iVb.U32== IBLOCKMV)
                iVb.U32= 0;
            else if (iVc.U32 == IBLOCKMV)
                iVc.U32= 0;
            goto nonZero1IBlocksRet;
    }
    else
    {
         pPMainLoop->PB.m_iPred.U32 = iPred.U32  = 0;
         goto nonZero2IBlocksRet;
    }
    
    return 0;
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV PredictHybridMVTop_EMB (EMB_PMainLoop  * pPMainLoop, I32_WMV iX,  Bool_WMV b1MV,  I32_WMV iblk)
{    
    UMotion_EMB * pMotion = pPMainLoop->PB.m_pMotion;
    UMotion_EMB  iPred;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(PredictHybridMVTop_EMB);
    
    iPred.U32 = 0;
    
    if (iX != 0)
    {
        I32_WMV iIndex = pPMainLoop->PB.m_rgiCurrBaseBK[0] + pPMainLoop->PB.m_rgiBlkIdx[iblk] -1;  //iY * iNumBlockX + iX;
        I32_WMV iTmp = pMotion[iIndex].I32;
        
        if(iTmp != IBLOCKMV)
        {
            iPred.U32 = PredictMVPullBackCheck_EMB(pPMainLoop, b1MV, iblk, iTmp);
        }
    }
    
    pPMainLoop->PB.m_iPred.I32 = iPred.I32;
    
    return 0;
}






//#pragma code_seg (EMBSEC_PML)
I32_WMV ComputeMVFromDiffMV_EMB (EMB_PMainLoop *pPMainLoop, I32_WMV  iblk )
{
    // I32_WMV  iXBlocks = pPMainLoop->PB.m_iNumBlockX;

    I32_WMV  k = pPMainLoop->PB.m_rgiCurrBaseBK[0] + pPMainLoop->PB.m_rgiBlkIdx[iblk];
    I32_WMV  dX, dY, iIBlock = 0;
    CDiffMV_EMB * pInDiffMV = pPMainLoop->PB.m_pDiffMV_EMB + iblk;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ComputeMVFromDiffMV_EMB);
    
    // 1MV non-anchor blocks get assigned anchor block MV
    // find prediction, and add
    
    if (pInDiffMV->iIntra) {
        iIBlock = 1;
        pInDiffMV->Diff.I32 = pPMainLoop->PB.m_pMotion[k].I32 = IBLOCKMV;
    }
    else  {
        if (pInDiffMV->iHybrid == 0) {
            dX = pPMainLoop->PB.m_iPred.I16[X_INDEX];
            dY = pPMainLoop->PB.m_iPred.I16[Y_INDEX];
        }
        else {
            I32_WMV iIdxStep = pPMainLoop->PB.m_rgiBlkIdx[pInDiffMV->iHybrid ];
            UMotion_EMB * pMotion = pPMainLoop->PB.m_pMotion + k - iIdxStep ;
            /*
            dX = pPMainLoop->PB.m_pMotion[k - iIdxStep ].I16[X_INDEX];
            dY = pPMainLoop->PB.m_pMotion[k - iIdxStep ].I16[Y_INDEX];      
            */
            dX = pMotion->I16[X_INDEX];
            dY = pMotion->I16[Y_INDEX];      
            if (dX == IBLOCKMV) {
                dX = dY = 0;
            }
        }
        
        {
            I32_WMV iShift = pPMainLoop->PB.m_bHalfPelMV;
            I32_WMV iRange = pPMainLoop->PB.m_iXMVRange;
            I32_WMV iFlag = pPMainLoop->PB.m_iXMVFlag;

            pInDiffMV->Diff.I16[X_INDEX] = (((pInDiffMV->Diff.I16[X_INDEX]<<iShift) + dX + iRange) & iFlag) - iRange;

            iRange = pPMainLoop->PB.m_iYMVRange;
            iFlag = pPMainLoop->PB.m_iYMVFlag;

            pInDiffMV->Diff.I16[Y_INDEX] = (((pInDiffMV->Diff.I16[Y_INDEX]<<iShift) + dY + iRange) & iFlag) - iRange;

            pPMainLoop->PB.m_pMotion[k].I32 = pInDiffMV->Diff.I32;

    //    pInDiffMV->Diff.I16[X_INDEX] = pPMainLoop->PB.m_pMotion[k].I16[X_INDEX] = (((pInDiffMV->Diff.I16[X_INDEX]<<pPMainLoop->PB.m_bHalfPelMV) + dX + pPMainLoop->PB.m_iXMVRange) & pPMainLoop->PB.m_iXMVFlag) - pPMainLoop->PB.m_iXMVRange;
    //    pInDiffMV->Diff.I16[Y_INDEX] = pPMainLoop->PB.m_pMotion[k].I16[Y_INDEX] = (((pInDiffMV->Diff.I16[Y_INDEX]<<pPMainLoop->PB.m_bHalfPelMV) + dY + pPMainLoop->PB.m_iYMVRange) & pPMainLoop->PB.m_iYMVFlag) - pPMainLoop->PB.m_iYMVRange;     
        }
    }
    
    return iIBlock;
}

//static int g_dbg_cnt, g_dbg_cnt1, g_dbg_cnt2, g_dbg_cnt3, g_dbg_cnt4, g_dbg_cnt5, g_dbg_cnt6;

//#pragma code_seg (EMBSEC_PML2)
I32_WMV     median4less(EMB_PMainLoop *pPMainLoop)
{

    

    I32_WMV i,j;
    CDiffMV_EMB * pInDiffMV = pPMainLoop->PB.m_pDiffMV_EMB;
    UMotion_EMB uiV, iParams[4];
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(median4less);

    i = 0;
	j = 0;
    do
    {
        I32_WMV data =  (pInDiffMV+i)->Diff.I32;
        if(data != IBLOCKMV)
        {
            iParams[j++].I32 = data;
        }
    } while ( ++i < 4 );
    
    
    if(j ==3)
    {
        // g_dbg_cnt3++;
        uiV.I16[X_INDEX] = medianof3(iParams[0].I16[X_INDEX], iParams[1].I16[X_INDEX], iParams[2].I16[X_INDEX]);
        uiV.I16[Y_INDEX] = medianof3(iParams[0].I16[Y_INDEX], iParams[1].I16[Y_INDEX], iParams[2].I16[Y_INDEX]);
    }
    else if(j== 2)
    {
        //  g_dbg_cnt4++;
        uiV.I16[X_INDEX] = (iParams[0].I16[X_INDEX] + iParams[1].I16[X_INDEX] )/2;
        uiV.I16[Y_INDEX] = (iParams[0].I16[Y_INDEX] + iParams[1].I16[Y_INDEX] )/2;
    }
    else
    {
        
        uiV.I32 = IBLOCKMV;
    }
    
    
    i = 0;
    do
    {
        if(!(pInDiffMV+i)->iIntra)
        {
#if 1
            
            (pInDiffMV+i)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+i)->Diff.I32);
#else
            I32_WMV dx = (pInDiffMV+i)->Diff.I16[X_INDEX];
            I32_WMV dy =  (pInDiffMV+i)->Diff.I16[Y_INDEX];
            PullBackMotionVector (pPMainLoop->PB.m_pWMVDec, &dx, &dy, iX0, iY0);
            (pInDiffMV+i)->Diff.I16[X_INDEX] =  dx;
            (pInDiffMV+i)->Diff.I16[Y_INDEX]   = dy;
#endif
        }
    }while ( ++i < 4 );

    return uiV.I32;
    
}

//#pragma code_seg (EMBSEC_PML)
I32_WMV ChromaMV_EMB (EMB_PMainLoop *pPMainLoop, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV)
{
	EMB_PBMainLoop *pPB = &pPMainLoop->PB;
    //EMB_PMainLoop *pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    CDiffMV_EMB * pInDiffMV = pPB->m_pDiffMV_EMB;
    // I32_WMV  iX0 = iX, iY0 = iY;
    I32_WMV  iIndex;
    UMotion_EMB uiV, uiV2;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(ChromaMV_EMB);
    
    // g_dbg_cnt++;
    // 1MV case
    if (!b1MV) 
        goto start4mv;

    {
        uiV.I32 =pInDiffMV->Diff.I32;
        
        if(uiV.I32 == IBLOCKMV)
        {
            //g_dbg_cnt1++;
            goto IntraBlock1MV;
        }
        
        // iNumBoundsChecks = 1;
        
        (pInDiffMV)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, uiV.I32);
        
        // g_dbg_cnt2++;
        
BoundsCheck:
        
        assert (pPB->m_pWMVDec->m_tFrmType == PVOP);
        
        iX = uiV.I16[X_INDEX];
        iY = uiV.I16[Y_INDEX];
        iIndex = pPB->m_rgiCurrBaseBK[1];
        
        if (pPB->m_iNumBFrames > 0) {
            
        /*
        uiV2.I16[X_INDEX] = uiV.I16[X_INDEX]<<1;
        uiV2.I16[Y_INDEX] = uiV.I16[Y_INDEX]<<1;
            */
            //  assert(iXf == (uiV2.I16[X_INDEX]>>1));
            // assert(iYf == (uiV2.I16[Y_INDEX]>>1));
            
            uiV2.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 4, 0, uiV.I32);
            pPB->m_rgmv1_EMB[iIndex].m_vctTrueHalfPel.U32 = uiV2.U32;
           // pPB->m_pWMVDec->m_rgmv1[iIndex].m_vctTrueHalfPel.y = (I16_WMV)uiV2.I16[Y_INDEX];
            
        }
        {
            U8_WMV * pTbl = pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl;
            iX = (iX + pTbl[iX & 3]) >> 1;
            iY = (iY + pTbl[iY & 3]) >> 1;
        }
        if (pPB->m_bUVHpelBilinear) {
            if (iX > 0) iX -= iX & 1 ;
            else iX+=  iX & 1;
            if (iY > 0) iY -= iY & 1 ;
            else iY+=  iY & 1;
        }
        
        // iIndex = iY0 * (I32_WMV) pPB->m_uintNumMBX + iX0;
        // iIndex = pPB->m_rgiCurrBaseBK[1];
        
        uiV2.I16[X_INDEX] = iX;
        uiV2.I16[Y_INDEX] = iY;
        
        uiV2.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 4, 0, uiV2.I32);
        
        //  iX = uiV2.I16[X_INDEX];
        // iY = uiV2.I16[Y_INDEX];
        
        
        (pInDiffMV+5)->Diff.I32 = (pInDiffMV+4)->Diff.I32 =  pPB->m_pMotionC[iIndex].I32 = (I32_WMV) uiV2.I32; 
        /*
        (pInDiffMV+5)->Diff.I16[X_INDEX] = (pInDiffMV+4)->Diff.I16[X_INDEX] =  pPB->m_pMotionC[iIndex].I16[X_INDEX] = (I16_WMV)(I32_WMV) iX; 
        (pInDiffMV+5)->Diff.I16[Y_INDEX] = (pInDiffMV+4)->Diff.I16[Y_INDEX] = pPB->m_pMotionC[iIndex].I16[Y_INDEX] = (I16_WMV)(I32_WMV) iY;
        */
        
        (pInDiffMV+5)->iIntra = (pInDiffMV+4)->iIntra = 0;
        
        return 0;
    }
    

start4mv:
    // count I blocks

    if((pInDiffMV->Diff.I32 != IBLOCKMV) &&
        ((pInDiffMV+1)->Diff.I32 != IBLOCKMV) &&
        ((pInDiffMV+2)->Diff.I32 != IBLOCKMV) &&
        ((pInDiffMV+3)->Diff.I32 != IBLOCKMV) )
    {
 
        uiV.I16[X_INDEX] = medianof4(pInDiffMV[0].Diff.I16[X_INDEX], pInDiffMV[1].Diff.I16[X_INDEX], pInDiffMV[2].Diff.I16[X_INDEX], pInDiffMV[3].Diff.I16[X_INDEX]);
        uiV.I16[Y_INDEX] = medianof4(pInDiffMV[0].Diff.I16[Y_INDEX], pInDiffMV[1].Diff.I16[Y_INDEX], pInDiffMV[2].Diff.I16[Y_INDEX], pInDiffMV[3].Diff.I16[Y_INDEX]);
        
        pInDiffMV = pPB->m_pDiffMV_EMB;

        (pInDiffMV+3)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+3)->Diff.I32);
        (pInDiffMV+2)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+2)->Diff.I32);
        (pInDiffMV+1)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+1)->Diff.I32);
        (pInDiffMV+0)->Diff.I32 = PredictMVPullBackCheck_EMB(pPMainLoop, 2, 0, (pInDiffMV+0)->Diff.I32);

        goto BoundsCheck;
    }
    else
    {
        //one of the comp is intra.
        uiV.I32 = median4less(pPMainLoop);
        if(uiV.I32 == IBLOCKMV)
            goto IntraBlock;

        goto BoundsCheck;
        
    }
    
    
IntraBlock:
    
IntraBlock1MV:
    
    //iIndex = iY0 * (I32_WMV) pPB->m_uintNumMBX + iX0;
    iIndex = pPB->m_rgiCurrBaseBK[1];

    //  iX = iY = IBLOCKMV;
    // pPB->m_pMotionC[iIndex].I16[X_INDEX] = (I16_WMV)(I32_WMV) iX; 
    //  pPB->m_pMotionC[iIndex].I16[Y_INDEX] = (I16_WMV)(I32_WMV) iY;

    pPB->m_pMotionC[iIndex].I32 = IBLOCKMV;
    
    (pInDiffMV+5)->iIntra = (pInDiffMV+4)->iIntra = 1;
    
    assert (pPB->m_pWMVDec->m_tFrmType == PVOP);
    
    if (pPB->m_iNumBFrames>0 /*&& pPB->m_tFrmType == PVOP*/) {
        pPB->m_rgmv1_EMB[iIndex].m_vctTrueHalfPel.U32 = 0;
        //pPB->m_pWMVDec->m_rgmv1[iIndex].m_vctTrueHalfPel.x = 0;
       // pPB->m_pWMVDec->m_rgmv1[iIndex].m_vctTrueHalfPel.y = 0;
    }
    
    return 1;
}



static I32_WMV gFineMV[] = {0,2,3,4,5,8};
//static I32_WMV aMVOffsets[] = {0,1,3,7,15,31};


//static int g_dbg_cnt7, g_dbg_cnt8, g_dbg_cnt9, g_dbg_cnt10;

//#pragma code_seg (EMBSEC_PML)
Void_WMV decodeMV_V9_EMB(EMB_PBMainLoop * pMainLoop, Huffman_WMV* pHufMVTable, CDiffMV_EMB *pDiffMV)
{
    I32_WMV iFine=0; 
    I32_WMV len;
    CInputBitStream_WMV *pbitstrmIn = pMainLoop->m_pbitstrmIn;
    I32_WMV iIndex = Huffman_WMV_get(pHufMVTable, pbitstrmIn);
    I32_WMV iTemp;

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeMV_V9_EMB);
    
    ++iIndex;
    // last symbol
    
    iTemp = iIndex >= 37;
    
    pDiffMV->iLast = iTemp;
    
    if(iTemp)  iIndex -= 37;
    
    pDiffMV->iIntra = 0;
    
    if(iIndex == 0)
    {
        pDiffMV->Diff.I32 = 0; 
    }
    else if(iIndex < 35)
    {
        I32_WMV i, iMask;
        I32_WMV iSign;
        I32_WMV iData = pMainLoop->m_rgiMVDecLUT_EMB[iIndex] ;
        for(i = 0; i < 2; i++, iData = iData >> 8)
        {
            len = iData&0xf;
            if(len>0)
                iFine = BS_getBits(pbitstrmIn,len);
            else
                iFine = 0;
            iSign = - (iFine & 1);
            iMask = (1<<((iData>>4)&0xf))-1;
#ifdef _BIG_ENDIAN_
            pDiffMV->Diff.I16[1-i] = (iSign ^ ((iFine >> 1) + iMask)) - iSign;
#else
            pDiffMV->Diff.I16[i] = (iSign ^ ((iFine >> 1) + iMask)) - iSign;
#endif
            //pDiffMV->Diff.I16[i] -= iSign;
            
        }
    }
    else if(iIndex == 36)
    {
        pDiffMV->iIntra = 1;
        //pDiffMV->iX = pDiffMV->iY = 0;
        pDiffMV->Diff.I32 = 0;
    }
    else
    {
        I32_WMV iLenX = pMainLoop->m_iLogXRange - pMainLoop->m_bHalfPelMV;
        I32_WMV iLenY = pMainLoop->m_iLogYRange - pMainLoop->m_bHalfPelMV;
        I32_WMV iData = BS_getBits(pbitstrmIn, iLenX + iLenY);
        pDiffMV->Diff.I16[X_INDEX] = (iData>>iLenY)&((1<<iLenX)-1);
        pDiffMV->Diff.I16[Y_INDEX] = (iData)&((1<<iLenY)-1);
    }
}

//static int g_dbg_cnt1, g_dbg_cnt2;
//#pragma code_seg (EMBSEC_PML2)
Bool_WMV decodeDCTPrediction_EMB (EMB_PMainLoop * pPMainLoop,
                                   CWMVMBMode *pmbmd, I16_WMV *pIntra, I32_WMV iblk,
                                   I32_WMV iX, I32_WMV iY, I32_WMV *piShift, I32_WMV *iDirection, I16_WMV *pPredScaled)

{
    
    Bool_WMV bACPredOn = TRUE_WMV;
    I32_WMV iUV = iblk >>2;
    I32_WMV iStride = pPMainLoop->PB.m_iNumBlockX >> iUV;
    Bool_WMV   iShift = 0; //= *piShift;
  //  I32_WMV  iWidth = iStride;
    I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
    Bool_WMV bDCACPred = FALSE_WMV;
    UMotion_EMB *pMotion = pPMainLoop->PB.m_pMotion;
   // I32_WMV iIndex = iX + (iY * pWMVDec->m_uintNumMBX << 1);
    I32_WMV iIndex = pPMainLoop->PB.m_rgiCurrBaseBK[iUV] + pPMainLoop->PB.m_rgiBlkIdx[iblk];

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDCTPrediction_EMB);

    //assert(pWMVDec->m_tFrmType != BVOP);
 
    pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;

    if(iUV) pMotion = pPMainLoop->PB.m_pMotionC;
    
 //   if ((iY && (pWMVDec->m_pMotion[iIndex - iWidth].I16[X_INDEX] == IBLOCKMV))) 
     if ((iY && (pMotion[iIndex - iStride].I32 == IBLOCKMV))) 
    {    
        pPred = pPredIntraTop = pIntra - 16 * iStride;// + pWMVDec->m_iACRowPredOffset;
        iShift = pPMainLoop->PB.m_iACRowPredShift;
    }
    
     if(iblk > 4) iblk = 4;
  //  if ((iX && (pWMVDec->m_pMotion[iIndex - 1].I16[X_INDEX] == IBLOCKMV))) 
     if ((iX && (pMotion[iIndex - 1].I32 == IBLOCKMV))) 
    {
        pPred = pPredIntraLeft = pIntra - 16;//pWMVDec->m_iACColPredOffset;
        iShift = pPMainLoop->PB.m_iACColPredShift;
    }
    if (pPredIntraLeft && pPredIntraTop) {
        I32_WMV iTopLeftDC = 0;
        I32_WMV iTopDC ;
        I32_WMV iLeftDC;
        I32_WMV iDiff0, iDiff1;
        if ((pMotion[(iIndex - iStride - 1)].I32== IBLOCKMV) ) {                
            iTopLeftDC = pPredIntraTop[pPMainLoop->PB.m_iACRowPredOffset - 16];
        }
        iTopDC = pPredIntraTop[pPMainLoop->PB.m_iACRowPredOffset];
        iLeftDC = pPredIntraLeft[pPMainLoop->PB.m_iACColPredOffset];                                   
        ScaleDCPredForDQuant (pPMainLoop->PB.m_pWMVDec, iblk, pmbmd, &iTopDC, &iLeftDC, &iTopLeftDC);
        
        iDiff0 = iTopLeftDC - iLeftDC;
        iDiff1 = iTopLeftDC - iTopDC;
        if (abs (iDiff0) < abs (iDiff1)) {
            pPred = pPredIntraTop;
            iShift = pPMainLoop->PB.m_iACRowPredShift;
        }
    }
    
    // preferred direction for 4MV
 //   assert(pWMVDec->m_bFieldCodingOn == 0);
    
    //  if (pPred != NULL && !(pmbmd->m_rgbDCTCoefPredPattern2[0]))
   //    bACPredOn = FALSE_WMV;
    
    if (pPred != NULL_WMV) {

        if(!pmbmd->m_rgbDCTCoefPredPattern2[0])
            bACPredOn = FALSE_WMV;

        bDCACPred = TRUE_WMV;
        if (iShift == pPMainLoop->PB.m_iACColPredShift) {
            ScaleLeftPredForDQuant (pPMainLoop->PB.m_pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        } else {
            ScaleTopPredForDQuant (pPMainLoop->PB.m_pWMVDec, iblk, pPred, pPredScaled, pmbmd);
        }
    }
    
    if (bACPredOn == FALSE_WMV) 
        iShift = -1;
    
    *iDirection = (pPred == pPredIntraTop);
    *piShift =  iShift ;
    return bDCACPred;
}

/*
 Bool_WMV decodeDCTPredictionUV_EMB(tWMVDecInternalMember *pWMVDec, 
      CWMVMBMode* pmbmd, I16_WMV*     pIntra, 
      I32_WMV imbX,     I32_WMV imbY, 
      I32_WMV     *piShift,
      I32_WMV *iDirection, I16_WMV *pPredScaled)
      
  {
      Bool_WMV bACPredOn = TRUE_WMV;
      Bool_WMV     iShift=  0;
      I16_WMV *pPredIntraLeft, *pPredIntraTop, *pPred;
      Bool_WMV bDCACPred = FALSE_WMV;
      DEBUG_PROFILE_FRAME_FUNCTION_COUNT(decodeDCTPredictionUV_EMB);

      
      pPred = pPredIntraLeft = pPredIntraTop = NULL_WMV;
      
      if (imbY) {
          if (pWMVDec->m_pMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].I16[X_INDEX] == IBLOCKMV) {
              pPred = pPredIntraTop = pIntra - 16 * pWMVDec->m_uintNumMBX;// + pWMVDec->m_iACRowPredOffset;
              iShift = pWMVDec->m_iACRowPredShift;
          }
      }
      if (imbX) {
          if (pWMVDec->m_pMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1].I16[X_INDEX] == IBLOCKMV) {
              pPred = pPredIntraLeft = pIntra - 16;// - pWMVDec->m_iACColPredOffset;
              iShift = pWMVDec->m_iACColPredShift;
          }
      }
      
      if (pPredIntraLeft && pPredIntraTop) {
          I32_WMV    iTopLeftDC = 0;
          I32_WMV iTopDC;
          I32_WMV iLeftDC;
          
          if (pWMVDec->m_pMotionC[(imbY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + imbX - 1].I16[X_INDEX] == IBLOCKMV)
              iTopLeftDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset - 16];
          
          iTopDC = pPredIntraTop[pWMVDec->m_iACRowPredOffset];
          iLeftDC = pPredIntraLeft[pWMVDec->m_iACColPredOffset];                                        
          ScaleDCPredForDQuant (pWMVDec, 4, pmbmd, &iTopDC, &iLeftDC, &iTopLeftDC);
          
          if (abs (iTopLeftDC - iLeftDC) < abs (iTopLeftDC - iTopDC)) {
              pPred = pPredIntraTop;
              iShift = pWMVDec->m_iACRowPredShift;
          }
      }
      
      if (pPred != NULL_WMV) 
      {
          bDCACPred = TRUE_WMV;
           if(!pmbmd->m_rgbDCTCoefPredPattern2[0])
            bACPredOn = FALSE_WMV;
      
    //  if (bDCACPred) {
          if (iShift == pWMVDec->m_iACColPredShift) {
              ScaleLeftPredForDQuant (pWMVDec, 4, pPred, pPredScaled, pmbmd);
          } else {
              ScaleTopPredForDQuant (pWMVDec, 4, pPred, pPredScaled, pmbmd);
          }
      }
      
      if (bACPredOn == FALSE_WMV) 
          iShift = -1;
      *iDirection = (pPred == pPredIntraTop);
      *piShift =  iShift;
      
      return bDCACPred;
 }

  */
 
 //#pragma code_seg (EMBSEC_PML2)
 I32_WMV IsIntraPredUV_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iMBX, I32_WMV iMBY)
 {
     I32_WMV iPred = 0;
     if (iMBY) {
         if (pWMVDec->m_pMotionC[(iMBY - 1) * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX].I16[X_INDEX] == IBLOCKMV)
             iPred = 1;
     }
     if (iMBX) {
         if (pWMVDec->m_pMotionC[iMBY * (I32_WMV) pWMVDec->m_uintNumMBX + iMBX - 1].I16[X_INDEX] == IBLOCKMV)
             iPred = 1;
     }
     
     return iPred;
 }
 
 //#pragma code_seg (EMBSEC_PML)
 I32_WMV  IsIntraPredY_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY)
 {
     I32_WMV iWidth = pWMVDec->m_uintNumMBX << 1;
     I32_WMV iIndex = iX + (iY * pWMVDec->m_uintNumMBX << 1);
     I32_WMV iPred = 0;
     
     if (iY && (pWMVDec->m_pMotion[(iIndex - iWidth)].I16[X_INDEX] == IBLOCKMV))           
         iPred = 1;
     
     if (iX && (pWMVDec->m_pMotion[(iIndex - 1)].I16[X_INDEX] == IBLOCKMV))         
         iPred = 1;
     
     return iPred;
 }
 
 //#pragma code_seg (EMBSEC_DEF)
 Void_WMV SetupMVDecTable_EMB(tWMVDecInternalMember *pWMVDec)
 {
     I32_WMV iTemp;
     //I32_WMV iFine=0; 
     const I32_WMV  iNm1 = 5;
     I32_WMV iHalfpelMV;
     I32_WMV iIndex1, iIndex2;
     I32_WMV len;
     I32_WMV iIndex;
     I32_WMV data, data1;
     
     for(iIndex = 0; iIndex < 36; iIndex++)
     {
         iTemp = pWMVDec->m_pAltTables->m_iHalfPelMV;
         iIndex1 = prvWMVModulus  ((I32_WMV)(iIndex), (I32_WMV)(iNm1 + 1));
         iHalfpelMV = iTemp && (iIndex1 == iNm1);
         len = gFineMV[iIndex1] - iHalfpelMV;
         if(len <0) len = 0;
         
         data = len | (iIndex1<<4);
         
         iIndex2 = prvWMVDivision (iIndex , (iNm1 + 1));
         iHalfpelMV = iTemp && (iIndex2 == iNm1);
         len = gFineMV[iIndex2] - iHalfpelMV;
         if(len <0) len = 0;
         
         data1 = len | (iIndex2<<4);
         
         pWMVDec->m_rgiMVDecLUT_EMB[iIndex] = data | (data1<<8);
         
         
     }
 }

extern const I16_WMV gaSubsampleBicubic[];
//#pragma code_seg (EMBSEC_DEF)
Void_WMV InitEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV i,j;
    EMB_PMainLoop * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    //EMB_PBMainLoop * pMainLoop = (EMB_PBMainLoop * )pPMainLoop;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(InitEMBPMainLoopCtl);

    pPMainLoop->PB.m_pWMVDec = pWMVDec;
    
    pPMainLoop->PB.m_pDiffMV_EMB = pWMVDec->m_pDiffMV_EMB;
    pPMainLoop->PB.m_pbitstrmIn = pWMVDec->m_pbitstrmIn;
    
 //   pPMainLoop->PB.m_pXMotion = pWMVDec->m_pXMotion;
    pPMainLoop->PB.m_pMotion = pWMVDec->m_pMotion;
    pPMainLoop->PB.m_pMotionC = pWMVDec->m_pMotionC;
 //   pPMainLoop->PB.m_pYMotion = pWMVDec->m_pYMotion;
 //   pPMainLoop->PB.m_pXMotionC = pWMVDec->m_pXMotionC;
 //   pPMainLoop->PB.m_pYMotionC = pWMVDec->m_pYMotionC;
    
    pPMainLoop->PB.m_iNumBlockX = pWMVDec->m_uintNumMBX<<1;
    pPMainLoop->PB.m_iNumBlockY = pWMVDec->m_uintNumMBY<<1;
    pPMainLoop->PB.m_rgiBlkIdx[0] = 0;
    pPMainLoop->PB.m_rgiBlkIdx[1] = 1;
    pPMainLoop->PB.m_rgiBlkIdx[2] = pPMainLoop->PB.m_iNumBlockX;
    pPMainLoop->PB.m_rgiBlkIdx[3] = pPMainLoop->PB.m_iNumBlockX+1;
    
    pPMainLoop->PB.m_iWidthY = pPMainLoop->PB.m_iNumBlockX<<3;
    pPMainLoop->PB.m_iHeightY = pPMainLoop->PB.m_iNumBlockY<<3;
    pPMainLoop->PB.m_iWidthUV = pPMainLoop->PB.m_iNumBlockX<<2;
    pPMainLoop->PB.m_iHeightUV = pPMainLoop->PB.m_iNumBlockY<<2;
    
    pPMainLoop->PB.m_iWidthPrevY = pWMVDec->m_iWidthPrevY;
    pPMainLoop->PB.m_iWidthPrevUV = pWMVDec->m_iWidthPrevUV;
    pPMainLoop->PB.m_iHeightPrevY = pWMVDec->m_iHeightPrevY;
    pPMainLoop->PB.m_iHeightPrevUV = pWMVDec->m_iHeightPrevUV;
    
    pPMainLoop->PB.m_iWidthPrev[0] = pPMainLoop->PB.m_iWidthPrevY;
    pPMainLoop->PB.m_iWidthPrev[1] = pPMainLoop->PB.m_iWidthPrevUV;
    
    pPMainLoop->m_rgMotionCompBuffer_EMB = pWMVDec->m_rgMotionCompBuffer_EMB;
    
    pPMainLoop->PB.m_rgiCoefReconBuf = pWMVDec->m_rgiCoefReconBuf;
    pPMainLoop->PB.m_pX9dct = pWMVDec->m_pX9dct;
    pPMainLoop->PB.m_pX9dctUV[0] = pWMVDec->m_pX9dctU;
    pPMainLoop->PB.m_pX9dctUV[1]= pWMVDec->m_pX9dctV;
    
    pPMainLoop->PB.m_rgmv1_EMB = pWMVDec->m_rgmv1_EMB;

    {
        I32_WMV iX, iblk, b1MV, iOffset, iX2;
        
        for(iX=0; iX < 3; iX++)
        {
            for(b1MV = 0; b1MV < 2; b1MV++)
            {
                for(iblk=0; iblk<4; iblk++)
                {
                    if(b1MV == 0) 
                    {
                        iX2 = iX + (iblk&1);
                        if (iX2)
                            iOffset = 1 - (((iblk == 0 || iblk == 3) || (iX == 2 && iblk == 1)) << 1);
                        else // First block column blocks use upper right predictor for B
                            iOffset = 1;
                        pPMainLoop->m_PredictMVLUT.m_rgcBOffset[iX*5 + b1MV*4 + iblk] = iOffset;
                    }
                    else
                    {
                        iOffset = (iX < (2)) * 3 - 1;            
                        pPMainLoop->m_PredictMVLUT.m_rgcBOffset[iX*5 + 4] = iOffset;
                    }
                }
            }
        }
    }
    
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[0].m_uiMax  = (pPMainLoop->PB.m_iNumBlockX << 5) - 4 + (((pPMainLoop->PB.m_iNumBlockY << 5) - 4)<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[1].m_uiMax  = (pPMainLoop->PB.m_iNumBlockX << 5) - 4 + (((pPMainLoop->PB.m_iNumBlockY << 5) - 4)<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[2].m_uiMax  = (pPMainLoop->PB.m_iNumBlockX << 5)  + (((pPMainLoop->PB.m_iNumBlockY << 5))<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[4].m_uiMax  = (pPMainLoop->PB.m_iNumBlockX << 4)  + (((pPMainLoop->PB.m_iNumBlockY << 4))<<16);
    
    pPMainLoop->PB.m_ppxliErrorQ = pWMVDec->m_ppxliErrorQ;
    
#ifdef _EMB_SSIMD_MC_
    pPMainLoop->PB.m_pDecodeInterError[XFORMMODE_8x4 -1] = g_DecodeInterError8x4_SSIMD_Fun;
    pPMainLoop->PB.m_pDecodeInterError[XFORMMODE_4x8 -1] = g_DecodeInterError4x8_SSIMD_Fun;
    pPMainLoop->PB.m_pDecodeInterError[XFORMMODE_4x4 -1] = g_DecodeInterError4x4_SSIMD_Fun;
    pPMainLoop->PB.m_pDecodeInterError8x8				 = g_DecodeInterError8x8_SSIMD_Fun;

    pPMainLoop->PB.m_pIDCTDec16_WMV3 = g_IDCTDec16_WMV3_Fun;
    pPMainLoop->PB.m_pAddError = g_AddError_SSIMD_Fun;
    pPMainLoop->PB.m_pAddNull = g_AddNull_SSIMD_Fun;

    pPMainLoop->PB.m_pInterpolateBlock_00 = g_InterpolateBlock_00_SSIMD_Fun;
    pPMainLoop->PB.m_pSubBlkIDCTClear = g_SubBlkIDCTClear_EMB;
#else
    pPMainLoop->PB.m_pDecodeInterError[XFORMMODE_8x4 -1] = g_DecodeInterError8x4_C;
    pPMainLoop->PB.m_pDecodeInterError[XFORMMODE_4x8 -1] = g_DecodeInterError4x8_C;
    pPMainLoop->PB.m_pDecodeInterError[XFORMMODE_4x4 -1] = g_DecodeInterError4x4_C;
    pPMainLoop->PB.m_pDecodeInterError8x8 = g_DecodeInterError8x8_C;

    pPMainLoop->PB.m_pIDCTDec16_WMV3 = g_IDCTDec16_WMV3_C;
    pPMainLoop->PB.m_pAddError = g_AddError_EMB_C;
    pPMainLoop->PB.m_pAddNull = g_AddNull_EMB_C;

    pPMainLoop->PB.m_pInterpolateBlock_00 = g_InterpolateBlock_00_C;
    pPMainLoop->PB.m_pSubBlkIDCTClear = g_SubBlkIDCTClear_EMB_C;
#endif

#if defined(_EMB_SSIMD_MC_) && defined(_ARM_)
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPMainLoop->PB.m_pInterpolateBlockBilinear[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun;
        }
    }

	pPMainLoop->PB.m_pInterpolateBlockBilinear[0][2] = g_InterpolateBlockBilinear_SSIMD_01_Fun;
	pPMainLoop->PB.m_pInterpolateBlockBilinear[2][0] = g_InterpolateBlockBilinear_SSIMD_10_Fun;
	pPMainLoop->PB.m_pInterpolateBlockBilinear[2][2] = g_InterpolateBlockBilinear_SSIMD_11_Fun;

#elif defined(_EMB_SSIMD_MC_)
	for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPMainLoop->PB.m_pInterpolateBlockBilinear[i][j] = g_InterpolateBlockBilinear_SSIMD_Fun;
        }
    }
#else
	for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            pPMainLoop->PB.m_pInterpolateBlockBilinear[i][j] = g_InterpolateBlockBilinear_EMB_C;
        }
    }
#endif

    pPMainLoop->PB.m_iFrmMBOffset[0] = 0;
    pPMainLoop->PB.m_iFrmMBOffset[1] = 0;
    
    for(i = 0; i< 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            if(i == (XFORMMODE_8x4-1))
            {
                pPMainLoop->PB.m_uiIDCTClearOffset[i][j] = (j<<3);
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x8-1))
            {
                pPMainLoop->PB.m_uiIDCTClearOffset[i][j] = j + 0xc0;
                if(j == 1)   break;
            }
            if(i == (XFORMMODE_4x4-1))
            {
                pPMainLoop->PB.m_uiIDCTClearOffset[i][j] =  ((j&2)<<2) + (j&1)+ 0x40;
                
            }
        }
    }
    
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[0].m_uiMin = 28 + (28<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[1].m_uiMin = 60 + (60<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[2].m_uiMin = 64 + (64<<16);
    pPMainLoop->m_PredictMVLUT.U.m_PullBack[4].m_uiMin = 32 + (32<<16);
    
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[0] = 0;
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[1] = 0;
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[2] = 0;
    pPMainLoop->m_PredictMVLUT.U.U.m_RndTbl[3] = 1;
    
    pPMainLoop->PB.m_iBlkXformTypeLUT[0] =  XFORMMODE_8x8, 
    pPMainLoop->PB.m_iBlkXformTypeLUT[1] = pPMainLoop->PB.m_iBlkXformTypeLUT[2] = pPMainLoop->PB.m_iBlkXformTypeLUT[3] =  XFORMMODE_8x4;
    pPMainLoop->PB.m_iBlkXformTypeLUT[4] = pPMainLoop->PB.m_iBlkXformTypeLUT[5] = pPMainLoop->PB.m_iBlkXformTypeLUT[6]= XFORMMODE_4x8;
    pPMainLoop->PB.m_iBlkXformTypeLUT[7] =   XFORMMODE_4x4;
    
    pPMainLoop->PB.m_iSubblkPatternLUT[0] = 0;
    pPMainLoop->PB.m_iSubblkPatternLUT[1] = 1;
    pPMainLoop->PB.m_iSubblkPatternLUT[2] = 2;
    pPMainLoop->PB.m_iSubblkPatternLUT[3] = 3;
    pPMainLoop->PB.m_iSubblkPatternLUT[4] = 1;
    pPMainLoop->PB.m_iSubblkPatternLUT[5] = 2;
    pPMainLoop->PB.m_iSubblkPatternLUT[6] = 3;
    pPMainLoop->PB.m_iSubblkPatternLUT[7] = 0;

#ifdef _EMB_SSIMD_MC_
#ifdef _EMB_SSIMD64_
    pPMainLoop->m_SSIMD_DstLUT_1MV[0] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[1] = 2;
    pPMainLoop->m_SSIMD_DstLUT_1MV[2] = 96;
    pPMainLoop->m_SSIMD_DstLUT_1MV[3] = 96+2;
    pPMainLoop->m_SSIMD_DstLUT_1MV[4] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[5] = 0;
#elif defined(_EMB_SSIMD32_)
    pPMainLoop->m_SSIMD_DstLUT_1MV[0] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[1] = 2;
    pPMainLoop->m_SSIMD_DstLUT_1MV[2] = 80;
    pPMainLoop->m_SSIMD_DstLUT_1MV[3] = 80+2;
    pPMainLoop->m_SSIMD_DstLUT_1MV[4] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[5] = 0;
#endif
#else
    pPMainLoop->m_SSIMD_DstLUT_1MV[0] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[1] = 4;
    pPMainLoop->m_SSIMD_DstLUT_1MV[2] = 80;
    pPMainLoop->m_SSIMD_DstLUT_1MV[3] = 80+4;
    pPMainLoop->m_SSIMD_DstLUT_1MV[4] = 0;
    pPMainLoop->m_SSIMD_DstLUT_1MV[5] = 0;
#endif //_EMB_SSIMD_MC_
    
    pPMainLoop->m_SSIMD_DstLUT_BK[0] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[1] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[2] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[3] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[4] = 0;
    pPMainLoop->m_SSIMD_DstLUT_BK[5] = 0;
    
    pPMainLoop->PB.m_iNumCoefTbl[0] = 64;
    pPMainLoop->PB.m_iNumCoefTbl[1] = 32;
    pPMainLoop->PB.m_iNumCoefTbl[2] = 32;
    pPMainLoop->PB.m_iNumCoefTbl[3] = 0;
    pPMainLoop->PB.m_iNumCoefTbl[4] = 16;

    for (i=0; i<64; i++)
    {
        if (i < 8)
            pPMainLoop->PB.m_IDCTShortCutLUT[i] = 0;
        else
            pPMainLoop->PB.m_IDCTShortCutLUT[i] = 1 << (i & 0x7);
    }

         //memcpy(pPMainLoop ->m_rgiSubsampleBicubic, gaSubsampleBicubic, sizeof(I16_WMV)*16);
    for(i = 0; i< 16; i++)
    {
        pPMainLoop ->PB.m_rgiSubsampleBicubic[i] = (I8_WMV)gaSubsampleBicubic[i];
    }
    
    // *(U32_WMV *)pPMainLoop ->m_rgiSubsampleBicubic = 0x00000800 + 12 + 2*32 + ((0x00001000 + 12 + 4*32)<<16);
    //  *(U32_WMV *)(pPMainLoop ->m_rgiSubsampleBicubic + 2) =  0x00000800 + 12 + 3*32 + ((0x00001000 + 12 + 5*32)<<16);
    
    pPMainLoop ->PB.m_rgiSubsampleBicubic[0] = 0x00;
    pPMainLoop ->PB.m_rgiSubsampleBicubic[1] = 0x80;
    pPMainLoop ->PB.m_rgiSubsampleBicubic[2] = 0x00;
    pPMainLoop ->PB.m_rgiSubsampleBicubic[3] = 0x80;
    //*(U32_WMV *)pPMainLoop ->PB.m_rgiSubsampleBicubic = 0x80008000;
#if 0 // it is only used in g_NewVertFilter0LongNoGlblTbl()    
    pPMainLoop ->PB.m_rgiSSIMD_00Tbl[0] = 0x00000800 + 12 + 2*32 + ((0x00001000 + 12 + 4*32)<<16);
    pPMainLoop ->PB.m_rgiSSIMD_00Tbl[1] = 0x00000800 + 12 + 3*32 + ((0x00001000 + 12 + 5*32)<<16);
#endif
    pPMainLoop ->PB. m_uiffff07ff =  0xffff07ff;
    pPMainLoop ->PB. m_uiffff7fff =  0xffff7fff;
    pPMainLoop ->PB. m_uiffff1fff =  0xffff1fff;;
    pPMainLoop ->PB. m_uic000c000 = 0xc000c000;
    //     pPMainLoop -> m_ui80008000 = 0x80008000;
    pPMainLoop ->PB. m_uif000f000 = 0xf000f000;;
    pPMainLoop ->PB. m_ui00ff00ff = 0x00ff00ff;;


    {
        I32_WMV iUV, b1MV;

        for(iUV=0; iUV<2; iUV++)
            for(b1MV=0; b1MV<2; b1MV++) {
                I32_WMV iSrcStride = pPMainLoop->PB.m_iWidthPrevY;
#               ifdef _EMB_SSIMD64_
                    I32_WMV iParam1 = 48, iParam2 = 24, iParam3 = 7;
#               else
                    I32_WMV iParam1 = 40, iParam2 = 20, iParam3 = 3;
#               endif

                if(iUV == 1)
                    iSrcStride = pPMainLoop->PB.m_iWidthPrevUV;

#ifdef COMBINE_2MC_TO_1MC
                pPMainLoop->PB.m_rgiNewVertTbl_2[iUV][b1MV][0] = 3*iSrcStride + (iSrcStride<<3) -1 ;
                pPMainLoop->PB.m_rgiNewVertTbl_2[iUV][b1MV][1] = (iParam1 <<3) - iParam2;
                pPMainLoop->PB.m_rgiNewVertTbl_2[iUV][b1MV][2] = 3*iSrcStride + (iSrcStride<<3) - iParam3 ;
                pPMainLoop->PB.m_rgiNewVertTbl_2[iUV][b1MV][3] = (iParam1 <<3) + 16;

                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][0] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) -1 ;
                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][1] = (iParam1 <<(3+b1MV)) - iParam2;
                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][2] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) - iParam3 ;
                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][3] = (iParam1 <<(3+b1MV)) + 16;
#else
                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][0] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) -1 ;
                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][1] = (iParam1 <<(3+b1MV)) - iParam2;
                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][2] = 3*iSrcStride + (iSrcStride<<(3+b1MV)) - iParam3 ;
                pPMainLoop->PB.m_rgiNewVertTbl[iUV][b1MV][3] = (iParam1 <<(3+b1MV)) + 16;
#endif
            }
        }
    }

//#pragma code_seg (EMBSEC_DEF)
Void_WMV SetupEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec)
{
     EMB_PMainLoop * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
      //EMB_PBMainLoop * pMainLoop = (EMB_PBMainLoop * )pPMainLoop;

     DEBUG_PROFILE_FRAME_FUNCTION_COUNT(SetupEMBPMainLoopCtl);

     InitEMBPMainLoopCtl(pWMVDec);

     pPMainLoop->PB.m_rgiMVDecLUT_EMB = pWMVDec->m_rgiMVDecLUT_EMB;
     pPMainLoop->PB.m_iQP= ((pWMVDec->m_iStepSize << 1) - 1) + pWMVDec->m_bHalfStep;
     pPMainLoop->PB.m_iMVSwitchCodingMode = pWMVDec->m_iMVSwitchCodingMode;
     pPMainLoop->PB.m_iSkipbitCodingMode = pWMVDec->m_iSkipbitCodingMode;
     pPMainLoop->PB.m_pHufMVTable_V9 = pWMVDec->m_pHufMVTable_V9;
     pPMainLoop->PB.m_bDQuantOn = pWMVDec->m_bDQuantOn;
     pPMainLoop->PB.m_bDCTTable_MB = pWMVDec->m_bDCTTable_MB;
     pPMainLoop->PB.m_bMBXformSwitching = pWMVDec->m_bMBXformSwitching;
     pPMainLoop->PB.m_bHalfPelMV = 
         (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL) | (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
     pPMainLoop->PB.m_pHufNewPCBPCYDec = pWMVDec->m_pHufNewPCBPCYDec;
     pPMainLoop->PB.m_pHufMBXformTypeDec = pWMVDec->m_pHufMBXformTypeDec;
     pPMainLoop->PB.m_rgiCurrBaseBK[0] = 0;
     pPMainLoop->PB.m_rgiCurrBaseBK[1] = 0;
     pPMainLoop->PB.m_rgiBlkIdx[4] =
     pPMainLoop->PB.m_rgiBlkIdx[5] = 0;
     
     pPMainLoop->PB.m_iXMVRange = pWMVDec->m_iXMVRange;
     pPMainLoop->PB.m_iYMVRange = pWMVDec->m_iYMVRange;
     pPMainLoop->PB.m_iXMVFlag = pWMVDec->m_iXMVFlag;
     pPMainLoop->PB.m_iYMVFlag = pWMVDec->m_iYMVFlag;
     pPMainLoop->PB.m_iLogXRange = pWMVDec->m_iLogXRange;
     pPMainLoop->PB.m_iLogYRange = pWMVDec->m_iLogYRange;
     
     pPMainLoop->PB.m_iNumBFrames = pWMVDec->m_iNumBFrames;
     pPMainLoop->PB.m_bUVHpelBilinear = pWMVDec->m_bUVHpelBilinear;
     
     pPMainLoop->PB.m_iFrameXformMode = pWMVDec->m_iFrameXformMode;
     pPMainLoop->PB.m_prgDQuantParam = pWMVDec->m_prgDQuantParam;
     pPMainLoop->PB.m_pInterDCTTableInfo_Dec_Set = pWMVDec->m_pInterDCTTableInfo_Dec_Set;
     pPMainLoop->PB.m_pIntraDCTTableInfo_Dec_Set = pWMVDec->m_pIntraDCTTableInfo_Dec_Set;
     pPMainLoop->PB.m_ppInterDCTTableInfo_Dec = pWMVDec->m_ppInterDCTTableInfo_Dec;
     pPMainLoop->PB.m_ppIntraDCTTableInfo_Dec = pWMVDec->m_ppIntraDCTTableInfo_Dec;
     pPMainLoop->PB.m_iDCTHorzFlags= pWMVDec->m_iDCTHorzFlags;
     pPMainLoop->PB.m_iOverlap= pWMVDec->m_iOverlap;
     
     pPMainLoop->PB.m_pZigzagScanOrder= pWMVDec->m_pZigzagScanOrder;
     pPMainLoop->PB.m_iFilterType[0] =  pWMVDec->m_iFilterType;
     pPMainLoop->PB.m_iFilterType[1] = FILTER_BILINEAR;

     pPMainLoop->PB.m_iRndCtrl  = (pWMVDec->m_iRndCtrl <<16)|pWMVDec->m_iRndCtrl ;
     
     pPMainLoop->PB.m_ppxliRefBlkBase[0] = pWMVDec->m_ppxliRef0YPlusExp;
     pPMainLoop->PB.m_ppxliRefBlkBase[1] = pPMainLoop->PB.m_ppxliRefBlkBase[0] + 8;
     pPMainLoop->PB.m_ppxliRefBlkBase[2] = pPMainLoop->PB.m_ppxliRefBlkBase[0] + pWMVDec->m_iWidthPrevY*8;
     pPMainLoop->PB.m_ppxliRefBlkBase[3] = pPMainLoop->PB.m_ppxliRefBlkBase[0] + pWMVDec->m_iWidthPrevY*8 + 8;
     pPMainLoop->PB.m_ppxliRefBlkBase[4] = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pPMainLoop->PB.m_ppxliRefBlkBase[5] = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     
//#ifdef _EMB_3FRAMES_
     if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled)
     {
                if(pWMVDec->m_bRef0InRefAlt == TRUE_WMV)
                {

                        pPMainLoop->PB.m_ppxliRefBlkBase[0] = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp; 
                         pPMainLoop->PB.m_ppxliRefBlkBase[1] = pPMainLoop->PB.m_ppxliRefBlkBase[0] + 8;
                         pPMainLoop->PB.m_ppxliRefBlkBase[2] = pPMainLoop->PB.m_ppxliRefBlkBase[0] + pWMVDec->m_iWidthPrevY*8;
                         pPMainLoop->PB.m_ppxliRefBlkBase[3] = pPMainLoop->PB.m_ppxliRefBlkBase[0] + pWMVDec->m_iWidthPrevY*8 + 8;
                         pPMainLoop->PB.m_ppxliRefBlkBase[4] =  pWMVDec->m_pfrmRef0AltQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
                         pPMainLoop->PB.m_ppxliRefBlkBase[5] = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
                }
     }
//#endif
     pPMainLoop->PB.m_ppxliCurrBlkBase[0] = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
     pPMainLoop->PB.m_ppxliCurrBlkBase[1] = pPMainLoop->PB.m_ppxliCurrBlkBase[0] + 8;
     pPMainLoop->PB.m_ppxliCurrBlkBase[2] = pPMainLoop->PB.m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8;
     pPMainLoop->PB.m_ppxliCurrBlkBase[3] = pPMainLoop->PB.m_ppxliCurrBlkBase[0] + pWMVDec->m_iWidthPrevY*8 + 8;
     pPMainLoop->PB.m_ppxliCurrBlkBase[4] = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     pPMainLoop->PB.m_ppxliCurrBlkBase[5] = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
     
     pPMainLoop->m_rgfPredictHybridMV_EMB[0] = PredictHybridMVTop_EMB;
     pPMainLoop->m_rgfPredictHybridMV_EMB[1] = PredictHybridMV_EMB;
     pPMainLoop->m_rgfPredictHybridMV_EMB[2] = PredictHybridMV_EMB;
     pPMainLoop->m_ppPredictHybridMV_EMB = pPMainLoop->m_rgfPredictHybridMV_EMB;

     pPMainLoop->PB.m_iBlkMBBase[X_INDEX] = pPMainLoop->PB.m_iBlkMBBase[Y_INDEX] = 0;
     
     pPMainLoop->PB.m_pIntraBlockRow[0] = pPMainLoop->PB.m_pIntraBlockRow[1]  = pWMVDec->m_pIntraBlockRow0[0];
     pPMainLoop->PB.m_pIntraBlockRow[2] = pPMainLoop->PB.m_pIntraBlockRow[3]  = pWMVDec->m_pIntraBlockRow0[2];
     pPMainLoop->PB.m_pIntraBlockRow[4]  = pWMVDec->m_pIntraMBRowU0[0];
     pPMainLoop->PB.m_pIntraBlockRow[5]  = pWMVDec->m_pIntraMBRowV0[0];
     
     pPMainLoop->PB.m_iACColPredShift =  pWMVDec->m_iACColPredShift;
     pPMainLoop->PB.m_iACRowPredShift =  pWMVDec->m_iACRowPredShift;
     pPMainLoop->PB.m_iACColPredOffset =  pWMVDec->m_iACColPredOffset;
     pPMainLoop->PB.m_iACRowPredOffset =  pWMVDec->m_iACRowPredOffset;

     pPMainLoop->PB.m_pHufBlkXformTypeDec = pWMVDec->m_pHufBlkXformTypeDec;
     pPMainLoop->PB.m_pHuf4x4PatternDec = pWMVDec->m_pHuf4x4PatternDec;
     
#ifdef _EMB_SSIMD_IDCT_
     pPMainLoop->PB.m_pZigzag[ XFORMMODE_8x4 -1] = pWMVDec->m_p8x4ZigzagSSIMD;
     pPMainLoop->PB.m_pZigzag[ XFORMMODE_4x8 -1] = pWMVDec->m_p4x8ZigzagScanOrder;
     pPMainLoop->PB.m_pZigzag[ XFORMMODE_4x4 -1] = pWMVDec->m_p4x4ZigzagSSIMD;
#endif

     pPMainLoop ->PB. m_ui001f001fPiRndCtl = 0x001f001f + pPMainLoop->PB.m_iRndCtrl;
     pPMainLoop ->PB. m_ui000f000fPiRndCtl = 0x000f000f + pPMainLoop->PB.m_iRndCtrl;
     pPMainLoop ->PB. m_ui00070007PiRndCtl = 0x00070007 + pPMainLoop->PB.m_iRndCtrl;
     pPMainLoop ->PB. m_ui00030003PiRndCtl = 0x00030003 + pPMainLoop->PB.m_iRndCtrl;
     pPMainLoop ->PB. m_ui00200020MiRndCtl = 0x00200020 - pPMainLoop->PB.m_iRndCtrl;
     pPMainLoop ->PB. m_ui00400040MiRndCtl = 0x00400040 - pPMainLoop->PB.m_iRndCtrl;
     pPMainLoop ->PB. m_ui00080008MiRndCtl = 0x00080008 - pPMainLoop->PB.m_iRndCtrl;
}

//#pragma code_seg (EMBSEC_PML)
Void_WMV MotionCompMB_WMV3_EMB (EMB_PMainLoop * pPMainLoop)
{
//#define NO_1MV
    I32_WMV iblk;
    I32_WMV iUV; 
    U8_WMV *pDst;
	EMB_PBMainLoop *pPB = &pPMainLoop->PB;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(MotionCompMB_WMV3_EMB);
#ifndef NO_1MV    
#ifdef COMBINE_2MC_TO_1MC
	{
		Bool_WMV bForce4MV = 0;
		g_MotionComp_X9_EMB (pPMainLoop,  0, 1, bForce4MV);
	}
#else
    g_MotionComp_X9_EMB (pPMainLoop,  0, 1);
#endif
#endif
    pPB->m_pSSIMD_DstOffSet = pPMainLoop->m_SSIMD_DstLUT_1MV;

    for(iblk = 0;iblk < 6; iblk++)
    {
#ifdef NO_1MV    
		Bool_WMV bForce4MV = 1;
		iUV = iblk>>2;
		g_MotionComp_X9_EMB (pPMainLoop,  iblk, 0, bForce4MV);
#else
        iUV = iblk>>2;
        if (iUV) { 
#ifdef COMBINE_2MC_TO_1MC
			Bool_WMV bForce4MV = 0;
			g_MotionComp_X9_EMB (pPMainLoop,  iblk, 0, bForce4MV);
#else
            g_MotionComp_X9_EMB (pPMainLoop,  iblk, 0);
#endif
        }
#endif
        pDst = pPB->m_ppxliCurrBlkBase[iblk] + pPB->m_iFrmMBOffset[iUV];
#ifdef NO_1MV    
        pPB->m_pAddNull(pDst, pPMainLoop->m_rgMotionCompBuffer_EMB, pPB->m_iWidthPrev[iUV]);
#else
        pPB->m_pAddNull(pDst, pPMainLoop->m_rgMotionCompBuffer_EMB + pPB->m_pSSIMD_DstOffSet[iblk], pPB->m_iWidthPrev[iUV]);
#endif
    }
}

//static int g_dbg_cnt10, g_dbg_cnt11, g_dbg_cnt12;
//#pragma code_seg (EMBSEC_PML)
tWMVDecodeStatus WMVideoDecDecodeP_X9_EMB (tWMVDecInternalMember *pWMVDec)
{
    U8_WMV * ppxliCurrQY;
    U8_WMV * ppxliCurrQU;
    U8_WMV * ppxliCurrQV;
    U8_WMV * ppxliRefY ;
    U8_WMV * ppxliRefU ;
    U8_WMV * ppxliRefV ;   
    I32_WMV  iPixliY,  iPixliUV ;
    U32_WMV imbX, imbY;
    I32_WMV x, y;
    U32_WMV uiNumMBFromSliceBdry;
    Bool_WMV bFrameInPostBuf;
    CWMVMBMode* pmbmd;
    tWMVDecodeStatus result = WMV_Succeeded;
    EMB_PMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop;
    HUFFMANGET_DBG_HEADER(":WMVideoDecDecodeP_X9",9);
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(WMVideoDecDecodeP_X9_EMB);
    
    pWMVDec->m_ppInterDCTTableInfo_Dec[0] = pWMVDec->m_pInterDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    pWMVDec->m_ppIntraDCTTableInfo_Dec[0] = pWMVDec->m_pIntraDCTTableInfo_Dec_Set[pWMVDec->m_rgiDCTACInterTableIndx[0]];
    
    pWMVDec->m_pHufDCTDCyDec = pWMVDec->m_pHufDCTDCDec_Set[pWMVDec->m_iIntraDCTDCTable<<1];
    pWMVDec->m_pHufDCTDCcDec = pWMVDec->m_pHufDCTDCDec_Set[(pWMVDec->m_iIntraDCTDCTable<<1) + 1];
    
    if (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR)
        pWMVDec->m_iFilterType = FILTER_BILINEAR;
    else
        pWMVDec->m_iFilterType = FILTER_BICUBIC;
#   ifdef SIXTAP
    pWMVDec->m_iFilterType = FILTER_6TAP;
#   endif // SIXTAP
    //printf ("QP:%d\n", pWMVDec->m_iStepSize);
    
    // update overlap related variables
    UpdateDCStepSize(pWMVDec, pWMVDec->m_iStepSize);
        
    // SET_NEW_FRAME
    pWMVDec->m_pAltTables->m_iHalfPelMV =
        (pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL || pWMVDec->m_iX9MVMode == ALL_1MV_HALFPEL_BILINEAR);
    t_AltTablesSetNewFrame(pWMVDec->m_pAltTables, pWMVDec->m_iStepSize, 1);
    
    SetTransformTypeHuffmanTable (pWMVDec, pWMVDec->m_iStepSize);
    SetupMVDecTable_EMB(pWMVDec);
    
    ppxliCurrQY = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp;
    ppxliCurrQU = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliCurrQV = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliRefY = pWMVDec->m_ppxliRef0YPlusExp;
    ppxliRefU = pWMVDec->m_ppxliRef0U + pWMVDec->m_iWidthPrevUVXExpPlusExp;
    ppxliRefV = pWMVDec->m_ppxliRef0V + pWMVDec->m_iWidthPrevUVXExpPlusExp;

//#ifdef _EMB_3FRAMES_
    if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled) {
        if(pWMVDec->m_bRef0InRefAlt == TRUE_WMV) {
            ppxliRefY = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp; 
            ppxliRefU = pWMVDec->m_pfrmRef0AltQ->m_pucUPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
            ppxliRefV = pWMVDec->m_pfrmRef0AltQ->m_pucVPlane + pWMVDec->m_iWidthPrevUVXExpPlusExp; 
        }
    }
//#endif
    
    DEBUG_SHOW_FRAME_BUFFERS(pWMVDec->m_ppxliCurrQY, pWMVDec->m_ppxliRef0Y, NULL);
    DEBUG_CACHE_START_FRAME;

    pmbmd = pWMVDec->m_rgmbmd;
    pWMVDec->m_iOffsetToTopMB = pWMVDec->m_uintNumMBX;
    uiNumMBFromSliceBdry = pWMVDec->m_uintNumMBX + 1;
    
    SetupEMBPMainLoopCtl(pWMVDec);
    
    iPixliY = 0;
    iPixliUV = 0;
    
    for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE) {        
        U8_WMV * ppxliCurrQYMB;
        U8_WMV * ppxliCurrQUMB;
        U8_WMV * ppxliCurrQVMB;
        U8_WMV * ppxliRefYMB;
        U8_WMV * ppxliRefUMB;
        U8_WMV * ppxliRefVMB;
        //Bool_WMV bNot1stRowInPict;
        //Bool_WMV bNot1stRowInSlice;
        // Bool_WMV bNot1stRowInSliceNew;
        // FUNCTION_PROFILE_DECL_START(fpInit,DECODEPMAININIT_PROFILE);
        DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBY_LoopCount");
        
        
        pWMVDec->m_EMB_PMainLoop.PB.m_iFrmMBOffset[0] = iPixliY;
        pWMVDec->m_EMB_PMainLoop.PB.m_iFrmMBOffset[1] = iPixliUV;
        
        pWMVDec->m_EMB_PMainLoop.PB.m_iBlkMBBase[X_INDEX] = 0;
        pWMVDec->m_EMB_PMainLoop.m_PredictMVLUT.U.U.m_pBOffset = pWMVDec->m_EMB_PMainLoop.m_PredictMVLUT.m_rgcBOffset;
        

        ppxliCurrQYMB = ppxliCurrQY;
        ppxliCurrQUMB = ppxliCurrQU;
        ppxliCurrQVMB = ppxliCurrQV;
        ppxliRefYMB = ppxliRefY;
        ppxliRefUMB = ppxliRefU;
        ppxliRefVMB = ppxliRefV;
        //bNot1stRowInPict;
        //bNot1stRowInSlice;
        
        pWMVDec->m_iOffsetToTopMB = -pWMVDec->m_iOffsetToTopMB;   
        
        if(pWMVDec->m_bStartCode) {
            if(SliceStartCode(pWMVDec, imbY)!= WMV_Succeeded)
                return WMV_Failed;
        }
        
        for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE) {
            Bool_WMV b1MV;
            I32_WMV iIntra;
            //  I32_WMV iX, iY ;
            //  FUNCTION_PROFILE_DECL_START(fpPMBMode, DECODEMBMODE_PROFILE)
            DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MBX_LoopCount");

#ifdef DUMP_YUV_TEST
			if( (imbY == 0) && imbX == 9) {
				g_IsDumpFlag = 1;
			}
			else {
				g_IsDumpFlag = 0;
			}
#endif
            uiNumMBFromSliceBdry ++;
            pWMVDec->m_bMBHybridMV = FALSE_WMV;
            
            result = decodeMBOverheadOfPVOP_WMV3_EMB (pPMainLoop, pmbmd, imbX, imbY);
            
            if (WMV_Succeeded != result) {
                // FUNCTION_PROFILE_STOP(&fp);
                return result;
            }
            
            b1MV = (pmbmd->m_chMBMode == MB_1MV);
            
            iIntra = ChromaMV_EMB(pPMainLoop, imbX, imbY, b1MV);  
            
            if (b1MV) {
                // I32_WMV iIntra = (pWMVDec->m_pMotionC[imbY * (I32_WMV) pWMVDec->m_uintNumMBX + imbX].I16[X_INDEX] == IBLOCKMV);
                
                if (pmbmd->m_bCBPAllZero == TRUE && !iIntra) {
                    U8_WMV *ppxlcRefY;
                    I32_WMV iUVLoc ;
                    I32_WMV iMVOffst;
#ifdef _WMV9AP_
                    if (pWMVDec->m_bLoopFilter) {
#else
                    if (pWMVDec->m_bV9LoopFilter) {
#endif
                        memset(pmbmd->m_rgbCodedBlockPattern2, 0, 6);
                        memset(pmbmd->m_rgcBlockXformMode, XFORMMODE_8x8, 6*sizeof(pmbmd->m_rgcBlockXformMode[0]));
                    }
                    
                    iMVOffst = 4 * imbY * (I32_WMV) pWMVDec->m_uintNumMBX + 2 * imbX;
                    if (!pWMVDec->m_pMotion[iMVOffst].I16[X_INDEX] && !pWMVDec->m_pMotion[iMVOffst].I16[Y_INDEX]) {
                        
                        if(pWMVDec->m_bEMB3FrmEnabled && pWMVDec->m_bAltRefFrameEnabled && pWMVDec->m_bRef0InRefAlt == TRUE_WMV)
                        {
                            ppxlcRefY = pWMVDec->m_pfrmRef0AltQ->m_pucYPlane + pWMVDec->m_iWidthPrevYXExpPlusExp + pWMVDec->m_iWidthPrevY * y + x;
                            iUVLoc = ((y >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (x >> 1) + pWMVDec->EXPANDUV_REFVOP;
                            
                            (*pWMVDec->m_pMotionCompZero) (
                                ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                                ppxlcRefY, pWMVDec->m_pfrmRef0AltQ->m_pucUPlane + iUVLoc, pWMVDec->m_pfrmRef0AltQ->m_pucVPlane + iUVLoc,
                                pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                        }
                        else
                        {
                            ppxlcRefY = pWMVDec->m_ppxliRef0YPlusExp + pWMVDec->m_iWidthPrevY * y + x;
                            iUVLoc = ((y >> 1) + pWMVDec->EXPANDUV_REFVOP) * pWMVDec->m_iWidthPrevUV + (x >> 1) + pWMVDec->EXPANDUV_REFVOP;
                            
                            (*pWMVDec->m_pMotionCompZero) (
                                ppxliCurrQYMB, ppxliCurrQUMB, ppxliCurrQVMB,
                                ppxlcRefY, pWMVDec->m_ppxliRef0U + iUVLoc, pWMVDec->m_ppxliRef0V + iUVLoc,
                                pWMVDec->m_iWidthPrevY, pWMVDec->m_iWidthPrevUV);
                        }
                        pmbmd->m_bSkip = TRUE_WMV;
                        pmbmd->m_dctMd = INTER;
                        result = WMV_Succeeded;
                        goto LOOP_END;
                    }
                    else
                    {
                        MotionCompMB_WMV3_EMB (pPMainLoop);
                        pmbmd->m_bSkip = FALSE_WMV;
                        // g_dbg_cnt10++;
                        result = WMV_Succeeded;
                        goto LOOP_END;
                    }
                }
                result = DecodeMB_X9_Fast (pPMainLoop, pmbmd,  imbX, imbY, TRUE_WMV);
            }
            else {  // 4MV
                result = DecodeMB_X9_Fast (pPMainLoop, pmbmd,  imbX, imbY, FALSE_WMV);
            }
            
            if (WMV_Succeeded != result) {
                return result;
            }
LOOP_END:

            pWMVDec->m_EMB_PMainLoop.PB.m_rgiCurrBaseBK[0] += 2;
            pWMVDec->m_EMB_PMainLoop.PB.m_rgiCurrBaseBK[1] ++;
         
            pWMVDec->m_EMB_PMainLoop.PB.m_iBlkMBBase[X_INDEX] += 2;
            
            pWMVDec->m_EMB_PMainLoop.PB.m_iFrmMBOffset[0] += MB_SIZE;
            pWMVDec->m_EMB_PMainLoop.PB.m_iFrmMBOffset[1] += BLOCK_SIZE;           
            pWMVDec->m_EMB_PMainLoop.m_PredictMVLUT.U.U.m_pBOffset = pWMVDec->m_EMB_PMainLoop.m_PredictMVLUT.m_rgcBOffset + 5;
            
            if(pWMVDec->m_EMB_PMainLoop.PB.m_iBlkMBBase[X_INDEX] == (pWMVDec->m_EMB_PMainLoop.PB.m_iNumBlockX - 2))
                pWMVDec->m_EMB_PMainLoop.m_PredictMVLUT.U.U.m_pBOffset = pWMVDec->m_EMB_PMainLoop.m_PredictMVLUT.m_rgcBOffset + 10;
            
            pmbmd++;
            ppxliCurrQYMB += MB_SIZE;
            ppxliCurrQUMB += BLOCK_SIZE;
            ppxliCurrQVMB += BLOCK_SIZE;
            
            ppxliRefYMB += MB_SIZE;
            ppxliRefUMB += BLOCK_SIZE;
            ppxliRefVMB += BLOCK_SIZE;
            
#           ifdef PROFILE_CACHE_ANALYSIS
                DEBUG_CACHE_STEP_MB;
                if (imbY == 2 && imbX == 15 ) {
                    int j = 0;  // place for a breakpoint
                    g_bDebugOutputCacheMiss = 0;  // 1 to turn on debug output for one MB
                } else {
                    g_bDebugOutputCacheMiss = 0;
                }
#           endif // PROFILE_CACHE_ANALYSIS
        } //for (x = 0, imbX = 0; imbX < pWMVDec->m_uintNumMBX; imbX++, x += MB_SIZE)
        
        pPMainLoop->m_ppPredictHybridMV_EMB = pPMainLoop->m_rgfPredictHybridMV_EMB + 1;
        
        pWMVDec->m_EMB_PMainLoop.PB.m_iBlkMBBase[Y_INDEX] += 2;
        pWMVDec->m_EMB_PMainLoop.PB.m_rgiCurrBaseBK[0] += pWMVDec->m_uintNumMBX<<1;
        iPixliY  += pWMVDec->m_iMBSizeXWidthPrevY;
        iPixliUV += pWMVDec->m_iBlkSizeXWidthPrevUV;
        
        if (pWMVDec->m_iOverlap)
            OverlapMBRow_EMB (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, pWMVDec->m_uintNumMBY, 0, 0);
        
        // point to the starting location of the first MB of each row
        ppxliCurrQY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliCurrQU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliCurrQV += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRefY += pWMVDec->m_iMBSizeXWidthPrevY;
        ppxliRefU += pWMVDec->m_iBlkSizeXWidthPrevUV;
        ppxliRefV += pWMVDec->m_iBlkSizeXWidthPrevUV;

        DEBUG_CACHE_STEP_MB_ROW;
    } //for (imbY = 0, y = 0; imbY < pWMVDec->m_uintNumMBY; imbY++, y += MB_SIZE)
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT("MB_Loop_Done");
    
    if (pWMVDec->m_iOverlap)
        OverlapMBRow_EMB (pWMVDec, imbY, ppxliCurrQY, ppxliCurrQU, ppxliCurrQV, pWMVDec->m_uintNumMBY, 0, 0);
    
    bFrameInPostBuf = FALSE_WMV;

#ifdef DUMP_YUV_TEST
	if(frameCount == 2)
	{
		VO_VIDEO_BUFFER Output;
		VO_VIDEO_OUTPUTINFO OutPutInfo;
		Output.Buffer[0] = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevY*32 + 32;
		Output.Buffer[1] = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUV*16 + 16;
		Output.Buffer[2] = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUV*16 + 16;
		Output.Stride[0] = pWMVDec->m_frameBufStrideY;
		Output.Stride[1] = pWMVDec->m_frameBufStrideUV;
		Output.Stride[2] = pWMVDec->m_frameBufStrideUV;
		OutPutInfo.Format.Width = pWMVDec->m_iFrmWidthSrc;
		OutPutInfo.Format.Height = pWMVDec->m_iFrmHeightSrc;
		DumpOneFrame( &Output, &OutPutInfo, g_DumpYUVFile);
	}
#endif

 #ifndef WMV9_SIMPLE_ONLY
   if (pWMVDec->m_bLoopFilter) {

		if (pWMVDec->m_cvCodecVersion == WMVA)
			ComputeLoopFilterFlags_WMVA_EMB(pWMVDec);
		else
			ComputeLoopFilterFlags_WMV9_EMB(pWMVDec);

        DeblockSLFrame_V9 (pWMVDec, 
        pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevYXExpPlusExp, 
        pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUVXExpPlusExp, 
        pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUVXExpPlusExp,
        pWMVDec->m_puchFilter8x8RowFlagY0, pWMVDec->m_puchFilter8x8RowFlagU0, pWMVDec->m_puchFilter8x8RowFlagV0,
        pWMVDec->m_puchFilter8x4RowFlagY0, pWMVDec->m_puchFilter8x4RowFlagU0, pWMVDec->m_puchFilter8x4RowFlagV0,
        pWMVDec->m_puchFilter8x8ColFlagY0, pWMVDec->m_puchFilter8x8ColFlagU0, pWMVDec->m_puchFilter8x8ColFlagV0,
        pWMVDec->m_puchFilter4x8ColFlagY0, pWMVDec->m_puchFilter4x8ColFlagU0, pWMVDec->m_puchFilter4x8ColFlagV0,
        pWMVDec->m_pLoopFilterFlags, 0, pWMVDec->m_uiMBEnd0);    
    }
#endif

#ifdef DUMP_YUV_TEST
	if(frameCount == 2)
	{
		VO_VIDEO_BUFFER Output;
		VO_VIDEO_OUTPUTINFO OutPutInfo;
		Output.Buffer[0] = pWMVDec->m_ppxliCurrQY + pWMVDec->m_iWidthPrevY*32 + 32;
		Output.Buffer[1] = pWMVDec->m_ppxliCurrQU + pWMVDec->m_iWidthPrevUV*16 + 16;
		Output.Buffer[2] = pWMVDec->m_ppxliCurrQV + pWMVDec->m_iWidthPrevUV*16 + 16;
		Output.Stride[0] = pWMVDec->m_frameBufStrideY;
		Output.Stride[1] = pWMVDec->m_frameBufStrideUV;
		Output.Stride[2] = pWMVDec->m_frameBufStrideUV;
		OutPutInfo.Format.Width = pWMVDec->m_iFrmWidthSrc;
		OutPutInfo.Format.Height = pWMVDec->m_iFrmHeightSrc;
		DumpOneFrame( &Output, &OutPutInfo, g_DumpYUVFile2);
	}
#endif

    if (pWMVDec->m_cvCodecVersion == WMVA && pWMVDec->m_bBFrameOn)
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
    }

    pWMVDec->m_bDisplay_AllMB = (pWMVDec->m_bLoopFilter || pWMVDec->m_iResIndex != 0 || pWMVDec->m_iRefFrameNum != -1);
    pWMVDec->m_bRenderFromPostBuffer = bFrameInPostBuf;
    pWMVDec->m_bCopySkipMBToPostBuf = TRUE_WMV; // PostBuf is not used in this frame so need to copy skipMB to PostBuf if PostBuf is used in next frame
    
    return WMV_Succeeded;
}

static I32_WMV LOOPFILTERFLAGS_WMV9[17][16] =
{
 {0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff},
 {0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xff,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff},
 {0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff},
 {0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff},
 {0x0f,0x5f,0x9f,0xdf,0x6f,0x7f,0xff,0xff,0xaf,0xff,0xbf,0xff,0xef,0xff,0xff,0xff}
};

static I32_WMV LEFTFLAGS[5][16] =
{{0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff},
 {0xfc,0xfc,0xfd,0xfd,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xff,0xff,0xfe,0xfe,0xff,0xff}
};
static I32_WMV LEFTPREDFLAGS[5][16] =
{{0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xfc,0xff,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff},
 {0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff,0xfc,0xfd,0xfc,0xfd,0xfe,0xff,0xfe,0xff}
};
static I32_WMV TOPFLAGS[5][16] =
{{0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff},
 {0xf3,0xf3,0xf3,0xf3,0xf7,0xf7,0xf7,0xf7,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff}
};
static I32_WMV TOPPREDFLAGS[5][16] =
{{0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xff,0xf3,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}, 
 {0xf3,0xf7,0xfb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff},
 {0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff,0xf3,0xf7,0xfb,0xff}
};
static I32_WMV MIDDLEFLAGS[16] =
 {0x0f,0x5f,0x9f,0xdf,0x6f,0x7f,0xff,0xff,0xaf,0xff,0xbf,0xff,0xef,0xff,0xff,0xff};

//#pragma code_seg (EMBSEC_DEF)
Void_WMV ComputeLoopFilterFlags_WMV9_EMB(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iMBX, iMBY;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    
    for (iMBY = 0; iMBY < (I32_WMV) pWMVDec->m_uintNumMBY; iMBY++) 
    {
        for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++) 
        {
            U8_WMV *pCBP                = pmbmd->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPTop             = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPLeft            = (pmbmd - 1)->m_rgbCodedBlockPattern2;
            I8_WMV *pBlockXformMode     = pmbmd->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeTop  = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;
            
            ComputeMBLoopFilterFlags_WMV9_EMB (pWMVDec, 
                iMBX, iMBY, pmbmd->m_chMBMode,
                pBlockXformMode, pBlockXformModeTop, pBlockXformModeLeft,
                pCBP, pCBPTop, pCBPLeft, 
                iMBY == 0, iMBX == 0);
            
            pmbmd ++;  
        }
    }           
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV ComputeLoopFilterFlags_WMVA_EMB(tWMVDecInternalMember *pWMVDec)
{
    I32_WMV iMBX, iMBY;
    CWMVMBMode* pmbmd = pWMVDec->m_rgmbmd;
    
    for (iMBY = 0; iMBY < (I32_WMV) pWMVDec->m_uintNumMBY; iMBY++) 
    {
        for (iMBX = 0; iMBX < (I32_WMV) pWMVDec->m_uintNumMBX; iMBX++) 
        {
            U8_WMV *pCBP                = pmbmd->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPTop             = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgbCodedBlockPattern2;
            U8_WMV *pCBPLeft            = (pmbmd - 1)->m_rgbCodedBlockPattern2;
            I8_WMV *pBlockXformMode     = pmbmd->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeTop  = (pmbmd - pWMVDec->m_uintNumMBX)->m_rgcBlockXformMode;
            I8_WMV *pBlockXformModeLeft = (pmbmd - 1)->m_rgcBlockXformMode;
            
            ComputeMBLoopFilterFlags_WMVA_EMB (pWMVDec, 
                iMBX, iMBY, pmbmd->m_chMBMode,
                pBlockXformMode, pBlockXformModeTop, pBlockXformModeLeft,
                pCBP, pCBPTop, pCBPLeft, 
                iMBY == 0, iMBX == 0);
            
            pmbmd ++;  
        }
    }           
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV ComputeMBLoopFilterFlags_WMV9_EMB (tWMVDecInternalMember *pWMVDec,
                                       I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
                                       I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
                                       U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft)
{
    I32_WMV iBlk0 = iMBY * 4 * pWMVDec->m_uintNumMBX + iMBX * 2;
    I32_WMV iBlk2 = iBlk0 + 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iBlkUp = iBlk0 - 2 * pWMVDec->m_uintNumMBX;
    I8_WMV* pchLoopFilterFlag = pWMVDec->m_pLoopFilterFlags[iMBY*pWMVDec->m_uintNumMBX + iMBX].chFlag;
    
    I32_WMV iBlk;
    
    // in the interests of time and complexity, sub-block Xforms in B frames 
    // aren't internally loop filtered - besides "loop filtering" isn't really
    // necessary - postfiltering with equal visual quality can take care of it
    if (pWMVDec->m_tFrmType == BVOP) {
#if 1 
		pchLoopFilterFlag[0] = (I8_WMV)0x0f;
        pchLoopFilterFlag[1] = (I8_WMV)0x0f;
        pchLoopFilterFlag[2] = (I8_WMV)0x0f;
        pchLoopFilterFlag[3] = (I8_WMV)0x0f;
        pchLoopFilterFlag[4] = (I8_WMV)0x0f;
        pchLoopFilterFlag[5] = (I8_WMV)0x0f;
#else
        for (iBlk = 0; iBlk < 6; iBlk++)
            pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
#endif
    } 
    else {
		I32_WMV *pLoopFliterFlag16 = LOOPFILTERFLAGS_WMV9[16];
		iBlk = 0;
        do {
            if (pBlockXformMode[iBlk] == XFORMMODE_8x8)
                pchLoopFilterFlag[iBlk] = 0x0f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_8x4)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0xcf;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x8)
                pchLoopFilterFlag[iBlk] = 0x3f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x4)
				pchLoopFilterFlag[iBlk] = (I8_WMV)pLoopFliterFlag16[pCBPCurr[iBlk]];
        } while ( ++iBlk < 6 );
    }
        
    if (bTop && bLeft) 
    {  // top left
        pchLoopFilterFlag[0] &= 0xf0;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xf0;
        pchLoopFilterFlag[5] &= 0xf0;
    }
    else if (bTop) 
    {  // top row
        pchLoopFilterFlag[0] &= 0xf3;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[4] &= 0xf3;
        pchLoopFilterFlag[5] &= 0xf3;
    }
    else if (bLeft) 
    {  // left col
        pchLoopFilterFlag[0] &= 0xfc;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xfc;
        pchLoopFilterFlag[5] &= 0xfc;
    }
    
    if (pWMVDec->m_tFrmType  == BVOP)
        return;
    
    if (chMBMode != MB_1MV || pWMVDec->m_pMotion[0].I32 != IBLOCKMV)
    {
        I32_WMV iBlkC;
		UMotion_EMB *pMotion  = pWMVDec->m_pMotion;
		UMotion_EMB *pMotionC = pWMVDec->m_pMotionC;

        // block 0 rules
        if (!bTop && (pMotion[iBlk0].I32 != IBLOCKMV) && (pMotion[iBlkUp].I32 == pMotion[iBlk0].I32)) {
            pchLoopFilterFlag[0] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+8][pCBPCurr[0]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[2]+12][pCBPTop[2]]);
		}
        if (!bLeft && (pMotion[iBlk0].I32 != IBLOCKMV) && (pMotion[iBlk0 - 1].I32 == pMotion[iBlk0].I32)) {
            pchLoopFilterFlag[0] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]][pCBPCurr[0]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[1]+4][pCBPLeft[1]]);
		}

        // block 1 rules
        if (!bTop && (pMotion[iBlk0 + 1].I16[X_INDEX] != IBLOCKMV) && (pMotion[iBlkUp + 1].I32 == pMotion[iBlk0 + 1].I32)) {
            pchLoopFilterFlag[1] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+8][pCBPCurr[1]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[3]+12][pCBPTop[3]]);
		}
        if ((pMotion[iBlk0 + 1].I32  != IBLOCKMV) && (pMotion[iBlk0].I32 == pMotion[iBlk0 + 1].I32)) {
            pchLoopFilterFlag[1] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]][pCBPCurr[1]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+4][pCBPCurr[0]]);        
		}
        
        // block 2 rules
        if ((pMotion[iBlk2].I32 != IBLOCKMV) && (pMotion[iBlk0].I32 == pMotion[iBlk2].I32)) {
            pchLoopFilterFlag[2] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]+8][pCBPCurr[2]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[0]+12][pCBPCurr[0]]);
		}
        if (!bLeft && (pMotion[iBlk2].I32 != IBLOCKMV) && (pMotion[iBlk2 - 1].I32 == pMotion[iBlk2].I32)) {
            pchLoopFilterFlag[2] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[2]][pCBPCurr[2]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[3]+4][pCBPLeft[3]]);
		}

        // block 3 rules
        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && (pMotion[iBlk0 + 1].I32 == pMotion[iBlk2 + 1].I32)) {
            pchLoopFilterFlag[3] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]+8][pCBPCurr[3]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+12][pCBPCurr[1]]);
		}
        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && (pMotion[iBlk2].I32== pMotion[iBlk2 + 1].I32)) {
            pchLoopFilterFlag[3] &= 
            (LOOPFILTERFLAGS_WMV9[pBlockXformMode[3]][pCBPCurr[3]] | 
             LOOPFILTERFLAGS_WMV9[pBlockXformMode[1]+4][pCBPCurr[1]]);
		}
        
        // block 4 & 5 rules
        iBlkC = iMBY * pWMVDec->m_uintNumMBX + iMBX;
        if (!bTop && (pMotionC[iBlkC].I32!= IBLOCKMV) && (pMotionC[iBlkC - pWMVDec->m_uintNumMBX].I32 == pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]+8][pCBPCurr[4]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[4]+12][pCBPTop[4]]);

            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]+8][pCBPCurr[5]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeTop[5]+12][pCBPTop[5]]);

        }
        if (!bLeft && (pMotionC[iBlkC].I32 != IBLOCKMV) && (pMotionC[iBlkC - 1].I32== pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[4]][pCBPCurr[4]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[4]+4][pCBPLeft[4]]);

            pchLoopFilterFlag[5] &= 
                (LOOPFILTERFLAGS_WMV9[pBlockXformMode[5]][pCBPCurr[5]] | 
                 LOOPFILTERFLAGS_WMV9[pBlockXformModeLeft[5]+4][pCBPLeft[5]]);
        }
    }
}

//#pragma code_seg (EMBSEC_DEF)
Void_WMV ComputeMBLoopFilterFlags_WMVA_EMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV* pCBPCurr, U8_WMV* pCBPTop, U8_WMV* pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft)
{
    I32_WMV iBlk0 = iMBY * 4 * pWMVDec->m_uintNumMBX + iMBX * 2;
    I32_WMV iBlk2 = iBlk0 + 2 * pWMVDec->m_uintNumMBX;
    I32_WMV iBlkUp = iBlk0 - 2 * pWMVDec->m_uintNumMBX;
    I8_WMV* pchLoopFilterFlag = pWMVDec->m_pLoopFilterFlags[iMBY*pWMVDec->m_uintNumMBX + iMBX].chFlag;

    I32_WMV iBlk;

    if (!pWMVDec->m_bInterlaceV2 && pWMVDec->m_tFrmType == BVOP) {
#if 1
        pchLoopFilterFlag[0] = (I8_WMV)0x0f;
        pchLoopFilterFlag[1] = (I8_WMV)0x0f;
        pchLoopFilterFlag[2] = (I8_WMV)0x0f;
        pchLoopFilterFlag[3] = (I8_WMV)0x0f;
        pchLoopFilterFlag[4] = (I8_WMV)0x0f;
        pchLoopFilterFlag[5] = (I8_WMV)0x0f;
#else
        for (iBlk = 0; iBlk < 6; iBlk++)
            pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
#endif
    }
    else
    {
		iBlk = 0;
        do {
            if (pBlockXformMode[iBlk] == XFORMMODE_8x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x0f;
            else if (pBlockXformMode[iBlk] == XFORMMODE_8x4)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0xcf;
            else if (pBlockXformMode[iBlk] == XFORMMODE_4x8)
                pchLoopFilterFlag[iBlk] = (I8_WMV)0x3f;
			else if (pBlockXformMode[iBlk] == XFORMMODE_4x4)
				pchLoopFilterFlag[iBlk] = (I8_WMV)MIDDLEFLAGS[pCBPCurr[iBlk]];
		} while ( ++iBlk < 6 );

    }

    if (bTop && bLeft) {  
        // top left
        pchLoopFilterFlag[0] &= 0xf0;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xf0;
        pchLoopFilterFlag[5] &= 0xf0;
    }
    else if (bTop) {  
        pchLoopFilterFlag[0] &= 0xf3;
        pchLoopFilterFlag[1] &= 0xf3;
        pchLoopFilterFlag[4] &= 0xf3;
        pchLoopFilterFlag[5] &= 0xf3;
    }
    else if (bLeft) {  
        pchLoopFilterFlag[pWMVDec->m_iBetaRTMMismatchIndex] &= 0xfc;
        pchLoopFilterFlag[2] &= 0xfc;
        pchLoopFilterFlag[4] &= 0xfc;
        pchLoopFilterFlag[5] &= 0xfc;
    }


    if (pWMVDec->m_tFrmType  == BVOP)
        return;

    if (chMBMode != MB_INTRA) {
        I32_WMV iBlkC;
		UMotion_EMB *pMotion  = pWMVDec->m_pMotion;
		UMotion_EMB *pMotionC = pWMVDec->m_pMotionC;

		// block 0 rules
        if (!bTop && (pMotion[iBlk0].I32 != IBLOCKMV) && 
            (pMotion[iBlkUp].I32 == pMotion[iBlk0].I32))
            pchLoopFilterFlag[0] &= 
                (TOPFLAGS[pBlockXformMode[0]][pCBPCurr[0]] | 
                TOPPREDFLAGS[pBlockXformModeTop[2]][pCBPTop[2]]);

        if (!bLeft && (pMotion[iBlk0].I32 != IBLOCKMV) && 
            (pMotion[iBlk0 - 1].I32 == pMotion[iBlk0].I32))
            pchLoopFilterFlag[0] &= 
                (LEFTFLAGS[pBlockXformMode[0]][pCBPCurr[0]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[1]][pCBPLeft[1]]);
              
        // block 1 rules
        if (!bTop && (pMotion[iBlk0 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlkUp + 1].I32 == pMotion[iBlk0 + 1].I32))
            pchLoopFilterFlag[1] &= 
                (TOPFLAGS[pBlockXformMode[1]][pCBPCurr[1]] | 
                TOPPREDFLAGS[pBlockXformModeTop[3]][pCBPTop[3]]);

        if ((pMotion[iBlk0 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlk0].I32 == pMotion[iBlk0 + 1].I32))
            pchLoopFilterFlag[1] &= 
                (LEFTFLAGS[pBlockXformMode[1]][pCBPCurr[1]] | 
                LEFTPREDFLAGS[pBlockXformMode[0]][pCBPCurr[0]]);        

        // block 2 rules
        if ((pMotion[iBlk2].I32 != IBLOCKMV) && 
            (pMotion[iBlk0].I32 == pMotion[iBlk2].I32))
            pchLoopFilterFlag[2] &= 
                (TOPFLAGS[pBlockXformMode[2]][pCBPCurr[2]] | 
                TOPPREDFLAGS[pBlockXformMode[0]][pCBPCurr[0]]);

        if (!bLeft && (pMotion[iBlk2].I32 != IBLOCKMV) && 
            (pMotion[iBlk2 - 1].I32 == pMotion[iBlk2].I32))
            pchLoopFilterFlag[2] &= 
                (LEFTFLAGS[pBlockXformMode[2]][pCBPCurr[2]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[3]][pCBPLeft[3]]);

        // block 3 rules
        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlk0 + 1].I32 == pMotion[iBlk2 + 1].I32))
            pchLoopFilterFlag[3] &= 
                (TOPFLAGS[pBlockXformMode[3]][pCBPCurr[3]] | 
                TOPPREDFLAGS[pBlockXformMode[1]][pCBPCurr[1]]);

        if ((pMotion[iBlk2 + 1].I32 != IBLOCKMV) && 
            (pMotion[iBlk2].I32 == pMotion[iBlk2 + 1].I32))
            pchLoopFilterFlag[3] &= 
                (LEFTFLAGS[pBlockXformMode[3]][pCBPCurr[3]] | 
                LEFTPREDFLAGS[pBlockXformMode[2]][pCBPCurr[2]]);

        // block 4 & 5 rules
        iBlkC = iMBY * pWMVDec->m_uintNumMBX + iMBX;
        if (!bTop && (pMotionC[iBlkC].I32 != IBLOCKMV) && 
            (pMotionC[iBlkC - pWMVDec->m_uintNumMBX].I32 == pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (TOPFLAGS[pBlockXformMode[4]][pCBPCurr[4]] | 
                TOPPREDFLAGS[pBlockXformModeTop[4]][pCBPTop[4]]);
            pchLoopFilterFlag[5] &= 
                (TOPFLAGS[pBlockXformMode[5]][pCBPCurr[5]] | 
                TOPPREDFLAGS[pBlockXformModeTop[5]][pCBPTop[5]]);
        }

        if (!bLeft && (pMotionC[iBlkC].I32 != IBLOCKMV) && 
            (pMotionC[iBlkC - 1].I32 == pMotionC[iBlkC].I32)) {
            pchLoopFilterFlag[4] &= 
                (LEFTFLAGS[pBlockXformMode[4]][pCBPCurr[4]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[4]][pCBPLeft[4]]);
            pchLoopFilterFlag[5] &= 
                (LEFTFLAGS[pBlockXformMode[5]][pCBPCurr[5]] | 
                LEFTPREDFLAGS[pBlockXformModeLeft[5]][pCBPLeft[5]]);
        }



    }
}

//#pragma code_seg (EMBSEC_PML2)
Void_WMV OverlapMBRow_EMB (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, U8_WMV *ppxliRecnU,
                           U8_WMV *ppxliRecnV, I32_WMV iMBYEnd, I32_WMV iMBYStart,
                           I32_WMV iThreadID)
{
    I32_WMV  ii, iXBlocks = (I32_WMV) pWMVDec->m_uintNumMBX << 1;
    I32_WMV  iStrideUV = (I32_WMV) pWMVDec->m_uintNumMBX << 3;
    I32_WMV  iStride = iStrideUV << 1;
    I32_WMV  iBlockIndex = imbY * iXBlocks << 1;
    UMotion_EMB  *pMotion = pWMVDec->m_pMotion + iBlockIndex;
    UMotion_EMB  *pMotionUV = pWMVDec->m_pMotionC + imbY * (I32_WMV) pWMVDec->m_uintNumMBX;
    I16_WMV     **pIntraBlockRow, **pIntraMBRowU, **pIntraMBRowV;
    
    pIntraBlockRow = pWMVDec->m_pIntraBlockRow0;
    pIntraMBRowU = pWMVDec->m_pIntraMBRowU0;
    pIntraMBRowV = pWMVDec->m_pIntraMBRowV0;
    
    // as long as there are I blocks in the current rows, filter vertical edges
    if (imbY < iMBYEnd) {
        for (ii = 1; ii < iXBlocks; ii++) {
            if (pMotion[ii].I16[X_INDEX] == IBLOCKMV && pMotion[ii - 1].I16[X_INDEX] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[0] + (ii << 3), iStride);
            }
            if (pMotion[ii + iXBlocks].I16[X_INDEX] == IBLOCKMV && pMotion[ii + iXBlocks - 1].I16[X_INDEX] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraBlockRow[2] + (ii << 3), iStride);
            }
        }
        for (ii = 1; ii < (I32_WMV) pWMVDec->m_uintNumMBX; ii++) {
            if (pMotionUV[ii].I16[X_INDEX] == IBLOCKMV && pMotionUV[ii - 1].I16[X_INDEX] == IBLOCKMV) {
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowU[0] + (ii << 3), iStrideUV);
                pWMVDec->m_pOverlapBlockVerticalEdge (pIntraMBRowV[0] + (ii << 3), iStrideUV);
            }
        }
    }
    
    for (ii = 0; ii < iXBlocks; ii++) {
        Bool_WMV  bTop = (imbY > iMBYStart) && pMotion[ii - iXBlocks].I16[X_INDEX] == IBLOCKMV;
        Bool_WMV  bDn  = (imbY < iMBYEnd), bUp = FALSE;
        if (bDn) {
            bUp = pMotion[ii].I16[X_INDEX] == IBLOCKMV;
            bDn = pMotion[ii + iXBlocks].I16[X_INDEX] == IBLOCKMV;
        }
        
        if  (bTop || bUp) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[1] + (ii << 3), pIntraBlockRow[0] + (ii << 3),
                iStride, ppxliRecnY + (ii << 3), pWMVDec->m_iWidthPrevY, bTop, bUp, FALSE);
        }
        
        if  (bUp || bDn) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraBlockRow[0] + (ii << 3), pIntraBlockRow[2] + (ii << 3),
                iStride, ppxliRecnY + ((pWMVDec->m_iWidthPrevY + ii) << 3), pWMVDec->m_iWidthPrevY, bUp, bDn, FALSE);
        }
    }
    for (ii = 0; ii < (I32_WMV) pWMVDec->m_uintNumMBX; ii++) {
        Bool_WMV  bUp = (imbY > iMBYStart) && (pMotionUV[ii - (I32_WMV) pWMVDec->m_uintNumMBX].I16[X_INDEX] == IBLOCKMV);
        Bool_WMV  bDn = (imbY < iMBYEnd) && (pMotionUV[ii].I16[X_INDEX] == IBLOCKMV);
        
        if  (bUp || bDn) {
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowU[1] + (ii << 3), pIntraMBRowU[0] + (ii << 3),
                iStrideUV, ppxliRecnU + (ii << 3), pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
            pWMVDec->m_pOverlapBlockHorizontalEdge ( pIntraMBRowV[1] + (ii << 3), pIntraMBRowV[0] + (ii << 3),
                iStrideUV, ppxliRecnV + (ii << 3), pWMVDec->m_iWidthPrevUV, bUp, bDn, FALSE);
        }
    }
    
    // switch pointers - use pMotion as a temp
    if (iThreadID == 0) {
        I16_WMV * pMotion1;
        pMotion1 = pWMVDec->m_pIntraBlockRow0[1];
        pWMVDec->m_pIntraBlockRow0[1] = pWMVDec->m_pIntraBlockRow0[2];
        pWMVDec->m_pIntraBlockRow0[2] = pMotion1;
        
        pMotion1 = pWMVDec->m_pIntraMBRowU0[0];
        pWMVDec->m_pIntraMBRowU0[0] = pWMVDec->m_pIntraMBRowU0[1];
        pWMVDec->m_pIntraMBRowU0[1] = pMotion1;
        
        pMotion1 = pWMVDec->m_pIntraMBRowV0[0];
        pWMVDec->m_pIntraMBRowV0[0] = pWMVDec->m_pIntraMBRowV0[1];
        pWMVDec->m_pIntraMBRowV0[1] = pMotion1;
        {
            EMB_PBMainLoop  * pPMainLoop = &pWMVDec->m_EMB_PMainLoop.PB;
            pPMainLoop->m_pIntraBlockRow[0] = pPMainLoop->m_pIntraBlockRow[1] = pWMVDec->m_pIntraBlockRow0[0];
            pPMainLoop->m_pIntraBlockRow[2] = pPMainLoop->m_pIntraBlockRow[3] = pWMVDec->m_pIntraBlockRow0[2];
            pPMainLoop->m_pIntraBlockRow[4] = pWMVDec->m_pIntraMBRowU0[0];
            pPMainLoop->m_pIntraBlockRow[5] = pWMVDec->m_pIntraMBRowV0[0];
        }
    }
}


#endif  // _EMB_WMV3_
