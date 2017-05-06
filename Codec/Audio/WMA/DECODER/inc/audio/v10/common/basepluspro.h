//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef __BASEPLUSPRO_H_
#define __BASEPLUSPRO_H_

#include "macros.h"
//#include "msaudio.h"
//#include "msaudiodec.h"
//#define BASEPLUS_CREATE_DECODER_BPCOEF_BUF

#if defined (BUILD_WMAPROLBRV2) && defined (BUILD_WMAPRO)

#define NUM_SFBAND_TABLES 4
#define MAX_SF_BANDS 162
#define SFFREQ_INF_SIZE 28
#define SFFREQ_16K_SIZE 54
#define SFFREQ_8K_SIZE 89
#define SFFREQ_4K_SIZE 162

#define BASEPLUS_MASK_ESC_RUNBITS 8
#define BASEPLUS_MASK_ESC_LEVBITS 8

#define BASEPLUS_WEIGHT_ESC_RUNBITS 5 //max bad segment is 31 so 5 bits is enough
#define BASEPLUS_WEIGHT_ESC_LEVBITS 4 //max weight is 15 so 4 bits is enough

#define MAX_WEIGHT_FACTOR    16

#define BPLUS_TOOLBOX_PREDICTIVE    0
#define BPLUS_TOOLBOX_INTERLEAVE    1
#define BPLUS_TOOLBOX_PDFSHIFT      2   

#define BPLUS_MIN_PERIODS_PERSUBFRM    16
#define BPLUS_MIN_PERIOD_LENGTH         8
#define BPLUS_PERIOD_FRACTION_BITS      3
#define BPLUS_MAX_PREROLL               4
#define BPLUS_MAX_FIRSTITLVPERIOD       8

#define BPLUS_QCOEF_LPCORDER_MAX 16  // 1, 2, 4, 8, 16
#define BPLUS_QCOEF_LPCSHIFT_MAX  8  // 1 - 8
#define BPLUS_QCOEF_LPCSEGMT_MAX  8  // 1 - 8
#define BPLUS_QCOEF_SEG_MINLEN  128

#define BPLUS_QCOEF_LONGHIST_FRAMES     5    
#define BPLUS_CROSSSUBFRM_PRED_FREQCUTOFF_INV 2

#define BPLUS_MIN_PERIODS_PERSUBFRM_PDFSHIFT    8

typedef enum  {
	SFPRED_SPECTRAL = 0, 
	SFPRED_SPATIAL, 
	SFPRED_TEMPORAL,
	MAX_SFPredictionType_VALUE = MAX_VOENUM_VALUE 
} SFPredictionType;

#define ROUNDFANSIC(x) ((x>=0)?(I32)(x+0.5):(I32)(x-0.5))

// copied from aecint. Platform other than X86 needs to implement their own version
// since they probably do not have shld instruction
INLINE I32 L_mult_shl(I32 x1, I32 x2, unsigned char shift)
{
#ifndef WMA_TARGET_X86
	return (I32)( ((I64)x1*(I64)x2)>>(32-shift) );
#else
    __asm
	{	mov eax,x1;
		imul x2;
        mov cl, shift;
        shld edx, eax, cl;   // overflow may happen, not checked
//      shl eax, cl;
//		shl eax, 1;          // the highest bit moved to CF
//		adc edx, 0;          // round (add CF)
		mov eax, edx;
	}
    //return with result in EAX;
#endif
};

#define SETBADCOEFFLAG(rgiBadCoef, index) \
	((I8*)rgiBadCoef)[(index) >> 3] = (((I8*)rgiBadCoef)[(index) >> 3]) & (~(1 << ((index) & 0X07)));

#define CLRBADCOEFFLAG(rgiBadCoef, index) \
    ((I8*)rgiBadCoef)[(index) >> 3] = (((I8*)rgiBadCoef)[(index) >> 3]) | (1 << ((index) & 0X07));

#define GETBADCOEFFLAG(rgiBadCoef, index) \
	(!((((I8*)rgiBadCoef)[(index) >> 3] >> ((index) & 0X07)) & 1))

// define FPNLQ (fixed point for NLQ) type and conversion from FLOAT
#ifndef BUILD_INTEGER
  #define FPNLQ Float
  #define FPNLQFRMFLT(x) (x)
  #define COEF_FROM_FPNLQ(x) (x)  
  #define COEF_FROM_LEVEL(level) ((CoefType)(level)) // same as COEF_FROM_INT
  #define LEVEL_FROM_COEF(coef) (ROUNDF(coef))
  #define MULT_COEF_BY_NLQSCALE(coef, scale) (coef * scale)
#else 
  #define FPNLQ_FRAC_BITS 16
  #define FPNLQ I32
  #define FPNLQFRMFLT(x) (ROUNDFANSIC(x*(1<<FPNLQ_FRAC_BITS)))
  #define COEF_FROM_FPNLQ(x) ((FPNLQ_FRAC_BITS >= pau->m_cLeftShiftBitsFixedPre)? \
                              ((x)>>(FPNLQ_FRAC_BITS-pau->m_cLeftShiftBitsFixedPre)): \
                              ((x)<<(pau->m_cLeftShiftBitsFixedPre-FPNLQ_FRAC_BITS)))
  #define COEF_FROM_LEVEL(level) ((CoefType)((level)<<(pau->m_cLeftShiftBitsFixedPre)))
  #define LEVEL_FROM_COEF(coef) ((Int)((coef)>>(pau->m_cLeftShiftBitsFixedPre)))
  #ifdef COEF64BIT
    #define MULT_COEF_BY_NLQSCALE(coef, scale) ((coef * scale) >> FPNLQ_FRAC_BITS)
  #else
    #define MULT_COEF_BY_NLQSCALE(coef, scale) (L_mult_shl(coef, scale, 32-FPNLQ_FRAC_BITS))
  #endif
#endif

// overlay mode
#define BPPLUS_MAXBADCOEFSEG    32 //actual max bad coef seg is 31

//typedef struct CChannelGroupInfo CChannelGroupInfo;
//typedef struct CBasePlusObject CBasePlusObject;
typedef struct CBasePlusObject
{
    Bool        m_fOverlayMode;
    Bool        m_fExtendMode;
    Bool        m_fFrameParamUpdate;
    Bool        m_fLowDelayWindow;
    Int         m_iOverlapWindowDelay; //in 10ms
    Bool        m_fUseProMaskRunLevelTbl;
    own Int**   m_rgrgcValidSFBand; // scale factor band
    own I16**   m_rgrgiSFBandIndexOrig;
    Int         m_cValidSFBand;      
    const I16*  m_rgiSFBandIndex;    
    own I16*    m_rgiSFBandIndexCurr;
    own I16*    m_rgiSFBandIndexPrev;
    own I32*    m_rgiSFQ;
    own I8*     m_rgiSFBandMarker;
    own I32*    m_rgiSFQResampled;
    Bool        m_fLinearQuantization;
#ifdef BASEPLUS_CREATE_DECODER_BPCOEF_BUF
    own CoefType* m_rgBPCoef;
#endif //BASEPLUS_CREATE_DECODER_BPCOEF_BUF
    Int         m_cBPTileQuant;
    Int         m_iBPChannelQuantMax;
    struct CChannelGroupInfo* m_rgBPChannelGrpInfo;
    Int         m_cBPChannelGroup;
    own Bool*   m_rgfBPVisited;
    Bool        m_fScalePriorToChannelXformAtDec;
    Int         m_iCurrSFBandTable;
    Bool        m_bFirstTile;
    Int         m_iSFQMultiplier;

    // following buffers are sharing same memory with badcoef.
    I16        *m_rgi16BPTmpBufA; // cFrameSampleHalf/4 bytes
    I16        *m_rgi16BPTmpBufB; // cFrameSampleHalf/4 bytes
    I16        *m_rgi16BPTmpBufC; // cFrameSampleHalf/2 bytes

    own I32    *m_rgiBPBadCoef;
    own I32   **m_rgrgiBPBadCoef;
    own I16    *m_rgiBadCoefSegPos;
    own I32    *m_rgiBPWeightFactor;

    I32         m_iHoleWidthMinIdx;
    I32         m_iHoleWidthMin;    
    I32         m_iHoleSegWidthMinIdx;
    I32         m_iHoleSegWidthMin;

    I32         m_iWeightQuantMultiplier;
    Bool        m_bSingleWeightFactor;
    Bool        m_bWeightFactorOnCodedChannel;

    Int         m_NLQIdx;               //nonlinear quantizer index.
    U16         m_NLQDecTblSize;
    FPNLQ const *m_NLQDecTbl;

    I32         m_iExplicitStartPos;
} CBasePlusObject;

#endif // BUILD_WMAPROLBRV2 && BUILD_WMAPRO
#endif // __BASEPLUSPRO_H_

