/*********************************************** BEGIN LICENSE BLOCK ************************************************** 
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
**************************************************** END LICENSE BLOCK ***************************************************
*********************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
*********************************************************************************************

*    Various tables. *

*******************************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1995 - 1999 Intel Corporation.
*    All Rights Reserved.
******************************************************************************************
************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/tables.h,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */

/* This file contains all table definitions used for the new algo. */
/* Implementation in ttables.cpp */

#ifndef TTABLES_H__
#define TTABLES_H__

#include "rvtypes.h"


#ifdef __cplusplus
extern "C" {
#endif
	/* Mapping from luma QP to chroma QP */
	extern const U8 chroma_quant[32];
	extern const U8 chroma_quant_DC[32];

	/* Mapping for QP applied to luma DC coefs */
	extern const U8 luma_intra_quant_DC[32];
	extern const U8 luma_quant_DC_RV8[32];
	extern const U8 luma_inter_quant_DC[32];

	/* Legacy Intra prediction probabilities */
	extern const U8 dec_iprob_rv8[84];
	extern const U8 dec_aic_prob_rv8[450];

	/* Quantization tables */
	extern const U32 BquantTable[32];

	/* Translation from block number in chroma loop to */
	/* actual chroma block number */
	extern const U8 block_trans[8];

	/* RD multiplier (Lagrange factor) */
	extern const U8 rd_quant[32];
	extern const U8 rd_quant2[32];
	extern const int QP2QUANT[32];

#define  COMPLIANCE_CLIP

#ifdef COMPLIANCE_CLIP
#define CLAMP_BIAS  (128+128+32) /* Bias in clamping table */
#else
#define CLAMP_BIAS  128 /* Bias in clamping table */
#endif
#define CLIP_RANGE  (CLAMP_BIAS + 256 + CLAMP_BIAS)

	extern const U8  ClampTbl[CLIP_RANGE];

#define ClampVal(x)  (ClampTbl[CLAMP_BIAS + (x)])

#define ClampTblLookup(x, y) ClampVal((x) + (y))

	extern const U8  aic_top_vlc[16]; /* Flawfinder: ignore */
	extern const U16 aic_table_index[20];
	extern const U16 aic_2mode_vlc[1620];
	extern const U8  aic_1mode_vlc[900]; /* Flawfinder: ignore */

	//nav
#if defined(_WIN32) && defined(_M_IX86) && !defined(__CW32__) && !defined(__WINS__)

#define GET_VLC_LENGTH_INFO_SHORT(info,length)\
	_asm mov eax, (info)					\
	_asm bsr edx, eax						\
	_asm btr eax, edx						\
	_asm mov (info), eax					\
	_asm mov (length), edx

#define GET_VLC_LENGTH_INFO_CHAR(info,length)\
	_asm mov eax, (info)					\
	_asm bsr edx, eax						\
	_asm btr eax, edx						\
	_asm mov (info), eax					\
	_asm mov (length), edx

#else

#define GET_VLC_LENGTH_INFO_SHORT(info,length)		\
	(length) =						\
	(info < 2)?(0):					\
	(info < 4)?(1):					\
	(info < 8)?(2):					\
	(info < 16)?(3):				\
	(info < 32)?(4):				\
	(info < 64)?(5):				\
	(info < 128)?(6):				\
	(info < 256)?(7):				\
	(info < 512)?(8):				\
	(info < 1024)?(9):				\
	(info < 2048)?(10):				\
	(info < 4096)?(11):				\
	(info < 8192)?(12):				\
	(info < 16384)?(13):			\
	(info < 32768)?(14):(15);		\
	info <<= (32 - length);			\
	info >>= (32 - length);

#define GET_VLC_LENGTH_INFO_CHAR(info,length)		\
	(length) =						\
	(info < 2)?(0):					\
	(info < 4)?(1):					\
	(info < 8)?(2):					\
	(info < 16)?(3):				\
	(info < 32)?(4):				\
	(info < 64)?(5):				\
	(info < 128)?(6):(7);			\
	info <<= (32 - length);			\
	info >>= (32 - length);

#endif	/*  defined(_WIN32) && defined(_M_IX86) */
	extern const U8 mbtype_p_dec_tab[8]; /* Flawfinder: ignore */
	extern const U8 mbtype_p_enc_tab[12]; /* Flawfinder: ignore */
	extern const U8 mbtype_b_dec_tab[8]; /* Flawfinder: ignore */
	extern const U8 mbtype_b_enc_tab[12]; /* Flawfinder: ignore */
	extern const U16 mbtype_pframe_vlc [56]; /* Flawfinder: ignore */
	extern const U8 mbtype_bframe_vlc [48]; /* Flawfinder: ignore */

	extern const short ditherL[16];
	extern const short ditherR[16];
	extern const I32 alpha_tab[32] ;
	extern const U8 beta_tab[32];

	/* mapping of DeblockingQPAdjust */
	extern const U8 DBQPA[6];

	//zou 
	// To avoid the conflicts with GPL license
	// we generate the dsc_to_l0123 in voRealVideoDecInit instead of using a const table.
	U8 dsc_to_l0123[108];


#ifdef __cplusplus
}
#endif



#endif /* TTABLES_H__ */
