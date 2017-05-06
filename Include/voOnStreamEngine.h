/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __VO_ONSTREAM_ENGN_API_H_
#define __VO_ONSTREAM_ENGN_API_H_

#include "voOnStreamType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
/**
* OnStream ENGINE function set
*/
typedef struct
{
	/**
	* Initialize ENGINE instance.
	* This function should be called first for a session.
	* \param phHandle [out] ENGINE handle.
	* \param nPlayerType [in] The type of media framework.refer to VOOSMP_PLAYER_TYPE
	* \param pInitParam [in] Initial param,depend on nInitParamFlag.
	* \param nInitParamFlag [in] Initial param flag,refer to VOOSMP_INIT_PARAM_FLAG.
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* Init) (void** phHandle, int nPlayerType, void* pInitParam, int nInitParamFlag);

	
	/**
	* Destroy ENGINE instance.
	* This function should be called last of the session.
	* \param pHandle [in] The ENGINE handle. Created by Init().
	* \retval VOOSMP_ERR_None Succeeded
	*/
	int (* Uninit) (void* pHandle);


	/**
	* Set the video view
	* \param pHandle [in] The ENGINE handle. Created by Init().
	* \param pView [in] The handle of view
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* SetView) (void* pHandle, void* pView);

	
	/**
	 * Set source URL
	 * \param pHandle [in] The ENGINE handle. Created by Init().
	 * \param pSource [in] The Data source URL
	 * \param nFlag [in] The flag for open source,refer to VOOSMP_OPEN_SOURCE_FLAG
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Open) (void* pHandle, void* pSource, int nFlag);
	
	
	/**
	 * Close source
	 * \param pHandle [in] The ENGINE handle. Created by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Close) (void* pHandle);

	
	/**
	* Start to play
	* \param pHandle [in] The ENGINE handle. Created by Init().
	* \retval VOOSMP_ERR_None Succeeded
	*/
	int (* Run) (void* pHandle);

	
	/**
	 * Pause
	 * \param pHandle [in] The ENGINE handle. Created by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Pause) (void* pHandle);

	
	/**
	* Stop playing
	* \param pHandle [in] The ENGINE handle. Created by Init().
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* Stop) (void* pHandle);
	
	
	/**
	 * Get current playing position
	 * \param pHandle [in] The ENGINE handle. Created by Init().
	 * \retval return current position,-1 if fialed 
	 */
	int (* GetPos) (void* pHandle);
	
	
	/**
	 * Seek operation
	 * \param pHandle [in] The ENGINE handle. Created by Init().
	 * \param nPos [in] The position of seeking.
	 * \retval The correct position after seek operation, -1 if seek failed
	 */
	int (* SetPos) (void* pHandle, int nPos);	
	
	
	/**
	 * Get subtile or close caption sample
	 * \param pHandle [in] The ENGINE handle. Created by Init().
	 * \param pSample [in/out] output the subtile ot close caption buffer
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetSubtitleSample) (void* pHandle, voSubtitleInfo* pSample);
    
    	
    /**
     * Select the subtitle language
     * \param hHandle [in] The ENGINE handle. Opened by Init().
     * \param nIndex [in] The index of language
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* SelectLanguage) (void* pHandle, int nIndex);
    
    
    /**
     * Get the subtitle language list
     * \param hHandle [in] The ENGINE handle. Opened by Init().
     * \param ppLangInfo [out] The language info list
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* GetLanguage) (void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);    

    /**
     * Get the SEI info
     * \param hHandle [in] The ENGINE handle. Opened by Init().
     * \param pInfo [in/out] The SEI info
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* GetSEI) (void* pHandle, VOOSMP_SEI_INFO* pInfo);
	
	/**
	* Get the special value from param ID
	* \param pHandle [in] The ENGINE handle. Created by Init().
	* \param nParamID [in] The param ID
	* \param pValue [out] The return value depend on the param ID.
	* \retval VOOSMP_ERR_None Succeeded
	*/
	int (* GetParam) (void* pHandle, int nParamID, void* pValue);


	/**
	* Set the special value from param ID
	* \param pHandle [in] The ENGINE handle. Created by Init().
	* \param nParamID [in] The param ID
	* \param pValue [in] The set value depend on the param ID.
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* SetParam) (void* pHandle, int nParamID, void* pValue);
} voOnStreamEngnAPI;


/**
* Get OnStream ENGINE API interface
* \param pFuncSet [in/out] Return the ENGINE API set.
* \retval VOOSMP_ERR_None Succeeded.
*/
int voGetOnStreamEngnAPI(voOnStreamEngnAPI* pFuncSet);

typedef int (* VOGETONSTREAMENGNAPI) (voOnStreamEngnAPI* pFuncSet);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_ONSTREAM_ENGN_API_H_ */


