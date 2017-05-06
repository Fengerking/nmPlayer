/************************************************* BEGIN LICENSE BLOCK ************************************************ 
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
******************************************************* END LICENSE BLOCK ********************************************/ 
/*****************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information.
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
****************************************************************************************
/Inverse transform interface. 
****************************************************************************************
*    INTEL Corporation Proprietary Information *
*    This listing is supplied under the terms of a license *
*    agreement with INTEL Corporation and may not be copied *
*    nor disclosed except in accordance with the terms of *
*    that agreement. *
*    Copyright (c) 1999-2000 Intel Corporation. *
*    All Rights Reserved. *
****************************************************************************************
************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/

/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/ixfrm.h,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */

#ifndef IDCT_H__
#define IDCT_H__

#include "rvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

	/* Inverse 4x4 transform function type declarations */
void  C_Transform4x4_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  C_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  C_Intra16x16ITransform4x4(I32 *pQuantBuf);
void  ARMV6_Transform4x4_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  ARMV6_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  ARMV6_Intra16x16ITransform4x4(I32 *pQuantBuf); 
void  ARMV7_Transform4x4_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  ARMV7_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  ARMV7_Intra16x16ITransform4x4(I32 *pQuantBuf); 
void  WMMX2_Transform4x4_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  WMMX2_ITransform4x4_DCOnly_Add(I32 *pQuantBuf, U8 *pDest, U32 uDestStride, const U8 *pPredSrc, U32 uPredPitch);
void  WMMX2_Intra16x16ITransform4x4(I32 *pQuantBuf);

#if defined(VOARMV4)
#elif defined(VOARMV6)
#define Transform4x4_Add			ARMV6_Transform4x4_Add
#define Transform4x4_DCOnly_Add		ARMV6_ITransform4x4_DCOnly_Add	
#define Intra16x16ITransform4x4		ARMV6_Intra16x16ITransform4x4


#elif defined(VOARMV7)
#define Transform4x4_Add			ARMV7_Transform4x4_Add
#define Transform4x4_DCOnly_Add		ARMV7_ITransform4x4_DCOnly_Add			
#define Intra16x16ITransform4x4		ARMV7_Intra16x16ITransform4x4
#elif defined(VOWMMX2)
#define Transform4x4_Add			WMMX2_Transform4x4_Add
#define Transform4x4_DCOnly_Add		WMMX2_ITransform4x4_DCOnly_Add			
#define Intra16x16ITransform4x4		WMMX2_Intra16x16ITransform4x4
#else
#define Transform4x4_Add			C_Transform4x4_Add
#define Transform4x4_DCOnly_Add		C_ITransform4x4_DCOnly_Add			
#define Intra16x16ITransform4x4		C_Intra16x16ITransform4x4
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IDCT_H__ */
