/* ************************************************** BEGIN LICENSE BLOCK **************************************** 
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
***************************************************** END LICENSE BLOCK **************************************************/ 

/****************************************************************************/
/*    RealNetworks, Inc. Confidential and Proprietary Information.          */
/*    Copyright (c) 1995-2002 RealNetworks, Inc.                            */
/*    All Rights Reserved.                                                  */
/*    Do not redistribute.                                                  */
/****************************************************************************/
/************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
//nav
#include <string.h>

#include "basic.h"
#include "decoder.h"

VO_U32 image_create(struct Decoder* t,IMAGE *image,const U32 edged_width,const U32 edged_height, const I32 uv_interlace_flage)	
{
	U32 newSize;

	const U32 edged_width2  = edged_width / 2;
	const U32 edged_height2 = edged_height / 2;

	newSize = edged_height*edged_width*3/2 + edged_width*16*2;
	image->m_pYPlane = MallocMem(newSize,CACHE_LINE,t->m_pMemOP);

	if (NULL == image->m_pYPlane){
		return VO_ERR_OUTOF_MEMORY;
	}

	image->m_pUPlane = image->m_pYPlane + edged_width*edged_height + edged_width*16;

	if(uv_interlace_flage){
		image->m_pVPlane = image->m_pUPlane + (edged_width>>1);
	}else{
		image->m_pVPlane = image->m_pUPlane + edged_width2 * edged_height2;
	}

	image->m_pYPlane += (YUV_Y_PADDING * edged_width * 2  + YUV_Y_PADDING);

	if(uv_interlace_flage){
		image->m_pUPlane += YUV_UV_PADDING * edged_width + YUV_UV_PADDING;
		image->m_pVPlane += YUV_UV_PADDING * edged_width + YUV_UV_PADDING;
	}else{
		image->m_pUPlane += YUV_UV_PADDING * edged_width2 + YUV_UV_PADDING;
		image->m_pVPlane += YUV_UV_PADDING * edged_width2 + YUV_UV_PADDING;
	}

	return VO_ERR_NONE;

}

void image_destroy(struct Decoder* t,IMAGE *image, const U32 edged_width)
{
	if(image->m_pYPlane){
		FreeMem(image->m_pYPlane - YUV_Y_PADDING * edged_width * 2 - YUV_Y_PADDING,t->m_pMemOP);
	}

	image->m_pYPlane = NULL;
	image->m_pUPlane = NULL;
	image->m_pVPlane = NULL;
}

/*--------------------------------------------------------------------------; */
/*  The algorithm fills in (1) the bottom (not including corners), */
/*  then (2) the sides (including the bottom corners, but not the */
/*  top corners), then (3) the top (including the top */
/*  corners) as shown below, replicating the outermost bytes */
/*  of the original frame outward: */
/* 											*/			
/*               ---------------------------- */
/*              |                            | */
/*              |            (3)             | */
/*              |                            | */
/*              |----------------------------| */
/*              |     |                |     | */
/*              |     |                |     | */
/*              |     |                |     | */
/*              |     |    original    |     | */
/*              |     |     frame      |     | */
/*              |     |                |     | */
/*              | (2) |                | (2) | */
/*              |     |                |     | */
/*              |     |                |     | */
/*              |     |----------------|     | */
/*              |     |                |     | */
/*              |     |      (1)       |     | */
/*              |     |                |     | */
/*               ---------------------------- */
/*--------------------------------------------------------------------------; */
void image_setedges(struct Decoder* t,IMAGE *image,const U32 ExWidth,const U32 ExHeight,U32 width, U32 height, const I32 uv_interlace_flage)
{
	const U32 edged_width2 = ExWidth >>1;
	U32 width2, height2;
	U32 ExWidth2 = 0;//, ExHeight2 = 0;

#if defined(VOARMV7)
	width  = (width+15)&~15;
	height = (height+15)&~15;

	if(uv_interlace_flage){
		ExWidth2 = ExWidth;
	}else{
		ExWidth2 = ExWidth >> 1;
	}
	width2    = width>>1;
	height2   = height>>1;
	//ExHeight2 = ExHeight>>1;

	Fill_Edge_Y_Ctx(image->m_pYPlane,ExWidth, ExHeight, width, height);
	Fill_Edge_UV_Ctx(image->m_pUPlane,ExWidth2, ExHeight>>1,width2, height2,edged_width2);
	Fill_Edge_UV_Ctx(image->m_pVPlane,ExWidth2, ExHeight>>1,width2, height2,edged_width2);
#else

	U32 i;
	U8  *dst;
	U8  *src;
	width  = (width+15)&~15;
	height = (height+15)&~15;

	if(uv_interlace_flage){
		ExWidth2 = ExWidth;
	}else{
		ExWidth2 = ExWidth >> 1;
	}
	//Y
	dst = image->m_pYPlane + height * ExWidth;
	src = image->m_pYPlane + (height - 1) * ExWidth;

	width2 = width>>1;
	height2 = height>>1;
	for (i = 0; i < YUV_Y_PADDING; i++) {
		CopyMem(dst, src, width, t->m_pMemOP);
		dst += ExWidth;
	}

	src  = image->m_pYPlane;
	dst  = image->m_pYPlane - YUV_Y_PADDING;
	for (i = 0; i < (height + YUV_Y_PADDING); i++) {
		SetMem(dst, *src, YUV_Y_PADDING, t->m_pMemOP);
		SetMem(dst + width + YUV_Y_PADDING, src[width - 1], YUV_Y_PADDING, t->m_pMemOP);
		dst += ExWidth;
		src += ExWidth;
	}

	src = image->m_pYPlane -  YUV_Y_PADDING;
	dst = src - ExWidth;
	for (i = 0; i < YUV_Y_PADDING; i++) {
		CopyMem(dst, src, ExWidth, t->m_pMemOP);
		dst -= ExWidth;
	}

	/* U */
	dst = image->m_pUPlane + height2 * ExWidth2;
	src = image->m_pUPlane + (height2 - 1) * ExWidth2;
	for (i = 0; i < YUV_UV_PADDING; i++) {
		CopyMem(dst, src, width2, t->m_pMemOP);
		dst += ExWidth2;
	}   

	src = image->m_pUPlane;
	dst = image->m_pUPlane - YUV_UV_PADDING;
	for (i = 0; i < (height2 + YUV_UV_PADDING); i++) {
		SetMem(dst, *src, YUV_UV_PADDING, t->m_pMemOP);
		SetMem(dst + width2 + YUV_UV_PADDING, src[width2 - 1], YUV_UV_PADDING,t->m_pMemOP);
		dst += ExWidth2;
		src += ExWidth2;
	}

	src = image->m_pUPlane - YUV_UV_PADDING;
	dst = src - ExWidth2;
	for (i = 0; i < YUV_UV_PADDING; i++) {
		CopyMem(dst, src, edged_width2, t->m_pMemOP);
		dst -= ExWidth2;
	}

	/* V */
	dst = image->m_pVPlane + height2 * ExWidth2;
	src = image->m_pVPlane + (height2 - 1) * ExWidth2;
	for (i = 0; i < YUV_UV_PADDING; i++) {
		CopyMem(dst, src, width2, t->m_pMemOP);
		dst += ExWidth2;
	}

	src = image->m_pVPlane;
	dst = image->m_pVPlane - YUV_UV_PADDING;
	for (i = 0; i < (height2 + YUV_UV_PADDING); i++) {
		SetMem(dst, *src, YUV_UV_PADDING, t->m_pMemOP);
		SetMem(dst + width2 + YUV_UV_PADDING, src[width2 - 1], YUV_UV_PADDING,t->m_pMemOP);
		dst += ExWidth2;
		src += ExWidth2;
	}

	src = image->m_pVPlane - YUV_UV_PADDING;
	dst = src - ExWidth2;
	for(i = 0; i < YUV_UV_PADDING; i++) {
		CopyMem(dst, src, edged_width2, t->m_pMemOP);
		dst -= ExWidth2;
	}
#endif

}

void image_swap(IMAGE * image1,IMAGE * image2)
{
	IMAGE tmp;

	tmp = *image1;
	*image1 = *image2;
	*image2 = tmp;
}

/***********************************************************************************/
/*                                                                                 */
/*  RPRUpsamplePlane                                                               */
/*                                                                                 */
/*	Using the filter defined in H263+ spec annexes O, P, and Q this function       */
/*	upsamples by a factor of 2 both horizontally and vertically a single           */
/*	plane.                                                                         */
/*                                                                                 */
/*	The plane is upsampled from bottom to top and right to left to allow           */
/*	resampling in place.                                                           */
/*                                                                                 */
/*	Notes:                                                                         */
/*	1. The final rounding add (before division by shifting right) value is         */
/*		determined by the input boolean argument bRounding, which when FALSE       */
/*		indicates that the usual rounding value be decremented by 1. This          */
/*		is specific to annex P (differing from annexes O and Q).                   */
/*                                                                                 */
/***********************************************************************************/
static void RPRUpsamplePlane(struct Decoder *t, U8 *pSrcPlane,U8 *pDstPlane, RV_Boolean bRounding, U32 uPlane)
{
	PU8 pSrc;			/* upper left corner of source plane */
	PU8 pDst;			/* upper left corner of destination plane */
	U32 uSrcPitch;
	U32 uDstPitch;
	U32 uSrcWidth;
	U32 uSrcHeight;
	I32 i;
	U32 j;
	PU8 pSrcLine2;
	PU8 pDstLine2;
	U32 uDstWidth;
	U32 uDstHeight;
	U8  uRound;

	pSrc = pSrcPlane;
	pDst = pDstPlane;

	/* if not luma plane divide dimensions by 2 for chroma planes */
	if (uPlane){
		uSrcPitch = t->m_img_UVstride;
		uDstPitch = t->m_img_UVstride;
		uSrcWidth = t->m_pre_out_img_width >> 1;
		uDstWidth = t->m_out_img_width >> 1;
		uSrcHeight = t->m_pre_out_img_height >> 1;
		uDstHeight = t->m_out_img_height >> 1;
	}else{
	uSrcPitch = t->m_img_stride;
	uDstPitch = t->m_img_stride;
	uSrcWidth = t->m_pre_out_img_width;
	uDstWidth = t->m_out_img_width;
	uSrcHeight = t->m_pre_out_img_height;
	uDstHeight = t->m_out_img_height;
	}

	RVAssert(uDstWidth == uSrcWidth<<1);
	RVAssert(uDstHeight == uSrcHeight<<1);

	uRound = bRounding ? 1 : 0;

	/* set pointers to last line */
	pSrc += uSrcPitch*(uSrcHeight-1);
	pDst += uDstPitch*(uDstHeight-1);

	pSrcLine2 = pSrc-uSrcPitch;
	pDstLine2 = pDst-uDstPitch;

	/* last pel on last line */
	*(pDst+uDstWidth-1) = *(pSrc+uSrcWidth-1);

	/* last line */
	for (i = (I32)uSrcWidth-2; i >= 0; i--) {
		*(pDst+(i<<1)+2) = (*(pSrc+i)   + *(pSrc+i+1)*3 + 1 + uRound)>>2;
		*(pDst+(i<<1)+1) = (*(pSrc+i)*3 + *(pSrc+i+1)   + 1 + uRound)>>2;
	}

	/* first pel on last line */
	*(pDst) = *(pSrc);

	pDst = pDstLine2;
	pDstLine2 -= uDstPitch;

	/* all lines except first and last */
	for (j = 1; j < uSrcHeight; j++) {

		/* last pel on each line */
		*(pDst +uDstWidth-1) = (*(pSrc+uSrcWidth-1)*3 + *(pSrcLine2+uSrcWidth-1)
			+ 1 + uRound)>>2;
		*(pDstLine2+uDstWidth-1) = (*(pSrc+uSrcWidth-1) +
			*(pSrcLine2+uSrcWidth-1)*3 + 1 + uRound)>>2;

		for (i = (I32)uSrcWidth-2; i >= 0; i--) {
			*(pDst +(i<<1)+1) = (*(pSrc+i)*9 + *(pSrc+i+1)*3 + *(pSrcLine2+i)*3
				+ *(pSrcLine2+i+1)   + 7 + uRound)>>4;
			*(pDst +(i<<1)+2) = (*(pSrc+i)*3 + *(pSrc+i+1)*9 + *(pSrcLine2+i)
				+ *(pSrcLine2+i+1)*3 + 7 + uRound)>>4;
			*(pDstLine2+(i<<1)+1) = (*(pSrc+i)*3 + *(pSrc+i+1) + *(pSrcLine2+i)*9
				+ *(pSrcLine2+i+1)*3 + 7 + uRound)>>4;
			*(pDstLine2+(i<<1)+2) = (*(pSrc+i) + *(pSrc+i+1)*3 + *(pSrcLine2+i)*3
				+ *(pSrcLine2+i+1)*9 + 7 + uRound)>>4;
		}

		/* first pel on each line */
		*(pDst)  = (*(pSrc)*3 + *(pSrcLine2)   + 1 + uRound)>>2;
		*(pDstLine2) = (*(pSrc)   + *(pSrcLine2)*3 + 1 + uRound)>>2;

		pDst -= (uDstPitch<<1);
		pDstLine2 -= (uDstPitch<<1);
		pSrc -= uSrcPitch;
		pSrcLine2 -= uSrcPitch;
	}

	/* last pel on first line */
	*(pDst+uDstWidth-1) = *(pSrc+uSrcWidth-1);

	/* first line */
	for (i = (I32)uSrcWidth-2; i >= 0; i--) {
		*(pDst+(i<<1)+2) = (*(pSrc+i)   + *(pSrc+i+1)*3 + 1 + uRound)>>2;
		*(pDst+(i<<1)+1) = (*(pSrc+i)*3 + *(pSrc+i+1)   + 1 + uRound)>>2;
	}

	/* first pel on first line */
	*(pDst) = *(pSrc);

}	/* end RPRUpsamplePlane */

/************************************************************************************/
/*                                                                                  */
/*  RPRDownsamplePlane                                                              */
/*                                                                                  */
/*	Using the filter defined in H263+ spec annexes O, P, and Q this function        */
/*	downsamples by a factor of 2 both horizontally and vertically a single          */
/*	plane.                                                                          */
/*                                                                                  */
/*	The filter is defined as follows (A,B,C,D inputs, x output):                    */
/*                                                                                  */
/*        A        B                                                                */
/*            x                                                                     */
/*        C        D                                                                */
/*                                                                                  */
/*	x = (A + B + C + D + RTYPE + 1) >> 2                                            */
/*                                                                                  */
/*	The plane is downsampled from left to right and top to bottom to allow          */
/*	resampling in place.                                                            */
/*                                                                                  */
/*	Notes:                                                                          */
/*	1. The final rounding add (before division by shifting right) value is          */
/*		determined by the input boolean argument bRounding, which when FALSE        */
/*		indicates that the usual rounding value be decremented by 1. This           */
/*		is specific to annex P (differing from annexes O and Q).                    */
/*	2. Destination plane is filled at the right and bottom edges the number         */
/*		of columns and rows specified by the input fill parameters by               */
/*		replicating the edge pels.                                                  */
/************************************************************************************/
static void RPRDownsamplePlane(struct Decoder * t,
						U8 *pSrcPlane,
						U8 *pDstPlane,
						U32 uWidthFill,		/* Num bytes to fill out on right edge */
						U32 uHeightFill,	/* Num bytes to fill out on bottom edge */
						RV_Boolean bRounding,
						U32 uPlane
						)
{
	PU8 pSrc;			/* upper left corner of source plane */
	PU8 pDst;			/* upper left corner of destination plane */
	U32 uSrcPitch;
	U32 uDstPitch;
	U32 uSrcWidth;
	U32 uSrcHeight;
	U32 uDstWidth;
	U32 row, col;
	U8 uRound;
	PU8 pSrcLine2;

	pSrc = pSrcPlane;
	pDst = pDstPlane;

	/* if not luma plane divide dimensions by 2 for chroma planes */
	if (uPlane){	
		uSrcPitch = t->m_img_UVstride;
		uDstPitch = t->m_img_UVstride;
		uSrcWidth = t->m_pre_out_img_width >> 1;
		uDstWidth = t->m_out_img_width >> 1;
		uSrcHeight = t->m_pre_out_img_height >>1;
	}else{
	uSrcPitch = t->m_img_stride;
	uDstPitch = t->m_img_stride;
	uSrcWidth = t->m_pre_out_img_width;
	uDstWidth = t->m_out_img_width;
	uSrcHeight = t->m_pre_out_img_height;
	}

	RVAssert(uDstWidth == uSrcWidth>>1);

	uRound = bRounding ? 1 : 0;
	pSrcLine2 = pSrc + uSrcPitch;

	for (row=0; row<uSrcHeight; row+=2)
	{
		for (col=0; col<uSrcWidth; col+=2)
			*(pDst + (col>>1)) = (*(pSrc + col) + *(pSrc + col + 1)
			+ *(pSrcLine2 + col) + *(pSrcLine2 + col + 1)
			+ 1 + uRound) >> 2;

		if (uWidthFill)
		{
			for (col=0; col<uWidthFill; col++)
				*(pDst + uDstWidth + col) = *(pDst + uDstWidth - 1);
		}

		pSrc += uSrcPitch<<1;
		pSrcLine2 += uSrcPitch<<1;
		pDst += uDstPitch;
	}	/* for row */

	if (uHeightFill)
	{
		for (row=0; row<uHeightFill; row++)
		{
			for (col=0; col<(uDstWidth+uWidthFill); col++)
				*(pDst + col) = *(pDst - uDstPitch + col);
			pDst += uDstPitch;
		}
	}
}	/* end RPRDownsamplePlane */

/*****************************************************************************************************************
* ResamplePlane 
* Resamples the reference plane to the new resolutions defined by the input 
* parameters using the resampling definition in Annex P of the H.263 spec. 
* This version straightforwardly implements the pseudocode from the annex, 
* simplified by supporting limited warping, specifically the warping 
* parameters are restricted to picture edge displacements, instead of the 
* more general corner displacements. For example, the left edge of the picture 
* can be moved left or right; but the horizontal displacement of the upper 
* left and bottom left corners must be the same. This restriction results 
* in four of the eight warping parameters required to always be zero 
* (wx_y, wy_x, wx_xy, wy_xy). 
* The annex includes methods for obtaining the x and y displacements of 
* the picture at (0,j+1/2) and (H',j+1/2), in 1/32 pel accuracy (divide by 
* 32 to get actual displacement. The simplfied equations for these, 
* assuming the four warp parameters are zero, are the following: 
*	UxL(j) = Ux_00 = 16*wx_0 
*	Ux_H0 = 16(wx_0 + wx_x + 2(Hr-H)) 
*	UxR(j) = ((H-H')Ux_00 + H'*Ux_H0)//H 
*	Uy_00 = Uy_H0 = 16*wy_0 
*	Uy_0V = Uy_HV = 16(wy_0 + wy_y + 2(Vr-V)) 
*	Uy_LT = Uy_RT = Uy_00 
*	Uy_LB = Uy_RB = (V*Uy_00 + (Uy_0V - Uy_00)*V')//V 
*	UyL(j) = UyR(j) = ((S*V'-1)Uy_00 - 2*j*Uy_00 + 2*j*Uy_LB + Uy_LB)//(S*V') 
*	using the notation in the annex. 
*	The interpolation function, given input pels:
*		A	B 
*		C	D 
*	is ((16-Cy)*((16-Cx)*A + Cx*B) + Cy*((16-Cx)*C + Cx*D) + 127 + RCRPR)/256 
*	Notes: 
*	1. Rotational warping is not supported. 
*	2. Displacement accuracy is 1/16 pel only (1/2 pel not supported). 
************************************************************************************************************************/
void ResamplePlane(struct Decoder *t,U8 *pSrc, U8 *pDst, U32 uWidthFill, U32 uHeightFill, RV_Boolean bRounding, T_RPR_EdgeParams *pEdgeParams, U32 uPlane)
{
	U32 uDestRow, uDestColumn;
	U32 uRCRPR;				/* interpolation rounding */
	I32 x, y;				/* offsets in input plane */
	U32 uBiC_x, uBiC_y;		/* bilinear filter coeffs */
	I32 iUxL, iUxR, iUyL;	/* row start/end offsets -- see notes */
	I32 iUy0V, iUy00, iUyLB;
	I32 iUyL_num;			/* UyL numerator */
	I32 iUyL_inc;			/* UyL numerator increment each row */
	I32 Hprime;				/* next power of 2 greater than uInWidth; */
	I32 Vprime;				/* next power of 2 greater than uInHeight; */
	I32 m;					/* log2(Hprime) */
	I32 n;					/* log2(Vprime) */
	I32 D;					/* a constant, see annex */
	I32 P = 16;				/* precision: 2 or 16 */
	U32 uLumaFactor;		/* 0 if chroma, else 1 */
	U32 uTemp;
	I32 ax_initial;
	I32 ax_increment;
	I32 ax, ay;
	PU8 pSrcRow;
	PU8 pDestRow;
	U8 uFillVal;
	I32 iLastSrcCol;
	I32 iLastSrcRow;

	/* input params from structs */
	PU8 pIn = pSrc;
	U32 uInWidth  = t->m_pre_out_img_width;
	U32 uInHeight = t->m_pre_out_img_height;
	U32 uOutWidth = t->m_out_img_width;
	U32 uOutHeight= t->m_out_img_height;
	U32 uInPitch, uPitch;

	if(!uPlane){
		uInPitch = t->m_img_stride;
		uPitch = t->m_img_stride;
	}else{
		uInPitch = t->m_img_UVstride;
		uPitch = t->m_img_UVstride;
	}

	/* initialize vars */
	uRCRPR = (TRUE == bRounding) ? 1 : 0;
	uLumaFactor = (0 == uPlane) ? 1: 0;
	iLastSrcCol = (uInWidth>>(1-uLumaFactor)) - 1;
	iLastSrcRow = (uInHeight>>(1-uLumaFactor)) - 1;
	m = 0;
	uTemp = uInWidth;

	while (uTemp > 0){
		m++;
		uTemp >>= 1;
	}
	/* check for case when uInWidth is power of two */
	if (uInWidth == (U32)(1<<(m-1))) m--;
	Hprime = 1<<m;
	D = (64*Hprime)/P;

	n = 0;
	uTemp = uInHeight;

	while (uTemp > 0){
		n++;
		uTemp >>= 1;
	}
	/* check for case when uInHeight is power of two */
	if (uInHeight == (U32)(1<<(n-1))) n--;
	Vprime = 1<<n;

	/* iUxL and iUxR are independent of row, so compute once only */
	iUxL = pEdgeParams->iDeltaLeft << 4;
	iUxR = (uOutWidth - Hprime)*iUxL + ((pEdgeParams->iDeltaRight +
		((uInWidth - uOutWidth)<<1))<<(4+m));		/* numerator part */
	/* complete iUxR init by dividing by H with rounding to nearest integer, */
	/* half-integers away from 0 */
	if (iUxR >= 0)
		iUxR = (iUxR + (uOutWidth>>1))/uOutWidth;
	else
		iUxR = (iUxR - (I32)(uOutWidth>>1))/(I32)uOutWidth;

	/* initial x displacement and the x increment are independent of row */
	/* so compute once only */
	ax_initial = (iUxL<<(m+uLumaFactor)) + (iUxR - iUxL) + (D>>1);
	ax_increment = (Hprime<<6) + ((iUxR - iUxL)<<1);

	/* most contributions to calculation of iUyL do not depend upon row, */
	/* compute once only */
	iUy00 = pEdgeParams->iDeltaTop<<4;
	iUy0V = (pEdgeParams->iDeltaBottom + ((uInHeight - uOutHeight)<<1))<<4;
	iUyLB = uOutHeight*iUy00 + ((iUy0V - iUy00)<<n); /* numerator */
	/* complete iUyLB by dividing by V with rounding to nearest integer, */
	/* half-integers away from 0 */
	if (iUyLB >= 0)
		iUyLB = (iUyLB + (uOutHeight>>1))/uOutHeight;
	else
		iUyLB = (iUyLB - (I32)(uOutHeight>>1))/(I32)uOutHeight;
	iUyL_inc = (iUyLB - iUy00)<<1;
	iUyL_num = ((Vprime<<uLumaFactor) - 1)*iUy00 + iUyLB;

	iUyL_num -= iUyL_inc;	/* predecrement for loop */
	pDestRow = pDst;

	for ( uDestRow = 0;uDestRow<(uOutHeight>>(1-uLumaFactor));uDestRow++, pDestRow += uPitch){	
		/* ay var is constant for all columns */
		iUyL_num += iUyL_inc;
		iUyL = iUyL_num >> (n + uLumaFactor);
		ay = uDestRow * D * P + (iUyL << (m + uLumaFactor)) + D/2;
		y = ay >> (m+6);
		uBiC_y = (ay >> (m+2)) & 0xf;
		ax = ax_initial;
		pSrcRow = pIn + y*uInPitch;

		for ( uDestColumn = 0;uDestColumn < (uOutWidth>>(1-uLumaFactor));uDestColumn++){
			U8 A, B, C, D;
			U32 uFilteredPel;

			x = ax >> (m+6);
			uBiC_x = (ax >> (m+2)) & 0xf;

			/* if all 4 pels are in reference just read them */
			if ( (x >= 0) &&
				(y >= 0) &&
				((x+1) <= iLastSrcCol) &&
				((y+1) <= iLastSrcRow) ){
				A = *(pSrcRow + x);
				B = *(pSrcRow + x + 1);
				C = *(pSrcRow + x + uInPitch);
				D = *(pSrcRow + x + uInPitch + 1);
			}else{
				/* at least one pel is outside the reference picture. Fetch */
				/* each of the 4 input pels individually, replacing with a fill */
				/* value when reference outside reference. */
				PU8 pRef;
				U32 xref;
				U32 yref;

				/* A */
				if ( (x >= 0) &&(y >= 0) && ((x) <= iLastSrcCol) &&((y) <= iLastSrcRow) ){
					A = *(pSrcRow + x);
				}else{
					xref = MIN(MAX(0, x), iLastSrcCol);
					yref = MIN(MAX(0, y), iLastSrcRow);
					pRef = pIn + yref*uInPitch + xref;
					A = *pRef;
				}

				/* B */
				if (((x+1) >= 0) && (y >= 0) && ((x+1) <= iLastSrcCol) &&(y <= iLastSrcRow)){
					B = *(pSrcRow + x + 1);
				}else{
					xref = MIN(MAX(0, x+1), iLastSrcCol);
					yref = MIN(MAX(0, y), iLastSrcRow);
					pRef = pIn + yref*uInPitch + xref;
					B = *pRef;
				}

				/* C */
				if((x >= 0) &&((y+1) >= 0) && (x <= iLastSrcCol) && ((y+1) <= iLastSrcRow)){
					C = *(pSrcRow + x + uInPitch);
				}else{
					xref = MIN(MAX(0, x), iLastSrcCol);
					yref = MIN(MAX(0, y+1), iLastSrcRow);
					pRef = pIn + yref*uInPitch + xref;
					C = *pRef;
				}

				/* D */
				if(((x+1) >= 0) && ((y+1) >= 0) && ((x+1) <= iLastSrcCol) &&((y+1) <= iLastSrcRow)){
					D = *(pSrcRow + x + uInPitch + 1);
				}else{
					xref = MIN(MAX(0, x+1), iLastSrcCol);
					yref = MIN(MAX(0, y+1), iLastSrcRow);
					pRef = pIn + yref*uInPitch + xref;
					D = *pRef;
				}
			}

			uFilteredPel =
				((16 - uBiC_y)*((16 - uBiC_x)*A + uBiC_x*B) +
				uBiC_y*((16 - uBiC_x)*C + uBiC_x*D) +
				256/2 - 1 + uRCRPR) / 256;
			*(pDestRow + uDestColumn) = (U8)uFilteredPel;
			ax += ax_increment;
		}	/* for uDestColumn */

		/* fill right edge to macroblock boundary if required */
		if (uWidthFill){

			uFillVal = *(pDestRow + uDestColumn - 1);

			for ( ; uDestColumn < (uOutWidth>>(1-uLumaFactor)) + uWidthFill;uDestColumn++)
				*(pDestRow + uDestColumn) = uFillVal;
		}	/* uWidthFill */

	}	/* for uDestRow */

	if (uHeightFill){

		uDestRow = uOutHeight>>(1-uLumaFactor);
		pDestRow = pDst + uDestRow * uInPitch;
		pSrcRow = pDestRow - uInPitch;

		for ( ; uDestRow < (uOutHeight>>(1-uLumaFactor)) + uHeightFill;uDestRow++, pDestRow += uInPitch ){
			for(uDestColumn=0;uDestColumn < (uOutWidth>>(1-uLumaFactor)) + uWidthFill;uDestColumn++){
				uFillVal = *(pSrcRow + uDestColumn);
				*(pDestRow + uDestColumn) = uFillVal;
			}
		}
	}	/* uHeightFill */

}	/* ResamplePlane */

/*******************************************************************/
/*  ResampleYUVFrame                                               */
/*	Resamples a YUV12 frame using the annex P algorithm.           */
/*                                                                 */
/*	Fills edge pad areas per fill parameters.                      */
/*                                                                 */
/*	Includes pan and/or zoom (non-zero EdgeParams).                */
/*******************************************************************/
void ResampleYUVFrame(struct Decoder *t,U8 *pSrcFrame[],U8 *pDstFrame[],T_RPR_EdgeParams *pEdgeParams)
{
	U32 uPlane;
	U32 uWidthFill = 0;
	U32 uHeightFill = 0;
	U32 src_img_width,src_img_height;
	U32 dst_img_width,dst_img_height;
	RV_Boolean bUpSamplex2 = FALSE;
	RV_Boolean bDownSamplex2 = FALSE;

	src_img_width = t->m_pre_out_img_width;
	src_img_height= t->m_pre_out_img_height;
	dst_img_width = t->m_out_img_width;
	dst_img_height= t->m_out_img_height;

	/* check for x2 up/down cases with no edge deltas and clip fill mode */
	if ( (pEdgeParams->iDeltaLeft | pEdgeParams->iDeltaRight |
		pEdgeParams->iDeltaTop | pEdgeParams->iDeltaBottom) == 0){
		if ((src_img_width << 1) == dst_img_width && (src_img_height << 1) == dst_img_height)
			bUpSamplex2 = TRUE;
		if ((src_img_width >> 1) == dst_img_width && (src_img_height >> 1) == dst_img_height)
			bDownSamplex2 = TRUE;
	}

	uWidthFill =  t->m_img_width - t->m_out_img_width;
	uHeightFill = t->m_img_height - t->m_out_img_height;

	/* for each plane */
	if (bUpSamplex2)
		RPRUpsamplePlane(t, pSrcFrame[0], pDstFrame[0], FALSE, 0);
	else if (bDownSamplex2)
		RPRDownsamplePlane(t, pSrcFrame[0], pDstFrame[0], uWidthFill, uHeightFill,FALSE, 0);
	else
		ResamplePlane(t, pSrcFrame[0], pDstFrame[0], uWidthFill, uHeightFill, FALSE,pEdgeParams,0);


	uWidthFill >>= 1;
	uHeightFill>>= 1;
	for(uPlane = 1; uPlane < 3; uPlane++){
		if (bUpSamplex2)
			RPRUpsamplePlane(t, pSrcFrame[uPlane], pDstFrame[uPlane], FALSE, uPlane);
		else if (bDownSamplex2)
			RPRDownsamplePlane(t, pSrcFrame[uPlane], pDstFrame[uPlane], uWidthFill, uHeightFill,FALSE, uPlane);
		else
			ResamplePlane(t, pSrcFrame[uPlane], pDstFrame[uPlane], uWidthFill, uHeightFill, FALSE,pEdgeParams,uPlane);
	}

}	/* end ResampleYUVFrame */
