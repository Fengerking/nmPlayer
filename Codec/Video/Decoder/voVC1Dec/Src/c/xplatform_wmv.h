//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#ifndef __XPLATFORM__
#define __XPLATFORM__

/* For debug ARMV7 optimization ASM code of deblock module. */
#ifdef _win32_ 
#define x86  
#endif

#define SHAREMEMORY 1
#define USE_FRAME_THREAD 1


#ifdef ARM_C //It is defined in project setting.
#if defined(LINUX) || defined(_IOS) || defined(_MAC_OS)
typedef void* HANDLE;
#endif
#endif //ARM_C


#if defined(_MAC) || defined(macintosh)
#   define macintosh
#   define _MAC
#   define __MACVIDEOPRE__
#   define __MACVIDEO__
#   define NO_WINDOWS
#   define _BIG_ENDIAN_

#   define _MAC_VEC_OPT
#   define _MAC_COLORSPACE_

    //#include "MacPort.h"
#   include <wchar_t.h>
#	include "voWmvPort.h"

    typedef void* HANDLE;

#   define TCHAR_2 char
#   define _tcslen strlen
#   define _tcsncat strncat
#   define _tcscat strcat
#   define _stprintf sprintf
#endif


#ifdef _BIG_ENDIAN_
#   define X_INDEX 1
#   define Y_INDEX 0
#else
#   define X_INDEX 0
#   define Y_INDEX 1
#endif //_BIG_ENDIAN_
//**************************************************************************************************

#ifndef ARM_C
#if defined(__arm)
    // Embedded ARM
#   define __huge
#   define NO_WINDOWS
    typedef void* LPVOID;
    typedef void* HANDLE;
    typedef long long LARGE_INTEGER;
    typedef char TCHAR_2;
    typedef char* LPCTSTR;
#   define TEXT(a) a
#   define _stprintf sprintf
#   define UINT_MAX  0xffffffffU
    typedef unsigned long DWORD;
#endif
#endif //ARM_C


//**************************************************************************************************
#if (defined(UNDER_CE)) && !defined(macintosh) // only works with little-endian for now, so no PowerPC

    // we have EMB and non-EMB code path, either way should work
#elif defined(_M_IX86)        //BUILD ONLY FOR X86 DESKTOP. 

#endif

#define INTERLACEV2_B

#   define  EMBSEC_DEF ".text"  // default code section

    // initially, all the sections are default
#   define  EMBSEC_PML    EMBSEC_DEF
#   define  EMBSEC_PML2  EMBSEC_DEF

#   define  EMBSEC_BML    EMBSEC_DEF
#   define  EMBSEC_BML2   EMBSEC_DEF
 
#   define _EMB_SSIMD32_

#   if defined(_X86_) || defined(x86)
#       define  _EMB_SSIMD32_
#   endif //_X86_


#ifndef ARM_C
#   if (defined(_ARM_) || defined(__arm))
//#       undef _EMB_3FRAMES_
#       define _EMB_SSIMD32_
#       undef  EMBSEC_PML
#       undef  EMBSEC_PML2
#       define EMBSEC_PML ".embsec_PMainLoopLvl1"  // for ARM, turn on individual sections
#       define EMBSEC_PML2 ".embsec_PMainLoopLvl2"  // for ARM, turn on individual sections
#       undef  EMBSEC_BML
#       undef  EMBSEC_BML2
#       define EMBSEC_BML ".embsec_BMainLoopLvl1"  // for ARM, turn on individual sections
#       define EMBSEC_BML2 ".embsec_BMainLoopLvl2"  // for ARM, turn on individual sections

// EMB related ARM optimization, 
#       define WMV_OPT_MOTIONCOMP_ARM
#       define WMV_OPT_DYNAMICPAD_ARM
//#     define WMV_OPT_BLKDEC_ARM

// the following three lines should be define(undefine) at the same time
#       define WMV_OPT_NAKED_ARM
#       define WMV_OPT_IDCT_ARM
#       define WMV_OPT_DQUANT_ARM

#   endif //ARM
#endif //ARM_C


#ifndef ARM_C
// EMB independent ARM optimization
#if (defined(_ARM_) || defined(__arm))
#   define _XSC_ 1
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

#endif //ARM
#endif //ARM_C

#ifdef VOARMV4
#define _XSC_   0
#define PLD_ENABLE 0
#endif


#define ALIGN(ptr)  (((U32_WMV)(ptr)+7)&~7);

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


// platform specific memory operations
#ifdef WMV_OPT_COMMON_ARM
    #define ALIGNED32_FASTMEMCPY(pDst,pSrc,nBytes)      prvWMMemCpyAligned_ARMV4(pDst, pSrc, nBytes)
    #define ALIGNED32_FASTMEMCLR(pDst, val, cBytes2Clear)   prvWMMemClrAligned_ARMV4(pDst, cBytes2Clear)
#else
    #define ALIGNED32_FASTMEMCPY(pDst,pSrc,nBytes)      memcpy(pDst, pSrc, nBytes)
    #define ALIGNED32_FASTMEMCLR(pDst, val, cBytes2Clear)   memset(pDst, 0, cBytes2Clear)
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

#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

#if defined(UNDER_CE) && !defined(DEBUG) && !defined(__arm)
#   define FORCEINLINE __forceinline
#else
#   define FORCEINLINE static __inline
#endif

#ifdef _IOS
#   define FORCEINLINE static __inline
#endif

#   define ARCH_V3 0

#   define PPC_TEST_SUPPORT_PROFILE(iMask)
#   define PPC_TEST_SUPPORT_PROFILE_LEVEL(pWMVDec,iW,iH,iMask)

#endif
