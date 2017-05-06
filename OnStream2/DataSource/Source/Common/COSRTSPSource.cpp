#include "COSRTSPSource.h"

#define  LOG_TAG    "COSRTSPSource"
#include "voLog.h"

COSRTSPSource::COSRTSPSource(VO_SOURCE2_LIB_FUNC *pLibop)
	: COSBaseSource(pLibop)	
{	
	memset(&m_sInitParam, 0, sizeof(m_sInitParam));
	m_sInitParam.uBufferTime = 4000;
	LoadDll();
}

COSRTSPSource::~COSRTSPSource ()
{
	unLoadDll();
}

int	COSRTSPSource::Init(void * pSource, int nFlag, void* pInitparam, int nInitFlag)
{
	int nRC = COSBaseSource::Init(pSource, nFlag, pInitparam, nInitFlag);
	if(nRC) return nRC;

	//nRC = COSBaseSource::SetParam(VO_PID_SOURCE2_RTSPINITPARAM, &m_sInitParam);
	return nRC;
}

int	COSRTSPSource::Uninit()
{
	int nRC = COSBaseSource::Uninit();
	return nRC;
}


int COSRTSPSource::GetParam (int nID, void * pValue)
{
	int nRC = 0;
	if(nID == VOOSMP_SRC_PID_RTSP_STATS){
		nRC = COSBaseSource::GetParam(VO_PID_SOURCE2_RTSP_STATS, pValue);

		return nRC;
	}
	
	nRC = COSBaseSource::GetParam(nID, pValue);
	return nRC;
}

int COSRTSPSource::SetParam (int nID, void * pValue)
{
	if(nID == VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE)
	{
		if(pValue)
			m_sInitParam.uRTPChan = *(int *)pValue;

		VOLOGI("VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE %d",m_sInitParam.uRTPChan);
		return COSBaseSource::SetParam(VO_PID_SOURCE2_RTSPINITPARAM, &m_sInitParam);
	}
	else if(nID == VOOSMP_PID_MIN_BUFFTIME)
	{
		if(pValue)
			m_sInitParam.uBufferTime = *(int *)pValue;

		VOLOGI("VOOSMP_PID_MIN_BUFFTIME %d",m_sInitParam.uBufferTime);
		return COSBaseSource::SetParam(VO_PID_SOURCE2_RTSPINITPARAM, &m_sInitParam);
	}
	else if(nID == VOOSMP_SRC_PID_RTSP_CONNECTION_PORT)
	{
		if(pValue)
		{
			VOOSMP_SRC_RTSP_CONNECTION_PORT * pRCP = (VOOSMP_SRC_RTSP_CONNECTION_PORT *)pValue;
			m_sInitParam.uAudioConnectionPort = pRCP->uAudioConnectionPort;
			m_sInitParam.uVideoConnectionPort = pRCP->uVideoConnectionPort;
		}

		VOLOGI("VOOSMP_SRC_PID_RTSP_CONNECTION_PORT AP %d VP %d",m_sInitParam.uAudioConnectionPort, m_sInitParam.uVideoConnectionPort);
		return COSBaseSource::SetParam(VO_PID_SOURCE2_RTSPINITPARAM, &m_sInitParam);
	}
	
	return COSBaseSource::SetParam(nID, pValue);
}

int COSRTSPSource::HandleEvent (int nID, int nParam1, int nParam2)
{
	int nRC = 0;	
	if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTSP_CONNECT_FAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_RTSP_Error;
		m_nParam1 = VOOSMP_SRC_RTSP_ERROR_CONNECT_FAIL;
		m_nParam2 = nParam1;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTSP_DESCRIBE_FAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_RTSP_Error;
		m_nParam1 = VOOSMP_SRC_RTSP_ERROR_DESCRIBE_FAIL;
		m_nParam2 = nParam1;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTSP_SETUP_FAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_RTSP_Error;
		m_nParam1 = VOOSMP_SRC_RTSP_ERROR_SETUP_FAIL;
		m_nParam2 = nParam1;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTSP_PLAY_FAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_RTSP_Error;
		m_nParam1 = VOOSMP_SRC_RTSP_ERROR_PLAY_FAIL;
		m_nParam2 = nParam1;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTSP_PAUSE_FAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_RTSP_Error;
		m_nParam1 = VOOSMP_SRC_RTSP_ERROR_PAUSE_FAIL;
		m_nParam2 = nParam1;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTSP_OPTION_FAIL)
	{
		m_nEventID = VOOSMP_SRC_CB_RTSP_Error;
		m_nParam1 = VOOSMP_SRC_RTSP_ERROR_OPTION_FAIL;
		m_nParam2 = nParam1;
	}
	else if((unsigned int)nID == VO_EVENTID_SOURCE2_ERR_RTP_SOCKET_ERROR)
	{
		m_nEventID = VOOSMP_SRC_CB_RTSP_Error;
		m_nParam1 = VOOSMP_SRC_RTSP_ERROR_SOCKET_ERROR;
		m_nParam2 = nParam1;
	}
	else
	{
		nRC = COSBaseSource::HandleEvent (nID, nParam1, nParam2);
		if(nRC) return nRC;
	}

	nRC = callBackEvent ();
	return nRC;
}

int COSRTSPSource::doReadAudio(VOOSMP_BUFFERTYPE * pBuffer)
{
	if(m_SourceAPI.GetSample == NULL  || m_hSource == NULL)
		return VOOSMP_ERR_Retry;

	voCAutoLock lockReadSrc (&m_MutexReadData);

	VO_SOURCE2_SAMPLE nSample;
	memset(&nSample, 0, sizeof(VO_SOURCE2_SAMPLE));
	nSample.uTime = pBuffer->llTime;

	int nRC = m_SourceAPI.GetSample(m_hSource, VO_SOURCE2_TT_RTSP_AUDIO, &nSample);

	if(nRC == VO_RET_SOURCE2_OK)
	{
		return	ConvertBufSrc2OS(&nSample, pBuffer, VOOSMP_SS_AUDIO);	
	}
	else if(nRC == (int)VO_RET_SOURCE2_NEEDRETRY)
	{
		return VOOSMP_ERR_Retry;
	}
	else if(nRC == (int)VO_RET_SOURCE2_END)
	{
		return VOOSMP_ERR_EOS;
	}
	else
	{
		return VOOSMP_ERR_Retry;
	}
}

int COSRTSPSource::doReadVideo(VOOSMP_BUFFERTYPE * pBuffer)
{
	if(m_SourceAPI.GetSample == NULL || m_hSource == NULL)
		return VOOSMP_ERR_Retry;

	voCAutoLock lockReadSrc (&m_MutexReadData);
	
	VO_SOURCE2_SAMPLE nSample;
	memset(&nSample, 0, sizeof(VO_SOURCE2_SAMPLE));
	nSample.uTime = pBuffer->llTime;
	nSample.uDuration = pBuffer->nDuration;

	int nRC = m_SourceAPI.GetSample(m_hSource, VO_SOURCE2_TT_RTSP_VIDEO, &nSample);

	if(nRC == VO_RET_SOURCE2_OK)
	{
		return	ConvertBufSrc2OS(&nSample, pBuffer, VOOSMP_SS_VIDEO);	
	}
	else if(nRC == (int)VO_RET_SOURCE2_NEEDRETRY)
	{
		return VOOSMP_ERR_Retry;
	}
	else if(nRC == (int)VO_RET_SOURCE2_END)
	{
		return VOOSMP_ERR_EOS;
	}
	else
	{
		return VOOSMP_ERR_Retry;
	}
}

int COSRTSPSource::LoadDll()
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = m_pLibop->LoadLib(m_pLibop->pUserData, "voSrcRTSP", 0);

	if (m_hDll == NULL)
	{
		VOLOGI("Load library voSrcRTSP failed");		
		return -1;
	}

	VOGETSRCFRAPI pGetAPI = (VOGETSRCFRAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetRTSP2ReadAPI", 0);

	if (pGetAPI == NULL)
	{
		VOLOGI("get API voGetRTSP2ReadAPI failed");
		return -1;
	}

	pGetAPI(&m_SourceAPI);

	if(m_SourceAPI.Init == NULL)
		return -1;

	return 0;
}

int COSRTSPSource::GetMediaCount(int ssType)
{
	return VOOSMP_ERR_Implement;
}

int COSRTSPSource::SelectMediaIndex(int ssType, int nIndex)
{
	return VOOSMP_ERR_Implement;
}

int COSRTSPSource::IsMediaIndexAvailable(int ssType, int nIndex)
{
	return VOOSMP_ERR_Implement;
}

int COSRTSPSource::GetMediaProperty(int ssType, int nIndex, VOOSMP_SRC_TRACK_PROPERTY **pProperty)
{
	return VOOSMP_ERR_Implement;
}

int COSRTSPSource::CommetSelection(int bSelect)
{
	return VOOSMP_ERR_Implement;
}

int COSRTSPSource::GetCurMediaTrack(VOOSMP_SRC_CURR_TRACK_INDEX *pCurIndex)
{
	return VOOSMP_ERR_Implement;
}