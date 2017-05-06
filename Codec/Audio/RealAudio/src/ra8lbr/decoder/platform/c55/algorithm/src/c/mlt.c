/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: mlt.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 * Fast MLT/IMLT functions.
 *
 * NOTES:
 * Requires inplace FFT module (without normalization) and reorder table.
 *
 */
#include <stdlib.h>
#include <string.h>
#include "coder.h"
#include "assembly.h"
#include "fixtabs.h"

/* InitMLT has been removed since the tables generated have been made global constant tables -- Venky */

/*
 * Pre-FFT complex multiplication.
 *
 * NOTES:
 * Includes input re-ordering, so cannot operate in-place.
 *
 * Fixed-point changes:
 *  - arithmetic is now fixed-point
 *  - number of integer bits:
 *      input:  zbuf = MLTBITS
 *      output: zfft = MLTBITS + 1
 */

#ifndef MLT_ASM

#ifndef __OPTIMISE_SECOND_PASS__
void
RA_TNI_PreMultipy(long *zbuf, long *zfft, short nsamples)
{

	long ar, ai, cr, ci, tempar;
	const long *cosptr, *sinptr, *zbuf2;

	cosptr = RA_TNI_fixcos4tab_tab[nsamples>>9];
	sinptr = RA_TNI_fixsin4tab_tab[nsamples>>9];
	zbuf2  = zbuf + nsamples - 1;

	for (nsamples>>=1; nsamples; nsamples--) {
		ar = *zbuf;						/* input re-order: 0, 2, 4, ... */
		ai = *zbuf2;					/* input re-order: 319, 317, ... */
		cr = *cosptr++;
		ci = *sinptr++;

		/* gain and eat one guard bit
		 * cr, ci = 1.31 signed, range = (0, 1)
		 * non output re-ordering
		 */
#ifndef __OPTIMISE_FIRST_PASS__
		*zfft++ = RA_TNI_MULSHIFT0(ar, cr)  + RA_TNI_MULSHIFT0(ai, ci);	/* Re{ z * exp(-j*pi(n+0.25)/N) } */
		*zfft++ = RA_TNI_MULSHIFT0(-ar, ci) + RA_TNI_MULSHIFT0(ai, cr);	/* Im{ z * exp(-j*pi(n+0.25)/N) } */
#else
		tempar = -ar;
		*zfft++ = RA_TNI_MULSHIFT0(&ar, &cr)  + RA_TNI_MULSHIFT0(&ai, &ci);	/* Re{ z * exp(-j*pi(n+0.25)/N) } */
		*zfft++ = RA_TNI_MULSHIFT0(&ai, &cr) + RA_TNI_MULSHIFT0(&tempar, &ci);	/* Im{ z * exp(-j*pi(n+0.25)/N) } */

#endif
		zbuf += 2;
		zbuf2 -= 2;
	}
	
}
#endif		// __OPTIMISE_SECOND_PASS__



/*
 * Post-FFT complex multiplication.
 *
 * NOTES:
 * Normalization by sqrt(2/N) is rolled into cos/sin tables.
 * Includes input re-ordering, for power-of-two FFT.
 * Includes output re-ordering, so cannot operate in-place.
 *
 * Fixed-point changes:
 *  - arithmetic is now fixed-point
 *  - number of integer bits:
 *      input:  fft = MLTBITS + 1 + nfftlog2 (may exceed 32, but don't worry)
 *      output: mlt = POSTBITS
 */
#ifndef __OPTIMISE_SECOND_PASS__
void RA_TNI_PostMultiply(long *fft, long *mlt, short nsamples)
{
	short i, idx, npostshifts;
	const short *tabptr;
	long ar, ai, cr, ci;
	const long *fixcosptr, *fixsinptr;
	long *mlt2;

	i = nsamples>>9;
/*
	npostshifts = 7 + i;
	npostshifts += MLTBITS + 1 - POSTBITS;
*/
	npostshifts = i + MLTBITS + 8 - POSTBITS;
	
	fixcosptr = RA_TNI_fixcos1tab_tab[i];
	fixsinptr = fixcosptr + (nsamples >> 1);	/* note: length of fixcos1tab = nmlt/2 + 1 */
	mlt2 = mlt + nsamples - 1;
	i = 1 - i;
	tabptr = RA_TNI_bitrevtab;

	/* JR - npostshifts converts the result into correct fixed-point format (see above) */
	for (nsamples >>= 1; nsamples; nsamples--) {
		idx = ((*tabptr++) >> i);
		ar = fft[idx];
		ai = fft[idx+1];
		cr = *fixcosptr++;
		ci = *fixsinptr--;
		/* cr, ci = 0.32 signed range ~= (-0.0000004,0.08839) */
#ifndef __OPTIMISE_FIRST_PASS__
		*mlt = RA_TNI_MULSHIFTNCLIP(ar, cr, npostshifts) + RA_TNI_MULSHIFTNCLIP(ai, ci, npostshifts);	/* saturates to POSTBITS (rare) */
		*mlt2 = RA_TNI_MULSHIFTNCLIP(ar, ci, npostshifts) - RA_TNI_MULSHIFTNCLIP(ai, cr, npostshifts);
#else
		*mlt = RA_TNI_MULSHIFTNCLIP(&ar, &cr, npostshifts) + RA_TNI_MULSHIFTNCLIP(&ai, &ci, npostshifts);	/* saturates to POSTBITS (rare) */
		*mlt2 = RA_TNI_MULSHIFTNCLIP(&ar, &ci, npostshifts) - RA_TNI_MULSHIFTNCLIP(&ai, &cr, npostshifts);
#endif
			
		mlt += 2;
		mlt2 -= 2;
	}
	return;
}
#endif		// __OPTIMISE_SECOND_PASS__


#endif


/*
 * Decode window, without overlap-add.
 * Returns pcm[2N] each call.
 *
 * Fixed-point changes:
 *  - arithmetic is now fixed-point
 *  - number of integer bits:
 *      input:  zbuf = POSTBITS
 *      output: pcm = POSTBITS
 */
#ifndef MLT_ASM

#ifndef __OPTIMISE_SECOND_PASS__
void RA_TNI_DecWindowNoOverlap(long *zbuf, long *pcm, short nsamples)
{
	int i, nmlt;
	long *zbuf2, *pcm2, *pcm3, *pcm4;
	const long *fixwnd, *fixwnd2 ;
	long fixtemp ;
	
	

	/* nmlt = nsamples; */
	nmlt = nsamples - 1;
	
/*
	pcm2 = pcm + nmlt;
	pcm3 = pcm2 + 1;
	pcm4 = pcm3 + nmlt;
*/
	pcm4 = (pcm3 = (pcm2 = pcm + nmlt) + 1) + nmlt;
	zbuf += (nsamples >> 1);
	zbuf2 = zbuf - 1;
	fixwnd = RA_TNI_fixwindow_tab[nsamples>>9];
	fixwnd2 = fixwnd + nmlt;

	for (i = (nsamples >> 1); i != 0; i--) {
	
#ifndef __OPTIMISE_FIRST_PASS__	
		*pcm++  = RA_TNI_MULSHIFT1(*zbuf2, *fixwnd);	/* remove extra sign bit */
		*pcm2-- = RA_TNI_MULSHIFT1(*zbuf2--, *fixwnd2);
		*pcm3++ = RA_TNI_MULSHIFT1(*zbuf, *fixwnd2--);
		*pcm4-- = RA_TNI_MULSHIFT1(*zbuf++, -(*fixwnd++));
#else
		*pcm++  = RA_TNI_MULSHIFT1(zbuf2, fixwnd);	/* remove extra sign bit */
		*pcm2-- = RA_TNI_MULSHIFT1(zbuf2--, fixwnd2);
		*pcm3++ = RA_TNI_MULSHIFT1(zbuf, fixwnd2--);
		fixtemp = -(*fixwnd++);
		*pcm4-- = RA_TNI_MULSHIFT1(zbuf++, &fixtemp);
		
#endif
	}

	return;
}
#endif //__OPTIMISE_SECOND_PASS__
#endif

///***********************/
// For debug! only
//#define MAXNSAMP 1024
//long decpcm_my[2*MAXNSAMP];
//unsigned long mlt_count = 0;
//#define MAXNSAMP 1024
//long buf_my[MAXNSAMP];			
/***************************/


/*
 * Inverse MLT transform, without overlap-add.
 * mlt[N] contains input MLT data.
 * pcm[2N] returns output samples.
 *
 * Fixed-point changes:
 *  - buf is now int instead of float
 *  - number of integer bits:
 *      input:  mlt = MLTBITS
 *      output: pcm = POSTBITS
 */
 
#ifndef __OPTIMISE_SECOND_PASS__
void RA_TNI_IMLTNoOverlap(long *mlt, long *pcm, short nsamples, long *buf)
{
	/* fast DCT-IV */
	RA_TNI_PreMultipy(mlt, pcm, nsamples);			/* adds one int bit */
				/* adds one int bit */
	RA_TNI_FFT( pcm, nsamples);
	/* adds one int bit, rescales to POSTBITS */

	RA_TNI_PostMultiply( pcm, buf, nsamples);
	RA_TNI_DecWindowNoOverlap(buf, pcm, nsamples);		/* doesn't change fixed-pt. format */

}


#endif //__OPTIMISE_SECOND_PASS__

