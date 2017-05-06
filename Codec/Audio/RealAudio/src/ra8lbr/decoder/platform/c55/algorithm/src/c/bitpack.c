/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: bitpack.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 * Bitpacking functions.
 */

//		NOTE 	: To use the assembly version of these rtns include the file assembly.asm						//
//					and define the pre-processor symbol __OPTIMISE_FIRST_PASS__ for both compiler and assembler //
//	

#include "coder.h"

/*
 * Unpack bits from stream.
 * Returns nbits if successful, or zero if out-of-bits.
 * NOTES:
 * out-of-bits not needed here?
 *
 * Fixed-point changes:
 *  - none
 */
 
#ifndef UNPACKBIT_ASM
short RA_TNI_Unpackbits(short nbits, USHORT *data, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	ULONG temp;
	/* USHORT mydata; */

	*pktotal += nbits;
	/* if (pktotal > pklimit) */
	if (*pktotal > bitstrmPtr->nframebits)
		return 0;	/* out of bits */

	temp = (*(bitstrmPtr->pkptr) << *pkbit);					/* left-justify data */
	*pkbit += nbits;

	if (*pkbit >= 32) {							/* into next word? */
		bitstrmPtr->pkptr++;
		*pkbit -= 32;
		if (*pkbit) {							/* if any spillover, */
			temp |= *(bitstrmPtr->pkptr) >> (nbits - *pkbit);	/* unpack the rest */
		}
	}
	*data = (USHORT)(temp >> (32-nbits));		/* right-justify result */
	return nbits;
}
#endif  // UNPACKBIT_ASM
/*
 * Same, but optimized for single bit.
 * NOTES:
 * should be inline?
 * redo using only codebuf+codebits?
 *
 * Fixed-point changes:
 *  - none
 */

#ifndef UNPACKBIT_ASM
short RA_TNI_Unpackbit(USHORT *data, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	/* if (++pktotal > pklimit) */
	if (++(*pktotal) > bitstrmPtr->nframebits)
		return 0;	/* out of bits */

	*data = (USHORT)((*(bitstrmPtr->pkptr) << *pkbit) >> 31);
	bitstrmPtr->pkptr += (++(*pkbit) >> 5);
	*pkbit &= 0x1f;
	return 1;
}
#endif	// UNPACKBIT_ASM

/*
 * Byte packing routines use shift and mask to force big-endian
 * storage, regardless of machine byte order.
 */
#ifdef NOPACK
const long RA_TNI_pkmask[4] = { 0x00ffffff, 0xff00ffff, 0xffff00ff, 0xffffff00 };
const short RA_TNI_pkshift[4] = { 24, 16, 8, 0 };
const short RA_TNI_pkinc[4] = { 0, 0, 0, 1 };
const USHORT RA_TNI_pkkey[4] = { 0x37, 0xc5, 0x11, 0xf2 };

#else

#ifndef UNPACKBIT_ASM
const USHORT RA_TNI_pkkey[2] = { 0x37c5, 0x11f2 };
#endif //UNPACKBIT_ASM
const USHORT RA_TNI_pkkey4[4] = { 0x3700, 0xc500, 0x1100, 0xf200 };

#endif


#ifndef UNPACKBIT_ASM
/*
 * Data comes in a byte at a time (always big-endian) and is reorganized here
 *   into ints with whatever byte order the machine uses.
 * Also has simple scrambling scheme of XOR with byte masks
 *
 * Fixed-point changes:
 *  - none
 */
#ifdef NOPACK
void RA_TNI_DecodeBytes(UCHAR *codebuf, short nbits, ULONG *pkbuf)
{
	short i, idx, nbytes;

	/* unpack codebytes, from big-endian format */
	nbytes = nbits >> 3;
	for (i = 0; i < nbytes; i++) {
		idx = i & 0x3;
		*pkbuf &= RA_TNI_pkmask[idx];
		*pkbuf |= ((ULONG)(*codebuf++ ^ RA_TNI_pkkey[idx]) << RA_TNI_pkshift[idx]);
		pkbuf += RA_TNI_pkinc[idx];
	}
}
#else
void RA_TNI_DecodeBytes(USHORT *codebuf, short nbits, ULONG *pkbuf)
{
	short i, nwords, nbytes, idx;
	USHORT curSh;

	/* unpack codebytes, from big-endian format */
	nwords = ((nbytes = nbits >> 3) >>1);

	/*for (i = 0; i < nwords; i+=2) {
		curSh   = codebuf[i];
		*pkbuf &= 0x0000ffff;
		*pkbuf |= ((ULONG)(curSh ^ 0x37c5) << 16);
		pkbuf++;
	}
	pkbuf -= (i>>1);
	for (i = 1; i < nwords; i+=2) {
		curSh   = codebuf[i];
		*pkbuf &= 0xffff0000;
		*pkbuf |= (ULONG)(curSh ^ 0x11f2);
		pkbuf++;
	}
	
	if(nbytes & 0x1) {
		codebuf += nwords;
		idx = (nbytes & 0x3) - 1;
		*pkbuf = ((ULONG)(*codebuf++ ^ RA_TNI_pkkey4[idx]) << 16);
	}*/
	
	for( i = 0 ; i < nwords/2 ; i++)	{	
		curSh = *codebuf ;
		*codebuf++ = curSh ^ RA_TNI_pkkey[0] ;
		curSh = *codebuf ;
		*codebuf++ = curSh ^ RA_TNI_pkkey[1] ;
		pkbuf++;
	}
	
	if( nwords & 0x01 )
		*codebuf++ = *codebuf ^ RA_TNI_pkkey[0] ;
	
	if( nbytes & 0x01 ){
		idx = (nbytes & 0x03) - 1 ;
		//[fj] 09/02/2003 02:49 : dual mono mode changes
		*codebuf =  (*codebuf ^ pkkey4[idx] ) | (*codebuf & 0x00FF );
	}
	
	
}
#endif	//NOPACK

#endif	// UNPACKBIT_ASM

