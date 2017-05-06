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
/************************************************************************
 * @file voAudioRenderType.h
 * Audio render type definition
 *
 * Audio render type definition
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef __VO_AUDIO_RENDER_TYPE_H__
#define __VO_AUDIO_RENDER_TYPE_H__

/*!
 @enum           AudioSession interruptions states
 @abstract       These are used with the AudioSessionInterruptionListener to indicate
 if an interruption begins or ends.
 @constant       voAudioSessionBeginInterruption
 Indicates that this AudioSession has just been interrupted.
 @constant       voAudioSessionEndInterruption
 Indicates the end of an interruption.
 */
enum {
    voAudioSessionBeginInterruption  = 1,
    voAudioSessionEndInterruption    = 0
};


typedef void (* VO_AUDIO_SESSION_LISTENER) (void* pUserData, int nID, void *pParam1, void *pParam2);
typedef struct
{
    VO_AUDIO_SESSION_LISTENER   pListener;
    void*			            pUserData;
}voAudioSessionListenerInfo;

#endif