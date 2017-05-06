/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: radec_sqvh.c,v 1.1 2005/04/05 21:22:07 hfrederickson Exp $ 
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
/*		1. Optimization achieved by usage of Intrinsic functions
/*		2. Data overlay performed
/*		3. Some C optimizations performed to suit platform 
/*
/*
 *    CONTRIBUTORS : Vinayak Bhat,Deepak,Sudeendra,Naveen
 *   */
/**************************************************************************** */


#include "radec_defines.h"
#include "radec_assembly.h"


extern int *dmem_rmsIndex;
extern int *dmem_catbuf;

#ifdef __DMA_AUDIOTAB__
#pragma section const .audiotabs
#endif
static const int vpr_tab[7] = {10, 10, 10, 5, 5, 4, 4};	/* number of vectors in one region (20 bins) */
#ifdef __DMA_AUDIOTAB__
#pragma section const 
#endif
/**************************************************************************************
 * Function:    DecodeHuffmanVectors
 *
 * Description: decode vector Huffman symbols from bitstream for one region
 *
 * Inputs:      pointer to initialized BitStreamInfo struct
 *              cat for this region
 *              buffer to receive decoded symbols
 *              number of bits remaining in bitstream
 *
 * Outputs:     NBINS decoded symbols
 *
 * Return:      number of bits left in bitstream, -1 if ran out of bits
 *
 * Notes:       the alphabet of decoded symbols has been remapped from the
 *                floating-pt reference, to replace the Horner-method polynomial 
 *                evaluation with shift-and-mask (i.e. change of basis to replace
 *                1/(kmax+1) with 1/(2^n) for unpacking vectorized codes)
 *              uses byte-cache Huffman decoder and conditional logic which is 
 *                ARM-friendly
 *              this has been carefully arranged to compile well with ADS, so if you 
 *                change anything, make sure you study the assembly code carefully!
 *              consider fusing with loop over all nRegions (rather than function call
 *                per loop) to avoid refilling cache each call
 **************************************************************************************/
int DecodeHuffmanVectors(SBitStreamInfo *bsi, int cat, int *k, int bitsLeft, unsigned char *ErrParam)
{
	int nBits, startBits, vpr, v, vPacked;
	int  key, cachedBits, padBits;
	unsigned int cache,count, t, total, cw;
	unsigned char *buf, *countCurr;
	const unsigned char *countBase;
	const unsigned short *map;


	buf = bsi->buf;
	key = bsi->key;
                    
	countBase = huffTabVectorInfo[cat].count;
	map = huffTabVector + huffTabVectorInfo[cat].offset;
	startBits = bitsLeft;
	vpr = vpr_tab[cat];

	/* initially fill cache with any partial byte */
	/*Added by Sudhi.. Keep the value of key <= 3*/
    key &= 0x03;
	cache = 0;
	cachedBits = (8 - bsi->off) & 0x07;
	if (cachedBits)
		cache = (unsigned int)((*buf++) ^ pkkey[key++]) << (32 + (-cachedBits));
	key =key & 0x03;
	bitsLeft =bitsLeft+ (-cachedBits); 

	padBits = 0;
	while (vpr > 0) 
    {
		/* refill cache - assumes cachedBits <= 24 */
		if (bitsLeft >= 8) 
        {
			/* load 1 new byte into left-justified cache */
			cache =cache | (unsigned int)((*buf++) ^ pkkey[key++]) << (24 + (-cachedBits));	
			key =key & 0x03;
			cachedBits =cachedBits+ 8;
			bitsLeft =bitsLeft+ (-8);
		} else 
        {
			/* last time through, pad cache with zeros and drain cache */
			if (cachedBits + bitsLeft <= 0)	
				return -1;
			if (bitsLeft > 0)	cache =cache | (unsigned int)((*buf++) ^ pkkey[key++]) << (24 - cachedBits);	
			key=key & 0x03;
			cachedBits =cachedBits + bitsLeft;
			bitsLeft = 0;

			cache =cache & (signed int)0x80000000 >> (cachedBits - 1);
			padBits = 21;
			cachedBits =cachedBits + padBits;	/* okay if this is > 32 (0's automatically shifted in from right) */
		}

		/* max bits per Huffman symbol = 16, max sign bits = 5, so 21 bits will do */
		while (vpr > 0 && cachedBits >= 21) 
        {
			/* left-justify to bit 16 */
			cw = cache >> 15;
			countCurr = (unsigned char *)countBase;
			count=*countCurr++;
			t = count << 16;
			total = 0;

			while (cw >= t) 
            {
				cw =cw + (- t);
				cw = cw <<1;
				total = total +count;
				count=*countCurr++;
				t = count << 16 ;
			}
			nBits = countCurr - countBase;
			cachedBits =cachedBits +(- nBits);
			cache =cache << nBits;

			/*Added By Deepak For Error Handling*/
			if((huffTabVectorInfo[cat].offset + total + (cw >> 16)) > 1275)
            {
				*ErrParam = 1;
				return 0;
            } 

			vPacked = (int)map[total + (cw >> 16)];
			
			/* sign bits */
			nBits = (vPacked >> 12) & 0x0f;
			cachedBits = cachedBits +(-nBits);
			if (cachedBits < padBits)
				return -1;

			
			if (cat < 3) 
            {
				v = (vPacked >> 0) & 0x0f;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*k = v;
				v = (vPacked >> 4) & 0x0f;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+1) = v;
				k=k+2;			
				} 
            else if (cat < 5) 
            {
				v = (vPacked >> 0) & 0x07;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*k = v;
				v = (vPacked >> 3) & 0x07;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+1) = v;
				v = (vPacked >> 6) & 0x07;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+2) = v;
				v = (vPacked >> 9) & 0x07;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+3) = v;
				k=k+4;
				}
            else 
            {
				v = (vPacked >> 0) & 0x03;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*k = v;
				v = (vPacked >> 2) & 0x03;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+1) = v;
				v = (vPacked >> 4) & 0x03;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+2) = v;
				v = (vPacked >> 6) & 0x03;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+3) = v;
				v = (vPacked >> 8) & 0x03;	if (v)	{v |= (cache & 0x80000000); cache <<= 1;}	*(k+4) = v;
				k=k+5;
				}
			vpr--;
		}
	}

	return startBits - bitsLeft - (cachedBits - padBits);
}

/* Q28 - first column (0's) are dither 
 *   rows ..... cat = [0, 7]
 *   columns .. k =   [0, kmax_tab[cat]]
 *   qcOffset[cat] is starting index for k = 0
 * 
 * qcTab[0] scaled by 1.0
 * qcTab[1] scaled by sqrt(2)
 *
 * lower cat = shorter vectors = finer Q = higher expected bits
 * higher cat = longer vectors = coarser Q = lower expected bits
 *
 * kmax_tab[8] = { 13,  9,  6,  4,  3,  2,  1,  0 };
 */
#ifdef __DMA_AUDIOTAB__
#pragma section const .audiotabs
#endif
static const int qcTab[2][14 + 10 + 7 + 5 + 4 + 3 + 2 + 1] = {
	{
	/* quant_centroid_tab * 1.0 */
	0x00000000, 0x0645a1c8, 0x0c2d0e50, 0x11eb8520, 0x17a1cac0, 0x1d4fdf40, 0x22ed9180, 0x28a7ef80, 0x2e49ba40, 0x33eb8500, 0x39916880, 0x3f126e80, 0x449ba600, 0x4b958100, 
	0x00000000, 0x08b43960, 0x10f5c280, 0x19020c40, 0x21168740, 0x2922d100, 0x3126e980, 0x38fdf3c0, 0x411eb880, 0x49eb8500, 
	0x00000000, 0x0bef9db0, 0x176c8b40, 0x22e147c0, 0x2e1cac00, 0x39581080, 0x450e5600, 
	0x00000000, 0x10189380, 0x20000000, 0x2fe35400, 0x3fc28f40, 
	0x00000000, 0x1522d0e0, 0x2b3f7d00, 0x3fba5e40, 
	0x02d413cc, 0x1a831260, 0x37db22c0, 
	0x04000000, 0x1f6c8b40, 
	0x0b504f33
	},
	{
	/* quant_centroid_tab * sqrt(2) */
	0x00000000, 0x08deb4dc, 0x11382ec8, 0x1957bba7, 0x216bb2a8, 0x297413e1, 0x31654988, 0x397f0b29, 0x41760b9f, 0x496d0c14, 0x5169d7b1, 0x593280ab, 0x6106bff4, 0x6ae454b2, 
	0x00000000, 0x0c4f2f4d, 0x17fc2cf0, 0x235ddce6, 0x2ecb22d2, 0x3a2cd2c9, 0x4582ecca, 0x50994ee6, 0x5c17f595, 0x6889e5e1, 
	0x00000000, 0x10e14b25, 0x212064ce, 0x3153e8c5, 0x413653ba, 0x5118bf09, 0x61a8f143, 
	0x00000000, 0x16c35fec, 0x2d413ccc, 0x43b94edf, 0x5a2b95ca, 
	0x00000000, 0x1de40c9b, 0x3d2972e9, 0x5a20002f, 
	0x04000000, 0x257e5e73, 0x4efe081d, 
	0x05a82799, 0x2c70b401, 
	0x10000000
	},
};

static const int qcOffset[8] = {  0, 14, 24, 31, 36, 40, 43, 45 };

#ifdef __DMA_AUDIOTAB__
#pragma section const
#endif
/**************************************************************************************
 * Function:    ScalarDequant
 *
 * Description: dequantize transform coefficients (in-place)
 *
 * Inputs:      buffer of decoded/un-vectorized symbols with sign in bit 31
 *              cat (quantizer) for this region
 *              pointer to dequantizer table (either scaled by 1.0 or sqrt(2.0))
 *              right-shift amount (i.e. 2^-n for normalizing to correct Q format)
 *              pointer to current contents of dither lfsr
 *              current guard bit mask
 *
 * Outputs:     NBINS dequantized coefficients
 *              updated dither generator
 *
 * Return:      updated guard bit mask
 *
 * Notes:       assumes that shift is in range [0, 31]
 **************************************************************************************/
static int ScalarDequant(int *buf, int cat, const int *dqTab, int shift, int *lfsrInit, int gbMask)
{
	int i, k;
	int dq, sign, lfsr;

	
	lfsr = *lfsrInit;

	/* multiply dequantized value (from centroid table) or the dither value (if quantized coeff == 0)
	 *   with the power in this region (from envelope)
	 *
	 * implements:
	 *   buf[i] = quant_centroid_tab[cat][buf[i]] * pow(2, DQ_FRACBITS_OUT + scale / 2.0)
	 *
	 * example (for testing with Q28 qcTab):
	 *   fixdeqnt = (int)((double)qcTab[0][qcOffset[cat] + k] / (double)(1 << 28) * pow(2, DQ_FRACBITS_OUT + scale / 2.0)); 
	 *	 *buf++ = fixdeqnt * (sign ? -1 : 1);
	 * 
	 * output format = Q(FBITS_OUT_DQ) = Q12 (currently)
	 */
	for (i = NBINS; i != 0; i--) {
		k = *buf;
		if (k == 0 || cat == 7) { 
			/* update the LFSR, producing a random sign */
			sign = (lfsr >> 31);
			lfsr = (lfsr << 1) ^ (sign & FEEDBACK);
			dq = dqTab[0] >> shift;
			gbMask |= dq;
			*buf++ = (dq ^ sign) - sign;				/* inflict the sign */
		} else {
			/* for stepsize[cat]*buf[i], use centroid[cat][buf[i]] */
			sign = k >> 31;
			k &= 0x7fffffff;
			dq = dqTab[k] >> shift;
			gbMask |= dq;
			*buf++ = (dq ^ sign) - sign;				/* inflict the sign */
		}
	}
	*lfsrInit = lfsr;

	/* fast way to bound min guard bits is just to return shift (smallest shift = min number of GB's)
     * tighter bound is OR'ing all the dequantized values together before applying sign (done here)
	 */
	return gbMask;
}

/**************************************************************************************
 * Function:    DecodeTransform
 *
 * Description: recover MLT coefficients from scalar-quantized, vector Huffman codes
 *
 * Inputs:      pointer to initialized Gecko2Info struct
 *              buffer to receive dequantized coefficients
 *              number of bits remaining in bitstream
 *              pointer to current contents of dither lfsr
 *              channel index
 *
 * Outputs:     nRegions*NBINS dequantized coefficients per channel
 *              updated dither generator
 *
 * Return:      minimum number of guard bits in coefficients
 *
 * Notes:       for joint stereo, does in-place deinterleaving into left and right 
 *                halves of mlt buffer 
 *                (left starts at mlt[0], right starts at mlt[2*MAXNSAMP])
 **************************************************************************************/
int DecodeTransform(Gecko2Info *gi,SBitStreamInfo *bsi, int *mlt, int availbits, int *lfsrInit, int ch, unsigned char *ErrParam)
{
	int r, gbMask, gbMin, bitsUsed, shift, fbits, rmsMax;
	int *dest, *catbuf, *rmsIndex;
	const int *dqTab;
	
	int cntRgns = gi->sHdInfo.cRegions;

	int cntCplStart = gi->sHdInfo.cplStart;

#ifdef __DMA_SQVH__	
	catbuf   = dmem_catbuf;
	rmsIndex = dmem_rmsIndex;
	/*
	catbuf =   INTERNAL_CATBUFF;
 	rmsIndex = INTERNAL_RMSINDEX;
	*/
#else
	catbuf = gi->sDecBuf.catbuf;
	rmsIndex = gi->sDecBuf.rmsIndex;
#endif

/* huffman decode - does joint stereo deinterleaving if necessary */
	for (r = 0; r < cntRgns; r++) 
    {
		if (r < 2*cntCplStart)
			dest = mlt + NBINS*(r >> 1) + 2*MAXNSAMP*(r & 0x01);
		else
			dest = mlt + NBINS*(r - cntCplStart);

		if (catbuf[r] < 7) 
        {	
			/* cat == 7 means region was not coded, and dequantizer will just add power-normalized dither */
			bitsUsed = DecodeHuffmanVectors(bsi, catbuf[r], dest, availbits,ErrParam);
			if(*ErrParam == 1)
				return 0;
			if (bitsUsed < 0 || bitsUsed > availbits)
				break;
			AdvanceBitstream(bsi, bitsUsed);
			availbits -= bitsUsed;
		}
	}

	/* if ran out of bits, use dither in rest of regions (zero scalars) */
	for (     ; r < cntRgns; r++)
	{
		catbuf[r] = 7;

	}
	/* for very large coefficients, we override the default format with fewer fraction bits, to avoid saturation 
	 * range of rmsIndex = [-31, 63] (from encoder)
	 */
	fbits = FBITS_OUT_DQ;
	rmsMax = gi->sBufferInfo.rmsMax[ch];

	if ( (rmsMax >> 1) > (28 - fbits) ) {
		fbits = 28 - (rmsMax >> 1);
		if (fbits < 0)
			fbits = 0;
	}

	/* dequantize - stops at cRegions*NBINS (doesn't zero out above this) */
	gbMask = 0;
	for (r = 0; r < cntRgns; r++) {
		if (r < 2*cntCplStart)
			dest = mlt + NBINS*(r >> 1) + 2*MAXNSAMP*(r & 0x01);
		else
			dest = mlt + NBINS*(r - cntCplStart);

		/* qcTab = Q28 */
		dqTab = qcTab[rmsIndex[r] & 0x01] + qcOffset[catbuf[r] & 0x07];
		shift = (28 - fbits - (rmsIndex[r] >> 1));
		shift = MIN(shift, 31);	/* underflow to +/- 0 */
		shift = MAX(shift, 0);	/* saturate exponent, could only happen for gigantic rmsMax (never seen in practice) */

		gbMask = ScalarDequant(dest, catbuf[r], dqTab, shift, lfsrInit, gbMask);
	}

	/* save the number of extra fraction bits we needed, if any */
	gi->sBufferInfo.xbits[ch][1] = FBITS_OUT_DQ - fbits;

	/* calculate minimum number of guard bits in mlt[] */
	gbMin = CLZ(gbMask) - 1;
	if (gbMin < 0)
		gbMin = 0;
	/* mlt[] values are Q31 * 2^bExp (i.e. Q(31-bExp) = Q(DQ_FRACBITS_OUT)) */	
	return gbMin;
}
