/********************************************************* BEGIN LICENSE BLOCK ******************************************** 
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
************************************************************** END LICENSE BLOCK *******************************************/ 
/**************************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 2001 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
*    Optimized core encoder subpel interpolation functions, C version. 
***************************************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/cinterp4.c,v 1.1.1.1 2005/12/19 23:01:44 rishimathew Exp $ */
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

#if defined(VOARMV4)
	T_InterpFnxNew ARMV4_Interpolate4_H00V00;
	T_InterpFnxNew ARMV4_Interpolate4_H01V00;
	T_InterpFnxNew ARMV4_Interpolate4_H02V00;
	T_InterpFnxNew ARMV4_Interpolate4_H03V00;
	T_InterpFnxNew ARMV4_Interpolate4_H00V01;
	T_InterpFnxNew ARMV4_Interpolate4_H01V01;
	T_InterpFnxNew ARMV4_Interpolate4_H02V01;
	T_InterpFnxNew ARMV4_Interpolate4_H03V01;
	T_InterpFnxNew ARMV4_Interpolate4_H00V02;
	T_InterpFnxNew ARMV4_Interpolate4_H01V02;
	T_InterpFnxNew ARMV4_Interpolate4_H02V02;
	T_InterpFnxNew ARMV4_Interpolate4_H03V02;
	T_InterpFnxNew ARMV4_Interpolate4_H00V03;
	T_InterpFnxNew ARMV4_Interpolate4_H01V03;
	T_InterpFnxNew ARMV4_Interpolate4_H02V03;
	T_InterpFnxNew ARMV4_Interpolate4_H03V03;
	/*chroma*/
	T_InterpFnxNew ARMV4_MCCopyChroma4_H00V00;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H01V00;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H02V00;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H03V00;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H00V01;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H01V01;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H02V01;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H03V01;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H00V02;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H01V02;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H02V02;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H03V02;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H00V03;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H01V03;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H02V03;
	T_InterpFnxNew ARMV4_MCCopyChroma4_H03V03;
#elif defined(VOARMV6)
	/*luma*/
	T_InterpFnxNew ARMV6_Interpolate4_H00V00;
	T_InterpFnxNew ARMV6_Interpolate4_H01V00;
	T_InterpFnxNew ARMV6_Interpolate4_H02V00;
	T_InterpFnxNew ARMV6_Interpolate4_H03V00;
	T_InterpFnxNew ARMV6_Interpolate4_H00V01;
	T_InterpFnxNew ARMV6_Interpolate4_H01V01;
	T_InterpFnxNew ARMV6_Interpolate4_H02V01;
	T_InterpFnxNew ARMV6_Interpolate4_H03V01;
	T_InterpFnxNew ARMV6_Interpolate4_H00V02;
	T_InterpFnxNew ARMV6_Interpolate4_H01V02;
	T_InterpFnxNew ARMV6_Interpolate4_H02V02;
	T_InterpFnxNew ARMV6_Interpolate4_H03V02;
	T_InterpFnxNew ARMV6_Interpolate4_H00V03;
	T_InterpFnxNew ARMV6_Interpolate4_H01V03;
	T_InterpFnxNew ARMV6_Interpolate4_H02V03;
	T_InterpFnxNew ARMV6_Interpolate4_H03V03;
	/*chroma*/
	T_InterpFnxNew ARMV6_MCCopyChroma4_H00V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H01V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H02V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H03V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H00V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H01V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H02V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H03V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H00V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H01V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H02V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H03V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H00V03;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H01V03;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H02V03;
	T_InterpFnxNew ARMV6_MCCopyChroma4_H03V03;
	/*luma*/
	T_InterpFnxNew ARMV6_Interpolate4Add_H00V00;
	T_InterpFnxNew ARMV6_Interpolate4Add_H01V00;
	T_InterpFnxNew ARMV6_Interpolate4Add_H02V00;
	T_InterpFnxNew ARMV6_Interpolate4Add_H03V00;
	T_InterpFnxNew ARMV6_Interpolate4Add_H00V01;
	T_InterpFnxNew ARMV6_Interpolate4Add_H01V01;
	T_InterpFnxNew ARMV6_Interpolate4Add_H02V01;
	T_InterpFnxNew ARMV6_Interpolate4Add_H03V01;
	T_InterpFnxNew ARMV6_Interpolate4Add_H00V02;
	T_InterpFnxNew ARMV6_Interpolate4Add_H01V02;
	T_InterpFnxNew ARMV6_Interpolate4Add_H02V02;
	T_InterpFnxNew ARMV6_Interpolate4Add_H03V02;
	T_InterpFnxNew ARMV6_Interpolate4Add_H00V03;
	T_InterpFnxNew ARMV6_Interpolate4Add_H01V03;
	T_InterpFnxNew ARMV6_Interpolate4Add_H02V03;
	T_InterpFnxNew ARMV6_Interpolate4Add_H03V03;
	/*chroma*/
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H00V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H01V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H02V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H03V00;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H00V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H01V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H02V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H03V01;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H00V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H01V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H02V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H03V02;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H00V03;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H01V03;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H02V03;
	T_InterpFnxNew ARMV6_MCCopyChroma4Add_H03V03;

#elif defined(VOARMV7)
	T_InterpFnxNew Interpolate4_H00V00_CtxA8;
	T_InterpFnxNew Interpolate4_H01V00_CtxA8;
	T_InterpFnxNew Interpolate4_H02V00_CtxA8;
	T_InterpFnxNew Interpolate4_H03V00_CtxA8;
	T_InterpFnxNew Interpolate4_H00V01_CtxA8;
	T_InterpFnxNew Interpolate4_H01V01_CtxA8;
	T_InterpFnxNew Interpolate4_H02V01_CtxA8;
	T_InterpFnxNew Interpolate4_H03V01_CtxA8;
	T_InterpFnxNew Interpolate4_H00V02_CtxA8;
	T_InterpFnxNew Interpolate4_H01V02_CtxA8;
	T_InterpFnxNew Interpolate4_H02V02_CtxA8;
	T_InterpFnxNew Interpolate4_H03V02_CtxA8;
	T_InterpFnxNew Interpolate4_H00V03_CtxA8;
	T_InterpFnxNew Interpolate4_H01V03_CtxA8;
	T_InterpFnxNew Interpolate4_H02V03_CtxA8;
	T_InterpFnxNew Interpolate4_H03V03_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H00V00_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H01V00_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H02V00_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H03V00_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H00V01_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H01V01_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H02V01_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H03V01_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H00V02_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H01V02_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H02V02_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H03V02_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H00V03_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H01V03_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H02V03_CtxA8;
	T_InterpFnxNew Interpolate4_Chroma_H03V03_CtxA8;
	T_InterpFnxNew Interpolate4Add_H00V00_CtxA8;
	T_InterpFnxNew Interpolate4Add_H01V00_CtxA8;
	T_InterpFnxNew Interpolate4Add_H02V00_CtxA8;
	T_InterpFnxNew Interpolate4Add_H03V00_CtxA8;
	T_InterpFnxNew Interpolate4Add_H00V01_CtxA8;
	T_InterpFnxNew Interpolate4Add_H01V01_CtxA8;
	T_InterpFnxNew Interpolate4Add_H02V01_CtxA8;
	T_InterpFnxNew Interpolate4Add_H03V01_CtxA8;
	T_InterpFnxNew Interpolate4Add_H00V02_CtxA8;
	T_InterpFnxNew Interpolate4Add_H01V02_CtxA8;
	T_InterpFnxNew Interpolate4Add_H02V02_CtxA8;
	T_InterpFnxNew Interpolate4Add_H03V02_CtxA8;
	T_InterpFnxNew Interpolate4Add_H00V03_CtxA8;
	T_InterpFnxNew Interpolate4Add_H01V03_CtxA8;
	T_InterpFnxNew Interpolate4Add_H02V03_CtxA8;
	T_InterpFnxNew Interpolate4Add_H03V03_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H00V00_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H01V00_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H02V00_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H03V00_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H00V01_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H01V01_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H02V01_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H03V01_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H00V02_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H01V02_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H02V02_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H03V02_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H00V03_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H01V03_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H02V03_CtxA8;
	T_InterpFnxNew Interpolate4_ChromaAdd_H03V03_CtxA8;
#elif defined(VOWMMX2)
void  WMMX2_Interpolate4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  WMMX2_Interpolate4_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  WMMX2_Interpolate4_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  WMMX2_Interpolate4_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  WMMX2_Interpolate4_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  WMMX2_Interpolate4_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void  WMMX2_MCCopyChroma4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  WMMX2_MCCopyChroma4_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void WMMX2_MCCopyChroma4_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
//void  C_MCCopyChroma4_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void  WMMX2_Interpolate4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  WMMX2_Interpolate4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_Interpolate4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void  WMMX2_MCCopyChroma4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  WMMX2_MCCopyChroma4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
//void  C_MCCopyChroma4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
#else
void  C_Interpolate4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  C_Interpolate4_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  C_Interpolate4_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  C_Interpolate4_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  C_Interpolate4_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  C_Interpolate4_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void  C_MCCopyChroma4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  C_MCCopyChroma4_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void  C_Interpolate4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch); 
void  C_Interpolate4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_Interpolate4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

void  C_MCCopyChroma4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);
void  C_MCCopyChroma4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch);

#endif

const T_InterpFnxTableNewPtr InterpolateLuma_RV9[16] =
{
#if defined(VOARMV4)
	ARMV4_Interpolate4_H00V00, ARMV4_Interpolate5_H01V00, ARMV4_Interpolate5_H02V00, ARMV4_Interpolate4_H03V00,
	ARMV4_Interpolate4_H00V01, ARMV4_Interpolate5_H01V01, ARMV4_Interpolate5_H02V01, ARMV4_Interpolate4_H03V01,
	ARMV4_Interpolate4_H00V02, ARMV4_Interpolate5_H01V02, ARMV4_Interpolate5_H02V02, ARMV4_Interpolate4_H03V02,
	ARMV4_Interpolate4_H00V03, ARMV4_Interpolate5_H01V03, ARMV4_Interpolate4_H02V03, ARMV4_Interpolate4_H03V03
#elif defined(VOARMV6)
	ARMV6_Interpolate4_H00V00, ARMV6_Interpolate4_H01V00, ARMV6_Interpolate4_H02V00, ARMV6_Interpolate4_H03V00,
	ARMV6_Interpolate4_H00V01, ARMV6_Interpolate4_H01V01, ARMV6_Interpolate4_H02V01, ARMV6_Interpolate4_H03V01,
	ARMV6_Interpolate4_H00V02, ARMV6_Interpolate4_H01V02, ARMV6_Interpolate4_H02V02, ARMV6_Interpolate4_H03V02,
	ARMV6_Interpolate4_H00V03, ARMV6_Interpolate4_H01V03, ARMV6_Interpolate4_H02V03, ARMV6_Interpolate4_H03V03
#elif defined(VOARMV7)
	Interpolate4_H00V00_CtxA8, Interpolate4_H01V00_CtxA8, Interpolate4_H02V00_CtxA8, Interpolate4_H03V00_CtxA8,
	Interpolate4_H00V01_CtxA8, Interpolate4_H01V01_CtxA8, Interpolate4_H02V01_CtxA8, Interpolate4_H03V01_CtxA8,
	Interpolate4_H00V02_CtxA8, Interpolate4_H01V02_CtxA8, Interpolate4_H02V02_CtxA8, Interpolate4_H03V02_CtxA8,
	Interpolate4_H00V03_CtxA8, Interpolate4_H01V03_CtxA8, Interpolate4_H02V03_CtxA8, Interpolate4_H03V03_CtxA8
#elif defined(VOWMMX2)
	WMMX2_Interpolate4_H00V00, WMMX2_Interpolate4_H01V00,WMMX2_Interpolate4_H02V00, WMMX2_Interpolate4_H03V00,
	WMMX2_Interpolate4_H00V01, WMMX2_Interpolate4_H01V01,WMMX2_Interpolate4_H02V01, WMMX2_Interpolate4_H03V01,
	WMMX2_Interpolate4_H00V02, WMMX2_Interpolate4_H01V02,WMMX2_Interpolate4_H02V02, WMMX2_Interpolate4_H03V02,
	WMMX2_Interpolate4_H00V03, WMMX2_Interpolate4_H01V03,WMMX2_Interpolate4_H02V03, WMMX2_Interpolate4_H03V03
#else
	C_Interpolate4_H00V00, C_Interpolate4_H01V00,C_Interpolate4_H02V00, C_Interpolate4_H03V00,
	C_Interpolate4_H00V01, C_Interpolate4_H01V01,C_Interpolate4_H02V01, C_Interpolate4_H03V01,
	C_Interpolate4_H00V02, C_Interpolate4_H01V02,C_Interpolate4_H02V02, C_Interpolate4_H03V02,
	C_Interpolate4_H00V03, C_Interpolate4_H01V03,C_Interpolate4_H02V03, C_Interpolate4_H03V03
#endif
};

const T_InterpFnxTableNewPtr InterpolateChroma_RV9[16] =
{
#if defined(VOARMV4)
	ARMV4_MCCopyChroma4_H00V00, ARMV4_MCCopyChroma4_H01V00, ARMV4_MCCopyChroma4_H02V00, ARMV4_MCCopyChroma4_H03V00,
	ARMV4_MCCopyChroma4_H00V01, ARMV4_MCCopyChroma4_H01V01, ARMV4_MCCopyChroma4_H02V01, ARMV4_MCCopyChroma4_H03V01,
	ARMV4_MCCopyChroma4_H00V02, ARMV4_MCCopyChroma4_H01V02, ARMV4_MCCopyChroma4_H02V02, ARMV4_MCCopyChroma4_H03V02,
	ARMV4_MCCopyChroma4_H00V03, ARMV4_MCCopyChroma4_H01V03, ARMV4_MCCopyChroma4_H02V03, ARMV4_MCCopyChroma4_H02V02
#elif defined(VOARMV6)
	ARMV6_MCCopyChroma4_H00V00, ARMV6_MCCopyChroma4_H01V00, ARMV6_MCCopyChroma4_H02V00, ARMV6_MCCopyChroma4_H03V00,
	ARMV6_MCCopyChroma4_H00V01, ARMV6_MCCopyChroma4_H01V01, ARMV6_MCCopyChroma4_H02V01, ARMV6_MCCopyChroma4_H03V01,
	ARMV6_MCCopyChroma4_H00V02, ARMV6_MCCopyChroma4_H01V02, ARMV6_MCCopyChroma4_H02V02, ARMV6_MCCopyChroma4_H03V02,
	ARMV6_MCCopyChroma4_H00V03, ARMV6_MCCopyChroma4_H01V03, ARMV6_MCCopyChroma4_H02V03, ARMV6_MCCopyChroma4_H02V02

	//C_MCCopyChroma4_H00V00, C_MCCopyChroma4_H01V00, C_MCCopyChroma4_H02V00, C_MCCopyChroma4_H03V00,
	//C_MCCopyChroma4_H00V01, C_MCCopyChroma4_H01V01, C_MCCopyChroma4_H02V01, C_MCCopyChroma4_H03V01,
	//C_MCCopyChroma4_H00V02, C_MCCopyChroma4_H01V02, C_MCCopyChroma4_H02V02, C_MCCopyChroma4_H03V02,
	//C_MCCopyChroma4_H00V03, C_MCCopyChroma4_H01V03, C_MCCopyChroma4_H02V03, C_MCCopyChroma4_H02V02
#elif defined(VOARMV7)
	Interpolate4_Chroma_H00V00_CtxA8, Interpolate4_Chroma_H01V00_CtxA8, Interpolate4_Chroma_H02V00_CtxA8, Interpolate4_Chroma_H03V00_CtxA8,
	Interpolate4_Chroma_H00V01_CtxA8, Interpolate4_Chroma_H01V01_CtxA8, Interpolate4_Chroma_H02V01_CtxA8, Interpolate4_Chroma_H03V01_CtxA8,
	Interpolate4_Chroma_H00V02_CtxA8, Interpolate4_Chroma_H01V02_CtxA8, Interpolate4_Chroma_H02V02_CtxA8, Interpolate4_Chroma_H03V02_CtxA8,
	Interpolate4_Chroma_H00V03_CtxA8, Interpolate4_Chroma_H01V03_CtxA8, Interpolate4_Chroma_H02V03_CtxA8, Interpolate4_Chroma_H02V02_CtxA8
#elif defined(VOWMMX2)
	WMMX2_MCCopyChroma4_H00V00, WMMX2_MCCopyChroma4_H01V00, WMMX2_MCCopyChroma4_H02V00, WMMX2_MCCopyChroma4_H03V00,
	WMMX2_MCCopyChroma4_H00V01, WMMX2_MCCopyChroma4_H01V01, WMMX2_MCCopyChroma4_H02V01, WMMX2_MCCopyChroma4_H03V01,
	WMMX2_MCCopyChroma4_H00V02, WMMX2_MCCopyChroma4_H01V02, WMMX2_MCCopyChroma4_H02V02, WMMX2_MCCopyChroma4_H03V02,
	WMMX2_MCCopyChroma4_H00V03, WMMX2_MCCopyChroma4_H01V03, WMMX2_MCCopyChroma4_H02V03, WMMX2_MCCopyChroma4_H02V02
#else
	C_MCCopyChroma4_H00V00, C_MCCopyChroma4_H01V00, C_MCCopyChroma4_H02V00, C_MCCopyChroma4_H03V00,
	C_MCCopyChroma4_H00V01, C_MCCopyChroma4_H01V01, C_MCCopyChroma4_H02V01, C_MCCopyChroma4_H03V01,
	C_MCCopyChroma4_H00V02, C_MCCopyChroma4_H01V02, C_MCCopyChroma4_H02V02, C_MCCopyChroma4_H03V02,
	C_MCCopyChroma4_H00V03, C_MCCopyChroma4_H01V03, C_MCCopyChroma4_H02V03, C_MCCopyChroma4_H02V02
#endif
};

const T_InterpFnxTableNewPtr InterpolateAddLuma_RV9[16] =
{
#if defined(VOARMV4)
#elif defined(VOARMV6)
	ARMV6_Interpolate4Add_H00V00, ARMV6_Interpolate4Add_H01V00, ARMV6_Interpolate4Add_H02V00, ARMV6_Interpolate4Add_H03V00,
	ARMV6_Interpolate4Add_H00V01, ARMV6_Interpolate4Add_H01V01, ARMV6_Interpolate4Add_H02V01, ARMV6_Interpolate4Add_H03V01,
	ARMV6_Interpolate4Add_H00V02, ARMV6_Interpolate4Add_H01V02, ARMV6_Interpolate4Add_H02V02, ARMV6_Interpolate4Add_H03V02,
	ARMV6_Interpolate4Add_H00V03, ARMV6_Interpolate4Add_H01V03, ARMV6_Interpolate4Add_H02V03, ARMV6_Interpolate4Add_H03V03
#elif defined(VOARMV7)
	Interpolate4Add_H00V00_CtxA8, Interpolate4Add_H01V00_CtxA8,Interpolate4Add_H02V00_CtxA8, Interpolate4Add_H03V00_CtxA8,
	Interpolate4Add_H00V01_CtxA8, Interpolate4Add_H01V01_CtxA8,Interpolate4Add_H02V01_CtxA8, Interpolate4Add_H03V01_CtxA8,
	Interpolate4Add_H00V02_CtxA8, Interpolate4Add_H01V02_CtxA8,Interpolate4Add_H02V02_CtxA8, Interpolate4Add_H03V02_CtxA8,
	Interpolate4Add_H00V03_CtxA8, Interpolate4Add_H01V03_CtxA8,Interpolate4Add_H02V03_CtxA8, Interpolate4Add_H03V03_CtxA8
#elif defined(VOWMMX2)
	WMMX2_Interpolate4Add_H00V00, WMMX2_Interpolate4Add_H01V00,WMMX2_Interpolate4Add_H02V00, WMMX2_Interpolate4Add_H03V00,
	WMMX2_Interpolate4Add_H00V01, WMMX2_Interpolate4Add_H01V01,WMMX2_Interpolate4Add_H02V01, WMMX2_Interpolate4Add_H03V01,
	WMMX2_Interpolate4Add_H00V02, WMMX2_Interpolate4Add_H01V02,WMMX2_Interpolate4Add_H02V02, WMMX2_Interpolate4Add_H03V02,
	WMMX2_Interpolate4Add_H00V03, WMMX2_Interpolate4Add_H01V03,WMMX2_Interpolate4Add_H02V03, WMMX2_Interpolate4Add_H03V03
#else
	C_Interpolate4Add_H00V00, C_Interpolate4Add_H01V00,C_Interpolate4Add_H02V00, C_Interpolate4Add_H03V00,
	C_Interpolate4Add_H00V01, C_Interpolate4Add_H01V01,C_Interpolate4Add_H02V01, C_Interpolate4Add_H03V01,
	C_Interpolate4Add_H00V02, C_Interpolate4Add_H01V02,C_Interpolate4Add_H02V02, C_Interpolate4Add_H03V02,
	C_Interpolate4Add_H00V03, C_Interpolate4Add_H01V03,C_Interpolate4Add_H02V03, C_Interpolate4Add_H03V03
#endif
};

const T_InterpFnxTableNewPtr InterpolateAddChroma_RV9[16] =
{
#if defined(VOARMV4)
#elif defined(VOARMV6)
	ARMV6_MCCopyChroma4Add_H00V00, ARMV6_MCCopyChroma4Add_H01V00, ARMV6_MCCopyChroma4Add_H02V00, ARMV6_MCCopyChroma4Add_H03V00,
	ARMV6_MCCopyChroma4Add_H00V01, ARMV6_MCCopyChroma4Add_H01V01, ARMV6_MCCopyChroma4Add_H02V01, ARMV6_MCCopyChroma4Add_H03V01,
	ARMV6_MCCopyChroma4Add_H00V02, ARMV6_MCCopyChroma4Add_H01V02, ARMV6_MCCopyChroma4Add_H02V02, ARMV6_MCCopyChroma4Add_H03V02,
	ARMV6_MCCopyChroma4Add_H00V03, ARMV6_MCCopyChroma4Add_H01V03, ARMV6_MCCopyChroma4Add_H02V03, ARMV6_MCCopyChroma4Add_H02V02
#elif defined(VOARMV7)
	Interpolate4_ChromaAdd_H00V00_CtxA8, Interpolate4_ChromaAdd_H01V00_CtxA8, Interpolate4_ChromaAdd_H02V00_CtxA8, Interpolate4_ChromaAdd_H03V00_CtxA8,
	Interpolate4_ChromaAdd_H00V01_CtxA8, Interpolate4_ChromaAdd_H01V01_CtxA8, Interpolate4_ChromaAdd_H02V01_CtxA8, Interpolate4_ChromaAdd_H03V01_CtxA8,
	Interpolate4_ChromaAdd_H00V02_CtxA8, Interpolate4_ChromaAdd_H01V02_CtxA8, Interpolate4_ChromaAdd_H02V02_CtxA8, Interpolate4_ChromaAdd_H03V02_CtxA8,
	Interpolate4_ChromaAdd_H00V03_CtxA8, Interpolate4_ChromaAdd_H01V03_CtxA8, Interpolate4_ChromaAdd_H02V03_CtxA8, Interpolate4_ChromaAdd_H02V02_CtxA8
#elif defined(VOWMMX2)
	WMMX2_MCCopyChroma4Add_H00V00, WMMX2_MCCopyChroma4Add_H01V00, WMMX2_MCCopyChroma4Add_H02V00, WMMX2_MCCopyChroma4Add_H03V00,
	WMMX2_MCCopyChroma4Add_H00V01, WMMX2_MCCopyChroma4Add_H01V01, WMMX2_MCCopyChroma4Add_H02V01, WMMX2_MCCopyChroma4Add_H03V01,
	WMMX2_MCCopyChroma4Add_H00V02, WMMX2_MCCopyChroma4Add_H01V02, WMMX2_MCCopyChroma4Add_H02V02, WMMX2_MCCopyChroma4Add_H03V02,
	WMMX2_MCCopyChroma4Add_H00V03, WMMX2_MCCopyChroma4Add_H01V03, WMMX2_MCCopyChroma4Add_H02V03, WMMX2_MCCopyChroma4Add_H02V02
#else
	C_MCCopyChroma4Add_H00V00, C_MCCopyChroma4Add_H01V00, C_MCCopyChroma4Add_H02V00, C_MCCopyChroma4Add_H03V00,
	C_MCCopyChroma4Add_H00V01, C_MCCopyChroma4Add_H01V01, C_MCCopyChroma4Add_H02V01, C_MCCopyChroma4Add_H03V01,
	C_MCCopyChroma4Add_H00V02, C_MCCopyChroma4Add_H01V02, C_MCCopyChroma4Add_H02V02, C_MCCopyChroma4Add_H03V02,
	C_MCCopyChroma4Add_H00V03, C_MCCopyChroma4Add_H01V03, C_MCCopyChroma4Add_H02V03, C_MCCopyChroma4Add_H02V02
#endif
};

/* pitch of the prediction buffer which is the destination buffer for the */
/* functions in this file */
//#define DEST_PITCH 16

/*******************************************************************
* C_Interpolate4_H00V00 
*	 0 horizontal displacement 
*	 0 vertical displacement 
*	 No interpolation required, simple block copy. 
*******************************************************************/

void  C_Interpolate4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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

/******************************************************************
* C_Interpolate4_H01V00 
*	1/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1, -5, 52, 20, -5, 1) / 64 
******************************************************************/
void  C_Interpolate4_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32	  lTemp;	 
	I32   dstRow,dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;	

	/*MAP -- Process 4 pixels at a time. Pixel values pSrc[x] are taken into*/
	/*temporary variables lTempX, so that number of loads can be minimized. */
	/*Decrementing loops are used for the purpose of optimization			*/

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = pSrc[-2]; 
			lTemp1 = pSrc[-1];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[1];
			lTemp4 = pSrc[2];
			lTemp5 = pSrc[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = pSrc[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = pSrc[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = pSrc[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)pDst)++ = lTemp;
			*((PU32)pDst) = lTemp;
			pDst += 4;
			pSrc += 4;
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}

}	/* H01V00 */

/*******************************************************************
* C_Interpolate4_H02V00 
*	2/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1, -5, 20, 20, -5, 1) / 32 
*******************************************************************/
void  C_Interpolate4_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32	  lTemp;
	I32	  dstRow,dstCol;    
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;	

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = pSrc[-2]; 
			lTemp1 = pSrc[-1];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[1];
			lTemp4 = pSrc[2];
			lTemp5 = pSrc[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp = ClampVal(lTemp0);			

			lTemp0 = pSrc[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3)) << 24;

			//*((PU32)pDst)++ = lTemp;
			*((PU32)pDst) = lTemp;
			pDst +=4;
			pSrc +=4;
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}

}	/* H02V00 */

/*******************************************************************
* C_Interpolate4_H03V00 
*	2/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1, -5, 20, 52, -5, 1) / 32 
******************************************************************/
void  C_Interpolate4_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32	  lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = pSrc[-2];
			lTemp1 = pSrc[-1];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[1];
			lTemp4 = pSrc[2];
			lTemp5 = pSrc[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp0 + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = pSrc[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp1 + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp2 + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp3 + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3)) << 24;

			//*((PU32)pDst)++ = lTemp;
			*((PU32)pDst) = lTemp;
			pDst +=4;
			pSrc +=4;			
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}

}	/* H03V00 */

/*********************************************************************
* C_Interpolate4_H00V01 
*	0 horizontal displacement 
*	1/3 vertical displacement 
*	Use vertical filter (1, -5, 52, 20, -5, 1) / 64 
*********************************************************************/
void  C_Interpolate4_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	/*MAP -- Process 4 pixels at a time. While doing vertical interploation  */
	/*we process along columns instead of rows so that loads can be minimised*/
	/*Decrementing loops are used for the purpose of optimization			 */

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch << 1)];
			lTemp1 = pSrc[-(I32)(uSrcPitch)];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[uSrcPitch];
			lTemp4 = pSrc[uSrcPitch<<1];
			lTemp5 = pSrc[3*uSrcPitch];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = pSrc[uSrcPitch << 2];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = pSrc[5*uSrcPitch];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = pSrc[6*uSrcPitch];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}

}	/* H00V01 */

/*******************************************************************
* C_Interpolate_H01V01 
*	1/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1) 
*	Use vertical filter (-1,5,52,20,5,-1) 
*****************************************************************/
void  C_Interpolate4_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{    
	U8  *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow,dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	/*MAP -- Process 4 pixels at a time. Temporary buffer "buff[]" is made U8*/
	/*instead of I32. Buffer is allocated as U32 to ensure that, it will be  */
	/*word alligned.*/
	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
      b +=4;
			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H01V01 */

/*********************************************************************
* C_Interpolate_H02V01 
*	2/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (-1,5,20,20,5,-1) 
*	Use vertical filter (-1,5,52,20,5,-1) 
**********************************************************************/
void  C_Interpolate4_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;

      b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/********************************************************************
* C_Interpolate_H03V01 
*	3/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (-1,5,20,52,5,-1) 
*	Use vertical filter (-1,5,52,20,5,-1) 
*********************************************************************/
void  C_Interpolate4_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp0 + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp1 + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp2 + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp3 + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3)) << 24;
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
      b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H03V01 */

/*******************************************************************
* C_Interpolate_H00V02 
*	0 horizontal displacement 
*	2/4 vertical displacement 
*	Use vertical filter (-1,5,20,20,5,-1) 
********************************************************************/
void  C_Interpolate4_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    

	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch << 1)];
			lTemp1 = pSrc[-(I32)(uSrcPitch)];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[uSrcPitch];
			lTemp4 = pSrc[uSrcPitch<<1];
			lTemp5 = pSrc[3*uSrcPitch];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = pSrc[uSrcPitch << 2];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = pSrc[5*uSrcPitch];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = (I32)pSrc[6*uSrcPitch];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}

}	/* H00V02 */

/***********************************************************************
* C_Interpolate4_H01V02 
*	1/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1) 
*	Use vertical filter (-1,5,20,20,5,-1) 
***********************************************************************/
void  C_Interpolate4_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;

			b += 4;
			p += 4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/************************************************************************
* C_Interpolate4_H02V02 
*	2/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (-1,5,20,20,5,-1) 
*	Use vertical filter (-1,5,20,20,5,-1) 
************************************************************************/
void  C_Interpolate4_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;

            b += 4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/**************************************************************************
* C_Interpolate4_H03V02 
*	3/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1) 
*	Use vertical filter (-1,5,20,20,5,-1) 
**************************************************************************/
void  C_Interpolate4_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp0 + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp1 + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp2 + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp3 + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3)) << 24;
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
			b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/******************************************************************
* C_Interpolate_H00V03 
*	0 horizontal displacement 
*	3/4 vertical displacement 
*	Use vertical filter (-1,5,20,52,5,-1) 
********************************************************************/
void  C_Interpolate4_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;

	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch << 1)];
			lTemp1 = pSrc[-(I32)(uSrcPitch)];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[uSrcPitch];
			lTemp4 = pSrc[uSrcPitch<<1];
			lTemp5 = pSrc[3*uSrcPitch];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp5 + 32) >> 6;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = pSrc[uSrcPitch << 2];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp0 + 32) >> 6;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = pSrc[5*uSrcPitch];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp1 + 32) >> 6;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = pSrc[6*uSrcPitch];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp2 + 32) >> 6;
			*pDst = ClampVal(lTemp3);

			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}

}	/* H00V02 */

/***************************************************************************
* C_Interpolate4_H01V03 
*	1/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1)
*	Use vertical filter (-1,5,20,52,5,-1) 
******************************************************************************/
void  C_Interpolate4_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8 + 5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
      b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp5 + 32) >> 6;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp0 + 32) >> 6;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp1 + 32) >> 6;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp2 + 32) >> 6;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/************************************************************************
* C_Interpolate4_H02V03 
*	2/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (-1,5,20,20,5,-1) 
*	Use vertical filter (-1,5,20,52,5,-1) 
************************************************************************/
void  C_Interpolate4_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
      b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp5 + 32) >> 6;
			*pDst = ClampVal(lTemp0);
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp0 + 32) >> 6;
			*pDst = ClampVal(lTemp1);
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp1 + 32) >> 6;
			*pDst = ClampVal(lTemp2);
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp2 + 32) >> 6;
			*pDst = ClampVal(lTemp3);
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/*************************************************************************************
* C_Interpolate4_H03V03 
*	3/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (-1,5,20,52,5,-1) 
*	Use vertical filter (-1,5,20,52,5,-1) 
**************************************************************************************/
void  C_Interpolate4_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32  lTemp;
	I32  dstRow, dstCol;
	I32  lTemp0, lTemp1;
	I32  lTemp2, lTemp3;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = pSrc[0];
			lTemp1 = pSrc[1];	
			lTemp2 = pSrc[uSrcPitch];
			lTemp3 = pSrc[uSrcPitch + 1];

			lTemp0 = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 2) >> 2;
			lTemp = ClampVal(lTemp0);

			lTemp0 = pSrc[2];
			lTemp2 = pSrc[uSrcPitch + 2];
			lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 2) >> 2;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[3];
			lTemp3 = pSrc[uSrcPitch + 3];
			lTemp0 = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 2) >> 2;
			lTemp |= (ClampVal(lTemp0)) << 16;

			lTemp0 = pSrc[4];
			lTemp2 = pSrc[uSrcPitch + 4];
			lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 2) >> 2;
			lTemp |= (ClampVal(lTemp1)) << 24;

			//*((PU32)pDst)++ = lTemp;
			*((PU32)pDst) = lTemp;
			pDst +=4;
			pSrc +=4;
		}
		pDst += (uDstPitch - 8);
		pSrc += (uSrcPitch - 8);
	}

}	/* H03V03 */

/* chroma functions */
/* Block size is 4x4 for all. */

/***********************************************************************
* C_MCCopyChroma_H00V00 
*	 0 horizontal displacement 
*	 0 vertical displacement 
*	 No interpolation required, simple block copy. 
**********************************************************************/
void  C_MCCopyChroma4_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
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

/***********************************************************************************
* C_MCCopyChroma4_H01V00 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	1/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (3,1) 
*	Dst pitch is uDstPitch. 
***********************************************************************************/
void  C_MCCopyChroma4_H01V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp  = (3*lTemp0 + lTemp1 + 1)>>2;

		lTemp0 = pRef[2];
		lTemp1 = (3*lTemp1 + lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp0 = (3*lTemp0 + lTemp1 + 1)>>2;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp1 = (3*lTemp1 + lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		pRef += uPitch;
		dd += uDstPitch;
	}
}	/* C_MCCopyChroma4_H01V00 */

/*************************************************************************
* C_MCCopyChroma4_H02V00 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	2/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1,1) 
*	Dst pitch is uDstPitch. 
*************************************************************************/
void  C_MCCopyChroma4_H02V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp  = (lTemp0 + lTemp1 + 1)>>1;

		lTemp0 = pRef[2];
		lTemp1 = (lTemp1 + lTemp0 + 1)>>1;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp0 = (lTemp0 + lTemp1 + 1)>>1;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp1 = (lTemp1 + lTemp0 + 1)>>1;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		pRef += uPitch;
		dd += uDstPitch;
	}

}	/* C_MCCopyChroma4_H02V00 */

/******************************************************************************
* C_MCCopyChroma4_H03V00 (round down) 
*	Motion compensated 4x4 chroma block copy.
*	3/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1,3) 
*	Dst pitch is uDstPitch. 
********************************************************************************/
void  C_MCCopyChroma4_H03V00(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp  = (lTemp0 + 3*lTemp1 + 1)>>2;

		lTemp0 = pRef[2];
		lTemp1 = (lTemp1 + 3*lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp0 = (lTemp0 + 3*lTemp1 + 1)>>2;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp1 = (lTemp1 + 3*lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		pRef += uPitch;
		dd += uDstPitch;
	}

}	/* C_MCCopyChroma4_H03V00 */

/*********************************************************************************
* C_MCCopyChroma4_H00V01 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	0 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
**********************************************************************************/
void  C_MCCopyChroma4_H00V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[uPitch];
		lTemp0 = (3*lTemp0 + lTemp1 + 2)>>2;
		dd[0] = (U8)lTemp0;

		lTemp0 = pRef[uPitch << 1];
		lTemp1 = (3*lTemp1 + lTemp0 + 2)>>2;
		dd[uDstPitch] = (U8)lTemp1;

		lTemp1 = pRef[3*uPitch];
		lTemp0 = (3*lTemp0 + lTemp1 + 2)>>2;
		dd[uDstPitch << 1] = (U8)lTemp0;

		lTemp0 = pRef[uPitch << 2];
		lTemp1 = (3*lTemp1 + lTemp0 + 2)>>2;
		dd[3*uDstPitch] = (U8)lTemp1;

		dd++;
		pRef++;
	}
}	/* C_MCCopyChroma4_H00V01 */

/**************************************************************************
* C_MCCopyChroma4_H00V02 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	0 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
***************************************************************************/
void  C_MCCopyChroma4_H00V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[uPitch];
		lTemp0 = (lTemp0 + lTemp1)>>1;
		dd[0] = (U8)lTemp0;

		lTemp0 = pRef[uPitch << 1];
		lTemp1 = (lTemp1 + lTemp0)>>1;
		dd[uDstPitch] = (U8)lTemp1;

		lTemp1 = pRef[3*uPitch];
		lTemp0 = (lTemp0 + lTemp1)>>1;
		dd[uDstPitch << 1] = (U8)lTemp0;

		lTemp0 = pRef[uPitch << 2];
		lTemp1 = (lTemp1 + lTemp0)>>1;
		dd[3*uDstPitch] = (U8)lTemp1;

		dd++;
		pRef++;
	}
}	/* C_MCCopyChroma4_H00V02 */

/***********************************************************************
* C_MCCopyChroma4_H00V03 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	0 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
************************************************************************/
void  C_MCCopyChroma4_H00V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[uPitch];
		lTemp0 = (lTemp0 + 3*lTemp1 + 2)>>2;
		dd[0] = (U8)lTemp0;

		lTemp0 = pRef[uPitch << 1];
		lTemp1 = (lTemp1 + 3*lTemp0 + 2)>>2;
		dd[uDstPitch] = (U8)lTemp1;

		lTemp1 = pRef[3*uPitch];
		lTemp0 = (lTemp0 + 3*lTemp1 + 2)>>2;
		dd[uDstPitch << 1] = (U8)lTemp0;

		lTemp0 = pRef[uPitch << 2];
		lTemp1 = (lTemp1 + 3*lTemp0 + 2)>>2;
		dd[3*uDstPitch] = (U8)lTemp1;

		dd++;
		pRef++;
	}
}	/* C_MCCopyChroma4_H00V03 */

/***********************************************************************
* C_MCCopyChroma4_H01V01 (round down) 
*	Motion compensated chroma 4x4 block copy. 
*	1/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (3,1) 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
***********************************************************************/
void  C_MCCopyChroma4_H01V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H01V01 */

/************************************************************************
* C_MCCopyChroma4_H02V01 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	2/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (2,2) 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
*************************************************************************/
void  C_MCCopyChroma4_H02V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (3*(lTemp0 + lTemp1) + lTemp2 + lTemp3 + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (3*(lTemp1 + lTemp0) + lTemp3 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (3*(lTemp0 + lTemp1) + lTemp2 + lTemp3 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (3*(lTemp1 + lTemp0) + lTemp3 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H02V01 */

/***************************************************************************
* C_MCCopyChroma4_H03V01 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	3/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (1,3) 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
***************************************************************************/
void  C_MCCopyChroma4_H03V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H03V01 */

/*******************************************************************************
* C_MCCopyChroma4_H01V02 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	1/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (3,1) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
*********************************************************************************/
void  C_MCCopyChroma4_H01V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H01V02 */

/****************************************************************************
* C_MCCopyChroma4_H02V02 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (1,1) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
****************************************************************************/
void  C_MCCopyChroma4_H02V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 1)>>2;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 1)>>2;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 1)>>2;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 1)>>2;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H02V02 */

/******************************************************************************
* C_MCCopyChroma4_H01V02 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	3/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (1,3) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
*********************************************************************************/
void  C_MCCopyChroma4_H03V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}

}	/* C_MCCopyChroma4_H01V02 */

/****************************************************************************
* C_MCCopyChroma4_H01V03 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	1/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (3,1) 
*	Use vertical filter (1,3) 
*	Dst pitch is uDstPitch. 
******************************************************************************/
void  C_MCCopyChroma4_H01V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H01V02 */

/**************************************************************************
* C_MCCopyChroma4_H02V03 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (3,5) 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
***************************************************************************/
void  C_MCCopyChroma4_H02V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (lTemp0 + lTemp1 + 3*(lTemp2 + lTemp3) + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (lTemp1 + lTemp0 + 3*(lTemp3 + lTemp2) + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (lTemp0 + lTemp1 + 3*(lTemp2 + lTemp3) + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (lTemp1 + lTemp0 + 3*(lTemp3 + lTemp2) + 4)>>3;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H02V03 */

/******************************************************************************
* C_MCCopyChroma4_H03V03 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	3/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (1,3) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
*******************************************************************************/
void  C_MCCopyChroma4_H03V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		((PU32)dd)[0] = lTemp;
		dd += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H03V03 */

#define AddRow4(a,c,d)			\
{								\
	U32 q;				\
	c=((U32*)pDst)[(d)];		\
	q=(a|c) & 0x01010101;		\
	q+=(a>>1) & 0x7F7F7F7F;		\
	q+=(c>>1) & 0x7F7F7F7F;		\
	((U32*)pDst)[(d)]=q;		\
}
void  C_Interpolate4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow, i;
	PU32 pTempSrc;

	U32 a,b,c;

	/* Do not perform a sequence of U32 copies, since this function  */
	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */

	/*MAP -- Loops are modified as decrementing loops for the purpose*/
	/*of ARM optimization.											 */

	if ((i = (I32)((PU8)pSrc - (PU8)0) & 0x03) == 0)
	{
		pTempSrc = (PU32)(pSrc);
		for (dstRow = 8; dstRow > 0; dstRow--)
		{
			a = pTempSrc[0];
			b = pTempSrc[1];
			AddRow4(a, c, 0);
			AddRow4(b, c, 1);

			pDst += uDstPitch;
			pTempSrc += (uSrcPitch >> 2);
		}
	}
	else
	{
		pTempSrc = (PU32)((PU8)pSrc - i);

		i = i<<3;
		for (dstRow = 8; dstRow > 0; dstRow--)
		{

			a = (pTempSrc[0] >> i)|(pTempSrc[1] << (32 - i));
			b = (pTempSrc[1] >> i)|(pTempSrc[2] << (32 - i));

			AddRow4(a, c, 0);
			AddRow4(b, c, 1);

			pDst += uDstPitch;
			pTempSrc += (uSrcPitch >> 2);
		}
	}
	
}	/* H00V00 */

/******************************************************************
* C_Interpolate4_H01V00 
*	1/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1, -5, 52, 20, -5, 1) / 64 
******************************************************************/
void  C_Interpolate4Add_H01V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32	  lTemp, c;	 
	I32   dstRow,dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;	

	/*MAP -- Process 4 pixels at a time. Pixel values pSrc[x] are taken into*/
	/*temporary variables lTempX, so that number of loads can be minimized. */
	/*Decrementing loops are used for the purpose of optimization			*/

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 0; dstCol <2; dstCol ++)
		{
			lTemp0 = pSrc[-2]; 
			lTemp1 = pSrc[-1];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[1];
			lTemp4 = pSrc[2];
			lTemp5 = pSrc[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = pSrc[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = pSrc[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = pSrc[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			
			AddRow4(lTemp, c, dstCol);

			pSrc +=4;
		}
		pDst += uDstPitch;
		pSrc += (uSrcPitch - 8);
	}

}	/* H01V00 */

/*******************************************************************
* C_Interpolate4_H02V00 
*	2/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1, -5, 20, 20, -5, 1) / 32 
*******************************************************************/
void  C_Interpolate4Add_H02V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32	  lTemp,c;
	I32	  dstRow,dstCol;    
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;	

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 0; dstCol < 2; dstCol++)
		{
			lTemp0 = pSrc[-2]; 
			lTemp1 = pSrc[-1];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[1];
			lTemp4 = pSrc[2];
			lTemp5 = pSrc[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp = ClampVal(lTemp0);			

			lTemp0 = pSrc[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3)) << 24;

			AddRow4(lTemp, c, dstCol);
			pSrc +=4;
		}
		pDst += uDstPitch;
		pSrc += (uSrcPitch - 8);
	}

}	/* H02V00 */

/*******************************************************************
* C_Interpolate4_H03V00 
*	2/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1, -5, 20, 52, -5, 1) / 32 
******************************************************************/
void  C_Interpolate4Add_H03V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32	  lTemp,c;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 0; dstCol < 2; dstCol ++)
		{
			lTemp0 = pSrc[-2];
			lTemp1 = pSrc[-1];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[1];
			lTemp4 = pSrc[2];
			lTemp5 = pSrc[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp0 + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = pSrc[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp1 + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp2 + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = pSrc[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp3 + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3)) << 24;

			AddRow4(lTemp, c, dstCol);
			pSrc +=4;			
		}
		pDst += uDstPitch;
		pSrc += (uSrcPitch - 8);
	}

}	/* H03V00 */

/*********************************************************************
* C_Interpolate4_H00V01 
*	0 horizontal displacement 
*	1/3 vertical displacement 
*	Use vertical filter (1, -5, 52, 20, -5, 1) / 64 
*********************************************************************/
void  C_Interpolate4Add_H00V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	/*MAP -- Process 4 pixels at a time. While doing vertical interploation  */
	/*we process along columns instead of rows so that loads can be minimised*/
	/*Decrementing loops are used for the purpose of optimization			 */

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch << 1)];
			lTemp1 = pSrc[-(I32)(uSrcPitch)];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[uSrcPitch];
			lTemp4 = pSrc[uSrcPitch<<1];
			lTemp5 = pSrc[3*uSrcPitch];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = pSrc[uSrcPitch << 2];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = (ClampVal(lTemp1)+ (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = pSrc[5*uSrcPitch];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = (ClampVal(lTemp2)+ (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = pSrc[6*uSrcPitch];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = (ClampVal(lTemp3)+ (*pDst) + 1)>>1;
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}

}	/* H00V01 */

/*******************************************************************
* C_Interpolate_H01V01 
*	1/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1) 
*	Use vertical filter (-1,5,52,20,5,-1) 
*****************************************************************/
void  C_Interpolate4Add_H01V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{    
	U8  *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow,dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	/*MAP -- Process 4 pixels at a time. Temporary buffer "buff[]" is made U8*/
	/*instead of I32. Buffer is allocated as U32 to ensure that, it will be  */
	/*word alligned.*/
	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
			
			b += 4;
			p +=4;			
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H01V01 */

/*********************************************************************
* C_Interpolate_H02V01 
*	2/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (-1,5,20,20,5,-1) 
*	Use vertical filter (-1,5,52,20,5,-1) 
**********************************************************************/
void  C_Interpolate4Add_H02V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
			
			b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/********************************************************************
* C_Interpolate_H03V01 
*	3/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (-1,5,20,52,5,-1) 
*	Use vertical filter (-1,5,52,20,5,-1) 
*********************************************************************/
void  C_Interpolate4Add_H03V01(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp0 + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp1 + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp2 + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp3 + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3)) << 24;
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
			
			b += 4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + (lTemp2 << 5) + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 32) >> 6;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + (lTemp3 << 5) + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 32) >> 6;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + (lTemp4 << 5) + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 32) >> 6;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + (lTemp5 << 5) + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 32) >> 6;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H03V01 */

/*******************************************************************
* C_Interpolate_H00V02 
*	0 horizontal displacement 
*	2/4 vertical displacement 
*	Use vertical filter (-1,5,20,20,5,-1) 
********************************************************************/
void  C_Interpolate4Add_H00V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;    

	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch << 1)];
			lTemp1 = pSrc[-(I32)(uSrcPitch)];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[uSrcPitch];
			lTemp4 = pSrc[uSrcPitch<<1];
			lTemp5 = pSrc[3*uSrcPitch];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = pSrc[uSrcPitch << 2];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = pSrc[5*uSrcPitch];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = (I32)pSrc[6*uSrcPitch];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}

}	/* H00V02 */

/***********************************************************************
* C_Interpolate4_H01V02 
*	1/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1) 
*	Use vertical filter (-1,5,20,20,5,-1) 
***********************************************************************/
void  C_Interpolate4Add_H01V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol >0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
			
			b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/************************************************************************
* C_Interpolate4_H02V02 
*	2/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (-1,5,20,20,5,-1) 
*	Use vertical filter (-1,5,20,20,5,-1) 
************************************************************************/
void  C_Interpolate4Add_H02V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
			
			b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/**************************************************************************
* C_Interpolate4_H03V02 
*	3/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1) 
*	Use vertical filter (-1,5,20,20,5,-1) 
**************************************************************************/
void  C_Interpolate4Add_H03V02(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp0 + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp1 + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp2 + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2)) << 16;

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp3 + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3)) << 24;
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
			
			b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp5 + 16) >> 5;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp0 + 16) >> 5;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp1 + 16) >> 5;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp2 + 16) >> 5;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/******************************************************************
* C_Interpolate_H00V03 
*	0 horizontal displacement 
*	3/4 vertical displacement 
*	Use vertical filter (-1,5,20,52,5,-1) 
********************************************************************/
void  C_Interpolate4Add_H00V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	I32 dstCol;

	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -= 4)
		{
			lTemp0 = pSrc[-(I32)(uSrcPitch << 1)];
			lTemp1 = pSrc[-(I32)(uSrcPitch)];
			lTemp2 = pSrc[0];
			lTemp3 = pSrc[uSrcPitch];
			lTemp4 = pSrc[uSrcPitch<<1];
			lTemp5 = pSrc[3*uSrcPitch];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp5 + 32) >> 6;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = pSrc[uSrcPitch << 2];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp0 + 32) >> 6;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = pSrc[5*uSrcPitch];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp1 + 32) >> 6;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = pSrc[6*uSrcPitch];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp2 + 32) >> 6;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;

			pDst += uDstPitch;

			pSrc += (uSrcPitch << 2);
		}
		pDst -= ((uDstPitch * 8) - 1);
		pSrc -= ((uSrcPitch * 8) - 1);
	}

}	/* H00V02 */

/***************************************************************************
* C_Interpolate4_H01V03 
*	1/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (-1,5,52,20,5,-1)
*	Use vertical filter (-1,5,20,52,5,-1) 
******************************************************************************/
void  C_Interpolate4Add_H01V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + (lTemp2 << 5) + lTemp5 + 32) >> 6;
			lTemp = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + (lTemp3 << 5) + lTemp0 + 32) >> 6;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + (lTemp4 << 5) + lTemp1 + 32) >> 6;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + (lTemp5 << 5) + lTemp2 + 32) >> 6;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = lTemp;
			*((PU32)b) = lTemp;
      b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp5 + 32) >> 6;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp0 + 32) >> 6;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp1 + 32) >> 6;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp2 + 32) >> 6;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/************************************************************************
* C_Interpolate4_H02V03 
*	2/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (-1,5,20,20,5,-1) 
*	Use vertical filter (-1,5,20,52,5,-1) 
************************************************************************/
void  C_Interpolate4Add_H02V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U8    *b;
	const U8 *p;
	U32   buff[4*21];/*U8 buff[16*21]*/
	U32   lTemp;
	I32   dstRow, dstCol;
	I32   lTemp0, lTemp1, lTemp2;
	I32   lTemp3, lTemp4, lTemp5;

	b = (PU8)buff;
	p = pSrc - (I32)(uSrcPitch << 1);

	for (dstRow = 8+5; dstRow > 0; dstRow--)
	{
		for (dstCol = 8; dstCol > 0; dstCol -=4)
		{
			lTemp0 = p[-2];
			lTemp1 = p[-1];
			lTemp2 = p[0];
			lTemp3 = p[1];
			lTemp4 = p[2];
			lTemp5 = p[3];

			lTemp0 = (5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + lTemp0 + lTemp5 + 16) >> 5;
			lTemp  = ClampVal(lTemp0);

			lTemp0 = p[4];
			lTemp1 = (5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + lTemp1 + lTemp0 + 16) >> 5;
			lTemp |= (ClampVal(lTemp1) << 8);

			lTemp1 = p[5];
			lTemp2 = (5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + lTemp2 + lTemp1 + 16) >> 5;
			lTemp |= (ClampVal(lTemp2) << 16);

			lTemp2 = p[6];
			lTemp3 = (5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + lTemp3 + lTemp2 + 16) >> 5;
			lTemp |= (ClampVal(lTemp3) << 24);
			//*((PU32)b)++ = (U32)lTemp;
			*((PU32)b) = lTemp;
			
			b +=4;
			p +=4;
		}
		b += (16 - 8);
		p += (uSrcPitch - 8);
	}

	b = (PU8)buff;
	for (dstCol = 8; dstCol > 0; dstCol--)
	{
		for (dstRow = 8; dstRow > 0; dstRow -=4)
		{
			lTemp0 = b[0];
			lTemp1 = b[16];
			lTemp2 = b[32];
			lTemp3 = b[48];
			lTemp4 = b[64];
			lTemp5 = b[80];

			lTemp0 = (lTemp0 + 5*(((lTemp2 + lTemp3) << 2) - (lTemp1 + lTemp4)) + (lTemp3 << 5) + lTemp5 + 32) >> 6;
			*pDst = (ClampVal(lTemp0) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp0 = b[96];
			lTemp1 = (lTemp1 + 5*(((lTemp3 + lTemp4) << 2) - (lTemp2 + lTemp5)) + (lTemp4 << 5) + lTemp0 + 32) >> 6;
			*pDst = (ClampVal(lTemp1) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp1 = b[112];
			lTemp2 = (lTemp2 + 5*(((lTemp4 + lTemp5) << 2) - (lTemp3 + lTemp0)) + (lTemp5 << 5) + lTemp1 + 32) >> 6;
			*pDst = (ClampVal(lTemp2) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			lTemp2 = b[128];
			lTemp3 = (lTemp3 + 5*(((lTemp5 + lTemp0) << 2) - (lTemp4 + lTemp1)) + (lTemp0 << 5) + lTemp2 + 32) >> 6;
			*pDst = (ClampVal(lTemp3) + (*pDst) + 1)>>1;
			pDst += uDstPitch;

			b += 64;
		}
		pDst -= ((uDstPitch * 8) - 1);
		b -= ((8 << 4) - 1);
	}

}	/* H02V01 */

/*************************************************************************************
* C_Interpolate4_H03V03 
*	3/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (-1,5,20,52,5,-1) 
*	Use vertical filter (-1,5,20,52,5,-1) 
**************************************************************************************/
void  C_Interpolate4Add_H03V03(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	U32  lTemp,c;
	I32  dstRow, dstCol;
	I32  lTemp0, lTemp1;
	I32  lTemp2, lTemp3;

	for (dstRow = 8; dstRow > 0; dstRow--)
	{
		for (dstCol = 0; dstCol < 2; dstCol++)
		{
			lTemp0 = pSrc[0];
			lTemp1 = pSrc[1];	
			lTemp2 = pSrc[uSrcPitch];
			lTemp3 = pSrc[uSrcPitch + 1];

			lTemp0 = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 2) >> 2;
			lTemp = ClampVal(lTemp0);

			lTemp0 = pSrc[2];
			lTemp2 = pSrc[uSrcPitch + 2];
			lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 2) >> 2;
			lTemp |= (ClampVal(lTemp1)) << 8;

			lTemp1 = pSrc[3];
			lTemp3 = pSrc[uSrcPitch + 3];
			lTemp0 = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 2) >> 2;
			lTemp |= (ClampVal(lTemp0)) << 16;

			lTemp0 = pSrc[4];
			lTemp2 = pSrc[uSrcPitch + 4];
			lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 2) >> 2;
			lTemp |= (ClampVal(lTemp1)) << 24;

			AddRow4(lTemp, c, dstCol);
			pSrc +=4;
		}
		pDst += uDstPitch;
		pSrc += (uSrcPitch - 8);
	}

}	/* H03V03 */

/* chroma functions */
/* Block size is 4x4 for all. */

/***********************************************************************
* C_MCCopyChroma_H00V00 
*	 0 horizontal displacement 
*	 0 vertical displacement 
*	 No interpolation required, simple block copy. 
**********************************************************************/
void  C_MCCopyChroma4Add_H00V00(const U8 *pSrc, U8 *pDst, U32 uSrcPitch, U32 uDstPitch)
{
	I32 dstRow;
	U32 a,c;
	/* Do not perform a sequence of U32 copies, since this function */
	/* is used in contexts where pSrc or pDst is not 4-byte aligned. */

	for (dstRow = 4; dstRow > 0; dstRow--)
	{
		a=pSrc[0]|(pSrc[1]<<8)|((pSrc[2]|(pSrc[3]<<8))<<16); \
		AddRow4(a, c, 0);	

		pDst += uDstPitch;
		pSrc += uSrcPitch;
	}
}	/* C_MCCopyChroma_H00V00 */

/***********************************************************************************
* C_MCCopyChroma4_H01V00 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	1/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (3,1) 
*	Dst pitch is uDstPitch. 
***********************************************************************************/
void  C_MCCopyChroma4Add_H01V00(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp  = (3*lTemp0 + lTemp1 + 1)>>2;

		lTemp0 = pRef[2];
		lTemp1 = (3*lTemp1 + lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp0 = (3*lTemp0 + lTemp1 + 1)>>2;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp1 = (3*lTemp1 + lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);

		pRef += uPitch;
		pDst += uDstPitch;
	}
}	/* C_MCCopyChroma4_H01V00 */

/*************************************************************************
* C_MCCopyChroma4_H02V00 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	2/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1,1) 
*	Dst pitch is uDstPitch. 
*************************************************************************/
void  C_MCCopyChroma4Add_H02V00(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp  = (lTemp0 + lTemp1 + 1)>>1;

		lTemp0 = pRef[2];
		lTemp1 = (lTemp1 + lTemp0 + 1)>>1;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp0 = (lTemp0 + lTemp1 + 1)>>1;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp1 = (lTemp1 + lTemp0 + 1)>>1;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pRef += uPitch;
		pDst += uDstPitch;
	}

}	/* C_MCCopyChroma4_H02V00 */

/******************************************************************************
* C_MCCopyChroma4_H03V00 (round down) 
*	Motion compensated 4x4 chroma block copy.
*	3/4 pel horizontal displacement 
*	0 vertical displacement 
*	Use horizontal filter (1,3) 
*	Dst pitch is uDstPitch. 
********************************************************************************/
void  C_MCCopyChroma4Add_H03V00(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp, lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp  = (lTemp0 + 3*lTemp1 + 1)>>2;

		lTemp0 = pRef[2];
		lTemp1 = (lTemp1 + 3*lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp0 = (lTemp0 + 3*lTemp1 + 1)>>2;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp1 = (lTemp1 + 3*lTemp0 + 1)>>2;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pRef += uPitch;
		pDst += uDstPitch;
	}

}	/* C_MCCopyChroma4_H03V00 */

/*********************************************************************************
* C_MCCopyChroma4_H00V01 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	0 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
**********************************************************************************/
void  C_MCCopyChroma4Add_H00V01(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[uPitch];
		lTemp0 = (3*lTemp0 + lTemp1 + 2)>>2;
		dd[0] = ((U8)lTemp0 + dd[0]+1)>>1;

		lTemp0 = pRef[uPitch << 1];
		lTemp1 = (3*lTemp1 + lTemp0 + 2)>>2;
		dd[uDstPitch] = ((U8)lTemp1 + dd[uDstPitch] + 1)>>1;

		lTemp1 = pRef[3*uPitch];
		lTemp0 = (3*lTemp0 + lTemp1 + 2)>>2;
		dd[uDstPitch << 1] = ((U8)lTemp0 + dd[uDstPitch << 1] + 1)>>1;

		lTemp0 = pRef[uPitch << 2];
		lTemp1 = (3*lTemp1 + lTemp0 + 2)>>2;
		dd[3*uDstPitch] = ((U8)lTemp1 + dd[3*uDstPitch] + 1)>>1;

		dd++;
		pRef++;
	}
}	/* C_MCCopyChroma4_H00V01 */

/**************************************************************************
* C_MCCopyChroma4_H00V02 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	0 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
***************************************************************************/
void  C_MCCopyChroma4Add_H00V02(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[uPitch];
		lTemp0 = (lTemp0 + lTemp1)>>1;
		dd[0] = ((U8)lTemp0 + dd[0] + 1)>>1;

		lTemp0 = pRef[uPitch << 1];
		lTemp1 = (lTemp1 + lTemp0)>>1;
		dd[uDstPitch] = ((U8)lTemp1 + dd[uDstPitch] + 1)>>1;

		lTemp1 = pRef[3*uPitch];
		lTemp0 = (lTemp0 + lTemp1)>>1;
		dd[uDstPitch << 1] = ((U8)lTemp0 + dd[uDstPitch << 1] + 1)>>1;

		lTemp0 = pRef[uPitch << 2];
		lTemp1 = (lTemp1 + lTemp0)>>1;
		dd[3*uDstPitch] = ((U8)lTemp1+dd[3*uDstPitch]+1)>>1;

		dd++;
		pRef++;
	}
}	/* C_MCCopyChroma4_H00V02 */

/***********************************************************************
* C_MCCopyChroma4_H00V03 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	0 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
************************************************************************/
void  C_MCCopyChroma4Add_H00V03(const U8 *pRef, U8 *dd, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp0, lTemp1;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[uPitch];
		lTemp0 = (lTemp0 + 3*lTemp1 + 2)>>2;
		dd[0] = ((U8)lTemp0 + dd[0] + 1)>>1;

		lTemp0 = pRef[uPitch << 1];
		lTemp1 = (lTemp1 + 3*lTemp0 + 2)>>2;
		dd[uDstPitch] = ((U8)lTemp1 + dd[uDstPitch] + 1)>>1;

		lTemp1 = pRef[3*uPitch];
		lTemp0 = (lTemp0 + 3*lTemp1 + 2)>>2;
		dd[uDstPitch << 1] = ((U8)lTemp0 + dd[uDstPitch << 1] + 1)>>1;

		lTemp0 = pRef[uPitch << 2];
		lTemp1 = (lTemp1 + 3*lTemp0 + 2)>>2;
		dd[3*uDstPitch] = ((U8)lTemp1 + dd[3*uDstPitch] + 1)>>1;

		dd++;
		pRef++;
	}
}	/* C_MCCopyChroma4_H00V03 */

/***********************************************************************
* C_MCCopyChroma4_H01V01 (round down) 
*	Motion compensated chroma 4x4 block copy. 
*	1/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (3,1) 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
***********************************************************************/
void  C_MCCopyChroma4Add_H01V01(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H01V01 */

/************************************************************************
* C_MCCopyChroma4_H02V01 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	2/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (2,2) 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
*************************************************************************/
void  C_MCCopyChroma4Add_H02V01(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;	
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (3*(lTemp0 + lTemp1) + lTemp2 + lTemp3 + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (3*(lTemp1 + lTemp0) + lTemp3 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (3*(lTemp0 + lTemp1) + lTemp2 + lTemp3 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (3*(lTemp1 + lTemp0) + lTemp3 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H02V01 */

/***************************************************************************
* C_MCCopyChroma4_H03V01 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	3/4 pel horizontal displacement 
*	1/4 vertical displacement 
*	Use horizontal filter (1,3) 
*	Use vertical filter (3,1) 
*	Dst pitch is uDstPitch. 
***************************************************************************/
void  C_MCCopyChroma4Add_H03V01(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp1 + 3*(lTemp0 + lTemp3) + lTemp2 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp0 + 3*(lTemp1 + lTemp2) + lTemp3 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H03V01 */

/*******************************************************************************
* C_MCCopyChroma4_H01V02 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	1/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (3,1) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
*********************************************************************************/
void  C_MCCopyChroma4Add_H01V02(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H01V02 */

/****************************************************************************
* C_MCCopyChroma4_H02V02 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (1,1) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
****************************************************************************/
void  C_MCCopyChroma4Add_H02V02(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 1)>>2;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 1)>>2;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (lTemp0 + lTemp1 + lTemp2 + lTemp3 + 1)>>2;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (lTemp1 + lTemp0 + lTemp3 + lTemp2 + 1)>>2;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H02V02 */

/******************************************************************************
* C_MCCopyChroma4_H01V02 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	3/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (1,3) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
*********************************************************************************/
void  C_MCCopyChroma4Add_H03V02(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (3*(lTemp1 + lTemp3) + lTemp0 + lTemp2 + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (3*(lTemp0 + lTemp2) + lTemp1 + lTemp3 + 4)>>3;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}

}	/* C_MCCopyChroma4_H01V02 */

/****************************************************************************
* C_MCCopyChroma4_H01V03 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	1/4 pel horizontal displacement 
*	3/4 vertical displacement 
*	Use horizontal filter (3,1) 
*	Use vertical filter (1,3) 
*	Dst pitch is uDstPitch. 
******************************************************************************/
void  C_MCCopyChroma4Add_H01V03(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H01V02 */

/**************************************************************************
* C_MCCopyChroma4_H02V03 (round up) 
*	Motion compensated 4x4 chroma block copy. 
*	2/3 pel horizontal displacement 
*	2/3 vertical displacement 
*	Use horizontal filter (3,5) 
*	Use vertical filter (3,5) 
*	Dst pitch is uDstPitch. 
***************************************************************************/
void  C_MCCopyChroma4Add_H02V03(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (lTemp0 + lTemp1 + 3*(lTemp2 + lTemp3) + 4)>>3;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (lTemp1 + lTemp0 + 3*(lTemp3 + lTemp2) + 4)>>3;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (lTemp0 + lTemp1 + 3*(lTemp2 + lTemp3) + 4)>>3;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (lTemp1 + lTemp0 + 3*(lTemp3 + lTemp2) + 4)>>3;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H02V03 */

/******************************************************************************
* C_MCCopyChroma4_H03V03 (round down) 
*	Motion compensated 4x4 chroma block copy. 
*	3/4 pel horizontal displacement 
*	2/4 vertical displacement 
*	Use horizontal filter (1,3) 
*	Use vertical filter (1,1) 
*	Dst pitch is uDstPitch. 
*******************************************************************************/
void  C_MCCopyChroma4Add_H03V03(const U8 *pRef, U8 *pDst, U32 uPitch, U32 uDstPitch)
{
	I32 j;
	U32 lTemp;
	U32 lTemp0, lTemp1;
	U32 lTemp2, lTemp3;
	const U8 *pRef2 = pRef + uPitch;

	for (j = 4; j > 0; j--)
	{
		lTemp0 = pRef[0];
		lTemp1 = pRef[1];
		lTemp2 = pRef2[0];
		lTemp3 = pRef2[1];
		lTemp  = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;

		lTemp0 = pRef[2];
		lTemp2 = pRef2[2];
		lTemp1 = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;
		lTemp |= (lTemp1 << 8);

		lTemp1 = pRef[3];
		lTemp3 = pRef2[3];
		lTemp0 = (9*lTemp3 + 3*(lTemp2 + lTemp1) + lTemp0 + 7)>>4;
		lTemp |= (lTemp0 << 16);

		lTemp0 = pRef[4];
		lTemp2 = pRef2[4];
		lTemp1 = (9*lTemp2 + 3*(lTemp3 + lTemp0) + lTemp1 + 7)>>4;
		lTemp |= (lTemp1 << 24);

		AddRow4(lTemp, lTemp0, 0);
		pDst += uDstPitch;
		pRef += uPitch;
		pRef2 += uPitch;
	}
}	/* C_MCCopyChroma4_H03V03 */



