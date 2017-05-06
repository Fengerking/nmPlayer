//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) 1996, 1997  Microsoft Corporation

Module Name:

    strmdec.cpp

Abstract:

    Decoder BitStream

Author:

    Craig Dowell (craigdo@microsoft.com) 10-December-1996
    Ming-Chieh Lee (mingcl@microsoft.com) 10-December-1996
    Bruce Lin (blin@microsoft.com) 10-December-1996

Revision History:

*************************************************************************/
#include "voWmvPort.h"
#include "xplatform_wmv.h"
#include "typedef.h"
#include "strmdec_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"

#ifdef OPT_HUFFMAN_DBGDMP
U32_WMV opt_huffman_dbgdmp[0x20000];
int opt_huffman_dbgdmp_idx;
#endif

//#define _BITSTRM_DUMP_

#ifdef _BITSTRM_DUMP_
static FILE *g_pInputStrmDumpFile = NULL;
#endif

Void_WMV BS_StartCodeProcess(CInputBitStream_WMV * pThis, U8_WMV * pBuffer, U32_WMV * pdwBufferLen, Bool_WMV bNotEndOfFrame);

Void_WMV BS_construct (CInputBitStream_WMV * pThis, U32_WMV uiUserData)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BS_construct);

    memset (pThis, 0, sizeof (*pThis));
    pThis->m_uiUserData = uiUserData;
}

Void_WMV BS_destruct (CInputBitStream_WMV * pThis)
{
}

Void_WMV BS_reset(
                  CInputBitStream_WMV * pThis,
                  U8_WMV          *pBuffer,
                  U32_WMV         dwBuflen,
                  Bool_WMV        bNotEndOfFrame
#ifdef _WMV9AP_
                  ,Bool_WMV       bStartcodePrevention
#endif
                  )
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BS_reset);

    pThis->m_uBitMask = 0;
    pThis->m_iBitsLeft = -16;
    pThis->m_iStatus = 0;
    pThis->m_pLast = pBuffer + dwBuflen - 1;
#ifdef _WMV9AP_
    pThis->m_bStartcodePrevention = bStartcodePrevention;
    if (pThis->m_bStartcodePrevention) {
        pThis->m_iStartcodePreventionStatus = 0;
        
        BS_StartCodeProcess(pThis, pBuffer, &dwBuflen, bNotEndOfFrame);
        pBuffer = pThis->m_pCurr;
    }
#endif
    
    // load up to 32 bits
    while (pBuffer <= pThis->m_pLast && pThis->m_iBitsLeft <= 8)
    {
        pThis->m_uBitMask += *pBuffer++ << (8 - pThis->m_iBitsLeft);
        pThis->m_iBitsLeft += 8;
    }
    
    pThis->m_pCurr = pBuffer;
    
#ifdef __STREAMING_MODE_DECODE_
    pThis->m_bNotEndOfFrame = bNotEndOfFrame;
#endif
}

U32_WMV BS_peekBitsTillByteAlign (CInputBitStream_WMV * pThis, I32_WMV* pnBitsToPeek)
{
    I32_WMV iBitsLeftMod8 = pThis->m_iBitsLeft & 7;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BS_peekBitsTillByteAlign);

    if (iBitsLeftMod8 == 0)
        iBitsLeftMod8 = 8;
    *pnBitsToPeek = iBitsLeftMod8;
    return BS_peek16 (pThis, iBitsLeftMod8);
    
    // New >>
    //return BS_peekBits (pThis, iBitsLeftMod8);
}

U32_WMV  BS_peekBitsFromByteAlign(CInputBitStream_WMV * pThis, I32_WMV nBitsToPeek)
{

    U32_WMV nBitsRet;
    I32_WMV iBitsToPeek = nBitsToPeek;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BS_peekBitsFromByteAlign);

    //BS_flushMPEG4 (pThis, 8);
    if ((pThis->m_iBitsLeft & 7) == 0) {
        iBitsToPeek += 8;
    }
    else {
        iBitsToPeek += (pThis->m_iBitsLeft & 7); 
    }   
    //if (nBitsToPeek > pThis->m_iBitsLeft)
    //    nBitsToPeek -= pThis->m_iBitsLeft;

    nBitsRet = BS_peekBits (pThis, iBitsToPeek);


    return (nBitsRet & (0xffffffff>>nBitsToPeek));
    //return BS_peekBits (pThis, nBitsToPeek);
}

#ifdef __arm
U32_WMV BS_getBits (CInputBitStream_WMV * pThis, register U32_WMV dwNumBits)
{  
    U32_WMV uMask = 0;
    U32_WMV nBitsLeft = (U32_WMV)(pThis->m_iBitsLeft + 16);

    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BS_getBits);
    
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

#if 1 //wshao 2011.3.30. Fix bug #4087
    if (dwNumBits > nBitsLeft && nBitsLeft > 0)
#else
    if (dwNumBits > nBitsLeft)		
#endif
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
#endif //__arm

#if !defined(WMV_OPT_HUFFMAN_GET_ARM)&&!defined(_EMB_ASM_MIPS_HUFFDEC_) &&!defined(WMV_OPT_HUFFMAN_GET_SHX)
U32_WMV BS_flush16_2 (CInputBitStream_WMV * pThis)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BS_flush16_2);

    if (pThis->m_pCurr == pThis->m_pLast)
    {
        // only one byte left -- load 8 bits more
        pThis->m_uBitMask += pThis->m_pCurr[0] << (8 - pThis->m_iBitsLeft);
        pThis->m_pCurr += 1;
        pThis->m_iBitsLeft += 8;
        return 0;
    }

#ifdef __STREAMING_MODE_DECODE_
    if(pThis->m_bNotEndOfFrame == TRUE_WMV)
    {
        BS_GetMoreData(pThis);
        return 1;
    }
#endif  /* _STREAMING_MODE_DECODE_ */
    

    if (pThis->m_iBitsLeft < -16)
    {
        // oops -- tried to read behind the buffer; broken bit stream
        if (pThis->m_iStatus == 0)
            pThis->m_iStatus = 2;
        pThis->m_iBitsLeft = 127;   // supply of garbage is really infinite
    }

    return 0;
}


Void_WMV BS_flush16 (CInputBitStream_WMV * pThis, U32_WMV iNumBits)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(BS_flush16);
    //assert (iNumBits <= 16);  // 0 <= iNumBits <= 16
    
    // Remove used bits from bit mask filling free space with 0s
    pThis->m_uBitMask <<= iNumBits;
    
    // Adjust counter of available bits
    if ((pThis->m_iBitsLeft -= iNumBits) < 0)
    {
        //
        // Lookup mask has less than 16 bits left -- time to reload
        //
        U8_WMV *p = pThis->m_pCurr;
        
restart:
        if (p < pThis->m_pLast)
        {
            //
            // we have 2 or more bytes available -- load 16 bits more;
            // since mask has (iBitsLeft + 16) bits, next 16 bits should be shifted left
            // by (32 - ((iBitsLeft+16) + 16)) = (-BitsLeft) bits
            //
            pThis->m_uBitMask += ((p[0] << 8) + p[1]) << (-pThis->m_iBitsLeft);
            p += 2;
            pThis->m_iBitsLeft += 16;
        }
        else
        {
#ifdef __STREAMING_MODE_DECODE_

            if (p == pThis->m_pLast)
            {
                // only one byte left -- load 8 bits more
                pThis->m_uBitMask += p[0] << (8 - pThis->m_iBitsLeft);
                p += 1;
                pThis->m_iBitsLeft += 8;
                pThis->m_pCurr = p;
                return;
            }

            if (pThis->m_bNotEndOfFrame == TRUE_WMV)
            {
                BS_GetMoreData(pThis);
                p = pThis->m_pCurr;
                goto restart;
            }
#endif  /* _STREAMING_MODE_DECODE_ */
            

            if (pThis->m_iBitsLeft < -16)
            {
                // oops -- tried to read behind the buffer; broken bit stream
                if (pThis->m_iStatus == 0)
                    pThis->m_iStatus = 2;
                pThis->m_iBitsLeft = 127;   // supply of garbage is really infinite
            }
            
        }
        pThis->m_pCurr = p;
    }
}
#endif

#ifdef _WMV9AP_
Void_WMV BS_StartCodeProcess(CInputBitStream_WMV * pThis, U8_WMV * pBuffer, U32_WMV * pdwBufferLen, Bool_WMV bNotEndOfFrame)
{
    U32_WMV i;
    U32_WMV dwBufferLen = * pdwBufferLen;
    I32_WMV status = pThis->m_iStartcodePreventionStatus;
    U8_WMV *pTmp = pThis->m_pWMVDec->m_pParseStartCodeBitstream;
    U8_WMV *pp = pBuffer;
    
    for (i=0;i<dwBufferLen;i++) {
        switch(status) {
        case 0:
            if (pp[i] == 0)
                status = 1;
            break;
        case 1:
            if (pp[i] == 0)
                status = 2;
            else 
                status = 0;
            break;
        case 2:
            status = 0;
            if (pp[i] == 3) {
                i++;
                if (i < dwBufferLen) {
                    if (pp[i] == 0) { 
                        status = 1;
                    }
                }
            }
            break;
        }
        if (i < dwBufferLen)
            *pTmp++ = pp[i];
    }
    
    dwBufferLen = ((U32_WMV)pTmp - (U32_WMV)pThis->m_pWMVDec->m_pParseStartCodeBitstream);
    pThis->m_iStartcodePreventionStatus = status ;
    
    pThis->m_pCurr = pThis->m_pWMVDec->m_pParseStartCodeBitstream;
    pThis->m_pLast = pThis->m_pCurr + dwBufferLen - 1;
    
    * pdwBufferLen = dwBufferLen;
}

#endif

U32_WMV BS_peekBits (CInputBitStream_WMV * pThis, const U32_WMV numBits)
{
    // Return iNumBits most significant bits
    //return (pThis->m_uBitMask >> (32 - numBits));

    U8_WMV *p = pThis->m_pCurr;
    
    assert ((numBits - 1) <= 31);       // 0 < numBits <= 32

restart_peekBits:        
    if (numBits > (U32_WMV)(pThis->m_iBitsLeft + 16))
    {
        if (p <= pThis->m_pLast)
        {
            // we have at least 1 byte available -- load [temporarily] 8 bits more
            if (pThis->m_iBitsLeft <= 8)
            {
                // we have space for 8 more bits -- load them right into the buffer
                pThis->m_uBitMask += p[0] << (8 - pThis->m_iBitsLeft);
                p += 1;
                pThis->m_pCurr = p;
                pThis->m_iBitsLeft += 8;
                goto restart_peekBits;
            }

            if (numBits > (U32_WMV)16 + pThis->m_iBitsLeft && p <= pThis->m_pLast)
            {
                //
                // we have at least 1 more byte available -- load [temporarily] 8 bits more;
                // temporarily because (m_iBitsLeft >= 8) now, and we cannot permanently load
                // partial byte (in case m_iBitsLeft > 8).
                //
                return ((pThis->m_uBitMask + (p[0] >> (pThis->m_iBitsLeft - 8))) >> (32 - numBits));
            }
        }
#ifdef __STREAMING_MODE_DECODE_
        else
        {
            if (pThis->m_bNotEndOfFrame == TRUE_WMV)
            {
                BS_GetMoreData(pThis);
                p = pThis->m_pCurr;
                goto restart_peekBits;
            }
        }
#endif /* __STREAMING_MODE_DECODE */
        
    }
    
    return (pThis->m_uBitMask >> (32 - numBits));
}


Void_WMV BS_GetMoreData(CInputBitStream_WMV * pThis)
{
    U8_WMV  *pTemp;
    U32_WMV uSize;
    
    WMVDecCBGetData (pThis->m_uiUserData, 0, &pTemp, 4, &uSize, &pThis->m_bNotEndOfFrame);
    if (pThis->m_pWMVDec->m_cvCodecVersion == WMVA && pThis->m_bPassingStartCode) {
        U8_WMV * p = pTemp;
        //uSize -= pThis->m_pWMVDec->m_iParseStartCodeRemLen;
        ParseStartCodeChunk(pThis->m_pWMVDec, p, uSize, &p, &uSize, pThis->m_bNotEndOfFrame);
        //assert(p == pTemp);
    
        if(pThis->m_bStartcodePrevention)
            BS_StartCodeProcess(pThis, p, &uSize, pThis->m_bNotEndOfFrame);
    }
    else
    {
        pThis->m_pCurr = pTemp;
        pThis->m_pLast = pTemp + uSize - 1;
    }
}
