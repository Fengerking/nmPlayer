//
//  CAVPlayer.h
//  
//
//  Created by Jim Lin on 9/29/12.
//
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudioTypes.h>
#include "voNPWrap.h"

@interface CAVPlayer : NSObject
{
    AVPlayer*           m_pPlayer;
    AVPlayerItem*       m_pPlaybackItem;
    UIView*             m_pView;
    char                m_szPlaybackURL[1024];
    
    id                  m_timeObserver;
    VONP_LISTENERINFO*  m_pListener;
    int                 m_nBufTime;
    bool                m_bPause;
}

- (int)initPlayer;
- (int)uninit;
- (int)open:(unsigned char*)pszURL;
- (int)close;
- (int)run;
- (int)pause;
- (int)replaceURL:(unsigned char*)pszURL;
- (int)setView:(UIView*)pView;

- (int)doReplaceURL:(unsigned char*)pszNewURL;
- (int)doPlay;
- (void)updatePlayingTime;
- (void)setListener:(VONP_LISTENERINFO*)pListener;
- (int)enableTrack:(bool)bAudio Enable:(bool)bEnable;
- (int)getBufTime;
- (int)getCurrBufTime;
- (void)setVolume:(int)nVolume;
@end
