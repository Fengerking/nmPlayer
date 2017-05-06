//
//  CAVFrameRW.h
//
//  Created by Lin Jun on 10/24/12.
//  Copyright (c) 2012 VisualOn. All rights reserved.
//

#ifndef _CAVFRAME_RW_H_
#define _CAVFRAME_RW_H_

#include <stdio.h>
#include <string.h>

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAVFrameWriter
{
public:
    CAVFrameWriter();
    ~CAVFrameWriter();
    
public:
    void    Init(const char* pszFullFileNameWithoutExt);
    void    Uninit();
    void    Write(bool bAudio, unsigned char* pBuf, int nSize, long long llTime, bool bKeyFrame, bool bHeadData);
    
private:
    FILE*   m_hAudioFile;
    FILE*   m_hVideoFile;
    FILE*   m_hLogFile;
    long long m_llLastAudioTime;
    long long m_llLastVideoTime;
};

class CAVFrameReader
{
public:
    CAVFrameReader();
    ~CAVFrameReader();
    
public:
    void    Init(const char* pszFullFileNameWithoutExt);
    void    Uninit();
    void    Read(bool bAudio, unsigned char* pBuf, int& nSize, long long& llTime, bool& bKeyFrame, bool& bHeadData);
    
private:
    FILE*   m_hAudioFile;
    FILE*   m_hVideoFile;
    FILE*   m_hAudioVerifyFile;
    FILE*   m_hVideoVerifyFile;
    bool    m_bOutputAudioHeadData;
    bool    m_bOutputVideoHeadData;
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif // end of _CAVFRAME_RW_H_
