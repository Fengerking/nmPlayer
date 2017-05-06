/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: couple.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 * Stereo coupling functions.
 */
#include "coder.h"
#include "assembly.h"
#ifdef _HUFFMAN_TABLES_
#include "cpl_tables.h"
#endif

#pragma DATA_SECTION( RA_TNI_cplband, "cplband_section")
#pragma DATA_ALIGN( RA_TNI_cplband,2) 
/* coupling band widths, based on Zwicker critical-bandwidth */
const short RA_TNI_cplband[MAXREGNS] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
	11, 11,
	12, 12,
	13, 13,
	14, 14, 14,
	15, 15, 15, 15,
	16, 16, 16, 16, 16,
	17, 17, 17, 17, 17, 17,
	18, 18, 18, 18, 18, 18, 18,
	19, 19, 19, 19, 19, 19, 19, 19, 19
};


#pragma DATA_SECTION ( RA_TNI_fixcplscale2, "cplscale_section")
#pragma DATA_ALIGN	( RA_TNI_fixcplscale2, 2)
/* Lookup tables for reconstructed scaling factors (1.31 signed) */
const long RA_TNI_fixcplscale2[3] = {
	0x79fc945f, 0x5a82799a, 0x26c59af8
};

#pragma DATA_SECTION ( RA_TNI_fixcplscale3, "cplscale_section")
#pragma DATA_ALIGN	( RA_TNI_fixcplscale3, 2)
const long RA_TNI_fixcplscale3[7] = {
	0x7d9a93a4, 0x77ef8978, 0x701e9f27, 0x5a82799a, 0x3dc04491,
	0x2cb7094c, 0x18a6b4ed
};

#pragma DATA_SECTION ( RA_TNI_fixcplscale4, "cplscale_section")
#pragma DATA_ALIGN	( RA_TNI_fixcplscale4, 2)
const long RA_TNI_fixcplscale4[15] = {
	0x7ee90962, 0x7c936c4f, 0x79fc9465, 0x770e9d8f, 0x73a45b28,
	0x6f749c4e, 0x69c04836, 0x5a82799a, 0x481dac45, 0x3ef11955,
	0x36df3415, 0x2f011d16, 0x26c59ae8, 0x1d688afa, 0x10aaa6fd
};

#pragma DATA_SECTION ( RA_TNI_fixcplscale5, "cplscale_section")
#pragma DATA_ALIGN	( RA_TNI_fixcplscale5, 2)
const long RA_TNI_fixcplscale5[31] = {
	0x7f7a8410, 0x7e66f9cf, 0x7d46e886, 0x7c18c36e, 0x7adaa745,
	0x798a3db9, 0x78249362, 0x76a5d7e1, 0x7508f83e, 0x7346f452,
	0x7155abbc, 0x6f257fbd, 0x6c9c00f0, 0x6985577e, 0x655acbf3,
	0x5a82799a, 0x4e2ca29a, 0x4873cee8, 0x43bc1a9b, 0x3f7c6426,
	0x3b7ddcf1, 0x37a26159, 0x33d59802, 0x30072e43, 0x2c27c13f,
	0x28266a5d, 0x23edbab6, 0x1f5e6927, 0x1a42fec7, 0x14294028,
	0x0b8ab0db
};

#pragma DATA_SECTION ( RA_TNI_fixcplscale6, "cplscale_section")
#pragma DATA_ALIGN	( RA_TNI_fixcplscale6, 2)
const long RA_TNI_fixcplscale6[63] = {
	0x7fbea8be, 0x7f39f9ab, 0x7eb28121, 0x7e281805, 0x7d9a93a8,
	0x7d09c543, 0x7c75796f, 0x7bdd7778, 0x7b418090, 0x7aa14ee1,
	0x79fc9466, 0x7952f983, 0x78a41b51, 0x77ef897b, 0x7734c38d,
	0x76733593, 0x75aa33b1, 0x74d8f46c, 0x73fe8908, 0x7319d333,
	0x722976ad, 0x712bc4eb, 0x701e9f29, 0x6eff48fd, 0x6dca2025,
	0x6c7a2377, 0x6b081945, 0x6968e250, 0x6789b6cc, 0x6545d976,
	0x623e1923, 0x5a82799a, 0x520d1a91, 0x4e47c3aa, 0x4b4243b5,
	0x489d2fc3, 0x46338b6a, 0x43f2498b, 0x41cdfe96, 0x3fbf1c82,
	0x3dc0448d, 0x3bcd6c20, 0x39e361f6, 0x37ff83c3, 0x361f8e06,
	0x34417ac7, 0x326368ee, 0x3083887b, 0x2ea0091f, 0x2cb70944,
	0x2ac683d0, 0x28cc3a99, 0x26c59ae4, 0x24af97f2, 0x22867502,
	0x20457311, 0x1de64c41, 0x1b604f6f, 0x18a6b4dc, 0x15a521e0,
	0x1237205d, 0x0e0d0479, 0x08144f82
};

#pragma DATA_SECTION ( RA_TNI_fixcplscale_tab, "cplscale_section")
#pragma DATA_ALIGN	( RA_TNI_fixcplscale_tab, 2)
const long *const RA_TNI_fixcplscale_tab[7] = {
	(const long *)0,	/* not used */
	(const long *)0,	/* not used */
	RA_TNI_fixcplscale2,
	RA_TNI_fixcplscale3,
	RA_TNI_fixcplscale4,
	RA_TNI_fixcplscale5,
	RA_TNI_fixcplscale6
};

/* decodes coupling data from bitstream
 * data was either Huffman coded or stored directly, whichever is more efficient
 * the hufmode flag (one bit) indicates which format was used
 *
 * Fixed-point changes:
 *  - none
 */
#ifndef	DECODECOUPLEINFO_ASM
int RA_TNI_DecodeCoupleInfo(unsigned short *cplindex, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	short bandstart = RA_TNI_cplband[bitstrmPtr->cplstart];
	short bandend   = RA_TNI_cplband[bitstrmPtr->nregions-1];
	int b, nbits;
	USHORT hufmode;

	nbits = RA_TNI_Unpackbit(&hufmode, pkbit, pktotal, bitstrmPtr);

	if (hufmode) {
		/* HUFFMAN */
		for (b = bandstart; b <= bandend; b++) {
#ifdef _HUFFMAN_TABLES_
		nbits += RA_TNI_DecodeNextSymbolWithTable( RA_TNI_cpl_table_tab[bitstrmPtr->cplqbits], RA_TNI_cpl_bitcount_tab[bitstrmPtr->cplqbits],
									&cplindex[b], pkbit, pktotal, bitstrmPtr);
#else //_HUFFMAN_TABLES_			

#ifdef _PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_cpl_tree_tab[bitstrmPtr->cplqbits], PROFILE_cpl_tree_tab[bitstrmPtr->cplqbits],
															&cplindex[b], pkbit, pktotal, bitstrmPtr);
#else	//_PROFILE_TREES_
		nbits += RA_TNI_DecodeNextSymbol(RA_TNI_cpl_tree_tab[bitstrmPtr->cplqbits], &cplindex[b], pkbit, pktotal, bitstrmPtr);

#endif	//_PROFILE_TREES_	
#endif  //_HUFFMAN_TABLES_
		}
	} else {
		/* DIRECT */
		for (b = bandstart; b <= bandend; b++) {
			nbits += RA_TNI_Unpackbits(bitstrmPtr->cplqbits, &cplindex[b], pkbit, pktotal, bitstrmPtr);
		}
	}

	return nbits;
}
#endif	//DECODECOUPLEINFO_ASM


/* decodes the jointly-coded MLT
 * first come the non-coupled regions, interleaved L-R-L-R
 * then come the coupled bins, plus derived scale factors indicating the power of each channel
 *
 * Fixed-point changes:
 *  - buffers made int
 *  - cplscale tables and multiplies made fixed-point
 */
//const short RA_TNI_mltincr[NBINS] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,NBINS};

void RA_TNI_JointDecodeMLT(short availbits, long *mltleft, 
						   long *mltrght, short *pkbit, 
						   short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	long scaleleft, scalerght;
	USHORT cplindex[NCPLBANDS];
	int i, r, q;
	int cplquant, cplstart, nregions, cplqbits;
	long *mltl, *mltcouprght;
	const long *fixcplscale;

	// [fj] : 06/02/2003 10:35
 	// reuse the decpcm buffer
 	mltcouprght = bitstrmPtr->decpcm ;

	//[fj] 06/02/2003 10:35
	/*mltcouprght = */mltl = mltleft;
	
	/* decode the coupling information */
	availbits -= RA_TNI_DecodeCoupleInfo((USHORT *)cplindex, pkbit, pktotal, bitstrmPtr);

	/* replace mlt with mltleft -- Venky */
	RA_TNI_DecodeMLT(availbits, mltcouprght, pkbit, pktotal, bitstrmPtr);

//[fj]09/02/2003 11:37 : Added this code
#ifdef CONST_TABLE_DMA
  	// page in the cos4 and sin4 tables
	ACPY2_start( bitstrmPtr->dmaChannel_0, 
				 IDMA2_ADRPTR(bitstrmPtr->fixcos4_sin4tab_extmem),
		 		 IDMA2_ADRPTR( bitstrmPtr->fixcos4tab),
				 bitstrmPtr->fixcos4sin4size); 												
#endif 

	cplstart = bitstrmPtr->cplstart;
	nregions = bitstrmPtr->nregions;
	cplqbits = bitstrmPtr->cplqbits;
	
	fixcplscale = &RA_TNI_fixcplscale_tab[cplqbits][0];

	
	//[fj] 14/02/2003
	//Removing the newmlt buffers

	/* de-interleave the coupled regions */
	for(r = cplstart; r; r--) {
		mltcouprght += NBINS;
		for(i = NBINS; i; i--) {
			*mltl++    = *(mltcouprght - NBINS);
			*mltrght++ = *mltcouprght++;
		}
	}

	cplquant = (1 << cplqbits) - 2;	/* quant levels */

	/* reconstruct the stereo channels */
	for (r = cplstart; r < nregions; r++) {

		/*
		 * dequantize the expanded coupling ratio
		 * expand = (q - (cplquant>>1)) * (2.0f/cplquant);
		 *
		 * square-law compression
		 * ratio = sqrt(fabs(expand));
		 * if (expand < 0.0f) ratio = -ratio;
		 *
		 * reconstruct the scaling factors
		 * scaleleft = sqrt(0.5f - 0.5f * ratio);
		 * scalerght = sqrt(0.5f + 0.5f * ratio);
		 */
		q = cplindex[RA_TNI_cplband[r]];

		scaleleft = *(fixcplscale + q);
		scalerght = *(fixcplscale + cplquant - q);

		/* scale by 1.31 signed numbers, and eat extra sign bit */
		for (i = NBINS; i; i--) {
#ifndef __OPTIMISE_FIRST_PASS__
			*mltrght++ = RA_TNI_MULSHIFT1(scalerght, *mltcouprght);
			*mltl++    = RA_TNI_MULSHIFT1(scaleleft, *mltcouprght++);
#else
			*mltrght++ = RA_TNI_MULSHIFT1(&scalerght, mltcouprght);
			*mltl++    = RA_TNI_MULSHIFT1(&scaleleft, mltcouprght++);
#endif
		}
	}

	/* set non-coded regions to zero */
	for (i = nregions * NBINS; i < bitstrmPtr->nsamples; i++) {
		*mltl++ = 0;
		*mltrght++ = 0;
	}
}
