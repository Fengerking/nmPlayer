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

#include "voWMVDecID.h"

#ifdef macintosh
#include <math64.h>
#include <DriverServices.h>
#endif


//#define COMBINE_2MC_TO_1MC

#ifdef VIDEO_MEM_SHARE
tYUV420Frame_WMV*  buf_seq_ctl(FIFOTYPE *priv, tYUV420Frame_WMV* img , const U32_WMV flag);
void	 reset_buf_seq(tWMVDecInternalMember *pWMVDec);
#endif

// ===========================================================================
// private functions
// ===========================================================================

// inits
Void_WMV reverseByteArray6(U8_WMV * p);
 Void_WMV  setCodedBlockPattern2 (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) ;
Void_WMV  setCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) ;

Bool_WMV getCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn) ;
typedef struct tCMVPred
{
    Bool_WMV bSameFieldHybridMV;
    Bool_WMV bOppFieldHybridMV;
    I32_WMV iPredSameX1;
    I32_WMV iPredSameY1;
    I32_WMV iPredSameX2;
    I32_WMV iPredSameY2;
    I32_WMV iPredOppX1;
    I32_WMV iPredOppY1;
    I32_WMV iPredOppX2;
    I32_WMV iPredOppY2;
    I32_WMV iPolarity;
} CMVPred;


typedef enum _VideoInterlaceMetaData {
    Interlace_Unknown	= 0,
	Interlace_Progressive	= 2,
	Interlace_FieldInterleavedUpperFirst	= 3,
	Interlace_FieldInterleavedLowerFirst	= 4,
	Interlace_FieldSingleUpperFirst	= 5,
	Interlace_FieldSingleLowerFirst	= 6,
	Interlace_MixedInterlace	= 7,
	Interlace_MixedInterlaceOrProgressive	= 8,
	Interlace_ForceDWORD	= 0x7fffffff
} VideoInterlaceMetaData; 


tWMVDecodeStatus WMVInternalMemberInit (
    tWMVDecInternalMember *pWMVDec,
    U32_WMV uiUserData,
    U32_WMV uiFOURCCCompressed,
    Float_WMV fltFrameRate,
    Float_WMV fltBitRate,
    I32_WMV iWidthSource,
    I32_WMV iHeightSource,
    I32_WMV iPostFilterLevel,
    tWMVDecodeDispRotateDegree iDispRotateDegree
);

// internal WMVideoDecDecodeSequenceHeader
tWMVDecodeStatus WMVideoDecDecodeSequenceHeaderInternal (HWMVDecoder hWMVDecoder, U8_WMV* pucBufferBits, U32_WMV uiBufferLengthFromCaller);

// destroys
tWMVDecodeStatus WMVInternalMemberDestroy (tWMVDecInternalMember *pWMVDec);

// Sequence level decoding
tWMVDecodeStatus setSliceCode (tWMVDecInternalMember *pWMVDec, I32_WMV iSliceCode);
tWMVDecodeStatus decodeVOLHead (tWMVDecInternalMember *pWMVDec);

// frame level decoding
tWMVDecodeStatus WMVideoDecGetOutputInternal (HWMVDecoder hWMVDecoder, U32_WMV uiFOURCCOutput, U16_WMV uiBitsPerPixelOutput, U32_WMV uiWidthBMPOutput, tWMVDecodeDispRotateDegree iDispRotateDegree, U8_WMV* pucDecodedOutput);
tWMVDecodeStatus WMVideoDecDecodeDataInternal (HWMVDecoder hWMVDecoder, U16_WMV* puiNumDecodedFrames);
tWMVDecodeStatus WMSpriteDecDecodeDataInternal (HWMVDecoder hWMVDecoder, I32_WMV iWidthDsiplay, I32_WMV iHeightDisplay);
Void_WMV WMVideoDecAssignMotionCompRoutines (tWMVDecInternalMember *pWMVDec);
Void_WMV WMVideoDecSwapCurrAndRef (tWMVDecInternalMember *pWMVDec, Bool_WMV bSwapPostBuffers);
tWMVDecodeStatus WMVideoDecDecodeFrameHead (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeFrameHead2 (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeClipInfo (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus WMVideoDecDecodeI_X8 (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeI (tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus WMVideoDecDecodeP (tWMVDecInternalMember *pWMVDec);

Bool_WMV IsProcessorFeaturePresentWMV (U32_WMV ProcessorFeature);

#if defined(WMV_OPT_COMMON_ARM)
    extern void memclr256_arm (void* pDst);
    extern void memclr128_arm (void* pDst);
    extern void memcpyOddEven_arm (void* pDst1, void* pSrc, const int iWidth1);

    extern void prvWMMemClr(const void *pDst, const int cBytes2Clear);
    /*cBytes2Clear mod 4 == 0 and pDest must be aligned*/
    extern void prvWMMemClrAligned(const void* pDest, const int cBytes2Clear );
    /*dest and src must both aligned and nbytes mod 4 = 0*/
    extern void prvWMMemCpyAligned(const void *dest, const void *src, const int nbytes);
    extern void prvWMMemSetAligned(const void *dest, const unsigned char val, const int nbytes);
    /*dest and src can be unaligned*/
    extern void prvWMMemCpy(const void *dest, const void *src, const int nbytes);

    /* Dest is word aligned, iDstStride, iSrcStride mod 4 = 0, copy 8 lines of 8 bytes with line increase of iDstStride, and iSrcStride */
    extern void FASTCPY_8x8(const void *pDst2, const void *pSrc2, const int iDstStride, const int iSrcStride);
    /* Dest is word aligned, iDstStride, iSrcStride mod 4 = 0, copy 8 lines of 16 bytes with line increase of iDstStride, and iSrcStride */
    extern void FASTCPY_8x16(const void *pDst2, const void *pSrc2, const int iDstStride, const int iSrcStride);
    /* Fast copy of 16 bytes with aligned dest and src */
    extern void prvWMMemCpy16Aligned(const void *dest, const void *src);
    /* Fast copy of 8 bytes with aligned dest and src */
    extern void prvWMMemCpy8Aligned(const void *dest, const void *src);
    /* Fast copy of 16 bytes with Destination aligned, Src can be not aligned */
    extern void prvWMMemCpy16DestAligned(const void *dest, const void *src);

    /* ARM CPU processors detection */
    extern U32_WMV g_supportStrongARM ();
    extern U32_WMV g_supportXScale ();
    extern U32_WMV g_supportARMv5 ();
    extern U32_WMV g_supportWMMX ();

#endif

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_GRAY4) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB12) ||defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)
tWMVDecodeStatus g_InitDstTables_WMV (tWMVDecInternalMember *pWMVDec);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8) || defined (OUTPUT_RGB16) || defined (OUTPUT_RGB24) || defined (OUTPUT_RGB32)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_GRAY4)
Void_WMV WMVideoDecUpdateDstPartialMBGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkGRAY4 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)
//Void_WMV g_InitDstDitherMap_WMV ();
Void_WMV WMVideoDecUpdateDstPartialMBRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB8 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB8)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB12)
Void_WMV WMVideoDecUpdateDstPartialMBRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
extern  Void_WMV WMVideoDecUpdateDstMBRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
extern  Void_WMV WMVideoDecUpdateDstBlkRGB12 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)
Void_WMV WMVideoDecUpdateDstPartialMBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDst411PartialMBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstPartialMBRGB15 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDst411PartialMBRGB15 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBRGB15 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDst411MBRGB15 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB15 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDst411MBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth);
extern  Void_WMV WMVideoDecUpdateDstMBRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
extern  Void_WMV WMVideoDecUpdateDstBlkRGB16 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#ifdef WMV_OPT_DECOLORCONV_ARM
extern  Void_WMV WMVideoDecUpdateDstMBRGB16Rotate90 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
extern  Void_WMV WMVideoDecUpdateDstMBRGB16Rotate270 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#   ifdef _WMMX_
    extern  Void_WMV WMVideoDecUpdateDstMBRGB16_WMMX (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
    extern Void_WMV WMVideoDecUpdateDstMBIYUV_WMMX (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBitsY, U8_WMV* pBitsU, U8_WMV* pBitsV,
        const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidthY, I32_WMV iBitmapWidthUV
    );
#   endif
#endif

#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB16)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)
Void_WMV WMVideoDecUpdateDstPartialMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);


Void_WMV WMVideoDecUpdateDst411MBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDst411PartialMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV *pBits,
    const U8_WMV *pucCurrYMB, const U8_WMV *pucCurrUMB, const U8_WMV *pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV, I32_WMV iRgbWidth, I32_WMV iRgbHeight
);




#   if  !defined(OPT_DECOLORCONV_SH4)
Void_WMV WMVideoDecUpdateDstMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV *pBits,
    const U8_WMV *pucCurrYMB, const U8_WMV *pucCurrUMB, const U8_WMV *pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);

#   else
    extern  Void_WMV WMVideoDecUpdateDstMBRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
    extern  Void_WMV WMVideoDecUpdateDstBlkRGB24 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);


//  extern  Void_WMV WMVVideoDecInitDecolorSH4 (float *matrix); This is defined if we use a vector transformation for decolor

#endif //OPT_DECOLORCONV_SH4
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB24)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)
Void_WMV WMVideoDecUpdateDstPartialMBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);

Void_WMV WMVideoDecUpdateDst411MBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDst411PartialMBRGB32 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV *pBits,
    const U8_WMV *pucCurrYMB, const U8_WMV *pucCurrUMB, const U8_WMV *pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV, I32_WMV iRgbWidth, I32_WMV iRgbHeight
);

#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_RGB32)

// packed YUV formats

Void_WMV WMVideoDecUpdateDst411PartialMBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDst411MBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)
Void_WMV WMVideoDecUpdateDstPartialMBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkYUY2 (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
    Void_WMV WMVideoDecUpdateDstLineYUY2 (
        U8_WMV* pucBmp,
        const U8_WMV*  pucCurrY, const U8_WMV*  pucCurrU, const U8_WMV* pucCurrV,
        const I32_WMV iWidthOrigY
    );
#   ifdef _WMV_TARGET_X86_
//#pragma warning(disable: 4799)  // emms warning disable
FORCEINLINE Void_WMV ALIGNED32_FASTMEMCLR_MMX(void * plus1, I32_WMV iSize)
{
        __asm {
//          push edi;
//          push ecx;
            pxor  mm1, mm1;
            mov  edi, plus1;
            mov  ecx, iSize
MEMSET_LOOP:
            movq [edi], mm1;
            movq [edi+8], mm1;
            movq [edi+16], mm1;
            movq [edi+24], mm1;
            add edi, 32;
            sub ecx, 32;
            jnz MEMSET_LOOP;
//          pop edi;
//          pop ecx;
        }
}

//#pragma warning(default: 4799)  // emms warning disable
        Void_WMV WMVideoDecUpdateDstLineYUY2MMX (
            U8_WMV* pucBmp,
            const U8_WMV*  pucCurrY, const U8_WMV*  pucCurrU, const U8_WMV* pucCurrV,
            const I32_WMV iWidthOrigY
        );
        Void_WMV WMVideoDecUpdateDstPlaneMBAlignedYV12MMX(
            U8_WMV* pBits,
             U8_WMV* pucCurr,
             I32_WMV iLines, I32_WMV iWidthOrig,
             I32_WMV iWidthOrigInc,
             I32_WMV iBitmapWidth
        );

        Void_WMV WMVideoDecUpdateDstPlaneNonMBAlignedYV12MMX(
            U8_WMV* pBits,
             U8_WMV* pucCurr,
             I32_WMV iLines, I32_WMV iWidthOrig,
             I32_WMV iWidthOrigInc,
             I32_WMV iBitmapWidth
        );

#       ifndef UNDER_CE
            Void_WMV WMVideoDecUpdateDstLineYUY2KNI (
                U8_WMV* pucBmp,
                const U8_WMV*  pucCurrY, const U8_WMV*  pucCurrU, const U8_WMV* pucCurrV,
                const I32_WMV iWidthOrigY
            );
#       endif // UNDER_CE
#   endif // _WMV_TARGET_X86_
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YUY2)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)
Void_WMV WMVideoDecUpdateDstPartialMBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDstMBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
    Void_WMV WMVideoDecUpdateDstLineUYVY (
        U8_WMV* pucBmp,
        const U8_WMV*  pucCurrY, const U8_WMV*  pucCurrU, const U8_WMV* pucCurrV
,
        const I32_WMV iWidthOrigY
    );
#   ifdef _WMV_TARGET_X86_
        Void_WMV WMVideoDecUpdateDstLineUYVYMMX (
            U8_WMV* pucBmp,
            const U8_WMV*  pucCurrY, const U8_WMV*  pucCurrU, const U8_WMV* pucCurrV,
            const I32_WMV iWidthOrigY
        );
#       ifndef UNDER_CE
            Void_WMV WMVideoDecUpdateDstLineUYVYKNI (
                U8_WMV* pucBmp,
                const U8_WMV*  pucCurrY, const U8_WMV*  pucCurrU, const U8_WMV* pucCurrV,
                const I32_WMV iWidthOrigY
            );
#       endif // UNDER_CE
#   endif // _WMV_TARGET_X86_
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_YVYU)
Void_WMV WMVideoDecUpdateDstPartialMBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);

Void_WMV WMVideoDecUpdateDstMBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDstBlkYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)

#if defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_IYUV) || defined (OUTPUT_I420) || defined (OUTPUT_YV12)
Void_WMV WMVideoDecUpdateDstPartialMBIYUV (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBitsY, U8_WMV* pBitsU, U8_WMV* pBitsV,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidthY, I32_WMV iBitmapWidthUV,
    I32_WMV iValidWidthY, I32_WMV iValidHeightY
);

Void_WMV WMVideoDecUpdateDstMBIYUV (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBitsY, U8_WMV* pBitsU, U8_WMV* pBitsV,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidthY, I32_WMV iBitmapWidthUV
);
 
Void_WMV WMVideoDecUpdateDstNonMBAlignedIYUV (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBitsY, U8_WMV* pBitsU, U8_WMV* pBitsV,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidthY, I32_WMV iBitmapWidthUV
);

Void_WMV WMVideoDecUpdateDstPlaneMBAlignedYV12(
    U8_WMV* pBits,
     U8_WMV* pucCurr,
     I32_WMV iLines, I32_WMV iWidthOrig,
     I32_WMV iWidthOrigInc,
     I32_WMV iBitmapWidth
);

Void_WMV WMVideoDecUpdateDstPlaneNonMBAlignedYV12(
    U8_WMV* pBits,
     U8_WMV* pucCurr,
     I32_WMV iLines, I32_WMV iWidthOrig,
     I32_WMV iWidthOrigInc,        
     I32_WMV iBitmapWidth        
);


#endif // defined (OUTPUT_ALL_FORMATS) || defined (OUTPUT_UYVY)



//Init routines
tWMVDecodeStatus VodecConstruct (
    tWMVDecInternalMember *pWMVDec, 
    U32_WMV               dwFOURCC,
    I32_WMV                 iFrameRate,
    I32_WMV                 iWidthSource, 
    I32_WMV                 iHeightSource,
    CInputBitStream_WMV     *pInputBitstream//,
);
tWMVDecodeStatus  VodecDestruct (tWMVDecInternalMember *pWMVDec);
//tWMVDecodeStatus  VodecDestruct_WMVA (tWMVDecInternalMember *pWMVDec);//sw:20080108
Void_WMV   m_InitIDCT_Dec(tWMVDecInternalMember *pWMVDec, Bool_WMV fFlag);
Void_WMV    m_InitFncPtrAndZigzag(tWMVDecInternalMember *pWMVDec);
Void_WMV    setRefreshPeriod(tWMVDecInternalMember *pWMVDec);
Void_WMV    decideMMXRoutines (tWMVDecInternalMember *pWMVDec);
I32_WMV     assignVODataMembers (tWMVDecInternalMember *pWMVDec);
U8_WMV    * g_InitDecGlobalVars (Void_WMV);
tWMVDecodeStatus InitDataMembers (
    tWMVDecInternalMember *pWMVDec,     
    I32_WMV                iWidthSource, 
    I32_WMV                iHeightSource
);
tWMVDecodeStatus InitDataMembers_WMVA (
    tWMVDecInternalMember *pWMVDec,     
    I32_WMV                iWidthSource, 
    I32_WMV                iHeightSource
);
Void_WMV DecodeSkipBit(tWMVDecInternalMember *pWMVDec);
Void_WMV decodeVOPHead_WMV2 (tWMVDecInternalMember *pWMVDec);
Void_WMV UpdateDCStepSize(tWMVDecInternalMember *pWMVDec, I32_WMV iStepSize);

// Loopfilter

#ifndef WMV9_SIMPLE_ONLY
extern Void_WMV g_FilterHorizontalEdge_WMV(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
extern Void_WMV g_FilterVerticalEdge_WMV(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#endif

#ifdef _WMV_TARGET_X86_
Void_WMV DeblockInterlaceFrame_MMX (tWMVDecInternalMember *pWMVDec, U32_WMV iMBStartY, U32_WMV iMBEndY);
Void_WMV DeblockFieldInterlaceV2_MMX (tWMVDecInternalMember *pWMVDec, 
        U8_WMV __huge* ppxliY,
        U8_WMV __huge* ppxliU,
        U8_WMV __huge* ppxliV,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );
Void_WMV DeblockIFrame_MMX (tWMVDecInternalMember *pWMVDec,
        U8_WMV * ppxliY,
        U8_WMV * ppxliU,
        U8_WMV * ppxliV,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );
    Void_WMV DeblockPFrame_MMX (tWMVDecInternalMember *pWMVDec,
        U8_WMV * ppxliY,
        U8_WMV * ppxliU,
        U8_WMV * ppxliV,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );
Void_WMV g_FilterHorizontalEdge_MMX(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdge_MMX(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9_MMX (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                          I32_WMV iNumPixel);
Void_WMV g_FilterHorizontalEdgeV9_MMX(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                            I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9_KNI (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                          I32_WMV iNumPixel);
Void_WMV g_FilterHorizontalEdgeV9_KNI(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, 
                            I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV8_MMX(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iVertSize);
Void_WMV g_FilterHorizontalEdgeV8_MMX(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
    Void_WMV WMVideoDecUpdateDstMBYUY2_MMX (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
    Void_WMV WMVideoDecUpdateDstMBYUY2_KNI (
        tWMVDecInternalMember *pWMVDec,
        U8_WMV* pBits,
        const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
        I32_WMV iWidthY, I32_WMV iWidthUV,
        I32_WMV iBitmapWidth
    );
    Void_WMV AdjustReconRange_MMX (tWMVDecInternalMember *pWMVDec);
#endif //_WMV_TARGET_X86_

#if defined(macintosh) && defined(_MAC_VEC_OPT)
Void_WMV g_FilterHorizontalEdge_AltiVec(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdge_AltiVec(U8_WMV* pV, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterHorizontalEdgeV9_AltiVec(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#endif

#ifndef WMV9_SIMPLE_ONLY
Void_WMV DeblockSLFrame (tWMVDecInternalMember *pWMVDec,
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    Bool_WMV bMultiThread, U32_WMV iThreadID, I32_WMV iMBStartX, I32_WMV iMBEndX, I32_WMV iMBStartY, I32_WMV iMBEndY
) ;

Void_WMV DeblockSLFrame_V8 (
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV,
    Bool_WMV bMultiThread, U32_WMV iThreadID, I32_WMV iMBStartX, I32_WMV iMBEndX, I32_WMV iMBStartY, I32_WMV iMBEndY
); 

//Deblocking 
Void_WMV DeblockFrame (tWMVDecInternalMember *pWMVDec, U32_WMV iMBStartY, U32_WMV iMBEndY);
Void_WMV DeblockInterlaceFrame (tWMVDecInternalMember *pWMVDec, U32_WMV iMBStartY, U32_WMV iMBEndY);
#endif
Void_WMV SetupPostFilterMode (tWMVDecInternalMember *pWMVDec);
#if defined(_SUPPORT_POST_FILTERS_)
    tWMVDecodeStatus WMVDecideDeblockLevel (tWMVDecInternalMember *pWMVDec, I32_WMV iWidth, I32_WMV iHeight);
#endif

    Void_WMV FilterEdgeShortTagBlockRow (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, 
                            I32_WMV iStepSize, I32_WMV iMBsPerRow, Bool_WMV bFirst, Bool_WMV bLast);
    Void_WMV FilterEdgeShortTagMBRow (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxlcCenter, I32_WMV iPixelDistance, 
                            I32_WMV iStepSize, I32_WMV iMBsPerRow, Bool_WMV bFirst, Bool_WMV bLast);

    I32_WMV divroundnearest(I32_WMV i, I32_WMV iDenom);

// Block based routines
tWMVDecodeStatus DecodeIMBAcPred (
    tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*        pmbmd, 
    U8_WMV*  ppxliTextureQMBY, 
    U8_WMV*  ppxliTextureQMBU, 
    U8_WMV*  ppxliTextureQMBV, 
    I16_WMV*            piQuanCoefACPred, 
    I16_WMV**           piQuanCoefACPredTable, 
    Bool_WMV            bLeftMB, 
    Bool_WMV            bTopMB, 
    Bool_WMV            bLeftTopMB,
    I32_WMV              iMBX
);

tWMVDecodeStatus DecodeIMBAcPred_PFrame (
    tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*        pmbmd, 
    U8_WMV*  ppxliTextureQMBY, 
    U8_WMV*  ppxliTextureQMBU, 
    U8_WMV*  ppxliTextureQMBV, 
    I16_WMV*            piQuanCoefACPred, 
    I16_WMV**           piQuanCoefACPredTable, 
    Bool_WMV            bLeftMB, 
    Bool_WMV            bTopMB, 
    Bool_WMV            bLeftTopMB
);

#ifndef _EMB_WMV2_
tWMVDecodeStatus DecodePMB (
    tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*        pmbmd,
    U8_WMV* ppxlcCurrQYMB,
    U8_WMV* ppxlcCurrQUMB,
    U8_WMV* ppxlcCurrQVMB,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
);
#endif //_EMB_WMV2_

// decode MB header 
tWMVDecodeStatus decodeMBOverheadOfIVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
tWMVDecodeStatus decodeMBOverheadOfPVOP_MSV (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
// decode MV
Bool_WMV decideHybridMVOn (tWMVDecInternalMember *pWMVDec, CoordI x, Bool_WMV bNot1stRowInSlice, const CVector* pmv);
Void_WMV find16x16MVpred (
    tWMVDecInternalMember *pWMVDec, 
    CVector* pvecPredHalfPel, 
    const CVector* pmv,
    const CWMVMBMode* pmbmd,
    Bool_WMV bLeftBndry, 
    Bool_WMV bRightBndry, 
    Bool_WMV bTopBndry
);
tWMVDecodeStatus decodeMVMSV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry, Huffman_WMV* pHufMVDec, U8_WMV* uXMvFromIndex, U8_WMV* uYMvFromIndex);
   

// Decode Coefs
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode,
       DQuantDecParam *pDQ                                 
     );
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MMX (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode,
       DQuantDecParam *pDQ                         
       );

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
    Bool_WMV CBP, I16_WMV* pDct, U8_WMV* piZigzagInv);

tWMVDecodeStatus DecodeInverseIntraBlockQuantizeAcPred (tWMVDecInternalMember *pWMVDec,
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
    Bool_WMV CBP, 
    I16_WMV* pDct, 
    U8_WMV* piZigzagInv
);

tWMVDecodeStatus decodeIntraBlockAcPred (tWMVDecInternalMember *pWMVDec, 
    CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
    CWMVMBMode*        pmbmd,
    U8_WMV   *ppxliTextureQMB,        //place to set the pixels
    I32_WMV             iOffsetToNextRowForDCT,
    I32_WMV             ib, 
    I16_WMV*            piQuanCoef1stRowColAC, 
    I16_WMV*            pRef, 
    Bool_WMV            bPredPattern,
    I32_WMV              iMBX
);
Void_WMV decodeBitsOfESCCode (tWMVDecInternalMember *pWMVDec);


// Motion Compensation
#ifndef _EMB_WMV2_
Void_WMV MotionCompMixed (MOTION_COMP_ARGS);
Void_WMV MotionCompMixedAndAddError (MOTION_COMP_ADD_ERROR_ARGS);
#endif //_EMB_WMV2_

Void_WMV MotionCompMixed_MMX (MOTION_COMP_ARGS);
Void_WMV MotionCompMixedAndAddError_MMX (MOTION_COMP_ADD_ERROR_ARGS);

Void_WMV MotionCompMixed_KNI (MOTION_COMP_ARGS);
Void_WMV MotionCompMixedAndAddError_KNI (MOTION_COMP_ADD_ERROR_ARGS);


// AC_DC Pred    
I16_WMV* decodeDiffDCTCoef(
    tWMVDecInternalMember *pWMVDec, 
    I16_WMV**   piQuanCoefACPredTable,
    Bool_WMV    bLeftMB,
    Bool_WMV    bTopMB,
    Bool_WMV    bLeftTopMB, 
    Bool_WMV*   pbPredPattern
);
I16_WMV* decodeDiffDCTCoef_WMV(
    tWMVDecInternalMember *pWMVDec, 
    U8_WMV* ppxliTextureQMB,
    I16_WMV**   piQuanCoefACPredTable,
    Bool_WMV    bLeftMB,
    Bool_WMV    bTopMB,
    Bool_WMV    bLeftTopMB, 
    Bool_WMV    bLeftMBAndInter,
    Bool_WMV    bTopMBAndInter,
    Bool_WMV    bLeftTopMBAndInter, 
    Bool_WMV*   pbPredPattern,
    I32_WMV     iWidthPrev,
    I32_WMV     iStepSize
);
I16_WMV* decodeDiffDCTCoef_WMV_Y23(
    I16_WMV** piQuanCoefACPredTable, 
    Bool_WMV bLeftMB, 
    Bool_WMV bTopMB, 
    Bool_WMV bLeftTopMB, 
    Bool_WMV* pbPredPattern
);
Void_WMV decodeIntraDCAcPredMSV (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCTStepSize);
I32_WMV BlkAvgX8(const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize);
I32_WMV BlkAvgX8_MMX(const U8_WMV* ppxlcCurrRecnMB, I32_WMV iWidthPrev, I32_WMV iStepSize);


// Routines to support MP4X's routines.
//#ifdef _WMVONLY_
tWMVDecodeStatus DecodeIMBAcPred_MP4X(tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, U8_WMV* ppxliTextureQMBY, U8_WMV* ppxliTextureQMBU, U8_WMV* ppxliTextureQMBV, 
    I16_WMV* piQuanCoefACPred, I16_WMV** piQuanCoefACPredTable, Bool_WMV bLeftMB, Bool_WMV bTopMB, Bool_WMV bLeftTopMB, I32_WMV dummy);
Void_WMV decodeIntraDCAcPred (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC);
tWMVDecodeStatus decodeMBOverheadOfIVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
tWMVDecodeStatus decodeMBOverheadOfPVOP_New (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
tWMVDecodeStatus decodeMV (tWMVDecInternalMember *pWMVDec, CVector* pmv, const CWMVMBMode* pmbmd, Bool_WMV bLeftBndry, Bool_WMV bRightBndry, Bool_WMV bTopBndry);
I32_WMV checkResyncMarker(tWMVDecInternalMember *pWMVDec);
tWMVDecodeStatus decodeMBOverheadOfIVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);
tWMVDecodeStatus decodeMBOverheadOfPVOP_MPEG4 (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
Void_WMV decodeIntraDCAcPred_MPEG4 (tWMVDecInternalMember *pWMVDec, tWMVDecodeStatus* pError, U8_WMV DecodeCodeTableIntraDC[], I32_WMV iMaxCodeWordLngIntraDC);
#ifndef WMV9_SIMPLE_ONLY
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** pInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode, DQuantDecParam *pDQ);
#endif
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MMX_MPEG4(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode,
     DQuantDecParam *pDQ                                                                                                     
      );
tWMVDecodeStatus decodeVOLHeadMP4S(tWMVDecInternalMember *pWMVDec);
I32_WMV decodeVideoPacketHeader(tWMVDecInternalMember *pWMVDec, I32_WMV* piCurrentQP);
Void_WMV getDiffMV (tWMVDecInternalMember *pWMVDec, CVector* pvctDiffMV);
Void_WMV fitMvInRange (CVector* pvctSrc, I32_WMV iMvRange);

#ifdef _EMB_WMV2_
tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCode_MPEG4_EMB(tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV *pZigzagInv, I32_WMV iXformMode);

tWMVDecodeStatus DecodePMB_EMB (
    tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
//    const Bool_WMV* rgCBP, 
    U8_WMV* ppxlcCurrQYMB,
    U8_WMV* ppxlcCurrQUMB,
    U8_WMV* ppxlcCurrQVMB,
    CoordI xRefY, CoordI yRefY,
    CoordI xRefUV, CoordI yRefUV
);

Void_WMV InitEMB_FuncTable(tWMVDecInternalMember *pWMVDec);

extern   Void_WMV MotionCompMixed011(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed010(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed110(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed001(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed101(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed000(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed100(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed111(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompWAddError01(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern  Void_WMV MotionCompWAddError10(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern  Void_WMV MotionCompWAddError00(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern  Void_WMV MotionCompMixedAlignBlock(U8_WMV*   pBlock, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev);
extern  Void_WMV MotionCompMixedHQHI(U8_WMV* ppxlcPredMB,  const U8_WMV* ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * ppxliErrorBuf, U8_WMV* alignedBlock);

extern  Void_WMV MotionCompMixed011Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed010Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed110Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed001Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed101Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed000Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed100Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompMixed111Complete(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf);
extern  Void_WMV MotionCompWAddError01Complete(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern  Void_WMV MotionCompWAddError10Complete(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern  Void_WMV MotionCompWAddError00Complete(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf);
extern  Void_WMV MotionCompMixedAlignBlockComplete(U8_WMV*   pBlock, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev);
extern  Void_WMV MotionCompMixedHQHIComplete(U8_WMV* ppxlcPredMB, const U8_WMV* ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * ppxliErrorBuf, U8_WMV* alignedBlock);

 tWMVDecodeStatus DecodeInverseInterBlockQuantizeEscCodeEMB (tWMVDecInternalMember *pWMVDec, CDCTTableInfo_Dec* InterDCTTableInfo_Dec, U8_WMV* pZigzag, EMB_DecodeP_ShortCut *);


#ifdef IDCTDEC_16BIT_3ARGS
extern  Void_WMV g_IDCTDec_WMV2_16bit (UnionBuffer __huge* piDst, UnionBuffer __huge* piSrc,  I32_WMV iDCTHorzFlags);
extern  Void_WMV g_8x4IDCTDec_WMV2_16bit (UnionBuffer __huge* piDstBuf, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
extern  Void_WMV g_4x8IDCTDec_WMV2_16bit (UnionBuffer __huge* piDstBuf, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
#else
extern  Void_WMV g_IDCTDec_WMV2_16bit (UnionBuffer __huge* piDst, UnionBuffer __huge* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
extern  Void_WMV g_8x4IDCTDec_WMV2_16bit (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
extern  Void_WMV g_4x8IDCTDec_WMV2_16bit (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iHalfAndiDCTHorzFlags);
#endif

Void_WMV InitEMB_DecodePShortcut(tWMVDecInternalMember *pWMVDec);
#endif


Void_WMV Huffman_WMV_construct(Huffman_WMV * pThis);
Void_WMV Huffman_WMV_destruct(tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis);
Void_WMV Huffman_WMV8_destruct(tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis);
VResultCode Huffman_WMV_init (tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis, U32_WMV uiUserData, tPackHuffmanCode_WMV huffArray[], U8_WMV tableSize);
VResultCode Huffman_WMV8_init (tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis, U32_WMV uiUserData, tPackHuffmanCode_WMV huffArray[], U8_WMV tableSize);
VResultCode Huffman_WMV_init16 (I16_WMV *pDecodeTable, tPackHuffmanCode_WMV huffArray[], I32_WMV *pSymbolInfo, U32_WMV iRootBits);

U32_WMV getHuffman(CInputBitStream_WMV *bs,
                   I16_WMV* pDecodeTable, 
                   U32_WMV  iRootBits     
);

U32_WMV getHuffmanAndPeekbit(CInputBitStream_WMV *bs,
                   I16_WMV* pDecodeTable, 
                   U32_WMV  iRootBits,
                   I32_WMV* signBit
);

FORCEINLINE U32_WMV Huffman_WMV_get(Huffman_WMV * pThis, CInputBitStream_WMV *bs)
{
    U32_WMV ret = getHuffman(bs, 
        pThis->m_hufDecTable, 
        pThis->m_tableSize);
    COVERAGE_HUFFMAN (pThis, ret);
    
    return ret;
}

FORCEINLINE U32_WMV Huffman_WMV_get_peek(Huffman_WMV * pThis, CInputBitStream_WMV *bs, I32_WMV* signBit)
{
    U32_WMV ret = getHuffmanAndPeekbit(bs, 
        pThis->m_hufDecTable, 
        pThis->m_tableSize,
        signBit);
    
    return ret;
}

Void_WMV BS_construct (CInputBitStream_WMV * pThis, U32_WMV uiUserData);
Void_WMV BS_reset(
    CInputBitStream_WMV * pThis,
    U8_WMV          *pBuffer,
    U32_WMV        dwBuflen,
    Bool_WMV        bNotEndOfFrame
#ifdef _WMV9AP_
                  ,Bool_WMV       bStartcodePrevention
#endif
    );

#ifdef DYNAMIC_EDGEPAD
tWMVDecodeStatus edgePadInit(tWMVDecInternalMember *pWMVDec, tWMVEdgePad * pEdgePad );
Void_WMV edgePadDestroy(tWMVEdgePad * pEdgePad );
Void_WMV edgePadSetup(tWMVDecInternalMember *pWMVDec, tWMVEdgePad * pEdgePad );
#endif

#if defined(_6BIT_COLORCONV_OPT_) || defined(OUTPUT_ALL_FORMATS) || defined(OUTPUT_RGB12) || defined (OUTPUT_GRAY4)
Void_WMV g_EndDstTables_WMV(tWMVDecInternalMember *pWMVDec);
#endif



tWMVDecodeStatus  Plane_init (tYUV420Frame_WMV * pFramePlane, I32_WMV iSizeYplane, I32_WMV iSizeUVplane, I32_WMV iPad, tWMVDecInternalMember *pWMVDec, I32_WMV iPlaneWidth, I32_WMV iPlaneHeight);

//interlace
   
 

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
    Void_WMV SetStartOfLibrary(CReferenceLibrary *,I32_WMV iStart);
    Void_WMV RL_RemoveFrame (CReferenceLibrary *,Void_WMV**, I32_WMV);
    Void_WMV RL_AddFrame (CReferenceLibrary *,Void_WMV *);
    I32_WMV RL_GetNextFrame (CReferenceLibrary *,Void_WMV **);


    Void_WMV InitMultires(tWMVDecInternalMember * pWMVDec);
    Void_WMV ChangeMultiresFilter(tWMVDecInternalMember *pWMVDec, I32_WMV iFilterIndex);
#ifndef WMV9_SIMPLE_ONLY
    Void_WMV SetupMultiResParams(tWMVDecInternalMember *pWMVDec);
    Void_WMV ResetParams(tWMVDecInternalMember *pWMVDec, I32_WMV iResIndex);
#endif
    Void_WMV UpsampleRefFrame(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV);
    Void_WMV DownsampleRefFrame(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV);
//    Void_WMV HorizDownsampleFilter(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iPitchY, I32_WMV iPitchUV);
//    Void_WMV VertDownsampleFilter(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, I32_WMV iWidth, I32_WMV iHeight, I32_WMV iPitchY, I32_WMV iPitchUV);
//    Void_WMV HorizUpsampleFilter(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, I32_WMV iVScale);
//    Void_WMV VertUpsampleFilter(U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV);

    Void_WMV ResetDecoderParams (tWMVDecInternalMember *pWMVDec);
//    Void_WMV swapCurrAndRef(tWMVDecInternalMember * pWMVDec);
    Void_WMV InitDeinterlace(tWMVDecInternalMember * pWMVDec);
    tWMVDecodeStatus DecodeSkipBitX9 (tWMVDecInternalMember * pWMVDec, I32_WMV iMode);

    tWMVDecodeStatus decodeVOPHead_WMV3(tWMVDecInternalMember * pWMVDec);
    tWMVDecodeStatus initMultiThreadVars_Dec (tWMVDecInternalMember * pWMVDec);
    Void_WMV InitBlockInterpolation (tWMVDecInternalMember * pWMVDec);
    tWMVDecodeStatus VideoDIDeinterlace(tWMVDecInternalMember * pWMVDec, U8_WMV* pucInput, U8_WMV* pucOutput, I32_WMV iFirstLine, I32_WMV iLastLine, I32_WMV iWidth, I32_WMV iHeight);

    tWMVDecodeStatus WMVideoDecDecodeP_X9 (tWMVDecInternalMember *pWMVDec);

    
    

#ifdef _WMV9AP_
    tWMVDecodeStatus decodeVOLHead_WMV3(tWMVDecInternalMember * pWMVDec, U32_WMV uiFormatLength, 
        I32_WMV *piPIC_HORIZ_SIZE , I32_WMV *piPIC_VERT_SIZE,
        I32_WMV *piDISP_HORIZ_SIZE , I32_WMV *piDISP_VERT_SIZE );
#endif

    tWMVDecodeStatus decodeSpriteVOLHead(tWMVDecInternalMember * pWMVDec);

    Void_WMV ReInitContext(t_CContextWMV        *pContext, I32_WMV iCol, I32_WMV iRow);

 
    Void_WMV decodeMV_V9(tWMVDecInternalMember *pWMVDec, Huffman_WMV * pHufMVTable, I32_WMV iMode, CDiffMV *pDiffMV);

    I32_WMV PredictHybridMV (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV,
                                I32_WMV *iPredX, I32_WMV *iPredY);
    
    I32_WMV ComputeMVFromDiffMV (tWMVDecInternalMember *pWMVDec, I32_WMV  iBlockX, I32_WMV iBlockY,
                                                CDiffMV *pInDiffMV, Bool_WMV b1MV );
    
    I32_WMV PredictMV (tWMVDecInternalMember *pWMVDec, I32_WMV iBlockX, I32_WMV iBlockY, I32_WMV iMVMode,
                               I32_WMV iNumBlockX, I32_WMV iNumBlockY,
                               const I16_WMV *pMVX, const I16_WMV *pMVY,
                               I32_WMV *iXMotion, I32_WMV *iYMotion, Bool_WMV bTopBndry);

     Void_WMV decodeMV_V9(tWMVDecInternalMember *pWMVDec, Huffman_WMV* pHufMVTable, 
         I32_WMV iMode, CDiffMV *pDiffMV);

    tWMVDecodeStatus decodeMBOverheadOfPVOP_WMV3 (tWMVDecInternalMember *pWMVDec, 
        CWMVMBMode* pmbmd, I32_WMV x, I32_WMV y);


    Void_WMV MotionComp_X9 (tWMVDecInternalMember *pWMVDec, U8_WMV *pDst, UnionBuffer *pError,
                                           I32_WMV iBlockX, I32_WMV iBlockY, U8_WMV *pSrc);

    Void_WMV MotionCompMB_WMV3 (tWMVDecInternalMember *pWMVDec, U8_WMV *pDstY, U8_WMV *pDstU, U8_WMV *pDstV, I32_WMV iBlockX, I32_WMV iBlockY);
    Void_WMV MotionCompChroma_X9 (tWMVDecInternalMember *pWMVDec,  U8_WMV *pDst, UnionBuffer *pError, I32_WMV iMBindex, U8_WMV *pSrc);

    Void_WMV InterpolateMB (tWMVDecInternalMember *pWMVDec, const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,
                        I32_WMV iDX, I32_WMV iDY, I32_WMV iFilterType);

       I32_WMV IsIntraPredY (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY);
    I32_WMV IsIntraPredUV (tWMVDecInternalMember *pWMVDec, I32_WMV iMBX, I32_WMV iMBY);
        Void_WMV DecodeVOPDQuant (tWMVDecInternalMember *pWMVDec, Bool_WMV bPFrame);
    I32_WMV decodeDQuantStepsize(tWMVDecInternalMember *pWMVDec, I32_WMV iShift);



    tWMVDecodeStatus DecodeMB_X9 (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV * ppxlcCurrQYMB,
    U8_WMV * ppxlcCurrQUMB,
    U8_WMV * ppxlcCurrQVMB,
    I32_WMV  iMBX, I32_WMV iMBY,
    I32_WMV xRefY, I32_WMV yRefY,
    I32_WMV xRefUV, I32_WMV yRefUV);


    tWMVDecodeStatus DecodeMB_1MV_WMVA_Intra (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV * ppxlcCurrQYMB,
    U8_WMV * ppxlcCurrQUMB,
    U8_WMV * ppxlcCurrQVMB,
    I32_WMV  iMBX, I32_WMV iMBY,
    I32_WMV xRefY, I32_WMV yRefY,
    I32_WMV xRefUV, I32_WMV yRefUV);

    tWMVDecodeStatus DecodeMB_1MV (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
    U8_WMV * ppxlcCurrQYMB,
    U8_WMV * ppxlcCurrQUMB,
    U8_WMV * ppxlcCurrQVMB,
    I32_WMV  iMBX, I32_WMV iMBY,
    I32_WMV xRefY, I32_WMV yRefY,
    I32_WMV xRefUV, I32_WMV yRefUV);

    Void_WMV CopyCurrToPost (tWMVDecInternalMember *pWMVDec);

   
    Void_WMV DeblockIFrame (tWMVDecInternalMember *pWMVDec,
        U8_WMV * ppxliY,
        U8_WMV * ppxliU,
        U8_WMV * ppxliV,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );
    Void_WMV DeblockIFrameCurrToPost (tWMVDecInternalMember *pWMVDec,
        U8_WMV * ppxliSrcY,
        U8_WMV * ppxliSrcU,
        U8_WMV * ppxliSrcV,
        U8_WMV * ppxliDstY,
        U8_WMV * ppxliDstU,
        U8_WMV * ppxliDstV,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );
    Void_WMV DeblockPFrame (tWMVDecInternalMember *pWMVDec,
        U8_WMV * ppxliY,
        U8_WMV * ppxliU,
        U8_WMV * ppxliV,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );

    Void_WMV RenderIFrame (tWMVDecInternalMember *pWMVDec,
        U8_WMV * ppxliRecnQY,
        U8_WMV * ppxliRecnQU,                                       
        U8_WMV * ppxliRecnQV
    );
    Void_WMV RenderPFrame (tWMVDecInternalMember *pWMVDec,
        U8_WMV * ppxliRecnQY,
        U8_WMV * ppxliRecnQU,                                       
        U8_WMV * ppxliRecnQV
    );


    // for multi-threading, have to be public
    Void_WMV decode0 (tWMVDecInternalMember *pWMVDec);
    Void_WMV decode1 (tWMVDecInternalMember *pWMVDec);
//    Void_WMV encode2 ();
//    Void_WMV encode3 ();
    Void_WMV loopfilter0 (tWMVDecInternalMember *pWMVDec);
    Void_WMV loopfilter1 (tWMVDecInternalMember *pWMVDec);
//    Void_WMV loopfilter2 ();
//    Void_WMV loopfilter3 ();
    Void_WMV process0 (tWMVDecInternalMember *pWMVDec);
    Void_WMV process1 (tWMVDecInternalMember *pWMVDec);
//    Void_WMV process2 ();
//    Void_WMV process3 ();
    Void_WMV RenderFrame0 (tWMVDecInternalMember *pWMVDec,
        U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, 
        U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV
    );
    Void_WMV RenderFrame1 (tWMVDecInternalMember *pWMVDec,
        U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, 
        U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV
    );

    Void_WMV Deblock0 (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV);
    Void_WMV Deblock1 (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV);
//    Void_WMV DeblockCurrToPost0 (U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV);
//    Void_WMV DeblockCurrToPost1 (U8_WMV* ppxliCurrQY, U8_WMV* ppxliCurrQU, U8_WMV* ppxliCurrQV, U8_WMV* ppxliPostQY, U8_WMV* ppxliPostQU, U8_WMV* ppxliPostQV);

    Void_WMV Deinterlaceprocess0 (tWMVDecInternalMember *pWMVDec);
    Void_WMV Deinterlaceprocess1 (tWMVDecInternalMember *pWMVDec);
    Void_WMV Deinterlaceprocess2 (tWMVDecInternalMember *pWMVDec);
    Void_WMV Deinterlaceprocess3 (tWMVDecInternalMember *pWMVDec);

   

    // member functions for X9
#ifndef WMV9_SIMPLE_ONLY
    Void_WMV IntensityCompensation (tWMVDecInternalMember *pWMVDec);
#endif
    I32_WMV  PredictHybridMV (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV, I32_WMV *iPredX, I32_WMV *iPredY);
    I32_WMV  ChromaMV (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV);
    Void_WMV InitBlockInterpolation (tWMVDecInternalMember *pWMVDec);
    Void_WMV InterpolateYMB_X9 (tWMVDecInternalMember *pWMVDec, U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst,
                                I32_WMV iDstStride, I32_WMV iX, I32_WMV iY, I32_WMV iFilterType, I32_WMV iRndCtrl);
    Void_WMV InterpolateBlock (tWMVDecInternalMember *pWMVDec, const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride, 
                                I32_WMV iDX, I32_WMV iDY, I32_WMV iFilterType);

    Bool_WMV  decodeDCTPredictionY(tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I16_WMV*     pIntra,  I32_WMV iBlk,
    I32_WMV iX,     I32_WMV iY, 
    I32_WMV   *piShift, I32_WMV * p_iDirection, I16_WMV * pPredScaled);
    Bool_WMV  decodeDCTPredictionY_IFrameNoDQuantAndNoSlice(tWMVDecInternalMember *pWMVDec,
    CWMVMBMode* pmbmd, I16_WMV*     pIntra,  I32_WMV iBlk,
    I32_WMV iX,     I32_WMV iY, 
    I32_WMV   *piShift, I32_WMV * p_iDirection, I16_WMV ** pPredScaled);


	

#ifndef _WMV9AP_
    tWMVDecodeStatus DecodeInverseIntraBlockX9(tWMVDecInternalMember *pWMVDec,
                           CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                           I32_WMV iblk,     U8_WMV *pZigzagInv,    Bool_WMV bResidual,
                           I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                           I16_WMV *pIntra, CWMVMBMode* pmbmd, DQuantDecParam *pDQ);
#else
    tWMVDecodeStatus DecodeInverseIntraBlockX9(tWMVDecInternalMember *pWMVDec,
                           CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                           I32_WMV iblk,       Bool_WMV bResidual,
                           I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                           I16_WMV *pIntra, CWMVMBMode* pmbmd, DQuantDecParam *pDQ, UnionBuffer *pOut, I32_WMV iOutStride);


#endif
    Void_WMV g_IDCTDec_WMV2_Inter (UnionBuffer* piDst, UnionBuffer* piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);

    Bool_WMV decodeDCTPredictionUV(tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I16_WMV*     pIntra, 
    I32_WMV iX,     I32_WMV iY, 
    I32_WMV     *piShift,  I32_WMV * p_iDirection, I16_WMV * pPredScaled);
    Bool_WMV decodeDCTPredictionUV_IFrameNoDQuantAndNoSlice(tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I16_WMV*     pIntra, 
    I32_WMV iX,     I32_WMV iY, 
    I32_WMV     *piShift,  I32_WMV * p_iDirection, I16_WMV ** pPredScaled);
    I32_WMV medianof3 (I32_WMV a0, I32_WMV a1, I32_WMV a2);


    Void_WMV ComputeDQuantDecParam (tWMVDecInternalMember *pWMVDec);
    Void_WMV ScaleDCPredForDQuant (tWMVDecInternalMember *pWMVDec,I32_WMV iBlk, CWMVMBMode* pmbmd, 
        I32_WMV * p_iTopDC, I32_WMV * p_iLeftDC, I32_WMV * p_iTopLeftDC);
    Void_WMV ScaleTopPredForDQuant (tWMVDecInternalMember *pWMVDec,I32_WMV iBlk, I16_WMV *pPred,
        I16_WMV *pPredScaled, CWMVMBMode* pmbmd);
    Void_WMV ScaleLeftPredForDQuant (tWMVDecInternalMember *pWMVDec,I32_WMV iBlk, I16_WMV *pPred,
        I16_WMV *pPredScaled, CWMVMBMode* pmbmd);
    tWMVDecodeStatus decodeDQuantParam (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd);
 
    Void_WMV SwapRefAndPost ( tWMVDecInternalMember *pWMVDec );

    Void_WMV g_IDCTDec16Interlace_WMV2 (I16_WMV* piDst, I32_WMV* piSrc, 
        I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);

#ifndef _WMV9AP_
    Void_WMV ComputeLoopFilterFlags();
#else
#ifndef WMV9_SIMPLE_ONLY
    Void_WMV ComputeLoopFilterFlags_WMV9(tWMVDecInternalMember *pWMVDec);
#endif
    Void_WMV ComputeLoopFilterFlags_WMVA(tWMVDecInternalMember *pWMVDec);
#endif

    Void_WMV LoopFilterMultiThreads_V9 ();

    //
    // Intra 411
    //
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


    Void_WMV RenderFrame_AllMB_Packed_X1_Aligned_Interlace411 (
        U8_WMV __huge* ppxliPostY,
        U8_WMV __huge* ppxliPostU,
        U8_WMV __huge* ppxliPostV,
        U8_WMV __huge* pBmpPtr,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );

    Void_WMV StoreDCACPredCoefYInterlace411 (tWMVDecInternalMember *pWMVDec, 
        I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I16_WMV *piLevelBlk);    
    Void_WMV StoreDCACPredCoefUVInterlace411 (tWMVDecInternalMember *pWMVDec, 
        I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, I32_WMV iSubblock, I16_WMV *piLevelBlk); 

    //
    // Inter 411
    //
#ifndef WMV9_SIMPLE_ONLY
    tWMVDecodeStatus decodePInterlace411 (tWMVDecInternalMember *pWMVDec);
#endif
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


#ifndef WMV9_SIMPLE_ONLY
	Void_WMV ComputeLoopFilterFlags(tWMVDecInternalMember *pWMVDec);

    Void_WMV DeblockSLFrame_V9 (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* ppxliCurrY, U8_WMV* ppxliCurrU, U8_WMV* ppxliCurrV,
        U8_WMV* puchFilter8x8RowFlagY, U8_WMV* puchFilter8x8RowFlagU, U8_WMV* puchFilter8x8RowFlagV,
        U8_WMV* puchFilter8x4RowFlagY, U8_WMV* puchFilter8x4RowFlagU, U8_WMV* puchFilter8x4RowFlagV,
        U8_WMV* puchFilter8x8ColFlagY, U8_WMV* puchFilter8x8ColFlagU, U8_WMV* puchFilter8x8ColFlagV,
        U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV,
        LOOPF_FLAG* pLoopFilterFlag, U32_WMV uiMBStart, U32_WMV uiMBEnd);
#endif

  //  Void_WMV ClearFieldMB (I16_WMV *ppxliFieldMB);
    I16_WMV* GetFieldBlockPtr (I16_WMV *ppxliFieldMB, I32_WMV ib);
#ifdef DEBLOCK_H_NEON_OPT
    Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, I32_WMV iNumRows, 
                      U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
                      U8_WMV* puchFilterFlagY8x4, U8_WMV* puchFilterFlagU8x4, U8_WMV* puchFilterFlagV8x4);
	Void_WMV FilterRow_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize);
	Void_WMV FilterRowLast8x4_CtxA8 (U8_WMV* pRow, U8_WMV uchBitField8x4, I32_WMV iPitch, I32_WMV iStepSize);
#else
    Void_WMV DeblockRows (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, I32_WMV iNumRows, 
                      U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV);
    Void_WMV FilterRow (tWMVDecInternalMember *pWMVDec, U8_WMV* pRow, U8_WMV uchBitField, I32_WMV iPitch, I32_WMV iStepSize);
#endif

#ifdef DEBLOCK_V_NEON_OPT
    Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, I32_WMV iNumCols, I32_WMV iHeight, 
                         U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV,
						 U8_WMV* puchFilter4x8ColFlagY, U8_WMV* puchFilter4x8ColFlagU, U8_WMV* puchFilter4x8ColFlagV);
	Void_WMV FilterColumn_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize);
	Void_WMV FilterColumnLast4x8_CtxA8 (U8_WMV* pCol, U8_WMV uchBitField4x8, I32_WMV iPitch, I32_WMV iStepSize);
#else
    Void_WMV DeblockColumns (tWMVDecInternalMember *pWMVDec, U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, I32_WMV iNumCols, I32_WMV iHeight, 
                         U8_WMV* puchFilterFlagY, U8_WMV* puchFilterFlagU, U8_WMV* puchFilterFlagV);
    Void_WMV FilterColumn (tWMVDecInternalMember *pWMVDec, U8_WMV* pCol, U8_WMV uchBitField, I32_WMV iPitch, I32_WMV iStepSize);
#endif

#ifndef _WMV9AP_
    Void_WMV ComputeMBLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
                    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
            I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
         U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft);
#else
#ifndef WMV9_SIMPLE_ONLY
    Void_WMV ComputeMBLoopFilterFlags_WMVA (tWMVDecInternalMember *pWMVDec, 
                    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
            I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
         U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft);

    Void_WMV ComputeMBLoopFilterFlags_WMV9 (tWMVDecInternalMember *pWMVDec, 
                    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
            I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
         U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft);
#endif
#endif

    Void_WMV GenerateColumnLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
            U8_WMV* puchFilterColFlagY, U8_WMV* puchFilterColFlagU, U8_WMV* puchFilterColFlagV,
            U8_WMV* puchFilterColFlagY2, U8_WMV* puchFilterColFlagU2, U8_WMV* puchFilterColFlagV2,
            LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBHeight);

    Void_WMV GenerateRowLoopFilterFlags (tWMVDecInternalMember *pWMVDec, 
            U8_WMV* puchFilterRowFlagY, U8_WMV* puchFilterRowFlagU, U8_WMV* puchFilterRowFlagV,
            U8_WMV* puchFilterRowFlagY2, U8_WMV* puchFilterRowFlagU2, U8_WMV* puchFilterRowFlagV2,
            LOOPF_FLAG* pLoopFilterFlag, I32_WMV iMBYStart, I32_WMV iMBYEnd, Bool_WMV bFirstRow);


    Void_WMV ClearFieldMBInterlace411 (I16_WMV *ppxliFieldMB, I32_WMV iField);

    Void_WMV CopyTopFieldToIntraFieldMBInterlace411 (
        I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, 
        U8_WMV *ppxliFrameMBU, U8_WMV *ppxliFrameMBV);

    Void_WMV CopyBotFieldToIntraFieldMBInterlace411 (
        I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, 
        U8_WMV *ppxliFrameMBU, U8_WMV *ppxliFrameMBV);

    Void_WMV CopyIntraFieldMBtoTopFieldInterlace411 (
        I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
        U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

    Void_WMV CopyIntraFieldMBtoBotFieldInterlace411 (
        I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
        U8_WMV *ppxliFrameMBV, I32_WMV iOffsetY, I32_WMV iOffsetUV);

        // older ones....
    Void_WMV CopyFrameMBtoIntraFieldMBInterlace411 (
        I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, 
        U8_WMV *ppxliFrameMBU, U8_WMV *ppxliFrameMBV);


    Void_WMV CopyIntraFieldMBtoFrameMBInterlace411 (
        I16_WMV *ppxliFieldMB, U8_WMV *ppxliFrameMBY, U8_WMV *ppxliFrameMBU, 
        U8_WMV *ppxliFrameMBV, I32_WMV iOffsetToNextRowY, I32_WMV iOffsetToNextRowUV, 
        Bool_WMV bSeparateField);


    Void_WMV CopyFrameMBtoInterFieldMBInterlace411 (I16_WMV *ppxliFieldMB, 
        I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, I16_WMV *ppxliFrameMBV);

    Void_WMV CopyTopFieldMBtoInterFieldMBInterlace411 (
        I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, 
        I16_WMV *ppxliFrameMBV);

    Void_WMV CopyBotFieldMBtoInterFieldMBInterlace411 (
        I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, 
        I16_WMV *ppxliFrameMBV);

    Void_WMV ChromaMVInterlace411 (    tWMVDecInternalMember *pWMVDec,
        I32_WMV imbX, I32_WMV imbY, I32_WMV iMvX, I32_WMV iMvY, I32_WMV *iChromaMvX, I32_WMV *iChromaMvY);

    Void_WMV LoopFilterIFrameInterlace411 (tWMVDecInternalMember *pWMVDec,
        U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV);


    Void_WMV loopfilter8x8rows0 ();
    Void_WMV loopfilter8x8rows1 ();
    Void_WMV loopfilter8x4rows0 ();
    Void_WMV loopfilter8x4rows1 ();
    Void_WMV loopfilter8x8cols0 ();
    Void_WMV loopfilter8x8cols1 ();
    Void_WMV loopfilter4x8cols0 ();
    Void_WMV loopfilter4x8cols1 ();

        // set WMV3 profiles
    Void_WMV SetProfileVariablesWMV3 ();

    Void_WMV CopyInterFieldMBtoTopFieldMBInterlace411 (
        I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY,I16_WMV *ppxliFrameMBU, 
        I16_WMV *ppxliFrameMBV, I32_WMV iOffsetToNextRowY, I32_WMV iOffsetToNextRowUV);

    Void_WMV CopyInterFieldMBtoBotFieldMBInterlace411 (
        I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY,I16_WMV *ppxliFrameMBU, 
        I16_WMV *ppxliFrameMBV, I32_WMV iOffsetToNextRowY, I32_WMV iOffsetToNextRowUV);

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

    
    tWMVDecodeStatus DecodeInverseInterBlockQuantize16 (tWMVDecInternalMember *pWMVDec,  CDCTTableInfo_Dec** InterDCTTableInfo_Dec, U8_WMV* pZigzag, I32_WMV iXformMode, DQuantDecParam *pDQ);
    Void_WMV LoopFilterPFrameDecInterlace411 (tWMVDecInternalMember *pWMVDec,
        U8_WMV* ppxliY, U8_WMV* ppxliU, U8_WMV* ppxliV, CWMVMBMode *pmbmd);

tWMVDecodeStatus InitThreads (tWMVDecInternalMember * pWMVDec);

Void_WMV  dump_minuspadding (tWMVDecInternalMember *pWMVDec,  I32_WMV iDumRef) ;

Void_WMV CloseThreads (tWMVDecInternalMember * pWMVDec);


Void_WMV UpsampleToDump (tWMVDecInternalMember *pWMVDec);


    Void_WMV SetMVRangeFlag (tWMVDecInternalMember *pWMVDec, I32_WMV iRangeIndex);

    Bool_WMV CheckSliceStartCode(tWMVDecInternalMember *pWMVDec, I32_WMV imbY);



        I32_WMV ComputePredCBPCY (tWMVDecInternalMember *pWMVDec,
        CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, I32_WMV iCBPCY); 

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
    Void_WMV m_InitLoopFilterFramebyFrame();

    
    Void_WMV  OverlapMBRow (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, 
        U8_WMV *ppxliRecnU, U8_WMV *ppxliRecnV, Bool_WMV bTopRowOfSlice, Bool_WMV bBottomRowOfSlice);

#ifndef _WMV9AP_
    Void_WMV  OverlapMBRowIFrame (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, U8_WMV *ppxliRecnU, U8_WMV *ppxliRecnV, I32_WMV iMBYEnd, I32_WMV iMBYStart,
                                   I32_WMV iThreadID);
#endif
    Void_WMV  Cast8x4Block (I16_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride);
    Void_WMV  OverlapBlockHorizontalEdge (tWMVDecInternalMember * pWMVDec, I16_WMV *pSrcTop, I16_WMV *pSrcCurr, I32_WMV iSrcStride,
                        U8_WMV *pDst, I32_WMV iDstStride, Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
    Void_WMV  OverlapBlockVerticalEdge ( I16_WMV *pInOut, I32_WMV iStride);

#ifndef WMV9_SIMPLE_ONLY
     Void_WMV AdjustReconRange (tWMVDecInternalMember *pWMVDec);
#endif

     Void_WMV SwapPostAndPostPrev(tWMVDecInternalMember *pWMVDec);

    
    Void_WMV PixelMean (const U8_WMV *pSrc1, I32_WMV iStride1, const U8_WMV *pSrc2, I32_WMV iStride2,
               U8_WMV *pDst, I32_WMV iDstStride, I32_WMV iWidth, I32_WMV iHeight);
    Void_WMV DirectModeMV (tWMVDecInternalMember * pWMVDec, I32_WMV iXMotion, I32_WMV iYMotion, Bool_WMV bHpelMode,
                                   I32_WMV imbX, I32_WMV imbY,
                                   I32_WMV *idfx, I32_WMV *idfy, I32_WMV *idbx, I32_WMV *idby);
 

tWMVDecodeStatus decodeMBOverheadOfBVOP_WMV3 (tWMVDecInternalMember *pWMVDec,CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY);
tWMVDecodeStatus DecodeBMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY,
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    CoordI xIndex, CoordI yIndex,
    CoordI xMv, CoordI yMv,
    CoordI xMv2, CoordI yMv2
);

Bool_WMV PullBackMotionVector (tWMVDecInternalMember *pWMVDec,
    I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY);
#ifndef WMV9_SIMPLE_ONLY
Void_WMV swapRef0Ref1Pointers (tWMVDecInternalMember *pWMVDec);
Void_WMV swapCurrRef1Pointers (tWMVDecInternalMember *pWMVDec);
#endif

tWMVDecodeStatus decodeB_Deblock (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus SliceStartCode(tWMVDecInternalMember *pWMVDec, I32_WMV imbY);

Void_WMV SetTransformTypeHuffmanTable (tWMVDecInternalMember *pWMVDec, I32_WMV iStep);

tWMVDecodeStatus decodeSkipP (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus decodeFrameBMBOverheadInterlace411 (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);

#ifndef WMV9_SIMPLE_ONLY
Void_WMV SetMotionFieldBFrame (I16_WMV *pMvX, I16_WMV *pMvY,
                           I16_WMV *pFMvX, I16_WMV *pFMvY,
                           I16_WMV *pBMvX, I16_WMV *pBMvY,
                           I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX, I32_WMV iBotMvY,
                           I32_WMV iFTopMvX, I32_WMV iFTopMvY, I32_WMV iFBotMvX, I32_WMV iFBotMvY,
                           I32_WMV iBTopMvX, I32_WMV iBTopMvY, I32_WMV iBBotMvX, I32_WMV iBBotMvY,
                           I32_WMV iTopLeftBlkIndex, I32_WMV iBotLeftBlkIndex
                           );
#endif

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

Void_WMV MotionCompFrameInterlace411_BFrame (tWMVDecInternalMember *pWMVDec,
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

#ifndef WMV9_SIMPLE_ONLY
Void_WMV CopyRefToPrevPost (tWMVDecInternalMember * pWMVDec);

Void_WMV HandleResolutionChange (tWMVDecInternalMember * pWMVDec);
#endif

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
        CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY);

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

        

    Void_WMV inverseIntraBlockQuantizeInterlaceV2 (tWMVDecInternalMember *pWMVDec,
        I16_WMV *piCoefRecon, I32_WMV iNumCoef, DQuantDecParam *pDQ);

    I32_WMV DecodeInverseIntraBlockQuantizeInterlaceV2 (tWMVDecInternalMember *pWMVDec,
                CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, U8_WMV* piZigzagInv, 
                        I16_WMV *rgiCoefRecon, U32_WMV uiNumCoef);

    I32_WMV decodeIntraDCInterlaceV2 (tWMVDecInternalMember *pWMVDec,
        I16_WMV *piDC, Huffman_WMV* hufDCTDCDec, I32_WMV iTCOEF_ESCAPE, I32_WMV iDCStepSize);

    Void_WMV ScaleDCPredForDQuantInterlaceV2 (tWMVDecInternalMember *pWMVDec,
        I32_WMV iBlk, CWMVMBMode* pmbmd, I32_WMV *piTopDC, I32_WMV *piLeftDC, I32_WMV *piTopLeftDC);

    Void_WMV ScaleLeftPredForDQuantInterlaceV2 (tWMVDecInternalMember *pWMVDec,
        I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd);

    Void_WMV ScaleTopPredForDQuantInterlaceV2 (tWMVDecInternalMember *pWMVDec,
        I32_WMV iBlk, I16_WMV *pPred, I16_WMV *pPredScaled, CWMVMBMode* pmbmd);

    Void_WMV SetIBlockBoundaryForDCPred (tWMVDecInternalMember *pWMVDec,
        I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode* pmbmd, Bool_WMV *pbIsTopBlockI,
        Bool_WMV *pbIsLeftBlockI, Bool_WMV *pbIsTopLeftBlockI);

    Void_WMV decodeDCACPredInterlaceV2 (tWMVDecInternalMember *pWMVDec,
       I32_WMV imbY, I32_WMV imbX, I32_WMV iBlk, CWMVMBMode* pmbmd,
       DCACPREDDIR *iPredDir, I16_WMV *rgiPredCoef);

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
#ifdef _WMV9AP_
    	Void_WMV MotionCompSingleFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec,
		I32_WMV imbX, I32_WMV imbY, U8_WMV *ppxliRefY, U8_WMV *ppxliRefU, U8_WMV *ppxliRefV,
		U8_WMV* ppxliReconY, U8_WMV* ppxliReconU, U8_WMV* ppxliReconV,
		I32_WMV iReconStrideY, I32_WMV iReconStrideUV, I32_WMV iTopMvX, I32_WMV iTopMvY, I32_WMV iBotMvX,
		I32_WMV iBotMvY, I32_WMV iBot);

    Void_WMV ComputeFrameMVFromDiffMVInterlaceV2B (tWMVDecInternalMember *pWMVDec,
        CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);
    Void_WMV ComputeFieldMVFromDiffMVInterlaceV2B (tWMVDecInternalMember *pWMVDec,
        CWMVMBMode* pmbmd, I32_WMV imbX, I32_WMV imbY, CDiffMV *pInDiffMV);
#ifndef WMV9_SIMPLE_ONLY
    tWMVDecodeStatus decodeBInterlaceV2 (tWMVDecInternalMember *pWMVDec);
#endif
    I32_WMV DecodeFieldModeBFrame (tWMVDecInternalMember *pWMVDec);
    I32_WMV DecodeBMBInterlaceV2 (tWMVDecInternalMember *pWMVDec,I32_WMV imbX, I32_WMV imbY, CWMVMBMode *pmbmd);
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
#endif

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

    I32_WMV decodePFieldMode (tWMVDecInternalMember *pWMVDec);

    Void_WMV MotionCompFieldMode1MV (tWMVDecInternalMember *pWMVDec,
        U8_WMV *pDstY, U8_WMV *pDstU, U8_WMV *pDstV,
        U8_WMV *pRefY, U8_WMV *pRefU, U8_WMV *pRefV,
        I32_WMV imbX, I32_WMV imbY, I16_WMV *MVx, I16_WMV* Mvy);

    I32_WMV ChromaMVFieldMode (tWMVDecInternalMember *pWMVDec,
        I16_WMV* piMVx, I16_WMV* piMVy, 
        I32_WMV imbX, I32_WMV imbY, Bool_WMV b1MV, 
        I32_WMV *piMVx2, I32_WMV *piMVy2);

    I32_WMV DecodeMBFieldMode1MV (tWMVDecInternalMember *pWMVDec,
    CWMVMBMode*            pmbmd, 
        U8_WMV __huge* ppxlcCurrQYMB, U8_WMV __huge* ppxlcCurrQUMB, U8_WMV __huge* ppxlcCurrQVMB,
        U8_WMV __huge* ppxlcRefYMB, U8_WMV __huge* ppxlcRefUMB, U8_WMV __huge* ppxlcRefVMB,
        I32_WMV imbX, I32_WMV imbY, I16_WMV* MVx, I16_WMV* MVy);

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

    Void_WMV CopyFrameMBtoInterFieldMBInterlaceV2 (
        I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, 
        I16_WMV *ppxliFrameMBV);

    Void_WMV CopyFieldMBtoInterFieldMBInterlaceV2 (
        I16_WMV *ppxliFieldMB, I16_WMV *ppxliFrameMBY, I16_WMV *ppxliFrameMBU, 
        I16_WMV *ppxliFrameMBV);

    I32_WMV PredictFieldModeMV (tWMVDecInternalMember *pWMVDec,
        I32_WMV* iPredX, I32_WMV* iPredY, I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry);

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
    Bool_WMV PullBackFieldModeMotionVector (tWMVDecInternalMember *pWMVDec,I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY);

    Void_WMV SetFieldModeResolution (tWMVDecInternalMember *pWMVDec,Bool_WMV bHalfVert);

#ifndef WMV9_SIMPLE_ONLY
	Void_WMV IntensityCompensationFieldMode (tWMVDecInternalMember *pWMVDec, I32_WMV iField, I32_WMV iLuminanceScale, I32_WMV iLuminanceShift);
#endif

    Void_WMV MotionCompBlockFieldMode (tWMVDecInternalMember *pWMVDec,
    U8_WMV *pDst, UnionBuffer *pError, U8_WMV *pSrc,
    I32_WMV iBlockX, I32_WMV iBlockY, I32_WMV iMVx, I32_WMV iMVy);

    Void_WMV MotionCompChromaBlockFieldMode (tWMVDecInternalMember *pWMVDec,
    U8_WMV *pDst, UnionBuffer *pError, U8_WMV *pSrc,
    I32_WMV iMVx, I32_WMV iMVy);

 I32_WMV medianof4 (I32_WMV a0, I32_WMV a1, I32_WMV a2, I32_WMV a3);

#ifndef WMV9_SIMPLE_ONLY
    Void_WMV DecodeFrac(tWMVDecInternalMember *pWMVDec, const I32_WMV iVal, const Bool_WMV bLong);
#endif

    I32_WMV DecodeFieldModeFrame (tWMVDecInternalMember *pWMVDec);

    Void_WMV  setCodedBlockPattern (CWMVMBMode* pmbmd, BlockNum blkn, Bool_WMV bisCoded) ;

        I32_WMV decodeSequenceHead_WMVB (tWMVDecInternalMember *pWMVDec, U32_WMV uiFormatLength);

    Void_WMV GetPerSampleInterlaceMode (tWMVDecInternalMember *pWMVDec, VideoInterlaceMetaData * peInterlaceMode, Bool_WMV *pbLowerField);


        Void_WMV DeblockFieldInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
        U8_WMV __huge* ppxliY,
        U8_WMV __huge* ppxliU,
        U8_WMV __huge* ppxliV,
        U32_WMV iMBStartY, U32_WMV iMBEndY
        );

#ifndef WMV9_SIMPLE_ONLY
		I32_WMV decodeMBOverheadOfBVOP_FieldPicture (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY);
#endif

#ifdef _WMV9AP_

        I32_WMV DecodePMBModeInterlaceV2 (tWMVDecInternalMember *pWMVDec, 
        CWMVMBMode* pmbmd, Bool_WMV *pbCBPPresent, Bool_WMV *pb1MVPresent);


    I32_WMV InitTwoLayerEntropyDecoding(tWMVDecInternalMember *pWMVDec);
	Void_WMV FreeTwoLayerEntropyDecoding(tWMVDecInternalMember *pWMVDec);

	// Spatial prediction
    Void_WMV	GetNbrBlkIdx_8x8(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	Void_WMV	GetNbrBlkIdx_8x4(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	Void_WMV	GetNbrBlkIdx_4x8(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	Void_WMV	GetNbrBlkIdx_4x4(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
    I32_WMV     GetPredictedRunNoFromTwoNbrs(tWMVDecInternalMember *pWMVDec, I32_WMV iMBMode);
    I32_WMV     GetPredictedRunNoFromTwoNbrs_8x8(tWMVDecInternalMember *pWMVDec, I32_WMV iMBMode);
    I32_WMV     GetPredictedRunNoFromTwoNbrs_8x4(tWMVDecInternalMember *pWMVDec );
    I32_WMV     GetPredictedRunNoFromTwoNbrs_4x8(tWMVDecInternalMember *pWMVDec );
    I32_WMV     GetPredictedRunNoFromTwoNbrs_4x4(tWMVDecInternalMember *pWMVDec );
    I32_WMV     GetPredictedTotalZerosFromTwoNbrs(tWMVDecInternalMember *pWMVDec, I32_WMV iMBMode);
    I32_WMV     GetPredictedTotalZerosFromTwoNbrs_8x8(tWMVDecInternalMember *pWMVDec, I32_WMV iMBMode);
    I32_WMV     GetPredictedTotalZerosFromTwoNbrs_8x4(tWMVDecInternalMember *pWMVDec );
    I32_WMV     GetPredictedTotalZerosFromTwoNbrs_4x8(tWMVDecInternalMember *pWMVDec );
    I32_WMV     GetPredictedTotalZerosFromTwoNbrs_4x4(tWMVDecInternalMember *pWMVDec );
	I32_WMV		GetNbrBlkRunNo_8x8(tWMVDecInternalMember *pWMVDec, I32_WMV iNbrBlkPos);
	I32_WMV		GetNbrBlkRunNo_8x4(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	I32_WMV		GetNbrBlkRunNo_4x8(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	I32_WMV		GetNbrBlkRunNo_4x4(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);

	I32_WMV		GetNbrBlkTotalZeros_8x8(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	I32_WMV		GetNbrBlkTotalZeros_8x4(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	I32_WMV		GetNbrBlkTotalZeros_4x8(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);
	I32_WMV		GetNbrBlkTotalZeros_4x4(tWMVDecInternalMember *pWMVDec, I32_WMV  iNbrBlkPos);



	Void_WMV	SaveRunNoAndNumOfZeros(tWMVDecInternalMember *pWMVDec, I32_WMV iRunNo, I32_WMV iNumofZeros, I32_WMV iBlkMode);

	// Inter
	I32_WMV DecodeInterBlock_TwoLayer(tWMVDecInternalMember *pWMVDec);

    

	I32_WMV DecodeBlkInfo_TwoLayer(tWMVDecInternalMember *pWMVDec, 
		I32_WMV iXformMode,	
		I32_WMV * piRunNo, I32_WMV * piNumofZeros);

	I32_WMV DecodeTotalZeros(tWMVDecInternalMember *pWMVDec, 
		I32_WMV iNbrBlkTotalZeros, I32_WMV iRunNo, I32_WMV iXformMode);

	I32_WMV DecodeLevels_TwoLayer(tWMVDecInternalMember *pWMVDec, 
		I32_WMV iRunNo, 
		I32_WMV iNumofZeros);

	I32_WMV DecodeRuns_TwoLayer(tWMVDecInternalMember *pWMVDec, 
		I32_WMV iRunNo, 
		I32_WMV iNumofZeros);

        I32_WMV DecodeNonOneLevel(tWMVDecInternalMember *pWMVDec, I32_WMV iLevelZone);

    I32_WMV DecodeRunOfOnes(tWMVDecInternalMember *pWMVDec,
        I32_WMV iContext,
        I32_WMV iNumOnesLeft,
        I32_WMV *piZone_RUNISL);


	// Intra
	I32_WMV DecodeIntraBlock_TwoLayer(tWMVDecInternalMember *pWMVDec);

    	

	I32_WMV DecodeBlkInfo_TwoLayer_Intra(tWMVDecInternalMember *pWMVDec, 
		I32_WMV iXformMode,	
		I32_WMV * piRunNo, I32_WMV * piNumofZeros);

	I32_WMV DecodeLevels_TwoLayer_Intra(tWMVDecInternalMember *pWMVDec, 
		
		I32_WMV iRunNo, 
		I32_WMV iNumofZeros);

	I32_WMV DecodeTotalZeros_Intra(tWMVDecInternalMember *pWMVDec, 
		I32_WMV iRunNo, 
		I32_WMV iXformMode);

	I32_WMV DecodeRuns_TwoLayer_Intra(tWMVDecInternalMember *pWMVDec, 
		I32_WMV iRunNo, 
		I32_WMV iNumofZeros);

    I32_WMV DecodeNonOneLevel_Intra(tWMVDecInternalMember *pWMVDec, I32_WMV iLevelZone);

    I32_WMV DecodeRunOfOnes_Intra(tWMVDecInternalMember *pWMVDec,
        I32_WMV iNumOnesLeft,
        I32_WMV *piZone_RUNISL);


	I32_WMV	InverseQuant16_TwoLayer(tWMVDecInternalMember *pWMVDec, 
		U8_WMV *pZigzagInv,
		DQuantDecParam *pDQ,
		I16_WMV *rgiCoefRecon);

	I32_WMV IntraACZigzagInv_TwoLayer(tWMVDecInternalMember *pWMVDec, 
		I16_WMV *rgiCoefRecon,
		U8_WMV *pZigzagInv,
		I32_WMV* iNonZeroIdx,
		I32_WMV * piNonZeroCnt);

	I32_WMV IVOPACZigzagInv_TwoLayer(tWMVDecInternalMember *pWMVDec, 
		I32_WMV *rgiCoefRecon, 
		I16_WMV *pDct, 
		U8_WMV *piZigzagInv);

	I32_WMV InverseQuant16_InterlaceV2_TwoLayer(tWMVDecInternalMember *pWMVDec, 
		U8_WMV *pZigzagInv,
		DQuantDecParam *pDQ,
		I16_WMV *rgiCoefRecon);

        I32_WMV DecodeInverseIntraBlockQuantizeInterlaceV2_TwoLayer (tWMVDecInternalMember *pWMVDec, 
        U8_WMV* piZigzagInv, 
        I16_WMV *rgiCoefRecon, U32_WMV uiNumCoef);

    I32_WMV GetNumZeroContext(tWMVDecInternalMember *pWMVDec, 
        I32_WMV     iMBMode);





#endif

#ifdef _WMV9AP_
	I32_WMV decodeBFieldMode (tWMVDecInternalMember *pWMVDec);
#endif


        I32_WMV decodeVOPHeadExtension (tWMVDecInternalMember *pWMVDec, I32_WMV* iWMVBFrameType);

	I32_WMV decodeVOPHeadFieldPicture (tWMVDecInternalMember *pWMVDec);

    I32_WMV decodeMBOverheadOfPVOP_FieldPicture (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY, Bool_WMV bHalfPelMode);

    I32_WMV DecodeMBModeFieldPicture (tWMVDecInternalMember *pWMVDec, CWMVMBMode* pmbmd, Bool_WMV *pbMV, Bool_WMV * pbCBPPresent);

    Void_WMV decodeMV_Interlace(tWMVDecInternalMember *pWMVDec, Huffman_WMV* pHufMVTable, CDiffMV *pDiffMV);

        I32_WMV AverageMVFieldMode (tWMVDecInternalMember *pWMVDec,
        I16_WMV* piMVx, I16_WMV* piMVy, 
        I32_WMV imbX, I32_WMV imbY, Bool_WMV b1MV, 
        I32_WMV *piMVx2, I32_WMV *piMVy2);
	Void_WMV PredictMVFieldBPicture (tWMVDecInternalMember *pWMVDec,I32_WMV iX, I32_WMV iY, Bool_WMV b1MV, Bool_WMV bMVPresent, 
                                I32_WMV *iPredX, I32_WMV *iPredY, I16_WMV* pXMotion, I16_WMV* pYMotion, I32_WMV iMVPolarity, MBType eDirection, Bool_WMV bHalfPelMode);

    	I32_WMV DecodeBMBFieldMode1MV (tWMVDecInternalMember *pWMVDec,
                                   CWMVMBMode*            pmbmd, 
		U8_WMV __huge* ppxlcCurrQYMB, U8_WMV __huge* ppxlcCurrQUMB, U8_WMV __huge* ppxlcCurrQVMB,
		U8_WMV __huge* ppxlcRefYMB, U8_WMV __huge* ppxlcRefUMB, U8_WMV __huge* ppxlcRefVMB,
		U8_WMV __huge* ppxlcRef1YMB, U8_WMV __huge* ppxlcRef1UMB, U8_WMV __huge* ppxlcRef1VMB,
		I32_WMV imbX, I32_WMV imbY, I16_WMV* MVx, I16_WMV* MVy, I16_WMV* MVxB, I16_WMV* MVyB);
	Void_WMV MotionCompFieldMode1MVB (tWMVDecInternalMember *pWMVDec,
		U8_WMV *pDstY, U8_WMV *pDstU, U8_WMV *pDstV,
		U8_WMV *pRefY, U8_WMV *pRefU, U8_WMV *pRefV,
		U8_WMV *pRef1Y, U8_WMV *pRef1U, U8_WMV *pRef1V,
		I32_WMV imbX, I32_WMV imbY, I16_WMV *MVx, I16_WMV* MVy, I16_WMV *MVxB, I16_WMV *MVyB);

    Void_WMV PredictHybridMVFieldPicture (tWMVDecInternalMember *pWMVDec,I32_WMV iX, I32_WMV iY, Bool_WMV b1MV, Bool_WMV bHalfPelMode, I32_WMV *iPredX, I32_WMV *iPredY, I32_WMV iMVPolarity);

    Void_WMV PredictFieldModeMVEx (tWMVDecInternalMember *pWMVDec,
        CMVPred *pMVPred,
        I32_WMV iBlkX, I32_WMV iBlkY,
        I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry, Bool_WMV bHalfPelMode);

    Void_WMV ComputeMVFromDiffMVFieldPicture (tWMVDecInternalMember *pWMVDec,I32_WMV  iBlockX, I32_WMV iBlockY, I32_WMV iPredMVX, I32_WMV iPredMVY, CDiffMV *pInDiffMV);



I32_WMV DecodeFieldMode4MV (tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    U8_WMV __huge* ppxlcSameRefY,
    U8_WMV __huge* ppxlcSameRefU,
    U8_WMV __huge* ppxlcSameRefV,
    U8_WMV __huge* ppxlcOppRefY,
    U8_WMV __huge* ppxlcOppRefU,
    U8_WMV __huge* ppxlcOppRefV,
                                   I32_WMV iMBX, I32_WMV iMBY
                                   );

#ifndef WMV9_SIMPLE_ONLY
Void_WMV IntensityCompensationFieldMode (tWMVDecInternalMember * pWMVDec, I32_WMV iField, I32_WMV iLuminanceScale, I32_WMV iLuminanceShift);
#endif



    I32_WMV  PredictMVB (tWMVDecInternalMember * pWMVDec, I32_WMV iBlockX, I32_WMV iBlockY, I32_WMV iMVMode,
                    I32_WMV iNumBlockX, I32_WMV iNumBlockY,
                    const I16_WMV *pMVX, const I16_WMV *pMVY,
                    I32_WMV *iXMotion, I32_WMV *iYMotion, Bool_WMV bTopBndry);

    Bool_WMV GetNbrBlkMode(tWMVDecInternalMember * pWMVDec,
        const CWMVMBMode* pmbmd,
        I32_WMV iNbrMBIdx,
        I32_WMV iNbrBlkIdx
        );
    Void_WMV    GetNbrBlkIdx(tWMVDecInternalMember * pWMVDec, I32_WMV iBlkMode, I32_WMV  iNbrBlkPos, I32_WMV * piNbrMBIdx, I32_WMV * piNbrBlkIdx);


    Void_WMV    GetTopNbrBlkIdx(tWMVDecInternalMember * pWMVDec);
    Void_WMV    GetLeftNbrBlkIdx(tWMVDecInternalMember * pWMVDec);

        Void_WMV decodeMV_Progressive(tWMVDecInternalMember * pWMVDec, Huffman_WMV* pHufMVTable, CDiffMV *pDiffMV);
    Bool_WMV PullBackFrameMBMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY);
    Bool_WMV PullBackFrameBlockMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk);
    Bool_WMV PullBackFieldMBMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iField);
    Bool_WMV PullBackFieldBlockMotionVector (tWMVDecInternalMember * pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY, I32_WMV imbX, I32_WMV imbY, I32_WMV iBlk);


I32_WMV decodeSequenceHead_Advanced (tWMVDecInternalMember *pWMVDec,  U32_WMV uiFormatLength, 
                                     I32_WMV *piPIC_HORIZ_SIZE , I32_WMV *piPIC_VERT_SIZE ,
                                     I32_WMV *piDISP_HORIZ_SIZE , I32_WMV *piDISP_VERT_SIZE );

I32_WMV decodeExtendedSequenceHead_Advanced (tWMVDecInternalMember *pWMVDec, I32_WMV *piPIC_HORIZ_SIZE , I32_WMV *piPIC_VERT_SIZE );

    I32_WMV decodeVOPType (tWMVDecInternalMember * pWMVDec);

    Void_WMV decideMMXRoutines_ZigzagOnly (tWMVDecInternalMember * pWMVDec);

        I32_WMV decodeVOPHeadProgressiveWMVA (tWMVDecInternalMember * pWMVDec);
    Void_WMV DecodeVOPType_WMVA(tWMVDecInternalMember *pWMVDec);
    Void_WMV DecodeVOPType_WMVA2(tWMVDecInternalMember *pWMVDec);

        I32_WMV decodeFieldHeadFieldPicture (tWMVDecInternalMember * pWMVDec);
    Void_WMV DecodeVOPTypeFieldPicture (tWMVDecInternalMember * pWMVDec);


    Void_WMV PredictFieldModeForwardBMVEx (tWMVDecInternalMember * pWMVDec,
        CMVPred *pMVPred,
        I32_WMV iBlkX, I32_WMV iBlkY,
        I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry, Bool_WMV bHalfPelMode);

    Void_WMV PredictFieldModeBackwardBMVEx (tWMVDecInternalMember * pWMVDec,
        CMVPred *pMVPred,
        I32_WMV iBlkX, I32_WMV iBlkY,
        I16_WMV* pFieldMvX, I16_WMV* pFieldMvY, Bool_WMV b1MV, Bool_WMV bTopBndry, Bool_WMV bHalfPelMode);

    Void_WMV SetupFieldPictureMVScaling (tWMVDecInternalMember * pWMVDec, I32_WMV iFrameDistance);
    Void_WMV SetupForwardBFieldPictureMVScaling (tWMVDecInternalMember * pWMVDec, I32_WMV iFrameDistance);
    Void_WMV SetupBackwardBFieldPictureMVScaling (tWMVDecInternalMember * pWMVDec, I32_WMV iFrameDistance);
    Void_WMV ScaleFarPred(tWMVDecInternalMember * pWMVDec,I32_WMV iXO, I32_WMV iYO, I32_WMV* piXS, I32_WMV * piYS, Bool_WMV bScaleForSameField);
    Void_WMV ScaleNearPred(tWMVDecInternalMember * pWMVDec,I32_WMV iXS, I32_WMV iYS, I32_WMV *piXO, I32_WMV* piYO, Bool_WMV bScaleForOppField);
    Void_WMV ScaleFarPredForward(tWMVDecInternalMember * pWMVDec,I32_WMV iXO, I32_WMV iYO, I32_WMV* piXS, I32_WMV* piYS, Bool_WMV bScaleForSameField);
    Void_WMV ScaleNearPredForward(tWMVDecInternalMember * pWMVDec, I32_WMV iXS, I32_WMV iYS, I32_WMV* piXO, I32_WMV* piYO, Bool_WMV bScaleForOppField);
    Void_WMV ScaleFarPredBackward(tWMVDecInternalMember * pWMVDec,I32_WMV iXO, I32_WMV iYO, I32_WMV* piXS, I32_WMV* piYS, Bool_WMV bScaleForSameField);
    Void_WMV ScaleNearPredBackward(tWMVDecInternalMember * pWMVDec, I32_WMV iXS, I32_WMV iYS, I32_WMV* piXO, I32_WMV* piYO, Bool_WMV bScaleForOppField);



    Void_WMV Repeatpad (tWMVDecInternalMember *pWMVDec);

    I32_WMV SwitchSequence (tWMVDecInternalMember *pWMVDec, Bool_WMV bDecodeVOLHead);
//    Void_WMV FreeSeqIndependentMemory(tWMVDecInternalMember *pWMVDec);//sw:20080108
    Void_WMV FreeFrameAreaDependentMemory (tWMVDecInternalMember *pWMVDec);
    //Void_WMV FreeFrameWidthAndHeightDependentMemory(tWMVDecInternalMember *pWMVDec);
    Void_WMV GetSeqFrameSize(tWMVDecInternalMember *pWMVDec, I32_WMV *piNewSeqFrmWidth, I32_WMV *piNewSeqFrmHeight);

    tWMVDecodeStatus initSeqIndependentMemory(tWMVDecInternalMember *pWMVDec);
    //I32_WMV InitDataMembers(tWMVDecInternalMember *pWMVDec, I32_WMV iWidthSource, I32_WMV iHeightSource);
    I32_WMV initFrameWidthAndHeightDependentVariables(tWMVDecInternalMember *pWMVDec, I32_WMV iWidthSource, I32_WMV iHeightSource);
    //I32_WMV initFrameWidthAndHeightDependentMemory(tWMVDecInternalMember *pWMVDec);
    Void_WMV initFrameWidthAndHeightDependentPointers(tWMVDecInternalMember *pWMVDec);
    I32_WMV initFrameAreaDependentMemory(tWMVDecInternalMember *pWMVDec,
                I32_WMV iMaxEncWidth,
        I32_WMV iMaxEncHeight);
    I32_WMV AllocatePictureCYUV420 (tWMVDecInternalMember *pWMVDec,
        I32_WMV iMaxEncWidth, 
        I32_WMV iMaxEncHeight);
    Void_WMV AdjustPictureCYUV420 (tWMVDecInternalMember *pWMVDec);    

    I32_WMV initVlcTables(tWMVDecInternalMember *pWMVDec);

    I32_WMV InitTwoLayerEntropyDecodingHuffTables(tWMVDecInternalMember *pWMVDec);
//#ifdef _NEW_SEQUENCE_ENTRY_
    I32_WMV DecodeEntryPointHeader (tWMVDecInternalMember *pWMVDec, I32_WMV *piPIC_HORIZ_SIZE, I32_WMV *piPIC_VERT_SIZE);
    I32_WMV SwitchEntryPoint (tWMVDecInternalMember *pWMVDec);
//#else if // _NEW_SEQUENCE_ENTRY_
//    I32_WMV DecodeEntryPointHeader (tWMVDecInternalMember *pWMVDec);
//#endif // _NEW_SEQUENCE_ENTRY_

    I32_WMV ReallocateStartCodeBuffer(tWMVDecInternalMember *pWMVDec, U32_WMV nOldBufferSize, U32_WMV nNewBufferSize);
    I32_WMV ParseStartCode ( tWMVDecInternalMember *pWMVDec, U8_WMV *  pBuffer, U32_WMV    dwBufferLen, U8_WMV *  *ppOutBuffer, U32_WMV    *pdwOutBufferLen, Bool_WMV *pbNotEndOfFrame);

    Void_WMV SetSliceWMVA ( tWMVDecInternalMember *pWMVDec, Bool_WMV bSliceWMVA, U32_WMV uiFirstMBRow, U32_WMV uiNumBytesSlice, Bool_WMV bSecondField);

    I32_WMV ReadSliceHeaderWMVA (tWMVDecInternalMember *pWMVDec,I32_WMV imbY );

    I32_WMV ParseStartCodeChunk ( tWMVDecInternalMember *pWMVDec, U8_WMV *  pBuffer, U32_WMV    dwBufferLen, U8_WMV *  *ppOutBuffer, U32_WMV    *pdwOutBufferLen, Bool_WMV bNotEndOfFrame);
    I32_WMV DecodePanScanInfo (tWMVDecInternalMember *pWMVDec, CPanScanInfo *pPanScanInfo, I32_WMV *piNumWindowsPresent);

	Void_WMV ResetConditionalVariablesForSequenceSwitch (tWMVDecInternalMember *pWMVDec);

	I32_WMV AllocateMultiThreadBufs_Dec_WMVA (tWMVDecInternalMember *pWMVDec, 
    I32_WMV iMaxEncWidth,
    I32_WMV iMaxEncHeight);

    Void_WMV initMultiThreadVars_Dec_WMVA (tWMVDecInternalMember *pWMVDec);

        Void_WMV initMultiThreadVars_Dec_WMVA (tWMVDecInternalMember *pWMVDec);
    I32_WMV AllocateMultiThreadBufs_Dec_WMVA (tWMVDecInternalMember *pWMVDec,
        I32_WMV iMaxEncWidth,
        I32_WMV iMaxEncHeight);


    Void_WMV PreGetOutput (tWMVDecInternalMember *pWMVDec);

//Multires global functions
    Void_WMV g_DownsampleWFilterLine6_Vert(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch);
    Void_WMV g_DownsampleWFilterLine6_Horiz(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size);
    Void_WMV g_UpsampleWFilterLine10_Vert(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch);
    Void_WMV g_UpsampleWFilterLine10_Horiz(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size);

#ifdef _EMB_SSIMD_IDCT_
Void_WMV SignPatch(I16_WMV * rgiCoefRecon, int len);
#if defined(VOARMV6) || defined(VOARMV7)
Void_WMV ARMV6_SignPatch32(I32_WMV * rgiCoefRecon, int len);
#else
Void_WMV SignPatch32(I32_WMV * rgiCoefRecon, int len);
#endif
#endif

#ifdef _EMB_WMV3_
tWMVDecodeStatus DecodeMB_X9_Fast (EMB_PMainLoop * pMainLoop,
    CWMVMBMode*            pmbmd, 
    I32_WMV iMBX, I32_WMV iMBY,
    Bool_WMV b1MV
);

tWMVDecodeStatus WMVideoDecDecodeP_X9_EMB (tWMVDecInternalMember *pWMVDec);


 tWMVDecodeStatus decodeMBOverheadOfPVOP_WMV3_EMB (EMB_PMainLoop * pMainLoop,  //EMB_PMainLoop * pMainLoop, 
        CWMVMBMode* pmbmd, I32_WMV x, I32_WMV y);



I32_WMV PredictHybridMV_EMB (EMB_PMainLoop * pMainLoop,  I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk);
    
I32_WMV ComputeMVFromDiffMV_EMB (EMB_PMainLoop * pMainLoop, I32_WMV iblk);
Void_WMV decodeMV_V9_EMB(EMB_PBMainLoop * pMainLoop, Huffman_WMV * pHufMVTable, CDiffMV_EMB *pDiffMV);


Void_WMV SetupEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec);

#ifdef COMBINE_2MC_TO_1MC
Void_WMV g_MotionComp_X9_EMB (EMB_PMainLoop *pPMainLoop,  I32_WMV iblk, Bool_WMV b1MV, Bool_WMV bForce4MV);
#else
Void_WMV g_MotionComp_X9_EMB (EMB_PMainLoop *pPMainLoop,   I32_WMV iblk, Bool_WMV b1MV);
#endif

Void_WMV MotionCompMB_WMV3_EMB (EMB_PMainLoop *pPMainLoop);




I32_WMV  ChromaMV_EMB (EMB_PMainLoop * pMainLoop, I32_WMV iX, I32_WMV iY, Bool_WMV b1MV);

/*
I32_WMV PredictMV_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iBlockX, I32_WMV iBlockY, I32_WMV iMVMode,
                               I32_WMV iNumBlockX, I32_WMV iNumBlockY,
                               const I16_WMV *pMVX, const I16_WMV *pMVY,
                               I32_WMV *iXMotion, I32_WMV *iYMotion, Bool_WMV bTopBndry);
*/


 I32_WMV medianof4 (I32_WMV a0, I32_WMV a1, I32_WMV a2, I32_WMV a3);

 Void_WMV SetupMVDecTable_EMB(tWMVDecInternalMember *pWMVDec);

 Void_WMV InitBlockInterpolation_EMB (tWMVDecInternalMember * pWMVDec);

Void_WMV g_SubBlkIDCTClear_EMB (UnionBuffer * piDstBuf,  I32_WMV iIdx);
Void_WMV g_SubBlkIDCTClear_EMB_C (UnionBuffer * piDstBuf,  I32_WMV iIdx);

#if !defined(WMV_OPT_DQUANT_ARM) //sw
tWMVDecodeStatus DecodeInverseInterBlockQuantize16_EMB(EMB_PBMainLoop* pMainLoop, 
                        CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, 
                        I32_WMV XFormType,
                        DQuantDecParam *pDQ);
#endif
Bool_WMV decodeDCTPrediction_EMB ( EMB_PMainLoop* pPMainLoop, 
    CWMVMBMode *pmbmd, I16_WMV *pIntra, I32_WMV iblk,
    I32_WMV iX, I32_WMV iY, I32_WMV *piShift, I32_WMV *iDirection, I16_WMV *pPredScaled);
/*
Bool_WMV decodeDCTPredictionUV_EMB(tWMVDecInternalMember *pWMVDec, 
    CWMVMBMode* pmbmd, I16_WMV*     pIntra, 
    I32_WMV imbX,     I32_WMV imbY, 
    I32_WMV     *piShift,
    I32_WMV *iDirection, I16_WMV *pPredScaled);
    */
I32_WMV  IsIntraPredY_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iX, I32_WMV iY);
I32_WMV IsIntraPredUV_EMB (tWMVDecInternalMember *pWMVDec, I32_WMV iMBX, I32_WMV iMBY);

Void_WMV OverlapMBRow_EMB (tWMVDecInternalMember * pWMVDec, I32_WMV imbY, U8_WMV *ppxliRecnY, U8_WMV *ppxliRecnU,
                                   U8_WMV *ppxliRecnV, I32_WMV iMBYEnd, I32_WMV iMBYStart,
                                   I32_WMV iThreadID);
Void_WMV ComputeLoopFilterFlags_WMV9_EMB(tWMVDecInternalMember *pWMVDec);
Void_WMV ComputeLoopFilterFlags_WMVA_EMB(tWMVDecInternalMember *pWMVDec);
Void_WMV ComputeMBLoopFilterFlags_WMV9_EMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft);
Void_WMV ComputeMBLoopFilterFlags_WMVA_EMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV iMBX, I32_WMV iMBY, I8_WMV chMBMode,
    I8_WMV* pBlockXformMode, I8_WMV* pBlockXformModeTop, I8_WMV* pBlockXformModeLeft,
    U8_WMV *pCBPCurr, U8_WMV *pCBPTop, U8_WMV *pCBPLeft, Bool_WMV bTop, Bool_WMV bLeft);

#if defined(_EMB_SSIMD_IDCT_)
Void_WMV EMB_InitZigZagTable_SSIMD(U8_WMV * pZigzag, U8_WMV * pNewZigZag, I32_WMV iNumEntries);
#endif

#if defined(_EMB_SSIMD_B_)
Void_WMV g_AddErrorB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV* pErrorBuf, I32_WMV iPitch, U32_WMV iOffset);
Void_WMV g_AddNullB_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef0 , U32_WMV* pRef1, I32_WMV iOffset, I32_WMV iPitch);
#endif

tWMVDecodeStatus DecodeBMB_EMB_Fast(EMB_BMainLoop  * pBMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY);


//#if defined(_EMB_SSIMD_MC_)
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



Void_WMV g_8x4IDCTDec_WMV3_SSIMD (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf, I32_WMV iDCTHorzFlags);
Void_WMV g_4x8IDCTDec_WMV3_SSIMD (UnionBuffer * piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer * rgiCoefReconBuf, I32_WMV iHalf, I32_WMV iDCTHorzFlags);
Void_WMV g_4x4IDCTDec_WMV3_SSIMD (UnionBuffer __huge* piDstBuf, I32_WMV iOffsetToNextRowForDCT, const UnionBuffer __huge* rgiCoefReconBuf, I32_WMV iQuadrant, I32_WMV iDCTHorzFlags);
   
Void_WMV g_OverlapBlockVerticalEdge(I16_WMV *pInOut, I32_WMV iStride);

#if defined(VOARMV7)

Void_WMV ARMV7_g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
I32_WMV  ARMV7_g_DecodeInterError8x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ);
I32_WMV  ARMV7_g_DecodeInterError8x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV  ARMV7_g_DecodeInterError4x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV  ARMV7_g_DecodeInterError4x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
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
/* The same as VOARMV6. */
Void_WMV g_FilterHorizontalEdgeV9(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV ARMV6_g_OverlapBlockHorizontalEdge(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
Void_WMV ARMV6_IntensityCompensation_asm(U8_WMV  *pSrcY,U8_WMV *pSrcU,U8_WMV *pSrcV, I32_WMV y_num,I32_WMV uv_num, I32_WMV iScale, I32_WMV iShift);

#elif defined(VOARMV6)

Void_WMV ARMV6_g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
I32_WMV  ARMV6_g_DecodeInterError8x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ);
I32_WMV  ARMV6_g_DecodeInterError8x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
Void_WMV ARMV6_g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
Void_WMV ARMV6_g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
Void_WMV ARMV6_g_OverlapBlockHorizontalEdge(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
Void_WMV ARMV6_IntensityCompensation_asm(U8_WMV  *pSrcY,U8_WMV *pSrcU,U8_WMV *pSrcV, I32_WMV y_num,I32_WMV uv_num, I32_WMV iScale, I32_WMV iShift);
/* The same as VOARMV4. */
I32_WMV  g_DecodeInterError4x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV  g_DecodeInterError4x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
Void_WMV g_InterpolateBlockBilinear_SSIMD_01 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_10 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_11 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
I_SIMD	 g_InterpolateBlock_00_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop,  I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV * pTbl); // iXFrac == 0; iYFrac == 0
Void_WMV g_NewVertFilter0LongNoGlblTbl(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
U_SIMD   g_NewHorzFilterX(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV);
U_SIMD   g_NewVertFilterX(const U8_WMV  *pSrc,const I32_WMV iSrcStride,U8_WMV * pDst, const I32_WMV iShift,const I_SIMD iRound,const I8_WMV * const pV,I32_WMV iNumHorzLoop,const U_SIMD uiMask,Bool_WMV b1MV,U16_WMV * pTbl);
Void_WMV g_FilterHorizontalEdgeV9(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterHorizontalEdgeV9Last8x4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9Last4x8 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);

#else   /* VOARMV4 or WIN32 */

Void_WMV g_IDCTDec16_WMV3_SSIMD (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);
I32_WMV  g_DecodeInterError8x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ);
I32_WMV  g_DecodeInterError8x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV  g_DecodeInterError4x8_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV  g_DecodeInterError4x4_SSIMD(EMB_PBMainLoop  * pMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
Void_WMV g_InterpolateBlockBilinear_SSIMD_01 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_10 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD_11 (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
Void_WMV g_InterpolateBlockBilinear_SSIMD (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);
I_SIMD	 g_InterpolateBlock_00_SSIMD (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pMainLoop,  I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV * pTbl); // iXFrac == 0; iYFrac == 0
Void_WMV g_NewVertFilter0LongNoGlblTbl(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
Void_WMV g_AddError_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
Void_WMV g_AddNull_SSIMD(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);
U_SIMD   g_NewHorzFilterX(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV);
U_SIMD   g_NewVertFilterX(const U8_WMV  *pSrc,const I32_WMV iSrcStride,U8_WMV * pDst, const I32_WMV iShift,const I_SIMD iRound,const I8_WMV * const pV,I32_WMV iNumHorzLoop,const U_SIMD uiMask,Bool_WMV b1MV,U16_WMV * pTbl);
#ifdef DEBLOCK_H_NEON_OPT
Void_WMV g_FilterHorizontalEdgeV9_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterHorizontalEdgeV9Last8x4_armv7_C(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterHorizontalEdgeV9(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#else
Void_WMV g_FilterHorizontalEdgeV9(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterHorizontalEdgeV9Last8x4(U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#endif
#ifdef DEBLOCK_V_NEON_OPT
Void_WMV g_FilterVerticalEdgeV9_armv7_C (U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterVerticalEdgeV9Last4x8_armv7_C (U8_WMV* pV5, I32_WMV iPixelDistance, I32_WMV iStepSize, U8_WMV uchBitField);
Void_WMV g_FilterVerticalEdgeV9 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#else
Void_WMV g_FilterVerticalEdgeV9 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
Void_WMV g_FilterVerticalEdgeV9Last4x8 (U8_WMV* pVhstart, I32_WMV iPixelDistance, I32_WMV iStepSize, I32_WMV iNumPixel);
#endif
Void_WMV g_OverlapBlockHorizontalEdge(I16_WMV *pSrcTop, I16_WMV *pSrcCurr,I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iDstStride,Bool_WMV bTop, Bool_WMV bCurrent, Bool_WMV bWindup);
Void_WMV IntensityCompensation_asm(U8_WMV  *pSrcY,U8_WMV *pSrcU,U8_WMV *pSrcV, I32_WMV y_num,I32_WMV uv_num, I32_WMV iScale, I32_WMV iShift);

#endif

//#else  //Non EMB SIMD MC

Void_WMV g_AddError_EMB_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV* pErrorBuf, I32_WMV iPitch);
Void_WMV g_InterpolateBlockBilinear_EMB_C (const U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, I32_WMV iXFrac, I32_WMV iYFrac, I32_WMV iRndCtrl, Bool_WMV b1MV);

//Types here are I_SIMD because the error calculated here is returned as a bitmask of SSIMD data types.
I_SIMD g_InterpolateBlock_00_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop,  I32_WMV iXFrac, I32_WMV iYFrac, Bool_WMV b1MV, U16_WMV * pTbl); // iXFrac == 0; iYFrac == 0
I_SIMD g_InterpolateBlockBicubic_0x_C ( const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_02_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_x0_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_20_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl); 
I_SIMD g_InterpolateBlockBicubic_xx_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);
I_SIMD g_InterpolateBlockBicubic_x2_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);
I_SIMD g_InterpolateBlockBicubic_2x_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst, EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);
I_SIMD g_InterpolateBlockBicubic_22_C (const  U8_WMV *pSrc, I32_WMV iSrcStride, U8_WMV *pDst,EMB_PBMainLoop  * pPMainLoop, I32_WMV iXFrac, I32_WMV iYFrac,   Bool_WMV b1MV, U16_WMV * pTbl);

Void_WMV g_IDCTDec16_WMV3_C (UnionBuffer * piDst, UnionBuffer * piSrc, I32_WMV iOffsetToNextRowForDCT, I32_WMV iDCTHorzFlags);

I32_WMV g_DecodeInterError4x4_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV g_DecodeInterError8x4_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV g_DecodeInterError4x8_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ, I32_WMV i);
I32_WMV g_DecodeInterError8x8_C(EMB_PBMainLoop  * pPMainLoop, CDCTTableInfo_Dec** ppInterDCTTableInfo_Dec, DQuantDecParam *pDQ);

Void_WMV g_AddNull_EMB_C(U8_WMV* ppxlcDst, U32_WMV* pRef , I32_WMV iPitch);

Void_WMV g_NewVertFilter0Long_C(const U8_WMV  *pSrc,   I32_WMV iSrcStride, U8_WMV * pDst, Bool_WMV b1MV);
U_SIMD g_NewHorzFilterX_C(U8_WMV *pF, const I32_WMV iShift, const I_SIMD iRound2_32, const I8_WMV * const pH, Bool_WMV b1MV);
U32_WMV g_NewVertFilterX_C(const U8_WMV  *pSrc, const I32_WMV iSrcStride, U8_WMV * pDst, const I32_WMV iShift, const I32_WMV iRound32, const I8_WMV * const pV, I32_WMV iNumHorzLoop, Bool_WMV b1MV);
U32_WMV g_NewVertFilterX_C_Clip(const U8_WMV  *pSrc, const I32_WMV iSrcStride, U8_WMV * pDst, const I32_WMV iShift, const I32_WMV iRound32, const I8_WMV * const pV, I32_WMV iNumHorzLoop, Bool_WMV b1MV);
//#endif //_EMB_SSIMD_MC_



tWMVDecodeStatus DecodeInverseInterBlockQuantize16_level2_EMB(
                                                              tWMVDecInternalMember *pWMVDec, 
                                                              CDCTTableInfo_Dec* InterDCTTableInfo_Dec,
                                                              U32_WMV uiNumCoefs,
                                                              U32_WMV * puiCoefCounter,
                                                              I32_WMV * pvalue,
                                                              I32_WMV * pSignBit,
                                                              U32_WMV * plIndex
                                                              );

Void_WMV SetupEMBBMainLoopCtl(tWMVDecInternalMember *pWMVDec);

Void_WMV InitEMBPMainLoopCtl(tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus decodeB_EMB (tWMVDecInternalMember *pWMVDec);

tWMVDecodeStatus DecodeBMB_EMB (tWMVDecInternalMember *pWMVDec,
    I32_WMV imbX, I32_WMV imbY,
    CWMVMBMode*            pmbmd, 
    U8_WMV __huge* ppxlcCurrQYMB,
    U8_WMV __huge* ppxlcCurrQUMB,
    U8_WMV __huge* ppxlcCurrQVMB,
    CoordI xIndex, CoordI yIndex,
    CoordI xMv, CoordI yMv,
    CoordI xMv2, CoordI yMv2
);
tWMVDecodeStatus decodeMBOverheadOfBVOP_WMV3_EMB (EMB_BMainLoop * pBMainLoop,CWMVMBMode* pmbmd, I32_WMV iMBX, I32_WMV iMBY);

//Void_WMV decodeBMV_V9_EMB(EMB_BMainLoop * pBMainLoop, Huffman_WMV* pHufMVTable, CDiffMV_EMB *pDiffMV);

Void_WMV PredictMV_EMB (EMB_BMainLoop  * pBMainLoop, I32_WMV iX, Bool_WMV b1MV, I32_WMV iblk);
I32_WMV PredictBMVPullBackCheck_EMB(EMB_BMainLoop  * pBMainLoop,  I32_WMV bBfrm, I32_WMV iPred);

Void_WMV DirectModeMV_EMB (EMB_BMainLoop * pBMainLoop, I32_WMV iXMotion, I32_WMV iYMotion, Bool_WMV bHpelMode, UMotion_EMB * defMotion);

tWMVDecodeStatus DecodeInverseIntraBlockX9_EMB(tWMVDecInternalMember *pWMVDec,
                                               CDCTTableInfo_Dec** ppIntraDCTTableInfo_Dec, 
                                               I32_WMV iblk,   U8_WMV *pZigzagInv,   Bool_WMV bResidual,
                                               I16_WMV *pPred, I32_WMV iDirection, I32_WMV iShift,
                                               I16_WMV   *pIntra, CWMVMBMode* pmbmd,
                                               DQuantDecParam *pDQ
                                               );

tWMVDecodeStatus DecodeBMB_EMB_Intra(EMB_BMainLoop  * pBMainLoop,
                                   CWMVMBMode*            pmbmd, 
                                   I32_WMV iMBX, I32_WMV iMBY
                                   );

#endif  //_EMB_WMV3_

Void_WMV WMVideoDecUpdateDst411PartialMBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDst411MBYVYU (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);
Void_WMV WMVideoDecUpdateDst411PartialMBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrY, const U8_WMV* pucCurrU, const U8_WMV* pucCurrV,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iRgbWidth, I32_WMV iRgbHeight
);
Void_WMV WMVideoDecUpdateDst411MBUYVY (
    tWMVDecInternalMember *pWMVDec,
    U8_WMV* pBits,
    const U8_WMV* pucCurrYMB, const U8_WMV* pucCurrUMB, const U8_WMV* pucCurrVMB,
    I32_WMV iWidthY, I32_WMV iWidthUV,
    I32_WMV iBitmapWidth
);

#ifndef WMV9_SIMPLE_ONLY
//#ifdef _EMB_3FRAMES_
Void_WMV AdjustReconRange2AltRef (tWMVDecInternalMember *pWMVDec);
Void_WMV IntensityCompensation2AltRef (tWMVDecInternalMember * pWMVDec);
//#endif

Void_WMV DeblockFrameMBRow (tWMVDecInternalMember *pWMVDec, 
                            U8_WMV* ppxliCurrQY, 
                            U8_WMV* ppxliCurrQU, 
                            U8_WMV* ppxliCurrQV, 
                            U8_WMV* ppxliPostY, 
                            U8_WMV* ppxliPostU, 
                            U8_WMV* ppxliPostV, 
                            I32_WMV imbY,
                            I32_WMV m_iBlkSizeXWidthPrevUV,
                            I32_WMV m_iMBSizeXWidthPrevY,
                            I32_WMV m_uintNumMBX,
                            I32_WMV m_uintNumMBY,
                            I32_WMV m_iStepSize,
                            I32_WMV m_iWidthPrevY,
                            I32_WMV m_iWidthPrevUV,
                            I32_WMV m_bDeringOn
                            );
#endif

Void_WMV    AdjustDecRangeMBRow(U8_WMV* ppxliPostQY, 
                            U8_WMV* ppxliPostQU, 
                            U8_WMV* ppxliPostQV, 
                            I32_WMV m_iMBSizeXWidthPrevY, 
                            I32_WMV m_iBlkSizeXWidthPrevUV, 
                            I32_WMV m_iRangeState);

Void_WMV CopyFramesMBRow (U8_WMV* ppxliDestQY, 
                            U8_WMV* ppxliDestQU, 
                            U8_WMV* ppxliDestQV, 
                            U8_WMV* ppxliSrcQY, 
                            U8_WMV* ppxliSrcQU, 
                            U8_WMV* ppxliSrcQV,
                             I32_WMV m_iMBSizeXWidthPrevY, 
                            I32_WMV m_iBlkSizeXWidthPrevUV
                            );


Void_WMV g_UpsampleWFilterLine7 (U8_WMV *pDst, U8_WMV *pSrc, I32_WMV iSize, I32_WMV *x, I32_WMV iPitch);
Void_WMV g_DownsampleWFilterLine9 (U8_WMV *pDst, U8_WMV *pSrc, I32_WMV *x, I32_WMV size, I32_WMV iPitch);
Void_WMV g_UpsampleWFilterLine10_Horiz_MBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV size);
Void_WMV g_HorizUpsampleFilter10_MBRow(
    U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV,
    I32_WMV iDstWidth, I32_WMV iDstWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV, 
    I32_WMV iDstPitchY, I32_WMV iDstPitchUV);
Void_WMV g_UpsampleWFilterLine10_Vert_MBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst, I32_WMV size);
Void_WMV g_UpsampleWFilterLine10_Vert_FirstMBRow(U8_WMV *pDst, U8_WMV *pSrc, I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst,  I32_WMV size);
Void_WMV g_UpsampleWFilterLine10_Vert_LastMBRow(U8_WMV *pDst, U8_WMV *pSrc,  I32_WMV width, I32_WMV iPitch, I32_WMV iPitchDst,  I32_WMV size);
Void_WMV g_VertUpsampleFilter10_FirstMBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV);
Void_WMV g_VertUpsampleFilter10_LastMBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV, I32_WMV iLastMBOffset);
Void_WMV g_VertUpsampleFilter10_MBRow(U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV, 
    U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, 
    I32_WMV iWidth,I32_WMV iWidthUV,  I32_WMV iSrcPitchY, I32_WMV iSrcPitchUV,
    I32_WMV iPitchY, I32_WMV iPitchUV);
tWMVDecodeStatus WMVideoDecColorConvertPerRow(tWMVDecInternalMember *pWMVDec, U8_WMV* pucDecodedOutput);

Void_WMV UpsampleRefFrame3Frm(tWMVDecInternalMember *pWMVDec, U8_WMV* pSrcY, U8_WMV* pSrcU, U8_WMV* pSrcV, U8_WMV* pDstY, U8_WMV* pDstU, U8_WMV* pDstV) ;

#ifdef _SUPPORT_POST_FILTERS_
    void initAdaptPostLevel(tWMVDecInternalMember *pWMVDec);
    void closeAdaptPostLevel(tWMVDecInternalMember *pWMVDec);
    void adaptPostLevel(tWMVDecInternalMember *pWMVDec);


    static void startPostLevelDecodeTime(tWMVDecInternalMember *pWMVDec) { 
        assert( pWMVDec!=NULL && !pWMVDec->m_bInTimeDecode );
        if ( pWMVDec!=NULL && !pWMVDec->m_bInTimeDecode ) {
#           ifdef macintosh
                Nanoseconds nanosec = AbsoluteToNanoseconds(UpTime());
                unsigned __int64 u64nsTime = UnsignedWideToUInt64(nanosec);
                pWMVDec->m_i64QPCDecode -= u64nsTime;
#           else // macintosh
                LARGE_INTEGER llCounter;
                QueryPerformanceCounter( &llCounter );
                pWMVDec->m_i64QPCDecode -= llCounter.QuadPart;
#           endif // macintosh
            pWMVDec->m_bInTimeDecode = TRUE_WMV;
        }
    }
    static void stopPostLevelDecodeTime(tWMVDecInternalMember *pWMVDec) { 
        assert( pWMVDec!=NULL && pWMVDec->m_bInTimeDecode );
        if ( pWMVDec!=NULL && pWMVDec->m_bInTimeDecode ) {
#           ifdef macintosh
                Nanoseconds nanosec = AbsoluteToNanoseconds(UpTime());
                unsigned __int64 u64nsTime = UnsignedWideToUInt64(nanosec);
                pWMVDec->m_i64QPCDecode += u64nsTime;
#           else // macintosh
                LARGE_INTEGER llCounter;
                QueryPerformanceCounter( &llCounter );
                pWMVDec->m_i64QPCDecode += llCounter.QuadPart;
#           endif // macintosh
            pWMVDec->m_bInTimeDecode = FALSE_WMV;
        }
    }
    static void startPostLevelPostTime(tWMVDecInternalMember *pWMVDec) { 
        assert( pWMVDec!=NULL && !pWMVDec->m_bInTimePost );
        if ( pWMVDec!=NULL && !pWMVDec->m_bInTimePost ) {
#           ifdef macintosh
                Nanoseconds nanosec = AbsoluteToNanoseconds(UpTime());
                unsigned __int64 u64nsTime = UnsignedWideToUInt64(nanosec);
                pWMVDec->m_i64QPCPost -= u64nsTime;
                pWMVDec->m_i64QPCPostThisFrame = u64nsTime;
#           else // macintosh
                LARGE_INTEGER llCounter;
                QueryPerformanceCounter( &llCounter );
                pWMVDec->m_i64QPCPost = -llCounter.QuadPart;
                pWMVDec->m_i64QPCPostThisFrame = llCounter.QuadPart;
#           endif // macintosh
            pWMVDec->m_bInTimePost = TRUE_WMV;
        }
    }
    static void stopPostLevelPostTime(tWMVDecInternalMember *pWMVDec) { 
        assert( pWMVDec!=NULL && pWMVDec->m_bInTimePost != 0 );
        if ( pWMVDec!=NULL && pWMVDec->m_bInTimePost != 0 ) {
#           ifdef macintosh
                Nanoseconds nanosec = AbsoluteToNanoseconds(UpTime());
                unsigned __int64 u64nsTime = UnsignedWideToUInt64(nanosec);
                pWMVDec->m_i64QPCPost += u64nsTime;
                pWMVDec->m_i64QPCPostThisFrame = u64nsTime - pWMVDec->m_i64QPCPostThisFrame;
#           else // macintosh
                LARGE_INTEGER llCounter;
                QueryPerformanceCounter( &llCounter );
                pWMVDec->m_i64QPCPost += llCounter.QuadPart;
                pWMVDec->m_i64QPCPostThisFrame = llCounter.QuadPart - pWMVDec->m_i64QPCPostThisFrame;
#           endif // macintosh
            pWMVDec->m_i64QPCDecode -= pWMVDec->m_i64QPCPostThisFrame;
            pWMVDec->m_i64QPCPostThisFrame = 0;
            pWMVDec->m_bInTimePost = FALSE_WMV;
        }
    }

#   ifdef macintosh
        static DWORD GetTickCount( void ) {
            Nanoseconds nanosec = AbsoluteToNanoseconds(UpTime());
            unsigned __int64 u64nsTime = UnsignedWideToUInt64(nanosec);
            return (DWORD)( u64nsTime/1000000 );
        }
#   endif macintosh

#   define INIT_ADAPT_POST_LEVEL(p) initAdaptPostLevel(p)
#   define CLOSE_ADAPT_POST_LEVEL(p) closeAdaptPostLevel(p)
#   define ADAPT_POST_LEVEL(p) adaptPostLevel(p)

#   define START_POST_LEVEL_DECODE_TIME(p) startPostLevelDecodeTime(p)
#   define STOP_POST_LEVEL_DECODE_TIME(p) stopPostLevelDecodeTime(p)
#   define START_POST_LEVEL_POST_TIME(p) startPostLevelPostTime(p)
#   define STOP_POST_LEVEL_POST_TIME(p) stopPostLevelPostTime(p)
#else // _SUPPORT_POST_FILTERS_
#   define INIT_ADAPT_POST_LEVEL(p) 
#   define CLOSE_ADAPT_POST_LEVEL(p)
#   define ADAPT_POST_LEVEL(p)

#   define START_POST_LEVEL_DECODE_TIME(p)
#   define STOP_POST_LEVEL_DECODE_TIME(p)
#   define START_POST_LEVEL_POST_TIME(p)
#   define STOP_POST_LEVEL_POST_TIME(p)

#endif  // _SUPPORT_POST_FILTERS_

I32_WMV prvWMVModulus (I32_WMV a, I32_WMV b);
I32_WMV prvWMVDivision (I32_WMV a, I32_WMV b);

I32_WMV prvWMVReadRegKey (enum KEYS eKey);

#ifdef PPC_SUPPORT_PROFILES
I32_WMV dbgWMVReadPPCRegistry (void);
#endif // PPC_SUPPORT_PROFILES


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

enum HEAP_CATEGORY { DHEAP_STRUCT=0, DHEAP_FRAMES=1, DHEAP_HUFFMAN=2, DHEAP_LOCALHUFF=3, DHEAP_LOCAL_COUNT=4 };

#define TOTAL_HEAP_CATEGORY			DHEAP_LOCAL_COUNT+1

/*
 * DEFINE wmvMalloc and HEAP measurement utilities
 */
#ifdef XDM

//WMV_EXTERN_C void * wmvMalloc(tWMVDecInternalMember *pWMVDec,unsigned int size, int category);
WMV_EXTERN_C void   wmvFree(tWMVDecInternalMember *pWMVDec,void *free_ptr);

#define WMVREGALLOCMEM(lpMem, iSize, category)
#define WMVUNREGALLOCMEM(lpMem)

#else

WMV_EXTERN_C void * MallocHandle(VO_MEM_OPERATOR *memOp, size_t pSize, char *pFile, int pLine, int category);
#define wmvMallocHandle(memOp, size, category)		\
	MallocHandle(memOp, size, __FILE__, __LINE__, category)

#ifndef WANT_HEAP_MEASURE

WMV_EXTERN_C void * vowmvMalloc(tWMVDecInternalMember *pWMVDec,unsigned int size, int category);
WMV_EXTERN_C void   vowmvFree  (tWMVDecInternalMember *pWMVDec,void *free_ptr);

#define wmvMalloc(pWMVDec, size, category)		\
	vowmvMalloc(pWMVDec, size, category)
								
#define wmvFree(pWMVDec, free_ptr)		\
	vowmvFree(pWMVDec, (free_ptr));\
	(free_ptr) = NULL;

#define WMVREGALLOCMEM(lpMem, iSize, category)
#define WMVUNREGALLOCMEM(lpMem)


#else //WANT_HEAP_MEASURE

WMV_EXTERN_C void * vowmvMalloc(tWMVDecInternalMember *pWMVDec,unsigned int size, int category);
WMV_EXTERN_C void   vowmvFree  (tWMVDecInternalMember *pWMVDec,void *free_ptr);

WMV_EXTERN_C void * prvMalloc(tWMVDecInternalMember *pWMVDec,unsigned int pSize, char *pFile, int pLine, int category);
WMV_EXTERN_C void   prvFree(tWMVDecInternalMember *pWMVDec,void *pMem, char *pFile, int pLine);
WMV_EXTERN_C void * prvRegAllocMem(void *new_ptr, size_t pSize, char *pFile, int pLine, int category);
WMV_EXTERN_C void	prvUnregAllocMem(void *lpMem, char *pFile, int pLine);

#define wmvMalloc(pWMVDec, size, category)		\
	prvMalloc(pWMVDec, size, __FILE__, __LINE__, category)
								
#define wmvFree(pWMVDec, free_ptr)		\
	prvFree(pWMVDec, free_ptr, __FILE__, __LINE__);\
	free_ptr = NULL;


#define WMVREGALLOCMEM(lpMem, size, category)	\
	prvRegAllocMem(lpMem, size, __FILE__, __LINE__, category)

#define WMVUNREGALLOCMEM(free_ptr)		\
	prvUnregAllocMem(free_ptr, __FILE__, __LINE__)

#endif //WANT_HEAP_MEASURE
#endif //XDM

/*
 * Define Stack Measurement Utilities
 */

#ifndef WANT_STACK_MEASURE

#define STACK_MEASURE_START(stack_no)
#define STACK_MEASURE_STOP(a, stack_no)
#define WRITE_STACK_LOG(logfile, stackno)

#else


#ifndef STACK_MEASURE_TOPS
#define STACK_MEASURE_TOPS 6000
#endif

WMV_EXTERN_C void prvStackMeasureStart(int stack_no);
WMV_EXTERN_C int  prvStackUsed(int stack_no);
WMV_EXTERN_C int  prvWriteStackTraceLog(const char *log_file, int stackno, int stackSize);

#define STACK_MEASURE_START(stack_no) prvStackMeasureStart(stack_no);
#define STACK_MEASURE_STOP(a, stack_no) int a = prvStackUsed(stack_no);
#define WRITE_STACK_LOG(logfile, stackNo)	\
	{ \
		STACK_MEASURE_STOP(stackSize, stackNo); \
		prvWriteStackTraceLog(logfile, stackNo, stackSize); \
	} \

#endif

/*
 * Define Stack/Heap Measurement Report and Initiailze Utilities
 */

#if defined(WANT_STACK_MEASURE) || defined(WANT_HEAP_MEASURE)

WMV_EXTERN_C	int		prvWriteMemTraceLog();
WMV_EXTERN_C	void	prvHeapMeasureStart(const char *log_file);

#define		WRITE_MEM_LOG()				prvWriteMemTraceLog();
#define		MEM_MEASURE_START(log_file)	prvHeapMeasureStart(log_file);

#else

#define MEM_MEASURE_START(log_file)
#define WRITE_MEM_LOG()
#endif //(WANT_STACK_MEASURE || WANT_HEAP_MEASURE)


#if defined(VOARMV7)

#define g_IDCTDec16_WMV3_Fun						ARMV7_g_IDCTDec16_WMV3_SSIMD
#define g_DecodeInterError8x8_SSIMD_Fun				ARMV7_g_DecodeInterError8x8_SSIMD
#define g_DecodeInterError8x4_SSIMD_Fun				ARMV7_g_DecodeInterError8x4_SSIMD
#define g_DecodeInterError4x8_SSIMD_Fun				ARMV7_g_DecodeInterError4x8_SSIMD
#define g_DecodeInterError4x4_SSIMD_Fun				ARMV7_g_DecodeInterError4x4_SSIMD
#define g_IDCTDec_WMV3_Fun							ARMV7_g_IDCTDec_WMV3
#define g_AddError_SSIMD_Fun						ARMV7_g_AddError_SSIMD
#define g_AddNull_SSIMD_Fun							ARMV7_g_AddNull_SSIMD
#define g_MotionCompZeroMotion_WMV_Fun				ARMV7_g_MotionCompZeroMotion_WMV
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
/* The same as VOARMV6. */
#define g_OverlapBlockHorizontalEdge_Fun			ARMV6_g_OverlapBlockHorizontalEdge 
#define IntensityCompensation_asm_Fun				ARMV6_IntensityCompensation_asm
#define SignPatch32_Fun								ARMV6_SignPatch32

#elif defined(VOARMV6)

#define g_IDCTDec16_WMV3_Fun						ARMV6_g_IDCTDec16_WMV3_SSIMD
#define g_DecodeInterError8x8_SSIMD_Fun				ARMV6_g_DecodeInterError8x8_SSIMD
#define g_DecodeInterError8x4_SSIMD_Fun				ARMV6_g_DecodeInterError8x4_SSIMD
#define g_IDCTDec_WMV3_Fun							ARMV6_g_IDCTDec_WMV3
#define g_AddError_SSIMD_Fun						ARMV6_g_AddError_SSIMD
#define g_AddNull_SSIMD_Fun							ARMV6_g_AddNull_SSIMD
#define g_MotionCompZeroMotion_WMV_Fun				ARMV6_g_MotionCompZeroMotion_WMV
#define g_OverlapBlockHorizontalEdge_Fun			ARMV6_g_OverlapBlockHorizontalEdge
#define IntensityCompensation_asm_Fun				ARMV6_IntensityCompensation_asm
#define SignPatch32_Fun								ARMV6_SignPatch32
/* The same as VOARMV4. */
#define g_DecodeInterError4x8_SSIMD_Fun				g_DecodeInterError4x8_SSIMD
#define g_DecodeInterError4x4_SSIMD_Fun				g_DecodeInterError4x4_SSIMD
#define g_NewVertFilterX_Fun						g_NewVertFilterX
#define g_NewHorzFilterX_Fun						g_NewHorzFilterX
#define g_NewVertFilter0LongNoGlblTbl_Fun			g_NewVertFilter0LongNoGlblTbl
#define g_InterpolateBlock_00_SSIMD_Fun				g_InterpolateBlock_00_SSIMD
#define g_InterpolateBlockBilinear_SSIMD_Fun		g_InterpolateBlockBilinear_SSIMD 
#define g_InterpolateBlockBilinear_SSIMD_11_Fun		g_InterpolateBlockBilinear_SSIMD_11
#define g_InterpolateBlockBilinear_SSIMD_01_Fun		g_InterpolateBlockBilinear_SSIMD_01
#define g_InterpolateBlockBilinear_SSIMD_10_Fun		g_InterpolateBlockBilinear_SSIMD_10
#define g_FilterHorizontalEdgeV9_Fun				g_FilterHorizontalEdgeV9
#define g_FilterHorizontalEdgeV9Last8x4_Fun			g_FilterHorizontalEdgeV9Last8x4
#define g_FilterVerticalEdgeV9_Fun					g_FilterVerticalEdgeV9
#define g_FilterVerticalEdgeV9Last4x8_Fun			g_FilterVerticalEdgeV9Last4x8

#else	/* VOARMV4 or WIN32 */

#define g_IDCTDec16_WMV3_Fun						g_IDCTDec16_WMV3_SSIMD
#define g_DecodeInterError8x8_SSIMD_Fun				g_DecodeInterError8x8_SSIMD
#define g_DecodeInterError8x4_SSIMD_Fun				g_DecodeInterError8x4_SSIMD
#define g_DecodeInterError4x8_SSIMD_Fun				g_DecodeInterError4x8_SSIMD
#define g_DecodeInterError4x4_SSIMD_Fun				g_DecodeInterError4x4_SSIMD
#define g_IDCTDec_WMV3_Fun							g_IDCTDec_WMV3
#define g_AddError_SSIMD_Fun						g_AddError_SSIMD
#define g_AddNull_SSIMD_Fun							g_AddNull_SSIMD
#define g_MotionCompZeroMotion_WMV_Fun				g_MotionCompZeroMotion_WMV
#define g_OverlapBlockHorizontalEdge_Fun			g_OverlapBlockHorizontalEdge
#define IntensityCompensation_asm_Fun				IntensityCompensation_asm
#define SignPatch32_Fun								SignPatch32
#define g_NewVertFilterX_Fun						g_NewVertFilterX
#define g_NewHorzFilterX_Fun						g_NewHorzFilterX
#define g_NewVertFilter0LongNoGlblTbl_Fun			g_NewVertFilter0LongNoGlblTbl
#define g_InterpolateBlock_00_SSIMD_Fun				g_InterpolateBlock_00_SSIMD
#define g_InterpolateBlockBilinear_SSIMD_Fun		g_InterpolateBlockBilinear_SSIMD 
#define g_InterpolateBlockBilinear_SSIMD_11_Fun		g_InterpolateBlockBilinear_SSIMD_11
#define g_InterpolateBlockBilinear_SSIMD_01_Fun		g_InterpolateBlockBilinear_SSIMD_01
#define g_InterpolateBlockBilinear_SSIMD_10_Fun		g_InterpolateBlockBilinear_SSIMD_10
#ifdef DEBLOCK_H_NEON_OPT
#define g_FilterHorizontalEdgeV9_Fun				g_FilterHorizontalEdgeV9_armv7_C
#define g_FilterHorizontalEdgeV9Last8x4_Fun			g_FilterHorizontalEdgeV9Last8x4_armv7_C
#else
#define g_FilterHorizontalEdgeV9_Fun				g_FilterHorizontalEdgeV9
#define g_FilterHorizontalEdgeV9Last8x4_Fun			g_FilterHorizontalEdgeV9Last8x4
#endif
#ifdef DEBLOCK_V_NEON_OPT
#define g_FilterVerticalEdgeV9_Fun					g_FilterVerticalEdgeV9_armv7_C
#define g_FilterVerticalEdgeV9Last4x8_Fun			g_FilterVerticalEdgeV9Last4x8_armv7_C
#else
#define g_FilterVerticalEdgeV9_Fun					g_FilterVerticalEdgeV9
#define g_FilterVerticalEdgeV9Last4x8_Fun			g_FilterVerticalEdgeV9Last4x8
#endif
#endif
void Round_MV_Chroma_FRAMEMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,I32_WMV xindex,I32_WMV yindex);
void Round_MV_Luam_FRAMEMV(tWMVDecInternalMember *pWMVDec, I32_WMV*IX, I32_WMV*IY,I32_WMV eBlk,I32_WMV xindex,I32_WMV yindex);
Bool_WMV PullBackMotionVector_UV (tWMVDecInternalMember* pWMVDec, I32_WMV*iMvX, I32_WMV*iMvY, I32_WMV imbX, I32_WMV imbY);
void Round_MV_Luam_FILEDMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,I32_WMV xindex,I32_WMV yindex);
void Round_MV_Chroma_FILEDMV(tWMVDecInternalMember *pWMVDec, I32_WMV *IX, I32_WMV*IY,I32_WMV* F,I32_WMV xindex,I32_WMV yindex);
void Round_MV_Luam(tWMVDecInternalMember *pWMVDec, I32_WMV *iMvX, I32_WMV *iMvY/*,int blk*/);
#endif // __WMVDEC_FUNCTION_H_

