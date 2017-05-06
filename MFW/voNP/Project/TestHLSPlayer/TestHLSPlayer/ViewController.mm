//
//  ViewController.m
//  TestHLSPlayer
//
//  Created by Lin Jun on 9/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ViewController.h"
#import <CoreMedia/CMTime.h>

@implementation ViewController

@synthesize playerView;

VONP_BUFFER_FORMAT	m_sAudioTrackInfo;
VONP_BUFFER_FORMAT	m_sVideoTrackInfo;

int ReturnCode_OSMP2OMXAL(int nRC);
int BufferFlag_OSMP2OMXAL(int nFlag);
int Buffer_OSMP2OMXAL(VOOSMP_BUFFERTYPE* pOSMP, VONP_BUFFERTYPE* pOMXAL, int nSSType);

VONP_READBUFFER_FUNC m_funcReadBuf;
int voReadAudioBuf(void * pUserData, VONP_BUFFERTYPE * pBuffer);
int voReadVideoBuf(void * pUserData, VONP_BUFFERTYPE * pBuffer);


- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)printTimestamp:(NSTimer*)theTimer
{
    if (m_hPlayer)
    {
        //NSLog(@"%G", CMTimeGetSeconds([player currentTime]));
        

		float currentTimeSec = (float)m_funcPlayer.GetPos(m_hPlayer);
        
        if(currentTimeSec == 0.0)
        {
            //NSLog(@"waiting to start...");
        }

		
		//NSNumber *duration = [playerItem valueForProperty:MPMediaItemPropertyPlaybackDuration];
		
        int duration = 0;
        if(m_pSrc)
            m_pSrc->GetDuration(&duration);
        
		int minutes = floor(currentTimeSec/1000 / 60);
		int seconds = trunc(currentTimeSec/1000 - minutes * 60);
		playingTimeLabel.text = [NSString stringWithFormat:@"%02d:%02d", minutes, seconds];
        
        float progress = (currentTimeSec*100) / duration;
        [slider setValue: progress];
        
        int left = duration - currentTimeSec;
        minutes = floor(left/1000 / 60);
		seconds = trunc(left/1000 - minutes * 60);
		leftTimeLabel.text = [NSString stringWithFormat:@"%02d:%02d", minutes, seconds];
    }
}

- (void)dealloc {

    if(timeObserver)
        [timeObserver release];
    
    if(m_hPlayer)
        m_funcPlayer.Uninit(m_hPlayer);
    m_hPlayer = NULL;
    
    if(m_pSrc)
    {
        m_pSrc->Stop();
        m_pSrc->Close();
        delete m_pSrc;
        m_pSrc = NULL;
    }
    
    [super dealloc];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [UIApplication sharedApplication].statusBarHidden = YES;
    
    timeObserver = nil;
    m_pSrc = NULL;
    m_hPlayer = NULL;
    bRunning = false;
    
    //[nextButton setHidden: YES];
    [playButton setTitle:@"Play" forState:UIControlStateNormal];
    switcher.on = NO;
    switcherLoop.on = NO;
    
    leftTimeLabel.text = [NSString stringWithFormat:@"%02d:%02d", 0, 0];
    playingTimeLabel.text = [NSString stringWithFormat:@"%02d:%02d", 0, 0];
    [slider setValue: 0.0];
    
    voGetHLSWrapperAPI(&m_funcPlayer);
    
    NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:0.2 target:self selector:@selector(printTimestamp:) userInfo:nil repeats:YES];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}



- (int)handlePlayerEvent:(int)nID pram1:(void*)pParam1 pram2:(void*)pParam2
{
    
    {
        //NSLog(@"Native player created!!!");
        //[(AVPlayerLayer *)[(UIView*)playerView layer] setPlayer: (AVPlayer*)pParam1];
        //[playerView setPlayer: (AVPlayer*)pParam1];
        
        /*
        player = (AVPlayer*)pParam1;
        timeObserver = [[(AVPlayer*)pParam1 addPeriodicTimeObserverForInterval:CMTimeMakeWithSeconds(0.01, NSEC_PER_SEC)
                                                             queue:NULL
                                                        usingBlock:
                         ^(CMTime time)
                         {
                             [self updatePlayingTime];
                         }] retain];
         */

    }
    
    if(VONP_CB_PlayComplete == nID)
    {
        printf("EOS...\n");
        
        [self performSelectorOnMainThread:@selector(onEOS) withObject:nil waitUntilDone:NO];
    }
    else if(VONP_CB_VideoSizeChanged == nID)
    {
        printf("Width %d, Height %d\n", (*(int*)pParam1), (*(int*)pParam2));
    }
    else if(VONP_CB_StartBuff == nID)
    {
        printf("VONP_CB_StartBuff...\n");
    }
    else if(VONP_CB_StopBuff == nID)
    {
        printf("VONP_CB_StopBuff...\n");
    }
    
    return 0;
}

- (int)onEOS
{
    if(switcherLoop.isOn)
    {
        //pause
        [self play: nil];
        
        if(m_pSrc)
            m_pSrc->SetCurPos(0);
        
        if(m_hPlayer)
        {
            m_funcPlayer.SetPos(m_hPlayer, 0);
        }
        
        [self play: nil];
    }
    else
    {
        [self stop: nil];
    }
    
    return 0;
}

int voPlayerEvent(void * pUserData, int nID, void * pParam1, void * pParam2)
{
    if(pUserData)
    {
        [(ViewController*)pUserData handlePlayerEvent:nID pram1:pParam1 pram2:pParam2];
    }
    
    return 0;
}


int voReadAudioBuf(void * pUserData, VONP_BUFFERTYPE * pBuffer)
{
    //@autoreleasepool
    {
        if(pUserData)
        {
            return [(ViewController*)pUserData handleReadAudio:pBuffer];
        }

        return 0;
    }
}

int voReadVideoBuf(void * pUserData, VONP_BUFFERTYPE * pBuffer)
{
    //@autoreleasepool
    {
        if(pUserData)
        {
            return [(ViewController*)pUserData handleReadVideo:pBuffer];
        }

        return 0;
    }
}

- (int) handleReadVideo:(VONP_BUFFERTYPE*) pBuffer
{
    if(m_pSrc)
    {
        VOOSMP_READBUFFER_FUNC* pR = (VOOSMP_READBUFFER_FUNC*)m_pSrc->GetReadBufPtr();
        
        if(pR)
        {
            VOOSMP_BUFFERTYPE buf;
            memset(&buf, 0, sizeof(VOOSMP_BUFFERTYPE));
            int nRet = pR->ReadVideo(pR->pUserData, &buf);
            
            Buffer_OSMP2OMXAL(&buf, pBuffer, VOOSMP_SS_VIDEO);
            
            return ReturnCode_OSMP2OMXAL(nRet);
        }
    }
    
    return VONP_ERR_Retry;
}

- (int) handleReadAudio:(VONP_BUFFERTYPE*) pBuffer
{
    if(m_pSrc)
    {
        VOOSMP_READBUFFER_FUNC* pR = (VOOSMP_READBUFFER_FUNC*)m_pSrc->GetReadBufPtr();
        
        if(pR)
        {
            VOOSMP_BUFFERTYPE buf;
            memset(&buf, 0, sizeof(VOOSMP_BUFFERTYPE));
            int nRet = pR->ReadAudio(pR->pUserData, &buf);
            
            Buffer_OSMP2OMXAL(&buf, pBuffer, VOOSMP_SS_AUDIO);
            
            return ReturnCode_OSMP2OMXAL(nRet);
        }
    }

    return VONP_ERR_Retry;
}


- (IBAction)play:(id)sender
{
    if(bRunning)
    {
        if(m_hPlayer)
            m_funcPlayer.Pause(m_hPlayer);
        if(m_pSrc)
            m_pSrc->Pause();
        
        bRunning = false;
        [playButton setTitle:@"Play" forState:UIControlStateNormal];
        return;
    }
    else // next
    {
        if(!m_hPlayer)
        {
            ///var/mobile/Applications/BEEF867C-F9C0-443D-B19A-8959DCB03269/Documents/playback.m3u8
            
//            NSString *path =
//            [NSSearchPathForDirectoriesInDomains(
//                                                 NSDocumentDirectory,
//                                                 NSUserDomainMask,
//                                                 YES)
//             objectAtIndex:0];
//            const char* pFile = [[path stringByAppendingPathComponent:@"playback.m3u8"] UTF8String];
//            NSLog(@"file is : %s", pFile);

            if(!m_pSrc)
                m_pSrc = new CSrcCenter();
            NSString* strPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/"];
            //m_pSrc->SetVideoOnly(true);
            //m_pSrc->SetAudioOnly(true);
            
            if(switcher.isOn)
                m_pSrc->Init((void *)[strPath UTF8String], 1);
            else
                m_pSrc->Init((void *)[strPath UTF8String], 0);
            

            m_funcPlayer.Init(&m_hPlayer, NULL);
            
            VONP_LISTENERINFO event;
            event.pUserData = self;
            event.pListener = voPlayerEvent;
            m_funcPlayer.SetParam(m_hPlayer, VONP_PID_LISTENER, &event);
        
            m_funcPlayer.SetView(m_hPlayer, playerView);
            
            memset(&m_funcReadBuf, 0, sizeof(VONP_READBUFFER_FUNC));
            m_funcReadBuf.pUserData = self;
            m_funcReadBuf.ReadAudio = voReadAudioBuf;
            m_funcReadBuf.ReadVideo = voReadVideoBuf;
            m_funcPlayer.Open(m_hPlayer, &m_funcReadBuf, VONP_FLAG_SOURCE_READBUFFER);
        }
    }
    
    
    
    //    CMTime endTime = CMTimeMakeWithSeconds([time floatValue], 1);
    //    timeObserver = [avPlayer addBoundaryTimeObserverForTimes:[NSArray arrayWithObject:[NSValue valueWithCMTime:endTime]] queue:NULL usingBlock:^(void) {
    //        [avPlayer removeTimeObserver:timeObserver];
    
    if(m_pSrc)
    {
        m_pSrc->Run();
        
        int nDur = 0;
        m_pSrc->GetDuration(&nDur);
        printf("Sourc duration:%d\n", nDur);
    }
        
    m_funcPlayer.Run(m_hPlayer);
    bRunning = true;
    [playButton setTitle:@"Pause" forState:UIControlStateNormal];
}

- (IBAction)stop:(id)sender
{
    if(timeObserver)
    {
        [timeObserver release];
        timeObserver = nil;
    }
        
    
    bRunning = false;
    
    if(m_hPlayer)
    {
        m_funcPlayer.Stop(m_hPlayer);
        m_funcPlayer.Uninit(m_hPlayer);
    }
        
    m_hPlayer = NULL;
    
    if(m_pSrc)
    {
        m_pSrc->Stop();
        m_pSrc->Close();
        delete m_pSrc;
        m_pSrc = NULL;
    }
    
    [playButton setTitle:@"Play" forState:UIControlStateNormal];
    
    leftTimeLabel.text = [NSString stringWithFormat:@"%02d:%02d", 0, 0];
    playingTimeLabel.text = [NSString stringWithFormat:@"%02d:%02d", 0, 0];
    [slider setValue: 0.0];
}

- (IBAction)seekTo:(id)sender
{
    if(!m_hPlayer)
        return;
    
    [self play: nil];
    
    int nDuration   = 0;

    if(m_pSrc)
    {
        m_pSrc->GetDuration(&nDuration);
    }
    
    int nPos    = slider.value*nDuration/100;
    
    printf("Seek to %d\n", nPos);
    
    if(m_pSrc)
        m_pSrc->SetCurPos(nPos);
    
    if(m_hPlayer)
    {
        m_funcPlayer.SetPos(m_hPlayer, nPos);
    }
    
    [self play: nil];
}

int BufferFlag_OSMP2OMXAL(int nFlag)
{
	int mnFlag = 0;
    
	if(VOOSMP_FLAG_BUFFER_KEYFRAME & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_KEYFRAME;
    
	if(VOOSMP_FLAG_BUFFER_NEW_PROGRAM & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_NEW_PROGRAM;
    
	if(VOOSMP_FLAG_BUFFER_NEW_FORMAT & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_NEW_FORMAT;
    
	if(VOOSMP_FLAG_BUFFER_HEADDATA & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_HEADDATA;
    
	if(VOOSMP_FLAG_BUFFER_DROP_FRAME & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_DROP_FRAME;
    
	if(VOOSMP_FLAG_BUFFER_DELAY_TO_DROP & nFlag)
		mnFlag |= VONP_FLAG_BUFFER_DELAY_TO_DROP;
    
	return mnFlag;
}


int Buffer_OSMP2OMXAL(VOOSMP_BUFFERTYPE* pOSMP, VONP_BUFFERTYPE* pOMXAL, int nSSType)
{
	pOMXAL->nSize		= pOSMP->nSize;
	pOMXAL->pBuffer		= pOSMP->pBuffer;
	pOMXAL->llTime		= pOSMP->llTime;
	pOMXAL->nFlag		= BufferFlag_OSMP2OMXAL(pOSMP->nFlag);
	pOMXAL->pData		= pOSMP->pData;
	pOMXAL->llReserve	= pOSMP->llReserve;
    
	VOOSMP_BUFFERTYPE*	sBuf = pOSMP;
	VONP_BUFFERTYPE*	dBuf = pOMXAL;
    
	//dBuf->nFlag = 0;
    
	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_KEYFRAME) == VOOSMP_FLAG_BUFFER_KEYFRAME)
	{
		//dBuf->nFlag |= VOMP_FLAG_BUFFER_KEYFRAME;
	}
    
	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM) == VOOSMP_FLAG_BUFFER_NEW_PROGRAM)
	{
		//dBuf->nFlag |= VOMP_FLAG_BUFFER_NEW_PROGRAM;
        
		if(sBuf->pData)
		{
			VOOSMP_BUFFER_FORMAT *pOSTackInfo = (VOOSMP_BUFFER_FORMAT *)sBuf->pData;
			if(nSSType == VOOSMP_SS_AUDIO)
			{
				m_sAudioTrackInfo.nStreamType = VONP_SS_Audio;
				//here has issue
				m_sAudioTrackInfo.nCodec = pOSTackInfo->nCodec;//ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);
				m_sAudioTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sAudioTrackInfo.sFormat.audio.nSampleRate = pOSTackInfo->sFormat.audio.SampleRate;
				m_sAudioTrackInfo.sFormat.audio.nChannels = pOSTackInfo->sFormat.audio.Channels;
				m_sAudioTrackInfo.sFormat.audio.nSampleBits =  pOSTackInfo->sFormat.audio.SampleBits;
                
				m_sAudioTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sAudioTrackInfo.pHeadData = pOSTackInfo->pHeadData;
                
				dBuf->pData = &m_sAudioTrackInfo;
			}
			else if(nSSType == VOOSMP_SS_VIDEO)
			{
				m_sVideoTrackInfo.nStreamType = VONP_SS_Video;
                
				m_sVideoTrackInfo.nCodec = pOSTackInfo->nCodec;
				m_sVideoTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sVideoTrackInfo.sFormat.video.nWidth = pOSTackInfo->sFormat.video.Width;
				m_sVideoTrackInfo.sFormat.video.nHeight = pOSTackInfo->sFormat.video.Height;
				m_sVideoTrackInfo.sFormat.video.nType =  pOSTackInfo->sFormat.video.Type;
                
				m_sVideoTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sVideoTrackInfo.pHeadData = pOSTackInfo->pHeadData;
                
				dBuf->pData = &m_sVideoTrackInfo;
			}
		}
	}
    
	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) == VOOSMP_FLAG_BUFFER_NEW_FORMAT)
	{
		if(sBuf->pData)
		{
			VOOSMP_BUFFER_FORMAT *pOSTackInfo = (VOOSMP_BUFFER_FORMAT *)sBuf->pData;
			if(nSSType == VOOSMP_SS_AUDIO)
			{
				m_sAudioTrackInfo.nStreamType = VONP_SS_Audio;
				// has issue
				m_sAudioTrackInfo.nCodec = pOSTackInfo->nCodec;//ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);
				m_sAudioTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sAudioTrackInfo.sFormat.audio.nSampleRate = pOSTackInfo->sFormat.audio.SampleRate;
				m_sAudioTrackInfo.sFormat.audio.nChannels = pOSTackInfo->sFormat.audio.Channels;
				m_sAudioTrackInfo.sFormat.audio.nSampleBits =  pOSTackInfo->sFormat.audio.SampleBits;
                
				m_sAudioTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sAudioTrackInfo.pHeadData = pOSTackInfo->pHeadData;
                
				dBuf->pData = &m_sAudioTrackInfo;
			}
			else if(nSSType == VOOSMP_SS_VIDEO)
			{
				m_sVideoTrackInfo.nStreamType = VONP_SS_Video;
				m_sVideoTrackInfo.nCodec = pOSTackInfo->nCodec;//ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);
				m_sVideoTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sVideoTrackInfo.sFormat.video.nWidth = pOSTackInfo->sFormat.video.Width;
				m_sVideoTrackInfo.sFormat.video.nHeight = pOSTackInfo->sFormat.video.Height;
				m_sVideoTrackInfo.sFormat.video.nType =  pOSTackInfo->sFormat.video.Type;
                
				m_sVideoTrackInfo.nHeadDataSize = pOSTackInfo->nHeadDataLen;
				m_sVideoTrackInfo.pHeadData = pOSTackInfo->pHeadData;
                
				dBuf->pData = &m_sVideoTrackInfo;
			}
		}
	}
    
	return VONP_ERR_None;
}


int ReturnCode_OSMP2OMXAL(int nRC)
{
	int nRet = nRC;
    
	switch(nRC)
	{
        case VOOSMP_ERR_None:
            nRet = VONP_ERR_None;
            break;
        case VOOSMP_ERR_EOS:
            nRet = VONP_ERR_EOS;
            break;
        case VOOSMP_ERR_Retry:
            nRet = VONP_ERR_Retry;
            break;
        case VOOSMP_ERR_Video:
            nRet = VONP_ERR_VideoCodec;
            break;
        case VOOSMP_ERR_Audio:
            nRet = VONP_ERR_AudioCodec;
            break;
        case VOOSMP_ERR_OutMemory:
            nRet = VONP_ERR_OutMemory;
            break;
        case VOOSMP_ERR_Pointer:
            nRet = VONP_ERR_Pointer;
            break;
        case VOOSMP_ERR_ParamID:
            nRet = VONP_ERR_ParamID;
            break;
        case VOOSMP_ERR_Status:
            nRet = VONP_ERR_Status;
            break;
        case VOOSMP_ERR_Implement:
            nRet = VONP_ERR_Implement;
            break;
        case VOOSMP_ERR_SmallSize:
            nRet = VONP_ERR_WaitTime;
            break;
        case VOOSMP_ERR_WaitTime:
            nRet = VONP_ERR_WaitTime;
            break;
        case VOOSMP_ERR_Unknown:
            nRet = VONP_ERR_Unknown;
            break;
        case VOOSMP_ERR_Audio_No_Now:
            nRet = VONP_ERR_Audio_No_Now;
            break;
        case VOOSMP_ERR_Video_No_Now:
            nRet = VONP_ERR_Video_No_Now;
            break;
        case VOOSMP_ERR_FLush_Buffer:
            nRet = VONP_ERR_FLush_Buffer;
            break;
        default:
		{
			//VOLOGI("Loss return node");
			nRet = VONP_ERR_Unknown;
		}
            break;
	}
    
	return nRet;
}

- (bool)isRunning
{
    return bRunning;
}


@end
