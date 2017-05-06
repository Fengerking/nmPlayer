/************************************************* BEGIN LICENSE BLOCK ********************************************** 
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
* Technology Compatibility Kit Test Suite(s) Location:  https://rarvcode-tck.helixcommunity.org 
* 
* Contributor(s): 
* 
*************************************************** END LICENSE BLOCK ********************************************************/ 

/********************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 2001 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
*    Inloop Deblocking Filter. 
********************************************************************************/
/*******************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1999 - 2000 Intel Corporation. 
*    All Rights Reserved. 
*    DESCRIPTION: 
*	  RV89Combo inloop 4x4 deblocking filter class implementation.
*	  This file contains the C version filter function. 
***********************************************************************************/
/************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/deblock.c,v 1.2 2006/10/26 06:29:30 pncbose Exp $ */
/********************************************************************************
Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		Optimized code for Deblocking
********************************************************************************/
#include <stdlib.h>

#include "tables.h" /* needed for ClampTbl */
#include "decoder.h"
#include "deblock.h"

#define MASK_CUR            0x0001
#define MASK_RIGHT          0x0008
#define MASK_BOTTOM         0x0010
#define MASK_TOP            0x1000

#define	uTopBlocksMask		0x0000000f
#define	uLeftBlocksMask		0x00001111
#define	uBottomBlocksMask	0x0000f000
#define	uRightBlocksMask	0x00008888

/* chroma masks, using bits 0-3 for chroma */
#define	uTopBlocksMaskChroma	0x00000003
#define	uLeftBlocksMaskChroma	0x00000005
#define	uBottomBlocksMaskChroma 0x0000000C
#define	uRightBlocksMaskChroma  0x0000000A

#if defined(VODEBUG)||defined(_DEBUG)
extern int g_inputFrameNum;
#endif


#if 0
static void rv9_h_loop_filter_2block_C(
							  U8 *pPels,			/* first of 4 */
							  U32 uPitch,
							  U32 Cr[],				/* clip for right */
							  U32 Cl[],				/* clip for left */
							  I32 alpha,
							  I32 betaX,
							  I32 beta2,
							  U32 yuv_h_deblock
							  );
#endif



#if defined(VOARMV4)
#elif defined(VOARMV6)
void ARMV6_rv9_h_loop_filter(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );
void ARMV6_rv9_v_loop_filter(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );

#define rv9_h_loop_filter ARMV6_rv9_h_loop_filter
#define rv9_v_loop_filter ARMV6_rv9_v_loop_filter
#elif defined(VOARMV7)
void ARMV7_rv9_h_loop_filter_2block(
								   U8 *pPels,			/* first of 4 */
								   U32 uPitch,
								   U32 Cr[],				/* clip for right */
								   U32 Cl[],				/* clip for left */
								   I32 alpha,
								   I32 betaX,
								   I32 beta2,
								   U32 yuv_h_deblock
								   );
void ARMV7_rv9_h_loop_filter_Y(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );
void ARMV7_rv9_v_loop_filter_Y(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );
void ARMV7_rv9_h_loop_filter_UV(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );
void ARMV7_rv9_v_loop_filter_UV(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );
#define rv9_h_loop_filter_2block ARMV7_rv9_h_loop_filter_2block
#define rv9_h_loop_filter_Y ARMV7_rv9_h_loop_filter_Y
#define rv9_v_loop_filter_Y ARMV7_rv9_v_loop_filter_Y
#define rv9_h_loop_filter_UV ARMV7_rv9_h_loop_filter_UV
#define rv9_v_loop_filter_UV ARMV7_rv9_v_loop_filter_UV
#else

static void rv9_h_loop_filter_C(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );
static void rv9_v_loop_filter_C(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 );
#define rv9_h_loop_filter rv9_h_loop_filter_C
#define rv9_v_loop_filter rv9_v_loop_filter_C
#endif

/* New Stronger Deblocking filter */
/* Strength Clip Table, Strength = 0, 1, 2 */
static const
I8 clip_table[3][QP_LEVELS] =
{
	/*   0         5        10        15        20        25        30 */
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,5,5},
	{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,4,4,5,5,5,7,8,9},
};
	
#define CLIP_SYMM(a, b) av_clip(a, -(b), b)
#define FFABS(a) ((a) >= 0 ? (a) : (-(a)))

static INLINE int av_clip(int a, int amin, int amax)
{
	if      (a < amin) return amin;
	else if (a > amax) return amax;
	else               return a;
}

static INLINE void rv40_weak_loop_filter(U8 *src, const int step,
										 const int filter_p1, const int filter_q1,
										 const int alpha, const int beta,
										 const int lim_p0q0,
										 const int lim_q1, const int lim_p1,
										 const int diff_p1p0, const int diff_q1q0,
										 const int diff_p1p2, const int diff_q1q2)
{
	int t, u, diff;

	t = src[0*step] - src[-1*step];
	if(!t)
		return;
	u = (alpha * FFABS(t)) >> 7;
	if(u > 3 - (filter_p1 && filter_q1))
		return;

	t <<= 2;
	if(filter_p1 && filter_q1)
		t += src[-2*step] - src[1*step];
	diff = CLIP_SYMM((t + 4) >> 3, lim_p0q0);
	if(filter_p1 || filter_q1){
		src[-1*step] = ClampTbl[CLAMP_BIAS + src[-1*step] + diff];
		src[ 0*step] = ClampTbl[CLAMP_BIAS + src[ 0*step] - diff];
	}
	if(FFABS(diff_p1p2) <= beta && filter_p1){
		t = (diff_p1p0 + diff_p1p2 - diff) >> 1;
		src[-2*step] = ClampTbl[CLAMP_BIAS + src[-2*step] - CLIP_SYMM(t, lim_p1)];
	}
	if(FFABS(diff_q1q2) <= beta && filter_q1){
		t = (diff_q1q0 + diff_q1q2 + diff) >> 1;
		src[ 1*step] = ClampTbl[CLAMP_BIAS + src[ 1*step] - CLIP_SYMM(t, lim_q1)];
	}
}


#if !(defined(VOARMV4)||defined(VOARMV6)|| defined(VOARMV7))
static void rv9_h_loop_filter_C(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 )
{
	I32 d1, d2, n;
	U8 Ar, Al, c, t1, t2;
	PU8	pL1, pL2, pL3, pL4, pR2, pR3, pR4;    	
	I32 delta;
	I32 lTemp;
	I32 deltaL[4], deltaL2[4], deltaR[4], deltaR2[4];
	RV_Boolean b3SmoothLeft, b3SmoothRight;

	I32 R1, R2, R3, R4, L1, L2, L3, L4, z;

	PU8 r = pPels;
	pR2 = r + uPitch;
	pR3 = pR2 + uPitch;
	pR4 = pR3 + uPitch;
	pL1 = r - uPitch;
	pL2 = pL1 - uPitch;
	pL3 = pL2 - uPitch;
	pL4 = pL3 - uPitch;            		

	lTemp = pL2[0];
	deltaL[0] = lTemp - pL1[0];
	deltaL2[0] = lTemp - pL3[0];
	lTemp = pL2[1];
	deltaL[1] = lTemp - pL1[1];
	deltaL2[1] = lTemp - pL3[1];
	lTemp = pL2[2];
	deltaL[2] = lTemp - pL1[2];
	deltaL2[2] = lTemp - pL3[2];
	lTemp = pL2[3];
	deltaL[3] = lTemp - pL1[3];
	deltaL2[3] = lTemp - pL3[3];
	lTemp = pR2[0];
	deltaR[0] = lTemp - r[0];
	deltaR2[0] = lTemp - pR3[0];
	lTemp = pR2[1];
	deltaR[1] = lTemp - r[1];
	deltaR2[1] = lTemp - pR3[1];
	lTemp = pR2[2];
	deltaR[2] = lTemp - r[2];
	deltaR2[2] = lTemp - pR3[2];
	lTemp = pR2[3];
	deltaR[3] = lTemp - r[3];
	deltaR2[3] = lTemp - pR3[3];		
#if defined(VOARMV7)
	Al = Ar = 1;
	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
	if (abs(delta) >= (betaX << 2))	
		Al = 0;	
	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
	if (abs(delta) >= (betaX << 2)) 	
		Ar = 0;	
#else
	Al = Ar = 3;
	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
	if (abs(delta) >= (betaX << 2))	
		Al = 1;	
	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
	if (abs(delta) >= (betaX << 2)) 	
		Ar = 1;	
#endif

#if defined(VOARMV7)
	if((Al+Ar) > 0)
	{
		c = Al + Ar + ((U8)(Cr + Cl)>>1) + 1;
		if (bStrong){
			b3SmoothLeft = (Al == 1);
			b3SmoothRight = (Ar == 1);
		}else{
			/* not strong, force select weak below */
			b3SmoothLeft = FALSE;
			b3SmoothRight = FALSE;
		}
#else
	if((Al+Ar) > 2)
	{
		c = ((U8)(Cr+Cl) + Al + Ar)>>1;
		if (bStrong){
			b3SmoothLeft = (Al == 3);
			b3SmoothRight = (Ar == 3);
		}else{
			/* not strong, force select weak below */
			b3SmoothLeft = FALSE;
			b3SmoothRight = FALSE;
		}
#endif

		if (b3SmoothLeft){
			delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
			if (abs(delta) >= beta2) 
				b3SmoothLeft = FALSE;
		}

		if (b3SmoothRight){
			delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
			if (abs(delta) >= beta2) 
				b3SmoothRight = FALSE;
		}		

		if(b3SmoothLeft && b3SmoothRight) {
			for(z=0;z<4;z++) 
			{
				L1 = (I32) pL1[z];
				R1 = (I32) r[z];
				delta = (R1 -L1);
				if(delta == 0) continue;
				n = (abs(delta)*alpha)>>7;
				if(n>1) continue;
				L2 = (I32) pL2[z];
				R2 = (I32) pR2[z];
				L3 = (I32) pL3[z];
				R3 = (I32) pR3[z];
				L4 = (I32) pL4[z];
				R4 = (I32) pR4[z];

				d1 = ditherL[uMBPos+z];
				d2 = ditherR[uMBPos+z];

				t1 = (U8)((25*(L3 + R2) + 26*(L2 + L1 + R1) + d1)>>7);
				t2 = (U8)((25*(L2 + R3) + 26*(L1 + R1 + R2) + d2)>>7);
				if(n)
				{
					if ((I32)t1 > (L1+c))
						t1 = (U8)(L1+c);
					if ((I32)t1 < (L1-c))
						t1 = (U8)(L1-c);					
					if ((I32)t2 > (R1+c))
						t2 = (U8)(R1+c);
					if ((I32)t2 < (R1-c))
						t2 = (U8)(R1-c);
				} 
				pL1[z] = t1;
				r[z] = t2;

				t1 = (U8)((25*(L4 + R1) + 26*(L3 + L2 + t1) + d1)>>7);
				t2 = (U8)((25*(L1 + R4) + 26*(t2 + R2 + R3) + d2)>>7);
				if(n)
				{
					if ((I32)t1 > (L2+c))
						t1 = (U8)(L2+c);
					if ((I32)t1 < (L2-c))
						t1 = (U8)(L2-c);					
					if ((I32)t2 > (R2+c))
						t2 = (U8)(R2+c);
					if ((I32)t2 < (R2-c))
						t2 = (U8)(R2-c);
				} 
				pL2[z] = t1;
				pR2[z] = t2;

				if(!bChroma)
				{
					pL3[z] = (U8)((26*(L3 + L4 + t1) + 25*(L3 + pL1[z]) + 64)>>7);
					pR3[z] = (U8)((25*(r[z] + R3) + 26*(t2 + R4 + R3) + 64)>>7);
				}
			}
		} 
		else 
		{
#if defined(VOARMV7)
			if(!Al || !Ar){
				c >>= 1;
				Cl>>= 1;
				Cr>>= 1;
			}
			for(z = 0; z < 4; z++, r++){
				rv40_weak_loop_filter(r, uPitch, Al, Ar, alpha, betaX, c, Cr, Cl,deltaL[z], deltaR[z],deltaL2[z], deltaR2[z]);
			}
#else
			if(Al!=1 && Ar!=1) 
			{
				for(z=0;z<4;z++) 
				{
					I32 d;
					L1 = (I32) pL1[z];
					R1 = (I32) r[z];
					delta = (R1 -L1);
					if(delta == 0) continue;
					if(((abs(delta)*alpha)>>7)>2) continue;
					L2 = (I32) pL2[z];
					R2 = (I32) pR2[z];
					d = (L2 - R2 + ((delta) << 2) + 4) >> 3;
					if (d > c)
						d = c;
					if ((d + c) < 0)
						d = -c;
					pL1[z] = ClampTbl[CLAMP_BIAS + L1 + d];
					r[z] = ClampTbl[CLAMP_BIAS + R1 - d];
					if(abs(deltaL2[z])<= betaX) 
					{
						d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
						if (d2 > (I32)Cl)
							d2 = (I32)Cl;
						if ((d2 + (I32)Cl) < 0)
							d2 = -(I32)Cl;
						pL2[z] = ClampTbl[CLAMP_BIAS + L2 - d2];
					}
					if(abs(deltaR2[z])<= betaX) 
					{
						d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
						if (d2 > (I32)Cr)
							d2 = (I32)Cr;
						if ((d2 + (I32)Cr) < 0)
							d2 = -(I32)Cr;
						pR2[z] = ClampTbl[CLAMP_BIAS + R2 - d2];
					}
				} /* z */
			}
			else 
			{ 
				for(z=0;z<4;z++) 
				{
					I32 d;
					L1 = (I32) pL1[z];
					R1 = (I32) r[z];
					delta = (R1 -L1);
					if(delta == 0) continue;
					if(((abs(delta)*alpha)>>7)>3) continue;
					d = (delta + 1)>> 1;
					lTemp = (c>>1);
					if (d > lTemp)
						d = lTemp;
					if ((d + lTemp) < 0)
						d = -lTemp;
					pL1[z] = ClampTbl[CLAMP_BIAS + L1 + d];
					r[z] = ClampTbl[CLAMP_BIAS + R1 - d];
					if(Al!=1 && abs(deltaL2[z])<= betaX) 
					{
						d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
						lTemp = (I32)(Cl>>1);
						if (d2 > lTemp)
							d2 = lTemp;
						if ((d2 + lTemp) < 0)
							d2 = -lTemp;
						pL2[z] = ClampTbl[CLAMP_BIAS + pL2[z] - d2];
					}
					if(Ar!=1 && abs(deltaR2[z])<= betaX)  
					{
						d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
						lTemp = (I32)(Cr>>1);
						if (d2 > lTemp)
							d2 = lTemp;
						if ((d2 + lTemp) < 0)
							d2 = -lTemp;
						pR2[z] = ClampTbl[CLAMP_BIAS + pR2[z] - d2];
					}								
				} /* z */
			}		

#endif
		} /* if */
	} /* >2 */
}	/* rv9_h_loop_filter */

/*---------------------------------------------------------------------------- */
/* filter, vertical */
/*---------------------------------------------------------------------------- */
static void rv9_v_loop_filter_C(
							 U8 *pPels,			/* first of 4 */
							 U32 uPitch,
							 U32 uMBPos,			/* offset in position from MB zero pixel for this edge (0,4,8,or 12) */
							 U32 Cr,				/* clip for right */
							 U32 Cl,				/* clip for left */
							 I32 alpha,
							 I32 betaX,
							 I32 beta2,
							 RV_Boolean bChroma,
							 RV_Boolean bStrong		/* true when strong filter enabled */
							 )
{
	I32 d1, d2, n;
	U8 Ar, Al, c, t1, t2;	
	I32 delta;
	I32 lTemp;
	I32 deltaL[4], deltaL2[4], deltaR[4], deltaR2[4];
	RV_Boolean b3SmoothLeft, b3SmoothRight;
	I32 R1, R2, R3, R4, L1, L2, L3, L4, z;
	PU8 rt = pPels;	

	lTemp =rt[-2]; 
	deltaL[0]  = lTemp - rt[-1];
	deltaL2[0] = lTemp - rt[-3];
	lTemp =rt[1];
	deltaR[0] = lTemp - rt[0];
	deltaR2[0] = lTemp - rt[2];

	rt = rt + uPitch;
	lTemp =rt[-2];
	deltaL[1] = lTemp - rt[-1];
	deltaL2[1] = lTemp - rt[-3];
	lTemp =rt[1];
	deltaR[1] = lTemp - rt[0];
	deltaR2[1] = lTemp - rt[2];

	rt = rt + uPitch;
	lTemp =rt[-2];
	deltaL[2] = lTemp - rt[-1];
	deltaL2[2] = lTemp - rt[-3];
	lTemp =rt[1];
	deltaR[2] = lTemp - rt[0];
	deltaR2[2] = lTemp - rt[2];

	rt = rt + uPitch;
	lTemp =rt[-2];
	deltaL[3] = lTemp -rt[-1];
	deltaL2[3] = lTemp - rt[-3];
	lTemp =rt[1];
	deltaR[3] = lTemp - rt[0];
	deltaR2[3] = lTemp - rt[2];			
#if defined(VOARMV7)
	Al = Ar = 1;
	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
	if (abs(delta) >= (betaX << 2)) 	
		Al = 0;	
	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
	if (abs(delta) >= (betaX << 2)) 	
		Ar = 0;
#else
	Al = Ar = 3;
	delta = deltaL[0]+deltaL[1]+deltaL[2]+deltaL[3];
	if (abs(delta) >= (betaX << 2)) 	
		Al =1;	
	delta = deltaR[0]+deltaR[1]+deltaR[2]+deltaR[3];
	if (abs(delta) >= (betaX << 2)) 	
		Ar =1;
#endif//VOARMV7

#if defined(VOARMV7)
	if((Al+Ar) > 0){
		c = Al + Ar + ((U8)(Cr + Cl)>>1) + 1;
		if (bStrong){
			b3SmoothLeft = (Al == 1);
			b3SmoothRight = (Ar ==1);
		}else{
			/* not strong, force select weak below */
			b3SmoothLeft = FALSE;
			b3SmoothRight = FALSE;
		}
#else
	if((Al+Ar) > 2){
		c = ((U8)(Cr+Cl) + Al + Ar)>>1;
		if (bStrong){
			b3SmoothLeft = (Al == 3);
			b3SmoothRight = (Ar == 3);
		}else{
			/* not strong, force select weak below */
			b3SmoothLeft = FALSE;
			b3SmoothRight = FALSE;
		}
#endif
		if (b3SmoothLeft){
			delta = deltaL2[0]+deltaL2[1]+deltaL2[2]+deltaL2[3];
			if (abs(delta) >= beta2) 
				b3SmoothLeft = FALSE;
		}

		if (b3SmoothRight){
			delta = deltaR2[0]+deltaR2[1]+deltaR2[2]+deltaR2[3];
			if (abs(delta) >= beta2) 
				b3SmoothRight = FALSE;
		}

		rt = pPels;
		if(b3SmoothLeft && b3SmoothRight) 
		{
			for(z=0;z<4;z++, rt+=uPitch)
			{		
				L1 = (I32) rt[-1];
				R1 = (I32) rt[0];								
				delta = (R1 -L1);
				if(delta == 0) continue;
				n=(abs(delta)*alpha)>>7;
				if(n>1) continue;
				L2 = (I32) rt[-2];
				R2 = (I32) rt[1];
				L3 = (I32) rt[-3];
				R3 = (I32) rt[2];
				L4 = (I32) rt[-4];
				R4 = (I32) rt[3];

				d1 = ditherL[uMBPos+z];
				d2 = ditherR[uMBPos+z];

				t1 = (U8)((25*(L3 + R2) + 26*(L2 + L1 + R1) + d1)>>7);
				t2 = (U8)((25*(L2 + R3) + 26*(L1 + R1 + R2) + d2)>>7);
				if(n) {
					if ((I32)t1 > (L1+c))
						t1 = (U8)(L1+c);
					if ((I32)t1 < (L1-c))
						t1 = (U8)(L1-c);					
					if ((I32)t2 > (R1+c))
						t2 = (U8)(R1+c);
					if ((I32)t2 < (R1-c))
						t2 = (U8)(R1-c);		
				} 
				rt[-1] = t1;
				rt[0] = t2;

				t1 = (U8)((25*(L4 + R1) + 26*(L3 + L2 + t1) + d1)>>7);
				t2 = (U8)((25*(L1 + R4) + 26*(t2 + R2 + R3) + d2)>>7);
				if(n) {
					if ((I32)t1 > (L2+c))
						t1 = (U8)(L2+c);
					if ((I32)t1 < (L2-c))
						t1 = (U8)(L2-c);					
					if ((I32)t2 > (R2+c))
						t2 = (U8)(R2+c);
					if ((I32)t2 < (R2-c))
						t2 = (U8)(R2-c);
				} 
				rt[-2] = t1;
				rt[1] = t2;

				if(!bChroma)
				{
					rt[-3] = (U8)((26*(L3 + L4 + t1) + 25*(L3 + rt[-1]) + 64)>>7);
					rt[2] = (U8)((25*(rt[0] + R3) + 26*(t2 + R4 + R3) + 64)>>7);
				}
			} /*z */
		} 
		else
		{
#if defined(VOARMV7)
			if(!Al || !Ar){
				c >>= 1;
				Cl>>= 1;
				Cr>>= 1;
			}
			for(z = 0; z < 4; z++, rt += uPitch){
				rv40_weak_loop_filter(rt, 1, Al, Ar, alpha, betaX, c, Cr, Cl,deltaL[z], deltaR[z],deltaL2[z], deltaR2[z]);
			}	
#else
			if(Al!=1 && Ar!=1) 
			{ 
				for(z=0;z<4;z++, rt+=uPitch)
				{	
					I32 d;

					L1 = (I32) rt[-1];
					R1 = (I32) rt[0];								
					delta = (R1 -L1);
					if(delta == 0) continue;
					if(((abs(delta)*alpha)>>7)>2) continue;
					L2 = (I32) rt[-2];
					R2 = (I32) rt[1];
					d = (L2 - R2 + ((delta) << 2) + 4) >> 3;
					if (d > c)
						d = c;
					if ((d  + c) < 0)
						d = -c;
					rt[-1] = ClampTbl[CLAMP_BIAS + L1 + d];
					rt[0] = ClampTbl[CLAMP_BIAS + R1 - d];
					if(abs(deltaL2[z])<= betaX) 
					{ 
						d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
						if (d2 > (I32)Cl)
							d2 = (I32)Cl;
						if ((d2 + (I32)Cl) < 0)
							d2 = -(I32)Cl;
						rt[-2] = ClampTbl[CLAMP_BIAS + L2 - d2];
					}
					if(abs(deltaR2[z])<= betaX) 
					{ 
						d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
						if (d2 > (I32)Cr)
							d2 = (I32)Cr;
						if ((d2 + (I32)Cr) < 0)
							d2 = -(I32)Cr;
						rt[1] = ClampTbl[CLAMP_BIAS + R2 - d2];
					}
				}
			}
			else 
			{						
				for(z=0;z<4;z++, rt+=uPitch)
				{
					I32 d;
					L1 = (I32) rt[-1];
					R1 = (I32) rt[0];								
					delta = (R1 -L1);
					if(delta == 0) continue;
					if(((abs(delta)*alpha)>>7)>3) continue;
					d = (delta + 1) >> 1;
					lTemp = (c>>1);
					if (d > lTemp)
						d = lTemp;
					if ((d + lTemp) < 0)
						d = -lTemp;
					rt[-1] = ClampTbl[CLAMP_BIAS + L1 + d];
					rt[0] = ClampTbl[CLAMP_BIAS + R1 - d];					
					if(Al!=1 && abs(deltaL2[z])<= betaX) 
					{ 
						d2 = (deltaL[z] + deltaL2[z] - d) >> 1;
						lTemp = (I32)(Cl>>1);
						if (d2 > lTemp)
							d2 = lTemp;
						if ((d2 + lTemp) < 0)
							d2 = -lTemp;
						rt[-2] = ClampTbl[CLAMP_BIAS + rt[-2] - d2];
					}
					if(Ar!=1 && abs(deltaR2[z])<= betaX) 
					{ 
						d2 = (deltaR[z] + deltaR2[z] + d) >> 1;
						lTemp = (I32)(Cr>>1);
						if (d2 > lTemp)
							d2 = lTemp;
						if ((d2 + lTemp) < 0)
							d2 = -lTemp;
						rt[1] = ClampTbl[CLAMP_BIAS + rt[1] - d2];
					}
				} /* z */
			}
#endif
		} /* if */
	} /* > 2 */
}	/* rv9_v_loop_filter */

#endif
#if 0
#if defined(VOARMV7)
static void rv9_h_loop_filter_2block_C(
									 U8 *pPels,			/* first of 4 */
									 U32 uPitch,
									 U32 Cr[],				/* clip for right */
									 U32 Cl[],				/* clip for left */
									 I32 alpha,
									 I32 betaX,
									 I32 beta2,
									 U32 yuv_h_deblock
									 )
{
	I32 deltaL[2][4], deltaL2[2][4], deltaR[2][4], deltaR2[2][4];
	I32 sum_p1p0[2],sum_q1q0[2];
	U32 Cl_temp[2],Cr_temp[2];
	U8  *ptr;
	I32 filter_p1[2], filter_q1[2];
	I32 i;
	I32 lims[2];

	for(i = 0; i < 2; i++){
		sum_p1p0[i] = 0;
		sum_q1q0[i] = 0;
	}

	for(i = 0, ptr = pPels; i < 4; i++, ptr++){
		deltaL[0][i] = ptr[-2*uPitch] - ptr[-1*uPitch];
		deltaR[0][i] = ptr[ 1*uPitch] - ptr[ 0*uPitch];
		sum_p1p0[0] += deltaL[0][i];
		sum_q1q0[0] += deltaR[0][i];
	}

	for(i = 0, ptr = pPels + 4; i < 4; i++, ptr++){
		deltaL[1][i] = ptr[-2*uPitch] - ptr[-1*uPitch];
		deltaR[1][i] = ptr[1*uPitch] - ptr[ 0*uPitch];
		sum_p1p0[1] += deltaL[1][i];
		sum_q1q0[1] += deltaR[1][i];
	}

	if(yuv_h_deblock & 1){
		filter_p1[0] = abs(sum_p1p0[0]) < (betaX << 2);
		filter_q1[0] = abs(sum_q1q0[0]) < (betaX << 2);
	}else{
		filter_p1[0] = 0;
		filter_q1[0] = 0;
	}

	if(yuv_h_deblock & 2){
		filter_p1[1] = abs(sum_p1p0[1]) < (betaX << 2);
		filter_q1[1] = abs(sum_q1q0[1]) < (betaX << 2);
	}else{
		filter_p1[1] = 0;
		filter_q1[1] = 0;
	}

	if(!filter_p1[0] && !filter_q1[0] && !filter_p1[1] && !filter_q1[1])
		return;

	for(i = 0, ptr = pPels; i < 4; i++, ptr++){
		deltaL2[0][i] = ptr[-2*uPitch] - ptr[-3*uPitch];
		deltaR2[0][i] = ptr[ 1*uPitch] - ptr[ 2*uPitch];
	}

	for(i = 0, ptr = pPels + 4; i < 4; i++, ptr++){
		deltaL2[1][i] = ptr[-2*uPitch] - ptr[-3*uPitch];
		deltaR2[1][i] = ptr[ 1*uPitch] - ptr[ 2*uPitch];
	}

	lims[0] = filter_p1[0] + filter_q1[0] + ((Cr[0] + Cl[0]) >> 1) + 1;
	lims[1] = filter_p1[1] + filter_q1[1] + ((Cr[1] + Cl[1]) >> 1) + 1;
	Cr_temp[0]  = Cr[0]; 
	Cl_temp[0]  = Cl[0];
	Cr_temp[1]  = Cr[1]; 
	Cl_temp[1]  = Cl[1];
	if(!filter_p1[0] || !filter_q1[0]){
		lims[0]    >>= 1;
		Cr_temp[0]   = Cr[0] >> 1; 
		Cl_temp[0]   = Cl[0] >> 1;
	}

	if(!filter_p1[1] || !filter_q1[1]){
		lims[1]   >>= 1;
		Cr_temp[1]  = Cr[1] >> 1; 
		Cl_temp[1]  = Cl[1] >> 1;
	}

	ptr = pPels;
	for(i = 0; i < 4; i++, ptr++)
		rv40_weak_loop_filter(ptr, uPitch, filter_p1[0], filter_q1[0], alpha, betaX, lims[0], Cr_temp[0], Cl_temp[0],deltaL[0][i], deltaR[0][i],
		deltaL2[0][i], deltaR2[0][i]);
	ptr = pPels + 4;
	for(i = 0; i < 4; i++, ptr++)
		rv40_weak_loop_filter(ptr, uPitch, filter_p1[1], filter_q1[1], alpha, betaX, lims[1], Cr_temp[1], Cl_temp[1],deltaL[1][i], deltaR[1][i], 
		deltaL2[1][i], deltaR2[1][i]);
}
#endif //VOARMV7
#endif
RV_Status InLoopFilterRV9(struct Decoder *t, U32 row)
{
	struct DecoderMBInfo  *pMB;
	U32 mb_x;
	U32 mb_width  = t->m_mbX;
	U32 mb_height = t->m_mbY;
	U32 cbp, cbp_left, cbp_above, cbp_below;//cur, left ,above,below
	U32 uvcbp[2],uvcbp_left[2],uvcbp_above[2],uvcbp_below[2];
	U32 mbtype,mbtype_left,mbtype_above,mbtype_below;
	U32 mvd,mvd_left,mvd_above,mvd_below;
	U32 mb_strong,mb_strong_left,mb_strong_above,mb_strong_below;
	U32 clip,clip_left,clip_above,clip_below; 
	U32 y_h_deblock, y_v_deblock;
	U32 c_v_deblock[2], c_h_deblock[2];
	U32 y_sh_deblock, y_sv_deblock;
	U32 c_sv_deblock[2], c_sh_deblock[2];
	U32 y_to_deblock, c_to_deblock[2];
	U32 cur_bit,right_bit;
	U32 y_v_deblock_bak;
	U32 c_v_deblock_bak[2];
	U32 CIdx;
	U8  *pYPlane, *pCPlane[2];
	U8  *Y, *C;
	I32 alpha, beta, betaY, betaC;
#if defined (VOARMV7)
        U32 clip_block_left, clip_block_cur[2],clip_block_top, clip_block_bot[2];
	I32 dither[2];
	U32 yuv_h_deblock;
	U32 i;
#else
        U32 clip_block_left, clip_block_cur,clip_block_top, clip_block_bot;
	I32 dither;
	U32 block_num;
#endif
	U32 j;
	I32 q;

	pYPlane = t->m_pCurrentFrame->m_pYPlane + (row << 4) * t->m_img_stride;
	pCPlane[0] = t->m_pCurrentFrame->m_pUPlane + (row << 3) * t->m_img_UVstride;
	pCPlane[1] = t->m_pCurrentFrame->m_pVPlane + (row << 3) * t->m_img_UVstride;

	pMB = &t->m_slice_di[0].m_pMBInfo[row * mb_width];

	for(mb_x = 0; mb_x < mb_width; mb_x++, pMB++){
		q = pMB->QP;
		alpha = alpha_tab[q];
		beta  = beta_tab[q];
		betaY = betaC = beta * 3;
		if(t->m_img_width * t->m_img_height <= 176*144)
			betaY += beta;

		cbp = pMB->deblock_cbpcoef;
		mvd = pMB->mvd;
		mbtype = pMB->mbtype;

		mbtype_left = mb_x ? (pMB - 1)->mbtype : mbtype;
		mvd_left    = mb_x ? (pMB - 1)->mvd : 0;
		cbp_left    = mb_x ? (pMB - 1)->deblock_cbpcoef : 0;

		mbtype_above = row ? (pMB - mb_width)->mbtype : mbtype;
		mvd_above    = row ? (pMB - mb_width)->mvd : 0;
		cbp_above    = row ? (pMB - mb_width)->deblock_cbpcoef : 0;

		mbtype_below = (row < mb_height - 1) ? (pMB + mb_width)->mbtype : mbtype;
		mvd_below    = (row < mb_height - 1) ? (pMB + mb_width)->mvd : 0;
		cbp_below    = (row < mb_height - 1) ? (pMB + mb_width)->deblock_cbpcoef : 0;

		mb_strong = IS_INTRA_MBTYPE(mbtype) || IS_INTER_16X16_MBTYPE(mbtype);

		if (q > 31){
			return RV_S_ERROR;
		}

		clip = clip_table[mb_strong + 1][q];

		mb_strong_left = IS_INTRA_MBTYPE(mbtype_left) || IS_INTER_16X16_MBTYPE(mbtype_left);
		clip_left = clip_table[mb_strong_left + 1][q];

		mb_strong_above = IS_INTRA_MBTYPE(mbtype_above) || IS_INTER_16X16_MBTYPE(mbtype_above);
		clip_above = clip_table[mb_strong_above + 1][q];

		mb_strong_below = IS_INTRA_MBTYPE(mbtype_below) || IS_INTER_16X16_MBTYPE(mbtype_below);
		clip_below = clip_table[mb_strong_below + 1][q];


		uvcbp[0] = (cbp >> 16) &0xf;
		uvcbp[1] = (cbp >> 20) &0xf;
		cbp &= 0xffff;


		uvcbp_left[0] = (cbp_left >> 16) & uRightBlocksMaskChroma;
		uvcbp_left[1] = (cbp_left >> 20) & uRightBlocksMaskChroma;
		cbp_left &= uRightBlocksMask;
		mvd_left &= uRightBlocksMask;


		uvcbp_above[0] = (cbp_above >> 16) & uBottomBlocksMaskChroma;
		uvcbp_above[1] = (cbp_above >> 20) & uBottomBlocksMaskChroma;
		cbp_above &= uBottomBlocksMask;
		mvd_above &= uBottomBlocksMask;


		uvcbp_below[0] = (cbp_below >> 16) & uTopBlocksMaskChroma;
		uvcbp_below[1] = (cbp_below >> 20) & uTopBlocksMaskChroma;
		cbp_below &= uTopBlocksMask;
		mvd_below &= uTopBlocksMask;

		mvd |= cbp;
		mvd_left |= cbp_left;
		mvd_above |= cbp_above;
		mvd_below |= cbp_below;

		y_to_deblock =  mvd | mvd_below << 16;

		y_h_deblock =   y_to_deblock
			| ((cbp <<  4) & ~uTopBlocksMask)
			| (cbp_above >> 12);

		y_v_deblock =   y_to_deblock
			| ((cbp << 1) & ~uLeftBlocksMask)
			| (cbp_left >> 3);

		c_to_deblock[0] = (uvcbp_below[0] << 4) | uvcbp[0];
		c_v_deblock[0] =   c_to_deblock[0]
		| ((uvcbp[0] << 1) & ~uLeftBlocksMaskChroma)
			| (uvcbp_left[0] >> 1);
		c_h_deblock[0] =   c_to_deblock[0]
		| (uvcbp_above[0] >> 2)
			|  (uvcbp[0] << 2);

		c_to_deblock[1] = (uvcbp_below[1] << 4) | uvcbp[1];
		c_v_deblock[1] =   c_to_deblock[1]
		| ((uvcbp[1] << 1) & ~uLeftBlocksMaskChroma)
			| (uvcbp_left[1] >> 1);
		c_h_deblock[1] =   c_to_deblock[1]
		| (uvcbp_above[1] >> 2)
			|  (uvcbp[1] << 2);


		if(!mb_x ){
			y_v_deblock &= ~uLeftBlocksMask;
			c_v_deblock[0] &= ~uLeftBlocksMaskChroma;
			c_v_deblock[1] &= ~uLeftBlocksMaskChroma;
		}

		if(!row){
			y_h_deblock &= ~uTopBlocksMask;
			c_h_deblock[0] &= ~uTopBlocksMaskChroma;
			c_h_deblock[1] &= ~uTopBlocksMaskChroma;
		}

		if(row == mb_height - 1 || (mb_strong || mb_strong_below)){
			y_h_deblock &= ~(uTopBlocksMask << 16);
			c_h_deblock[0] &= ~(uTopBlocksMaskChroma << 4);
			c_h_deblock[1] &= ~(uTopBlocksMaskChroma << 4);
		}

		y_v_deblock_bak = y_v_deblock;
		c_v_deblock_bak[0] = c_v_deblock[0];
		c_v_deblock_bak[1] = c_v_deblock[1];

		if (mb_strong || mb_strong_above)
		{
			y_sh_deblock = y_h_deblock & uTopBlocksMask;
			c_sh_deblock[0] = c_h_deblock[0] & uTopBlocksMaskChroma;
			c_sh_deblock[1] = c_h_deblock[1] & uTopBlocksMaskChroma;
		}else{
			y_sh_deblock = 0;
			c_sh_deblock[0] = 0;
			c_sh_deblock[1] = 0;
		}
		if (mb_strong || mb_strong_left)
		{
			y_sv_deblock = y_v_deblock & uLeftBlocksMask;
			c_sv_deblock[0] = c_v_deblock[0] & uLeftBlocksMaskChroma;
			c_sv_deblock[1] = c_v_deblock[1] & uLeftBlocksMaskChroma;

			y_v_deblock &= ~uLeftBlocksMask;
			c_v_deblock[0] &= ~uLeftBlocksMaskChroma;
			c_v_deblock[1] &= ~uLeftBlocksMaskChroma;
		}else{
			y_sv_deblock = 0;
			c_sv_deblock[0] = 0;
			c_sv_deblock[1] = 0;
		}
#if defined(VOARMV7)
		//Y
		if(y_h_deblock | y_v_deblock_bak){

			//block 0 1
			Y = pYPlane + (mb_x << 4);
			clip_block_cur[0] = (y_to_deblock & MASK_CUR) ? clip : 0;
			clip_block_cur[1] = (y_to_deblock & (MASK_CUR << 1)) ? clip : 0;
			dither[0] = 0;
			dither[1] = 4;

			// if bottom block is coded then we can filter its top edge
			// (or bottom edge of this block, which is the same)
			if(y_h_deblock & 0x30){
				yuv_h_deblock = (y_h_deblock & 0x30) >> 4;
				clip_block_bot[0] = (y_to_deblock & MASK_BOTTOM) ? clip : 0;
				clip_block_bot[1] = (y_to_deblock & (MASK_BOTTOM << 1)) ? clip : 0;
				rv9_h_loop_filter_2block(Y + 4*t->m_img_stride, t->m_img_stride,
					clip_block_bot,clip_block_cur, alpha, beta, betaY, yuv_h_deblock);
			}

			for(i = 0;i < 2;i++, Y += 4){
				cur_bit = MASK_CUR << i;
				if(!i){
					clip_block_left = (mvd_left & MASK_RIGHT) ? clip_left : 0;
				} else{
					clip_block_left = (y_to_deblock & cur_bit >> 1) ? clip : 0;
				}
				// filter left block edge in ordinary mode (with low filtering strength)
				if(y_v_deblock & cur_bit){
					rv9_v_loop_filter_Y(Y, t->m_img_stride, dither[i], clip_block_cur[i], clip_block_left,alpha, beta, betaY, 0, 0);
				}
				// filter top edge of the current macroblock when filtering strength is high
				if(y_sh_deblock & cur_bit){
					rv9_h_loop_filter_Y(Y, t->m_img_stride, dither[i],clip_block_cur[i],
						(mvd_above & (MASK_TOP << i)) ? clip_above : 0,
						alpha, beta, betaY, 0, 1);
				}
				// filter left block edge in edge mode (with high filtering strength)
				if(y_sv_deblock & cur_bit){
					rv9_v_loop_filter_Y(Y, t->m_img_stride, dither[i],clip_block_cur[i],clip_block_left,alpha, beta, betaY, 0, 1);
				}				
			}

			//block 2 3
			clip_block_cur[0] = (y_to_deblock & (MASK_CUR << 2)) ? clip : 0;
			clip_block_cur[1] = (y_to_deblock & (MASK_CUR << 3)) ? clip : 0;
			dither[0] = 8;
			dither[1] = 12;
			if(y_h_deblock & 0xc0){
				yuv_h_deblock = (y_h_deblock & 0xc0) >> 6;
				clip_block_bot[0] = (y_to_deblock & (MASK_BOTTOM << 2)) ? clip : 0;
				clip_block_bot[1] = (y_to_deblock & (MASK_BOTTOM << 3)) ? clip : 0;
				rv9_h_loop_filter_2block(Y + 4*t->m_img_stride, t->m_img_stride,
					clip_block_bot,clip_block_cur, alpha, beta, betaY,yuv_h_deblock);
			}

			for(i = 2;i < 4;i++, Y += 4){
				cur_bit = MASK_CUR << i;
				// filter left block edge in ordinary mode (with low filtering strength)
				if(y_v_deblock & cur_bit){
					clip_block_left = (y_to_deblock & cur_bit >> 1) ? clip : 0;
					rv9_v_loop_filter_Y(Y, t->m_img_stride, dither[i - 2], clip_block_cur[i - 2], clip_block_left,alpha, beta, betaY, 0, 0);
				}
				// filter top edge of the current macroblock when filtering strength is high
				if(y_sh_deblock & cur_bit){
					rv9_h_loop_filter_Y(Y, t->m_img_stride, dither[i - 2],clip_block_cur[i - 2],
						(mvd_above & (MASK_TOP << i)) ? clip_above : 0,
						alpha, beta, betaY, 0, 1);
				}			
			}

			//Loop over lower 3 rows of MB blocks 
			for(j = 4;j < 16; j += 4){
				Y = pYPlane + (mb_x << 4) + j*t->m_img_stride;

				cur_bit = MASK_CUR << j;
				clip_block_cur[0] = (y_to_deblock & cur_bit) ? clip : 0;
				clip_block_cur[1] = (y_to_deblock & (cur_bit << 1)) ? clip : 0;
				dither[0] = j;
				dither[1] = j + 1;
				if(y_h_deblock & (0x30 << j)){
					yuv_h_deblock = (y_h_deblock & (0x30 << j)) >> (j + 4);
					if(j < 12){
					clip_block_bot[0] = (y_to_deblock & (MASK_BOTTOM << j)) ? clip : 0;
					clip_block_bot[1] = (y_to_deblock & (MASK_BOTTOM << (1 + j))) ? clip : 0;
					}else{
						clip_block_bot[0] = (y_to_deblock & (MASK_BOTTOM << j)) ? clip_below : 0;
						clip_block_bot[1] = (y_to_deblock & (MASK_BOTTOM << (1 + j))) ? clip_below : 0;
					}					
					rv9_h_loop_filter_2block(Y + 4*t->m_img_stride, t->m_img_stride,
						clip_block_bot,clip_block_cur, alpha, beta, betaY,yuv_h_deblock);
				}

				for (i = 0; i < 2; i++, Y += 4){
					cur_bit = MASK_CUR << (i + j);
					right_bit = MASK_RIGHT << (i + j);
					if(!i){
						clip_block_left = mvd_left & MASK_RIGHT << j ? clip_left : 0;
					}else{
						clip_block_left = (y_to_deblock & (cur_bit >> 1)) ? clip : 0;
					}
					// filter left block edge in ordinary mode (with low filtering strength)
					if(y_v_deblock & cur_bit){
						rv9_v_loop_filter_Y(Y, t->m_img_stride, dither[i], clip_block_cur[i], clip_block_left,alpha, beta, betaY, 0, 0);
					}
					// filter left block edge in edge mode (with high filtering strength)
					if(y_sv_deblock & cur_bit){
						rv9_v_loop_filter_Y(Y, t->m_img_stride, dither[i],clip_block_cur[i],clip_block_left,alpha, beta, betaY, 0, 1);
					}
				}

				cur_bit = MASK_CUR << (2 +j);
				clip_block_cur[0] = (y_to_deblock & cur_bit) ? clip : 0;
				clip_block_cur[1] = (y_to_deblock & (cur_bit << 1)) ? clip : 0;
				dither[0] = j + 2;
				dither[1] = j + 3;
				if(y_h_deblock & (0x30 << (j + 2))){
					yuv_h_deblock = (y_h_deblock & (0x30 << (j + 2))) >> (j + 6);
					if(j < 12){
						clip_block_bot[0] = (y_to_deblock & (MASK_BOTTOM << (2 + j))) ? clip : 0;
						clip_block_bot[1] = (y_to_deblock & (MASK_BOTTOM << (3 + j))) ? clip : 0;
					}else{
						clip_block_bot[0] = (y_to_deblock & (MASK_BOTTOM << (2 + j))) ? clip_below : 0;
						clip_block_bot[1] = (y_to_deblock & (MASK_BOTTOM << (3 + j))) ? clip_below : 0;
					}
					rv9_h_loop_filter_2block(Y + 4*t->m_img_stride, t->m_img_stride,
						clip_block_bot,clip_block_cur, alpha, beta, betaY,yuv_h_deblock);
				}

				for (i = 2; i < 4; i++, Y += 4){
					cur_bit = MASK_CUR << (i + j);
					// filter left block edge in ordinary mode (with low filtering strength)
					if(y_v_deblock & cur_bit){
						clip_block_left = (y_to_deblock & (cur_bit >> 1)) ? clip : 0;
						rv9_v_loop_filter_Y(Y, t->m_img_stride, dither[i - 2], clip_block_cur[i - 2], clip_block_left,alpha, beta, betaY, 0, 0);
					}
				}
			}
		}

		//UV
		if(c_h_deblock[0] | c_v_deblock_bak[0] | c_h_deblock[1] | c_v_deblock_bak[1]){
			for(CIdx = 0; CIdx < 2; CIdx++){
				//chrom block 0 1
				C = pCPlane[CIdx] + (mb_x << 3);
				clip_block_cur[0] = (c_to_deblock[CIdx] & MASK_CUR) ? clip : 0;
				clip_block_cur[1] = (c_to_deblock[CIdx] & (MASK_CUR << 1)) ? clip : 0;
				dither[0] = 0;
				dither[1] = 8;
				if(c_h_deblock[CIdx] & 0x0c){
					yuv_h_deblock = (c_h_deblock[CIdx] & 0x0c) >> 2;
					clip_block_bot[0] = (c_to_deblock[CIdx] & (MASK_CUR << 2)) ? clip : 0;
					clip_block_bot[1] = (c_to_deblock[CIdx] & (MASK_CUR << 3)) ? clip : 0;
					rv9_h_loop_filter_2block(C + 4*t->m_img_UVstride, t->m_img_UVstride,
						clip_block_bot,
						clip_block_cur,
						alpha, beta, betaC,yuv_h_deblock);
				}

				for(i = 0; i < 2; i++, C += 4){
					cur_bit = MASK_CUR << i;
					if(!i){
						clip_block_left = uvcbp_left[CIdx] & (MASK_CUR << 1) ? clip_left : 0;
					}else{
						clip_block_left = (c_to_deblock[CIdx] & (cur_bit >> 1)) ? clip : 0;
					}
					if(c_v_deblock[CIdx] & cur_bit){
						rv9_v_loop_filter_UV(C, t->m_img_UVstride, 0,
							clip_block_cur[i],
							clip_block_left,
							alpha, beta, betaC, 1, 0);
					}
					if(c_sh_deblock[CIdx] & cur_bit){
						clip_block_top = uvcbp_above[CIdx] & (MASK_CUR << (2 + i)) ? clip_above : 0;
						rv9_h_loop_filter_UV(C, t->m_img_UVstride, i * 8,
							clip_block_cur[i],
							clip_block_top,
							alpha, beta, betaC, 1, 1);
					}
					if(c_sv_deblock[CIdx] & cur_bit){
						rv9_v_loop_filter_UV(C, t->m_img_UVstride, 0,
							clip_block_cur[i],
							clip_block_left,
							alpha, beta, betaC, 1, 1);
					}
				}

				//chrom block 2 3
				C = pCPlane[CIdx] + (mb_x << 3) + (t->m_img_UVstride << 2);
				clip_block_cur[0] = (c_to_deblock[CIdx] & (MASK_CUR << 2)) ? clip : 0;
				clip_block_cur[1] = (c_to_deblock[CIdx] & (MASK_CUR << 3)) ? clip : 0;
				dither[0] = 0;
				dither[1] = 8;
				if(c_h_deblock[CIdx] & 0x30){
					yuv_h_deblock = (c_h_deblock[CIdx] & 0x30) >> 4;
					clip_block_bot[0] = (c_to_deblock[CIdx] & (MASK_CUR << 4)) ? clip_below : 0;
					clip_block_bot[1] = (c_to_deblock[CIdx] & (MASK_CUR << 5)) ? clip_below : 0;
					rv9_h_loop_filter_2block(C + 4*t->m_img_UVstride, t->m_img_UVstride,
						clip_block_bot,
						clip_block_cur,
						alpha, beta, betaC,yuv_h_deblock);
				}

				for(i = 0; i < 2; i++, C += 4){
					cur_bit = MASK_CUR << (i + 2);
					if(!i){
						clip_block_left =(uvcbp_left[CIdx] & MASK_CUR << 3) ? clip_left : 0;
					}else{
						clip_block_left = (c_to_deblock[CIdx] & (cur_bit >> 1)) ? clip : 0;
					}
					if(c_v_deblock[CIdx] & cur_bit){
						rv9_v_loop_filter_UV(C, t->m_img_UVstride, 8,
							clip_block_cur[i],
							clip_block_left,
							alpha, beta, betaC, 1, 0);
					}
					if(c_sv_deblock[CIdx] & cur_bit){
						rv9_v_loop_filter_UV(C, t->m_img_UVstride, 8,
							clip_block_cur[i],
							clip_block_left,
							alpha, beta, betaC, 1, 1);
					}
				}
			}
		}
#else //VOARMV7
		//Y
		if(y_h_deblock | y_v_deblock_bak){
			//block 0
			Y = pYPlane + (mb_x << 4);
			if(((y_h_deblock|y_v_deblock_bak) & MASK_CUR)||(y_h_deblock & MASK_BOTTOM)){
				clip_block_cur = (y_to_deblock & MASK_CUR) ? clip : 0;
				// if bottom block is coded then we can filter its top edge
				// (or bottom edge of this block, which is the same)
				if(y_h_deblock & MASK_BOTTOM){
					rv9_h_loop_filter(Y + 4*t->m_img_stride, t->m_img_stride, 0,
						(y_to_deblock & MASK_BOTTOM) ? clip : 0,
						clip_block_cur,
						alpha, beta, betaY, 0, 0);

				}
				// filter left block edge in ordinary mode (with low filtering strength)
				if(y_v_deblock & MASK_CUR){
					clip_block_left = (mvd_left & MASK_RIGHT) ? clip_left : 0;
					rv9_v_loop_filter(Y, t->m_img_stride, 0, clip_block_cur, clip_block_left,alpha, beta, betaY, 0, 0);
				}
				// filter top edge of the current macroblock when filtering strength is high
				if(y_sh_deblock & MASK_CUR){
					rv9_h_loop_filter(Y, t->m_img_stride, 0,clip_block_cur,
						(mvd_above & MASK_TOP) ? clip_above : 0,
						alpha, beta, betaY, 0, 1);
				}
				// filter left block edge in edge mode (with high filtering strength)
				if(y_sv_deblock & MASK_CUR){
					clip_block_left = (mvd_left & MASK_RIGHT) ? clip_left : 0;
					rv9_v_loop_filter(Y, t->m_img_stride, 0,clip_block_cur,clip_block_left,alpha, beta, betaY, 0, 1);
				}
	}

			Y += 4;
			//block 1 2 3
			for(block_num = 1; block_num < 4; block_num++,Y += 4){
				U32 bottom_bit;

				cur_bit = MASK_CUR << block_num;
				bottom_bit = MASK_BOTTOM << block_num;
				if(((y_h_deblock|y_v_deblock_bak) & cur_bit)||(y_h_deblock & bottom_bit)){
					clip_block_cur = (y_to_deblock & cur_bit) ? clip : 0;
					dither = block_num << 2;
					// if bottom block is coded then we can filter its top edge
					// (or bottom edge of this block, which is the same)
					if(y_h_deblock & bottom_bit){
						rv9_h_loop_filter(Y + 4*t->m_img_stride, t->m_img_stride, dither,
							y_to_deblock & bottom_bit ? clip : 0,
							clip_block_cur,
							alpha, beta, betaY, 0, 0);
					}
					// filter left block edge in ordinary mode (with low filtering strength)
					if(y_v_deblock & cur_bit){
						clip_block_left = (y_to_deblock & cur_bit >> 1) ? clip : 0;
						rv9_v_loop_filter(Y, t->m_img_stride, dither, clip_block_cur, clip_block_left,alpha, beta, betaY, 0, 0);
					}
					// filter top edge of the current macroblock when filtering strength is high
					if(y_sh_deblock & cur_bit){
						rv9_h_loop_filter(Y, t->m_img_stride, dither,clip_block_cur,
							mvd_above & (MASK_TOP << block_num) ? clip_above : 0,
							alpha, beta, betaY, 0, 1);
					}
				}
			}

			//Loop over lower 3 rows of MB blocks 
			for(j = 4;j < 16; j += 4){
				U32 bottom_bit;
				Y = pYPlane + (mb_x << 4) + j*t->m_img_stride;
				cur_bit = MASK_CUR << j;
				bottom_bit = MASK_BOTTOM << j;
				right_bit = MASK_RIGHT << j;
				//block 4,8,12
				if((y_v_deblock_bak & cur_bit)||(y_h_deblock & bottom_bit)){
					// if bottom block is coded then we can filter its top edge
					// (or bottom edge of this block, which is the same)
					clip_block_cur = y_to_deblock & cur_bit ? clip : 0;
					dither = j;
					if(y_h_deblock & bottom_bit){
						if(j < 12){
							clip_block_bot = y_to_deblock & bottom_bit ? clip : 0;
						}else{
							clip_block_bot = y_to_deblock & bottom_bit ? clip_below: 0;
						}
						rv9_h_loop_filter(Y + 4*t->m_img_stride, t->m_img_stride, dither,
							clip_block_bot,
							clip_block_cur,
							alpha, beta, betaY, 0, 0);

					}

					// filter left block edge in ordinary mode (with low filtering strength)
					if(y_v_deblock & cur_bit){
						clip_block_left = mvd_left & right_bit ? clip_left : 0;
						rv9_v_loop_filter(Y, t->m_img_stride, dither, clip_block_cur, clip_block_left,alpha, beta, betaY, 0, 0);
					}
					// filter left block edge in edge mode (with high filtering strength)
					if(y_sv_deblock & cur_bit){
						clip_block_left = mvd_left & right_bit ? clip_left : 0;
						rv9_v_loop_filter(Y, t->m_img_stride, dither,clip_block_cur,clip_block_left,alpha, beta, betaY, 0, 1);
					}
				}

				Y += 4;
				if((y_v_deblock_bak & (0x0e << j))||(y_h_deblock & (0xe0 << j) )){
					//blocks 5-7, 9-11, 13-15
					for(block_num = j + 1;block_num < j + 4; block_num++, Y += 4){
						cur_bit = MASK_CUR << block_num;
						bottom_bit= MASK_BOTTOM << block_num;
						clip_block_cur = (y_to_deblock & cur_bit) ? clip : 0;
						dither = block_num;
						// if bottom block is coded then we can filter its top edge
						// (or bottom edge of this block, which is the same)
						if(y_h_deblock & bottom_bit){
							if(j < 12){
								clip_block_bot = y_to_deblock & bottom_bit ? clip : 0;
							}else{
								clip_block_bot = y_to_deblock & bottom_bit ? clip_below : 0;
							}
							rv9_h_loop_filter(Y + 4*t->m_img_stride, t->m_img_stride, dither,
								clip_block_bot,clip_block_cur,
								alpha, beta, betaY, 0, 0);
						}
						// filter left block edge in ordinary mode (with low filtering strength)
						if(y_v_deblock & cur_bit){
							clip_block_left = (y_to_deblock & (cur_bit >> 1)) ? clip : 0;
							rv9_v_loop_filter(Y,t->m_img_stride, dither, clip_block_cur, clip_block_left,alpha, beta, betaY, 0, 0);
						}
					}
				}
			}
		}

		//UV
		if(c_h_deblock[0] | c_v_deblock_bak[0] | c_h_deblock[1] | c_v_deblock_bak[1]){
			for(CIdx = 0; CIdx < 2; CIdx++){
				U32 bottom_bit;
				C = pCPlane[CIdx] + (mb_x << 3);
				bottom_bit = MASK_CUR <<  2;
				//chrom block 0
				if(((c_h_deblock[CIdx]|c_v_deblock_bak[CIdx]) & MASK_CUR)||(c_h_deblock[CIdx] & bottom_bit)){
					clip_block_cur = (c_to_deblock[CIdx] & MASK_CUR) ? clip : 0;
					if(c_h_deblock[CIdx] & bottom_bit){
						clip_block_bot = c_to_deblock[CIdx] & bottom_bit ? clip : 0;
						rv9_h_loop_filter(C+4*t->m_img_UVstride, t->m_img_UVstride, 0,
							clip_block_bot,
							clip_block_cur,
							alpha, beta, betaC, 1, 0);
					}
					if(c_v_deblock[CIdx] & MASK_CUR){
						clip_block_left = uvcbp_left[CIdx] & (MASK_CUR << 1) ? clip_left : 0;
						rv9_v_loop_filter(C, t->m_img_UVstride, 0,
							clip_block_cur,
							clip_block_left,
							alpha, beta, betaC, 1, 0);
					}
					if(c_sh_deblock[CIdx] & MASK_CUR){
						clip_block_top = uvcbp_above[CIdx] & bottom_bit ? clip_above : 0;
						rv9_h_loop_filter(C, t->m_img_UVstride, 0,
							clip_block_cur,
							clip_block_top,
							alpha, beta, betaC, 1, 1);
					}
					if(c_sv_deblock[CIdx] & MASK_CUR){
						clip_block_left = (uvcbp_left[CIdx] & (MASK_CUR << 1) )? clip_left : 0;
						rv9_v_loop_filter(C, t->m_img_UVstride, 0,
							clip_block_cur,
							clip_block_left,
							alpha, beta, betaC, 1, 1);
					}
				}

				C += 4;
				cur_bit = MASK_CUR << 1;
				bottom_bit = MASK_CUR << (2 + 1);
				//chrom block 1
				if(((c_h_deblock[CIdx]|c_v_deblock_bak[CIdx]) & cur_bit)||(c_h_deblock[CIdx] & bottom_bit)){
					clip_block_cur = (c_to_deblock[CIdx] & cur_bit ) ? clip : 0;
					if(c_h_deblock[CIdx] & bottom_bit){
						clip_block_bot = (c_to_deblock[CIdx] & bottom_bit) ? clip : 0;
						rv9_h_loop_filter(C+4*t->m_img_UVstride, t->m_img_UVstride, 8,
							clip_block_bot,
							clip_block_cur,
							alpha, beta, betaC, 1, 0);
					}
					if(c_v_deblock[CIdx] & cur_bit){
						clip_block_left = (c_to_deblock[CIdx] & (cur_bit >> 1))  ? clip : 0;
						rv9_v_loop_filter(C, t->m_img_UVstride, 0,
							clip_block_cur,
							clip_block_left,
							alpha, beta, betaC, 1, 0);
					}
					if(c_sh_deblock[CIdx] & cur_bit){
						int clip_block_top = (uvcbp_above[CIdx] & bottom_bit) ? clip_above : 0;
						rv9_h_loop_filter(C, t->m_img_UVstride, 8,
							clip_block_cur,
							clip_block_top,
							alpha, beta, betaC, 1, 1);
					}
				}

				//chrom block 2
				C = pCPlane[CIdx] + (mb_x << 3) + (t->m_img_UVstride << 2);
				cur_bit = MASK_CUR << 2;
				bottom_bit = MASK_CUR << (2 + 2);
				right_bit = MASK_CUR << (2 + 1);
				if((c_v_deblock_bak[CIdx] & cur_bit)||(c_h_deblock[CIdx] & bottom_bit)){
					clip_block_cur = (c_to_deblock[CIdx] & cur_bit ) ? clip : 0;
					if(c_h_deblock[CIdx] & bottom_bit){
						clip_block_bot = (c_to_deblock[CIdx] & bottom_bit) ? clip_below : 0;
						rv9_h_loop_filter(C+4*t->m_img_UVstride, t->m_img_UVstride, 0,
							clip_block_bot,
							clip_block_cur,
							alpha, beta, betaC, 1, 0);
					}

					if(c_v_deblock[CIdx] & cur_bit){
						clip_block_left = (uvcbp_left[CIdx] & right_bit) ? clip_left : 0;
						rv9_v_loop_filter(C, t->m_img_UVstride, 8,
							clip_block_cur,
							clip_block_left,
							alpha, beta, betaC, 1, 0);
					}

					if(c_sv_deblock[CIdx] & cur_bit){
						clip_block_left = (uvcbp_left[CIdx] & right_bit) ? clip_left : 0;
						rv9_v_loop_filter(C, t->m_img_UVstride, 8,
							clip_block_cur,
							clip_block_left,
							alpha, beta, betaC, 1, 1);
					}
				}

				//chrom block 3
				C += 4;
				cur_bit = MASK_CUR << 3;
				bottom_bit = MASK_CUR << (3 + 2);
				if((c_v_deblock_bak[CIdx] & cur_bit)||(c_h_deblock[CIdx] & bottom_bit)){
					clip_block_cur = (c_to_deblock[CIdx] & cur_bit) ? clip : 0;
					if(c_h_deblock[CIdx] & bottom_bit){
						clip_block_bot = c_to_deblock[CIdx] & bottom_bit ? clip_below : 0;
						rv9_h_loop_filter(C+4*t->m_img_UVstride, t->m_img_UVstride, 8,
							clip_block_bot,
							clip_block_cur,
							alpha, beta, betaC, 1, 0);
					}
					if(c_v_deblock[CIdx] & cur_bit){
						clip_block_left = c_to_deblock[CIdx] & (cur_bit >> 1)  ? clip : 0;
						rv9_v_loop_filter(C, t->m_img_UVstride, 8,
							clip_block_cur,
							clip_block_left,
							alpha, beta, betaC, 1, 0);
					}
				}
			}
		}
#endif //VOARMV7
	}
	return RV_S_OK;
}

