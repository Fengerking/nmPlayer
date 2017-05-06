//
//  CAVFrameTools.cpp
//
//  Created by Lin Jun on 10/17/12.
//  Copyright (c) 2012 VisualOn. All rights reserved.
//

#include "voLog.h"
#include "CAVFrameTools.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif	// _VONAMESPACE


CAVFrameTools::CAVFrameTools()
:m_hVideoParser(NULL)
,m_nAudioTimeOffset(0)
,m_nTotalAudioFrameCount(0)
,m_nFirstVideoTime(-1)
,m_nTotalVideoFrameCount(0)
,m_nLastAudioTime(0)
,m_nDurationPerFrame(21)
{
    memset(&m_funcVideoParser, 0, sizeof(VO_VIDEO_PARSERAPI));
}

CAVFrameTools::~CAVFrameTools()
{
    Uninit();
}

int CAVFrameTools::Init()
{
    voGetVideoParserAPI(&m_funcVideoParser,VO_VIDEO_CodingH264);
    
    if(!m_funcVideoParser.Init)
    {
        VOLOGW("[NPW]Get vdeo parser API failed!!!");
        return VONP_ERR_OutMemory;
    }
    
    m_funcVideoParser.Init(&m_hVideoParser);
    
    if(!m_hVideoParser)
        return VONP_ERR_OutMemory;
    
    VOLOGI("[NPW]Video parser init ok!!!");

    return VONP_ERR_None;
}

int CAVFrameTools::Uninit()
{
    if(m_hVideoParser)
    {
        m_funcVideoParser.Uninit(m_hVideoParser);
        m_hVideoParser = NULL;
    }
    
    return VONP_ERR_None;
}


int CAVFrameTools::GetAspectRatio(VONP_BUFFERTYPE* pBuf, int* pAspectRatio)
{
    if(m_hVideoParser && pBuf->pBuffer && pBuf->nSize>0)
    {
        int nRet = VONP_ERR_Unknown;
        
        VO_CODECBUFFER buf;
        memset(&buf, 0, sizeof(VO_CODECBUFFER));
        
        buf.Buffer	= pBuf->pBuffer;
        buf.Length	= pBuf->nSize;
                
        nRet = m_funcVideoParser.Process(m_hVideoParser, &buf);
        
        if(VO_ERR_NONE == nRet)
        {
            VO_PARSER_ASPECTRATIO sRatio;
            memset(&sRatio, 0, sizeof(VO_PARSER_ASPECTRATIO));
            sRatio.mode = -1;
                        
            nRet = m_funcVideoParser.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_RATIO, &sRatio);
        
            if(VO_ERR_NONE == nRet)
            {                
                if(sRatio.mode == VO_RATIO_00)
                    *pAspectRatio = VONP_RATIO_00;
                else if(sRatio.mode == VO_RATIO_11)
                    *pAspectRatio = VONP_RATIO_11;
                else if(sRatio.mode == VO_RATIO_43)
                    *pAspectRatio = VONP_RATIO_43;
                else if(sRatio.mode == VO_RATIO_169)
                    *pAspectRatio = VONP_RATIO_169;
                else if(sRatio.mode == VO_RATIO_21)
                    *pAspectRatio = VONP_RATIO_21;
                else if(sRatio.mode == VO_RATIO_MAX)
                {
                    int nWidth  = sRatio.width;
                    int nHeight = sRatio.height;

                    VOLOGW("[NPW]Handle aspect ratio byself,%dx%d", nWidth, nHeight);
                    
                    *pAspectRatio   = VONP_RATIO_00;
                    int nRatio      = 0;
                    
                    if(nHeight > 0)
                        nRatio = nWidth*10/nHeight;
                    
                    if(nRatio == 0)
                        *pAspectRatio = VONP_RATIO_00;   //0
                    else if(nRatio <= 10)
                        *pAspectRatio = VONP_RATIO_11;   //1
                    else if(nRatio <= 42/3)
                        *pAspectRatio = VONP_RATIO_43;   //2
                    else if(nRatio <= 168 / 9)
                        *pAspectRatio = VONP_RATIO_169;  //3
                    else if(nRatio <= 20)
                        *pAspectRatio = VONP_RATIO_21;   //4
                }
                else
                {
                    VOLOGE("[NPW]Video parser output error!!!");
                }
                
                VOLOGI("[NPW]Video parser output respect ratio: %d", *pAspectRatio);

                return VONP_ERR_None;
            }
        }
    }
    
    return VONP_ERR_Unknown;
}

int CAVFrameTools::GetProfileLevel(VONP_BUFFERTYPE* pBuf, int* pProfileLevel)
{
    if(m_hVideoParser && pBuf->pBuffer && pBuf->nSize>0)
    {
        int nRet = VONP_ERR_Unknown;
        
        VO_CODECBUFFER buf;
        memset(&buf, 0, sizeof(VO_CODECBUFFER));
        
        buf.Buffer	= pBuf->pBuffer;
        buf.Length	= pBuf->nSize;
        
        nRet = m_funcVideoParser.Process(m_hVideoParser, &buf);
        
        if(VO_ERR_NONE == nRet)
        {
            //refer to enum VO_VIDEO_LEVELTYPE
            int nProfileLevel = -1;
            nRet = m_funcVideoParser.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_PROFILE, &nProfileLevel);
            
            if(VO_ERR_NONE == nRet)
            {                
                VOLOGI("[NPW]Profile level is 0X%x", nProfileLevel);
                return VONP_ERR_None;
            }
            else
            {
                VOLOGW("[NPW]Profile level is 0X%x", nProfileLevel);
                return VONP_ERR_Unknown;
            }
        }
    }
    
    return VONP_ERR_Unknown;
}


int CAVFrameTools::GetAACSampleRate(VONP_BUFFERTYPE* pBuf)
{
//    if(!CheckValidADTSFrame(pBuf))
//    {
//        return -1;
//    }
    
    unsigned char* pData = pBuf->pBuffer;
    
    int nIdx = (pData[2]&0x3c) >> 2;
    
    if(nIdx >= MAXNUM_AAC_SAMPLE_RATES)
    {
        VOLOGW("[NPW]ADTS AAC samplerate index error!!!");
        return -1;
    }
    
    int nSampleRate = AACSampRateTab[nIdx];
    
    //VOLOGI("[NPW]ADTS AAC samplerate is %d", nSampleRate);
    
    return nSampleRate;
}

long long CAVFrameTools::GetVideoFrameTime(VONP_BUFFERTYPE* pBuf)
{
    if(m_nTotalVideoFrameCount == 0)
        m_nFirstVideoTime = pBuf->llTime*90;
    
    m_nTotalVideoFrameCount++;
    
    return pBuf->llTime*90;
}

long long CAVFrameTools::GetAACFrameTime(VONP_BUFFERTYPE* pBuf)
{
//    if(!CheckValidADTSFrame(pBuf))
//    {
//        return -1;
//    }
    
    int nSampleRate = GetAACSampleRate(pBuf);
    
    if(-1 == nSampleRate)
    {
        return -1;
    }
    
    if(m_nTotalAudioFrameCount == 0)
    {
//        long long llDurationPerFrame = (1024*1000)/nSampleRate;
//        long long llFrameCount = pBuf->llTime/llDurationPerFrame;
//        long long llLeft = pBuf->llTime%llDurationPerFrame;
//        llLeft = llLeft*((1024*90000)/nSampleRate)/llDurationPerFrame;
//        m_nAudioTimeOffset = ((llFrameCount*1024*90000)/nSampleRate);
        
//        if(m_nFirstVideoTime >= 0)
//        {
//            m_nAudioTimeOffset = m_nFirstVideoTime;
//        }
//        else
//        {
//            //VOLOGW("[NPW]Video frame not ready, audio %lld", pBuf->llTime);
//            return -1;
//        }
        
        m_nDurationPerFrame     = (1024*1000)/nSampleRate;
        m_nAudioTimeOffset      = pBuf->llTime*90;
    }
        
    long long nDur = ((m_nTotalAudioFrameCount*1024*90000)/nSampleRate) + m_nAudioTimeOffset;
    m_nLastAudioTime = pBuf->llTime;
       
    
#if 0
    static long long nLast = 0;
    
    VOLOGI("[NPW]TS %04lld ADTS AAC duration is %lld, timestamp %lld(%lld), size %d, offset %lld", m_nTotalAudioFrameCount, nDur, pBuf->llTime, pBuf->llTime-nLast, pBuf->nSize, pBuf->llTime - nDur);
    
    nLast = pBuf->llTime;
#endif
    
    m_nTotalAudioFrameCount++;
    
    return nDur;
}


bool CAVFrameTools::CheckValidADTSFrame(VONP_BUFFERTYPE* pBuf)
{
    unsigned char* pData = pBuf->pBuffer;
    
    if(pData == NULL)
    {
        return false;
    }
    
    //7 is the adts header length
    if(pBuf->nSize < ADTS_HEADER_LEN)
    {
        VOLOGW("[NPW]Find invalid ADTS by buffer size error");
        return false;
    }
    
    if( ((*pData)!=0xFF) || ((*(pData+1))&0xF0)!=0xF0 )
    {
        VOLOGW("[NPW]Find invalid ADTS by key flag");
        return false;
    }
    
    //Get the frame Length
    int nLen = ((pData[3] & 0x3) << 11) + (pData[4] << 3) + (pData[5] >> 5);
    
    if(nLen != pBuf->nSize)
    {
        VOLOGW("[NPW]Find invalid ADTS by buffer size is not match");
        return false;
    }
    
    return true;
}

void CAVFrameTools::ResetFrameCount()
{
    m_nTotalAudioFrameCount = 0;
    m_nTotalVideoFrameCount = 0;
    
    m_nFirstVideoTime   = -1;
    m_nLastAudioTime    = 0;
}

bool CAVFrameTools::IsKeyFrame(VONP_BUFFERTYPE* pBuf)
{
    if(m_hVideoParser && pBuf->pBuffer && pBuf->nSize>0)
    {
        int nRet = VONP_ERR_Unknown;
        
        VO_CODECBUFFER buf;
        memset(&buf, 0, sizeof(VO_CODECBUFFER));
        
        buf.Buffer	= pBuf->pBuffer;
        buf.Length	= pBuf->nSize;
        
        nRet = m_funcVideoParser.Process(m_hVideoParser, &buf);
        
        if(VO_RETURN_FMHEADER==nRet || VO_RETURN_SQFMHEADER==nRet)
        {
            int nFrameType = -1;
            nRet = m_funcVideoParser.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_FRAMETYPE, &nFrameType);
            
            if(VO_ERR_NONE == nRet)
            {
                if(nFrameType == VO_VIDEO_FRAME_I)
                {
                    VOLOGI("[NPW]Found key frame : %lld, %d", pBuf->llTime, pBuf->nSize);
                    return true;
                }
                else if(nFrameType == VO_VIDEO_FRAME_B)
                {
                    //VOLOGI("[NPW]Found B frame : %lld, %d", pBuf->llTime, pBuf->nSize);
                }
                else if(nFrameType == VO_VIDEO_FRAME_P)
                {
                    //VOLOGI("[NPW]Found P frame : %lld, %d", pBuf->llTime, pBuf->nSize);
                }
                else if(nFrameType == VO_VIDEO_FRAME_S)
                {
                    //VOLOGI("[NPW]Found S frame : %lld, %d", pBuf->llTime, pBuf->nSize);
                }
                else
                {
                    //VOLOGI("[NPW]Found other frame : %lld, %d", pBuf->llTime, pBuf->nSize);
                }
            }
            else
            {
                return false;
            }
        }
    }
    
    return false;
}

int CAVFrameTools::GetDurationPerFrame()
{
    return m_nDurationPerFrame;
}

int CAVFrameTools::UpdateDurationPerFrame(VONP_BUFFERTYPE* pBuf)
{
    int nSampleRate         = GetAACSampleRate(pBuf);
    m_nDurationPerFrame     = (1024*1000)/nSampleRate;
    return m_nDurationPerFrame;
}

int CAVFrameTools::GetChannelCount(VONP_BUFFERTYPE* pBuf)
{
//    if(!CheckValidADTSFrame(pBuf))
//        return -1;
    
    if(pBuf->nSize < ADTS_HEADER_LEN)
    {
        VOLOGW("[NPW]Find invalid ADTS by buffer size error");
        return -1;
    }
    
    unsigned char* pHead = pBuf->pBuffer;
    
    if( ((*pHead)!=0xFF) || ((*(pHead+1))&0xF0)!=0xF0 )
    {
        VOLOGW("[NPW]Find invalid ADTS by key flag");
        return -1;
    }

	int nIdx =  ( (*(pHead + 2) << 2) | (*(pHead + 3) >> 6) ) & 0x07;
    
    if(nIdx<0 || nIdx>=MAXNUM_AAC_CHANNEL_COUNT)
        return -1;
    
    VOLOGI("[NPW]Audio channel is %d", AdtsChannelCountTab[nIdx]);
    return AdtsChannelCountTab[nIdx];
}
