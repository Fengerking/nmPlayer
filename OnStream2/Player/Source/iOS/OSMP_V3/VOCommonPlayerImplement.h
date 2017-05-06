/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All rights Reserved
 
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

#import "VOCommonPlayerDelegate.h"
#import "VOCommonPlayer.h"
#import "voOSMediaPlayer.h"

@interface VOCommonPlayerImplement : NSObject<VOCommonPlayer>
{
@private
    int _volumeValue;
    
    VOOSMP_SUBTITLE_SETTINGS _subtitleSetting;
    voOSMediaPlayer* _player;
    NSTimer *_timer;
    bool _enableDVRPosition;
    
    id <VOCommonPlayerDelegate> _playerDelegate;
}

@end
