/* ************************************************ BEGIN LICENSE BLOCK ************************************************ 
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
* **************************************************************** END LICENSE BLOCK **********************************************/ 

/***********************************************************************************************
*    RealNetworks, Inc. Confidential and Proprietary Information. 
*    Copyright (c) 1995-2002 RealNetworks, Inc. 
*    All Rights Reserved.
*    Do not redistribute. 
**********************************************************************************************
************************************************************************
*																		*
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/
//The DecodedFrame class represents a YUV work space which was obtained
//by decoding a bitstream picture.
#ifndef FRAME_H__
#define FRAME_H__

#include "rvtypes.h"
#include "rvstatus.h"

#ifdef __cplusplus
extern "C" {
#endif

	//Define classes that encapsulate pitched YUV image data.

	/***********************************************************************************************************
	* The PaddedYUVBuffer class represents a YUV image laid out in memory such 
	* that there are padding bytes surrounding the three planes.  This extra 
	* padding allows pixels at the edge of the planes to be replicated, as 
	* well as facilitating algorithms that need to read or write a few bytes 
	* beyond the end of a line of pixels.  The frame is laid out as shown below.
	* 
	*          +---------------------+ 
	*          |                     | 
	*          |    +-----------+    |     The amount of padding around the 
	*          |    |           |    |     U and V planes is half the amount 
	*          |    |     Y     |    |     around the Y plane.  The dotted line 
	*          |    |           |    |     separating the planes is there just 
	*          |    +-----------+    |     to emphasize that the padding around 
	*          |                     |     each plane does not overlap the 
	*          | .  .  .  .  .  .  . |     padding around the other planes. 
	*          |          .          | 
	*          |  +----+  .  +----+  |     Note that all three planes have 
	*          |  | U  |  .  | V  |  |     the same pitch.
	*          |  +----+  .  +----+  | 
	*          +---------------------+ 
	* 
	* Note that the class is designed to allow reusing existing buffer space 
	* for a smaller image size.  For example, if the frame buffer was originally 
	* allocated to hold a CIF image, and is then adjusted to hold a QCIF image, 
	* then the QCIF Y, U and V planes will reside in or about the upper left 
	* quadrant of the CIF-size frame buffer.  Such reuse can result in much 
	* wasted space.  If this is undesireable, then the existing large buffer 
	* should be explicitly deallocated prior to allocating the smaller size. 
	* To be fully general, this class should allow the amount of padding to be 
	* specified via a constructor parameter.  However, we don't need such 
	* generality currently.  To simplify the class, we will use the hard coded 
	* padding amounts defined below. 
	*****************************************************************************************/

#define YUV_Y_PADDING       16
	/* The Y plane has this many bytes (a.k.a. pels) of padding around each */
	/* of its 4 sides */

#define YUV_UV_PADDING      8
	/* The U and V planes have this many bytes (a.k.a. pels) of padding */
	/* around each of their 4 sides */

#define YUV_ALIGNMENT       32
	/* The beginning of the padded buffer is aligned thusly. */
	/* (These numbers imply that the entire buffer is 32-byte aligned, */
	/* while the Y plane is only 16-byte aligned.  Is 16-byte alignment */
	/* sufficient for the overall buffer?) */

	typedef struct
	{
		/* YUVPointers */
		U8     *m_pYPlane;
		U8     *m_pUPlane;
		U8     *m_pVPlane;
		VO_S64 Time;
		VO_PTR		UserData;
	} IMAGE;

	typedef struct
	{
		/* YUVPackPointers */
		U8     *m_pYPlane;
		U8     *m_pUVPack;
	} YUV420PACK;

	typedef struct
	{
		I32 iDeltaLeft;
		I32 iDeltaRight;
		I32 iDeltaTop;
		I32 iDeltaBottom;
	} T_RPR_EdgeParams;

#if defined(VOARMV7)
	void  Fill_Edge_Y_Ctx(U8 *Y,const U32 ExWidth,const U32 ExHeight,U32 width, U32 height);
	void  Fill_Edge_UV_Ctx(U8 *U,const U32 ExWidth2,const U32 ExHeight2,U32 width2, U32 height2, U32 edged_width2);
#endif

#ifdef __cplusplus
}
#endif

#endif//FRAME_H__
