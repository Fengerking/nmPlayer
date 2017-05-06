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

#ifndef __voWebVideo_H__
#define __voWebVideo_H__

#if defined __cplusplus
extern "C" {
#endif

#include "voType.h"

/**
 * Web Video Parser function set
 */
typedef struct
{
	/**
	 * Initialize the parser and return parser handle
	 * \param ppHandle [OUT] Return the parser operator handle
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API *Init) (VO_HANDLE *ppHandle);

	/**
	 * unInitialize the parser.
	 * \param pHandle [IN] The handle which was create by Init function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API *Uninit) (VO_HANDLE pHandle);

	/**
	 * Get final download URL from Web URL
	 * \param pHandle [IN] The handle which was create by Init function..
	 * \param szWebLink [IN] The Web URL.
	 * \param ppDLs [OUT] The final download URLs. Maybe a list of URLs.
	 * \param ppCookie [OUT]. There Cookies which be used in HTTP request. 
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API *GetVideoURL) (VO_HANDLE pHandle, const VO_PCHAR szWebLink, VO_PCHAR *ppDLs, VO_PCHAR *ppCookie);

	/**
	 * Set user info
	 * \param pHandle [IN] The handle which was create by Init function..
	 * \param szWebLink [IN] The Web URL.
	 * \param szUserName [IN] The user's name
	 * \param szPassWord [IN] There user's password 
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API *SetLoginInfo) (VO_HANDLE pHandle, const VO_PCHAR szWebLink, const VO_PCHAR szUserName, const VO_PCHAR szPassWord);
} VO_WVPARSER_API;

/**
 * Get Web Video Parser API interface
 * \param pReadHandle [out] Return the web video parser handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetWVParserAPI (VO_WVPARSER_API * pHandle, VO_U32 uFlag);


#if defined __cplusplus
}
#endif

#endif //__voWebVideo_H__