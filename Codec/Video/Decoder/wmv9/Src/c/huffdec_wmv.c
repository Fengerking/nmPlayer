//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//+-------------------------------------------------------------------------
//
//  Microsoft Windows Media
//
//  Copyright (C) Microsoft Corporation, 1998 - 1999
//
//  File:       huffman.cpp
//
//--------------------------------------------------------------------------
#include "xplatform_wmv.h"
#include "wmvdec_api.h"
#include "wmvdec_member.h"
#include "wmvdec_function.h"
#include "voWmvPort.h"
#include "huffman_wmv.h"
#include "strmdec_wmv.h"


#define SIGN_BIT(TypeOrValue) (((U32_WMV) 1) << (8 * sizeof (TypeOrValue) - 1))


Void_WMV Huffman_WMV_construct(Huffman_WMV * pThis)
{
    pThis->m_hufDecTable = NULL_WMV;
}


Void_WMV Huffman_WMV_destruct(tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis)
{
    if (pThis->m_hufDecTable != NULL_WMV)
		if (!pWMVDec->m_bPHYSMEM){
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO Huffman_WMV_destruct, m_hufDecTable:%x",(U32_WMV)pThis->m_hufDecTable);
#endif
           FREE_PTR (pWMVDec, pThis->m_hufDecTable);
		}
}

Void_WMV Huffman_WMV8_destruct(tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis)
{
	if (pThis->m_hufDecTable != NULL_WMV){
#ifdef VO_LOG_INFO_MemoryFree
	VOLOGI("\n VO_LOG_INFO Huffman_WMV8_destruct, m_hufDecTable:%x",(U32_WMV)pThis->m_hufDecTable);
#endif
        FREE_PTR (pWMVDec, pThis->m_hufDecTable);
	}
}

VResultCode Huffman_WMV_init16 (I16_WMV *pDecodeTable, tPackHuffmanCode_WMV huffArray[], I32_WMV *pSymbolInfo, U32_WMV iRootBits)
{
    I32_WMV i, n, iMaxIndex;
    I32_WMV iAlphabetSize = huffArray[0].code;
    U32_WMV m, b;
    I32_WMV k;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Huffman_WMV_init16);
    
    iMaxIndex = HUFFMAN_DECODE_TABLE_SIZE (iRootBits, iAlphabetSize);
    memset (pDecodeTable, 0, iMaxIndex * sizeof (I16_WMV));
    
    if (iRootBits >= (1 << HUFFMAN_DECODE_ROOT_BITS_LOG))
        return (vrFail);
    
    // first available intermediate node index
    n = (1 << iRootBits) - SIGN_BIT (pDecodeTable[0]);
    iMaxIndex -= SIGN_BIT (pDecodeTable[0]);
    
    for (i = 0; i < iAlphabetSize; ++i)
    {
        m = huffArray[i+1].code;
        b = huffArray[i+1].length;
        
        if (b > HUFFMAN_MAX_CODEWORD_LENGTH || (m >> b) != 0)
        {
            // oops, stated length is smaller than actual codeword length
            //return (__LINE__);
            return (vrFail);
        }
        
        if (b <= iRootBits)
        {
            // short codeword goes right into root table
            int k = m << (iRootBits - b);
            int kLast = (m + 1) << (iRootBits - b);
            
            if ((U32_WMV) (pSymbolInfo == 0 ? i : pSymbolInfo[i]) >= (SIGN_BIT (pDecodeTable[0]) >> HUFFMAN_DECODE_ROOT_BITS_LOG))
            {
                // oops, symbol is out of range of decoding table
                //return (__LINE__);
                return (vrFail);
            }
            b += (pSymbolInfo == 0 ? i : pSymbolInfo[i]) << HUFFMAN_DECODE_ROOT_BITS_LOG;
            
            do
            {
                if (pDecodeTable[k] != 0)
                {
                    // oops, there is already path to this location -- not a proper prefix code
                    //return (__LINE__);
                    return (vrFail);
                }
                pDecodeTable[k] = (I16_WMV) b;
            }
            while (++k != kLast);
        }
        else
        {
            // long codeword -- generate bit-by-bit decoding path
            b -= iRootBits;
            
#if 0   /* now we can decode tails longer than 16 bits */
            if (b > 16)
            {
                // cannot decode without flushing (see HuffmanDecodeShort () for more details)
                //return (__LINE__);
                return (vrFail);
            }
#endif
            
            k = m >> b;   // these bit will be decoded by root table
            do
            {
                if (pDecodeTable[k] > 0)
                {
                    // oops, it's not an internal node as it should be
                    //return (__LINE__);
                    return (vrFail);
                }
                
                if (pDecodeTable[k] == 0)
                {
                    // slot is empty -- create new internal node
                    pDecodeTable[k] = (I16_WMV) n;
                    n += 2;
                    if (n > iMaxIndex)
                    {
                        // oops, too many holes in the code; [almost] full prefix code is needed
                        //return (__LINE__);
                        return (vrFail);
                    }
                }
                k = pDecodeTable[k] + SIGN_BIT (pDecodeTable[0]);
                
                // find location in this internal node (will we go left or right)
                --b;
                if ((m >> b) & 1)
                    ++k;
            }
            while (b != 0);
            
            // reached the leaf
            if (pDecodeTable[k] != 0)
            {
                // oops, the slot should be reserved for current leaf
                //return (__LINE__);
                return (vrFail);
            }
            
            if ((U32_WMV) (pSymbolInfo == 0 ? i : pSymbolInfo[i]) >= SIGN_BIT (pDecodeTable[0]))
            {
                // oops, symbol is out of range of short table
                //return (__LINE__);
                return (vrFail);
            }
            pDecodeTable[k] = (I16_WMV) (pSymbolInfo == 0 ? i : pSymbolInfo[i]);
        }
    }
    
    return (vrNoError);
}

VResultCode Huffman_WMV_init (tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis, U32_WMV uiUserData, tPackHuffmanCode_WMV huffArray[], U8_WMV tableSize)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Huffman_WMV_init);
    pThis->m_uiUserData = uiUserData;
    pThis->m_tableSize = tableSize;
    
	if (pThis->m_hufDecTable != NULL_WMV) {// support re-initialization
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO m_hufDecTable :%x",(U32_WMV)pThis->m_hufDecTable );
#endif
        FREE_PTR (pWMVDec, pThis->m_hufDecTable);
	}

    if (pWMVDec->m_bPHYSMEM) {
        pThis->m_hufDecTable = (I16_WMV *)(pWMVDec->m_ppxlCurrPHYSADDR);
        pWMVDec->m_ppxlCurrPHYSADDR += sizeof(I16_WMV) * ((1 << tableSize) + (huffArray[0].code << 1));
    } else {
#ifdef XDM
		pThis->m_hufDecTable    = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
		pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV) * ((1 << tableSize) + (huffArray[0].code << 1));
#else
        pThis->m_hufDecTable = (I16_WMV *) wmvMalloc (pWMVDec, sizeof(I16_WMV) * ((1 << tableSize) + (huffArray[0].code << 1)), DHEAP_HUFFMAN);
#endif
	}
    if (pThis->m_hufDecTable != NULL)
        Huffman_WMV_init16(pThis->m_hufDecTable, huffArray, 0, tableSize);
    else
        return vrOutOfMemory;

    COVERAGE_CLEAR_HUFFMAN(pThis);
    
    return vrNoError;
}

VResultCode Huffman_WMV8_init (tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis, U32_WMV uiUserData, tPackHuffmanCode_WMV huffArray[], U8_WMV tableSize)
{
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(Huffman_WMV_init);
    pThis->m_uiUserData = uiUserData;
    pThis->m_tableSize = tableSize;
    
	if (pThis->m_hufDecTable != NULL_WMV) {// support re-initialization
#ifdef VO_LOG_INFO_MemoryFree
		VOLOGI("\n VO_LOG_INFO Huffman_WMV8_init m_hufDecTable :%x",(U32_WMV)pThis->m_hufDecTable );
#endif
        FREE_PTR (pWMVDec, pThis->m_hufDecTable);
	}
    
#ifdef XDM
	pThis->m_hufDecTable    = (I16_WMV *)pWMVDec->m_MemXDM.pPrivateBuff;
	pWMVDec->m_MemXDM.pPrivateBuff += sizeof(I16_WMV) * ((1 << tableSize) + (huffArray[0].code << 1));
#else
    pThis->m_hufDecTable = (I16_WMV *) wmvMalloc (pWMVDec, sizeof(I16_WMV) * ((1 << tableSize) + (huffArray[0].code << 1)), DHEAP_HUFFMAN);
#endif
    if (pThis->m_hufDecTable != NULL)
        Huffman_WMV_init16(pThis->m_hufDecTable, huffArray, 0, tableSize);
    else
        return vrOutOfMemory;

    COVERAGE_CLEAR_HUFFMAN(pThis);
    
    return vrNoError;
}


#if !defined(WMV_OPT_HUFFMAN_GET_ARM)&&!defined(_EMB_ASM_MIPS_HUFFDEC_)&&!defined(WMV_OPT_HUFFMAN_GET_SHX)
U32_WMV getHuffman(CInputBitStream_WMV *bs, I16_WMV* pDecodeTable, U32_WMV iRootBits)
{
	I32_WMV iSymbol;
	DEBUG_PROFILE_FRAME_FUNCTION_COUNT(getHuffman);
    
	iSymbol = pDecodeTable[BS_peek16(bs, iRootBits)];

	if (iSymbol >= 0)
	{
		BS_flush16_online (bs, (iSymbol & ((1 << HUFFMAN_DECODE_ROOT_BITS_LOG) - 1)));
		iSymbol >>= HUFFMAN_DECODE_ROOT_BITS_LOG;
	}
	else
	{
		BS_flush16(bs, iRootBits);
        
#   if 1 // (iRootBits < HUFFMAN_MAX_CODEWORD_LENGTH - 16)
		do
		{
			iSymbol += BS_peekBit(bs);

			BS_flush16(bs, 1);
			iSymbol = pDecodeTable[iSymbol + 0x8000];

		}
		while (iSymbol < 0);
#   else // >= 10 case
		while ((iSymbol = pDecodeTable[iSymbol + 0x8000 + BS_peekBit(bs)]) < 0) {
			BS_skipBit(bs);
			BS_flush16(bs, 1);
		}
#   endif
    }
    
    return (U32_WMV)(iSymbol);
}
#endif

U32_WMV getHuffmanAndPeekbit(CInputBitStream_WMV *bs, I16_WMV* pDecodeTable, U32_WMV iRootBits, I32_WMV* signBit)
{
    I32_WMV iSymbol;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(getHuffmanAndPeekbit);
    
    iSymbol = pDecodeTable[BS_peek16(bs, iRootBits)];
    if (iSymbol >= 0)
    {
        BS_flush16_peek_online (bs, (iSymbol & ((1 << HUFFMAN_DECODE_ROOT_BITS_LOG) - 1)), signBit);
        iSymbol >>= HUFFMAN_DECODE_ROOT_BITS_LOG;
    }
    else
    {
        BS_flush16(bs, iRootBits);
        
        do
        {
            iSymbol += BS_peekBit(bs);
            BS_flush16(bs, 1);
            iSymbol = pDecodeTable[iSymbol + 0x8000];
        }
        while (iSymbol < 0);
        
        *signBit = ((I32_WMV)(bs->m_uBitMask) >> 31);
        BS_flush16(bs, 1);
    }
    
    return (U32_WMV)(iSymbol);
}
