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
#include "voOSFunc.h"
#include "CHTTPPDSource.h"

#define LOG_TAG "CHTTPPDSource"
#include "voLog.h"

CHTTPPDSource::CHTTPPDSource(VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB)
	: CRTSPSource (hInst, nFormat, pMemOP, pFileOP, pDrmCB)
{
	memset(&m_initParam,0,sizeof(m_initParam));
	m_initParam.nBufferTime = 3;			//<S>
	m_initParam.nPlayBufferTime = 10;		//<S>
	m_initParam.nHTTPDataTimeOut = 30000;	//<MS>
#ifdef _WIN32_WCE
	m_initParam.nMaxBuffer = 8192;			//<KB>, WinCE can not allocate so much memory!!
#else	//_WIN32_WCE
	m_initParam.nMaxBuffer = 16400;			//<KB>
#endif	//_WIN32_WCE
//	m_initParam.nMaxBuffer = 1024;			//<KB>
	m_initParam.nTempFileLocation = 0;		// 0: memory, 1: storage, 2: ext.storage		default 1
	m_initParam.nPacketLength = 5;			//<KB>
	m_initParam.nHttpProtocol = 1;
	m_initParam.nSeekUnsupported = 0;
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

			pEntry(MM_PROP_HTTPNoByteSeek, value, "0");
			m_initParam.nSeekUnsupported = atoi(value);
		}
	}
	dllLoad.FreeLib ();
#endif	//_HTC

	m_initParam.nSDKlogflag   = 0;
	strcpy((char*)m_initParam.mUserAgent, "VisualOn-PD Player");

	m_paramOpen.pLibOP = NULL;
#ifdef _MAC_OS
	m_paramOpen.nReserve = (unsigned long)&m_initParam;
#else
	m_paramOpen.nReserve = (VO_U32)&m_initParam;
#endif
	
	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_RTSPCALLBACK | VO_SOURCE_OPENPARAM_FLAG_OPENPD;
	m_paramOpen.pDrmCB = pDrmCB;

	m_nSourceType = VO_SOURCE_TYPE_HTTP;
}

CHTTPPDSource::~CHTTPPDSource ()
{
	CloseSource ();
}

#ifdef MS_HTTP
VO_U32	CHTTPPDSource::CheckHTTPType(VO_LIVESRC_FORMATTYPE *type)
{
	CDllLoad dlEngine;
	vostrcpy( dlEngine.m_szDllFile, _T("voPreHTTP.so") );
	vostrcpy( dlEngine.m_szAPIName, _T("voGetPreHTTPAPI") );

	dlEngine.SetLibOperator(m_pLibOP);
	dlEngine.SetWorkPath(m_pWorkPath);

	if ( dlEngine.LoadLib(NULL) == 0 ) {
		VOLOGI("!LoadLib");
		return VO_ERR_BASE;
	}

	VO_PREHTTP_API apiPreHttp = {0};
	void * handlePreHttp = NULL;

	pvoGetPreHTTPAPI getapi = (pvoGetPreHTTPAPI)dlEngine.m_pAPIEntry;
	if (getapi)
	{
		getapi( &apiPreHttp , 0 );
	}
	else
	{
		VOLOGI("!getapi");
		return VO_ERR_BASE;
	}

	if ( VO_ERR_NONE != apiPreHttp.voPreHTTPInit(&handlePreHttp) ) {
		VOLOGI("!voPreHTTPInit");
		return VO_ERR_BASE;
	}

	int nTryTime = 0;

	do {
		*type = VO_LIVESRC_FT_MAX;

		if ( VO_ERR_NONE != apiPreHttp.voPreHTTPSetSrcURL(handlePreHttp, (VO_CHAR *)m_szHTTPURL) ) {
			VOLOGI("!voPreHTTPSetSrcURL");
			return VO_ERR_BASE;
		}

		VO_CHAR *pURL = NULL;
		int iRet = 0;
		int mTryTimes = 0;

		do {
			iRet = apiPreHttp.voPreHTTPGetDesInfo(handlePreHttp, type, &pURL);

			mTryTimes++;
			if(mTryTimes > 1000)
				break;			
		} while (iRet == 0xfffffffe);

		nTryTime++;
		if(nTryTime > 1000)
			break;

	} while (*type == VO_LIVESRC_FT_FILE);

	apiPreHttp.voPreHTTPUninit(handlePreHttp);

	return VO_ERR_NONE;
}
#endif

VO_U32 CHTTPPDSource::LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	voCAutoLock lock (&m_csHTTPStatus);

	m_bForceClosed = VO_FALSE;
	vostrcpy (m_szHTTPURL, (VO_TCHAR *)pSource);
	m_bHadOpenned = VO_FALSE;

#ifdef MS_HTTP	
	VO_LIVESRC_FORMATTYPE LiveType = VO_LIVESRC_FT_MAX;
	CheckHTTPType(&LiveType);

	if(LiveType == VO_LIVESRC_FT_PD || LiveType == VO_LIVESRC_FT_WMS)
		m_LiveSrcType = LiveType;
#endif



	return LoadHTTPSource ();
}

int CHTTPPDSource::LoadHTTPSource (void)
{
	if (m_hFile != NULL)
		CloseSource ();

	m_bForceClosed = VO_FALSE;

	m_nError = 0;
	m_bSendError = VO_FALSE;

	if (LoadLib (m_hInst) == 0)
		return VO_ERR_FAILED;

	CheckMotoProxySetting ();
	m_initParam.nSDKlogflag   = m_nDumpLogLevel;

	StreamingNotifyEventFunc funEvent;
	funEvent.funtcion = voRTSPStreamCallBack;
	funEvent.userdata = this;

#ifdef _WIN32
	vostrcpy(m_initParam.mFilePath, _T("C:\\vohttpPDSource.3gp"));
#else//_WIN32
	vostrcpy(m_initParam.mFilePath, _T("/sdcard/vohttppdtmp.3gp"));
#endif//_WIN32

	m_paramOpen.pLibOP = m_pLibOP;
	m_paramOpen.pSource = m_szHTTPURL;
	m_paramOpen.pSourceOP = &funEvent;

	VO_U32 nRC = m_funFileRead.Open (&m_hFile, &m_paramOpen);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_funFileRead.Open failed. 0X%08X", (unsigned int)nRC);
		return nRC;
	}

	nRC = SetSourceParam (VOID_STREAMING_OPENURL, 0);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGW ("VOID_STREAMING_OPENURL failed. 0X%08X", (unsigned int)nRC);

		if(E_PROXY_NEEDED == (VO_S32)m_nError || E_PROXY_AUTH_REQUIRED == (VO_S32)m_nError)
			return VO_ERR_SOURCE_NEEDPROXY;

		return nRC;
	}

	voCAutoLock lockClose (&m_csForceClose);
	if (m_bForceClosed)
		return VO_ERR_WRONG_STATUS;

	VO_S32 nParam = 1;
	nRC = SetSourceParam (VOID_STREAMING_AUTO_EXTEND_BUFTIME, &nParam);

	nRC = m_funFileRead.GetSourceInfo (m_hFile, &m_filInfo);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("m_funFileRead.GetSourceInfo failed. 0X%08X", (unsigned int)nRC);
		return nRC;
	}
	m_nDuration = m_filInfo.Duration;
	m_bHadOpenned = VO_TRUE;

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;
	for (i = 0; i < m_filInfo.Tracks; i++)
	{
		nRC = m_funFileRead.GetTrackInfo (m_hFile, i, &trkInfo);
		if (trkInfo.Type == VO_SOURCE_TT_VIDEO && m_nVideoTrack == -1)
		{
			m_nVideoTrack = i;

			if(VO_VIDEO_CodingH264 == trkInfo.Codec && m_bAVC2NAL)
			{
				VO_U32 nFourCC = 0;
				nRC = GetTrackParam (i, VO_PID_SOURCE_CODECCC, &nFourCC);
				if (nRC == VO_ERR_NONE && memcmp((char*)&nFourCC, (char*)"AVC1", 4) == 0)
				{
					m_bIsAVC = VO_TRUE;
					nRC = GetTrackParam (i, VO_PID_SOURCE_MAXSAMPLESIZE, &m_nFrameSize);
				}
			}
		}
		else if (trkInfo.Type == VO_SOURCE_TT_AUDIO && m_nAudioTrack == -1)
			m_nAudioTrack = i;
	}

	AllocOutputConfigDatas();
	
	m_bBufferEndMsg = VO_FALSE;

	nRC = CheckCodecLegal();
	if(VO_ERR_NONE != nRC)
		return nRC;

	return VO_ERR_NONE;
}

VO_U32 CHTTPPDSource::CloseSource (void)
{
	if (m_bForceClosed)
		return 0;

	voCAutoLock lockClose (&m_csForceClose);
	m_bForceClosed = VO_TRUE;
	if (m_hFile != NULL)
	{
		m_funFileRead.Close (m_hFile);
		m_hFile = NULL;
		m_bHadOpenned = VO_FALSE;
	}

	return VO_ERR_NONE;
}

VO_U32 CHTTPPDSource::Start(void)
{
	VO_QUICKRESPONSE_COMMAND cmd = VO_QUICKRESPONSE_RUN;
	SetSourceParam(VOID_STREAMING_QUICKRESPONSE_COMMAND, &cmd);

	voCAutoLock lock (&m_csHTTPStatus);

	if (m_bHadOpenned && m_nError > 0)
		return VO_ERR_WRONG_STATUS;

	if (m_hFile != NULL && m_bHadOpenned == VO_FALSE)
		return VO_ERR_WRONG_STATUS;

	if (m_bForceClosed)
		return VO_ERR_WRONG_STATUS;

	if (m_hFile == NULL && m_bHadOpenned == VO_FALSE)
	{
		if (LoadHTTPSource () != VO_ERR_NONE)
			return VO_ERR_WRONG_STATUS;
	}

	return VO_ERR_NONE;
}

VO_U32 CHTTPPDSource::Pause(void)
{
	VO_QUICKRESPONSE_COMMAND cmd = VO_QUICKRESPONSE_PAUSE;
	SetSourceParam(VOID_STREAMING_QUICKRESPONSE_COMMAND, &cmd);

	voCAutoLock lock (&m_csHTTPStatus);

	if (m_bHadOpenned && m_nError > 0)
		return VO_ERR_WRONG_STATUS;

	if (m_hFile != NULL && m_bHadOpenned == VO_FALSE)
		return VO_ERR_WRONG_STATUS;

	if (m_bForceClosed)
		return VO_ERR_WRONG_STATUS;

	if (m_hFile == NULL && m_bHadOpenned == VO_FALSE)
	{
		if (LoadHTTPSource () != VO_ERR_NONE)
			return VO_ERR_WRONG_STATUS;
	}

	return VO_ERR_NONE;
}

VO_U32 CHTTPPDSource::Stop(void)
{
	VO_QUICKRESPONSE_COMMAND cmd = VO_QUICKRESPONSE_STOP;
	SetSourceParam(VOID_STREAMING_QUICKRESPONSE_COMMAND, &cmd);

	voCAutoLock lock (&m_csHTTPStatus);

	CloseSource ();
	FreeLib ();

	m_bForceClosed = VO_FALSE;

	return VO_ERR_NONE;
}

VO_U32 CHTTPPDSource::GetTrackData (VO_U32 nTrack, VO_PTR pTrackData)
{
	voCAutoLock lock (&m_csHTTPStatus);

	if (m_bHadOpenned && m_nError > 0)
		return VO_ERR_WRONG_STATUS;

	if (m_hFile != NULL && m_bHadOpenned == VO_FALSE)
		return VO_ERR_WRONG_STATUS;

	if (m_bForceClosed)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = 0;
// 	VO_U32 nBuffering = 0;
// 	nRC = GetSourceParam (VOID_STREAMING_BUFFERING_PROGRESS, &nBuffering);
// 	if (m_bBuffering && nBuffering <= 20)
	if (m_bBuffering)
		return VO_ERR_SOURCE_NEEDRETRY;

	nRC = CFileSource::GetTrackData (nTrack, pTrackData);

	// make sure it is actual media data form GetSample, East 20111110
	if (!m_bBufferEndMsg && nRC == VO_ERR_NONE && !(((VO_SOURCE_SAMPLE *)pTrackData)->Flag & VO_SOURCE_SAMPLE_FLAG_CODECCONFIG))
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

	return nRC;
}

VO_U32 CHTTPPDSource::SetTrackPos (VO_U32 nTrack, VO_S64 * pPos)
{
	VO_QUICKRESPONSE_COMMAND cmd = VO_QUICKRESPONSE_FLUSH;
	SetSourceParam(VOID_STREAMING_QUICKRESPONSE_COMMAND, &cmd);

	voCAutoLock lock (&m_csHTTPStatus);

	if (m_bHadOpenned && m_nError > 0)
		return VO_ERR_WRONG_STATUS;

	if (m_hFile != NULL && m_bHadOpenned == VO_FALSE)
		return VO_ERR_WRONG_STATUS;

	if (m_bForceClosed)
		return VO_ERR_WRONG_STATUS;

	if (m_hFile == NULL && m_bHadOpenned == VO_FALSE)
	{
		if (LoadHTTPSource () != VO_ERR_NONE)
			return VO_ERR_WRONG_STATUS;
	}

	return CFileSource::SetTrackPos (nTrack, pPos);
}

int CHTTPPDSource::HandleStreamEvent (long EventCode, long * EventParam1)
{
	if (m_bForceClosed)
		return 0;

	m_lEventCode = EventCode;

	if (m_lEventCode == VO_EVENT_BUFFERING_BEGIN)
	{
		m_bBuffering = VO_TRUE;
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_BUFFERSTART, (long *)EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_EVENT_BUFFERING_PERCENT)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_BUFFERSTATUS, (long *)EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_EVENT_BUFFERING_END)
	{
		m_bBuffering = VO_FALSE;
		m_bBufferEndMsg = VO_TRUE;
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_BUFFERSTOP, (long *)EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_PD_EVENT_DOWNLOADPOS)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_DOWNLODPOS, (long *)EventParam1, (long *)m_pUserData);
	}
	else if (m_lEventCode == VO_PD_EVENT_SEEKFINALPOS)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (VO_STREAM_SEEKFINALPOS, (long *)EventParam1, (long *)m_pUserData);
	}
	else if ((m_lEventCode >= VO_EVENT_CONNECT_FAIL && m_lEventCode <= VO_EVENT_SOCKET_ERR) ||
			m_lEventCode == VO_EVENT_DISABLE_SCAN ||
			(m_lEventCode >= VO_PD_EVENT_INSUFFICIENT_SPACE && m_lEventCode <= VO_PD_EVENT_GET_ASX))

	{
		VOLOGW ("Error Event %d, Send Error %d", (int)m_lEventCode, (int)m_bSendError);
		m_nError = 1;
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

void CHTTPPDSource::CheckMotoProxySetting (void)
{
	VO_TCHAR szFile[256];
#ifdef _WIN32
	vostrcpy (szFile, _T("e:/proxy_setting.txt"));
#else
	vostrcpy (szFile, _T("/data/wpa/proxy_setting"));
#endif // _WIN32

	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.pSource = (VO_PTR) szFile;
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_READ_ONLY;

	VO_PTR hFile = cmnFileOpen (&filSource);
	if (hFile == NULL)
	{
		VOLOGI ("It could not open the file %s!", szFile);
		return;
	}

	VO_U32 nFileSize = (VO_U32)cmnFileSize (hFile);
	VO_TCHAR * pFileData = new VO_TCHAR[(int)nFileSize];

	cmnFileRead (hFile, pFileData, (VO_U32)nFileSize);

	cmnFileClose (hFile);

	vostrcpy (m_initParam.mProxyName, _T(""));

	int nPos = 0;
	int nTextPos = 0;
	if (vostrstr (pFileData, _T("http.proxyEnable=on")) != NULL)
	{
		VO_TCHAR szProxy[256];
		VO_TCHAR * pHostName = vostrstr (pFileData, _T("http.proxyHost="));
		if (pHostName != NULL)
		{
			nPos = 0;
			pHostName = pHostName + vostrlen (_T("http.proxyHost="));
			nTextPos = pHostName - pFileData;
			while (pHostName[nPos] != _T('\n') && ((nTextPos + nPos) * sizeof (TCHAR) < nFileSize))
				nPos++;
			pHostName[nPos] = 0;
			vostrcpy (szProxy, pHostName);

			VOLOGI ("Proxy Name: %s", pHostName);

			VO_TCHAR * pHostPort = vostrstr (pFileData, _T("http.proxyPort="));
			if (pHostPort != NULL)
			{
				nPos = 0;
				pHostPort = pHostPort + vostrlen (_T("http.proxyPort="));
				nTextPos = pHostPort - pFileData;
				while (pHostPort[nPos] != _T('\n') && ((nTextPos + nPos) * sizeof (TCHAR) < nFileSize))
					nPos++;
				pHostPort[nPos] = 0;
				vostrcat (szProxy, _T(":"));
				vostrcat (szProxy, pHostPort);

				VOLOGI ("Proxy Port: %s", pHostPort);

				vostrcpy (m_initParam.mProxyName, szProxy);

				VOLOGI ("Proxy is: %s", m_initParam.mProxyName);
			}
			else
			{
				VOLOGI ("It could not find the string: http.proxyPort");
			}
		}
		else
		{
			VOLOGI ("It could not find the string: http.proxyHost");
		}
	}
	else
	{
		VOLOGI ("It could not find the string: http.proxyEnable=on");
	}

	delete []pFileData;
}
