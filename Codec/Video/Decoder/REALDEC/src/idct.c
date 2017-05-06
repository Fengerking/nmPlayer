/*********************************************** BEGIN LICENSE BLOCK **************************************************** 
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
******************************************************* END LICENSE BLOCK *********************************************************/ 

/********************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc.
*    All Rights Reserved. 
*    Do not redistribute. 
*********************************************************************************************
*********************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1999 Intel Corporation. 
*    All Rights Reserved. 
*    DESCRIPTION: 
*		RV89Combo optimized core inverse transform C functions 
***********************************************************************************************
************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/

/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/ixfrm.c,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */

#include "idct.h"

/*********************************************************************************************
* C_ITransform4x4 
* Compute the inverse transform of the input 4x4 block. Input and output 
* are I32 in a buffer with a pitch of 4*sizeof(I32). The transform is 
* done in place (output buffer is the input buffer). 
***********************************************************************************************/

#define SAT(Value) Value = Value < 0 ? 0: (Value > 255 ? 255: Value);

#define ADDSAT32(a,Dst,Add32)		\
	b = a + Add32;					\
	c = a & Add32;					\
	a ^= Add32;						\
	a &= ~b;						\
	a |= c;							\
	a &= MaskCarry;					\
	c = a << 1;						\
	b -= c;	/* adjust neighbour */	\
	b |= c - (a >> 7); /* mask */	\
	Dst = b;						

#define SUBSAT32(a,Dst,Add32)		\
	a = ~a;							\
	b = a + Add32;					\
	c = a & Add32;					\
	a ^= Add32;						\
	a &= ~b;						\
	a |= c;							\
	a &= MaskCarry;					\
	c = a << 1;						\
	b -= c;	/* adjust neighbour */	\
	b |= c - (a >> 7); /* mask */	\
	Dst = ~b;					

void  C_Transform4x4_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch)
{
	I32 *pin;
	I32 a0, a1, a2, a3;
	I32 b0, b1, b2, b3;
	I32 x0,x1,x2,x3,x4;
	I32 i;

	/* horizontal */
	pin = pQuantBuf;
	for (i = 0; i <= 3; i ++)
	{
		//For Telenor complaint CPK, downshifting by 20 is done later during 
		//reconstruction.  For APK and non-Telenor-complaint CPK,  
		//downshifting by 10 was done earlier during dequantization. 
		a0 = pin[0];
		a1 = pin[4];
		a2 = pin[8];
		a3 = pin[12];

		if ( !(a1|a2|a3) ){
			pin[0] = pin[12] = pin[4] = pin[8] = a0 * 13;
			pin++;
			continue;
		}

		b0 = (a0 + a2) * 13;
		b1 = (a0 - a2) * 13;
		b2 = a1 * 7  - a3 * 17;
		b3 = a1 * 17 + a3 * 7;

		pin[0] = b0 + b3;
		pin[12] = b0 - b3;
		pin[4] = b1 + b2;
		pin[8] = b1 - b2;

		pin++;
	}

	/* vertical */
	pin = pQuantBuf;
	for (i = 0; i <= 3; i ++)
	{
		a0 = pin[0];
		a1 = pin[1];
		a2 = pin[2];
		a3 = pin[3];

		if (!(a1|a2|a3)){
			x0 = x1 = x2 = x3 = (a0 * 13 + 0x200) >> 10;
		} 
		else{
			b0 = (a0 + a2) * 13;
			b1 = (a0 - a2) * 13;
			b2 = a1 * 7  - a3 * 17;
			b3 = a1 * 17 + a3 * 7;

			x0  = (b0 + b3 + 0x200) >> 10;
			x3  = (b0 - b3 + 0x200) >> 10;
			x1  = (b1 + b2 + 0x200) >> 10;
			x2  = (b1 - b2 + 0x200) >> 10;
		}

		x0 += pPredSrc[0];
		x1 += pPredSrc[1];
		x2 += pPredSrc[2];
		x3 += pPredSrc[3];

		x4 = (x0|x1|x2|x3)>>8;
		if (x4)
		{
			SAT(x0)
			SAT(x1)
			SAT(x2)
			SAT(x3)
		}

		pDest[0]  = (U8)x0;
		pDest[1]  = (U8)x1;
		pDest[2]  = (U8)x2;
		pDest[3]  = (U8)x3;

		pin += 4;
		pDest += uDestStride;
		pPredSrc += uPredPitch;
	}
} /* C_Transform4x4_Add */

void  C_Intra16x16ITransform4x4(I32 *in)
{
	I32 *pin;
	I32 a0, a1, a2, a3;
	I32 b0, b1, b2, b3;
	I32 i;

	/* horizontal */
	pin = in;
	for (i = 0; i <= 3; i ++)
	{
		/* For Telenor complaint CPK, downshifting by 20 is done later during */
		/* reconstruction.  For APK and non-Telenor-complaint CPK,  */
		/* downshifting by 10 was done earlier during dequantization. */
		a0 = pin[0];
		a1 = pin[1];
		a2 = pin[2];
		a3 = pin[3];

		b0 = (a0 + a2) * 13;
		b1 = (a0 - a2) * 13;
		b2 = a1 * 7  - a3 * 17;
		b3 = a1 * 17 + a3 * 7;

		pin[0] = b0 + b3;
		pin[3] = b0 - b3;
		pin[1] = b1 + b2;
		pin[2] = b1 - b2;

		pin += 4;
	}

	/* vertical */
	pin = in;
	for (i = 0; i <= 3; i ++)
	{
		b0 = (pin[0] + pin[8]) * 13;
		b1 = (pin[0] - pin[8]) * 13;
		b2 = pin[4] * 7  - pin[12] * 17;
		b3 = pin[4] * 17 + pin[12] * 7;

		pin[0]  = ((b0 + b3) * 48) >> 15;
		pin[12] = ((b0 - b3) * 48) >> 15;
		pin[4]  = ((b1 + b2) * 48) >> 15;
		pin[8]  = ((b1 - b2) * 48) >> 15;

		pin ++;
	}
} /* C_ITransform16x16 */
/*************************************************************************************************/
/* C_ITransform4x4_DCOnly_Add 	 */
/* Compute the inverse transform of the input 4x4 block with all of its AC  */
/* values zeros . Input and output are I32 in a buffer with a pitch of  */
/* 4*sizeof(I32). The transform is done in place. */
/************************************************************************************************/
#if 0
void  C_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch)
{
	I32 i;
	U32 MaskCarry = 0x80808080U;
	U32 a,b,c;
	I32 v = ((pQuantBuf[0] * 13 * 13) + 0x200) >> 10;

	if (v>0)
	{
		SAT(v)          
		v |= v << 8;
		v |= v << 16;

		for (i = 0; i < 4; i++)
		{
			a = ((U32*)pPredSrc)[0];
			ADDSAT32(a,((U32*)pDest)[0],v);
			pDest += uDestStride;
			pPredSrc += uPredPitch;
		}
	}
	else if (v<0)
	{
		v = -v;
		SAT(v)     
		v |= v << 8;
		v |= v << 16;

		for (i = 0; i < 4; i++)
		{
			a = ((U32*)pPredSrc)[0];
			SUBSAT32(a,((U32*)pDest)[0],v);
			pDest += uDestStride;
			pPredSrc += uPredPitch;
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			a = ((U32*)pPredSrc)[0];
			((U32*)pDest)[0] = a;
			pDest += uDestStride;
			pPredSrc += uPredPitch;

		}
	}

} /* C_ITransform4x4_DCOnly_Add */
#endif

void  C_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch)
{
	I32 i;
	//U32 MaskCarry = 0x80808080U;
	I32 a,b,c,d;
	I32 v = ((pQuantBuf[0] * 13 * 13) + 0x200) >> 10;

	if (v > 0)
	{
		SAT(v)         

		for (i = 0; i < 4; i++)
		{
			a = pPredSrc[0];
			b = pPredSrc[1];
			c = pPredSrc[2];
			d = pPredSrc[3];

			a += v;
			b += v;
			c += v;
			d += v;

			SAT(a)
			SAT(b)
			SAT(c)
			SAT(d)

			a |= b << 8;
			a |= c << 16;
			a |= d << 24;

			((U32*)pDest)[0] = a;
			pDest += uDestStride;
			pPredSrc += uPredPitch;
		}
	}
	else if (v < 0)
	{
		v = -v;
		SAT(v)    

		for (i = 0; i < 4; i++)
		{
			a = pPredSrc[0];
			b = pPredSrc[1];
			c = pPredSrc[2];
			d = pPredSrc[3];

			a -= v;
			b -= v;
			c -= v;
			d -= v;

			SAT(a)
			SAT(b)
			SAT(c)
			SAT(d)

			a |= b << 8;
			a |= c << 16;
			a |= d << 24;

			((U32*)pDest)[0] = a;
			pDest += uDestStride;
			pPredSrc += uPredPitch;
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			a = ((U32*)pPredSrc)[0];
			((U32*)pDest)[0] = a;
			pDest += uDestStride;
			pPredSrc += uPredPitch;

		}
	}

} /* C_ITransform4x4_DCOnly_Add */


