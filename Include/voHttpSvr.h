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


#ifndef __voHttpSvr_H__
#define __voHttpSvr_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voIndex.h>
#include <voString.h>
#include <voMem.h>

#include <voDataStream.h>

/**
 * http server function set
 */
typedef struct
{
	VO_U32		nVersion;	// It should be 0X1000XXXX.

	VO_PTR		hHandle;

	VO_PTR		hUserData;

	VO_U32 (VO_API * Init) (VO_PTR * ppHandle, VO_PTR pUserData, VO_DATA_STREAM_API* pStreamAPI, int nFlag);

	VO_U32 (VO_API * Uninit) (VO_PTR pHandle);

	VO_U32 (VO_API * Start) (VO_PTR pHandle);

	VO_U32 (VO_API * Stop) (VO_PTR pHandle);


	/**
	 * Set sink param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	 * Get sink param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [Out] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

} VO_HTTP_SERVER_API;

VO_S32 VO_API voGetHttpServerAPI (VO_HTTP_SERVER_API* pHttpSvrAPI, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voHttpSvr_H__
