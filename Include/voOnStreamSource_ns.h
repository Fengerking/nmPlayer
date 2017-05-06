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

#import <Foundation/Foundation.h>

#include "voOnStreamType.h"
#include "voOnStreamSourceType.h"

@protocol voOnStreamSourceDelegate <NSObject>
@optional
- (int)voHandleSourceEvent:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;
@end

@interface voOnStreamSource_ns : NSObject
{
    void * m_pOSHandle;
    void * m_pOSApiSet;
    id <voOnStreamSourceDelegate> m_pDelegate;
    VOOSMP_LISTENERINFO m_cInfo;
}

/**
 * Initial a source session and Set the Source( url etc. ) into the module
 * For this function please only copy the pSource param, and cache the callback, do not do anything else
 * This function should be called first for a session.
 * \param pSource [in] source file description, should be an url or fd etc.
 * \param nSourceFlag [in] The flag for open source,refer to VOOSMP_OPEN_SOURCE_FLAG
 * \param nSourceType [in] indicate the source format, refer to VOOSMP_SRC_SOURCE_FORMAT
 * \param pInitParam [in] Initial param,depend on nInitParamFlag.
 * \param nInitParamFlag [in] Initial param flag,refer to VOOSMP_INIT_PARAM_FLAG.
 * \retval VOOSMP_ERR_None Succeeded
 */
- (id) init:(void*)pSource nSourceFlag:(int)nSourceFlag nSourceType:(int)nSourceType pInitParam:(void*)pInitParam nInitParamFlag:(int)nInitParamFlag;


/**
 * Set the delegate for handle event, the event is post by sub-thread.
 * \param delegate [in] The id which will handle event
 */
- (void)setDelegate:(id <voOnStreamSourceDelegate>)delegate;


/**
 * Set source URL
 * \param pSource [in] The Data source URL
 * \param nFlag [in] The flag for open source,refer to VOOSMP_OPEN_SOURCE_FLAG
 * \param nSourceType [in] indicate the source format, refer to VOOSMP_SRC_SOURCE_FORMAT
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) open;


/**
 * Close source
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) close;


/**
 * Start to play
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) run;


/**
 * Pause
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) pause;


/**
 * Stop playing
 * \retval VOOSMP_ERR_None Succeeded 
 */
- (int) stop;


/**
 * Seek operation
 * \param nPos [in] The position of seeking.
 * \retval The correct position after seek operation, -1 if seek failed
 */
- (int) setPos:(long long*)pTimeStamp;


/**
 * Get Duration of the stream
 * \param pDuration [out] The duration of the stream
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) getDuration:(long long*)pDuration;


/**
 * Get sample from the source
 * \param nTrackType [in] The sample type of the stream, it will be audio/video/closed caption
 * \param pSample [out] The sample of the stream, all buffer in the sample( include reserve field ) will be mantained by source, they will be available until you call GetSample again
 *                       the param type is depended on the nTrackType, for VOOSMP_SS_AUDIO & VOOSMP_SS_VIDEO please use VOOSMP_BUFFERTYPE
 *                                                                      for VOOSMP_SS_SUBTITLE please use voSubtitleInfo
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) getSample:(int)nTrackType pSample:(void*)pSample;


/**
 * It retrieve how many program in the live stream
 * For Stream, it should be defined as: all sub streams that share the same angle for video and same language for audio
 * \param pProgramCount [out] The number of the program
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) getProgramCount:(int *)pProgramCount;


/**
 * Enum all the program info based on program count
 * \param nProgramIndex  [in] The program sequence based on the program counts
 * \param ppProgramInfo [out] return the info of the program, source will keep the info structure available until you call close
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) getProgramInfo:(int)nProgramIndex ppProgramInfo:(VOOSMP_SRC_PROGRAM_INFO**)ppProgramInfo;


/**
 * Get the selected track info
 * \param pHandle [in] The PLAYER handle. Created by Init().
 * \param nTrackType [in] The sample type of the stream, it will be audio/video/closed caption
 * \param ppTrackInfo [out] The trackinfo of the selected track. The trackinfo memory will be mantained in source until you call close
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) getCurTrackInfo:(int)nTrackType ppTrackInfo:(VOOSMP_SRC_TRACK_INFO**)ppTrackInfo;


/**
 * Select the Program
 * \param nProgram [in] Program ID
 */
- (int) selectProgram:(int)nProgram;


/**
 * Select the Stream
 * \param nStream [in] Stream ID
 */
- (int) selectStream:(int)nStream;


/**
 * Select the Track
 * \param nTrack [in] Track ID
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) selectTrack:(int)nTrack;


/**
 * Select the subtitle language
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of language
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) selectLanguage:(int)nIndex;


/**
 * Get the subtitle language list
 * \param hHandle [in] The source handle. Opened by Init().
 * \param ppLangInfo [out] The language info list
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) getLanguage:(VOOSMP_SUBTITLE_LANGUAGE_INFO**)ppLangInfo;


/**
 * Send the buffer into the source, if you want to use this function, you should use flag VOOSMP_FLAG_SOURCE_SENDBUFFER
 * \param buffer [in] The buffer send to the source
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) sendBuffer:(const VOOSMP_BUFFERTYPE&)buffer;


/**
 * Get the special value from param ID
 * \param nParamID [in] The param ID
 * \param pValue [out] The return value depend on the param ID.
 *  \retval VOOSMP_ERR_None Succeeded
 */
- (int) getParam:(int)nParamID pValue:(void *)pValue;


/**
 * Set the special value from param ID
 * \param pHandle [in] The ENGINE handle. Created by Init().
 * \param nParamID [in] The param ID
 * \param pValue [in] The set value depend on the param ID.
 * \retval VOOSMP_ERR_None Succeeded 
 */
- (int) setParam:(int)nParamID pValue:(void *)pValue;

@end

