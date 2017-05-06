/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
// voPreHTTP.cpp : Defines the exported functions for the DLL application.
//

#ifndef __voPreHTTP_H__
#define __voPreHTTP_H__

//#include "stdafx.h"

#include "voLiveSource.h"

#if defined __cplusplus
extern "C" {
#endif

	typedef struct
	{
		/**
		* Open the source and return source handle
		* \param ppHandle [OUT] Return the source operator handle
		* \param pParam [IN] The source open param
		* \retval VO_ERR_NONE Succeeded.
		*/
		VO_U32 (VO_API* voPreHTTPInit)(VO_PTR * ppHandle);

		/**
		* Close the opened source.
		* \param pHandle [IN] The handle which was create by open function.
		* \retval VO_ERR_NONE Succeeded.
		*/
		VO_U32 (VO_API* voPreHTTPUninit)(VO_PTR pHandle);

		/**
		* Set the source URL for special target.
		* \param pHandle [IN] The handle which was create by open function.
		* \param pSourceURL [IN] HTTP URL.
		* \retval VO_ERR_NONE Succeeded.
		*/
		VO_U32 (VO_API* voPreHTTPSetSrcURL)(VO_PTR pHandle, VO_CHAR * pSourceURL);

		/**
		* Get the source URL for special target.
		* \param pHandle [IN] The handle which was create by open function.
		* \param pType [OUT] HTTP URL.
		* \param ppDesURL [OUT] HTTP URL.
		* \retval VO_ERR_NONE Succeeded.
		*/
		VO_U32 (VO_API* voPreHTTPGetDesInfo)(VO_PTR pHandle, VO_LIVESRC_FORMATTYPE *pType,VO_PCHAR *ppDesURL);

	} VO_PREHTTP_API;

	VO_S32 VO_API voGetPreHTTPAPI (VO_PREHTTP_API * pHandle, VO_U32 uFlag);

#if defined __cplusplus
}
#endif

#endif //__voPreHTTP_H__