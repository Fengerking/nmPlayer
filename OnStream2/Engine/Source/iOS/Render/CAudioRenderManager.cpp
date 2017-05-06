
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
	File:		CAudioRenderManager.cpp

	Contains:	CAudioRenderManager class file

	Written by:	Jim Lin 

	Change History (most recent first):
	2011-11-16		Jim			Create file

*******************************************************************************/
#include "CAudioRenderManager.h"
#ifdef _IOS
#include "voAudioRenderFactory.h"
#else
#include "CAudioQueueRender.h"
#endif

#include "voOSFunc.h"


#define LOG_TAG "CAudioRenderManager"
#include "voLog.h"

CAudioRenderManager::CAudioRenderManager(void* pUserData, RenderQueryCallback pQuery)
:voBaseRender (pUserData, pQuery)
,m_pAudioRender (NULL)
,m_dLeftVolume (1.0)
,m_dRightVolume (1.0)
,m_pAudioBuffer (NULL)
,m_uRenderSize (0)
,m_nSetBufferTime (0)
,m_bNeedInit (true)
,m_nReadSize (0)
{
	strcpy (m_szThreadName, "CAudioRenderManager");
}

CAudioRenderManager::~CAudioRenderManager()
{
    voBaseRender::Stop(true);
    
	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
#ifdef _IOS
		voAudioRenderFactory::Instance()->ReclaimAudioRender(m_pAudioRender);
#else
        delete m_pAudioRender;
#endif
		m_pAudioRender = NULL;
	}
}

int	CAudioRenderManager::Start (void)
{
	voBaseRender::Start ();

#if defined(_USE_OLD_AUDIO_RENDER) || defined (_MAC_OS)
	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();
#endif
	
	return 0;
}

int CAudioRenderManager::Pause ()
{
#ifdef _USE_OLD_AUDIO_RENDER
	voBaseRender::Pause (false);
#else
    voBaseRender::Pause (true);
#endif
    
	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();
    
	return 0;
}

int CAudioRenderManager::Stop ()
{
#ifdef _USE_OLD_AUDIO_RENDER
	voBaseRender::Stop (false);
#else
    voBaseRender::Stop (true);
#endif
    
    m_bNeedInit = true;

	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
	}
    
    m_pAudioBuffer = NULL;
    m_nReadSize = 0;
    
	return 0;
}

int CAudioRenderManager::Flush (void)
{
    m_pAudioBuffer = NULL;
    m_nReadSize = 0;
    
	if (m_pAudioRender != NULL)
		m_pAudioRender->Flush ();
    
#ifndef _USE_OLD_AUDIO_RENDER
    m_bNeedInit = true;
#endif
    
	m_uRenderSize = 0;
	return 0;
}

int CAudioRenderManager::SetVolume (float leftVolume, float rightVolume)
{
	m_dLeftVolume = leftVolume;
	m_dRightVolume = rightVolume;

	if (m_pAudioRender != NULL) {
		m_pAudioRender->SetVolume(m_dLeftVolume, m_dRightVolume);
    }

	return 0;
}

void CAudioRenderManager::InitAudioRender()
{    
    VOLOGI("Init audio render");
    
    m_bNeedInit = true;
    
	VOMP_AUDIO_FORMAT		sAudioFormat;
	memset (&sAudioFormat, 0, sizeof (VOMP_AUDIO_FORMAT));
	
    int nRet = VOMP_ERR_Pointer;
	if (m_pQuery)
		nRet = m_pQuery(m_pUserData, QT_GET_AUDIO_FMT, &sAudioFormat, NULL);

    if ((VOMP_ERR_None == nRet) && (0 < sAudioFormat.SampleRate)  && (0 < sAudioFormat.Channels)  && (0 < sAudioFormat.SampleBits)) {
        nRet = Open(sAudioFormat.SampleRate, sAudioFormat.Channels, sAudioFormat.SampleBits, 2);
    }
    else {
        VOLOGI("Init audio render fail");
        return;
    }
    
    if (VOMP_ERR_None == nRet) {
        m_bNeedInit = false;
    }
    else {
        return;
    }

	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();
	
	m_nSetBufferTime = 0;
}

int CAudioRenderManager::Open (int sampleRate, int channelCount, int format, int bufferCount)
{
	VOLOGI("open samplerate: %d channelcount:%d format:%d buffercount%d ",	sampleRate, channelCount, format, bufferCount );

	if (m_pAudioRender == NULL)
	{
#ifdef _IOS
        m_pAudioRender = voAudioRenderFactory::Instance()->CreateAudioRender();
#else
        m_pAudioRender = new CAudioQueueRender(NULL, NULL);
#endif
	}
	
	VO_AUDIO_FORMAT		sAudioFormat;
	sAudioFormat.SampleBits = format;
	sAudioFormat.Channels	= channelCount;
	sAudioFormat.SampleRate	= sampleRate;
	m_pAudioRender->SetFormat (&sAudioFormat);
    
    if (m_pAudioRender != NULL) {
        
        m_pAudioRender->SetCallBack((VOAUDIOCALLBACKPROC)AudioCallBack, this);
		m_pAudioRender->SetVolume(m_dLeftVolume, m_dRightVolume);
    }
    
	return 0;
}

int CAudioRenderManager::RenderThreadLoop (void)
{
	int nRC = VOMP_ERR_None;
    
#if defined (_USE_OLD_AUDIO_RENDER) || defined (_MAC_OS)
	m_pAudioBuffer = NULL;
	
	if(m_pQuery)
		nRC = m_pQuery(m_pUserData, QT_GET_ADUIO_BUFFER, &m_pAudioBuffer, NULL);

	if (nRC == VOMP_ERR_None && m_pAudioBuffer != NULL && m_pAudioBuffer->pBuffer)
	{
		if ((m_pAudioRender == NULL) || m_bNeedInit)
		{
            if (VO_THREAD_RUNNING != m_bThreadStatus) {
                voOS_Sleep(2);
                return nRC;
            }

			InitAudioRender();

			if (m_pAudioRender != NULL)
				m_pAudioRender->Start ();
		}

		if (m_pAudioRender != NULL) 
		{
			int nRet = m_pAudioRender->Render (m_pAudioBuffer->pBuffer, m_pAudioBuffer->nSize, 0, VO_TRUE);
			
			if (NULL != m_pQuery)
			{
                if (VO_ERR_FINISH == nRet) {
                    //Notify audio render done
                    m_pQuery(m_pUserData, QT_NOTIFY_AUDIO_RENDER_READY, NULL, NULL);
                }
                
                //Update audio render buffer time
				VO_S32 buffTime = 0;
				m_pAudioRender->GetBufferTime(&buffTime);
                
                if (m_nSetBufferTime != buffTime) {
                    m_nSetBufferTime = buffTime;
                    m_pQuery(m_pUserData, QT_SET_AUDIO_RENDER_BUF_TIME, &buffTime, NULL);
                    VOLOGI("Audio render buffer time %ld", buffTime);
                }
			}
		}
	}
	else if (nRC == VOMP_ERR_FormatChange)
	{
		InitAudioRender();
	}
	else if (nRC == VOMP_ERR_Retry)
	{
		voOS_Sleep(5);
	}
	else if(nRC == VOMP_ERR_WaitTime)
	{
		voOS_Sleep(2);
	}
	else
	{
		voOS_Sleep(5);
	}

#else
    
    if ((m_pAudioRender == NULL) || m_bNeedInit)
    {
        if (VO_THREAD_RUNNING != m_bThreadStatus) {
            voOS_Sleep(2);
            return nRC;
        }
        
        InitAudioRender();
    }
    
    if (!m_bNeedInit) {
        m_bThreadStatus = VO_THREAD_STOPPED;
        
        if (m_nSetBufferTime != 10) {
            m_nSetBufferTime = 10;
            m_pQuery(m_pUserData, QT_SET_AUDIO_RENDER_BUF_TIME, &m_nSetBufferTime, NULL);
            VOLOGI("Audio render buffer time %ld", m_nSetBufferTime);
        }
        
        if (m_pAudioRender != NULL)
            m_pAudioRender->Start ();
    }
#endif
    
	return nRC;
}

int CAudioRenderManager::doAudioCallBack(VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nStart)
{
    return fillData(pAudioBuffer, pAudioFormat, 0);
}

int CAudioRenderManager::fillData(VO_CODECBUFFER * pFillBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nFilledSize)
{
    if ((NULL == pFillBuffer) || (NULL == pAudioFormat)) {
        return VO_ERR_INVALID_ARG;
    }
    
    if (NULL == m_pAudioBuffer) {
        
        int nRC = VOMP_ERR_Retry;
        int nTry = 0;
        
        while (VOMP_ERR_None != nRC) {
            int nTime = voOS_GetSysTime();
            
            if(m_pQuery) {
                nRC = m_pQuery(m_pUserData, QT_GET_ADUIO_BUFFER, &m_pAudioBuffer, NULL);
            }
            
            int nTime1 = voOS_GetSysTime();
            if (nTime1 - nTime > 3) {
                VOLOGI("Use Time:%d", nTime1 - nTime);
            }
            
            if ((VOMP_ERR_None == nRC) && (m_pAudioBuffer != NULL)) {
                VOLOGR("Get success size:%d time:%d", m_pAudioBuffer->nSize, m_pAudioBuffer->llTime);
//#define DUMP_VOME_RENDER_PCM
#ifdef DUMP_VOME_RENDER_PCM
                // ============dump start
                static FILE *pFileAudioRenderOut = NULL;
                
                static void * pSelf = NULL;
                
                if (pSelf != this) {
                    pSelf = this;
                    if (NULL != pFileAudioRenderOut) {
                        fclose(pFileAudioRenderOut);
                        pFileAudioRenderOut = NULL;
                    }
                }
                
                if (NULL == pFileAudioRenderOut) {
                    char szTmp[256];
                    voOS_GetAppFolder(szTmp, 256);
                    strcat(szTmp, "AudioRenderVome.pcm");
                    pFileAudioRenderOut = fopen(szTmp, "wb");
                    
                    if (NULL == pFileAudioRenderOut) {
                        VOLOGI("------------------Open AudioRenderOut.pcm file error!\n");
                    }
                }
                
                if ((NULL != pFileAudioRenderOut) && (m_pAudioBuffer->nSize > 0))
                {
                    fwrite(m_pAudioBuffer->pBuffer, m_pAudioBuffer->nSize, 1, pFileAudioRenderOut);
                    fflush(pFileAudioRenderOut);
                }
                // ============dump end
                //	printf("RenderProcCallback size:%ld, time:%ld\n", m_pAudioBuffer->nSize, voOS_GetSysTime());
#endif
            }
            
            if (VOMP_ERR_Retry == nRC)
            {
                ++nTry;
                if (nTry >= 3) {
                    VOLOGI("Retry count:%d", nTry);
                    pFillBuffer->Length = nFilledSize;
                    return VO_ERR_FAILED;
                }
                voOS_Sleep(2);
                continue;
            }
            else if (nRC == VOMP_ERR_FormatChange)
            {
                VOLOGI("VOMP_ERR_FormatChange");
                
                VOMP_AUDIO_FORMAT sAudioFormat;
                memset (&sAudioFormat, 0, sizeof (VOMP_AUDIO_FORMAT));
                
                int nRet = VOMP_ERR_Pointer;
                if (m_pQuery) {
                    nRet = m_pQuery(m_pUserData, QT_GET_AUDIO_FMT, &sAudioFormat, NULL);
                }
                
                if (VOMP_ERR_None == nRet) {
                    pAudioFormat->Channels = sAudioFormat.Channels;
                    pAudioFormat->SampleBits = sAudioFormat.SampleBits;
                    pAudioFormat->SampleRate = sAudioFormat.SampleRate;
                }
                else {
                    VOLOGE("Get audio format fail");
                    return VO_ERR_FAILED;
                }
                
                pFillBuffer->Length = nFilledSize;
                
                return VO_ERR_NONE;
            }
            else if (nRC != VOMP_ERR_None)
            {
                VOLOGI("ERR:%d", nRC);
                m_pAudioBuffer = NULL;
                
                pFillBuffer->Length = nFilledSize;
                return VO_ERR_FAILED;
            }
        }
    }
    
    if (m_pAudioBuffer != NULL && m_pAudioBuffer->pBuffer)
    {
        if ((pFillBuffer->Length - nFilledSize) < (m_pAudioBuffer->nSize - m_nReadSize)) {
            memcpy(pFillBuffer->Buffer + nFilledSize, m_pAudioBuffer->pBuffer + m_nReadSize, pFillBuffer->Length - nFilledSize);
            m_nReadSize += pFillBuffer->Length - nFilledSize;
            return VO_ERR_NONE;
        }
        else {
            memcpy(pFillBuffer->Buffer + nFilledSize, m_pAudioBuffer->pBuffer + m_nReadSize, m_pAudioBuffer->nSize - m_nReadSize);
            nFilledSize += m_pAudioBuffer->nSize - m_nReadSize;
            m_pAudioBuffer = NULL;
            m_nReadSize = 0;
            
            // Need try to get next buffer, so we can know the format early
//            if (nFilledSize == pFillBuffer->Length) {
//                return VO_ERR_NONE;
//            }
            
            return fillData(pFillBuffer, pAudioFormat, nFilledSize);
        }
    }

    return VO_ERR_FAILED;
}

int CAudioRenderManager::AudioCallBack(VO_PTR pUserData, VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nStart)
{
    CAudioRenderManager *pManager = (CAudioRenderManager *)pUserData;
    if (NULL == pManager) {
        return VO_ERR_FAILED;
    }
    
    return pManager->doAudioCallBack(pAudioBuffer, pAudioFormat, nStart);
}
