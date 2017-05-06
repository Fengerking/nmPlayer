/* ****************************************** BEGIN LICENSE BLOCK ********************************************** 
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
*************************************************** END LICENSE BLOCK **********************************************/ 

/***************************************************************************/
/*    RealNetworks, Inc. Confidential and Proprietary Information. */
/*    Copyright (c) 1995-2002 RealNetworks, Inc.						 */
/*    All Rights Reserved.															 */
/*    Do not redistribute.															 */
/* *************************************************************************/
/* *************************************************************************/
/*    INTEL Corporation Proprietary Information							 */
/*    This listing is supplied under the terms of a license				 */
/*    agreement with INTEL Corporation and may not be copied    */
/*    nor disclosed except in accordance with the terms of			 */
/*    that agreement.																 */
/*    Copyright (c) 1997 Intel Corporation.									 */
/*    All Rights Reserved.															 */
/***************************************************************************/
/************************************************************************
*																	    *
*	VisualOn, Inc. Confidential and Proprietary, 2008		            *
*								 										*
************************************************************************/

//Header: /cvsroot/rarvcode-video/codec/rv89combo/platform/emuzed/cdeclib/rvdefs.h,v 1.1.1.1 2005/12/19 23:01:45 rishimathew Exp 
/******************************************************************************/
/* This file doesn't declare or define, in the C sense, any objects.	      */
/* It references preprocessor macros that can be defined, before              */
/* including this file, to extract the appropriate information.               */
/* These macros are automatically #undef'd at the end of this file.           */
/* This file is intended to be included multiple times.                       */
/******************************************************************************/
/**********************************************************************************************
* DEFRVFMT lists information associated with a color format. 
* The first argument will be used to define the RV_FID enumeration type         
* that identifies all the image formats.  It can also be used to index into             
* various arrays that hold information about the format.                                 
* The second argument gives the bit depth of the format.                              
*                                                                                                            
* The third argument is a 1 if the concepts of topdown and bottom-up              
* apply to the format.  Otherwise it is a 0.  When 0, the 'topdown'                   
* member of the RV_Image_Format structure is meaningless, and should       
* be set to TRUE for consistency.                                                               
*                                                                                                                 
* The fourth argument indicates which union member this format uses in           
* the RV_Image_Format and RV_Image data structures.  Currently only            
* one union member, yuv_info, exists.  So, this argument is a 1 if the               
* yuv_info union member is relevant, otherwise it is a 0.                                    
*                                                                                                                 
* The fifth argument is the Video-for-Windows Four CC code associated             
* with the color format.  It should be FOURCC_UNDEFINED if one doesn't exist. 
*                                                                                                                   
* The sixth argument is a 1 if there is a one-to-one correspondence between       
* the RV_FID and the VFW fourcc code, otherwise it is a zero.  This                 
* information is used to map a fourcc code to a RV_FID, without having              
* any other information, like bit depth.                                                             
*                                                                                                                   
* The seventh argument is the Active Movie Media subtype associated               
* with the color format.  It is MEDIASUBTYPE_NULL if unknown.                       
**********************************************************************************************/

#if defined(DEFRVFMT)

DEFRVFMT(RV_FID_UNDEFINED,     0, 0, 0,
		 FOURCC_UNDEFINED,      0,
		 MEDIASUBTYPE_NULL)
		 /* an uninitialized color format 0*/

		 DEFRVFMT(RV_FID_RGB4,          4, 1, 0,
		 BI_RGB,                0,
		 MEDIASUBTYPE_RGB4)
		 /* 16 level gray scale */

		 DEFRVFMT(RV_FID_CLUT8,         8, 1, 0,
		 BI_RGB,                0,
		 MEDIASUBTYPE_RGB8)
		 /* 8-bit color lookup table */

		 DEFRVFMT(RV_FID_XRGB16_1555,  16, 1, 0,
		 BI_RGB,                0,
		 MEDIASUBTYPE_555)
		 /* 1 unused, 5-bits each red, green, blue */

		 DEFRVFMT(RV_FID_RGB16_565,    16, 1, 0,
		 BI_BITFIELDS,          0,
		 MEDIASUBTYPE_565)
		 /* 16-bit color variant */

		 DEFRVFMT(RV_FID_RGB16_655,    16, 1, 0,
		 BI_BITFIELDS,          0,
		 MEDIASUBTYPE_NULL)
		 /* 16-bit color variant */

		 DEFRVFMT(RV_FID_RGB16_664,    16, 1, 0,
		 BI_BITFIELDS,          0,
		 MEDIASUBTYPE_NULL)
		 /* 16-bit color variant */

		 DEFRVFMT(RV_FID_RGB24,        24, 1, 0,
		 BI_RGB,                0,
		 MEDIASUBTYPE_RGB24)
		 /* 8-bits each: red, green, blue 7 */

		 DEFRVFMT(RV_FID_XRGB32,       32, 1, 0,
		 BI_RGB,                0,
		 MEDIASUBTYPE_RGB32)
		 /* 8-bits each: blue, green, red */

		 DEFRVFMT(RV_FID_MPEG2V,       24, 0, 0,
		 FOURCC_MPEG2V,         1,
		 MEDIASUBTYPE_MPEG2_VIDEO)

		 DEFRVFMT(RV_FID_YVU9,          9, 0, 1,
		 FOURCC_YVU9,           1,
		 MEDIASUBTYPE_YVU9)

		 DEFRVFMT(RV_FID_YUV12,        12, 0, 1,
		 FOURCC_YUV12,          1,
		 AM_MEDIASUBTYPE_I420)
		 /* Also known as I420 */
		 /* Y-plane comes first, followed by U-plane then V-plane */

		 DEFRVFMT(RV_FID_IYUV,         12, 0, 1,
		 FOURCC_IYUV,           1,
		 AM_MEDIASUBTYPE_IYUV)
		 /* This format has the same representation as RV_FID_YUV12 */

		 DEFRVFMT(RV_FID_YV12,         12, 0, 1,
		 FOURCC_YV12,           1,
		 AM_MEDIASUBTYPE_YV12)
		 /* Like RV_FID_YUV12, except order of V and U planes is swapped */

		 DEFRVFMT(RV_FID_YUY2,         16, 0, 0,
		 FOURCC_YUY2,           1,
		 MEDIASUBTYPE_YUY2)

		 DEFRVFMT(RV_FID_BITMAP16,     16, 1, 0,
		 BI_BITMAP,             0,
		 MEDIASUBTYPE_NULL)

		 DEFRVFMT(RV_FID_H261,         24, 0, 0,
		 FOURCC_H261,           1,
		 AM_MEDIASUBTYPE_H261)

		 DEFRVFMT(RV_FID_H263,         24, 0, 0,
		 FOURCC_H263,           1,
		 AM_MEDIASUBTYPE_H263)		/* 17 */

		 DEFRVFMT(RV_FID_H263PLUS,     24, 0, 0,
		 FOURCC_H263PLUS,       1,
		 AM_MEDIASUBTYPE_H263PLUS)

		 DEFRVFMT(RV_FID_RV89COMBO,     24, 0, 0,
		 FOURCC_RV89COMBO,       1,
		 AM_MEDIASUBTYPE_RV89COMBO)
		 /* Temporary FID to use for RV89COMBO encoder */

		 DEFRVFMT(RV_FID_ILVR,         24, 0, 0,
		 FOURCC_ILVR,           1,
		 AM_MEDIASUBTYPE_ILVR)
		 /* ILVR is Intel's H.263+ format that includes IETF RTP */
		 /* bitstream extensions which assist in network packetization. */

		 DEFRVFMT(RV_FID_REALVIDEO21,   24, 0, 0,
		 FOURCC_UNDEFINED,      1,
		 MEDIASUBTYPE_NULL)
		 /* Represents the RealVideo 2 compression format, version 2.1 */
		 /* The actual bit stream format might change periodically, */
		 /* as RealVideo is enhanced.  To prevent version problems, */
		 /* the RealVideo RealVideo takes care of negotiating with the */
		 /* application that the appropriate version of a RealVideo 2 */
		 /* bitstream version is in fact being used. */
		 DEFRVFMT(RV_FID_REALVIDEO22,   24, 0, 0,
		 FOURCC_UNDEFINED,      1,
		 MEDIASUBTYPE_NULL)
		 /* Represents the RealVideo 2 compression format, version 2.2 */

		 DEFRVFMT(RV_FID_REALVIDEO30,   24, 0, 0,
		 FOURCC_UNDEFINED,      1,
		 MEDIASUBTYPE_NULL)		   /* 23 */
		 /* Represents the future RealVideo compression format, version 3.0 */

#undef  DEFRVFMT

#endif /* DEFRVFMT */


		 /*********************************************************************************************************
		 * DEFRVSTATUS lists the error codes returned by HIVE/RV methods, and 
		 * identifies a corresponding error code for each video environment.
		 * The mapping from a RV status to an environment specific status always
		 * gives an appropriate value, but the reverse is not always true. 
		 * 
		 * The first argument will be used to define an enumeration type that 
		 * lists all the HIVE/RV status codes. 
		 * 
		 * The second argument gives the closest corresponding Video-for-Windows 
		 * error code. 
		 * 
		 * The third argument gives the closest corresponding Active Movie 
		 * error code. 
		 * 
		 * The fourth argument gives the closest corresponding RealVideo 
		 * error code. 
		 *********************************************************************************************************/

#if defined(DEFRVSTATUS)

		 /* Everything is a-okay. */
		 DEFRVSTATUS( RV_S_OK
		 , ICERR_OK
		 , S_OK
		 , HXR_OK
		 )

		 /* A non-specific error. */
		 DEFRVSTATUS( RV_S_ERROR
		 , ICERR_ERROR
		 , E_FAIL
		 , HXR_FAIL
		 )

		 /* A memory allocation request failed */
		 DEFRVSTATUS( RV_S_OUT_OF_MEMORY
		 , ICERR_MEMORY
		 , E_OUTOFMEMORY
		 , HXR_OUTOFMEMORY
		 )

		 /* An unsupported operation or feature was requested. */
		 DEFRVSTATUS( RV_S_UNSUPPORTED
		 , ICERR_UNSUPPORTED
		 , E_NOTIMPL
		 , HXR_NOTIMPL
		 )

		 /* User does not have the privileges to use a requested feature. */
		 DEFRVSTATUS( RV_S_ACCESS_DENIED
		 , ICERR_UNSUPPORTED
		 , E_ACCESSDENIED
		 , HXR_ACCESSDENIED
		 )

		 /* A supported operation or feature was requested, but is not */
		 /* available in the current state. */
		 DEFRVSTATUS( RV_S_UNAVAILABLE
		 , ICERR_UNSUPPORTED
		 , E_ACCESSDENIED
		 , HXR_ACCESSDENIED
		 )

		 /* An unexpected NULL parameter was seen. */
		 DEFRVSTATUS( RV_S_NULL_PARAM
		 , ICERR_BADPARAM
		 , E_INVALIDARG
		 , HXR_POINTER
		 )

		 /* The image dimensions are not supported. */
		 DEFRVSTATUS( RV_S_BAD_DIMENSIONS
		 , ICERR_BADIMAGESIZE
		 , E_INVALIDARG
		 , HXR_INVALID_PARAMETER
		 )

		 /* The image format is not supported. */
		 DEFRVSTATUS( RV_S_BAD_FORMAT
		 , ICERR_BADFORMAT
		 , VFW_E_INVALIDMEDIATYPE
		 , HXR_BAD_FORMAT
		 )

		 /* The requested functionality is not supported on the current CPU. */
		 DEFRVSTATUS( RV_S_BAD_CPU
		 , ICERR_UNSUPPORTED
		 , VFW_E_PROCESSOR_NOT_SUITABLE
		 , HXR_NOTIMPL
		 )

		 /* A numerical parameter was not within a specified range of legal values. */
		 DEFRVSTATUS( RV_S_OUT_OF_RANGE
		 , ICERR_BADPARAM
		 , E_INVALIDARG
		 , HXR_INVALID_PARAMETER
		 )

		 /* The image is not prepared for display. */
		 DEFRVSTATUS( RV_S_DONT_DRAW
		 , ICERR_DONTDRAW
		 , S_FALSE
		 , HXR_FAIL   /* Sorry, there is no such concept in RealVideo. */
		 )

#undef  DEFRVSTATUS

#endif

