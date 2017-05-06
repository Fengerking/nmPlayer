/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: radec_huffman.c,v 1.1 2005/04/05 21:22:07 hfrederickson Exp $ 
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

#include "radec_defines.h"



/**************************************************************************************
 * Function:    DecodeHuffmanScalar
 *
 * Description: decode one Huffman symbol from bitstream
 *
 * Inputs:      pointers to table and info
 *              right-aligned bit buffer with MAX_HUFF_BITS bits
 *              pointer to receive decoded symbol
 *
 * Outputs:     decoded symbol
 *
 * Return:      number of bits in symbol
 **************************************************************************************/
int DecodeHuffmanScalar(const unsigned short *huffTab, const SHuffInfo *huffTabInfo, int bitBuf, int *val,unsigned char *FuncDetect)
{
	const unsigned char *countPtr;
    unsigned int cache, total, count, t;
	const unsigned short *map;

	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/
	map = huffTab + huffTabInfo->offset;
    countPtr = huffTabInfo->count;
	cache = (unsigned int)(bitBuf << (17 - MAX_HUFF_BITS));	
	total = 0;
	count = *countPtr++;
	t = count << 16;
	while (cache >= t) 
    {
		cache -=  t;
		cache <<= 1;
		total += count;
		count = *countPtr++;
		t = count << 16;
	}
	if(*FuncDetect == 0)
    { 
		if((total + (cache >> 16) + huffTabInfo->offset) > 118)
			return -1;
    }  
    else
	{ 
		if((total + (cache >> 16) + huffTabInfo->offset) > 311)
			return -1;
    }
	*val = map[total + (cache >> 16)];
	/* return number of bits in symbol */
	return (countPtr - huffTabInfo->count);
}
