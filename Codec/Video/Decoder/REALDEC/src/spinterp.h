/************************************************ BEGIN LICENSE BLOCK ********************************************************** 
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
***************************************************** END LICENSE BLOCK **************************************************************** 
*****************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved.
*    Do not redistribute. 
****************************************************************************************
*    Interpolation function interface. *
****************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 2000 Intel Corporation.
*    All Rights Reserved.
******************************************************************************************
************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/spinterp.h,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */
#ifndef SPINTERP_H__
#define SPINTERP_H__

#include "rvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Subpel interpolation function pointer */

typedef void (T_InterpFnxNew)(
	const U8 *pSrc,
	U8 *pDst,
	U32 uSrcPitch,
	U32 uDstPitch
);


typedef T_InterpFnxNew * T_InterpFnxTableNewPtr;

extern const T_InterpFnxTableNewPtr InterpolateLuma_RV9[16];
extern const T_InterpFnxTableNewPtr InterpolateChroma_RV9[16];
extern const T_InterpFnxTableNewPtr InterpolateAddLuma_RV9[16];
extern const T_InterpFnxTableNewPtr InterpolateAddChroma_RV9[16];

extern const T_InterpFnxTableNewPtr InterpolateLuma_RV8[16];
extern const T_InterpFnxTableNewPtr InterpolateChroma_RV8[16];
extern const T_InterpFnxTableNewPtr InterpolateAddLuma_RV8[16];
extern const T_InterpFnxTableNewPtr InterpolateAddChroma_RV8[16];

#if defined(VOWMMX2)
void  WMMX2_CopyBlock8x8(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  WMMX2_CopyBlock16x16(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  WMMX2_AddBlock8x8(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);
void  WMMX2_AddBlock16x16(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);

#define CopyBlock8x8        WMMX2_CopyBlock8x8
#define CopyBlock16x16      WMMX2_CopyBlock16x16
#define AddBlock8x8         WMMX2_AddBlock8x8
#define AddBlock16x16       WMMX2_AddBlock16x16
#elif defined(VOARMV6)
void  ARMV6_CopyBlock8x8(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  ARMV6_CopyBlock16x16(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  ARMV6_AddBlock8x8(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);
void  ARMV6_AddBlock16x16(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);

#define CopyBlock8x8        ARMV6_CopyBlock8x8
#define CopyBlock16x16      ARMV6_CopyBlock16x16
#define AddBlock8x8         ARMV6_AddBlock8x8
#define AddBlock16x16       ARMV6_AddBlock16x16
#elif defined(VOARMV7)
void  CopyBlock8x8_Ctx(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  CopyBlock16x16_Ctx(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  AddBlock8x8_Ctx(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);
void  AddBlock16x16_Ctx(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);

#define CopyBlock8x8        CopyBlock8x8_Ctx
#define CopyBlock16x16      CopyBlock16x16_Ctx
#define AddBlock8x8         AddBlock8x8_Ctx
#define AddBlock16x16       AddBlock16x16_Ctx
#else
void  CopyBlock8x8_C(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  CopyBlock16x16_C(const U8 *Src, U8 *Dst,  U32 SrcPitch, U32 DstPitch);
void  AddBlock8x8_C(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);
void  AddBlock16x16_C(const U8 *Src, U8 * Src1, U8 *Dst,  U32 Pitch);

#define CopyBlock8x8        CopyBlock8x8_C
#define CopyBlock16x16      CopyBlock16x16_C
#define AddBlock8x8         AddBlock8x8_C
#define AddBlock16x16       AddBlock16x16_C
#endif

#define  gc_Interpolate InterpolateLuma_RV9 

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* SPINTERP_H__ */
