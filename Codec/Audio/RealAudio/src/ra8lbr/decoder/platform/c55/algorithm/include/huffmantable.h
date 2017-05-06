/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: huffmantable.h,v 1.1 2005/02/18 23:30:39 hfrederickson Exp $ 
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
//	File : huffmantable.h
//
//	Definition of datatypes and declaration of functions needed
//	to perform Huffman decoding using cascaded lookup tables
//
//	Uses n-3-3 type of cascaded tables.
//-------------------------------------------------------------------------
#ifdef _HUFFMAN_TABLES_

#ifndef _HUFFMANTABLE_C_
#define _HUFFMANTABLE_C_
#include "RA_tni_priv.h"

// old version

/*typedef struct HUFFMANTABLENODE
{
	struct {
		short symbol;		// code => symbol decompanding		// doubles as offset
		struct HUFFMANTABLENODE *ptrTab;	// pointer to cascaded table
	} payload ;

	int bitcount ;		// bit count of code for symbol
	int flag ;			// -1 => ptrTab is active field
						// 0 => symbol field	


} HuffmanTableNode ;*/

#ifndef HUFFTABLE_ASM		// if using C version of Huffman table routines
typedef struct HUFFMANTABLENODE
{

	int symbol/*:11*/;		// code => symbol decompanding		
						// doubles as offset into table
	
	int bitcount /*:5*/ ;		// bit count of code for symbol / offset
						
} HuffmanTableNode ;
#else 	//HUFFTABLE_ASM					// if using asm version of Huffman table routines
typedef struct HUFFMANTABLENODE
{

	int symbol:11;		// code => symbol decompanding		
						// doubles as offset into table
	
	int bitcount :5 ;		// bit count of code for symbol / offset
						
} HuffmanTableNode ;
#endif //HUFFTABLE_ASM



typedef HuffmanTableNode *HuffmanTable;



short RA_TNI_DecodeNextSymbolWithTable( HuffmanTable table, int n, USHORT *val, short *pkbit, short *pktotal, RA_TNI_Obj *bitstrmPtr);



#endif // _HUFFMANTABLE_C_

#endif // _HUFFMAN_TABLES_


