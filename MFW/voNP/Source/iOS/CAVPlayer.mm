//
//  CAVPlayer.mm
//  
//
//  Created by Jim Lin on 9/29/12.
//
//

#import "CAVPlayer.h"
#include "CBaseNativePlayer.h"
#include "voOSFunc.h"

#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


@implementation CAVPlayer

NSString * const kTracksKey         = @"tracks";
NSString * const kPlayableKey		= @"playable";
NSString * const kStatusKey         = @"status";
NSString * const kTimeRangesKVO     = @"loadedTimeRanges";
NSString * const kPlaybackBufferEmpty     = @"playbackBufferEmpty";
NSString * const kPlaybackLikelyToKeepUp     = @"playbackLikelyToKeepUp";

static void *AVPlayerPlaybackViewControllerRateObservationContext = &AVPlayerPlaybackViewControllerRateObservationContext;
static void *AVPlayerPlaybackViewControllerStatusObservationContext = &AVPlayerPlaybackViewControllerStatusObservationContext;
static void *AVPlayerPlaybackViewControllerCurrentItemObservationContext = &AVPlayerPlaybackViewControllerCurrentItemObservationContext;
static void *AVPlayerPlaybackViewControllerPlaybackBufferObservationContext = &AVPlayerPlaybackViewControllerPlaybackBufferObservationContext;

- (NSTimeInterval) availableDuration
{
    if(!m_pPlayer)
        return 0;
    
    NSArray *loadedTimeRanges = [[m_pPlayer currentItem] loadedTimeRanges];
    CMTimeRange timeRange = [[loadedTimeRanges objectAtIndex:0] CMTimeRangeValue];
    float startSeconds = CMTimeGetSeconds(timeRange.start);
    float durationSeconds = CMTimeGetSeconds(timeRange.duration);
    NSTimeInterval result = startSeconds + durationSeconds;
    return result*1000;
}

- (int)initPlayer
{
    m_pPlayer           = nil;
    m_pPlaybackItem     = nil;
    m_pView             = nil;
    m_timeObserver      = nil;
    m_pListener         = nil;
    m_nBufTime          = 0;
    
    return VONP_ERR_None;
}

- (int)uninit
{
    [self close];
    return VONP_ERR_None;
}

- (int)open:(unsigned char*)pszURL
{
    memset(m_szPlaybackURL, 0, 1024);
    strcpy(m_szPlaybackURL, (const char*)pszURL);
    
    // Category will be set in OSEngine
    NSError *err = nil;
    if (NO == [[AVAudioSession sharedInstance] setActive:YES error:&err]) {
        VOLOGE("setActive error:%d", err.code);
    }

    return VONP_ERR_None;
}

- (int)close
{
    m_pListener = nil;
    
    if(m_pView)
    {
        [(AVPlayerLayer *)[m_pView layer] setPlayer: nil];
        [m_pView release];
        m_pView = nil;
    }
    
    if(m_pPlayer)
    {
        [m_pPlayer removeTimeObserver: m_timeObserver];
        
        //VOLOGI("[NPW]Release AVPlayer");
        [m_pPlayer release];
        
        m_pPlayer         = nil;
        m_pPlaybackItem   = nil;
    }
    
    NSError *err = nil;
    if (NO == [[AVAudioSession sharedInstance] setActive:NO error:&err]) {
        VOLOGE("setActive error:%d", err.code);
    }
    
    VOLOGI("[NPW]AVPlayer destroyed %x", (unsigned int)self);
    
    return VONP_ERR_None;
}

- (int)run
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    if(!m_pPlayer)
    {
        NSString* pURL = [[NSString alloc] initWithUTF8String: m_szPlaybackURL];
        NSURL *sourceURL = [NSURL URLWithString: pURL];
        
        AVURLAsset *sourceAsset = [AVURLAsset URLAssetWithURL:sourceURL options:nil];
        
        m_pPlaybackItem     = [AVPlayerItem playerItemWithAsset:sourceAsset];
        
        [m_pPlaybackItem addObserver:self
                                forKeyPath:kStatusKey
                                   options:NSKeyValueObservingOptionInitial | NSKeyValueObservingOptionNew
                                   context:AVPlayerPlaybackViewControllerStatusObservationContext];

        [m_pPlaybackItem addObserver:self forKeyPath:kPlaybackBufferEmpty options:NSKeyValueObservingOptionNew context:AVPlayerPlaybackViewControllerPlaybackBufferObservationContext];
        [m_pPlaybackItem addObserver:self forKeyPath:kPlaybackLikelyToKeepUp options:NSKeyValueObservingOptionNew context:AVPlayerPlaybackViewControllerPlaybackBufferObservationContext];
        
//        [m_pPlaybackItem addObserver:self
//                    forKeyPath:kTimeRangesKVO
//                       options:NSKeyValueObservingOptionInitial | NSKeyValueObservingOptionNew
//                       context:AVPlayerPlaybackViewControllerCurrentItemObservationContext];

        m_pPlayer           = [AVPlayer playerWithPlayerItem: (AVPlayerItem*)m_pPlaybackItem];
        
        m_pPlayer.closedCaptionDisplayEnabled = NO;
        
        
        if(m_pView)
        {
            AVPlayerLayer* layer = (AVPlayerLayer *)[m_pView layer];
            [layer setPlayer: m_pPlayer];
            
            [CATransaction begin];
            [CATransaction setAnimationDuration:0];
            [CATransaction setDisableActions:YES];
            layer.videoGravity = AVLayerVideoGravityResize;
            [CATransaction commit];
        }
        
        [pURL release];
    }
    
    [self doPlay];
    
    [pool release];

    return VONP_ERR_None;
}

- (int)pause
{
    m_bPause = true;
    if(m_pPlayer)
        [m_pPlayer pause];
    return VONP_ERR_None;
}

- (int)replaceURL:(unsigned char*)pszURL
{
    [self doReplaceURL: pszURL];
    
    return VONP_ERR_None;
}


- (int)doReplaceURL:(unsigned char*)pszNewURL
{
    if(m_pPlayer)
    {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        
        NSString* pURL = [[NSString alloc] initWithUTF8String: m_szPlaybackURL];
        NSURL *sourceURL = [NSURL URLWithString: pURL];
        
        AVURLAsset *sourceAsset = [AVURLAsset URLAssetWithURL:sourceURL options:nil];
        
        AVPlayerItem* pPlaybackItem     = [AVPlayerItem playerItemWithAsset:sourceAsset];
        
        [m_pPlayer replaceCurrentItemWithPlayerItem: pPlaybackItem];
        
        [pURL release];
        
        [self doPlay];
        
        [pool release];
        
        return VONP_ERR_None;
    }

    return VONP_ERR_None;
}

- (int)doPlay
{
    m_bPause = false;
    
    if(m_pPlayer)
        [m_pPlayer play];
    
    m_timeObserver = [[m_pPlayer addPeriodicTimeObserverForInterval:CMTimeMakeWithSeconds(0.1, NSEC_PER_SEC)
                                                                     queue:NULL
                                                                usingBlock:
                     ^(CMTime time)
                     {
                         [self updatePlayingTime];
                     }] retain];

    
    return VONP_ERR_None;
}

- (void)updatePlayingTime
{
    if (m_pPlayer && CMTimeGetSeconds([m_pPlayer currentTime]))
    {
        CMTime cTime = [m_pPlayer currentTime];
        int nCurrPos = (cTime.value*1000) / cTime.timescale;
        
        m_nBufTime = [self getCurrBufTime];
        
        if(m_pListener)
        {
            if(m_pListener->pListener && m_pListener->pUserData && !m_bPause)
                m_pListener->pListener(m_pListener->pUserData, NATIVE_PLAYER_EVENT_POS_UPDATE, (void*)&nCurrPos, &m_nBufTime);
        }
        
//        static int Time = voOS_GetSysTime();
//        VOLOGI("[NPW]Playing time %d, buffer %f, %lu", nCurrPos, [self availableDuration], voOS_GetSysTime()-Time);
//        Time = voOS_GetSysTime();
        
        return;
    }
}

- (int)setView:(UIView*)pView
{
    if (m_pView != pView) {
        
        if (m_pView) {
            [m_pView release];
        }
        
        m_pView = pView;
        [m_pView retain];
    }
    
    return 0;
}

- (void)setListener:(VONP_LISTENERINFO*)pListener
{
    m_pListener = pListener;
}

- (void)observeValueForKeyPath:(NSString*) path
                      ofObject:(id)object
                        change:(NSDictionary*)change
                       context:(void*)context
{
	/* AVPlayerItem "status" property value observer. */
	if (context == AVPlayerPlaybackViewControllerStatusObservationContext)
	{
        AVPlayerStatus status = [[change objectForKey:NSKeyValueChangeNewKey] integerValue];
        switch (status)
        {
                /* Indicates that the status of the player is not yet known because
                 it has not tried to load new media resources for playback */
            case AVPlayerStatusUnknown:
            {
                VOLOGW("[NPW]AVPLayer status unknown.");
            }
                break;
                
            case AVPlayerStatusReadyToPlay:
            {
                if(m_pListener)
                {
                    if(m_pListener->pListener && m_pListener->pUserData)
                        m_pListener->pListener(m_pListener->pUserData, NATIVE_PLAYER_EVENT_START_RUN, NULL, NULL);
                }
            }
                break;
                
            case AVPlayerStatusFailed:
            {
                VOLOGE("[NPW]AVPLayer status failed!!!");
            }
                break;
        }
	}
    else if (context == AVPlayerPlaybackViewControllerPlaybackBufferObservationContext)
    {
        if (!m_pPlayer)
        {
            return;
        }
        else if (object == m_pPlaybackItem && [path isEqualToString:kPlaybackBufferEmpty])
        {
            if (m_pPlaybackItem.playbackBufferEmpty) {
                
                if (m_pListener && m_pListener->pListener && m_pListener->pUserData) {
                    m_pListener->pListener(m_pListener->pUserData, NATIVE_PLAYER_EVENT_START_BUFFER, 0, 0);
                }
            }
        }
        else if (object == m_pPlaybackItem && [path isEqualToString:kPlaybackLikelyToKeepUp])
        {
            if (m_pPlaybackItem.playbackLikelyToKeepUp)
            {
                if (m_pListener && m_pListener->pListener && m_pListener->pUserData) {
                    m_pListener->pListener(m_pListener->pUserData, NATIVE_PLAYER_EVENT_STOP_BUFFER, 0, 0);
                }
            }
        }
    }
	/* AVPlayer "rate" property value observer. */
	else if (context == AVPlayerPlaybackViewControllerRateObservationContext)
	{
	}
	/* AVPlayer "currentItem" property observer.
     Called when the AVPlayer replaceCurrentItemWithPlayerItem:
     replacement will/did occur. */
	else if (context == AVPlayerPlaybackViewControllerCurrentItemObservationContext)
	{
//        VOLOGI("[NPW]AVPlayerItem event %d", (int)[self availableDuration]);
//        NSLog(@"Buffering status: %@", [object loadedTimeRanges]);
        return;
    }
	else
	{
		[super observeValueForKeyPath:path ofObject:object change:change context:context];
	}
}

- (int)enableTrack:(bool)bAudio Enable:(bool)bEnable
{
    if(!m_pPlaybackItem)
        return VONP_ERR_Pointer;
    /*
    AVAsset *asset = [[m_pPlayer currentItem] asset];
    NSArray *audioTracks = [asset tracksWithMediaType:AVMediaTypeAudio];
    
    if(audioTracks)
    {
        // Mute all the audio tracks
        NSMutableArray *allAudioParams = [NSMutableArray array];
        for (AVAssetTrack *track in audioTracks)
        {
            AVMutableAudioMixInputParameters *audioInputParams =    [AVMutableAudioMixInputParameters audioMixInputParameters];
            [audioInputParams setVolume:0.0 atTime:kCMTimeZero];
            [audioInputParams setTrackID:[track trackID]];
            [allAudioParams addObject:audioInputParams];
        }
        AVMutableAudioMix *audioZeroMix = [AVMutableAudioMix audioMix];
        [audioZeroMix setInputParameters:allAudioParams];
        
        [[m_pPlayer currentItem] setAudioMix:audioZeroMix];        
    }
    
    return 0;
     */
    
    NSArray* pTracks = [m_pPlaybackItem tracks];
    
    if(pTracks)
    {
        for(int n=0; n<[pTracks count]; n++)
        {
            AVPlayerItemTrack* pTrack = [pTracks objectAtIndex:n];
            
            if(pTrack)
            {
                AVAssetTrack* pAsset = [pTrack assetTrack];
                
                if(pAsset)
                {
                    NSString* pMediaType = [pAsset mediaType];
                    
                    if(pMediaType)
                    {
                        if([pMediaType isEqualToString:(bAudio?AVMediaTypeAudio:AVMediaTypeVideo)])
                        {
                            VOLOGI("[NPW]%s %s track ok,total track count %d", bEnable?"Enable":"Disable", bAudio?"audio":"video", [pTracks count]);
                            pTrack.enabled = bEnable?YES:NO;
                            return VONP_ERR_None;
                        }
                    }
                }
            }
        }
    }
    
    return VONP_ERR_Unknown;
}

- (int)getBufTime
{
    return m_nBufTime;
}

- (int)getCurrBufTime
{
    if(!m_pPlayer)
        return -1;
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    AVPlayerItem* pItem = [m_pPlayer currentItem];
    
    if(!pItem)
    {
        [pool release];
        return -1;
    }
    
    //don't call loadedTimeRanges continually,it will increse CPU load
    NSArray *loadedTimeRanges = [pItem loadedTimeRanges];
    
    if(!loadedTimeRanges || [loadedTimeRanges count]<=0)
    {
        [pool release];
        return -1;
    }
    
    NSValue* range = [loadedTimeRanges objectAtIndex:0];
    
    if(!range)
    {
        [pool release];
        return -1;
    }
    
    CMTimeRange timeRange = [range CMTimeRangeValue];
    //float startSeconds = CMTimeGetSeconds(timeRange.start);
    float durationSeconds = CMTimeGetSeconds(timeRange.duration);
    //NSTimeInterval result = startSeconds + durationSeconds;
    
    [pool release];
    return (int)(durationSeconds*1000);
}

- (void)setVolume:(int)nVolume
{
#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE))
    if(m_pPlayer)
    {
        float value = nVolume/100.0;
        m_pPlayer.volume = value;
    }
#endif
}


@end
