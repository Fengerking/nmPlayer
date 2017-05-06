/* ********************************************** BEGIN LICENSE BLOCK ************************************************ 
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
* ************************************************** END LICENSE BLOCK *************************************************/ 
/**********************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information.
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
**********************************************************************************
**********************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1999 - 2000 Intel Corporation. 
*    All Rights Reserved. 
*    DESCRIPTION: 
*		RV89Combo inloop 4x4 deblocking filter class implementation. 
*		This file contains the C version filter function. 
***********************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/deblock8.c,v 1.2 2006/10/26 06:29:30 pncbose Exp $ */
/********************************************************************************
Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		Added two different implementations for 
********************************************************************************/
/* from tromcfg.h for RV8 */
#include <string.h>

#include "tables.h" /* needed for ClampTbl */
#include "decoder.h"
#include "deblock.h"
#include "basic.h"


#if defined(VOARMV4)
#elif defined(VOARMV6)
void ARMV6_rv8_edge_filter(U8 *src, const U32	stride, const U32	Width, const U32 Height,U8	*pStrengthV, U8	*pStrengthH, U32 row);
#define rv8_edge_filter ARMV6_rv8_edge_filter
//#define rv8_edge_filter rv8_edge_filter_C
#elif defined(VOARMV7)
void ARMV7_rv8_edge_filter(U8 *src, const U32	stride, const U32	Width, const U32 Height,U8	*pStrengthV, U8	*pStrengthH, U32 row);
#define rv8_edge_filter ARMV7_rv8_edge_filter
//#define rv8_edge_filter ARMV6_rv8_edge_filter
#else
static  void rv8_edge_filter_C(U8 *src, const U32	stride, const U32	Width, const U32 Height,U8	*pStrengthV, U8	*pStrengthH, U32 row);
#define rv8_edge_filter rv8_edge_filter_C
#endif
/* Filter strength tables */
/* default strength specified by RV_Default_Deblocking_Strength (0) */

#define RV_Maximum_Deblocking_Strength     6

const I8 s_deblock_rv89combo[RV_Maximum_Deblocking_Strength + 1][QP_LEVELS] =
{
	/*   0         5        10        15        20        25        30 */
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,3,3,4,4,4,5,5,5,6,6,6,7,7,7,8,8,8},
	{0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,4,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9},
};

#if !(defined(VOARMV4)|| defined(VOARMV6) || defined(VOARMV7))
static void rv8_edge_filter_C(U8 *src, const U32	stride, const U32	Width, const U32 Height,U8	*pStrengthV, U8	*pStrengthH, U32 row)
{
	U32 i, j;
	I32 diff;
	I32 str;
	U8 *pCur,*lpr;

	/* filter vertical edges */
	for (j = 0; j < Height; j += 4){
		pCur = src + j*stride + 4;
		pStrengthV += 1;
		for (i = (Width - 4); i > 0 ; i -= 4){
			str = *pStrengthV++;

			if (str != 0)
			{
				lpr = pCur;
				diff = (lpr[-2] - lpr[1] - ((lpr[-1] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
							lpr[-1] = ClampTbl[CLAMP_BIAS + lpr[-1] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];

							lpr += stride;
							diff = (lpr[-2] - lpr[1] - ((lpr[-1] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
							lpr[-1] = ClampTbl[CLAMP_BIAS + lpr[-1] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];

							lpr += stride;
							diff = (lpr[-2] - lpr[1] - ((lpr[-1] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
							lpr[-1] = ClampTbl[CLAMP_BIAS + lpr[-1] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];

							lpr += stride;
							diff = (lpr[-2] - lpr[1] - ((lpr[-1] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
							lpr[-1] = ClampTbl[CLAMP_BIAS + lpr[-1] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];
			}
			pCur += 4;			
		}

		if(Width & 15){
			pStrengthV += 2;
		}
	}


	/* filter horizontal edges */	

	/*Topmost horizantal edge(picture edge) is not filtered. So advance pStrengthH*/
	/*by number of 4x4 blocks in row*/
	pStrengthH = pStrengthH + (Width >> 2)*!row;

	if((Width & 15) && (!row)){
		pStrengthH += 2;
	}

	for(j = 4*!row; j < Height; j+= 4){
		lpr = src + j*stride;
		for (i = Width; i > 0; i -= 4)
		{
			str = *pStrengthH++;

			if (str != 0)
			{
				diff = (lpr[- (I32)(stride * 2)] - lpr[stride] - ((lpr[-(I32)stride] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
				lpr[-(I32)stride] = ClampTbl[CLAMP_BIAS + lpr[-(I32)stride] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];

				lpr++;
				diff = (lpr[- (I32)(stride * 2)] - lpr[stride] - ((lpr[-(I32)stride] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
				lpr[-(I32)stride] = ClampTbl[CLAMP_BIAS + lpr[-(I32)stride] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];

				lpr++;
				diff = (lpr[- (I32)(stride * 2)] - lpr[stride] - ((lpr[-(I32)stride] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
				lpr[-(I32)stride] = ClampTbl[CLAMP_BIAS + lpr[-(I32)stride] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];

				lpr++;
				diff = (lpr[- (I32)(stride * 2)] - lpr[stride] - ((lpr[-(I32)stride] - lpr[0]) << 2)) >> 3;
				if (diff > str)
					diff = str;
				if (diff < -str)
					diff = -str;
				lpr[-(I32)stride] = ClampTbl[CLAMP_BIAS + lpr[-(I32)stride] + diff];
				lpr[0           ] = ClampTbl[CLAMP_BIAS + lpr[0           ] - diff];

				lpr -= 3;
			}

				lpr += 4;		
		}

			if(Width & 15){
				pStrengthH += 2;
		}
	}


} /* C_rv8_edge_filter */

#endif
const static U32 CBPMask[16] =
{
	0X00000000, 0X000000FF, 0X0000FF00, 0X0000FFFF, 
	0X00FF0000, 0X00FF00FF, 0X00FFFF00, 0X00FFFFFF, 
	0XFF000000, 0XFF0000FF, 0XFF00FF00, 0XFF00FFFF, 
	0XFFFF0000, 0XFFFF00FF, 0XFFFFFF00, 0XFFFFFFFF
};

#define	uTopBlocksMask		0x0000000f
#define uLeftBlocksMask		0x00001111
#define uBottomBlocksMask	0x0000f000
#define	uRightBlocksMask	0x00008888

RV_Status InLoopFilterRV8(struct Decoder *t, U32 row)
{
	struct DecoderMBInfo  *pMB;
	I32 i;
	U32 mb_x;
	U32 mb_width  = t->m_mbX;	
	const I8  *pQPSTab   = t->m_pQPSTab;
	U32 b_per_row = t->m_img_width >> 2;
	//U32 b_per_col = t->m_img_height>> 2;
	U32 b_per_row_chrom = ((b_per_row + 7) >> 3) << 2;
	//U32 b_per_col_chrom = b_per_col >> 1;
	U32 loc_lim, cur_lim, left_lim = 0, top_lim = 0;
	U8  *pYPlane, *pUPlane, *pVPlane;
	PU8 pSyv, pSyh;

	U32 str_zero = 0;

	U32 cbp, cbp_left, cbp_top;

	U32 cbph_adj;
	U32 cbpv_adj;
	U32 bhfilter;		/* bit-packed boolean, filter H edge (1=yes) */
	U32 bvfilter;		/* bit-packed boolean, filter V edge (1=yes) */

	pMB     = &t->m_slice_di[0].m_pMBInfo[row * mb_width];
	pSyh    = t->m_pYsh + (row << 2) * b_per_row;
	pSyv    = t->m_pYsv + (row << 2)*b_per_row;

	pYPlane = t->m_pCurrentFrame->m_pYPlane + (row << 4) * t->m_img_stride;

	if(ISINTRAPIC(t->m_ptype)){
		PU8 pSuv, pSuh; 
		PU8 pSvv, pSvh;
		U16 cur_chrom_lim;

		pSuh    = t->m_pUsh + (row << 1) * b_per_row_chrom;
		pSuv    = t->m_pUsv + (row << 1) * b_per_row_chrom;
		pSvh    = t->m_pVsh + (row << 1) * b_per_row_chrom;
		pSvv    = t->m_pVsv + (row << 1) * b_per_row_chrom;

		for(mb_x = 0; mb_x< mb_width; mb_x++,pMB++){
			cur_lim       = pQPSTab[pMB->QP];
			cur_chrom_lim = pQPSTab[pMB->QP];
			str_zero |= cur_lim;
			cur_lim  |= (cur_lim << 8);
			cur_lim  |= (cur_lim << 16);

			cur_chrom_lim |= (cur_chrom_lim << 8); 

			if(row){
				((U32 *)pSyh)[mb_x] = cur_lim;
				((U16 *)pSuh)[mb_x] = cur_chrom_lim;
				((U16 *)pSvh)[mb_x] = cur_chrom_lim;
			}

			((U32 *)pSyh)[mb_x + mb_width]      = cur_lim;
			((U32 *)pSyh)[mb_x + 2 * mb_width]  = cur_lim;
			((U32 *)pSyh)[mb_x + 3 * mb_width]  = cur_lim;

			((U16 *)pSuh)[mb_x + (b_per_row_chrom >> 1)] = cur_chrom_lim;
			((U16 *)pSvh)[mb_x + (b_per_row_chrom >> 1)] = cur_chrom_lim;

			if(!mb_x){
				cur_lim &= ~15;
				cur_chrom_lim &= 0xff00;
			}

			((U32 *)pSyv)[mb_x] = cur_lim;
			((U32 *)pSyv)[mb_x + mb_width]      = cur_lim;
			((U32 *)pSyv)[mb_x + 2 * mb_width]  = cur_lim;
			((U32 *)pSyv)[mb_x + 3 * mb_width]  = cur_lim;

			((U16 *)pSuv)[mb_x] = cur_chrom_lim;
			((U16 *)pSuv)[mb_x + (b_per_row_chrom >> 1)] = cur_chrom_lim;
			((U16 *)pSvv)[mb_x] = cur_chrom_lim;
			((U16 *)pSvv)[mb_x + (b_per_row_chrom >> 1)] = cur_chrom_lim;
		}			

		pUPlane = t->m_pCurrentFrame->m_pUPlane + (row << 3) * t->m_img_UVstride;
		pVPlane = t->m_pCurrentFrame->m_pVPlane + (row << 3) * t->m_img_UVstride;

		if(str_zero){
			rv8_edge_filter(pYPlane,t->m_img_stride,t->m_img_width, 16, pSyv, pSyh, row);
			rv8_edge_filter(pUPlane,t->m_img_UVstride,(t->m_img_width >> 1), 8, pSuv, pSuh, row);
			rv8_edge_filter(pVPlane,t->m_img_UVstride,(t->m_img_width >> 1), 8, pSvv, pSvh, row);
		}
	}else{

		if (!row){
			pSyh += b_per_row;
		}

		for(mb_x = 0; mb_x< mb_width; mb_x++, pMB++){

			cur_lim   = pQPSTab[pMB->QP];
			str_zero |= cur_lim;
			cbp = pMB->deblock_cbpcoef;

			if(mb_x){
				left_lim = pQPSTab[(pMB - 1)->QP];
				cbp_left = (pMB - 1)->deblock_cbpcoef;
			}else{
				left_lim = 0;
				cbp_left = 0;
			}

			if(row){
				top_lim = pQPSTab[(pMB - mb_width)->QP];
				cbp_top = (pMB - mb_width)->deblock_cbpcoef;
			}else{
				top_lim = 0;
				cbp_top = 0;
			}

			/* clear chroma bits from CBP*/
			cbp &= 0xffff;
			/* only want right edge blocks of Left MacroBlock*/
			cbp_left &= uRightBlocksMask;
			/* only want bottom edge blocks of Above MacroBlock*/
			cbp_top &= uBottomBlocksMask;

			/* Form cbp bit vars for adjacent blocks vertically (to left) by shifting the */
			/* cbp bits for this MB left 1 bit and replacing the adjacent bits */
			/* for blocks 0,4,8,12 with the cbp bits from the MB to the left, */
			/* bits 3,7,11,15: */
			/*  adj V: 14 13 12 15L 10 9 8 11L 6 5 4 7L 2 1 0 3L */
			cbpv_adj = (cbp<<1) & (~uLeftBlocksMask);
			cbpv_adj |= (cbp_left>>3);

			/* Form the cbp bit vars for adjacent blocks horizontally (above) by shifting */
			/* the cbp bits left 4 bits and replacing the adjacent bits for blocks */
			/* 0-3 with the cbp bits from the MB above, bits 12-15. */
			/*  adj H: 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 15A 14A 13A 12A */
			cbph_adj = (cbp<<4) & (~uTopBlocksMask);
			cbph_adj |= (cbp_top>>12);

			/* filter an edge if cbp for the block or the adjacent block is set */
			bhfilter = cbp | cbph_adj;
			bvfilter = cbp | cbpv_adj;

			cur_lim  |= (cur_lim << 8);
			cur_lim  |= (cur_lim << 16);
			/*This loop sets the strengths of all vertical edges of LUMA MB*/
			for (i=0; i<4; i++){
				loc_lim = bvfilter&0xF;
				loc_lim = CBPMask[loc_lim];

				loc_lim &= cur_lim;
				if(!(bvfilter&1)){
					if(cbp_left&8){
						loc_lim |= left_lim;
					}
				}

				if(!mb_x){
					loc_lim &= ~15;
				}

				bvfilter >>= 4;
				cbp_left >>= 4;

				((U32 *)pSyv)[0] = loc_lim;
				pSyv += b_per_row;
			}

			pSyv = pSyv + 4 - (b_per_row << 2);

			/*first row H */
			if(row){
				top_lim  |= (top_lim << 8);
				top_lim  |= (top_lim << 16);

				loc_lim = bhfilter&0xF;
				loc_lim = CBPMask[loc_lim];
				cbp_top = CBPMask[cbp_top>>12];
				cbp_top &= ~loc_lim;
				top_lim &= cbp_top;
				loc_lim &= cur_lim;
				loc_lim |= top_lim;

				((U32 *)pSyh)[0] = loc_lim;

				pSyh += b_per_row;
			}

			/* left three row H*/
			bhfilter >>= 4;
			for(i = 1; i < 4; i++){
				loc_lim = bhfilter&0xF;
				loc_lim = CBPMask[loc_lim];
				loc_lim &= cur_lim;
				bhfilter >>= 4;

				((U32 *)pSyh)[0] = loc_lim;

				pSyh += b_per_row;
			}

			pSyh = pSyh + 4 - b_per_row * (4 - !row);
		}

		if(str_zero){
			pSyh    = t->m_pYsh + (row << 2) * b_per_row;
			pSyv    = t->m_pYsv + (row << 2) * b_per_row;
			rv8_edge_filter(pYPlane,t->m_img_stride, t->m_img_width, 16, pSyv, pSyh, row);
		}
	}
	return RV_S_OK;
}
