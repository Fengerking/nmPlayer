/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: sqvh.c,v 1.1 2005/02/18 23:30:41 hfrederickson Exp $ 
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
 */
#include "coder.h"
#include "assembly.h"
#ifdef _HUFFMAN_TABLES_
#include "sqvh_tables.h"
#endif

/*
 * Unpack region MLT vectors from bitstream.
 * returns scalar index and sign for each MLT.
 *
 * TODO:
 * redo out-of-bits stuff!
 * put UnVectorize() in separate function?
 * redo all k[n*vd] crap!
 *
 * The encoder does vector Huffman coding of several bins at once
 * As bit rate goes up, we can code more, shorter vectors
 * As bit rate goes down, we must make the vectors longer to be efficient
 *
 * cat is the cat for this region
 * k will hold the quantized MLT coefficients
 * s will hold the signs for the coefficients
 *
 * Fixed-point changes:
 *  - none
 */
#ifndef DECODEVECTORS_NEW		// activate older version

int RA_TNI_UnpackSQVH(short cat, short *k, short *s, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	int n, i;
	int vd;
	short radix;
	short udiv;
	USHORT vindex, bit;
#if 1
	int iradix, vpr;
	long div;
#endif
	int  outOfBits = 0, kindex;

	vd = RA_TNI_vd_tab[cat];
	kindex = vd - 1;
	radix = RA_TNI_kmax_tab[cat];

#if 1
	vpr = RA_TNI_vpr_tab[cat];
	iradix = RA_TNI_iradix_tab[cat];
#endif

	for (n = RA_TNI_vpr_tab[cat]; n; n--) {

#ifdef _HUFFMAN_TABLES_
		if (!RA_TNI_DecodeNextSymbolWithTable( RA_TNI_sqvh_tab [cat], RA_TNI_sqvh_bitcount_tab[cat], 
													&vindex, pkbit, pktotal, bitstrmPtr) ) {
			vindex = 0;
			outOfBits = 1;
		}
#else //_HUFFMAN_TABLES_
#ifdef _PROFILE_TREES_
		if (!RA_TNI_DecodeNextSymbol(RA_TNI_sqvh_tree[cat], PROFILE_sqvh_tree[cat], &vindex, pkbit, pktotal, bitstrmPtr)) {
			vindex = 0;
			outOfBits = 1;
		}
#else	//_PROFILE_TREES_
		if (!RA_TNI_DecodeNextSymbol(RA_TNI_sqvh_tree[cat], &vindex, pkbit, pktotal, bitstrmPtr)) {
			vindex = 0;
			outOfBits = 1;
		}
#endif	//_PROFILE_TREES_	

#endif //_HUFFMAN_TABLES_

		/*
		 * Recover scalars from vector, LSD to MSD.
		 * XXX early exit when vindex == 0?
		 */
		k += kindex;
		
		if(!vindex) {
			for (i = vd; i; i--) {
				*k-- = 0;
			}
		}
		else {
			for (i = vd; i; i--) {
				/* div = vindex / radix; */
				udiv = vindex / radix;

				/* k = vindex % radix; */
				*k-- = vindex - (udiv * radix);
				vindex = udiv;
			}
		}

		/* get any trailing signbits, stored MSD to LSD */
		k++;
		for (i = vd; i; i--) {
			if (*k++) {	/* for each non-zero k, get sign */
				if (!RA_TNI_Unpackbit(&bit, pkbit, pktotal, bitstrmPtr)) {
					outOfBits = 1;
					*s++ = 0;
				} else
					*s++ = bit;
			} else
				*s++ = 0;
		}
	}
	return outOfBits;
}
#endif //DECODEVECTORS_NEW

/*
 * Random bit generator, using a 32-bit linear feedback shift register.
 * Primative polynomial is x^32 + x^7 + x^5 + x^3 + x^2 + x^1 + x^0.
 *
 * Update step:
 * sign = (lfsr >> 31);
 * lfsr = (lfsr << 1) ^ (sign & FEEDBACK);
 */
#define FEEDBACK ((1<<7)|(1<<5)|(1<<3)|(1<<2)|(1<<1)|(1<<0))
//static long _lfsr = 'k' | (long)'e'<<8 | (long)'n'<<16 | (long)'c'<<24;	/* well-chosen seed */

/* MLTBITS - 2 because the result of MUL64FAST(fixdeqnt, man) is in 2.30 format (1.31 * 1.31) */
#define UNDERCLIP	(MLTBITS - 2 - 31)
#define OVERCLIP	(MLTBITS - 2)

/*
 * Here we multiply two normalized numbers together and scale the result to
 *  fit in a given fixed-point format, saturating if necessary.
 *
 * cat = cat for the region
 * r = region number
 * rms_index[r] = rms power in region r
 * k = vector of quantized MLT coefficients in region r
 * s = sign bits for k
 * mlt = vector to put dequantized coefficients into (MLTBITS.32 - MLTBITS)
 *
 * Fixed-point changes:
 *  - we are passed rms_index (which we dequantize here) instead of rms_quant
 *  - sqrt(pow(2,n)) is implemented in fixed-point, but in a mantissa-exponent format
 *      because of the large range of values we must represent
 *  - use FIXRANDSIGN to choose + or - row in tables instead of multiplying
 *      fixdeqnt by +/- 1
 */
#ifndef DECODEVECTORS_NEW		// activate older version
void RA_TNI_ScalarDequant(short cat, short r, short *rms_index, short *k, short *s, long *mlt, RA_TNI_Obj *bitstrmPtr)
{
	int i;
	long fixdeqnt;
	short exp, exp2;
	long sign;
	ULONG man;
	const char *expindex;
	const long *manindex;

	/* sqrt(pow(2,n)) normalized as man*(2^exp), encoder clips rms_index[r] to [-31, 63] */
	man = FIXRP2MAN(rms_index[r]);	/* 1.31 (signed) */
//	man = RA_TNI_fixrootpow2man_new[rms_index[r]&0x01];

	/* exp = FIXRP2EXP(rms_index[r]);			integers, range = [-31, 32] */
	exp = -31 + ((rms_index[r] + 63) + 1) / 2; /* replaced FIXRP2EXP by -31 + ((i+63)+1)/2 -- Venky */

	expindex = &RA_TNI_fqct_exp[cat][0];
	manindex = &RA_TNI_fqct_man[cat][0];

	/* JR -
	 * MLTBITS = 22 (set in coder.h)
	 * From studying statistics of encoder, we decide that 22.10 is adequate to hold most MLT coefficients.
	 *   On rare case that coefficients are larger than this, we saturate.
	 * NOTE - this is a rather arbitrary setting, and can be changed if experimental results dictate
	 * Instead of true saturation to the maximum value representable by MLTBITS, we are actually
	 *	clipping the exponent. The error introduced with this approach is slightly larger than if
	 *	true saturation were used, but clipping the exponent is simpler to implement in fixed-point. It might
	 *	be worth rewriting this someday, but in practice exponent clipping almost never occurs (with 32 bits).
	 * As expected, clipping an arbitrary MLT coefficient leads to smooth harmonic distortion (e.g. windowed
	 *  sinusoid) in the region of the clipping. This is normally inaudible.
	 */
	for (i = 0; i < NBINS; s++,i++) {
		if (!*k++) {
			/* update the LFSR, producing a random sign */
			sign = (bitstrmPtr->_lfsr >> 31);
			bitstrmPtr->_lfsr = (bitstrmPtr->_lfsr << 1) ^ (sign & FEEDBACK);

			exp2 = exp;
			fixdeqnt = RA_TNI_fixdither_tab[cat];			/* 1.31 signed */
			fixdeqnt = (fixdeqnt ^ sign) - sign;	/* inflict the sign */
		}
		else {	/* for stepsize[cat]*k[i], use centroid[cat][k[i]] */
			sign = -*s;
			exp2 = exp + *(expindex + *(k-1));		/* exp(stepsize[cat]*k[i]) */
			fixdeqnt = *(manindex + *(k-1));			/* man(stepsize[cat]*k[i]) */
			fixdeqnt = (fixdeqnt ^ sign) - sign;	/* inflict the sign */
		}

		if (exp2 < UNDERCLIP) {		/* underflow towards zero (+ or -) */
/*			LogFormattedString("underflowed MLT coeff, exp2 = %d\n", exp2);
*/			exp2 = UNDERCLIP;
		}
		if (exp2 > OVERCLIP) {		/* saturate (clip the exponent) to avoid overflowing MLTBITS */
/*			LogFormattedString("saturated MLT coeff, exp2 = %d\n", exp2);
*/			exp2 = OVERCLIP;
		}
#ifndef __OPTIMISE_FIRST_PASS__	
		*mlt++  = (long)(RA_TNI_MULSHIFT0(fixdeqnt, man))>>(MLTBITS - 2 - exp2);
#else
		*mlt++  = (long)(RA_TNI_MULSHIFT0(&fixdeqnt, &man))>>(MLTBITS - 2 - exp2);
#endif

	}
	return;
}
#endif //DECODEVECTORS_NEW


/*
 * Recover MLT from SQVH vectors.
 *
 * Fixed-point changes:
 *  - Instead of passing *rms_quant (float) to ScalarDequant(), now we pass *rms_index (int).
 *      This is because we now dequantize the envelope in ScalarDequant() instead
 *      of in DequantEnvelope() (which is no longer in the code).
 *  - *mlt is now vector of ints instead of floats
 */
#ifndef DECODEVECTORS_NEW		// activate older version
void RA_TNI_DecodeVectors(short *catbuf, short *rms_index, long *mlt, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	short r, i, cat;
	short k[NBINS];
	short s[NBINS];
	short cregions;

	cregions = bitstrmPtr->nregions + bitstrmPtr->cplstart;

	for (r = 0; r < cregions; r++) {					/* now use cregions instead of nregions */

		cat = catbuf[r];

		if (cat < 7) {	/* cat==7 not coded */
			if(RA_TNI_UnpackSQVH(cat, (short *)k, (short *)s, pkbit, pktotal, bitstrmPtr)) { /* if out-of-bits */
				cat = 7;
				for (i = r; i < cregions; i++)			/* now use cregions instead of nregions */
					catbuf[i] = 7;	/* set cats to 7 */
			}
		}
		if (cat == 7) {
			/* maybe dequantizer should detect cat==7 instead? */
			for (i = 0; i < NBINS; i++)
				k[i] = s[i] = 0;	/* zero scalars */
		}
		RA_TNI_ScalarDequant(cat, r, rms_index, (short *)k, (short *)s, mlt, bitstrmPtr );
		mlt += NBINS;
	}

	/* set non-coded regions to zero */
	for (i = cregions * NBINS; i < bitstrmPtr->nsamples; i++)		/* now use cregions instead of nregions */
		*mlt++ = 0;

}
#endif // DECODEVECTORS_NEW

#ifndef DECODEVECTORS_ASM
#ifdef DECODEVECTORS_NEW
//-----------------------------------------------------------------------------------
// An integrated (leaf) version of RA_TNI_DecodeVectors. RA_TNI_UnpackSQVH and RA_TNI_ScalarDequant 
//	are now merged into one super-function

void RA_TNI_DecodeVectors(short *catbuf, short *rms_index, long *mlt, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{



	short r, i, cat;
	short k[NBINS];
	short s[NBINS];
	short cregions;
	
	short *kptr, *sptr , *mltptr;
	//---------------------------ScalarDequant vars ----------------------------------------------
	long fixdeqnt;
	short exp, exp2;
	long sign;
	ULONG man;
	const char *expindex;
	const long *manindex;
	//---------------------------ScalarDequant vars ----------------------------------------------
	
	cregions = bitstrmPtr->nregions + bitstrmPtr->cplstart;

	for (r = 0; r < cregions; r++) {					/* now use cregions instead of nregions */

		cat = catbuf[r];

		if (cat < 7) {	/* cat==7 not coded */
		
			//	if(RA_TNI_UnpackSQVH(cat, (short *)k, (short *)s, pkbit, pktotal, bitstrmPtr)) { /* if out-of-bits */
			//---------------------------UnpackSQVH inlined ----------------------------------------------
			int n ;
			int vd;
			short radix;
			short udiv;
			USHORT vindex, bit;
			int  outOfBits = 0, kindex;
		#if 1
			int iradix, vpr;
			long div;
		#endif	

		#if 1
			vpr = RA_TNI_vpr_tab[cat];
			iradix = RA_TNI_iradix_tab[cat];
		#endif

			
			kptr = k ;
			sptr = s;
		
			vd = RA_TNI_vd_tab[cat];
			kindex = vd - 1;
			radix = RA_TNI_kmax_tab[cat];
		
			for (n = RA_TNI_vpr_tab[cat]; n; n--) {
		
#ifdef _HUFFMAN_TABLES_
				if (!RA_TNI_DecodeNextSymbolWithTable( RA_TNI_sqvh_tab [cat], RA_TNI_sqvh_bitcount_tab[cat], 
															&vindex, pkbit, pktotal, bitstrmPtr) ) {
					vindex = 0;
					outOfBits = 1;
				}
#else //_HUFFMAN_TABLES_
#ifdef _PROFILE_TREES_
				if (!RA_TNI_DecodeNextSymbol(RA_TNI_sqvh_tree[cat], PROFILE_sqvh_tree[cat], &vindex, pkbit, pktotal, bitstrmPtr)) {
					vindex = 0;
					outOfBits = 1;
				}
#else	//_PROFILE_TREES_
				if (!RA_TNI_DecodeNextSymbol(RA_TNI_sqvh_tree[cat], &vindex, pkbit, pktotal, bitstrmPtr)) {
					vindex = 0;
					outOfBits = 1;
				}
#endif	//_PROFILE_TREES_	
		
#endif //_HUFFMAN_TABLES_
		
				/*
				 * Recover scalars from vector, LSD to MSD.
				 * XXX early exit when vindex == 0?
				 */
				kptr += kindex;
				
				if(!vindex) {
					for (i = vd; i; i--) {
						*kptr-- = 0;
					}
				}
				else {
					for (i = vd; i; i--) {
						/* div = vindex / radix; */
						udiv = ((long)vindex * iradix )>>15;
						//udiv = vindex / radix;
		
						/* k = vindex % radix; */
						*kptr-- = vindex - (udiv * radix);
						vindex = udiv;
					}
				}
		
				/* get any trailing signbits, stored MSD to LSD */
				kptr++;
				for (i = vd; i; i--) {
					if (*kptr++) {	/* for each non-zero k, get sign */
						if (!RA_TNI_Unpackbit(&bit, pkbit, pktotal, bitstrmPtr)) {
							outOfBits = 1;
							*sptr++ = 0;
						} else
							*sptr++ = bit;
					} else
						*sptr++ = 0;
				}
			}
			//	return outOfBits;
			//---------------------------End of UnpackSQVH inlined ----------------------------------------------
			
			if( outOfBits ) { /* if out-of-bits */
				cat = 7;
				for (i = r; i < cregions; i++)			/* now use cregions instead of nregions */
					catbuf[i] = 7;	/* set cats to 7 */
			}
		}
		if (cat == 7) {
			/* maybe dequantizer should detect cat==7 instead? */
			for (i = 0; i < NBINS; i++)
				k[i] = s[i] = 0;	/* zero scalars */
		}
		
		
		//RA_TNI_ScalarDequant(cat, r, rms_index, (short *)k, (short *)s, mlt, bitstrmPtr );
		//---------------------------ScalarDequant inlined ----------------------------------------------
		/* sqrt(pow(2,n)) normalized as man*(2^exp), encoder clips rms_index[r] to [-31, 63] */
		kptr = k ;
		sptr = s ;
//		mltptr = mlt ;
		
		man = RA_TNI_fixrootpow2man_new[rms_index[r]&0x01];
		/* exp = FIXRP2EXP(rms_index[r]);			integers, range = [-31, 32] */
		exp = -31 + ((rms_index[r] + 63) + 1) / 2; /* replaced FIXRP2EXP by -31 + ((i+63)+1)/2 -- Venky */
	
		expindex = RA_TNI_fqct_exp[cat];
		manindex = RA_TNI_fqct_man[cat];
	
		for (i = 0; i < NBINS; sptr++,i++) {
			if (!*kptr++) {
				/* update the LFSR, producing a random sign */
				sign = (bitstrmPtr->_lfsr >> 31);
				bitstrmPtr->_lfsr = (bitstrmPtr->_lfsr << 1) ^ (sign & FEEDBACK);
	
				exp2 = exp;
				fixdeqnt = RA_TNI_fixdither_tab[cat];			/* 1.31 signed */
				fixdeqnt = (fixdeqnt ^ sign) - sign;	/* inflict the sign */
			}
			else {	/* for stepsize[cat]*k[i], use centroid[cat][k[i]] */
				sign = -*sptr;
				exp2 = exp + *(expindex + *(kptr-1));		/* exp(stepsize[cat]*k[i]) */
				fixdeqnt = *(manindex + *(kptr-1));			/* man(stepsize[cat]*k[i]) */
				fixdeqnt = (fixdeqnt ^ sign) - sign;	/* inflict the sign */
			}
	
			if (exp2 < UNDERCLIP) {		/* underflow towards zero (+ or -) */
				exp2 = UNDERCLIP;
			}
			if (exp2 > OVERCLIP) {		/* saturate (clip the exponent) to avoid overflowing MLTBITS */
				exp2 = OVERCLIP;
			}
#ifndef __OPTIMISE_FIRST_PASS__	
			*mlt++  = (long)(RA_TNI_MULSHIFT0(fixdeqnt, man))>>(MLTBITS - 2 - exp2);
#else
			*mlt++  = (long)(RA_TNI_MULSHIFT0(&fixdeqnt, &man))>>(MLTBITS - 2 - exp2);
#endif
	
		}
		// return ;
		//---------------------------End of ScalarDequant inlined ----------------------------------------------
		
//		mlt += NBINS;
	}

	/* set non-coded regions to zero */
	for (i = cregions * NBINS; i < bitstrmPtr->nsamples; i++)		/* now use cregions instead of nregions */
		*mlt++ = 0;

}

#endif //DECODEVECTORS_NEW
#endif //DECODEVECTORS_ASM

