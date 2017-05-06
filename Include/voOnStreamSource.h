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


#ifndef __VOOSMP_SOURCE_H__
#define __VOOSMP_SOURCE_H__

#include "voOnStreamSourceType.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
    /**
	 * Initial a source session and Set the Source( url etc. ) into the module
	 * \param ppHandle [out] source handle.
	 * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* Create) (void** ppHandle);
    
    
    /**
	 * Destroy a source session
	 * \param ppHandle [in] source handle.
	 * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* Destroy) (void* ppHandle);
    
    
	/**
	 * Initial a source session and Set the Source( url etc. ) into the module
	 * For this function please only copy the pSource param, and cache the callback, do not do anything else
	 * This function should be called first for a session.
	 * \param ppHandle [out] source handle.
	 * \param pSource [in] source file description, should be an url or fd etc.
	 * \param nSourceFlag [in] The flag for open source,refer to VOOSMP_OPEN_SOURCE_FLAG
	 * \param nSourceType [in] indicate the source format, refer to VOOSMP_SRC_SOURCE_FORMAT
	 * \param pInitParam [in] Initial param,depend on nInitParamFlag,refer to VOOSMP_INIT_PARAM.
	 * \param nInitParamFlag [in] Initial param flag,refer to VOOSMP_INIT_PARAM_FLAG.
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Init) (void* ppHandle, void* pSource, int nSourceFlag, int nSourceType, void* pInitParam, int nInitParamFlag);

	
	/**
	 * Uninitial a source session
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Uninit) (void* pHandle);

	
	/**
	 * Check and analyze the stream, get information
	 * For this function, please do not start to parser the media data
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Open) (void* pHandle);

	
	/**
	 * Destroy the information of the stream
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Close) (void* pHandle);

	
	/**
	 * Start to parse the media data
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Run) (void* pHandle);

	
	/**
	 * Pause
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Pause) (void* pHandle);

	
	/**
	 * Stop
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* Stop) (void* pHandle);

	
	/**
	 * Seek
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param pTimeStamp [in/out] The seek pos and return the available pos after seek
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* SetPos) (void* pHandle, long long* pTimeStamp);

	
	/**
	 * Get Duration of the stream
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param pDuration [out] The duration of the stream
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetDuration) (void* pHandle, long long* pDuration);

	
	/**
	 * Get sample from the source
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param nTrackType [in] The sample type of the stream, it will be audio/video/closed caption
	 * \param pSample [out] The sample of the stream, all buffer in the sample( include reserve field ) will be mantained by source, they will be available until you call GetSample again
	 *                       the param type is depended on the nTrackType, for VOOSMP_SS_AUDIO & VOOSMP_SS_VIDEO please use VOOSMP_BUFFERTYPE
	 *                                                                      for VOOSMP_SS_SUBTITLE please use voSubtitleInfo
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetSample) (void* pHandle, int nTrackType, void* pSample);

	
	/**
	 * It retrieve how many program in the live stream
	 * For program, it should be defined as: all sub streams that share the same angle for video and same language for audio
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param pProgramCount [out] The number of the program
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetProgramCount) (void* pHandle, int* pProgramCount);

	
	/**
	 * Enum all the program info based on stream count
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param nProgram  [in] The program sequence based on the program counts
	 * \param ppProgramInfo [out] return the info of the Program, source will keep the info structure available until you call close
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetProgramInfo) (void* pHandle, int nProgram, VOOSMP_SRC_PROGRAM_INFO** ppProgramInfo);

	
	/**
	 * Get the track info we selected
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param nTrackType [in] The sample type of the stream, it will be audio/video/closed caption
	 * \param pTrackInfo [out] The trackinfo of the selected track. The trackinfo memory will be mantained in source until you call close
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetCurTrackInfo) (void* pHandle, int nTrackType, VOOSMP_SRC_TRACK_INFO** ppTrackInfo);
	
	
	/**
	 * Select the Program
	 * \param hHandle [in] The source handle. Opened by Init().
	 * \param nProgram [in] Program ID
     * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* SelectProgram) (void* pHandle, int nProgram);
	
	/**
	 * Select the Stream
	 * \param hHandle [in] The source handle. Opened by Init().
	 * \param nStream [in] Stream ID
     * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* SelectStream) (void* pHandle, int nStream);
    
    /**
     * Select the Track
     * \param hHandle [in] The source handle. Opened by Init().
     * \param nTrack [in] Track ID
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* SelectTrack) (void* pHandle, int nTrack);

    /**
     * Select the subtitle language
     * \param hHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of language
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* SelectLanguage) (void* pHandle, int nIndex);
    
    /**
     * Get the subtitle language list
     * \param hHandle [in] The source handle. Opened by Init().
     * \param ppLangInfo [out] The language info list
     * \retval VOOSMP_ERR_None Succeeded
     */
    int (* GetLanguage) (void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);
	
	/**
	* Send the buffer into the source, if you want to use this function, you should use flag VOOSMP_FLAG_SOURCE_SENDBUFFER
	* \param pHandle [in] The source handle. Opened by Init().
	* \param buffer [in] The buffer send to the source
	* \retval VOOSMP_ERR_None Succeeded
	*/
	int (* SendBuffer) (void* pHandle, const VOOSMP_BUFFERTYPE& buffer);

	
	/**
	 * Get the special value from param ID
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param nParamID [in] The param ID
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* GetParam) (void* pHandle, int nParamID, void* pParam);

	
	/**
	 * Set the special value from param ID
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \param nParamID [in] The param ID
	 * \param pParam [in] The set value depend on the param ID.
	 * \retval VOOSMP_ERR_None Succeeded
	 */
	int (* SetParam) (void* pHandle, int nParamID, void* pParam);
    
    
 	/**
	 * Get video track count
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval count of video track
	 */
    int (* GetVideoCount) (void* pHandle);
    
 	/**
	 * Get audio track count
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval count of audio track
	 */
    int (* GetAudioCount) (void* pHandle);
    
 	/**
	 * Get subtitle track count
	 * \param pHandle [in] The source handle. Opened by Init().
	 * \retval count of subtitle track
	 */
    int (* GetSubtitleCount) (void* pHandle);
    
 	/**
	 * Select video track
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
	 * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* SelectVideo) (void* pHandle, int nIndex);
    
 	/**
	 * Select audio track
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
	 * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* SelectAudio) (void* pHandle, int nIndex);
    
  	/**
	 * Select subtitle track
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
	 * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* SelectSubtitle) (void* pHandle, int nIndex);
   
    /**
	 * Check if video track is available
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
	 * \retval true Succeeded
	 */
    bool (* IsVideoAvailable) (void* pHandle, int nIndex);
    
    /**
	 * Check if audio track is available
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
	 * \retval true Succeeded
	 */
    bool (* IsAudioAvailable) (void* pHandle, int nIndex);
    
    /**
	 * Check if subtitle track is available
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
	 * \retval true Succeeded
	 */
    bool (* IsSubtitleAvailable) (void* pHandle, int nIndex);
    
    /**
	 * Commit all current selections
	 * \param pHandle [in] The source handle. Opened by Init().
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* CommitSelection) (void* pHandle);
    
    /**
	 * Remove all current uncommitted selections
	 * \param pHandle [in] The source handle. Opened by Init().
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* ClearSelection) (void* pHandle);

    /**
	 * Get the properties of specified video stream.
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
     * \param pProperty [out] The properties of track
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* GetVideoProperty) (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    
    /**
	 * Get the properties of specified audio stream.
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
     * \param pProperty [out] The properties of track
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* GetAudioProperty) (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    
    /**
	 * Get the properties of specified subtitle stream.
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param nIndex [in] The index of track
     * \param pProperty [out] The properties of track
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* GetSubtitleProperty) (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    
    /**
	 * Retrieve the assets which is currently being played or default assets before playing
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param pCurrIndex [in/out] The current selected index
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* GetCurrTrackSelection) (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
	
    /**
	 * Retrieve the assets which is currently being playing
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param pCurrIndex [in/out] The current selected index
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* GetCurrPlayingTrackIndex) (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

    /**
	 * Retrieve the assets which is currently being selected or default assets before playing
	 * \param pHandle [in] The source handle. Opened by Init().
     * \param pCurrIndex [in/out] The current selected index
     * \retval VOOSMP_ERR_None Succeeded
	 */
    int (* GetCurrSelectedTrackIndex) (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

}voOSMPSourceAPI;
	
	
/**
 * Get OnStream Source API interface
 * \param pFuncSet [in/out] Return the Source API set.
 * \retval VOOSMP_ERR_None Succeeded.
 */
int voGetOnStreamSourceAPI(voOSMPSourceAPI* pFuncSet);

typedef int (* VOGETONSTREAMSOURCEAPI) (voOSMPSourceAPI* pFuncSet);
	

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif // end __VOOSMP_SOURCE_H__