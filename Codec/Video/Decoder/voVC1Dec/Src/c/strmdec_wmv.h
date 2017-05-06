//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) 1996, 1997  Microsoft Corporation

Module Name:

    strmdec.h

Abstract:

    Decoder BitStream

Author:

    Craig Dowell (craigdo@microsoft.com) 10-December-1996
    Ming-Chieh Lee (mingcl@microsoft.com) 10-December-1996
    Bruce Lin (blin@microsoft.com) 10-December-1996
    Chuang Gu (chuanggu@microsoft.com) 2-Feb-2000

Revision History:
    Bruce Lin (blin@microsoft.com) 02-May-1997
    Add getMaxBits()

*************************************************************************/

#ifndef __STRMDEC_H_
#define __STRMDEC_H_

//Class istream;

// Hide Debugging/Analysis macros called HUFFMANGET_DBG... in another file
//#include "wmvdec_function.h"
#include "strmdecdbg.h"

#include "voVC1DID.h"

#if STRMPARSER_NUMBITS != 64
#define STRMPARSER_NUMBITS 32
#define STRMPARSER_MASK ((U32_WMV)0xffffffff)
typedef U32_WMV STRMPARSER_BUFFER;
#else
#define STRMPARSER_MASK ((U64_WMV)0xffffffffffffffffL)
typedef U64_WMV STRMPARSER_BUFFER;
#endif

#define VALID               0
#define END_OF_FILE         1
#define ILLEGAL_PATTERN     3
//#define EOF                 (-1)

#ifdef FORCEINLINE
#undef FORCEINLINE
#endif

#if defined(UNDER_CE) && !defined(DEBUG) && !defined(__arm)	
//For WINCE
#   define FORCEINLINE __forceinline
#elif defined(LINUX)&& !defined(RVDS)                       
//For Android,iOS
#   define FORCEINLINE static __inline__
#elif defined (_IOS)
#   define FORCEINLINE static __inline__
#else                                                       
//For others(WIN32,RVDS,etc)
#   define FORCEINLINE __inline
#endif


#ifdef _ARM_
#define BS_flush16_2     BS_flush16_2_ARMV4
#define BS_flush16  BS_flush16_ARMV4
#else
#define BS_flush16        BS_flush16_C
#define BS_flush16_2     BS_flush16_2_C
#endif

typedef struct tagCInputBitStream_WMV {
    U32_WMV m_uBitMask;     // bit mask contains at least 16 bits all the time (until we have less than 16 bits left)
    I32_WMV m_iBitsLeft;    // number of bits available minus 16
    U8_WMV *m_pCurr;        // current data pointer
    U8_WMV *m_pLast;        // address of last available byte
    I32_WMV m_iStatus;      // status of the stream:
                            // 0: valid stream
                            // 1: user-invalidated stream
                            // 2: attempt to read behind the end of buffer
                            // 3: bad value detected by getMaxBits()

    Bool_WMV m_bNotEndOfFrame;


    //Bool_WMV bNotEndOfFrame;
    Bool_WMV m_bStartcodePrevention;
    Bool_WMV m_bPassingStartCode;

    I32_WMV m_iStartcodePreventionStatus;
    struct tagWMVDecInternalMember * m_pWMVDec;

    //U32_WMV  m_uiUserData;
}CInputBitStream_WMV;

FORCEINLINE Bool_WMV BS_invalid (CInputBitStream_WMV * pThis)  { return pThis->m_iStatus != 0; /*return pThis->BNMS.m_BMS8.m_fStreamStaus;*/ }
//FORCEINLINE  Void_WMV BS_setInvalid(CInputBitStream_WMV * pThis) { pThis->BNMS.m_BMS8.m_fStreamStaus = ILLEGAL_PATTERN; }

 FORCEINLINE  U32_WMV BS_BitsLeft (CInputBitStream_WMV * pThis)
{ 
    if (pThis->m_iStatus != 0)
    {
        // broken stream is empty
        return (0);
    }
    //
    // modulo 8 because we don't want anyone know exact size of lookup buffer;
    // pretend it's one byte only
    //
    return (pThis->m_iBitsLeft & 7);
}

// Lookup 1 bit
FORCEINLINE U32_WMV BS_peekBit (CInputBitStream_WMV * pThis)
{
    return (((I32_WMV) pThis->m_uBitMask) < 0 ? 1 : 0);
}

// Skip 1 bit
FORCEINLINE Void_WMV BS_skipBit (CInputBitStream_WMV * pThis)
{
    pThis->m_uBitMask <<= 1;
    pThis->m_iBitsLeft -= 1;
    assert (pThis->m_iBitsLeft >= -16);
}

 FORCEINLINE U32_WMV BS_peek16 (CInputBitStream_WMV * pThis, U32_WMV iNumBits)
{    
    // Return iNumBits most significant bits
    return (pThis->m_uBitMask >> (32 - iNumBits));
}

U32_WMV BS_flush16_2_C (CInputBitStream_WMV * pThis);
Void_WMV BS_flush16_C (CInputBitStream_WMV * pThis, U32_WMV iNumBits);
Void_WMV BS_flush16_ARMV4 (CInputBitStream_WMV * pThis, U32_WMV iNumBits);
U32_WMV BS_flush16_2_ARMV4 (CInputBitStream_WMV * pThis);

U32_WMV BS_getBits (CInputBitStream_WMV * pThis, register U32_WMV dwNumBits);

// Get 16 or less bits from input stream
FORCEINLINE U32_WMV BS_get16 (CInputBitStream_WMV * pThis, I32_WMV iNumBits)
{
    U32_WMV uMask;
    assert ((U32_WMV) (iNumBits - 1) <= 15);    // 0 < iNumBits <= 16
    uMask = BS_peek16 (pThis, iNumBits);
    BS_flush16 (pThis, iNumBits);
    return (uMask);
}

 FORCEINLINE I32_WMV BS_flush (CInputBitStream_WMV * pThis)
  {
    if (pThis->m_bStartcodePrevention) { 
        I32_WMV iBit = BS_getBits (pThis, 1); // Read the first flushing bit which is a 'one'
        if(iBit != 1)
            return -1;
    }
    BS_flush16 (pThis, pThis->m_iBitsLeft & 7);

    return 0;
  }
// Get 1 bit from input stream
 FORCEINLINE U32_WMV BS_getBit (CInputBitStream_WMV * pThis)
{
    //return (BS_get16 (pThis, 1));
    U32_WMV uMask = 0;
    uMask = pThis->m_uBitMask >> 31;
    BS_flush16 (pThis, 1);
    return (uMask);
}

U32_WMV BS_peekBits (CInputBitStream_WMV * pThis, const U32_WMV numBits);

Void_WMV BS_GetMoreData(CInputBitStream_WMV * pThis);
#if 0
U32_WMV BS_peekBitsTillByteAlign (CInputBitStream_WMV * pThis, I32_WMV* pnBitsToPeek);
U32_WMV BS_peekBitsFromByteAlign (CInputBitStream_WMV * pThis, I32_WMV numBits); // peek from byte bdry
#endif
#endif // __STRMDEC_H_
