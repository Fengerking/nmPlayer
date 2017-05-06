/* ************************************************* BEGIN LICENSE BLOCK ********************************************* 
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
**************************************************** END LICENSE BLOCK *************************************************/ 
/***************************************************************************/
/*    RealNetworks, Inc. Confidential and Proprietary Information. */
/*    Copyright (c) 1995-2002 RealNetworks, Inc.                        */
/*    All Rights Reserved.                                                         */
/*    Do not redistribute.                                                           */
/***************************************************************************/
/***************************************************************************/
/*    INTEL Corporation Proprietary Information                            */
/*    This listing is supplied under the terms of a license              */
/*    agreement with INTEL Corporation and may not be copied    */
/*    nor disclosed except in accordance with the terms of           */
/*    that agreement.                                                                */
/*    Copyright (c) 1997-1998 Intel Corporation.                           */
/*    All Rights Reserved.                                                          */
/***************************************************************************/
/************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
//Header: /cvsroot/rarvcode-video/codec/rv89combo/rv89combo_c/common/realvdo.h,v 1.6 2005/03/11 23:05:16 rascar Exp

#ifndef REALVDO_H__
#define REALVDO_H__

//The file defines the base RealVideo class.
#include "rvtypes.h"
#include "rvstatus.h" 
#include "rvfid.h"
#include <voType.h>

#ifdef __cplusplus
extern "C" {
#endif


	/* This counter allows Load() and Unload() to perform */
	/* initialization only once, and cleanup only after all */
	/* loaded instances have terminated. */

	/* The RealVideo class represents an arbitrary host-independent video */
	/* environment.  This is the view of the world from the underlying codec */
	/* looking up.  Its implementation mediates communication between a */
	/* specific video environment and the codec's RV interface. */
	/* Typically only one RealVideo object is allocated, and is used by all */
	/* codec instances. */

	/* Define the information that gets passed into, and returned from, */
	/* each call to decompress a frame. */

	/* This set of flags gets passed into each Decode() invocation. */

	typedef U32     RV_Decoder_Notes;
	/* This set of notes gets returned from each Decode() invocation. */

	/* Construct the specific Decoder object. */
	void   Decoder_Delete(void *coredecoder);

	RV_Status Decoder_DecAFrame(
		void *decoder, 
		RV_Decoder_Notes *notes,
		U8* const	pb, U32 pb_size);

	VO_U32 Decoder_Malloc_buf(
		void *coredecoder,
		RV_FID	fid,
		U32 uWidth, 
		U32 uHeight);

#ifdef __cplusplus
}
#endif


#endif /* REALVDO_H__ */
