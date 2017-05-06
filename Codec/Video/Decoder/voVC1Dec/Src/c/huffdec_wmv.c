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


VResultCode Huffman_WMV_init16 (I16_WMV *pDecodeTable, tPackHuffmanCode_WMV huffArray[], I32_WMV *pSymbolInfo, U32_WMV iRootBits)
{
    I32_WMV i, n, iMaxIndex;
    I32_WMV iAlphabetSize = huffArray[0].code;
    U32_WMV m, b;
    I32_WMV k;
    
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
//
VResultCode Huffman_WMV_init (tWMVDecInternalMember *pWMVDec, Huffman_WMV * pThis, tPackHuffmanCode_WMV huffArray[], U8_WMV tableSize)
{
	
    pThis->m_tableSize = tableSize;

   //zou 625
	pThis->m_hufDecTable = (I16_WMV*)(((U8_WMV*)pWMVDec->huffmanbuffer)+pWMVDec->huffmanused);
	pWMVDec->huffmanused += sizeof(I16_WMV) * ((1 << tableSize) + (huffArray[0].code << 1));
	pWMVDec->huffmanused = ((pWMVDec->huffmanused+7)&~7);
	if(pWMVDec->huffmanused>pWMVDec->huffmansize)
		return vrOutOfMemory;


    if (pThis->m_hufDecTable != NULL)
        Huffman_WMV_init16(pThis->m_hufDecTable, huffArray, 0, tableSize);
    else
        return vrOutOfMemory;
    
    return vrNoError;
}

FORCEINLINE Void_WMV BS_flush16_online (CInputBitStream_WMV * pThis, U32_WMV iNumBits)
{    
    // Remove used bits from bit mask filling free space with 0s
    pThis->m_uBitMask <<= iNumBits;
    
    // Adjust counter of available bits
    if ((pThis->m_iBitsLeft -= iNumBits) < 0)
    {
        // Lookup mask has less than 16 bits left -- time to reload       
restart:
        if (pThis->m_pCurr < pThis->m_pLast)
        {
            // we have 2 or more bytes available -- load 16 bits more;
            // since mask has (iBitsLeft + 16) bits, next 16 bits should be shifted left
            // by (32 - ((iBitsLeft+16) + 16)) = (-BitsLeft) bits
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

U32_WMV getHuffman_C(CInputBitStream_WMV *bs, I16_WMV* pDecodeTable, U32_WMV iRootBits)
{
	I32_WMV iSymbol;
    // BS_peek16(bs, iRootBits)
	iSymbol = pDecodeTable[BS_peek16(bs, iRootBits)];

	if (iSymbol >= 0)
	{
		//BS_flush16_online 
		BS_flush16_online (bs, (iSymbol & ((1 << HUFFMAN_DECODE_ROOT_BITS_LOG) - 1)));
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
    }
    
    return (U32_WMV)(iSymbol);
}
