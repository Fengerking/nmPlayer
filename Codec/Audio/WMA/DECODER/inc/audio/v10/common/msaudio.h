//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    MsAudio.h

Abstract:

    Implementation of public member functions for CAudioObject.

Author:

    Wei-ge Chen (wchen) 11-March-1998

Revision History:


*************************************************************************/

// Compile Time Flags normally defined in project seetings
// these vary from specific project to project
// BUILD_INTEGER - build integer version
// BUILD_INT_FLOAT - build float version of integer tree = !BUILD_INTEGER
// ENCODER - build encoder
// DECODER - build decoder = !ENCODER
// ENABLE_ALL_ENCOPT - build support for all encoder options (otherwise just HighRate)
// ENABLE_LPC - build LPC spectrogram weighting for LowRate configurations (only defined if ENABLE_ALL_ENCOPT)
// ENABLE_EQUALIZER - build support for equalizer
// Known Useful Combinations
//   !BUILD_INTEGER &&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC    = Normal Encoder
//    BUILD_INTEGER &&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC    = Partially Integerized Encoder
//    BUILD_INTEGER &&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC    = Normal Integer Do-It-All Decoder
//    BUILD_INTEGER &&  ENABLE_ALL_ENCOPT && !ENABLE_LPC    = Normal Integer HighRate & MidRate Decoder
//    BUILD_INTEGER && !ENABLE_ALL_ENCOPT && !ENABLE_LPC    = Normal Integer HighRate Only Decoder
//   !BUILD_INTEGER &&  ENABLE_ALL_ENCOPT &&  ENABLE_LPC    = IntFloat Do-It-All Decoder
//   !BUILD_INTEGER &&  ENABLE_ALL_ENCOPT && !ENABLE_LPC    = IntFloat HighRate & MidRate Decoder
//   !BUILD_INTEGER && !ENABLE_ALL_ENCOPT && !ENABLE_LPC    = IntFloat HighRate Only Decoder
#ifndef __MSAUDIO_H_
#define __MSAUDIO_H_

// ***********************************************



#include <stdio.h>
//#include <stdlib.h>
#include "voWMADecID.h"
#include "macros.h"
#include "wmamalloc.h"
#include "DecTables.h"
#include "pcmfmt.h"
#include "drccommon.h"
#include "strmdec_wma.h"
#include "wmaprodecS_api.h"
#include "wmabuffilt.h"
#include "wmawfxdefs.h"


#ifdef BUILD_UES
// Universal Elementry Stream
#include "ues.h"
#endif
// header info for debugging purposes
#ifdef WMA_DEBUG_DUMP
#include "wmadebug.h"
#endif
//#define WMA_ENTROPY_TEST
#ifdef WMA_ENTROPY_TEST
#define SANJEEVM_LOGGING
#endif

// define _WMA_FUZZ_ for WinCE assert handling in DEBUG
//#define _WMA_FUZZ_
#if defined(_WMA_FUZZ_) && (defined(UNDER_CE) || defined(LINUX)) && defined(DEBUG)
// choose from: dump to file, break or do nothing
#undef assert
extern FILE* g_fpText;
#define THIS_FILE ((strrchr(__FILE__, '/') ?: __FILE__ - 1) + 1)
#define assert(a) { if (!(a)) fprintf(g_fpText, "%s:%d\n", strrchr(__FILE__, '\\') + 1, __LINE__); }
//#define assert(a) { if (!(a)) DebugBreak(); }
//#define assert(a)
#endif

#if defined(BUILD_WMAPROLBRV2)
#include "basepluspro.h"
#endif

//
// To verify desktop v10ce vs desktop v10, define DISABLE_ARM_OPTS and undef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL
//
 
//#ifdef WMA_TARGET_ARM
//#include <arm_flags.h>
//#endif //WMA_TARGET_ARM

#if defined (WMA_OPT_VLC_DEC_ARM) && WMA_OPT_VLC_DEC_ARM
extern const U16    g_rgiHuffDecTbl16smOb_Pre[];
extern const U16    g_rgiHuffDecTbl16ssOb_Pre[];
extern const U16    g_rgiHuffDecTbl44smOb_Pre[];
extern const U16    g_rgiHuffDecTbl44ssOb_Pre[];
extern const U16    g_rgiHuffDecTbl44smQb_Pre[];
extern const U16    g_rgiHuffDecTbl44ssQb_Pre[];
#endif

#if defined(_PPC_) || defined(_M_PPCBE)
#   define _BIG_ENDIAN_
#endif

#if defined(UNDER_CE) || defined(LINUX)		// kludgy, but incgen doesn't work properly with __bound. To be resolved later

typedef Int BoundInt;
typedef U16 InBoundU16;
typedef U32 InBoundU32;
typedef Int InBoundInt;
typedef I32 InBoundI32;
typedef I64 InBoundI64;

#else	// defined(UNDER_CE) || defined(LINUX)

#ifndef __bound
#define __bound
#endif // __bound

//__in_bound has not been defined in Xenon <sal.h> yet
#ifdef _XBOX
#ifndef __in_bound
#define __in_bound
#endif //__in_bound
#endif	// _XBOX

typedef __bound Int BoundInt;
typedef __in_bound U16 InBoundU16;
typedef __in_bound U32 InBoundU32;
typedef __in_bound Int InBoundInt;
typedef __in_bound I32 InBoundI32;
typedef __in_bound I64 InBoundI64;

#endif	// defined(UNDER_CE) || defined(LINUX)

#ifdef _XBOX

#if _XBOX_VER == 200
#	define _WMA_TARGET_XBOX2_
#else
#	define _WMA_TARGET_XBOX1_
#endif
#endif //_XBOX

#ifdef _WMA_TARGET_XBOX2_
#define WMA_OPT_AURECON_XENON
#define WMA_OPT_WTSPECNORM_XENON
#define WMA_OPT_AUWINDOW_XENON
#define WMA_OPT_QUANTLOOP_XENON
#define WMA_OPT_FFT_XENON
#define WMA_OPT_DCT_XENON
#define WMA_OPT_SUBFRAMERECON_XENON
#define WMA_OPT_HUFFDEC_XENON
#define WMA_OPT_LPC_XENON
#define WMA_OPT_STRMDEC_XENON
#define WMA_OPT_AUGETPCM_XENON
#define WMA_OPT_AURECONCOEFFSHIGH_XENON
#define WMA_OPT_AURECONCOEFFSHIGHPRO_XENON
#define WMA_OPT_STRMDECPRO_XENON
//#define WMA_OPT_FLOAT_PREDICTOR_XENON  
#endif //_WMA_TARGET_XBOX2_

#include "playropt.h"

// code for testing.
//#define TEST_CODE
#ifdef TEST_CODE
    #include "msaudiotest.h"
#endif // TEST_CODE

// code for experiment. maybe used for further improvement.
// #define EXPT_CODE

// Encoder-only/decoder-only defines are no longer allowed. All audio components
// must be built with same defines so that encoder and decoder may both be linked into
// one EXE.
#if defined(ENCODER) || defined(DECODER)
#error Encoder- and Decoder-specific defines are no longer allowed. Encoder/decoder-specific code must perform encoder/decoder identification at runtime.
#endif  // defined(ENCODER) || defined(DECODER)



#ifdef WMA_TARGET_ARM
#ifdef _IOS
//ARM specific optimization
//define as 1 to turn on asm optimization, 0 to turn it off
#define WMA_OPT_LPCLSL_ARM              1       //lpclsl_arm.s
#define WMA_OPT_LPCSTD_ARM              1       //lpcstd_arm.s
#define WMA_OPT_FFT_ARM                 1       //fft_arm.s
#define WMA_OPT_AURECON_ARM             1       //msaudio_arm.s -- iOS platform disable
#define WMA_OPT_INVERSQUAN_LOWRATE_ARM  1       //lowrate_arm.s
#define WMA_OPT_SUBFRAMERECON_ARM       1       //msaudiostdpro_arm.s 20111010
#define WMA_OPT_SCALE_COEFFS_V3_ARM     0       //msaudiopro_arm.s 20111010
#else
//ARM specific optimization
//define as 1 to turn on asm optimization, 0 to turn it off
#define WMA_OPT_LPCLSL_ARM              1       //lpclsl_arm.s
#define WMA_OPT_LPCSTD_ARM              1       //lpcstd_arm.s
#define WMA_OPT_FFT_ARM                 1       //fft_arm.s
#define WMA_OPT_AURECON_ARM             1       //msaudio_arm.s -- iOS platform disable
#define WMA_OPT_INVERSQUAN_LOWRATE_ARM  1       //lowrate_arm.s
#define WMA_OPT_SUBFRAMERECON_ARM       1       //msaudiostdpro_arm.s
#define WMA_OPT_SCALE_COEFFS_V3_ARM     1       //msaudiopro_arm.s
#endif  //_IOS
#endif //WMA_TARGET_ARM

//
// begin decide CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL            
//

#undef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL_ENABLE    

#if CX_DCT_ONLY==1
#if !WMA_OPT_FFT_ARM 
#if !( defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_DCTIV))
#if !defined (BUILD_INT_FLOAT)
#define CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL_ENABLE    // combine auApplyWindow, Relfection, and DCTIV for scale factor 1 using lookup tables
#endif
#endif
#endif
#endif

#if defined (_WMA_TARGET_XBOX2_) && defined (WMA_OPT_DCT_XENON)
#undef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL_ENABLE    
#endif

#if defined(WMA_TARGET_X86) && defined(BUILD_INT_FLOAT) && !defined (WMA_DISABLE_SSE1)
#undef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL_ENABLE    
#endif

#if defined(WMA_TARGET_SH4)
#undef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL_ENABLE    
#endif

#ifdef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL_ENABLE
#define CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL    1
#else
#define CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL    0
#endif

//
// end decide CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL            
//

// temporarily disable FFT_LOOKUPTBL for PK release. mismatch found for half transform. bug 13417
#if defined(BUILD_INTEGER) && !(defined(WMA_TARGET_SH3) || defined(WMA_TARGET_SH4) || defined(WMA_DECPK_BUILD))

#define FFT_LOOKUPTBL                   1       // use table-based FFT for size=128
#define DCT_LOOKUPTBL                   1       // use table-based DCT for size=256
#define BITREVERSAL_FFTDCT_LOOKUPTBL    1       // table DCT/FFT co-optimization for 256 pt DCT
                                                // both DCT_LOOKUPTBL and FFT_LOOKUPTBL must be on to properly enable BITREVERSAL_FFTDCT_LOOKUPTBL
                                                // Valid configurations:
                                                // 1) FFT_LOOKUPTBL = 1, DCT_LOOKUPTBL = 1, BITREVERSAL_FFTDCT_LOOKUPTBL = 1
                                                //    enable table DCT, table FFT, combined bit reversal/postFFT twiddle/demodulation opt
                                                //    either in ASM if WMA_OPT_FFT_ARM = 1, or in C if WMA_OPT_FFT_ARM = 0
                                                // 2) FFT_LOOKUPTBL = 1, DCT_LOOKUPTBL = 1, BITREVERSAL_FFTDCT_LOOKUPTBL = 0
                                                //    enable table DCT, table FFT, disable combined bit reversal/postFFT twiddle/demodulation opt
                                                //    either in ASM if WMA_OPT_FFT_ARM = 1, or in C if WMA_OPT_FFT_ARM = 0
                                                // 3) FFT_LOOKUPTBL = 0, DCT_LOOKUPTBL = 0, BITREVERSAL_FFTDCT_LOOKUPTBL = 1
                                                //    disable table DCT, table FFT, combined bit reversal/postFFT twiddle/demodulation opt for both C and ASM
                                                //    BITREVERSAL_FFTDCT_LOOKUPTBL has no effect regardless it is on or off
                                                // 4) FFT_LOOKUPTBL = 0, DCT_LOOKUPTBL = 0, BITREVERSAL_FFTDCT_LOOKUPTBL = 0
                                                //    disable table DCT, table FFT, combined bit reversal/postFFT twiddle/demodulation opt for both C and ASM
                                                //    BITREVERSAL_FFTDCT_LOOKUPTBL has no effect regardless it is on or off

#define SMALL_BR128TWIDDLE_LOOKUPTBL    1       // small bit reversal/post 128 pt FFT twiddle lookup tables
                                                // table sizes reduced by half when this flag is on 
                                                // only has effect when BITREVERSAL_FFTDCT_LOOKUPTBL is enabled

#else   // !( (defined(WMA_TARGET_SH3) || defined(WMA_TARGET_SH4)) && defined(BUILD_INTEGER) )

#ifdef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL
    #undef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL
#endif

#define CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL    0
#define FFT_LOOKUPTBL                   0
#define DCT_LOOKUPTBL                   0
#define BITREVERSAL_FFTDCT_LOOKUPTBL    0
#define SMALL_BR128TWIDDLE_LOOKUPTBL    0

#endif  // !( (defined(WMA_TARGET_SH3) || defined(WMA_TARGET_SH4)) && defined(BUILD_INTEGER) )

#if BITREVERSAL_FFTDCT_LOOKUPTBL && (!DCT_LOOKUPTBL || !FFT_LOOKUPTBL) 
#pragma COMPILER_MESSAGE(__FILE__ "(238) : Error - Both DCT_LOOKUPTBL and FFT_LOOKUPTBL must be on to enable BITREVERSAL_FFTDCT_LOOKUPTBL.")
#endif

#if (defined(CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL) && CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL) && (!DCT_LOOKUPTBL) 
#pragma COMPILER_MESSAGE(__FILE__ "(346) : Error - DCT_LOOKUPTBL must be on to enable CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL.")
#endif

#ifdef WMA_TARGET_ARM
//#define OVERLAPADD_LOOKUPTBL
//#define MAX_OVADD_TLU_SIZE 8192
//#define ENABLE_FFTDCT_VAR_LOOKUPTBL
#endif

#ifdef ENABLE_FFTDCT_VAR_LOOKUPTBL 
#define FFTDCT_VAR_LOOKUPTBL            1
#undef CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL
#undef FFT_LOOKUPTBL
#undef DCT_LOOKUPTBL
#undef BITREVERSAL_FFTDCT_LOOKUPTBL
#undef SMALL_BR128TWIDDLE_LOOKUPTBL
#define CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL    0
#define FFT_LOOKUPTBL                   0
#define DCT_LOOKUPTBL                   0
#define BITREVERSAL_FFTDCT_LOOKUPTBL    0
#define SMALL_BR128TWIDDLE_LOOKUPTBL    0

// select desired lookup table coverage.  Sizes below refer to FFT; DCT size is 2x FFT size
#define MIN_FFT_LUT_SIZE_LOG2           6       // must be >= 5 and <= 12
#define MAX_FFT_LUT_SIZE_LOG2           10      // must be >= MIN_FFT_LUT and <= 12
#define MIN_FFT_LUT_SIZE				(1 << MIN_FFT_LUT_SIZE_LOG2)
#define MAX_FFT_LUT_SIZE        		(1 << MAX_FFT_LUT_SIZE_LOG2)
// total size of the tables in bytes is 16 * SIZE_FFTDCT_LUT
#define SIZE_FFTDCT_LUT                 0xf80 //(4 * MAX_FFT_LUT_SIZE - 2 * MIN_FFT_LUT_SIZE)
// additional space for prefetch
#define SIZE_FFTDCT_LUT_CACHE SIZE_FFTDCT_LUT + 512

#define FFT_SINGLE_BUTTERFLIES		    1	// Do one butterfly pr. (inner) loop iteration, instead of 2
#define FFT_SEPARATE_LOOP_STAGE123		1	// Separate loops for 1st, 2nd and 3rd stage (non-trivial bflys)									  // NOTE: Assumes FFT size >= 16
#define FFT_SEPARATE_NEXT_TO_LAST_STAGE 1	// Separate loop for second to last stage
#define RADIX4_FFT                      0   // Use Radix-4 instead of Radix-2 (ASM version only)
#endif	// ENABLE_FFTDCT_VAR_LOOKUPTBL

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

// Forward data type decl
typedef struct PerChannelInfoENC PerChannelInfoENC;
//typedef struct _DRC DRC;

typedef void (*PFNSETSAMPLE)(const PCMSAMPLE iValue, PCMSAMPLE *pCurrentPos,
                             const struct CAudioObject *pau, const Int iOffset);
typedef PCMSAMPLE (*PFNGETSAMPLE)(const PCMSAMPLE *pCurrentPos,
                                  const Int nBytePerSample,
                                  const Int nValidBitsPerSample,
                                  const Int iOffset);

typedef Float (*PFNGETFLOATSAMPLE)(const PCMSAMPLE *pCurrentPos,
                                   const Int nBytePerSample,
                                   const Int nValidBitsPerSample,
                                   const Int iOffset);


typedef enum {
	STEREO_INVALID = -1, 
	STEREO_LEFTRIGHT = 0, 
	STEREO_SUMDIFF,
	MAX_STEREOMODE_VAL = MAX_VOENUM_VALUE
} StereoMode;

typedef enum {
	MULTICH_INVALID = 0, 
	MULTICH_HADAMARD = 1, 
	MULTICH_IDENTITY = 2, 
	MULTICH_DCT = 3,
	MAX_MULTICH_VALUE = MAX_VOENUM_VALUE
} PredefinedXformType;

typedef enum {
	BEGIN_PACKET, 
	SEEK_TO_NEXT_PACKET, 
	BEGIN_FRAME, 
	FRAME_HDR, 
	BEGIN_SUBFRAME, 
	DECODE_SUBFRAME,
	DECODE_STUFFINGBITS, 
	DECODE_TRAILERBIT, 
	END_SUBFRAME1, 
	END_SUBFRAME2,
	MAX_DecodeStatus_VALUE = MAX_VOENUM_VALUE
} DecodeStatus;
typedef enum {
	FHDR_DONE, 
	FHDR_SIZE, 
	FHDR_PREV, 
	FHDR_CURR, 
	FHDR_PPXFORM, 
	FHDR_DRC_PARAM,
	FHDR_SILENCE, 
	FHDR_SILENCE_LEAD_FLAG, 
	FHDR_SILENCE_LEAD, 
	FHDR_SILENCE_TRAIL_FLAG, 
	FHDR_SILENCE_TRAIL, 
	FHDR_ADJUST_TIMESTAMP,
	MAX_FHdrDecodeStatus_VALUE = MAX_VOENUM_VALUE
} FHdrDecodeStatus; 
typedef enum {
	SUBFRM_HDR, 
	SUBFRM_COEFDEC_LOSSY, 
	SUBFRM_COEFDEC_MLLM,
	SUBFRM_COEFDEC_PLLM, 
	SUBFRM_REC_MLLM, 
	SUBFRM_REC_LOSSY, 
	SUBFRM_REC_PLLM,
	SUBFRM_SAVE_PREV, 
	SUBFRM_DONE,
	MAX_SubFrmDecodeStatus_VALUE = MAX_VOENUM_VALUE
} SubFrmDecodeStatus;
typedef enum {HDR_SIZE, HDR_RAWPCM, HDR_SEEKABLE, HDR_INTERCH_DECORR, HDR_QUANT,
HDR_V3_QUANT_MODIFIER, HDR_NOISE1, HDR_NOISE2, HDR_MSKUPD, HDR_BARK, HDR_DONE,
HDR_V3_ESCAPE, HDR_V3_ESCAPE_SKIPDATA, HDR_V3_FREQEX_DATA, HDR_V3_LLM, HDR_V3_CHXFORM, HDR_INFO_SEEKABLE_BEGIN, 
HDR_DO_ACFILTER, HDR_ENTROPY_CODING, HDR_INTERCH_DECORR_MCLMS, 
HDR_ACFILTER_ORDER, HDR_ACFILTER_SCALING, HDR_ACFILTER, HDR_MCLMS_ORDER, HDR_MCLMS_SCALING, HDR_MCLMS_SEND,
HDR_MCLMS_COEFF_BITS, HDR_MCLMS_COEFF_A,HDR_MCLMS_COEFF_B,
HDR_CDLMS_SEND, HDR_V3_VECCODER_POSEXIST, HDR_V3_VECCODER_POS,
HDR_V3_POWER, HDR_FILTERS_PARA, HDR_RES_MEM_SCALING, HDR_QSTEPSIZE_B4_PLLM, HDR_V3_RAWPCM_LLM, 
HDR_V3_LPC_ORDER, HDR_V3_LPC_UNIFIED,
HDR_PURE_IN_MLLM_SEEKABLE, HDR_PURE_IN_MLLM, HDR_PURE_IN_MLLM_FIRST_FRM, HDR_PURE_IN_MLLM_LAST_FRM, 
HDR_MLLM_FILTER, HDR_V3_LPC_UNIFIED_SCALING, HDR_MLLM_QUANT_STEPSIZE, HDR_V3_LPC_UNIFIED_INTBITS,
HDR_V3_LPC_COEF,
HDR_V3_POWER_LLM, HDR_MLTCHL_MIX_SEQ, HDR_PADDED_ZEROS, HDR_PADDED_ZEROS_B, HDR_V2_POWER,
MAX_HdrDecodeStatus_VALUE = MAX_VOENUM_VALUE
} HdrDecodeStatus; //and a lot more
typedef enum {
	HDRFLT_CLMS_AMOUNT, 
	HDRFLT_CLMS_ORDERS, 
	HDRFLT_CLMS_SCALINGS, 
	HDRFLT_CLMS_COEFF,
	MAX_HdrDecodeFltParStatus_VALUE = MAX_VOENUM_VALUE
}HdrDecodeFltParStatus;
typedef enum {
	HDRFLT_CLMS_COEFFS_SENT, 
	HDRFLT_CLMS_COEFFS_SENTBITS, 
	HDRFLT_CLMS_COEFFS_VALUE,
	MAX_HdrDFPCDLMSS_VALUE = MAX_VOENUM_VALUE
}HdrDecodeFltParCDLMSStatus;
typedef enum {
	VLC, 
	FRAME_END, 
	NORMAL, 
	ESCAPE, 
	TBL_INDEX, 
	ESCAPE_LEVEL, 
	ESCAPE_RUN,
	MAX_RunLevelStatus_VALUE = MAX_VOENUM_VALUE
} RunLevelStatus;
typedef enum {
	CHXFORM_BEGIN, 
	CHXFORM_LEVEL, 
	CHXFORM_CHGRP, 
	CHXFORM_XFORM_ANGLES,
	CHXFORM_XFORM_SIGNS, 
	CHXFORM_XFORMONOFF0, 
	CHXFORM_XFORMONOFF1, 
	CHXFORM_XFORMONOFF_DONE,
	CHXFORM_CONTINUED, 
	CHXFORM_DONE,
	MAX_ChXformStatus_VALUE = MAX_VOENUM_VALUE
} ChXformStatus;
typedef enum {
	PPXFORM_BEGIN, 
	PPXFORM_ONOFF, 
	PPXFORM_GENERIC, 
	PPXFORM_COEF, 
	PPXFORM_DONE,
	MAX_PPXformStatus_VALUE = MAX_VOENUM_VALUE
} PPXformStatus;
//V3 LLM. Chao. Added.
typedef enum {
	CH_BEGIN, 
	CH_LSP_LPCMODE, 
	CH_TRANSIENT, 
	CH_TRANSIENT_POS, 
	CH_LSP_DIVISOR, 
	CH_LSP_COEF, 
	CH_LPC_COEF, 
	CH_DIVISOR_WIDTH, 
	CH_INIT_ARITHDECODER, 
	CH_DIVISOR, 
	CH_COEF, 
	CH_DONE,
	MAX_LLMDCS_VALUE = MAX_VOENUM_VALUE
} LLMDecodeCoefStatus;
typedef enum {
	FIRST_PART, 
	FIRST_PART_B, 
	FIRST_PART_C, 
	SECOND_PART,
	MAX_CDS_VALUE = MAX_VOENUM_VALUE
} ColombDecodeStatus;
typedef enum {
	FIRSTSPL_SIGN_BIT, 
	FIRSTSPL_OTHER_BITS,
	MAX_D1SplS_VALUE = MAX_VOENUM_VALUE
} Decode1stSplStatus;

typedef enum {
	LargeValBitCnt, 
	LargeVal8, 
	LargeVal16, 
	LargeVal24,
	LargeVal32_1, 
	LargeVal32_2,
	MAX_LVDS_VALUE = MAX_VOENUM_VALUE
} LargeValDecodeStatus;

typedef enum {TABLE_INDEX,
GET_VEC4, GET_VEC2, GET_VEC1, GET_ESC_VEC1, GET_SIGN4,
GET_VEC4SMALL, GET_VEC2SMALL, GET_VEC1SMALL, DECODE_RLSMALL,
FREQADAPTIDX_4D, FREQADAPTIDX_2D, FREQADAPTIDX_1D,
FREQADAPTIDX_RL,
DECODE_NONRL,
DECODE_RL,
DECODE_ESC_RLRUN,
DECODE_ESC_RLLEVEL,
DECODE_ESC_RLSIGN,
MAX_VecDecodeStatus_VALUE = MAX_VOENUM_VALUE
} VecDecodeStatus;


//#define _WMA_COUNT_CPU_CYCLES_IF_REGKEY_
#ifdef _WMA_COUNT_CPU_CYCLES_IF_REGKEY_

typedef enum{
	CCS_ibstrmAttach,
	CCS_audecDecode,
	CCS_audecGetPCM,
	CCS_audecDecodeInfo,
	CCS_audecDecodeData,
	CCS_NUM_FUNS,
	MAX_CCSID_VALUE = MAX_VOENUM_VALUE
} CCSID;

typedef struct _CCSPerFunStruct {
	I64 iCurStartCycles;
	I64 iCurFrmCycles;
	I64 iTotalCycles;
	I64 iMaxFrmCycles;
	I32 iMaxPos;
	Bool fIn;
} CCSPerFunStruct;

typedef struct _CCSStruct {
	Bool fEnabled;
	Bool fDirty;
	I32 iFrmCount;
	I64 iMaxOverallFrmCycles;
	I32 iOverallMaxPos;
	WMAFormat Format;
	CCSPerFunStruct fns[CCS_NUM_FUNS];
} CCSStruct;

#endif

typedef struct WmaGetDataParam
{
	CBDATA  m_dwUser;
	Bool m_fTimeIsValid;
	I64  m_iTime;
} WmaGetDataParam;

typedef enum {
	RLCTBL_SM=0, 
	RLCTBL_SS,
	MAX_RlcTblType_VALUE=MAX_VOENUM_VALUE
} RlcTblType;

typedef struct DecRunLevelVecState
{
	VecDecodeStatus getState;
	LargeValDecodeStatus getLargeState;
	Int vecIdx;
	Int curRun;
	Int trip;
	Int vec[4];
	Int sgn[4];
	Int symbol;
	Int cVecsInNonRLMode;
	Int iBark;
	Int iCoeff;
	Int iEscVal4d;
	Int iEscVal2d;
	Int iEscVal1d;
	Int iEscValRL;
	Int iLastSwitchPt[4];
	Int nTbls[4];
	Int iTblIdx[4];
	Int iVecType;
	RlcTblType rlctt;
	const U16 *huffDecTbl4d;
	const U16 *huffDecTbl2d;
	const U16 *huffDecTbl1d;
	const U16 *huffDecTblRL;
	const U16 *smallHuffDecTblRL;
	const U16 *uSymToVec4;
	const U8  *uSymToVec2;
	const U16 *smallSymLeafRunLevelToRun;
	const U16 *smallSymLeafRunLevelToLevel;
} DecRunLevelVecState;
typedef enum {
	CONFIG_CURRFRM = 0, 
	CONFIG_NEXTFRM = 1,
	MAX_CFGFRAME_VAL = MAX_VOENUM_VALUE
} ConfigFrame;

typedef struct PerChannelInfoDEC
{
	// For V3 decoding.
	Bool               m_fVisited;
	Bool               m_fMaskHeaderDone;
} PerChannelInfoDEC;

#define BITDEPTH_ID(validBits, bytePerSample)  (((validBits - 1) << 2) | (bytePerSample - 1))

typedef enum {BITDEPTH_24   = BITDEPTH_ID(24, 3),
              BITDEPTH_2024 = BITDEPTH_ID(20, 3),
              BITDEPTH_16   = BITDEPTH_ID(16, 2),
			  MAX_BITDEPTH_VAL = MAX_VOENUM_VALUE
} BITDEPTH;

typedef enum {
	CODEC_NULL,
	CODEC_STEADY, 
	CODEC_DONE, 
	CODEC_BEGIN, 
	CODEC_LAST, 
	CODEC_ONHOLD, 
	CODEC_HEADER, 
	CODEC_DATA,
	MAX_CODEC_VAL = MAX_VOENUM_VALUE
} Status;

typedef enum {
	FMU_DONE = 0, 
	FMU_QUANTIZE, 
	FMU_PROCESS,
	MAX_FMU_STATUS = MAX_VOENUM_VALUE
} FMU_STATUS;

typedef enum {
	FFT_FORWARD = 0, 
	FFT_INVERSE,
	MAX_FFTDIRECTION = MAX_VOENUM_VALUE
} FftDirection;

typedef void (*PFNFFT) (Void *pFFTInfo, CoefType data[], Int nLog2np, FftDirection fftDirection);
//typedef void (*PFNFFT) (CoefType data[], Int nLog2np, FftDirection fftDirection);

typedef struct SubFrameConfigInfo
{ 
    I16  m_cSubFrame;
    I16  m_cMaskUpdate;                  //for enc only
    own I16* m_rgiSizeBuf; //include prev and next subfrm
    I16* m_rgiSubFrameSize;
    I16* m_rgiSubFrameStart;
    U8*  m_rgfMaskUpdate;
    U8   m_fMaskUpdateFromParser;
    U8*  m_bUnifiedLLM; //encoder only
    U8*  m_rgbCodingFex;// encoder only
} SubFrameConfigInfo;

typedef struct TileConfigInfo
{
    SubFrameConfigInfo *m_psfci;
    Int m_iSubFrame; // index of subframe withing SubFrameConigInfo
    Int m_iSubFrameSize;
    Int m_iSubFrameStart;
    Int m_cChannels;
    U32 m_uChannelMask;
    U8  m_bCodingFex;
    Int m_iCodingFexMaxFreq;
} TileConfigInfo;

#define m_cSubband m_cSubFrameSampleHalf //to be used as in freq. domain

/*****************************************************************************
    Lossless defines. Root of all lossless code.
*****************************************************************************/
//V3 LLM-B. Chao. Add. 03-20-02
// --- Pure Lossless Mode --- //

typedef struct MCLMSPredictor
{
    I32     m_iOrder;       //predictor order.
    I32     m_iOrder_X_CH;
    I32     m_iScaling; //scaling ratio between floating point and integer version of the filter. in power of 2.
    I32     m_iScalingOffset; //offset value before scaling.
    I32     m_cBitsSentPerCoeff;
    I32     m_iRecent;      //point to the last (most recent) value in PrevBuffer.
    own I32 * m_rgiPrevVal; //circular buffer of previous values used for predict the current value.
    own I16 * m_rgiFilter;  //filter.
    own I16 * m_rgiFilterBackup;  //filter.
    own I16 * m_rgiFilterCurrTime;  //filter used for prediction within the current PCM samples. 
                                        //For example, in stereo, we can use L to predict R.
    own I16 * m_rgiFilterCurrTimeBackup;
    own I16 * m_rgiUpdate;
    I16     m_iUpdStepSize; //stepsize of updating.
} MCLMSPredictor;

//V3 LLM-B. Chao. Add. 03-15-02
typedef struct LMSPredictor
{
    I32     m_iOrder;       //predictor order
//  I32     m_iOrder_Div_2;   //order divided by 2
//  I32     m_iOrder_Div_4;   //order divided by 4
    I32     m_iOrder_Div_8;   //order divided by 8
    I32     m_iOrder_Div_16;   //order divided by 16
    I32     m_cCoeffsSent;      //How many coefficients are sent in bitstream
    I32     m_cBitsSentPerCoeff; //If a coefficient is sent, how many bits are sent. >2. 
                                 //First 2 bits are used for sign (1) and integer part (1). 
                                 //Therefore the region is 01.11-10.00 if we use 4 bits.
    I32     m_iScaling; //scaling ratio between floating point and integer version of the filter. in power of 2.
    I32     m_iScalingOffset; //offset value before scaling.
    I16     m_iUpdStepSize; //stepsize of updating. will replace m_fltUpdLmt.
    I16     m_iUpdStepSize_X_8;
    I32     m_iRecent;      //point to the last (most recent) value in PrevBuffer.
    own I32 * m_rgiPrevVal; //circular buffer of previous values used for predict the current value.
#if 0
    own I32 * m_rgiPrevValBackup;
#endif
#if defined(WMA_OPT_FLOAT_PREDICTOR_XENON)
    own Float * m_rgiUpdate;
    own Float * m_rgiUpdateBackup;
    own Float * m_rgiFilter;  //filter.
    own Float * m_rgiFilterBackup; // Previous filter. This is also used for filter coefficients which are sent in bitstream.
#else
    own I16 * m_rgiUpdate;
    own I16 * m_rgiUpdateBackup;
    own I16 * m_rgiFilter;  //filter.
    own I16 * m_rgiFilterBackup; // Previous filter. This is also used for filter coefficients which are sent in bitstream.
#endif
} LMSPredictor;

#ifdef EXPT_CODE
//V3 LLMB. Chao. Add. 04-02-02
typedef struct RLSPredictor
{
    U16     m_iOrder;       //predictor order.
    Float   m_fltLamda;
    Float   m_fltDelta;

    I16     m_iRecent;      //point to the last (most recent) value in PrevBuffer.
    own Float * m_rgfltPrevVal; //circular buffer of previous values used for predict the current value.
    own Float * m_rgfltFilter;  //filter.
    own Float * m_rgfltP;   //iOrder X iOrder
    own Float * m_rgfltPp;   //iOrder X iOrder
    own Float * m_rgfltp;   //iOrder X 1
    own Float * m_rgfltK;   //iOrder X 1
} RLSPredictor;
#endif //EXPT_CODE

//typedef enum FexScaleFacPredType FexScaleFacPredType;
typedef enum FexScaleFacPredType
{
    FexScaleFacIntraPred = 0,
    FexScaleFacIntraPredNoRef	= 1,
    FexScaleFacInterPred = 2,
    FexScaleFacIntplPred = 3,
    FexScaleFacChPred    = 4,
    FexScaleFacReconPred = 5,
    FexScaleFacNo        = 6,
	MAX_FexScaleFacPredType		= MAX_VOENUM_VALUE
} FexScaleFacPredType;

typedef enum FexMvRangeType
{
    MvRangeBase = 0,
    MvRangePrevBand = 1,
    MvRangeFullNoOverwriteBase = 2,
    MvRangeFullOverwriteBase = 3,
	MAX_FexMvRangeType = MAX_VOENUM_VALUE
} FexMvRangeType;

// currently only needed for cx, but good structure to have to store per
// channel elements which should not be separate for encoder buckets
// don't put ifdef around elements here, otherwise we end up with 0 element
// structure for non BUILD_WMAPROLBRV2 builds (which may be bad??)
typedef struct _PerChannelInfoShr
{
    // leave at least one element in structure, otherwise 0 element structures
    // are sometimes not liked
#if defined(BUILD_WMAPROLBRV1)
    // chex
    CoefType *m_pDCTBufCurr, *m_pDCTBufNext;
    CoefType *m_pIDCTBufPrev, *m_pIDCTBufCurr, *m_pIDCTBufNext;

    // fex
    Bool m_bFxUseExponent;
    Bool m_bFxUseNoiseFloor;
    Bool m_bFxUseSign;
    Bool m_bFxUseReverse;
    Bool m_bFxUseRandomMv2;
    Int  m_iFxNoiseFloorThresh;

    FexMvRangeType m_eFxMvRangeType;
    Int m_iFxMvResType;

    U32 m_uFxChannelDependencyMask;
    U32 m_uFxChannelGroupMask;

    Bool m_bFxUseImplicitStartPos;

    Bool m_bFxUseUniformBands;

    Bool m_bFxArbitraryBandConfig;
    Int  m_iFxChannelDependencyGroup;

    Int m_iFxScaleBands;
    Int m_iFxMvBands;
    Int m_iFxBandConfig;
#endif

    CoefType m_fFxSplitTileSaveCoeff;

} PerChannelInfoShr;

typedef struct PerChannelInfo 
{
    //whenever the memory gest moved to buckets, update is needed
    // Decoder and Global Members
    I32*           m_rgiCoefQ;                      //quantized coef
    Int*           m_rgiMaskQ;                      //quantized maskq ((could be I16))
    Int*           m_rgiMaskQResampled;             // V3. Anchor mask resampled for current subframe size.

    U8*            m_rgbBandNotCoded;               //band not coded
    FastFloat*     m_rgffltSqrtBWRatio;             //MidRate/LowRate for Bands not coded
    Int*           m_rgiNoisePower;
    const U32*     m_rgiHuffDecTbl;                 //decoder only
#if defined (WMA_OPT_VLC_DEC_ARM) && WMA_OPT_VLC_DEC_ARM
    const U16*     m_rgiHuffDecTbl_Pre;             //table for decoding whole symbol, if possible
#endif
    const U16*     m_rgiRunEntry;                   //decoder only        
    const U16*     m_rgiLevelEntry;                 //decoder only
    Int            m_cSubbandActual;
    Int            m_iPower;                        //channel power
    Int            m_iPowerPreQ;                    //channel power before quantization.
    Int            m_iPowerSave;
    Int            m_iPowerBase;                    //baselayer's final power. Saved here because plus may update m_iPower.
    Int            m_iPowerBaseForBasePlusExclusveEnc;  //baselayer's power before baseplus exclusive encoding. Baseplus exclusive
                                                        //use this to decide whether to code a channel or not so:
                                                        // 1) consistent with base
                                                        // 2) consistent with other plus
    Int            m_iPowerBaseplus;                //baseplus layer's power.
    // V3 LLM
    Int            m_iPowerOri;                     //original channel power.

    Int            m_iActualPower;                  // Power as suggested by CoefQ/CoefRecon
    UInt*          m_rguiWeightFactor;              //weighting factor
    CBT*           m_rgiCoefRecon;                  //reconstructed coef
    CBT*           m_rgiPCMBuffer;                  // PCM output buffer pointer
    Int            m_iMaxMaskQ; 
    StereoMode     m_stereoMode;                    // would be per bucket per channel

#define CURRGETPCM_INVALID  0x7FFF

    //wchen: these were defined as V4V5_COMPARE, retired
    //wchen: we may not need these to the lookup table case
    BP2Type        m_fiSinRampUpStart;
    BP2Type        m_fiCosRampUpStart;
    BP2Type        m_fiSinRampUpPrior;
    BP2Type        m_fiCosRampUpPrior;
    BP2Type        m_fiSinRampUpStep;
    BP2Type        m_fiSinRampDownStart;
    BP2Type        m_fiCosRampDownStart;
    BP2Type        m_fiSinRampDownPrior;
    BP2Type        m_fiCosRampDownPrior;
    BP2Type        m_fiSinRampDownStep;

    I16            m_iCurrGetPCM_SubFrame;          // reconstruction index
    I16            m_iCurrSubFrame;                    // for tiling
    I16            m_iCurrCoefPosition;   // current subframe starting point    
    I16            m_cSubFrameSampleHalf;
    I16            m_cSubbandAdjusted;
    I16            m_iSizePrev;
    I16            m_iSizeCurr;
    I16            m_iSizeNext;
    I16            m_iCoefRecurQ1;
    I16            m_iCoefRecurQ2;
    I16            m_iCoefRecurQ3;
    I16            m_iCoefRecurQ4;
    
  //I16     m_cSubband;
  //Int     m_cSubbandAdjusted;
    Int     m_cSubFrameSampleAdjusted;
    Int     m_cSubFrameSampleHalfAdjusted;

    // Float-only members
    // The following is NOT YET INTEGERIZED but after it is,
    // it should be moved into the #ifndef BUILD_INTEGER above
    CoefType*      m_rgCoefRecon;                //reconstructed coef soon to be removed

    CoefType *m_rgCoefReconBase;

    // The following are NOT YET INTEGERIZED but after they are,
    // they should be moved into the #ifndef BUILD_INTEGER above

    Float*  m_rgfltWeightFactor;
    WtType* m_rgWeightFactor;

    FinalWeightType m_wtMaxWeight;

    Int            m_cLeftShiftBitsQuant;
    Int            m_cLeftShiftBitsTotal;
    QuantFloat     m_qfltMaxQuantStep;

    // Pointer to encoder data, if applicable
    PerChannelInfoENC*  ppcinfoENC;

    // This is encoder-only but I've put it here to avoid having to #include "msaudioenc.h"
    // in common files.
    Bool*            m_rgfMaskNeededForBark;   // for header squeezing

    // V3 stuff
    U8               m_ucQuantStepModifierIndex;         // Quality balancing
    I8               m_iPaddedZeros;                  //Sometimes the lowest n bits of PCM samples are all zeros. 
	
    //V3 LLM-B. Chao. Add. 03-15-02
    I16              m_cLMSPredictors;
    I16              m_iLMSPredictorCurr;

    // we suppose 24 bits memory len < 256
    I16              m_iColombK;
    U32              m_iSum;
    U32              m_iSumBackup;

    QuantStepType    m_qstQuantStepModifier;             // Quality balancing
    LMSPredictor     m_rgLMSPredictors[4];  // LLMB_CLMSFLT_TTL_MAX

    SubFrameConfigInfo* m_rgsubfrmconfig;             // Use CONFIG_CURRFRM  & CONFIG_NEXTFRM for indexing.
    Int              m_cSubFrameSampleHalfWithUpdate;    // Values known to be available at the decoder (V3)
    Int              m_cValidBarkBandLatestUpdate;       // Values known to be available at the decoder (V3)
    Int              m_iMaskQuantMultiplier;             // Scales MASK_QUANT for better compression
    Int              m_iMaskQuantMultiplierFromParser;
    Bool             m_fAnchorMaskAvailable;

    // V3 unseekable subframe
    // ppcinfo and ppcinfoENC usually appear as pair. In encoder, there are several copies of ppcinfo/ppcinfoENC. Each 
    // frame bucket has one pair and pau has one common pair. In encoder although we malloc the common pair 
    // as pau->rgpcinfo pauenc->rgpcinfoENC, we don't actually use them to access the common pair. pau->rgpcinfo/ENC 
    // is saved as pauenc->rgpcinfo/ENC/Com in prvInitEncoderSecondaryAllocated and pau->rgpcinfo/ENC is used to access the current bucket's pair.
    // In decoder, this is only one pair (common pair).
    // The common pair is malloced and initialized to zero in prvInitCommonAllocate.
    // Next two arrays are only valid in common pair.

    Bool            m_bUpdateSpeedUp; // 0: regular, 1: fast.
    I32             m_iUpdSpdUpSamples; 
    I32             m_iTransientPosition;
    //TODO Is U32 enough
    U32             m_iShrtAve;     // need to be reset at seekable frame
    U32             m_iLongAve;     // need to be reset at seekable frame
    I32             m_iLastSamples[LLM_VERB_ACFILTER_ORDER_MAX];  // need to be reset at seekable frame

    /*  moved to pau
    I32             m_iACFilter[LLM_VERB_ACFILTER_ORDER_MAX]; // AC filter.
    I32             m_iACFilterOrder;
    I32             m_iACFilterCurr; // for decoder.
    */

#ifdef USE_SIN_COS_TABLES
    //Tables for reconstruction sin values
    BP2Type *m_piSinForRecon;    
#endif

    Int m_iVecsInNonRLMode;

    // Needed by auGetPCM
    U16 m_cSamplesFromHistory;          
    U16 m_cLastCodedIndex;  // To reduce decoder complexity in V3 & use in fex
    // CTODO: reconfex reuses this variable for recon transforms. So we need to save and restore m_cLastCodedIndex. Eventually
    // reconfex should have it own lastcodedindex variable.
    U16 m_cLastCodedIndexSave; 
    U32 m_cSamplesLeft;
	
	PerChannelInfoShr *m_ppcinfoShr;
    // We can save some memory if we move some of following arrays to pau or pauenc then use pointer to access them.
    // Some of them are not necessary to be per FrmBucket.
    // V3 lossless Mode LPC. first half and second half.
    // currently since there is no subframe support in PLLM, this works well. But later if we (we should) use
    // subframe or tile structure, following definition should be moved to SubFrameConfigInfo. Chao.
    Double           m_rgrgdblLLMLPCFilter[2][LLM_LPC_ORDER];  // should be moved to PerChannelInfo.
    Float            m_rgrgfltLLMLPCFilterRec[2][LLM_LPC_ORDER]; // reconstructed LPC filter used on both encoder/decoder.
    // Residue should go to PerChannelInfoENC
    I32              m_rgiLPCFilter[LLM_LPC_ORDER];
    I32              m_rgiLPCPrevSamples[LLM_LPC_ORDER];

#if defined(BUILD_WMAPROLBRV1)
    // fex variables
    FastFloat *m_pffltFexScaleFac;
    FastFloat *m_pffltFexScaleFacNoise;
    FastFloat *m_pffltFexScaleFacBase;
    I32       *m_piFexLogScaleFacIndex;
    I32       *m_piFexLogScaleFacBaseIndex;
    I32       *m_piFexLogScaleFacIndexRef;
    I32       *m_piFexMv;
    I32       *m_piFexMv2;
    I32       *m_piFexMvOffsetNoiseCB;
    I32       *m_piFexCb;
    FexType   *m_pFexExp;
    I32       *m_piFexSign;
    Bool      *m_pbFexRev;
    I32       *m_pcFexSubframesMvMerged;
    FastFloat *m_pffBandScale;
    FexScaleFacPredType *m_pFexScaleFacPred;
    FexScaleFacPredType *m_pFexScaleFacBasePred;


    // chex variables
    __ecount(RECONPROC_HIST_SIZE*pau->m_cChannel) CoefType* m_rgCoefHistMCLT;
    Bool         m_fHistIsMCLT;
    CoefType *m_rgCoefSampMCLT;
    CoefType *m_rgCoefSampMCLTCurr;

    CoefType *m_rgPCMBuf;

#endif // BUILD_WMAPROLBRV1

#ifdef BUILD_WMAPROLBRV3
    I16 *m_piBasePeakCoefPos;
    Int *m_pcBasePeakCoefs;
    Int *m_piBasePeakMaskQSave;
    Int m_iBasePeakBarkStart;
    Int m_iBasePeakMaskDelta;
    Int m_iBasePeakShapeCB;
#endif

#ifdef WMA_TARGET_ARM
#if ARM_FAST_CONSTANT	
    U8 m_nPadding[8];  //Extra padding for ARM 8 bit immediate operands requires 
                       //Need to re-adjust the padding size if add or remove member(s)
#endif //ARM_FAST_CONSTANT
#endif //WMA_TARGET_ARM

#if defined (DEBUG_BIT_DISTRIBUTION)
    I32              m_rgcBitsPerBand [NUM_BARK_BAND];
#endif // DEBUG_BIT_DISTRIBUTION

    // BasePlus
#if defined (BUILD_WMAPROLBRV2)
    I32*      m_rgiSFQ; // Quantized scale factor values
    I32*      m_rgiSFQResampled; // from previous subframes
    I32       m_iMaxSFQ;
    I32*      m_rgiBPWeightFactor;
    CoefType* m_rgBPCoef;
    I16      *m_rgiBadCoefSegPos;
    I16       m_cBadCoefSegs;
    Int       m_cBPBadCoef;
    Int       m_cBPSF4BadCoef;
    FinalWeightType m_wtMaxSFWeight;
    Int       m_iBPChannelQuant;
    Bool      m_fAnchorSFAvailable;
    Bool      m_fFirstChannelInGrp;
    Int       m_iFirstChannelInGrp;
    Int       m_cSubFrameSampleHalfWithSFUpdate;
    Int       m_cValidSFBandLatestUpdate;
    SFPredictionType m_sfPredictionType;
    Bool      m_fIntlvSF;
    Int       m_iSFQHuffmanTbl;

    // interleave mode.
    Int       m_iPeriod;
    Int       m_iPeriodFraction;
    Int       m_iFirstItlvPeriod;
    Int       m_iLastItlvPeriod;
    Int       m_iPreroll;
    
    // predictive mode.
    Int       m_rgiQuantizedCoefPredictor[BPLUS_QCOEF_LPCSEGMT_MAX][BPLUS_QCOEF_LPCORDER_MAX];
    Int       m_iQCoefLPCOrder;
    Int       m_iQCoefLPCShift;
    Int       m_iQCoefLPCSegmt;
    Int       m_iQCoefLPCMasks;   

    // periodic mode
    Int       m_iPeriod4PDFShift;
    Int       m_iInsertPos;

    I32       m_iMaxWeightFactor;
    I32      *m_rgiBPBadCoef;  
	U16       m_cLastCodedIndexBasePlus;  // last coded coef in base plus layer.
#endif // BUILD_WMAPROLBRV2

    I16 m_cSubFrameSampleHalfSave;

    Int m_iStartPtInFile;

    Bool m_bFex;
    Bool m_bNoDecodeForCx;

#ifdef OVERLAPADD_LOOKUPTBL
    BP2Type *m_overlapAddTable;
    I16  m_overlapAddSize;
#endif

#ifdef WMA_TARGET_ARM
//#if ARM_FAST_CONSTANT
#ifdef BUILD_WMAPROLBRV3
	U8 m_nPaddingEnd[4];  //Extra padding for ARM 8 bit immediate operands requires 
#else
 //   U8 m_nPaddingEnd[4];  //Extra padding for ARM 8 bit immediate operands requires 
                       //Need to re-adjust the padding size if add or remove member(s)
#endif
//#endif //ARM_FAST_CONSTANT
#ifdef _IOS
	/*struct size is 8-bit align on linux, however,on iOS padding bits must be added, 
	so the actural size is equal to PerChannelInfo_size defined in wma_member_arm.inc.
	*/
	U8 m_nPaddingStruct[4];   
#endif
#endif //WMA_TARGET_ARM
//	int	pad[4];//1, pad2, pad3;	//pad 8 bytes to 0xa10, delete them if adding parameter.
} PerChannelInfo;

typedef struct _ReconTileInfo ReconTileInfo;

//#define ALLOW_EXTERNAL_FILE_FOR_RECON

//#define CXTEST_NOQUANT

#ifdef ALLOW_EXTERNAL_FILE_FOR_RECON
extern Bool g_bReplaceForFex;
extern Bool g_bReplaceForChex;
extern Bool g_bReplaceCFx;
//extern TCHAR g_szReplaceFile[256];
extern Bool g_bVerify;
extern Int g_iNTilesPerFrame;
extern Int g_iCodingMethod;
extern Int g_iCodeAll;
extern Int g_iPowerForAllBands;
extern Int g_iPreservePower;
extern Int g_iPredScale;
extern Int g_iPredPhase;
extern Int g_iScBins;
extern Int g_iPhBins;
extern Int g_iPowerBins;
extern Int g_iScPredBins;
extern Int g_iPhPredBins;
extern Int g_iPowerPredBins;
#if 0
Void prvReplaceSamplesFromFile(CAudioObjectDecoder *paudec, CAudioObject *pau, Float fStart, Float fEnd);
#endif
Void prvReplaceSamplesExtBufCh(CAudioObject *pau,
                               PerChannelInfo *ppcinfo,
                               Int iStart, Int iEnd);
Void prvReplaceSamplesFromFileReconProc(CAudioObjectDecoder *paudec, 
										CAudioObject *pau,
                                        ReconTileInfo *prti,
                                        Int iChCode, CoefType *pDest,
                                        Float fStart, Float fEnd);

Void initReplArgs();

#define REPL_PARAM(y, x) { if ((x)>=0) (y) = (x); }

#endif // ALLOW_EXTERNAL_FILE_FOR_RECON

typedef enum {
	LPC_MODE=0,
	BARK_MODE,
	MAX_WEIGHTMODE = MAX_VOENUM_VALUE
} WeightMode;

typedef struct CChannelGroupInfo {
    Int   m_cChannelsInGrp;           // Number of channels in the group
    own Bool* m_rgfChannelMask;
    Bool  m_fIsSuperGroupXform;       // Is this xform top level in hierarchy?
    Bool  m_fIsPredefinedXform;
    PredefinedXformType  m_predefinedXformType;
    Bool  m_fAllBarksOn;
    Bool  m_rgfXformOn[NUM_BARK_BAND];
    I8*   m_rgbRotationAngle;         // Quantized index
    I8*   m_rgbRotationSign;          // 0 or 1

    own ChXFormType *m_rgfltMultiXForward;
    own ChXFormType *m_rgfltMultiXInverse;
} CChannelGroupInfo;

// Forward data type decl
//typedef struct CAudioObject CAudioObject;

#if defined(BUILD_WMAPROLBRV1)

typedef struct _HuffInfo
{

    Int m_iBins;
    Int m_iBits;
    Int m_iMinSym;
    Int m_iMaxSym;
    Int m_iMinSymBits;
    Int m_iMaxSymBits;

    Int m_iExtraBits;

    Int m_iIndexMask;
    Int m_iResMask;

    const U32 *m_uEncTable;
    const U16 *m_uDecTable;

} HuffInfo;

typedef enum FexGlobalParamType
{
    FexCodingGlobalParamFrame     = 0,
    FexReconGlobalParamFrame      = 1,
    FexCodingGlobalParamFramePrev = 2,
    FexReconGlobalParamFramePrev  = 3,
	MAX_FexGlobalParamType = MAX_VOENUM_VALUE
} FexGlobalParamType;

#ifdef BUILD_WMAPROLBRV3
typedef enum
{
    FexGlobalParamUpdateNo       = 0,
    FexGlobalParamUpdateFull     = 1,
    FexGlobalParamUpdateList     = 2,
    FexGlobalParamUpdateTileList = 3,
    FexGlobalParamUpdateFrame    = 4,
    FexGlobalParamUpdateTile     = 5,
	MAX_FexGlobalParamUpdateType = MAX_VOENUM_VALUE
} FexGlobalParamUpdateType;

typedef enum
{
    BasePeakCoefNo              = 0,
    BasePeakCoefInd             = 1,
    BasePeakCoefInterPred       = 2,
    BasePeakCoefInterPredAndInd = 3,
	MAX_BasePeakCoefPredType	= MAX_VOENUM_VALUE
} BasePeakCoefPredType;
#endif

typedef struct FexMvCodebook
{
    Bool    m_bPredMv;
    Bool    m_bPredExp;
    Bool    m_bPredSign;
    Bool    m_bPredRev;
    Bool    m_bPredNoiseFloor;

    Bool    m_bNoiseMv;
    Bool    m_bNoiseExp;
    Bool    m_bNoiseSign;
    Bool    m_bNoiseRev;

    Int   m_cBitsWithNoiseFloor;
    Int   m_iIndexWithNoiseFloor;
    Int   m_cBitsWithoutNoiseFloor;
    Int   m_iIndexWithoutNoiseFloor;
} FexMvCodebook;

typedef struct FexGlobalParam
{
    // GroupA
    Bool    m_bScaleBandSplitV2;
    Bool    m_bUseUniformScaleBands;
    Bool    m_bArbitraryScaleBandConfig;
    Bool    m_bNoArbitraryUniformConfig;
    Int     m_cScaleBands;
    Int     m_cMvBands;
    Int     m_iMinRatioBandSizeM;
    Int*    m_piBandSizeM;
    // GroupB
    Bool    m_bBaseBandSplitV2;
    Int     m_cBaseBands;
    Int     m_iBaseFacStepSize;
    Bool    m_bOverlay;
    Int     m_iMinFreq;
    Int     m_iMaxBaseFreq;
    Bool    m_bUseImplicitStartPos;
    Int     m_cMinRunOfZerosForOverlayIndex;
    // GroupC
    Int     m_iScFacStepSize;
    Int     m_iMvBinsIndex;
    Int     m_iScBinsIndex;
    Int     m_iMvCodebook;
    Bool    m_bEnableNoiseFloor;
    Bool    m_bEnableExponent;
    Bool    m_bEnableSign;
    Bool    m_bEnableReverse;
    // GroupD
    Bool    m_bRecursiveCwGeneration;
    Int     m_iKHzRecursiveCwWidth;
    FexMvRangeType  m_iMvRangeType;
    Int     m_iMvResType;
    Bool    m_bEnableV1Compatible;
    Int     m_iMvCodebookSet;
    Bool    m_bUseRandomNoise;
    Int     m_iNoiseFloorThresh;
    Int     m_iMaxFreq;

#ifdef BUILD_WMAPROLBRV3
    Bool    m_bUseSingleMv;
    Bool    m_bUseCb4;
    Int     m_cMaxRunOfZerosPerBandForOverlayIndex;
    Int     m_iEndHoleFillConditionIndex;
    U32     m_uUpdateListFrame;
    U32     m_uUpdateListTile;
#endif
} FexGlobalParam;

typedef struct _QuantFlt
{

    ChexType m_fMin;
    ChexType m_fMax;
    ChexType m_fMinPred;

    ChexType m_fStep;
    BoundInt m_iBins;
    Int m_iBits;

    ChexType m_fStepPred;
    Int m_iBinsPred;
    Int m_iBitsPred;

    ChexType m_fMinStepHalf;
    Int m_iMinStepHalfBins;
    Int m_iStep;
    Int m_iStepPred;

    // optional, associate a huffenc/huffdec with a quant float
    HuffInfo *m_phi;
    HuffInfo *m_phiPredT;
    HuffInfo *m_phiPredF;

    Bool m_bRndSign;

    Int m_iMin;
    Int m_iMax;

} QuantFlt;

typedef struct Fex
{
    struct CAudioObject *m_pau;

#ifndef BUILD_INTEGER
    Float m_fltScaleQuantStep;
    Float m_fltScaleQuantStepPred;
#endif

    I32 m_iFirstChLogScaleIndex;
    I32 m_iFirstChLogScaleBaseIndex;
    I32 m_iPrevLogScaleIndex;
    I32 m_iMvThreshold;

    Int m_cScaleDefaultBins;
    Int m_cScaleBins;
    Int m_cScaleBinsPred;
    Int m_cScaleBits;
    Int m_cScaleBitsPred;
    Int m_iScBinsMultiplier;
    Int m_iPrevScBinsMultiplier;

    Int m_cMvDefaultBins;
    Int m_cMvBins;
    Int m_cMvBinsPred;
    Int m_iMvBinsMultiplier;
    Int m_iPrevMvBinsMultiplier;

    Int m_cStartPosBins;
    Int m_cEndPosBins;

    Bool m_bEntireHoleOverlay;
    Int m_iHoleBandStart;
    Int m_iHoleBandSize;
    Int m_iCurrHoleSearchPos;
    Bool m_bHoleBand;
    Int m_cHoleBands;
    Int *m_rgiBandStart;

    Int *m_rgiScaleBandSizes;
    Int *m_piScaleBandSizes;
    Int m_cScaleBands;

    Int *m_rgiMvBandSizes;
    Int *m_piMvBandSizes;
    Int m_cMvBands;

    Int *m_rgiBaseBandSizes;
    Int *m_piBaseBandSizes;
    Int m_cBaseBands;
    Int m_cBaseBandsGlobal;
    Bool m_bBaseScaleOnly;
    Bool m_bShrinkBaseBand;

    Int m_cBandsBits;
    Int m_cMvBits;
    Int m_cStartPosBits;
    Int m_cEndPosBits;

    Int *m_rgiChCode;

    Int m_cStart;
    Int m_cEnd;
    BoundInt m_cOrigStart;
    Int m_cOrigEnd;
    Int m_cBaseEnd;
    Int m_cStartHoleFill;
    Int m_cEndHoleFill;

    Int m_iPrevFrame;
    Int m_iPrevTile;
    CoefType *m_pfLowband;
    BoundInt m_cBandSize;
    Int m_cStartIndex;
    Int m_iBand;
    Bool m_bUseScalePred;
#ifdef BUILD_INTEGER
    Int m_iBitScale;   // stored as # of shift-bits
#else
    Float m_fBitScale; // stored as float scale factor
#endif
    Int m_iMinStartPos;
    Int m_iMaxStartPos;
    Int m_cScaleHuffIndexMask;
    Int m_cScaleIndexExtraBits;
    Int m_cScaleResIndexMask;
    Int *m_rgiBandSizeM; // band size multiplier (arbitrary)
    Int *m_piBandSizeM;
    Int *m_rgiExpBandSizeM;
    Int m_iExpBandSizeM;
    UInt m_uiExpBandSizeMIndex;
    Int m_iMinRatioBandSizeM;

    QuantFlt m_qfMCLTPhase;

    Int *m_rgcScaleBandsPerMvBand;
    const Int *m_piFexBandSizesM;
    const Int *m_piBaseBandSizesM;
    Bool m_bScaleBandSplitV2;
    Bool m_bBaseBandSplitV2;

    BoundInt m_cMaxBands;
    Int *m_rgiMaxBandsTemp;

    CoefType *m_rgfReconChannel;

    Bool m_bHasPower;

    Int m_iMinFreq;
    Int m_iMaxFreq;
    Int m_iMaxBaseFreq;
    Int m_iMaxLastCodedIndex;
    Int m_iMaxLastCodedIndexOrig;
    Int m_iExplicitStartPos;

    Bool m_bUseSingleMv;
    Bool m_bMergeNoiseVecAdj;
    Bool m_bMergeNoiseVecAll;
    Bool m_bUseRandomNoise;

    Bool m_bNoiseMvDecoded;
    Int m_cMergeBandSize;

    Int m_iMinBandSize;
    Int m_iMaxBandSize;

    Int m_iScaleBand;
    Int m_iCoeffScaleFacBand;

    Int m_cTileSize;
    Int m_cSubFrameSampleHalf;
    Int m_iSubframeStartInFrame;
    Int m_cSubtiles;

    Bool m_bSplitTileIntoSubtiles;
    CoefType *m_pfRecon;
    CoefType *m_pfBaseRfx;

    Bool m_bNoiseFloorParamsCoded;
    Bool m_bMvRangeFull;
    Bool m_bUseMvPredLowband;
    Bool m_bUseMvPredNoise;
    Bool m_bOverwriteBase;
    Bool m_bMvResTypeCoded;

    Int m_iChCode;
    Int m_iChSrc;
    Int m_nChCode;
    Int m_iChCfg;

    U8 *m_rgucCwMask;

    Int m_iNumMvChannels;

    Bool m_bFirstTile;
    Bool m_bFirstTileV2Recon;
    Bool m_bFirstTileV2Coding;

    Int m_iPredMvLowband;
    Int m_iPredMvNoise;
    Int m_iPredMvNoiseFloor;
    Int m_iPredMvIndex;
    Int m_iPredMvLeftClose;
    Int m_iPredMvRightClose;
    Int m_iPredMvClose;
    Int m_iPredMvCloseBits;
    Int m_iPredMvRemVals;
    Int m_iPredMvRemValsBits;
    Int m_iPredMvRemShortVals;

    Int m_iMergeMv;
    Int m_iMergeMv2;
    Int m_iMergeBand;

    Int m_cMinRunOfZerosForOverlay;
    Int m_cMinRunOfZerosForOverlayIndex;
    Bool m_bMinRunOfZerosForOverlayCoded;
    Int m_bIsValidForOverlay;
    Int m_cMaxRunOfZerosPerBandForOverlay;
    Int m_cMaxRunOfZerosPerBandForOverlayIndex;
    Int m_iEndHoleFillConditionIndex;

    Bool *m_rgbMvCoded;
    Bool *m_pbMvCoded;
    Bool m_bNeedMvCoding;

    Int m_iIteration;

    Bool m_bChannelIndependent;

    Bool m_bFirstScaleFacCh;
    Bool m_bFirstScaleFacBand;

    Int m_cRunOfZeros;

    Bool m_bOverlay;
    Bool m_bOverlayCoded;
    Bool m_bOverlayOnly;
    Int m_cScaleBandsOverlayOnly;
    Int m_cMvBandsOverlayOnly;

    Bool m_bFirstLowbandAfterMaxCodedIndex;
    Bool m_bOverlayUsedForBand;

    Int m_iVersion;
    Bool  m_bV1Compatible;
    Bool  m_bReconDomain;
    Bool  m_bRecursiveCwGeneration;
    Int m_iKHzRecursiveCwWidth;

    FexGlobalParam *m_rgGlobalParam;
    Int *m_rgiBandSizeMGlobalParam;

    Int m_cScaleBitsV2;
    FexScaleFacPredType  m_iScaleFacPred;
    FexScaleFacPredType  m_iBaseScaleFacPred;

    FastFloat *m_pffltFexScaleFacPeaks;
    FastFloat *m_pffltFexScaleFacNoise;

    FastFloat **m_rgrgpfLogScaleFac;
    FastFloat **m_rgrgpfLogScaleFacBase;
    Int ***m_rgrgpiLogScaleFacIndex;
    Int ***m_rgrgpiLogScaleFacBaseIndex;
    Int **m_rgpiLogScaleFacRef;
    FexScaleFacPredType **m_rgpScFacPredType;
    FexScaleFacPredType **m_rgpScFacBasePredType;
    Int  **m_rgrgiScFacResidualIndex; // these two are common buffer for encoder/decoder
    UInt *m_rguScFacSignIndex;     // (never used at the same time)
    Int  *m_rgiScFacFirstNoRef;    // absolutely coded scale fac
    Int  *m_rgiScFacResidualBufOffset;// offset for residual buffer
    Int  *m_rgcScFacBaseBands;
    Int  m_iTilesInSubframeProcFx;
    Int  m_iTilesInSubframeProcBs;
    Bool  m_bScFacIntraPredNoRef;
    Bool  m_bScFacChPred;
    Int   m_iScFacChPredOffset;

    ReconTileInfo *m_prtiCurr;      // current processed recon tile 
    ReconTileInfo **m_rgprtiMvAnchor;  // the last anchor recon tile for mv

    Bool m_bUseMvPredPrev;

    Bool m_bUseObtainedBandScale;
    Bool m_bAdjustScaleForDelayedExp;
    FastFloat m_fObtainedBandScale;

    Bool m_bUsePhase;

    Bool m_bIncludeCrossTerms;

    BoundInt m_iAllocCh;

    U32 m_uChGroupProcessed;
    Int m_cMinRunOfZerosForOverlayProcessed;    // record MinRun for the currently processed mask

    WMARESULT m_wmaResult; // to detect errors related to Fex functions

    Bool m_bRFxTilesProc;

} Fex;

// ==================================================================
// CHEX

//#define CHEX_VERIFY

typedef struct _PerChannelRecon
{
    // recon fex params

    // base scaling - 2 params per band
    FastFloat *m_rgfFexScaleFacBase;
    Int *m_rgiFexLogScaleFacBaseIndex;
    FexScaleFacPredType m_iFexScaleFacBasePred;

    // Scale factors - 4 params per band
    FastFloat *m_rgfFexScaleFac;
    FastFloat *m_rgfFexScaleFacNoise;
    Int *m_rgiFexLogScaleFacIndex;
    Int m_iFexLogScaleFacIndexRef;
    FexScaleFacPredType m_iFexScaleFacPred;

    // mv - 7 params per band
    Int *m_rgiFexCb;
    Int *m_rgiFexMv;
    Int *m_rgiFexMv2;
    FexType *m_rgFexExp;
    Int *m_rgiFexSign;
    Bool *m_rgbFexRev;

} PerChannelRecon;

typedef enum _CxPredType
{
    CxNoPred = 0,
    CxPredF,
    CxPredT,
	MAX_CxPredType = MAX_VOENUM_VALUE
} CxPredType;

#define NUM_CXMATELEMS (12)

typedef enum _CxScale
{
    e_CxScMonoRealL = 0,
    e_CxScMonoImagL,
    e_CxScDiffRealL,
    e_CxScDiffImagL,
    e_CxScMonoRealR,
    e_CxScMonoImagR,
    e_CxScDiffRealR,
    e_CxScDiffImagR,
    e_CxScDiffMonoL,
    e_CxScDiffMonoR,
    e_CxScMonoAbsLSq,
    e_CxScMonoAbsRSq,
	MAX_CxScale = MAX_VOENUM_VALUE
} CxScale;


#define NUM_CXAUTOADJUSTVALS (3)

typedef enum _ChexAutoAdjust
{
    ChexAdjustScaleExact = 0,
    ChexAdjustScaleApprox,
    ChexAutoAdjustPerTile,
	MAX_ChexAutoAdjust = MAX_VOENUM_VALUE
} ChexAutoAdjust;

#define NUM_CXCHCODINGVALS (4)

typedef enum _ChexChCoding
{
    ChexMono = 0, // all bands mono
    ChexDiff, // all bands diff
    ChexChCodingPerBand, // decide per band
    ChexChCodingPerTile, // decide per tile
	MAX_ChexChCoding = MAX_VOENUM_VALUE
} ChexChCoding;

#define NUM_CXFILTEROUTPUTVALS (5)

typedef enum _ChexFilterOutput
{
    ChexFilterOutput0 = 0, // just a delay
    ChexFilterOutput1, // first delay
    ChexFilterOutput2,
    ChexFilterOutput3,
    ChexFilterOutputPerTile, // decide per tile
	MAX_ChexFilterOutput = MAX_VOENUM_VALUE
} ChexFilterOutput;

// Cx specific anchor tile information
typedef struct _CxAnchorReconTileInfo
{
    // General params
    Int m_iRefCount;

    // Parameters to go with segment -- access with m_pcxartiSeg
    ChexAutoAdjust   m_eAutoAdjustScale;
    ChexChCoding     m_eCxChCoding;
    ChexFilterOutput m_eFilterTapOutput;

    Int m_iNumBandIndex;
    Int m_iBandMultIndex; // specifies band configuration
    Int m_cBands;
    Int m_iStartBand; // band to start cx on

    Int *m_iCodeMono;

    // not needed so much for interpolation, as for figuring out
    // start and end of segment so that sum/diff decision can be correctly made
    // even though this is per segment, use from coding since it is used
    // to calculate interpolation ratio
    Bool m_bEvenLengthCxSegment;

    // Parameters to go with coding -- access with m_pcxartiCode
    own ChexType **m_param0;
    own ChexType **m_param1;
    own ChexType **m_param2;

    ChexType *m_lScNorm;
    ChexType *m_lrScNorm;
    ChexType *m_lrScAng;

    ChexType *m_lmSc;
    ChexType *m_rmSc;

    CxPredType m_predTypeLScale;
    CxPredType m_predTypeLRScale;
    CxPredType m_predTypeLRAng;

    CxPredType m_predTypeLMScale;
    CxPredType m_predTypeRMScale;

    Bool m_bCxInterpolate;

    Bool m_bCodeLMRM;

} CxAnchorReconTileInfo;

typedef struct _RFxReconTileInfo
{

    Int m_cFexScaleBands;
    Int m_cFexMvBands;
    Int m_cFexBaseBands;
    Int m_cFexStart;
    Int m_cFexStartIndex;
    Int m_iMaxLastCodedIndex;

    Int *m_rgiFexScaleBandSizes;
    Int *m_rgiFexMvBandSizes;
    Int *m_rgiFexBaseBandSizes;

    Bool m_bLastTileInFrame;

    // Other per channel recon params (currently for rfex), just pointers
    own PerChannelRecon *m_rgpcr;

#if defined(_DEBUG)
    ReconTileInfo *m_prti; // pointer back to recon tile info
#endif

} RFxReconTileInfo;

// structure to store subframe info needed to encode/decode
struct _ReconTileInfo
{
    // Common Info
    Int m_iStartPt; // overall starting point
    Int m_iStartPtInFrame; // starting point within current frame
    Int m_iTileCnt; // tile counter
    Int m_iStart, m_iEnd; // valid start/end points between 0 & iSizeCurr
#if defined(ALLOW_EXTERNAL_FILE_FOR_RECON)
    Int m_iStartPtInFile; // for debugging
#endif
    Int m_iTileInFrame; // tile index within current frame

    U32 m_uInfoStat;

    U32 m_uChannelMask;

    // the following sizes are adjusted for half/2x transform
    Int m_iSizePrev;
    Int m_iSizeCurr;
    Int m_iSizeNext;

    Int *m_rgiPower; // power from base tile as it maps to recon proc tile

    // Cx specific
    Int m_iNChGroups; // # of channnel groups
    U32 *m_rguGroupCh; // channels within group
    //Bool *m_rgbGroupCx; // whether channel group "i" has cx

    Bool m_bParamsCoded;
    Int  m_iTilesInCoding;

    CxAnchorReconTileInfo *m_pcxartiCode; // coding parameters (interpolated)
    CxAnchorReconTileInfo *m_pcxartiSeg; // segment parameters (fixed for seg)

#ifdef CHEX_VERIFY
    CoefType m_DCT[2048];
    CoefType m_DST[2048];
#endif

    // RFx specific
    Bool m_bCodingFex;
    Int  m_iCodingFexMaxFreq;
    RFxReconTileInfo *m_prfxrti;

};

typedef enum
{
    ReconProcTileSplitBasic = 0,
    ReconProcTileSplitBaseSmall,
    ReconProcTileSplitArbitrary,
	MAX_ReconProcTileSplit = MAX_VOENUM_VALUE
} ReconProcTileSplit;

typedef struct _ReconProc
{

    struct CAudioObject *m_pau;
    Void *m_phParent;

    // # of samples ready for output from CHEX
    Int *m_rgiPCMSamplesReady;
    Int m_cSamplesReady;

    ReconTileInfo *m_rgchextiBuf;
    BoundInt m_nTilesInBuf; // total tile buffer size

    CxAnchorReconTileInfo *m_rgcxarti;
    Int m_icxarti;
    Int m_ncxarti;

    RFxReconTileInfo *m_rgrfxrti;
    Int m_irfxrti;
    Int m_nrfxrti;

    BoundInt m_cFrameSampleChex;
    BoundInt m_cFrameSampleHalfChex;

    Bool m_bFirstTile, m_bLastTile;

    // index in buffer for current positions
    Int m_iBufAddPos;
    Int m_iBufCalcPos; // only for encoder
    Int m_iBufCodePos;
    Int m_iBufReconPos; // index in buffer of current recon position
    Int m_iBufXFormPos; // only for encoder
    Int m_iBufXFormReconPos;
    Int m_iBufSegPos;

    Int m_iBufCalcFxPos;
    Int m_iBufCodeFxPos;
    Int m_iDecodeRFxTileInFrame;

    Int m_iBufBasePos;

    Int m_iBufMonoPos;

    // # of chex tiles within current base tile
    Int m_iTilesInSubframeAdd;
    Int m_iTilesInSubframeCalc; // only for encoder
    Int m_iTilesInSubframeCode;
    Int m_iTilesInSubframeXForm; // only for encoder
    Int m_iTilesInSubframeXFormRecon; // only for encoder
    Int m_iTilesInSubframeSeg; // only for encoder

    Int m_rgiTilesInSubframeAdd[4];
    Int m_iTilesInSubframeAddPos;

    // # of fx tiles within current base tile
    Int m_iTilesInSubframeCalcFx;
    Int m_iTilesInSubframeCodeFx;
    Int m_iTilesInSubframeProcFx;
    Int m_iTilesInSubframeProcBs;
    Int m_iStartTileInSubframeProcFx;

    Int m_iTilesInSubframeBase;

    Bool m_bTileInit;

    BoundInt m_iPCMSize; // size of buffer (either extra buffer or total buffer)
    BoundInt m_iDCTBufSize;
    Int m_iPCMPrevFrameSave;
    Int *m_rgiPCMStart; // time index of left point in buffer
    Int *m_rgiPCMEndReconPoint; // ending point of reconstruction for base
    Int m_iRecon;

    ReconProcTileSplit m_iTileSplitType;
    Int  m_iNTilesPerFrameBasic;
    Int  m_iMaxTilesPerFrame;
    Int  m_iMinTileSize;
    Bool *m_rgbTileSplitArbitrary;

    Int m_iTrailingSize;
    Int m_iStartPt, m_iEndPt, m_iEndOutPt;

    Void (*m_pfnParentSetTileNotInUse)(Void *phParent, ReconTileInfo *prti);

    WMARESULT (*m_pfnDecodeReconFexTile)(struct CAudioObject *pau,
                                         ReconTileInfo *prti,
                                         Int iCh);

    I64 m_iSampleOffset;

    Bool m_bNoRecon;

    WMARESULT m_wmaResult; // to detect errors related to ReconProc functions

} ReconProc;

#define CXREVERB_STAGES   (3)
#define CXREVERB_HIST_LEN (10)

typedef struct _CxReverbFilter
{
    struct CAudioObject *m_pau;

    Int m_iNumChannel;

    Int m_iNumFreq;
    BoundInt m_iNumFreqRanges;
    Int *m_rgiNumFreq; // number of frequencies in each range
    Int *m_rgiFreqEnd; // frequency boundaries for the ranges

#if ( 1 == CX_SMALL_REVERB )

    Int (*m_iHistCurr)[CXREVERB_STAGES];

    RvrbType *m_rgfGains;
    CoefType *(*m_rgpHistBuf)[CXREVERB_STAGES][CXREVERB_HIST_LEN];
    Int m_nTotalHistSize;

#else // so ( 0 == CX_SMALL_REVERB )

    Int *m_iHistCurr;

    CoefType *(*m_rgpHistBuf)[CXREVERB_HIST_LEN];

    RvrbType *m_rgGainInDReal[3];
    RvrbType *m_rgGainInDImag[3];
    RvrbType *m_rgGainOutReal[3];
    RvrbType *m_rgGainOutImag[3];

#endif // CX_SMALL_REVERB

    Int m_iNumBands[CXREVERB_STAGES+1];   // # of bands to cover for each cascade
    Int m_iNumFreqs[CXREVERB_STAGES+1];   // # of frequencies to cover for each cascade

} CxReverbFilter;

extern const Int g_iMinCxBandsForTwoConfigs;

#define MCMASK_5DOT1_DTS (MCMASK_THREE | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_LOW_FREQUENCY)
#define MCMASK_5DOT1_DOLBY (MCMASK_THREE | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT | SPEAKER_LOW_FREQUENCY)

typedef enum _CxLowerChOrigChMask
{

    CxSpeakerOrig = 0,
    CxSpeaker2 = 1, // LEFT | RIGHT
    CxSpeaker3 = 2,
    CxSpeaker4 = 3,
    CxSpeaker5 = 4,
    CxSpeaker6DTS = 5, // 5.1 DTS
    CxSpeaker6Dolby = 6, // 5.1 Dolby
    CxSpeaker8 = 7, // 7.1
	MAX_CxLowerChOrigChMask = MAX_VOENUM_VALUE
} CxLowerChOrigChMask;

#define NUM_CX_MAXBANDDEP (4)

typedef struct _Chex
{
    struct CAudioObject *m_pau;

    Int m_cBands;
    Int m_iBand;
    Int m_cStartIndex;
    Int *m_rgiBandSizes;
    Int *m_rgiBandSizesInterp;
    Int *m_rgiBandStart;

    Int m_cStart;
    Int m_cEnd;

    BoundInt m_cMaxBands;

    QuantFlt m_qfScale, m_qfLRAng, m_qfLRScale;
    QuantFlt m_qfLRReal, m_qfLRImag;

    Int m_iPrevLScale, m_iPrevLRScale, m_iPrevLRAng;
    Int m_iPrevLRReal, m_iPrevLRImag;
    Int m_iPrevLMScale, m_iPrevRMScale;

    HuffInfo m_hiParam0, m_hiParam0PredT, m_hiParam0PredF;
    HuffInfo m_hiParam1, m_hiParam1PredT, m_hiParam1PredF;

    Bool m_bPowerForAllBands;

    Bool m_bUseUniformBands;
    Int  m_iNumBandsInConfig;
    const I16 *m_piBandConfig;

    U32 m_uCodingMethod;

    Bool m_bBandConfigComputed; // only for encoder

    // specifies which chex channels are specified by given fex channel
    U32 *m_rguFexChannel;

    // for base tile
    Int m_iNChGroups;
    U32 *m_rguGroupCh; // channels within group

    ReconTileInfo *m_prtiNextCodedTile;
    ReconTileInfo *m_prtiLastCodedTileInSeg;

    BoundInt m_iMaxPairs;

    own Int *m_rgiPrevParam0;
    own Int *m_rgiPrevParam1;
    own Int *m_rgiPrevParam2;

    Int *m_rgiPrevLScale;
    Int *m_rgiPrevLRScale;
    Int *m_rgiPrevLRAng;

    Int *m_rgiPrevLMScale;
    Int *m_rgiPrevRMScale;

    Bool m_bNeedIntra;

    Bool m_bCodePowerMatrix;
    Int  m_iEndPhaseBand;

    // header info
    Int m_iNumBandIndex, m_iNumBandIndexBits;
    Int m_iQuantStepIndex, m_iQuantStepIndexPhase, m_iQuantStepIndexLR;
    Int m_iQuantStepIndexBits;
    Int m_iBandMultIndex, m_iBandMultIndexBits;
    Int m_iAdjustScaleThreshIndex, m_iAdjustScaleThreshBits;
    ChexAutoAdjust m_eAutoAdjustScale;
    Int m_iAutoAdjustScaleVals;
    Int m_iMaxMatrixScaleIndex, m_iMaxMatrixScaleBits;
    Int m_iStartBand; // band to start cx on
    Bool m_bStartBandPerTile;
    Bool m_bBandConfigPerTile;
    Bool m_bCodeLMRM;
    ChexFilterOutput m_eFilterTapOutput;
    Int m_iFilterTapOutputVals;
    ChexChCoding m_eCxChCoding;
    Int m_iCxChCodingVals;

    Int m_rgiMaxStartBandForFrame[NUM_CX_MAXBANDDEP]; // prev, curr, next
    Int m_iMaxStartBandIndex;
    Int m_iMaxStartBandCurr;

    Bool m_bDoReverbAsPostProcess;
    ChexType m_maxMatrixScale;
    ChexType m_maxMatrixScaleThreshExact, m_maxMatrixScaleThreshApprox;
    ChexType  m_adjustScaleThreshSq;

    ChexType *m_rgReconBandScales;

    Bool m_bSegmentationDone;

    ReconTileInfo *m_prtiSegStart;

    CxReverbFilter *m_pcxrf;

#ifndef BUILD_INTEGER
    Float *m_rgMonoSc;
#else
    I64   *m_rgMonoSc;
#endif
    own ChexType **m_rgrgScMat;

    Bool m_bNoInterpFromPrev;

    Int m_iMaxAnchorsPerFrame;
    Bool m_bNoNextCodedTile;
    Int m_iTilesRemInSegment;

    Int m_cPrevBands;

    Bool m_bNoPhase;

    const Int *m_piCxBands;
    const Int *m_piChexBandMultMSize;
    const I16 ***m_iChexBandSizesM;

} Chex;
// =================================================================
typedef struct CAudioObject
{
    //Step Size
    QuantStepType  m_qstQuantStep;

    // packetization (has nothing to do with the rest)
    Int m_iPacketCurr;
    Int m_cBitPackedFrameSize;
    Int m_cBitPacketLength;

    //utility vars for entropy decoding for high rate only
    I32 m_cRunOfZeros;
    I32 m_iLevel; 
    Int m_iSign;

    // utility vars for runlevel decoding of V3 mask: should go to decoder.
    I16 m_cRunOfMaskZeros;
    I16 m_iMaskLevel; 

    I16 m_iCurrReconMaskBand;
    InBoundU16 m_cChannel;

    // original number of channels
    InBoundU16 m_cOrigChannel;
    // channels coded using base codec
    InBoundU16 m_cCodedChannel;
    InBoundU16 m_cSaveChannel;

    Bool m_bSaveChannelInUse;

    Int m_iMaskSign;

    Bool    m_fNoiseSub; 
    BPSType m_BitsPerSample;
    BPSType m_WeightedBitsPerSample;

    Int m_iMaxEscSize;          // in bits
    Int m_iMaxEscLevel;         // (1<<m_iMaxEscSize)
    
    Int     m_iVersion;
    I32     m_iEncodeOpt;
    I32     m_iAdvancedEncodeOpt;
    U32     m_uAdvancedEncodeOpt2;
    Status  m_codecStatus;          //current status of the codec
    Bool    m_fSeekAdjustment;
    Int     m_iSamplingRate;        //audio sampling rate in Hz
    Int     m_cBytePerSec;
    InBoundU32     m_nBytePerSample;
    U32     m_nBitsPerSample;       // Same as m_nBytePerSample * 8, just avoid the calculation
    BITDEPTH m_iBitDepthSelector;
    PCMData  m_dataType;
    U32     m_nChannelMask;
    U32     m_nOrigChannelMask;
    U32     m_nCodedChannelMask;
    U32     m_nSaveChannelMask;
    I16     m_nSubWooferChannel;    // Channel number of subwoofer in channel array. -1 if it does not exist
    U16     m_nValidBitsPerSample;

    I32     m_iPCMSampleMin;
    I32     m_iPCMSampleMax;

    // V3 RTM bit stream
    Bool    m_bV3RTM;    

    //V3 LLM only! for mixed lossless mode. ChaoHe 8-20-2001
    Bool    m_bEnforcedUnifiedLLM;          //Encoder encodes certain frames in pure lossless mode in WMA Pro.
    Bool    m_fEnableUnifiedLLM;            // Two-pass encoder disables mixed lossless mode for now
    Bool    m_bUnifiedLLM;       //Current subframe is coded in lossless or lossy mode. ChaoHe 8-20-2001
    
    Bool    m_bUnifiedMixedLLM;
//    Bool    m_bMLLMUsePLLMPrevFrm;      //We need to know the prev frame in prvcalcQ1Q2 function.
    Bool    m_bUnifiedPureLLMCurrFrm;      //We use pure lossless codec in mix mode.
    Bool    m_bUnifiedPureLLMNextFrm;      // because we have frame delay in encoding (like confignextsubframe).
    
    Bool    m_bFirstUnifiedPureLLMFrm;
    Bool    m_bSecondUnifiedPureLLMFrm;
    Bool    m_bLastUnifiedPureLLMFrm;

    Bool    m_bDoUpdUpdtingSpd;

    Bool    m_bDoLPC;
    I16     m_iLPCOrder;
    I16     m_iLPCFilterScaling;
    I16     m_iLPCFilterIntBits;

//    I16     m_iMLLMFilterCoeff;         // 
    I16     m_iMLLMResidueQStepSize;    // Quantization stepsize.
    //V3 True LLM mode.
    Bool    m_bPureLosslessMode;
    
    Bool    m_bDoACFilter;          // apply AC filter to remove DC component at the begining.
    Bool    m_bDoInterChDecorr;     // any inter channel decorrelation in current subframe.
    Bool    m_bDoArithmCoding;      // use arithmetic coding to code LPC residues.
    Bool    m_bSeekable;            // Whether the current frame is seekable. (only 1 subfrm per frm now.)
    // We need a seekable subFrm. But it may not be the current sbFrm depending on whether it can be seekable. (pau->m_bNextSbFrmCanBeSeekable)
    // in MLLMUsePLLM, it means next frame must be seekable because the current frame is RAMPCM.
    Bool    m_bNeedSeekable;        

    I8      m_iPCMTrailZeros;       // bitplane Trailing zeros of current subframe.
    // Dynamic range compression stuff
    U8 m_ucDrcFrameScaleFactor;
    I16     m_iCurrReconCoef;       //which coef we are doing; util. var for state machine

    Bool    m_bOutputRawPCM;        // Output raw PCM if LPC fails.
    I16     m_bPLLMQStepSize;       // Stepsize before pure lossless mode.
    //TODO: more the paudec.
    U16     m_cGetPCMSamplesDone; // How many samples have been outputed in current frame.
    
    Bool    m_fAllowSuperFrame;
    Bool    m_fAllowSubFrame;
    Bool    m_fV5Lpc;

    Int     m_iSubFrameSizeWithUpdate;
    InBoundInt     m_iMaxSubFrameDiv;      //maximum no. division into subwindows
    Int     m_cMinSubFrameSample;   //min size of subframes in current frame; many change from frame to frame
    Int     m_cMinSubFrameSampleHalf;   //min size of subframes in current frame; many change from frame to frame
    Int     m_cMinSubFrameSampleQuad;   //min size of subframes in current frame; many change from frame to frame
    BoundInt     m_cPossibleWinSize;

    //should be reset every subfrm in next version
    Int     m_cBitsSubbandMax;      //no. of bits for m_cSubband; 
    InBoundInt     m_cFrameSample;         //no. of input/output samples = 2 * cSubband
    InBoundInt     m_cFrameSampleHalf;     //no. of input/output samples = cSubband
                                               // prefast Integer overflow annotation winos #1121910
    Int     m_cFrameSampleQuad;     //no. of input/output samples = cSubband / 2

    Int     m_cLowCutOff;           //How many coeffs to zero at the low end of spectrum
    Int     m_cHighCutOff;          //How many coeffs to zero at the high end of spectrum
    Int     m_cLowCutOffLong;
    Int     m_cHighCutOffLong;      

    Int     m_iWeightingMode;
    StereoMode  m_stereoMode;       // left-right or sum-difference coding 
    Int     m_iEntropyMode;         //entropy coding mode
    Float   m_fltDitherLevel;       //=.12; Relative intensity of Roberts' pseudo-random noise quantization
    Int     m_iQuantStepSize;
    BP2Type m_fltDctScale;
    InBoundInt     m_cValidBarkBand;       //valid bark band in range; some are out of range in the subband decompositio
    const Int*  m_rgiBarkIndex;     //corresponding coef index for each valid bark
    Int     m_cSubWooferCutOffIndex;//zeros after this
    I32*    m_rgiCoefQ;             // just a pointer

    //random accessed memory
    //overall
    PerChannelInfo* m_rgpcinfo; //this is just a pointer now

    own PerChannelInfoShr *m_rgpcinfoShr;

    //transform
    Int         m_iCoefReconSize;
    own CBT*    m_rgiCoefReconOrig;
    own CBT*    m_rgiCoefReconMLLMOrig; // Used by Mixed lossless decoder, when WMA Pro is in half-tranform mode.
    BoundInt m_iCoefBufSize;

    //weighting
    own Int*    m_rgiMaskQ;
    own Int*    m_rgiMaskQResampled;
    own Int*    m_rgcValidBarkBand;
    own Int*    m_rgiBarkIndexOrig;
    own U8***   m_rgrgrgnBarkResampleMatrix;
    own Int*    m_rgcSubWooferCutOffIndex;

    //output
    I32        *m_rgiPCMInHistory;  //num completed PCM samples in hist buf
    I32        *m_rgiDiscardSilence;
    CBT        *m_rgiInterlacedInput;
    CBT        *m_rgiInterlacedInputT;
    Bool        m_fLeadingSilence;    
    Bool        m_fTrailingSilence;
    U32         m_u32LeadingSize;
    U32         m_u32TrailingSize;
    U32         m_u32ValidSamples; // frame size - trailing size. Only used in LLM
    InBoundI32         m_iPCMReconStart;   // the start point in PCM recon buffer for output
    I32         m_iPCMReconEnd;     // the end point in PCM recon buffer

#ifdef ENABLE_ALL_ENCOPT
    Float   m_fltFirstNoiseFreq;
    Int     m_iFirstNoiseBand;
    InBoundInt     m_iFirstNoiseIndex;
    Int     m_iNoisePeakIgnoreBand; // Num of coefficients to ignore when detecting peaks during noise substitution

    //only used for NoiseSub mode
    own Int*        m_rgiFirstNoiseBand;
    own U8*         m_rgbBandNotCoded;
    own FastFloat*  m_rgffltSqrtBWRatio;
    own Int*        m_rgiNoisePower;

#if defined(_DEBUG) && defined(LPC_COMPARE)
    Float* m_rgfltLpsSpecCos [LPCORDER];
    Float* m_rgfltLpsSpecSin [LPCORDER];
#endif  // _DEBUG
#else   // ENABLE_ALL_ENCOPT
#endif  // ENABLE_ALL_ENCOPT

    WtType *m_rgWeightFactor; 
    Float  *m_rgfltWeightFactor; 
    UInt   *m_rguiWeightFactor;   //share with m_rgiMaskQ when bark

    U32 m_iFrameNumber; // frame counter to the common code

    // 1/2^n and 2^n transforms
    Bool    m_fHalfTransform;
    Bool    m_fPad2XTransform;
    Int     m_iXformSamplingRate;
    InBoundInt     m_iAdjustSizeShiftFactor;
    Bool    m_fWMAProHalfTransform;  // To reduce WMA Pro decoder memory requirements for half-transform
        
    // the following values are adjusted to reflect HalfTransform or PAd2XTransform
    // They are used for the DCT and for Reconstruction.
    Int     m_cFrameSampleAdjusted; 
    InBoundInt     m_cFrameSampleHalfAdjusted;
    InBoundInt     m_cHighCutOffAdjusted;
    // m_iCoefRecurQ1..4 and Trig values only exist in their adjusted states

    // Function Pointers
    WMARESULT (*aupfnInverseQuantize) (struct CAudioObject* pau,
        PerChannelInfo* ppcinfo, Int* rgiWeightFactor);
    WMARESULT (*prvpfnInverseTransformMono) (struct CAudioObject* pau,
        PerChannelInfo* ppcinfo, PCMSAMPLE* piOutput, PCMSAMPLE* piPrevOutput,
        I16* piPrevOutputSign, Bool fForceTransformAll);
    WMARESULT (*aupfnGetNextRun) (Void* paudec, struct PerChannelInfo* ppcinfo);

    WMARESULT (*aupfnReconSample) (struct CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
    WMARESULT (*m_pfnPostProcess) (Void *pDecHandle, U16* pcSamplesReturned, U8 *pbDst, U32 cbDstLength);

    WMARESULT (*aupfnDctIV) (CoefType* rgiCoef, BP2Type fltAfterScaleFactor, U32 *piMagnitude,
        const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo, const Int iFrameNumber,
        const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
        const Int cSubFrameSampleAdjusted);

    void (*m_pfnApplyWindow)(struct CAudioObject *pau, CoefType *pctCoef, Int iSizePrev, 
                             Int iSizeCurr, Int iSizeNext, Bool fBase);

    // --- Lossless LMS function pointers. We will have many sets of functions MMX/X86 16bits/24Bits --- //
    // Multi-channel LMS function pointers.
    Void (* pprvMCLMSPred)(struct CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT *rgiInput, CBT *rgiPred, Bool bDecoder);
    Void (* pprvMCLMSUpdate)(struct CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT * rgiInput, CBT *rgiPred);
    // Cascaded LMS function pointers.
    I32 (* pprvCDLMSPred)(LMSPredictor * pLMSPredictor);
    Void (* pprvCDLMSUpdate)(struct CAudioObject * pau, LMSPredictor * pLMSPredictor, I32 iInput, I32 iPred);


#ifdef BUILD_WMAPROLBRV2
    Void (*cxpfnReverbFilter)( CxReverbFilter *pcxrf, Int iCh );
    Void (*cxpfnReconMonoDiff)( struct CAudioObject *pau, ReconTileInfo *prti,
                           Int iCh, const CoefType *pCoefSrc, CoefType *pCoefDst,
                           Bool bMono );
#else
    Void (*cxpfnReverbFilter)( void*, Int iCh );
    Void (*cxpfnReconMonoDiff)( struct CAudioObject *pau, void *, Int, const CoefType *,
                            CoefType *, Bool );
#endif

    PFNFFT aupfnFFT;
    WMARESULT (*m_pfnInvFftComplexConjugate) (Void *hIntelFFTInfo, CoefType rgfltData[], Int nLog2np, CoefType *rgfltTmp, U32 cSizeTmp);

    PFNSETSAMPLE m_pfnSetSample;
    PFNGETSAMPLE m_pfnGetSample;

    Bool    m_fIntelFFT;        // Intel FFT needs setup/takedown, so we need to know this
    Bool    m_fIntelCcsFFT;
    Void*   m_hIntelFFTInfo;    // Intel FFT Info
    Void*   m_hIntelFFTCcsInfo; // Intel FFTCcs Info

    tRandState m_tRandState;

#ifdef USE_SIN_COS_TABLES
    //Tables for reconstruction sin values
    BP2Type *m_piSinForRecon8192;
    BP2Type *m_piSinForRecon4096;
    BP2Type *m_piSinForRecon2048;
    BP2Type *m_piSinForRecon1024;
    BP2Type *m_piSinForRecon512;
    BP2Type *m_piSinForRecon256;
    BP2Type *m_piSinForRecon128;
    BP2Type *m_piSinForRecon64;
#endif
    
    own ChXFormType ***m_rgrgrgfltMultiXIDCT;
    own CoefType      *m_rgfltCoefDst;           // Temporary for inverse channel transform in V3.
    own CoefType     **m_rgpfltCoefGrpSrc;       // Temporary for inverse channel transform in V3.
    own ChXFormType  *m_rgfltGivensTmp0;         // Temporary vector for inverse multi
    own ChXFormType  *m_rgfltGivensTmp1;         // Temporary vector for inverse multi

    // Dynamic range compression stuff
    DRC *m_pDrc; // Class to do dynamic range compression, encode/decode
 

    // V3 stuff: These are really in encoder buckets or decoder.
    Int     m_cChannelGroup;
    CChannelGroupInfo *m_rgChannelGrpInfo; 
    I16     m_cChInTile;                    //how many src channels are included
    I16     m_iACFilterScaling;

    I16*    m_rgiChInTile;                  //which src chs are included

    Bool m_fUseVecCoder;
    Bool m_fExpVecsInNonRLMode;

    Bool m_fByteAlignFrame;
    Bool m_fWriteFrameSize;
    Bool m_fExtraBitsInPktHeader;
    Int m_cComprFrameSizeBits;
    Int m_cBitsBytePacketLength;
    Int m_cBitsBitsPacketLength;
    Int m_cBytePacketLength; 

    // generate drc scale factors
    Bool m_fGenerateDrcParams;
    // V3 LLM.
    Bool    m_bDoChMixVoting;
    // V3 LLM. AC filter
    I32             m_iACFilter[LLM_VERB_ACFILTER_ORDER_MAX]; // AC filter.
    I32             m_iACFilterOrder;
    I32             m_iACFilterCurr; // for decoder.
    // V3 LLMB Chao multichannel LMS
    Bool    m_bDoMCLMS;
    Bool    m_bSendMCLMS;
    MCLMSPredictor  m_MCLMSPredictor;

    Bool    m_bSendCDLMS;

    I32     m_iSampleMaxValue;
    I32     m_iSampleMinValue;

    I16     m_iResidueMemScaling;  // LLMB_RESIDUE_MEMORY_TTLWEIGHT    
    U16     m_cLastCodedIndexSave;
	U16     m_cLastCodedIndex;     // To reduce decoder complexity in V3

    Bool m_bNextSbFrmCanBeSeekable;
    Bool m_bNextSbFrmMustBeSeekable;
    // Temporary space required for auGetPCM
    own U16  *m_rgcSamplesREQ;

    Bool m_bMonoV3; // for skipping some header bits

#ifdef WMA_COMPRESSION_TOOL
    //
    // Following members are added for professional WMA encoder.
    //
    Bool m_bSegDecode;  // Decode a segment of a WMA file.
    // Peek a packet. Mainly used to check if the current packet could be start of seg.
    // That means 1) must be seekable, 2) first frame can not be unified LLM frame. 
    Bool m_bSegDecodePeekMode;  
    Bool m_bPeekModeDone;
    Bool m_bPeekModeNonSekPkt; // we can start from this packet.
    // The actual decoded bits are from offset of iSegSta packet to bit before 
    // the offset of the iSegEnd packet. The packet count starts from 0.
    U32  m_cFrmsInSeg; // Total frames in this segment.
    U32  m_cPktsInSeg; // Total packets used.
#endif //WMA_COMPRESSION_TOOL

    Int  m_cLeftShiftBitsFixedPre;
    Int  m_cLeftShiftBitsFixedPost;

    // V3 LLMA Chao multichannel mixing sequence
//    U16  m_iMltChlsMixingSeq[(MAX_CHANNEL_MIXING_ROUND + 1)][2];
    U16  m_iMltChlsMixingRnd;

    // Profesional Encoder Related
    U8 * m_rgbFrameBits;    // buffer to hold a frame's bits.
    I32 m_cFrameBitsBufLen; // buffer length in bytes.
    I32 m_cFrmUsedBitsFromParser;    // total bits used in buffer.
    // total frame hdr bits. before the removeSeekhdr it is all head bits. After the call
    // it excludes the hdr0 bits and framesize bits.
    I32 m_cFrmHdrBitsFromParser; 
    I32 m_cFrmHdrBitsFromParserOrg; // original hdr bits. includes everything. Used for matching.

#ifdef BUILD_INTEGER
    // used for calling auPreScaleCoeffsV3()
    I32 m_iLog2MaxIntvalCh;
    CoefType m_maxOutputAllowed;

    Bool m_bUseDynamicIntegerPrec;
#endif

    // plus syntax
    Bool m_bPlusV1;
    Bool m_bPlusV2;

    // fex
    Bool m_bFreqex;
    Bool m_bFreqexPresent;
    Bool m_bDoFexOnCodedChannels;
    Bool m_bIgnoreFreqex;

    // recon fex
    Bool m_bReconFex;
    Bool m_bReconFexPresent;

    // tile-level control of fex
    Bool m_bTileCodingFex;
    Bool m_bTileReconFex;
    Bool m_bAlignFexBoundary;
    Bool m_bTileReconBs;
    Bool m_bTileReconAnyBsOnly;

    // chex & buffers for chex
    Bool m_bCx;
    Bool m_bCxPresent;
    Bool m_bIgnoreCx;

    // base peak coding
    Bool m_bBasePeakPresent;
    Bool m_bBasePeakPresentTile;
    Bool m_bIgnoreBasePeak;

    // recon processing
    Bool m_bReconProc;

    // always code presence of all plus modes at subframe level
    Bool m_bCodePresentForAllPlus;
    // the following are only used to determine if presence bit is present
    // or not -- overwriten to false if  m_bCodePresentForAllPlus=true
    Bool m_bBxIsLast, m_bCxIsLast, m_bFxIsLast, m_bRFxIsLast, m_bBPxIsLast;

    Bool m_bLastTilePlusHeaderCoded;

    // potentially a smarter implementation should share memory with others
    own CoefType *m_rgCoefHistMCLT;
#if defined(CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL) && CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL
    own BP1Type *m_rgCoefDct256tab;
#endif
    own CoefType *m_rgCoefXFormMCLT;
    own CoefType *m_rgCoefXFormMCLTDiff;
    own CoefType *m_rgCoefXFormDST;
    own BP2Type* m_rgIMCLTSinCosTable;

#ifdef BUILD_INTEGER
    Int m_cLeftShiftBitsMCLT;
#endif

    own CoefType *m_rgDCTBuf;

    Int m_iCurrTile;
    Int m_cTiles;
    TileConfigInfo *m_rgtci;
    Int *m_rgiSubframeStart, *m_rgiCurrSubframe;

#if defined(BUILD_WMAPROLBRV1)

    Fex m_fex;
    own FastFloat *m_rgffltFexScaleFac;
    own FastFloat *m_rgffltFexScaleFacBase;
    own I32       *m_rgiFexLogScaleFacIndex;
    own I32       *m_rgiFexLogScaleFacBaseIndex;
    own I32       *m_rgiFexLogScaleFacIndexRef;
    own I32       *m_rgiFexMv;
    own I32       *m_rgiFexMvOffsetNoiseCB;
    own I32       *m_rgiFexCb;
    own FexType   *m_rgFexExp;
    own I32       *m_rgiFexSign;
    own Bool      *m_rgbFexRev;
    own I32       *m_rgcFexSubframesMvMerged;
    own FastFloat *m_rgffBandScale;
    own FexScaleFacPredType *m_rgFexScaleFacPred;
    own FexScaleFacPredType *m_rgFexScaleFacBasePred;
#ifdef BUILD_WMAPROLBRV3
    Int m_iMaxBasePeakCoef;
    Int m_iBasePeakStepSize;
    own I16 *m_rgiBasePeakCoefPos;
    own Int *m_rgcBasePeakCoefs;
    own Int *m_rgiBasePeakMaskQSave;
    U16 m_iBasePeakNLQDecTblSize;
    const FPNLQ *m_pBasePeakNLQDecTbl;
    Bool m_bBasePeakFirstTile;
    U16 m_cBasePeakLastCodedIndex;
#endif

    own U8  *m_ucNonzeroCoeffMask;
    U8 *m_pucNonzeroMask; // the current non-zero mask
    Int m_iNonzeroMaskCh;
    Int m_cChannelDependencyGroups;
    Int m_cChannelGroups;
    Int m_iChGroup;
    Bool m_bNeedMaskProc;
    Bool m_bXChannelProc;

    own CoefType *m_rgCoefSampMCLT;

    ReconProc *m_prp;
    Chex *m_pcx;

    own I32 *m_rgiPlusBsBuf; // used for reading/writing plus bitstream
    Int m_iPlusBits; // # of subframe bits in plus bitstream (remaining)
    Int m_iPlusBsSize; // size of plus bitstream

#endif // BUILD_WMAPROLBRV1

    Int m_iPlusVersion;

    Bool m_bUseV4EntropyCoding;

    Bool m_bFirstPlusFrameAfterReset;
    Bool m_bFirstPlusFrameAfterSuperframe;
    Bool m_bSkipPlusDueToRFx;
    Bool m_bFirstFrameAfterReset;

    // UES   
    Bool m_fUES;
#ifdef BUILD_UES
    CUESObject *m_pUES;
#endif
    // for debugging
#ifdef WMA_DEBUG_OUTPUT
    FILE *fp[WMA_DEBUG_NUM_MODES+1];
    Int  m_iWmaDebugModes[WMA_DEBUG_NUM_MODES+1];
#endif

    Bool m_fBasePlusPresent;
    Bool m_fBasePlus;
    Bool m_fBasePlusPresentTile;
#if defined(BUILD_WMAPROLBRV2)
    CBasePlusObject *m_pbp;
#endif // BUILD_WMAPRO && BUILD_WMAPROLBRV2

    Bool m_bEncoder;
    Void *m_hParent;
    Int  m_iFrameStart; // 1/4 frame size into starting point of frame

    Bool m_bPlusSuperframe;

#ifdef WMA_TARGET_ARM
    U32  m_ulSizeOfPerChannelInfo;	// Used by ARM asm functions, instead of the incgen-generated symbol, which depends on compiler padding
#endif
} CAudioObject;

typedef struct CAudioObjectDecoder 
{
    CAudioObject* pau;
    own struct PerChannelInfo* m_rgpcinfo; //this is just a pointer now
    own PerChannelInfoDEC* m_rgpcinfoDEC;
    
    //packetization stuff
    Bool  m_fPacketLoss;
    I16   m_cFrmInPacket;
    I16   m_cSeekableFrmInPacket; // for LLM. do we have seekable frm in this packet?
    Bool  m_bGotValidFilterInfo; // Have we decoded valid LMS filter info?
    U8*   m_pbSrcCurr;  //ptr to src of curr packet, only for non-strm mode
    U16   m_cbSrcCurrLength; //only for non-strm mode
    DecodeStatus       m_decsts;
    SubFrmDecodeStatus m_subfrmdecsts;
    HdrDecodeStatus    m_hdrdecsts;
    HdrDecodeFltParStatus m_hdrdecFilterParasts;
    HdrDecodeFltParCDLMSStatus m_hdrdecFilterParaCLMSsts;
    FHdrDecodeStatus   m_fhdrdecsts;
    RunLevelStatus     m_rlsts;
    I32                m_iSubfrmEscDataBits;

    Int                m_cBitSeekToNextPacket;
    U32                m_cBitsOverCurrPkt;

    // V3 LLM. Chao. Added.
    LLMDecodeCoefStatus m_LLMdecsts;    
    ColombDecodeStatus  m_Colombdecsts;
    Decode1stSplStatus  m_Dec1stSplsts;
    Bool               m_1stSplSignBit;

    // V3 stuff
    ChXformStatus      m_chxformsts;
    Int                m_cChannelsLeft;
    Bool               m_fDecodingSuperGroupXform;
    Int                m_cAngles;
    Int                m_cSigns;
    Int                m_iAngle;
    Int                m_iSign;
    __in_bound Int     m_cChannelGroup;
    own struct CChannelGroupInfo *m_rgChannelGrpInfo; 
    RunLevelStatus     m_maskrlsts;
    Bool               m_fTileAligned;
    Int                m_cBitQuantStepModifierIndex;
    Int                m_iQuantStepSign;
    Bool               m_fQuantStepEscaped;

    // WMA Voice related 
    U8  m_cbWMAV_Version;

    // V3 LLM. Chao. Added. more varibles to record decoder status for supporting decoder onhold/resume.
    I8                 m_iCurrPart;            //which part we are doing
    I16                m_iLMSCurrCoeff;
    I16                m_iCurrLPCCoef; //which lpc filter coefficient we are doing.

    // Common to V2, V3
    I16                m_iChannel;  //current channel we are doing
    I16                m_iBand;     //current band we are doing
    I16                m_iTtlNTBs;
    Bool               m_fNewTimeBase;
    Bool               m_fContinuousDecodeAdj;
    Bool               m_iContinuousDecodeCountdown; // Countdown to first packet w/ continuous adj
    I64                m_iNewTimeBase;//in 100nano second
    I64                m_iNewTimeBaseTemp;// The second (later one) NewTimeBase.
    I64                m_iCurrPresTime;//in samples

    own U8**           m_rgpiRecon; // Used to manage recon pointers in audecGetPCM

    // iCurrReconCoef is in pau.
    U16                m_iDivisorWidth;
    U32                m_iResQ;
    U32                m_iResQBits;
    U32                m_iRem;
#if defined ENABLE_ARITHCODER
    // V3 LLM (LLMB) Chao. Arithmetic coding.
    ArithDecoder       *m_pArithDecLLM;
    ArithCoderModel    *m_pACModelLLM;
#endif //ENABLE_ARITHCODER

    Void (* pprvDecLMSPredictor)(struct CAudioObject * pau, struct LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);

    Bool               m_fLastSubFrame;
    I16                m_iCurrTile;
    CWMAInputBitStream m_ibstrm;
    I16                m_cMaxRun;
    I16                m_iMaxLevelInVlc;

#ifdef ENABLE_EQUALIZER
#define MAX_NO_EQBAND 10
    Float       m_rgfltEqBandWeight[MAX_NO_EQBAND];
    U16         m_rgwEqBandBoundary[MAX_NO_EQBAND + 1];
    I8          m_cEqBand;
    I8          m_rgbBandPower[MAX_NO_EQBAND];
    Bool        m_fComputeBandPower;
    Int         m_iEqFrameSize;
#endif // ENABLE_EQUALIZER

#if defined(SEEK_DECODE_TEST)
    tRandState *m_pSaveRandState;
#endif // SEEK_DECODE_TEST

    // INTERPOLATED_DOWNSAMPLE
    Bool    m_fUpsample2x;
    Bool    m_fLowPass;             // Specify independently of downsampling
    Bool    m_fResample;          // Eg, Downsample 32kHz output to 22kHz for some devices
    Int     m_iInterpolSrcBlkSize;  // Size of 1 indivisible source block
    Int     m_iInterpolDstBlkSize;  // Size of 1 indivisible destination block

    Int     m_iDstSamplingRate;
    Int     m_iInterpolCurPos;
    I32       *m_iPriorSample;
    PCMSAMPLE *m_iLastSample;

    // Channel & sample downsampling at playback time
    Bool       m_fChannelFoldDown;      // Eg, from 5.1 to 2 channels or 7.1 to 5.1
    Bool       m_fReQuantizeTo16;      // Eg. from 3 bytes to 2 bytes/sample
    Int        m_cDstChannel;          // Destination channels
    U32        m_nDstChannelMask;      // Destination channel mask
    U32        m_nDstBytePerSample;    // Destination resolution, bytes per sample
    Bool       m_bAuthorControlledChDnMixMtx; // Did user specify channel downmix matrix?
    Int        m_cDstChannelInChDnMixMtx;    // This will be equal to m_cDstChannel in all cases except for 
                                             // LtRt folddown where it will be equal to LTRT_SRC_CHANNELS
    ChDnMixType**    m_rgrgfltChDnMixMtx;    // user specified, or automatically generated?
#if 0
    Float**          m_rgrgfltChDnMixMtxTmp; // Used in generation of default matrices
#endif
    PCMSAMPLE* m_rgpcmsTemp;           // Used in channel down mixing, of size cChannel: can be shared for other uses.
    CoefType* m_pctTemp; // Used in sample places as m_rgpcmsTemp, shares memory with m_rgpcmsTemp, so no real allocation here (i.e. DO NOT FREE THIS!!)
    CoefType** m_rgpctTemp; // Array of pointers to CoefType buffer
    CBT**      m_ppcbtTemp; //
    U16        m_nDstValidBitsPerSample;
    Float      m_fltChannelFoldDownGain;   // Gain factor used to boost folded down channels. Used in channel downmixing and drcControl.

    Bool m_bPlusV2Init;
    CWMAInputBitStream *m_pibsPlus;
    Int m_iFexByteCnt;
    Bool m_bIgnoreFreqexThroughRegistry;// To use in all audecGetPCM calls

    Bool m_bSkipPlus;

    Bool m_fLtRtDownmix;
    Bool m_fLtRtDownmixNonnormal;
    struct LtRtDownmix *m_pLtRtDownmix;
    Int m_cLtRtBufSize;

    // Post-processing channel transform at playback time
    PPXformStatus     m_ppxformsts;
    Bool              m_fPostProcXform;
    Bool              m_fGeneralPostProcXform; // as opposed to pre-defined
    ChPPXFormType*    m_rgfltPostProcXform;
    Int               m_iPPXformIndex;  // to iterate over all coefs
    Int*              m_rgiPostProcBuf; // temporary buffer.

    Bool              m_fPostProcXformPrev;    // Status in previous frame
    ChPPXFormType*    m_rgfltPostProcXformPrev;// Status in previous frame
    ChPPXFormType*    m_rgfltPostProcXformBlend;// Blend of previous and current transform
    
    // Dynamic range compression at playback time
    Bool    m_fDRC;                 // Perform DRC - either filewise or dynamic
    WMAPlayerInfo m_wmapi;

    WMARESULT (*m_pfnDecodeSubFrame) (struct CAudioObjectDecoder* paudec);
    WMARESULT (*m_pfnDecodeCoefficient) (struct CAudioObjectDecoder* paudec, struct PerChannelInfo* ppcinfo);   
    Void      (*m_pfnResetEntropyCoder)(struct CAudioObjectDecoder *paudec);

    DecRunLevelVecState m_decRunLevelVecState;

    audecState m_externalState; // which public function should be called next
    Bool m_fNeedHeader;
    U32 m_cSamplesReady;
    Bool m_fSPDIF;
    Bool m_fSPDIFStyleGetPCM;
#ifdef WMA_COMPRESSION_TOOL
    // Professional Encoder related.
    Bool m_fWriteMetaFile;
    const char * m_pMetaFileDir;
    Bool m_fDumpMetaData;
    Bool m_fParsingOnly;
    I32 m_fFrameBitsReady;
    I32 m_ulCurrSeekPoint;
#endif //WMA_COMPRESSION_TOOL

#ifdef TEST_VRHEADPHONE
    PCMFormat m_fmtI;
    PCMFormat m_fmtO;
    Void * m_pVRHPhone;
#endif //TEST_VRHEADPHONE

#ifdef _WMA_COUNT_CPU_CYCLES_IF_REGKEY_
    CCSStruct m_CCSStruct;
#endif
    // UES
#ifdef BUILD_UES
    enum UES_DEC_INIT_FLAG  m_iUESInitFlag;
    CUESObjectDecoder *m_pCUESdec;
#endif

#ifdef BUILD_WMAPROLBRV2
    Bool m_fDecodingBPlus;      // is decoder decoding bplus layer now? control ibstrm.
#endif //BUILD_WMAPROLBRV2

                                // Is the current frame broken? (bit stream corruption)
    Bool m_bBrokenFrame;        // This determines whether the call to GetPCM should be made
                                // Impacts only scenarios where GetPCM is called with a small buffer.

    WMARESULT (*m_pprvNewInputBuffer)(struct CAudioObjectDecoder *paudec, audecInputBufferInfo *pBuf);
	//Witten added
	int f_DataRemnent, cbIn, bInLength;
	U8 *pBufferRement;
	WMA_U8 *pbIn;

	WMAProRawDecState	decState;
	WMAPlayerInfo		PlayerInfo;
	WMAFormat *			pWMAForm;
	WMA_I32				Isheader;
	WMA_U32				cSubband;
	WMA_U32				cbDst;
	WMA_U32				cSampleReady;

	VO_MEM_OPERATOR *	vopMemOP;
	WMA_U32				SupptHiSmplrt;
	WMA_U32				SupptMtChanl;
	WMA_U32				Support24Bit;

	//voCheck lib need
	VO_PTR				hCheck;
} CAudioObjectDecoder;

WMARESULT cxReverbInit(struct CAudioObjectDecoder *paudec, struct CAudioObject *pau, CxReverbFilter *pcxrf);
Void cxReverbFree(struct CAudioObjectDecoder *paudec, CxReverbFilter *pcxrf);

Void cxReverbAddInput(CxReverbFilter *pcxrf,
                      const CoefType *pDataReal, 
                      const CoefType *pDataImag,
                      Int iSize, Int iCh);

Void cxReverbFilter( CxReverbFilter *pcxrf, Int iCh );
Void chexReconMonoDiff(struct CAudioObject *pau, ReconTileInfo *prti, Int iCh, 
        const CoefType *pCoefSrc, CoefType *pCoefDst, Bool bMono );

#if defined(WMA_OPT_CHEXPRO_ARM) && WMA_OPT_CHEXPRO_ARM
Void cxReverbFilterLoop(const Int iNumFreqs, const Int iNumFreq, 
    const RvrbType  gainIn    , const RvrbType *pGainInDRe, const RvrbType *pGainInDIm, 
    const RvrbType *pGainOutRe, const RvrbType *pGainOutIm, const CoefType *pInCurRe  , 
          CoefType *pOutCurRe , const CoefType *pInDelRe  , const CoefType *pOutDelRe );
Void cxReverbFilterLoopSmall(CxReverbFilter *pcxrf, CoefType **ppPointer, const RvrbType *rgfGains);

Void CoefMulCxscArray(const CoefType *pMDCTSrc, CoefType *pMDCTDst, ChexType scale, Int iCount);
Void CoefMulAddCxscArray(const CoefType *pMDCTSrc, CoefType *pMDCTDst, ChexType scale, Int iCount);

Void chexComputeBandSc_loop(const CoefType *pMDCT, Int iLen, I64 *paccum);

Void memcpy_ARM(void *dst, const void *src, int len);

Void chexReconMonoDiff_loop1(ChexType *pScaleR, ChexType *pScaleI, 
    const CoefType *pMDCTSrc, const CoefType *pMDCTDst, const Int iSizeCurr, Int nFreqs);

Void chexReconMonoDiff_loop2(ChexType *pScaleR, ChexType *pScaleI, 
    const CoefType *pMDCTSrc, const CoefType *pMDCTDst, const Int iSizeCurr, Int nFreqs);

Void chexReconMonoDiff_loop3(ChexType *pScaleR, 
    const CoefType *pMDCTSrc, const CoefType *pMDCTDst, const Int iSizeCurr, Int nFreqs);
#endif  // WMA_OPT_CHEXPRO_ARM

#if defined(BUILD_INT_FLOAT) && !defined(WMA_DISABLE_SSE1)
Void cxReverbFilter_SSE1( CxReverbFilter *pcxrf, Int iCh );

Void chexReconMonoDiff_SSE1(CAudioObject *pau, ReconTileInfo *prti, Int iCh, 
        const CoefType *pCoefSrc, CoefType *pCoefDst, Bool bMono );
#endif

Void cxReverbGetOutput(CxReverbFilter *pcxrf,
                       CoefType *pDataReal, CoefType *pDataImag, Int iSize,
                       Int iCh, ChexFilterOutput eFilterTap);
Void cxReverbDumpHistBuf(CxReverbFilter *pcxrf, Int iCh, Int iType, FILE *fp);

#if ( 1 == CX_SMALL_REVERB )

INLINE
Int cxReverbOffsetHist(CxReverbFilter *pcxrf, Int iCh, Int iPos, Int iNumStages )
{
    const Int iLen = (iNumStages+2)*(iNumStages+1)/2;
    return ((pcxrf->m_iHistCurr[iCh][3-iNumStages]+iPos)%iLen + iLen )%iLen;
}

#else // so ( 0 == CX_SMALL_REVERB )

INLINE
Int cxReverbOffsetHist(CxReverbFilter *pcxrf, Int iCh, Int iPos )
{
    return ((pcxrf->m_iHistCurr[iCh]+iPos)%CXREVERB_HIST_LEN+CXREVERB_HIST_LEN)%CXREVERB_HIST_LEN;
}

#endif // CX_SMALL_REVERB

#endif // BUILD_WMAPROLBRV1


#ifdef USE_V4ENTCODING
#define ENCOPT3_V4ENTCODING 0x2000
#undef  ENCOPT3_UNSUPPORTED_OPTS
#ifdef ENABLE_MULTIPLE_OF_160
#define ENCOPT3_UNSUPPORTED_OPTS 0xd000
#else
#define ENCOPT3_UNSUPPORTED_OPTS 0xde00
#endif
#endif

//public and protected

#define HALF(fHalfTransform, x) \
((fHalfTransform) ? ((x)>>pau->m_iAdjustSizeShiftFactor) : (x))
#define DOUBLE(fPad2X, x) \
((fPad2X) ? ((x)<<pau->m_iAdjustSizeShiftFactor) : (x))
#define AU_HALF_OR_DOUBLE(fHalf, fPad2X, x) \
((fHalf) ? ((x)>>pau->m_iAdjustSizeShiftFactor) : ((fPad2X) ? ((x)<<pau->m_iAdjustSizeShiftFactor) : (x)))

#include "configcommon.h"

U32 DefaultChannelMask(U32 nChannels);

CAudioObject* auNew (struct CAudioObjectDecoder *paudec);
Void    auDelete (struct CAudioObjectDecoder *paudec, struct CAudioObject* pau);        //destructor
WMARESULT prvInitOutputSampleFormatParameters(CAudioObject *pau,
                                              U32 nBytePerSample,
                                              PCMData dataType);
WMARESULT prvInitOutputSampleFormatSecondaryFixed(CAudioObject *pau);
WMARESULT prvInitOutputSampleFormatSetFunctionPtrs(CAudioObject *pau);
WMARESULT auInvChannelXForm (CAudioObject *pau, Bool fInvertSuperGroupXform);
WMARESULT prvMultiXIDCTAllocate  (struct CAudioObjectDecoder *paudec, CAudioObject *pau);
WMARESULT prvMultiXIDCTInit (CAudioObject *pau);
Void      prvMultiXIDCTDelete  (struct CAudioObjectDecoder *paudec, CAudioObject *pau);
Void    auPreGetPCM (CAudioObject* pau, U16* pcSampleDecoded, I16* pcSamplesSeekAdj, Bool fSPDIF);
WMARESULT auSubframeRecon(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
WMARESULT auGetPCM (CAudioObject* pau, Void* pHandle, U16* pcSample, U8** ppbDst, U32 cbDstLength, U16 cChannel, Bool fSPDIF);

#if defined(WMA_OPT_SUBFRAMERECON_ARM) && WMA_OPT_SUBFRAMERECON_ARM
Void auSubframeRecon_swapData_Std(CoefType *piCurrCoef, CoefType *piCurrCoef2, I16 iSizeCurr);
#ifdef OVERLAPADD_LOOKUPTBL
Void auSubframeRecon_applyWindow_Std_TLU(CoefType *piPrevCoef, CoefType *piCurrCoef, CoefType *piPrevCoef2, CoefType *piCurrCoef2, 
    BP2Type *OverlapAddTable, Int iOverlapSize, Int rightShifts);
#endif
Void auSubframeRecon_applyWindow_Std(CoefType *piPrevCoef, CoefType *piCurrCoef, CoefType *piPrevCoef2, CoefType *piCurrCoef2, 
    BP2Type bp2Sin, BP2Type bp2Cos, BP2Type bp2Sin1, BP2Type bp2Cos1, BP2Type bp2Step, Int iOverlapSize, Int rightShifts);
Void auSubframeRecon_ScaleCoef(CoefType *piCurrCoef, Int iSize, Int rightShifts);
#endif

#define CAP_NODISCARDSILENCE    0
#define CAP_DISCARDSILENCE      1
#define CAP_SUBFRAMELVL         0
#define CAP_ENTIREFRAME         1
void prvCountAlignedPCM(CAudioObject *pau,
                        U16 *piAlignedPCM,
                        Bool fDiscardSilence,
                        Bool fEntireFrame,
                        Bool fSPDIF);
void prvGetFramePCM( CAudioObject* pau, Int *piFramePCM );
Void auShiftPCMBuffer(CAudioObject *pau, Int iShiftAmt);
void prvShiftPCMBuffer( CAudioObject* pau );

I32 prvNextSeekPoint(const CAudioObject *pau);
I32 prvContinuousDecodeAdjustment(const CAudioObject *pau);
void prvCalcQ1Q2(CAudioObject * pau, Bool bCurrWindow, const I16 iSizePrev, const I16 iSizeCurr,
                 I16 *piCoefRecurQ1, I16 *piCoefRecurQ2);
void prvCalcQ3Q4(CAudioObject * pau, Bool bCurrWindow, const I16 iSizeCurr, const I16 iSizeNext,
                 const Int cSubFrameSampleHalfAdjusted,
                 I16 *piCoefRecurQ3, I16 *piCoefRecurQ4);

void prvInitDiscardSilence(CAudioObject *pau, Bool fSPDIF);

#ifdef TRAILING_SILENCE
void prvInitTrailingSilence(CAudioObject *pau);
#endif  // TRAILING_SILENCE

WMARESULT auReconSample(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
WMARESULT auReconSampleFloat(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);

WMARESULT auAdaptToSubFrameConfig (CAudioObject* pau);
//wchen: need to move the msaudiodec.h
WMARESULT auAdaptToSubFrameConfigDEC (CAudioObject* pau);
WMARESULT prvAdaptTrigToSubframeConfig(CAudioObject *pau);

Int     auPowerToRMS(CAudioObject* pau, Int iCurrMaskQ, Int iPrevMaskQ, Int iPrevResult);
Void    auUpdateMaxEsc(CAudioObject* pau, Int iQuantStepSize);

typedef WMARESULT(FNDCTIV)(CoefType* rgiCoef, BP2Type fltAfterScaleFactor, U32 *piMagnitude,
                  const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo, const Int iFrameNumber,
                  const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted);

// DCT type 4
WMARESULT auDctIV(CoefType* rgiCoef, BP2Type fltAfterScaleFactor, U32 *piMagnitude,
                  const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo, const Int iFrameNumber,
                  const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted);

WMARESULT auWindowReflectionAndDctIV256(CoefType* rgiCoef, BP2Type fltAfterScaleFactor, U32 *piMagnitude,
                  const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo, const Int iFrameNumber,
                  const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted);

#if defined(BUILD_INT_FLOAT) && !defined(WMA_DISABLE_SSE1)

WMARESULT auDctIV_SSE1(CoefType* rgiCoef, Float fltAfterScaleFactor, U32 *piMagnitude,
                  const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo, const Int iFrameNumber,
                  const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted);
#endif

// DST type 4
WMARESULT auDstIV(CoefType* rgiCoef, BP2Type fltAfterScaleFactor,
                  U32 *piMagnitude,
                  const Int cSubbandAdjusted, FNDCTIV *pfnDctIV, PFNFFT pfnFFT, Void *pFFTInfo,
                  const Int iFrameNumber,
                  const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted,
                  Bool fFlip);

#if defined(FFT_LOOKUPTBL) && FFT_LOOKUPTBL
extern BP2Type g_W128_i[128]; // real, imaginary part of Wnp_i, i = 0, 1, 2, ..., np/2 - 1
void prvFFT4DCT_128Tbl(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection);
void conFFT128Tbl(); 
#endif  // defined(FFT_LOOKUPTBL) && FFT_LOOKUPTBL

#if defined(DCT_LOOKUPTBL) && DCT_LOOKUPTBL
extern ROPI_CONST SinCosTable g_sct256;
extern I32 g_nFacExponent_scale1;
extern I32 g_nFacExponent_scale2_256;
extern BP1Type g_pre128FFTTbl_scale1[256];           // pre 128FFT lookup table for scale factor = 1.0f
extern BP1Type g_pre128FFTTbl_scale2_256[256];       // pre 128FFT lookup table for scale factor = 2.0f/256
extern BP1Type g_post128FFTTbl[256];                 // post 128FFT lookup table (real, imaginary part of e^(-j*n*PI/256), n = 0, 1, 2, 3, ..., 127

void conpre128FFTbl(BP1Type *preFFTTbl, I32 *pg_nFacExponent, float fltAfterScaleFactor);
void conpost128FFTbl(BP1Type *postFFTTbl);
WMARESULT auDctIV256Tbl(CoefType* rgiCoef,
                  BP2Type fltAfterScaleFactor,  // Used to be float type. Extra conversion needed to convert BP2Type back to float. Could hurt perf due to division. Changed to BP2Type to match v10 in anyway. 
                  U32 *piMagnitude,
                  const Int cSubbandAdjusted,
                  PFNFFT pfnFFT, Void *pFFTInfo,
                  const Int iFrameNumber,
                  const Int iCurrSubFrame,
                  const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted);	
#endif  // defined(DCT_LOOKUPTBL) && DCT_LOOKUPTBL

#ifdef FFTDCT_VAR_LOOKUPTBL
extern BP2Type g_pbp2VarTblFFT[];       // LUTs for table based FFT
extern BP1Type g_pbp1VarTblPreFFT[];    // LUTs for table based DCT
extern BP1Type g_pbp1VarTblPreFFT2[];   // LUTs for table based DCT
extern BP1Type g_pbp1VarTblPostFFT[];   // LUTs for table based DCT
extern I32 g_nFacVarTblPreFFT[];        // scale factors for g_varTblPreFFT
extern I32 g_nFacVarTblPreFFT2[];       // scale factors for g_varTblPreFFT2
WMARESULT auVarTblDCT(CoefType* rgiCoef, BP2Type fltAfterScaleFactor, U32 *piMagnitude,
                      const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo,
                      const Int iFrameNumber, const Int iCurrSubFrame,
                      const Int cFrameSampleAdjusted, const Int cSubFrameSampleAdjusted);
void prvVarTblFFT(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection);
void genVarTblFFT();
void genVarTblPreFFT(BP1Type *preFFTTbl, I32 *pg_nFacExponent, float fltAfterScaleFactor);
void genVarTblPostFFT(BP1Type *postFFTTbl);
#endif // FFTDCT_VAR_LOOKUPTBL

#if defined(CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL) && CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL
void conWinRefPre128FFTbl(BP1Type* PreCookedTab);
#if !(defined(WMA_OPT_FFT_ARM) && WMA_OPT_FFT_ARM)     
void WindowRefPre128FFT(CoefType* rgiCoef);
void FFTPost128FFTTwiddle(CoefType* rgiCoef,
                  const Int cSubbandAdjusted,
                  PFNFFT pfnFFT, Void *pFFTInfo, U32 iMagnitude); 
#endif	// !WMA_OPT_FFT_ARM
#endif  // CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL


WMARESULT auResampleMaskV3 (CAudioObject* pau, 
                            Int* rgiMaskQIn, Int cSubbandIn, Int cValidBarksIn, 
                            Int* rgiMaskQOut, Int cSubbandOut, Int cValidBarksOut);

// LPC functions
#ifdef ENABLE_ALL_ENCOPT
Void    auResampleWeightFactorLPC (CAudioObject* pau, PerChannelInfo* ppcinfo);
Void    auLsp2lpc(struct CAudioObjectDecoder* paudec, CAudioObject* pau, U8 *lsfQ, LpType *lpc, Int order);
#endif //ENABLE_ALL_ENCOPT

WMARESULT prvAllocate (CAudioObject* pau);

//private:

// *** TEMPORARY: prvAllocate moved from here to extern "C" block above ***
Void       auInitBarkResampleMatrix      (CAudioObject* pau);
WMARESULT  prvAllocateBarkResampleMatrix (struct CAudioObjectDecoder *paudec, CAudioObject* pau);
Void       prvDeleteBarkResampleMatrix   (struct CAudioObjectDecoder *paudec, CAudioObject* pau);

Void    prvSetBarkIndex (CAudioObject* pau);
Void    prvSetSubWooferCutOffs (CAudioObject* pau);
WMARESULT prvInverseQuantizeHighRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor);

Void auCheckForOverflowSamp(CoefType rgiCoefRecon,
                            CoefType iMaxOutputAllowed);
Void auCheckForOverflowCh(CoefType *rgiCoefRecon, Int cSubbandAdjusted,
                          CoefType iMaxOutputAllowed,
                          CoefType iLog2MaxOutputAllowed);
Void auCheckForOverflow(CAudioObject *pau,
                        CoefType iMaxOutputAllowed,
                        CoefType iLog2MaxOutputAllowed);

WMARESULT auPreScaleCoeffsV3(CAudioObject *pau,
                             CoefType iLog2MaxOutputAllowed,
                             Bool fUseQuantStep,
                             Bool fUseGlobalScale, char c );
WMARESULT auPreScaleQuantV3(CAudioObject *pau,
                            Bool fUseGlobalQuantScale);
WMARESULT auPostScaleCoeffsV3(CAudioObject *pau);

WMARESULT auGetTransformFromGivensFactors(I8* rgbRotationAngle, 
                                          I8* rgbRotationSign, 
                                          Int cSize,
                                          Int cMaxChannel,
                                          ChXFormType* rgfltMultiXInverse,
                                          ChXFormType* rgfltTmpCurr,
                                          ChXFormType* rgfltTmpFinal);

WMARESULT auInvWeightSpectrumV3 (CAudioObject* pau, 
                                 PerChannelInfo* ppcinfo,
                                 U8 fMaskUpdate);

Void prvGetChannelNumAtSpeakerPosition (U32 nChannelMask,
                                        U32 nSpeakerPosition,
                                        I16 *pnChannelNo);

WMARESULT prvInverseTransformMono (CAudioObject* pau, PerChannelInfo* ppcinfo,
                                   PCMSAMPLE* piOutput, PCMSAMPLE* piPrevOutput,
                                   I16* piPrevOutputSign, Bool fForceTransformAll);

// V3 Pure LLM. Chao. Add.
WMARESULT auReconMonoPureLLM ( CAudioObject* pau, 
                        PerChannelInfo* ppcinfo, 
                        PCMSAMPLE* piOutput, 
                        I16* pcSampleGet,
                        Bool fForceTransformAll,
                        I16 iCh);
WMARESULT auReconstructPureLLM (CAudioObject* pau, U8** ppbDst,
                         I16* pcSampleGet, Bool fForceTransformAll);

Void      prvForwSDWT(I32 * rgiBuf, I32 * rgiBufT, U32 iLen);
Void      prvInveSDWT(I32 * rgiBuf, I32 * rgiBufT, U32 iLen);
//V3 LLMA Chao 03-25-02
Void prvChsForwSTransform(I32 * rgiInput1, I32 * rgiInput2, I32 * rgiS, I32 * rgiD, U32 cLen);
Void prvChsInveSTransform(I32 * rgiInput1, I32 * rgiInput2, I32 * rgiS, I32 * rgiD, U32 cLen);

//V3 LLMB chao 04-24-02
void prvResetColombKinLLMB(CAudioObject * pau, PerChannelInfo * ppcinfo);

#if !defined(DISABLE_INTELFFT)
void prvFFT4DCT_INTEL(Void *pFFTInfo, CoefType data[], Int nLog2np, FftDirection fftDirection);
#ifdef ENABLE_NON_POWER_OF_TWO
void prvDFT4DCT_INTEL(Void *pDFTInfo, CoefType data[], Int nSize,   FftDirection fftDirection);
#endif
WMARESULT auInitIntelFFT(CAudioObject *pau, const int iCode);
WMARESULT auFreeIntelFFT(CAudioObject *pau, const int iCode);
#endif  // DISABLE_INTELFFT

#if !defined(DISABLE_INTELFFT)
// From x86enc.c. X86-only stuff shouldn't be called directly from msaudioenc.c,
// but no time to fix now. Fix later.
WMARESULT auInitIntelCcsFFT(CAudioObject *pau, const int iCode);
WMARESULT auFreeIntelCcsFFT(CAudioObject *pau, const int iCode);
#endif  // defined(DISABLE_INTELFFT)

#if !defined(DISABLE_INTELFFT)
WMARESULT prvInvFftComplexConjugate_INTEL(Void *hIntelFFTInfo, CoefType rgfltData[], Int nLog2np, CoefType *rgfltTmp, U32 cSizeTmp);
#ifdef ENABLE_NON_POWER_OF_TWO
WMARESULT prvInvDftComplexConjugate_INTEL(Void *hIntelDFTInfo, CoefType rgfltData[], Int nSize,   CoefType *rgfltTmp, U32 cSizeTmp);
#endif
#endif  // !defined(DISABLE_INTELFFT)

// Implemented in x86.c, lpcenc.c
WMARESULT prvInvFftComplexConjugate(Void *ptrNotUsed, CoefType *rgfltData, Int nOrder, CoefType *rgfltTmp, U32 cSizeTmp);

#if defined(WMA_TARGET_X86) && !defined (DISABLE_OPT)

#ifndef BUILD_INTEGER
WMARESULT auReconSample_X86(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);

#ifndef WMA_DISABLE_SSE1
WMARESULT auReconSample16_SSE1(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
WMARESULT auReconSample24_SSE1(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
#endif //#ifndef WMA_DISABLE_SSE1
#endif //#ifndef BUILD_INTEGER

#ifndef WMV_DISABLE_MMX
WMARESULT prvInverseTransformMono_MMX (CAudioObject* pau, Float* rgfltCoefRecon,
                                     I32* piCurrOutput, Bool fForceTransformAll);
#endif //WMV_DISABLE_MMX

#endif // defined(WMA_TARGET_X86) && !defined (DISABLE_OPT)

//#ifdef WMA_TARGET_ARM///////////////////////////////////////////////////////////////////////////////////////////////
#if WMA_OPT_AURECON_ARM
WMARESULT auReconSample16_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
WMARESULT auReconSample24_ARM(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
#endif //WMA_TARGET_ARM

#if defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_RECON)
WMARESULT auReconSample16_MIPS(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
WMARESULT auReconSample24_MIPS(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
#endif //WMA_TARGET_MIPS && PLATFORM_SPECIFIC_RECON

#if defined(_WMA_TARGET_XBOX2_)
WMARESULT auReconSample16_XENON(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
WMARESULT auReconSample24_XENON(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
WMARESULT auReconSampleFloat_XENON(CAudioObject* pau, PCMSAMPLE* piOutput, U16 cSamples);
void prvFFT4DCT_XENON(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection);
WMARESULT auDctIV_XENON(CoefType* rgiCoef, Float fltAfterScaleFactor, U32 *piMagnitude,
                  const Int cSubbandAdjusted, PFNFFT pfnFFT, Void *pFFTInfo, const Int iFrameNumber,
                  const Int iCurrSubFrame, const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted);
#endif //_WMA_TARGET_XBOX2_

#if defined(WMA_TARGET_MIPS)
	void auCheckMIPSIIProcessor(CAudioObject* pau);
#endif

Void    prvFFT_Int(Int data[], Int np);
Void    prvFFT4DCT(Void *ptrNotUsed, CoefType data[], Int nLog2np, FftDirection fftDirection);

#ifdef ENABLE_ALL_ENCOPT
//for noise sub and low rates only
WMARESULT prvInitNoiseSubSecondaryFixed (CAudioObject* pau);
WMARESULT prvInitNoiseSubSecondaryAllocated (CAudioObject* pau);
WMARESULT prvInverseQuantizeMidRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor);
WMARESULT prvInverseQuantizeLowRate (CAudioObject* pau, PerChannelInfo* ppcinfo, Int* rgiWeightFactor);

//private lpc functions
WMARESULT prvLpcToSpectrum (CAudioObject* pau, const LpType* rgfltLpcCoef, PerChannelInfo* ppcinfo);
//Void prvInitInverseQuadRootTable (struct CAudioObjectDecoder *paudec, CAudioObject* pau);
#endif //ENABLE_ALL_ENCOPT

#if defined(WMA_OPT_INV_CH_XFORM_ARM) && WMA_OPT_INV_CH_XFORM_ARM
void auInvChannelXForm_Loop(CoefType *piCoefRecon0, CoefType *piCoefRecon1, I16 iCount);
#endif

#ifndef PREVOUTPUT_16BITS
// **************************************************************************
// Signbit Macros
// I've put them here because they're used in both msaudio.c and float.c
// **************************************************************************
#define I17_MIN ((signed) 0xFFFF0000)
#define I17_MAX ((signed) 0x0000FFFF)

#define OVERLAPADD_MIN  PCMSAMPLE_MIN(pau->m_nValidBitsPerSample + 1)
#define OVERLAPADD_MAX  PCMSAMPLE_MAX(pau->m_nValidBitsPerSample + 1)

#define CALC_SIGN_OFFSETS16(piSrcBase, piSrc, piSrcSignBase, piSrcSign,         \
                           uiSignbitData, uiSignbitCount, cChan)                \
    {                                                                           \
    const int c_iOffset = (piSrc - piSrcBase) / cChan;                          \
    const int c_iSignOffset = (c_iOffset+((BITS_PER_BYTE * sizeof(I16))-1))     \
                            / (BITS_PER_BYTE * sizeof(I16));                    \
    assert(2 == pau->m_nBytePerSample);                                         \
    piSrcSign = piSrcSignBase + c_iSignOffset;                                  \
    uiSignbitCount = 16 - (c_iOffset % (BITS_PER_BYTE * sizeof(I16)));          \
    if (uiSignbitCount == 16)                                                   \
        uiSignbitData = *piSrcSign++;                                           \
    else                                                                        \
        uiSignbitData = *(piSrcSign-1);                                         \
    }

#define CALC_SIGN_OFFSETS(piSrcBase, piSrc, piSrcSignBase, piSrcSign,           \
                           uiSignbitData, uiSignbitCount, cChan)                \
    {                                                                           \
    const int c_iOffset = prvCountSamples(piSrc, piSrcBase, pau, cChan);        \
    const int c_iSignOffset = (c_iOffset+((BITS_PER_BYTE * sizeof(I16))-1))     \
                            / (BITS_PER_BYTE * sizeof(I16));                    \
    const unsigned int c_iBitsPerSignbitReg = (BITS_PER_BYTE * sizeof(I16));    \
    piSrcSign = piSrcSignBase + c_iSignOffset;                                  \
    uiSignbitCount = c_iBitsPerSignbitReg - (c_iOffset % c_iBitsPerSignbitReg); \
    if (uiSignbitCount == c_iBitsPerSignbitReg)                                 \
        uiSignbitData = *piSrcSign++;                                           \
    else                                                                        \
        uiSignbitData = *(piSrcSign-1);                                         \
    }

// Calculate sign offset, but don't load signbit register. For use by auSaveHistoryMono.
#define CALC_SIGN_OFFSETS_NOLOAD(piSrcBase, piSrc, piSrcSignBase, piSrcSign, cChan) \
    {                                                                           \
    const int c_iOffset = prvCountSamples(piSrc, piSrcBase, pau, cChan);        \
    const int c_iSignOffset = (c_iOffset+((BITS_PER_BYTE * sizeof(I16))-1))     \
                            / (BITS_PER_BYTE * sizeof(I16));                    \
    piSrcSign = piSrcSignBase + c_iSignOffset;                                  \
    }

#define RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData, uiSignbitCount, incr) \
    uiSignbitData <<= 1;                                                        \
    uiSignbitData |= ((iResult >> 31) & 0x00000001);                            \
    uiSignbitCount += 1;                                                        \
    if (uiSignbitCount >= (BITS_PER_BYTE * sizeof(I16)))                        \
    {   /* Signbit register is full! Write it out */                            \
        *piDstSign = (I16) uiSignbitData;                                       \
        piDstSign += incr;                                                      \
        DEBUG_ONLY( uiSignbitData = 0 );                                        \
        uiSignbitCount = 0;                                                     \
    }

#define SIGNBIT_MASK(iValidBits)    (~((1 << iValidBits) - 1))

#define COMBINE_SIGNBIT(iResult, iSrc, piSrcSign, uiSignbitData, uiSignbitCount, iValidBits) \
    if (0 == uiSignbitCount)                                                     \
    {   /* Signbit register is empty! Read new one in */                         \
        uiSignbitData = *piSrcSign;                                              \
        piSrcSign += 1;                                                          \
        uiSignbitCount = 16;                                                     \
    }                                                                            \
    if (uiSignbitData & 0x00008000)                                              \
        iResult = (-1) & SIGNBIT_MASK(iValidBits);                               \
    else                                                                         \
        iResult = 0;                                                             \
    iResult |= (iSrc & ~SIGNBIT_MASK(iValidBits));                               \
    uiSignbitData <<= 1;                                                         \
    uiSignbitCount -= 1;

#define SAVE_SIGNBIT_STATE(piSrcSign,uiSignbitData)                              \
    if (0 != uiSignbitCount)                                                     \
        *(piSrcSign-1) = (I16)uiSignbitData;    /* must save partial shift */

#define ASSERT_SIGNBIT_POINTER(piSrcOrDstSign)                                   \
    assert( piSrcOrDstSign != NULL );

#else   // PREVOUTPUT_16BITS

// Use I16 min/max in place of I17
#define I17_MIN SHRT_MIN
#define I17_MAX SHRT_MAX

#define CALC_SIGN_OFFSETS16(piSrcBase, piSrc, piSrcSignBase, piSrcSign,          \
                           uiSignData, uiSignCount, cChan)                             

#define CALC_SIGN_OFFSETS(piSrcBase, piSrc, piSrcSignBase, piSrcSign,            \
                           uiSignData, uiSignCount, cChan)                             
#define RECORD_SIGNBIT(iResult, piDstSign, uiSignbitData, uiSignbitCount, incr)
#define COMBINE_SIGNBIT(iResult, iSrc, piSrcSign, uiSignbitData, uiSignbitCount, \
                        iValidBits) \
    iResult = iSrc
#define SAVE_SIGNBIT_STATE(piSrcSign,uiSignData)
#define ASSERT_SIGNBIT_POINTER(piSrcOrDstSign)

#endif  // PREVOUTPUT_16BITS

#define INTERPOL_FRACTBITS  30

void auLowPass(CAudioObject *pau, CoefType *rgiCoef, Int iNumCoefs,
               Int iPassThruCoefs);

#ifndef BUILD_INTEGER_LPC
// do not scale WeightFactor when using float
#undef WEIGHTFACTOR_FRACT_BITS
#define WEIGHTFACTOR_FRACT_BITS 0
#endif

//Void auCalcQuantStep(Int iQuantStepSize, U32 *piQuantStep, U32 *piFractBits);
QuantStepType qstCalcQuantStep( const Int iQuantStepSize, const Int cLeftShiftBits );
QuantFloat prvWeightedQuantization(CAudioObject *pau, PerChannelInfo *ppcinfo, Int iBark);

void SetActualPower(const I32 *piCoefQ, const I32 iCount,
                    PerChannelInfo *ppcinfo, const Status codecStatus);
void SetActualPowerHighRate(const CBT *piCoefRecon, const int iCount,
                    PerChannelInfo *ppcinfo, const Status codecStatus);


// **************** Monitor Range of Values for Debugging ******************************
// Place MONITOR_RANGE(name,variable) in the code where you want to monitor a variable,
// and use DUMP_MONITOR_RANGES(0) in the exit code where you want to print out the results
// If you want to dump on a frame-by-frame basis, use REINIT_MONITOR_RANGE after the dump
// Finally, add an appropriate DECL_MONITOR_RANGE(name) in wmaOS.c 
// and add DUMP_MONITOR_RANGE(name) to the DumpMonitorRanges() function there.
// **************************************************************************************
#if defined(_DEBUG) && defined(WMA_MONITOR)
// some MONITOR_RANGE variations can be defined.
// First variation is to print the variable to stdout
//# define WMA_MONITOR_PRINT "gMR_rgfltWeightFactor"
// Second variation allows you to break when the fabs(variable) exceeds a threshold
//# define WMA_MONITOR_BREAK "gMR_rgfltWeightFactor"
//# define WMA_MONITOR_BREAK_THRESHOLD 50
// both of these variations are slow because strcmp is done at runtime instead of compile time - but this is debug code.
#   ifdef WMA_MONITOR_PRINT
#       define MONITOR_RANGE(a,x) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; \
                                    if (strcmp(#a,WMA_MONITOR_PRINT)==0) WMAPrintf("%14.6g\n",(double)(x));  }
#   elif defined(WMA_MONITOR_BREAK)
#       define MONITOR_RANGE(a,x) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; \
                                    if (strcmp(#a,WMA_MONITOR_BREAK)==0 && fabs(x)>WMA_MONITOR_BREAK_THRESHOLD) DEBUG_BREAK();  }
#   else
#       define MONITOR_RANGE(a,x) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; }
#   endif
#   define MONITOR_RANGE_CONDITIONAL(cond,a,x) if (cond) { extern double a[]; extern long lc##a; if ((double)(x)<a[0]) a[0]=(double)(x); if ((double)(x)>a[1]) a[1]=(double)(x); a[2] += (x); a[3] += (x)*(x); lc##a++; }
#   define MONITOR_COUNT(a,c) { extern long lc##a; lc##a+=c; }
#   define MONITOR_COUNT_CONDITIONAL(cond,a,c) if (cond) { extern long lc##a; lc##a+=c; }
#   define DUMP_MONITOR_RANGES(fAppend) { void DumpMonitorRanges(int f); DumpMonitorRanges(fAppend); }
#   define REINIT_MONITOR_RANGE(a) { extern double a[]; extern long lc##a; a[1] = -( a[0] = 1.0e30 );  a[2] = a[3] = 0; lc##a = 0; }
#   define DUMP_MONITOR_RANGE(a) { extern double a[]; extern long lc##a; if ( a[0] == 1.0e30 && a[3]==0 ) { a[0] = a[1] = 0; } if (lc##a>0) WMAPrintf("%14.6g %14.6g %14.6g %14.6g %8ld " #a "\n", a[0], a[1], a[2]/lc##a, sqrt((lc##a*a[3]-a[2]*a[2])/((double)(lc##a)*(lc##a-1))), lc##a ); }
#else // not (defined(_DEBUG) && defined(WMA_MONITOR))
#   define MONITOR_RANGE(a,x)
#   define MONITOR_RANGE_CONDITIONAL(cond,a,x)
#   define MONITOR_COUNT(a,c)
#   define MONITOR_COUNT_CONDITIONAL(cond,a,c)
#   define DUMP_MONITOR_RANGES(fAppend)
#   define REINIT_MONITOR_RANGE(a)
#   define DUMP_MONITOR_RANGE(a) 
#endif

#if defined(WMA_SHOW_FRAMES) && defined(_DEBUG)
    void prvWmaShowFrames(const Int iFrameNumber, const Int iCurrSubFrame,
        const Int cFrameSampleAdjusted, const Int cSubFrameSampleAdjusted,
        const Int cSubbandAdjusted, const char* pszID, const char* pszFmt, ...);
#endif

#if defined(HEAP_DEBUG_TEST) && defined(_DEBUG)
void HeapDebugCheck();
#define HEAP_DEBUG_CHECK HeapDebugCheck()
#else
#define HEAP_DEBUG_CHECK
#endif

#ifdef TEST_CORRUPT_BITSTREAM
    #define CORRUPT_PROBABILITY 100
    #define REPORT_BITSTREAM_CORRUPTION() {WMAPrintf("We encountered a bit stream corruption!\n"); assert(WMAB_FALSE);}
#else
    //#define BITSTREAM_CORRUPTION_ASSERTS
    #ifdef BITSTREAM_CORRUPTION_ASSERTS
        #define REPORT_BITSTREAM_CORRUPTION() assert (WMAB_FALSE)
    #else
        #define REPORT_BITSTREAM_CORRUPTION()
    #endif
#endif //TEST_CORRUPT_BITSTREAM

#define REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wr) { \
  REPORT_BITSTREAM_CORRUPTION(); \
  wr = TraceResult(WMA_E_BROKEN_FRAME); \
  goto exit; \
}

#define NO_ERROR_REPORT_BITSTREAM_CORRUPTION_AND_EXIT { \
  REPORT_BITSTREAM_CORRUPTION(); \
  goto exit; \
}

#include "playropt.h"

#define MSAUDIO_PEAKANDRMS_CMP


//***************************************************************************
// Encoder/Decoder Separation
//***************************************************************************

#define PRECESSION_BITS_FOR_INVQUADROOT 12

//#if defined(_DEBUG) && defined(LPC_COMPARE)
//void prvSetupLpcSpecTrig(CAudioObjectDecoder *paudec, CAudioObject* pau);
//#define SETUP_LPC_SPEC_TRIG(pau) prvSetupLpcSpecTrig(paudec, pau)
//#else
//#define SETUP_LPC_SPEC_TRIG(pau) 
//#endif


//***************************************************************************
// 24-bit Encoding
//***************************************************************************

void prvInitGetSetFn(PFNGETSAMPLE *ppfnGetSample,
                     PFNSETSAMPLE *ppfnSetSample,
                     const BITDEPTH iBitDepthSelector);

INLINE PCMSAMPLE *prvAdvancePCMPointer(const PCMSAMPLE *pCurrentPos,
                                const Int nBytePerSample,
                                const Int iOffset)
{
    return (PCMSAMPLE *) (((U8*) pCurrentPos) + (nBytePerSample * iOffset));
}


INLINE PCMSAMPLE *prvAdvancePCMPointer16(const PCMSAMPLE *pCurrentPos,
                                const Int nBytePerSample,
                                const Int iOffset)
{
    return (PCMSAMPLE *) ((I16*) pCurrentPos + iOffset);
}


INLINE PCMSAMPLE *prvAdvancePCMPointer24(const PCMSAMPLE *pCurrentPos,
                                const Int nBytePerSample,
                                const Int iOffset)
{
    return (PCMSAMPLE *) ((U8*) pCurrentPos + 3*iOffset);
}

// =========================================
#ifdef DEVICEPCMOUT16
#define PAUPRVADVANCEPCMPTR(pCurrentPos, iOffset) \
  prvAdvancePCMPointer((pCurrentPos), paudec->m_nDstBytePerSample, (iOffset))/* Witten at 09-04-20*/
#else	//DEVICEPCMOUT16
#define PAUPRVADVANCEPCMPTR(pCurrentPos, iOffset) \
  prvAdvancePCMPointer((pCurrentPos), pau->m_nBytePerSample, (iOffset))
#endif	//DEVICEPCMOUT16

#define PAUPFNGETSAMPLE(pCurrentPos, iOffset) \
  pau->m_pfnGetSample((pCurrentPos), pau->m_nBytePerSample, \
                      pau->m_nValidBitsPerSample, (iOffset))

#define PAUPFNGETSAMPLE0(pCurrentPos) \
  PAUPFNGETSAMPLE((pCurrentPos), 0)

#define PAUPFNSETSAMPLE(iValue, pCurrentPos, iOffset) \
  pau->m_pfnSetSample((iValue), (pCurrentPos), pau, (iOffset))

#define PAUPFNSETSAMPLE0(iValue, pCurrentPos) \
  PAUPFNSETSAMPLE((iValue), (pCurrentPos), 0)

// ========================================

INLINE BITDEPTH prvBitDepthSelector(int nBytePerSample,
                                           int nValidBitsPerSample)
{
  return  (BITDEPTH)((nBytePerSample - 1) | ((nValidBitsPerSample - 1) << 2));
}

Int prvCountSamples(const PCMSAMPLE *pCurrentPos,
                    const PCMSAMPLE *pBasePos,
                    const CAudioObject *pau,
                    const Int iChannels);

INLINE void prvSetFloatSampleFloatBuf(const Float fltValue,
                                             PCMSAMPLE *pCurrentPos,
                                             const CAudioObject *pau,
                                             const Int iOffset)
{
    Float *pfltDst = ((Float*) pCurrentPos) + iOffset;

    *pfltDst = fltValue;
} // prvSetFloatSampleFloatBuf


INLINE Float prvGetFloatSampleFloatBuf(const PCMSAMPLE *pCurrentPos,
                                              const Int nBytePerSample,
                                              const Int nValidBitsPerSample,
                                              const Int iOffset)
{
    // nBytePerSample & nValidBitsPerSample are not used
    return ((Float*)pCurrentPos)[iOffset];
} // prvGetFloatSampleFloatBuf

void prvSetPCMSampleFloatBuf(const PCMSAMPLE iValue,
                             PCMSAMPLE *pCurrentPos,
                             const CAudioObject *pau,
                             const Int iOffset);

PCMSAMPLE prvGetPCMSampleFloatBuf(const PCMSAMPLE *pCurrentPos,
                                  const Int nBytePerSample,
                                  const Int nValidBitsPerSample,
                                  const Int iOffset);


INLINE PCMSAMPLE prvGetSample16INLINE(const PCMSAMPLE *pCurrentPos,
                                             const Int nBytePerSample,
                                             const Int nValidBitsPerSample,
                                             const Int iOffset)
{
    U8 *pbSrc = (U8*) pCurrentPos;

    pbSrc += 2 * iOffset; // Skip iOffset worth of samples
    return *((I16*)pbSrc);
}

INLINE void prvSetSample16INLINE(const PCMSAMPLE iValue,
                    PCMSAMPLE *pCurrentPos,
                    const CAudioObject *pau,
                    const Int iOffset)
{
    U8 *pbDst = (U8*) pCurrentPos;
    U8 *pbSrc = (U8*) &iValue;

    pbDst += 2 * iOffset; // Skip iOffset worth of samples
//#ifdef BIG_ENDIAN
   // *((I16*)pbDst) = *((I16*)(pbSrc+2));
//#else
    *((I16*)pbDst) = *((I16*)pbSrc);
//#endif
}


INLINE PCMSAMPLE prvGetSample24INLINE(const PCMSAMPLE *pCurrentPos,
                                             const Int nBytePerSample,
                                             const Int nValidBitsPerSample,
                                             const Int iOffset)
{
    U8 *pbSrc = (U8*) pCurrentPos;
    I32 iResult;

    pbSrc += 3 * iOffset; // Skip iOffset worth of samples
#if defined (BIG_ENDIAN)
    iResult = (*((I8*)pbSrc) << 16 ) | (*((U16*)(pbSrc+1)));
#elif defined (UNDER_CE) || defined(LINUX)
    iResult = (*((U8*)pbSrc) | (*((U8*)(pbSrc + 1)) << 8) | (*((I8*)(pbSrc + 2)) << 16));
#else
    iResult = (*((U16*)pbSrc) | (*((I8*)(pbSrc + 2)) << 16));
#endif
    return iResult;
}


INLINE PCMSAMPLE prvGetSample24INLINE32(const PCMSAMPLE *pCurrentPos,
                                               const Int nBytePerSample,
                                               const Int nValidBitsPerSample,
                                               const Int iOffset)
{
    U8 *pbSrc = (U8*) pCurrentPos;
    I32 iResult;

    pbSrc += 3 * iOffset; // Skip iOffset worth of samples
    iResult = (*((U16*)pbSrc) | (*((I8*)(pbSrc + 2)) << 16));
    iResult <<= 8;
    //iResult >>= 8;

    return iResult;
}

PCMSAMPLE INLINE prvGetSample2024INLINE(const PCMSAMPLE *pCurrentPos,
                                               const Int nBytePerSample,
                                               const Int nValidBitsPerSample,
                                               const Int iOffset)
{
    U8 *pbSrc = (U8*) pCurrentPos;
    I32 iResult;

    pbSrc += 3 * iOffset; // Skip iOffset worth of samples
    iResult = (*((U16*)pbSrc) | (*((I8*)(pbSrc + 2)) << 16));

    // This assert fired when encoding content with a bit depth of 24 to 20-bits
    // In such cases we will always throw away the 4 least significant bits as is 
    // done in prvGetSample
    // assert(0 == (iResult & 0x0F)); // Verify that 4 least signficant bits always zero

    iResult >>= 4; // Dump the 4 least-significant bits (should always be zero)
    return iResult;
}


PCMSAMPLE INLINE prvGetSample2024INLINE32(const PCMSAMPLE *pCurrentPos,
                                                 const Int nBytePerSample,
                                                 const Int nValidBitsPerSample,
                                                 const Int iOffset)
{
    U8 *pbSrc = (U8*) pCurrentPos;
    I32 iResult;

    pbSrc += 3 * iOffset; // Skip iOffset worth of samples
    iResult = (*((U16*)pbSrc) | (*((I8*)(pbSrc + 2)) << 16));

    assert(0 == (iResult & 0x0F)); // Verify that 4 least signficant bits always zero

    // Sign extension
    iResult <<= 8;
    //iResult >>= 12; // Dump the 4 least-significant bits (should always be zero)

    return iResult;
}


INLINE void prvSetSample24INLINE(const PCMSAMPLE iValue,
                    PCMSAMPLE *pCurrentPos,
                    const CAudioObject *pau,
                    const Int iOffset)
{
    U8 *pbDst = (U8*) pCurrentPos;
    U8 *pbSrc = (U8*) &iValue;

    pbDst += 3 * iOffset; // Skip iOffset worth of samples
#if defined (BIG_ENDIAN)
    *((I8*)pbDst) = *((I8*)pbSrc + 0);
    *((I8*)(pbDst + 1)) = *((I8*)(pbSrc + 1));
    *((I8*)(pbDst + 2)) = *((I8*)(pbSrc + 2));
#elif defined (UNDER_CE) || defined(LINUX)
    *((I8*)pbDst) = *((I8*)pbSrc);
    *((I8*)(pbDst + 1)) = *((I8*)(pbSrc + 1));
    *((I8*)(pbDst + 2)) = *((I8*)(pbSrc + 2));
#else
    *((I16*)pbDst) = *((I16*)pbSrc);
    *((I8*)(pbDst + 2)) = *((I8*)(pbSrc + 2));
#endif
}

INLINE void prvSetSample24OUT16INLINE(const PCMSAMPLE iValue,
                    PCMSAMPLE *pCurrentPos,
                    const CAudioObject *pau,
                    const Int iOffset)
{
    U8 *pbDst = (U8*) pCurrentPos;
	int temp = (iValue >> 8);
#ifdef BIG_ENDIAN
	U8 *pbSrc = NULL;	
	pbSrc = (U8*) &temp;
#endif
    pbDst += 2 * iOffset; // Skip iOffset worth of samples
//	printf("pbDst addr: 0x%.8x, pbSrc: 0x%.4x\n", (int)pbDst, *pbSrc);
//#ifdef BIG_ENDIAN
   // *((I16*)pbDst) = *((I16*)(pbSrc+2));
//#else
    *((I16*)pbDst) = (I16)temp;//*((I16*)pbSrc);
//#endif
}

void INLINE prvSetSample2024INLINE(const PCMSAMPLE iValue,
                      PCMSAMPLE *pCurrentPos,
                      const CAudioObject *pau,
                      const Int iOffset)
{
    I32 iNewValue;
    U8 *pbDst = (U8*) pCurrentPos;
    U8 *pbSrc = (U8*) &iNewValue;

    // We used to check if incoming value was < 20-bit: don't, because auSaveHistory
    // can feed us values > 20-bit. Expectation is that we truncate to 20 bits.
    iNewValue = (iValue << 4);

    pbDst += 3 * iOffset; // Skip iOffset worth of samples
    //*((I16*)pbDst) = *((I16*)pbSrc);
	*((I8*)pbDst) = *((I8*)pbSrc);
	*((I8*)(pbDst + 1)) = *((I8*)(pbSrc + 1));
    *((I8*)(pbDst + 2)) = *((I8*)(pbSrc + 2));
}

//*****************************************************************************************
// Forward Function Declarations
//*****************************************************************************************

void prvSetSample(const PCMSAMPLE iValue, PCMSAMPLE *pCurrentPos,
                  const CAudioObject *pau, const Int iOffset);
void prvSetSampleFloat(const Float fltValue, PCMSAMPLE *pCurrentPos,
                       const CAudioObject *pau, const Int iOffset);
void prvSetSample16(const PCMSAMPLE iValue, PCMSAMPLE *pCurrentPos,
                    const CAudioObject *pau, const Int iOffset);
void prvSetSample24(const PCMSAMPLE iValue, PCMSAMPLE *pCurrentPos,
                    const CAudioObject *pau, const Int iOffset);
void prvSetSample2024(const PCMSAMPLE iValue,
                      PCMSAMPLE *pCurrentPos,
                      const CAudioObject *pau,
                      const Int iOffset);

PCMSAMPLE prvGetSample(const PCMSAMPLE *pCurrentPos,
                       const Int nBytePerSample,
                       const Int nValidBitsPerSample,
                       const Int iOffset);
PCMSAMPLE prvGetSample16(const PCMSAMPLE *pCurrentPos,
                         const Int nBytePerSample,
                         const Int nValidBitsPerSample,
                         const Int iOffset);
PCMSAMPLE prvGetSample24(const PCMSAMPLE *pCurrentPos,
                         const Int nBytePerSample,
                         const Int nValidBitsPerSample,
                         const Int iOffset);
PCMSAMPLE prvGetSample2024(const PCMSAMPLE *pCurrentPos,
                           const Int nBytePerSample,
                           const Int nValidBitsPerSample,
                           const Int iOffset);

//************************************************************************

//#include <math.h>
#ifndef FFLT_SQRT_RATIO
    INLINE FastFloat ffltSqrtRatio( FastFloat fflt1, FastFloat fflt2 )
    {
#       if defined(BUILD_INTEGER)
            U64 u64Ratio;
            FastFloat fflt;
            Int iMSF8;
            U32 uiMSF;
            U32 uiFrac1;
            Int iExp = 0;
            if ( fflt2.iFraction==0 ) {
                assert( fflt2.iFraction != 0 ); // divide by 0
                fflt.iFraction = 0x7FFFFFFF;
                fflt.iFracBits = 0;
                return fflt;
            }
            //// fflt1.iFraction*0.5F/(1<<(fflt1.iFracBits-1))
            //// fflt2.iFraction*0.5F/(1<<(fflt2.iFracBits-1))
            u64Ratio = (((U64)fflt1.iFraction)<<32)/fflt2.iFraction;
            uiMSF = (U32)(u64Ratio>>32);
            if ( uiMSF==0 ) {
                iExp = 32;
                uiMSF = (U32)u64Ratio;
            }
            assert( uiMSF != 0 ) ;
            // normalize the most significant fractional part
            while( (uiMSF & 0xF0000000)==0 ) {
                iExp += 4;
                uiMSF <<= 4;
            }
            while( (uiMSF & 0x80000000)==0 ) {
                iExp++;
                uiMSF <<= 1;
            }
            // discard the most significant one bit (it's presence is built into g_InvQuadRootFraction)
            iExp++;
            // get all 32 bits from source
#           if defined(PLATFORM_OPTIMIZE_MINIMIZE_BRANCHING)
                uiMSF = (U32)((u64Ratio<<iExp)>>32);
#           else
                uiMSF = (iExp>32) ? (U32)(u64Ratio<<(iExp-32)) : (U32)(u64Ratio>>(32-iExp));
#           endif
            // split into top SQRT_FRACTION_TABLE_LOG2_SIZE==8 bits for fractional lookup and bottom bits for interpolation
            iMSF8 = uiMSF>>(32-SQRT_FRACTION_TABLE_LOG2_SIZE);
            uiMSF <<= SQRT_FRACTION_TABLE_LOG2_SIZE;
            // lookup and interpolate
            uiFrac1  = g_SqrtFraction[iMSF8++];   // BP2
            uiFrac1 += MULT_HI_UDWORD( uiMSF,  g_SqrtFraction[iMSF8] - uiFrac1 );
            // adjust by sqrt(1/2) if expoenent is odd
            if ( (iExp+fflt1.iFracBits-fflt2.iFracBits) & 1 ) {
                // multiply by 1/sqrt(2) and adjust fracbits by 1/2
                uiFrac1 = MULT_HI_UDWORD( uiFrac1, UBP0_FROM_FLOAT(0.70710678118654752440084436210485) );
                fflt.iFracBits = ((fflt1.iFracBits-(fflt2.iFracBits+1))>>1)+(iExp-3);
            } else {
                fflt.iFracBits = ((fflt1.iFracBits-fflt2.iFracBits)>>1)+(iExp-3);
            }
            fflt.iFraction = uiFrac1>>1;    // make sure sign is positive
            //// fflt.iFraction*0.5F/(1<<(fflt.iFracBits-1))
            Norm4FastFloatU( &fflt );
#           if defined(_DEBUG) && 0
            {   // old way used float
                Float flt = (Float)sqrt( FloatFromFastFloat(fflt1) / FloatFromFastFloat(fflt2) );
                if ( fabs( flt - FloatFromFastFloat(fflt) ) > 0.01 ) {
                    DEBUG_BREAK();
                }
            }
#           endif
            return fflt;
#       else
            return (Float)sqrt( fflt1 / fflt2 );
#       endif
    }
#   define FFLT_SQRT_RATIO(fflt1,fflt2) ffltSqrtRatio(fflt1,fflt2)
#endif


//************************************************************************

Int prvGetPlusVersion(Int uAdvancedEncodeOpt);
WMARESULT prvInitCommonParameters(CAudioObject *pau,
                                  Int           iVersionNumber,
                                  Bool          fUES,
                                  Int           cSubband, 
                                  Int           cSamplePerSec, 
                                  U16           cChannel, 
                                  U32           nBytePerSample,
                                  U16           nValidBitsPerSample,
                                  U32           nChannelMask,
                                  Int           cBytePerSec, 
                                  Int           cbPacketLength,
                                  U16           iEncodeOpt,
                                  U16           uAdvancedEncodeOpt,
                                  U32           uAdvancedEncodeOpt2,
                                  Int           iOutputSamplingRate,
                                  PCMData       dataType,
                                  WMAPlayerInfo *pWmaPlayerInfo);
WMARESULT prvInitCommonSecondaryFixed(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
WMARESULT prvInitCommonAllocate(struct CAudioObjectDecoder *paudec, CAudioObject *pau, Bool bEncoder);
WMARESULT prvInitCommonSecondaryAllocated(CAudioObject *pau);
WMARESULT prvInitCommonResetOperational(CAudioObject *pau);
WMARESULT prvInitCommonSetFunctionPtrs(CAudioObject *pau);
Void auDeletePcInfoCommon (struct CAudioObjectDecoder *paudec, CAudioObject* pau, PerChannelInfo* rgpcinfo);
WMARESULT prvAllocatePcInfoCommon(struct CAudioObjectDecoder *paudec, CAudioObject* pau, PerChannelInfo* rgpcinfo);
WMARESULT prvResetPcInfoCommon(CAudioObject* pau, PerChannelInfo* rgpcinfo);

WMARESULT prvAllocChannelGrpInfo(struct CAudioObjectDecoder *paudec, CChannelGroupInfo **prgChannelGrpInfo, Int cChannel);
Void prvDeleteChannelGrpInfo(struct CAudioObjectDecoder *paudec, CChannelGroupInfo **prgChannelGrpInfo, Int cChannel);


/*****************************************************************************
    V3 LLM Lossless functions Chao
*****************************************************************************/
// Noise feedback coding.
Void prvNoiseFeedbackFilterEncode(I32 *rgiInput, I32 iLen, I16 iQuant);
Void prvNoiseFeedbackFilterDecode(CBT *rgiInput, I32 iLen, I16 iQuant);

WMARESULT prvInitLMSPredictor(struct CAudioObjectDecoder *paudec, LMSPredictor * pLMSPrdtr, U16 iMaxOrder, U8 * pMem, I32 * pcbMemUsed, I32 cbMemTtl);
Void prvResetLMSPredictor(CAudioObject * pau, LMSPredictor * pLMSPrdtr);
Void    prvLMSPredictorUpdateSpeedChange(CAudioObject * pau, LMSPredictor * pLMSPredictor, Bool bFast);

I32     prvLMSPredictorPred_I16_C(LMSPredictor * pLMSPredictor);
Void    prvLMSPredictorUpdate_I16_C(CAudioObject * pau, LMSPredictor * pLMSPredictor, I32 iInput, I32 iPred);
I32     prvLMSPredictorPred_I16_MMX(LMSPredictor * pLMSPredictor);
Void    prvLMSPredictorUpdate_I16_MMX(CAudioObject * pau, LMSPredictor * pLMSPredictor, I32 iInput, I32 iPred);
I32     prvLMSPredictorPred_I32_C(LMSPredictor * pLMSPredictor);
Void    prvLMSPredictorUpdate_I32_C(CAudioObject * pau, LMSPredictor * pLMSPredictor, I32 iInput, I32 iPred);
// I32     prvLMSPredictorPred_I32_MMX(LMSPredictor * pLMSPredictor);
// Void    prvLMSPredictorUpdate_I32_MMX(CAudioObject * pau, LMSPredictor * pLMSPredictor, I32 iInput, I32 iPred);
Void    prvFreeLMSPredictor(struct CAudioObjectDecoder *paudec, LMSPredictor * pLMSPrdtr);

WMARESULT prvInitMCLMSPredictor(struct CAudioObjectDecoder *paudec, CAudioObject * pau, MCLMSPredictor * pMCLMSPrdtr, I16 iMaxOrder);
Void    prvMCLMSPredictorReset(CAudioObject * pau, MCLMSPredictor * pMCLMSPrdtr);
Void    prvMCLMSPredictorPred_I16_C(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT *rgiInput, CBT *rgiPred, Bool bDecoder);
Void    prvMCLMSPredictorUpdate_I16_C(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT * rgiInput, CBT *rgiPred);
Void    prvMCLMSPredictorPred_I32_C(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT *rgiInput, CBT *rgiPred, Bool bDecoder);
Void    prvMCLMSPredictorUpdate_I32_C(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT * rgiInput, CBT *rgiPred);
Void    prvFreeMCLMSPredictor(struct CAudioObjectDecoder *paudec, MCLMSPredictor * pMCLMSPrdtr);

Void    prvDecMCLMSPredictorPredAndUpdate_I16(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT * rgiInput, CBT *rgiPred);
Void    prvDecMCLMSPredictorPredAndUpdate_I32(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT * rgiInput, CBT *rgiPred);

#if defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_LPCLSL)
Void    prvDecMCLMSPredictorPredAndUpdate_I16_MIPSII(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT * rgiInput, CBT *rgiPred);
Void    prvDecMCLMSPredictorPredAndUpdate_I32_MIPSII(CAudioObject * pau, MCLMSPredictor * pMCLMSPredictor, CBT * rgiInput, CBT *rgiPred);
#endif

Void prvLLMVerBResetAllCom(CAudioObject * pau, PerChannelInfo * ppcInfoComms);
WMARESULT prvLLMVerBResetAllCom_MLLM(CAudioObject * pau, PerChannelInfo * ppcInfoComms);

Void prvBackupFiltersLLM_VERB(CAudioObject * pau, PerChannelInfo * ppcInfoComms);
//Void prvRestoreFiltersLLM_VERB(CAudioObject * pau, PerChannelInfo * ppcInfoComms);

Void prvColombCodingBackup(CAudioObject * pau, PerChannelInfo * ppcInfoComms);
Void prvColombCodingRestore(CAudioObject * pau, PerChannelInfo * ppcInfoComms);

//V3 LLMB. leave here temporarily.
Void prvSubFrameQuantize(CAudioObject *pau);
Void prvSubFrameDeQuantize(CAudioObject *pau);

#ifdef LLM_RLS_EXPT
WMARESULT prvInitRLSPredictor(RLSPredictor * pRLSPrdtr, U16 iOrder, Float fltLamda, Float fltDelta);
WMARESULT prvResetRLSPredictor(RLSPredictor * pRLSPrdtr, Bool fClearHistory);
Float     prvRLSPredictorPred(RLSPredictor * pRLSPredictor);
Void      prvRLSPredictorUpdate(RLSPredictor * pRLSPredictor, Float fltInput, Float fltPred);
Void      prvFreeRLSPredictor(RLSPredictor * pRLSPrdtr);
#endif //LLM_RLS_EXPT

/* End of lossless */


WMARESULT prvMatrixMultiply(Float * rgMatrixInput1, Float * rgMatrixInput2, Float * rgMatrixOutput, 
                            const Int cSize1, const Int cSize2, const Int cSize3);

//INLINE Float auFabs(const Float fltIn)
//{
//    Float fltOut = fltIn;
//    I32 iFltAbs = ((*((Int*)(&fltOut))) & 0x7fffffff);
//
//    fltOut = *((Float*)(&iFltAbs));
//    return fltOut;
//}

// Arithmetic coder stuff =========================================

INLINE Int zeroPerToContext(Float nzp)
{
  if (nzp >= 0.90) return 0;
  if (nzp >= 0.80) return 1;
  if (nzp >= 0.60) return 2;
  return 3;
}

INLINE Int nonRLLevelContext(Int prevLevel2, Int prevLevel)
{
  if (prevLevel==0 && prevLevel2==0) {
    return 0; // 0
  } else if (prevLevel < 2) {
    return prevLevel + 1; // 1, 2
  } else {
    return 3; // 3
  }
}

INLINE Int runContext(Int zeroPerIndex, Int prevRun)
{
  return zeroPerIndex*2 + (prevRun==0 ? 0 : 1);
}

INLINE Int levelContext(Int prevLevel2, Int prevRun, Int prevLevel, Int curRun)
{
#define MRUN (1)
  if (curRun > MRUN && prevRun > MRUN) return 0; // 0
  else if (prevRun > MRUN) { // only current run small
    if (prevLevel==1 || prevLevel==2) return prevLevel; // 1, 2
    else return 3; // 3
  }
  else { // 2 small runs
    if (prevLevel2==1 && prevLevel==1)
      return 4; // 4
    else if (prevLevel==1 || prevLevel==2)
      return prevLevel + 4; // 5, 6
    else
      return 7; // 7
  }
#undef MRUN
}

INLINE Int signContext(Int prevSign2, Int prevRun, Int prevSign, Int curRun)
{
#define MRUN (1)
  if (curRun > MRUN && prevRun > MRUN) return 0; // 0
  else if (prevRun > MRUN) { // current run <= MRUN
    return (prevSign ? 1 : 0) + 1; // 1, 2
  }
  else {
    return (prevSign2 ? 1 : 0)*2 + (prevSign ? 1 : 0) + 3; // 3,4,5,6
  }
#undef MRUN
}

//*****************************************************************************************
//
// VERIFY_DECODED_COEFFICENT
//
// define VERIFY_DECODED_COEFS and set fltCoefIntFloatThreshold
//
//*****************************************************************************************
//#define VERIFY_DECODED_COEFS
#if defined(_DEBUG) && defined(VERIFY_DECODED_COEFS)
#define VERIFY_DECODED_COEFFICENT(idxBark) VerifyDecodedCoefficentHR(pau, ppcinfo, idxBark, iRecon, iLevel, ctCoefRecon)
#else
#define VERIFY_DECODED_COEFFICENT(idxBark)
#endif

// *************************************************************************************
// DEBUG_BREAK at a particular Frame in auReconCoefficentsHighRate
//
//#define DECODE_COEF_AT_FRAME 435
//#define DECODE_COEF_AT_SUBFRAME 0
//
#if defined(_DEBUG) && ( defined(DECODE_COEF_AT_FRAME) || defined(DECODE_COEF_AT_SUBFRAME) )
#   pragma COMPILER_MESSAGE(__FILE__ "(1154) : Warning - Decode Coefficient Debug Code Enabled.")
#   if defined(DECODE_COEF_AT_FRAME) && defined(DECODE_COEF_AT_SUBFRAME)
#       define DEBUG_BREAK_AT_FRAME_DECODE                                                          \
            if (  ( pau->m_iFrameNumber == DECODE_COEF_AT_FRAME || DECODE_COEF_AT_FRAME < 0 )       \
                &&( pau->m_iCurrSubFrame == DECODE_COEF_AT_SUBFRAME || DECODE_COEF_AT_SUBFRAME < 0 ) ) \
                DEBUG_BREAK();              
#   elif defined(DECODE_COEF_AT_FRAME)
#       define DEBUG_BREAK_AT_FRAME_DECODE                                                          \
            if ( pau->m_iFrameNumber == DECODE_COEF_AT_FRAME || DECODE_COEF_AT_FRAME < 0 )          \
                DEBUG_BREAK();              
#   else
#       define DEBUG_BREAK_AT_FRAME_DECODE                                                          \
            if ( pau->m_iCurrSubFrame == DECODE_COEF_AT_SUBFRAME || DECODE_COEF_AT_SUBFRAME < 0 )   \
                DEBUG_BREAK();
#   endif               
#else   // defined(_DEBUG) && defined(DECODE_COEF_AT_FRAME)
#   define DEBUG_BREAK_AT_FRAME_DECODE
#endif  // defined(_DEBUG) && defined(DECODE_COEF_AT_FRAME)

//#define RL_PRINT_SRC
#if defined(_DEBUG) && defined(RL_PRINT_SRC) && !defined(WMA_MONITOR)
extern Int g_cBitGet;
#   define DBG_RUNLEVEL(a,b,c,d,e)                          \
        if(e) {                                             \
            WMAPrintf("%d %d %d %d\n", a, b, c, d+1);          \
            FFLUSH (stdout);                                \
        }
#elif defined(_DEBUG) && defined(RL_PRINT_SRC) && defined(WMA_MONITOR)
#   define DBG_RUNLEVEL(a,cRunOfZeros,iLevel,iSign,e)                       \
        if(e) {                                                             \
            WMAPrintf("%d %d %d %d\n", a, b, c, d+1);                          \
            FFLUSH (stdout);                                                \
        }                                                                   \
        MONITOR_RANGE(gMR_iCoefQ,(iLevel^iSign)-iSign);                     \
        MONITOR_COUNT_CONDITIONAL(cRunOfZeros>0,gMC_0CoefQ,cRunOfZeros);    \
        {   int ii;                                                         \
            for( ii = 0; ii < cRunOfZeros; ii++ )                           \
                MONITOR_RANGE(gMR_CoefRecon,0);                             \
            MONITOR_COUNT(gMC_zeroCoefRecon,cRunOfZeros);                   \
        }
#elif defined(_DEBUG) && !defined(RL_PRINT_SRC) && defined(WMA_MONITOR)
#   define DBG_RUNLEVEL(a,cRunOfZeros,iLevel,iSign,e)                       \
        MONITOR_RANGE(gMR_iCoefQ,(iLevel^iSign)-iSign);                     \
        MONITOR_COUNT_CONDITIONAL(cRunOfZeros>0,gMC_0CoefQ,cRunOfZeros);    \
        {   int ii;                                                         \
            for( ii = 0; ii < cRunOfZeros; ii++ )                           \
                MONITOR_RANGE(gMR_CoefRecon,0);                             \
            MONITOR_COUNT(gMC_zeroCoefRecon,cRunOfZeros);                   \
        }
#else
#   define DBG_RUNLEVEL(a,b,c,d,e)
#endif 


// DecodeCoefsHighRate with combined INTEGER and INT_FLOAT code

#ifdef BUILD_INTEGER

#define MULT_QUANT(iLevel,ffltQuantizer) MULT_HI(((iLevel)<<16),(ffltQuantizer).iFraction)
#ifdef HIGHBIT_IQ
#define MAXINTVAL_IQ MAX40BITVAL
#define MAXINTVAL_IQ_LOG2 (LOG2MAX40BITVAL)   //LOG2_64(MAXINTVAL_IQ)
#define MULT_QUANT_AND_SCALE(iLevel,ffltQuantizer) \
        (multHi64(iLevel, (ffltQuantizer).iFraction, (ffltQuantizer).iFracBits))
#else
#define MAXINTVAL_IQ MAX32BITVAL
#define MAXINTVAL_IQ_LOG2 (LOG2MAX32BITVAL)   //LOG2(MAXINTVAL_IQ)
#define MULT_QUANT_AND_SCALE(iLevel, ffltQuantizer) \
        COEF_MUL_FFLT(iLevel, ffltQuantizer)
#endif

// SCALE_COEF_RECON shifts CoefRecon to give it TRANSFORM_FRACT_BITS==5 fractional bits
#if defined(PLATFORM_OPTIMIZATION_MINIMIZE_BRANCHING)
    // This SCALE_COEF_RECON macro requires 6 ops and no branches
    // This MAKE_MASK_FOR_SCALING requires 5 ops plus 1 branch.
    // SCALE_COEFFICENT gets executed 25x as often as MAKE_MASK_FOR_SCALING, so this method requires 6.2 ops plus 0.04 branches per SCALE_COEFFICENT
#   define SCALE_COEF_RECON(iCR) (((iCR>>iShift) & iMask2) | ((iCR<<-iShift) & iMask1))
#   define MAKE_MASK_FOR_SCALING(iFB) iShift=iFB-(TRANSFORM_FRACT_BITS+31-16);                      \
                                      iMask2 = 0xFFFFFFFF ^ (iMask1 = (iShift<0) ? 0xFFFFFFFF : 0);
#else
    // When branching is not a high penaty activity, do it the simplier way
    //   iCoefRecon = (t=iFracBits-5)<0 ? iCoefRecon>>-t : iCoefRecon<<t
    // This SCALE_COEF_RECON requires 3 ops plus 1 branch or 2 ops plus 1 branch.  
    // This MAKE_MASK_FOR_SCALING requires 2 ops
    // SCALE_COEFFICENT gets executed 25x as often as MAKE_MASK_FOR_SCALING, so this method requires 2.58 ops plus 0.04 branches per SCALE_COEFFICENT
    // On one test on a 500 MHz Pentium 686, This way saves 1% execution time over masking.
#   define SCALE_COEF_RECON(iCR) (iShift<0) ? (iCR)<<-iShift : (iCR)>>iShift
#   define MAKE_MASK_FOR_SCALING(iFB) iShift=iFB+(16-31-TRANSFORM_FRACT_BITS)
#endif

#else

#define MULT_QUANT(iLevel,ffltQuantizer) ((iLevel)*(ffltQuantizer))
#define MULT_QUANT_AND_SCALE(iLevel,ffltQuantizer) MULT_QUANT(iLevel,ffltQuantizer)
#define SCALE_COEF_RECON(iCR) (iCR)
#define MAKE_MASK_FOR_SCALING(iFB)

#endif

// ===============================================================
// Entropy Code testing **************************

#ifdef WMA_ENTROPY_TEST
#include "../../../../app/enttest/entropytest.h"
extern Bool g_maskTest;
#endif

#if defined(RECALC_SINCOS)
#define RECALC
#endif

#ifdef RECALC
extern Int g_rcFFT;
extern Int g_rcEverySampleFFT;
extern Int g_rcNxFFT;
extern Int g_rcNFFT;
extern Int g_rcR;
extern Int g_rcEverySampleR;
extern Int g_rcNxR;
extern Int g_rcNR;
extern I64 g_maxValFFT;
#if 0
#define DORECALC(x, i, L) (g_rc##x && \
                           (g_rcEverySample##x || \
                           (g_rcNx##x && ((i%(L/g_rcN##x))==0))))
#else
#define DORECALC(x, i, L) (g_rc##x && \
                           (g_rcEverySample##x || \
                           (g_rcNx##x && ((i%g_rcN##x)==0))))
#endif
#define DORECALC_FFT(i, L) DORECALC(FFT, i, L)
#define DORECALC_R(i) DORECALC(R, i, iOverlapSize)
//#undef MAXINTVAL_FFT
//#define MAXINTVAL_FFT g_maxValFFT
#endif

#if 0
INLINE void gReadExtraArgs()
{
  FILE *f = NULL;
  f = FOPEN("exargs.txt", "rt");
  if (f == NULL) {
#ifdef BUILD_INTEGER
#ifdef COEF64BIT
    g_maxValFFT = MAX40BITVAL;
#else
    g_maxValFFT = MAX32BITVAL;
#endif
#endif
  } else {
    fscanf(f, "%d", &g_rcFFT);
    fscanf(f, "%d", &g_rcEverySampleFFT);
    fscanf(f, "%d", &g_rcNxFFT);
    fscanf(f, "%d", &g_rcNFFT);
    fscanf(f, "%d", &g_rcR);
    fscanf(f, "%d", &g_rcEverySampleR);
    fscanf(f, "%d", &g_rcNxR);
    fscanf(f, "%d", &g_rcNR);
    fscanf(f, "%I64x", &g_maxValFFT);
  }
  FCLOSE(f);
}
#endif

#ifdef WMA_COMPRESSION_TOOL
typedef struct RAWBS_FRMINFOtag 
{
    I32 iFrameNumber; // PCM frame number.
    I32 cFrameBits;
    I64 iOffset;
}RAWBS_FRMINFO;

// Professional Encoder Related.
I32 prvBitsCopy(U8 *pbSrc, I32 cbSrc, I32 iSrcStart, I32 iSrcEnd, U8 * pbDst, I32 cbDst, I32 iDstCurr);
I32 prvBitsMove(CAudioObjectDecoder *paudec, U8 *pbSrc, I32 cbSrc, I32 iSrcStart, I32 iSrcEnd, U8 * pbDst, I32 cbDst, I32 iDstCurr);
I32 prvGetPCMFrameNumber (I32 iWinFrameNumber, Bool fEncoder, I32 iVersion);
#endif //WMA_COMPRESSION_TOOL

// function counter for debugging
#if 0
#define FN_CNT \
  static int count = -1; \
  count++;
#else
#define FN_CNT
#endif

// ===========================================================

INLINE
Void prvSaveCurChannels(CAudioObject *pau)
{
    if (pau)
    {
        assert(!pau->m_bSaveChannelInUse);
        pau->m_cSaveChannel     = pau->m_cChannel;
        pau->m_nSaveChannelMask = pau->m_nChannelMask;
        pau->m_bSaveChannelInUse = WMAB_TRUE;
    }
}

INLINE
Void prvRestoreCurChannels(CAudioObject *pau)
{
    if (pau)
    {
        pau->m_cChannel     = pau->m_cSaveChannel;
        pau->m_nChannelMask = pau->m_nSaveChannelMask;
        pau->m_bSaveChannelInUse = WMAB_FALSE;
    }
}

INLINE
Void prvSetOrigChannels(CAudioObject *pau)
{
    if (pau)
    {
        pau->m_cChannel     = pau->m_cOrigChannel;
        pau->m_nChannelMask = pau->m_nOrigChannelMask;
    }
}

INLINE
Void prvSetCodedChannels(CAudioObject *pau)
{
    if (pau)
    {
        pau->m_cChannel     = pau->m_cCodedChannel;
        pau->m_nChannelMask = pau->m_nCodedChannelMask;
    }
}

// ===========================================================

Void freqexReset(CAudioObject *pau);

#if defined(BUILD_WMAPROLBRV1)

// fex
WMARESULT freqexInit(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
Void freqexReset(CAudioObject *pau);
Void freqexFree(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
Void freqexInitPcInfo( CAudioObject *pau, PerChannelInfo *ppcinfo,
                       const Int iChSrc );

Int freqexMvBinsToIndex(Int iMvBins);
Int freqexIndexToMvBins(Int iIndex);
Int freqexScaleBinsToIndex(Int iScaleBins);
Int freqexIndexToScaleBins(Int iIndex);
Int freqexMaxBandsToIndex(Int iMaxBands);
Int freqexIndexToMaxBands(Int iIndex);

Int freqexGetMvBins(Int nSamplesPerSec, Int nChannels, Int nBitrate);
Int freqexGetScaleBins();
Int freqexGetMaxBands();

Int reconProcGetCap();
Int reconProcCapToIndex(Int iCap);
Int reconProcGetMaxTiles(CAudioObject *pau);

Void freqexFrameInit(CAudioObject *pau);

Void freqexSetChannelDependencyGroup(CAudioObject *pau,
                                     CChannelGroupInfo *rgcgi,
                                     Int cChannelGroup);
Void freqexZeroNonzeroMask(CAudioObject *pau);
Void freqexProcessZeroMaskXChannel(CAudioObject *pau);
Bool freqexTileCoded(CAudioObject *pau);
WMARESULT freqexSwitchFexDomain( CAudioObject *pau, Bool bRecon );

Bool freqexReconTileCoded(CAudioObject *pau);

WMARESULT plusGetIndexH( struct CAudioObjectDecoder *paudec,
                           U32 *uVal, HuffInfo *phdi);
#ifdef BUILD_WMAPROLBRV3
Void bpeakFrameInit( CAudioObject *pau );
Void bpeakReset( CAudioObject *pau );
Void bpeakResamplePeakPos( PerChannelInfo *ppcinfo,
                           I16            iSizeFrom,
                           I16            iSizeTo );
Int bpeakGetBasePeakBarkStart( CAudioObject *pau,
                               I16 cLastCodedIndex );
Int bpeakGetBasePeakBarkStart_BasePlus( CBasePlusObject *pbp,
                                        I16 cLastCodedIndex );
WMARESULT bpeakGetShape( Int iIndex, CoefType fCoef, CoefType *pfCoef0, CoefType *pfCoef1 );
//From msaudiodec.h
WMARESULT   freqexDecodeGlobalParamV3( struct CAudioObjectDecoder       *paudec,
                                       FexGlobalParamUpdateType  iUpdateType );
#endif


// CHEX ====================

#if defined(BUILD_WMAPROLBRV2)

WMARESULT reconProcInit(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
WMARESULT chexInit(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
Void reconProcFree(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
Void chexFree(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
Void chexInitPcInfo(CAudioObject *pau, PerChannelInfo *ppcinfo, Int iCh);
Void chexInitPcInfoShr(CAudioObject *pau, PerChannelInfo *ppcinfo, Int iCh);

#if 0
Void chexCopyChannelsForIdentityXForm(CAudioObject *pau);
#endif

Void chexReset(CAudioObject *pau);
WMARESULT chexSubframeRecon(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
Int chexGetSamplesReady(CAudioObject *pau);
WMARESULT reconProcessUnprocessedTiles(struct CAudioObjectDecoder *paudec, CAudioObject *pau);
WMARESULT chexGetPCM(CAudioObject *pau,  // [in]  the audio structure
                   Void *pHandle,      // [in]  the caller (encoder/decoder)
                   U16 *pcSample,      // [in/out] number of output samples
                   U8 **ppbDst,        // [in/out] output buffer
                   U32 cbDstLength,    // [in] the output buffer length
                   U16 cChannel,       // [in] the number of channels
                   Bool fSPDIF);
WMARESULT chexAuGetPCM(CAudioObject* pau,  // [in]  the audio structure
                     Bool fSPDIF,
                     Int *pcSamples);
Void chexSetPtrFexScaleFac(Fex *pfx, PerChannelRecon *ppcr, Int iTile);
WMARESULT chexClearFexParamsBasic(Fex *pfx, ReconTileInfo *prti);
Void chexClearFexParams(Fex *pfx, ReconTileInfo *prti);
Void chexSetNoCodeCx(CAudioObject *pau);
Void chexSetNoCodeRFx(CAudioObject *pau);
Void chexShiftPoints(CAudioObject *pau, Int iSampleShift);
WMARESULT chexShiftBuf(CAudioObject *pau, Int iChSrc);
WMARESULT chexTileInit(CAudioObject *pau);
Int chexIndexToMaxAnchors(Int iIndex, Int iMinTiles);
WMARESULT chexGetOrigChMaskToIndex(Int *piIndex, U32 nChannelMask);
WMARESULT chexOrigChToLowerCh(Int iOrigCh, Int *piCodedCh,
                              U32 uOrigChMask, U32 *puCodedChMask);
Int chexMaxAnchorsToIndex(Int iMaxAnchors, Int iMinTiles);

WMARESULT chexInitConfig(CAudioObject *pau);
Void reconProcInitConfig(CAudioObject *pau);
Void reconProcUpdateCodingFexFlag(CAudioObject *pau);

Int reconProcTilesToIndex(Int iMinTiles, Int iMaxTiles);
Void reconProcIndexToTiles(Int iIndex, Int *piMinTiles, Int *piMaxTiles);
Void reconProcEncGetMinMaxTiles(Int nSamplePerSec, U32 dwBitPerSec,
                                Int iVersion, U16 wEncOpt,
                                Int *piMin, Int *piMax);

INLINE
Int chexOffsetBuf( const ReconProc * const prp, const Int iPos, const Int iOffset )
{
#if 0
    return (((iPos + iOffset) % prp->m_nTilesInBuf) + prp->m_nTilesInBuf) %
        prp->m_nTilesInBuf;
#else
    Int i = iPos + iOffset;

    assert(iPos >= 0 && iPos < prp->m_nTilesInBuf);
    assert(iOffset >= -prp->m_nTilesInBuf && iPos <= prp->m_nTilesInBuf);

    if (i < 0)
        return i + prp->m_nTilesInBuf;

    if (i >= prp->m_nTilesInBuf)
        return i - prp->m_nTilesInBuf;

    return i;
#endif
}

INLINE
Void chexDctHelper(CAudioObject *pau, PerChannelInfo *ppcinfo,
                 CoefType *pCoef, BP2Type fScale, Int iSize)
{
    // only frist 6 args are important, rest are for debugging
    (*pau->aupfnDctIV)(pCoef, fScale, NULL, iSize,
                       pau->aupfnFFT, 
                       pau->m_hIntelFFTInfo,
                       pau->m_iFrameNumber,
                       ppcinfo ? ppcinfo->m_iCurrSubFrame : 0,
                       pau->m_cFrameSampleAdjusted,
                       iSize*2);
}

#if defined(CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL) && CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL

WMARESULT auWindowReflectionAndDctIV256(CoefType* rgiCoef,
                  BP2Type fltAfterScaleFactor,
                  U32 *piMagnitude,
                  const Int cSubbandAdjusted,
                  PFNFFT pfnFFT, Void *pFFTInfo,
                  const Int iFrameNumber,
                  const Int iCurrSubFrame,
                  const Int cFrameSampleAdjusted,
                  const Int cSubFrameSampleAdjusted);

INLINE
Void chexWindowReflectionAndDct256(CAudioObject *pau, PerChannelInfo *ppcinfo,
                 CoefType *pCoef, BP2Type fScale, Int iSize)
{
//    catchFP();

    // only frist 7 args are important, rest are for debugging
    auWindowReflectionAndDctIV256(pCoef, fScale, NULL, iSize,
                       pau->aupfnFFT, 
                       pau->m_hIntelFFTInfo,
                       pau->m_iFrameNumber,
                       ppcinfo ? ppcinfo->m_iCurrSubFrame : 0,
                       pau->m_cFrameSampleAdjusted,
                       iSize*2);
//    initFP();
}

#endif  // CHEX_WINDOW_REFLECTION_DCT_LOOKUPTBL

INLINE
Void chexDstHelper(CAudioObject *pau, PerChannelInfo *ppcinfo,
                 CoefType *pCoef, BP2Type fScale, Int iSize)
{
    // only frist 6 args are important, rest are for debugging
    auDstIV(pCoef, fScale, NULL, iSize,
            pau->aupfnDctIV,
            pau->aupfnFFT, 
            pau->m_hIntelFFTInfo,
            pau->m_iFrameNumber,
            ppcinfo ? ppcinfo->m_iCurrSubFrame : 0,
            pau->m_cFrameSampleAdjusted,
            iSize*2,
            WMAB_TRUE);
}

INLINE
Void chexDstHelperNoFlip(CAudioObject *pau, PerChannelInfo *ppcinfo,
                 CoefType *pCoef, BP2Type fScale, Int iSize)
{
    // only frist 6 args are important, rest are for debugging
    auDstIV(pCoef, fScale, NULL, iSize,
            pau->aupfnDctIV,
            pau->aupfnFFT, 
            pau->m_hIntelFFTInfo,
            pau->m_iFrameNumber,
            ppcinfo ? ppcinfo->m_iCurrSubFrame : 0,
            pau->m_cFrameSampleAdjusted,
            iSize*2,
            WMAB_FALSE);
}


Int chexGetLastTileNextSize(CAudioObject *pau);
Int prvReconProcDecodeAdjustment(const CAudioObject *pau);
Void chexAdjustTileStartEnd(CAudioObject *pau);
WMARESULT chexInitSubframeTilingInfo(CAudioObject *pau,
                                     Int iChInTile, I16 *rgiChInTile,
                                     Int iTile, Bool bNext, Bool bNoCode);

Void chexEndSubframeTilingInfo(CAudioObject *pau, Int iTile);

WMARESULT chexCopyFex(CAudioObject *pau, Int iCh);
#endif // BUILD_WMAPROLBRV2

Void prvSetPlusOrder(CAudioObject *pau);

#endif // BUILD_WMAPROLBRV1

Int prvShiftTimeIndex(CAudioObject *pau);

Void auFlipCoeff(CoefType *pctCoeff, Int iSize);
Void auIntFromCoeff(CAudioObject *pau, CoefType *pctCoeff, Int iSize);

WMARESULT auOverlapAddV3(CAudioObject *pau,
                         CoefType *pctCoefOut,
                         CoefType *pctCoefHist,
                         CoefType *pctCoefNew,
                         CoefType *pctCoefHistDST,
                         CoefType *pctCoefNewDST,
                         Int iSizePrev, Int iSizeCurr,
                         Bool bUseDCT,
                         Bool bUseMCLT,
                         Bool bFlip,
                         Bool fBase);
WMARESULT auOverlapAddV3DirectMCLT(CoefType *pctCoefOut,
                                  CoefType *pctCoefHist,
                                  CoefType *pctCoefNew,
                                  Int iSizeCurr);
WMARESULT auOverlapAddV3DirectMCLTHistDCTDSTWrap(CAudioObject *pau, CoefType *pctCoefOut,
                         CoefType *pctCoefHist, CoefType *pctCoefNew,
                         CoefType *pctCoefHistDST, CoefType *pctCoefNewDST,
                         Int iSizePrev, Int iSizeCurr, Bool fBase);

void auApplyWindow(CAudioObject *pau, CoefType *pctCoef,
                     Int iSizePrev, Int iSizeCurr, Int iSizeNext,
                     Bool fBase);

#if defined(BUILD_INT_FLOAT) && !defined(WMA_DISABLE_SSE1)
void auApplyWindow_SSE1(CAudioObject *pau, CoefType *pctCoef, 
                        Int iSizePrev, Int iSizeCurr, Int iSizeNext, 
                        Bool fBase);
#endif

#if defined(WMA_OPT_APPLYWINDOW_ARM) && WMA_OPT_APPLYWINDOW_ARM
void auApplyWindowLoop(CoefType *pctCoef, BP2Type bp2Sin, BP2Type bp2Cos, 
    BP2Type bp2Sin1, BP2Type bp2Cos1, BP2Type bp2Step, Int i);
void auApplyWindowLoop_LUT_Forward(CoefType* pctCoef, const BP2Type* piWindow, Int n);
void auApplyWindowLoop_LUT_Reverse(CoefType* pctCoef, const BP2Type* piWindow, Int n);
void auApplyWindowLoop_LUT_Bidirectional(CoefType* pctCoefF, CoefType* pctCoefR, const BP2Type* piWindow, Int n);
#endif

#if defined(WMA_OPT_OVERLAPADDV3DCT_ARM) && WMA_OPT_OVERLAPADDV3DCT_ARM
void auOverlapAddV3DCTLoop_LUT_Flip(CoefType* pctHist, CoefType* pctNew, CoefType* pctOut, const BP2Type* pbp2Sin, Int n);
void auOverlapAddV3DCTLoop_LUT_NoFlip(CoefType* pctHist, CoefType* pctNew, CoefType* pctOut, const BP2Type* pbp2Sin, Int n);
#endif

Void auApplyReflectionV3(CoefType *pctCoef, Int iSizeCurr);
//Void auApplyReflectionPartialV3(CoefType *pctCoef, Int iSizeCurr);
Void auShiftHistoryV3(CoefType *pctCoefHist,
                      CoefType *pctCoefNew,
                      Int iSizeCurr,
                      Bool bFlip);
Void auDSTFromDCTV3(CAudioObject *pau,
                          CoefType *pctDST,
                    const CoefType *pctIDCTPrev,
                    const CoefType *pctIDCTCurr,
                    const CoefType *pctIDCTNext,
                    Int iSizePrev, Int iSizeCurr, Int iSizeNext);

Int auGetAdjustedSize( const CAudioObject *pau, const Int iSize);

INLINE
Int prvGetStartPtInFrame(SubFrameConfigInfo *psubfrmconfig, Int iSubframe)
{
    Int i, iStart;

    iStart = 0;
    for (i=0; i < iSubframe && i < psubfrmconfig->m_cSubFrame; i++)
        iStart += psubfrmconfig->m_rgiSubFrameSize[i];
    return iStart;
    // following is not accurate at decoder end
    //return psubfrmconfig->m_rgiSubFrameStart[ppcinfo->m_iCurrSubFrame];
}

Void initSubframeTilingInfo(CAudioObject *pau,
                            Int iChInTile, I16 *rgiChInTile);

Void auConvertSubframeConfigToTileConfig(CAudioObject *pau,
                                         ConfigFrame ecfConfig);

// ==========================================

WMARESULT prvAllocateNDimArr(struct CAudioObjectDecoder *paudec, U8 ***p, Int nDims, Int *dims, Int *size);
Void prvDeleteNDimArr(struct CAudioObjectDecoder *paudec, U8 **p, Int nDims, Int *dims);

// ==========================================

#if !defined(WMA_OPT_MATH_ARM) || !WMA_OPT_MATH_ARM
INLINE U16 isqrt32( U32 x )
{
    U32 r = 0;
    U32 y = 0;
    U32 i = 0;
    
    for( i = 0; i < 16; ++i )
    {
        r <<= 2;
        r  += x >> 30;
        x <<= 2;
        y <<= 1;

        if( r > y )
        {
            r -= ++y; ++y;
        }
    }
    return (U16)(y >> 1);
}

INLINE U32 isqrt64( U64 x )
{
    U64 r = 0;
    U64 y = 0;
    U32 i = 0;
    
    for( i = 0; i < 32; ++i )
    {
        r <<= 2;
        r  += x >> 62;
        x <<= 2;
        y <<= 1;

        if( r > y )
        {
            r -= ++y; ++y;
        }
    }
    return (U32)(y >> 1);
}
#else   // !WMA_OPT_MATH_ARM
extern U16 isqrt32( U32 x );
extern U32 isqrt64( U64 x );
#endif  // !WMA_OPT_MATH_ARM

#define FILE_DUMP_INIT(file) \
{ \
    static FILE *fpLoc=NULL; \
    if (!fp) \
    { \
        if (!fpLoc && pau->m_bEncoder) \
            fpLoc = fopen(#file"enc.txt", "wt"); \
        else if (!fpLoc) \
            fpLoc = fopen(#file"dec.txt", "wt"); \
        fp = fpLoc; \
        if (!fp) return; \
    } \
}

// ==============================================================================
// Functions to disable floating point exceptions
// ==============================================================================
#if defined (WMA_TARGET_X86) || defined (WMA_TARGET_WIN64)
#define DISABLE_FLOATINGPOINT_EXCEPTIONS
#endif

#include <float.h>
// mask these floating point exceptions in the encoder
// Leave the other three - (DivideByZero, Invalid operation and Overflow) as they are
#define DISABLED_EXCEPTIONS    (_EM_DENORMAL | _EM_UNDERFLOW | _EM_INEXACT)
#define DEFAULT_FP_EXCEPTION_MASK (_EM_INVALID | _EM_DENORMAL | _EM_ZERODIVIDE | \
                                   _EM_OVERFLOW | _EM_UNDERFLOW | _EM_INEXACT)
/*
#ifdef DISABLE_FLOATINGPOINT_EXCEPTIONS

static void INLINE prvDisableFPExceptions(unsigned int *pSavedExceptionMask)
{
    // save the original exception mask
    *pSavedExceptionMask = _controlfp(0, 0);

    // Disable exceptions deemed as harmless
    _controlfp(*pSavedExceptionMask | DISABLED_EXCEPTIONS, _MCW_EM);
}

static void INLINE prvRestoreFPExceptions(unsigned int FPExceptionMask)
{
    // Clear the FP status word
    _clearfp();

    // Set the FP exception mask
    _controlfp(FPExceptionMask, _MCW_EM);
}
#else
#define prvDisableFPExceptions(x)
#define prvRestoreFPExceptions(x)
#endif
*/
#ifdef __cplusplus
}
#endif

//From msaudiodec.h
extern Void prvDecLMSPredictor_I16_C(CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);
extern Void prvDecLMSPredictor_I32_C(CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);

// from msaudiodec.h
//#include "com_wmsglobal.h"
//#include "com_wmsdefines.h"
//#include "wmsdec.h"
typedef struct CSpeechObjectDecoder CSpeechObjectDecoder;
//typedef struct CAudioObjectDecoder CAudioObjectDecoder;



#ifdef RVDS
typedef struct LMSPredictor LMSPredictor;
typedef struct _DRC	DRC;
#endif

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif

#ifdef ENABLE_EQUALIZER         
WMARESULT audecSetEqualizer (CAudioObjectDecoder* paudec, Int iBand, Int iGainIndB);
WMARESULT audecResetEqualizer (CAudioObjectDecoder* paudec); 
Void    audecStartComputeBandPower (CAudioObjectDecoder* paudec);
Void    audecStopComputeBandPower (CAudioObjectDecoder* paudec);
WMARESULT audecGetBandPower (CAudioObjectDecoder* paudec, I8* rgbBandPower);
#endif //ENABLE_EQUALIZER
#ifdef __cplusplus
}
#endif
/*
INLINE
Int audecGetNoiseSubUsed(CAudioObjectDecoder *paudec)
{
    return paudec->pau->m_fNoiseSub;
}
*/
//streaming decoding interface
WMARESULT prvDecodeInfo (CAudioObjectDecoder* paudec);
WMARESULT prvDecodeData (CAudioObjectDecoder* paudec, U16* pcSampleReady,
                           I16* pcSampleSeekAdj);

WMARESULT prvDecodeFrameHeader (CAudioObjectDecoder* paudec);

Void prvResetRunLevelState(CAudioObjectDecoder *paudec);

WMARESULT prvDecodeSubFrame (CAudioObjectDecoder* paudec);
WMARESULT prvDecodeSubFrameHighRate (CAudioObjectDecoder* paudec);

WMARESULT prvDecodeSubFrameHeader (CAudioObjectDecoder* paudec);
WMARESULT prvDecodeSubFrameHeaderLpc (CAudioObjectDecoder* paudec);
WMARESULT prvDecodeSubFrameHeaderNoiseSub (CAudioObjectDecoder* paudec);
WMARESULT prvGetNextRunDEC (Void* pvoid, struct PerChannelInfo* ppcinfo);
WMARESULT prvGetNextRunDECVecTableIndex(Void*           pobjin, 
                                        struct PerChannelInfo* ppcinfo);
WMARESULT prvGetNextRunDECVecNonRL(Void*           pobjin, 
                                   struct PerChannelInfo* ppcinfo);
WMARESULT prvGetNextRunDECVecRL(Void*           pobjin, 
                                struct PerChannelInfo* ppcinfo);
WMARESULT prvGetNextRunDECVecNonRLV4(Void*           pobjin, 
                                     struct PerChannelInfo* ppcinfo);
WMARESULT prvGetNextRunDECVecRLV4(Void*           pobjin, 
                                  struct PerChannelInfo* ppcinfo);
Void prvResetVecCoder(CAudioObjectDecoder *paudec);
#define AUINV_RECON_CHANNEL
#define AUINV_RECON_CHANNEL_DEC
#include "msaudiotemplate.h"
#undef AUINV_RECON_CHANNEL_DEC
#undef AUINV_RECON_CHANNEL

WMARESULT prvNewInputBuffer(CAudioObjectDecoder* paudec, audecInputBufferInfo* pBuf);

WMARESULT prvUpdateSubFrameConfig (CAudioObjectDecoder* paudec, Int iSizeNext);

WMARESULT prvDecodeCoefficientMono  
                (CAudioObjectDecoder* paudec, struct PerChannelInfo* ppcinfo);
WMARESULT prvDecodeCoefficientStereo 
                (CAudioObjectDecoder* paudec, struct PerChannelInfo* ppcinfo);
WMARESULT prvDecodeRunLevel  (CAudioObjectDecoder* paudec, struct PerChannelInfo* ppcinfo,
                              Int iCoefFirst, 
                              Int iCoefLim);
WMARESULT prvDecodeMaskRunLevel  (CAudioObjectDecoder* paudec, 
                                  struct PerChannelInfo* ppcinfo,
                                  Int iBandLim);
Int prvNumDstSamplesFromXFormSamples(CAudioObjectDecoder *paudec,
                                     Int nTransformSamples);
//From drccommon.h
WMARESULT drcInit(CAudioObjectDecoder *paudec, 
				  struct _DRC *pDrc,
                  WAVEFORMATEXTENSIBLE *pwfx,
                  struct _DRCData *pDrcData,
                  struct _DRCOptions *pDrcOptions);

void drcFree(CAudioObjectDecoder *paudec, struct _DRC *p);

WMARESULT drcDecodeBlockLsl(struct _DRC *p,
                            CAudioObjectDecoder *paudec,
                            U8 quantScaleFactor,
                            Int nDRCSetting,
                            Int cSamples);
WMARESULT drcDecodeBlockPro(struct _DRC *p,
                            CAudioObjectDecoder *paudec,
                            U8 quantScaleFactor,
                            Int nDRCSetting,
                            Int cSamples);

#if defined (BUILD_WMAPRO) || defined (BUILD_WMALSL)
// ================================ V3 Lossless Start =========================== //
// ------------------------------- Pure Lossless Mode -------------------------- //
WMARESULT prvDecodeSubFramePureLosslessMode (CAudioObjectDecoder* paudec);
WMARESULT prvDecodeSubFrameHeaderPureLosslessMode (CAudioObjectDecoder* paudec);
WMARESULT prvDecodeSubFrameChannelResiduePureLosslessModeVerA (struct CAudioObject* pau, CAudioObjectDecoder *paudec,
                                                               struct PerChannelInfo* ppcinfo);
WMARESULT prvDecodeSubFrameChannelResiduePureLosslessModeVerB (struct CAudioObject* pau, CAudioObjectDecoder *paudec,
                                                               struct PerChannelInfo* ppcinfo);
WMARESULT prvDecodeSubFrameChannelRawPCMPureLosslessMode (struct CAudioObject* pau, CAudioObjectDecoder *paudec,
                                                          struct PerChannelInfo* ppcinfo);
Void prvSubFrameRestoreChannelDC(struct CAudioObject *pau);

// ----------------------------- V3 Unified MLLM or PLLM ------------------------- //
WMARESULT prvDecodeSubFrameChannelResidueMono_Unified_MLLM_Or_PLLM(struct CAudioObject* pau, CAudioObjectDecoder *paudec,
                                                                   struct PerChannelInfo* ppcinfo);
WMARESULT prvDecodeSubFrameReconWindowedFrameMono_Unified_MLLM_Or_PLLM(struct CAudioObject* pau, CAudioObjectDecoder *paudec,
                                                                       struct PerChannelInfo* ppcinfo);
WMARESULT prvDecodeSubFrameReconWindowedFrameMono_Unified_PLLM(CAudioObjectDecoder * paudec,
                                                               struct PerChannelInfo * ppcinfoComm, 
                                                               CBT * rgiCoefRecon, 
                                                               CBT * rgiLPCResidue, 
                                                               I32 iLen);
WMARESULT prvDecodeSubFrameReconWindowedFrameMono_Unified_MLLM_LPC(struct CAudioObject * pau,
                                                                   struct PerChannelInfo * ppcinfoComm, 
                                                                   CBT * rgiCoefRecon, 
                                                                   CBT * rgiLPCResidue, 
                                                                   I32 iLen);
WMARESULT prvDecodeSubFrameReconWindowedFrameMono_Unified_MLLM_POLY(struct CAudioObject * pau,
                                                                    struct PerChannelInfo * ppcinfoComm, 
                                                                    CBT * rgiCoefRecon, 
                                                                    CBT * rgiLPCResidue, 
                                                                    I32 iLen);
Void prvFixMLLM4HalfTransform (CAudioObjectDecoder *paudec, struct CAudioObject * pau, struct PerChannelInfo* ppcinfo, CoefType * rgfltCoef);

Void prvLPCFilteringInv(struct CAudioObject *pau, CBT * rgiCoefRecon, I32 * rgiLPCPrevSamples, CBT * rgiLPCResidue, I32 * rgiLPCFilter, I32 iLen, I16 iOrder);

Void prvDecoderCDLMSPredictorHelper(CAudioObjectDecoder *paudec, struct PerChannelInfo* ppcinfoComm, struct PerChannelInfo* ppcinfo, CBT * rgiResidue, I32 iDataLen);


Void prvDecLMSPredictor_I16_MMX(struct CAudioObject * pau, struct LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);

#if defined(_WMA_TARGET_XBOX2_) && defined(WMA_OPT_FLOAT_PREDICTOR_XENON)
extern Void prvDecLMSPredictor_I16_C_XENON(struct CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);
extern Void prvDecLMSPredictor_I32_C_XENON(struct CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);
#endif

#if defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_LOSSLESSDECPROLSL)
extern int mips_use_MIPSII_strict;
extern Void prvDecLMSPredictor_I16_MIPSII(struct CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);
extern Void prvDecLMSPredictor_I16_MIPS32(struct CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);
extern Void prvDecLMSPredictor_I32_MIPSII(struct CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);
extern Void prvDecLMSPredictor_I32_MIPS32(struct CAudioObject * pau, LMSPredictor * pLMSPredictor, CBT *rgiResidue, I32 iLen);
#endif


// ================================ V3 Lossless End =========================== //

// Common to WMAPRO & both kinds of WMA Lossless
//Void  prvInitDecodeTileHdr(CAudioObjectDecoder *paudec, enum ConfigFrame iFrame);
Void prvInitDecodeTileHdr(CAudioObjectDecoder *paudec, int iFrame);
//I32 prvDecodeTileHdr(CAudioObjectDecoder* paudec, enum ConfigFrame iFrame);
#endif // BUILD_WMAPRO || BUILD_WMALSL

//equalize
Void    prvAdaptEqToSubFrame(CAudioObjectDecoder* paudec);
WMARESULT prvEqualize (CAudioObjectDecoder* paudec, struct PerChannelInfo* ppcinfo);
Void    prvComputeBandPower (CAudioObjectDecoder* paudec);

U8 WMA_get_nHdrBits(CAudioObjectDecoder* hWMA);

void prvInterpolateInit(CAudioObjectDecoder *paudec, Int iSrcSampleRate,
                        Int iDstSampleRate);



#if defined (BUILD_WMAPRO)
// These functions are only needed for WMA Pro.
WMARESULT prvPrepareResampledMaskV3_Channel(struct CAudioObject* pau, struct PerChannelInfo* ppcinfo);

WMARESULT prvInitChDnMix(CAudioObjectDecoder* paudec, I32* rgiMixDownMatrix,
                         Int cSrcChannel, U32 nSrcChannelMask,
                         Int cDstChannel, U32 nDstChannelMask);

WMARESULT prvChannelDownMix (CAudioObjectDecoder *paudec, U8 *piSourceBuffer, Int nBlocks,
                             U8 *piDestBuffer);
WMARESULT prvChannelDownMixFloat (CAudioObjectDecoder *paudec, U8 *piSourceBuffer, Int nBlocks,
                                  U8 *piDestBuffer);
WMARESULT prvChannelDownMixLtRt (CAudioObjectDecoder *paudec, U8 *piSourceBuffer, U16 *pnBlocks,
                                 U8 *piDestBuffer);

// Perform post-processing channel transform
WMARESULT prvPostProcChannelXformV3 (CAudioObjectDecoder* paudec, 
                                     U8 *piDataBuffer, const U16 cSampleReady);

WMARESULT prvInitDecodeDRC(CAudioObjectDecoder *paudec,
                           Int nDRCSetting);
#endif // BUILD_WMAPRO

#if defined (BUILD_WMAPRO) || defined (BUILD_WMALSL)
WMARESULT prvRequantizeTo16(CAudioObjectDecoder *paudec, U8 *piSrc, Int nBlocks);

#endif //BUILD_WMAPRO || BUILD_WMALSL

WMARESULT audecPostProcessPrePCM(Void *pDecHandle, U16 *pcSamplesReturned,
                                 U8 *pbDst, U32 cbDstLength);
WMARESULT audecPostProcessPostPCM(Void *pDecHandle, U16 *pcSamplesReturned,
                                  U8 *pbDst, U32 cbDstLength, I16 nDRCSetting);

#ifdef WMA_COMPRESSION_TOOL
// Professional Encoder related
WMARESULT prvWriteMetaFileSubFrmConfig (CAudioObjectDecoder* paudec, PerChannelInfo * ppcinfo, 
                                        ConfigFrame iFrame, LPCSTR pMeteFileName);

// Professional Encoder Related
WMARESULT prvRecordFrameStartInBitStrm (CAudioObjectDecoder * paudec, CWMAInputBitStream * pibstrm);
WMARESULT prvRecordFrameHdrBits (CAudioObjectDecoder * paudec, I32 cFrmHdrBits);
WMARESULT prvWriteMetaFileMask (CAudioObjectDecoder * paudec, I32 iPCMFrameNumber, I16 iCurrTile, LPCSTR pMetaFileName);
WMARESULT prvRecordFrameEndInBitStrm (CAudioObjectDecoder * paudec, CWMAInputBitStream * pibstrm);
WMARESULT prvWriteMetaFileGetCommonFrmHdr (CAudioObjectDecoder * paudec);
WMARESULT prvWriteMetaFileFrameBitsInfo(CAudioObjectDecoder * paudec, LPCSTR pMetaFileName);
WMARESULT prvWriteMetaFileFrameBitsData (CAudioObjectDecoder * paudec, LPCSTR pMetaFileName, I32 iPCMFrameNumber);
WMARESULT prvAddBitsToFrameBitsBuf (void *pDec, U8 * pbSrc, I32 cbSrc, I32 iStart, I32 iEnd);
WMARESULT prvWriteMetaFileTileInfo (CAudioObjectDecoder * paudec, I32 iPCMFrameNumber, I16 iCurrTile, LPCSTR pMetaFileName);
WMARESULT prvWriteMetaFileCHX (CAudioObjectDecoder * paudec, I32 iPCMFrameNumber, I16 iCurrTile, LPCSTR pMetaFileName);
WMARESULT prvWriteMetaFileMDCTCoeffs (CAudioObjectDecoder * paudec, I32 iPCMFrameNumber, I16 iCurrTile, LPCSTR pMetaFileName);

#endif //WMA_COMPRESSION_TOOL

// freqex related
WMARESULT freqexDecodeTile(CAudioObjectDecoder *paudec);
WMARESULT freqexDecInit(CAudioObjectDecoder *paudec);
Void freqexDecFree(CAudioObjectDecoder *paudec);
WMARESULT chexEndDecodeData(CAudioObjectDecoder *paudec, U16 *pcSampleReady);
WMARESULT chexDecodeReconFex(CAudioObjectDecoder *paudec);
Void freqexDecodeTileInit(CAudioObjectDecoder *paudec);
WMARESULT chexDecodeTile(CAudioObjectDecoder *paudec);
#ifdef BUILD_WMAPROLBRV2
WMARESULT   freqexDecodeCodingGlobalParam( CAudioObjectDecoder *paudec, 
                                           Bool fUpdateGrpA, Bool fUpdateGrpB, Bool fUpdateGrpC, Bool fUpdateGrpD );
WMARESULT   freqexDecodeReconGlobalParam( CAudioObjectDecoder *paudec, 
                                          Bool fUpdateGrpA, Bool fUpdateGrpB, Bool fUpdateGrpC, Bool fUpdateGrpD );
WMARESULT   freqexDecodeReconTileConfigV2( CAudioObjectDecoder *paudec );
#ifdef BUILD_WMAPROLBRV3
WMARESULT   freqexDecodeReconTileConfigV3( CAudioObjectDecoder *paudec );

WMARESULT   bpeakDecodeTile_Channel( CAudioObjectDecoder *paudec,
                                     struct PerChannelInfo *ppcinfo );
Void        bpeakClearPeakCoef(struct PerChannelInfo *ppcinfo, Int iMaxPeakCoef );
#endif
#endif
#if defined(BUILD_WMAPROLBRV2)
WMARESULT chexDecodeReconFexTile(struct CAudioObject *pau, struct _ReconTileInfo *prti,
                                 Int iCh);
#endif

#if defined (WMA_OPT_VLC_DEC_ARM) && WMA_OPT_VLC_DEC_ARM
WMARESULT decodeVLC(CAudioObject* pau, Void* pcaller, PerChannelInfo* ppcinfo, I16 iNextBarkIndex, 
                    Int iShift, QuantFloat qfltQuantizer, CoefType ctCoefRecon, I16* piRecon);
#endif

// ==============================================================

#ifdef BUILD_WMAPROLBRV1

WMARESULT plusGetIndex( const CAudioObjectDecoder *paudec,
                          const U16 *pDecTable   , UInt numBits, U32 *puVal,
                          const Int iMinSym    , const Int iMaxSym,
                          const Int cMinSymBits, const Int cMaxSymBits,
                          const Int iExtraBits );
#endif


#if !defined(WRITE_PLUS_TO_FILE)
#define PLUS_GETDATA(wr, fn) { \
  wr = fn; \
  if (WMA_FAILED(wr)) REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wr); \
}
WMARESULT plusGetMoreData(CAudioObjectDecoder *paudec);
#else
#define PLUS_GETDATA(wr, fn) { \
  while (1) { \
    wr = fn; \
    if (wr == WMA_E_ONHOLD) { TRACEWMA_EXIT(wr, plusGetMoreData(paudec)); } \
    else if (WMA_FAILED(wr)) { REPORT_BITSTREAM_CORRUPTION_AND_EXIT(wr); } \
    else { break; } /* success */ \
  } \
}
#endif

#define PLUS_GETBITS(wr, numBits, puVal) { \
  PLUS_GETDATA(wr, ibstrmLookForBits(paudec->m_pibsPlus, numBits)); \
  TRACEWMA_EXIT(wr, ibstrmGetBits(paudec->m_pibsPlus, numBits, puVal)); \
}

#define PLUS_PEEKBITS(wr, numBits, puVal) { \
  PLUS_GETDATA(wr, ibstrmLookForBits(paudec->m_pibsPlus, numBits)); \
  TRACEWMA_EXIT(wr, ibstrmPeekBits(paudec->m_pibsPlus, numBits, puVal)); \
  *puVal >>= 32-numBits; \
}

#define PLUS_FLUSHBITS(wr, numBits) { \
  ASSERTWMA_EXIT(wr, ibstrmFlushBits(paudec->m_pibsPlus, numBits)); \
}

WMARESULT freqexGet1ofNVals(const CAudioObjectDecoder *paudec,
                            const Int iVals, U32 *puVal);

//wmaltrt.h
void ltrtDownmixFree(CAudioObjectDecoder *paudec, struct LtRtDownmix *pLtRtDownmix);
//WMARESULT ltrtDownmix(struct CAudioObjectDecoder *paudec, LtRtDownmix *pLtRtDownmix, Int len, Int *pnOut);
WMARESULT ltrtDownmixInit(CAudioObjectDecoder *paudec, struct LtRtDownmix *pLtRtDownmix, Int nHalfFilterLen,
                          Int samplingRate, Float maxVal, Int bufSize);
//                          Int samplingRate, DataType maxVal, Int bufSize);
// APIs for LtRtBufType (float) I/O
WMARESULT ltrtDownmixInitBuf(CAudioObjectDecoder *paudec, struct LtRtDownmix *pLtRtDownmix, Int nHalfFilterLen,
                             Int samplingRate, Float maxVal,
                             Int bufSize, PCMFormat *pcmIn);
//                             Int samplingRate, DataType maxVal,
//                             Int bufSize, PCMFormat *pcmIn);
#if 0
// APIs for U8 (PCM) I/O
WMARESULT ltrtDownmixInitBufPCM(CAudioObjectDecoder *paudec, LtRtDownmix *pLtRtDownmix,
                                Int nHalfFilterLen, Int samplingRate,
                                DataType maxVal, Int bufSize, PCMFormat *pcmIn);
#endif
//From wmabuffilt.h
WMARESULT bufFiltInit(CAudioObjectDecoder *paudec, struct BufFilt *pBufFilt, Double *coeffs, __in_bound Int startPos, __in_bound Int endPos, Int cInputs);
void bufDelayFree(CAudioObjectDecoder *paudec, struct BufDelay *pBufDelay);
WMARESULT bufDelayInit(CAudioObjectDecoder *paudec, struct BufDelay *pBufDelay, Int delay);
void bufFiltFree(CAudioObjectDecoder *paudec, BufFilt *pBufFilt);
WMARESULT bufFilt(CAudioObjectDecoder *paudec, BufFilt *pBufFilt, float *pInOutBuf, Int cLen);
WMARESULT bufFilt_SSE(CAudioObjectDecoder *paudec, BufFilt *pBufFilt, float *pInOutBuf, Int cLen);
WMARESULT bufDelay(CAudioObjectDecoder *paudec, BufDelay *pBufDelay, float *in, Int inLen, Int *pOutLen);

#ifdef BUILD_WMAPROLBRV3
WMARESULT plusDecodeBasePeak(CAudioObjectDecoder *paudec);
WMARESULT plusDecodeBasePeak_Channel(CAudioObjectDecoder *paudec, struct PerChannelInfo *ppcinfo);
#endif

WMARESULT prvWipeCleanDecoder(CAudioObjectDecoder* paudec);

#ifdef __cplusplus
extern "C" 
{  // only need to export C interface if
	// used by C++ source code
#endif

	CBasePlusObject* bpNew(struct CAudioObjectDecoder *paudec);
	Void             bpDelete(struct CAudioObjectDecoder *paudec, 
	struct CAudioObject*    pau,
		CBasePlusObject* pbp);
	WMARESULT        bpInit (struct CAudioObjectDecoder *paudec, 
	struct CAudioObject*    pau,
		CBasePlusObject* pbp);
	WMARESULT prvBPInvChannelXForm(struct CAudioObject* pau, 
		CoefType**           rgpCoefGrpSrc, 
	struct CChannelGroupInfo*   pcgi);
	WMARESULT prvBPInvChannelXFormOverlay(struct CAudioObject*        pau, 
		CoefType**           rgpCoefGrpSrc, 
		I32**                 rgpiBPBadCoef,
	struct CChannelGroupInfo*   pcgi);

	WMARESULT prvBPInvQuantize(struct CAudioObject*        pau, 
	struct CChannelGroupInfo*   pcgi);
	WMARESULT prvBasePlusReconCoefficients(struct CAudioObject* pau);
	WMARESULT prvBPAddBasePlustoBase(struct CAudioObject* pau);
	WMARESULT prvBasePlusResampleScaleFactor (const struct CAudioObject* pau, 
		__in_ecount(cValidSFIn) Int* rgiSFQIn, Int cSubbandIn, Int cValidSFIn, 
		__out_ecount(cValidSFOut) Int* rgiSFQOut, Int cSubbandOut, Int cValidSFOut);
	Void  prvSetSFIndex(struct CAudioObject* pau,
		const I32  iTblIdx,
		const U32* rgiSFFreq,
		const I32  cSFFreqSize,
		I16*       rgiSFBandIndexOrig,
		Int*       rgcValidSFBand);
	Void prvBasePlusAdaptToSubFrameConfig(struct CAudioObject* pau);
	Void prvBasePlusFrameInit(struct CAudioObject* pau);

	WMARESULT prvBasePlusInitMinMaxOffset (struct CAudioObject *pau);
	WMARESULT prvBasePlusSaveSubFramePCMReconToLongHist(struct CAudioObject *pau);
	WMARESULT prvBasePlusGetMinMaxOffsetFromSubFrmSize (struct CAudioObject *pau, I32 iSizeCurr, I32 *piMinOffset, I32 *piMaxOffset);

	// Forward data type decl
	//typedef struct PerChannelInfo PerChannelInfo;
	WMARESULT prvBasePlusGetRefInLongHistPerCh (struct CAudioObject * pau, struct PerChannelInfo *ppcinfo, Int iCh);
	WMARESULT prvBasePlusTransformMono(struct CAudioObject *pau,
		const struct PerChannelInfo *ppcinfo,
		U32 *piMagnitude,
		const CoefType *pInput,
		CoefType *rgCoef,
		Int iCh);
	Int prvBasePlusIdentifyBadlyCodedCoef(struct CAudioObject* pau);
	WMARESULT prvBasePlusUpdateLastCodedCoefIndex(CAudioObject* pau, 
	struct PerChannelInfo *ppcinfo);

	WMARESULT prvInvWeightBPCoefOverlay(struct CAudioObject *pau, struct CChannelGroupInfo* pcgi);
	WMARESULT prvInvWeightBPCoefOverlay16dBCompensation(struct CAudioObject *pau, struct CChannelGroupInfo* pcgi);
	WMARESULT prvBasePlusCopyChannelXFormFromBaseCommon (struct CAudioObject *pau);
	Void prvBasePlusSetNonLinearQuantizer (CBasePlusObject *pbp);

	Void prvBasePlusGetSFIndex(const struct CAudioObject *pau, I16 *piSFBandIndex, Int iWin);
	WMARESULT prvBasePlusCopyMaskBandIndexFromBaseOverlay(struct CAudioObject *pau);
	Bool prvBasePlusIfBaseIsZero(struct CAudioObject *pau);
	WMARESULT prvBasePlusDecideStartPositionExclusiveMode(struct CAudioObject *pau);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" 
{  // only need to export C interface if
	// used by C++ source code
#endif

	typedef struct DecoderRunLevelTables
	{
		const U16   *piIndexToRun;
		const U16   *piIndexToLevel;
		const U16   *piRunLevelTableDec;
	}DecoderRunLevelTables;

	WMARESULT bpdecInit(struct CAudioObjectDecoder* paudec);
	WMARESULT bpdecDecodeTile(struct CAudioObjectDecoder* paudec, U32 uVal);
	WMARESULT prvBasePlusDecodeRLCCoefQ (struct CAudioObject*  pau, 
	struct CAudioObjectDecoder* paudec,
	struct PerChannelInfo* ppcinfo);
	WMARESULT prvGetNextBPRunDECVecTableIndex(Void*           pobjin, 
	struct PerChannelInfo* ppcinfo);
	WMARESULT prvGetNextBPRunDECVecNonRLV4(Void*           pobjin, 
	struct PerChannelInfo* ppcinfo);

	WMARESULT prvBasePlusDecodeTileQuantStepSize(struct CAudioObjectDecoder* paudec,
		Int* pcBPTileQuant);
	WMARESULT prvBasePlusDecodeChannelQuantStepSize(struct CAudioObjectDecoder* paudec);
	WMARESULT prvBasePlusEntropyDecodeChannelXform (struct CAudioObjectDecoder* paudec);
	WMARESULT prvBasePlusDecodeTileScaleFactors(struct CAudioObjectDecoder* paudec);
	WMARESULT prvDecodeSFBandTableIndex(struct CAudioObjectDecoder* paudec);
	WMARESULT prvBasePlusDecodeFirstTileHeaderExclusiveMode(struct CAudioObjectDecoder* paudec);
	WMARESULT prvBasePlusDecodeScaleFactorRunLevel  (struct CAudioObjectDecoder* paudec, 
	struct PerChannelInfo* ppcinfo,
		Int iBandLim,
	struct PerChannelInfo* ppcinfoPred,
		const U16     *rgiRunEntry,
		const U16     *rgiLevelEntry,
		const U16     *rgunHuffDecTblMaskRLCV3);
	WMARESULT prvGetNextSFRunDEC(Void* pobjin, 
	struct PerChannelInfo *ppcinfo,
		const U16      *rgiRunEntry,
		const U16      *rgiLevelEntry,
		const U16      *rgunHuffDecTblMaskRLCV3,
		const U16      ESC_RUNBITS,
		const U16      ESC_LEVBITS);

	WMARESULT prvBasePlusDecodeCoefQPredictor (struct CAudioObjectDecoder* paudec, 
	struct PerChannelInfo* ppcinfo);
	WMARESULT prvBasePlusInverseCoefQPrediction(struct CAudioObject*  pau, 
	struct CAudioObjectDecoder* paudec,
	struct PerChannelInfo* ppcinfo);

	WMARESULT prvBasePlusDecodeInterleaveModeParams(struct CAudioObjectDecoder* paudec, struct PerChannelInfo *ppcinfo);
	WMARESULT prvBasePlusDecodeRLCCoefQAndDeinterleaveCoefQ(struct CAudioObjectDecoder* paudec, struct PerChannelInfo* ppcinfo);

	WMARESULT prvDecodeCfpdModeParams (struct CAudioObjectDecoder* paudec, 
	struct PerChannelInfo* ppcinfo);

	WMARESULT prvBasePlusDecoderPrepareRefSubFrmPerTile (struct CAudioObject * pau);

	WMARESULT prvBasePlusDecodePDFShiftModeParams (struct CAudioObjectDecoder* paudec, 
	struct PerChannelInfo* ppcinfo);

	WMARESULT prvBasePlusInversePDFShiftRLCCoefQ(struct CAudioObjectDecoder *paudec, struct PerChannelInfo* ppcinfo);

	WMARESULT prvDecodeBasePlusOverlayMode(struct CAudioObjectDecoder *paudec);
	WMARESULT prvBasePlusDecodeTileExclusiveMode(struct CAudioObjectDecoder *paudec);
	WMARESULT prvBasePlusDecodeFirstTileHeaderOverlayMode(struct CAudioObjectDecoder* paudec);

	WMARESULT plusDecodeBasePlusSuperFrameHeader(struct CAudioObjectDecoder *paudec);
	WMARESULT prvBasePlusCopyChannelXFormToBase (struct CAudioObjectDecoder *paudec);
	WMARESULT prvBasePlusDecodeWeightFactorOverlayMode(struct CAudioObjectDecoder *paudec);
	WMARESULT prvBasePlusReset(struct CAudioObjectDecoder *paudec);

	WMARESULT prvBasePlusDecodeRLCCoefQ_ExplicitStartPos (struct CAudioObject* pau, 
	struct CAudioObjectDecoder* pcaller, 
	struct PerChannelInfo* ppcinfo);

#ifdef __cplusplus
}
#endif

#endif //__MSAUDIO_H_

