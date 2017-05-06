	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include <stdlib.h>
#include "CRTSPSource.h"

#include "voLog.h"

#define VORTSP_MAX_FRAME_SIZE		100*1024

static int g_voRTSPSourceStatus = 0;

CRTSPSource * CRTSPSource::g_pRTSPSource = NULL;

CRTSPSource::CRTSPSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
	: CFileSource (hInst, nFormat, pMemOP, pFileOP)
	, m_bHadOpenned (VO_FALSE)
	, m_nTracks (0)
	, m_ppBuffer (NULL)
	, m_bBuffering(VO_FALSE)
	, m_bBufferEndMsg (VO_FALSE)
	, m_nAudioFirstTime (0)
	, m_nAudioSendTime (0)
	, m_nAudioSendSamples (0)
	, m_nVideoSendSamples (0)
	, m_lEventCode (0)
	, m_pEventParam (NULL)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_bSendError (VO_FALSE)
{
	VOLOGF ();

	memset(&m_initParam,0,sizeof(m_initParam));
	m_initParam.nMaxUdpPort		= DEFAULT_PORT_MAX;
	m_initParam.nMinUdpPort		= DEFAULT_PORT_MIN;
	m_initParam.nRTPTimeOut		= DEFAULT_RTPTIMEOUT;
	m_initParam.nBufferTime		= 3;//DEFAULT_BUFFERTIME;
	m_initParam.nRTCPInterval	= 5000;
	m_initParam.nFirstIFrame	= 1;
	m_initParam.nExtraRTPPacket	= 1;
	m_initParam.nConnectTimeout	= DEFAULT_RTPTIMEOUT;
	m_initParam.nRTSPTimeout	= DEFAULT_RTPTIMEOUT;
	m_initParam.nSDKlogflag		= 0;//for test
	strcpy(m_initParam.szUserAgent,"VisualOn Streaming Player 2.2");

	m_paramOpen.nReserve = (VO_U32)&m_initParam;
	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_RTSPCALLBACK | VO_SOURCE_OPENPARAM_FLAG_OPENRTSP;

	g_pRTSPSource = this;
	g_voRTSPSourceStatus++;
}

CRTSPSource::~CRTSPSource ()
{
	VOLOGF ();

	ReleaseFrameBuffer ();

	g_pRTSPSource = NULL;
	g_voRTSPSourceStatus--;
}

void CRTSPSource::ReleaseFrameBuffer (void)
{
	VOLOGF ();

	if (m_ppBuffer != NULL)
	{
		for (VO_U32 i = 0; i < m_nTracks; i++)
		{
			if (m_ppBuffer[i] != NULL)
				free (m_ppBuffer [i]);
		}

		free (m_ppBuffer);
		m_ppBuffer = NULL;
	}
}

VO_U32 CRTSPSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	VOLOGF ();

	if (g_voRTSPSourceStatus > 1)
	{
		VOLOGE ("The RTSP Source didn't close correctly!");
		return VO_ERR_WRONG_STATUS;
	}
	CloseSource ();
	m_bHadOpenned = VO_FALSE;
	m_bForceClosed = VO_FALSE;

	m_bSendError = VO_FALSE;

	if (LoadLib (m_hInst) == 0)
		return VO_ERR_FAILED;

	StreamingNotifyEventFunc funEvent;
	funEvent.funtcion = voRTSPStreamCallBack;
	funEvent.userdata = this;

#ifdef _WIN32
	memset (m_szURL, 0, sizeof (m_szURL));
	WideCharToMultiByte (CP_ACP, 0, (VO_PTCHAR)pSource, -1, m_szURL, 1024, NULL, NULL);
#elif defined LINUX
	strcpy (m_szURL, (VO_PCHAR)pSource);
#endif // _WIN32

	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_RTSPCALLBACK;
	m_paramOpen.pSource = m_szURL;
	m_paramOpen.pSourceOP = &funEvent;
	VO_U32 nRC = m_funFileRead.Open (&m_hFile, &m_paramOpen);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_funFileRead.Open failed. 0X%08X", nRC);
		return nRC;
	}

	nRC = SetSourceParam (VOID_STREAMING_OPENURL, 0);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("VOID_STREAMING_OPENURL failed. 0X%08X", nRC);
		return nRC;
	}
	if (m_bForceClosed)
		return VO_ERR_WRONG_STATUS;

	// m_funFileRead.SetSourceParam (m_hFile, VOID_STREAMING_LOAD_CALLBACK, &funEvent);

	nRC = m_funFileRead.GetSourceInfo (m_hFile, &m_filInfo);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_funFileRead.GetSourceInfo failed. 0X%08X", nRC);
		return nRC;
	}

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;
	for (i = 0; i < m_filInfo.Tracks; i++)
	{
		nRC = m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
		if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			m_nVideoTrack = i;
		else if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
			m_nAudioTrack = i;
	}

	ReleaseFrameBuffer ();

	m_nTracks = m_filInfo.Tracks;
	if (m_nTracks > 0)
	{
		m_ppBuffer = (VO_PBYTE *) malloc (m_nTracks * sizeof (VO_PBYTE));
		for (VO_U32 i = 0; i < m_nTracks; i++)
			m_ppBuffer[i] = (VO_PBYTE) malloc (VORTSP_MAX_FRAME_SIZE);
	}

	if (m_pFirstSample != NULL)
		delete []m_pFirstSample;
	m_pFirstSample = new VO_BOOL[m_filInfo.Tracks];
	for (VO_U32 i = 0; i < m_filInfo.Tracks; i++)
		m_pFirstSample[i] = VO_TRUE;

	m_bHadOpenned = VO_TRUE;
	m_bBufferEndMsg = VO_FALSE;

	return VO_ERR_NONE;
}

VO_U32 CRTSPSource::CloseSource (void)
{
	VOLOGF ();

//	voCAutoLock lock (&m_csRTSPStatus);
	if (m_bForceClosed)
		return 0;

	m_bForceClosed = VO_TRUE;
	if (m_hFile != NULL)
	{
		m_funFileRead.Close (m_hFile);
		m_hFile = NULL;
	}

//	VOLOGI ("The HTTP PD Source g_voHTTPPDSourceStatus is %d.", g_voHTTPPDSourceStatus);

	return VO_ERR_NONE;
}

VO_U32 CRTSPSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	VOLOGR ();

	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	if (nTrack < 0 || nTrack >= m_nTracks)
		return VO_ERR_WRONG_STATUS;

	if (m_bBuffering)
		return VO_ERR_SOURCE_NEEDRETRY;

/*
	// make sure send audio data first.
	if ((nTrack == m_nVideoTrack && m_nVideoSendSamples > 0) && (m_nAudioTrack >= 0 && m_nAudioSendTime < 500))
	{
		VOLOGW ("VO_ERR_SOURCE_NEEDRETRY");
		return VO_ERR_SOURCE_NEEDRETRY;
	}
*/

	VO_U32 nRC = 0;

	VO_SOURCE_SAMPLE * pSample = (VO_SOURCE_SAMPLE *)pTrackData;
	pSample->Buffer = m_ppBuffer[nTrack];
	pSample->Size = VORTSP_MAX_FRAME_SIZE;

	if (m_pFirstSample[nTrack])
	{
		m_pFirstSample[nTrack] = VO_FALSE;

		VO_SOURCE_TRACKINFO trkInfo;
		nRC = GetTrackInfo (nTrack, &trkInfo);
		if (nRC == VO_ERR_NONE)
		{
			if (trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO)
			{
				if (trkInfo.Codec == VO_AUDIO_CodingWMA || trkInfo.Codec == VO_AUDIO_CodingADPCM)
				{
					trkInfo.HeadSize = 0;
					nRC = GetTrackParam (nTrack, VO_PID_SOURCE_WAVEFORMATEX, &trkInfo.HeadData);
					if (nRC == VO_ERR_NONE)
					{
						VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)trkInfo.HeadData;
						trkInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
					}
				}
			}
			else if (trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO)
			{
				if (trkInfo.Codec == VO_VIDEO_CodingWMV)
				{
					trkInfo.HeadSize = 0;
					nRC = GetTrackParam (nTrack, VO_PID_SOURCE_BITMAPINFOHEADER, &trkInfo.HeadData);
					if (nRC == VO_ERR_NONE)
					{
						VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)trkInfo.HeadData;
						trkInfo.HeadSize = pBmpInfo->biSize;
					}
				}
			}

			if (trkInfo.HeadSize > 0)
			{
				pSample->Buffer = trkInfo.HeadData;
				pSample->Size = 0X80000000 + trkInfo.HeadSize;
				pSample->Time = 0;

				return VO_ERR_NONE;
			}
		}
	}

	VOLOGR ("m_funFileRead.GetSample");
	nRC = m_funFileRead.GetSample (m_hFile, nTrack, pSample);
	VOLOGR ("m_funFileRead.GetSample. Size %d, Time %d, Track %d Result 0X%08X", (pSample->Size & 0X7FFFFFFF), (int)pSample->Time, nTrack, nRC);

	if (nRC == VO_ERR_NONE)
	{
		if (!m_bBufferEndMsg)
		{
			m_bBufferEndMsg = VO_TRUE;
			if (m_pCallBack != NULL)
			{
				long nValue = 100;
				m_pCallBack (VO_STREAM_BUFFERSTOP, &nValue, (long *)m_pUserData);
			}
		}

		if (nTrack == m_nVideoTrack)
		{
			m_nVideoSendSamples++;
		}
		else if (nTrack == m_nAudioTrack)
		{
			m_nAudioSendSamples++;

			if (m_nAudioFirstTime == 0)
				m_nAudioFirstTime = pSample->Time;
			m_nAudioSendTime = pSample->Time - m_nAudioFirstTime;
		}
	}

	return nRC;
}

VO_BOOL CRTSPSource::CanSeek (void)
{
	VOLOGF ();

	if (m_bBuffering)
		return VO_FALSE;

	return VO_TRUE;
}

VO_U32 CRTSPSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	VOLOGF ();

	if (m_nVideoTrack >= 0 && nTrack != m_nVideoTrack)
		return VO_ERR_NONE;

	if (m_bBuffering)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = CFileSource::SetTrackPos (nTrack, pPos);

	m_bBuffering = VO_TRUE;

	return nRC;
}

VO_U32 CRTSPSource::Start(void)
{
	VOLOGF ();

	VO_U32	nParam = 0;
	return 	SetSourceParam (VO_RTSP_PLAY, &nParam);
}

VO_U32 CRTSPSource::Pause(void)
{
	VOLOGF ();

	VO_U32	nParam = 0;
	return 	SetSourceParam (VO_RTSP_PAUSE, &nParam);
}

VO_U32 CRTSPSource::Stop(void)
{
	VOLOGF ();

	VO_U32	nParam = 0;
	return 	SetSourceParam (VO_RTSP_STOP, &nParam);
}

VO_U32 CRTSPSource::SetCallBack (VO_PTR pCallBack, VO_PTR pUserData)
{
	VOLOGF ();

	m_pCallBack = (STREAMING_CALLBACK_NOTIFY) pCallBack;
	m_pUserData = pUserData;

	return 0;
}

int CRTSPSource::HandleStreamEvent (long EventCode, long * EventParam1)
{
	VOLOGF ();
	if (m_bForceClosed)
		return 0;

	m_lEventCode = EventCode;
	m_pEventParam = EventParam1;

	if (m_lEventCode == VO_EVENT_BUFFERING_BEGIN)
	{
		m_bBuffering = VO_TRUE;
		m_nAudioFirstTime = 0;
		m_nAudioSendTime = 0;
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_BUFFERSTART, EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_EVENT_BUFFERING_PERCENT)
	{
		m_bBuffering = VO_TRUE;
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_BUFFERSTATUS, EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_EVENT_BUFFERING_END)
	{
		m_bBuffering = VO_FALSE;
		m_bBufferEndMsg = VO_TRUE;
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_BUFFERSTOP, EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode >= VO_EVENT_CONNECT_FAIL && m_lEventCode <= VO_EVENT_SOCKET_ERR)
	{
		VOLOGI ("Error Event %d", m_lEventCode);

		if (m_pCallBack != NULL && !m_bSendError)
		{
			m_pCallBack (VO_STREAM_ERROR, (long *)EventParam1, (long *)m_pUserData);
			m_bSendError = VO_TRUE;
		}
	}

	return 1;
}

int CRTSPSource::voRTSPStreamCallBack (long EventCode, long * EventParam1, long * userData)
{
	if (g_pRTSPSource == NULL)
		return 0;

	CRTSPSource * pSource = (CRTSPSource *)userData;

	return pSource->HandleStreamEvent (EventCode, EventParam1);
}
