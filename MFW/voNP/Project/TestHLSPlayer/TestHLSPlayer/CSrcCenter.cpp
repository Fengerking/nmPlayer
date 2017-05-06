//
//  CSrcCenter.cpp
//  
//
//  Created by Jim Lin on 9/22/12.
//
//

#include "CSrcCenter.h"


CSrcCenter::CSrcCenter ()
:m_pSrc1(NULL)
,m_pSrc2(NULL)
,m_pCurrSrc(NULL)
,m_nTimeStampOffset(0)
,m_nLastTimeStamp(0)
,m_bVideoOnly(false)
,m_bAudioOnly(false)
{
    m_fReadBuf.pUserData = this;
	m_fReadBuf.ReadAudio = SrcReadAudio;
	m_fReadBuf.ReadVideo = SrcReadVideo;
}

CSrcCenter::~CSrcCenter (void)
{
    Uninit();
}

int CSrcCenter::Init(void * pSource, int nFlag)
{
    Uninit();
    
    m_pSrc1     = new OnStreamReadSrc();
    m_pSrc1->SetParam(AV_FILE_PATH, pSource);
    m_pSrc1->Init((void*)"dump", 0);
    
    if(nFlag == 1)
    {
        m_pSrc2     = new OnStreamReadSrc();
        m_pSrc2->SetParam(AV_FILE_PATH, pSource);
        m_pSrc2->Init((void*)"dump2", 0);
    }
    
    m_pCurrSrc  = m_pSrc1;
    m_nTimeStampOffset = 0;
    m_nLastTimeStamp = 0;
    
    return VOOSMP_ERR_None;
}

int	CSrcCenter::Uninit(void)
{
    if(m_pSrc1)
    {
        m_pSrc1->Stop();
        m_pSrc1->Close();
        delete m_pSrc1;
        m_pSrc1 = NULL;
    }

    if(m_pSrc2)
    {
        m_pSrc2->Stop();
        m_pSrc2->Close();
        delete m_pSrc2;
        m_pSrc2 = NULL;
    }
    
    m_pCurrSrc = NULL;
    m_nTimeStampOffset = 0;
    m_nLastTimeStamp = 0;
    
    return VOOSMP_ERR_None;
}

int CSrcCenter::Open(void)
{
    if(m_pSrc1)
    {
        m_pSrc1->Open();
    }
    if(m_pSrc2)
    {
        m_pSrc2->Open();
    }

    return VOOSMP_ERR_None;
}

int CSrcCenter::Run (void)
{
    if(m_pSrc1)
    {
        m_pSrc1->Run();
    }
    if(m_pSrc2)
    {
        m_pSrc2->Run();
    }

    return VOOSMP_ERR_None;
}

int CSrcCenter::Pause (void)
{
    if(m_pCurrSrc)
        return m_pCurrSrc->Pause();
    
    return VOOSMP_ERR_None;
}

int CSrcCenter::Stop (void)
{
    if(m_pCurrSrc)
        return m_pCurrSrc->Stop();
    
    return VOOSMP_ERR_None;
}

int CSrcCenter::Close (void)
{
    if(m_pSrc1)
    {
        m_pSrc1->Close();
    }
    if(m_pSrc2)
    {
        m_pSrc2->Close();
    }
    return VOOSMP_ERR_None;
}

int CSrcCenter::Flush (void)
{
    if(m_pCurrSrc)
        return m_pCurrSrc->Flush();
    return VOOSMP_ERR_None;
}

int CSrcCenter::GetDuration (int * pDuration)
{
    int nDuration = 0;
    
    if(m_pSrc1)
    {
        m_pSrc1->GetDuration(&nDuration);
        *pDuration = nDuration;
    }

    if(m_pSrc2)
    {
        m_pSrc2->GetDuration(&nDuration);
        *pDuration += nDuration;
    }

    return VOOSMP_ERR_None;
}

int CSrcCenter::SetCurPos (int nCurPos)
{
    nCurPos -= m_nTimeStampOffset;
    
    if(m_pCurrSrc)
        return m_pCurrSrc->SetCurPos(nCurPos);
    
    return VOOSMP_ERR_None;
}

int CSrcCenter::GetParam (int nID, void * pValue)
{
    return VOOSMP_ERR_None;
}

int CSrcCenter::SetParam (int nID, void * pValue)
{
    if(m_pSrc1)
        m_pSrc1->SetParam(nID, pValue);

    if(m_pSrc2)
        m_pSrc2->SetParam(nID, pValue);
    
    return VOOSMP_ERR_None;
}

void* CSrcCenter::GetReadBufPtr()
{    
    return &m_fReadBuf;
}

int CSrcCenter::doSrcReadAudio(VOOSMP_BUFFERTYPE * pBuffer)
{
    if(m_pCurrSrc)
    {
        VOOSMP_READBUFFER_FUNC* pR = (VOOSMP_READBUFFER_FUNC*)m_pCurrSrc->GetReadBufPtr();
        
        if(m_bVideoOnly)
            return VOOSMP_ERR_Audio_No_Now;
        
        if(pR)
        {
            int nRet = pR->ReadAudio(pR->pUserData, pBuffer);
            
            if(VOOSMP_ERR_EOS == nRet)
            {
                if(m_pCurrSrc==m_pSrc1 && m_pSrc2)
                {
                    //switch next source
                    m_pCurrSrc = m_pSrc2;
                    m_nTimeStampOffset = m_nLastTimeStamp;
                    nRet = VOOSMP_ERR_None;
                }
            }
            else if(VOOSMP_ERR_None == nRet)
            {
                m_nLastTimeStamp = pBuffer->llTime;
                pBuffer->llTime += m_nTimeStampOffset;
            }
            
            return nRet;
        }
    }

    return VOOSMP_ERR_Retry;
}

int CSrcCenter::doSrcReadVideo(VOOSMP_BUFFERTYPE * pBuffer)
{
    if(m_pCurrSrc)
    {
        VOOSMP_READBUFFER_FUNC* pR = (VOOSMP_READBUFFER_FUNC*)m_pCurrSrc->GetReadBufPtr();
        
        if(m_bAudioOnly)
            return VOOSMP_ERR_Video_No_Now;

        if(pR)
        {
            int nRet = pR->ReadVideo(pR->pUserData, pBuffer);
            
            if(VOOSMP_ERR_EOS == nRet)
            {
                if(m_pCurrSrc==m_pSrc1 && m_pSrc2)
                {
                    //switch next source
                    m_pCurrSrc = m_pSrc2;
                    m_nTimeStampOffset = m_nLastTimeStamp;
                    nRet = VOOSMP_ERR_None;
                }
            }
            else if(VOOSMP_ERR_None == nRet)
            {
                m_nLastTimeStamp = pBuffer->llTime;
                pBuffer->llTime += m_nTimeStampOffset;
            }
            
            return nRet;
        }
    }
    
    return VOOSMP_ERR_Retry;
}


int CSrcCenter::SrcReadAudio(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer)
{
    if(pUserData)
    {
        CSrcCenter* pSrc = (CSrcCenter*)pUserData;
        return pSrc->doSrcReadAudio(pBuffer);
    }
    
    return VOOSMP_ERR_None;
}

int CSrcCenter::SrcReadVideo(void * pUserData, VOOSMP_BUFFERTYPE * pBuffer)
{
    if(pUserData)
    {
        CSrcCenter* pSrc = (CSrcCenter*)pUserData;
        return pSrc->doSrcReadVideo(pBuffer);
    }
    
    return VOOSMP_ERR_None;
}
