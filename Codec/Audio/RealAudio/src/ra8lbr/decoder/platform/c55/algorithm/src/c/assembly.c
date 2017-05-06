/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: assembly.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//																												//
//		File 	: Assembly.c																				    //	
//		Desc 	: Core routines that should be converted to native/assembly 									//
//																												//
//		NOTE 	: To use the assembly version of these rtns include the file assembly.asm						//
//					and define the pre-processor symbol __OPTIMISE_FIRST_PASS__ for both compiler and assembler //
//																												//	
//																												//
//																												//
//																												//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "coder.h"
#include "assembly.h"

#ifdef C_PROFILE
unsigned long mkshortcount = 0 ;
#endif

#ifndef MULSHI
long mulshi(long x, long y, short h);

#ifndef __OPTIMISE_FIRST_PASS__
long mulshi(long x, long y, short h)
{
	unsigned short xl, yl;
	short xh, yh;
    unsigned long xmidlo, xmidhi;
	long xhyl, xlyh, xhyh, xlyl;

    if((x==0) || (y==0)) return(0);
    
	/* xh = (x & 0xFFFF0000)>>16; */
    xh = x>>16;
    xl = x & 0xFFFF;

    /* yh = (y & 0xFFFF0000)>>16; */
    yh = y>>16;
    yl = y & 0xFFFF;

    xhyl = (long)xh * (unsigned long)yl;
    xlyh = (unsigned long)(xl) * (long)(yh);
    xhyh = (long)xh * (long)yh;
    xlyl = (unsigned long)xl * (unsigned long)yl;

    xmidlo = ((unsigned long)xlyl>>16) + ((xlyh & 0xFFFF) + (xhyl & 0xFFFF));
	xmidhi = (xmidlo >> 16) + (xhyh & 0xFFFF);
	/* xmidhi += (xhyh & 0xFFFF); */
	xmidhi += (xlyh>>16) + (xhyl>>16);
	xhyh = ((unsigned long)xhyh>>16) + (xmidhi>>16);

	/* xmidlo = xmidlo & 0xFFFF; */
	/* xlyl = (xlyl & 0xFFFF); */
	/* xmidhi &= 0xFFFF; */

	x = (xhyh<<16) + (xmidhi & 0xFFFF);

	if(h)
	{
		/* x = (x<<h); */
		/* xmidlo = ((unsigned long)(xmidlo & 0xFFFF)>>(16-h)); */
		x = (x<<h) | ((unsigned long)(xmidlo & 0xFFFF) >> (16- h));
	}

	return(x);
}

long RA_TNI_MULSHIFT0(long x, long y)
{	
	
	return(mulshi(x,y,0));
}

long RA_TNI_MULSHIFT1(long x, long y)
{
	return(mulshi(x,y,1));
}

long RA_TNI_MULSHIFT2(long x, long y)
{
	return(mulshi(x,y,2));
}


//#endif		//__OPTIMISE_FIRST_PASS__


long RA_TNI_MULSHIFTNCLIP(long x, long y, short clip)
{
	unsigned short xl, yl;
	short xh, yh;
	unsigned long xmidlo, xmidhi;
	long xhyl, xlyh, xhyh, xlyl;
	long temp_xh;

	/* char n8 = (char)clip; */

	if((x==0) || (y==0)) return(0);

	xh = x>>16;
	xl = x & 0xFFFF;

	yh = y>>16;
	yl = y & 0xFFFF;
	xlyl = (unsigned long)xl * (unsigned long)yl;
	xhyl = (long)xh * (unsigned long)yl;
	xlyh = (unsigned long)xl * (long)yh;
	xhyh = (long)xh * (long)yh;

	xmidlo = ((unsigned long)xlyl>>16) + ((xlyh & 0xFFFF) + (xhyl & 0xFFFF));
	xmidhi = (xmidlo >> 16) + (xhyh & 0xFFFF);
	/* xmidhi += (xhyh & 0xFFFF); */
	xmidhi += (xlyh>>16) + (xhyl>>16);
	xhyh = ((unsigned long)xhyh>>16) + (xmidhi>>16);

	/* xmidlo = xmidlo & 0xFFFF; */
	/* xmidhi &= 0xFFFF; */

	x = (xhyh<<16) | (xmidhi & 0xFFFF);

	/* xlyl = (xlyl & 0xFFFF); */
	y = (xlyl & 0xFFFF) | ((unsigned long)(xmidlo & 0xFFFF)<<16);

	temp_xh = (x>>(31-clip));

	x = (x<<clip);
	/* clip = 32-clip; */
	y = ((unsigned long)y>>(32-clip));
	x |= y;

/*
	clip--;
	temp_xh >>= clip;
*/
	x = (temp_xh > 0) ? 0x7FFFFFFF : ((temp_xh < -1) ? 0x80000000 : x);

	return(x);
}


#endif		//__OPTIMISE_FIRST_PASS__


#ifndef INTERP_ASM
/* usually leave this in C to let compiler optimize with it */
short RA_TNI_MAKESHORT(long x, long y)
{
	long z;/*, posclip, negclip;*/
	short ret_val;
	z = x + y;

#ifdef C_PROFILE
		mkshortcount++;
#endif
	if (z > (long)POSCLIP)
	{
		ret_val = (short)((unsigned long)POSCLIP >> PCMFBITS);
		return (ret_val);
	}
	else if (z < (long)NEGCLIP)
	{
		ret_val = (short)((unsigned long)NEGCLIP >> PCMFBITS);
		return (ret_val);
	}
	
	ret_val = RNDFIXTOS(z);

	return(ret_val);
	

}
#endif  // INTERP_ASM


#endif

/* twiddle factors are stored as 1.31 signed, to cover range (-1, 1)
 * we gain one guard bit each pass (total passes = FFT order - 3)
 *   because of the signed multiply
 * this should be sufficient to cover the maximum bit growth predicted
 *	 with the law of large numbers
 * the original C code for the butterfly loop is included below
 */
#ifndef ASM
#ifndef __OPTIMISE_FIRST_PASS__
void RA_TNI_BUTTERFLY(long *zptr1, long *zptr2, const long *ztwidptr, short bg)
{
	short i;
	long ar, ai, br, bi, cr, ci;
	long rtemp, itemp;

	cr = *(ztwidptr+0);
	ci = *(ztwidptr+1);

	for (i = 0; i<bg; i++) {
		br = *(zptr2+0);
		bi = *(zptr2+1);
#ifndef __OPTIMISE_FIRST_PASS__
        rtemp = RA_TNI_MULSHIFT0(br,cr) + RA_TNI_MULSHIFT0(bi,ci);
		itemp = RA_TNI_MULSHIFT0(br,ci) - RA_TNI_MULSHIFT0(bi,cr);
#else
        rtemp = RA_TNI_MULSHIFT0(&br,&cr) + RA_TNI_MULSHIFT0(&bi,&ci);
		itemp = RA_TNI_MULSHIFT0(&br,&ci) - RA_TNI_MULSHIFT0(&bi,&cr);
#endif
		
		ar = *(zptr1+0) >> 1;		/* shift for addition */
		ai = *(zptr1+1) >> 1;

		*(zptr1+0) = ar + rtemp;
		*(zptr2+0) = ar - rtemp;

		*(zptr2+1) = ai + itemp;
		*(zptr1+1) = ai - itemp;

		zptr1 += 2;
		zptr2 += 2;
	}
	return;
}
#endif
#endif
