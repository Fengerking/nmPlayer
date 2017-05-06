/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: huffman.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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
 * Huffman coding functions.
 *
 * Huffman decoding trees are stored as array of nodes (int [2])
 * that point to child nodes (if positive) or store the leaf value
 * (if negative).  Since values can be zero, store as (-value-1)
 * to free zero to represent uninitialized nodes.
 *
 * Example, for code=6 with value=98:
 *	tree[0][1] -> 2		next node at 2
 *	tree[2][1] -> 7		next node at 7
 *	tree[7][0] -> -99	leaf! value = 99-1 = 98
 */
#include "RA_tni_priv.h"
#ifndef _HUFFMAN_TABLES_

#include "coder.h"

/*
 * Huffman decode the next symbol.
 * Returns nbits if successful, or zero if out-of-bits.
 *
 * Fixed-point changes:
 *  - none
 */
 
#ifdef _PROFILE_TREES_
// generate tree usage profile data
short RA_TNI_DecodeNextSymbol(const HUFFNODE *tree, TreeProfile*profile_tree, USHORT *val, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	short nbits;
	short node, bit;

	nbits = 0;
	node = 0;	/* start at root */

	profile_tree->numCalls++;
	do {
		nbits++;
		if (!RA_TNI_Unpackbit((USHORT *)&bit, pkbit, pktotal, bitstrmPtr))
			return 0;		/* out of bits */
		else
			node = tree[node][bit];

	 } while (node > 0);	/* node < 0 is a leaf */

	ASSERT(node != 0);		/* uninitialized node! */

	/* Now at the leaf */
	*val = -node - 1;		/* recover value */
	
	profile_tree->totalBits += nbits ;
	profile_tree->maxBits = ( profile_tree->maxBits > nbits) ? profile_tree->maxBits : nbits ;
	
	profile_tree->HitsForBits[nbits-1]++;	//score up a hit for these no of bits
	
	return nbits;
}

#else 
short RA_TNI_DecodeNextSymbol(const HUFFNODE *tree, USHORT *val, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr)
{
	short nbits;
	short node, bit;

	nbits = 0;
	node = 0;	/* start at root */

	do {
		nbits++;
		if (!RA_TNI_Unpackbit((USHORT *)&bit, pkbit, pktotal, bitstrmPtr))
			return 0;		/* out of bits */
		else
			node = tree[node][bit];

	 } while (node > 0);	/* node < 0 is a leaf */

	ASSERT(node != 0);		/* uninitialized node! */

	/* Now at the leaf */
	*val = -node - 1;		/* recover value */
	return nbits;
}
#endif



#endif	// _HUFFMAN_TABLES_

