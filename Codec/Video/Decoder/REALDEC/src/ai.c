/***************************************************** BEGIN LICENSE BLOCK ******************************************* 
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
******************************************************* END LICENSE BLOCK ************************************************ 
****************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
****************************************************************************************
****************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1999 - 2000 Intel Corporation. 
*    All Rights Reserved. 
*    DESCRIPTION: 
*	  RV89Combo optimized advanced intra SSE functions 
****************************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************
* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/ai.c,v 1.1.1.1 2005/12/19 23:01:44 rishimathew Exp $ *
* This file contains the advanced intra decoding functions. *

********************************************************************************
Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		Optimized code for INTRA MacroBlocks
Reconstruction	
********************************************************************************/
#include "tables.h"
#include "ai.h"

#if defined(VOARMV4)
#elif defined(VOARMV6)
static void pred16x16_vertical_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_horizontal_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_left_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_top_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_128_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
//static void pred16x16_plane_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void ARMV6_pred16x16_plane(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);

static void pred8x8_vertical_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_horizontal_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_left_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_top_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_128_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
#elif defined(VOARMV7)
void pred16x16_vertical_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred16x16_horizontal_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred16x16_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred16x16_left_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred16x16_top_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred16x16_128_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred16x16_plane_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);

void pred8x8_vertical_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred8x8_horizontal_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred8x8_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred8x8_left_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred8x8_top_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
void pred8x8_128_dc_CtxA8(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
#else
static void pred16x16_vertical_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_horizontal_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_left_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_top_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_128_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred16x16_plane_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);

static void pred8x8_vertical_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_horizontal_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_left_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_top_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred8x8_128_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
#endif

static void pred4x4_vertical_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_horizontal_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_left_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_top_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_128_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_right_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_right_no_left_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_right_no_top_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_right_no_left_top_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_left_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_left_no_left_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_left_no_top_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_down_left_no_left_top_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_vertical_left_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_vertical_left_no_left_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_vertical_left_no_top_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_vertical_left_no_left_top_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_vertical_right_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_horizontal_up_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);
static void pred4x4_horizontal_down_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride);

const INTRA_PRED_PTR IntraPred16x16[7] =
{
#if defined(VOARMV4)
#elif defined(VOARMV6)
	//pred16x16_dc_c, pred16x16_vertical_c, pred16x16_horizontal_c, pred16x16_plane_c,
	//pred16x16_left_dc_c, pred16x16_top_dc_c, pred16x16_128_dc_c
	pred16x16_dc_c, pred16x16_vertical_c, pred16x16_horizontal_c, ARMV6_pred16x16_plane,
	pred16x16_left_dc_c, pred16x16_top_dc_c, pred16x16_128_dc_c
#elif defined(VOARMV7)
	pred16x16_dc_CtxA8, pred16x16_vertical_CtxA8, pred16x16_horizontal_CtxA8, pred16x16_plane_CtxA8,
	pred16x16_left_dc_CtxA8, pred16x16_top_dc_CtxA8, pred16x16_128_dc_CtxA8
#else
	pred16x16_dc_c, pred16x16_vertical_c, pred16x16_horizontal_c, pred16x16_plane_c,
	pred16x16_left_dc_c, pred16x16_top_dc_c, pred16x16_128_dc_c
#endif
};

const INTRA_PRED_PTR IntraPred8x8[7] =
{
#if defined(VOARMV4)
#elif defined(VOARMV6)
	pred8x8_dc_c, pred8x8_vertical_c, pred8x8_horizontal_c, pred8x8_dc_c,
	pred8x8_left_dc_c, pred8x8_top_dc_c, pred8x8_128_dc_c
#elif defined(VOARMV7)
	pred8x8_dc_CtxA8, pred8x8_vertical_CtxA8, pred8x8_horizontal_CtxA8, pred8x8_dc_CtxA8,
	pred8x8_left_dc_CtxA8, pred8x8_top_dc_CtxA8, pred8x8_128_dc_CtxA8
#else
	pred8x8_dc_c, pred8x8_vertical_c, pred8x8_horizontal_c, pred8x8_dc_c,
	pred8x8_left_dc_c, pred8x8_top_dc_c, pred8x8_128_dc_c
#endif
};

const INTRA_PRED_PTR IntraPred4x4[21] =
{
	pred4x4_dc_c, pred4x4_vertical_c, pred4x4_horizontal_c, pred4x4_horizontal_up_c,
	pred4x4_down_right_c, pred4x4_vertical_right_c, pred4x4_down_left_c,
	pred4x4_vertical_left_c, pred4x4_horizontal_down_c, pred4x4_left_dc_c,
	pred4x4_top_dc_c, pred4x4_128_dc_c,pred4x4_down_right_no_left_c,
	pred4x4_down_right_no_top_c,pred4x4_down_right_no_left_top_c,
	pred4x4_down_left_no_left_c,pred4x4_down_left_no_top_c,
	pred4x4_down_left_no_left_top_c,pred4x4_vertical_left_no_left_c,
	pred4x4_vertical_left_no_top_c,pred4x4_vertical_left_no_left_top_c
};

#if !(defined(VOARMV4) || defined(VOARMV7))
static void pred16x16_vertical_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride)
{
    I32 i;
    const U32 a= ((U32*)(src-src_stride))[0];
    const U32 b= ((U32*)(src-src_stride))[1];
    const U32 c= ((U32*)(src-src_stride))[2];
    const U32 d= ((U32*)(src-src_stride))[3];

    for(i=0; i<16; i++){
        ((U32*)dst)[0]= a;
        ((U32*)dst)[1]= b;
        ((U32*)dst)[2]= c;
        ((U32*)dst)[3]= d;
		dst += dst_stride;
    }
}

static void pred16x16_horizontal_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride){
    I32 i;

    for(i=0; i<16; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]=
        ((U32*)dst)[2]=
        ((U32*)dst)[3]= src[-1+i*src_stride]*0x01010101;
		dst += dst_stride;
    }
}

static void pred16x16_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride){
    I32 i, dc=0;

    for(i=0;i<16; i++){
        dc+= src[-1+i*src_stride];
    }

    for(i=0;i<16; i++){
        dc+= src[i-src_stride];
    }

    dc= 0x01010101*((dc + 16)>>5);

    for(i=0; i<16; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]=
        ((U32*)dst)[2]=
        ((U32*)dst)[3]= dc;
		dst += dst_stride;
    }
}

static void pred16x16_left_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride)
{
    I32 i, dc=0;

    for(i=0;i<16; i++){
        dc+= src[-1+i*src_stride];
    }

    dc= 0x01010101*((dc + 8)>>4);

    for(i=0; i<16; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]=
        ((U32*)dst)[2]=
        ((U32*)dst)[3]= dc;
		dst += dst_stride;
    }
}

static void pred16x16_top_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride){
    I32 i, dc=0;

    for(i=0;i<16; i++){
        dc+= src[i-src_stride];
    }
    dc= 0x01010101*((dc + 8)>>4);

    for(i=0; i<16; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]=
        ((U32*)dst)[2]=
        ((U32*)dst)[3]= dc;
		dst += dst_stride;
    }
}

static void pred16x16_128_dc_c(U8 *src, U8 *dst, const I32 src_stride, const I32 dst_stride){
    I32 i;

    for(i=0; i<16; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]=
        ((U32*)dst)[2]=
        ((U32*)dst)[3]= 0x01010101U*128U;
		dst += dst_stride;
    }
}




static void pred8x8_vertical_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
{
    I32 i;
    const U32 a= ((U32*)(src-src_stride))[0];
    const U32 b= ((U32*)(src-src_stride))[1];

    for(i=0; i<8; i++){
        ((U32*)dst)[0]= a;
        ((U32*)dst)[1]= b;
		dst += dst_stride;
    }
}

static void pred8x8_horizontal_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
{
    I32 i;

    for(i=0; i<8; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]= src[-1+i*src_stride]*0x01010101;
		dst += dst_stride;
    }
}

static void pred8x8_128_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
{
    I32 i;

    for(i=0; i<8; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]= 0x01010101U*128U;
		dst += dst_stride;
    }
}

static void pred8x8_left_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
{
    I32 i, dc=0;

    for(i=0;i<8; i++){
        dc+= src[-1+i*src_stride];
    }

    dc= 0x01010101*((dc + 4)>>3);

    for(i=0; i<8; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]= dc;
		dst += dst_stride;
    }
}


static void pred8x8_top_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride){
    I32 i, dc=0;

    for(i=0;i<8; i++){
        dc+= src[i-src_stride];
    }
    dc= 0x01010101*((dc + 4)>>3);

    for(i=0; i<8; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]= dc;
		dst += dst_stride;
    }
}


static void pred8x8_dc_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
{
    I32 i, dc=0;

    for(i=0;i<8; i++){
        dc+= src[-1+i*src_stride];
    }

    for(i=0;i<8; i++){
        dc+= src[i-src_stride];
    }

    dc= 0x01010101*((dc + 8)>>4);

    for(i=0; i<8; i++){
        ((U32*)dst)[0]=
        ((U32*)dst)[1]= dc;
		dst += dst_stride;
    }	
}

#endif

//TBD same as T_PlanarPredict
#if !(defined(VOARMV4) || defined(VOARMV6)|| defined(VOARMV7))
static void pred16x16_plane_c(U8 *src, U8* dst, const I32 src_stride, const I32 dst_stride)
{
	U8  *tp, *lp, *pp, t0;
	U32 *pTemp, lTemp;
	U32 topp[4]={0}, leftp[4]={0};
	I32 iH;
	I32 iV;
	I32 i, j;
	I32 a,b,c;
	I32 temp, lTemp1;

	/*MAP -- topp and leftp are the top and left pixels that would be used for*/
	/*prediction. topp and leftp are U32 arrays and each element of this array*/
	/*holds four pixel values. tp and lp are U8 pointers which point to topp  */
	/*and leftp respectively.												  */
	tp = (PU8)topp;
	lp = (PU8)leftp;

	t0 = (src-src_stride)[-1];

	pTemp = (PU32)(src-src_stride);
	topp[0] = pTemp[0];
	topp[1] = pTemp[1];
	topp[2] = pTemp[2];
	topp[3] = pTemp[3];

	pp = src;
	for (i=0;i < 4; i++) {
		lTemp  = (pp)[-1];
		lTemp |= (((pp +   src_stride)[-1]) << 8);
		lTemp |= (((pp + 2*src_stride)[-1]) << 16);
		lTemp |= (((pp + 3*src_stride)[-1]) << 24);
		leftp[i] = lTemp;
		pp += (src_stride << 2);
	}

	iH = 0;	
	iH +=	(tp[8 ] - tp[6]);
	iH +=	(tp[9 ] - tp[5]) << 1;
	iH += 3*(tp[10] - tp[4]);
	iH +=	(tp[11] - tp[3]) << 2;
	iH += 5*(tp[12] - tp[2]);
	iH += 6*(tp[13] - tp[1]);
	iH += 7*(tp[14] - tp[0]);
	iH +=	(tp[15] - t0) << 3;

	iV = 0;
	iV +=	(lp[8 ] - lp[6]);
	iV +=	(lp[9 ] - lp[5]) << 1;
	iV += 3*(lp[10] - lp[4]);
	iV +=	(lp[11] - lp[3]) << 2;
	iV += 5*(lp[12] - lp[2]);
	iV += 6*(lp[13] - lp[1]);
	iV += 7*(lp[14] - lp[0]);
	iV +=	(lp[15] - t0) << 3;

	a = (tp[15] + lp[15])<<4;
	b = (iH + (iH>>2))>>4;
	c = (iV + (iV>>2))>>4;


	pTemp  = (PU32)dst;
	lTemp1 = a - 7*(b+c) + 16;
	for (j=0; j<16; j++)
	{
		temp = lTemp1;
		for (i=0; i<16; i += 4)
		{
			lTemp  = ClampVal(temp >> 5);

			temp += b;			
			lTemp |= (ClampVal(temp >> 5) << 8);

			temp += b;				
			lTemp |= (ClampVal(temp >> 5) << 16);

			temp += b;
			lTemp |= (ClampVal(temp >> 5) << 24);

			temp += b;
			*pTemp++ = lTemp;			
		}
		lTemp1 += c;
	}
}
#endif
static void pred4x4_vertical_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
    const U32 dc= ((U32*)(src-stride))[0];
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
}

static void pred4x4_horizontal_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
	*((U32*)dst) = src[-1+0*stride]*0x01010101;
	dst += dst_stride;
	*((U32*)dst) = src[-1+1*stride]*0x01010101;
	dst += dst_stride;
	*((U32*)dst) = src[-1+2*stride]*0x01010101;
	dst += dst_stride;
	*((U32*)dst) = src[-1+3*stride]*0x01010101;
}

static void pred4x4_dc_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
    const U32 dc= ((  src[-stride] + src[1-stride] + src[2-stride] + src[3-stride]
                   + src[-1+0*stride] + src[-1+1*stride] + src[-1+2*stride] + src[-1+3*stride] + 4) >>3)*0x01010101;

    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
}

static void pred4x4_left_dc_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
    const U32 dc= ((src[-1+0*stride] + src[-1+1*stride] + src[-1+2*stride] + src[-1+3*stride] + 2) >>2)* 0x01010101;

    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
}

static void pred4x4_top_dc_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
    const U32 dc= ((src[-stride] + src[1-stride] + src[2-stride] + src[3-stride] + 2) >>2)* 0x01010101;

    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
}

static void pred4x4_128_dc_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
	const U32 dc = 128U*0x01010101U;

    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
	dst += dst_stride;
    *((U32*)dst) = dc;
}


#define LOAD_TOP_RIGHT_EDGE\
    const I32 t4= src[ 4-1*stride];\
    const I32 t5= src[ 5-1*stride];\
    const I32 t6= src[ 6-1*stride];\
    const I32 t7= src[ 7-1*stride];\

#define LOAD_DOWN_LEFT_EDGE\
    const I32 l4= src[-1+4*stride];\
    const I32 l5= src[-1+5*stride];\
    const I32 l6= src[-1+6*stride];\
    const I32 l7= src[-1+7*stride];\

#define LOAD_LEFT_EDGE\
    const I32 l0= src[-1+0*stride];\
    const I32 l1= src[-1+1*stride];\
    const I32 l2= src[-1+2*stride];\
    const I32 l3= src[-1+3*stride];\

#define LOAD_TOP_EDGE\
    const I32 t0= src[ 0-1*stride];\
    const I32 t1= src[ 1-1*stride];\
    const I32 t2= src[ 2-1*stride];\
    const I32 t3= src[ 3-1*stride];\

//MODE 3
static void pred4x4_horizontal_up_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
	const int lt= src[-1-1*stride];
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];

	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];

	dst[0+3*dst_stride] = (U8)((l3 + 2*l2 + l1 + 2) >> 2);
	dst[0+2*dst_stride] = 
	dst[1+3*dst_stride] = (U8)((l2 + 2*l1 + l0 + 2) >> 2);
	dst[0+1*dst_stride] = 
	dst[1+2*dst_stride] = 
	dst[2+3*dst_stride] = (U8)((l1 + 2*l0 + lt + 2) >> 2);
	dst[0+0*dst_stride] = 
	dst[1+1*dst_stride] = 
	dst[2+2*dst_stride] = 
	dst[3+3*dst_stride] = (U8)((l0 + 2*lt + t0 + 2) >> 2);
	dst[1+0*dst_stride] = 
	dst[2+1*dst_stride] = 
	dst[3+2*dst_stride] = (U8)((lt + 2*t0 + t1 + 2) >> 2);
	dst[2+0*dst_stride] = 
	dst[3+1*dst_stride] = (U8)((t0 + 2*t1 + t2 + 2) >> 2);
	dst[3+0*dst_stride] = (U8)((t1 + 2*t2 + t3 + 2) >> 2);

}


//MODE 4
static void pred4x4_down_right_c(U8 *src, U8 *dst, const I32 stride, const I32 dst_stride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_TOP_RIGHT_EDGE
        const I32 t4= src[ 4-1*stride];
        const I32 t5= src[ 5-1*stride];
        const I32 t6= src[ 6-1*stride];
        const I32 t7= src[ 7-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];
	//LOAD_DOWN_LEFT_EDGE
        const I32 l4= src[-1+4*stride];
        const I32 l5= src[-1+5*stride];
        const I32 l6= src[-1+6*stride];
        const I32 l7= src[-1+7*stride];

    dst[0+0*dst_stride]=(U8)((t0 + 2*t1 + t2 + l0 + 2*l1 + l2 + 4)>>3);
    dst[1+0*dst_stride]=
    dst[0+1*dst_stride]=(U8)((t1 + 2*t2 + t3 + l1 + 2*l2 + l3 + 4)>>3);
    dst[0+2*dst_stride]=
    dst[1+1*dst_stride]=
    dst[2+0*dst_stride]=(U8)((t2 + 2*t3 + t4 + l2 + 2*l3 + l4 + 4)>>3);
    dst[3+0*dst_stride]=
    dst[2+1*dst_stride]=
    dst[1+2*dst_stride]=
	dst[0+3*dst_stride]=(U8)((t3 + 2*t4 + t5 + l3 + 2*l4 + l5 + 4)>>3);
    dst[3+1*dst_stride]=
    dst[2+2*dst_stride]=
    dst[1+3*dst_stride]=(U8)((t4 + 2*t5 + t6 + l4 + 2*l5 + l6 + 4)>>3);
    dst[3+2*dst_stride]=
    dst[2+3*dst_stride]=(U8)((t5 + 2*t6 + t7 + l5 + 2*l6 + l7 + 4)>>3);
    dst[3+3*dst_stride]=(U8)((t6 + t7 + l6 + l7 + 2)>>2);

}

static void pred4x4_down_right_no_left_c(U8 *src,U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_TOP_RIGHT_EDGE
        const I32 t4= src[ 4-1*stride];
        const I32 t5= src[ 5-1*stride];
        const I32 t6= src[ 6-1*stride];
        const I32 t7= src[ 7-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];

	dst[0+0*dststride]=(U8)((t0 + 2*t1 + t2 + l0 + 2*l1 + l2 + 4)>>3);
	dst[1+0*dststride]=
	dst[0+1*dststride]=(U8)((t1 + 2*t2 + t3 + l1 + 2*l2 + l3 + 4)>>3);
	dst[0+2*dststride]=
	dst[1+1*dststride]=
	dst[2+0*dststride]=(U8)((t2 + 2*t3 + t4 + l2 + 3*l3 + 4)>>3);
	dst[3+0*dststride]=
	dst[2+1*dststride]=
	dst[1+2*dststride]=
	dst[0+3*dststride]=(U8)((t3 + 2*t4 + t5 + 4*l3 + 4)>>3);
	dst[3+1*dststride]=
	dst[2+2*dststride]=
	dst[1+3*dststride]=(U8)((t4 + 2*t5 + t6 + 4*l3 + 4)>>3);
	dst[3+2*dststride]=
	dst[2+3*dststride]=(U8)((t5 + 2*t6 + t7 + 4*l3 + 4)>>3);
	dst[3+3*dststride]=(U8)((t6 + t7 + 2*l3 + 2)>>2);

}

static void pred4x4_down_right_no_top_c(U8 *src,U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];
	//LOAD_DOWN_LEFT_EDGE
        const I32 l4= src[-1+4*stride];
        const I32 l5= src[-1+5*stride];
        const I32 l6= src[-1+6*stride];
        const I32 l7= src[-1+7*stride];

	dst[0+0*dststride]=(U8)((t0 + 2*t1 + t2 + l0 + 2*l1 + l2 + 4)>>3);
    dst[1+0*dststride]=
	dst[0+1*dststride]=(U8)((t1 + 2*t2 + t3 + l1 + 2*l2 + l3 + 4)>>3);
	dst[0+2*dststride]=
	dst[1+1*dststride]=
	dst[2+0*dststride]=(U8)((t2 + 2*t3 + t3 + l2 + 2*l3 + l4 + 4)>>3);
	dst[3+0*dststride]=
	dst[2+1*dststride]=
	dst[1+2*dststride]=
	dst[0+3*dststride]=(U8)((4*t3 + l3 + 2*l4 + l5 + 4)>>3);
	dst[3+1*dststride]=
	dst[2+2*dststride]=
	dst[1+3*dststride]=(U8)((4*t3 + l4 + 2*l5 + l6 + 4)>>3);
	dst[3+2*dststride]=
	dst[2+3*dststride]=(U8)((4*t3 + l5 + 2*l6 + l7 + 4)>>3);
	dst[3+3*dststride]=(U8)((2*t3 + l6 + l7 + 2)>>2);

}

static void pred4x4_down_right_no_left_top_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];

	dst[0+0*dststride]=(U8)((t0 + 2*t1 + t2 + l0 + 2*l1 + l2 + 4)>>3);
	dst[1+0*dststride]=
	dst[0+1*dststride]=(U8)((t1 + 2*t2 + t3 + l1 + 2*l2 + l3 + 4)>>3);
	dst[0+2*dststride]=
    dst[1+1*dststride]=
	dst[2+0*dststride]=(U8)((t2 + 3*t3 + l2 + 3*l3 + 4)>>3);
	dst[3+0*dststride]=
    dst[2+1*dststride]=
	dst[1+2*dststride]=
	dst[0+3*dststride]=
	dst[3+1*dststride]=
	dst[2+2*dststride]=
    dst[1+3*dststride]=
	dst[3+2*dststride]=
	dst[2+3*dststride]=(U8)((4*t3 + 4*l3 + 4)>>3);
	dst[3+3*dststride]=(U8)((2*t3 + 2*l3 + 2)>>2);

}

//MODE 5
static void pred4x4_vertical_right_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
    const int lt= src[-1-1*stride];
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
        //LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
       // const I32 l3= src[-1+3*stride];

    dst[0+0*dststride]=
    dst[1+2*dststride]=(U8)((lt + t0 + 1)>>1);
    dst[1+0*dststride]=
    dst[2+2*dststride]=(U8)((t0 + t1 + 1)>>1);
    dst[2+0*dststride]=
    dst[3+2*dststride]=(U8)((t1 + t2 + 1)>>1);
    dst[3+0*dststride]=(U8)((t2 + t3 + 1)>>1);
    dst[0+1*dststride]=
    dst[1+3*dststride]=(U8)((l0 + 2*lt + t0 + 2)>>2);
    dst[1+1*dststride]=
    dst[2+3*dststride]=(U8)((lt + 2*t0 + t1 + 2)>>2);
    dst[2+1*dststride]=
    dst[3+3*dststride]=(U8)((t0 + 2*t1 + t2 + 2)>>2);
    dst[3+1*dststride]=(U8)((t1 + 2*t2 + t3 + 2)>>2);
    dst[0+2*dststride]=(U8)((lt + 2*l0 + l1 + 2)>>2);
    dst[0+3*dststride]=(U8)((l0 + 2*l1 + l2 + 2)>>2);
}

//MODE 6
static void pred4x4_down_left_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        //const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];
	//LOAD_TOP_RIGHT_EDGE
        const I32 t4= src[ 4-1*stride];
        const I32 t5= src[ 5-1*stride];
        const I32 t6= src[ 6-1*stride];
        //const I32 t7= src[ 7-1*stride];
	const I32 l4= src[-1+4*stride];

	dst[0 + 0*dststride] = (U8)((2*t0 + 2*t1 + l1 + 2*l2 +l3 + 4)>>3);
	dst[1 + 0*dststride] =
	dst[0 + 2*dststride] = (U8)((t1 + t2 + 1)>>1);
	dst[2 + 0*dststride] = 
	dst[1 + 2*dststride] = (U8)((t2 + t3 + 1)>>1);
	dst[3 + 0*dststride] = 
	dst[2 + 2*dststride] = (U8)((t3 + t4 + 1)>>1);
	dst[3 + 2*dststride] = (U8)((t4 + t5 + 1) >> 1);
	dst[0 + 1*dststride] = (U8)((t0 + 2*t1 + t2 + l2 + 2*l3 + l4 + 4)>>3);
	dst[1 + 1*dststride] = 
	dst[0 + 3*dststride] = (U8)((t1 + 2*t2 + t3 + 2)>>2);
	dst[2 + 1*dststride] = 
	dst[1 + 3*dststride] = (U8)((t2 + 2*t3 + t4 + 2)>>2);
	dst[3 + 1*dststride] = 
	dst[2 + 3*dststride] = (U8)((t3 + 2*t4 + t5 + 2)>>2);
	dst[3 + 3*dststride] = (U8)((t4 + 2*t5 + t6 + 2)>>2);
}

static void pred4x4_down_left_no_left_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        //const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];
	//LOAD_TOP_RIGHT_EDGE
        const I32 t4= src[ 4-1*stride];
        const I32 t5= src[ 5-1*stride];
        const I32 t6= src[ 6-1*stride];
        //const I32 t7= src[ 7-1*stride];

	dst[0 + 0*dststride] = (U8)((2*t0 + 2*t1 + l1 + 2*l2 +l3 + 4)>>3);
	dst[1 + 0*dststride] =
	dst[0 + 2*dststride] = (U8)((t1 + t2 + 1)>>1);
	dst[2 + 0*dststride] = 
	dst[1 + 2*dststride] = (U8)((t2 + t3 + 1)>>1);
	dst[3 + 0*dststride] = 
	dst[2 + 2*dststride] = (U8)((t3 + t4 + 1)>>1);
	dst[3 + 2*dststride] = (U8)((t4 + t5 + 1) >> 1);
	dst[0 + 1*dststride] = (U8)((t0 + 2*t1 + t2 + l2 + 3*l3 + 4)>>3);
	dst[1 + 1*dststride] = 
	dst[0 + 3*dststride] = (U8)((t1 + 2*t2 + t3 + 2)>>2);
	dst[2 + 1*dststride] = 
	dst[1 + 3*dststride] = (U8)((t2 + 2*t3 + t4 + 2)>>2);
	dst[3 + 1*dststride] = 
	dst[2 + 3*dststride] = (U8)((t3 + 2*t4 + t5 + 2)>>2);
	dst[3 + 3*dststride] = (U8)((t4 + 2*t5 + t6 + 2)>>2);
}

static void pred4x4_down_left_no_top_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        //const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];
	const I32 l4= src[-1+4*stride];

	dst[0 + 0*dststride] = (U8)((2*t0 + 2*t1 + l1 + 2*l2 +l3 + 4)>>3);
	dst[1 + 0*dststride] =
	dst[0 + 2*dststride] = (U8)((t1 + t2 + 1)>>1);
	dst[2 + 0*dststride] = 
	dst[1 + 2*dststride] = (U8)((t2 + t3 + 1)>>1);
	dst[3 + 0*dststride] = 
	dst[2 + 2*dststride] = 
	dst[3 + 2*dststride] = (U8)((2*t3 + 1)>>1);
	dst[0 + 1*dststride] = (U8)((t0 + 2*t1 + t2 + l2 + 2*l3 + l4 + 4)>>3);
	dst[1 + 1*dststride] = 
	dst[0 + 3*dststride] = (U8)((t1 + 2*t2 + t3 + 2)>>2);
	dst[2 + 1*dststride] = 
	dst[1 + 3*dststride] = (U8)((t2 + 3*t3 + 2)>>2);
	dst[3 + 1*dststride] = 
	dst[2 + 3*dststride] = 
	dst[3 + 3*dststride] = (U8)((4*t3 + 2)>>2);
}

static void pred4x4_down_left_no_left_top_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        //const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];

	dst[0 + 0*dststride] = (U8)((2*t0 + 2*t1 + l1 + 2*l2 +l3 + 4)>>3);
	dst[1 + 0*dststride] =
	dst[0 + 2*dststride] = (U8)((t1 + t2 + 1)>>1);
	dst[2 + 0*dststride] = 
	dst[1 + 2*dststride] = (U8)((t2 + t3 + 1)>>1);
	dst[3 + 0*dststride] = 
	dst[2 + 2*dststride] = 
	dst[3 + 2*dststride] = (U8)((2*t3 + 1)>>1);
	dst[0 + 1*dststride] = (U8)((t0 + 2*t1 + t2 + l2 + 3*l3 + 4)>>3);
	dst[1 + 1*dststride] = 
	dst[0 + 3*dststride] = (U8)((t1 + 2*t2 + t3 + 2)>>2);
	dst[2 + 1*dststride] = 
	dst[1 + 3*dststride] = (U8)((t2 + 3*t3 + 2)>>2);
	dst[3 + 1*dststride] = 
	dst[2 + 3*dststride] = 
	dst[3 + 3*dststride] = (U8)((4*t3 + 2)>>2);
}

//MODE 7
static void pred4x4_vertical_left_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        //const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_TOP_RIGHT_EDGE
        const I32 t4= src[ 4-1*stride];
        const I32 t5= src[ 5-1*stride];
        const I32 t6= src[ 6-1*stride];
        const I32 t7= src[ 7-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];
	//LOAD_DOWN_LEFT_EDGE
        const I32 l4= src[-1+4*stride];
        const I32 l5= src[-1+5*stride];
        const I32 l6= src[-1+6*stride];
        //const I32 l7= src[-1+7*stride];

    dst[0+0*dststride]=(U8)((t1 + 2*t2 + t3 + 2*l0 + 2*l1 + 4)>>3);

    dst[1 + 0*dststride]=(U8)((t2 + 2*t3 + t4 + l0 + 2*l1 + l2 + 4)>>3);

    dst[2+0*dststride]=
    dst[0+1*dststride]=(U8)((t3 + 2*t4 + t5 + 2*l1 + 2*l2 + 4)>>3);

    dst[3+0*dststride]=
    dst[1+1*dststride]=(U8)((t4 + 2*t5 + t6 + l1 + 2*l2 + l3 + 4)>>3);

    dst[2+1*dststride]=
    dst[0+2*dststride]=(U8)((t5 + 2*t6 + t7 + 2*l2 + 2*l3 + 4)>>3);

    dst[3+1*dststride]=
	dst[1 + 2*dststride]=(U8)((t6 + 3*t7 + l2 + 3*l3 + 4)>>3) ;

    dst[3+2*dststride]=
    dst[1+3*dststride]=(U8)((l3 + 2*l4 + l5 + 2)>>2 );

    dst[0+3*dststride]=
	dst[2+2*dststride]=(U8)((t6 + t7 + l3 + l4 + 2)>>2);

    dst[2+3*dststride]=(U8)((l4 + l5 + 1)>>1);
	dst[3+3*dststride]=(U8)((l4 + 2*l5 + l6 + 2)>>2);
}

static void pred4x4_vertical_left_no_left_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        //const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_TOP_RIGHT_EDGE
        const I32 t4= src[ 4-1*stride];
        const I32 t5= src[ 5-1*stride];
        const I32 t6= src[ 6-1*stride];
        const I32 t7= src[ 7-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];

	dst[0 + 0*dststride]=(U8)((t1 + 2*t2 + t3 + 2*l0 + 2*l1 + 4)>>3);

	dst[1 + 0*dststride]=(U8)((t2 + 2*t3 + t4 + l0 + 2*l1 + l2 + 4)>>3);

	dst[2 + 0*dststride]=
	dst[0 + 1*dststride]=(U8)((t3 + 2*t4 + t5 + 2*l1 + 2*l2 + 4)>>3);

	dst[3 + 0*dststride]=
	dst[1 + 1*dststride]=(U8)((t4 + 2*t5 + t6 + l1 + 2*l2 + l3 + 4)>>3);

	dst[2 + 1*dststride]=
	dst[0 + 2*dststride]=(U8)((t5 + 2*t6 + t7 + 2*l2 + 2*l3 + 4)>>3);

	dst[3 + 1*dststride]=
	dst[1 + 2*dststride]=(U8)((t6 + 3*t7 + l2 + 3*l3 + 4)>>3) ;

	dst[3 + 2*dststride]=
	dst[1 + 3*dststride]=(U8)((4*l3 + 2)>>2) ;

	dst[0 + 3*dststride]=
	dst[2 + 2*dststride]=(U8)((t6 + t7 + 2*l3 + 2)>>2);

	dst[2 + 3*dststride]=(U8)((2*l3 + 1)>>1);
	dst[3 + 3*dststride]=(U8)((4*l3 + 2)>>2);
}

static void pred4x4_vertical_left_no_top_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        //const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];
	//LOAD_DOWN_LEFT_EDGE
        const I32 l4= src[-1+4*stride];
        const I32 l5= src[-1+5*stride];
        const I32 l6= src[-1+6*stride];
        //const I32 l7= src[-1+7*stride];

	dst[0 + 0*dststride]=(U8)((t1 + 2*t2 + t3 + 2*l0 + 2*l1 + 4)>>3);

	dst[1 + 0*dststride]=(U8)((t2 + 3*t3 + l0 + 2*l1 + l2 + 4)>>3);

	dst[2 + 0*dststride]=
	dst[0 + 1*dststride]=(U8)((4*t3 + 2*l1 + 2*l2 + 4)>>3);

	dst[3 + 0*dststride]=
	dst[1 + 1*dststride]=(U8)((4*t3 + l1 + 2*l2 + l3 + 4)>>3);

	dst[2 + 1*dststride]=
	dst[0 + 2*dststride]=(U8)((4*t3 + 2*l2 + 2*l3 + 4)>>3);

	dst[3 + 1*dststride]=
	dst[1 + 2*dststride]=(U8)((4*t3 + l2 + 3*l3 + 4)>>3) ;

	dst[3 + 2*dststride]=
	dst[1 + 3*dststride]=(U8)((l3 + 2*l4 + l5 + 2)>>2) ;

	dst[0 + 3*dststride]=
	dst[2 + 2*dststride]=(U8)((2*t3 + l3 + l4 + 2)>>2);

	dst[2 + 3*dststride]=(U8)((l4 + l5 + 1)>>1);
	dst[3 + 3*dststride]=(U8)((l4 + 2*l5 + l6 + 2)>>2);
}

static void pred4x4_vertical_left_no_left_top_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
	//LOAD_TOP_EDGE
        //const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];

	dst[0 + 0*dststride]=(U8)((t1 + 2*t2 + t3 + 2*l0 + 2*l1 + 4)>>3);

	dst[1 + 0*dststride]=(U8)((t2 + 3*t3 + l0 + 2*l1 + l2 + 4)>>3);

	dst[2 + 0*dststride]=
	dst[0 + 1*dststride]=(U8)((4*t3 + 2*l1 + 2*l2 + 4)>>3);

	dst[3 + 0*dststride]=
	dst[1 + 1*dststride]=(U8)((4*t3 + l1 + 2*l2 + l3 + 4)>>3);

	dst[2 + 1*dststride]=
	dst[0 + 2*dststride]=(U8)((4*t3 + 2*l2 + 2*l3 + 4)>>3);

	dst[3 + 1*dststride]=
	dst[1 + 2*dststride]=(U8)((4*t3 + l2 + 3*l3 + 4)>>3) ;

	dst[3 + 2*dststride]=
	dst[1 + 3*dststride]=(U8)((4*l3 + 2)>>2) ;

	dst[0 + 3*dststride]=
	dst[2 + 2*dststride]=(U8)((2*t3 + 2*l3 + 2)>>2);

	dst[2 + 3*dststride]=(U8)((2*l3 + 1)>>1);
	dst[3 + 3*dststride]=(U8)((4*l3 + 2)>>2);
}
//MODE 8
static void pred4x4_horizontal_down_c(U8 *src, U8 *dst, const I32 stride, const I32 dststride)
{
    const I32 lt= src[-1-1*stride];
	//LOAD_TOP_EDGE
        const I32 t0= src[ 0-1*stride];
        const I32 t1= src[ 1-1*stride];
        const I32 t2= src[ 2-1*stride];
        //const I32 t3= src[ 3-1*stride];
	//LOAD_LEFT_EDGE
        const I32 l0= src[-1+0*stride];
        const I32 l1= src[-1+1*stride];
        const I32 l2= src[-1+2*stride];
        const I32 l3= src[-1+3*stride];

    dst[0+0*dststride]=
    dst[2+1*dststride]=(U8)((lt + l0 + 1)>>1);
    dst[1+0*dststride]=
    dst[3+1*dststride]=(U8)((l0 + 2*lt + t0 + 2)>>2);
    dst[2+0*dststride]=(U8)((lt + 2*t0 + t1 + 2)>>2);
    dst[3+0*dststride]=(U8)((t0 + 2*t1 + t2 + 2)>>2);
    dst[0+1*dststride]=
    dst[2+2*dststride]=(U8)((l0 + l1 + 1)>>1);
    dst[1+1*dststride]=
    dst[3+2*dststride]=(U8)((lt + 2*l0 + l1 + 2)>>2);
    dst[0+2*dststride]=
    dst[2+3*dststride]=(U8)((l1 + l2+ 1)>>1);
    dst[1+2*dststride]=
    dst[3+3*dststride]=(U8)((l0 + 2*l1 + l2 + 2)>>2);
    dst[0+3*dststride]=(U8)((l2 + l3 + 1)>>1);
    dst[1+3*dststride]=(U8)((l1 + 2*l2 + l3 + 2)>>2);
}



