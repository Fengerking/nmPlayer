/****************************************************** BEGIN LICENSE BLOCK ************************************************ 
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
****************************************************** END LICENSE BLOCK **************************************************/ 
/********************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved. 
*    Do not redistribute. 
*******************************************************************************
*******************************************************************************
*    INTEL Corporation Proprietary Information 
*    This listing is supplied under the terms of a license 
*    agreement with INTEL Corporation and may not be copied 
*    nor disclosed except in accordance with the terms of 
*    that agreement. 
*    Copyright (c) 1995 - 1999 Intel Corporation. 
*    All Rights Reserved. 
*******************************************************************************
************************************************************************
*																						  *
*	VisualOn, Inc. Confidential and Proprietary, 2008		          *
*								 														  *
************************************************************************/
/* $Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/decdefs.h,v 1.1.1.1 2005/12/19 23:01:44 rishimathew Exp $ */

#ifndef DECDEFS_H__
#define DECDEFS_H__

#include "rvtypes.h"

#ifdef __cplusplus
extern "C" 
{
#endif

/* This file defines some data structures and constants used by the decoder, */
/* that are also needed by other classes, such as post filters and */
/* error concealment. */

#define INTERP_FACTOR 4
#define INTERP_FACTOR_RV8 3
#define INTERP_ROUND  1




/* The decoder stores motion vectors for each luma 4x4 subblock in an */
/* entire picture. */
/* A motion vector is defined as 2 signed components in third-pel */
/* units - one for x and one for y. */

typedef I16 MVComponent;
/* An I16 is sufficient to hold any subpel motion vector for */
/* our supported image dimensions. */
/* Note that "our" encoder currently only emits motion vectors that */
/* fit in an I8.  However, we use an I16 to allow decoding bitstreams */
/* from other sources, such as Telenor. */

struct DecoderPackedMotionVector
{
	MVComponent  mvx;
	MVComponent  mvy;
};


/* The decoder stores intra-coding types (0..4) in U8's. */

//typedef U8 DecoderIntraType;
typedef I8 DecoderIntraType;

/* Declare some reconstruction functionality used across several files. */

#ifdef __cplusplus
}
#endif


#endif /* DECDEFS_H__ */
