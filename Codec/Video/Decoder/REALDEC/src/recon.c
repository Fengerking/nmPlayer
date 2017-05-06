/********************************************************* BEGIN LICENSE BLOCK *********************************************** 
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
 *********************************************************** END LICENSE BLOCK ***************************************************/ 

/************************************************************************************/
/*    RealNetworks, Inc. Confidential and Proprietary Information. */
/*    Copyright (c) 1995-2002 RealNetworks, Inc. */
/*    All Rights Reserved. */
/*    Do not redistribute. */
/************************************************************************************/

/*    Motion compensation for decoder. */

/***************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1999 - 2000 Intel Corporation.
*    All Rights Reserved. 
***************************************************************************************/
/************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/

/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/mc.c,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */

/* This file includes the decoder's motion compensation functions. */
/********************************************************************************
						Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		optimized functions
											C_DirectB_InterpolMB_Lu
											C_DirectB_InterpolMB_Cr
											Decoder_reconstructLumaMacroblock
											ReconstructInterMB
											Decoder_reconstructChromaBlock
											Decoder_reconstructChromaBlock_RV8
********************************************************************************/
#include "decoder.h"
#include "recon.h"
#include "ai.h"
#include "idct.h"

#define INTRA16x16_DC							0
#define INTRA16x16_VERT							1
#define INTRA16x16_HORZ							2
#define INTRA16x16_PLANAR						3
#define INTRA16x16_LEFT_DC						4
#define INTRA16x16_TOP_DC						5
#define INTRA16x16_128_DC						6

#define INTRA4X4_MODE_0							0
#define INTRA4X4_MODE_1							1
#define INTRA4X4_MODE_2							2
#define INTRA4X4_MODE_3							3
#define INTRA4X4_MODE_4							4
#define INTRA4X4_MODE_5							5
#define INTRA4X4_MODE_6							6
#define INTRA4X4_MODE_7							7
#define INTRA4X4_MODE_8							8
#define INTRA4X4_MODE_0_LEFT                    9
#define INTRA4X4_MODE_0_TOP                     10
#define INTRA4X4_MODE_0_128                     11
#define INTRA4X4_MODE_4_LEFT					12
#define INTRA4X4_MODE_4_TOP                     13
#define INTRA4X4_MODE_4_LEFT_TOP				14
#define INTRA4X4_MODE_6_LEFT                    15
#define INTRA4X4_MODE_6_TOP                     16
#define INTRA4X4_MODE_6_LEFT_TOP                17
#define INTRA4X4_MODE_7_LEFT                    18
#define INTRA4X4_MODE_7_TOP                     19
#define INTRA4X4_MODE_7_LEFT_TOP                20

#define TRUNCATE_LO(val, lim, tmp)		\
	(tmp) = (lim);						\
	if ((tmp) < (val))					\
	(val) = (tmp);

#define TRUNCATE_HI(val, lim, tmp)		\
	(tmp) = (lim);						\
	if ((tmp) > (val))					\
	(val) = (tmp);

//YU TBD optimization*/
static void MV_Sub2FullPels_RV8(const I32 mv, I32* mv_intg, I32* mv_frac, const RV_Boolean	no_subpel, I32 SubPixelFactor, U32 direct_flage)
{
	if(no_subpel){

		if (direct_flage){
			*mv_intg = (mv + 1)/ SubPixelFactor;
		}
		else{
			*mv_intg = (mv + 1) >> 2;
		}

		*mv_frac = 0;
	}else{

		if (direct_flage){
			*mv_intg = mv / SubPixelFactor;
		} 
		else{
			*mv_intg = mv >> 2;
		}

		*mv_frac = mv - *mv_intg * SubPixelFactor;

		if (*mv_frac < 0){
			*mv_intg -= 1;
			*mv_frac += SubPixelFactor;
		}
	}
}

I32 InterpolateMB(struct Decoder *t, RV89_DEC_LOCAL  *rv89_local,
				  I32 uPitch,struct DecoderPackedMotionVector *fmv,struct DecoderPackedMotionVector *pmv,
				  PU8 pPrev8x8,RV_Boolean	no_subpel,RV_Boolean isRV8, I32 slice_index)
{
	I32  k;
	U32 uDstPitch, uPitch2;
	I32 xintf, yintf, xhf, yhf;
	I32 xintp, yintp, xhp, yhp;
	U8 *dst, *pPrev, *pFutr;
	I32 ref_top_edge,ref_down_edge;
	U32 direct_flage = 1;
	struct DecoderPackedMotionVector futrMV[4];//huwei 20110315 stability
	struct DecoderPackedMotionVector prevMV[4];
	I32 SubPixelFactor;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];

	if(isRV8){
		SubPixelFactor = 3;
	}else{
		SubPixelFactor = 4;
		direct_flage = pmv ? 1 : 0;
	}

	if(t->m_uv_interlace_flage){
		uPitch2 = t->m_img_stride;
	}else{
		uPitch2 = t->m_img_UVstride;
	}

	ref_top_edge  = -(I32)((slice_di->m_xmb_pos << 4) + YUV_Y_PADDING + (((slice_di->m_ymb_pos << 4) + YUV_Y_PADDING) * t->m_img_stride));
	ref_down_edge = ((t->m_mbX - slice_di->m_xmb_pos) << 4) + YUV_Y_PADDING - 8 
		+ (((t->m_mbY - slice_di->m_ymb_pos) << 4) + YUV_Y_PADDING - 8) * t->m_img_stride;

	for(k = 0; k < 4; k++){

		dst = rv89_local->dst[0] + ((k&1)<<3) + ((k&2)<<2)*uPitch;
		uDstPitch = uPitch;
		if(pmv){
			prevMV[k].mvx = pmv->mvx;
			prevMV[k].mvy = pmv->mvy;
			MV_Sub2FullPels_RV8(prevMV[k].mvx, &xintp, &xhp, no_subpel, SubPixelFactor, direct_flage);
			MV_Sub2FullPels_RV8(prevMV[k].mvy, &yintp, &yhp, no_subpel, SubPixelFactor, direct_flage);
			pPrev = rv89_local->cur[0] + ((k&1)<<3) + xintp + (((k&2)<<2)+yintp)*uPitch;

			if ((pPrev < rv89_local->cur[0] + ref_top_edge)|| (pPrev > rv89_local->cur[0] + ref_down_edge)){
				return -1;//RV_S_OUT_OF_MEMORY;
			}

			t->gc_C_InterpolateNew[yhp*4+xhp]((U8 *)pPrev, dst, uPitch, uDstPitch);
			pmv++;
		}

		if(fmv){
			futrMV[k].mvx = fmv->mvx;
			futrMV[k].mvy = fmv->mvy;
			MV_Sub2FullPels_RV8(futrMV[k].mvx, &xintf, &xhf, no_subpel, SubPixelFactor, direct_flage);
			MV_Sub2FullPels_RV8(futrMV[k].mvy, &yintf, &yhf, no_subpel, SubPixelFactor, direct_flage);
			pFutr = rv89_local->ref[0] + ((k&1)<<3) + xintf + (((k&2)<<2)+yintf)*uPitch;

			if ((pFutr < rv89_local->ref[0] + ref_top_edge)|| (pFutr > rv89_local->ref[0] + ref_down_edge)){
				return -1;//RV_S_OUT_OF_MEMORY;
			}

			if (!pmv){
				t->gc_C_InterpolateNew[yhf*4+xhf]((U8 *)pFutr, dst, uPitch, uDstPitch);
			}else{
				t->gc_C_AddInterpolateNew[yhf*4+xhf]((U8 *)pFutr, dst, uPitch, uDstPitch);
			}

			fmv++;
		}
	}

	pPrev8x8 += 16*16;

	for(k = 0; k < 4; k++){
		dst = rv89_local->dst[1] + ((k & 1) << 2) + ((k & 2) << 1) * uPitch2;
		uDstPitch = uPitch2;

		if(pmv){
			MV_Sub2FullPels_RV8(prevMV[k].mvx/2, &xintp, &xhp, no_subpel, SubPixelFactor, direct_flage);
			MV_Sub2FullPels_RV8(prevMV[k].mvy/2, &yintp, &yhp, no_subpel, SubPixelFactor, direct_flage);
			pPrev = rv89_local->cur[1] + ((k & 1) << 2) + xintp + (((k & 2) << 1) + yintp) * uPitch2;
			t->gc_C_InterpolateChromaNew[yhp*4+xhp](pPrev, dst, uPitch2, uDstPitch);
		}

		if(fmv){
			MV_Sub2FullPels_RV8(futrMV[k].mvx/2, &xintf, &xhf, no_subpel, SubPixelFactor, direct_flage);
			MV_Sub2FullPels_RV8(futrMV[k].mvy/2, &yintf, &yhf, no_subpel, SubPixelFactor, direct_flage);
			pFutr = rv89_local->ref[1] + ((k & 1) << 2) + xintf + (((k & 2) << 1) + yintf) * uPitch2;
			if (!pmv){
				t->gc_C_InterpolateChromaNew[yhf*4+xhf](pFutr, dst, uPitch2, uDstPitch);
			}else{
				t->gc_C_AddInterpolateChromaNew[yhf*4+xhf](pFutr, dst, uPitch2, uDstPitch);
			}
		}
	}

	pPrev8x8 += 8;

	for(k = 0; k < 4; k++){

		dst = rv89_local->dst[2] + ((k & 1) << 2) + ((k & 2) << 1) * uPitch2;
		uDstPitch = uPitch2;


		if(pmv){
			MV_Sub2FullPels_RV8(prevMV[k].mvx/2, &xintp, &xhp, no_subpel, SubPixelFactor, direct_flage);
			MV_Sub2FullPels_RV8(prevMV[k].mvy/2, &yintp, &yhp, no_subpel, SubPixelFactor, direct_flage);
			pPrev = rv89_local->cur[2] + ((k & 1) << 2) + xintp + (((k & 2) << 1) + yintp) * uPitch2;
			t->gc_C_InterpolateChromaNew[yhp*4+xhp](pPrev, dst, uPitch2, uDstPitch);
		}

		if(fmv){
			MV_Sub2FullPels_RV8(futrMV[k].mvx/2, &xintf, &xhf, no_subpel, SubPixelFactor, direct_flage);
			MV_Sub2FullPels_RV8(futrMV[k].mvy/2, &yintf, &yhf, no_subpel, SubPixelFactor, direct_flage);
			pFutr = rv89_local->ref[2] + ((k & 1) << 2) + xintf + (((k & 2) << 1) + yintf) * uPitch2;
			if (!pmv){
				t->gc_C_InterpolateChromaNew[yhf * 4 + xhf](pFutr, dst, uPitch2, uDstPitch);
			}else{
				t->gc_C_AddInterpolateChromaNew[yhf * 4 + xhf](pFutr, dst, uPitch2, uDstPitch);
			}
		}
	}
	return RV_S_OK;
}

void C_InterpolateDirectLuma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1)
{
	U32 i, j;

	for (j = 0; j < 8; j++)
	{
		for (i = 0; i < 8; i += 4)
		{
#if defined(VOARAMV4) || defined(VOARMV6)
			dst[i] = (U8) (((pFutr[i] * iRatio0 +
				pPrev[i] * iRatio1) + TR_RND) >> TR_SHIFT);
			dst[i + 1] = (U8) (((pFutr[i + 1] * iRatio0 +
				pPrev[i + 1] * iRatio1) + TR_RND) >> TR_SHIFT);
			dst[i + 2] = (U8) (((pFutr[i + 2] * iRatio0 +
				pPrev[i + 2] * iRatio1) + TR_RND) >> TR_SHIFT);
			dst[i + 3] = (U8) (((pFutr[i + 3] * iRatio0 +
				pPrev[i + 3] * iRatio1) + TR_RND) >> TR_SHIFT);
#elif defined(VOARMV7) || defined(VOWMMX2)
			dst[i] = (U8) ((((((pFutr[i]<<4) * iRatio0)>> 16) +
				(((pPrev[i]<< 4) * iRatio1)>>16)) + 2) >> 2);
			dst[i+1] = (U8) ((((((pFutr[i+1]<<4) * iRatio0)>> 16) +
				(((pPrev[i+1]<< 4) * iRatio1)>>16)) + 2) >> 2);
			dst[i+2] = (U8) ((((((pFutr[i+2]<<4) * iRatio0)>> 16) +
				(((pPrev[i+2]<< 4) * iRatio1)>>16)) + 2) >> 2);
			dst[i+3] = (U8) ((((((pFutr[i+3]<<4) * iRatio0)>> 16) +
				(((pPrev[i+3]<< 4) * iRatio1)>>16)) + 2) >> 2);
#else
	                U32 v1;
	                U32 v2;
	                U32 w;
			/* matches 16-bit arithmetics in the asm implementation */
			v1 = (U32) pFutr[i] << 7;
			v2 = (U32) pPrev[i] << 7;
			w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
			dst[i] = (U8) ((w + 0x10) >> 5);

			v1 = (U32) pFutr[i + 1] << 7;
			v2 = (U32) pPrev[i + 1] << 7;
			w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
			dst[i + 1] = (U8) ((w + 0x10) >> 5);

			v1 = (U32) pFutr[i + 2] << 7;
			v2 = (U32) pPrev[i + 2] << 7;
			w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
			dst[i + 2] = (U8) ((w + 0x10) >> 5);

			v1 = (U32) pFutr[i + 3] << 7;
			v2 = (U32) pPrev[i + 3] << 7;
			w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
			dst[i + 3] = (U8) ((w + 0x10) >> 5);
#endif
		}
		dst   += uDstPitch;
		pFutr += uPitch;
		pPrev += uPitch;
	}

}

void C_InterpolateDirectChroma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1)
{
	U32 i;

	for (i = 0; i < 4; i++)
	{
#if defined(VOARAMV4) || defined(VOARMV6)
		dst[0] = (U8) (((pFutr[0] * iRatio0 +
			pPrev[0] * iRatio1) + TR_RND) >> TR_SHIFT);
		dst[1] = (U8) (((pFutr[1] * iRatio0 +
			pPrev[1] * iRatio1) + TR_RND) >> TR_SHIFT);
		dst[2] = (U8) (((pFutr[2] * iRatio0 +
			pPrev[2] * iRatio1) + TR_RND) >> TR_SHIFT);
		dst[3] = (U8) (((pFutr[3] * iRatio0 +
			pPrev[3] * iRatio1) + TR_RND) >> TR_SHIFT);
#elif defined(VOARMV7) || defined(VOWMMX2)
		dst[0] = (U8) ((((((pFutr[0]<<4) * iRatio0)>> 16) +
			(((pPrev[0]<< 4) * iRatio1)>>16)) + 2) >> 2);
		dst[1] = (U8) ((((((pFutr[1]<<4) * iRatio0)>> 16) +
			(((pPrev[1]<< 4) * iRatio1)>>16)) + 2) >> 2);
		dst[2] = (U8) ((((((pFutr[2]<<4) * iRatio0)>> 16) +
			(((pPrev[2]<< 4) * iRatio1)>>16)) + 2) >> 2);
		dst[3] = (U8) ((((((pFutr[3]<<4) * iRatio0)>> 16) +
			(((pPrev[3]<< 4) * iRatio1)>>16)) + 2) >> 2);
#else
	        U32 v1;
	        U32 v2;
	        U32 w;
		/* matches 16-bit arithmetics in the asm implementation */
		v1 = (U32) pFutr[0] << 7;
		v2 = (U32) pPrev[0] << 7;
		w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
		dst[0] = (U8) ((w + 0x10) >> 5);

		v1 = (U32) pFutr[1] << 7;
		v2 = (U32) pPrev[1] << 7;
		w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
		dst[1] = (U8) ((w + 0x10) >> 5);

		v1 = (U32) pFutr[2] << 7;
		v2 = (U32) pPrev[2] << 7;
		w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
		dst[2] = (U8) ((w + 0x10) >> 5);

		v1 = (U32) pFutr[3] << 7;
		v2 = (U32) pPrev[3] << 7;
		w = ((v1 * iRatio0) >> 16) + ((v2 * iRatio1) >> 16);
		dst[3] = (U8) ((w + 0x10) >> 5);
#endif

		dst   += uDstPitch;
		pFutr += uPitch;
		pPrev += uPitch;
	}

}

I32 InterpolateMBWeight(struct Decoder *t, RV89_DEC_LOCAL  *rv89_local, I32 uPitch,struct DecoderPackedMotionVector *fmv, struct DecoderPackedMotionVector *pmv,
						PU8 pPrev8x8,RV_Boolean	no_subpel,RV_Boolean isRV8, I32 iRatio0,I32 iRatio1, I32 slice_index)
{
	U32 uDstPitch, uPitch2;
	I32 xintf, yintf, xhf, yhf;
	I32 xintp, yintp, xhp, yhp;
	U8 *dst, *pPrev, *pFutr;
	I32 ref_top_edge,ref_down_edge;
	U32 direct_flage = 1;
	struct DecoderPackedMotionVector futrMV[4];//huwei 20110315 stability
	struct DecoderPackedMotionVector prevMV[4];
	I32 SubPixelFactor;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];
	U32 k;

	if(isRV8){
		SubPixelFactor = 3;
	}else{
		SubPixelFactor = 4;
		direct_flage = pmv ? 1 : 0;
	}

	uPitch2 = t->m_img_UVstride;

	ref_top_edge  = -(I32)((slice_di->m_xmb_pos << 4) + YUV_Y_PADDING + (((slice_di->m_ymb_pos << 4) + YUV_Y_PADDING) * t->m_img_stride));
	ref_down_edge = ((t->m_mbX - slice_di->m_xmb_pos) << 4) + YUV_Y_PADDING - 8 
		+ (((t->m_mbY - slice_di->m_ymb_pos) << 4) + YUV_Y_PADDING - 8) * t->m_img_stride;

	for(k = 0; k < 4; k++){
		dst = rv89_local->dst[0] + ((k&1)<<3) + ((k&2)<<2)*uPitch;
		uDstPitch = uPitch;

		prevMV[k].mvx = pmv->mvx;
		prevMV[k].mvy = pmv->mvy;
		MV_Sub2FullPels_RV8(prevMV[k].mvx, &xintp, &xhp, 1, SubPixelFactor, direct_flage);
		MV_Sub2FullPels_RV8(prevMV[k].mvy, &yintp, &yhp, 1, SubPixelFactor, direct_flage);
		pPrev = rv89_local->cur[0] + ((k&1)<<3) + xintp + (((k&2)<<2)+yintp)*uPitch;
		pmv++;

		if ((pPrev < rv89_local->cur[0] + ref_top_edge)|| (pPrev > rv89_local->cur[0] + ref_down_edge)){
			return -1;//RV_S_OUT_OF_MEMORY;
		}

		futrMV[k].mvx = fmv->mvx;
		futrMV[k].mvy = fmv->mvy;
		MV_Sub2FullPels_RV8(futrMV[k].mvx, &xintf, &xhf, 1, SubPixelFactor, direct_flage);
		MV_Sub2FullPels_RV8(futrMV[k].mvy, &yintf, &yhf, 1, SubPixelFactor, direct_flage);
		pFutr = rv89_local->ref[0] + ((k&1)<<3) + xintf + (((k&2)<<2)+yintf)*uPitch;
		fmv++;

		if ((pFutr < rv89_local->ref[0] + ref_top_edge)|| (pFutr > rv89_local->ref[0] + ref_down_edge)){
			return -1;//RV_S_OUT_OF_MEMORY;
		}

		InterpolateDirectLuma(pPrev, pFutr, uPitch, dst,uDstPitch, iRatio0, iRatio1);

	}

	for(k = 0; k < 4; k++){
		dst = rv89_local->dst[1] + ((k & 1) << 2) + ((k & 2) << 1) * uPitch2;
		uDstPitch = uPitch2;

		MV_Sub2FullPels_RV8(prevMV[k].mvx/2, &xintp, &xhp, 1, SubPixelFactor, direct_flage);
		MV_Sub2FullPels_RV8(prevMV[k].mvy/2, &yintp, &yhp, 1, SubPixelFactor, direct_flage);
		pPrev = rv89_local->cur[1] + ((k & 1) << 2) + xintp + (((k & 2) << 1) + yintp) * uPitch2;

		MV_Sub2FullPels_RV8(futrMV[k].mvx/2, &xintf, &xhf, 1, SubPixelFactor, direct_flage);
		MV_Sub2FullPels_RV8(futrMV[k].mvy/2, &yintf, &yhf, 1, SubPixelFactor, direct_flage);
		pFutr = rv89_local->ref[1] + ((k & 1) << 2) + xintf + (((k & 2) << 1) + yintf) * uPitch2;

		InterpolateDirectChroma(pPrev, pFutr, uPitch2, dst, uDstPitch, iRatio0, iRatio1);

	}

	for(k = 0; k < 4; k++){
		dst = rv89_local->dst[2] + ((k & 1) << 2) + ((k & 2) << 1) * uPitch2;
		uDstPitch = uPitch2;

		MV_Sub2FullPels_RV8(prevMV[k].mvx/2, &xintp, &xhp, 1, SubPixelFactor, direct_flage);
		MV_Sub2FullPels_RV8(prevMV[k].mvy/2, &yintp, &yhp, 1, SubPixelFactor, direct_flage);
		pPrev = rv89_local->cur[2] + ((k & 1) << 2) + xintp + (((k & 2) << 1) + yintp) * uPitch2;

		MV_Sub2FullPels_RV8(futrMV[k].mvx/2, &xintf, &xhf, 1, SubPixelFactor, direct_flage);
		MV_Sub2FullPels_RV8(futrMV[k].mvy/2, &yintf, &yhf, 1, SubPixelFactor, direct_flage);
		pFutr = rv89_local->ref[2] + ((k & 1) << 2) + xintf + (((k & 2) << 1) + yintf) * uPitch2;
		
		InterpolateDirectChroma(pPrev, pFutr, uPitch2, dst, uDstPitch, iRatio0, iRatio1);
	}

	return RV_S_OK;
}

static INLINE U32 adjust_pred4(U32 itype, U32 top, U32 left, U32 right, U32 down)
{
	U32 itype_temp = itype;

	if (itype == INTRA4X4_MODE_0){

		if(!top && !left)
			itype_temp = INTRA4X4_MODE_0_128;
		else if(!top){
			if(itype == INTRA4X4_MODE_0)	itype_temp = INTRA4X4_MODE_0_LEFT;
		}else if(!left){
			if(itype == INTRA4X4_MODE_0)	itype_temp = INTRA4X4_MODE_0_TOP;
		}

	}else{

		switch (itype)
		{
		case INTRA4X4_MODE_4:
			if (!right && !down){
				itype_temp = INTRA4X4_MODE_4_LEFT_TOP;
			} else if(!right){
				itype_temp = INTRA4X4_MODE_4_TOP;
			} else if (!down){
				itype_temp = INTRA4X4_MODE_4_LEFT;
			}
			break;
		case INTRA4X4_MODE_6:
			if (!right && !down){
				itype_temp = INTRA4X4_MODE_6_LEFT_TOP;
			} else if(!right){
				itype_temp = INTRA4X4_MODE_6_TOP;
			} else if (!down){
				itype_temp = INTRA4X4_MODE_6_LEFT;
			}
			break;
		case INTRA4X4_MODE_7:
			if (!right && !down){
				itype_temp = INTRA4X4_MODE_7_LEFT_TOP;
			} else if(!right){
				itype_temp = INTRA4X4_MODE_7_TOP;
			} else if (!down){
				itype_temp = INTRA4X4_MODE_7_LEFT;
			}
			break;	
		}

	}
	return itype_temp;
}


static INLINE U32 adjust_pred16(SLICE_DEC *slice_di, U32 itype)
{
	U32 edge_avail = slice_di->uEdgeAvail;

	switch (itype)
	{
	case INTRA16x16_PLANAR:
		if(!(edge_avail & 0x2)){
			itype = INTRA16x16_HORZ;
		}else if(!(edge_avail & 0x10)){
			itype = INTRA16x16_VERT;
		}
		break;
	case INTRA16x16_DC:
		if(!(edge_avail & 0x12))
			itype = INTRA16x16_128_DC;
		else if(!(edge_avail & 0x2)){
			itype = INTRA16x16_LEFT_DC;
		}else if(!(edge_avail & 0x10)){
			itype = INTRA16x16_TOP_DC;
		}
		break;
	case INTRA16x16_VERT:
	case INTRA16x16_HORZ:
		if(!(edge_avail & 0x2)){
			itype = INTRA16x16_HORZ;
		}else if(!(edge_avail & 0x10)){
			itype = INTRA16x16_VERT;
		}
		break;
	}
	return itype;
}

/*4x4*/
RV_Status ReconIntraMB(struct Decoder *t,const struct DecoderMBInfo *pMB,const RV89_DEC_LOCAL *rv89_local, I32 slice_index)
{
	RV_Status status = RV_S_OK;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];
	I32 *pQuantBuf = slice_di->m_pQuantBuf;
	const DecoderIntraType *pMBIntraTypes = slice_di->m_pMBIntraTypes;
	U32 i, j, idx;
	U32 subblock;
	U32 pitch = t->m_img_stride;
	U32 pitch2 = t->m_img_UVstride;
	U32 intra_mode;
	U32	cbp = pMB->cbp;
	U32 edge_avail = slice_di->uEdgeAvail;
	U8  *predLuma, *predChromaU, *predChromaV;
	U8  *pDstLuma, *pDstChromaU, *pDstChromaV;
	U32 avail[6*6] = {0};

	predLuma    = slice_di->m_pPredBuf;
	predChromaU = predLuma + LUMA_SIZE;
	predChromaV = predChromaU + 64;

	pDstLuma    = rv89_local->dst[0];
	pDstChromaU = rv89_local->dst[1];
	pDstChromaV = rv89_local->dst[2];

	if (edge_avail & 0x10){
		avail[6] = avail[12] = avail[18] = avail[24] = 1;
	}

	if(edge_avail & 0x2){
		avail[1] = avail[2] = avail[3] = avail[4] = 1;
		if(edge_avail & 0x8)
			avail[5] = 1;
	}


	//Y 4X4
	for(j = 0; j < 4; j++){
		idx = 7 + j*6;
		for (i = 0; i < 4; i++ ,idx++){
			subblock = i + (j << 2);
			intra_mode = pMBIntraTypes[subblock];
			intra_mode = adjust_pred4(intra_mode,avail[idx-6],avail[idx-1],avail[idx-5],avail[idx+5]);//top, left, right, down
			avail[idx] = 1;
			if (intra_mode > 20){
				intra_mode = 20;
			}
			IntraPred4x4[intra_mode](pDstLuma + (i << 2), pDstLuma + (i << 2), pitch, pitch);

			if (cbp & (1 << subblock))
				Transform4x4_Add(pQuantBuf, pDstLuma + (i << 2), pitch, pDstLuma + (i << 2), pitch);

			predLuma  += 16;
			pQuantBuf += 16;
		}
		pDstLuma += (pitch << 2);
	}

	//UV 4X4
	subblock     = 16;
	for(j = 0; j < 2; j++){
		idx = 5 + j*4;
		for (i = 0; i < 2; i++, idx++){
			intra_mode = pMBIntraTypes[(i<< 1) + (j << 3)];
			intra_mode = adjust_pred4(intra_mode, edge_avail & (1 << (idx - 4)), edge_avail & (1 << (idx - 1)),edge_avail & (1 << (idx - 3)),!i && !j);//top, left, right, down
			if (intra_mode > 20){
				intra_mode = 20;
			}
			IntraPred4x4[intra_mode](pDstChromaU + (i << 2), pDstChromaU + (i << 2), pitch2, pitch2);
			IntraPred4x4[intra_mode](pDstChromaV + (i << 2), pDstChromaV + (i << 2), pitch2, pitch2);

			if (cbp & (1 << subblock))
				Transform4x4_Add(pQuantBuf, pDstChromaU + (i << 2), pitch2, pDstChromaU + (i << 2), pitch2);

			if (cbp & (1 << (subblock + 4)))
				Transform4x4_Add(pQuantBuf + 64, pDstChromaV + (i << 2), pitch2, pDstChromaV + (i << 2), pitch2);

			pQuantBuf   += 16;
			predChromaU += 16;
			predChromaV += 16;

			subblock++;
		}
		pDstChromaU += (pitch2 << 2);
		pDstChromaV += (pitch2 << 2);
	}

	return status;

}/* 4 mode */



RV_Status ReconIntraMB16x16(struct Decoder *t, const struct DecoderMBInfo *pMB,const RV89_DEC_LOCAL *rv89_local, I32 slice_index)
{
	RV_Status status    = RV_S_OK;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];
	PU8 pDst            = rv89_local->dst[0];
	U32 pitch		    = t->m_img_stride;
	U32 pitch2          = t->m_img_UVstride;
	I32 intra_mode	    = slice_di->m_pMBIntraTypes[0];
	U8 *pPredBuf	    = slice_di->m_pPredBuf;

	if(intra_mode > 6){
		intra_mode = 6;
	}

	/*Y 16X16 place to small buffer*/
	intra_mode = adjust_pred16(slice_di, intra_mode);
	IntraPred16x16[intra_mode](pDst, pPredBuf, pitch, 16);

	/*U,V, place to frame buffer*/
	/*U 8X8*/
	pDst	= rv89_local->dst[1];
	pPredBuf += LUMA_SIZE;
	IntraPred8x8[intra_mode](pDst, pDst, pitch2, pitch2);

	/*V 8X8*/
	pDst	= rv89_local->dst[2];
	pPredBuf += CHROMA_SIZE;
	IntraPred8x8[intra_mode](pDst, pDst, pitch2, pitch2);

	return status;

} 



RV_Status Decoder_transferMB(struct Decoder *t,const struct DecoderMBInfo *pMB,const RV89_DEC_LOCAL *rv89_local, I32 slice_index)
{
	RV_Status status = RV_S_OK;
	PU8 pDst = rv89_local->dst[0];
	U32 pitch = t->m_img_stride;
	U32 pitch2 = t->m_img_UVstride;
	SLICE_DEC *slice_di = &t->m_slice_di[slice_index];

	I32 *pQuantBuf = slice_di->m_pQuantBuf;
	U8 *pPredBuf   = slice_di->m_pPredBuf;
	I32 subblock;
	U32 cbp = pMB->cbp;

	//TBD YU 
	for(subblock=0; subblock<16; subblock += 4){ 
		if (cbp & (1U << subblock))
		    Transform4x4_Add((I32 *)pQuantBuf, pDst, pitch, pPredBuf, 16);
		else
			Transform4x4_DCOnly_Add((I32 *)pQuantBuf, pDst, pitch, pPredBuf, 16);

		if (cbp & (2U << subblock))
			Transform4x4_Add((I32 *)(pQuantBuf + 16), pDst + 4, pitch, pPredBuf + 4, 16);
		else
			Transform4x4_DCOnly_Add((I32 *)(pQuantBuf + 16), pDst + 4, pitch, pPredBuf + 4, 16);

		if (cbp & (4U << subblock))
			Transform4x4_Add((I32 *)(pQuantBuf + 32), pDst + 8, pitch, pPredBuf + 8, 16);
		else
			Transform4x4_DCOnly_Add((I32 *)(pQuantBuf + 32), pDst + 8, pitch, pPredBuf + 8, 16);

		if (cbp & (8U << subblock))
			Transform4x4_Add((I32 *)(pQuantBuf + 48), pDst + 12, pitch, pPredBuf + 12, 16);
		else
			Transform4x4_DCOnly_Add((I32 *)(pQuantBuf + 48), pDst + 12, pitch, pPredBuf + 12, 16);

		pPredBuf += 64;
		pQuantBuf += 64;
		pDst += (pitch << 2);
	}


	//U
	pDst = rv89_local->dst[1];

	if (cbp & (1<<16))
	    Transform4x4_Add(pQuantBuf, pDst, pitch2, pDst, pitch2);

	if (cbp & (2<<16))
		Transform4x4_Add(pQuantBuf + 16, pDst + 4, pitch2, pDst + 4, pitch2);

	if (cbp & (4<<16))
		Transform4x4_Add(pQuantBuf + 32, pDst + (pitch2 << 2), pitch2, pDst + (pitch2 << 2), pitch2);

	if (cbp & (8<<16))
		Transform4x4_Add(pQuantBuf + 48, pDst + (pitch2 << 2) + 4, pitch2, pDst + (pitch2 << 2) + 4, pitch2);

	//V
	pPredBuf += 64;
	pQuantBuf += 64;
	pDst = rv89_local->dst[2];

	if (cbp & (1<<20))
		Transform4x4_Add(pQuantBuf, pDst, pitch2, pDst, pitch2);

	if (cbp & (2<<20))
		Transform4x4_Add(pQuantBuf + 16, pDst + 4, pitch2, pDst + 4, pitch2);

	if (cbp & (4<<20))
		Transform4x4_Add(pQuantBuf + 32, pDst + (pitch2 << 2), pitch2, pDst + (pitch2 << 2), pitch2);

	if (cbp & (8<<20))
		Transform4x4_Add(pQuantBuf + 48, pDst + (pitch2 << 2) + 4, pitch2, pDst + (pitch2 << 2) + 4, pitch2);


	return status;

} /* reconstructLumaIntraMacroblock_16x16 */



void IdctAddInterMB(struct Decoder *t,struct DecoderMBInfo  *pMB, RV89_DEC_LOCAL *rv89_local, I32 slice_index)
{
	U8  *lpDst = rv89_local->dst[0];
	U32	uDCOnly = rv89_local->uDCOnly;
	U8	*pPrev8x8 = lpDst;
	I32 b, *lpQuantBuf = t->m_slice_di[slice_index].m_pQuantBuf;
	U32 uSrcPitch = t->m_img_stride;
	U32	uDstPitch = t->m_img_stride;
	U32 cbp = pMB->cbp;


	for (b = 0; b < 16; b += 4)
	{
		if((!(cbp&(1<<b))) && uDCOnly){
			Transform4x4_DCOnly_Add(lpQuantBuf, lpDst, uDstPitch, pPrev8x8, uSrcPitch);
		}else if (cbp&(1<<b)){
			Transform4x4_Add(lpQuantBuf, lpDst, uDstPitch, pPrev8x8, uSrcPitch);
		}
		if((!(cbp&(2<<b))) && uDCOnly){
			Transform4x4_DCOnly_Add(lpQuantBuf + 16, lpDst + 4, uDstPitch, pPrev8x8 + 4, uSrcPitch);
		}else if (cbp&(2<<b)){
			Transform4x4_Add(lpQuantBuf + 16, lpDst + 4, uDstPitch, pPrev8x8 + 4, uSrcPitch);
		}

		if((!(cbp&(4<<b))) && uDCOnly){
			Transform4x4_DCOnly_Add(lpQuantBuf + 32,lpDst + 8, uDstPitch, pPrev8x8 + 8, uSrcPitch);
		}else if (cbp&(4<<b)){
			Transform4x4_Add(lpQuantBuf + 32,lpDst + 8, uDstPitch, pPrev8x8 + 8, uSrcPitch);
		}

		if((!(cbp&(8<<b))) && uDCOnly){
			Transform4x4_DCOnly_Add(lpQuantBuf + 48, lpDst + 12, uDstPitch, pPrev8x8 + 12, uSrcPitch);
		}else if (cbp&(8<<b)){
			Transform4x4_Add(lpQuantBuf + 48, lpDst + 12, uDstPitch, pPrev8x8 + 12, uSrcPitch);
		}

		lpDst += (uDstPitch << 2);
		pPrev8x8 += (uSrcPitch << 2);
		lpQuantBuf += 64;
	}

	uSrcPitch = t->m_img_UVstride;
	uDstPitch = t->m_img_UVstride;
	
	lpDst = rv89_local->dst[1];
	pPrev8x8 = rv89_local->dst[1];

	if (cbp&(1<<16))
		Transform4x4_Add(lpQuantBuf, lpDst, uDstPitch, pPrev8x8, uSrcPitch);
	if (cbp&(1<<17))
		Transform4x4_Add(lpQuantBuf + 16, lpDst + 4, uDstPitch, pPrev8x8 + 4, uSrcPitch);
	if (cbp&(1<<18))
		Transform4x4_Add(lpQuantBuf + 32, lpDst + 4*uDstPitch, uDstPitch, pPrev8x8 + 4*uSrcPitch, uSrcPitch);
	if (cbp&(1<<19))
		Transform4x4_Add(lpQuantBuf + 48, lpDst + 4*uDstPitch + 4, uDstPitch, pPrev8x8 + 4*uSrcPitch + 4, uSrcPitch);

	lpDst = rv89_local->dst[2];
	lpQuantBuf = lpQuantBuf + 64;

	pPrev8x8 = rv89_local->dst[2];

	if (cbp&(1<<20))
		Transform4x4_Add(lpQuantBuf, lpDst, uDstPitch, pPrev8x8, uSrcPitch);
	if (cbp&(1<<21))
		Transform4x4_Add(lpQuantBuf + 16, lpDst + 4, uDstPitch, pPrev8x8 + 4, uSrcPitch);
	if (cbp&(1<<22))
		Transform4x4_Add(lpQuantBuf + 32, lpDst + 4*uDstPitch, uDstPitch, pPrev8x8 + 4*uSrcPitch, uSrcPitch);
	if (cbp&(1<<23))
		Transform4x4_Add(lpQuantBuf + 48, lpDst + 4*uDstPitch + 4, uDstPitch, pPrev8x8 + 4*uSrcPitch + 4, uSrcPitch);

} /* reconDirectInterpLuma */


/*****************************************************************************************/
/* Based on luma motion vectors, perform frame reconstruction for 						*/							
/* one direct coded 16x16 luma macroblock.												*/
/****************************************************************************************/

RV_Status DecodeDirectMV(
	struct Decoder *t,
	const U32               mbTypeRef,
	/* We must not be called for intra and skipped macroblocks */
	struct DecoderPackedMotionVector *pMV,
	struct DecoderPackedMotionVector *pMVf,
	struct DecoderPackedMotionVector *pMVp,
	const U32               mbXOffset,
	const U32               mbYOffset,
	const I32				iRatio0,
	const I32				iRatio1)
{

	const I32 width = t->m_img_width;
	const I32 height = t->m_img_height;

	I32 mvxP, mvyP, mvxF, mvyF;
	I32 posx, posy;
	I32 lo, hi;		/* temp vars for TRUNCATE macro */
	I32 inter_factor;

	/* this routine only handles MBTYPE_DIRECT and MBTYPE_SKIPPED */
	inter_factor = t->m_bIsRV8 ? INTERP_FACTOR_RV8: INTERP_FACTOR;

	if (mbTypeRef == MBTYPE_INTER || mbTypeRef == MBTYPE_INTER_16x16){
		I32 mvx, mvy;
		/* derived the forward MV from the reference MV: */
		/* MVPrevious = ratio1*MV */
		/* since r1<=1, no clipping is needed for MVPrevious,  */
		/* assuming reference MV is ok. */

		/* clipping is needed for MVb */
		/* notes: the luma frame data is padded by 16 on each side, and the  */
		/* subpel interpolation filter is 6-tap. */
		/* right edge:	pos_x*4 + MVx < (width + 16-16-3)*4 */
		/* left edge:	pos_x*4 + MVx > -(16-2)*4 */
		/* upper edge:	pos_y*4 + MVy > -(16-2)*4 */
		/* bottom edge:	pos_y*4 + MVy < (height + 16-16-3)*4 */
		mvx = pMV->mvx;
		if (mvx){
			mvxP = (iRatio0 * mvx + TR_RND) >> TR_SHIFT;
			/* derived the backward MV from the reference MV:  */
			/* MVFuture = MVPrevious-MV = -(1-r1)*MV */
			mvxF = mvxP - mvx;
			TRUNCATE_LO(mvxF, (width - 3 - mbXOffset) * inter_factor - 1, lo);//TBD
			TRUNCATE_HI(mvxF, (-14 - mbXOffset) * inter_factor + 1, hi);
		}else{
			mvxF = mvxP = 0;
		}
		pMVf[0].mvx = pMVf[1].mvx = pMVf[2].mvx = pMVf[3].mvx = (I16)mvxF;
		pMVp[0].mvx = pMVp[1].mvx = pMVp[2].mvx = pMVp[3].mvx = (I16)mvxP;

		mvy = pMV->mvy;
		if (mvy){
			mvyP = (iRatio0 * mvy + TR_RND) >> TR_SHIFT;
			mvyF = mvyP - mvy;
			//TBD
			TRUNCATE_LO(mvyF, (height - 6 - mbYOffset) * inter_factor - 1, lo);
			TRUNCATE_HI(mvyF, (-11 - mbYOffset) * inter_factor + 1, hi);
		}else{
			mvyF = mvyP = 0;
		}

		pMVf[0].mvy = pMVf[1].mvy = pMVf[2].mvy = pMVf[3].mvy = (I16)mvyF;
		pMVp[0].mvy = pMVp[1].mvy = pMVp[2].mvy = pMVp[3].mvy = (I16)mvyP;

	}else{
		I32 i;
		U32 off_x,off_y;
		/* process four 8x8 blocks */
		for (i = 0; i < 4; i++){
			I32 mvx, mvy;

			off_x = (i&1)<<3;
			off_y = (i&2)<<2;

			posx = mbXOffset + off_x;
			posy = mbYOffset + off_y;

			mvx = pMV->mvx;	
			/* derived the forward MV from the reference MV: */
			if (mvx){
				mvxP = (iRatio0 * mvx + TR_RND) >> TR_SHIFT;
				mvxF = mvxP - mvx;
				TRUNCATE_LO(mvxF, (width + 5 - posx) * inter_factor - 1, lo);
				TRUNCATE_HI(mvxF, (-14 - posx) * inter_factor + 1, hi);
			}else{
				mvxF = mvxP = 0;
			}
			pMVf[i].mvx = (I16)mvxF;
			pMVp[i].mvx = (I16)mvxP;

			mvy = pMV->mvy;	
			if (mvy){
				mvyP = (iRatio0 * mvy + TR_RND) >> TR_SHIFT;
				mvyF = mvyP - mvy;

				TRUNCATE_LO(mvyF, (height + 2 - posy) * inter_factor - 1, lo);
				TRUNCATE_HI(mvyF, (-11 - posy) * inter_factor + 1, hi);
			}else{
				mvyF = mvyP = 0;
			}
			pMVf[i].mvy = (I16)mvyF;
			pMVp[i].mvy = (I16)mvyP;

			pMV += 1;

		} /* for subblock */
	}

	return RV_S_OK;

}





