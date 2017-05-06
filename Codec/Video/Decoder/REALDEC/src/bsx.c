/************************************************** BEGIN LICENSE BLOCK ****************************************************** 
* Version: RCSL 1.0 and Exhibits. 
* REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
* Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
* All Rights Reserved. 
* 
* The contents of this file, and the files included with this file, are 
* subject to the current version of the RealNetworks Community Source 
* License Version 1.0 (the "RCSL"), including Attachments A though H, 
* all available at http://www.helixcommunity.org/content/rcsl. 
* You may also obtain the license terms directly from RealNetworks. 
* You may not use this file except in compliance with the RCSL and 
* its Attachments. There are no redistribution rights for the source 
* code of this file. Please see the applicable RCSL for the rights, 
* obligations and limitations governing use of the contents of the file. 
* 
* This file is part of the Helix DNA Technology. RealNetworks is the 
* developer of the Original Code and owns the copyrights in the portions 
* it created. 
* 
* This file, and the files included with this file, is distributed and made 
* available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
* INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
* FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
* 
* Technology Compatibility Kit Test Suite(s) Location: 
* https://rarvcode-tck.helixcommunity.org 
* 
* Contributor(s): 
* 
****************************************************************** END LICENSE BLOCK *******************************************/ 

/***********************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information.
*    Copyright (c) 2001 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
*    Bitstream functionality.
***********************************************************************************

*************************************************************************************
*    INTEL Corporation Proprietary Information
*    This listing is supplied under the terms of a license
*    agreement with INTEL Corporation and may not be copied
*    nor disclosed except in accordance with the terms of
*    that agreement.
*    Copyright (c) 1995 - 1998 Intel Corporation.
*    All Rights Reserved.
***************************************************************************************/
/************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/bsx.c,v 1.1.1.1 2005/12/19 23:01:44 rishimathew Exp $ */
/********************************************************************************
Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		CB_DecodeSuperSubBlockCoefficients is 
modified to support combined block 
decoding and dequantization	
********************************************************************************/
#include "decoder.h"
#include "bsx.h"
/*************************************************************************/
/*		CBaseBitstream::GetBsOffset() 										*/
/*		Returns the bit position of the buffer pointer relative to the	*/		
/*		beginning of the buffer. 										*/
/*************************************************************************/
static U32 CB_GetBsOffset(SLICE_DEC *slice_di)
{
	return((U32)(slice_di->m_pBitptr - slice_di->m_pBitstart - 4) * 8 + slice_di->m_bitpos);
} /* CBaseBitstream::GetBsOffset() */

/* --------------------------------------------------------------------------- */
/*  CBaseBitstream::GetBsMaxSize() */
/*		Returns the maximum bitstream size. */
/* --------------------------------------------------------------------------- */
static U32 CB_GetMaxBsSize(SLICE_DEC *slice_di)
{
	return(slice_di->m_maxBsSize);

} /* CBaseBitstream::GetBsMaxSize() */

/* gs_VLCDecodeTable maps the leading 8-bits of the unread bitstream */
/* into the length and info of the VLC code at the head of those 8 bits. */
/* The table's contents can be summarized as follows, where p, q, r, s */
/* and '*' are arbitrary binary digits {0, 1}. */
/* */
/*  8 Bits      Length  Info */
/*  ========    ======  ==== */
/*  0p0q0r0s    8       pqrs combined with VLC at head of next 8 bits */
/*  0p0q0r1*    7       pqr */
/*  0p0q1***    5       pq */
/*  0p1*****    3       p */
/*  1*******    1       0 */
/* */
/* Note that when the length is 8, or even, we need to read another 8 bits */
/* from the bitstream, look up these 8 bits in the same table, and combine */
/* the length and info with the previously decoded bits. */
/* */
/* For a given 8-bit chunk of bitstream bits, the maximum length is 8, */
/* and the maximum info value is contained in 4 bits.  Thus, we can pack */
/* the length and info in a U8.  The info is in the least significant */
/* 4 bits, and the length is in the most significant 4 bits. */

#define PACK_LENGTH_AND_INFO(length, info) (((length) << 4) | (info))

static const U8 gs_VLCDecodeTable[256] =
{
	PACK_LENGTH_AND_INFO(8,  0),   /* 00000000 */
	PACK_LENGTH_AND_INFO(8,  1),   /* 00000001 */
	PACK_LENGTH_AND_INFO(7,  0),   /* 00000010 */
	PACK_LENGTH_AND_INFO(7,  0),   /* 00000011 */
	PACK_LENGTH_AND_INFO(8,  2),   /* 00000100 */
	PACK_LENGTH_AND_INFO(8,  3),   /* 00000101 */
	PACK_LENGTH_AND_INFO(7,  1),   /* 00000110 */
	PACK_LENGTH_AND_INFO(7,  1),   /* 00000111 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001000 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001001 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001010 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001011 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001100 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001101 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001110 */
	PACK_LENGTH_AND_INFO(5,  0),   /* 00001111 */
	PACK_LENGTH_AND_INFO(8,  4),   /* 00010000 */
	PACK_LENGTH_AND_INFO(8,  5),   /* 00010001 */
	PACK_LENGTH_AND_INFO(7,  2),   /* 00010010 */
	PACK_LENGTH_AND_INFO(7,  2),   /* 00010011 */
	PACK_LENGTH_AND_INFO(8,  6),   /* 00010100 */
	PACK_LENGTH_AND_INFO(8,  7),   /* 00010101 */
	PACK_LENGTH_AND_INFO(7,  3),   /* 00010110 */
	PACK_LENGTH_AND_INFO(7,  3),   /* 00010111 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011000 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011001 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011010 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011011 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011100 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011101 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011110 */
	PACK_LENGTH_AND_INFO(5,  1),   /* 00011111 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100000 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100001 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100010 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100011 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100100 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100101 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100110 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00100111 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101000 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101001 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101010 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101011 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101100 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101101 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101110 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00101111 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110000 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110001 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110010 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110011 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110100 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110101 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110110 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00110111 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111000 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111001 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111010 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111011 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111100 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111101 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111110 */
	PACK_LENGTH_AND_INFO(3,  0),   /* 00111111 */
	PACK_LENGTH_AND_INFO(8,  8),   /* 01000000 */
	PACK_LENGTH_AND_INFO(8,  9),   /* 01000001 */
	PACK_LENGTH_AND_INFO(7,  4),   /* 01000010 */
	PACK_LENGTH_AND_INFO(7,  4),   /* 01000011 */
	PACK_LENGTH_AND_INFO(8, 10),   /* 01000100 */
	PACK_LENGTH_AND_INFO(8, 11),   /* 01000101 */
	PACK_LENGTH_AND_INFO(7,  5),   /* 01000110 */
	PACK_LENGTH_AND_INFO(7,  5),   /* 01000111 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001000 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001001 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001010 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001011 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001100 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001101 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001110 */
	PACK_LENGTH_AND_INFO(5,  2),   /* 01001111 */
	PACK_LENGTH_AND_INFO(8, 12),   /* 01010000 */
	PACK_LENGTH_AND_INFO(8, 13),   /* 01010001 */
	PACK_LENGTH_AND_INFO(7,  6),   /* 01010010 */
	PACK_LENGTH_AND_INFO(7,  6),   /* 01010011 */
	PACK_LENGTH_AND_INFO(8, 14),   /* 01010100 */
	PACK_LENGTH_AND_INFO(8, 15),   /* 01010101 */
	PACK_LENGTH_AND_INFO(7,  7),   /* 01010110 */
	PACK_LENGTH_AND_INFO(7,  7),   /* 01010111 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011000 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011001 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011010 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011011 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011100 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011101 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011110 */
	PACK_LENGTH_AND_INFO(5,  3),   /* 01011111 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100000 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100001 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100010 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100011 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100100 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100101 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100110 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01100111 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101000 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101001 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101010 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101011 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101100 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101101 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101110 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01101111 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110000 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110001 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110010 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110011 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110100 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110101 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110110 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01110111 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111000 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111001 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111010 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111011 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111100 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111101 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111110 */
	PACK_LENGTH_AND_INFO(3,  1),   /* 01111111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10000111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10001111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10010111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10011111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10100111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10101111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10110111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 10111111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11000111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11001111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11010111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11011111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11100111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11101111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110110 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11110111 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11111000 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11111001 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11111010 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11111011 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11111100 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11111101 */
	PACK_LENGTH_AND_INFO(1,  0),   /* 11111110 */
	PACK_LENGTH_AND_INFO(1,  0)    /* 11111111 */
};


/************************************************************/
/*  [DEC] CBaseBitstream::GetVLCBits() 						*/	
/*        Reads one general VLC code from the bitstream.	*/	 
/************************************************************/
U32 CB_GetVLCBits(SLICE_DEC *slice_di, U32 *pInfo)
{

	U32 vlcBits, info = 0, length = 0;
	U32 thisChunksLength;
	I32 bits,bitpos;

	bits     = slice_di->m_bits;
	bitpos   = slice_di->m_bitpos;

	do
	{
		/* Extract the leading 8-bits into 'bits' */
		loadbits_check(slice_di, bits, bitpos, 32 - 8);
		vlcBits = showbits_pos(bits,bitpos,8);

		vlcBits = gs_VLCDecodeTable[vlcBits];
		thisChunksLength = vlcBits >> 4;

		info = (info << (thisChunksLength >> 1)) | (vlcBits & 0xf);
		length += thisChunksLength;
		flushbits_pos(bitpos,thisChunksLength);

	} while (!(length & 1));  /* We're done when length is an odd number */

	slice_di->m_bits   = bits;
	slice_di->m_bitpos = bitpos;

	*pInfo = info;

	return length;
} /* CBaseBitstream::GetVLCBits() */

I32 realVideoFrametype(struct Decoder *dec,U8* bits)
{
	U32 length, PicCodType;

	if(bits==NULL)
		return -1;

	if(dec->m_fid == RV_FID_RV89COMBO){
		U32 vlcBits,thisChunksLength;
		U32 info = 0;

		if(dec->m_bIsRV8 && bits[0]==0 && bits[1]==0 && bits[2]==1){
			bits += 3;	
		}else if(bits[0]==0x55 && bits[1]==0x55 && bits[2]==0x55 && bits[3]==0x55){
			bits += 4;
		}else{
			return -1;
		}

		//picture header is 31 bit
		/* Picture type */
		bits += 3;
		if(bits[0] & 1){
			length = 1;
		}else{
			vlcBits = (bits[1] >> 1) & 0x7f;
			vlcBits = gs_VLCDecodeTable[vlcBits];
			thisChunksLength = vlcBits >> 4;

			info = (info << (thisChunksLength >> 1)) | (vlcBits & vlcBits);
			length = thisChunksLength;
		}

		if (length == 1){//P
			PicCodType = 1;
		}else if (length == 3 && info == 1){//I
			PicCodType = 0;
		}else if (length == 5 && info == 0){//B
			PicCodType = 2;
		}else{
			return -1;
		}

	}else{
		I32 type;

		if(dec->m_bIsRV8){
			type = (bits[0]>>3)&0x03;
		}else{
			type = (bits[0]>>5)&0x03;
		}

		switch(type)
		{
		case RV_FORCED_INTRAPIC:
			PicCodType = 0;
			break;
		case RV_INTRAPIC:
			PicCodType = 0;
			break;
		case RV_INTERPIC:
			PicCodType = 1;
			break;
		case RV_TRUEBPIC:
			PicCodType = 2;
			break;
		default:
			return -1;
		}
	}

	return PicCodType;
}

/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::GetBits() */
/*		Reads bits from buffer.  Supports up to 25 bits. */
/*		nbits	: number of bits to be read */
/* --------------------------------------------------------------------------- */

const U32 GetBitsMask[25] =
{
	0x00000000, 0x00000001, 0x00000003, 0x00000007,
	0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
	0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
	0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
	0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
	0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
	0x00ffffff
};
/* --------------------------------------------------------------------------- */
/*  [DEC] CBaseBitstream::SearchBits() */
/*		Searches for a code with known number of bits.  Bitstream state,  */
/*		pointer and bit offset, will be updated if code found. */
/*		nbits		: number of bits in the code */
/*		code		: code to search for */
/*		lookahead	: maximum number of bits to parse for the code */
/* --------------------------------------------------------------------------- */
static RV_Boolean CB_SearchBits(SLICE_DEC *slice_di, U32 nbits, U32 code,U32 lookahead)
{
	U32	w, n;
	U8*	pbs;
	I32	offset,bits;

	loadbits(slice_di);

	pbs		= slice_di->m_pBitptr;
	offset	= slice_di->m_bitpos;
	bits    = slice_di->m_bits;

	w = getbits(slice_di, nbits);

	for (n = 0; w != code && n < lookahead; n ++){
		w = ((w << 1) & GetBitsMask[nbits]) | (getbits1(slice_di));
	}
		

	if (w == code){
		return(TRUE);
	}else{
		slice_di->m_pBitptr		= pbs;
		slice_di->m_bitpos      = offset;
		slice_di->m_bits        = bits;
		return(FALSE);
	}

} /* CBaseBitstream::SearchBits() */

/* ******************************************************************************
*  [DEC] CBaseBitstream::SearchBits_IgnoreOnly0() 
*		Searches for a code with known number of bits.  Skips only zero bits.
*		Search fails if code has not been found and non-zero bit encountered. 
*		nbits		: number of bits in the code 
*		code		: code to search for 
*		lookahead	: maximum number of bits to parse for the code 
********************************************************************************/
static RV_Boolean	CB_SearchBits_IgnoreOnly0(SLICE_DEC *slice_di,const U32	nbits,const U32	code,const U32	lookahead)
{
	U32	w, n, msb;
	I32	offset, bits;
	U8*	pbs;

	loadbits(slice_di);

	pbs		= slice_di->m_pBitptr;
	offset	= slice_di->m_bitpos;
	bits    = slice_di->m_bits;
	msb		= 1 << (nbits - 1);

	w = getbits(slice_di, nbits);
	loadbits(slice_di);

	for (n = 0; w != code && n < lookahead; n ++){
		if (w & msb) break;
		w = ((w << 1) & GetBitsMask[nbits]) | (getbits1(slice_di));
	}

	if (w == code){
		return(TRUE);
	}else{
		slice_di->m_pBitptr	= pbs;
		slice_di->m_bitpos 	= offset;
		slice_di->m_bits    = bits;
		return(FALSE);
	}

} /* CBaseBitstream::SearchBits_IgnoreOnly0() */


#define MAX_SSC_LOOKAHEAD_NUMBER 7 


/* Bitstream Version Information */
/* Starting with RV_FID_REALVIDEO30, for RealVideo formats we embed the */
/* minor bitstream version number in the slice header. */
/* The encoder only ever produces one bitstream format.  But the decoder */
/* must be backwards compatible, and able to decode any prior minor */
/* bitstream version number. */
/* */
/* It is assumed that if the bitstream major version number changes, */
/* then a new RV_FID is introduced.  So, only the minor version number */
/* is present in the bitstream. */
/* */
/* The minor version number is encoded in the bitstream using 3 bits. */
/* RV89Combo's first bitstream minor version number is "2", which is encoded */
/* as all 0's.  The following table maps the bitstream value to the */
/* actual minor version number. */

#define UNSUPPORTED_RV_BITSTREAM_VERSION 9999

#define ENCODERS_CURRENT_RV_BITSTREAM_VERSION 2
/* This should match RV_BITSTREAM_MINOR_VERSION in "hiverv.h" */

static const U32 s_RVVersionEncodingToMinorVersion[8] = {
	2,
	UNSUPPORTED_RV_BITSTREAM_VERSION,
	UNSUPPORTED_RV_BITSTREAM_VERSION,
	UNSUPPORTED_RV_BITSTREAM_VERSION,
	UNSUPPORTED_RV_BITSTREAM_VERSION,
	UNSUPPORTED_RV_BITSTREAM_VERSION,
	UNSUPPORTED_RV_BITSTREAM_VERSION,
	UNSUPPORTED_RV_BITSTREAM_VERSION
};

#define NUMBER_OF_RV_BITSTREAM_VERSIONS \
	(sizeof(s_RVVersionEncodingToMinorVersion) \
	/ sizeof(s_RVVersionEncodingToMinorVersion[0]))

/* macros for getting slice valid flag and slice offset */
#define SliceFlag(x)		(t->m_sliceInfo[(x)].is_valid)
#define SliceOffset(x)		(t->m_sliceInfo[(x)].offset)	
#define SliceBitOffset(x)	(SliceOffset(x) << 3)


/***************************************************************************************************/
/*		CB_SetDimensions( ) 																		*/
/*		use in the decoder to initialize size, m_nGOBs, m_nMBs, and m_mbaSize 						*/
/**************************************************************************************************/

/* for PutSliceHeader              SQCIF, QCIF, CIF, 4CIF, 16CIF, 2048x1152 */
static const U32 MBA_NumMBs[]     = { 47,   98, 395, 1583,  6335, 9215 };
static const U32 MBA_FieldWidth[] = {  6,    7,   9,   11,    13,   14 };
static const U32 MBA_Widths =
sizeof( MBA_FieldWidth ) / sizeof( MBA_FieldWidth[0] );


void CB_SetDimensions(struct Decoder *t)
{
	U32 i,j;
	U32 MBWidth,MBHeight;
	U32 mbaSize;
	/* Set m_nMBs and m_nGOBs based on the image dimensions. */

	MBWidth  = t->m_mbX;
	MBHeight = t->m_mbY;

	/* initialize m_mbaSize */

	j = MBWidth * MBHeight - 1;

	for (i = 0; i < (MBA_Widths - 1) && MBA_NumMBs[i] < j; i++)
		;

	mbaSize = MBA_FieldWidth[i];
	t->m_mbaSize = mbaSize;

} /* CB_SetDimensions() */

void initbits(SLICE_DEC* slice_info, U8  *stream, const I32 len)
{
	slice_info->m_bits		= 0;
	slice_info->m_bitpos	= 32;
	slice_info->m_pBitptr	= stream;
	slice_info->m_pBitstart = stream;
	slice_info->m_pBitend   = stream + len + 4;
	slice_info->m_maxBsSize = len;
}

/* lookup table used for chroma blocks when modified quantization is in effect */
/* Annex T tables used in encoder too  */
static const I32 gNewTAB_DQUANT_MQ_RV89[32][2] =
{
	{ 0, 0}, { 2, 1}, {-1, 1}, {-1, 1}, /* 0 .. 3 */
	{-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, /* 4 .. 7 */
	{-1, 1}, {-1, 1}, {-1, 1}, {-2, 2}, /* 8 .. 11 */
	{-2, 2}, {-2, 2}, {-2, 2}, {-2, 2}, /* 12 .. 15 */
	{-2, 2}, {-2, 2}, {-2, 2}, {-2, 2}, /* 16 .. 19 */
	{-2, 2}, {-3, 3}, {-3, 3}, {-3, 3}, /* 20 .. 23 */
	{-3, 3}, {-3, 3}, {-3, 3}, {-3, 3}, /* 24 .. 27 */
	{-3, 3}, {-3, 2}, {-3, 1}, {-3,-5}  /* 28 .. 31 */
};

/****************************************************************/
/*  CRealVideoBs::GetDQUANT() 									*/
/***************************************************************/
void CB_GetDQUANT(SLICE_DEC *slice_di, U8 * quant, const U32 quant_prev)
{
	I32 qp;


	loadbits(slice_di);
	if (getbits1(slice_di)){
		qp = quant_prev + gNewTAB_DQUANT_MQ_RV89[quant_prev][getbits1(slice_di)];
	}else{
		qp = getbits(slice_di, 5);
	}

	qp = MIN(MAX(0, qp), 31);
	*quant = (U8)qp;

} /* CRealVideoBs::GetDQUANT() */

/****************************************************************/
/*  CRealVideoBs::GetPicSize() 									*/
/*  Gets the picture size from the bitstream. 					*/
/****************************************************************/
void CB_GetPicSize(struct Decoder *t, U32 *width, U32 *height, SLICE_DEC *slice_di)
{
	U32 code;
	U32 w, h;

	I32 bits,bitpos;

	static const U32 code_width[8] = {160, 176, 240, 320, 352, 640, 704, 0};
	static const U32 code_height1[8] = {120, 132, 144, 240, 288, 480, 0, 0};
	static const U32 code_height2[4] = {180, 360, 576, 0};

	loadbits(slice_di);
	/* If we're decoding a P or B frame, get 1 bit that signals */
	/* difference in size from previous reference frame. */
	if (t->m_ptype == RV_INTERPIC || t->m_ptype == RV_TRUEBPIC)
	{
		/* no diff in size */
		if (getbits1(slice_di))
		{
			*width = t->m_out_img_width;
			*height= t->m_out_img_height;
			return;
		}
	}

	/* width */
	code = getbits(slice_di, 3);

	bits   = slice_di->m_bits;
	bitpos = slice_di->m_bitpos;

	w = code_width[code];
	if (w == 0)
	{
		do
		{
			loadbits_check(slice_di,bits,bitpos,32 - 8);
			code = showbits_pos(bits,bitpos,8);
			flushbits_pos(bitpos,8);
			w += (code << 2);
		}
		while (code == 255);
	}


	/* height */
	loadbits_check(slice_di,bits,bitpos,32 - 8);
	code = showbits_pos(bits,bitpos,3);
	flushbits_pos(bitpos,3);
	h = code_height1[code];
	if (h == 0)
	{
		code <<= 1;
		code |= showbits_pos(bits,bitpos,1);
		flushbits_pos(bitpos,1);
		code &= 3;
		h = code_height2[code];
		if (h == 0)
		{
			do
			{
				loadbits_check(slice_di,bits,bitpos,32 - 8);
				code = showbits_pos(bits,bitpos,8);
				flushbits_pos(bitpos,8);
				h += (code << 2);
			}
			while (code == 255);
		}
	}

	*width = w;
	*height = h;

	slice_di->m_bits = bits;
	slice_di->m_bitpos = bitpos;
}

/* --------------------------------------------------------------------------- */
/*  CB_GetCPFMT() */	
/* --------------------------------------------------------------------------- */
RV_Boolean	CB_GetCPFMT(struct Decoder *t, SLICE_DEC *slice_di)
{
	I32 pixel_aspect_ratio;

	loadbits(slice_di);
	pixel_aspect_ratio = getbits(slice_di,4);
	/* Frame width indication */
	/* The number of pixels per line is given by (FWI+1)*4 */
	t->m_out_img_width = (getbits(slice_di,9) + 1) << 2;
	if(t->m_out_img_width > t->m_max_img_width){
		t->m_out_img_width = t->m_max_img_width;
	}
	t->m_mbX = (t->m_out_img_width + 15) >> 4;
	t->m_img_width = t->m_mbX << 4;

	//getbits1(slice_di);
	flushbits(slice_di,1);

	/* Frame height indication */
	/* The number of lines is given by FHI*4 */
	t->m_out_img_height = getbits(slice_di, 9) << 2;
	if(t->m_out_img_height > t->m_max_img_height){
		t->m_out_img_height = t->m_max_img_height;
	}
	t->m_mbY = (t->m_out_img_height + 15) >> 4;
	t->m_img_height = t->m_mbY << 4;

	return(TRUE);

} /* CB_GetCPFMT() */

/****************************************************************/
/*  CB_GetPictureHeader( )		*/
/*  RV9							*/
/****************************************************************/
RV_Status CB_GetPictureHeader(struct Decoder *t, SLICE_DEC *slice_di)
{
	if (t->m_fid == RV_FID_REALVIDEO30){
		U32 width, height;
		/* note: header should be initialized earlier by SetFrameProperties() */
		/* skip if this is the first slice called from IAPass1ProcessFrame */
		/* otherwise read the slice header */

		/* ECC */
		loadbits(slice_di);
		if (getbits1(slice_di) != 0){
			RVDebug((RV_TL_SUBPICT, "WARNING GetSliceHeader_RV9 - "
				"ECC packet"));
			return RV_S_ERROR;
		}

		/* PTYPE */
		t->m_ptype = (EnumRVPicCodType) getbits(slice_di,2);


		/* PQUANT/SQUANT */
		slice_di->m_pquant = (U8) getbits(slice_di,5);
		t->m_deblock_pquant = slice_di->m_pquant;

		/* Bitstream version */
		if (getbits1(slice_di) != 0){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
				"Unrecognized RV bitstream version"));
			return RV_S_ERROR;
		}

		if (getbits1(slice_di)){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
				"Interlaced bit set."));
			return RV_S_ERROR;
		}


		/* OSV Quant */
		slice_di->m_OSVQUANT = (U8) getbits(slice_di,2);

		/* In-loop deblocking */
		t->m_deblocking_filter_passthrough = getbits(slice_di,1);

		loadbits(slice_di);
		t->m_tr = getbits(slice_di, 13); 

		/* PCTSZ (RealVideo RPR picture size) */

		CB_GetPicSize(t, &width, &height,slice_di);

		if(width > t->m_max_img_width){
			width = t->m_max_img_width;
		}

		if(height > t->m_max_img_height){
			height = t->m_max_img_height;
		}

		t->m_out_img_width = width;
		t->m_out_img_height = height;

		t->m_mbX = (width  + 15) >> 4;
		t->m_mbY = (height + 15) >> 4;

		t->m_img_width  = t->m_mbX << 4;
		t->m_img_height = t->m_mbY << 4;

		//TBD
		CB_SetDimensions(t);

		/* MBA */
		loadbits(slice_di);
		slice_di->m_mba = getbits(slice_di,t->m_mbaSize);

	}else{
		U32 length, info;
		const U32 PSC_LEN = 31;

		/* SUPER_VLC needs longer start code to avoid emulation */
		loadbits(slice_di);
		if (21845 != getbits(slice_di,16)){
			RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
				"Failed to find Picture Start Code"));
			return RV_S_ERROR;
		}

		loadbits(slice_di);
		if (21845 != getbits(slice_di,16)){
			RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
				"Failed to find Picture Start Code"));
			return RV_S_ERROR;
		}

		length = CB_GetVLCBits(slice_di, &info);

		if (length < PSC_LEN){
			RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
				"Failed to find Picture Start Code"));
			return RV_S_ERROR;
		}

		if (info & 1){
			RVDebug((RV_TL_ALWAYS,"ERROR GetPictureHeader -- "
				"Found sequence end code"));
			return RV_S_OUT_OF_RANGE;
			/* This is probably not the right error code to use, */
			/* but it will have to do for now. */
		}


		if (!((info>>1) & 1)){
			/* QCIF; */
			t->m_img_width = 176;
			t->m_img_height = 144;
		}else{ 
			t->m_img_width = 0;
			t->m_img_height = 0;
		}

		slice_di->m_pquant = (U8)((info>>2) & 31);
		t->m_tr = (info>>(2+5)) & 255;

		t->m_deblock_pquant = slice_di->m_pquant;

		/* Picture type */
		length = CB_GetVLCBits(slice_di, &info);

		if (length == 1){
			t->m_ptype = RV_INTERPIC;
		}else if (length == 3 && info == 1){
			t->m_ptype = RV_INTRAPIC;
		}else if (length == 5 && info == 0){
			t->m_ptype = RV_TRUEBPIC;
		}else{
			RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
				"Unsupported picture coding type"));
			return RV_S_UNSUPPORTED;
		}

		/* Non-QCIF frame size */
		if (t->m_img_width == 0){
			CB_GetCPFMT(t, slice_di);
		}

		CB_SetDimensions(t);

		/* OSV Quant */
		loadbits(slice_di);
		slice_di->m_OSVQUANT = (U8) getbits(slice_di,2);
		slice_di->m_mba = 0;
	}

	slice_di->m_entropyqp = get_entropyqp(slice_di->m_pquant, slice_di->m_OSVQUANT);
	if(t->m_ptype == RV_INTERPIC){
		t->GetInterMBType = DecodePMBType_RV9;
	}else{/*only used in B frame*/
		t->GetInterMBType = DecodeBMBType_RV9;
	}

	return RV_S_OK;
}

/* --------------------------------------------------------------------------- */
/*  CB_SetRPRSizes() */
/* --------------------------------------------------------------------------- */
void CB_SetRPRSizes(struct Decoder *t,U32 num_sizes, U32 *sizes)
{
	U8 size[9] = {0,1,1,2,2,3,3,3,3};

	t->m_numSizes = num_sizes;
	t->m_pctszSize = size[num_sizes];

	t->m_pSizes = sizes;
}
/****************************************************************/
/*  CB_GetPictureHeader_RV8() 								*/
/****************************************************************/
RV_Status CB_GetPictureHeader_RV8(struct Decoder *t, SLICE_DEC *slice_di)
{
	RV_Status Status = RV_S_OK;
	U32 length, info;

	CB_SetRPRSizes(t, t->m_options.Num_RPR_Sizes, &t->m_options.RPR_Sizes[0]);

	loadbits(slice_di);

	/* Actually, it's the first slice header*/
	if (t->m_fid == RV_FID_REALVIDEO30){
		U32 versionEncoding;
		/* find the first valid slice */
		//TBD CB_IsSegmentValid

		/* Bitstream version */
		versionEncoding = getbits(slice_di,3);
		if (versionEncoding >= NUMBER_OF_RV_BITSTREAM_VERSIONS
			|| s_RVVersionEncodingToMinorVersion[versionEncoding]> ENCODERS_CURRENT_RV_BITSTREAM_VERSION){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
				"Unrecognized RV bitstream version %ld",
				versionEncoding));
			return RV_S_ERROR;
		}

		/* Eventually, store the bitstream version in m_rv. */
		/* Currently not necessary since we only support one */
		/* bitstream version. */

		/* PTYPE */
		t->m_ptype = (EnumRVPicCodType) getbits(slice_di,2);

		/* ECC */
		//getbits1(slice_di);
		flushbits(slice_di,1);


		/* PQUANT/SQUANT */
		slice_di->m_pquant = (U8) getbits(slice_di,5);

		/* 0-31 are legitimate QP values for RV89Combo */

		/* In-loop deblocking */
		t->m_deblocking_filter_passthrough = getbits(slice_di,1)? TRUE:FALSE;

		t->m_tr = getbits(slice_di,13);

		/* PCTSZ (RealVideo RPR picture size) */
		if (t->m_pctszSize > 0){
			U32 width, height;

			loadbits(slice_di);
			t->m_pctsz = (U8)getbits(slice_di,t->m_pctszSize);

			if (t->m_pSizes != NULL){
				width  = t->m_pSizes[2 * t->m_pctsz];
				height = t->m_pSizes[2 * t->m_pctsz + 1];

				if(width > t->m_max_img_width){
					width = t->m_max_img_width;
				}

				if(height > t->m_max_img_height){
					height = t->m_max_img_height;
				}

				t->m_out_img_width = width;
				t->m_out_img_height = height;

				t->m_mbX = (width  + 15) >> 4;
				t->m_mbY = (height + 15) >> 4;

				t->m_img_width  = t->m_mbX << 4;
				t->m_img_height = t->m_mbY << 4;

				CB_SetDimensions(t);
			}else{
				RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
					"t->m_pSizes array not initialized"));
				return RV_S_ERROR;
			}
		}

		/* MBA */
		loadbits(slice_di);
		slice_di->m_mba = getbits(slice_di, t->m_mbaSize);

		/* RTYPE */
		flushbits(slice_di,1);
	}else{
		const U32 PSC_LEN = 31;

			loadbits(slice_di);
			if (1 != getbits(slice_di,24)){
			RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
				"Failed to find Picture Start Code"));
			return RV_S_ERROR;
		}

		length = CB_GetVLCBits(slice_di, &info);

		if (length < PSC_LEN){
			RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader_RV8 -- "
				"Failed to find Picture Start Code"));
			return RV_S_ERROR;
		}

		if (info & 1){
			RVDebug((RV_TL_ALWAYS,"ERROR GetPictureHeader_RV8 -- "
				"Found sequence end code"));
			return RV_S_OUT_OF_RANGE;
			/* This is probably not the right error code to use, */
			/* but it will have to do for now. */
		}


		if (!((info>>1) & 1)){
			/* QCIF; */
			t->m_img_width  = 176;
			t->m_img_height = 144;
		}else{ 
			t->m_img_width  = 0;
			t->m_img_height = 0;
		}

		slice_di->m_pquant = (U8)((info>>2) & 31);
		t->m_tr = (info>>(2+5)) & 255;

		/* Picture type */
		length = CB_GetVLCBits(slice_di, &info);

		if (length == 1){
			t->m_ptype = RV_INTERPIC;
		}else if (length == 3 && info == 1){
			t->m_ptype = RV_INTRAPIC;
		}else if (length == 5 && info == 0){
			t->m_ptype = RV_TRUEBPIC;
		}else{
			RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader_RV8 -- "
				"Unsupported picture coding type"));
			return RV_S_UNSUPPORTED;
		}

		/* Non-QCIF frame size */
		if (t->m_img_width == 0){
			CB_GetCPFMT(t, slice_di);
		}
		
		CB_SetDimensions(t);
		slice_di->m_mba = 0;
	}
	slice_di->m_entropyqp = slice_di->m_pquant;
	if(t->m_ptype == RV_INTERPIC){
		t->GetInterMBType = DecodePMBType_RV8;
	}else{/*only used in B frame*/
		t->GetInterMBType = DecodeBMBType_RV8;
	}

	return Status;
} /* getpictureheader_rv8 */

/*********************************************/
/*  CB_GetSSC() 							*/
/* *******************************************/
//TBD clean up
RV_Boolean CB_GetSSC(struct Decoder *t, U32 slice_number, SLICE_DEC *slice_di)
{
	U32 offset,deltaofs;

	if (t->m_fid == RV_FID_REALVIDEO30){
		offset = CB_GetBsOffset(slice_di);

		if (offset < SliceBitOffset(0) ||offset >= (CB_GetMaxBsSize(slice_di) << 3))
			return FALSE;

		/* there is no more slice header in the bitstream */
		if (slice_number == t->m_nSlices)
			return FALSE;

		/* check if just beginning to decode the current slice */
		if(t->m_nSlices > 1){
			if (offset == SliceBitOffset(slice_number))
				return TRUE;
		}

		/* skip stuffing if any */
		deltaofs = SliceBitOffset(slice_number) - offset;
		if (deltaofs < 8 && CB_SearchBits(slice_di, deltaofs, 0, 0))
			return TRUE;

	}else{
		U32 nbits, code;
		if(t->m_bIsRV8){
			nbits = 17;
			code = (0x00008000 >> (32 - 17));
		}else{
			nbits = 24;
			code = (0x1d1c105e >> (32 - 24));
		}
		if (CB_SearchBits_IgnoreOnly0(slice_di, nbits, code,MAX_SSC_LOOKAHEAD_NUMBER))
			return TRUE;

		RVDebug((RV_TL_SUBPICT, "GetSSC - SSC not found"));
	}

	return FALSE;

} /* CB_GetSSC() */


/*******************************************/
/*  CB_GetSliceHeader() 					*/
/*******************************************/
//TBD merge bits read
RV_Status CB_GetSliceHeader(struct Decoder *t, I32 *iSliceMBA, SLICE_DEC *slice_di)
{
	loadbits(slice_di);

	if (t->m_fid == RV_FID_REALVIDEO30){
		/* skip if this is the first slice called from IAPass1ProcessFrame */
		/* otherwise read the slice header */

		U32 fieldlen;
		U32 width, height;

		/* RealNetworks Slice Header */
		//flushbits(slice_di,1 + 2);//ECC + PTYPE

		if (getbits1(slice_di) != 0){//huwei 20110315 stabilty
			return RV_S_ERROR;
		}

		/* PTYPE */
		flushbits(slice_di, 2);//PTYPE

		/* PQUANT/SQUANT */
		slice_di->m_pquant = (U8) getbits(slice_di,5);

		flushbits(slice_di,2);

		/* OSV Quant */
		slice_di->m_OSVQUANT = (U8) getbits(slice_di,2);

		/* In-loop deblocking */
		//t->m_deblocking_filter_passthrough = getbits(&t->m_slice_di[slice_index],1)?TRUE:FALSE;
		flushbits(slice_di,1);
		fieldlen = 13;

		loadbits(slice_di);
		flushbits(slice_di,fieldlen) ;

		/* PCTSZ (RealVideo RPR picture size) */
		CB_GetPicSize(t, &width, &height, slice_di);

		if (t->m_out_img_width != width || t->m_out_img_height != height){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
				"Unmatched PCTSZ"));
			return RV_S_ERROR;
		}

		/* MBA */
		loadbits(slice_di);
		slice_di->m_mba = getbits(slice_di,t->m_mbaSize);
		*iSliceMBA	= slice_di->m_mba;

	}else{
		/* The SSC either does not exist because this is the first slice */
		/* after the picture start code, or the SSC has already been read */
		/* in IAPass1. */



		/* Must be 1 to prevent start code emulation (SEPB1) */
		if (getbits1(slice_di) != 1){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
			return RV_S_ERROR;
		}

		/* No SSBI since CPM can not be "1". */

		/* Get macro-block address */
		slice_di->m_mba = *iSliceMBA = getbits(slice_di, t->m_mbaSize);
		loadbits(slice_di);
		if (t->m_mbaSize > 11){
			/* Must be 1 to prevent start code emulation (SEPB2) */
			if (getbits1(slice_di) != 1){
				RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
				return RV_S_ERROR;
			}
		}

		/* Get SQUANT */
		slice_di->m_pquant = (U8)getbits(slice_di,5);

		/* OSV Quant */
		slice_di->m_OSVQUANT = (U8) getbits(slice_di,2);

		/* No SWI field. */

		/* Must be 1 to prevent start code emulation (SEPB3) */
		if (getbits1(slice_di) != 1){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
			return RV_S_ERROR;
		}

		/* Get GOB frame ID. */
		getbits(slice_di,2);

		RVDebug((RV_TL_SUBPICT, 
			"CB_GetSliceHeader_RV8 - MBA=%ld ",
			*iSliceMBA));
	}

	slice_di->m_entropyqp = get_entropyqp(slice_di->m_pquant,slice_di->m_OSVQUANT);
	return RV_S_OK;

} /* CB_GetSliceHeader() */


/*****************************************************************/
/*  CB_GetSliceHeader() 										*/
/****************************************************************/
RV_Status CB_GetSliceHeader_RV8(struct Decoder*t, I32* iSliceMBA, SLICE_DEC *slice_di)
{
	U32 fieldlen;

	loadbits(slice_di);

	if (t->m_fid == RV_FID_REALVIDEO30){
		U32 versionEncoding = 0;

		/* Bitstream version + PTYPE(2bit) + ECC(1bit)*/
		//flushbits(slice_di, 3 + 2 + 1);

		versionEncoding = getbits(slice_di,3);
		if (versionEncoding >= NUMBER_OF_RV_BITSTREAM_VERSIONS
			|| s_RVVersionEncodingToMinorVersion[versionEncoding]> ENCODERS_CURRENT_RV_BITSTREAM_VERSION){
				return RV_S_ERROR;
		}

		/* Eventually, store the bitstream version in m_rv. */
		/* Currently not necessary since we only support one */
		/* bitstream version. */

		/* PTYPE */
		flushbits(slice_di, 2 + 1);//PTYPE + ECC

		/* PQUANT/SQUANT Max is 31*/
		slice_di->m_pquant = (U8) getbits(slice_di,5);

		/* In-loop deblocking */
		//t->m_deblocking_filter_passthrough = getbits(&t->m_slice_di[slice_index],1);
		flushbits(slice_di,1);

		fieldlen = 13;

		flushbits(slice_di,fieldlen);

		loadbits(slice_di);

		if (t->m_pctszSize > 0){
			if (getbits(slice_di,t->m_pctszSize) != t->m_pctsz){
				RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
					"Unmatched PCTSZ"));
				return RV_S_ERROR;
			}
		}

		/* MBA */
		loadbits(slice_di);
		slice_di->m_mba = getbits(slice_di, t->m_mbaSize);
		*iSliceMBA	= slice_di->m_mba;
		flushbits(slice_di,1);//RTYPE		

	}else{

		/* The SSC either does not exist because this is the first slice */
		/* after the picture start code, or the SSC has already been read */
		/* in IAPass1. */

		/* Must be 1 to prevent start code emulation (SEPB1) */
		if (getbits1(slice_di) != 1){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - Start code emulation error"));
			return RV_S_ERROR;
		}

		/* No SSBI since CPM can not be "1". */

		/* Get macro-block address */
		slice_di->m_mba = *iSliceMBA = getbits(slice_di,t->m_mbaSize);

		loadbits(slice_di);

		if (t->m_mbaSize > 11){
			/* Must be 1 to prevent start code emulation (SEPB2) */
			if (getbits1(slice_di) != 1){
				RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - Start code emulation error"));
				return RV_S_ERROR;
			}
		}

		/* Get SQUANT */
		slice_di->m_pquant = (U8)getbits(slice_di,5);

		/* Must be 1 to prevent start code emulation (SEPB3) */
		if (getbits1(slice_di) != 1){
			RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - Start code emulation error"));
			return RV_S_ERROR;
		}

		/* Get GOB frame ID. */
		getbits(slice_di,2);

	}

	slice_di->m_entropyqp = slice_di->m_pquant;

	return RV_S_OK;

} /* CB_GetSliceHeader_RV8() */
