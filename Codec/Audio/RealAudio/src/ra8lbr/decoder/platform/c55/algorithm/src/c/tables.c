/* ***** BEGIN LICENSE BLOCK *****   
 * Source last modified: $Id: tables.c,v 1.1 2005/02/18 23:30:41 hfrederickson Exp $ 
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
 * NOTES:
 * Huffman codes are now generated at runtime, from code-length tables.
 */

#include "coder.h"

/*
char *redHerrings[] = {
	"ERROR: fatal error occurred in %s, line %d.",
	"ERROR: malloc failed.",
	"WARN: frame[%4d] LPC filter not stable, damping...",
	"WARN: frame[%4d] unexpected escape-code, skipping...",
	"STAT: frame[%4d] switching to short transform.",
	"STAT: frame[%4d] switching to long transform.",
	"STAT: frame[%4d] codebook %d selected.",
	"STAT: frame[%4d] codebook %d updated.",
	"STAT: frame[%4d] required %d bits [%d %d %d %d].",
	"enabled",
	"disabled",
	"STAT: post-filter is %s.",
	"STAT: LPC prediction is %s.",
	"STAT: temporal prediction is %s.",
	"STAT: adaptive codebook is %s.",
};
*/

/*
 * Delta power coding.
 * NOTE:
 * for now, replicate last code...
 * big enough for 1024 = 51 regions
 */

const char RA_TNI_expbits_tab[8] = { 52, 47, 43, 37, 29, 22, 16, 0 };

#ifndef DECODEVECTORS_ASM
const char RA_TNI_kmax_tab[7] = { 14, 10, 7, 5, 4, 3, 2 };

/*
 * This table replaces (i / (kmax+1)) with (unsigned)(i * inv) >> nbits.
 * Note that this must be EXACT for all i, kmax that are in use!
 *
 * Using these gives exact result for all i<4096 and (kmax+1)<256
 *     nbits = 15; 20 previously -- Venky
 *     inv = ((1<<nbits) + kmax) / (kmax+1);
 *
 * To allow larger kmax, or Huffman table bigger than 4095,
 * must redesign and exhaustively verify exactness!
 */
/* const int invradix_tab[7] = { 74899, 104858, 149797, 209716, 262144, 349526, 524288 }; */
const short RA_TNI_iradix_tab[7] = { 2341, 3277, 4682, 6554, 8192, 10923, 16384 };
#else // DECODEVECTORS_ASM

/* combinatrion of kmax and iradix tab */
#pragma DATA_SECTION( RA_TNI_kmax_iradix_tab, "dec_vectors_section")
#pragma DATA_ALIGN( RA_TNI_kmax_iradix_tab, 2)

const short	RA_TNI_kmax_iradix_tab[14] = {	/*kmax*/	/*iradix*/
										 14,		 2341,
										 10,		 3277,
										 7,			 4682,
										 5,			 6554,
										 4,			 8192,
										 3,			 10923,
										 2,			 16384
										};
#endif // DECODEVECTORS_ASM

/*
 * SQVH
 */

/* Vector params for 20 MLT regions */
#ifndef DECODEVECTORS_ASM
const char RA_TNI_vd_tab[7]  =	{	2,	2,	2,	4,	4,	5,	5	};
const char RA_TNI_vpr_tab[7] =	{	10,	10,	10,	5,	5,	4,	4	};
#else // DECODEVECTORS_ASM
/* combination of vpr and vd tabs */
#pragma DATA_SECTION( RA_TNI_vpr_vd_tab, "dec_vectors_section")
#pragma DATA_ALIGN( RA_TNI_vpr_vd_tab, 2)
const char RA_TNI_vpr_vd_tab[14] = {	/* vpr */	/*vd*/
										 10,		2,
										 10,		2,
										 10,		2,
										 5,			4,
										 5,			4,
										 4,			5,
										 4,			5
									};
										 
									
#endif // DECODEVECTORS_ASM

/* Dequantization:
 * RA_TNI_fix_quant_centroid_tab: mantissas are 1.31 signed, scaled as man * 2^exp
 * these are all positive values, and the sign bit is used to take a two's complement
 *	(see ScalarDequant())
 * We only have non-zero entries up to kmax[j] in row j
 */
const long RA_TNI_fqct_man_0[14] = { 0x00000000, 0x645a1cac, 0x616872b0, 0x47ae147b, 0x5e872b02, 0x753f7cee, 0x45db22d1, 
									 0x514fdf3b, 0x5c9374bc, 0x67d70a3d, 0x7322d0e5, 0x7e24dd2f, 0x449ba5e3, 0x4b958106 };
									 
const long RA_TNI_fqct_man_1[14] = { 0x00000000, 0x45a1cac1, 0x43d70a3d, 0x64083127, 0x422d0e56, 0x5245a1cb, 0x624dd2f2, 
									 0x71fbe76d, 0x411eb852, 0x49eb851f, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };									 

const long RA_TNI_fqct_man_2[14] ={ 0x00000000, 0x5f7ced91, 0x5db22d0e, 0x45c28f5c, 0x5c395810, 0x72b020c5, 0x450e5604, 
									0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 } ;

const long RA_TNI_fqct_man_3[14] ={ 0x00000000, 0x40624dd3, 0x40000000, 0x5fc6a7f0, 0x7f851eb8, 0x00000000, 0x00000000, 
									0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

const long RA_TNI_fqct_man_4[14] ={ 0x00000000, 0x548b4396, 0x567ef9db, 0x7f74bc6a, 0x00000000, 0x00000000, 0x00000000, 
									0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

const long RA_TNI_fqct_man_5[14] ={ 0x00000000, 0x6a0c49ba, 0x6fb645a2, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
									0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

const long RA_TNI_fqct_man_6[14] ={ 0x00000000, 0x7db22d0e, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 
									0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

#pragma DATA_SECTION (RA_TNI_fqct_man, "fqct_section")
#pragma DATA_ALIGN (RA_TNI_fqct_man,2)
const long* const RA_TNI_fqct_man[7] = {
	RA_TNI_fqct_man_0 ,
	RA_TNI_fqct_man_1 ,
	RA_TNI_fqct_man_2 ,
	RA_TNI_fqct_man_3 ,
	RA_TNI_fqct_man_4 ,
	RA_TNI_fqct_man_5 ,
	RA_TNI_fqct_man_6 ,
};

const char RA_TNI_fqct_exp_0[14] = { 0, -1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3};

const char RA_TNI_fqct_exp_1[14] = { 0,  0, 1, 1, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0};

const char RA_TNI_fqct_exp_2[14] = { 0,  0, 1, 2, 2, 2, 3, 0, 0, 0, 0, 0, 0, 0};

const char RA_TNI_fqct_exp_3[14] = { 0,  1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const char RA_TNI_fqct_exp_4[14] = { 0,  1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const char RA_TNI_fqct_exp_5[14] = { 0,  1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

const char RA_TNI_fqct_exp_6[14] = { 0,  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#pragma DATA_SECTION (RA_TNI_fqct_exp, "fqct_section")
#pragma DATA_ALIGN (RA_TNI_fqct_exp,2)
const char* const RA_TNI_fqct_exp[7] = {
	RA_TNI_fqct_exp_0,
	RA_TNI_fqct_exp_1,
	RA_TNI_fqct_exp_2,
	RA_TNI_fqct_exp_3,
	RA_TNI_fqct_exp_4,
	RA_TNI_fqct_exp_5,
	RA_TNI_fqct_exp_6
	
};

/* Dither amplitudes, 1.31 signed */
const long RA_TNI_fixdither_tab[8] = {0, 0, 0, 0, 0, 0x16a0a0f5, 0x20000000, 0x5a827b70};	/* appx = sqrt(2)/8, 1/4, sqrt(2)/2 */
#if !defined  DECODEVECTORS_NEW || !defined DECODEVECTORS_ASM
/*
 * Implements sqrt(pow(2,n)) for n = -63..63
 * mantissas: 1.31 signed
 * exponents: integers
 * ans: man * 2^exp
 */
const long RA_TNI_fixrootpow2man[2] =	{
	/* only two elements are required -- Venky */
	0x5a82799a, 0x40000000, /* 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a, 0x40000000,
	0x5a82799a, 0x40000000, 0x5a82799a */
};
#else	// DECODEVECTORS_NEW	DECODEVECTORS_ASM
// A new way of doing the same thing.
// This will eliminate the need to add 63
const long RA_TNI_fixrootpow2man_new[2] = {
	0x40000000,	0x5a82799a
};
#endif //DECODEVECTORS_NEW		
		
/* The following array has been replaced by -31 + ((i+63)+1)/2 -- Venky */
/*
const char RA_TNI_fixrootpow2exp[127] = {
	-31, -30, -30, -29,
	-29, -28, -28, -27,
	-27, -26, -26, -25,
	-25, -24, -24, -23,
	-23, -22, -22, -21,
	-21, -20, -20, -19,
	-19, -18, -18, -17,
	-17, -16, -16, -15,
	-15, -14, -14, -13,
	-13, -12, -12, -11,
	-11, -10, -10,  -9,
	 -9,  -8,  -8,  -7,
	 -7,  -6,  -6,  -5,
	 -5,  -4,  -4,  -3,
	 -3,  -2,  -2,  -1,
	 -1,   0,   0,   1,
	  1,   2,   2,   3,
	  3,   4,   4,   5,
	  5,   6,   6,   7,
	  7,   8,   8,   9,
	  9,  10,  10,  11,
	 11,  12,  12,  13,
	 13,  14,  14,  15,
	 15,  16,  16,  17,
	 17,  18,  18,  19,
	 19,  20,  20,  21,
	 21,  22,  22,  23,
	 23,  24,  24,  25,
	 25,  26,  26,  27,
	 27,  28,  28,  29,
	 29,  30,  30,  31,
	 31,  32,  32
};
*/
