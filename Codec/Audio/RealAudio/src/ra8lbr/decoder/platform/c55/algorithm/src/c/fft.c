/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: fft.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 * Power-of-two FFT routines.
 */
#include <stdlib.h>
#include "coder.h"
#include "assembly.h"
#include <string.h>


/*
 * Forward FFT.
 * Input is fftbuf[2*nfft] in interleaved format.
 * Operates in-place.
 *
 * Fixed-point changes:
 *  - math is all fixed-point, rescaling done as we go
 *  - inner loop of fft butterfly made into inline function which can be
 *      rewritten in asm or mixed C/asm for speed
 *  - number of integer bits:
 *      input:  zfft = MLTBITS + 1
 *      output: zfft = MLTBITS + 1 + nfftlog2 (may exceed 32, but don't worry)
 */
 


#ifndef FFT_ASM

#ifndef __OPTIMISE_SECOND_PASS__
void
RA_TNI_FFT(long *fftbuf, short nsamples)
{
	int i, j, k, gp;
	const long *ztwidptr;
	const long *ztwidtab;
	long *zptr1, *zptr2, *zptr3, *zptr4;
	long ar, ai, br, bi, cr, ci, dr, di;
	long aici, brdr, arcr, bidi;
	short bg, bgOffset;	/* to increment counter */
	
	
	/* first two passes as radix-4 pass */
	short nfft = nsamples>>2;
	zptr4=(zptr3=(zptr2=(zptr1=fftbuf)+nfft)+nfft) + nfft;


	for (i = (nsamples>>3); i != 0 ; i--) {

		/* manually add one guard bit before each of the two additions */
		ar  = *(zptr1+0)  >> 1;
		ai  = *(zptr1+1)  >> 1;

		cr  = *(zptr3+0) >> 1;
		ci  = *(zptr3+1) >> 1;

		aici = (ai + ci) >> 1;
		arcr = (ar + cr) >> 1;

		br  = *(zptr2+0) >> 1;
		bi  = *(zptr2+1) >> 1;

		dr  = *(zptr4+0)  >> 1;
		di  = *(zptr4+1)  >> 1;

		bidi = (bi + di) >> 1;
		brdr = (br + dr) >> 1;

		*(zptr1+0) = arcr  + brdr;
		*(zptr1+1) = aici  + bidi;

		*(zptr2+0) = arcr  - brdr;
		*(zptr2+1) = aici  - bidi;

		aici = (ai - ci) >> 1;
		brdr = (br - dr) >> 1;
		arcr = (ar - cr) >> 1;
		bidi = (bi - di) >> 1;

		*(zptr3+0) = arcr  + bidi;
		*(zptr3+1) = aici  - brdr;

		*(zptr4+0) = arcr  - bidi;
		*(zptr4+1) = aici  + brdr;

		zptr1 += 2;
		zptr2 += 2;
		zptr3 += 2;
		zptr4 += 2;
	}

	/* radix-2 passes, excluding first two */
	bg = nsamples >> 4;			/* butterflies per group */
	gp = 4;					/* groups per pass */

	ztwidtab = (const long *)RA_TNI_twidtabs[nsamples>>9];
	for (k = ((nsamples>>9) + 5); k != 0 ; k--) {

		/* zptr1 = fftbuf; */
		zptr2 = (zptr1 = fftbuf) + (bg << 1);
		ztwidptr = ztwidtab;
		bgOffset = (bg << 2);

		for (j = gp; j; j--) {
		
			RA_TNI_BUTTERFLY( zptr1, zptr2, ztwidptr, bg);
			
			zptr1 += bgOffset;
			zptr2 += bgOffset;
			ztwidptr += 2;
		}

		bg >>= 1;
		gp <<= 1;
	}
}


//#include <string.h>
//long myfftbuf[1024] ;
/* Test FFT*/
void RA_TNI_FFT_test(long *fftbuf, short nsamples)
{
	int i, j, k, gp;
	const long *ztwidptr;
	const long *ztwidtab;
	long *zptr1, *zptr2, *zptr3, *zptr4;
	long ar, ai, br, bi, cr, ci, dr, di;
	long aici, brdr, arcr, bidi;
	short bg, bgOffset;	/* to increment counter */
	
	long AC0, AC1, AC2 , AC3 ;
	
	long *myzptr1, *myzptr2, *myzptr3, *myzptr4 ;
	
	
	
	
	/* first two passes as radix-4 pass */
	short nfft = nsamples>>2;
	zptr4=(zptr3=(zptr2=(zptr1=fftbuf)+nfft)+nfft) + nfft;
	/*memcpy( myfftbuf, fftbuf, nsamples*sizeof(long));
	
	myzptr4=(myzptr3=(myzptr2=(myzptr1=myfftbuf)+nfft)+nfft) + nfft;*/
	
	
	for (i = (nsamples>>3); i != 0 ; i--) {

		/* manually add one guard bit before each of the two additions */
		/* this is gonna be tough 
		ar  = *(myzptr1+0)  >> 1;
		ai  = *(myzptr1+1)  >> 1;

		cr  = *(myzptr3+0) >> 1;
		ci  = *(myzptr3+1) >> 1;

		aici = (ai + ci) >> 1;
		arcr = (ar + cr) >> 1;

		br  = *(myzptr2+0) >> 1;
		bi  = *(myzptr2+1) >> 1;

		dr  = *(myzptr4+0)  >> 1;
		di  = *(myzptr4+1)  >> 1;

		bidi = (bi + di) >> 1;
		brdr = (br + dr) >> 1;

		*(myzptr1+0) = arcr  + brdr;
		*(myzptr1+1) = aici  + bidi;

		*(myzptr2+0) = arcr  - brdr;
		*(myzptr2+1) = aici  - bidi;

		aici = (ai - ci) >> 1;
		brdr = (br - dr) >> 1;
		arcr = (ar - cr) >> 1;
		bidi = (bi - di) >> 1;

		*(myzptr3+0) = arcr  + bidi;
		*(myzptr3+1) = aici  - brdr;

		*(myzptr4+0) = arcr  - bidi;
		*(myzptr4+1) = aici  + brdr;

		myzptr1 += 2;
		myzptr2 += 2;
		myzptr3 += 2;
		myzptr4 += 2;*/
		
		AC0 = *(zptr1) >> 1 ;				// ar
		AC1 = *(zptr3) >> 1 ;				// cr
		AC2 = *(zptr2) >> 1 ;			// br
		AC3 = *(zptr4) >> 1 ;			// dr

		*(zptr3) = (AC0 - AC1) >> 1 ;			// arcr_2
		AC0 = (AC0 + AC1) >> 1 ;				// arcr_1
		
		*(zptr4) = ( AC2 - AC3) >> 1 ;   // brdr_2
		AC2 = ( AC2 + AC3) >> 1 ;	// brdr_1
		
		AC1 = AC0 + AC2	;	// arcr_1 + brdr_1
		AC3 = AC0 - AC2  ; // arcr_1  - brdr_1 
		(*zptr1) = AC1 ;
		(*zptr2 ) = AC3 ; 	
		
		AC0 = *(zptr1 + 1) >> 1 ; 		// ai
		AC1 = *(zptr3 + 1 ) >> 1 ;		// ci
		AC2 = *(zptr2 + 1 ) >>1 ;		// bi
		AC3 = *(zptr4 + 1 ) >> 1 ; 		// di
		
		*(zptr3+1) = (AC0 - AC1 ) >> 1;	// aici_2
		AC0 = ( AC0 + AC1) >> 1 ;	// aici_1
		AC1 = ( AC2 - AC3 ) >> 1 ;  // bidi_2
		AC2 = ( AC2 + AC3) >> 1 ;  // bidi_1 
		
		AC3 = AC0 + AC2	;	// aici_1 + bidi_1
		AC2 = AC0 - AC2  ; // aici_1  - bidi_1 
		*(zptr1+1) = AC3 ;
		*(zptr2+1) = AC2 ; 	
						
		
		AC0 = *(zptr3) + AC1	;					 ;  // arcr_2 + bidi_2		
		AC1 = *(zptr3) - AC1	;						 ;  // (zptr4) = arcr_2 - bidi_2 
		*(zptr3) = AC0 ;
		AC0 = *(zptr3+1) + *(zptr4)  ;				// (zptr4+1) = aici_2 + brdr_2 
		*(zptr4+1) = AC0 ;
		AC0 = *(zptr3+1) - *(zptr4)  ; 				// aici_2 - brdr_2 
		*(zptr4) = AC1 ;
		*(zptr3+1) = AC0 ;
	

		
		zptr1 += 2;
		zptr2 += 2;
		zptr3 += 2;
		zptr4 += 2;
			
		
		
	}

	/* radix-2 passes, excluding first two */
	bg = nsamples >> 4;			/* butterflies per group */
	gp = 4;					/* groups per pass */

	ztwidtab = (const long *)RA_TNI_twidtabs[nsamples>>9];
	for (k = ((nsamples>>9) + 5); k != 0 ; k--) {

		/* zptr1 = fftbuf; */
		zptr2 = (zptr1 = fftbuf) + (bg << 1);
		ztwidptr = ztwidtab;
		bgOffset = (bg << 2);

		for (j = gp; j; j--) {
		
			RA_TNI_BUTTERFLY( zptr1, zptr2, ztwidptr, bg);
			
			zptr1 += bgOffset;
			zptr2 += bgOffset;
			ztwidptr += 2;
		}

		bg >>= 1;
		gp <<= 1;
	}
}
#endif //__OPTIMISE_SECOND_PASS__

#endif
