/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: envelope.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
 * 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM   
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc.   
 * All Rights Reserved.   
 *   
 * The contents of this file, and the files included with this file, 
 * are subject to the current version of the RealNetworks Community 
 * Source License (the "RCSL"), including Attachment G and any 
 * applicable attachments, all available at 
 * http://www.helixcommunity.org/content/rcsl.  You may also obtain 
 * the license terms directly from RealNetworks.  You may not use this 
 * file except in compliance with the RCSL and its Attachments. There 
 * are no redistribution rights for the source code of this 
 * file. Please see the applicable RCSL for the rights, obligations 
 * and limitations governing use of the contents of the file. 
 *   
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the 
 * portions it created. 
 *   
 * This file, and the files included with this file, is distributed 
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
 * ENJOYMENT OR NON-INFRINGEMENT. 
 *   
 * Technology Compatibility Kit Test Suite(s) Location:   
 * https://rarvcode-tck.helixcommunity.org   
 *   
 * Contributor(s):   
 *   
 * ***** END LICENSE BLOCK ***** */   

/*
 * Gecko2 stereo audio codec.
 * Developed by Ken Cooke (kenc@real.com)
 * August 2000
 *
 * Last modified by:	Jon Recker (jrecker@real.com)
 *						01/24/01
 *
 * Power envelope functions.
 */
#include "coder.h"

#ifdef _HUFFMAN_TABLES_
#include "power_table.h"
#define RA_TNI_10_PT\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12,\
	(  HuffmanTable)RA_TNI_power_table_12

// 50 Power tables corresponding to 50 power trees
const HuffmanTable RA_TNI_power_table[50] = { 
	(  HuffmanTable)RA_TNI_power_table_0,
	(  HuffmanTable)RA_TNI_power_table_1,
	(  HuffmanTable)RA_TNI_power_table_2,
	(  HuffmanTable)RA_TNI_power_table_3,
	(  HuffmanTable)RA_TNI_power_table_4,
	(  HuffmanTable)RA_TNI_power_table_5,
	(  HuffmanTable)RA_TNI_power_table_6,
	(  HuffmanTable)RA_TNI_power_table_7,
	(  HuffmanTable)RA_TNI_power_table_8,
	(  HuffmanTable)RA_TNI_power_table_9,
	(  HuffmanTable)RA_TNI_power_table_10,
	(  HuffmanTable)RA_TNI_power_table_11,
	(  HuffmanTable)RA_TNI_power_table_12,			/* 13 different tables */
	RA_TNI_10_PT,					/* 10 clones of the 13th table*/
	RA_TNI_10_PT,					/* 10 clones of the 13th table*/
	RA_TNI_10_PT,					/* 10 clones of the 13th table*/	
	(  HuffmanTable)RA_TNI_power_table_12,			/* 7 clones of the 13 table */	
	(  HuffmanTable)RA_TNI_power_table_12,
	(  HuffmanTable)RA_TNI_power_table_12,
	(  HuffmanTable)RA_TNI_power_table_12,
	(  HuffmanTable)RA_TNI_power_table_12,
	(  HuffmanTable)RA_TNI_power_table_12,
	(  HuffmanTable)RA_TNI_power_table_12
};

#define RA_TNI_10_BITCOUNT\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT,\
	RA_TNI_POWER_TABLE_12_BIT_COUNT




const short RA_TNI_power_bitcount_table[50] = {
	RA_TNI_POWER_TABLE_0_BIT_COUNT,
	RA_TNI_POWER_TABLE_1_BIT_COUNT,
	RA_TNI_POWER_TABLE_2_BIT_COUNT,
	RA_TNI_POWER_TABLE_3_BIT_COUNT,
	RA_TNI_POWER_TABLE_4_BIT_COUNT,
	RA_TNI_POWER_TABLE_5_BIT_COUNT,
	RA_TNI_POWER_TABLE_6_BIT_COUNT,
	RA_TNI_POWER_TABLE_7_BIT_COUNT,
	RA_TNI_POWER_TABLE_8_BIT_COUNT,
	RA_TNI_POWER_TABLE_9_BIT_COUNT,
	RA_TNI_POWER_TABLE_10_BIT_COUNT,
	RA_TNI_POWER_TABLE_11_BIT_COUNT,
	RA_TNI_POWER_TABLE_12_BIT_COUNT,	/* 13 different tables */
	RA_TNI_10_BITCOUNT,					/* 10 clones of the 13th table*/
	RA_TNI_10_BITCOUNT,					/* 10 clones of the 13th table*/
	RA_TNI_10_BITCOUNT,					/* 10 clones of the 13th table*/
	RA_TNI_POWER_TABLE_12_BIT_COUNT,	
	RA_TNI_POWER_TABLE_12_BIT_COUNT,	
	RA_TNI_POWER_TABLE_12_BIT_COUNT,	
	RA_TNI_POWER_TABLE_12_BIT_COUNT,	
	RA_TNI_POWER_TABLE_12_BIT_COUNT,	
	RA_TNI_POWER_TABLE_12_BIT_COUNT,	
	RA_TNI_POWER_TABLE_12_BIT_COUNT
};

#endif	//_HUFFMAN_TABLES_

/* coding params for rms[0] */
/* old settings
#define RMS0BITS	5
#define RMS0MIN	3
*/
#define RMS0BITS	6	/* bits for env[0] */
#define RMS0MIN		-6	/* arbitrary! */
#define CODE2RMS(i)	((i)+(RMS0MIN))

/*
 * Huffman decodes the power envelope.
 * - overwrites global env_nbits
 * - unpacks categorization code
 *
 * Continues unpacking from pkbuf (global which was initialized by RA_TNI_DecodeGainInfo() )
 * Each region has its own Huffman power tree
 * The bit stream contains the Huffman code for each region (all packed together) so
 *   RA_TNI_DecodeNextSymbol is passed a pointer to the appropriate tree (a HUFFNODE structure)
 *   It unpacks a bit at a time and traverses  Huffman tree to get the leaf value
 * env_nbits contains the total number of bits spent coding the envelope (sum up the lengths
 *   of each region's Huffman code)
 * The mysterious value 12 in there comes from the encoder, which uses differential coding with
 *   differences constrained to the range [-12, 11]
 *
 * Fixed-point changes:
 *  - none
 */
int RA_TNI_DecodeEnvelope(short *rms_index, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	int r, nbits;
	/* int rprime; */
	short code;
	short cregions;
	short interlvRegns;

	/* unpack first index */
	nbits = RA_TNI_Unpackbits(RMS0BITS, (USHORT *)&code, pkbit, pktotal, bitstrmPtr);
	rms_index[0] = CODE2RMS(code);

	/* check for escape code */
	ASSERT(rms_index[0] != 0);

	cregions = bitstrmPtr->nregions + bitstrmPtr->cplstart;
#if 0
	for (r = 1; r < cregions; r++) {

		/* for interleaved regions, choose a reasonable table */
		if (r < 2 * bitstrmPtr->cplstart) {
			rprime = (r >> 1);
			if (!rprime) rprime = 1;
		} else {
			rprime = r - bitstrmPtr->cplstart;
		}

#ifdef _HUFFMAN_TABLES_
		nbits += RA_TNI_DecodeNextSymbolWithTable( RA_TNI_power_table[rprime-1], RA_TNI_power_bitcount_table[rprime-1],
								 (USHORT *)&code, pkbit, pktotal, bitstrmPtr);
#else	// _HUFFMAN_TABLES_
#ifdef _PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[rprime-1], &PROFILE_power_tree[rprime-1],
								(USHORT *)&code, pkbit, pktotal, bitstrmPtr);

#else	//_PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[rprime-1], (USHORT *)&code, pkbit, pktotal, bitstrmPtr);
#endif	//_PROFILE_TREES_	
#endif	// _HUFFMAN_TABLES_
		rms_index[r] = rms_index[r-1] + (code-12);
	}
	
	
#else

	if(bitstrmPtr->cplstart) {

#ifdef _HUFFMAN_TABLES_
		nbits += RA_TNI_DecodeNextSymbolWithTable(RA_TNI_power_table[0],RA_TNI_power_bitcount_table[0], 
									(USHORT *)&code, pkbit, pktotal, bitstrmPtr);

#else	// _HUFFMAN_TABLES_

#ifdef _PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[0],&PROFILE_power_tree[0], (USHORT *)&code, pkbit, pktotal, bitstrmPtr);

#else	//_PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[0], (USHORT *)&code, pkbit, pktotal, bitstrmPtr);

#endif	//_PROFILE_TREES_	

#endif	// _HUFFMAN_TABLES_


		rms_index[1] = rms_index[0] + (code-12);
		interlvRegns = (cregions < 2*bitstrmPtr->cplstart) ? cregions :
							2*bitstrmPtr->cplstart;
		for(r = 2; r < interlvRegns; r++) {

#ifdef _HUFFMAN_TABLES_
		nbits += RA_TNI_DecodeNextSymbolWithTable(RA_TNI_power_table[(r>>1)-1],RA_TNI_power_bitcount_table[(r>>1)-1], 
									(USHORT *)&code, pkbit, pktotal, bitstrmPtr);

#else	// _HUFFMAN_TABLES_

#ifdef _PROFILE_TREES_
			nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[(r>>1)-1], &PROFILE_power_tree[(r>>1)-1],
										(USHORT *)&code, 
									   pkbit, pktotal, bitstrmPtr);

#else	//_PROFILE_TREES_
			nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[(r>>1)-1], (USHORT *)&code, 
									   pkbit, pktotal, bitstrmPtr);

#endif	//_PROFILE_TREES_	
#endif	// _HUFFMAN_TABLES_
						
			rms_index[r] = rms_index[r-1] + (code-12);
		}
	}
	else {
		interlvRegns = 1;
	}	

	for(r = interlvRegns; r < cregions; r++) {
#ifdef _HUFFMAN_TABLES_
		nbits += RA_TNI_DecodeNextSymbolWithTable(RA_TNI_power_table[(r-bitstrmPtr->cplstart)-1],RA_TNI_power_bitcount_table[(r-bitstrmPtr->cplstart)-1], 
									(USHORT *)&code, pkbit, pktotal, bitstrmPtr);

#else	// _HUFFMAN_TABLES_

#ifdef _PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[(r-bitstrmPtr->cplstart)-1], &PROFILE_power_tree[(r-bitstrmPtr->cplstart)-1],
							   (USHORT *)&code,pkbit, pktotal, bitstrmPtr);

#else	//_PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_power_tree[(r-bitstrmPtr->cplstart)-1],
							   (USHORT *)&code,pkbit, pktotal, bitstrmPtr);

#endif	//_PROFILE_TREES_	
#endif	// _HUFFMAN_TABLES_
	
					
		rms_index[r] = rms_index[r-1] + (code-12);
	}
#endif

	return nbits;
}

