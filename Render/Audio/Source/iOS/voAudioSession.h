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
 * @file voAudioSession.h
 * Audio Session
 *
 * Audio Session
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import <Foundation/Foundation.h>
#include "voAudioRenderType.h"

@interface voAudioSession : NSObject
{
    voAudioSessionListenerInfo    m_cInfoCB;
}

-(id) init:(voAudioSessionListenerInfo *)pInfo;
-(void) setDelegateCB:(voAudioSessionListenerInfo *)pInfo;
-(void) OnAudioSessionInterruptionListener:(UInt32)inInterruptionState;

@end
