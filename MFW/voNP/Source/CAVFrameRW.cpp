//
//  CAVFrameRW.cpp
//
//  Created by Lin Jun on 10/24/12.
//  Copyright (c) 2012 VisualOn. All rights reserved.
//

#include "CAVFrameRW.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif	// _VONAMESPACE

/////////////////////////////////////////////////////////////
//////////Class CAVFrameWriter //////////////////////////////

CAVFrameWriter::CAVFrameWriter()
:m_hAudioFile(NULL)
,m_hVideoFile(NULL)
,m_hLogFile(NULL)
,m_llLastAudioTime(0)
,m_llLastVideoTime(0)
{
    
}

CAVFrameWriter::~CAVFrameWriter()
{
    Uninit();
}

void CAVFrameWriter::Init(const char* pszFullFileNameWithoutExt)
{
    Uninit();
    
    char szTmp[1024];
   
    memset(szTmp, 0, 1024);
    strcpy(szTmp, pszFullFileNameWithoutExt);
    strcat(szTmp, ".aac");
    m_hAudioFile = fopen(szTmp, "wb");
    
    memset(szTmp, 0, 1024);
    strcpy(szTmp, pszFullFileNameWithoutExt);
    strcat(szTmp, ".h264");
    m_hVideoFile = fopen(szTmp, "wb");

    memset(szTmp, 0, 1024);
    strcpy(szTmp, pszFullFileNameWithoutExt);
    strcat(szTmp, ".txt");
    m_hLogFile = fopen(szTmp, "wb");
}

void CAVFrameWriter::Uninit()
{
    if(m_hAudioFile)
    {
        fclose(m_hAudioFile);
        m_hAudioFile = NULL;
    }

    if(m_hVideoFile)
    {
        fclose(m_hVideoFile);
        m_hVideoFile = NULL;
    }
    
    if(m_hLogFile)
    {
        fclose(m_hLogFile);
        m_hLogFile = NULL;
    }
}

void CAVFrameWriter::Write(bool bAudio, unsigned char* pBuf, int nSize, long long llTime, bool bKeyFrame, bool bHeadData)
{
    if(!pBuf || nSize<=0)
        return;
    
    if(bAudio && bHeadData)
    {
        // don't dump audio head data,becasue we don't need mux it
        return;
    }
    
    FILE* hFile = NULL;
    
    if(bAudio)
        hFile = m_hAudioFile;
    else
        hFile = m_hVideoFile;
    
    if(!hFile)
        return;

#if 1
    //write time stamp
    int nTmp = llTime;
    fwrite(&nTmp, 1, 4, hFile);
    
    //write size
    nTmp = nSize;
    if(bKeyFrame)
        nTmp |= 0x80000000;
    fwrite(&nTmp, 1, 4, hFile);
#endif
    
    //write data
    fwrite((char*)pBuf, 1, nSize, hFile);
    
    //write frame info
    hFile = m_hLogFile;
    
    if(!hFile)
        return;
    
    char szTmp[512];
    memset(szTmp, 0, 512);
    sprintf(szTmp, "%s,%06lld,%06d,%d,%lld\n", bAudio?"AUDIO":"VIDEO", llTime, nSize, bKeyFrame?1:0, llTime-(bAudio?m_llLastAudioTime:m_llLastVideoTime));
    fwrite(szTmp, 1, strlen(szTmp), hFile);
    
    if(bAudio)
        m_llLastAudioTime = llTime;
    else
        m_llLastVideoTime = llTime;
}

/////////////




/////////////////////////////////////////////////////////////
//////////Class CAVFrameReader //////////////////////////////

CAVFrameReader::CAVFrameReader()
:m_hAudioFile(NULL)
,m_hVideoFile(NULL)
,m_hAudioVerifyFile(NULL)
,m_hVideoVerifyFile(NULL)
,m_bOutputAudioHeadData(false)
,m_bOutputVideoHeadData(false)
{
    
}

CAVFrameReader::~CAVFrameReader()
{
    Uninit();
}

void CAVFrameReader::Init(const char* pszFullFileNameWithoutExt)
{
    Uninit();
    
    char szTmp[1024];
    
    memset(szTmp, 0, 1024);
    strcpy(szTmp, pszFullFileNameWithoutExt);
    strcat(szTmp, ".aac");
    m_hAudioFile = fopen(szTmp, "rb");
    
    memset(szTmp, 0, 1024);
    strcpy(szTmp, pszFullFileNameWithoutExt);
    strcat(szTmp, ".h264");
    m_hVideoFile = fopen(szTmp, "rb");
    
#if 0
    memset(szTmp, 0, 1024);
    strcpy(szTmp, pszFullFileNameWithoutExt);
    strcat(szTmp, "_dump.aac");
    m_hAudioVerifyFile = fopen(szTmp, "wb");
    
    memset(szTmp, 0, 1024);
    strcpy(szTmp, pszFullFileNameWithoutExt);
    strcat(szTmp, "_dump.h264");
    m_hVideoVerifyFile = fopen(szTmp, "wb");
#endif
}

void CAVFrameReader::Uninit()
{
    if(m_hAudioFile)
    {
        fclose(m_hAudioFile);
        m_hAudioFile = NULL;
    }
    
    if(m_hVideoFile)
    {
        fclose(m_hVideoFile);
        m_hVideoFile = NULL;
    }
    
    if(m_hAudioVerifyFile)
    {
        fclose(m_hAudioVerifyFile);
        m_hAudioVerifyFile = NULL;
    }

    if(m_hVideoVerifyFile)
    {
        fclose(m_hVideoVerifyFile);
        m_hVideoVerifyFile = NULL;
    }
}

void CAVFrameReader::Read(bool bAudio, unsigned char* pBuf, int& nSize, long long& llTime, bool& bKeyFrame, bool& bHeadData)
{
    if(!pBuf)
        return;
    
    nSize       = 0;
    llTime      = 0;
    bKeyFrame   = false;
    bHeadData   = false;
    
    FILE* hVerify   = NULL;
    FILE* hFile     = NULL;
    
    if(bAudio)
    {
        hVerify = m_hAudioVerifyFile;
        hFile   = m_hAudioFile;
        
        if(!m_bOutputAudioHeadData)
        {
            bHeadData = true;
            m_bOutputAudioHeadData = true;
        }
    }
    else
    {
        hVerify = m_hVideoVerifyFile;
        hFile   = m_hVideoFile;
        
        if(!m_bOutputVideoHeadData)
        {
            bHeadData = true;
            m_bOutputVideoHeadData = true;
        }
    }
    
    if(!hFile)
        return;
    
    //read time stamp
    int nTmp = 0;
    fread(&nTmp, 1, 4, hFile);
    llTime = nTmp;
    
    //read size
    nTmp = 0;
    fread(&nTmp, 1, 4, hFile);
    
    if(nTmp & 0x80000000)
    {
        nTmp &= ~0x80000000;
        bKeyFrame = true;
    }
    else
        bKeyFrame = false;
    
    nSize = nTmp;
    
    //read data
    fread((char*)pBuf, 1, nSize, hFile);
    
    if(nSize > 0 && hVerify)
        fwrite(pBuf, 1, nSize, hVerify);
    
    printf("Read %s: time-%06lld size-%06d key-%d headdata-%d\n", bAudio?"AUDIO":"VIDEO", llTime, nSize, bKeyFrame?1:0, bHeadData?1:0);
}


