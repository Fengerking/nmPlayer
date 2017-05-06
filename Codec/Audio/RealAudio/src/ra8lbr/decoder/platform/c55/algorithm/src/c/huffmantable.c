/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: huffmantable.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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

//-------------------------------------------------------------------------
//	File : huffmantable.c
//
//	Definition of functions to perform Huffman decoding using cascaded lookup tables
//
//	Uses n-3-3 type of cascaded tables.
//		
//-------------------------------------------------------------------------
#include "RA_tni_priv.h"
#ifndef HUFFTABLE_ASM
#ifdef _HUFFMAN_TABLES_

#include "huffmantable.h"

const int FIXTABSIZE = 0x03 ;	// fixed table size for non-level 1 tables

#ifndef PEEKBIT_ASM

// removes nbits ...used to commit a peek operation. Updates globals
// Lightweight version of Unpackbits
short RA_TNI_Removebits(short nbits, /*USHORT *data,*/ short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	*pktotal += nbits;
	/* if (pktotal > pklimit) */
	if (*pktotal > bitstrmPtr->nframebits)
		return 0;	/* out of bits */

	*pkbit += nbits;
	if (*pkbit >= 32) {							/* into next word? */
		bitstrmPtr->pkptr++;
		*pkbit -= 32;
	}
	return nbits;
}



// Peek 'nbits' no. of bits from the bitstream. Do not update any global pointers.
short RA_TNI_Peekbits(short nbits, USHORT *data, short pkbit, short pktotal, RA_TNI_Obj *bitstrmPtr)
{
	ULONG temp;
	ULONG *pkptr = bitstrmPtr->pkptr;
	
	/* USHORT mydata; */

	// work on copies of these parameters , not on the originals
		
	if (pktotal + nbits > bitstrmPtr->nframebits)
		/* out of bits */
		/* read as many as possible */
		nbits = bitstrmPtr->nframebits - pktotal ;
		// re-adjust nbits

	temp = (*(pkptr) << pkbit);					/* left-justify data */
	pkbit += nbits;

	if (pkbit >= 32) {							/* into next word? */
		pkptr++;
		pkbit -= 32;
		if (pkbit) {							/* if any spillover, */
			temp |= *(pkptr) >> (nbits - pkbit);	/* unpack the rest */
		}
	}
	*data = (USHORT)(temp >> (32-nbits));		/* right-justify result */
	return nbits;
}

#endif // PEEKBIT_ASM



//int debug_count = 0;

// decode the next code from the stream..using the specified stream.
// Returns -1 for error in decoding process
// On success returns no. of bits read from stream.
short RA_TNI_DecodeNextSymbolWithTable( HuffmanTable table, int n, USHORT *val, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	short nbits;
	short retbits,bitfield;
	int  node, retbits1 ;
	short flag , symbol;
	int bitcount ;
	int *myptr ;
	nbits = 0;
	node = 0;	/* start at root */

	do {

		retbits =	RA_TNI_Peekbits( n, &bitfield, *pkbit, *pktotal, bitstrmPtr) ; 		// peek 'n' bits from stream
		if( !retbits  )	{
			/*RA_TNI_Unpackbits( n, &bitfield, pkbit, pktotal, bitstrmPtr);	// commit the operation */
			RA_TNI_Removebits( n,  pkbit, pktotal, bitstrmPtr);	// commit the operation 
			return 0 ;	// totally failed
		}

		if( retbits != n )										// could not read reqd no. of bits
			bitfield = bitfield << ( n - retbits);				// pad 0's
// Bitfields do not work in C : Must do in assembly
//		myptr = (int*)(&table[bitfield]) ;		// read 16 bits
//		node = *myptr ;
//		bitcount = (node << 11) >>11		; //isolate lower 5 bits retain the sign
//		symbol = (node >> 5) & 0x7FF 		; // upper 11 bits 		
		
		bitcount = table[bitfield].bitcount ;
		flag = ( bitcount < 0 ) ? -1 : 0 ;
		bitcount = ( bitcount < 0 ) ? -bitcount : bitcount ;

		//commit the read operation
	/*	retbits = RA_TNI_Unpackbits(  table[bitfield].bitcount, &node, pkbit, pktotal, bitstrmPtr ) ;*/
		retbits = RA_TNI_Removebits( /* table[bitfield].*/bitcount,  pkbit, pktotal, bitstrmPtr ) ;
		nbits += retbits ;

		if( retbits != /*table[bitfield].*/bitcount ) 
				return 0 ;			// out of bits
		
		symbol = table[bitfield].symbol ;
		
		// look up table
		if( /*table[bitfield].*/flag == 0 )	{		// a hit in the table
			*val = symbol ;
			return nbits;
		}
		else {		// another table lookup
			if( table[bitfield].symbol == 0 )
					return -1 ;
			table = table +symbol;	
		/*	n = n -1 ;		// reduced size*/
			n = FIXTABSIZE ;	// fixed size table lookup
		}
		
	} while (1);	/* node < 0 is a leaf */


	
}

#endif // _HUFFMAN_TABLES_

#endif //HUFFTABLE_ASM

