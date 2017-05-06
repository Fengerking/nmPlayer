	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCLivePlayer.cpp

	Contains:	voCLivePlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <stdlib.h>
#include <aknnotewrappers.h>
#elif defined _IOS
#include <stdlib.h>
#elif defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__
#include <string.h>
#include <stdio.h>

#ifdef _LINUX_ANDROID
#ifndef __VO_NDK__
#include <cutils/properties.h>
#include <utils/threads.h>
#endif  //__VO_NDK__
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif // _LINUX_ANDROID

#include "voOSFunc.h"

#include "voCLivePlayer.h"

#define LOG_TAG "voCLivePlayer"
#include "voLog.h"

#ifdef _WIN32
#ifdef _METRO

#else   //METRO
#include "CWaveOutRender.h"
#include "CGDIVideoRender.h"
#endif  //METRO
#endif // _WIN32

#if defined (_IOS)
//#include "CAudioQueueRender.h"
#include "CAudioUnitRender.h"
#include "CiOSVideoRender.h"
#endif

#if defined (_MAC_OS)
#include "CAudioQueueRender.h"
#include "CMacOSVideoRender.h"
#endif

#ifdef _MAC_OS_CG
#include "CCGRender.h"
#endif

voCLivePlayer::voCLivePlayer(VOMPListener pListener, void * pUserData)
	: voCMediaPlayer (pListener, pUserData)
	, m_nBufferMode (1)
	, m_llAVOffsetTime (0)
	, m_pMixSource (NULL)
	, m_nAudioRenderCount (10)
	, m_ppAudioOutBuffers (NULL)
	, m_nAudioOutFillIndex (0)
	, m_nAudioOutPlayIndex (0)
	, m_pAudioRndBuff (NULL)
	, m_nAudioSourceCount (64)
	, m_ppAudioSrcBuffers (NULL)
	, m_nAudioSrcFillIndex (0)
	, m_nAudioSrcPlayIndex (0)
	, m_llLastAudioRndTime (0)
	, m_llStartVideoTime (0)
{
	m_nVideoRenderCount = 6;
	memset (&m_sAudioEmptyData, 0, sizeof (VOMP_BUFFERTYPE));
}

voCLivePlayer::~voCLivePlayer(void)
{
	ReleaseAudioBuffer ();

	if (m_sAudioEmptyData.pBuffer != NULL)
	{
		delete []m_sAudioEmptyData.pBuffer;
		m_sAudioEmptyData.pBuffer = NULL;
		delete (VOMP_AUDIO_FORMAT *)m_sAudioEmptyData.nReserve;
		m_sAudioEmptyData.nReserve = 0;
	}

	if (m_pMixSource != NULL)
	{
		delete m_pMixSource;
		m_pMixSource = NULL;
	}
}

int voCLivePlayer::SetDataSource (void * pSource, int nFlag)
{
	voCAutoLock lock (&m_mtStatus);
#ifndef _MAC_OS
#ifndef _IOS
	VOLOGI ("pSource %d, nFlag %d", (int)pSource, nFlag);
#endif
#endif
	if (m_pMsgThread != NULL)
		m_pMsgThread->Start (m_fThreadCreate);

	if (m_nStatus == VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	Reset ();

	m_pSource = pSource;
	m_nOpenFlag = nFlag;

	if (!CreateAudioDec ())
		return VOMP_ERR_Audio;

	if (!CreateVideoDec ())
		return VOMP_ERR_Video;

	m_bAudioEOS = VO_FALSE;
	m_bVideoEOS = VO_FALSE;

	m_nVideoEOSArrived = 0;

	m_llAVOffsetTime = 0;

	m_nAudioOutFillIndex = 0;
	m_nAudioOutPlayIndex = 0;

	UpdateAudioFormat (&m_sAudioFormat);

	memcpy (&m_sAudioRndFmt, &m_sAudioFormat, sizeof (m_sAudioRndFmt));

	m_nFF = VO_FILE_FFUNKNOWN;
	if (m_nOpenFlag & VOMP_FLAG_SOURCE_SENDBUFFER_TS)
		m_nFF = VO_FILE_FFMOVIE_TS;
	else if (m_nOpenFlag & VOMP_FLAG_SOURCE_SENDBUFFER_MFS)
		m_nFF = VO_FILE_FFMOVIE_CMMB;

	if (m_nFF != VO_FILE_FFUNKNOWN)
		m_pMixSource = new voCMixDataSource (m_hInst, m_nFF, &g_memOP, m_pFileOP, NULL);
	if (m_pMixSource != NULL)
	{
		m_pMixSource->SetLivePlayer (this);
		m_pMixSource->LoadSource (pSource, 0, 0, 0);
	}

	if (m_nBufferMode == 0)
	{
		if (m_ppAudioSrcBuffers == NULL)
			m_ppAudioSrcBuffers = new VOMP_BUFFERTYPE *[m_nAudioSourceCount];
		if(!m_ppAudioSrcBuffers)
			return VOMP_ERR_OutMemory;
		for (VO_U32 i = 0; i < m_nAudioSourceCount; i++)
			m_ppAudioSrcBuffers[i] = NULL;
	}

	return VOMP_ERR_None;
}

int voCLivePlayer::SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
//	voCAutoLock lock (&m_mtStatus);
	if (nSSType == VOMP_SS_Video)
		return SendVideoSample (pBuffer);
	else if (nSSType == VOMP_SS_Audio)
		return SendAudioSample (pBuffer);
	else if (m_pMixSource != NULL)
		return m_pMixSource->AddBuffer (nSSType, pBuffer);
	
	return VOMP_ERR_Unknown;
}

int voCLivePlayer::SendAudioSample (VOMP_BUFFERTYPE * pBuffer)
{
	if (pBuffer == NULL)
		return VO_ERR_INVALID_ARG;

	VO_S32 nRC = VO_ERR_NONE;
	if (m_nBufferMode == 0)
	{
		voCAutoLock lockAudioSrc (&m_mtAudioSource);
		if (m_nAudioSrcFillIndex >= m_nAudioSrcPlayIndex + m_nAudioSourceCount - 1)
			return VOMP_ERR_Retry;

		int nIndex = m_nAudioSrcFillIndex % m_nAudioSourceCount;
		if (m_ppAudioSrcBuffers[nIndex] == NULL)
		{
			m_ppAudioSrcBuffers[nIndex] = new VOMP_BUFFERTYPE ();
			if(!m_ppAudioSrcBuffers[nIndex])
				return VOMP_ERR_OutMemory;
			memset (m_ppAudioSrcBuffers[nIndex], 0, sizeof (VOMP_BUFFERTYPE));
		}

		if (m_ppAudioSrcBuffers[nIndex]->nReserve < pBuffer->nSize)
		{
			if (m_ppAudioSrcBuffers[nIndex]->pBuffer != NULL)
			{
				delete []m_ppAudioSrcBuffers[nIndex]->pBuffer;
				m_ppAudioSrcBuffers[nIndex]->pBuffer = NULL;
			}

			m_ppAudioSrcBuffers[nIndex]->nReserve = pBuffer->nSize * 2;
			m_ppAudioSrcBuffers[nIndex]->pBuffer = new VO_BYTE[pBuffer->nSize * 2];
			if(!m_ppAudioSrcBuffers[nIndex]->pBuffer)
				return VOMP_ERR_OutMemory;
		}

		memcpy (m_ppAudioSrcBuffers[nIndex]->pBuffer, pBuffer->pBuffer, pBuffer->nSize);
		m_ppAudioSrcBuffers[nIndex]->nSize = pBuffer->nSize;
		m_ppAudioSrcBuffers[nIndex]->llTime = pBuffer->llTime;
		m_ppAudioSrcBuffers[nIndex]->nFlag = pBuffer->nFlag;

		m_nAudioSrcFillIndex++;

		return nRC;
	}

	if (m_pAudioDec == NULL)
		return VOMP_ERR_Status;

	if (pBuffer->nFlag == VOMP_FLAG_BUFFER_FORCE_FLUSH)
	{
		Flush ();
		return VOMP_ERR_None;
	}

	if (pBuffer->nFlag == VOMP_FLAG_BUFFER_EOS)
	{
		m_bAudioEOS = VO_TRUE;
		return VOMP_ERR_EOS;
	}

	if (pBuffer->nFlag == VOMP_FLAG_BUFFER_NEW_PROGRAM || pBuffer->nFlag == VOMP_FLAG_BUFFER_NEW_FORMAT)
	{
		bool bAudio = CreateAudioDec ();
		if(bAudio == false)
		{
			if(m_pAudioDec)
			{
				delete m_pAudioDec;
				m_pAudioDec = NULL;
			}
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sAudioTrackInfo.Codec);
			return VOMP_ERR_Audio;
		}

		voCAutoLock lock (&m_mtPlayTime);
//		m_nStartSysTime = 0;
//		m_llAVOffsetTime = 0;
//		m_nStartSysTime = 0;
	}

	if (pBuffer->pBuffer == NULL || pBuffer->nSize <= 0)
		return VOMP_ERR_Pointer;

//	if (rand () % 2 == 0)
//		return VO_ERR_NONE;
//	voOS_Sleep (30);

//	VOLOGI ("Fill %d, Play %d, Buff: %d", m_nAudioOutFillIndex, m_nAudioOutPlayIndex, m_nAudioOutFillIndex - m_nAudioOutPlayIndex);

	if (m_nAudioOutFillIndex >= m_nAudioOutPlayIndex + m_nAudioRenderCount - 1)
		return VOMP_ERR_Retry;

	if (m_nAudioOutFillIndex > 1 && (m_nAudioOutPlayIndex + 1 > m_nAudioOutFillIndex))
		return VOMP_ERR_Retry;

	if (m_llAVOffsetTime == 0)
	{
		voCAutoLock lock (&m_mtPlayTime);
		m_llAVOffsetTime = pBuffer->llTime;
	}

	if (m_sAudioSample.Size > 0)
	{
		nRC = DecAudioSample ();
		if ( nRC == VO_ERR_NONE || nRC == VOMP_ERR_FormatChange )
			return VOMP_ERR_Retry;
	}

	m_sAudioDecInBuf.Buffer = pBuffer->pBuffer;
	m_sAudioDecInBuf.Length = pBuffer->nSize;
	m_sAudioDecInBuf.Time   = pBuffer->llTime - m_llAVOffsetTime;
	if (m_sAudioDecInBuf.Time < 0)
	{
		m_sAudioDecInBuf.Time = 0;
		m_llAVOffsetTime = m_sAudioDecInBuf.Time;
	}

	m_sAudioSample.Time = m_sAudioDecInBuf.Time;
	m_sAudioSample.Buffer = m_sAudioDecInBuf.Buffer;
	m_sAudioSample.Size = m_sAudioDecInBuf.Length;

	m_nAudioDecSize = 0;

	nRC = m_pAudioDec->SetInputData(&m_sAudioDecInBuf);
	if(nRC != VO_ERR_NONE)
		return VOMP_ERR_Audio;

	nRC = DecAudioSample ();

	return nRC;
}

int voCLivePlayer::SendVideoSample (VOMP_BUFFERTYPE * pBuffer)
{
	VO_S32 nRC = VO_ERR_NONE;

	if (m_pVideoDec == NULL)
		return VOMP_ERR_Status;

	if (pBuffer->nFlag == VOMP_FLAG_BUFFER_FORCE_FLUSH)
	{
		Flush ();
		return VOMP_ERR_None;
	}

	if (pBuffer->nFlag == VOMP_FLAG_BUFFER_EOS)
	{
		if (!m_bVideoEOS)
		{
			VOLOGI ("Read at end of track!");
			ShowResult ();
		}

		m_bVideoEOS = VO_TRUE;
		return VOMP_ERR_EOS;
	}

	if (pBuffer->nFlag == VOMP_FLAG_BUFFER_NEW_PROGRAM || pBuffer->nFlag == VOMP_FLAG_BUFFER_NEW_FORMAT)
	{
 		bool bVideo = CreateVideoDec ();
		if(bVideo == false)
		{
			postCheckStatusEvent (VOMP_CB_CodecNotSupport, m_sVideoTrackInfo.Codec);
			return VOMP_ERR_Implement;
		}

		voCAutoLock lock (&m_mtPlayTime);
		m_nStartSysTime = 0;
		m_llAVOffsetTime = 0;
		m_nStartSysTime = 0;
	}

	if (pBuffer->pBuffer == NULL || pBuffer->nSize <= 0)
		return VOMP_ERR_Pointer;

//	VOLOGI ("Fill %d, Play %d, Buff: %d", m_nVideoOutFillIndex, m_nVideoOutPlayIndex, m_nVideoOutFillIndex - m_nVideoOutPlayIndex);

	if (m_nVideoOutFillIndex >= m_nVideoOutPlayIndex + m_nVideoRenderCount - 1)
		return VOMP_ERR_Retry;

	if (m_llAVOffsetTime == 0)
	{
		voCAutoLock lock (&m_mtPlayTime);
		m_llAVOffsetTime = pBuffer->llTime;
	}

	if (m_bMoreVideoBuff && m_sVideoSample.Size > 0)
	{
		nRC = DecVideoSample ();
		if (nRC == VOMP_ERR_None || nRC == VOMP_ERR_Retry || nRC == VOMP_ERR_FormatChange )
			return VOMP_ERR_Retry;
	}

	m_sVideoDecInBuf.Buffer = pBuffer->pBuffer;
	m_sVideoDecInBuf.Length = pBuffer->nSize;
	m_sVideoDecInBuf.Time   = pBuffer->llTime - m_llAVOffsetTime;
	if (m_sVideoDecInBuf.Time < 0)
	{
		m_sVideoDecInBuf.Time = 0;
		m_llAVOffsetTime = m_sVideoDecInBuf.Time;
	}

	m_sVideoSample.Time = m_sVideoDecInBuf.Time;
	m_sVideoSample.Buffer = m_sVideoDecInBuf.Buffer;
	m_sVideoSample.Size = m_sVideoDecInBuf.Length;

	nRC = m_pVideoDec->SetInputData(&m_sVideoDecInBuf);
	if(nRC != VO_ERR_NONE)
		return VOMP_ERR_Video;

	nRC = DecVideoSample ();

	return nRC;
}

int voCLivePlayer::Run (void)
{
	voCAutoLock lock (&m_mtStatus);

	if (m_nStatus == VOMP_STATUS_RUNNING)
		return VOMP_ERR_None;

	m_nStatus = VOMP_STATUS_RUNNING;
	
	if (m_pMixSource != NULL)
		m_pMixSource->Start ();

#if defined _IOS || defined(_MAC_OS)
	m_nStartSysTime = 0;
#endif	

	// start audio stream
	if (m_nAudioPlayMode == VOMP_PUSH_MODE)
	{
		if (m_pAudioThread == NULL)
		{
			m_pAudioThread = new voCBaseThread ("vomp Audio Render");
			if(!m_pAudioThread)
				return VOMP_ERR_OutMemory;
		}
		m_pAudioThread->Start (m_fThreadCreate);

		postAudioRenderEvent (-1);
	}

	// Start video stream
	if (m_nVideoPlayMode == VOMP_PUSH_MODE)
	{
#if defined _IOS || defined(_MAC_OS)
		// It seems that need to create video render in main thread on iOS.
		if(m_pVideoRender == NULL)
		{			
#ifdef _IOS
			m_pVideoRender = new CiOSVideoRender (NULL, m_hView, NULL);
#else
			
#ifdef _MAC_OS_CG
			m_pVideoRender = new CCGRender(NULL, m_hView, NULL);
#else
			m_pVideoRender = new CMacOSVideoRender (NULL, m_hView, NULL);
#endif
			
#endif
			if(!m_pVideoRender)
				return VOMP_ERR_OutMemory;
			m_pVideoRender->Start();
			VO_RECT r;
			r.left=m_rcDraw.left;r.top=m_rcDraw.top;r.right=m_rcDraw.right;r.bottom=m_rcDraw.bottom;
			m_pVideoRender->SetDispRect (m_hView, &r, (VO_IV_COLORTYPE)m_nColorType);
			m_pVideoRender->SetVideoInfo (m_sVideoFormat.Width, m_sVideoFormat.Height, VO_COLOR_YUV_PLANAR420);
			m_pVideoRender->SetParam (VO_PID_COMMON_LIBOP, (VO_LIB_OPERATOR *)m_pLibFunc);
		}
#endif	
		if (m_pVideoThread == NULL)
		{
			m_pVideoThread = new voCBaseThread ("vomp Video Render");
			if(!m_pVideoThread)
				return VOMP_ERR_OutMemory;
		}
		m_pVideoThread->Start (m_fThreadCreate);

		postVideoRenderEvent (-1);
	}

	m_bSendStartVideoBuff = VO_TRUE;
	postCheckStatusEvent (VOMP_CB_VideoStartBuff);

	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();

	if (m_pAudioDec != NULL)
		m_pAudioDec->Start ();
	if (m_pVideoDec != NULL)
		m_pVideoDec->Start ();

#ifndef _MAC_OS
#ifndef _IOS
	m_nStartSysTime = 0;
#endif
#endif
	
	m_nLastVideoSysTime = 0;
	if (m_nStartPlayTime == 0)
		m_nStartPlayTime = voOS_GetSysTime () - m_nSeekPos;

	if (m_pMsgThread != NULL)
	{
		m_pMsgThread->Start ();
		// m_pMsgThread->CancelEventByType (VOMP_EVENT_STATUS);
		m_pMsgThread->CancelEventByMsg (VOMP_CB_CheckStatus);
	}

	postCheckStatusEvent (VOMP_CB_CheckStatus, 0, 0, m_nStatusCheckTime);

	return VOMP_ERR_None;
}

int voCLivePlayer::Pause (void)
{
	if (m_pVideoDecThread != NULL)
		m_pVideoDecThread->Pause ();
	if (m_pVideoThread != NULL)
		m_pVideoThread->Pause ();
	if (m_pAudioThread != NULL)
		m_pAudioThread->Pause ();

	voCAutoLock lock (&m_mtStatus);

	m_nStatus = VOMP_STATUS_PAUSED;
	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();

	if (m_pAudioDec != NULL)
		m_pAudioDec->Pause ();
	if (m_pVideoDec != NULL)
		m_pVideoDec->Pause ();

	return VOMP_ERR_None;
}

int voCLivePlayer::Stop (void)
{
	if (m_nStatus == VOMP_STATUS_STOPPED)
		return VOMP_ERR_None;

	ShowResult ();

	if (m_pMixSource != NULL)
		m_pMixSource->Stop ();

	if (m_pVideoDecThread != NULL)
		m_pVideoDecThread->Stop ();
	if (m_pVideoThread != NULL)
		m_pVideoThread->Stop ();
	if (m_pAudioThread != NULL)
		m_pAudioThread->Stop ();

	voCAutoLock lock (&m_mtStatus);
	
	VOMP_STATUS nOldStatus = m_nStatus;
	m_nStatus = VOMP_STATUS_STOPPED;
	m_bSeekingPos = VO_FALSE;

	if (m_pAudioDec != NULL)
		m_pAudioDec->Stop ();
	if (m_pVideoDec != NULL)
		m_pVideoDec->Stop ();

	if (m_pAudioRender != NULL)
		m_pAudioRender->Stop ();

#ifndef __SYMBIAN32__
	if (m_pVideoRender != NULL)
		delete m_pVideoRender;
	m_pVideoRender = NULL;
#endif // __SYMBIAN32__

	if (m_nFF != VO_FILE_FFSTREAMING_RTSP && m_nFF != VO_FILE_FFAPPLICATION_SDP && m_nFF != VO_FILE_FFSTREAMING_HTTPPD)
	{
		if (nOldStatus != VOMP_STATUS_STOPPED && m_nGetThumbnail == 0)
		{
			SetCurPos (0);
		}
	}
	
	ResetVideoSample(NULL);

	m_bNotifyCompleted = false;

	if (m_pMsgThread != NULL)
		m_pMsgThread->Stop ();

	if (m_pFileSink != NULL)
	{
		m_pFileSink->Stop ();
	}
	return VOMP_ERR_None;
}

int voCLivePlayer::Flush (void)
{
	VOLOGI ("!");

	voCAutoLock lockAudio (&m_mtAudioFlush);
	voCAutoLock lockVideo (&m_mtVideoFlush);

	if (m_nStatus <= VOMP_STATUS_LOADING)
		return VOMP_ERR_Status;

	VOMP_STATUS oldStatus = m_nStatus;
	if (m_nStatus == VOMP_STATUS_RUNNING)
		Pause ();

	voCAutoLock lock (&m_mtStatus);

	if (m_pVideoDec != NULL)
		m_pVideoDec->Flush();

	if (m_pAudioDec != NULL)
		m_pAudioDec->Flush();

	if (m_pAudioRender != NULL)
		m_pAudioRender->Flush ();

	m_sVideoDecOutBuf.Time = 0;
	m_sAudioDecOutBuf.Time = 0;
	m_nStartSysTime = 0;
	m_nStartMediaTime = 0;
	m_nVideoRenderNum = 0;
	m_nVideoRndNuming = 0;
	m_nVideoDecoderNum = 0;
	m_nAudioRenderNum = 0;
	m_nAudioOutSize = 0;
	m_nStartPlayTime = 0;
	m_llAudioRenderTime = 0;
	m_llRenderBufferTime = 0;
	m_llRenderSystemTime = 0;

	m_bSendStartVideoBuff = VO_FALSE;
	m_bSendStartAudioBuff = VO_FALSE;

	m_nStartPlayTime = voOS_GetSysTime ();
	m_nVideoDcdTimeIndex = 0;
	m_nVideoRndTimeIndex = 0;
	m_nDroppedVidooFrames = 0;

	m_nVideoOutFillIndex = 0;
	m_nVideoOutPlayIndex = 0;

	if (m_nVideoTrack >= 0)
		m_bVideoEOS = VO_FALSE;
	if (m_nAudioTrack >= 0)
		m_bAudioEOS = VO_FALSE;

	m_bNotifyCompleted = false;

	m_llAVOffsetTime = 0;
	m_nAudioOutFillIndex = 0;
	m_nAudioOutPlayIndex = 0;

	m_llLastAudioRndTime = 0;

	if (oldStatus == VOMP_STATUS_RUNNING)
	{
		Run ();
	}

	return 0;
}

int voCLivePlayer::GetDuration (int * pDuration)
{
	voCAutoLock lock (&m_mtStatus);

	*pDuration = 0;

	return VOMP_ERR_None;
}

int voCLivePlayer::GetCurPos (int * pCurPos)
{
	voCAutoLock lock (&m_mtStatus);

	*pCurPos = 0;

	return VOMP_ERR_None;
}

int voCLivePlayer::SetCurPos (int nCurPos)
{
	return VOMP_ERR_Implement;
}

int voCLivePlayer::GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	if (m_bAudioEOS)
	{
		voOS_Sleep (10);
		return VOMP_ERR_EOS;
	}

	voCAutoLock lockAudio (&m_mtAudioFlush);

	if (m_nAudioPlayMode == VOMP_PUSH_MODE)
		return VOMP_ERR_Status;

	if (m_nStatus == VOMP_STATUS_PAUSED)
		return VOMP_ERR_Retry;

	m_pAudioOutBuff = m_pAudioPcmBuff;

	int nRC = DoAudioRender (ppBuffer);

	return nRC;
}

int voCLivePlayer::DoAudioRender (VOMP_BUFFERTYPE ** ppBuffer)
{
	if (!m_bSetAudioRendererThreadPriority)
	{
		m_bSetAudioRendererThreadPriority = true;
#ifdef _LINUX_ANDROID
//		setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_AUDIO);
#endif //_LINUX_ANDROID
#ifdef _WIN32
		voThreadSetPriority(GetCurrentThread (), VO_THREAD_PRIORITY_ABOVE_NORMAL);
#endif // _WIN32
	}

	if (m_bAudioEOS  && m_nAudioOutPlayIndex >= m_nAudioOutFillIndex)
	{
		voOS_Sleep (10);
		return VOMP_ERR_EOS;
	}

	int nRC = VOMP_ERR_None; //ReadAudioSample ();
	if (m_nBufferMode == 0)
	{
		nRC = ReadAudioSample ();
		if (nRC == VOMP_ERR_FormatChange)
		{
			m_nAudioOutSize = 0;
			if (m_pAudioRender != NULL)
				m_pAudioRender->SetFormat (&m_sAudioFormat);
		}
		if (nRC != VOMP_ERR_None)
			return nRC;

		if (m_nAudioOutSize < m_nAudioStepSize)
			return VOMP_ERR_Retry;

		if (ppBuffer == NULL)
		{
			voCMediaPlayer::RenderAudioSample ();
		}
		else
		{
			if (*ppBuffer == NULL)
			{
				m_bufAudio.pBuffer = m_pAudioOutBuff;
				m_bufAudio.nSize = m_nAudioOutSize;
				m_bufAudio.llTime = m_llAudioRenderTime;
				*ppBuffer = (VOMP_BUFFERTYPE *)&m_bufAudio;
			}
			else
			{
				// memcpy (pBuffer->pBuffer, m_pAudioOutBuff, m_nAudioOutSize);
				(*ppBuffer)->nSize = m_nAudioOutSize;
				(*ppBuffer)->llTime = m_llAudioRenderTime;
			}
		}		
		
		return nRC;
	}

	{ // lock the index of the play list
	voCAutoLock lockAudioDec (&m_mtAudioRender);

//	VOLOGI ("              Fill %d, Play %d, Buff: %d", m_nAudioOutFillIndex, m_nAudioOutPlayIndex, m_nAudioOutFillIndex - m_nAudioOutPlayIndex);

	if (m_ppAudioOutBuffers == NULL || m_nAudioOutFillIndex <= 1)
		return VOMP_ERR_Retry;

	if (m_nAudioOutPlayIndex == 0 && m_nAudioOutFillIndex < m_nAudioRenderCount / 3)
		return VOMP_ERR_Retry;

	if (m_sAudioEmptyData.pBuffer == NULL)
	{
		m_sAudioEmptyData.pBuffer = new unsigned char[m_nAudioOutSize];
		if(!m_sAudioEmptyData.pBuffer)
			return VOMP_ERR_OutMemory;
		m_sAudioEmptyData.nSize = m_nAudioOutSize;
		memset (m_sAudioEmptyData.pBuffer, 0, m_nAudioOutSize);

#ifdef _MAC_OS
		m_sAudioEmptyData.nReserve = (unsigned long) new VOMP_AUDIO_FORMAT;
#else
		m_sAudioEmptyData.nReserve = (int) new VOMP_AUDIO_FORMAT;
#endif
		if(!m_sAudioEmptyData.nReserve)
			return VOMP_ERR_OutMemory;
	}

	if (m_nAudioOutPlayIndex >= m_nAudioOutFillIndex - 1)
	{
		if ((VO_S64)GetPlayingTime () + (VO_S64)m_nAudioBuffTime < m_llLastAudioRndTime)
			return VOMP_ERR_Retry;

		memcpy ((VOMP_AUDIO_FORMAT*)m_sAudioEmptyData.nReserve, &m_sAudioRndFmt, sizeof (VOMP_AUDIO_FORMAT));
		m_pAudioRndBuff = &m_sAudioEmptyData;

		VOLOGI ("@@@@@@   Add empty audio data   %d, %d", (int)m_nAudioOutFillIndex, (int)m_nAudioOutPlayIndex);
	}
	else
	{
		VO_U32 nIndex = m_nAudioOutPlayIndex % m_nAudioRenderCount;
		m_pAudioRndBuff = m_ppAudioOutBuffers[nIndex];
		if (m_pAudioRndBuff == NULL)
			return VOMP_ERR_Retry;

		VO_U64 nPlayingTime = GetPlayingTime ();
		if ((m_nAudioOutFillIndex < m_nAudioOutPlayIndex + 2) && (m_pAudioRndBuff->llTime > (VO_S64)nPlayingTime + (VO_S64)m_nAudioBuffTime + (VO_S64)(m_nAudioStepTime * 2)))
		{
			memcpy ((VOMP_AUDIO_FORMAT*)m_sAudioEmptyData.nReserve, &m_sAudioRndFmt, sizeof (VOMP_AUDIO_FORMAT));
			m_pAudioRndBuff = &m_sAudioEmptyData;

			VOLOGI ("@@@ ### Add the empty audio frame   %d, %d, %d", (int)m_pAudioRndBuff->llTime, (int)nPlayingTime, (int)(nPlayingTime + m_nAudioBuffTime + m_nAudioStepTime * 2 - nPlayingTime));
		}
		else
		{
			m_nAudioOutPlayIndex++;

			if (m_nAudioOutPlayIndex > 20)
			{
				if (nPlayingTime > m_pAudioRndBuff->llTime + m_nAudioBuffTime / 2)
				{
					VOLOGI ("####### Drap the audio frame   %d, %d, %d        %d,  %d", (int)nPlayingTime, (int)m_pAudioRndBuff->llTime, (int)(nPlayingTime - (m_pAudioRndBuff->llTime + m_nAudioStepTime * 2)), (int)m_nAudioOutFillIndex, (int)m_nAudioOutPlayIndex);
					return VOMP_ERR_Retry;
				}
			}
		}

	}
	}

	VOMP_AUDIO_FORMAT * pAudioFmt = (VOMP_AUDIO_FORMAT *)m_pAudioRndBuff->nReserve;

	if (pAudioFmt->SampleRate != m_sAudioRndFmt.SampleRate || pAudioFmt->Channels != m_sAudioRndFmt.Channels)
	{
		memcpy (&m_sAudioRndFmt, pAudioFmt, sizeof (VOMP_AUDIO_FORMAT));
		return VOMP_ERR_FormatChange;
	}

//	VOLOGI ("Audio Render Time: Buff: % 8d, Play: % 8d, Diff: % 8d",(int)m_pAudioRndBuff->llTime, GetPlayingTime (), GetPlayingTime () - (int)m_pAudioRndBuff->llTime);

	if (ppBuffer == NULL)
	{
		RenderAudioSample ();
	}
	else
	{
		if (*ppBuffer == NULL)
		{
			m_bufAudio.pBuffer = m_pAudioRndBuff->pBuffer;
			m_bufAudio.nSize = m_pAudioRndBuff->nSize;
			m_bufAudio.llTime = m_pAudioRndBuff->llTime;
			*ppBuffer = (VOMP_BUFFERTYPE *)&m_bufAudio;
		}
		else
		{
			memcpy ((*ppBuffer)->pBuffer, m_pAudioRndBuff->pBuffer, m_pAudioRndBuff->nSize);
			(*ppBuffer)->nSize = m_pAudioRndBuff->nSize;
			(*ppBuffer)->llTime = m_pAudioRndBuff->llTime;
		}
	}

	m_llLastAudioRndTime = m_pAudioRndBuff->llTime;

	return VOMP_ERR_None;
}

int voCLivePlayer::DoVideoRender (VOMP_BUFFERTYPE ** ppBuffer)
{
	return voCMediaPlayer::DoVideoRender (ppBuffer);
}

int voCLivePlayer::onRenderAudio (int nMsg, int nV1, int nV2)
{
	return voCMediaPlayer::onRenderAudio (nMsg, nV1, nV2);
}

int voCLivePlayer::onRenderVideo (int nMsg, int nV1, int nV2)
{
	return voCMediaPlayer::onRenderVideo (nMsg, nV1, nV2);
}

int voCLivePlayer::Reset (void)
{
	return voCMediaPlayer::Reset ();
}

int voCLivePlayer::ReadAudioSample (void)
{
	VO_S32 nRC = 0;
	if (m_sAudioSample.Size > 0 && m_pAudioDec != NULL)
	{
		// tag: 20101228
#ifndef USE_HW_AUDIO_DEC	
		nRC = voCMediaPlayer::DecAudioSample ();
		if ( nRC == VO_ERR_NONE || nRC == VOMP_ERR_FormatChange )
			return nRC;
#endif
	}

	voCAutoLock lockAudioSrc (&m_mtAudioSource);

	if (m_nAudioSrcPlayIndex >= m_nAudioSrcFillIndex)
		return VOMP_ERR_Retry;

	int nIndex = m_nAudioSrcPlayIndex % m_nAudioSourceCount;
	m_nAudioSrcPlayIndex++;

	VOMP_BUFFERTYPE * pBuffer = m_ppAudioSrcBuffers[nIndex];
	if (pBuffer == NULL)
		return VOMP_ERR_Pointer;

	m_sAudioDecInBuf.Buffer = pBuffer->pBuffer;
	m_sAudioDecInBuf.Length = pBuffer->nSize;
	m_sAudioDecInBuf.Time   = pBuffer->llTime - m_llAVOffsetTime;

	m_sAudioSample.Time = m_sAudioDecInBuf.Time;
	m_sAudioSample.Buffer = m_sAudioDecInBuf.Buffer;
	m_sAudioSample.Size = m_sAudioDecInBuf.Length;

	m_nAudioDecSize = 0;

	nRC = m_pAudioDec->SetInputData(&m_sAudioDecInBuf);
	if(nRC != VO_ERR_NONE)
		return VOMP_ERR_Audio;

	nRC = voCMediaPlayer::DecAudioSample ();
	if (nRC >= 0)
		return nRC;

	return VOMP_ERR_Unknown;
}

int voCLivePlayer::DecAudioSample (void)
{
//	int nRC = voCMediaPlayer::DecAudioSample ();
	if (m_pAudioDec == NULL)
		return VOMP_ERR_Status;

//	VOLOGI ("m_nAudioFrameSize  %d,   m_nAudioOutSize is %d  ", m_nAudioFrameSize, m_nAudioOutSize);

	if (m_nAudioOutSize >= m_nAudioStepSize)
	{
		m_sAudioDecOutBuf.Time = m_sAudioSample.Time + m_nAudioDecSize * 1000 / m_nAudioFrameSize;
		m_nAudioOutSize = 0;
	}

	if (m_pAudioEffect == NULL)
	{
		m_sAudioDecOutBuf.Buffer = m_pAudioOutBuff + m_nAudioOutSize;
		m_sAudioDecOutBuf.Length = m_nAudioBufferSize - m_nAudioOutSize;	
	}
	else
	{
		m_sAudioDecOutBuf.Buffer = m_pAudioEffectPcmBuff + m_nAudioOutSize;
		m_sAudioDecOutBuf.Length = m_nAudioFrameSize - m_nAudioOutSize;	
	}

	VO_U64	nStartTime = (VO_U64)voOS_GetSysTime ();
	VO_AUDIO_FORMAT	fmtAudio;
	int nRC = m_pAudioDec->GetOutputData(&m_sAudioDecOutBuf , &fmtAudio);

	if(nRC == VOMP_ERR_None)
	{
		if (m_pAudioEffect == NULL)
		{
			if(fmtAudio.Channels != m_sAudioFormat.Channels || fmtAudio.SampleBits != m_sAudioFormat.SampleBits || fmtAudio.SampleRate != m_sAudioFormat.SampleRate)
			{
				memcpy(&m_sAudioFormat , &fmtAudio , sizeof(m_sAudioFormat));
				UpdateAudioFormat (&m_sAudioFormat);
				VOLOGI ("The audio format was changed to SampleRate %d, Channels %d, Bits %d",  (int)fmtAudio.SampleRate, (int)fmtAudio.Channels,  (int)fmtAudio.SampleBits);
				return VOMP_ERR_FormatChange;
			}
		}
		else
		{
			if(fmtAudio.Channels != m_sAudioEftFmt.Channels || fmtAudio.SampleBits != m_sAudioEftFmt.SampleBits || fmtAudio.SampleRate != m_sAudioEftFmt.SampleRate)
			{
				memcpy(&m_sAudioEftFmt , &fmtAudio , sizeof(m_sAudioEftFmt));
				m_pAudioEffect->SetFormat (&m_sAudioEftFmt);
			}
		}
		
		if (m_pFileSink != NULL && m_smpSinkAudio.Buffer == NULL)
		{
			nRC = m_pFileSink->SetParam (VO_PID_AUDIO_FORMAT, &m_sAudioFormat);

			m_smpSinkAudio.Buffer   = m_sAudioSample.Buffer;
			m_smpSinkAudio.Size     = m_sAudioSample.Size;
			m_smpSinkAudio.Time     = m_sAudioSample.Time;
			m_smpSinkAudio.Duration = 1;
			m_smpSinkAudio.nAV		= 0; // audio
			
			m_pFileSink->AddSample (&m_smpSinkAudio);
		}

		nRC = effectAudioSample_Volume (m_sAudioDecOutBuf.Buffer, m_sAudioDecOutBuf.Length);
		if (nRC == VOMP_ERR_FormatChange)
		{
			return VOMP_ERR_FormatChange;
		}

		if (m_pAudioEffect == NULL)
		{
			m_nAudioOutSize = m_nAudioOutSize + m_sAudioDecOutBuf.Length;
		}

		m_nAudioDecSize = m_nAudioDecSize + m_sAudioDecOutBuf.Length;
		m_nAudioDecTimes = m_nAudioDecTimes + ((VO_U64)voOS_GetSysTime () - nStartTime);
	}
	else if ((nRC & 0X8000000F) == VO_ERR_INPUT_BUFFER_SMALL)
	{
		return VOMP_ERR_SmallSize;
	}
	else if ((nRC & 0X8000000F) == VO_ERR_OUTPUT_BUFFER_SMALL)
	{
		if (m_nAudioOutSize > 4096 && m_nAudioStepSize > m_nAudioOutSize)
		{
			m_nAudioStepSize = m_nAudioOutSize;
			return VOMP_ERR_None;
		}
	}

	if (nRC >= 0)
	{
		if (nRC == VOMP_ERR_None)
		{
			if (m_nAudioOutSize < m_nAudioStepSize)
				return VOMP_ERR_None;

			voCAutoLock lockAudio (&m_mtAudioRender);

			if (m_ppAudioOutBuffers == NULL)
			{
				m_ppAudioOutBuffers = new VOMP_BUFFERTYPE * [m_nAudioRenderCount];
				for (VO_U32 i = 0; i < m_nAudioRenderCount; i++)
				{
					m_ppAudioOutBuffers[i] = new VOMP_BUFFERTYPE ();
					if(!m_ppAudioOutBuffers[i])
						return VOMP_ERR_OutMemory;
					memset (m_ppAudioOutBuffers[i], 0, sizeof (VOMP_BUFFERTYPE));
					
#ifdef _MAC_OS
					m_ppAudioOutBuffers[i]->nReserve = (unsigned long) new VOMP_AUDIO_FORMAT;
#else
					m_ppAudioOutBuffers[i]->nReserve = (int) new VOMP_AUDIO_FORMAT;
#endif
					if(!m_ppAudioOutBuffers[i]->nReserve)
						return VOMP_ERR_OutMemory;
				}
			}

			int nIndex = m_nAudioOutFillIndex % m_nAudioRenderCount;
			if ((VO_U32)m_ppAudioOutBuffers[nIndex]->nFlag < m_nAudioOutSize)
			{
				if (m_ppAudioOutBuffers[nIndex]->pBuffer != NULL)
				{
					delete []m_ppAudioOutBuffers[nIndex]->pBuffer;
					m_ppAudioOutBuffers[nIndex]->pBuffer = NULL;
				}

				m_ppAudioOutBuffers[nIndex]->nFlag = m_nAudioOutSize * 2;
				m_ppAudioOutBuffers[nIndex]->pBuffer = new VO_BYTE[m_nAudioOutSize * 2];
				if(!m_ppAudioOutBuffers[nIndex]->pBuffer)
					return VOMP_ERR_OutMemory;
			}

			memcpy (m_ppAudioOutBuffers[nIndex]->pBuffer, m_pAudioOutBuff, m_nAudioOutSize);
			m_ppAudioOutBuffers[nIndex]->nSize = m_nAudioOutSize;
			m_ppAudioOutBuffers[nIndex]->llTime = m_sAudioDecOutBuf.Time;

			memcpy ((VOMP_AUDIO_FORMAT*)m_ppAudioOutBuffers[nIndex]->nReserve, &m_sAudioFormat, sizeof (VOMP_AUDIO_FORMAT));

			m_nAudioOutFillIndex++;

			return VOMP_ERR_None;
		}
		else if (nRC == VOMP_ERR_FormatChange)
		{
			return VOMP_ERR_FormatChange;
		}
	}

	return nRC;
}

int voCLivePlayer::RenderAudioSample (void)
{
//	if (m_nAudioOutSize < m_nAudioStepSize)
//		return -1;

	if (m_pListener != NULL)
	{
		m_bufAudio.pBuffer = m_pAudioRndBuff->pBuffer;
		m_bufAudio.nSize = m_pAudioRndBuff->nSize;
		m_bufAudio.llTime = m_pAudioRndBuff->llTime;
		if (m_pListener (m_pUserData, VOMP_CB_RenderAudio, &m_bufAudio, &m_sAudioRndFmt) == VOMP_ERR_None)
		{
			return 0;
		}
	}

	if (m_pAudioRender == NULL)
	{
#ifdef _METRO
#elif defined  _WIN32
		m_pAudioRender = new CWaveOutRender (NULL, NULL);
		if(!m_pAudioRender)
			return VOMP_ERR_OutMemory;
		m_pAudioRender->SetFormat ((VO_AUDIO_FORMAT *)&m_sAudioRndFmt);
		m_pAudioRender->Start ();
#elif defined _IOS
		//m_pAudioRender = new CAudioQueueRender (NULL, NULL);
		m_pAudioRender = new CAudioUnitRender (NULL, NULL);
		if(!m_pAudioRender)
			return VOMP_ERR_OutMemory;
		
#ifdef USE_HW_AUDIO_DEC
		if(m_sAudioTrackInfo.Codec == VOMP_AUDIO_CodingAAC)
			m_pAudioRender->SetInputFormat(INPUT_AAC);
		else if(m_sAudioTrackInfo.Codec == VOMP_AUDIO_CodingMP3)
			m_pAudioRender->SetInputFormat(INPUT_MP3);
#endif
		m_pAudioRender->SetFormat (&m_sAudioFormat);
		m_pAudioRender->Start ();
#elif defined _MAC_OS
		m_pAudioRender = new CAudioQueueRender (NULL, NULL);
		if(!m_pAudioRender)
					return VOMP_ERR_OutMemory;
		m_pAudioRender->SetFormat ((VO_AUDIO_FORMAT *)&m_sAudioFormat);
		m_pAudioRender->Start ();		
#endif // _WIN32
	}

	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Render (m_pAudioRndBuff->pBuffer, m_pAudioRndBuff->nSize, m_pAudioRndBuff->llTime, VO_TRUE);
		
#if defined _MAC_OS || defined _IOS
		VO_S32 buffTime = 0;
		m_pAudioRender->GetBufferTime(&buffTime);
		// removed by Lin Jun 20110425
		//m_nAudioBuffTime = buffTime;
		//printf("Audio buffer time = %d\n", buffTime);
#endif
	}
		
	return 0;
}

int voCLivePlayer::DecVideoSample (void)
{
	return voCMediaPlayer::DecVideoSample ();
}

int voCLivePlayer::RenderVideoSample (void)
{
	return voCMediaPlayer::RenderVideoSample ();
}

int voCLivePlayer::CheckVideoRenderTime (void)
{
	return voCMediaPlayer::CheckVideoRenderTime ();
}

bool voCLivePlayer::CreateAudioDec (void)
{
	if (m_pAudioDec != NULL)
	{
		delete m_pAudioDec;
		m_pAudioDec = NULL;
	}
//	ReleaseAudioBuffer ();

	if (m_sAudioTrackInfo.Codec <= 0)
		m_sAudioTrackInfo.Codec = VOMP_AUDIO_CodingAAC;
	
#if defined(_IOS) && defined(USE_HW_AUDIO_DEC)
	m_pAudioDec = new CAudioMockDecoder (m_hInst , m_sAudioTrackInfo.Codec, &g_memOP);
#else
	m_pAudioDec = new CAudioDecoder (m_hInst , m_sAudioTrackInfo.Codec, &g_memOP);
#endif	
	if (m_pAudioDec == NULL)
	{
		VOLOGE ("It could not create audio dec class!");
		return false;
	}
	m_pAudioDec->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
#ifndef __SYMBIAN32__
#ifndef _MAC_OS
#ifndef _IOS
	m_pAudioDec->SetConfig (&m_cfgVOSDK);
#endif // _IOS
#endif // _MAC_OS
#endif  //__SYMBIAN32__

	int nRC = m_pAudioDec->Init (NULL, 0, &m_sAudioFormat);
	if (nRC == VOMP_ERR_None)
		return true;

	return false;
}

bool voCLivePlayer::CreateVideoDec (void)
{
	if (m_pVideoDec != NULL)
	{
		delete m_pVideoDec;
		m_pVideoDec = NULL;
	}

	ReleaseVideoRenderBuffers ();

	if (m_sVideoTrackInfo.Codec <= 0)
		m_sVideoTrackInfo.Codec = VOMP_VIDEO_CodingH264;

	m_pVideoDec = new CVideoDecoder (m_hInst, m_sVideoTrackInfo.Codec, &g_memOP);
	if (m_pVideoDec == NULL)
	{
		VOLOGE ("It could not create CVideoDecoder");
		return false;
	}
	m_pVideoDec->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibFunc);
#ifndef __SYMBIAN32__
#ifndef _MAC_OS
#ifndef _IOS
	m_pVideoDec->SetConfig (&m_cfgVOSDK);
#endif // _IOS
#endif // _MAC_OS
#endif // __SYMBIAN32__

		// david 2011/11/30
	m_pVideoDec->SetSourcePtr((void*)m_pFileSource);
	VOMP_LISTENERINFO li;
	li.pListener = m_pListener;
	li.pUserData = m_pUserData;
	m_pVideoDec->SetListenerInfo(&li);
	//end 

	int nRC = m_pVideoDec->Init (NULL, 0, &m_sVideoFormat);
	if (nRC == VOMP_ERR_None)
		return true;

	return false;
}

VO_U64 voCLivePlayer::GetPlayingTime (VO_BOOL bForUser /* = VO_FALSE */)
{
	voCAutoLock lock (&m_mtPlayTime);

	if (m_nVideoRenderNum == 0)
		return 0;

	// for discontinue program
	if (m_llStartVideoTime > m_pVideoOutputBuff->Time)
		m_nStartSysTime = 0;

	if (m_nStartSysTime == 0)
	{
		if (m_pVideoOutputBuff->Time <= 0)
			return 1;
		m_nStartSysTime = (VO_U64)(voOS_GetSysTime () - m_pVideoOutputBuff->Time);
		m_llStartVideoTime = m_pVideoOutputBuff->Time;
	}

	if (m_nStatus == VOMP_STATUS_RUNNING)
	{
		return (VO_U64)voOS_GetSysTime () - m_nStartSysTime;
	}
	else if (m_nStatus == VOMP_STATUS_PAUSED)
	{
		if (m_pVideoOutputBuff == NULL)
			return 0;

		return (VO_U64)(m_pVideoOutputBuff->Time - m_nAudioBuffTime);
	}
	else
		return 0;
}

int voCLivePlayer::ReleaseAudioBuffer (void)
{
	// Release audio source buffers
	voCAutoLock lockAudioSrc (&m_mtAudioSource);
	VO_U32 i = 0; 

	if (m_ppAudioSrcBuffers != NULL)
	{
		for (i = 0; i < m_nAudioSourceCount; i++)
		{
			if (m_ppAudioSrcBuffers[i] != NULL)
			{
				if (m_ppAudioSrcBuffers[i]->pBuffer != NULL)
				{
					delete []m_ppAudioSrcBuffers[i]->pBuffer;
					m_ppAudioSrcBuffers[i]->pBuffer = NULL;
				}
				delete m_ppAudioSrcBuffers[i];
				m_ppAudioSrcBuffers[i] = NULL;
			}
		}

		delete []m_ppAudioSrcBuffers;
		m_ppAudioSrcBuffers = NULL;
	}
	m_ppAudioSrcBuffers = NULL;

	m_nAudioSrcFillIndex = 0;
	m_nAudioSrcPlayIndex = 0;

	// Release the render buffers
	voCAutoLock lockAudioRnd (&m_mtAudioRender);
	if (m_ppAudioOutBuffers != NULL)
	{
		for (i = 0; i < m_nAudioRenderCount; i++)
		{
			if (m_ppAudioOutBuffers[i] != NULL)
			{
				if (m_ppAudioOutBuffers[i]->pBuffer != NULL)
				{
					delete []m_ppAudioOutBuffers[i]->pBuffer;
					m_ppAudioOutBuffers[i]->pBuffer = NULL;
				}

				delete (VOMP_AUDIO_FORMAT *) m_ppAudioOutBuffers[i]->nReserve;
				m_ppAudioOutBuffers[i]->nReserve = 0;

				delete m_ppAudioOutBuffers[i];
				m_ppAudioOutBuffers[i] = NULL;
			}
		}

		delete []m_ppAudioOutBuffers;
		m_ppAudioOutBuffers = NULL;
	}
	m_ppAudioOutBuffers = NULL;

	m_nAudioOutFillIndex = 0;
	m_nAudioOutPlayIndex = 0;

	return 0;
}
