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

extern int dbg_currbits[0x10000];
extern int dbg_currbits_idx;

//Class istream;

// Hide Debugging/Analysis macros called HUFFMANGET_DBG... in another file
#include "strmdecdbg.h"



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
#else                                                       
//For others(WIN32,RVDS,etc)
#   define FORCEINLINE __inline
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

#ifdef __STREAMING_MODE_DECODE_
    Bool_WMV m_bNotEndOfFrame;
#endif /* _STREAMING_MODE_DECODE */

#ifdef _WMV9AP_
    //Bool_WMV bNotEndOfFrame;
    Bool_WMV m_bStartcodePrevention;
    Bool_WMV m_bPassingStartCode;

    I32_WMV m_iStartcodePreventionStatus;
    struct tagWMVDecInternalMember * m_pWMVDec;
#endif

    U32_WMV  m_uiUserData;
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
    //assert ((iNumBits - 1) <= 15);    // 0 < iNumBits <= 16
    
    // Return iNumBits most significant bits
    return (pThis->m_uBitMask >> (32 - iNumBits));
}

U32_WMV BS_flush16_2 (CInputBitStream_WMV * pThis);

Void_WMV BS_flush16 (CInputBitStream_WMV * pThis, U32_WMV iNumBits);

FORCEINLINE Void_WMV BS_flush16_online (CInputBitStream_WMV * pThis, U32_WMV iNumBits)
{
    //assert (iNumBits <= 16);  // 0 <= iNumBits <= 16
    
    // Remove used bits from bit mask filling free space with 0s
    pThis->m_uBitMask <<= iNumBits;
    
    // Adjust counter of available bits
    if ((pThis->m_iBitsLeft -= iNumBits) < 0)
    {
        //
        // Lookup mask has less than 16 bits left -- time to reload
        //
       
restart:
        if (pThis->m_pCurr < pThis->m_pLast)
        {
            //
            // we have 2 or more bytes available -- load 16 bits more;
            // since mask has (iBitsLeft + 16) bits, next 16 bits should be shifted left
            // by (32 - ((iBitsLeft+16) + 16)) = (-BitsLeft) bits
            //
            pThis->m_uBitMask += ((pThis->m_pCurr[0] << 8) + pThis->m_pCurr[1]) << (-pThis->m_iBitsLeft);
            pThis->m_pCurr += 2;
            pThis->m_iBitsLeft += 16;
        }
        else
        {
            if (BS_flush16_2(pThis) == 1) goto restart;
        }
    }
}

FORCEINLINE Void_WMV BS_flush16_peek_online (CInputBitStream_WMV * pThis, U32_WMV iNumBits, I32_WMV* signBit)
{
    // Remove used bits from bit mask filling free space with 0s
    pThis->m_uBitMask <<= iNumBits;

    *signBit = ((I32_WMV)(pThis->m_uBitMask) >> 31);

    pThis->m_uBitMask <<= 1;
    
    // Adjust counter of available bits
    if ((pThis->m_iBitsLeft -= (iNumBits+1)) < 0)
    {
        //
        // Lookup mask has less than 16 bits left -- time to reload
        //
       
restart:
        if (pThis->m_pCurr < pThis->m_pLast)
        {
            //
            // we have 2 or more bytes available -- load 16 bits more;
            // since mask has (iBitsLeft + 16) bits, next 16 bits should be shifted left
            // by (32 - ((iBitsLeft+16) + 16)) = (-BitsLeft) bits
            //
            pThis->m_uBitMask += ((pThis->m_pCurr[0] << 8) + pThis->m_pCurr[1]) << (-pThis->m_iBitsLeft);
            pThis->m_pCurr += 2;
            pThis->m_iBitsLeft += 16;
        }
        else
        {
            if (BS_flush16_2(pThis) == 1) goto restart;
        }
    }
}

#ifdef __arm
U32_WMV BS_getBits (CInputBitStream_WMV * pThis, register U32_WMV dwNumBits);
#endif

#ifndef __arm
FORCEINLINE U32_WMV BS_getBits (CInputBitStream_WMV * pThis, register U32_WMV dwNumBits)
{  
    U32_WMV uMask = 0;
    U32_WMV nBitsLeft = (U32_WMV)(pThis->m_iBitsLeft + 16);
    
    assert (dwNumBits <= 32);   // 0 <= iNumBits <= 32

    if (dwNumBits == 0)
    {
        //
        // special case -- cannot shift 32-bit value right by 32 bits
        // because result is hardware-dependent
        //
        return (0);
    }

getBits_restart:    
    if (dwNumBits > nBitsLeft && nBitsLeft > 0)
    {
        dwNumBits -= nBitsLeft;
        uMask += BS_peek16 (pThis, nBitsLeft) << dwNumBits;
        BS_flush16 (pThis, nBitsLeft);

        nBitsLeft = (U32_WMV)(pThis->m_iBitsLeft + 16);
        goto getBits_restart;
    }
    else
    {
        uMask += BS_peek16 (pThis, dwNumBits);
        BS_flush16 (pThis, dwNumBits);
    }

    return (uMask); 
}
#endif

FORCEINLINE U8_WMV BS_getMaxBits (CInputBitStream_WMV * pThis, register U32_WMV dwNumBits, U8_WMV* rgCodeSymSize)
{
    U32_WMV uBitPattern = BS_peek16(pThis, dwNumBits) << 1;
    BS_flush16 (pThis, rgCodeSymSize [uBitPattern]);
    uBitPattern = rgCodeSymSize[uBitPattern+1];
    if (uBitPattern == 255)
        pThis->m_iStatus = 3;
    return ((U8_WMV) uBitPattern);
}

// Get 16 or less bits from input stream
FORCEINLINE U32_WMV BS_get16 (CInputBitStream_WMV * pThis, I32_WMV iNumBits)
{
    U32_WMV uMask;
    assert ((U32_WMV) (iNumBits - 1) <= 15);    // 0 < iNumBits <= 16
    uMask = BS_peek16 (pThis, iNumBits);
    BS_flush16 (pThis, iNumBits);
    return (uMask);
}

FORCEINLINE Void_WMV BS_flush (CInputBitStream_WMV * pThis)
  {
    if (pThis->m_bStartcodePrevention) { 
        I32_WMV iBit = BS_getBits (pThis, 1); // Read the first flushing bit which is a 'one'
        if(iBit !=1)
            return;
    }
    BS_flush16 (pThis, pThis->m_iBitsLeft & 7);
  }
// Get 1 bit from input stream
FORCEINLINE U32_WMV BS_getBit (CInputBitStream_WMV * pThis)
{
    //return (BS_get16 (pThis, 1));
    U32_WMV uMask;
    uMask = pThis->m_uBitMask >> 31;
    BS_flush16 (pThis, 1);
    return (uMask);
}

FORCEINLINE Void_WMV BS_flushMPEG4 (CInputBitStream_WMV* pThis, I32_WMV nExtraBits)
{
    assert ((U32_WMV) nExtraBits <= 16);
    if ((pThis->m_iBitsLeft & 7) != 0)
        nExtraBits = pThis->m_iBitsLeft & 7;
    BS_flush16(pThis, nExtraBits);
}

// following a peekbits and numBits is guaranteed to be smaller than m_wBitsLeft
FORCEINLINE Void_WMV BS_adjustBits(CInputBitStream_WMV * pThis, const U32_WMV numBits)
{
    //pThis->m_iBitsLeft-=(U8_WMV)numBits;
    BS_flush16(pThis, numBits);
}

U32_WMV BS_peekBits (CInputBitStream_WMV * pThis, const U32_WMV numBits);

Void_WMV BS_GetMoreData(CInputBitStream_WMV * pThis);

U32_WMV BS_peekBitsTillByteAlign (CInputBitStream_WMV * pThis, I32_WMV* pnBitsToPeek);
U32_WMV BS_peekBitsFromByteAlign (CInputBitStream_WMV * pThis, I32_WMV numBits); // peek from byte bdry
#endif // __STRMDEC_H_
