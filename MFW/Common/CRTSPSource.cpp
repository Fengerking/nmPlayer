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
#include "voOSFunc.h"
#include "CRTSPSource.h"

#define LOG_TAG "CRTSPSource"
#include "voLog.h"

#if defined (_LINUX_ANDROID)
#ifdef _ACER
#   include <cutils/properties.h>
#endif // _ACER
#endif

#define VORTSP_MAX_FRAME_SIZE		524288		//512K

static int			g_voRTSPSourceStatus = 0;

#ifdef _HTC
static char			g_szExtraFields[1024] = {0};
#endif	//_HTC

CRTSPSource::CRTSPSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	: CFileSource (hInst, nFormat, pMemOP, pFileOP, pDrmCB)
	, m_bHadOpenned (VO_FALSE)
	, m_nTracks (0)
	, m_ppBuffer (NULL)
	, m_bBuffering(VO_FALSE)
	, m_bBufferEndMsg (VO_FALSE)
	, m_nAudioSendSamples (0)
	, m_nVideoSendSamples (0)
	, m_nInitPlayPos(0)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_bSendError (VO_FALSE)
	, m_nStatus(RTSP_STATUS_NULL)
	, m_lEventCode (0)
{
	memset(&m_initParam, 0, sizeof(m_initParam));

	VO_BOOL bParamInitialized = VO_FALSE;

#ifdef _HTC
	CDllLoad dllLoad;
	vostrcpy (dllLoad.m_szDllFile, _T("HTC_mm_property.so"));
	vostrcpy (dllLoad.m_szAPIName , _T("mm_property_get"));
	if (dllLoad.LoadLib (NULL) > 0)
	{
		MMPROPERTYGETAPI pEntry = (MMPROPERTYGETAPI)dllLoad.m_pAPIEntry;
		if (pEntry != NULL)
		{
			char value[MM_PROP_VALUE_MAX];

			pEntry(MM_PROP_MaxUdpPort, value, "65535");
			m_initParam.nMaxUdpPort = atoi(value);

			pEntry(MM_PROP_MinUdpPort, value, "1024");
			m_initParam.nMinUdpPort = atoi(value);

			pEntry(MM_PROP_RTPTimeOut, value, "60000");
			m_initParam.nRTPTimeOut = atoi(value) / 1000;

			pEntry(MM_PROP_BufferTime, value, "3");
			m_initParam.nBufferTime = atoi(value);

			pEntry(MM_PROP_RTCPReportInterval, value, "5000");
			m_initParam.nRTCPInterval = atoi(value);

			pEntry(MM_PROP_RTSPTimeOut, value, "60000");
			m_initParam.nRTSPTimeout = atoi(value) / 1000;

			pEntry(MM_PROP_Bandwidth, value, "0");
			m_initParam.nBandWidth = atoi(value);

			pEntry(MM_PROP_WAPProfile, m_initParam.szProfile, "");
			pEntry(MM_PROP_RTSPUserAgent, m_initParam.szUserAgent, "VisualOn Streaming Player 2.2");
			pEntry(MM_PROP_RTSPProxyIP, m_initParam.szProxyIP, "");
			if(strlen(m_initParam.szProxyIP) > 0)
			{
				m_initParam.bUseRTSPProxy = 1;

				pEntry(MM_PROP_RTSPProxyPort, value, "");
				if(strlen(value) > 0)
				{
					strcat(m_initParam.szProxyIP, ":");
					strcat(m_initParam.szProxyIP, value);
				}
			}
			
			memset(g_szExtraFields, 0, sizeof(g_szExtraFields));
			pEntry(MM_PROP_RTSPEXTPARAM1, value, "");
			if(strlen(value) > 0)
			{
				strcpy(g_szExtraFields, value);
				strcat(g_szExtraFields, "\r\n");

				pEntry(MM_PROP_RTSPEXTPARAM2, value, "");
				if(strlen(value) > 0)
				{
					strcat(g_szExtraFields, value);
					strcat(g_szExtraFields, "\r\n");

					pEntry(MM_PROP_RTSPEXTPARAM3, value, "");
					if(strlen(value) > 0)
					{
						strcat(g_szExtraFields, value);
						strcat(g_szExtraFields, "\r\n");

						pEntry(MM_PROP_RTSPEXTPARAM4, value, "");
						if(strlen(value) > 0)
						{
							strcat(g_szExtraFields, value);
							strcat(g_szExtraFields, "\r\n");

							pEntry(MM_PROP_RTSPEXTPARAM5, value, "");
							if(strlen(value) > 0)
							{
								strcat(g_szExtraFields, value);
								strcat(g_szExtraFields, "\r\n");
							}
						}
					}
				}
			}

			if(strlen(g_szExtraFields) > 0)
			{
				m_initParam.bUseExtraFields = 1;
				m_initParam.szExtraFields = g_szExtraFields;
			}

			bParamInitialized = VO_TRUE;
		}
	}
	dllLoad.FreeLib ();
#endif	//_HTC

	if(bParamInitialized == VO_FALSE)
	{
		m_initParam.nMaxUdpPort		= DEFAULT_PORT_MAX;
		m_initParam.nMinUdpPort		= DEFAULT_PORT_MIN;
		m_initParam.nRTPTimeOut		= 60;//DEFAULT_RTPTIMEOUT;
		m_initParam.nBufferTime		= 3;//DEFAULT_BUFFERTIME;
		m_initParam.nRTCPInterval	= 5000;
		m_initParam.nRTSPTimeout	= 60; // DEFAULT_RTPTIMEOUT;
		strcpy(m_initParam.szUserAgent,"VisualOn Streaming Player 2.2");		
	}

	m_initParam.nFirstIFrame	= 1;
	m_initParam.nExtraRTPPacket	= 1;
	m_initParam.nConnectTimeout	= DEFAULT_RTPTIMEOUT;
	m_initParam.nSDKlogflag		= 0;//for test

	m_initParam.digestAuth = new DigestAuthParam;
	memset(m_initParam.digestAuth, 0, sizeof(DigestAuthParam));

	// keep the older version can work
	m_paramOpen.pDrmCB = (VO_SOURCEDRM_CALLBACK *)&m_initParam;
	m_paramOpen.pLibOP = NULL;//(VO_LIB_OPERATOR *)&m_initParam;
#ifdef _MAC_OS
	m_paramOpen.nReserve = (unsigned long)&m_initParam;
#else
	m_paramOpen.nReserve = (VO_U32)&m_initParam;
#endif

#if defined(_ACER)
		char nProLink[1024];
		char nName[1024];
		property_get("ro.product.modelalias", nName, "S100");
		sprintf(nProLink, "http://support.acer.com/UAprofile/Acer_%s_Profile.xml", nName);
		strcpy(m_initParam.szProfile,  nProLink);
		
		char nTemp[124];
		char* pNames[] = 
		{
			"ro.product.brand"  ,
			"ro.product.device" ,
			"ro.product.name"
		} ;
		
		int nLength = 0 ;
		for( int i = 0; i < sizeof(pNames) /sizeof(pNames[0]); i++)
		{
			strcat(m_initParam.szUserAgent, "/");
			memset(nTemp, 0, sizeof(nTemp));
			property_get(pNames[i], nTemp, "");
			nLength = strlen(nTemp) ;

			if( nLength > 0 && (strlen(m_initParam.szUserAgent) + nLength ) <= 256 )
				strcat(m_initParam.szUserAgent, nTemp);
		
		}
		VOLOGI("nProLink = %s\n", nProLink);
		VOLOGI("m_initParam.szUserAgent = %s\n", m_initParam.szUserAgent);
#endif

	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_RTSPCALLBACK | VO_SOURCE_OPENPARAM_FLAG_OPENRTSP;
	m_nSourceType = VO_SOURCE_TYPE_RTSP;

	if(VO_FILE_FFSTREAMING_RTSP == nFormat || VO_FILE_FFAPPLICATION_SDP == nFormat)
	{
		g_voRTSPSourceStatus++;
	}
}

CRTSPSource::~CRTSPSource ()
{
	ReleaseFrameBuffer ();

	if(m_initParam.digestAuth)
	{
		delete m_initParam.digestAuth;
	}

	if(VO_SOURCE_TYPE_RTSP == m_nSourceType)
	{
		g_voRTSPSourceStatus--;
	}
	m_nStatus = RTSP_STATUS_NULL;
}

void CRTSPSource::ReleaseFrameBuffer (void)
{
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
	if (g_voRTSPSourceStatus > 1)
	{
		VOLOGE ("The RTSP Source didn't close correctly!");
		return VO_ERR_WRONG_STATUS;
	}
	CloseSource ();
	m_bHadOpenned = VO_FALSE;
	m_bForceClosed = VO_FALSE;

	voCAutoLock lock (&m_csRTSPStatus);

	m_nError = 0;
	m_bSendError = VO_FALSE;

	if (LoadLib (m_hInst) == 0)
		return VO_ERR_FAILED;

	m_initParam.nSDKlogflag = m_nDumpLogLevel;

	StreamingNotifyEventFunc funEvent;
	funEvent.funtcion = voRTSPStreamCallBack;
	funEvent.userdata = this;

#ifdef _WIN32
	memset (m_szURL, 0, sizeof (m_szURL));
	WideCharToMultiByte (CP_ACP, 0, (VO_PTCHAR)pSource, -1, m_szURL, 2048, NULL, NULL);
#elif defined LINUX
	strcpy (m_szURL, (VO_PCHAR)pSource);
#endif // _WIN32

	m_paramOpen.pLibOP = m_pLibOP;
	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_RTSPCALLBACK;
	m_paramOpen.pSource = m_szURL;
	m_paramOpen.pSourceOP = &funEvent;
	VO_U32 nRC = m_funFileRead.Open (&m_hFile, &m_paramOpen);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_funFileRead.Open failed. 0X%08X", (unsigned int)nRC);
		return nRC;
	}

//  	VO_U32 nTimeMarker = 10;	// default 10s, so we need not set.
//  	nRC = SetSourceParam(VOID_STREAMING_TIMEMARKER, &nTimeMarker);

	if(m_nInitPlayPos > 0)
		nRC = SetSourceParam(VOID_STREAMING_INITPLAY_POS, &m_nInitPlayPos);

	nRC = SetSourceParam(VOID_STREAMING_INIT_PARAM, &m_initParam);
	if(nRC != VO_ERR_NONE)
	{
		VOLOGE("Set VOID_STREAMING_INIT_PARAM failed; 0X%X", (unsigned int)nRC);
	}
	
	VOLOGI("RTSP socket connection type: %d", m_initParam.nUseTCP);

	nRC = SetSourceParam (VOID_STREAMING_OPENURL, 0);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("VOID_STREAMING_OPENURL failed. 0X%08X", (unsigned int)nRC);

		if(E_UNSUPPORTED_FORMAT == (VO_S32)m_nError || E_UNSUPPORTED_MEDIA_TYPE == (VO_S32)m_nError)
			return VO_ERR_SOURCE_FORMATUNSUPPORT;
		else if(E_UNSUPPORTED_CODEC == (VO_S32)m_nError)
			return VO_ERR_SOURCE_CODECUNSUPPORT;
		else if(E_PROXY_NEEDED == (VO_S32)m_nError || E_PROXY_AUTH_REQUIRED == (VO_S32)m_nError)
			return VO_ERR_SOURCE_NEEDPROXY;
		else
			return nRC;
	}
	voCAutoLock lockClose (&m_csForceClose);
	if (m_bForceClosed)
		return VO_ERR_WRONG_STATUS;

	nRC = m_funFileRead.GetSourceInfo (m_hFile, &m_filInfo);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_funFileRead.GetSourceInfo failed. 0X%08X", (unsigned int)nRC);
		return nRC;
	}

	m_nDuration = m_filInfo.Duration;

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;
	for (i = 0; i < m_filInfo.Tracks; i++)
	{
		nRC = m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
		if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO) && m_nVideoTrack == -1)
			m_nVideoTrack = i;
		else if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1)
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

	AllocOutputConfigDatas();

	m_bHadOpenned = VO_TRUE;
	m_bBufferEndMsg = VO_FALSE;

	nRC = CheckCodecLegal();
	if(VO_ERR_NONE != nRC)
		return nRC;
#ifdef _LG
	VO_U32	nParam = 0;
	m_nStatus = RTSP_STATUS_PLAY;
	SetSourceParam (VO_RTSP_PLAY, &nParam);	
#endif
	return VO_ERR_NONE;
}

VO_U32 CRTSPSource::CloseSource (void)
{
	if (m_bForceClosed)
		return 0;

	voCAutoLock lockClose (&m_csForceClose);
	m_bForceClosed = VO_TRUE;
	if (m_hFile != NULL)
	{
		m_funFileRead.Close (m_hFile);
		m_hFile = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 CRTSPSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	if (m_hFile == 0)
		return VO_ERR_WRONG_STATUS;

	if (nTrack < 0 || nTrack >= m_nTracks)
		return VO_ERR_WRONG_STATUS;

	if (m_bBuffering)
		return VO_ERR_SOURCE_NEEDRETRY;

	voCAutoLock lock (&m_csRead);

	VO_U32 nRC = 0;

	VO_SOURCE_SAMPLE * pSample = (VO_SOURCE_SAMPLE *)pTrackData;
	pSample->Buffer = m_ppBuffer[nTrack];
	pSample->Size = VORTSP_MAX_FRAME_SIZE;

	if (IsOutputConfigData(nTrack))
	{
		SetOutputConfigData(nTrack, VO_FALSE);

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
				if (trkInfo.Codec == VO_VIDEO_CodingWMV  || trkInfo.Codec == VO_VIDEO_CodingVC1)
				{
					VO_U32 nFourCC = 0;
					nRC = GetTrackParam (nTrack, VO_PID_SOURCE_CODECCC, &nFourCC);
					if(memcmp((char*)&nFourCC, (char*)"WVC1", 4) != 0 || m_nWVC1Decoder != 1)	// if WVC1 and QCM hardware decoder, use sequence data directly, East 20110408
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
			}

			if (trkInfo.HeadSize > 0)
			{
				pSample->Buffer = trkInfo.HeadData;
				pSample->Size = 0X80000000 + trkInfo.HeadSize;
				pSample->Time = 0;
				pSample->Flag = VO_SOURCE_SAMPLE_FLAG_CODECCONFIG;

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
				VOLOGI ("Source send VO_STREAM_BUFFERSTOP");
				long nValue = 100;
				m_pCallBack (VO_STREAM_BUFFERSTOP, &nValue, (long *)m_pUserData);
				voOS_Sleep (100);
			}
		}

		if ((VO_S32)nTrack == m_nVideoTrack)
		{
			if (m_nVideoSendSamples == 0 && (pSample->Size & 0X80000000) == 0)
			{
				VOLOGW ("first sample not I frame, drop it...");
				pSample->Size = 0;
				return VO_ERR_SOURCE_NEEDRETRY;
			}

			m_nVideoSendSamples++;
		}
		else if ((VO_S32)nTrack == m_nAudioTrack)
			m_nAudioSendSamples++;
	}

	return nRC;
}

VO_BOOL CRTSPSource::CanSeek (void)
{
	if (m_bBuffering)
		return VO_FALSE;

	return VO_TRUE;
}

VO_U32 CRTSPSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	if (m_nVideoTrack >= 0 && (VO_S32)nTrack != m_nVideoTrack)
		return VO_ERR_NONE;

	if (m_bBuffering)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = CFileSource::SetTrackPos (nTrack, pPos);

	m_bBuffering = VO_TRUE;

	return nRC;
}

VO_U32 CRTSPSource::Start(void)
{
	voCAutoLock lock (&m_csRead);

	if (m_nStatus == RTSP_STATUS_PLAY)
		return VO_ERR_NONE;

	m_nStatus = RTSP_STATUS_PLAY;
	VO_U32	nParam = 0;
	return 	SetSourceParam (VO_RTSP_PLAY, &nParam);
}

VO_U32 CRTSPSource::Pause(void)
{
	voCAutoLock lock (&m_csRead);

	m_nStatus = RTSP_STATUS_PAUSE;
	VO_U32	nParam = 0;
	return 	SetSourceParam (VO_RTSP_PAUSE, &nParam);
}

VO_U32 CRTSPSource::Stop(void)
{
	voCAutoLock lock (&m_csRead);

	m_nVideoSendSamples = 0;
	m_nStatus = RTSP_STATUS_STOP;
	VO_U32	nParam = 0;
	return 	SetSourceParam (VO_RTSP_STOP, &nParam);
}

VO_U32 CRTSPSource::SetCallBack (VO_PTR pCallBack, VO_PTR pUserData)
{
	m_pCallBack = (STREAMING_CALLBACK_NOTIFY) pCallBack;
	m_pUserData = pUserData;

	return 0;
}

int CRTSPSource::HandleStreamEvent (long EventCode, long * EventParam1)
{
	if (m_bForceClosed)
		return 0;

	m_lEventCode = EventCode;

	if (m_lEventCode == VO_EVENT_BUFFERING_BEGIN)
	{
		m_bBuffering = VO_TRUE;

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
		if(!m_bBufferEndMsg)
			NotifyBitrateThroughput();

		m_bBuffering = VO_FALSE;
		m_bBufferEndMsg = VO_TRUE;
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_BUFFERSTOP, EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_EVENT_PACKET_LOST)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_PACKETLOST, EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_EVENT_TIMEMARKER)
	{
		if(!m_bBufferEndMsg)
			NotifyBitrateThroughput();
	}
	else if (m_lEventCode == VO_EVENT_RTCP_RR)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_RTCP_RR, EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode >= VO_RTSP_EVENT_SESSION_START && m_lEventCode <= VO_RTSP_EVENT_SESSION_TEARDOWN)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (m_lEventCode - VO_STREAM_VALUEOFFSET, EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode >= VO_EVENT_CONNECT_FAIL && m_lEventCode <= VO_EVENT_SOCKET_ERR)
	{
		VOLOGW ("Error Event %d, Send Error %d", (int)m_lEventCode, (int)m_bSendError);
		m_nError = 1;
		if(m_lEventCode == VO_EVENT_DESCRIBE_FAIL)
		{
			m_nError = *EventParam1;
			if(m_pCallBack)
				m_pCallBack (VO_STREAM_DESCRIBE_FAIL, EventParam1, (long *)m_pUserData);
		}
			
		if(m_lEventCode == VO_EVENT_OPTION_FAIL)
			m_nError = *EventParam1;

		if (m_pCallBack != NULL && !m_bSendError)
		{
			if(m_bHadOpenned == VO_TRUE)
				m_pCallBack (VO_STREAM_RUNTIMEERROR, &m_lEventCode, (long *)m_pUserData);
			else
				m_pCallBack (VO_STREAM_ERROR, &m_lEventCode, (long *)m_pUserData);

			m_bSendError = VO_TRUE;
		}
	}

	return 1;
}

int CRTSPSource::voRTSPStreamCallBack (long EventCode, long * EventParam1, long * userData)
{
	CRTSPSource * pSource = (CRTSPSource *)userData;

	return pSource->HandleStreamEvent (EventCode, EventParam1);
}

VO_U32 CRTSPSource::SetInitPlayPos(VO_U32 nInitPlayPos)
{
	m_nInitPlayPos = nInitPlayPos;

	return VO_ERR_NONE;
}

void CRTSPSource::NotifyBitrateThroughput()
{
	if(m_pCallBack)
	{
		VOStreamBitrateThroughput sStreamBitrateThroughput;
		sStreamBitrateThroughput.nBitrate = 0x7FFFFFFF;
		sStreamBitrateThroughput.nThroughput = 0;

		VO_U32 nRC = 0;

		VOStreamingInfo sStreamingInfo;
		nRC = GetSourceParam(VOID_STREAMING_INFO, &sStreamingInfo);
		if(nRC == VO_ERR_NONE)
			sStreamBitrateThroughput.nBitrate = sStreamingInfo.clip_bitrate;

		nRC = GetSourceParam(VOID_STREAMING_THROUGHPUT, &sStreamBitrateThroughput.nThroughput);

		m_pCallBack(VO_STREAM_BITRATETHROUGHPUT, (long*)&sStreamBitrateThroughput, (long*)m_pUserData);
	}
}

VO_U32 CRTSPSource::SetSourceParam (VO_U32 nID, VO_PTR pValue)
{
	if(nID == VOMP_PID_SOCKET_CONNECTION_TYPE)
	{
		int nVal = *((int*)pValue);
		m_initParam.nUseTCP = nVal;
		//CFileSource::SetSourceParam(VOID_STREAMING_INIT_PARAM, &m_initParam);
		return VO_ERR_NONE;
	}
	else if(nID == VOMP_PID_MIN_BUFFTIME)
	{
		int nVal = *((int*)pValue);
		m_initParam.nBufferTime = nVal/1000;
		VOLOGI("Set RTSP min buffer time %d", m_initParam.nBufferTime);
		return VO_ERR_NONE;
	}
	else if(nID == VOID_STREAMING_DIGEST_AUTHEN)
	{
		PDigestAuthParam pDigestAuth = (PDigestAuthParam)pValue;
		
		if(m_initParam.digestAuth)
		{
			m_initParam.digestAuth->user = pDigestAuth->user;
			m_initParam.digestAuth->userLen = pDigestAuth->userLen;
			m_initParam.digestAuth->passwd = pDigestAuth->passwd;
			m_initParam.digestAuth->passwdLen = pDigestAuth->passwdLen;
		}

		return VO_ERR_NONE; 
	}

	
	return CFileSource::SetSourceParam(nID, pValue);
}
