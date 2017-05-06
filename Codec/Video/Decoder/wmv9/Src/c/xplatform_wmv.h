//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************


#ifndef __XPLATFORM__
#define __XPLATFORM__

#include "voWMVDecID.h"

//#define XDM
//#define WMV9_SIMPLE_ONLY

/* Disable unused code.*/
#define DISABLE_UNUSED_CODE_INTERPOLATE
#define DISABLE_UNUSED_CODE_IDCT
#define DISABLE_UNUSED_CODE_FRAMETIMER
#define DISABLE_UNUSED_CODE_INIT

#ifdef VOARMV7
#define DEBLOCK_H_NEON_OPT
#define DEBLOCK_V_NEON_OPT
#endif

/* For debug ARMV7 optimization ASM code of deblock module. */
#ifdef _win32_ 
#define DEBLOCK_H_NEON_OPT
#define DEBLOCK_V_NEON_OPT
#endif

//If want to build simple profile version,should enable this marco and set "PRO_VER	EQU 1" in xplatform_arm_asm.h file at the same time.
//#define WMV9_SIMPLE_ONLY  

//wshao added in 08.11.16
#ifdef _win32_ //It is defined in project setting.
#define _EMB_WMV3_  
#define x86  
#define __STREAMING_MODE_DECODE_
#endif

#define WMVIMAGE_V2

#ifdef ARM_C //It is defined in project setting.

#if defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
typedef void* HANDLE;
typedef unsigned long DWORD_PTR;
#endif

#define _EMB_WMV3_  
#define _EMB_WMV2_
#define __STREAMING_MODE_DECODE_
#define WMV_OPT_COMBINE_LF_CC
#define WMV_OPT_SPRITE
#define IDCTDEC_32BIT_INPUT
#define OUTPUT_ALL_FORMATS
#define _SLIM_C_
#	ifndef WMV9_SIMPLE_ONLY
#		define WMVIMAGE_V2
#		define WMV_C_OPT_WVP2
#	endif

#endif //ARM_C

#ifdef LINUX
#	ifndef WMVIMAGE_V2
#		define WMVIMAGE_V2
#	endif
#endif

#ifdef WMV9_SIMPLE_ONLY
#    define WMV789_ONLY
#endif

#if defined(_MAC) || defined(macintosh)
#   define macintosh
#   define _MAC
#   define __MACVIDEOPRE__
#   define __MACVIDEO__
#   define NO_WINDOWS
#   define _BIG_ENDIAN_
#   define _SLIM_C_
#   define __STREAMING_MODE_DECODE_
# if 1 //wshao.Must define this marco,or doesn't support WVP2.See begeinning of the struct tagWMVDecInternalMember.
#   define OUTPUT_ALL_FORMATS       // all color conversion
# endif
#   define _MAC_VEC_OPT
#   define _MAC_COLORSPACE_
    //#define _BIG_HUFFMAN_TABLE_
    //#undef  _EMB_WMV2_
    //#define DebugBreak() (void)0

#   define _EMB_WMV3_
#   define WMV_OPT_SPRITE

#   define _SUPPORT_POST_FILTERS_
    // Jerry -- should this be 1 or 4 for the Mac?
#   define _TOP_POST_FILTER_LEVEL_ 4
    //#define PROFILE_FRAMES
    //#define PROFILE_FRAMES_TERSE

    //#include "MacPort.h"
#   include <wchar_t.h>
#	include "voWmvPort.h"

    typedef void* HANDLE;
    typedef unsigned int DWORD_PTR;

#   define TCHAR_2 char
//#   define _T(a) a
#   define _tcslen strlen
#   define _tcsncat strncat
#   define _tcscat strcat
#   define _stprintf sprintf

#endif //macintosh

#ifdef _XBOX
#   include <xtl.h>
#endif

#ifdef _BIG_ENDIAN_
#   define X_INDEX 1
#   define Y_INDEX 0
#else
#   define X_INDEX 0
#   define Y_INDEX 1
#endif //_BIG_ENDIAN_

//**************************************************************************************************
#if defined( _Embedded_x86)||defined(_APOGEE_)
#   define NO_WINDOWS
#   undef _USE_INTEL_COMPILER
    typedef unsigned long       DWORD;
    typedef unsigned short      WORD;
    typedef long LONG;
    typedef unsigned char BYTE;
    typedef void *HANDLE; 
    typedef void *LPVOID; 
    typedef long HDC; // IW
    typedef BYTE BOOL; //IW
    typedef unsigned int UINT; // IW
    typedef void VOID;
    // IW #define I32 long
    //#define Bool int
#   define NULL    0//((void *)0)
#   define NOERROR             0
#   define TRUE 1
#   define FALSE    0
#   define AVIIF_KEYFRAME  0x00000010L

#   undef WMAPI
#   define WMAPI
#   undef _stdcall
#   define _stdcall

#   define CONST const
#endif

#if defined(_Embedded_x86) || defined(_XBOX) || defined(_APOGEE_)

#   ifndef _APOGEE_
#     define BI_BITFIELDS     3L
#     define BI_RGB           0
#   endif

#   ifndef _HRESULT_DEFINED
#       define _HRESULT_DEFINED
        typedef LONG HRESULT;
#   endif // !_HRESULT_DEFINED

#   define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#   define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#   define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#   define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#   define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#   define MoveMemory RtlMoveMemory
#   define CopyMemory RtlCopyMemory
#   define FillMemory RtlFillMemory
#   define ZeroMemory RtlZeroMemory

#   define min(a,b)    (((a) < (b)) ? (a) : (b))

#   ifndef _BITMAPINFOHEADER
#       define _BITMAPINFOHEADER
        typedef struct tagBITMAPINFOHEADER{
            DWORD      biSize;
            LONG       biWidth;
            LONG       biHeight;
            WORD       biPlanes;
            WORD       biBitCount;
            DWORD      biCompression;
            DWORD      biSizeImage;
            LONG       biXPelsPerMeter;
            LONG       biYPelsPerMeter;
            DWORD      biClrUsed;
            DWORD      biClrImportant;
        } BITMAPINFOHEADER, *LPBITMAPINFOHEADER;
#   endif

#   ifndef _BITMAPINFO
#       define _BITMAPINFO
#       ifdef _XBOX
            typedef struct tagRGBQUAD {
                BYTE    rgbBlue; 
                BYTE    rgbGreen; 
                BYTE    rgbRed; 
                BYTE    rgbReserved; 
            } RGBQUAD; 
            typedef struct tagBITMAPINFO {
                BITMAPINFOHEADER    bmiHeader;
                RGBQUAD             bmiColors[1];
            } BITMAPINFO, *LPBITMAPINFO;
#       endif
#   endif

#   if !defined( _BITMAPFILEHEADER)&& (defined(_APOGEE_) || defined(_XBOX))
#       define _BITMAPFILEHEADER
        typedef struct _tagBITMAPFILEHEADER {
            WORD    bfType;
            DWORD   bfSize;
            WORD    bfReserved1;
            WORD    bfReserved2;
            DWORD   bfOffBits;
        } BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#   endif
    //IW check MAKEFOURCC
#   ifndef _XBOX
#       define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
            ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
            ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#   endif
#   ifndef mmioFOURCC
#       define mmioFOURCC(ch0, ch1, ch2, ch3)  MAKEFOURCC(ch0, ch1, ch2, ch3)
#   endif
#   ifndef _XBOX
#       ifndef _WAVEFORMATEX_
#           define _WAVEFORMATEX_
            typedef struct tWAVEFORMATEX
            {
                WORD    wFormatTag;         /* format type */
                WORD    nChannels;          /* number of channels (i.e. mono, stereo...) */
                DWORD   nSamplesPerSec;     /* sample rate */
                DWORD   nAvgBytesPerSec;    /* for buffer estimation */
                WORD    nBlockAlign;        /* block size of data */
                WORD    wBitsPerSample;     /* number of bits per sample of mono data */
                WORD    cbSize;             /* the count in bytes of the size of */
            }   WAVEFORMATEX;
#       endif
#   endif // xbox
#endif // EMbedded_x86 || xbox

//**************************************************************************************************
#if UNDER_CE>410
#   define WM_PF_ARM_V4             PF_ARM_V4
#   define WM_PF_ARM_V5             PF_ARM_V5
#   define WM_PF_ARM_INTEL_XSCALE   PF_ARM_INTEL_XSCALE
#   define WM_PF_ARM_INTEL_WMMX     PF_ARM_INTEL_WMMX
#else
#   define WM_PF_ARM_V4             0x80000001
#   define WM_PF_ARM_V5             0x80000002
#   define WM_PF_ARM_INTEL_XSCALE   0x80010001
#   define WM_PF_ARM_INTEL_WMMX     0x80010003
#endif

#ifndef ARM_C 
#if defined(__arm) || defined(LINUX)
    // Embedded ARM
//#   define _ARM_                    // inherit _ARM_ config 
#   define _EMB_WMV3_
#   define __STREAMING_MODE_DECODE_
#   define _SLIM_C_                 // no multithread
# if 1 //wshao.Must define this marco,or doesn't support WVP2.See begeinning of the struct tagWMVDecInternalMember.
#   define OUTPUT_ALL_FORMATS       // all color conversion
# endif
#   define __huge
#   define NO_WINDOWS
    typedef void* LPVOID;
    typedef void* HANDLE;
    typedef long long LARGE_INTEGER;
    typedef char TCHAR_2;
    typedef char* LPCTSTR;
//#   define _T(a) a
#   define TEXT(a) a
#   define _stprintf sprintf
#   define UINT_MAX  0xffffffffU
    typedef unsigned long DWORD;
    typedef unsigned int DWORD_PTR;
#endif
#endif //ARM_C

#if defined(_APOGEE_)
#   define _EMB_WMV2_
#   define __STREAMING_MODE_DECODE_
#   define _SLIM_C_                 // no multithread
# if 1 //wshao.Must define this marco,or doesn't support WVP2.See begeinning of the struct tagWMVDecInternalMember.
#   define OUTPUT_ALL_FORMATS       // all color conversion
# endif
#   define __huge
#   define NO_WINDOWS
    typedef void* LPVOID;
    typedef void* HANDLE;
    typedef long long LARGE_INTEGER;
    typedef char TCHAR_2;
    typedef char* LPCTSTR;
//#   define _T(a) a
#   define TEXT(a) a
#   define _stprintf sprintf
    typedef unsigned long DWORD;
#endif

#if defined(_TI_C55X_)
    // Embedded TI c55x
#   define _CE_WMV2_
#   define __huge
#   define NO_WINDOWS
    typedef void* LPVOID;
    typedef void* HANDLE;
    typedef long long LARGE_INTEGER;
    typedef char TCHAR_2;
    typedef char* LPCTSTR;
    typedef int WORD;
//#   define _T(a) a
#   define TEXT(a) a
#   define _stprintf sprintf
#   define UINT_MAX  0xffffffffU
    typedef unsigned long DWORD;
#   define OUTPUT_RGB16

#if   !defined( _BITMAPFILEHEADER)
    #define _BITMAPFILEHEADER
      typedef struct _tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
      } BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;
#endif

#endif

//**************************************************************************************************
#if (defined(UNDER_CE) || defined(_XBOX)) && !defined(macintosh) // only works with little-endian for now, so no PowerPC
#   if defined(_X86_) && 0
        // To simulate a CEPC on eVC X86em, define _WMV_TARGET_X86_ and _SUPPORT_POST_FILTERS_ here, and enable all *_x86.c files in wmvdec.vcp
#       ifndef _WMV_TARGET_X86_
#           define _WMV_TARGET_X86_
#       endif
#   endif // defined(_X86_) 

/*
 * _SUPPORT_POST_FILTERS_ is defined for X86 to match the build for command line for x86 platform. 
 *  This helps command line wmvdiff tool conforms to evc4.0 wmvdiff. 
*/ 

#   if defined(_X86_)
#       ifndef _SUPPORT_POST_FILTERS_
//#           define _SUPPORT_POST_FILTERS_
#       endif
#   endif


#   if defined(_WMV_TARGET_X86_)
#       undef _EMB_WMV2_
//#       define _EMB_WMV3_
#   else
    // we have EMB and non-EMB code path, either way should work
#       define _EMB_WMV3_
#   endif

#   define __STREAMING_MODE_DECODE_

#   ifndef WMV9_SIMPLE_ONLY
#   define WMVIMAGE_V2
#	define WMV_C_OPT_WVP2
#   endif


#   ifndef _SLIM_C_
#       define _SLIM_C_                 // no multithread
#   endif
# if 1 //wshao.Must define this marco,or doesn't support WVP2.See begeinning of the struct tagWMVDecInternalMember.
#   ifndef OUTPUT_ALL_FORMATS
#       define OUTPUT_ALL_FORMATS       // all color conversion
#   endif
# endif
#   if (defined(WMV9_SIMPLE_ONLY) || defined(WMV789_ONLY))
#       undef OUTPUT_ALL_FORMATS       // all color conversion
#       define OUTPUT_YUY2             // these are the two formats for xbox
#       define OUTPUT_RGB32
#   endif
#elif defined(_M_IX86) && !defined(_XBOX)       //BUILD ONLY FOR X86 DESKTOP. 
//#       define _WMV_TARGET_X86_
//#       define _SUPPORT_POST_FILTERS_
#       define __STREAMING_MODE_DECODE_
#       define WMVIMAGE_V2
#		define WMV_C_OPT_WVP2
#       define WMV_OPT_SPRITE

//_EMB_WMV3_ cannot turned on parallel with _WMV_TARGET_X86_ is on for WMVP because the 
// optimization that x86 had doesn't support _EMB_WMV3_ path
// However ansi C _EMB_WMV3_ and _EMB_WMV2_ path should be same.
//#       define _EMB_WMV3_

# if 1 //wshao.Must define this marco,or doesn't support WVP2.See begeinning of the struct tagWMVDecInternalMember.
#   ifndef OUTPUT_ALL_FORMATS
#       define OUTPUT_ALL_FORMATS       // all color conversion
#   endif
#endif
#endif

#define INTERLACEV2_B
#define _WMV9AP_  // this flag has to be on all the time now
//#define _NO_EMB_WMV3_  // use this to turn on and off teh EMB for WMV3

#ifndef ARM_C 
#undef _EMB_WMV2_
#endif

#ifdef _NO_EMB_WMV3_
#   undef  _EMB_WMV3_
#endif


#   define  EMBSEC_DEF ".text"  // default code section

    // initially, all the sections are default
#   define  EMBSEC_PML    EMBSEC_DEF
#   define  EMBSEC_PML2  EMBSEC_DEF

#   define  EMBSEC_BML    EMBSEC_DEF
#   define  EMBSEC_BML2   EMBSEC_DEF

#ifdef _EMB_WMV3_

    //This flag _EMB_SSIMD32_ can only be defined when _EMB_WMV3_ is on, and this is guarded at the later part of the file by a check. 
    // When _EMB_WMV3_ is on, ideally it should be able to be turned on or off. However, due to the fact that some code have not been implemented
    // when it is off, currently it has to be on when _EMB_WMV3_ is on.

#   define _EMB_SSIMD32_

#ifndef _WMV9AP_
    // only defined in the C ansi C code, and gets turned off on every platform below except x86em
#   define _EMB_3FRAMES_

#endif


#   if defined(_X86_) || defined(x86)
#       define  _EMB_SSIMD32_
//#       define _EMB_SSIMD64_
#       define _EMB_WMV2_
#       define WMV_OPT_SPRITE
#       define WMV_OPT_COMBINE_LF_CC
#   endif //_X86_


#   ifdef _MIPS_
#       undef _EMB_3FRAMES_
#       define _EMB_WMV2_
#       define WMV_OPT_SPRITE
#       define WMV_OPT_COMBINE_LF_CC
#       define _MIPS_ASM_MOTIONCOMP_OPT_
#       define _MIPS_ASM_LOOPFILTER_OPT_
#       define _EMB_ASM_MIPS_DECOLORCONV_
//
// Once turn on _MIPS_ASM_IDCTDEC_OPT_, must turn on IDCTDEC_16BIT_3ARGS.
//
#       define _MIPS_ASM_IDCTDEC_OPT_
#       ifdef _MIPS_ASM_IDCTDEC_OPT_
#           define IDCTDEC_16BIT_3ARGS
#       endif

#       if defined(_MIPS64)
#           define _64BIT_
#           define _MIPS64_OPTS_
//#           define _MIPS_5432_
#           define _EMB_SSIMD64_
#           ifndef REG_SIZE
#               define REG_SIZE 8
#           endif
#       else //_MIPS64
#           define _EMB_SSIMD32_
#           ifndef REG_SIZE
#               define REG_SIZE 4
#           endif
#       endif //_MIPS64
#   endif //_MIPS_

#ifndef ARM_C
#   if (defined(_ARM_) || defined(__arm))
#       undef _EMB_3FRAMES_
#       define _EMB_SSIMD32_
#       define _EMB_WMV2_
#       undef  EMBSEC_PML
#       undef  EMBSEC_PML2
#       define EMBSEC_PML ".embsec_PMainLoopLvl1"  // for ARM, turn on individual sections
#       define EMBSEC_PML2 ".embsec_PMainLoopLvl2"  // for ARM, turn on individual sections
#       undef  EMBSEC_BML
#       undef  EMBSEC_BML2
#       define EMBSEC_BML ".embsec_BMainLoopLvl1"  // for ARM, turn on individual sections
#       define EMBSEC_BML2 ".embsec_BMainLoopLvl2"  // for ARM, turn on individual sections

// EMB related ARM optimization, 
#       define WMV_OPT_COMBINE_LF_CC
#       define IDCTDEC_32BIT_INPUT
#       define WMV_OPT_MOTIONCOMP_ARM
#       define WMV_OPT_DYNAMICPAD_ARM
//#     define WMV_OPT_BLKDEC_ARM

// the following three lines should be define(undefine) at the same time
#       define WMV_OPT_NAKED_ARM
#       define WMV_OPT_IDCT_ARM
#       define WMV_OPT_DQUANT_ARM

#   endif //ARM
#endif //ARM_C

#   ifdef _SH3_
#       undef _EMB_3FRAMES_
#       define _EMB_WMV2_
#       ifdef  _EMB_WMV2_
//            this is an important optimization that should be supported but V8 is broken with this enabled and v9 is not yet implemented
//#           define DYNAMIC_EDGEPAD
//#     `     define DYNAMIC_EDGEPAD_0
#       endif
        // SH3_DSP is defined at the project level
        // It is also defined in the custom build settings of each sh3_dsp.src and sh3.src file.
#       ifdef SH3_DSP
#           define USE_SH3_DSP_ASM      
#           ifdef USE_SH3_DSP_ASM
                // you must change the custom build asm settings to match these defines in each sh3_dsp.src file
#               define SH3_DSP_ASM_DECOLORCONV      // asm defines USE_YUV_RGB16_ASM, USE_YUV_RGB24_ASM, USE_YUV_RGB32_ASM
#               define SH3_DSP_ASM_LOOPFILTER
#               define SH3_DSP_ASM_IDCT
// disable SH3_DSP motion comp as SH3 motion comp failed and sils has not tested sh3_dsp.
#               define SH3_DSP_ASM_MOTION_COMP
#               define WMV_OPT_IDCT_SHX
#               define WMV_OPT_MOTIONCOMP_SHX
#               define WMV_OPT_MOTIONCOMP_SH3DSP
#               define WMV_OPT_HUFFMAN_GET_SHX

#           endif
#       else
#           define USE_SH3_ASM          ;; you must change *_sh3.src asm settings to match these
#           ifdef USE_SH3_ASM
                // you must change the custom build asm settings to match these defines in each sh3.src file
#               define SH3_ASM_DECOLORCONV
#               define SH3_ASM_LOOPFILTER
// disable Motion comp ASM for SH3 as it fails wmvdiff test -- likely changes have occured to codebase.
#               define SH3_ASM_MOTION_COMP
#               define WMV_OPT_IDCT_SHX
#               define WMV_OPT_MOTIONCOMP_SHX
#               define WMV_OPT_HUFFMAN_GET_SHX
#               define OPT_IDCT_SHX 3 
#               define _SHX_ASM_X8_OPT_ 3
#           endif
#       endif
#       if defined(SH3_ASM_MOTION_COMP) || defined(SH3_DSP_ASM_MOTION_COMP) 
#           undef DYNAMIC_EDGEPAD
#           undef DYNAMIC_EDGEPAD_0
#       endif
#   endif //_SH3_

#   ifdef _SH4_

        // typedef unsigned int DWORD_PTR;
#       undef _EMB_3FRAMES_
        // you must change the custom build asm settings to match these defines in each sh4.src file
#       define _EMB_WMV2_ 
#       define _SH4_ASM_LOOPFILTER_OPT_
#       define OPT_DECOLORCONV_SH4
        //#       define _SH4_ASM_MOTIONCOMP_OPT_

#       define WMV_OPT_IDCT_SHX
//#       define WMV_OPT_MOTIONCOMP_SHX
//#       define WMV_OPT_HUFFMAN_GET_SHX
#       define OPT_IDCT_SHX 4
#       define _SHX_ASM_X8_OPT_ 4
#   endif //_SH4_

#endif  //_EMB_WMV3_ 

#ifndef ARM_C
// EMB independent ARM optimization
#if (defined(_ARM_) || defined(__arm))
#   define _XSC_ 1
#   define WMV_OPT_SPRITE
#   define WMV_OPT_SPRITE_ARM
#   define WMV_OPT_COMMON_ARM
#   define WMV_OPT_REPEATPAD_ARM
#   define WMV_OPT_INTENSITYCOMP_ARM
#   define WMV_OPT_LOOPFILTER_ARM
#   define WMV_OPT_MULTIRES_ARM
#   define WMV_OPT_HUFFMAN_GET_ARM 
#   define WMV_OPT_DECOLORCONV_ARM
#   define WMV_OPT_PPL_ARM
#   define WMV_OPT_X8_ARM

#   define WMV_OPT_WVP2_ARM

#   if !defined(__arm)
#       if UNDER_CE > 410
//#           define _WMMX_
#       endif
//#       define _SUPPORT_POST_FILTERS_
#       define _TOP_POST_FILTER_LEVEL_ 2
#   endif
#endif //ARM
#endif //ARM_C

// WMMX related begin
#ifdef _WMMX_
#   define PHYSADDR     0x5C000000>>8
#   define PHYSMEMSIZE  256*1024
#endif
// WMMX related end

#if !defined(_EMB_WMV3_)
#   undef _EMB_SSIMD32_
#   undef _EMB_SSIMD64_
#endif

#if defined(_EMB_SSIMD32_) && defined(_EMB_SSIMD64_)
#   undef  _EMB_SSIMD32_
#endif

/* You should not define _EMB_SSIMD_. It will automatically defined if either _EMB_SSIMD32,64,128 is defined */
#if defined(_EMB_SSIMD32_) || defined(_EMB_SSIMD64_) || defined(_EMB_SSIMD128_)
#   define _EMB_SSIMD_B_
    //#define _EMB_SSIMD_
#   define _EMB_SSIMD_IDCT_
#   define _EMB_SSIMD_MC_
#   define _EMB_IDCT_SSIMD32_
    //#define _EMB_HYBRID_16_32_IDCT_
    //#define _EMB_IDCT_SSIMD64_
    //#define _EMB_IDCT_SSIMD16_
#endif

#if defined(_SUPPORT_POST_FILTERS_) && !defined(_TOP_POST_FILTER_LEVEL_)
#   define _TOP_POST_FILTER_LEVEL_ 4
#elif !defined(_SUPPORT_POST_FILTERS_) && !defined(_TOP_POST_FILTER_LEVEL_)
#   define _TOP_POST_FILTER_LEVEL_ 0
#endif

//**************************************************************************************************
// memcpy of aligned U32_WMV with cbSiz = number of bytes to copy
// assumes pDst and pSrc are aligned, but casts them in case they are actually byte or short pointers.
#define ALIGNED32_MEMCPY(pDst,pSrc,cbSiz) {                 \
            U32_WMV* pD = (U32_WMV*)(pDst), *pS = (U32_WMV*)(pSrc);     \
            int j;                                         \
            for(j=(cbSiz)>>2; j>0;j--)                  \
                *pD++ = *pS++;                              \
        }
#define ALIGNED32_MEMCPY16(pDst,pSrc) {                     \
            U32_WMV* pD = (U32_WMV*)(pDst), *pS = (U32_WMV*)(pSrc);     \
            *pD++ = *pS++;                                  \
            *pD++ = *pS++;                                  \
            *pD++ = *pS++;                                  \
            *pD   = *pS;                                    \
        }
#define ALIGNED32_MEMCPY8(pDst,pSrc) {                      \
            U32_WMV* pD = (U32_WMV*)(pDst), *pS = (U32_WMV*)(pSrc);     \
            *pD++ = *pS++;                                  \
            *pD   = *pS;                                    \
        }
// memset of aligned U32_WMV with cbSiz = number of bytes to copy
#define ALIGNED32_MEMSET_U32(pDst,u32C,cbSiz) {             \
            U32_WMV* pD=(U32_WMV*)(pDst);                   \
            int j;                                          \
            for( j=(cbSiz)>>2; j>0;j--)                  \
                *pD++ = u32C;                               \
        }
#define ALIGNED32_MEMSET_2U32(pDst,u32C) {                  \
            U32_WMV* pD = (U32_WMV*)(pDst);                         \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
// replicate a byte 4 times in an U32_WMV.
#ifndef OPT_SLOW_MULTIPLY
#   define ALIGNED32_REPLICATE_4U8(a) (((U8_WMV)(a)) * 0x01010101u)
#else
#   define ALIGNED32_REPLICATE_4U8(a) (((U8_WMV)(a)<<24) | ((U8_WMV)(a)<<16) | ((U8_WMV)(a)<<8) | (U8_WMV)(a))
#endif
// memset of aligned U32_WMV with a replicated U8_WMV constant
#define ALIGNED32_MEMSET_U8(pDst,u8C,cbSiz) {               \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            ALIGNED32_MEMSET_U32( (pDst), u32C, (cbSiz) );  \
        }
#define ALIGNED32_MEMSET_8U8(pDst,u8C) {                    \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
#ifndef OPT_SMALL_CODE_SIZE
#   define ALIGNED32_MEMSET_20U8(pDst,u8C) {                \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
#   define ALIGNED32_MEMSET_24U8(pDst,u8C) {                \
            const U8_WMV  u8T = (U8_WMV)(u8C);                      \
            const U16_WMV u16T = (u8T<<8) | u8T;                \
            const U32_WMV u32C = (u16T<<16) | u16T;             \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD++ = u32C;                                   \
            *pD   = u32C;                                   \
        }
#   define ALIGNED32_MEMCLR_128U8(pDst) {                   \
            U32_WMV* pD=(U32_WMV*)(pDst);                           \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
            *pD++ = 0;  *pD++ = 0;  *pD++ = 0; *pD++ = 0;   \
        }
#else // OPT_SMALL_CODE_SIZE
#   define ALIGNED32_MEMSET_20U8(pDst,u8C) ALIGNED32_MEMSET_U8((pDst),(u8C),20)
#   define ALIGNED32_MEMSET_24U8(pDst,u8C) ALIGNED32_MEMSET_U8((pDst),(u8C),24)
#   define ALIGNED32_MEMCLR_128U8(pDst)    ALIGNED32_MEMSET_U32((pDst),0,128)
#endif

// platform specific memory operations
#ifdef WMV_OPT_COMMON_ARM
    #define ALIGNED32_MEMCLR_256(pDst,u8C,cbSiz)        memclr256_arm(pDst)
    #define ALIGNED32_MEMCLR_128(pDst,u8C,cbSiz)        memclr128_arm(pDst)
    #define ALIGNED32_FASTMEMCPY(pDst,pSrc,nBytes)      prvWMMemCpyAligned(pDst, pSrc, nBytes)
    #define ALIGNED_FASTMEMCPY16(pDst,pSrc,nBytes)      prvWMMemCpy16Aligned(pDst,pSrc)
    #define ALIGNED_FASTMEMCPY8(pDst,pSrc,nBytes)       prvWMMemCpy8Aligned(pDst,pSrc)
    #define ALIGNEDEST_FASTMEMCPY16(pDst,pSrc,nBytes)   prvWMMemCpy16DestAligned(pDst, pSrc)
    #define ALIGNED32_FASTMEMCLR(pDst, val, cBytes2Clear)   prvWMMemClrAligned(pDst, cBytes2Clear)
    #define FASTMEMCLR(pDst, val, cBytes2Clear)             prvWMMemClr(pDst, cBytes2Clear)
    #define FASTLARGEMEMCPY(pDst,pSrc,nBytes)           prvWMMemCpy(pDst, pSrc, nBytes)
    #define ALIGNED32_FASTMEMSET(pDst, cval, cBytesSet) prvWMMemSetAligned(pDst, cval, cBytesSet)
#else
    #define ALIGNED32_MEMCLR_256(pDst,u8C,cbSiz)        memset(pDst,0,256)
    #define ALIGNED32_MEMCLR_128(pDst,u8C,cbSiz)        memset(pDst,0,128)
    #define ALIGNED32_FASTMEMCPY(pDst,pSrc,nBytes)      memcpy(pDst, pSrc, nBytes)
    #define ALIGNED32_FASTMEMCLR(pDst, val, cBytes2Clear)   memset(pDst, 0, cBytes2Clear)
    #define ALIGNEDEST_FASTMEMCPY16(pDst,pSrc,nBytes)   memcpy(pDst, pSrc, 16)
    #define ALIGNED_FASTMEMCPY16(pDst,pSrc,nBytes)      memcpy(pDst,pSrc, 16)
    #define ALIGNED_FASTMEMCPY8(pDst,pSrc,nBytes)       memcpy(pDst,pSrc, 8)
    #define FASTMEMCLR(pDst, val, cBytes2Clear)             memset(pDst, 0, cBytes2Clear)
    #define ALIGNED32_FASTMEMSET(pDst, cval, cBytesSet) memset(pDst, cval, cBytesSet)
    #define FASTLARGEMEMCPY(pDst,pSrc,nBytes)           memcpy(pDst, pSrc, nBytes)
#endif     

// Average of 8 bytes
#define ALIGNED32_AVERAGE_8U8(avg,pSrc,Rnd) {                                                                           \
            const U32_WMV u32S0 = *((U32_WMV*)(pSrc)), u32S1 = *((U32_WMV*)(pSrc+4));                                               \
            avg = (u32S0 & 0x00ff00ff) + ((u32S0>>8) & 0x00ff00ff) + (u32S1 & 0x00ff00ff) + ((u32S1>>8) & 0x00ff00ff);  \
            avg = ((avg & 0x0000ffff) + ((avg>>16) & 0x0000ffff) + Rnd) >> 3;                                           \
        }
// Sum of 8 bytes (takes 13 ops + 2 reads)
#define ALIGNED32_SUM_8U8(sum,pSrc) {                                                                                   \
            const U32_WMV u32S0 = *((U32_WMV*)(pSrc)), u32S1 = *((U32_WMV*)(pSrc+4));                                               \
            sum = (u32S0 & 0x00ff00ff) + ((u32S0>>8) & 0x00ff00ff) + (u32S1 & 0x00ff00ff) + ((u32S1>>8) & 0x00ff00ff);  \
            sum = (sum & 0x0000ffff) + ((sum>>16) & 0x0000ffff);                                                        \
        }

//**************************************************************************************************
#if defined(PROFILE) 
#if 0 // CLEAR_USELESS_CODE
#   include "AutoProfile.h"
#endif
#else
#   define FUNCTION_PROFILE(fp)
#   define FUNCTION_PROFILE_START(fp,id)
#   define FUNCTION_PROFILE_STOP(fp)
#   define FUNCTION_PROFILE_DECL_START(fp,id)
#   define FUNCTION_PROFILE_SWITCH(fp,id)
#endif
#ifdef _ARM_
//#   define PROFILE_FRAMES_TERSE
//#   define PROFILE_FRAMES
#endif

//#   define PROFILE_FRAMES_TERSE
//#   define PROFILE_FRAMES

//#define PROFILE_CACHE_ANALYSIS

#ifdef PROFILE_FRAMES_TERSE
    // only define this to collect function distributions - but ignore timing while doing this collection
    //#   define PROFILE_FRAMES_FUNCTION_COUNT
#endif
#if defined(PROFILE_FRAMES) || defined(PROFILE_FRAMES_TERSE)
#   define DEBUG_PROFILE_FRAME_INIT WMVInitFrameProfile()
    extern void WMVInitFrameProfile();
#   define DEBUG_PROFILE_FRAME_DECODE_DATA_START WMVStartFrameProfile()
    extern void WMVStartFrameProfile(void);
#   define DEBUG_PROFILE_FRAME_DECODE_DATA_STOP WMVMidFrameProfile()
    extern void WMVMidFrameProfile(void);
#   define DEBUG_PROFILE_FRAME_GET_OUTPUT_START WMVStartGetOutputFrameProfile()
    extern void WMVStartGetOutputFrameProfile(void);
#   define DEBUG_PROFILE_FRAME_GET_OUTPUT_STOP WMVEndFrameProfile()
    extern void WMVEndFrameProfile(void);
#   define DEBUG_PROFILE_FRAME_SET_TYPE(iType) WMVTypeFrameProfile( iType )
    extern void WMVTypeFrameProfile( int iType );
#   define DEBUG_PROFILE_FRAME_SET_FLAGS(iFlags) WMVFlagFrameProfile( iFlags )
    extern void WMVFlagFrameProfile( unsigned int uiFlags );
#   define DEBUG_PROFILE_FRAME_OUTPUT_LOG WMVOutputFrameProfile()
    extern void WMVOutputFrameProfile();
#   define DEBUG_PROFILE_FRAME_PRINTSTR(tmsg) WMVFrameProfilePrint(tmsg)
    extern void WMVFrameProfilePrint(void* msg);
#   define DEBUG_SHOW_FRAME_BUFFERS(pCur,pRef0,pRef1) WMVShowBuffersFrameProfile((pCur),(pRef0),(pRef1))
    extern void WMVShowBuffersFrameProfile(void* pCurr, void* pRef0, void* pRef1);
#   define DEBUG_SHOW_FRAME_OUTPUT(pCur) WMVShowOutputFrameProfile(pCur);
    extern void WMVShowOutputFrameProfile(void* pCurr);
#   define DEBUG_SHOW_FRAME_INPUT(pWMVDec,uiLen,pBuf) WMVShowInputFrameProfile(pWMVDec,uiLen,pBuf);
    extern void WMVShowInputFrameProfile(void* pWMVDec, unsigned int uiLen, void* pBuff);
#   define DEBUG_SHOW_FRAME_COPY_BUFFER(pDst,pSrc) WMVShowCopyBufferFrameProfile(pDst,pSrc);
    extern void WMVShowCopyBufferFrameProfile(void* pDst, void* pSrc);
#   define DEBUG_PROFILE_FRAME_MESSAGE(szMsg) WMVMessageFrameProfile(szMsg);
    extern void WMVMessageFrameProfile(void* pMsg);
#   define DEBUG_PROFILE_FRAME_ANNOTATE(pAnnotate,pAppend,iT,pTail) WMVAnnotateFrameProfile(pAnnotate,pAppend,iT,pTail);
    extern void WMVAnnotateFrameProfile(const void* const pAnnotate,const void* const pAppend, const int iT, const void* const pTail);
#else
#   define DEBUG_PROFILE_FRAME_INIT
#   define DEBUG_PROFILE_FRAME_DECODE_DATA_START
#   define DEBUG_PROFILE_FRAME_DECODE_DATA_STOP
#   define DEBUG_PROFILE_FRAME_GET_OUTPUT_START
#   define DEBUG_PROFILE_FRAME_GET_OUTPUT_STOP
#   define DEBUG_PROFILE_FRAME_SET_TYPE(iType)
#   define DEBUG_PROFILE_FRAME_SET_FLAGS(iFlags)
#   define DEBUG_PROFILE_FRAME_OUTPUT_LOG 
#   define DEBUG_PROFILE_FRAME_PRINTSTR(tmsg)
#   define DEBUG_SHOW_FRAME_BUFFERS(pCur,pRef0,pRef1)
#   define DEBUG_SHOW_FRAME_OUTPUT(pCur)
#   define DEBUG_SHOW_FRAME_INPUT(pWMVDec,uiLen,pBuf)
#   define DEBUG_SHOW_FRAME_COPY_BUFFER(pDst,pSrc)
#   define DEBUG_PROFILE_FRAME_MESSAGE(szMsg)
#   define DEBUG_PROFILE_FRAME_ANNOTATE(pAnnotate,pAppend,iT,pTail)
#endif

#ifdef PROFILE_CACHE_ANALYSIS
#   define DEBUG_CACHE_READ_BYTES(pB,iCnt) WMVCacheReadBytes((pB),(iCnt));
    extern void WMVCacheReadBytes( const void* const pB, int iCnt );
#   define DEBUG_CACHE_WRITE_BYTES(pB,iCnt) WMVCacheWriteBytes((pB),(iCnt));
    extern void WMVCacheWriteBytes( const void* const pB, int iCnt );
#   define DEBUG_CACHE_READ_STRIDES(pB,iCnt,iStride,iCntStrides) WMVCacheReadStrides((pB),(iCnt),(iStride),(iCntStrides));
    extern void WMVCacheReadStrides( const void* const pB, int iCnt, int iStride, int iCntStrides);
#   define DEBUG_CACHE_WRITE_STRIDES(pB,iCnt,iStride,iCntStrides) WMVCacheWriteStrides((pB),(iCnt),(iStride),(iCntStrides));
    extern void WMVCacheWriteStrides( const void* const pB, int iCnt, int iStride, int iCntStrides);
#   define DEBUG_CACHE_START_FRAME WMVCacheStartFrame();
    extern void WMVCacheStartFrame( void );
#   define DEBUG_CACHE_STEP_MB WMVCacheStepMB();
    extern void WMVCacheStepMB( void );
#   define DEBUG_CACHE_STEP_MB_ROW WMVCacheStepMBRow();
    extern void WMVCacheStepMBRow( void );
    extern int g_bDebugOutputCacheMiss;             // flag to turn on and off messages used by debugging code
#else // PROFILE_CACHE_ANALYSIS
#   define DEBUG_CACHE_READ_BYTES(pB,iCnt)
#   define DEBUG_CACHE_WRITE_BYTES(pB,iCnt)
#   define DEBUG_CACHE_READ_STRIDES(pB,iCnt,iStride,iCntStrides)
#   define DEBUG_CACHE_WRITE_STRIDES(pB,iCnt,iStride,iCntStrides)
#   define DEBUG_CACHE_START_FRAME
#   define DEBUG_CACHE_STEP_MB
#   define DEBUG_CACHE_STEP_MB_ROW
#endif // PROFILE_CACHE_ANALYSIS

#ifdef PROFILE_FRAMES_FUNCTION_COUNT
#   define DEBUG_PROFILE_FRAME_FUNCTION_COUNT(idxFunction) WMVFunctionCountFrameProfile(idxFunction);
    extern void WMVFunctionCountFrameProfile(void *pFunction);
#else
#   define DEBUG_PROFILE_FRAME_FUNCTION_COUNT(idxFunction)
#endif


//**************************************************************************************************
#ifndef DEBUG_ONLY
#   if defined(DEBUG) || defined(_DEBUG)
#       define DEBUG_ONLY(a) a
#   else
#       define DEBUG_ONLY(a)
#   endif
#endif // DEBUG_ONLY
#ifndef DEBUG_CON_ONLY
#   if (defined(DEBUG) || defined(_DEBUG)) && !defined(UNDER_CE)
        // Some winCE platforms do not support printf.
#       define DEBUG_CON_ONLY(a) a
#   else
#       define DEBUG_CON_ONLY(a)
#   endif
#endif // DEBUG_CON_ONLY

#       define DEBUG_HEAP_ADD(c,a)
#       define DEBUG_HEAP_SUB(c,a)

/*
 * Turn on Heap/Stack Measure on Debug Build by Default
 */

#   if defined(DEBUG) || defined(_DEBUG)
#   define WANT_HEAP_MEASURE
#   define WANT_STACK_MEASURE
#   endif

/*
#ifndef DEBUG_HEAP_ADD
#   if defined(WANT_HEAP_MEASURE)
#       define DEBUG_HEAP_ADD(c,a) {if (!g_HeapMeasureInitialize) {int ci;for (ci = 0;ci<6;ci++){g_cbHeapSize[ci]=0; g_cbMaxHeapSize[ci]=0;} g_HeapMeasureInitialize=1;} g_cbHeapSize[g_iHeapLastClass=c] += a;  if (g_cbHeapSize[c]>g_cbMaxHeapSize[c]) g_cbMaxHeapSize[c] = g_cbHeapSize[c]; }
#       define DEBUG_HEAP_SUB(c,a) { g_cbHeapSize[g_iHeapLastClass=c] -= a; }
#       ifdef __cplusplus
            extern "C"
#       else
            extern 
#       endif
        
        int g_cbMaxHeapSize[7], g_cbHeapSize[7], g_iHeapLastClass,g_HeapMeasureInitialize;

#       define DHEAP_STRUCT 0
#       define DHEAP_FRAMES 1
#       define DHEAP_HUFFMAN 2
#       define DHEAP_LOCALHUFF 3
#       define DHEAP_LOCAL_COUNT 4
#   else
#       define DEBUG_HEAP_ADD(c,a)
#       define DEBUG_HEAP_SUB(c,a)
#   endif
#endif // DEBUG_HEAP_MEASURE
*/

#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

#if defined(UNDER_CE) && !defined(DEBUG) && !defined(__arm)
#   define FORCEINLINE __forceinline
#else
#ifdef _IOS
#   define FORCEINLINE inline
#else
#   define FORCEINLINE __inline
#endif
#endif

// the following two lines set values needed by the arm assembler via incgen
//#if defined(_ARM_) || defined(__arm)
#   define ARCH_V3 0
//#endif // defined(_ARM_) || defined(__arm)

#if defined(_ARM_)
    // This is a test feature for PPC which is ARM only.
//#   define PPC_SUPPORT_PROFILES
#endif // defined(_ARM_)

#ifdef PPC_SUPPORT_PROFILES

    // The following tests a HKCU\Software\Microsoft\Scrunch\Video\Support Profiles to check if support is desired.
    // This is to allow PPC WMP Test team to test the Player's rollover handling for pluggable codecs and is not a feature
    // normally built into the codec.
    // 
    // When this RegKey is not defined, its value is assumed to be 0xFFFFFFFF and so all codecs and profiles are enabled
    // To disable all codecs except a single one, use the value from the mostly zero column below
    // To disable a single codec, use the value from the mostly F column below   
    // To combine several codecs, "or" their mostly 0 values together and take the ones-compliment.

    typedef enum tagWMVSupportProfiles {    // one's comp   decimal 
        WMVSupportMP4S =      0x00000001,   // 0xFFFFFFFE   4294967294    MPEG4 Simple Profile v1.0 subset
        WMVSupportMPG4 =      0x00000002,   // 0xFFFFFFFD   4294967293    MPG4 Early MS 
        WMVSupportMP42 =      0x00000004,   // 0xFFFFFFFB   4294967291    MP42 Early MS 
        WMVSupportMP43 =      0x00000008,   // 0xFFFFFFF7   4294967287    MP43 Early MS 
        WMVSupportWMV7 =      0x00000010,   // 0xFFFFFFEF   4294967279    WMV7 
        WMVSupportWMV8 =      0x00000020,   // 0xFFFFFFDF   4294967263    WMV8
        WMVSupportWMV9SPLL =  0x00000040,   // 0xFFFFFFBF   4294967231    WMV9 Simple Profile Low Level (176x144)
        WMVSupportWMV9SPML =  0x00000080,   // 0xFFFFFF7F   4294967167    WMV9 Simple Profile Medium Level (356x288 but allow larger)
        WMVSupportWMV9MPLL =  0x00000100,   // 0xFFFFFEFF   4294967039    WMV9 Main Profile Low Level Progressive (356x288)
        WMVSupportWMV9MPML =  0x00000200,   // 0xFFFFFDFF   4294966783    WMV9 Main Profile Medium Level Progressive (720x576)
        WMVSupportWMV9MPHL =  0x00000400,   // 0xFFFFFBFF   4294966271    WMV9 Main Profile High Level progressive (1920x1080 but allow larger)
        WMVSupportWMV9MPLLB = 0x00000800,   // 0xFFFFF7FF   4294965247    WMV9 Main Profile Low Level Progressive B-Frames (356x288)
        WMVSupportWMV9MPMLB = 0x00001000,   // 0xFFFFEFFF   4294963199    WMV9 Main Profile Medium Level Progressive B-Frames (720x576)
        WMVSupportWMV9MPHLB = 0x00002000,   // 0xFFFFDFFF   4294959103    WMV9 Main Profile High Level progressive B-Frames (1920x1080 but allow larger)
        WMVSupportWMV9MPMLI = 0x00004000,   // 0xFFFFBFFF   4294950911    WMV9 Main Profile Medium Level Interlace (720x576)
        WMVSupportWMV9MPHLI = 0x00008000,   // 0xFFFF7FFF   4294934527    WMV9 Main Profile High Level interlace (1920x1080 but allow larger)
        WMVSupportWMV9PC =    0x00010000,   // 0xFFFEFFFF   4294901759    WMV9 PC profile
        WMVSupportWMVP =      0x00020000    // 0xFFFDFFFF   4294836223    WMV9 Image
        WMVSupportWMVP2 =     0x00040000    // 0xFFFDFFFF   4294836223    WMV9 Image v2
    } tWMVSupportProfiles;

#   define PPC_TEST_SUPPORT_PROFILE(iMask) if ( 0 == ( dbgWMVReadPPCRegistry() & ((I32_WMV)(iMask)) ) ) \
        { MessageBeep(MB_ICONEXCLAMATION); return( WMV_UnSupportedCompressedFormat ); }

#   define PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,iW,iH,iMask)                       \
        if (   (pWMVDec->m_iFrmWidthSrc <= iW && pWMVDec->m_iFrmHeightSrc <= iH)      \
            || (pWMVDec->m_iFrmWidthSrc <= iH && pWMVDec->m_iFrmHeightSrc <= iW) )    \
            { if ( 0 == ( dbgWMVReadPPCRegistry() & ((I32_WMV)(iMask)) ) )            \
                { MessageBeep(MB_ICONEXCLAMATION); return( WMV_UnSupportedCompressedFormat );  } } \
        else
        // follow this macro with either another call to one of these two or  {}

#else  // PPC_SUPPORT_PROFILES

#   define PPC_TEST_SUPPORT_PROFILE(iMask)
#   define PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,iW,iH,iMask)

#endif // PPC_SUPPORT_PROFILES


#ifdef COVERAGE
#   include "coverage.h"
#else
#   define CVOFFSET(m)
#   define COVERAGE_FREE()
#   define COVERAGE_UPDATE(_fp)
#   define COVERAGE_SEQUENCE(_p)
#   define COVERAGE_PICTURE(_p)
#   define COVERAGE_LINK_TO_WMVDEC(_m,_p)
#   define COVERAGE_CVSIZE(_m)
#   define COVERAGE_CVNUMELEMENTS(_m)
#   define COVERAGE_FILE_ADDR(_m)
#   define COVERAGE_SET_ADDR(_m,_pt)
#   define COVERAGE_CVSET(_m,_v)
#   define COVERAGE_SET_FRAMETYPE(_p)
#   define COVERAGE_INIT_FILE_HANDLE
#   define COVERAGE_CLOSE_FILE
#   define COVERAGE_HUFFMAN(p,v)
#   define COVERAGE_SPECIAL_ICBPCY(p,X,Y,i)
#   define COVERAGE_CLEAR_HUFFMAN(p)
#   define COVERAGE_INIT_HUFFMAN(p,i,t,n)
#   define COVERAGE_INIT_HUFFMAN_ICBPCY
#   define COVERAGE_INIT_HUFFMAN_PCBPCY
#   define COVERAGE_INIT_FILE_STATS(_n)
#   define COVERAGE_ACPRED_FRAME(p,X,Y,v)
#   define COVERAGE_ACPRED_TFIELD(p,X,Y,v)
#   define COVERAGE_ACPRED_BFIELD(p,X,Y,v)
#   define COVERAGE_DCTTAB(p,X,Y,v)
#   define COVERAGE_DCDIFF_SIGN(p,s)
#   define COVERAGE_DCDIFF(p,s)
#   define COVERAGE_DCPRED(p,v)
#   define COVERAGE_ACCOEFSIGN(p,s)
#   define COVERAGE_ACCOEFESC(p,e)
#   define COVERAGE_ACPRED(p,e)
#   define COVERAGE_COEF_INPUT_BITS(n,p)
#   define COVERAGE_COEF_INPUT_BITS16(n,p)
#   define COVERAGE_COEF_INTERNAL_BITS(v)
#   define COVERAGE_BLOCK_NUMBITSESC(p)
#   define COVERAGE_PXFORM(p,x)
#   define COVERAGE_SUBBLKPATUV(p,i,v)
#endif

#endif
