//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    HuffDec.cpp

Abstract:

    Huffman decoder. Simplified from Sanjeevm's huffman.cpp

Author:

    Wei-ge Chen (wchen) 19-July-1999

Revision History:

	Peter X. Zuo (peterzuo) 9-June-2003: C Opt: Unroll loop

*************************************************************************/

#include <stdio.h>
//#include <stdlib.h>
#include "macros.h"
#include "huffdec.h"
#include "msaudiodec.h"

#ifdef WMAPROFILE
#include "autoprofile.h"
#endif


#if 0
// Old reference code.
WMARESULT huffDecGet(const U16 *pDecodeTable, CWMAInputBitStream *bitstream,
                     U32* puBitCnt, U32 *puResult, U32* puSign)
{
     const int FIRST_LOAD = 10;
     const int SECOND_LOAD = 12;

     unsigned int ret_value;
     const unsigned short* node_base = pDecodeTable;

     U32 uBits;
     U32 codeword;
     int i;

     WMARESULT  wmaResult;
#ifdef WMAPROFILE
	 //profiling a function that gets called this often has too much overhead.
     //FunctionProfile fp;
     //FunctionProfileStart(&fp,HUFF_DEC_GET_PROFILE);
#endif

     TRACEWMA_EXIT(wmaResult, ibstrmPeekBits(bitstream, FIRST_LOAD + SECOND_LOAD + 1, &uBits));
     codeword = uBits;

     // Do first five 2-bit tables
     i = 5;
     do
     {
         node_base += ((codeword & 0xC0000000) >> 30);  // Use top 2 bits as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 2;            // Advance to next 2 bits
         i -= 1;
     } while (i > 0);

     // Do remaining three 2-bit tables
     i = 3;
     do
     {
         node_base += ((codeword & 0xC0000000) >> 30);  // Use top 2 bits as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 2;            // Advance to next 2 bits
         i -= 1;
     } while (i > 0);

     // Do six 1-bit tables
     i = 6;
     do
     {
         node_base += ((codeword & 0x80000000) >> 31);  // Use top bit as offset
         ret_value = *node_base;
         if (ret_value & 0x8000)
             goto decode_complete;  // Found a leaf node!

         node_base += ret_value;    // No leaf node, follow the offset
         codeword <<= 1;            // Advance to next bit
         i -= 1;
     } while (i > 0);


decode_complete:
     assert(ret_value & 0x8000);
     *puBitCnt = ((ret_value >> 10) & (0x0000001F));
     *puResult = ret_value & 0x000003FF;
     if (*puResult >= 0x03FC)
         *puResult = *(node_base + (*puResult & 0x0003) + 1);
     
     if (puSign != NULL)
        *puSign = uBits << *puBitCnt;

exit:
#ifdef WMAPROFILE
    //FunctionProfileStop(&fp);
#endif
     return wmaResult;
}

#else
// Removed redundant & operation, unrolled for speed.
// Currenty used code.
#if (!WMA_OPT_HUFFDEC_ARM && !defined(WMA_OPT_HUFFDEC_XENON) && !(defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_HUFFDEC)))
WMARESULT huffDecGet(const U16 *pDecodeTable, CWMAInputBitStream *bitstream,
                     U32* puBitCnt, U32 *puResult, U32* puSign)
{
     const int FIRST_LOAD = 10;
     const int SECOND_LOAD = 12;

     register unsigned int ret_value;
     register const unsigned short* node_base = pDecodeTable;

     U32 uBits;
     register U32 codeword;

     WMARESULT  wmaResult;
#ifdef WMAPROFILE
	 //profiling a function that gets called this often has too much overhead.
     //FunctionProfile fp;
     //FunctionProfileStart(&fp,HUFF_DEC_GET_PROFILE);
#endif

     TRACEWMA_EXIT(wmaResult, ibstrmPeekBits(bitstream, FIRST_LOAD + SECOND_LOAD + 1, &uBits));
     codeword = uBits;

	 // Do first five 2-bit tables

	 //unroll, 1
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x8000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 2
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x8000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 4
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 5
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

     // Do remaining three 2-bit tables
	//unroll 1
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 2
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits


     // Do six 1-bit tables
	//unroll 1
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

	//unroll 2
	node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

	//unroll 3
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

	//unroll 4
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

	//unroll 5
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

	//unroll 6
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

decode_complete:
     assert(ret_value & 0x8000);
     *puBitCnt = ((ret_value >> 10) & (0x0000001F));
     *puResult = ret_value & 0x000003FF;
     if (*puResult >= 0x03FC)
         *puResult = *(node_base + (*puResult & 0x0003) + 1);
     
     if (puSign != NULL)
        *puSign = uBits << *puBitCnt;
exit:
#ifdef WMAPROFILE
    //FunctionProfileStop(&fp);
#endif
     return wmaResult;
}

#define LOAD_BITS_FROM_DotT \
if (bs->m_cBitDotT > 0) \
{ \
    I32 cBitMoved = min (32 - bs->m_dwBitsLeft, bs->m_cBitDotT); \
    bs->m_dwDot <<= cBitMoved; \
    bs->m_cBitDotT -= cBitMoved; \
    bs->m_dwDot |= (bs->m_dwDotT >> bs->m_cBitDotT); \
    bs->m_dwDotT &= ((I32) (1 << bs->m_cBitDotT)) - 1; \
    bs->m_dwBitsLeft += cBitMoved; \
}

#define LOAD_BITS_FROM_STREAM \
while (bs->m_dwBitsLeft <= 24 && bs->m_cbBuflen > 0) { \
    bs->m_dwDot <<= 8; \
    bs->m_dwDot |= *(bs->m_pBuffer)++; \
    --(bs->m_cbBuflen); \
    bs->m_dwBitsLeft += 8; \
}
#if !(defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_HUFFDEC))
WMARESULT huffDecGet_i(const U16 *pDecodeTable, CWMAInputBitStream *bs,
					U32 *puResult, U32* puSign)
{
     const int FIRST_LOAD = 10;
     const int SECOND_LOAD = 12;

     register unsigned int ret_value;
     register const unsigned short* node_base = pDecodeTable;

     U32 uBits, uBitCnt;
     register U32 codeword;

     WMARESULT  wmaResult;
#ifdef PROFILE
	 //profiling a function that gets called this often has too much overhead.
     //FunctionProfile fp;
     //FunctionProfileStart(&fp,HUFF_DEC_GET_PROFILE);
#endif
     TRACEWMA_EXIT(wmaResult, ibstrmPeekBits(bs, FIRST_LOAD + SECOND_LOAD + 1, &uBits));
     codeword = uBits;

	 // Do first five 2-bit tables

	 //unroll, 1
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x8000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 2
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x8000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 4
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 5
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

     // Do remaining three 2-bit tables
	//unroll 1
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 2
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits


     // Do six 1-bit tables
	//unroll 6
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


	node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x8000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

decode_complete:
	assert(ret_value & 0x8000);
	uBitCnt = ((ret_value >> 10) & (0x0000001F));
	*puResult = ret_value & 0x000003FF;
	if (*puResult >= 0x03FC)
		*puResult = *(node_base + (*puResult & 0x0003) + 1);
     
	if (puSign != NULL)
		*puSign = uBits << uBitCnt;
	if (bs->m_dwBitsLeft < uBitCnt)
	{
		LOAD_BITS_FROM_DotT;
		LOAD_BITS_FROM_STREAM;

		if (bs->m_dwBitsLeft < uBitCnt)
		{
			TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(bs, ModeGetFlush, uBitCnt));
		}
	}
	//take the data and go home; or we have to pause
    bs->m_dwBitsLeft -= uBitCnt;
exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
     return wmaResult;
}
#endif // WMA_TARGET_MIPS && PLATFORM_SPECIFIC_HUFFDEC
WMARESULT huffDecGetMix(const U32 *pDecodeTable, CWMAInputBitStream *bs, huffResult *HResult, U32* puSign)
{
     const int FIRST_LOAD = 10;
     const int SECOND_LOAD = 12;

     register unsigned int ret_value;
     register const unsigned int* node_base = pDecodeTable;

     U32 uBits, ubitcount;
     register U32 codeword;

     WMARESULT  wmaResult;
#ifdef PROFILE
	 //profiling a function that gets called this often has too much overhead.
     //FunctionProfile fp;
     //FunctionProfileStart(&fp,HUFF_DEC_GET_PROFILE);
#endif

     TRACEWMA_EXIT(wmaResult, ibstrmPeekBits(bs, FIRST_LOAD + SECOND_LOAD + 1, &uBits));
     codeword = uBits;

	 // Do first five 2-bit tables

	 //unroll, 1
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x80000000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bitszz

	 //unroll, 2
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x80000000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 4
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 5
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

     // Do remaining three 2-bit tables
	//unroll 1
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 2
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits


     // Do six 1-bit tables
	//unroll 6
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


	node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

decode_complete:
     assert(ret_value & 0x80000000);
	 ubitcount = (ret_value & (0x0000001F));
	 if((HResult->state = ((ret_value>>23) & (0x3))) >= 2)
	 {
		 HResult->run = ((ret_value>>5) & (0x1ff));
		 HResult->level = ((ret_value>>14) & (0x1ff));
	 }     
     if (puSign != NULL)
        *puSign = uBits << ubitcount;
	if (bs->m_dwBitsLeft < ubitcount)
	{
		LOAD_BITS_FROM_DotT;
		LOAD_BITS_FROM_STREAM;

		if (bs->m_dwBitsLeft < ubitcount)
		{
			TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(bs, ModeGetFlush, ubitcount));
		}
	}
	//take the data and go home; or we have to pause
    bs->m_dwBitsLeft -= ubitcount;
exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
     return wmaResult;
}
WMARESULT huffDecGetMIXVec(const U32 *pDecodeTable, CWMAInputBitStream *bs, huffVecResult *HVResult)
//                     U32* puBitCnt, U32 *puResult, U32 *state)
{
     const int FIRST_LOAD = 10;
     const int SECOND_LOAD = 12;

     register unsigned int ret_value;
     register const unsigned int* node_base = pDecodeTable;

     U32 uBits, uBitCnt;
     register U32 codeword;

     WMARESULT  wmaResult;
#ifdef PROFILE
	 //profiling a function that gets called this often has too much overhead.
     //FunctionProfile fp;
     //FunctionProfileStart(&fp,HUFF_DEC_GET_PROFILE);
#endif

     TRACEWMA_EXIT(wmaResult, ibstrmPeekBits(bs, FIRST_LOAD + SECOND_LOAD + 1, &uBits));
     codeword = uBits;

	 // Do first five 2-bit tables

	 //unroll, 1
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x80000000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 2
	node_base += codeword >> 30;  // Use top 2 bits as offset
	ret_value = *node_base;
	if (ret_value & 0x80000000)
		goto decode_complete;  // Found a leaf node!

	node_base += ret_value;    // No leaf node, follow the offset
	codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 4
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	 //unroll, 5
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

     // Do remaining three 2-bit tables
	//unroll 1
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 2
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits

	//unroll 3
    node_base += codeword >> 30;  // Use top 2 bits as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 2;            // Advance to next 2 bits


     // Do six 1-bit tables
	//unroll 6
    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


	node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit


    node_base += codeword >> 31;  // Use top bit as offset
    ret_value = *node_base;
    if (ret_value & 0x80000000)
        goto decode_complete;  // Found a leaf node!

    node_base += ret_value;    // No leaf node, follow the offset
    codeword <<= 1;            // Advance to next bit

decode_complete:
	assert(ret_value & 0x80000000);
	uBitCnt = (ret_value & (0x0000001F));
	HVResult->state = ((ret_value>>21)&1);
	if(!(HVResult->state))
		HVResult->iResult = ((ret_value>>5) & 0x0000FFFF);
	if (bs->m_dwBitsLeft < uBitCnt)
	{
		LOAD_BITS_FROM_DotT;
		LOAD_BITS_FROM_STREAM;

		if (bs->m_dwBitsLeft < uBitCnt)
		{
			TRACEWMA_EXIT(wmaResult, ibstrmGetMoreData(bs, ModeGetFlush, uBitCnt));
		}
	}
	//take the data and go home; or we have to pause
    bs->m_dwBitsLeft -= uBitCnt;

exit:
#ifdef PROFILE
    //FunctionProfileStop(&fp);
#endif
     return wmaResult;
}
#else  // !WMA_OPT_HUFFDEC_ARM && !defined(WMA_OPT_HUFFDEC_XENON) && !defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_HUFFDEC)
void huffDecGetDummy(void)
{
}
#endif // !WMA_OPT_HUFFDEC_ARM && !defined(WMA_OPT_HUFFDEC_XENON) && !defined(WMA_TARGET_MIPS) && defined(PLATFORM_SPECIFIC_HUFFDEC)

#endif // 0
