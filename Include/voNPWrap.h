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


#ifndef __VO_NP_WRAPPER_API_H_
#define __VO_NP_WRAPPER_API_H_

#include "voNPWrapType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
	
/**
* Native Player wrapper function set
*/
typedef struct
{
	/**
	* Initialize wrapper instance.
	* This function should be called first for a session.
	* \param phHandle [out] SDK handle.
	* \param nPlayerType [in] The type of media framework.refer to VONP_PLAYER_TYPE
	* \param pInitParam [in] The init param,now it's loading library function address
	* \retval VONP_ERR_None Succeeded 
	*/
	int (* Init) (void** phHandle, void* pInitParam);

	
	/**
	* Destroy wrapper instance.
	* This function should be called last of the session.
	* \param pHandle [in] The SDK handle. Created by Init().
	* \retval VONP_ERR_None Succeeded
	*/
	int (* Uninit) (void* pHandle);


	/**
	* Set the video view
	* \param pHandle [in] The SDK handle. Created by Init().
	* \param pView [in] The handle of view
	* \retval VONP_ERR_None Succeeded 
	*/
	int (* SetView) (void* pHandle, void* pView);

	
	/**
	 * Set source URL
	 * \param pHandle [in] The SDK handle. Created by Init().
	 * \param pSource [in] The Data source URL
	 * \param nFlag [in] The flag for open source,refer to VONP_OPEN_SOURCE_FLAG
	 * \retval VONP_ERR_None Succeeded
	 */
	int (* Open) (void* pHandle, void* pSource, int nFlag);
	
	
	/**
	 * Close source
	 * \param pHandle [in] The SDK handle. Created by Init().
	 * \retval VONP_ERR_None Succeeded
	 */
	int (* Close) (void* pHandle);

	
	/**
	* Start to play
	* \param pHandle [in] The SDK handle. Created by Init().
	* \retval VONP_ERR_None Succeeded
	*/
	int (* Run) (void* pHandle);

	
	/**
	 * Pause
	 * \param pHandle [in] The SDK handle. Created by Init().
	 * \retval VONP_ERR_None Succeeded
	 */
	int (* Pause) (void* pHandle);

	
	/**
	* Stop playing
	* \param pHandle [in] The SDK handle. Created by Init().
	* \retval VONP_ERR_None Succeeded 
	*/
	int (* Stop) (void* pHandle);
	
	
	/**
	 * Get current playing position
	 * \param pHandle [in] The SDK handle. Created by Init().
	 * \retval return current position,-1 if fialed 
	 */
	int (* GetPos) (void* pHandle);
	
	
	/**
	 * Seek operation
	 * \param pHandle [in] The SDK handle. Created by Init().
	 * \param nPos [in] The position of seeking.
	 * \retval The correct position after seek operation, -1 if seek failed
	 */
	int (* SetPos) (void* pHandle, int nPos);	
	
	
	/**
	* Get the special value from param ID
	* \param pHandle [in] The SDK handle. Created by Init().
	* \param nParamID [in] The param ID
	* \param pValue [out] The return value depend on the param ID.
	* \retval VONP_ERR_None Succeeded
	*/
	int (* GetParam) (void* pHandle, int nParamID, void* pValue);


	/**
	* Set the special value from param ID
	* \param pHandle [in] The SDK handle. Created by Init().
	* \param nParamID [in] The param ID
	* \param pValue [in] The set value depend on the param ID.
	* \retval VONP_ERR_None Succeeded 
	*/
	int (* SetParam) (void* pHandle, int nParamID, void* pValue);
} VO_NP_WRAPPER_API;


typedef int (* VONPWRAPPERAPI) (VO_NP_WRAPPER_API* pFuncSet);

/**
* Get OpenMAX AL wrapper API interface
* \param pFuncSet [in/out] Return the OpenMAX AL wrapper API set.
* \retval VONP_ERR_None Succeeded.
*/
int voOMXALWrapperAPI(VO_NP_WRAPPER_API* pFuncSet);

    
/**
 * Get HLS player wrapper API interface
 * \param pFuncSet [in/out] Return the HLS player wrapper API set.
 * \retval VONP_ERR_None Succeeded.
 */
int voGetHLSWrapperAPI(VO_NP_WRAPPER_API* pFuncSet);
    
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_NP_WRAPPER_API_H_ */


