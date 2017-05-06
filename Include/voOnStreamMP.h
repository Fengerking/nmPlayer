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


#ifndef __VO_ONSTREAM_MP_API_H_
#define __VO_ONSTREAM_MP_API_H_

#include "voOnStreamSource.h"
#include "voOnStreamEngine.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	
	
/**
* OnStream PLAYER function set
*/
typedef struct
{
	/**
	* Initialize PLAYER instance.
	* This function should be called first for a session.
	* \param phHandle [out] PLAYER handle.
	* \param nPlayerType [in] The type of media framework.refer to VOOSMP_PLAYER_TYPE
     * \param pInitParam [in] Initial param,depend on nInitParamFlag, refer to VOOSMP_INIT_PARAM.
	* \param nInitParamFlag [in] Initial param flag,refer to VOOSMP_INIT_PARAM_FLAG.
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* Init) (void** phHandle, int nPlayerType, void* pInitParam, int nInitParamFlag);

	
	/**
	* Destroy PLAYER instance.
	* This function should be called last of the session.
	* \param pHandle [in] The PLAYER handle. Created by Init().
	* \retval VOOSMP_ERR_None Succeeded
	*/
	int (* Uninit) (void* pHandle);


	/**
	* Set the video view
	* \param pHandle [in] The PLAYER handle. Created by Init().
	* \param pView [in] The handle of view
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* SetView) (void* pHandle, void* pView);

	
	/**
	 * Set source URL
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \param pSource [in] The Data source URL
	 * \param nFlag [in] The flag for open source,refer to VOOSMP_OPEN_SOURCE_FLAG
	 * \param nSourceType [in] indicate the source format, refer to VOOSMP_SRC_SOURCE_FORMAT
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Open) (void* pHandle, void* pSource, int nFlag, int nSourceType);
	
	
	/**
	 * It retrieve how many program in the live stream
	 * For Stream, it should be defined as: all sub streams that share the same angle for video and same language for audio
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \param pProgramCount [out] The number of the program
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetProgramCount) (void* pHandle, int* pProgramCount);
	
	
	/**
	 * Enum all the program info based on program count
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \param nProgramIndex  [in] The program sequence based on the program counts
	 * \param ppProgramInfo [out] return the info of the program, source will keep the info structure available until you call close
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetProgramInfo) (void* pHandle, int nProgramIndex, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo);
	
	
	/**
	 * Get the selected track info
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \param nTrackType [in] The sample type of the track, it will be audio/video/closed caption
	 * \param ppTrackInfo [out] The trackinfo of the selected track. The trackinfo memory will be mantained in source until you call close
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetCurTrackInfo) (void* pHandle, int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo );
	
	
	/**
	 * Get sample from the source
	 * \param pHandle [in] The PLAYER handle. Opened by Init().
	 * \param nTrackType [in] The sample type of the track, it will be audio/video/closed caption
	 * \param pSample [out] The sample of the stream, all buffer in the sample( include reserve field ) will be mantained by source, they will be available until you call GetSample again
	 *                       the param type is depended on the nTrackType, for VOOSMP_SS_AUDIO & VOOSMP_SS_VIDEO please use VOOSMP_BUFFERTYPE
	 *                                                                      for VOOSMP_SS_SUBTITLE please use voSubtitleInfo
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetSample) (void* pHandle, int nTrackType, void* pSample);	
	
	
	/**
	 * Select the Program
	 * \param hHandle [in] The PLAYER handle. Opened by Init().
	 * \param nProgram [in] Program ID
     * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* SelectProgram) (void* pHandle, int nProgram);
	
	
	/**
	 * Select the Stream
	 * \param hHandle [in] The PLAYER handle. Opened by Init().
	 * \param nStream [in] Stream ID, -1 indicates auto select
     * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* SelectStream) (void* pHandle, int nStream);
    
    /**
     * Select the Track
     * \param hHandle [in] The PLAYER handle. Opened by Init().
     * \param nTrack [in] Track ID
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* SelectTrack) (void* pHandle, int nTrack);
	
    /**
     * Select the subtitle language
     * \param hHandle [in] The PLAYER handle. Opened by Init().
     * \param nIndex [in] The index of language
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* SelectLanguage) (void* pHandle, int nIndex);
    
    /**
     * Get the subtitle language list
     * \param hHandle [in] The PLAYER handle. Opened by Init().
     * \param ppLangInfo [out] The language info list
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* GetLanguage) (void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);


	/**
	 * Close source
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Close) (void* pHandle);

	
	/**
	* Start to play
	* \param pHandle [in] The PLAYER handle. Created by Init().
	* \retval VOOSMP_ERR_None Succeeded
	*/
	int (* Run) (void* pHandle);

	
	/**
	 * Pause
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Pause) (void* pHandle);

	
	/**
	* Stop playing
	* \param pHandle [in] The PLAYER handle. Created by Init().
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* Stop) (void* pHandle);
	
	
	/**
	 * Get current playing position
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \retval return current position,-1 if fialed 
	 */
	int (* GetPos) (void* pHandle);
	
	
	/**
	 * Seek operation
	 * \param pHandle [in] The PLAYER handle. Created by Init().
	 * \param nPos [in] The position of seeking.
	 * \retval The correct position after seek operation, -1 if seek failed
	 */
	int (* SetPos) (void* pHandle, int nPos);
	
	/**
	 * Get Duration of the stream
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param pDuration [out] The duration of the stream
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetDuration) (void* pHandle, long long* pDuration);	
	
	
	/**
	* Get the special value from param ID
	* \param pHandle [in] The PLAYER handle. Created by Init().
	* \param nParamID [in] The param ID
	* \param pValue [out] The return value depend on the param ID.
	* \retval VOOSMP_ERR_None Succeeded
	*/
	int (* GetParam) (void* pHandle, int nParamID, void* pValue);


	/**
	* Set the special value from param ID
	* \param pHandle [in] The PLAYER handle. Created by Init().
	* \param nParamID [in] The param ID
	* \param pValue [in] The set value depend on the param ID.
	* \retval VOOSMP_ERR_None Succeeded 
	*/
	int (* SetParam) (void* pHandle, int nParamID, void* pValue);
}voOnStreamMediaPlayerAPI;


/**
* Get OnStream PLAYER API interface
* \param pFuncSet [in/out] Return the PLAYER API set.
* \retval VOOSMP_ERR_None Succeeded.
*/
int voGetOnStreamMediaPlayerAPI(voOnStreamMediaPlayerAPI* pFuncSet);

typedef int (* VOGETONSTREAMPLAYERAPI)(voOnStreamMediaPlayerAPI* pFuncSet);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_ONSTREAM_MP_API_H_ */


