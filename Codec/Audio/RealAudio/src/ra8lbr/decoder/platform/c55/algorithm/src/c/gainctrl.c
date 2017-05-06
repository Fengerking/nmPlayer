/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: gainctrl.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 * Gain Control functions.
 */
#include "coder.h"
#include "assembly.h"

/*
 * JR - fixed-point tables for n = (-GAINDIF...GAINDIF)
 *	RA_TNI_fixroot2ntab = 2^(n/npsamps), in fixed 2.30 format (range ~= 0.788 -> 1.269)
 */
const long RA_TNI_fixroot2ntab[3][2*GAINDIF + 1] = {	/* 3 rows, for N = 256, 512, 1024, respectively */
  {
	0x326e6f61, 0x33892305, 0x34aa0764, 0x35d13f32, 0x36feede6, 0x383337bb, 0x396e41ba, 0x3ab031ba,
	0x3bf92e67, 0x3d495f45, 0x3ea0ecb7, 0x40000000, 0x4166c34c, 0x42d561b4, 0x444c0740, 0x45cae0f2,
	0x47521cc5, 0x48e1e9ba, 0x4a7a77d4, 0x4c1bf829, 0x4dc69cdd, 0x4f7a9930, 0x51382181
  },
  {
	0x38cfe25c, 0x396e41ba, 0x3a0e5a94, 0x3ab031ba, 0x3b53cc07, 0x3bf92e67, 0x3ca05dcf, 0x3d495f45,
	0x3df437dd, 0x3ea0ecb7, 0x3f4f8303, 0x40000000, 0x40b268fa, 0x4166c34c, 0x421d1462, 0x42d561b4,
	0x438fb0cb, 0x444c0740, 0x450a6aba, 0x45cae0f2, 0x468d6fad, 0x47521cc5, 0x4818ee21
  },
  {
	0x3c4c8c2a, 0x3ca05dcf, 0x3cf4a3f7, 0x3d495f45, 0x3d9e905b, 0x3df437dd, 0x3e4a566f, 0x3ea0ecb7,
	0x3ef7fb5b, 0x3f4f8303, 0x3fa78457, 0x40000000, 0x4058f6a8, 0x40b268fa, 0x410c57a1, 0x4166c34c,
	0x41c1aca7, 0x421d1462, 0x4278fb2b, 0x42d561b4, 0x433248ae, 0x438fb0cb, 0x43ed9ac0
  }
};

/* #define FIXPOW2(n)		(RA_TNI_fixpow2tab[(n)-2*GAINMIN]) */
#define FIXPOW2(n)		((unsigned long)0x100<<((n)-2*GAINMIN))
#define FIXROOT2N(x,n)	(RA_TNI_fixroot2ntab[(x)][(n)+GAINDIF])

/*
 * Interpolates part of gain control window.
 * log interp done by successive multiplication.
 *
 * gain ranges from -7 to 4 so window ranges from 2^-7 to 2^4
 * if gain0 != gain1, we let fixwnd start at 2^gain0 and increase it logarithmically
 *   to a final value of gain1
 *
 * This is the last step before converting to short, so fractional roundoff error is
 *	not a big issue
 * Gain window represented as 16.16 signed to accomodate gains of [2*GAINMIN to 2*GAINMAX]
 * Interpolation factors stored as 2.30 signed
 *
 * Fixed-point changes:
 *  - arithmetic now fixed-point
 *  - fixfactor comes from hard-coded lookup table, so we index it (dep. on trans. size) with rootindex
 */
void Interpolate(long *inptr, short gain0, short gain1, short rootindex, short npsamps);

#ifndef INTERP_ASM
void Interpolate(long *inptr, short gain0, short gain1, short rootindex, short npsamps)
{
	long fixwnd, fixfactor;

	fixwnd = FIXPOW2(gain0);	/* always NINTBITS.(32-NINTBITS) */

	if (gain0 == gain1)			/* fast path */
		for (;npsamps; npsamps--) {
#ifndef __OPTIMISE_FIRST_PASS__
			*inptr = RA_TNI_MULSHIFT0(*inptr, fixwnd);			/* adds NINTBITS, removed after overlap-add */
#else
			*inptr = RA_TNI_MULSHIFT0(inptr, &fixwnd);			/* adds NINTBITS, removed after overlap-add */
#endif
			inptr++;
		}
	else {
		fixfactor = FIXROOT2N(rootindex, gain1 - gain0);	/* in 2.30 signed format (range ~= 0.788 -> 1.269) */
		for (;npsamps; npsamps--) {
#ifndef __OPTIMISE_FIRST_PASS__
			*inptr = RA_TNI_MULSHIFT0(*inptr, fixwnd);
			fixwnd = RA_TNI_MULSHIFT2(fixwnd, fixfactor);		/* restore to NINTBITS.(32-NINTBITS) format */
#else
			*inptr = RA_TNI_MULSHIFT0(inptr, &fixwnd);
			fixwnd = RA_TNI_MULSHIFT2(&fixwnd, &fixfactor);		/* restore to NINTBITS.(32-NINTBITS) format */
#endif
			inptr++;
		}
	}
}
#endif

/*
 * Multiply input by gain control window.
 * gainc0 is for first half, gainc1 is for second half.
 *
 * Fixed-point changes:
 *  - removed sign variable (so it's always in decode mode)
 *	- made input int
 */


 // Define GAIN_WINDOW_OLD to activate old version
 // Define GAINWINDOW_ASM to activate asm version

#ifndef GAINWINDOW_ASM 
void RA_TNI_GainWindow(long *input, GAINC *gainc0, GAINC *gainc1, short nsamps); 

#ifdef GAIN_WINDOW_OLD
/* Old version of RA_TNI_GainWindow*/
void RA_TNI_GainWindow(long *input, GAINC *gainc0, GAINC *gainc1, short nsamps)
{
	int i, nats;
	short exgain[NPARTS+1], offset;
	long *inptr;
	short rootindex, npsamps;

/*
    npsamps   = nsamps>>3;
    rootindex = npsamps>>6;
*/
	rootindex = (npsamps = nsamps>>3)>>6;
/*
 * Second half
 */
	/* expand gains, working backwards */
	exgain[NPARTS] = 0;		/* always finish at 1.0 */
	nats = gainc1->nats;		/* gain changes left */
#ifdef TESTING
	for (i = NPARTS-1; i >= 0; i--) {
		if (nats && (i == gainc1->loc[nats-1]))		/* at gain change */
			exgain[i] = gainc1->gain[--nats];		/* use it */
		else
			exgain[i] = exgain[i+1];					/* repeat last gain */
	}
#else
	if(nats) {
		for (i = NPARTS-1; i >= 0; i--) {
			if (nats && (i == gainc1->loc[nats-1]))		/* at gain change */
				exgain[i] = gainc1->gain[--nats];		/* use it */
			else
				exgain[i] = exgain[i+1];					/* repeat last gain */
		}
	}
	else {
		for (i = NPARTS-1; i >= 0; i--) {
			exgain[i] = exgain[i+1];					/* repeat last gain */
		}
	}
#endif

	/* interpolate the window */
	inptr = input + nsamps;
	for (i = 0; i < NPARTS; i++) {
	
		Interpolate( inptr, exgain[i], exgain[i+1], rootindex, npsamps);
		inptr += npsamps;
	}

	/*
	 * Pull any discontinuity thru first half, by offsetting all gains
	 * with the starting gain of second half.
	 */
	offset = exgain[0];
/*
 * First half
 */
	/* expand gains, working backwards */
	exgain[NPARTS] = 0;		/* always finish at 1.0 */
	nats = gainc0->nats;		/* gain changes left */
#ifdef TESTING
	for (i = NPARTS-1; i >= 0; i--) {
		if (nats && (i == gainc0->loc[nats-1]))			/* at gain change */
			exgain[i] = gainc0->gain[--nats];		/* use it */
		else
			exgain[i] = exgain[i+1];					/* repeat last gain */
	}
#else
	if (nats) {
		for (i = NPARTS-1; i >= 0; i--) {
			if (nats && (i == gainc0->loc[nats-1]))		/* at gain change */
				exgain[i] = gainc0->gain[--nats];		/* use it */
			else
				exgain[i] = exgain[i+1];					/* repeat last gain */
		}
	}
	else {
		for (i = NPARTS-1; i >= 0; i--) {
			exgain[i] = exgain[i+1];					/* repeat last gain */
		}
	}
#endif
	/* interpolate the window */
	inptr = input;
	for (i = 0; i < NPARTS; i++) {
		Interpolate(inptr, exgain[i] + offset, exgain[i+1] + offset, rootindex, npsamps);
		inptr += npsamps;
	}
}   

#else //GAIN_WINDOW_OLD


/* New version of RA_TNI_GainWindow */
void RA_TNI_GainWindow(long *input, GAINC *gainc0, GAINC *gainc1, short nsamps)
{

	int i, nats;
	short exgain[NPARTS+1], offset;
	long *inptr;
	short rootindex, npsamps;

	rootindex = (npsamps = nsamps>>3)>>6;

	/* expand gains, working backwards */
	exgain[NPARTS] = 0;		/* always finish at 1.0 */
	nats = gainc1->nats;		/* gain changes left */
/*
 * Second half
 */
	if(nats) {
		for (i = NPARTS-1; i >= 0; i--) {
			if (nats && (i == gainc1->loc[nats-1]))		/* at gain change */
				exgain[i] = gainc1->gain[--nats];		/* use it */
			else
				exgain[i] = exgain[i+1];					/* repeat last gain */
		}
	
		inptr = input + nsamps;
		for (i = 0; i < NPARTS; i++) {
			Interpolate( inptr, exgain[i], exgain[i+1], rootindex, npsamps);
			inptr += npsamps;
		}
		offset = exgain[0];
	}
	else {
		inptr = input + nsamps;
		for (i = NPARTS-1; i >= 0; i--) {
			Interpolate( inptr, 0, 0, rootindex, npsamps);
			inptr += npsamps ;
		}
		offset = 0 ;
	}
	
	
	/* interpolate the window */
	
	/*
	 * Pull any discontinuity thru first half, by offsetting all gains
	 * with the starting gain of second half.
	 */

/*
 * First half
 */
	/* expand gains, working backwards */
	exgain[NPARTS] = 0;		/* always finish at 1.0 */
	nats = gainc0->nats;		/* gain changes left */
	if (nats) {
		for (i = NPARTS-1; i >= 0; i--) {
			if (nats && (i == gainc0->loc[nats-1]))		/* at gain change */
				exgain[i] = gainc0->gain[--nats];		/* use it */
			else
				exgain[i] = exgain[i+1];					/* repeat last gain */
		}
		inptr = input;
		for (i = 0; i < NPARTS; i++) {
			Interpolate(inptr, exgain[i] + offset, exgain[i+1] + offset, rootindex, npsamps);
			inptr += npsamps;
		}
	}
	else {
		inptr = input;
		for (i = NPARTS-1; i >= 0; i--) {
			Interpolate(inptr, offset, offset, rootindex, npsamps);
			inptr += npsamps;
				
		}
	}
	/* interpolate the window */

}


#endif //GAIN_WINDOW_OLD
#endif // GAINWINDOW_ASM


void GcLoop1(long *inPtr1, long *inPtr2, long *overlap, short *outbuf, short npsamps, short pcmIFact);
void GcLoop2(long *inPtr1, long *inPtr2, long *overlap, short *outbuf, short npsamps, short pcmIFact);

#ifndef INTERP_ASM
/*
 * Compensate the signal using the gain control function.
 * Includes the overlap-add stage of inverse transform.
 *
 * NOTE: operates in-place, with result in first half of input.
 *
 * gainc0 has the gain info for the first half of the input buffer
 * gainc1 has the gain info for the second half of the input buffer
 *
 * Fixed-point changes:
 *  - conversion to short is done here instead of in Decode() because
 *      RA_TNI_MAKESHORT does the overlap-add as well as rounding to 16 bits,
 *      (this saves us from looping over the whole frame one more time)
 */

 
void RA_TNI_GainCompensate(long *input, GAINC *gainc0, GAINC *gainc1, short nsamps, short pcmInterleaveFact,
			   long *overlap, int *outbuf)
{
	long *inPtr1  = input, *inPtr2 = input+nsamps;

/*
	long *overPtr = overlap;
	short *outPtr = outbuf;
*/

	/* gain window the data if there are attacks */
	if (gainc0->nats || gainc1->nats) {
		RA_TNI_GainWindow(input, gainc0, gainc1, nsamps);						
		
		
#ifndef GCLOOP
		for (; nsamps; nsamps--) {
			*outbuf = RA_TNI_MAKESHORT(*inPtr1++, *overlap);
			outbuf += pcmInterleaveFact;
			*overlap++ = *inPtr2++;
		}
#else
	GcLoop2(inPtr1, inPtr2, overlap, outbuf, nsamps, pcmInterleaveFact);
#endif
	}

	/* if no attacks, just scale the inputs and overlap-add */
	else {
#ifndef GCLOOP
		for (; nsamps; nsamps--) {
			/* long inData = ((long)(*inPtr1++) >> NINTBITS); */
			*outbuf = RA_TNI_MAKESHORT((long)((*inPtr1++)>>NINTBITS), *overlap);
			outbuf += pcmInterleaveFact; /* ifact; */
			*overlap++ = ((long)(*inPtr2++) >> NINTBITS);
			/* overlap++; */
		}
#else
	GcLoop1(inPtr1, inPtr2, overlap, outbuf, nsamps, pcmInterleaveFact);
#endif
	}
}
#endif //INTERP_ASM


#ifndef INTERP_ASM
int RA_TNI_DecodeGainInfo(GAINC *gainc, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	int i, nbits;
	short code;

	/* unpack nattacks */
	nbits = 0;
	do {
		nbits += RA_TNI_Unpackbit((USHORT *)&code, pkbit, pktotal, bitstrmPtr);		/* count bits until zero reached */
	} while (code);

	/* unpack any location/gain pairs */
	if (gainc->nats = (nbits - 1)) {
		for (i = 0; i < gainc->nats; i++) {
			/* location */
			nbits += RA_TNI_Unpackbits(LOCBITS, (USHORT *)&gainc->loc[i], pkbit, pktotal, bitstrmPtr);
			/* gain code */
			nbits += RA_TNI_Unpackbit((USHORT *)&code, pkbit, pktotal, bitstrmPtr);
			if (!code) {
				gainc->gain[i] = -1;
			} else {
				nbits += RA_TNI_Unpackbits(GAINBITS, (USHORT *)&code, pkbit, pktotal, bitstrmPtr);
				gainc->gain[i] = CODE2GAIN(code);
			}
		}
	}

	return nbits;
}
#endif //INTERP_ASM
