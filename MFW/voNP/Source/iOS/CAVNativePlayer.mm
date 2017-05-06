/*
 *  COMXALMediaPlayer.cpp
 *
 *  Created by Lin Jun on 10/09/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

/*
#include "CAVNativePlayer.h"
#include "voOSFunc.h"
#include <string.h>
#include "voLog.h"
#include "CAVPlayer.h"

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define PLAYER      ((AVPlayer*)m_pPlayer)
#define PLAYER_ITEM ((AVPlayerItem*)m_pPlaybackItem)
#define AVPLAYER    ((AVPlayer*)m_pAVPlayer)


NSString * const kTracksKey         = @"tracks";
NSString * const kPlayableKey       = @"playable";


CAVNativePlayer::CAVNativePlayer()
:m_pPlaybackItem(NULL)
,m_pPlayer(NULL)
,m_pAVPlayer(NULL)
{
    memset(m_szPlaybackURL, 0, 1024);
    
    //AVPLAYER = [CAVPlayer alloc];
}

CAVNativePlayer::~CAVNativePlayer(void)
{
	Uninit();
}

int CAVNativePlayer::Init()
{
    if(AVPLAYER)
        [AVPLAYER init];
    
    return VONP_ERR_None;
}

int CAVNativePlayer::Uninit()
{
    Close();
    
    return VONP_ERR_None;
}


int CAVNativePlayer::Open(void* pSource, int nFlag)
{
    VOLOGI("[NPW]+Open");
    
    memset(m_szPlaybackURL, 0, 1024);
    strcpy(m_szPlaybackURL, (char*)pSource);
        
    VOLOGI("[NPW]-Open %s", m_szPlaybackURL);
    
    return VONP_ERR_None;
}

int CAVNativePlayer::Close()
{
    if(m_pView)
        [(AVPlayerLayer *)[(UIView*)m_pView layer] setPlayer: nil];
    
    if(PLAYER)
    {
        VOLOGI("[NPW]Release AVPlayer");
        [PLAYER release];
    
        m_pPlayer         = nil;
        m_pPlaybackItem   = nil;
    }
    
    m_pView = NULL;
    
    if(AVPLAYER)
    {
        [AVPLAYER release];
        AVPLAYER = NULL;
    }

    return VONP_ERR_None;
}

void CAVNativePlayer::doPlay()
{
    VOLOGI("[NPW]Native player doPlay");
    
    if(PLAYER)
        [PLAYER play];
}

void CAVNativePlayer::doRun()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    VOLOGI("[NPW]Native player Run");
    
    if(!m_pPlayer)
    {
        NSString* pURL = [[NSString alloc] initWithUTF8String: m_szPlaybackURL];
        NSURL *sourceURL = [NSURL URLWithString: pURL];
        
        AVURLAsset *sourceAsset = [AVURLAsset URLAssetWithURL:sourceURL options:nil];
        
        m_pPlaybackItem     = [AVPlayerItem playerItemWithAsset:sourceAsset];
        m_pPlayer           = [[AVPlayer playerWithPlayerItem: (AVPlayerItem*)m_pPlaybackItem] retain];
        
        PLAYER.closedCaptionDisplayEnabled = NO;
        
        if(m_pView)
        {
            AVPlayerLayer* layer = (AVPlayerLayer *)[(UIView*)m_pView layer];
            [layer setPlayer: PLAYER];
            //AVLayerVideoGravityResizeAspect;//AVLayerVideoGravityResizeAspectFill;//;
            layer.videoGravity = AVLayerVideoGravityResize;
        }
        
        //SendEvent(VONP_CB_NativePlayerCreated, m_pPlayer, NULL);
        [pURL release];
        
        VOLOGI("[NPW]AVPlayer open %s", m_szPlaybackURL);
    }
    
    doPlay();
    
    [pool release];
}

int CAVNativePlayer::Run()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    voRunRequestOnMain::PostRunOnMainRequest(false, 1, NULL, NULL);
    [pool release];
    return VONP_ERR_None;

        
    
    //[pool release];
    
    //VOLOGI("[NPW]count %d", [PLAYER retainCount]);
    return VONP_ERR_None;
}

int CAVNativePlayer::Pause()
{
//    NSArray *tracks = [PLAYER_ITEM tracks];
//    if(tracks)
//    {
//        for(int n=0; n<[tracks count]; n++)
//        {
//            AVPlayerItemTrack* pTrack = [tracks objectAtIndex: n];
//            if(pTrack)
//            {
//                if(n == 0)
//                    [pTrack setEnabled: NO];
//            }
//        }
//    }

    VOLOGI("[NPW]Native player Pause");
    if(PLAYER)
    {
        [PLAYER pause];
    }

    return VONP_ERR_None;
}

int CAVNativePlayer::Stop()
{
    VOLOGI("[NPW]Native player Stop");
    Pause();
    return VONP_ERR_None;
}

int CAVNativePlayer::GetPos()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    if (PLAYER && CMTimeGetSeconds([PLAYER currentTime]))
    {
        CMTime cTime = [PLAYER currentTime];
        float currentTimeSec = cTime.value / cTime.timescale;
        //VOLOGI("[NPW]Playing time %d", (int)currentTimeSec*1000);
        [pool release];
        return (int)currentTimeSec*1000;
    }

    [pool release];
    return 0;
}

int CAVNativePlayer::SetPos(int nPos)
{
    return VONP_ERR_None;
}

int CAVNativePlayer::GetBufferTime()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    if(PLAYER)
    {
        NSArray *loadedTimeRanges = [[PLAYER currentItem] loadedTimeRanges];
        
        if(loadedTimeRanges && [loadedTimeRanges count]>0)
        {
            CMTimeRange timeRange = [[loadedTimeRanges objectAtIndex: [loadedTimeRanges count]>=2?1:0] CMTimeRangeValue];
            //float startSeconds = CMTimeGetSeconds(timeRange.start);
            float durationSeconds = CMTimeGetSeconds(timeRange.duration);
            int result = durationSeconds*1000;
            return result;                
        }
    }
    
    [pool release];
    
    return 0;
}

int CAVNativePlayer::doReplaceURL(unsigned char* pszNewURL)
{
    if(PLAYER)
    {
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        
        //Open(pszNewURL, 0);
        VOLOGI("[NPW]Replace URL: %s", m_szPlaybackURL);
        
        //NSString* pURL = [[NSString alloc] initWithUTF8String: "http://akamedia2.lsops.net/live/smil:bbcworld1_en.smil/playlist.m3u8"];
        NSString* pURL = [[NSString alloc] initWithUTF8String: m_szPlaybackURL];
        NSURL *sourceURL = [NSURL URLWithString: pURL];
        
        AVURLAsset *sourceAsset = [AVURLAsset URLAssetWithURL:sourceURL options:nil];
        
        AVPlayerItem* pPlaybackItem     = [AVPlayerItem playerItemWithAsset:sourceAsset];
        
        [PLAYER replaceCurrentItemWithPlayerItem: pPlaybackItem];
        
        [pURL release];
        
        doPlay();
        
        [pool release];
        
        return VONP_ERR_None;
    }
    
    return VONP_ERR_None;
}

int CAVNativePlayer::ReplaceURL(unsigned char* pszNewURL)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    VOLOGI("[NPW]Old URL: %s", m_szPlaybackURL);
    Open(pszNewURL, 0);
    voRunRequestOnMain::PostRunOnMainRequest(false, 0, m_szPlaybackURL, NULL);
    [pool release];
    return VONP_ERR_None;
}

void CAVNativePlayer::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    if(nID == 0)
    {
        doReplaceURL((unsigned char*)pParam1);
    }
    else if(nID == 1)
    {
        doRun();
    }
}

int CAVNativePlayer::SetView(void* pView)
{
    CBaseNativePlayer::SetView(pView);
}

*/

#include "CAVNativePlayer.h"
#include "voOSFunc.h"
#include <string.h>
#include "voLog.h"
#include "CAVPlayer.h"

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define AVPLAYER    ((CAVPlayer*)m_pAVPlayer)


NSString * const kTracksKey         = @"tracks";
NSString * const kPlayableKey       = @"playable";


CAVNativePlayer::CAVNativePlayer()
:m_pAVPlayer(NULL)
{
    memset(m_szPlaybackURL, 0, 1024);
    
    m_pAVPlayer = (void*)[CAVPlayer alloc];
    
    VOLOGI("[NPW][Seek] create NP %x %x", (unsigned int)this, (unsigned int)AVPLAYER);
}

CAVNativePlayer::~CAVNativePlayer(void)
{
	Uninit();
}

int CAVNativePlayer::Init()
{
    if(AVPLAYER)
        [AVPLAYER initPlayer];
    
    return VONP_ERR_None;
}

int CAVNativePlayer::Uninit()
{
    Close();
    
    return VONP_ERR_None;
}


int CAVNativePlayer::Open(void* pSource, int nFlag)
{
    VOLOGI("[NPW]+Open");
    
    memset(m_szPlaybackURL, 0, 1024);
    strcpy(m_szPlaybackURL, (char*)pSource);
    
    if(AVPLAYER)
    {
        [AVPLAYER open: (unsigned char*)pSource];
    }

    
    VOLOGI("[NPW]-Open %s", m_szPlaybackURL);
    
    return VONP_ERR_None;
}

int CAVNativePlayer::Close()
{    
    if(AVPLAYER)
    {
        VOLOGI("[NPW][Seek] destroy NP %x %x", (unsigned int)this, (unsigned int)AVPLAYER);
        
        [AVPLAYER uninit];
        [AVPLAYER release];
        m_pAVPlayer = NULL;
    }
    
    return VONP_ERR_None;
}


int CAVNativePlayer::Run()
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    voRunRequestOnMain::PostRunOnMainRequest(false, 1, NULL, NULL);
    [pool release];
    return VONP_ERR_None;
}

int CAVNativePlayer::Pause()
{
    VOLOGI("[NPW]Native player Pause");
    if(AVPLAYER)
    {
        [AVPLAYER pause];
    }
    
    return VONP_ERR_None;
}

int CAVNativePlayer::Stop()
{
    VOLOGI("[NPW]Native player Stop");
    
    Pause();
    
    CBaseNativePlayer::Stop();
    
    return VONP_ERR_None;
}

int CAVNativePlayer::SetPos(int nPos)
{
    return VONP_ERR_None;
}

int CAVNativePlayer::GetBufferTime()
{
    if(AVPLAYER)
    {
        return [AVPLAYER getBufTime];
    }

    return -1;
}


int CAVNativePlayer::ReplaceURL(unsigned char* pszNewURL)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    VOLOGI("[NPW]Old URL: %s", m_szPlaybackURL);
    Open(pszNewURL, 0);
    voRunRequestOnMain::PostRunOnMainRequest(false, 0, m_szPlaybackURL, NULL);
    [pool release];
    return VONP_ERR_None;
}

void CAVNativePlayer::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    if(nID == 0)
    {
        if(AVPLAYER)
            [AVPLAYER replaceURL: (unsigned char*)pParam1];
    }
    else if(nID == 1)
    {
        if(AVPLAYER)
        {
            [AVPLAYER setListener: &m_NotifyEventInfo];
            [AVPLAYER run];
        }
    }
}

int CAVNativePlayer::SetView(void* pView)
{
    CBaseNativePlayer::SetView(pView);
    
    if(AVPLAYER)
        [AVPLAYER setView: (UIView*)pView];
    
    return VONP_ERR_None;
}

int CAVNativePlayer::EnableTrack(bool bAudio, bool bEnable)
{
    if(AVPLAYER)
        return [AVPLAYER enableTrack:bAudio Enable:bEnable];

    return VONP_ERR_Pointer;
}
