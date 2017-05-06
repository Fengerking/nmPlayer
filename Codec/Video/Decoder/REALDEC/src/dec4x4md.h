/************************************************ BEGIN LICENSE BLOCK ******************************************* 
* Version: RCSL 1.0 and Exhibits. 
* REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
* Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
* All Rights Reserved. 
* 
* The contents of this file, and the files included with this file, are 
* subject to the current version of the RealNetworks Community Source 
* License Version 1.0 (the "RCSL"), including Attachments A though H, 
* all available at http://www.helixcommunity.org/content/rcsl. 
* You may also obtain the license terms directly from RealNetworks. 
* You may not use this file except in compliance with the RCSL and 
* its Attachments. There are no redistribution rights for the source 
* code of this file. Please see the applicable RCSL for the rights, 
* obligations and limitations governing use of the contents of the file. 
* 
* This file is part of the Helix DNA Technology. RealNetworks is the 
* developer of the Original Code and owns the copyrights in the portions 
* it created. 
* 
* This file, and the files included with this file, is distributed and made 
* available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
* INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
* FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
* 
* Technology Compatibility Kit Test Suite(s) Location: 
* https://rarvcode-tck.helixcommunity.org 
* 
* Contributor(s): 
* 
********************************************** END LICENSE BLOCK ******************************************************/ 
/*****************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
****************************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************
* dec4x4m.h -- functions for 4x4 transform coefficients decoding
* 03-21-00 06:38pm, written by Yuriy A. Reznik 
* 12-15-00 07:42pm, standalone decoder version (for embedded apps) 
*****************************************************************************************
Emuzed India Private Limited
******************************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
2.Modified signature of the function
These changes are to support combined    
VLD decoding and dequantization.
*******************************************************************************
*/
#ifndef __DEC4X4_H__
#define __DEC4X4_H__    1

#ifdef __cplusplus
extern "C" {  /* be nice to our friends in C++ */
#endif

#include "rvtypes.h"
/*
* A set of compression/decomresssion tables for each
* quantization range and frame type (intra/inter) modes:
*/

#define BS_MAXTRYBITS   16
/* maximum code length: */
#define MAX_DEPTH   16
#if MAX_DEPTH > BS_MAXTRYBITS
#error "BS_TRYBITS() must be able to process the longest codeword!!!"
#endif


/*
* 4x4 transform coefficients encoding.
*
* We attempt to group most frequently used combinations of events
* (values of coefficients) into units called descriptors.
*
* Thus, for each block we will use a 4x4 descriptor
* that combines most probable values of 4 lowest-frequency
* coefficients with indicators of non-zero elements in
* the remaining (high-frequency) 2x2 sub-blocks.
*
* In turn, if any of these 2x2 sub-blocks is coded, it is being
* represented by another descriptor, combining the most probable
* values of each of the coefficients.
*
* If any of the coefficints is larger than levels included
* in the descriptor, it is being encoded separately as
* <level + sign> pair, where level is the (relative) absolute
* value of the coefficient, and sign is a bit indicating whether
* the coefficient is positive or negative.
*/

/* extra level descriptor: */
#define MAX_EXTRA_LEVEL 23
#define MAX_EXTRA_BITS  8

/* abs. value (level) of quantized transform coefficients: */
#define MAX_LEVEL       (MAX_EXTRA_LEVEL + (1 << (MAX_EXTRA_BITS+1)) )

/* the number of level_dsc codewords: */
#define MAX_LEVEL_DSC   (MAX_EXTRA_LEVEL+MAX_EXTRA_BITS+1)

/*
* Block descriptors and level thresholds:
*
* 2x2 block descriptor:     l0 values:
*                             0 -> l0 = 0
*  l0 | l1                    1 -> abs(l0) = 1
*  ---+---                    2 -> abs(l0) = 2
*  l2 | l3                    3 -> abs(l0) >= 3
*
* 4x4 block descriptor:     l1..3 values:
*                             0 -> lx = 0
*  l0 | l1 |                  1 -> abs(lx) = 1
*  ---+----|   l4             2 -> abs(lx) >= 2
*  l2 | l3 |
*  --------+---------       l4..6 values:
*          |                  0 -> lx = 0
*    l5    |   l6             1 -> lx != 0
*          |                      -> 2x2 descriptor to follow
*/
#define MAX_0_LEVEL     4
#define MAX_123_LEVEL   3
#define MAX_456_LEVEL   2

/* the number of 2x2 descriptor codewords: */
#define MAX_2x2_DSC             \
	(MAX_0_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL)   /* 108 */

/* 4x4 descriptor: */
#define MAX_4x4_DSC             \
	(MAX_0_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL \
	* MAX_456_LEVEL * MAX_456_LEVEL * MAX_456_LEVEL)            /* 864 */

/*
* 8x8 block descriptors.
*
* These are the additional codewords that indicate which of
* the 4x4 blocks in the 8x8 region are actually coded.
*/
#define MAX_8x8_DSC 16

/*
* CBP macroblock descriptor:
*
*    y0  | y1    yi values:
*        |         0 -> the corresponding 8x8 luma block not coded
*  ------+------   1 -> 8x8 descriptor to follow
*    y2  | y3
*        |
*  +             ci values:
*     c0 | c1      0 -> cr[i] = 0 and cb[i] = 0
*     ---+---      1 -> (cr[i] = 0 & cb[i] != 0) or (cr[i] != 0 & cb[i] = 0)
*     c2 | c3         -> 1 bit to follow
*                  2 -> cr[i]!=0 and cb[i]!=0
*/
#define MAX_CBP     (16*3*3*3*3)    /* 1296 */

/* 4x4 descriptor tables: */
typedef struct {
	unsigned int next [MAX_DEPTH+2];
	unsigned int base [MAX_DEPTH+2];
	unsigned int offset [MAX_DEPTH+1];
	unsigned short symbol [MAX_4x4_DSC];
} _4x4_DSC_DECODE_TABLE;

/* 2x2 descriptor table: */
typedef struct {
	unsigned int next [MAX_DEPTH+2];
	unsigned int base [MAX_DEPTH+2];
	unsigned int offset [MAX_DEPTH+1];
	unsigned char symbol [MAX_2x2_DSC]; /* Flawfinder: ignore */
} _2x2_DSC_DECODE_TABLE;

/* extra large levels table: */
typedef struct {
	unsigned int next [MAX_DEPTH+2];
	unsigned int base [MAX_DEPTH+2];
	unsigned int offset [MAX_DEPTH+1];
	unsigned char symbol [MAX_LEVEL_DSC]; /* Flawfinder: ignore */
} LEVEL_DSC_DECODE_TABLE;

/* CBP encoding/decoding tables: */
typedef struct
{
	/* CBP main descriptor: */
	struct {
		unsigned int next [MAX_DEPTH+2];
		unsigned int base [MAX_DEPTH+2];
		unsigned int offset [MAX_DEPTH+1];
		unsigned short symbol [MAX_CBP];
	} cbp;

	/* 8x8 descriptors: */
	struct {
		unsigned int next [MAX_DEPTH+2];
		unsigned int base [MAX_DEPTH+2];
		unsigned int offset [MAX_DEPTH+1];
		unsigned char symbol [MAX_8x8_DSC]; /* Flawfinder: ignore */
	} _8x8_dsc [4]; /* context = # of encoded 8x8 blocks - 1 */

} CBP_DECODE_TABLE;

/* combined encoding/decoding tables: */
typedef struct INTRA_TABLES
{
	/* cbp tables: */
	CBP_DECODE_TABLE cbp [2];         /* 2 types of intra CBPs   */
	/* 1, if normal intra macroblock 2, if DC-removed intra macroblock */
	/* luma descriptors: */
	struct {
		_4x4_DSC_DECODE_TABLE _4x4_dsc[3]; /* 3 types of intra luma blocks!!! */
		_2x2_DSC_DECODE_TABLE _2x2_dsc[2];
	} luma;
	/* chroma descriptors: */
	struct {
		_4x4_DSC_DECODE_TABLE _4x4_dsc;
		_2x2_DSC_DECODE_TABLE _2x2_dsc[2];
	} chroma;
	/* extra levels: */
	LEVEL_DSC_DECODE_TABLE level_dsc;

}INTRA_DECODE_TABLES;

typedef struct INTER_TABLES
{
	/* cbp table: */
	CBP_DECODE_TABLE cbp;
	/* luma descriptors: */
	struct {
		_4x4_DSC_DECODE_TABLE _4x4_dsc;
		_2x2_DSC_DECODE_TABLE _2x2_dsc[2];
	} luma;
	/* chroma descriptors: */
	struct {
		_4x4_DSC_DECODE_TABLE _4x4_dsc;
		_2x2_DSC_DECODE_TABLE _2x2_dsc[2];
	} chroma;
	/* extra levels: */
	LEVEL_DSC_DECODE_TABLE level_dsc;

}INTER_DECODE_TABLES;


/*
* Conversion between progressively scanned 4x4 block bits
* and CBP descriptor:
*  Y:            Cr:     Cb
*    0  1  2  3   16 17    20 21
*    4  5  6  7   18 19    22 23
*    8  9 10 11
*   12 13 14 15
*
*  cbp4x4 := [...|23..16|15..8|7..0]
*/

/*****************************************************************************
* QP range partition
*
* In order to reduce # of tables, we partition
* the range of all possible qp values into a set of
* regions (which, in turn will be associated with the
* corresponding set of tables).
*****************************************************************************/
/*MAP -- MAX_QP is modified to 31 to avoid decoder crash when the QP value is 31*/
//#define MAX_QP  30
#define MAX_QP  31

extern const int intra_qp_to_idx [MAX_QP+1];
extern const int inter_qp_to_idx [MAX_QP+1];

typedef struct
{
	/* luma descriptor to cbp4x4 conversion tables: */
	U32 *y_to_cbp4x4;
	/* 8x8 cbp to context-predictor conversion table: */
	U32 *cbp_to_cx;
	struct INTRA_TABLES *intra_decode_tables;
	struct INTER_TABLES *inter_decode_tables;
}tDecoderTables;

#ifdef __cplusplus
}
#endif

#endif /* __DEC4X4_H__ */
