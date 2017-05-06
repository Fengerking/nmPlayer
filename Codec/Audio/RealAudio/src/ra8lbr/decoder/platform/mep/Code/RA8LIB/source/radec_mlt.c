/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: radec_mlt.c,v 1.1 2005/04/05 21:22:07 hfrederickson Exp $ 
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


extern const char  __far D_PREMUL_TAB_SRC ;
extern const char  __far D_PREMUL_TAB_DST ;
extern const char  __far D_PREMUL_TAB_SIZ ;

extern const char  __far D_POSTMUL_TAB_SRC ;
extern const char  __far D_POSTMUL_TAB_DST ;
extern const char  __far D_POSTMUL_TAB_SIZ ;

extern const char  __far D_XTRATAB_SRC ;
extern const char  __far D_XTRATAB_DST ;
extern const char  __far D_XTRATAB_SIZ ;

/* mlt */
void PostMultiply(int tabidx, int *fft1);
static void PostMultiplyRescale(int tabidx, int *fft1, int es);


#ifdef __DMA_POSTMUL__
#pragma section const .tables_postmul
#endif

static const int postSkip[NUM_MLT_SIZES] = {7, 3, 1};

#ifdef __DMA_POSTMUL__
#pragma section const 
#endif


/**************************************************************************************
 * Function:    PreMultiply
 *
 * Description: pre-twiddle stage of MDCT
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       minimum 1 GB in, 2 GB out - loses (1+tabidx) int bits
 *              normalization by 2/sqrt(N) is rolled into tables here
 *              uses 3-mul, 3-add butterflies instead of 4-mul, 2-add
 *              should asm code (compiler not doing free pointer updates, etc.)
 **************************************************************************************/
void PreMultiply(int tabidx, int *zbuf1)
{
	int i,nmlt,ai1,*dummy;
	int t  ;
	int *zbuf2;
	const int *csptr;

	const int nmltTab[NUM_MLT_SIZES] = {256, 512, 1024};
	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/


	dummy=zbuf1;
	nmlt = nmltTab[tabidx];		
	zbuf2 = zbuf1 + nmlt - 1;
	csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

//	printf("\n\n %x \n\n",cos4sin4tab);

	/* whole thing should fit in registers - verify that compiler does this */
	for (i = nmlt >> 2; i !=0 ; i--) {
		/* cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin) */
		ai1=*(zbuf1 + 1);
				
		/* gain 1 int bit from MULSHIFT32, but drop 2, 3, or 4 int bits from table scaling */
		
		t  = MULSHIFT32(*(csptr+1), *zbuf1 + *zbuf2);
		
		*zbuf1 = t+MULSHIFT32(*csptr -2*(*(csptr+1)), *zbuf1);
		*(zbuf1+1) = MULSHIFT32(*csptr, *zbuf2) +(-t);
					
		t  = MULSHIFT32(*(csptr+3), *(zbuf2 - 1) + ai1);
		*(zbuf2-1) = t + MULSHIFT32(*(csptr+2) -2*(*(csptr+3)), *(zbuf2 - 1));
		*zbuf2 = MULSHIFT32(*(csptr+2), ai1) +(-t);
		zbuf2=zbuf2-2;
		zbuf1=zbuf1+2;
		csptr=csptr+4;
	}

	/* Note on scaling... 
	 * assumes 1 guard bit in, gains (1 + tabidx) fraction bits 
	 *   i.e. an1, 2, or 3 fraction bits, for nSamps = 256, 512, 1024
	 *   (left-shifting, since table scaled by 2 / sqrt(nSamps))
	 * this offsets the fact that each extra pass in FFT gains one more int bit
	 */

#ifdef __DMA_XTRAMUL__
	dma_transfer_wait(&D_XTRATAB_SRC, &D_XTRATAB_DST, &D_XTRATAB_SIZ);
#endif	
#ifdef __DMA_POSTMUL__
	dma_transfer_wait(&D_POSTMUL_TAB_SRC, &D_POSTMUL_TAB_DST, 0x1e40/*&D_POSTMUL_TAB_SIZ*/);
#endif		
	
	PostMultiply(tabidx,dummy);
	return;		
}
/**************************************************************************************
 * Function:    PostMultiply
 *
 * Description: post-twiddle stage of MDCT
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       minimum 1 GB in, 2 GB out - gains 1 int bit
 *              uses 3-mul, 3-add butterflies instead of 4-mul, 2-add
 *              should asm code (compiler not doing free pointer updates, etc.)
 **************************************************************************************/
void PostMultiply(int tabidx, int *fft1)
{
	int i, nmlt,ai2,skipFactor;
	int t,cms2,  cps2, sin2;
	int *fft2;
	const int *csptr;

	const int nmltTab[NUM_MLT_SIZES] = {256, 512, 1024};
	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/

	
	R4FFT(tabidx, fft1);	
	nmlt = nmltTab[tabidx];	
	csptr = cos1sin1tab;
	skipFactor = postSkip[tabidx]+1;

	fft2 = fft1 + nmlt + (-1);

	/* load coeffs for first pass
	 * cps2 = (cos+sin)/2, sin2 = sin/2, cms2 = (cos-sin)/2
	 */
	cps2 = *csptr;
	sin2 = *(csptr+1);
	cms2 = cps2 + (-2*sin2);

	for (i = nmlt >> 2; i !=0 ; i--) {
		/* gain 1 int bit from MULSHIFT32, and one since coeffs are stored as 0.5 * (cos+sin), 0.5*sin */
		
		t = MULSHIFT32(sin2, *(fft1) + *(fft1 + 1));
		*fft1 = t + MULSHIFT32(cms2, *(fft1));
		ai2= t +(-MULSHIFT32(cps2, *(fft1 + 1)));
		csptr =csptr+ skipFactor;
		cps2 = *csptr;
		sin2 = *(csptr+1);
				
		t = MULSHIFT32(sin2, *(fft2 - 1) + (-*(fft2)));
		cms2 = cps2 +(-2*sin2);
		*(fft1+1) = t + MULSHIFT32(cms2, *(fft2 - 1));
		*(fft2-1) = t +(-MULSHIFT32(cps2, (-*(fft2))));
		
		*fft2=ai2;
		fft2=fft2-2;
		fft1=fft1+2;
	
	}

	/* Note on scaling... 
	 * assumes 1 guard bit in, gains 2 int bits
	 * max gain of abs(cos) + abs(sin) = sqrt(2) = 1.414, so current format 
	 *   guarantees 1 guard bit in output
	 */
	return;	
}

/**************************************************************************************
 * Function:    PreMultiplyRescale
 *
 * Description: pre-twiddle stage of MDCT, with rescaling for extra guard bits
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *              number of guard bits to add to input before processing
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       see notes on PreMultiply(), above
 **************************************************************************************/
static void PreMultiplyRescale(int tabidx, int *zbuf1, int es)
{
	int i, nmlt, ar1, ai1,ai2,*dummy;
	int t;
	int *zbuf2;
	const int *csptr;

	const int nmltTab[NUM_MLT_SIZES] = {256, 512, 1024};
	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/

	
	dummy=zbuf1;		
	nmlt = nmltTab[tabidx];		
	zbuf2 = zbuf1 + nmlt +(- 1);
	csptr = cos4sin4tab + cos4sin4tabOffset[tabidx];

	/* whole thing should fit in registers - verify that compiler does this */
	for (i = nmlt >> 2; i !=0 ; i--) {
		/* cps2 = (cos+sin), sin2 = sin, cms2 = (cos-sin) */
		ar1 = *(zbuf1)>>es;
		ai1 = *(zbuf2)>>es;	
		ai2 = *(zbuf1 + 1) >> es;
		
		/* gain 1 int bit from MULSHIFT32, but drop 2, 3, or 4 int bits from table scaling */
		t  = MULSHIFT32(*(csptr+1),  ar1+ai1);
		*zbuf1= MULSHIFT32(*csptr -2*(*(csptr+1)), ar1) + t;
		*(zbuf1+1) = MULSHIFT32(*csptr, ai1) +(-t);
		
			
		ar1 = *(zbuf2 - 1) >> es;	/* do here to free up register used for es */
		t  = MULSHIFT32(*(csptr+3), ar1+ai2);
		*(zbuf2-1) = MULSHIFT32(*(csptr+2) -2*(*(csptr+3)), ar1) + t;
		*zbuf2 = MULSHIFT32(*(csptr+2), ai2) +(-t);
		csptr=csptr+4;
		zbuf2=zbuf2-2;
		zbuf1=zbuf1+2;

	}
	
#ifdef __DMA_XTRAMUL__
	 dma_transfer_wait(&D_XTRATAB_SRC, &D_XTRATAB_DST, &D_XTRATAB_SIZ);
#endif
	 
#ifdef __DMA_POSTMUL__
	/* see comments in PreMultiply() for notes on scaling */
	dma_transfer_wait(&D_POSTMUL_TAB_SRC, &D_POSTMUL_TAB_DST, 0x1e40/*&D_POSTMUL_TAB_SIZ*/);
#endif
    
	PostMultiplyRescale(tabidx, dummy, es);	
	return;
}

/**************************************************************************************
 * Function:    PostMultiplyRescale
 *
 * Description: post-twiddle stage of MDCT, with rescaling for extra guard bits
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *              number of guard bits to remove from output
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       clips output to [-2^30, 2^30 - 1], guaranteeing at least 1 guard bit
 *              see notes on PostMultiply(), above
 **************************************************************************************/
static void PostMultiplyRescale(int tabidx, int *fft1, int es)
{
	int i, nmlt,skipFactor, z,ar2;
	int t, cs2, sin2;
	int *fft2;
	const int *csptr;

	const int nmltTab[NUM_MLT_SIZES] = {256, 512, 1024};
	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/


	R4FFT(tabidx, fft1);
	nmlt = nmltTab[tabidx];	

	
	csptr = cos1sin1tab;
	//intf("\n\n cos1sin1 -- %x\n\n",csptr);
	
	skipFactor = postSkip[tabidx]+1;
	fft2 = fft1 + nmlt - 1;

	/* load coeffs for first pass
	 * cps2 = (cos+sin)/2, sin2 = sin/2, cms2 = (cos-sin)/2
	 */
	cs2 = *csptr;
	sin2 = *(csptr+1);
	

	for (i = nmlt >> 2; i !=0 ; i--) {
		

		/* gain 1 int bit from MULSHIFT32, and one since coeffs are stored as 0.5 * (cos+sin), 0.5*sin */
		t = MULSHIFT32(sin2,*(fft1 ) + *(fft1 + 1) );
		z = t + MULSHIFT32(cs2+(-2*sin2),  *(fft1 ));	
		CLIP_2N_SHIFT(z, es);
		*fft1 = z;
		ar2 = t +(-MULSHIFT32(cs2, *(fft1 + 1)));	
		CLIP_2N_SHIFT(ar2, es);	 


		csptr =csptr+ skipFactor;
		cs2 = *csptr;
		sin2 = *(csptr+1);
		

		
		t = MULSHIFT32(sin2, *(fft2-1) + (-*(fft2 )));
		z = t + MULSHIFT32(cs2+(- 2*sin2), *(fft2-1));	
		CLIP_2N_SHIFT(z, es);
		*(fft1+1) = z;
		z = t +(-MULSHIFT32(cs2, (-*(fft2 ))));	
		CLIP_2N_SHIFT(z, es);	 
		*(fft2-1) = z;
		*fft2 = ar2;
		fft2=fft2-2;
		fft1=fft1+2;
	}

	/* see comments in PostMultiply() for notes on scaling */
	return;	
}
/**************************************************************************************
 * Function:    IMLTNoWindow
 *
 * Description: inverse MLT without window or overlap-add
 *
 * Inputs:      table index (for transform size)
 *              buffer of nmlt samples
 *              number of guard bits in the input buffer
 *
 * Outputs:     processed samples in same buffer
 *
 * Return:      none
 *
 * Notes:       operates in-place, and generates nmlt output samples from nmlt input
 *                samples (doesn't do synthesis window which expands to 2*nmlt samples)
 *              if number of guard bits in input is < GBITS_IN_IMLT, the input is 
 *                scaled (>>) before the IMLT and rescaled (<<, with clipping) after
 *                the IMLT (rare)
 *              the output has FBITS_LOST_IMLT fewer fraction bits than the input
 *              the output will always have at least 1 guard bit
 **************************************************************************************/
void IMLTNoWindow(int tabidx, int *mlt, int gb)
{
	int es;

	/******** Stack point value **************
	iStackCounter = __R15;

	if(iStackCounter < iLastCount)
	{
		iLastCount = iStackCounter;
	}
	/*****************************************/

#ifdef __DMA_PREMUL__
		 dma_transfer_wait(&D_PREMUL_TAB_SRC, &D_PREMUL_TAB_DST, &D_PREMUL_TAB_SIZ);
#endif
				

	/* fast in-place DCT-IV - adds guard bits if necessary */
	if (gb < GBITS_IN_IMLT) {
		es = GBITS_IN_IMLT - gb;
		PreMultiplyRescale(tabidx, mlt, es);
		
	} else {
				
		PreMultiply(tabidx, mlt);
		
	}
	return;
}

