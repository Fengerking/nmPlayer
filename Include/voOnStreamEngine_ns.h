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

#import <Foundation/Foundation.h>
#import "voOnStreamType.h"

@protocol voOnStreamEngineDelegate <NSObject>
@optional
- (int)voHandleEngineEvent:(int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;
@end

@interface voOnStreamEngine_ns : NSObject
{
    void * m_pOSHandle;
    void * m_pOSApiSet;
    id <voOnStreamEngineDelegate> m_pDelegate;
}

/**
 * Initialize ENGINE instance.
 * This function should be called first for a session.
 * \param nPlayerType [in] The type of media framework.refer to VOOSMP_PLAYER_TYPE
 * \param pInitParam [in] Initial param,depend on nInitParamFlag.
 * \param nInitParamFlag [in] Initial param flag,refer to VOOSMP_INIT_PARAM_FLAG.
 * \retval id 
 */
- (id) init:(int)nPlayerType pInitParam:(void *)pInitParam nInitParamFlag:(int)nInitParamFlag;


/**
 * Set the delegate for handle event, the event is post by sub-thread.
 * \param delegate [in] The id which will handle event
 */
- (void)setDelegate:(id <voOnStreamEngineDelegate>)delegate;


/**
 * Set the video view
 * \param pView [in] The handle of view
 * \retval VOOSMP_ERR_None Succeeded 
 */
- (int) setView:(void *)pView;


/**
 * Set source URL
 * \param pHandle [in] The ENGINE handle. Created by Init().
 * \param pSource [in] The Data source URL
 * \param nFlag [in] The flag for open source,refer to VOOSMP_OPEN_SOURCE_FLAG
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) open:(void *)pSource nFlag:(int)nFlag;


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
 * Get current playing position
 * \retval return current position,-1 if fialed 
 */
- (int) getPos;
	
	
/**
 * Seek operation
 * \param nPos [in] The position of seeking.
 * \retval The correct position after seek operation, -1 if seek failed
 */
- (int) setPos:(int)nPos;


/**
 * Select the subtitle language
 * \param nIndex [in] The index of language
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) selectLanguage:(int)nIndex;


/**
 * Get the subtitle language list
 * \param ppLangInfo [out] The language info list
 * \retval VOOSMP_ERR_None Succeeded
 */
- (int) getLanguage:(VOOSMP_SUBTITLE_LANGUAGE_INFO**)ppLangInfo;


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

