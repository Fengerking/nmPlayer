/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: assembly.h,v 1.1 2005/02/18 23:30:39 hfrederickson Exp $ 
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

/* assembly.h
 * --------------------------------------
 *
 * Written by:			Jon Recker
 *
 * Last modified by:	Jon Recker (jrecker@real.com)
 *						01/24/01
 *
 * - inline rountines with access to 64-bit multiply results
 * - x86 and ARM 7 versions included
 * - some inline functions are mix of asm and C for speed (see note below)
 *
 * Inline functions
 * RA_TNI_MULSHIFT0(x, y):			signed multiply of two 32-bit integers (x and y), returns top 32-bits of 64-bit result
 * RA_TNI_MULSHIFT1(x, y):			same as RA_TNI_MULSHIFT0 but drops MSB (does << 1 on the 64-bit result then returns top 32 bits)
 * RA_TNI_MULSHIFT2(x, y):			same as RA_TNI_MULSHIFT0 but drops 2 MSB's (does << 2 on the 64-bit result then returns top 32 bits)
 * RA_TNI_MULSHIFTNCLIP(x, y, n):	signed multiply of two 32-bit integers (x and y), rescales answer by left-shifting the
 *								64-bit result by n bits and returning the new top 32 bits, saturating if necessary
 *								(useful when abs(x*y) is only rarely larger than some maximum value)
 * RA_TNI_MAKESHORT(x, y):			adds overlapping transform windows and rescales to 16-bit PCM, clipping if necessary
 * RA_TNI_BUTTERFLY(...):			butterfly loop for FFT, made into a single function because calling lots of individual
 *								multiplies from the normal C code introduces lots of function overhead
 */

#ifndef _RA_TNI_ASSEMBLY_H_
#define _RA_TNI_ASSEMBLY_H_

/* macros for converting int to short (platform-independent) */
#define PCMFBITS			(32 - POSTBITS - NINTBITS)	/* fraction bits */
#define RNDMASK				(1 << (PCMFBITS - 1))
#define POSCLIP				((((unsigned long)RNDMASK << 16) - 1) - (((unsigned long)RNDMASK << 1) - 1))
#define NEGCLIP				(((long)~POSCLIP) - (((long)RNDMASK << 1) - 1))
#define RNDFIXTOS(x)		(short)((long)(x + RNDMASK) >> PCMFBITS)

/* we group the platform-specific code by platform rather than by function because different compilers
 *   sometimes use different variants of the 'inline' keyword
 * it also seems easier to read this way, rather than each function being filled with #ifdef's
 * the price we pay is having to repeat the function prototype for each platform, which would
 *   make it messy if we ever changed the prototype
 * note that some platforms have certain functions in native asm files, so not all functions will
 *   necessarily be implemented here for each platform
 */
 
// These functions are converted during first pass optimization to take pointers
// rather than values - pass by reference

#ifndef __OPTIMISE_FIRST_PASS__
long RA_TNI_MULSHIFT0(long x, long y);
long RA_TNI_MULSHIFT1(long x, long y);
long RA_TNI_MULSHIFT2(long x, long y);
#else
long RA_TNI_MULSHIFT0( const long *x, const long *y);
long RA_TNI_MULSHIFT1( const long *x, const long *y);
long RA_TNI_MULSHIFT2( const long *x, const long *y);
#endif

#ifndef __OPTIMISE_FIRST_PASS__
long RA_TNI_MULSHIFTNCLIP(long x, long y, short clip);
#else
long RA_TNI_MULSHIFTNCLIP(const long *x, const long *y, short clip);
#endif
/* usually leave this in C to let compiler optimize with it */
short RA_TNI_MAKESHORT(long x, long y);

/* twiddle factors are stored as 1.31 signed, to cover range (-1, 1)
 * we gain one guard bit each pass (total passes = FFT order - 3)
 *   because of the signed multiply
 * this should be sufficient to cover the maximum bit growth predicted
 *	 with the law of large numbers
 * the original C code for the butterfly loop is included below
 */
//void RA_TNI_BUTTERFLY1(long *zptr1, long *zptr2, const long *ztwidptr, short bg);
void RA_TNI_BUTTERFLY(long *zptr1, long *zptr2, const long *ztwidptr, short bg);
#endif	/* _ASSEMBLY_H_ */
