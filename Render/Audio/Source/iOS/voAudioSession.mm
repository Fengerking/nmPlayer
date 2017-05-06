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
 * @file voAudioSession.mm
 * Audio Session
 *
 * Audio Session
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

#ifdef _IOS
#import <UIKit/UIKit.h>
#endif

#import "voAudioSession.h"
#import "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

void S_voAudioSessionInterruptionListener(void *inClientData, UInt32 inInterruptionState)
{
	voAudioSession *pFactory = (voAudioSession *)inClientData;
	
	if (NULL == pFactory) {
		VOLOGE("S_AudioSessionInterruptionListener pFactory null\n");
		return;
	}
	
	[pFactory OnAudioSessionInterruptionListener:inInterruptionState];
}

#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

@interface voAudioSession ()
// Properties that don't need to be seen by the outside world.
-(void) handleInterruption:(NSNotification *)notification;
-(void) handleRouteChange:(NSNotification *)notification;

@end

@implementation voAudioSession

- (id) init
{
    return [self init:nil];
}

- (id) init:(voAudioSessionListenerInfo *)pInfo
{
    self = [super init];
    
    if (nil != self) {
        
        memset(&m_cInfoCB, 0, sizeof(m_cInfoCB));
        
        if (NULL != pInfo) {
            memcpy(&m_cInfoCB, pInfo, sizeof(m_cInfoCB));
        }
        
        float osVersion = [[UIDevice currentDevice].systemVersion floatValue];
        if (osVersion >=6.0)
        {
            NSError *error = nil;
            
            AVAudioSession *sessionInstance = [AVAudioSession sharedInstance];
            
            [sessionInstance setCategory:AVAudioSessionCategoryPlayback error:&error];
            
            if (noErr != error.code) {
                VOLOGE("couldn't set audio category :%d", error.code);
            }
            else {
                // add the interruption handler
                [[NSNotificationCenter defaultCenter] addObserver:self
                                                         selector:@selector(handleInterruption:)
                                                             name:AVAudioSessionInterruptionNotification
                                                           object:sessionInstance];
                
                // we don't do anything special in the route change notification
                [[NSNotificationCenter defaultCenter] addObserver:self
                                                         selector:@selector(handleRouteChange:)
                                                             name:AVAudioSessionRouteChangeNotification
                                                           object:sessionInstance];
            }
        }
        else {
            // Only init once
            OSStatus result = AudioSessionInitialize(NULL, NULL, S_voAudioSessionInterruptionListener, self);
            if (result == kAudioSessionNoError)
            {
                UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
                AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory), &sessionCategory);
                VOLOGI("voAudioRenderFactory kAudioSessionNoError\n");
            }
            else {
                VOLOGE("voAudioRenderFactory init error:%ld\n", result);
            }
        }
    }
    
    memset(&m_cInfoCB, 0, sizeof(m_cInfoCB));
    
    return self;
}

-(void) setDelegateCB:(voAudioSessionListenerInfo *)pInfo
{
    if (NULL == pInfo) {
        return;
    }
    
    memcpy(&m_cInfoCB, pInfo, sizeof(m_cInfoCB));
}

-(void) handleRouteChange:(NSNotification *)notification
{
    UInt8 reasonValue = [[notification.userInfo valueForKey:AVAudioSessionRouteChangeReasonKey] intValue];
    AVAudioSessionRouteDescription *routeDescription = [notification.userInfo valueForKey:AVAudioSessionRouteChangePreviousRouteKey];
    
    VOLOGI("Route change:\n");
    
    switch (reasonValue) {
        case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
            VOLOGI("NewDeviceAvailable");
            break;
        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
            VOLOGI("OldDeviceUnavailable");
            break;
        case AVAudioSessionRouteChangeReasonCategoryChange:
            VOLOGI("CategoryChange");
            break;
        case AVAudioSessionRouteChangeReasonOverride:
            VOLOGI("Override");
            break;
        case AVAudioSessionRouteChangeReasonWakeFromSleep:
            VOLOGI("WakeFromSleep");
            break;
        case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory:
            VOLOGI("NoSuitableRouteForCategory");
            break;
        default:
            VOLOGI("ReasonUnknown");
    }
    
    NSString *str = [NSString stringWithFormat:@"%@", routeDescription];
    if (nil != str) {
        VOLOGI("routeDescription: %s", [str UTF8String]);
    }
}

-(void) handleInterruption:(NSNotification *)notification
{
    UInt8 theInterruptionType = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue];
    
    VOLOGI("Session interrupted! --- %s ---\n", theInterruptionType == AVAudioSessionInterruptionTypeBegan ? "Begin Interruption" : "End Interruption");
    
    int nID = voAudioSessionEndInterruption;
    
    if (theInterruptionType == AVAudioSessionInterruptionTypeBegan) {
        nID = voAudioSessionBeginInterruption;
    }
    else if (theInterruptionType == AVAudioSessionInterruptionTypeEnded) {
//        // make sure we are again the active session
//        [[AVAudioSession sharedInstance] setActive:YES error:nil];
        nID = voAudioSessionEndInterruption;
    }
    
    if (NULL != m_cInfoCB.pListener) {
        m_cInfoCB.pListener(m_cInfoCB.pUserData, nID, 0, 0);
    }
}

-(void) OnAudioSessionInterruptionListener:(UInt32)inInterruptionState
{
    int nID = voAudioSessionEndInterruption;
    
    if (kAudioSessionBeginInterruption == inInterruptionState) {
        nID = voAudioSessionBeginInterruption;
    }
    else {
        nID = voAudioSessionEndInterruption;
    }
    
    if (NULL != m_cInfoCB.pListener) {
        m_cInfoCB.pListener(m_cInfoCB.pUserData, nID, 0, 0);
    }
}

@end
