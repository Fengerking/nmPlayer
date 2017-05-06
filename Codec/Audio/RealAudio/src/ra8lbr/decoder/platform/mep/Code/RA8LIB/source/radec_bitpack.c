/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: radec_bitpack.c,v 1.1 2005/04/05 21:22:07 hfrederickson Exp $ 
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

/**************************************************************************** */
/*    Socrates Software Ltd : Toshiba Group Company */
/*    DESCRIPTION OF CHANGES: */
/*   
/*		1. Data overlay performed
/*		2. Debug trace features added
/* 
 *    CONTRIBUTORS : Vinayak Bhat,Deepak,Sudeendra,Naveen
 *   */
/**************************************************************************** */

#include "radec_defines.h"
#include "radec_assembly.h"

#ifdef RA8_DEBUG
extern struct SRA8_Debug sRA8_Debug;
#endif

#ifdef __DMA_AUDIOTAB__
#pragma section const .audiotabs
#endif
const unsigned char pkkey[4] =	{ 0x37, 0xc5, 0x11, 0xf2 };
#ifdef __DMA_AUDIOTAB__
#pragma section const
#endif
/**************************************************************************************
 * Function:    GetBits
 *
 * Description: get bits from bitstream, optionally advance bitstream pointer
 *
 * Inputs:      pointer to initialized BitStreamInfo struct
 *              number of bits to get from bitstream
 *              flag to indicate whether to advance bitstream pointer or not
 *
 * Outputs:     updated bitstream info struct (if advanceFlag set)
 *
 * Return:      the next nBits bits of data from bitstream buffer (right-justified)
 *
 * Notes:       nBits must be in range [0, 31], nBits outside this range masked by 0x1f
 *              for speed, does not indicate error if you overrun bit buffer 
 *              if nBits = 0, returns 0
 *              applies XOR key when reading (rather than using out-of-place buffer)
 *              reads byte-at-a-time (not cached 32-bit ints) so not designed to be
 *                especially fast (i.e. handy for Huffman decoding of a few symbols, 
 *                but not optimal for decoding long sequences of codewords)
 **************************************************************************************/
unsigned int GetBits(SBitStreamInfo *bsi, int nBits, int advanceFlag)
{
	int readBits, off, key, nBytes;
	unsigned int data;
	unsigned char *buf;

	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/

	

	nBits =nBits & 0x1f;
	if (!nBits)
		return 0;

	buf = bsi->buf;
	off = bsi->off;
	key = bsi->key;

	/* first, partial byte */
	data = ((unsigned int)(*buf++ ^ pkkey[key++])) << (24 + off);
	key =key & 0x03;
	readBits = 8 + (-off);

	/* whole bytes */
	while (readBits < nBits && readBits <= 24) {
		data =data | ((unsigned int)(*buf++ ^ pkkey[key++])) << (24 - readBits);
		key =key & 0x03;
		readBits =readBits + 8;
	}

	/* final, partial byte (no need to update local key and readBits) */
	if (readBits < nBits)
		data =data |  ((unsigned int)(*buf++ ^ pkkey[key])) >> (readBits - 24);

	if (advanceFlag) {
		nBytes = (bsi->off + nBits) >> 3;
		bsi->buf =bsi->buf+ nBytes;
		bsi->off = (bsi->off + nBits) & 0x07;
		bsi->key = (bsi->key + nBytes) & 0x03;
	}

    data = (data >> (32 + (-nBits)));
	return data;
}

/**************************************************************************************
 * Function:    AdvanceBitstream
 *
 * Description: move bitstream pointer ahead 
 *
 * Inputs:      pointer to initialized BitStreamInfo struct
 *              number of bits to advance bitstream
 *
 * Outputs:     updated bitstream info struct
 *
 * Return:      none
 *
 * Notes:       generally use following a GetBits(bsi, maxBits, 0) (i.e. no advance)
 **************************************************************************************/
void AdvanceBitstream(SBitStreamInfo *bsi, int nBits)
{
	int nBytes;

	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/


	nBytes = (bsi->off + nBits) >> 3;
	bsi->buf =bsi->buf+ nBytes;
	bsi->off = (bsi->off + nBits) & 0x07;
	bsi->key = (bsi->key + nBytes) & 0x03;
}

/**************************************************************************************
 * Function:    DecodeSideInfo
 *
 * Description: parse bitstream and decode gain info, coupling info, power envelope, 
 *                and categorization code
 *
 * Inputs:      pointer to initialized Gecko2Info struct
 *              pointer to bitstream buffer (byte-aligned)
 *              number of bits available in buf
 *              channel index
 *
 * Outputs:     filled-in dgainc structs, channel coupling indices (if joint stereo),
 *                power envelope index for each region, 
 *                and rate code (selected categorization)
 *
 * Return:      number of bits remaining in bitstream, -1 if out-of-bits
 * 
 * Notes:       encoder guarantees that gain, couple, envelope, and rateCode do 
 *                not run out of bits
 **************************************************************************************/
int DecodeSideInfo(Gecko2Info *gi,SBitStreamInfo *bsi, unsigned char *buf, int availbits, int ch)
{

	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	*****************************************/
	/* init bitstream reader */
	int i;
	bsi->buf = buf; /*gi->sHdInfo.inBuf; */
	bsi->off = 0;
	bsi->key = 0;

	/* decode gain info */
	availbits = DecodeGainInfo(gi,bsi, &(gi->sGainC[ch][1]), availbits);
#ifdef RA8_DEBUG
	sRA8_Debug.iNats[ch] = gi->sGainC[ch][1].nats;
	for(i = 0;i < gi->sGainC[ch][1].nats;i++)
	{
		sRA8_Debug.iLocNats[ch][i]=gi->sGainC[ch][1].loc[i];
		sRA8_Debug.iGainAttck[ch][i]=gi->sGainC[ch][1].gain[i];
	}
#endif
	if (availbits < 0)
	{
		if(availbits == -1)
			return -1;
		if(availbits == -2)
			return -2;
	}
	if (availbits == 0)
		return -1;
	/* replicate gain control and decode coupling info, if joint stereo */
	if (gi->sHdInfo.jointStereo) 
    {	    
		CopyGainInfo(&gi->sGainC[1][1], &gi->sGainC[0][1]);
		availbits = DecodeCoupleInfo(gi,bsi, availbits);
		if (availbits <= 0)
		{
			return -1;
		}
	}
	/* decode power envelope (return error if runs out of bits) */
	availbits = DecodeEnvelope(gi,bsi, availbits, ch);
	/* decode rate code (return error if runs out of bits) */
	if (availbits < gi->sHdInfo.rateBits)
	{
		return -1;
	}
	gi->sBufferInfo.rateCode = GetBits(bsi, gi->sHdInfo.rateBits, 1);	
	availbits =availbits+ (-gi->sHdInfo.rateBits);
#ifdef RA8_DEBUG
	sRA8_Debug.iRateCode[ch]=gi->sBufferInfo.rateCode;
#endif
	return availbits;
}
