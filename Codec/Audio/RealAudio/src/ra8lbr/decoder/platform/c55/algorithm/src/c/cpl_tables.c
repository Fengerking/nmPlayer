/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: cpl_tables.c,v 1.1 2005/02/18 23:30:40 hfrederickson Exp $ 
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

/*-------------------------------------------------------------------------*
 *	FileName : cpl_table.c												   *
 *																		   *
 *	Huffman lookup tables												   *
 *																		   *
 *																		   *
 *-------------------------------------------------------------------------*/
#ifdef _HUFFMAN_TABLES_

#include "cpl_tables.h"

HuffmanTable const RA_TNI_cpl_table_tab[7] = {
	( HuffmanTable )0,
	( HuffmanTable )0,
	( HuffmanTable )RA_TNI_cpl_table_2, 
	( HuffmanTable )RA_TNI_cpl_table_3,
	( HuffmanTable )RA_TNI_cpl_table_4, 
	( HuffmanTable )RA_TNI_cpl_table_5,
	( HuffmanTable )RA_TNI_cpl_table_6
};



const int RA_TNI_cpl_bitcount_tab[7] = {
	0,
	0,
	RA_TNI_CPL_TABLE_2_BIT_COUNT,
	RA_TNI_CPL_TABLE_3_BIT_COUNT,
	RA_TNI_CPL_TABLE_4_BIT_COUNT,
	RA_TNI_CPL_TABLE_5_BIT_COUNT,
	RA_TNI_CPL_TABLE_6_BIT_COUNT
};
#endif // _HUFFMAN_TABLES_
