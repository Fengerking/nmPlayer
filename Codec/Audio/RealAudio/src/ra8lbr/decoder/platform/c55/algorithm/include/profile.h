/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: profile.h,v 1.1 2005/02/18 23:30:39 hfrederickson Exp $ 
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

//-------------------------------------------------------------------
// tree profile.h
//
// Definition of datatype to generate profile information
// 	about huffman tree usage.
//-------------------------------------------------------------------


#ifndef _TREE_PROFILE_H__
#define _TREE_PROFILE_H__

typedef struct 
{
	ULONG numCalls ;	// no of times this tree was processed
	ULONG totalBits ;	// total no. of code bits searched
	ULONG maxBits	;   // max code bit length
	ULONG HitsForBits[16]	 ; // the no. of hits per code bit length [1-16]
} TreeProfile ;



extern TreeProfile* const PROFILE_cpl_tree_tab[7] ;

extern TreeProfile PROFILE_power_tree[50];

extern TreeProfile* const PROFILE_sqvh_tree[7] ;




#endif //_TREE_PROFILE_H__


