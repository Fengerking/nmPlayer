/**************************************************** BEGIN LICENSE BLOCK ************************************************** 
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
********************************************************* END LICENSE BLOCK ************************************************************/ 

/*********************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
* *******************************************************************************************
**********************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 2000 Intel Corporation. 
*    All Rights Reserved. 
*    DESCRIPTION: 
*	  RV89Combo optimized core encoder subpel interpolation functions, C version.
**************************************************************************************************
************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/cinterp.c,v 1.1.1.1 2005/12/19 23:01:44 rishimathew Exp $ */
/********************************************************************************
Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		Optimized code for Interpolation
********************************************************************************/
#include "tables.h"
#include "spinterp.h"

/* pitch of the prediction buffer which is the destination buffer for the */
/* functions in this file */
//#define DEST_PITCH 16

#if defined(VOARMV4)
	T_InterpFnxNew ARMV4_Interpolate_H00V00;
	T_InterpFnxNew ARMV4_Interpolate_H01V00;
	T_InterpFnxNew ARMV4_Interpolate_H02V00;
	T_InterpFnxNew ARMV4_Interpolate_H00V01;
	T_InterpFnxNew ARMV4_Interpolate_H01V01;
	T_InterpFnxNew ARMV4_Interpolate_H02V01;
	T_InterpFnxNew ARMV4_Interpolate_H00V02;
	T_InterpFnxNew ARMV4_Interpolate_H01V02;
	T_InterpFnxNew ARMV4_Interpolate_H02V02;
#elif defined(VOARMV6)
	T_InterpFnxNew ARMV6_Interpolate_H00V00;
	T_InterpFnxNew ARMV6_Interpolate_H01V00;
	T_InterpFnxNew ARMV6_Interpolate_H02V00;
	T_InterpFnxNew ARMV6_Interpolate_H00V01;
	T_InterpFnxNew ARMV6_Interpolate_H01V01;
	T_InterpFnxNew ARMV6_Interpolate_H02V01;
	T_InterpFnxNew ARMV6_Interpolate_H00V02;
	T_InterpFnxNew ARMV6_Interpolate_H01V02;
	T_InterpFnxNew ARMV6_Interpolate_H02V02;

	T_InterpFnxNew ARMV6_MCCopyChroma_H00V00;
	T_InterpFnxNew ARMV6_MCCopyChroma_H01V00;
	T_InterpFnxNew ARMV6_MCCopyChroma_H02V00;
	T_InterpFnxNew ARMV6_MCCopyChroma_H00V01;
	T_InterpFnxNew ARMV6_MCCopyChroma_H01V01;
	T_InterpFnxNew ARMV6_MCCopyChroma_H02V01;
	T_InterpFnxNew ARMV6_MCCopyChroma_H00V02;
	T_InterpFnxNew ARMV6_MCCopyChroma_H01V02;
	T_InterpFnxNew ARMV6_MCCopyChroma_H02V02;

	T_InterpFnxNew ARMV6_AddInterpolate_H00V00;
	T_InterpFnxNew ARMV6_AddInterpolate_H01V00;
	T_InterpFnxNew ARMV6_AddInterpolate_H02V00;
	T_InterpFnxNew ARMV6_AddInterpolate_H00V01;
	T_InterpFnxNew ARMV6_AddInterpolate_H01V01;
	T_InterpFnxNew ARMV6_AddInterpolate_H02V01;
	T_InterpFnxNew ARMV6_AddInterpolate_H00V02;
	T_InterpFnxNew ARMV6_AddInterpolate_H01V02;
	T_InterpFnxNew ARMV6_AddInterpolate_H02V02;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H00V00;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H01V00;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H02V00;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H00V01;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H01V01;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H02V01;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H00V02;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H01V02;
	T_InterpFnxNew ARMV6_AddMCCopyChroma_H02V02;
#elif defined(VOARMV7)
	T_InterpFnxNew ARMV7_Interpolate_H00V00;
	T_InterpFnxNew ARMV7_Interpolate_H01V00;
	T_InterpFnxNew ARMV7_Interpolate_H02V00;
	T_InterpFnxNew ARMV7_Interpolate_H00V01;
	T_InterpFnxNew ARMV7_Interpolate_H01V01;
	T_InterpFnxNew ARMV7_Interpolate_H02V01;
	T_InterpFnxNew ARMV7_Interpolate_H00V02;
	T_InterpFnxNew ARMV7_Interpolate_H01V02;
	T_InterpFnxNew ARMV7_Interpolate_H02V02;

	T_InterpFnxNew ARMV7_MCCopyChroma_H00V00;
	T_InterpFnxNew ARMV7_MCCopyChroma_H01V00;
	T_InterpFnxNew ARMV7_MCCopyChroma_H02V00;
	T_InterpFnxNew ARMV7_MCCopyChroma_H00V01;
	T_InterpFnxNew ARMV7_MCCopyChroma_H01V01;
	T_InterpFnxNew ARMV7_MCCopyChroma_H02V01;
	T_InterpFnxNew ARMV7_MCCopyChroma_H00V02;
	T_InterpFnxNew ARMV7_MCCopyChroma_H01V02;
	T_InterpFnxNew ARMV7_MCCopyChroma_H02V02;

	T_InterpFnxNew ARMV7_AddInterpolate_H00V00;
	T_InterpFnxNew ARMV7_AddInterpolate_H01V00;
	T_InterpFnxNew ARMV7_AddInterpolate_H02V00;
	T_InterpFnxNew ARMV7_AddInterpolate_H00V01;
	T_InterpFnxNew ARMV7_AddInterpolate_H01V01;
	T_InterpFnxNew ARMV7_AddInterpolate_H02V01;
	T_InterpFnxNew ARMV7_AddInterpolate_H00V02;
	T_InterpFnxNew ARMV7_AddInterpolate_H01V02;
	T_InterpFnxNew ARMV7_AddInterpolate_H02V02;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H00V00;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H01V00;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H02V00;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H00V01;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H01V01;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H02V01;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H00V02;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H01V02;
	T_InterpFnxNew ARMV7_AddMCCopyChroma_H02V02;
#else
void   C_Interpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H01V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H02V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H00V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H01V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H02V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H00V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H01V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_Interpolate_H02V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void   C_MCCopyChroma_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H01V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H02V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H00V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H01V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H02V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H00V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H01V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_MCCopyChroma_H02V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void   C_AddInterpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H01V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H02V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H00V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H01V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H02V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H00V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H01V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddInterpolate_H02V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H01V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H02V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H00V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H01V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H02V01(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H00V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H01V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void   C_AddMCCopyChroma_H02V02(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
#endif

const T_InterpFnxTableNewPtr InterpolateLuma_RV8[16] =
{
#if defined(VOARMV4)
	ARMV4_Interpolate_H00V00, ARMV4_Interpolate_H01V00,
	ARMV4_Interpolate_H02V00, NULL,
	ARMV4_Interpolate_H00V01, ARMV4_Interpolate_H01V01,
	ARMV4_Interpolate_H02V01, NULL,
	ARMV4_Interpolate_H00V02, ARMV4_Interpolate_H01V02,
	ARMV4_Interpolate_H02V02, NULL
#elif defined(VOARMV6)
	ARMV6_Interpolate_H00V00, ARMV6_Interpolate_H01V00,
	ARMV6_Interpolate_H02V00, NULL,
	ARMV6_Interpolate_H00V01, ARMV6_Interpolate_H01V01,
	ARMV6_Interpolate_H02V01, NULL,
	ARMV6_Interpolate_H00V02, ARMV6_Interpolate_H01V02,
	ARMV6_Interpolate_H02V02, NULL

	//ARMV6_Interpolate_H00V00, ARMV6_Interpolate_H01V00,
	//ARMV6_Interpolate_H02V00, NULL,
	//ARMV6_Interpolate_H00V01, ARMV6_Interpolate_H01V01,
	//ARMV6_Interpolate_H02V01, NULL,
	//ARMV6_Interpolate_H00V02, C_Interpolate_H01V02,
	//ARMV6_Interpolate_H02V02, NULL
#elif defined(VOARMV7)
	ARMV7_Interpolate_H00V00, ARMV7_Interpolate_H01V00,
	ARMV7_Interpolate_H02V00, NULL,
	ARMV7_Interpolate_H00V01, ARMV7_Interpolate_H01V01,
	ARMV7_Interpolate_H02V01, NULL,
	ARMV7_Interpolate_H00V02, ARMV7_Interpolate_H01V02,
	ARMV7_Interpolate_H02V02, NULL
#else
	C_Interpolate_H00V00, C_Interpolate_H01V00,
	C_Interpolate_H02V00, NULL,
	C_Interpolate_H00V01, C_Interpolate_H01V01,
	C_Interpolate_H02V01, NULL,
	C_Interpolate_H00V02, C_Interpolate_H01V02,
	C_Interpolate_H02V02, NULL
#endif
};

const T_InterpFnxTableNewPtr InterpolateChroma_RV8[16] =
{
#if defined(VOARMV4)
#elif defined(VOARMV6)
	ARMV6_MCCopyChroma_H00V00, ARMV6_MCCopyChroma_H01V00,
	ARMV6_MCCopyChroma_H02V00, NULL,
	ARMV6_MCCopyChroma_H00V01, ARMV6_MCCopyChroma_H01V01,
	ARMV6_MCCopyChroma_H02V01, NULL,
	ARMV6_MCCopyChroma_H00V02, ARMV6_MCCopyChroma_H01V02,
	ARMV6_MCCopyChroma_H02V02, NULL
#elif defined(VOARMV7)
	ARMV7_MCCopyChroma_H00V00, ARMV7_MCCopyChroma_H01V00,
	ARMV7_MCCopyChroma_H02V00, NULL,
	ARMV7_MCCopyChroma_H00V01, ARMV7_MCCopyChroma_H01V01,
	ARMV7_MCCopyChroma_H02V01, NULL,
	ARMV7_MCCopyChroma_H00V02, ARMV7_MCCopyChroma_H01V02,
	ARMV7_MCCopyChroma_H02V02, NULL
#else
	C_MCCopyChroma_H00V00, C_MCCopyChroma_H01V00,
	C_MCCopyChroma_H02V00, NULL,
	C_MCCopyChroma_H00V01, C_MCCopyChroma_H01V01,
	C_MCCopyChroma_H02V01, NULL,
	C_MCCopyChroma_H00V02, C_MCCopyChroma_H01V02,
	C_MCCopyChroma_H02V02, NULL
#endif
};

const T_InterpFnxTableNewPtr InterpolateAddLuma_RV8[16] =
{
#if defined(VOARMV4)
#elif defined(VOARMV6)
	ARMV6_AddInterpolate_H00V00, ARMV6_AddInterpolate_H01V00,
	ARMV6_AddInterpolate_H02V00, NULL,
	ARMV6_AddInterpolate_H00V01, ARMV6_AddInterpolate_H01V01,
	ARMV6_AddInterpolate_H02V01, NULL,
	ARMV6_AddInterpolate_H00V02, ARMV6_AddInterpolate_H01V02,
	ARMV6_AddInterpolate_H02V02, NULL
#elif defined(VOARMV7)
	ARMV7_AddInterpolate_H00V00, ARMV7_AddInterpolate_H01V00,
	ARMV7_AddInterpolate_H02V00, NULL,
	ARMV7_AddInterpolate_H00V01, ARMV7_AddInterpolate_H01V01,
	ARMV7_AddInterpolate_H02V01, NULL,
	ARMV7_AddInterpolate_H00V02, ARMV7_AddInterpolate_H01V02,
	ARMV7_AddInterpolate_H02V02, NULL
#else
	C_AddInterpolate_H00V00, C_AddInterpolate_H01V00,
	C_AddInterpolate_H02V00, NULL,
	C_AddInterpolate_H00V01, C_AddInterpolate_H01V01,
	C_AddInterpolate_H02V01, NULL,
	C_AddInterpolate_H00V02, C_AddInterpolate_H01V02,
	C_AddInterpolate_H02V02, NULL
#endif
};

const T_InterpFnxTableNewPtr InterpolateAddChroma_RV8[16] =
{
#if defined(VOARMV4)
#elif defined(VOARMV6)
	ARMV6_AddMCCopyChroma_H00V00, ARMV6_AddMCCopyChroma_H01V00,
	ARMV6_AddMCCopyChroma_H02V00, NULL,
	ARMV6_AddMCCopyChroma_H00V01, ARMV6_AddMCCopyChroma_H01V01,
	ARMV6_AddMCCopyChroma_H02V01, NULL,
	ARMV6_AddMCCopyChroma_H00V02, ARMV6_AddMCCopyChroma_H01V02,
	ARMV6_AddMCCopyChroma_H02V02, NULL
#elif defined(VOARMV7)
	ARMV7_AddMCCopyChroma_H00V00, ARMV7_AddMCCopyChroma_H01V00,
	ARMV7_AddMCCopyChroma_H02V00, NULL,
	ARMV7_AddMCCopyChroma_H00V01, ARMV7_AddMCCopyChroma_H01V01,
	ARMV7_AddMCCopyChroma_H02V01, NULL,
	ARMV7_AddMCCopyChroma_H00V02, ARMV7_AddMCCopyChroma_H01V02,
	ARMV7_AddMCCopyChroma_H02V02, NULL
#else
	C_AddMCCopyChroma_H00V00, C_AddMCCopyChroma_H01V00,
	C_AddMCCopyChroma_H02V00, NULL,
	C_AddMCCopyChroma_H00V01, C_AddMCCopyChroma_H01V01,
	C_AddMCCopyChroma_H02V01, NULL,
	C_AddMCCopyChroma_H00V02, C_AddMCCopyChroma_H01V02,
	C_AddMCCopyChroma_H02V02, NULL
#endif
};


/******************************************************************
* C_Interpolate_H00V00 
*	 0 horizontal displacement 
*	 0 vertical displacement 
*	 No interpolation required, simple block copy. 
**************************************************************** */
void  C_Interpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow, i,j;
	PU32 pTempSrc;
	PU32 pTempDst;

	/* Do not perform a sequence of U32 copies, since this function  */
	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */

	/*MAP -- Loops are modified as decrementing loops for the purpose*/
	/*of ARM optimization.											 */

	if ((i = (I32)((PU8)pSrc - (PU8)0) & 0x03) == 0)
	{
		pTempSrc = (PU32)(pSrc);
		pTempDst = (PU32)(pDst);
		for (dstRow = 8; dstRow > 0; dstRow--)
		{
			for (j = 8; j > 0; j-=8)
			{
				pTempDst[0] = pTempSrc[0];
				pTempDst[1] = pTempSrc[1];
				pTempDst += 2;
				pTempSrc += 2;
			}
			pTempDst += ((uDstPitch - 8) >> 2);
			pTempSrc += ((uSrcPitch - 8) >> 2);
		}
	}
	else
	{
		pTempSrc = (PU32)((PU8)pSrc - i);
		pTempDst = (PU32)(pDst);
		i = i<<3;
		for (dstRow = 8; dstRow > 0; dstRow--)
		{
			for (j = 8; j > 0; j-=8)
			{
				pTempDst[0] = (pTempSrc[0] >> i)|(pTempSrc[1] << (32 - i));
				pTempDst[1] = (pTempSrc[1] >> i)|(pTempSrc[2] << (32 - i));
				pTempDst += 2;
				pTempSrc += 2;
			}
			pTempDst += ((uDstPitch - 8) >> 2);
			pTempSrc += ((uSrcPitch - 8) >> 2);
		}
	}
}	/* H00V00 */

/*******************************************************************
* C_Interpolate_H01V00 
*	1/3 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (-1,12,6,-1) 
********************************************************************/
void  C_Interpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32 lTemp;
	I32 dstRow, dstCol;        
	I32 lTemp0, lTemp1;
	I32 lTemp2, lTemp3;

	/*MAP -- Process 4 pixels at a time. Pixel values pSrc[x] are taken into*/
	/*temporary variables lTempX, so that number of loads can be minimized. */
	/*Decrementing loops are used for the purpose of optimization			*/

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -= 4)
		{
			lTemp0 = pSrc[-1]; 
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[1];
			lTemp3 = pSrc[2];

			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 8) >> 4;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = pSrc[3];
			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[4];
			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[5];
			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp3)) << 24;

			//*((PU32)pDst)++ = lTemp;
			*((PU32)pDst) = lTemp;
			pDst += 4;
			pSrc += 4;
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}
}	/* H01V00 */

/******************************************************************
* C_Interpolate_H02V00 
*	2/3 pel horizontal displacement
*	0 vertical displacement 
*	Use horizontal filter (-1,6,12,-1) 
******************************************************************/
void C_Interpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32 lTemp;
	I32 dstRow, dstCol;
	I32 lTemp0, lTemp1;
	I32 lTemp2, lTemp3;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -= 4)
		{
			lTemp0 = pSrc[-1]; 
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[1];
			lTemp3 = pSrc[2];

			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 8) >> 4;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = pSrc[3];
			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[4];
			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[5];
			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp3)) << 24;

			//*((PU32)pDst)++ = lTemp;
			*((PU32)pDst) = lTemp;
			pDst += 4;
			pSrc += 4;		
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}
}	/* H02V00 */

/*******************************************************************************
* C_Interpolate_H00V01 
*	0 horizontal displacement 
*	1/3 vertical displacement 
*	Use vertical filter (-1,12,6,-1) 
******************************************************************************/

void  C_Interpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    

	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	/*MAP -- Process 4 pixels at a time. While doing vertical interploation  */
	/*we process along columns instead of rows so that loads can be minimised*/
	/*Decrementing loops are used for the purpose of optimization			 */

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch)];
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[uSrcPitch];
			lTemp3 = pSrc[uSrcPitch<<1];

			lTemp0 = (6*((lTemp1 << 1) + lTemp2) - (lTemp0 + lTemp3) + 8) >> 4;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = pSrc[3*uSrcPitch];
			lTemp1 = (6*((lTemp2 << 1) + lTemp3) - (lTemp1 + lTemp0) + 8) >> 4;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = pSrc[uSrcPitch << 2];
			lTemp2 = (6*((lTemp3 << 1) + lTemp0) - (lTemp2 + lTemp1) + 8) >> 4;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = pSrc[5*uSrcPitch];
			lTemp3 = (6*((lTemp0 << 1) + lTemp1) - (lTemp3 + lTemp2) + 8) >> 4;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}
}	/* H00V01 */

/****************************************************************************
* C_Interpolate_H01V01 
*	1/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (-1,12,6,-1) 
*	Use vertical filter (-1,12,6,-1) 
***************************************************************************/
void  C_Interpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*19];
	I32 *b;
	const U8 *p;
	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	/*MAP -- Process 4 pixels at a time. First do horizantal interpolation   */
	/*followed by vertical interpolation. Decrementing loops are used for the*/
	/*purpose of ARM optimization											 */

	b = buff;
	p = pSrc - (I32)(uSrcPitch);

	for (dstRow = 8+3; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-1];
			lTemp1 = p[0];
			lTemp2 = p[1];
			lTemp3 = p[2];

			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
			b++;

			lTemp0 = p[3];
			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
			b++;

			lTemp1 = p[4];
			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
			b++;

			lTemp2 = p[5];
			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
			b++;

			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];

			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[64];
			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[80];
			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[96];
			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H01V01 */

/************************************************************************
* C_Interpolate_H02V01 
*	2/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (-1,6,12,-1)
*	Use vertical filter (-1,12,6,-1) 
************************************************************************/
void  C_Interpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*19];
	I32 *b;
	const U8 *p;
	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	b = buff;
	p = pSrc - (I32)(uSrcPitch);

	for (dstRow = 8+3; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-1];
			lTemp1 = p[0];
			lTemp2 = p[1];
			lTemp3 = p[2];

			*b = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3);
			b++;

			lTemp0 = p[3];
			*b = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0);
			b++;

			lTemp1 = p[4];
			*b= (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1);
			b++;

			lTemp2 = p[5];
			*b = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2);			
			b++;

			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];

			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[64];
			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[80];
			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[96];
			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}	
}	/* H02V01 */

/******************************************************************************
* C_Interpolate_H00V02 
*	0 horizontal displacement 
*	2/3 vertical displacement 
*	Use vertical filter (-1,6,12,-1) 
*****************************************************************************/

void C_Interpolate_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;

	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch)];
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[uSrcPitch];
			lTemp3 = pSrc[uSrcPitch<<1];

			lTemp0 = ((6*(lTemp1 + (lTemp2 << 1))) - (lTemp0 + lTemp3) + 8) >> 4;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = pSrc[3*uSrcPitch];
			lTemp1 = ((6*(lTemp2 + (lTemp3 << 1))) - (lTemp1 + lTemp0) + 8) >> 4;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = pSrc[4*uSrcPitch];
			lTemp2 = ((6*(lTemp3 + (lTemp0 << 1))) - (lTemp2 + lTemp1) + 8) >> 4;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = pSrc[5*uSrcPitch];
			lTemp3 = ((6*(lTemp0 + (lTemp1 << 1))) - (lTemp3 + lTemp2) + 8) >> 4;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}
}	/* H00V02 */

/*******************************************************************
* C_Interpolate_H01V02 
*	1/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (-1,12,6,-1) 
*	Use vertical filter (-1,6,12,-1) 
********************************************************************/
void  C_Interpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*19];
	I32 *b;
	const U8 *p;
	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	b = buff;
	p = pSrc - (I32)(uSrcPitch);

	for (dstRow = 8+3; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-1];
			lTemp1 = p[0];
			lTemp2 = p[1];
			lTemp3 = p[2];

			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
			b++;

			lTemp0 = p[3];
			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
			b++;

			lTemp1 = p[4];
			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
			b++;

			lTemp2 = p[5];
			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
			b++;

			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];

			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 128) >> 8;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[64];
			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 128) >> 8;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[80];
			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 128) >> 8;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[96];
			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 128) >> 8;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}
}	/* H01V02 */

/****************************************************************
* C_Interpolate_H02V02 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (6,9,1) 
*	Use vertical filter (6,9,1) 
****************************************************************/
void C_Interpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*18];
	I32 *b;	
	I32   lTemp0, lTemp1, lTemp2;    

	b = buff;
	for (dstRow = 8+2; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = pSrc[0];
			lTemp1 = pSrc[1];
			lTemp2 = pSrc[2];

			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);
			b++;

			lTemp0 = pSrc[3];
			*b = (6*lTemp1 + 9*lTemp2 + lTemp0);
			b++;

			lTemp1 = pSrc[4];
			*b = (6*lTemp2 + 9*lTemp0 + lTemp1);
			b++;

			lTemp2 = pSrc[5];
			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);			
			b++;

			pSrc +=4;			
		}
		b += (16 - 8);
		pSrc += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];

			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[48];
			lTemp1 = (6*lTemp1 + 9*lTemp2 + lTemp0 + 128) >> 8;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[64];
			lTemp2 = (6*lTemp2 + 9*lTemp0 + lTemp1 + 128) >> 8;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[80];
			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}
}	/* H02V02 */

/******************************************************************
* C_AddInterpolate_H00V00 
*	 0 horizontal displacement 
*	 0 vertical displacement 
*	 No interpolation required, simple block copy. 
**************************************************************** */
void C_AddInterpolate_H00V00(const U8 *pSrc,U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	U32 a,b,c,d;
	U32 q,w;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		a=pSrc[0]|(pSrc[1]<<8)|((pSrc[2]|(pSrc[3]<<8))<<16);
		b=pSrc[4]|(pSrc[5]<<8)|((pSrc[6]|(pSrc[7]<<8))<<16);
		c=((U32 *)pDst)[0];
		d=((U32 *)pDst)[1];
		q=(a|c) & 0x01010101;
		w=(b|d) & 0x01010101;
		q+=(a>>1) & 0x7F7F7F7F;
		w+=(b>>1) & 0x7F7F7F7F;
		q+=(c>>1) & 0x7F7F7F7F;
		w+=(d>>1) & 0x7F7F7F7F;
		((U32 *)pDst)[0]=q;
		((U32 *)pDst)[1]=w;
		pDst += uDstPitch;
		pSrc += uSrcPitch;
	}
}	/* H00V00 */

/*******************************************************************
* C_AddInterpolate_H01V00 
*	1/3 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (-1,12,6,-1) 
********************************************************************/
void  C_AddInterpolate_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32 lTemp;
	U32 c,q;
	I32 dstRow, dstCol;        
	I32 lTemp0, lTemp1;
	I32 lTemp2, lTemp3;

	/*MAP -- Process 4 pixels at a time. Pixel values pSrc[x] are taken into*/
	/*temporary variables lTempX, so that number of loads can be minimized. */
	/*Decrementing loops are used for the purpose of optimization			*/

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -= 4)
		{
			lTemp0 = pSrc[-1]; 
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[1];
			lTemp3 = pSrc[2];

			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 8) >> 4;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = pSrc[3];
			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[4];
			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[5];
			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp3)) << 24;

			c = ((U32 *)pDst)[0];
			q = (lTemp|c) & 0x01010101;
			q += (lTemp>>1) & 0x7F7F7F7F;
			q += (c>>1) & 0x7F7F7F7F;

			//*((PU32)pDst)++ = q;
			*((PU32)pDst) = q;
			pDst += 4;
			pSrc += 4;
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}
}	/* H01V00 */

/******************************************************************
* C_AddInterpolate_H02V00 
*	2/3 pel horizontal displacement
*	0 vertical displacement 
*	Use horizontal filter (-1,6,12,-1) 
******************************************************************/
void  C_AddInterpolate_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32 lTemp;
	U32 c,q;
	I32 dstRow, dstCol;
	I32 lTemp0, lTemp1;
	I32 lTemp2, lTemp3;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -= 4)
		{
			lTemp0 = pSrc[-1]; 
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[1];
			lTemp3 = pSrc[2];

			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 8) >> 4;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = pSrc[3];
			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[4];
			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[5];
			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 8) >> 4;
			lTemp  |= (ClampVal(lTemp3)) << 24;

			c=((U32 *)pDst)[0];
			q=(lTemp|c) & 0x01010101;
			q+=(lTemp>>1) & 0x7F7F7F7F;
			q+=(c>>1) & 0x7F7F7F7F;

			//*((PU32)pDst)++ = q;
			*((PU32)pDst) = q;

			pDst += 4;
			pSrc += 4;		
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}
}	/* H02V00 */

/*******************************************************************************
* C_AddInterpolate_H00V01 
*	0 horizontal displacement 
*	1/3 vertical displacement 
*	Use vertical filter (-1,12,6,-1) 
******************************************************************************/

void  C_AddInterpolate_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    

	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	/*MAP -- Process 4 pixels at a time. While doing vertical interploation  */
	/*we process along columns instead of rows so that loads can be minimised*/
	/*Decrementing loops are used for the purpose of optimization			 */

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch)];
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[uSrcPitch];
			lTemp3 = pSrc[uSrcPitch<<1];

			lTemp0 = (6*((lTemp1 << 1) + lTemp2) - (lTemp0 + lTemp3) + 8) >> 4;
			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = pSrc[3*uSrcPitch];
			lTemp1 = (6*((lTemp2 << 1) + lTemp3) - (lTemp1 + lTemp0) + 8) >> 4;
			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = pSrc[uSrcPitch << 2];
			lTemp2 = (6*((lTemp3 << 1) + lTemp0) - (lTemp2 + lTemp1) + 8) >> 4;
			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = pSrc[5*uSrcPitch];
			lTemp3 = (6*((lTemp0 << 1) + lTemp1) - (lTemp3 + lTemp2) + 8) >> 4;
			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}
}	/* H00V01 */

/****************************************************************************
* C_AddInterpolate_H01V01 
*	1/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (-1,12,6,-1) 
*	Use vertical filter (-1,12,6,-1) 
***************************************************************************/
void C_AddInterpolate_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*19];
	I32 *b;
	const U8 *p;
	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	/*MAP -- Process 4 pixels at a time. First do horizantal interpolation   */
	/*followed by vertical interpolation. Decrementing loops are used for the*/
	/*purpose of ARM optimization											 */

	b = buff;
	p = pSrc - (I32)(uSrcPitch);

	for (dstRow = 8+3; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-1];
			lTemp1 = p[0];
			lTemp2 = p[1];
			lTemp3 = p[2];

			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
			b++;

			lTemp0 = p[3];
			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
			b++;

			lTemp1 = p[4];
			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
			b++;

			lTemp2 = p[5];
			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
			b++;

			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];

			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[64];
			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[80];
			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[96];
			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H01V01 */

/************************************************************************
* C_AddInterpolate_H02V01 
*	2/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (-1,6,12,-1)
*	Use vertical filter (-1,12,6,-1) 
************************************************************************/
void C_AddInterpolate_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*19];
	I32 *b;
	const U8 *p;
	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	b = buff;
	p = pSrc - (I32)(uSrcPitch);

	for (dstRow = 8+3; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-1];
			lTemp1 = p[0];
			lTemp2 = p[1];
			lTemp3 = p[2];

			*b = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3);
			b++;

			lTemp0 = p[3];
			*b = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0);
			b++;

			lTemp1 = p[4];
			*b= (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1);
			b++;

			lTemp2 = p[5];
			*b = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2);			
			b++;

			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];

			lTemp0 = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3 + 128) >> 8;
			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[64];
			lTemp1 = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0 + 128) >> 8;
			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[80];
			lTemp2 = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1 + 128) >> 8;
			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[96];
			lTemp3 = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2 + 128) >> 8;
			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}	
}	/* H02V01 */

/******************************************************************************
* C_AddInterpolate_H00V02 
*	0 horizontal displacement 
*	2/3 vertical displacement 
*	Use vertical filter (-1,6,12,-1) 
*****************************************************************************/

void  C_AddInterpolate_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;

	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch)];
			lTemp1 = pSrc[0];
			lTemp2 = pSrc[uSrcPitch];
			lTemp3 = pSrc[uSrcPitch<<1];

			lTemp0 = ((6*(lTemp1 + (lTemp2 << 1))) - (lTemp0 + lTemp3) + 8) >> 4;
			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = pSrc[3*uSrcPitch];
			lTemp1 = ((6*(lTemp2 + (lTemp3 << 1))) - (lTemp1 + lTemp0) + 8) >> 4;
			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = pSrc[4*uSrcPitch];
			lTemp2 = ((6*(lTemp3 + (lTemp0 << 1))) - (lTemp2 + lTemp1) + 8) >> 4;
			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = pSrc[5*uSrcPitch];
			lTemp3 = ((6*(lTemp0 + (lTemp1 << 1))) - (lTemp3 + lTemp2) + 8) >> 4;
			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}
}	/* H00V02 */

/*******************************************************************
* C_AddInterpolate_H01V02 
*	1/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (-1,12,6,-1) 
*	Use vertical filter (-1,6,12,-1) 
********************************************************************/
void  C_AddInterpolate_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*19];
	I32 *b;
	const U8 *p;
	I32   lTemp0, lTemp1;
	I32   lTemp2, lTemp3;

	b = buff;
	p = pSrc - (I32)(uSrcPitch);

	for (dstRow = 8+3; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-1];
			lTemp1 = p[0];
			lTemp2 = p[1];
			lTemp3 = p[2];

			*b = (-lTemp0 + 6*((lTemp1 << 1) + lTemp2) - lTemp3);
			b++;

			lTemp0 = p[3];
			*b = (-lTemp1 + 6*((lTemp2 << 1) + lTemp3) - lTemp0);
			b++;

			lTemp1 = p[4];
			*b = (-lTemp2 + 6*((lTemp3 << 1) + lTemp0) - lTemp1);
			b++;

			lTemp2 = p[5];
			*b = (-lTemp3 + 6*((lTemp0 << 1) + lTemp1) - lTemp2);			
			b++;

			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];

			lTemp0 = (-lTemp0 + 6*(lTemp1 + (lTemp2 << 1)) - lTemp3 + 128) >> 8;
			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[64];
			lTemp1 = (-lTemp1 + 6*(lTemp2 + (lTemp3 << 1)) - lTemp0 + 128) >> 8;
			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[80];
			lTemp2 = (-lTemp2 + 6*(lTemp3 + (lTemp0 << 1)) - lTemp1 + 128) >> 8;
			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[96];
			lTemp3 = (-lTemp3 + 6*(lTemp0 + (lTemp1 << 1)) - lTemp2 + 128) >> 8;
			*pDst = (ClampVal(lTemp3) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}
}	/* H01V02 */

/****************************************************************
* C_AddInterpolate_H02V02 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (6,9,1) 
*	Use vertical filter (6,9,1) 
****************************************************************/
void  C_AddInterpolate_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32 buff[16*18];
	I32 *b;	
	I32   lTemp0, lTemp1, lTemp2;    

	b = buff;
	for (dstRow = 8+2; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = pSrc[0];
			lTemp1 = pSrc[1];
			lTemp2 = pSrc[2];

			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);
			b++;

			lTemp0 = pSrc[3];
			*b = (6*lTemp1 + 9*lTemp2 + lTemp0);
			b++;

			lTemp1 = pSrc[4];
			*b = (6*lTemp2 + 9*lTemp0 + lTemp1);
			b++;

			lTemp2 = pSrc[5];
			*b = (6*lTemp0 + 9*lTemp1 + lTemp2);			
			b++;

			pSrc +=4;			
		}
		b += (16 - 8);
		pSrc += (uSrcPitch - 8);
	}

	b = buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];

			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[48];
			lTemp1 = (6*lTemp1 + 9*lTemp2 + lTemp0 + 128) >> 8;
			*pDst = (ClampVal(lTemp1) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[64];
			lTemp2 = (6*lTemp2 + 9*lTemp0 + lTemp1 + 128) >> 8;
			*pDst = (ClampVal(lTemp2) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[80];
			lTemp0 = (6*lTemp0 + 9*lTemp1 + lTemp2 + 128) >> 8;
			*pDst = (ClampVal(lTemp0) + pDst[0] + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}
}	/* H02V02 */

/* chroma functions */
/* Block size is 4x4 for all. */

/******************************************************************
* C_MCCopyChroma_H00V00 
*	 0 horizontal displacement 
*	 0 vertical displacement 
*	 No interpolation required, simple block copy. 
*******************************************************************/
void  C_MCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;

	/* Do not perform a sequence of U32 copies, since this function */
	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */

	for (dstRow = 4; dstRow > 0; dstRow--)
	{
		pDst[0] = pSrc[0];
		pDst[1] = pSrc[1];
		pDst[2] = pSrc[2];
		pDst[3] = pSrc[3];

		pDst += uDstPitch;
		pSrc += uSrcPitch;
	}
}	/* C_MCCopyChroma_H00V00 */

/*********************************************************************
* C_MCCopyChroma_H01V00 
*	Motion compensated 4x4 chroma block copy.
*	1/3 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (5,3) 
*	Dst pitch is uDstPitch. 
**********************************************************************/
void  C_MCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp  = (5*lTemp0 + 3*lTemp1 + 4)>>3;

		lTemp0 = pSrc[2];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];	
		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		*((PU32)pDst) = lTemp;
		pSrc += uSrcPitch;
		pDst += uDstPitch;
	}
}	/* C_MCCopyChroma_H01V00 */


/******************************************************************
* C_MCCopyChroma_H02V00 
*	Motion compensated 4x4 chroma block copy.
*	2/3 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (3,5) 
*	Dst pitch is uDstPitch. 
********************************************************************/
void  C_MCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp  = (3*lTemp0 + 5*lTemp1 + 4)>>3;

		lTemp0 = pSrc[2];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];	
		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		*((PU32)pDst) = lTemp;
		pSrc += uSrcPitch;
		pDst += uDstPitch;
	}
}	/* C_MCCopyChroma_H02V00 */

/******************************************************************
* C_MCCopyChroma_H00V01 
*	Motion compensated 4x4 chroma block copy.
*	0 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use vertical filter (5,3) 
*	Dst pitch is uDstPitch. 
******************************************************************/
void  C_MCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[uSrcPitch];
		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
		pDst[0] = (U8)lTemp0;

		lTemp0 = pSrc[uSrcPitch << 1];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		pDst[uDstPitch] = (U8)lTemp1;

		lTemp1 = pSrc[3*uSrcPitch];
		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
		pDst[uDstPitch << 1] = (U8)lTemp0;

		lTemp0 = pSrc[uSrcPitch << 2];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		pDst[3*uDstPitch] = (U8)lTemp1;

		pDst++;
		pSrc++;
	}
}	/* C_MCCopyChroma_H00V01 */

/*****************************************************************
* C_MCCopyChroma_H00V02 
*	Motion compensated 4x4 chroma block copy.
*	0 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
*****************************************************************/
void  C_MCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[uSrcPitch];
		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
		pDst[0] = (U8)lTemp0;

		lTemp0 = pSrc[uSrcPitch << 1];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		pDst[uDstPitch] = (U8)lTemp1;

		lTemp1 = pSrc[3*uSrcPitch];
		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
		pDst[uDstPitch << 1] = (U8)lTemp0;

		lTemp0 = pSrc[uSrcPitch << 2];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		pDst[3*uDstPitch] = (U8)lTemp1;

		pDst++;
		pSrc++;
	}
}	/* C_MCCopyChroma_H00V02 */

/******************************************************************
* C_MCCopyChroma_H01V01 
*	Motion compensated chroma 4x4 block copy.
*	1/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (5,3) 
*	Use vertical filter (5,3) 
*	Dst pitch is uDstPitch. 
*******************************************************************/
void C_MCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32   lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	const U8 *pSrc2 = pSrc + uSrcPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 24);

		*((PU32)pDst) = lTemp;
		pDst  += uDstPitch;
		pSrc  += uSrcPitch;
		pSrc2 += uSrcPitch;
	}
}	/* C_MCCopyChroma_H01V01 */

/*****************************************************************
* C_MCCopyChroma_H02V01 
*	Motion compensated 4x4 chroma block copy.
*	2/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (3,5) 
*	Use vertical filter (5,3) 
*	Dst pitch is uDstPitch. 
******************************************************************/
void C_MCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32   lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	const U8 *pSrc2 = pSrc + uSrcPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
		lTemp |= (lTemp1 << 24);

		*((PU32)pDst) = lTemp;
		pDst  += uDstPitch;
		pSrc  += uSrcPitch;
		pSrc2 += uSrcPitch;
	}

}	/* C_MCCopyChroma_H02V01 */

/**********************************************************************
* C_MCCopyChroma_H01V02 
*	Motion compensated 4x4 chroma block copy.
*	1/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (5,3) 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
**********************************************************************/
void  C_MCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32	  lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	const U8 *pSrc2 = pSrc + uSrcPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
		lTemp |= (lTemp1 << 24);

		*((PU32)pDst) = lTemp;
		pDst  += uDstPitch;
		pSrc  += uSrcPitch;
		pSrc2 += uSrcPitch;
	}
}	/* C_MCCopyChroma_H01V02 */

/*******************************************************************
* C_MCCopyChroma_H02V02 
*	Motion compensated 4x4 chroma block copy. 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (3,5) 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
********************************************************************/
void  C_MCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32   lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	const U8 *pSrc2 = pSrc + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 24);

		*((PU32)pDst) = lTemp;
		pDst  += uDstPitch;
		pSrc  += uPitch;
		pSrc2 += uPitch;
	}

}	/* C_MCCopyChroma_H02V02 */

/******************************************************************
* C_AddMCCopyChroma_H00V00 
*	 0 horizontal displacement 
*	 0 vertical displacement 
*	 No interpolation required, simple block copy. 
*******************************************************************/
void  C_AddMCCopyChroma_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;

	/* Do not perform a sequence of U32 copies, since this function */
	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */

	for (dstRow = 4; dstRow > 0; dstRow--)
	{
		pDst[0] = (pSrc[0] + pDst[0] + 1)>>1;
		pDst[1] = (pSrc[1] + pDst[1] + 1)>>1;
		pDst[2] = (pSrc[2] + pDst[2] + 1)>>1;
		pDst[3] = (pSrc[3] + pDst[3] + 1)>>1;

		pDst += uDstPitch;
		pSrc += uSrcPitch;
	}
}	/* C_MCCopyChroma_H00V00 */

/*********************************************************************
* C_AddMCCopyChroma_H01V00 
*	Motion compensated 4x4 chroma block copy.
*	1/3 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (5,3) 
*	Dst pitch is uDstPitch. 
**********************************************************************/
void  C_AddMCCopyChroma_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;
	U32 c,q;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp  = (5*lTemp0 + 3*lTemp1 + 4)>>3;

		lTemp0 = pSrc[2];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];	
		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		c=((U32 *)pDst)[0];
		q=(lTemp|c) & 0x01010101;
		q+=(lTemp>>1) & 0x7F7F7F7F;
		q+=(c>>1) & 0x7F7F7F7F;

		*((PU32)pDst) = q;
		pSrc += uSrcPitch;
		pDst += uDstPitch;
	}
}	/* C_MCCopyChroma_H01V00 */


/******************************************************************
* C_AddMCCopyChroma_H02V00 
*	Motion compensated 4x4 chroma block copy.
*	2/3 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (3,5) 
*	Dst pitch is uDstPitch. 
********************************************************************/
void  C_AddMCCopyChroma_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;
	U32 c,q;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp  = (3*lTemp0 + 5*lTemp1 + 4)>>3;

		lTemp0 = pSrc[2];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];	
		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		c=((U32 *)pDst)[0];
		q=(lTemp|c) & 0x01010101;
		q+=(lTemp>>1) & 0x7F7F7F7F;
		q+=(c>>1) & 0x7F7F7F7F;

		*((PU32)pDst) = q;
		pSrc += uSrcPitch;
		pDst += uDstPitch;
	}
}	/* C_MCCopyChroma_H02V00 */

/******************************************************************
* C_AddMCCopyChroma_H00V01 
*	Motion compensated 4x4 chroma block copy.
*	0 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use vertical filter (5,3) 
*	Dst pitch is uDstPitch. 
******************************************************************/
void  C_AddMCCopyChroma_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[uSrcPitch];
		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
		pDst[0] = ((U8)lTemp0 + pDst[0] + 1)>>1;

		lTemp0 = pSrc[uSrcPitch << 1];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		pDst[uDstPitch] = ((U8)lTemp1 + pDst[uDstPitch] + 1)>>1;

		lTemp1 = pSrc[3*uSrcPitch];
		lTemp0 = (5*lTemp0 + 3*lTemp1 + 4)>>3;
		pDst[uDstPitch << 1] = ((U8)lTemp0 + pDst[uDstPitch << 1] + 1)>>1;

		lTemp0 = pSrc[uSrcPitch << 2];
		lTemp1 = (5*lTemp1 + 3*lTemp0 + 4)>>3;
		pDst[3*uDstPitch] = ((U8)lTemp1 + pDst[3*uDstPitch] + 1)>>1;

		pDst++;
		pSrc++;
	}
}	/* C_MCCopyChroma_H00V01 */

/*****************************************************************
* C_AddMCCopyChroma_H00V02 
*	Motion compensated 4x4 chroma block copy.
*	0 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
*****************************************************************/
void  C_AddMCCopyChroma_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[uSrcPitch];
		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
		pDst[0] = ((U8)lTemp0 + pDst[0] + 1)>>1;

		lTemp0 = pSrc[uSrcPitch << 1];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		pDst[uDstPitch] = ((U8)lTemp1 + pDst[uDstPitch] + 1)>>1;

		lTemp1 = pSrc[3*uSrcPitch];
		lTemp0 = (3*lTemp0 + 5*lTemp1 + 4)>>3;
		pDst[uDstPitch << 1] = ((U8)lTemp0 + pDst[uDstPitch << 1] + 1)>>1;

		lTemp0 = pSrc[uSrcPitch << 2];
		lTemp1 = (3*lTemp1 + 5*lTemp0 + 4)>>3;
		pDst[3*uDstPitch] = ((U8)lTemp1 + pDst[3*uDstPitch] + 1)>>1;

		pDst++;
		pSrc++;
	}
}	/* C_MCCopyChroma_H00V02 */

/******************************************************************
* C_AddMCCopyChroma_H01V01 
*	Motion compensated chroma 4x4 block copy.
*	1/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (5,3) 
*	Use vertical filter (5,3) 
*	Dst pitch is uDstPitch. 
*******************************************************************/
void  C_AddMCCopyChroma_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32   j;
	U32   lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	U32   c,q;
	const U8 *pSrc2 = pSrc + uSrcPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (25*lTemp0 + 15*(lTemp1 + lTemp2) + 9*lTemp3 + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (25*lTemp1 + 15*(lTemp0 + lTemp3) + 9*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 24);

		c=((U32 *)pDst)[0];
		q=(lTemp|c) & 0x01010101;
		q+=(lTemp>>1) & 0x7F7F7F7F;
		q+=(c>>1) & 0x7F7F7F7F;

		*((PU32)pDst) = q;
		pDst  += uDstPitch;
		pSrc  += uSrcPitch;
		pSrc2 += uSrcPitch;
	}
}	/* C_MCCopyChroma_H01V01 */

/*****************************************************************
* C_AddMCCopyChroma_H02V01 
*	Motion compensated 4x4 chroma block copy.
*	2/3 pel horizontal displacement 
*	1/3 vertical displacement 
*	Use horizontal filter (3,5) 
*	Use vertical filter (5,3) 
*	Dst pitch is uDstPitch. 
******************************************************************/
void  C_AddMCCopyChroma_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32   j;
	U32   lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	U32   c,q;
	const U8 *pSrc2 = pSrc + uSrcPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (15*(lTemp0 + lTemp3) + 25*lTemp1 + 9*lTemp2 + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (15*(lTemp1 + lTemp2) + 25*lTemp0 + 9*lTemp3 + 32)>>6;
		lTemp |= (lTemp1 << 24);

		c=((U32 *)pDst)[0];
		q=(lTemp|c) & 0x01010101;
		q+=(lTemp>>1) & 0x7F7F7F7F;
		q+=(c>>1) & 0x7F7F7F7F;

		*((PU32)pDst) = q;
		pDst  += uDstPitch;
		pSrc  += uSrcPitch;
		pSrc2 += uSrcPitch;
	}

}	/* C_MCCopyChroma_H02V01 */

/**********************************************************************
* C_AddMCCopyChroma_H01V02 
*	Motion compensated 4x4 chroma block copy.
*	1/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (5,3) 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
**********************************************************************/
void  C_AddMCCopyChroma_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32   j;
	U32	  lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	U32   c,q;
	const U8 *pSrc2 = pSrc + uSrcPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (9*lTemp1 + 25*lTemp2 + 15*(lTemp0 + lTemp3) + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (9*lTemp0 + 25*lTemp3 + 15*(lTemp1 + lTemp2) + 32)>>6;
		lTemp |= (lTemp1 << 24);

		c=((U32 *)pDst)[0];
		q=(lTemp|c) & 0x01010101;
		q+=(lTemp>>1) & 0x7F7F7F7F;
		q+=(c>>1) & 0x7F7F7F7F;

		*((PU32)pDst) = q;
		pDst  += uDstPitch;
		pSrc  += uSrcPitch;
		pSrc2 += uSrcPitch;
	}
}	/* C_MCCopyChroma_H01V02 */

/*******************************************************************
* C_AddMCCopyChroma_H02V02 
*	Motion compensated 4x4 chroma block copy. 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (3,5) 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
********************************************************************/
void  C_AddMCCopyChroma_H02V02(const U8 *pSrc, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32   j;
	U32   lTemp;
	U32   lTemp0, lTemp1;
	U32   lTemp2, lTemp3;
	U32   c,q;
	const U8 *pSrc2 = pSrc + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pSrc[0];
		lTemp1 = pSrc[1];
		lTemp2 = pSrc2[0];
		lTemp3 = pSrc2[1];
		lTemp  = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;

		lTemp0 = pSrc[2];
		lTemp2 = pSrc2[2];
		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pSrc[3];
		lTemp3 = pSrc2[3];
		lTemp0 = (9*lTemp0 + 15*(lTemp1 + lTemp2) + 25*lTemp3 + 32)>>6;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pSrc[4];
		lTemp2 = pSrc2[4];
		lTemp1 = (9*lTemp1 + 15*(lTemp0 + lTemp3) + 25*lTemp2 + 32)>>6;
		lTemp |= (lTemp1 << 24);

		c=((U32 *)pDst)[0];
		q=(lTemp|c) & 0x01010101;
		q+=(lTemp>>1) & 0x7F7F7F7F;
		q+=(c>>1) & 0x7F7F7F7F;

		*((PU32)pDst) = q;
		pDst  += uDstPitch;
		pSrc  += uPitch;
		pSrc2 += uPitch;
	}

}	/* C_MCCopyChroma_H02V02 */


/********************************************************************************************* *
* CopyBlock4x4 
* Copy the 4x4 block from pSrc to pDst.
* Predictor block is U8 and has a pitch of 16*sizeof(U8).
* Result is U8 stored in the reconstructed block using specified pitch.
***********************************************************************************************/
void CopyBlock4x4(const U8 *Src, U8 *Dst,  I32 SrcPitch, I32 DstPitch)
{
	U8 *SrcEnd = (U8*)Src + 4*SrcPitch;
	U32 a,b;

	do
	{
		a=((U32*)Src)[0];
		Src += SrcPitch;
		b=((U32*)Src)[0];
		Src += SrcPitch;
		((U32*)Dst)[0]=a;
		Dst += DstPitch;
		((U32*)Dst)[0]=b;
		Dst += DstPitch;
	}
	while (Src != SrcEnd);

}	/* CopyBlock4x4 */

/********************************************************************************************* *
* CopyBlock8x8 
* Copy the 8x8 block from pSrc to pDst.
* Predictor block is U8 and has a pitch of 16*sizeof(U8).
* Result is U8 stored in the reconstructed block using specified pitch.
***********************************************************************************************/
void CopyBlock8x8_C(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
{
	U8 *SrcEnd = (U8 *)Src + 8*SrcPitch;
	U32 a,b,c,d;
	do
	{
		a=((U32*)Src)[0];
		b=((U32*)Src)[1];
		Src += SrcPitch;
		c=((U32*)Src)[0];
		d=((U32*)Src)[1];
		Src += SrcPitch;
		((U32*)Dst)[0]=a;
		((U32*)Dst)[1]=b;
		Dst += DstPitch;
		((U32*)Dst)[0]=c;
		((U32*)Dst)[1]=d;
		Dst += DstPitch;
	}
	while (Src != SrcEnd);

}	/* CopyBlock4x4 */
/**********************************************************************************************
* CopyBlock16x16 
* Copy the 16x16 block from pSrc to pDst.
* Predictor block is U8 and has a pitch of 16*sizeof(U8).
* Result is U8 stored in the reconstructed block using specified pitch.
***********************************************************************************************/
void CopyBlock16x16_C(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch)
{
	U8 *SrcEnd = (U8 *)Src + 16*SrcPitch;
	U32 a,b,c,d;

	do
	{
		a =((U32*)Src)[0];
		b =((U32*)Src)[1];
		c =((U32*)Src)[2];
		d =((U32*)Src)[3]; 
		Src += SrcPitch;
		((U32*)Dst)[0] = a;
		((U32*)Dst)[1] = b;
		((U32*)Dst)[2] = c;
		((U32*)Dst)[3] = d;
		Dst += DstPitch;
	}
	while (Src != SrcEnd);

}	/* CopyBlock16x16 */

#define AddRow(a,b,c,d,e)			\
{								\
	U32 q,w;				\
	c=((U32*)Src1)[e];		\
	d=((U32*)Src1)[e+1];		\
	q=(a|c) & 0x01010101;		\
	w=(b|d) & 0x01010101;		\
	q+=(a>>1) & 0x7F7F7F7F;		\
	w+=(b>>1) & 0x7F7F7F7F;		\
	q+=(c>>1) & 0x7F7F7F7F;		\
	w+=(d>>1) & 0x7F7F7F7F;		\
	((U32*)Dst)[e]=q;		\
	((U32*)Dst)[e+1]=w;		\
}
void AddBlock8x8_C(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
{
	U8 *SrcEnd = (U8 *)Src + 8*Pitch;
	U32 a,b,c,d;
	do
	{
		a=((U32*)Src)[0];
		b=((U32*)Src)[1];
		AddRow(a,b,c,d, 0);

		Src	 += Pitch;
		Src1 += Pitch;
		Dst  += Pitch;

		a=((U32*)Src)[0];
		b=((U32*)Src)[1];
		AddRow(a,b,c,d, 0);

		Src	 += Pitch;
		Src1 += Pitch;
		Dst  += Pitch;
	}
	while (Src != SrcEnd);

}

/**********************************************************************************************
* CopyBlock16x16 
* Copy the 16x16 block from pSrc to pDst.
* Predictor block is U8 and has a pitch of 16*sizeof(U8).
* Result is U8 stored in the reconstructed block using specified pitch.
***********************************************************************************************/
void AddBlock16x16_C(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch)
{
	U8 *SrcEnd = (U8 *)Src + 16*Pitch;
	U32 a,b,c,d;

	do
	{
		a=((U32*)Src)[0];
		b=((U32*)Src)[1];
		AddRow(a,b,c,d, 0);

		a=((U32*)Src)[2];
		b=((U32*)Src)[3];
		AddRow(a,b,c,d, 2);
		Src	 += Pitch;
		Src1 += Pitch;
		Dst  += Pitch;
		a=((U32*)Src)[0];
		b=((U32*)Src)[1];
		AddRow(a,b,c,d, 0);

		a=((U32*)Src)[2];
		b=((U32*)Src)[3];
		AddRow(a,b,c,d, 2);
		Src	 += Pitch;
		Src1 += Pitch;
		Dst  += Pitch;
	}
	while (Src != SrcEnd);


}	/* CopyBlock16x16 */

