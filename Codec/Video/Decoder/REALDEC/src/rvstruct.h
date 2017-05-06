/* *************************************************** BEGIN LICENSE BLOCK ******************************************** 
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
********************************* END LICENSE BLOCK *******************************************************************/ 

/***********************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information.
*    Copyright (c) 1995-2002 RealNetworks, Inc.
*    All Rights Reserved. 
*    Do not redistribute. 
***********************************************************************************/

//Various structure definitions. 

/***********************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1995 - 2000 Intel Corporation. 
*    All Rights Reserved. 
**********************************************************************************/
/************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/rvstruct.h,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp $ */

#ifndef RVSTRUCT_H__
#define RVSTRUCT_H__


#include "rvtypes.h"
#include "rvfid.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RV_Dimensions
{
	U32     width;
	U32     height;
};


#define FID_I420  100
#define FID_YUV12 120
#define FID_IYUV  140
#define FID_NOTSUPPORTED 999
/**************************************************************************************************
* Macroblock type definitions 
* Keep these ordered such that intra types are first, followed by
* inter types.  Otherwise you'll need to change the definitions
* of IS_INTRA_MBTYPE and IS_INTER_MBTYPE.
**************************************************************************************************
* WARNING:  Because the decoder exposes macroblock types to the application, 
* these values cannot be changed without affecting users of the decoder. 
* If new macro block types need to be inserted in the middle of the list, 
* then perhaps existing types should retain their numeric value, the new 
* type should be given a new value, and for coding efficiency we should 
* perhaps decouple these values from the ones that are encoded in the  bitstream. 
***************************************************************************************************/

	typedef enum {
		MBTYPE_INTRA,			/* 0 */
		MBTYPE_INTRA_16x16,		/* 1 */
		MBTYPE_INTER,			/* 2 */
		MBTYPE_INTER_4V,		/* 3 */
		MBTYPE_FORWARD,			/* 4 */
		MBTYPE_BACKWARD,		/* 5 */
		MBTYPE_SKIPPED,			/* 6 */
		MBTYPE_DIRECT,			/* 7 */
		MBTYPE_INTER_16x8V,		/* 8 */
		MBTYPE_INTER_8x16V,		/* 9  */
		MBTYPE_BIDIR,			/* 10 */
		MBTYPE_INTER_16x16,		/* 11 */
		NUMBER_OF_MBTYPES,		/* 12 */
		MB_Type_MAX = VO_MAX_ENUM_VALUE
	} MB_Type;


	/* macro - yields TRUE if a given MB type is INTRA */
#define IS_INTRA_MBTYPE(mbtype) ((mbtype) < MBTYPE_INTER)
#define IS_INTER_16X16_MBTYPE(mbtype) (MBTYPE_INTER_16x16 == (mbtype))

	/* macro - yields TRUE if a given MB type is INTER */
#define IS_INTER_MBTYPE(mbtype) ((mbtype) >= MBTYPE_INTER)

	/* macro - yields TRUE if a given MB type is INTERLACED */
#define IS_INTERLACE_MBTYPE(mbtype) ((mbtype) >= MBTYPE_INTER_16x16i)

	/* macro - yields TRUE if a given MB type is INTERLACED & B Frame type*/
#define IS_INTERLACE_B_MBTYPE(mbtype) ((mbtype) >= MBTYPE_FORWARD_16x16i)

#define IS_FORWARD_OR_BIDIR(mbtype)(((mbtype) == MBTYPE_FORWARD) || \
	((mbtype) == MBTYPE_BIDIR))

#define IS_BACKWARD_OR_BIDIR(mbtype)(((mbtype) == MBTYPE_BACKWARD) || \
	((mbtype) == MBTYPE_BIDIR))


/******************************************************************************************************
* A DecoderMBInfo structure describes one decoded macroblock. 
* We should keep this structure reasonably small and well aligned, 
* since we need to allocate one per luma macroblock, twice (one set 
* for B frames and one set for non-B frames). 
* Technically, we don't need two sets of the 'missing' and 'edge_type' 
* members.  But they are relatively small, and keeping them along with 
* QP and mbtype will help localize data cache accesses.
*******************************************************************************************************/

	struct DecoderMBInfo
	{
		U8  QP;
		U8  mbtype;
		U8  data0;
		U8  data2;
		U32 deblock_cbpcoef;
		U32 cbp;
		U32 mvd;
	};


#ifdef __cplusplus
}
#endif

#endif /* RVSTRUCT_H__ */

