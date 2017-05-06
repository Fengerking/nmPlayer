/*********************************************************** BEGIN LICENSE BLOCK ***************************************** 
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
********************************************************* END LICENSE BLOCK *************************************************************/ 
/*********************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 2001 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
*    Inloop Deblocking Filter interface. 
********************************************************************************************
********************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1999 - 2000 Intel Corporation. 
*    All Rights Reserved. 
*    DESCRIPTION: 
*	  Header file corresponding to RV89Combo inloop 4x4 deblocking filter  
*		class implementation. 
***********************************************************************************************/
/************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
#ifndef RECON_H__
#define RECON_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_AI_MODES 9
I32 InterpolateMB(struct Decoder *t, RV89_DEC_LOCAL  *rv89_local,I32 uPitch,	
					struct DecoderPackedMotionVector *fmv,
					struct DecoderPackedMotionVector *pmv,
					PU8 pPrev8x8,
					RV_Boolean	no_subpel,
					RV_Boolean	isRV8,
					I32 slice_index);

I32 InterpolateMBWeight(struct Decoder *t, RV89_DEC_LOCAL  *rv89_local,I32 uPitch,	
						struct DecoderPackedMotionVector *fmv,
						struct DecoderPackedMotionVector *pmv,
						PU8 pPrev8x8,RV_Boolean	no_subpel,
						RV_Boolean	isRV8,I32 iRatio0,I32 iRatio1, I32 slice_index);

RV_Status ReconIntraMB(struct Decoder *t,const struct DecoderMBInfo*,const RV89_DEC_LOCAL *rv89_local, I32 slice_index);
RV_Status ReconIntraMB16x16(struct Decoder *t, const struct DecoderMBInfo *pMB,const RV89_DEC_LOCAL *rv89_local, I32 slice_index);
RV_Status Decoder_transferMB(struct Decoder *t,const struct DecoderMBInfo *pMB,const RV89_DEC_LOCAL *rv89_local, I32 slice_index);
void IdctAddInterMB(struct Decoder *t, struct DecoderMBInfo  *pMB, RV89_DEC_LOCAL *rv89_local, I32 slice_index);

#if defined(VOWMMX2)
void WMMX2_InterpolateDirectLuma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);
void WMMX2_InterpolateDirectChroma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);

#define InterpolateDirectLuma		WMMX2_InterpolateDirectLuma
#define InterpolateDirectChroma		WMMX2_InterpolateDirectChroma
#elif defined(VOARMV7)
void InterpolateDirectLuma_Ctx(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);
void InterpolateDirectChroma_Ctx(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);
#define InterpolateDirectLuma		InterpolateDirectLuma_Ctx
#define InterpolateDirectChroma     InterpolateDirectChroma_Ctx
/*void C_InterpolateDirectLuma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);
void C_InterpolateDirectChroma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);
#define InterpolateDirectLuma		C_InterpolateDirectLuma
#define InterpolateDirectChroma		C_InterpolateDirectChroma			
*/
#else
void C_InterpolateDirectLuma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);
void C_InterpolateDirectChroma(U8 *pPrev,U8 *pFutr,I32 uPitch,U8 *dst, U32 uDstPitch, I32 iRatio0, I32 iRatio1);

#define InterpolateDirectLuma		C_InterpolateDirectLuma
#define InterpolateDirectChroma		C_InterpolateDirectChroma			
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
#endif	/* RECON_H__ */
