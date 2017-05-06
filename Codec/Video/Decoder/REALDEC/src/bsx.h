/************************************************ BEGIN LICENSE BLOCK ********************************************** 
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
* ********************************************************END LICENSE BLOCK *************************************************/ 

/**************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 2001 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
*    Bitstream functionality. 
*************************************************************************************

****************************************************************************
*    INTEL Corporation Proprietary Information
*    This listing is supplied under the terms of a license
*    agreement with INTEL Corporation and may not be copied
*    nor disclosed except in accordance with the terms of
*    that agreement.
*    Copyright (c) 1995 - 1999 Intel Corporation.
*    All Rights Reserved.
*****************************************************************************/
/************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/bsx.h,v 1.1.1.1 2005/12/19 23:01:44 rishimathew Exp $ */
/********************************************************************************
Emuzed India Private Limited
*******************************************************************************
Revision Record:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Date		    Author					Comment 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
2nd May, 2005	Real Networks			Initial Code
16th May, 2005	M. Anjaneya Prasad		1.Added Fuctions CB_ShowBits, CB_FlushBits
which help in INTRA and MB Type Decoding

********************************************************************************/

#ifndef BSX_H__
#define BSX_H__

#include "rvtypes.h"
#include "rvstatus.h"
#include "rvstruct.h"

#ifdef __cplusplus
extern "C" {
#endif

	void            initbits(SLICE_DEC* slice_info, U8 *stream, const I32 len);

	/* Reads one general VLC code from bitstream and returns its length.*/
	/* The info portion of the code is returned via the 'info' paramter.*/ 
	U32             CB_GetVLCBits(SLICE_DEC *slice_di, U32 *info); 
	void			CB_GetDQUANT(SLICE_DEC *slice_di,U8 *quant, const U32 quant_prev);

	RV_Status		CB_GetPictureHeader(struct Decoder *t, SLICE_DEC *slice_di);
	RV_Status		CB_GetPictureHeader_RV8(struct Decoder *t, SLICE_DEC *slice_di);

	RV_Boolean		CB_GetSSC(struct Decoder *t, U32 slice_number, SLICE_DEC *slice_di);

	RV_Status		CB_GetSliceHeader(struct Decoder *t, I32* iMBA, SLICE_DEC *slice_di);
	RV_Status		CB_GetSliceHeader_RV8(struct Decoder *t, I32* iMBA, SLICE_DEC *slice_di);

	I32             realVideoFrametype(struct Decoder *dec,U8* bits);

	static INLINE U8 get_entropyqp(U32 QP, U32 QSVQP)//TBD
	{
		U32 EntropyQP = QP;

		if(QSVQP == 2 && QP < 19) 
			EntropyQP = MIN(30,QP+10);
		else if(QSVQP && QP < 26) 
			EntropyQP = MIN(30,QP+5);
		return (U8)EntropyQP;
	}

#ifdef __cplusplus
} 
#endif


#endif /* BSX_H__ */
