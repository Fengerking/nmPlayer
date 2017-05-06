//
//  CAVFrameTools.h
//
//  Created by Lin Jun on 10/17/12.
//  Copyright (c) 2012 VisualOn. All rights reserved.
//

#ifndef _CAVFRAME_TOOLS_H_
#define _CAVFRAME_TOOLS_H_

#include "voNPWrap.h"
#include "voVideoParser.h"


#define MAXNUM_AAC_SAMPLE_RATES     12
#define MAXNUM_AAC_CHANNEL_COUNT    8
#define ADTS_HEADER_LEN             7

static const int AACSampRateTab[MAXNUM_AAC_SAMPLE_RATES] = {
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025,  8000
};

static const int AdtsChannelCountTab[MAXNUM_AAC_CHANNEL_COUNT] =
{
    2, 1, 2, 3, 4, 5, 6, 7
};


class CAVFrameTools
{
public:
    CAVFrameTools();
    ~CAVFrameTools();
    
public:
    int         Init();
    int         Uninit();

    int         GetAspectRatio(VONP_BUFFERTYPE* pBuf, int* pAspectRatio);
    int         GetProfileLevel(VONP_BUFFERTYPE* pBuf, int* pProfileLevel);
    int         GetAACSampleRate(VONP_BUFFERTYPE* pBuf);
    bool        CheckValidADTSFrame(VONP_BUFFERTYPE* pBuf);
    bool        IsKeyFrame(VONP_BUFFERTYPE* pBuf);
    
    long long   GetAACFrameTime(VONP_BUFFERTYPE* pBuf);
    long long   GetVideoFrameTime(VONP_BUFFERTYPE* pBuf);
    void        ResetFrameCount();
    int         GetDurationPerFrame();
    int         UpdateDurationPerFrame(VONP_BUFFERTYPE* pBuf);
    int         GetChannelCount(VONP_BUFFERTYPE* pBuf);
	int 		gcd(int, int);
    
private:
    VO_HANDLE           m_hVideoParser;
    VO_VIDEO_PARSERAPI  m_funcVideoParser;

    
    long long           m_nAudioTimeOffset;
    long long           m_nTotalAudioFrameCount;
    long long           m_nFirstVideoTime;
    long long           m_nTotalVideoFrameCount;
    long long           m_nLastAudioTime;
    int                 m_nDurationPerFrame;
};

#endif // end of _CAVFRAME_TOOLS_H_
