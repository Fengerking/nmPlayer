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
#ifndef __voBaseFun_H__
#define __voBaseFun_H__

#include "voType.h"

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */


#define    VO_PID_BASEFUNC_Module			      0x80111000
 	
#define    VO_PID_BASEFUNC_SERVER_TIME						(VO_PID_BASEFUNC_Module | 0x0001)
#define    VO_PID_BASEFUNC_NETWORK_TIME						(VO_PID_BASEFUNC_Module | 0x0002)
#define    VO_PID_BASEFUNC_GPS_TIME							(VO_PID_BASEFUNC_Module | 0x0003)
#define    VO_PID_BASEFUNC_LOCAL_TIME                    (VO_PID_BASEFUNC_Module | 0x0004)

/**
 * Base Function Set.
 */
typedef struct
{
	/**
	 * Init the common base module and return base handle
	 * \param phDec [OUT] Return the common base function handle
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * pInit) (VO_HANDLE * phHandle);

	/**
	 * Set the param for special target.
	 * \param hDec [IN]] The Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [IN] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * pSetParam) (VO_HANDLE hHandle, VO_S32 nParamID, VO_PTR pData);

	/**
	 * Get the param for special target.
	 * \param hDec [IN]] The Handle which was created by Init function.
	 * \param uParamID [IN] The param ID.
	 * \param pData [OUT] The param value depend on the ID>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * pGetParam) (VO_HANDLE hHandle, VO_S32 nParamID, VO_PTR pData);

		/**
	 * Set the config for special target.
	 * \param hDec [IN]] The Handle which was created by Init function.
	 * \param nFeature [IN] The feature you want to set.
	 * \param nParam1 [IN] The feature param1.
	 * \param pData [IN] The param value depend on the nFeature and nParam1>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * pSetConfig) (VO_HANDLE hHandle, VO_S32 nFeature, VO_S32 nParam1, VO_PTR pData);

	/**
	 * Get the config for special target.
	 * \param hDec [IN]] The Handle which was created by Init function.
	 * \param nFeature [IN] The feature you want to get.
	 * \param nParam1 [IN] The feature param1.
	 * \param pData [OUT] The param value depend on the nFeature and nParam1>
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * pGetConfig) (VO_HANDLE hHandle, VO_S32 nFeature, VO_S32 nParam1, VO_PTR pData);

	/**
	 * Uninit the decoder.
	 * \param hDec [IN]] The Handle which was created by Init function.
	 * \retval VO_ERR_NONE Succeeded.
	 */
	VO_U32 (VO_API * pUninit) (VO_HANDLE hHandle);
} VO_BASE_FUNC_API;


/**
 * Get os base function API interface
 * \param pBaseHandle [IN/OUT] Return the os base common API handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetBaseOSFunctionAPI (VO_BASE_FUNC_API * pBaseHandle);

typedef int (* VOGETBASEOSAPI) (VO_BASE_FUNC_API* pFuncSet);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
