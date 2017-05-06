/************************************************** BEGIN LICENSE BLOCK *********************************************** 
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
******************************************************** END LICENSE BLOCK ***************************************************/ 

/************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc.
*    All Rights Reserved. 
*    Do not redistribute. 
***********************************************************************************/
/************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/

#ifndef RVDEBUG_H__
#define RVDEBUG_H__

#include "rvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  RV_TL_ALWAYS	0
#define  RV_TL_INTRA	1
#define  RV_TL_DEBLOCK	2
#define  RV_TL_INIT		4
#define  RV_TL_PICT		8
#define  RV_TL_SUBPICT	16
#define  RV_TL_MBLK		32
#define  RV_TL_BLK		64
#define  RV_TL_HDET		128
#define  RV_TL_ALL		0xffffffff

/* YU TBD Clean up debug info*/
#ifndef NDEBUG
#define RVAssert(expr) 
#define RVDebug(params) 
#else
#define RVDebug(params)
#define RVAssert(expr)
#endif


#ifdef __cplusplus
}
#endif

#endif /* RVDEBUG_H__ */

