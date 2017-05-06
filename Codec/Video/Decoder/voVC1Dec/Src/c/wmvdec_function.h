//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef __WMVDEC_FUNCTION_H_
#define __WMVDEC_FUNCTION_H_

#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "voVC1DID.h"
#ifdef macintosh
#include <math64.h>
#include <DriverServices.h>
#endif

#define VOIDC_DC 1


void	 reset_buf_seq(tWMVDecInternalMember *pWMVDec);

// ===========================================================================
// private functions
// ===========================================================================

// inits
Void_WMV  setCodedBlockPattern2 (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) ;
Void_WMV  setCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) ;
Bool_WMV getCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn) ;

tWMVDecodeStatus WMVInternalMemberInit ( tWMVDecInternalMember *pWMVDec,I32_WMV iWidthSource,I32_WMV iHeightSource);

// destroys
tWMVDecodeStatus WMVInternalMemberDestroy (tWMVDecInternalMember *pWMVDec);

// Sequence level decoding
tWMVDecodeStatus setSliceCode (tWMVDecInternalMember *pWMVDec, I32_WMV iSliceCode);
//tWMVDecodeStatus decodeVOLHead (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus WMVideoDecDecodeDataInternal (HWMVDecoder hWMVDecoder);

//Void_WMV WMVideoDecAssignMotionCompRoutines (tWMVDecInternalMember *pWMVDec);
Void_WMV WMVideoDecSwapCurrAndRef (tWMVDecInternalMember *pWMVDec, Bool_WMV bSwapPostBuffers);
tWMVDecodeStatus WMVideoDecDecodeFrameHead (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeFrameHead2 (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeClipInfo (tWMVDecInternalMember *pWMVDec);

//tWMVDecodeStatus WMVideoDecDecodeI_X8 (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeI (tWMVDecInternalMember *pWMVDec);
//tWMVDecodeStatus WMVideoDecDecodeP (tWMVDecInternalMember *pWMVDec);

//Init routines
tWMVDecodeStatus VodecConstruct ( tWMVDecInternalMember *pWMVDec, I32_WMV iWidthSource, I32_WMV iHeightSource);
tWMVDecodeStatus  VodecDestruct (tWMVDecInternalMember *pWMVDec);

Void_WMV m_InitIDCT_Dec(tWMVDecInternalMember *pWMVDec, Bool_WMV fFlag);
Void_WMV m_InitFncPtrAndZigzag(tWMVDecInternalMember *pWMVDec);
Void_WMV decide_vc1_routines (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus InitDataMembers_WMVA ( tWMVDecInternalMember *pWMVDec,     
                                                        I32_WMV iWidthSource, 
                                                        I32_WMV iHeightSource);
Void_WMV UpdateDCStepSize(tWMVDecInternalMember *pWMVDec, I32_WMV iStepSize);

#if defined(_ARM_)
    extern void memcpyOddEven_ARMV4 (void* pDst1, void* pSrc, const int iWidth1);
    /*cBytes2Clear mod 4 == 0 and pDest must be aligned*/
    extern void prvWMMemClrAligned_ARMV4(const void* pDest, const int cBytes2Clear );
    /*dest and src must both aligned and nbytes mod 4 = 0*/
    extern void prvWMMemCpyAligned_ARMV4(const void *dest, const void *src, const int nbytes);
    /* Dest is word aligned, iDstStride, iSrcStride mod 4 = 0, copy 8 lines of 8 bytes with line increase of iDstStride, and iSrcStride */
    extern void FASTCPY_8x8_ARMV4(const void *pDst2, const void *pSrc2, const int iDstStride, const int iSrcStride);
    /* Dest is word aligned, iDstStride, iSrcStride mod 4 = 0, copy 8 lines of 16 bytes with line increase of iDstStride, and iSrcStride */
    extern void FASTCPY_8x16_ARMV4(const void *pDst2, const void *pSrc2, const int iDstStride, const int iSrcStride);
#endif

#ifdef _ARM_
#define getHuffman   getHuffman_ARMV4
#else
#define getHuffman   getHuffman_C
#endif

Void_WMV DeblockSLFrame (tWMVDecInternalMember *pWMVDec,
                                      U8_WMV* ppxliY, 
                                      U8_WMV* ppxliU, 
                                      U8_WMV* ppxliV);
//Deblocking 
Void_WMV FilterEdgeShortTagBlockRow (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, 
                            I32_WMV iStepSize, I32_WMV iMBsPerRow, Bool_WMV bFirst, Bool_WMV bLast);
Void_WMV FilterEdgeShortTagMBRow (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, 
                            I32_WMV iStepSize, I32_WMV iMBsPerRow, Bool_WMV bFirst, Bool_WMV bLast);

I32_WMV divroundnearest(I32_WMV i, I32_WMV iDenom);

// Block based routines
tWMVDecodeStatus DecodeIMBAcPred ( tWMVDecInternalMember *pWMVDec, 
                                                    CWMVMBMode* pmbmd,
                                                    CWMVMBMode* pmbmd_lastrow, 
                                                    U8_WMV*  ppxliTextureQMBY, 
                                                    U8_WMV*  ppxliTextureQMBU, 
                                                    U8_WMV*  ppxliTextureQMBV, 
                                                    I16_WMV* piQuanCoefACPred, 
                                                    I16_WMV** piQuanCoefACPredTable, 
                                                    Bool_WMV bLeftMB, 
                                                    Bool_WMV bTopMB, 
                                                    Bool_WMV bLeftTopMB,
                                                    I32_WMV iMBX);

// decode MB header 
tWMVDecodeStatus decodeMBOverheadOfIVOP_MSV (tWMVDecInternalMember *pWMVDec, 
                                                                        CWMVMBMode* pmbmd,
                                                                        CWMVMBMode* pmbmd_lastrow,
                                                                        I32_WMV imbX, I32_WMV imbY);

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred (tWMVDecInternalMember *pWMVDec,
                                                                                CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                                                                Bool_WMV CBP, 
                                                                                I16_WMV* pDct, 
                                                                                U8_WMV* piZigzagInv );

tWMVDecodeStatus decodeIntraBlockAcPred (tWMVDecInternalMember *pWMVDec, 
                                                            CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                                            CWMVMBMode* pmbmd,
                                                            U8_WMV *ppxliTextureQMB,        //place to set the pixels
                                                            I32_WMV iOffsetToNextRowForDCT,
                                                            I32_WMV ib, 
                                                            I16_WMV* piQuanCoef1stRowColAC, 
                                                            I16_WMV* pRef, 
                                                            Bool_WMV bPredPattern,
                                                            I32_WMV iMBX );

Void_WMV decodeBitsOfESCCode (tWMVDecInternalMember *pWMVDec);

// AC_DC Pred    
I16_WMV* decodeDiffDCTCoef( tWMVDecInternalMember *pWMVDec, 
                                        I16_WMV**   piQuanCoefACPredTable,
                                        Bool_WMV    bLeftMB,
                                        Bool_WMV    bTopMB,
                                        Bool_WMV    bLeftTopMB, 
                                        Bool_WMV*   pbPredPattern );

Void_WMV decodeIntraDCAcPredMSV (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCTStepSize);

VResultCode Huffman_WMV_init (tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis, tPackHuffmanCode_WMV huffArray[], U8_WMV tableSize);
VResultCode Huffman_WMV_init16 (I16_WMV *pDecodeTable, tPackHuffmanCode_WMV huffArray[], I32_WMV *pSymbolInfo, U32_WMV iRootBits);

U32_WMV getHuffman_C(CInputBitStream_WMV *bs,
                           I16_WMV* pDecodeTable, 
                           U32_WMV  iRootBits);
U32_WMV getHuffman_ARMV4(CInputBitStream_WMV *bs,
                           I16_WMV* pDecodeTable, 
                           U32_WMV  iRootBits);


FORCEINLINE U32_WMV Huffman_WMV_get(Huffman_WMV * pThis, CInputBitStream_WMV *bs)
{
    U32_WMV ret = getHuffman(bs, pThis->m_hufDecTable,(U32_WMV)pThis->m_tableSize);
    return ret;
}
#if 0
Void_WMV BS_construct (CInputBitStream_WMV * pThis, U32_WMV uiUserData);
#endif
Void_WMV BS_reset( CInputBitStream_WMV * pThis,
                            U8_WMV *pBuffer,
                            U32_WMV dwBuflen,
                            Bool_WMV bNotEndOfFrame,
                            Bool_WMV bStartcodePrevention );

tWMVDecodeStatus  Plane_init (tYUV420Frame_WMV * pFramePlane, 
                                        I32_WMV iSizeYplane, 
                                        I32_WMV iSizeUVplane, 
                                        I32_WMV iPad, 
                                        tWMVDecInternalMember *pWMVDec, 
                                        I32_WMV iPlaneWidth,
                                        I32_WMV iPlaneHeight); 

I32_WMV PredictFieldTopMV (tWMVDecInternalMember *pWMVDec, I32_WMV *iPredX, I32_WMV *iPredY, 
        I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);
I32_WMV PredictFieldBotMV (tWMVDecInternalMember *pWMVDec, I32_WMV *iPredX, I32_WMV *iPredY, 
        I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);
I32_WMV PredictFieldOneMV (tWMVDecInternalMember *pWMVDec, I32_WMV *iPredX, I32_WMV *iPredY, 
        I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);

Void_WMV InitFieldInterpolation (tWMVDecInternalMember *pWMVDec);

Void_WMV RL_ResetLibrary(CReferenceLibrary *);
tWMVDecodeStatus RL_SetupReferenceLibrary (tWMVDecInternalMember *pWMVDec,I32_WMV size_y, I32_WMV size_uv, I32_WMV, I32_WMV iPad, I32_WMV iPlaneWidth, I32_WMV iPlaneHeight);
    
Void_WMV RL_CloseReferenceLibrary(tWMVDecInternalMember * pWMVDec);
Void_WMV RL_RemoveFrame (CReferenceLibrary *,Void_WMV**, I32_WMV);


Void_WMV InitMultires(tWMVDecInternalMember * pWMVDec);
Void_WMV SetupMultiResParams(tWMVDecInternalMember *pWMVDec);
Void_WMV ResetParams(tWMVDecInternalMember *pWMVDec, I32_WMV iResIndex);

Void_WMV UpsampleRefFrame(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV);
Void_WMV DownsampleRefFrame(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV);

Void_WMV ResetDecoderParams (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus DecodeSkipBitX9 (tWMVDecInternalMember * pWMVDec, I32_WMV iMode);

tWMVDecodeStatus decodeVOPHead_WMV3(tWMVDecInternalMember * pWMVDec);
Void_WMV InitBlockInterpolation (tWMVDecInternalMember * pWMVDec);

tWMVDecodeStatus decodeVOLHeadVC1(tWMVDecInternalMember * pWMVDec, 
                                                    U32_WMV uiFormatLength, 
                                                    I32_WMV *piPIC_HORIZ_SIZE ,   
                                                    I32_WMV *piPIC_VERT_SIZE,
                                                    I32_WMV *piDISP_HORIZ_SIZE ,
                                                    I32_WMV *piDISP_VERT_SIZE );

//Void_WMV ReInitContext(t_CContextWMV *pContext, I32_WMV iCol, I32_WMV iRow);

Void_WMV decodeMV_V9(tWMVDecInternalMember *pWMVDec, 
                                Huffman_WMV * pHufMVTable,
                                I32_WMV iMode, 
                                CDiffMV *pDiffMV);
Void_WMV DecodeVOPDQuant (tWMVDecInternalMember *pWMVDec, Bool_WMV bPFrame);
I32_WMV decodeDQuantStepsize(tWMVDecInternalMember *pWMVDec, I32_WMV iShift);



tWMVDecodeStatus DecodeMB_1MV_WMVA_Intra (tWMVDecInternalMember *pWMVDec,
                                                                    CWMVMBMode* pmbmd, 
                                                                    CWMVMBMode* pmbmd_lastrow,
                                                                    U8_WMV * ppxlcCurrQYMB,
                                                                    U8_WMV * ppxlcCurrQUMB,
                                                                    U8_WMV * ppxlcCurrQVMB,
                                                                    I32_WMV  iMBX, I32_WMV iMBY);

tWMVDecodeStatus CopyCurrToPost (tWMVDecInternalMember *pWMVDec);  

// member functions for X9
//Void_WMV IntensityCompensation (tWMVDecInternalMember *pWMVDec);

Void_WMV InitBlockInterpolation (tWMVDecInternalMember *pWMVDec);

Bool_WMV  decodeDCTPredictionY(tWMVDecInternalMember *pWMVDec,
                                            CWMVMBMode* pmbmd,
                                            CWMVMBMode* pmbmd_lastrow,
                                            I16_WMV* pIntra, 
                                            I16_WMV* pIntra_lastrow,
                                            I32_WMV iBlk,
                                            I32_WMV iX,     I32_WMV iY, 
                                            I32_WMV   *piShift, 
                                            I32_WMV * p_iDirection, 
                                            I16_WMV * pPredScaled);

Bool_WMV  decodeDCTPredictionY_IFrameNoDQuantAndNoSlice(tWMVDecInternalMember *pWMVDec,
                                                                                    CWMVMBMode* pmbmd,
                                                                                    I16_WMV* pIntra, 
                                                                                    I16_WMV* pIntra_lastrow,
                                                                                    I32_WMV iBlk,
                                                                                    I32_WMV iX, 
                                                                                    I32_WMV iY, 
                                                                                    I32_WMV   *piShift,
                                                                                    I32_WMV * p_iDirection,
                                                                                    I16_WMV ** pPredScaled);

tWMVDecodeStatus DecodeInverseIntraBlockX9(tWMVDecInternalMember *pWMVDec,
                                                           CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                                           I32_WMV iblk,       
                                                           Bool_WMV bResidual,
                                                           I16_WMV *pPred, 
                                                           I32_WMV iDirection,
                                                           I32_WMV iShift,
                                                           I16_WMV *pIntra,
                                                           CWMVMBMode* pmbmd, 
                                                           DQuantDecParam *pDQ);


Bool_WMV decodeDCTPredictionUV(tWMVDecInternalMember *pWMVDec, 
                                            CWMVMBMode* pmbmd, 
                                            CWMVMBMode* pmbmd_lastrow,
                                            I16_WMV*     pIntra, 
                                            I16_WMV*     pIntra_lastrow,
                                            I32_WMV iX,    
                                            I32_WMV iY, 
                                            I32_WMV     *piShift,  
                                            I32_WMV * p_iDirection,
                                            I16_WMV * pPredScaled);

Bool_WMV decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice(tWMVDecInternalMember *pWMVDec, 
                                                                                    CWMVMBMode* pmbmd,
                                                                                    I16_WMV*     pIntra, 
                                                                                    I16_WMV*     pIntra_lastrow,
                                                                                    I32_WMV iX,  
                                                                                    I32_WMV iY, 
                                                                                    I32_WMV *piShift, 
                                                                                    I32_WMV * p_iDirection, 
                                                                                    I16_WMV ** pPredScaled);


I32_WMV medianof3_C (I32_WMV a0, I32_WMV a1, I32_WMV a2);
I32_WMV medianof3_ARMV4 (I32_WMV a0, I32_WMV a1, I32_WMV a2);


#ifdef _ARM_
#define medianof3     medianof3_ARMV4
#else
#define medianof3    medianof3_C
#endif


Void_WMV ComputeDQuantDecParam (tWMVDecInternalMember *pWMVDec);
Void_WMV ScaleDCPredForDQuant (tWMVDecInternalMember *pWMVDec,
                                            I32_WMV iBlk,
                                            CWMVMBMode* pmbmd, 
                                            CWMVMBMode* pmbmd_last,
                                            I32_WMV * p_iTopDC, 
                                            I32_WMV * p_iLeftDC, 
                                            I32_WMV * p_iTopLeftDC);

Void_WMV ScaleTopPredForDQuant (tWMVDecInternalMember *pWMVDec,
                                                I32_WMV iBlk, 
                                                I16_WMV *pPred,    
                                                I16_WMV *pPredScaled,
                                                CWMVMBMode* pmbmd,
                                                CWMVMBMode* pmbmd_lastrow);

Void_WMV ScaleLeftPredForDQuant (tWMVDecInternalMember *pWMVDec,
                                                I32_WMV iBlk, 
                                                I16_WMV *pPred,
                                                I16_WMV *pPredScaled, 
                                                CWMVMBMode* pmbmd);

tWMVDecodeStatus decodeDQuantParam (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);

tWMVDecodeStatus decodeIInterlace411 (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus decodeFrameIMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);

tWMVDecodeStatus decodeFieldIMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);

tWMVDecodeStatus DecodeFrameIMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV);

tWMVDecodeStatus DecodeTopFieldIMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV);

tWMVDecodeStatus DecodeBotFieldIMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV);

Void_WMV inverseIntraBlockQuantizeInterlace411 (tWMVDecInternalMember *pWMVDec, I16_WMV *piCoefRecon, I32_WMV iNumCoef, DQuantDecParam *pDQ);

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeInterlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, U8_WMV* piZigzagInv, U32_WMV uiNumCoef);

tWMVDecodeStatus decodeIntraDCInterlace411 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *piDC, Huffman_WMV * hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCStepSize);

Void_WMV ScaleDCPredUVForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iSubblock, CWMVMBMode* pmbmd, I32_WMV * piTopDC, I32_WMV * piLeftDC, I32_WMV *piTopLeftDC);

Void_WMV ScaleLeftPredUVForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iSubblock, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd);

Void_WMV ScaleTopPredUVForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iSubblock, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd);

Void_WMV ScaleDCPredYForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, CWMVMBMode* pmbmd, I32_WMV *piTopDC, I32_WMV *piLeftDC, I32_WMV *piTopLeftDC);

Void_WMV ScaleLeftPredYForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd);

Void_WMV ScaleTopPredYForDQuantInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd);


Bool_WMV decodeDCACPredYInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, 
    CWMVMBMode* pmbmd, Bool_WMV bAcPredOn, I16_WMV* piLevelBlk);

Void_WMV decodeDCACPredUVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, 
    I32_WMV iSubblock, CWMVMBMode* pmbmd, Bool_WMV bAcPredOn, I16_WMV* piLevelBlk);


tWMVDecodeStatus DecodeInterBlockYInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk, CWMVMBMode *pmbmd,
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ);

Void_WMV StoreDCACPredCoefYInterlace411 (tWMVDecInternalMember *pWMVDec, 
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I16_WMV *piLevelBlk);

Void_WMV StoreDCACPredCoefUVInterlace411 (tWMVDecInternalMember *pWMVDec, 
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I32_WMV iSubblock, I16_WMV *piLevelBlk); 

    //
    // Inter 411
    //

tWMVDecodeStatus decodePInterlace411 (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus DecodeTopFieldPMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB,U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB);

tWMVDecodeStatus DecodeBotFieldPMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB,U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB);

tWMVDecodeStatus DecodeFramePMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB,U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB);

tWMVDecodeStatus decodeFieldPMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY);

tWMVDecodeStatus decodeFramePMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY);

I32_WMV ComputeFrameMVFromDiffMVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);

I32_WMV ComputeTopFieldMVFromDiffMVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);

I32_WMV ComputeBotFieldMVFromDiffMVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);

Void_WMV DeblockSLFrame_V9 (tWMVDecInternalMember *pWMVDec, U32_WMV uiMBStart, U32_WMV uiMBEnd);

Void_WMV FilterRow_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize);
Void_WMV FilterRowLast8x4_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize);
Void_WMV FilterColumn_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize);
Void_WMV FilterColumnLast4x8_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize);

extern void ARMV6_g_IDCTDec_WMV3_Pass1_Naked(const I32_WMV* piSrc0, I32_WMV* tmpBuffer, const I32_WMV iNumLoops, I32_WMV iDCTHorzFlags);

#ifdef VOARMV6
Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV);
Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumRows, U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV);
#else
Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, int bottom_row,
        U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, I32_WMV iNumRows, 
                      U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
                      U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4);


Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, I32_WMV iNumCols, I32_WMV iHeight, 
                         U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
						 U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV);
#endif

Void_WMV GenerateColumnLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* puchFilterColFlagY, U8_WMV* puchFilterColFlagU, U8_WMV* puchFilterColFlagV,
        U8_WMV* puchFilterColFlagY2, U8_WMV* puchFilterColFlagU2, U8_WMV* puchFilterColFlagV2,
        LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight);

Void_WMV GenerateColumnLoopFilterFlags_Y (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* puchFilterColFlagY, U8_WMV* puchFilterColFlagY2,
        LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight);

Void_WMV GenerateColumnLoopFilterFlags_UV (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* puchFilterColFlagU, U8_WMV* puchFilterColFlagV,
        U8_WMV* puchFilterColFlagU2, U8_WMV* puchFilterColFlagV2,
        LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight);

Void_WMV GenerateRowLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* puchFilterRowFlagY, U8_WMV* puchFilterRowFlagU, U8_WMV* puchFilterRowFlagV,
        U8_WMV* puchFilterRowFlagY2, U8_WMV* puchFilterRowFlagU2, U8_WMV* puchFilterRowFlagV2,
        LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBYStart, I32_WMV iMBYEnd, Bool_WMV bFirstRow);

Void_WMV GenerateRowLoopFilterFlags_Y (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* puchFilterRowFlagY ,
        U8_WMV* puchFilterRowFlagY2 ,
        LOOPF_FLAG* pLoopFilterFlag, Bool_WMV bFirstRow);

Void_WMV GenerateRowLoopFilterFlags_UV (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* puchFilterRowFlagU, U8_WMV* puchFilterRowFlagV,
        U8_WMV* puchFilterRowFlagU2, U8_WMV* puchFilterRowFlagV2,
        LOOPF_FLAG* pLoopFilterFlag, Bool_WMV bFirstRow);


Void_WMV ClearFieldMBInterlace411 (I16_WMV *ppxliFieldMB, I32_WMV iField);


Void_WMV CopyIntraFieldMBtoTopFieldInterlace411 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV CopyIntraFieldMBtoBotFieldInterlace411 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

    // older ones....
Void_WMV CopyIntraFieldMBtoFrameMBInterlace411 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetToNextRowY, I32_WMV iOffsetToNextRowUV, 
    Bool_WMV bSeparateField);

I32_WMV ChromaMVInterlace411 (    tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, I32_WMV iMvX, I32_WMV iMvY, I32_WMV *iChromaMvX, I32_WMV *iChromaMvY);

Void_WMV LoopFilterIFrameInterlace411 (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV);

Void_WMV MotionCompFrameInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV);

Void_WMV MotionCompTopFieldInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV);

Void_WMV MotionCompBotFieldInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV);

Void_WMV AddErrorFrameInterlace411 (tWMVDecInternalMember *pWMVDec, 
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV AddErrorTopFieldInterlace411 (tWMVDecInternalMember *pWMVDec, 
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV AddErrorBotFieldInterlace411 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV SetDefaultDQuantSetting(tWMVDecInternalMember *pWMVDec);

//tWMVDecodeStatus DecodeInverseInterBlockQuantize16 (tWMVDecInternalMember *pWMVDec,  CDCTTableInfo_Dec** InterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode, DQuantDecParam *pDQ);
Void_WMV LoopFilterPFrameDecInterlace411 (tWMVDecInternalMember *pWMVDec,
                                                        U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, CWMVMBMode *pmbmd);

Void_WMV CloseThreads (tWMVDecInternalMember * pWMVDec);

Void_WMV SetMVRangeFlag (tWMVDecInternalMember *pWMVDec, I32_WMV iRangeIndex);

I32_WMV ComputePredCBPCY (tWMVDecInternalMember *pWMVDec,
                                        CWMVMBMode* pmbmd,
                                        I32_WMV imbX, 
                                        I32_WMV imbY, 
                                        I32_WMV iCBPCY); 

I32_WMV DecodeIntraBlockYInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode *pmbmd, 
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, DQuantDecParam *pDQ,
    Bool_WMV bAcPredOn);

I32_WMV DecodeIntraBlockUVInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I32_WMV iSubblock, CWMVMBMode *pmbmd, 
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ,
    Bool_WMV bAcPredOn);

Void_WMV AdjustDecRange (tWMVDecInternalMember *pWMVDec);
Void_WMV AdjustDecRangeWMVA (tWMVDecInternalMember *pWMVDec);

Void_WMV  OverlapMBRow (tWMVDecInternalMember * pWMVDec, 
                                    I32_WMV imbY, 
                                    U8_WMV *ppxliRecnY, 
                                    U8_WMV *ppxliRecnU,
                                    U8_WMV *ppxliRecnV, 
                                    Bool_WMV bTopRowOfSlice, 
                                    Bool_WMV bBottomRowOfSlice);

 //Void_WMV AdjustReconRange (tWMVDecInternalMember *pWMVDec);
 Void_WMV SwapPostAndPostPrev(tWMVDecInternalMember *pWMVDec);

Void_WMV DirectModeMV (tWMVDecInternalMember * pWMVDec, I32_WMV iXMotion, I32_WMV iYMotion, Bool_WMV bHpelMode,
                               I32_WMV imbX, I32_WMV imbY,
                               I32_WMV *idfx, I32_WMV *idfy, I32_WMV *idbx, I32_WMV *idby); 

Bool_WMV PullBackMotionVector (tWMVDecInternalMember *pWMVDec,
                                            I32_WMV *iMvX, 
                                            I32_WMV *iMvY, 
                                            I32_WMV imbX,
                                            I32_WMV imbY);

Void_WMV swapRef0Ref1Pointers (tWMVDecInternalMember *pWMVDec);
Void_WMV swapCurrRef1Pointers (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus SliceStartCode(tWMVDecInternalMember *pWMVDec, I32_WMV imbY);

Void_WMV SetTransformTypeHuffmanTable (tWMVDecInternalMember *pWMVDec, I32_WMV iStep);

tWMVDecodeStatus decodeSkipP (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus decodeFrameBMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);

Void_WMV SetMotionFieldBFrame (I16_WMV *pMvX, I16_WMV *pMvY,
                           I16_WMV *pFMvX, I16_WMV *pFMvY,
                           I16_WMV *pBMvX, I16_WMV *pBMvY,
                           I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX, I32_WMV iBotMvY,
                           I32_WMV iFTopMvX, I32_WMV iFTopMvY, I32_WMV iFBotMvX, I32_WMV iFBotMvY,
                           I32_WMV iBTopMvX, I32_WMV iBTopMvY, I32_WMV iBBotMvX, I32_WMV iBBotMvY,
                           I32_WMV iTopLeftBlkIndex, I32_WMV iBotLeftBlkIndex );


I32_WMV ComputeFrameMVFromDiffMVInterlace411_BFrame (tWMVDecInternalMember *pWMVDec, 
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV, I16_WMV *m_pXTrue, I16_WMV *m_pYTrue, I16_WMV *m_pXPred, I16_WMV *m_pYPred);

tWMVDecodeStatus DecodeFrameBMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB,
    U8_WMV *ppxlcRef1QYMB, U8_WMV *ppxlcRef1QUMB, U8_WMV *ppxlcRef1QVMB, I32_WMV iscaleFactor);

tWMVDecodeStatus decodeFieldBMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);

I32_WMV ComputeTopFieldMVFromDiffMVInterlace411_BFrame ( tWMVDecInternalMember * pWMVDec, 
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV, I16_WMV *m_pXTrue, I16_WMV *m_pYTrue, I16_WMV *m_pXPred, I16_WMV *m_pYPred);

I32_WMV ComputeBotFieldMVFromDiffMVInterlace411_BFrame ( tWMVDecInternalMember *  pWMVDec, 
    I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV, I16_WMV *m_pXTrue, I16_WMV *m_pYTrue, I16_WMV *m_pXPred, I16_WMV *m_pYPred);

tWMVDecodeStatus DecodeTopFieldBMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB,
    U8_WMV *ppxlcRef1QYMB, U8_WMV *ppxlcRef1QUMB, U8_WMV *ppxlcRef1QVMB, I32_WMV iscaleFactor);

tWMVDecodeStatus DecodeBotFieldBMBInterlace411 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd,
    U8_WMV *ppxlcCurrQYMB, U8_WMV *ppxlcCurrQUMB, U8_WMV *ppxlcCurrQVMB,
    U8_WMV *ppxlcRefQYMB, U8_WMV *ppxlcRefQUMB, U8_WMV *ppxlcRefQVMB,
    U8_WMV *ppxlcRef1QYMB, U8_WMV *ppxlcRef1QUMB, U8_WMV *ppxlcRef1QVMB, I32_WMV iscaleFactor);

tWMVDecodeStatus Decode4x8Interlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam* pDQ, 
    I16_WMV *ppxliDst, U32_WMV iDstOffset);

I32_WMV Decode8x4Interlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam* pDQ, I16_WMV *ppxliDst);

I32_WMV Decode8x8Interlace411 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam* pDQ, I16_WMV *ppxliDst);

I32_WMV MotionCompFrameInterlace411_BFrame (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV, U8_WMV *ppxliRefY1, U8_WMV *ppxliRefU1, U8_WMV *ppxliRefV1,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, MBType mbType);

Void_WMV MotionCompTopFieldInterlace411_BFrame (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV, U8_WMV *ppxliRefY1, U8_WMV *ppxliRefU1, U8_WMV *ppxliRefV1,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, MBType mbType);

Void_WMV MotionCompBotFieldInterlace411_BFrame (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY,
    U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV, U8_WMV *ppxliRefY1, U8_WMV *ppxliRefU1, U8_WMV *ppxliRefV1,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, MBType mbType);


tWMVDecodeStatus decodeBInterlace411 (tWMVDecInternalMember *pWMVDec);
//Void_WMV CopyRefToPrevPost (tWMVDecInternalMember * pWMVDec);
I32_WMV HandleResolutionChange (tWMVDecInternalMember * pWMVDec);
Void_WMV SwapMultiresAndPost(tWMVDecInternalMember *pWMVDec);
Void_WMV ChromaMV_B (tWMVDecInternalMember * pWMVDec, I32_WMV *iX, I32_WMV *iY);
//
// InterlaceV2 frame header
//

 I32_WMV decodeVOPHeadInterlaceV2 (tWMVDecInternalMember *pWMVDec);
Void_WMV SwapMVTables (tWMVDecInternalMember *pWMVDec);
//
// InterlaceV2 intra
//

tWMVDecodeStatus decodeIInterlaceV2 (tWMVDecInternalMember *pWMVDec);

Void_WMV decodePredictIntraCBP(tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd,CWMVMBMode* pmbmd_lastrow, I32_WMV imbX, I32_WMV imbY);

I32_WMV decodeIMBOverheadInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);

Void_WMV PerformACPredictionInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, CWMVMBMode *pmbmd, I16_WMV *piLevelBlk, I16_WMV *rgiPredCoef,
    DCACPREDDIR iPredDir);

I32_WMV DecodeIntraBlockInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode *pmbmd, 
    CDCTTableInfo_Dec** ppDCTTableInfo_Dec, DQuantDecParam *pDQ,
    Bool_WMV bAcPredOn);

        

I32_WMV DecodeIMBInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode *pmbmd, U8_WMV *ppxliTextureQMBY, 
    U8_WMV *ppxliTextureQMBU, U8_WMV *ppxliTextureQMBV, I16_WMV *ppxliErrorY,
    I16_WMV *ppxliErrorU, I16_WMV *ppxliErrorV);

Void_WMV InvOverlapInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, CWMVMBMode* pmbmd, 
    U8_WMV* ppxliCurrRecnMBY, U8_WMV* ppxliCurrRecnMBU, U8_WMV* ppxliCurrRecnMBV,
    I16_WMV *ppxliErrorQY, I16_WMV *ppxliErrorQU, I16_WMV *ppxliErrorQV,
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliIntraRowY, I16_WMV *ppxliIntraRowU, 
    I16_WMV *ppxliIntraRowV);    

I32_WMV DecodeInverseIntraBlockQuantizeInterlaceV2 (tWMVDecInternalMember *pWMVDec,
            CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, U8_WMV* piZigzagInv, 
                    I16_WMV *rgiCoefRecon, U32_WMV uiNumCoef);

I32_WMV decodeIntraDCInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *piDC, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCStepSize);

DCACPREDDIR  ScaleDCPredForDQuantInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV iBlk, CWMVMBMode* pmbmd, I32_WMV iTopDC, I32_WMV iLeftDC, I32_WMV iTopLeftDC);

DCACPREDDIR SetIBlockBoundaryForDCPred (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode* pmbmd, 
    I16_WMV*pPredIntraL, I16_WMV* pPredIntraT);

Void_WMV decodeDCACPredInterlaceV2 (tWMVDecInternalMember *pWMVDec,
   I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode* pmbmd,
   DCACPREDDIR *iPredDir, I16_WMV *rgiPredCoef,I32_WMV bAcPredOn);

Void_WMV StoreDCACPredCoefInterlaceV2 (tWMVDecInternalMember *pWMVDec,
            I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I16_WMV *piLevelBlk);

// End of InterlaceV2 intra

// InterlaceV2 Core
Void_WMV CopyIntraFieldMBtoFieldMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV CopyIntraFieldMBtoFrameMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
    U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV CopyIntraFieldMBtoOverlapFieldMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliMBY, I16_WMV *ppxliMBU, 
    I16_WMV *ppxliMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV CopyIntraFieldMBtoOverlapFrameMBInterlaceV2 (
    I16_WMV *ppxliFieldMB, I16_WMV *ppxliMBY, I16_WMV *ppxliMBU, 
    I16_WMV *ppxliMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);



Void_WMV LoopFilterRowIFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    CWMVMBMode *pmbmd);

Void_WMV LoopFilterColIFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    CWMVMBMode *pmbmd);

// End of InterlaceV2 Core

// Interlace V2B
	Void_WMV MotionCompSingleFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec,
	I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
	U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV,
	I32_WMV iReconStrideY, I32_WMV iReconStrideUV, I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX,
	I32_WMV iBotMvY, I32_WMV iBot);

Void_WMV ComputeFrameMVFromDiffMVInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);
Void_WMV ComputeFieldMVFromDiffMVInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);

tWMVDecodeStatus decodeBInterlaceV2 (tWMVDecInternalMember *pWMVDec);

//I32_WMV DecodeBMBInterlaceV2 (tWMVDecInternalMember *pWMVDec,I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd);
Void_WMV MotionCompFrameInterlaceV2MV (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, 
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, I32_WMV iMvX, I32_WMV iMvY);
Void_WMV MotionCompFieldInterlaceV2MV (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV,
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX,
    I32_WMV iBotMvY);
Void_WMV MotionCompFrameInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRef0Y, U8_WMV *ppxliRef0U, U8_WMV *ppxliRef0V,
    U8_WMV *ppxliRef1Y, U8_WMV *ppxliRef1U, U8_WMV *ppxliRef1V,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV, 
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, MBType mbType);
Void_WMV MotionCompFieldInterlaceV2B (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRef0Y, U8_WMV *ppxliRef0U, U8_WMV *ppxliRef0V,
    U8_WMV *ppxliRef1Y, U8_WMV *ppxliRef1U, U8_WMV *ppxliRef1V,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV,
    I32_WMV iReconStrideY, I32_WMV iReconStrideUV, MBType mbType);
Void_WMV AssignDirectMode(tWMVDecInternalMember *pWMVDec,I32_WMV imbX, I32_WMV imbY, I32_WMV iTopLeftBlkIndex, I32_WMV iBotLeftBlkIndex, Bool_WMV bFieldMode);


//
// Interlace V2 P
//
tWMVDecodeStatus decodePInterlaceV2 (tWMVDecInternalMember *pWMVDec);

I32_WMV DecodePBlockInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk, CWMVMBMode *pmbmd,
    CDCTTableInfo_Dec **ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, Bool_WMV *pbFirstBlock);

I32_WMV DecodePMBInterlaceV2 (tWMVDecInternalMember *pWMVDec,I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd);

    I32_WMV DecodeInverseInterBlockQuantizeInterlaceV2 (tWMVDecInternalMember *pWMVDec,
                CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, 
                        DQuantDecParam *pDQ, U32_WMV uiNumCoef);

I32_WMV decodePMBOverheadInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, Bool_WMV bCBPPresent, Bool_WMV b1MVPresent);

I32_WMV decodeBMBOverheadInterlaceV2 (tWMVDecInternalMember *pWMVDec,
	CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, Bool_WMV bCBPPresent, Bool_WMV b1MVPresent);

Void_WMV ComputeFrameMVFromDiffMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);

Void_WMV ComputeFieldMVFromDiffMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);

Void_WMV ComputeFrame4MVFromDiffMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);

Void_WMV ComputeField4MVFromDiffMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);
    // End of Interlace V2 P

// Interlace V2 P Core
Void_WMV LoopFilterRowPFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, 
    CWMVMBMode *pmbmd);

Void_WMV LoopFilterColPFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    U32_WMV uiMBStart, U32_WMV uiMBEnd, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, 
    CWMVMBMode *pmbmd);

Void_WMV ChromaMVInterlaceV2 (
    I32_WMV imbX, I32_WMV imbY, I32_WMV iMvX, I32_WMV iMvY, I32_WMV *iChromaMvX, I32_WMV *iChromaMvY,
    Bool_WMV bField);

Void_WMV MotionCompFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV);

Void_WMV MotionCompFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV);

Void_WMV MotionCompFrame4MvInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV);

Void_WMV MotionCompField4MvInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
    U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV);

Void_WMV AddErrorFrameInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

Void_WMV AddErrorFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    I16_WMV *ppxliFieldMB, U8_WMV *ppxlcCurrY, U8_WMV *ppxlcCurrU, 
    U8_WMV *ppxlcCurrV, I32_WMV iOffsetY, I32_WMV iOffsetUV);
// helper fns

Void_WMV AverageFieldMv (tWMVDecInternalMember *pWMVDec,I32_WMV* iAvgMvX, I32_WMV* iAvgMvY, I16_WMV 
    *pFieldMvX, I16_WMV *pFieldMvY, I32_WMV iCurrIndex, I32_WMV iColumn);

Void_WMV ComputeFieldMvPredictorFromNeighborMv (
    I32_WMV *piValidPredMvX, I32_WMV *piValidPredMvY, I32_WMV iTotalValidMv, I32_WMV *piMvX, I32_WMV *piMvY);

Void_WMV ComputeFrameMvPredictorFromNeighborMv (
    I32_WMV *piValidPredMvX, I32_WMV *piValidPredMvY, I32_WMV iTotalValidMv, I32_WMV *piMvX, I32_WMV *piMvY);

// frame
Void_WMV PredictFrameOneMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);

// frame 4mv
Void_WMV PredictFrameTopLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);

Void_WMV PredictFrameTopRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);

Void_WMV PredictFrameBotLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);

Void_WMV PredictFrameBotRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);

// field 
Void_WMV PredictFieldTopMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);
Void_WMV PredictFieldBotMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);

// field 4mv
Void_WMV PredictFieldTopLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);
Void_WMV PredictFieldTopRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);
Void_WMV PredictFieldBotLeftMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);
Void_WMV PredictFieldBotRightMVInterlaceV2 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV* iPredX, I32_WMV* iPredY, 
    I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV bTopBndry);


I32_WMV medianof4_C (I32_WMV a0, I32_WMV a1, I32_WMV a2, I32_WMV a3);
I32_WMV medianof4_ARMV4 (I32_WMV a0, I32_WMV a1, I32_WMV a2, I32_WMV a3);


#ifdef _ARM_
#define medianof4     medianof4_ARMV4
#else
#define medianof4    medianof4_C
#endif

I32_WMV DecodeFrac(tWMVDecInternalMember *pWMVDec, const I32_WMV iVal, const Bool_WMV bLong);

Void_WMV  setCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) ;

I32_WMV DecodePMBModeInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
                        CWMVMBMode* pmbmd, Bool_WMV *pbCBPPresent, Bool_WMV *pb1MVPresent);


Void_WMV decodeMV_Progressive(tWMVDecInternalMember * pWMVDec, Huffman_WMV* pHufMVTable, CDiffMV *pDiffMV);
Bool_WMV PullBackFrameMBMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY);
Bool_WMV PullBackFrameBlockMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk);
Bool_WMV PullBackFieldMBMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iField);
Bool_WMV PullBackFieldBlockMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk);


I32_WMV decodeSequenceHead_Advanced (tWMVDecInternalMember *pWMVDec,  U32_WMV uiFormatLength, 
                                 I32_WMV *piPIC_HORIZ_SIZE , I32_WMV *piPIC_VERT_SIZE ,
                                 I32_WMV *piDISP_HORIZ_SIZE , I32_WMV *piDISP_VERT_SIZE );

Void_WMV decideMMXRoutines_ZigzagOnly (tWMVDecInternalMember * pWMVDec);

I32_WMV decodeVOPHeadProgressiveWMVA (tWMVDecInternalMember * pWMVDec);
Void_WMV DecodeVOPType_WMVA(tWMVDecInternalMember *pWMVDec);
Void_WMV DecodeVOPType_WMVA2(tWMVDecInternalMember *pWMVDec);

Void_WMV Repeatpad (tWMVDecInternalMember *pWMVDec);

I32_WMV SwitchSequence (tWMVDecInternalMember *pWMVDec, Bool_WMV bDecodeVOLHead);
Void_WMV FreeFrameAreaDependentMemory (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus initSeqIndependentMemory(tWMVDecInternalMember *pWMVDec);
I32_WMV initFrameWidthAndHeightDependentVariables(tWMVDecInternalMember *pWMVDec, I32_WMV iWidthSource, I32_WMV iHeightSource);
Void_WMV initFrameWidthAndHeightDependentPointers(tWMVDecInternalMember *pWMVDec);
I32_WMV initFrameAreaDependentMemory(tWMVDecInternalMember *pWMVDec,
                                                        I32_WMV iMaxEncWidth,
                                                        I32_WMV iMaxEncHeight);
I32_WMV AllocatePictureCYUV420 (tWMVDecInternalMember *pWMVDec,
                                            I32_WMV iMaxEncWidth, 
                                            I32_WMV iMaxEncHeight);

I32_WMV initVlcTables(tWMVDecInternalMember *pWMVDec);
I32_WMV DecodeEntryPointHeader (tWMVDecInternalMember *pWMVDec, I32_WMV *piPIC_HORIZ_SIZE, I32_WMV *piPIC_VERT_SIZE);
I32_WMV SwitchEntryPoint (tWMVDecInternalMember *pWMVDec);


I32_WMV ReallocateStartCodeBuffer(tWMVDecInternalMember *pWMVDec, U32_WMV nOldBufferSize, U32_WMV nNewBufferSize);
I32_WMV ParseStartCode ( tWMVDecInternalMember *pWMVDec, U8_WMV *  pBuffer, U32_WMV    dwBufferLen, U8_WMV *  *ppOutBuffer, U32_WMV    *pdwOutBufferLen, Bool_WMV *pbNotEndOfFrame);

I32_WMV SetSliceWMVA ( tWMVDecInternalMember *pWMVDec, Bool_WMV bSliceWMVA, U32_WMV uiFirstMBRow, U32_WMV uiNumBytesSlice, Bool_WMV bSecondField);

I32_WMV ReadSliceHeaderWMVA (tWMVDecInternalMember *pWMVDec,I32_WMV imbY );

I32_WMV ParseStartCodeChunk ( tWMVDecInternalMember *pWMVDec, U8_WMV *  pBuffer, U32_WMV    dwBufferLen, U8_WMV *  *ppOutBuffer, U32_WMV    *pdwOutBufferLen, Bool_WMV bNotEndOfFrame);
I32_WMV DecodePanScanInfo (tWMVDecInternalMember *pWMVDec, CPanScanInfo *pPanScanInfo, I32_WMV *piNumWindowsPresent);

Void_WMV ResetConditionalVariablesForSequenceSwitch (tWMVDecInternalMember *pWMVDec);

I32_WMV AllocateMultiThreadBufs_Dec_WMVA (tWMVDecInternalMember *pWMVDec, 
                                                            I32_WMV iMaxEncWidth,
                                                            I32_WMV iMaxEncHeight);

//Void_WMV initMultiThreadVars_Dec_WMVA (tWMVDecInternalMember *pWMVDec);

//Void_WMV PreGetOutput (tWMVDecInternalMember *pWMVDec);


//Multires global functions
Void_WMV g_DownsampleWFilterLine6_Vert_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch);
Void_WMV g_DownsampleWFilterLine6_Horiz_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size);
Void_WMV g_UpsampleWFilterLine10_Vert_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch);
Void_WMV g_UpsampleWFilterLine10_Horiz_C(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size);

Void_WMV g_DownsampleWFilterLine6_Vert_ARMV4(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch);
Void_WMV g_DownsampleWFilterLine6_Horiz_ARMV4(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size);
Void_WMV g_UpsampleWFilterLine10_Vert_ARMV4(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch);
Void_WMV g_UpsampleWFilterLine10_Horiz_ARMV4(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size);

#ifdef _ARM_
#define g_DownsampleWFilterLine6_Vert       g_DownsampleWFilterLine6_Vert_ARMV4
#define g_DownsampleWFilterLine6_Horiz      g_DownsampleWFilterLine6_Horiz_ARMV4
#define g_UpsampleWFilterLine10_Vert         g_UpsampleWFilterLine10_Vert_ARMV4
#define g_UpsampleWFilterLine10_Horiz        g_UpsampleWFilterLine10_Horiz_ARMV4
#else
#define g_DownsampleWFilterLine6_Vert       g_DownsampleWFilterLine6_Vert_C
#define g_DownsampleWFilterLine6_Horiz      g_DownsampleWFilterLine6_Horiz_C
#define g_UpsampleWFilterLine10_Vert         g_UpsampleWFilterLine10_Vert_C
#define g_UpsampleWFilterLine10_Horiz        g_UpsampleWFilterLine10_Horiz_C
#endif



Void_WMV ARMV6_SignPatch32(I32_WMV * rgiCoefRecon, int len);
Void_WMV SignPatch32_ARMV4(I32_WMV * rgiCoefRecon, int len);
Void_WMV SignPatch_ARMV4(I16_WMV * rgiCoefRecon, int len);
Void_WMV SignPatch32_C(I32_WMV * rgiCoefRecon, int len);
Void_WMV SignPatch_C(I16_WMV * rgiCoefRecon, int len);


tWMVDecodeStatus DecodeMB_X9_Fast (EMB_PMainLoop * pMainLoop,
                                                    CWMVMBMode*            pmbmd, 
                                                    I32_WMV iMBX, I32_WMV iMBY,
                                                    Bool_WMV b1MV);

tWMVDecodeStatus WMVideoDecDecodeP_X9_EMB (tWMVDecInternalMember *pWMVDec);
 tWMVDecodeStatus decodeMBOverheadOfPVOP_WMV3_EMB (EMB_PMainLoop * pMainLoop,  //EMB_PMainLoop * pMainLoop, 
        CWMVMBMode* pmbmd, I32_WMV x, I32_WMV y,EMB_PBMainLoop *pPB);

I32_WMV PredictHybridMV_EMB (EMB_PMainLoop * pMainLoop,  I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPB);    
I32_WMV ComputeMVFromDiffMV_EMB (EMB_PMainLoop * pMainLoop, I32_WMV iblk, I8_WMV b1MV,EMB_PBMainLoop *pPB);
Void_WMV decodeMV_V9_EMB(EMB_PBMainLoop * pMainLoop, Huffman_WMV * pHufMVTable, CDiffMV_EMB *pDiffMV);

Void_WMV SetupEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec,EMB_PBMainLoop *pPB);

I32_WMV g_MotionComp_X9_EMB (EMB_PMainLoop *pPMainLoop,I32_WMV irows,I32_WMV icols,I32_WMV iblk, Bool_WMV b1MV,EMB_PBMainLoop *pPB);

tWMVDecodeStatus MotionCompMB_WMV3_EMB (EMB_PMainLoop *pPMainLoop,I32_WMV irows,I32_WMV icols,EMB_PBMainLoop *pPB);
tWMVDecodeStatus MotionCompMB_WMV3_EMB_new (EMB_PMainLoop *pPMainLoop,I32_WMV irows,I32_WMV icols,EMB_PBMainLoop *pPB);

I32_WMV  ChromaMV_EMB (EMB_PMainLoop * pMainLoop, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV,EMB_PBMainLoop *pPB);

Void_WMV SetupMVDecTable_EMB(tWMVDecInternalMember *pWMVDec);

 Void_WMV InitBlockInterpolation_EMB (tWMVDecInternalMember * pWMVDec);

Void_WMV g_SubBlkIDCTClear_EMB_C (UnionBuffer * piDstBuf,  I32_WMV iIdx);
Void_WMV g_SubBlkIDCTClear_EMB_ARMV4 (UnionBuffer * piDstBuf,  I32_WMV iIdx);

//#if !defined(WMV_OPT_DQUANT_ARM) //sw
tWMVDecodeStatus DecodeInverseInterBlockQuantize16_EMB(EMB_PBMainLoop* pMainLoop, 
                        CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, 
                        I32_WMV XFormType,
                        DQuantDecParam *pDQ,
                        I16_WMV* buffer);
tWMVDecodeStatus DecodeInverseInterBlockQuantize16_EMB_new(EMB_PBMainLoop* pMainLoop, 
                                                       CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, 
                                                       I32_WMV XFormType,
                                                       DQuantDecParam *pDQ,
                                                       I16_WMV* buffer);
//#endif
Bool_WMV decodeDCTPrediction_EMB ( EMB_PMainLoop* pPMainLoop, 
    CWMVMBMode *pmbmd,
    CWMVMBMode *pmbmd_lastrow,
    I16_WMV *pIntra,I16_WMV *pIntra_lastrow,
    I32_WMV iblk,
    I32_WMV iX, I32_WMV iY, I32_WMV *piShift, 
    I32_WMV *iDirection, I16_WMV *pPredScaled,EMB_PBMainLoop *pPB);

I32_WMV  IsIntraPredY_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iblk,I32_WMV iX, I32_WMV iY);
I32_WMV IsIntraPredUV_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iMBX, I32_WMV iMBY);
Void_WMV OverlapMBRow_Slice_EMB (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, U8_WMV *ppxliRecnU,
                           U8_WMV *ppxliRecnV, Bool_WMV bTopRowOfSlice, Bool_WMV bBottomRowOfSlice,U32_WMV threadflag);

//Void_WMV ComputeLoopFilterFlags_WMV9_EMB(tWMVDecInternalMember *pWMVDec);
//Void_WMV ComputeLoopFilterFlags_WMVA_EMB(tWMVDecInternalMember *pWMVDec);
Void_WMV ComputeMBLoopFilterFlags_WMV9_EMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft);
Void_WMV ComputeMBLoopFilterFlags_WMVA_EMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft);

Void_WMV EMB_InitZigZagTable_SSIMD(U8_WMV * pZigzag, U8_WMV * pNewZigZag, I32_WMV iNumEntries);


Void_WMV g_AddErrorB_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, U32_WMV iOffset);
Void_WMV g_AddErrorB_SSIMD_ARMV4(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, U32_WMV iOffset);

Void_WMV g_AddNullB_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch);
Void_WMV g_AddNullB_SSIMD_ARMV4(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch);

tWMVDecodeStatus DecodeBMB_EMB_Fast(EMB_BMainLoop  * pBMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY);


Void_WMV g_InterpolateBlockBicubicOverflow_EMB(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);

//Types here are I_SIMD because the error calculated here is returned as a bitmask of SSIMD data types.
I_SIMD g_InterpolateBlockBicubic_0x_SSIMD ( const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_02_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_x0_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_20_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_xx_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);
I_SIMD g_InterpolateBlockBicubic_x2_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);
I_SIMD g_InterpolateBlockBicubic_2x_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);
I_SIMD g_InterpolateBlockBicubic_22_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst,EMB_PBMainLoop  * pMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);

Void_WMV g_OverlapBlockVerticalEdge_C(I16_WMV *pInOut, I32_WMV iStride);
Void_WMV g_OverlapBlockVerticalEdge_ARMV4(I16_WMV *pInOut, I32_WMV iStride);

extern I32_WMV g_DecodeIDCT8x4_SSIMD(UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags,I32_WMV i);
extern I32_WMV g_DecodeIDCT4x8_SSIMD(UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags,I32_WMV i);
extern I32_WMV g_DecodeIDCT4x4_SSIMD(UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags,I32_WMV i);

void voVC1InvTrans_8x8_Overlap_C(U8_WMV*pDest,I32_WMV dststride,U8_WMV* pRef,I16_WMV* pDestOverlap,I32_WMV refstride,I16_WMV *block,I32_WMV overlapstride,unsigned long *table);
void voVC1InvTrans_8x8_C(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table0,U32_WMV* table1); 
void voVC1InvTrans_4x8_C(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table0,U32_WMV* table1); 
void voVC1InvTrans_8x4_C(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table0,U32_WMV* table1); 
void voVC1InvTrans_4x4_C(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table0,U32_WMV* table1); 
void voVC1Copy_8x8_C(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);
void voVC1Copy_8x4_C(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);
void voVC1Copy_4x8_C(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);
void voVC1Copy_4x4_C(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);

#if defined(VOARMV7)
void voVC1InvTrans_8x8_Overlap_ARMV7(U8_WMV*pDest,I32_WMV dststride,U8_WMV* pRef,I16_WMV* pDestOverlap,I32_WMV refstride,I16_WMV *block,I32_WMV overlapstride,U32_WMV* table);
void voVC1InvTrans_8x8_ARMV7(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table,U32_WMV* table1); 
void voVC1InvTrans_4x8_ARMV7(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table,U32_WMV* table1); 
void voVC1InvTrans_8x4_ARMV7(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table,U32_WMV* table1); 
void voVC1InvTrans_4x4_ARMV7(U8_WMV* pDest,I32_WMV dststride,U8_WMV* pRef0,U8_WMV* pRef1,I32_WMV refstride,I16_WMV* block,U32_WMV* table,U32_WMV* table1); 
void voVC1Copy_8x8_ARMV7(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);
void voVC1Copy_8x4_ARMV7(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);
void voVC1Copy_4x8_ARMV7(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);
void voVC1Copy_4x4_ARMV7(U8_WMV* pDst, I32_WMV dststride, U8_WMV *pRef0,U8_WMV*pRef1,I32_WMV refstride);
void voVC1InvTrans_8x8_DC_ARMV7(unsigned char* pDest,int dststride,unsigned char* pRef0,unsigned char* pRef1,int refstride,short *block,unsigned long *table0,unsigned long *table1);
void voVC1InvTrans_8x4_DC_ARMV7(unsigned char* pDest,int dststride,unsigned char* pRef0,unsigned char* pRef1,int refstride,short *block,unsigned long *table0,unsigned long *table1);
void voVC1InvTrans_4x8_DC_ARMV7(unsigned char* pDest,int dststride,unsigned char* pRef0,unsigned char* pRef1,int refstride,short *block,unsigned long *table0,unsigned long *table1);
void voVC1InvTrans_4x4_DC_ARMV7(unsigned char* pDest,int dststride,unsigned char* pRef0,unsigned char* pRef1,int refstride,short *block,unsigned long *table0,unsigned long *table1);

void voMemoryCpy_8x8_ARMV7(U8_WMV*pSrc,I32_WMV iSrcStride,U8_WMV *pDstTmp,I32_WMV iDstStride);
void voMemoryCpy_16x16_ARMV7(U8_WMV*pSrc,I32_WMV iSrcStride,U8_WMV *pDstTmp,I32_WMV iDstStride);
U32_WMV g_NewHorzFilterX_Only_6_ARMV7(U8_WMV *pSrc, 
                                I32_WMV iSrcStride,
                                U8_WMV *pDst, 
                                I32_WMV iDstStride,
                               const I32_WMV iShift,
                               const I32_WMV iRound2_32, 
                               const I8_WMV * const pH, 
                               Bool_WMV b1MV);
U32_WMV g_NewHorzFilterX_Only_4_ARMV7(U8_WMV *pSrc, 
                                I32_WMV iSrcStride,
                                U8_WMV *pDst, 
                                I32_WMV iDstStride,
                               const I32_WMV iShift,
                               const I32_WMV iRound2_32, 
                               const I8_WMV * const pH, 
                               Bool_WMV b1MV);
U32_WMV g_NewVertFilterX_Only_6_ARMV7(U8_WMV  *pSrc,
                         I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         I32_WMV iDstStride, 
                         I32_WMV iShift, 
                         I32_WMV iRound32, 
                         I8_WMV * pV, 
                         I32_WMV iNumHorzLoop, 
                         U32_WMV uiMask,
                         Bool_WMV b1MV);
U32_WMV g_NewVertFilterX_Only_4_ARMV7(U8_WMV  *pSrc,
                         I32_WMV iSrcStride, 
                         U8_WMV * pDst, 
                         I32_WMV iDstStride, 
                         I32_WMV iShift, 
                         I32_WMV iRound32, 
                         I8_WMV * pV, 
                         I32_WMV iNumHorzLoop, 
                         U32_WMV uiMask,
                         Bool_WMV b1MV);
void  voInterpolateBlockBicubic_ARMV7(U8_WMV* pSrc,
                                      I32_WMV iSrcStride, 
                                      U8_WMV *pDst, 
                                      I32_WMV iDstStride,
                                      U32_WMV shift,
                                      U32_WMV iRndCtl0,
                                      U32_WMV iRndCtl1,
                                      I8_WMV *pV,
                                      I8_WMV *pH,
                                      Bool_WMV b1MV);


extern void ARMV7_g_4x4IDCT(const I32_WMV* piSrc0, I16_WMV* blk16,U32_WMV stride,I32_WMV iDCTHorzFlags);
extern void ARMV7_g_4x8IDCT(const I32_WMV* piSrc0, I16_WMV* blk16,U32_WMV stride,I32_WMV iDCTHorzFlags);
extern void ARMV7_g_8x4IDCT(const I32_WMV* piSrc0, I16_WMV* blk16,U32_WMV stride,I32_WMV iDCTHorzFlags);
extern void ARMV7_g_8x8IDCT(const I32_WMV* piSrc0, I16_WMV* blk16,U32_WMV stride,I32_WMV iDCTHorzFlags);

Void_WMV ARMV7_g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
Void_WMV ARMV7_g_InterpolateBlockBilinear_SSIMD_01 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV ARMV7_g_InterpolateBlockBilinear_SSIMD_10 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV ARMV7_g_InterpolateBlockBilinear_SSIMD_11 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV ARMV7_g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
I_SIMD	 ARMV7_g_InterpolateBlock_00_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop,  I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV * pTbl); // iXFrac == 0; iYFrac == 0
Void_WMV ARMV7_g_NewVertFilter0LongNoGlblTbl(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
Void_WMV ARMV7_g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
Void_WMV ARMV7_g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
U_SIMD   ARMV7_g_NewHorzFilterX(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV);
U_SIMD   ARMV7_g_NewVertFilterX(const U8_WMV  *pSrc,const I32_WMV iSrcStride,U8_WMV * pDst, const I32_WMV iShift,const I_SIMD iRound,const I8_WMV * const pV,I32_WMV iNumHorzLoop,const U_SIMD uiMask,Bool_WMV b1MV,U16_WMV * pTbl);
Void_WMV ARMV7_g_FilterHorizontalEdgeV9(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV ARMV7_g_FilterHorizontalEdgeV9Last8x4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV ARMV7_g_FilterVerticalEdgeV9 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV ARMV7_g_FilterVerticalEdgeV9Last4x8 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV ARMV7_IntraDequantACPred( I16_WMV* pDct, I32_WMV* m_rgiCoefRecon, I32_WMV iDoubleStepSize, I32_WMV iStepMinusStepIsEven);
Void_WMV ARMV7_IntraBlockDequant8x8(I32_WMV iDCStepSize, I32_WMV iDoubleStepSize, I32_WMV iStepMinusStepIsEven, I16_WMV *rgiCoefRecon);

Void_WMV voInterpolateBlockBilinear_ARMV7 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV voInterpolateBlockBilinear_11_ARMV7(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV voInterpolateBlockBilinear_10_ARMV7(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV voInterpolateBlockBilinear_01_ARMV7(const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);

Void_WMV g_FilterHorizontalEdgeV9_ARMV4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9_ARMV4 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV ARMV6_g_OverlapBlockHorizontalEdge(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
Void_WMV ARMV6_IntensityCompensation_asm(U8_WMV  *pSrcY,U8_WMV *pSrcU,U8_WMV *pSrcV, I32_WMV y_num,I32_WMV uv_num, I32_WMV iScale, I32_WMV iShift);

#elif defined(VOARMV6)

Void_WMV ARMV6_g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
Void_WMV ARMV6_g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
Void_WMV ARMV6_g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
Void_WMV ARMV6_g_OverlapBlockHorizontalEdge(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
Void_WMV ARMV6_IntensityCompensation_asm(U8_WMV  *pSrcY,U8_WMV *pSrcU,U8_WMV *pSrcV, I32_WMV y_num,I32_WMV uv_num, I32_WMV iScale, I32_WMV iShift);
/* The same as VOARMV4. */
Void_WMV g_InterpolateBlockBilinear_SSIMD_01_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_10_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_11_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
I_SIMD	 g_InterpolateBlock_00_SSIMD_ARMV4 (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop,  I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV * pTbl); // iXFrac == 0; iYFrac == 0
Void_WMV g_NewVertFilter0LongNoGlblTbl_ARMV4(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
U_SIMD   g_NewHorzFilterX_ARMV4(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV);
U_SIMD   g_NewVertFilterX_ARMV4(const U8_WMV  *pSrc,const I32_WMV iSrcStride,U8_WMV * pDst, const I32_WMV iShift,const I_SIMD iRound,const I8_WMV * const pV,I32_WMV iNumHorzLoop,const U_SIMD uiMask,Bool_WMV b1MV,U16_WMV * pTbl);
Void_WMV g_FilterHorizontalEdgeV9_ARMV4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterHorizontalEdgeV9Last8x4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9_ARMV4 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9Last4x8 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);

Void_WMV g_FilterHorizontalEdgeV9_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterHorizontalEdgeV9Last8x4_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterHorizontalEdgeV9_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);

Void_WMV g_FilterVerticalEdgeV9_armv7_C (U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterVerticalEdgeV9Last4x8_armv7_C (U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterVerticalEdgeV9_C (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);

#elif defined(VOARMV4)
Void_WMV g_InterpolateBlockBilinear_SSIMD_01_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_10_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_11_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_ARMV4 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
I_SIMD	 g_InterpolateBlock_00_SSIMD_ARMV4 (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop,  I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV * pTbl); // iXFrac == 0; iYFrac == 0
Void_WMV g_NewVertFilter0LongNoGlblTbl_ARMV4(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
U_SIMD   g_NewHorzFilterX_ARMV4(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV);
U_SIMD   g_NewVertFilterX_ARMV4(const U8_WMV  *pSrc,const I32_WMV iSrcStride,U8_WMV * pDst, const I32_WMV iShift,const I_SIMD iRound,const I8_WMV * const pV,I32_WMV iNumHorzLoop,const U_SIMD uiMask,Bool_WMV b1MV,U16_WMV * pTbl);
Void_WMV g_AddError_SSIMD_ARMV4(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
Void_WMV g_AddNull_SSIMD_ARMV4(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
Void_WMV g_FilterHorizontalEdgeV9_ARMV4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9_ARMV4 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_OverlapBlockHorizontalEdge_ARMV4(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
Void_WMV IntensityCompensation_ARMV4(U8_WMV  *pSrcY,U8_WMV *pSrcU,U8_WMV *pSrcV, I32_WMV y_num,I32_WMV uv_num, I32_WMV iScale, I32_WMV iShift);
#endif

Void_WMV g_IDCTDec16_WMV3_SSIMD_C (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
Void_WMV g_InterpolateBlockBilinear_SSIMD_01_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_10_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_11_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_C_new (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);

I_SIMD	 g_InterpolateBlock_00_SSIMD_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop,  I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV * pTbl); // iXFrac == 0; iYFrac == 0
I_SIMD	 g_InterpolateBlock_00_SSIMD_C_new ( U8_WMV *pSrc,
                                            I32_WMV iSrcStride, 
                                            U8_WMV *pDst,
                                            I32_WMV iDstStride, 
                                            EMB_PBMainLoop  * pMainLoop,  
                                            I32_WMV iXFrac,
                                            I32_WMV iYFrac,
                                            Bool_WMV b1MV); // iXFrac == 0; iYFrac == 0

Void_WMV g_AddError_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
Void_WMV g_AddError_SSIMD_C_new(U8_WMV* ppxlcDst, I32_WMV iDststride,U8_WMV* pRef , I32_WMV iRefstride,I16_WMV* pErrorBuf);
Void_WMV g_AddNull_SSIMD_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
Void_WMV g_AddNull_SSIMD_C_new(U8_WMV* ppxlcDst,I32_WMV iDststride, U8_WMV* pRef , I32_WMV iRefstride);

U_SIMD   g_NewHorzFilterX_C(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV);
U_SIMD   g_NewVertFilterX_C(const U8_WMV  *pSrc,const I32_WMV iSrcStride,U8_WMV * pDst, const I32_WMV iShift,const I_SIMD iRound,const I8_WMV * const pV,I32_WMV iNumHorzLoop,const U_SIMD uiMask,Bool_WMV b1MV,U16_WMV * pTbl);
Void_WMV g_FilterHorizontalEdgeV9Last8x4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9Last4x8 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterHorizontalEdgeV9_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterHorizontalEdgeV9Last8x4_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterHorizontalEdgeV9_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9_armv7_C (U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterVerticalEdgeV9Last4x8_armv7_C (U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterVerticalEdgeV9_C (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_OverlapBlockHorizontalEdge_C(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);


tWMVDecodeStatus DecodeInverseInterBlockQuantize16_level2_EMB(
                                                              tWMVDecInternalMember *pWMVDec, 
                                                              CDCTTableInfo_Dec* InterDCTTableInfo_Dec,
                                                              U32_WMV uiNumCoefs,
                                                              U32_WMV * puiCoefCounter,
                                                              I32_WMV * pvalue,
                                                              I32_WMV * pSignBit,
                                                              U32_WMV * plIndex
                                                              );

Void_WMV SetupEMBBMainLoopCtl(tWMVDecInternalMember *pWMVDec,EMB_PBMainLoop *pPBB);

Void_WMV InitEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec,EMB_PBMainLoop *pPB);

tWMVDecodeStatus decodeB_EMB (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus decodeMBOverheadOfBVOP_WMV3_EMB (EMB_BMainLoop * pBMainLoop,CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY,EMB_PBMainLoop *pPBB);

//Void_WMV decodeBMV_V9_EMB(EMB_BMainLoop * pBMainLoop, Huffman_WMV* pHufMVTable, CDiffMV_EMB *pDiffMV);

Void_WMV PredictMV_EMB (EMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk,EMB_PBMainLoop *pPBB,U32_WMV ncols, U32_WMV nrows);
I32_WMV PredictBMVPullBackCheck_EMB(EMB_BMainLoop  * pBMainLoop,  I32_WMV bBfrm, I32_WMV iPred,EMB_PBMainLoop *pPBB);

Void_WMV DirectModeMV_EMB (EMB_BMainLoop * pBMainLoop, I32_WMV iXMotion, I32_WMV iYMotion, Bool_WMV bHpelMode, UMotion_EMB * defMotion);

tWMVDecodeStatus DecodeInverseIntraBlockX9_EMB(tWMVDecInternalMember *pWMVDec,
                                               CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                               I32_WMV iblk,   U8_WMV *pZigzagInv,   Bool_WMV bResidual,
                                               I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                                               I16_WMV   *pIntra, CWMVMBMode* pmbmd,
                                               DQuantDecParam *pDQ,I16_WMV* buffer
                                               );

tWMVDecodeStatus DecodeBMB_EMB_Intra(EMB_BMainLoop  * pBMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY );

I32_WMV prvWMVModulus (I32_WMV a, I32_WMV b);
I32_WMV prvWMVDivision (I32_WMV a, I32_WMV b);

enum HEAP_CATEGORY { DHEAP_STRUCT=0, DHEAP_FRAMES=1, DHEAP_HUFFMAN=2, DHEAP_LOCALHUFF=3, DHEAP_LOCAL_COUNT=4 };

/*************************************************************************
 * wmvMalloc/wmvFree functions declaration 
 ************************************************************************/

#ifndef WMV_EXTERN_C

#ifdef __cplusplus
#define WMV_EXTERN_C extern "C"
#else
#define WMV_EXTERN_C
#endif

#endif

typedef struct VOMEMCHECK
{
	int alloc_add;
	int alloc_size;
	int free_add;
}voMemCheck;

/*
 * DEFINE wmvMalloc and HEAP measurement utilities
 */
WMV_EXTERN_C void * MallocHandle(VO_MEM_OPERATOR *memOp, size_t pSize, char *pFile, int pLine, int category);
#define wmvMallocHandle(memOp, size, category)		\
	MallocHandle(memOp, size, __FILE__, __LINE__, category)

WMV_EXTERN_C void * wmvMalloc(tWMVDecInternalMember *pWMVDec,unsigned int size, int category);
WMV_EXTERN_C void   wmvFree  (tWMVDecInternalMember *pWMVDec,void *free_ptr);

#if defined(VOARMV7)
#define voVC1InvTrans_8x8_Overlap        voVC1InvTrans_8x8_Overlap_ARMV7
#define voVC1InvTrans_8x8        voVC1InvTrans_8x8_ARMV7
#define voVC1InvTrans_4x8        voVC1InvTrans_4x8_ARMV7
#define voVC1InvTrans_8x4        voVC1InvTrans_8x4_ARMV7
#define voVC1InvTrans_4x4        voVC1InvTrans_4x4_ARMV7
#define voVC1InvTrans_8x8_DC        voVC1InvTrans_8x8_DC_ARMV7
#define voVC1InvTrans_4x8_DC        voVC1InvTrans_4x8_DC_ARMV7
#define voVC1InvTrans_8x4_DC        voVC1InvTrans_8x4_DC_ARMV7
#define voVC1InvTrans_4x4_DC        voVC1InvTrans_4x4_DC_ARMV7

#define voVC1Copy_8x8        voVC1Copy_8x8_ARMV7
#define voVC1Copy_8x4        voVC1Copy_8x4_ARMV7
#define voVC1Copy_4x8        voVC1Copy_4x8_ARMV7
#define voVC1Copy_4x4        voVC1Copy_4x4_ARMV7

#define g_IDCT8x4_WMV3_Fun                              g_DecodeIDCT8x4_SSIMD
#define g_IDCT4x8_WMV3_Fun                              g_DecodeIDCT4x8_SSIMD
#define g_IDCT4x4_WMV3_Fun                              g_DecodeIDCT4x4_SSIMD
#define g_IDCTDec16_WMV3_Fun						  ARMV7_g_IDCTDec16_WMV3_SSIMD
#define g_IDCTDec_WMV3_Fun							ARMV7_g_IDCTDec_WMV3
#define g_AddError_SSIMD_Fun						ARMV7_g_AddError_SSIMD
#define g_AddNull_SSIMD_Fun							ARMV7_g_AddNull_SSIMD

//ZOU TODO
#define g_MotionCompZeroMotion_WMV_Fun				g_MotionCompZeroMotion_WMV_C //ARMV7_g_MotionCompZeroMotion_WMV
#define g_NewHorzFilterX_Fun						ARMV7_g_NewHorzFilterX
#define g_NewVertFilterX_Fun						ARMV7_g_NewVertFilterX
#define g_NewVertFilter0LongNoGlblTbl_Fun			ARMV7_g_NewVertFilter0LongNoGlblTbl
#define g_InterpolateBlock_00_SSIMD_Fun				ARMV7_g_InterpolateBlock_00_SSIMD
#define g_InterpolateBlockBilinear_SSIMD_Fun		ARMV7_g_InterpolateBlockBilinear_SSIMD 
#define g_InterpolateBlockBilinear_SSIMD_11_Fun		ARMV7_g_InterpolateBlockBilinear_SSIMD_11
#define g_InterpolateBlockBilinear_SSIMD_01_Fun		ARMV7_g_InterpolateBlockBilinear_SSIMD_01
#define g_InterpolateBlockBilinear_SSIMD_10_Fun		ARMV7_g_InterpolateBlockBilinear_SSIMD_10
#define g_FilterHorizontalEdgeV9_Fun				ARMV7_g_FilterHorizontalEdgeV9
#define g_FilterHorizontalEdgeV9Last8x4_Fun			ARMV7_g_FilterHorizontalEdgeV9Last8x4
#define g_FilterVerticalEdgeV9_Fun					ARMV7_g_FilterVerticalEdgeV9
#define g_FilterVerticalEdgeV9Last4x8_Fun			ARMV7_g_FilterVerticalEdgeV9Last4x8

#define g_OverlapBlockVerticalEdge_Fun              g_OverlapBlockVerticalEdge_ARMV4
#define g_OverlapBlockHorizontalEdge_Fun			ARMV6_g_OverlapBlockHorizontalEdge 
#define IntensityCompensation_asm_Fun				ARMV6_IntensityCompensation_asm
#define SignPatch32_Fun								ARMV6_SignPatch32
#define SignPatch                                        SignPatch_ARMV4
#define g_SubBlkIDCTClear_EMB             g_SubBlkIDCTClear_EMB_ARMV4
#define g_AddNullB_SSIMD_Fun              g_AddNullB_SSIMD_ARMV4
#define g_AddErrorB_SSIMD_Fun            g_AddErrorB_SSIMD_ARMV4

// voInterpolateBlockBilinear_ARMV7  g_InterpolateBlockBilinear_SSIMD_C_new
#define g_InterpolateBlockBilinear_SSIMD_Fun_new		    voInterpolateBlockBilinear_ARMV7
#define g_InterpolateBlockBilinear_SSIMD_11_Fun_new		voInterpolateBlockBilinear_11_ARMV7
#define g_InterpolateBlockBilinear_SSIMD_01_Fun_new		voInterpolateBlockBilinear_01_ARMV7
#define g_InterpolateBlockBilinear_SSIMD_10_Fun_new		voInterpolateBlockBilinear_10_ARMV7

#define g_InterpolateBlock_00_SSIMD_Fun_new            g_InterpolateBlock_00_SSIMD_C_new
#define g_AddError_SSIMD_Fun_new                           g_AddError_SSIMD_C_new
#define g_AddNull_SSIMD_Fun_new            g_AddNull_SSIMD_C_new
#define g_AddNullB_SSIMD_Fun_new           g_AddNullB_SSIMD_C_new

#elif defined(VOARMV6)

#define voVC1InvTrans_8x8_Overlap        voVC1InvTrans_8x8_Overlap_C
#define voVC1InvTrans_8x8        voVC1InvTrans_8x8_C
#define voVC1InvTrans_4x8        voVC1InvTrans_4x8_C
#define voVC1InvTrans_8x4        voVC1InvTrans_8x4_C
#define voVC1InvTrans_4x4        voVC1InvTrans_4x4_C
#define voVC1InvTrans_8x8_DC        voVC1InvTrans_8x8_DC_C
#define voVC1InvTrans_4x8_DC        voVC1InvTrans_4x8_DC_C
#define voVC1InvTrans_8x4_DC        voVC1InvTrans_8x4_DC_C
#define voVC1InvTrans_4x4_DC        voVC1InvTrans_4x4_DC_C

#define voVC1Copy_8x8        voVC1Copy_8x8_C
#define voVC1Copy_8x4        voVC1Copy_8x4_C
#define voVC1Copy_4x8        voVC1Copy_4x8_C
#define voVC1Copy_4x4        voVC1Copy_4x4_C

#define g_IDCTDec16_WMV3_Fun           ARMV6_g_IDCTDec16_WMV3_SSIMD//g_IDCTDec16_WMV3_SSIMD_C//
#define g_IDCT8x4_WMV3_Fun               g_DecodeIDCT8x4_SSIMD
#define g_IDCT4x8_WMV3_Fun               g_DecodeIDCT4x8_SSIMD
#define g_IDCT4x4_WMV3_Fun               g_DecodeIDCT4x4_SSIMD

#define g_IDCTDec_WMV3_Fun              ARMV6_g_IDCTDec_WMV3//g_IDCTDec_WMV3//
#define g_AddError_SSIMD_Fun              ARMV6_g_AddError_SSIMD//g_AddError_SSIMD_C//
#define g_AddNull_SSIMD_Fun                ARMV6_g_AddNull_SSIMD//g_AddNull_SSIMD_C//
#define g_AddNullB_SSIMD_Fun              g_AddNullB_SSIMD_ARMV4//g_AddNullB_SSIMD_C//
#define g_AddErrorB_SSIMD_Fun            g_AddErrorB_SSIMD_ARMV4//g_AddErrorB_SSIMD_C//

#define g_MotionCompZeroMotion_WMV_Fun    g_MotionCompZeroMotion_WMV_C //ARMV6_g_MotionCompZeroMotion_WMV//g_MotionCompZeroMotion_WMV_C//
#define g_OverlapBlockHorizontalEdge_Fun       ARMV6_g_OverlapBlockHorizontalEdge//g_OverlapBlockHorizontalEdge_C//
#define g_OverlapBlockVerticalEdge_Fun          g_OverlapBlockVerticalEdge_ARMV4//g_OverlapBlockVerticalEdge_C//

#define IntensityCompensation_asm_Fun      ARMV6_IntensityCompensation_asm
#define SignPatch32_Fun                           ARMV6_SignPatch32//SignPatch32_C//
#define SignPatch                                     SignPatch_ARMV4//SignPatch_C//

/* The same as VOARMV4. */
#define g_NewVertFilterX_Fun						g_NewVertFilterX_ARMV4//g_NewVertFilterX_C//
#define g_NewHorzFilterX_Fun						g_NewHorzFilterX_ARMV4//g_NewHorzFilterX_C//
#define g_NewVertFilter0LongNoGlblTbl_Fun			g_NewVertFilter0LongNoGlblTbl_ARMV4//g_NewVertFilter0Long_C//
#define g_InterpolateBlock_00_SSIMD_Fun			g_InterpolateBlock_00_SSIMD_ARMV4//g_InterpolateBlock_00_SSIMD_C//
#define g_InterpolateBlockBilinear_SSIMD_Fun		g_InterpolateBlockBilinear_SSIMD_ARMV4//g_InterpolateBlockBilinear_SSIMD_C// 
#define g_InterpolateBlockBilinear_SSIMD_11_Fun		g_InterpolateBlockBilinear_SSIMD_11_ARMV4//g_InterpolateBlockBilinear_SSIMD_11_C//
#define g_InterpolateBlockBilinear_SSIMD_01_Fun		g_InterpolateBlockBilinear_SSIMD_01_ARMV4//g_InterpolateBlockBilinear_SSIMD_01_C//
#define g_InterpolateBlockBilinear_SSIMD_10_Fun		g_InterpolateBlockBilinear_SSIMD_10_ARMV4//g_InterpolateBlockBilinear_SSIMD_10_C//
#define  g_SubBlkIDCTClear_EMB                            g_SubBlkIDCTClear_EMB_ARMV4//g_SubBlkIDCTClear_EMB_C//

#define g_FilterHorizontalEdgeV9_Fun				g_FilterHorizontalEdgeV9_armv7_C
#define g_FilterHorizontalEdgeV9Last8x4_Fun			g_FilterHorizontalEdgeV9Last8x4_armv7_C
#define g_FilterVerticalEdgeV9_Fun					g_FilterVerticalEdgeV9_armv7_C
#define g_FilterVerticalEdgeV9Last4x8_Fun			g_FilterVerticalEdgeV9Last4x8_armv7_C
#define g_InterpolateBlock_00_SSIMD_Fun_new            g_InterpolateBlock_00_SSIMD_C_new
#define g_InterpolateBlockBilinear_SSIMD_Fun_new	g_InterpolateBlockBilinear_SSIMD_C_new

#define g_NewHorzFilterX_Fun_new						g_NewHorzFilterX_C_new
#define g_NewVertFilterX_Fun_new						g_NewVertFilterX_C_new

#elif defined(VOARMV4)
#define g_IDCTDec16_WMV3_Fun       g_IDCTDec16_WMV3_SSIMD_ARMV4
#define  g_SubBlkIDCTClear_EMB        g_SubBlkIDCTClear_EMB_ARMV4
#define g_AddNull_SSIMD_Fun            g_AddNull_SSIMD_ARMV4
#define g_AddNullB_SSIMD_Fun           g_AddNullB_SSIMD_ARMV4
#define g_AddErrorB_SSIMD_Fun        g_AddErrorB_SSIMD_ARMV4
#define g_AddError_SSIMD_Fun          g_AddError_SSIMD_ARMV4
#define IntensityCompensation_asm_Fun          IntensityCompensation_ARMV4
#define SignPatch32_Fun								SignPatch32_ARMV4
#define SignPatch                                        SignPatch_ARMV4
#define g_NewVertFilterX_Fun						g_NewVertFilterX_ARMV4 
#define g_NewHorzFilterX_Fun						g_NewHorzFilterX_ARMV4
#define g_NewVertFilter0LongNoGlblTbl_Fun      g_NewVertFilter0LongNoGlblTbl_ARMV4
#define g_InterpolateBlock_00_SSIMD_Fun       g_InterpolateBlock_00_SSIMD_ARMV4
#define g_InterpolateBlockBilinear_SSIMD_Fun		g_InterpolateBlockBilinear_SSIMD_ARMV4
#define g_InterpolateBlockBilinear_SSIMD_11_Fun		g_InterpolateBlockBilinear_SSIMD_11_ARMV4
#define g_InterpolateBlockBilinear_SSIMD_01_Fun		g_InterpolateBlockBilinear_SSIMD_01_ARMV4
#define g_InterpolateBlockBilinear_SSIMD_10_Fun		g_InterpolateBlockBilinear_SSIMD_10_ARMV4
#define g_OverlapBlockVerticalEdge_Fun              g_OverlapBlockVerticalEdge_ARMV4
#define g_OverlapBlockHorizontalEdge_Fun			g_OverlapBlockHorizontalEdge_ARMV4
#define g_MotionCompZeroMotion_WMV_Fun				g_MotionCompZeroMotion_WMV_ARMV4
#define g_IDCT8x4_WMV3_Fun                              g_DecodeIDCT8x4_SSIMD
#define g_IDCT4x8_WMV3_Fun                              g_DecodeIDCT4x8_SSIMD
#define g_IDCT4x4_WMV3_Fun                              g_DecodeIDCT4x4_SSIMD
#define g_IDCTDec_WMV3_Fun                              g_IDCTDec_WMV3
#define g_FilterHorizontalEdgeV9_Fun				g_FilterHorizontalEdgeV9_armv7_C
#define g_FilterHorizontalEdgeV9Last8x4_Fun			g_FilterHorizontalEdgeV9Last8x4_armv7_C
#define g_FilterVerticalEdgeV9_Fun					g_FilterVerticalEdgeV9_armv7_C
#define g_FilterVerticalEdgeV9Last4x8_Fun			g_FilterVerticalEdgeV9Last4x8_armv7_C
#define g_InterpolateBlockBilinear_SSIMD_Fun_new	g_InterpolateBlockBilinear_SSIMD_C_new
#else
#define voVC1InvTrans_8x8_Overlap        voVC1InvTrans_8x8_Overlap_C
#define voVC1InvTrans_8x8        voVC1InvTrans_8x8_C
#define voVC1InvTrans_4x8        voVC1InvTrans_4x8_C
#define voVC1InvTrans_8x4        voVC1InvTrans_8x4_C
#define voVC1InvTrans_4x4        voVC1InvTrans_4x4_C
#define voVC1InvTrans_8x8_DC        voVC1InvTrans_8x8_DC_C
#define voVC1InvTrans_4x8_DC        voVC1InvTrans_4x8_DC_C
#define voVC1InvTrans_8x4_DC        voVC1InvTrans_8x4_DC_C
#define voVC1InvTrans_4x4_DC        voVC1InvTrans_4x4_DC_C

#define voVC1Copy_8x8        voVC1Copy_8x8_C
#define voVC1Copy_8x4        voVC1Copy_8x4_C
#define voVC1Copy_4x8        voVC1Copy_4x8_C
#define voVC1Copy_4x4        voVC1Copy_4x4_C


#define g_IDCTDec16_WMV3_Fun       g_IDCTDec16_WMV3_SSIMD_C
#define  g_SubBlkIDCTClear_EMB         g_SubBlkIDCTClear_EMB_C
#define  g_SubBlkIDCTClear_EMB_new         g_SubBlkIDCTClear_EMB_C_new
#define g_AddNull_SSIMD_Fun            g_AddNull_SSIMD_C
#define g_AddNull_SSIMD_Fun_new            g_AddNull_SSIMD_C_new
#define g_AddNullB_SSIMD_Fun           g_AddNullB_SSIMD_C
#define g_AddNullB_SSIMD_Fun_new           g_AddNullB_SSIMD_C_new
#define g_AddErrorB_SSIMD_Fun        g_AddErrorB_SSIMD_C
#define g_AddErrorB_SSIMD_Fun_new        g_AddErrorB_SSIMD_C_new
#define g_AddError_SSIMD_Fun          g_AddError_SSIMD_C
#define g_AddError_SSIMD_Fun_new          g_AddError_SSIMD_C_new
#define SignPatch32_Fun								SignPatch32_C
#define SignPatch                                        SignPatch_C
#define g_NewVertFilterX_Fun						g_NewVertFilterX_C
#define g_NewVertFilterX_Fun_new						g_NewVertFilterX_C_new
#define g_NewHorzFilterX_Fun						g_NewHorzFilterX_C
#define g_NewHorzFilterX_Fun_new						g_NewHorzFilterX_C_new
#define g_NewVertFilter0LongNoGlblTbl_Fun      g_NewVertFilter0Long_C
#define g_InterpolateBlock_00_SSIMD_Fun        g_InterpolateBlock_00_SSIMD_C
#define g_InterpolateBlock_00_SSIMD_Fun_new   g_InterpolateBlock_00_SSIMD_C_new
#define g_InterpolateBlockBilinear_SSIMD_Fun		g_InterpolateBlockBilinear_SSIMD_C 
#define g_InterpolateBlockBilinear_SSIMD_11_Fun		g_InterpolateBlockBilinear_SSIMD_11_C
#define g_InterpolateBlockBilinear_SSIMD_01_Fun		g_InterpolateBlockBilinear_SSIMD_01_C
#define g_InterpolateBlockBilinear_SSIMD_10_Fun		g_InterpolateBlockBilinear_SSIMD_10_C

#define g_InterpolateBlockBilinear_SSIMD_Fun_new	g_InterpolateBlockBilinear_SSIMD_C_new
//#define g_InterpolateBlockBilinear_SSIMD_11_Fun_new		g_InterpolateBlockBilinear_SSIMD_11_C_new
//#define g_InterpolateBlockBilinear_SSIMD_01_Fun_new		g_InterpolateBlockBilinear_SSIMD_01_C_new
//#define g_InterpolateBlockBilinear_SSIMD_10_Fun_new		g_InterpolateBlockBilinear_SSIMD_10_C_new

#define g_OverlapBlockVerticalEdge_Fun              g_OverlapBlockVerticalEdge_C
#define g_OverlapBlockHorizontalEdge_Fun			g_OverlapBlockHorizontalEdge_C
#define g_MotionCompZeroMotion_WMV_Fun				g_MotionCompZeroMotion_WMV_C
#define g_IDCT8x4_WMV3_Fun                              g_DecodeIDCT8x4_SSIMD
#define g_IDCT4x8_WMV3_Fun                              g_DecodeIDCT4x8_SSIMD
#define g_IDCT4x4_WMV3_Fun                              g_DecodeIDCT4x4_SSIMD
#define g_IDCTDec_WMV3_Fun                              g_IDCTDec_WMV3
#define g_FilterHorizontalEdgeV9_Fun				g_FilterHorizontalEdgeV9_armv7_C
#define g_FilterHorizontalEdgeV9Last8x4_Fun			g_FilterHorizontalEdgeV9Last8x4_armv7_C
#define g_FilterVerticalEdgeV9_Fun					g_FilterVerticalEdgeV9_armv7_C
#define g_FilterVerticalEdgeV9Last4x8_Fun			g_FilterVerticalEdgeV9Last4x8_armv7_C
#endif


void Round_MV_Luam_FRAMEMV(tWMVDecInternalMember *pWMVDec, I32_WMV*IX, I32_WMV*IY,I32_WMV eBlk,
                           I32_WMV xindex,I32_WMV yindex,
                           U32_WMV ncols,	U32_WMV nrows);
Bool_WMV PullBackMotionVector_UV (tWMVDecInternalMember* pWMVDec, I32_WMV*iMvX, I32_WMV*iMvY, I32_WMV imbX, I32_WMV imbY);

void Round_MV_Chroma_FRAMEMV(tWMVDecInternalMember *pWMVDec, 
                             I32_WMV *IX, I32_WMV*IY,
                             I32_WMV xindex,I32_WMV yindex,
                             U32_WMV ncols, U32_WMV nrows);

void Round_MV_Luam_FILEDMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,
                           I32_WMV xindex,I32_WMV yindex,
                           I32_WMV ncols,I32_WMV nrows);

void Round_MV_Chroma_FILEDMV(tWMVDecInternalMember *pWMVDec, 
                                            I32_WMV *IX, I32_WMV*IY,I32_WMV* F,
                                            I32_WMV xindex,I32_WMV yindex,
                                            I32_WMV imbX, I32_WMV imbY);

Void_WMV DirectModeMV_Prog_BVOP (tWMVDecInternalMember * pWMVDec,
                                                CWMVMBMode*pmbmd,
                                                I32_WMV iXMotion, I32_WMV iYMotion, Bool_WMV bHpelMode,
                                                I32_WMV imbX, I32_WMV imbY,
                                                I32_WMV *idfx, I32_WMV *idfy, I32_WMV *idbx, I32_WMV *idby);
tWMVDecodeStatus ComputeLoopFilterFlags_EMB_MB(tWMVDecInternalMember *pWMVDec, 
                                               CWMVMBMode* pmbmd,
                                               CWMVMBMode* pmbmd_lastrow,
                                               U32_WMV imbY,U32_WMV imbX);
tWMVDecodeStatus ProcessMBRow_Parser(tWMVDecInternalMember *pWMVDec,
                                                        CWMVMBMode* pmbmd,
                                                        CWMVMBMode* pmbmd_lastrow,
                                                        int imbY,
                                                        I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,
                                                        EMB_PBMainLoop *pPB);
tWMVDecodeStatus ProcessMBRow_MC(tWMVDecInternalMember *pWMVDec,CWMVMBMode* pmbmd,I32_WMV imbY,
                                                     I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,EMB_PBMainLoop *pPB, U32_WMV threadflag );
tWMVDecodeStatus ProcessMBRow_Parser_BVOP(tWMVDecInternalMember *pWMVDec,
                                                            CWMVMBMode* pmbmd,
                                                            CWMVMBMode* pmbmd_lastrow,
                                                            I32_WMV imbY,I32_WMV y,
                                                            I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,EMB_PBMainLoop *pPBB);
tWMVDecodeStatus ProcessMBRow_MC_BVOP(tWMVDecInternalMember *pWMVDec,CWMVMBMode* pmbmd,U32_WMV imbY,U32_WMV y,
                                                     I32_WMV *bIntraFlag, I32_WMV *b1MVFlag,EMB_PBMainLoop *pPBB,U32_WMV threadflag);
void AdjustDecRangeWMVA_DOWN(tWMVDecInternalMember *pWMVDec);

Void_WMV AdjustDecRange_DOWN(tWMVDecInternalMember *pWMVDec);
Void_WMV DeblockRowsRightCols (tWMVDecInternalMember *pWMVDec, int bottom_row,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,I32_WMV iNumRows, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4);
Void_WMV DeblockColumnsDownRows (tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    I32_WMV iNumCols, I32_WMV iHeight, 
    U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
	U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV);
I32_WMV initModeDependentMemory(tWMVDecInternalMember *pWMVDec, I32_WMV iMaxEncWidth, I32_WMV iMaxEncHeight);

#endif // __WMVDEC_FUNCTION_H_

