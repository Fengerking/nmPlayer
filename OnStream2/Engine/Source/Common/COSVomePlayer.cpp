#include "voIndex.h"

#include "vompType.h"
#include "vompAPI.h"
#include "COSVomePlayer.h"
#include "voOSFunc.h"

#define LOG_TAG "COSVomePlayer"
#include "voLog.h"

COSVomePlayer::COSVomePlayer()
	: m_nColor(VOMP_COLOR_RGB565_PACKED)
{
	memset(&m_rcDraw, 0, sizeof(VOMP_RECT));
	memset(&m_fListener, 0, sizeof(VOOSMP_LISTENERINFO));
	memset(&m_nAudioBuffer, 0, sizeof(VOOSMP_BUFFERTYPE));
	memset(&m_nVideoBuffer, 0, sizeof(VOOSMP_BUFFERTYPE));
	memset(&m_nSendBuffer, 0, sizeof(VOMP_BUFFERTYPE));
	memset(&m_sAudioFormat, 0, sizeof(VOMP_AUDIO_FORMAT));
	memset(&m_sVideoFormat, 0, sizeof(VOMP_VIDEO_FORMAT));
	memset(&m_sAudioTrackInfo, 0, sizeof(VOMP_TRACK_INFO));
	memset(&m_sVideoTrackInfo, 0, sizeof(VOMP_TRACK_INFO));

	memset(&m_sCPUInfo, 0, sizeof(VOMP_CPU_INFO));
	memset(&m_sPerformanceData, 0, sizeof(VOMP_PERFORMANCEDATA));

	m_sCPUInfo.nCoreCount = 1;
	m_sCPUInfo.nCPUType = 0;
	m_sCPUInfo.nFrequency = 500;

	m_fSendBufFunc.pUserData = this;
	m_fSendBufFunc.SendData = voSendBuffer;

	m_fReadBufFunc.pUserData = this;
	m_fReadBufFunc.ReadAudio = voReadAudio;
	m_fReadBufFunc.ReadVideo = voReadVideo;

	m_nSEICount = 300;
}

COSVomePlayer::~COSVomePlayer ()
{
	VOLOGUNINIT();
}

int COSVomePlayer::Init()
{
	VOLOGINIT(m_szPathLib);

	int nRC = COSVomeEngine::Init();
	if(nRC != VOMP_ERR_None) return ReturnCode(nRC);

	//nRC = COSVomeEngine::SetParam (VOMP_PID_FUNC_LIB, m_pLibFunc);
	//if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);

	nRC = COSVomeEngine::SetParam(VOMP_PID_PLAYER_PATH, m_szPathLib);
	if (nRC != VOOSMP_ERR_None) return ReturnCode(nRC);

	return ReturnCode(nRC);
}

int COSVomePlayer::Uninit()
{
	int nRC = COSVomeEngine::Uninit();

	return ReturnCode(nRC);
}

int COSVomePlayer::SetDataSource (void * pSource, int nFlag)
{
	int nRC;
	int mpnFlag = VOMP_FLAG_SOURCE_SYNC;
	void *mpSource = pSource;

	if((nFlag& VOOSMP_FLAG_SOURCE_URL) == VOOSMP_FLAG_SOURCE_URL)
		mpnFlag |= VOMP_FLAG_SOURCE_URL;

	if((nFlag& VOOSMP_FLAG_SOURCE_READBUFFER) == VOOSMP_FLAG_SOURCE_READBUFFER)
	{
		m_pOSReadBufFunc = (VOOSMP_READBUFFER_FUNC *)pSource;
		mpnFlag |= VOMP_FLAG_SOURCE_READBUFFER;
		mpSource = (void *) &m_fReadBufFunc;
	}

	if((nFlag& VOOSMP_FLAG_SOURCE_SENDBUFFER) == VOOSMP_FLAG_SOURCE_SENDBUFFER)
	{
		mpSource = NULL;
		mpnFlag |= VOMP_FLAG_SOURCE_SENDBUFFER;
	}

	nRC = COSVomeEngine::SetDataSource(mpSource, mpnFlag);
	
	return ReturnCode(nRC);
}

int COSVomePlayer::GetVideoBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	int nRC = COSVomeEngine::GetVideoBuffer(ppBuffer);

	return ReturnCode(nRC);
}

int COSVomePlayer::GetAudioBuffer (VOMP_BUFFERTYPE ** ppBuffer)
{
	int nRC = COSVomeEngine::GetAudioBuffer(ppBuffer);

	return ReturnCode(nRC);
}

int COSVomePlayer::SendBuffer (int nSSType, VOMP_BUFFERTYPE * pBuffer)
{
	int nRC = COSVomeEngine::SendBuffer(nSSType, pBuffer);

	return ReturnCode(nRC);
}

int COSVomePlayer::Run (void)
{
	int nRC = COSVomeEngine::Run();

	return ReturnCode(nRC);
}

int COSVomePlayer::Pause (void)
{
	int nRC = COSVomeEngine::Pause();

	return ReturnCode(nRC);
}

int COSVomePlayer::Stop (void)
{
	int nRC = COSVomeEngine::Stop();

	return ReturnCode(nRC);
}

int COSVomePlayer::Flush (void)
{
	int nRC = COSVomeEngine::Flush();

	return ReturnCode(nRC);
}

int COSVomePlayer::GetPos (int * pCurPos)
{
	int nRC = COSVomeEngine::GetPos(pCurPos);

	return ReturnCode(nRC);
}

int COSVomePlayer::SetPos (int nCurPos)
{
	int nRC = COSVomeEngine::SetPos(nCurPos);

	return ReturnCode(nRC);
}

int COSVomePlayer::GetParam (int nID, void * pValue)
{
	if (!pValue)
		return  VOOSMP_ERR_Pointer;

	int mpID = nID;
	void *mpValue = pValue;

	VOOSMP_PERFORMANCE_DATA *pPerformance = NULL;

	switch(nID)
	{
	case VOOSMP_PID_AUDIO_VOLUME:
		mpID = VOMP_PID_AUDIO_VOLUME;
		break;
	case VOOSMP_PID_DRAW_RECT:
		mpID = VOMP_PID_DRAW_RECT;
		break;
	case VOOSMP_PID_DRAW_COLOR:
		mpID = VOMP_PID_DRAW_COLOR;
		break;
	case VOOSMP_PID_FUNC_LIB:
		mpID = VOMP_PID_FUNC_LIB;
		pValue = (void *)m_pOSLibFunc;
		return VOOSMP_ERR_None;
	case VOOSMP_PID_MAX_BUFFTIME:
		mpID = VOMP_PID_MAX_BUFFTIME;
		break;
	case VOOSMP_PID_MIN_BUFFTIME:
		mpID = VOMP_PID_MIN_BUFFTIME;
		break;
	case VOOSMP_PID_VIDEO_REDRAW:
		mpID = VOMP_PID_VIDEO_REDRAW;
		break;
	case VOOSMP_PID_AUDIO_EFFECT_ENABLE:
		mpID = VOMP_PID_AUDIO_EFFECT_ENABLE;
		break;
	case VOOSMP_PID_LICENSE_TEXT:
		mpID = VOMP_PID_LICENSE_TEXT;
		break;
	case VOOSMP_PID_LICENSE_FILE_PATH:
		mpID = VOMP_PID_LICENSE_FILE_PATH;
		break;
	case VOOSMP_PID_PLAYER_PATH:
		mpID = VOMP_PID_PLAYER_PATH;
		break;
	case VOOSMP_PID_FUNC_DRM:
		mpID = VOMP_PID_FUNC_DRM;
		break;
	case VOOSMP_PID_AUDIO_REND_ADJUST_TIME:
		mpID = VOMP_PID_AUDIO_REND_ADJUTIME;
		break;
	case VOOSMP_PID_AUDIO_STEP_BUFFTIME:
		mpID = VOMP_PID_AUDIO_STEP_BUFFTIME;
		break;
	case VOOSMP_PID_AUDIO_SINK_BUFFTIME:
		mpID = VOMP_PID_AUDIO_SINK_BUFFTIME;
		break;
	case VOOSMP_PID_AUDIO_DEC_BITRATE:
		mpID = VOMP_PID_AUDIO_DEC_BITRATE;
		break;
	case VOOSMP_PID_VIDEO_DEC_BITRATE:
		mpID = VOMP_PID_VIDEO_DEC_BITRATE;
		break;
	case VOOSMP_PID_STATUS:
		return GetStatus ((int *)pValue);
	case VOOSMP_PID_SENDBUFFERFUNC:
		((VOOSMP_SENDBUFFER_FUNC *)pValue)->pUserData = m_fSendBufFunc.pUserData;
		((VOOSMP_SENDBUFFER_FUNC *)pValue)->SendData = m_fSendBufFunc.SendData;
		return VOOSMP_ERR_None;
	case VOOSMP_PID_AUDIO_FORMAT:
		mpID = VOMP_PID_AUDIO_FORMAT;
		mpValue = (void *)&m_sAudioFormat;
		break;
	case VOOSMP_PID_AUDIO_RENDER_DATA_FORMAT:
		mpID = VOMP_PID_AUDIO_RENDER_DATA_FORMAT;
		break;
	case VOOSMP_PID_VIDEO_FORMAT:
		mpID = VOMP_PID_VIDEO_FORMAT;
		mpValue = (void *)&m_sVideoFormat;
		break;
	case VOOSMP_PID_AUDIO_PLAYBACK_SPEED:
		mpID = VOMP_PID_AUDIO_PLAYBACK_SPEED;
		break;
	case VOOSMP_PID_CPU_INFO:
		mpID = VOMP_PID_CPU_INFO;
		mpValue = (void *)&m_sCPUInfo;
		break;
	case VOOSMP_PID_VIDEO_PERFORMANCE_OPTION:
		mpID = VOMP_PID_VIDEO_PERFORMANCE_OPTION;

		pPerformance = (VOOSMP_PERFORMANCE_DATA *)pValue;

#if 0
		m_sPerformanceData.nLastTime		= pPerformance->nLastTime;
		m_sPerformanceData.nSourceDropNum	= pPerformance->nSourceDropNum;
		m_sPerformanceData.nCodecDropNum	= pPerformance->nCodecDropNum;
		m_sPerformanceData.nRenderDropNum	= pPerformance->nRenderDropNum;
		m_sPerformanceData.nDecodedNum		= pPerformance->nDecodedNum;
		m_sPerformanceData.nRenderNum		= pPerformance->nRenderNum;
		m_sPerformanceData.nSourceTimeNum	= pPerformance->nSourceTimeNum;
		m_sPerformanceData.nCodecTimeNum	= pPerformance->nCodecTimeNum;
		m_sPerformanceData.nRenderTimeNum	= pPerformance->nRenderTimeNum;
		m_sPerformanceData.nJitterNum		= pPerformance->nJitterNum;
		m_sPerformanceData.nCodecErrorsNum	= pPerformance->nCodecErrorsNum;
		m_sPerformanceData.nCodecErrors		= pPerformance->nCodecErrors;
		m_sPerformanceData.nCPULoad			= pPerformance->nCPULoad;
		m_sPerformanceData.nFrequency		= pPerformance->nFrequency;
		m_sPerformanceData.nMaxFrequency	= pPerformance->nMaxFrequency;
		m_sPerformanceData.nWorstDecodeTime	= pPerformance->nWorstDecodeTime;
		m_sPerformanceData.nWorstRenderTime	= pPerformance->nWorstRenderTime;
		m_sPerformanceData.nAverageDecodeTime= pPerformance->nAverageDecodeTime;
		m_sPerformanceData.nAverageRenderTime= pPerformance->nAverageRenderTime;
		m_sPerformanceData.nTotalCPULoad	= pPerformance->nTotalCPULoad;
		m_sPerformanceData.nTotalPlaybackDuration	= pPerformance->nTotalPlaybackDuration;
		m_sPerformanceData.nTotalSourceDropNum	= pPerformance->nTotalSourceDropNum;
		m_sPerformanceData.nTotalCodecDropNum	= pPerformance->nTotalCodecDropNum;
		m_sPerformanceData.nTotalRenderDropNum	= pPerformance->nTotalRenderDropNum;
		m_sPerformanceData.nTotalDecodedNum	= pPerformance->nTotalDecodedNum;
		m_sPerformanceData.nTotalRenderNum	= pPerformance->nTotalRenderNum;
#endif
		mpValue = (void *)&m_sPerformanceData;
		break;
	}
	
	int nRC = COSVomeEngine::GetParam(mpID, mpValue); 

	if(nID == VOOSMP_PID_AUDIO_FORMAT)
	{
		VOOSMP_AUDIO_FORMAT *pAudioFormat = (VOOSMP_AUDIO_FORMAT *)pValue;
		pAudioFormat->Channels = m_sAudioFormat.Channels;
		pAudioFormat->SampleBits = m_sAudioFormat.SampleBits;
		pAudioFormat->SampleRate = m_sAudioFormat.SampleRate;
	}
	else if(nID == VOOSMP_PID_VIDEO_FORMAT)
	{
		VOOSMP_VIDEO_FORMAT *pVideoFormat = (VOOSMP_VIDEO_FORMAT *)pValue;
		pVideoFormat->Width = m_sVideoFormat.Width;
		pVideoFormat->Height = m_sVideoFormat.Height;
		pVideoFormat->Type = m_sVideoFormat.Type;

		//m_sInVideoFormat.sFormat.video.Width = m_sVideoFormat.Width;
		//m_sInVideoFormat.sFormat.video.Height = m_sVideoFormat.Height;
		//m_sInVideoFormat.sFormat.video.Type = m_sVideoFormat.Type;
	}
	else if(nID == VOOSMP_PID_CPU_INFO)
	{
		VOOSMP_CPU_INFO *pCPUInfo = (VOOSMP_CPU_INFO *)pValue;

		pCPUInfo->nCoreCount	= m_sCPUInfo.nCoreCount;
		pCPUInfo->nCPUType		= m_sCPUInfo.nCPUType;
		pCPUInfo->nFrequency	= m_sCPUInfo.nFrequency;
		pCPUInfo->llReserved	= m_sCPUInfo.lReserved;
	}
	else if(nID == VOOSMP_PID_VIDEO_PERFORMANCE_OPTION)
	{
		if ((VO_U32)nRC == VO_ERR_LICENSE_ERROR)
			return VOOSMP_ERR_LicenseFailed;

		pPerformance = (VOOSMP_PERFORMANCE_DATA *)pValue;

		pPerformance->nLastTime			= m_sPerformanceData.nLastTime;
		pPerformance->nSourceDropNum	= m_sPerformanceData.nSourceDropNum;
		pPerformance->nCodecDropNum		= m_sPerformanceData.nCodecDropNum;
		pPerformance->nRenderDropNum	= m_sPerformanceData.nRenderDropNum;
		pPerformance->nDecodedNum		= m_sPerformanceData.nDecodedNum;
		pPerformance->nRenderNum		= m_sPerformanceData.nRenderNum;
		pPerformance->nSourceTimeNum	= m_sPerformanceData.nSourceTimeNum;
		pPerformance->nCodecTimeNum		= m_sPerformanceData.nCodecTimeNum;
		pPerformance->nRenderTimeNum	= m_sPerformanceData.nRenderTimeNum;
		pPerformance->nJitterNum		= m_sPerformanceData.nJitterNum;
		pPerformance->nCodecErrorsNum	= m_sPerformanceData.nCodecErrorsNum;
		pPerformance->nCodecErrors		= m_sPerformanceData.nCodecErrors;
		pPerformance->nCPULoad			= m_sPerformanceData.nCPULoad;
		pPerformance->nFrequency		= m_sPerformanceData.nFrequency;
		pPerformance->nMaxFrequency		= m_sPerformanceData.nMaxFrequency;
		pPerformance->nWorstDecodeTime	= m_sPerformanceData.nWorstDecodeTime;
		pPerformance->nWorstRenderTime	= m_sPerformanceData.nWorstRenderTime;
		pPerformance->nAverageDecodeTime = m_sPerformanceData.nAverageDecodeTime;
		pPerformance->nAverageRenderTime = m_sPerformanceData.nAverageRenderTime;
		pPerformance->nTotalCPULoad		 = m_sPerformanceData.nTotalCPULoad;

		pPerformance->nTotalPlaybackDuration = m_sPerformanceData.nTotalPlaybackDuration; 
		pPerformance->nTotalSourceDropNum = m_sPerformanceData.nTotalSourceDropNum;
		pPerformance->nTotalCodecDropNum = m_sPerformanceData.nTotalCodecDropNum;
		pPerformance->nTotalRenderDropNum = m_sPerformanceData.nTotalRenderDropNum;
		pPerformance->nTotalDecodedNum = m_sPerformanceData.nTotalDecodedNum;
		pPerformance->nTotalRenderNum = m_sPerformanceData.nTotalRenderNum;
	}
    else
    {
        return COSBasePlayer::GetParam(nID, pValue);
    }

	return ReturnCode(nRC);
}

int	COSVomePlayer::GetStatus (int * pStatus)
{
	VOMP_STATUS mpStatus = VOMP_STATUS_INIT;

	int nRC = COSVomeEngine::GetStatus((int *)&mpStatus);
	if(nRC) return nRC;

	if(mpStatus == VOMP_STATUS_INIT)
	{
		*pStatus = VOOSMP_STATUS_INIT;
	}
	else if(mpStatus == VOMP_STATUS_LOADING)
	{
		*pStatus = VOOSMP_STATUS_LOADING;
	}
	else if(mpStatus == VOMP_STATUS_RUNNING)
	{
		*pStatus = VOOSMP_STATUS_RUNNING;
	}
	else if(mpStatus == VOMP_STATUS_PAUSED)
	{
		*pStatus = VOOSMP_STATUS_PAUSED;
	}
	else if(mpStatus == VOMP_STATUS_STOPPED)
	{
		*pStatus = VOOSMP_STATUS_STOPPED;
	}

	return ReturnCode(nRC);
}

int COSVomePlayer::SetParam (int nID, void * pValue)
{
	int mpID = nID;
	void *mpValue = pValue;
	VOOSMP_RECT *pRect = NULL;
	VOOSMP_LISTENERINFO *pListenInfo = NULL;
	VOOSMP_PERFORMANCE_DATA *pPerformance = NULL;

	if(COSBasePlayer::SetParam(nID, pValue) == VOOSMP_ERR_None)
		return VOOSMP_ERR_None;
	
	switch(nID)
	{
	case VOOSMP_PID_COMMON_LOGFUNC:
		if(pValue)
			memcpy (&m_cbVOLOG, pValue, sizeof (m_cbVOLOG));
		return VOOSMP_ERR_None;
	case VOOSMP_PID_AUDIO_VOLUME:
		mpID = VOMP_PID_AUDIO_VOLUME;
		break;
	case VOOSMP_PID_DRAW_RECT:
		mpID = VOMP_PID_DRAW_RECT;
		pRect = (VOOSMP_RECT *)pValue;
		if(pRect != NULL)
		{
			m_rcDraw.left = pRect->nLeft;
			m_rcDraw.top = pRect->nTop;
			m_rcDraw.right = pRect->nRight;
			m_rcDraw.bottom = pRect->nBottom;
		}
		mpValue = (void *)&m_rcDraw;
		break;
	case VOOSMP_PID_DRAW_COLOR:
		mpID = VOMP_PID_DRAW_COLOR;
		m_nColor = (VOMP_COLORTYPE)ConvertColorOS2MP((VOOSMP_COLORTYPE *)pValue);
		mpValue =(void *) &m_nColor;
		break;
	case VOOSMP_PID_FUNC_LIB:
		mpID = VOMP_PID_FUNC_LIB;
		m_pOSLibFunc = (VOOSMP_LIB_FUNC*)pValue;
		if(m_pOSLibFunc != NULL)
		{
			m_fLibFunc.pUserData	= m_pOSLibFunc->pUserData;
			m_fLibFunc.LoadLib		= m_pOSLibFunc->LoadLib;
			m_fLibFunc.GetAddress	= m_pOSLibFunc->GetAddress;
			m_fLibFunc.FreeLib		= m_pOSLibFunc->FreeLib;
			
			m_pLibFunc = &m_fLibFunc;
		}
		mpValue = (void *)m_pLibFunc;
		return VOOSMP_ERR_None;
		break;
	case VOOSMP_PID_MAX_BUFFTIME:
		mpID = VOMP_PID_MAX_BUFFTIME;
		break;
	case VOOSMP_PID_MIN_BUFFTIME:
		mpID = VOMP_PID_MIN_BUFFTIME;
		break;
	case VOOSMP_PID_VIDEO_REDRAW:
		mpID = VOMP_PID_VIDEO_REDRAW;
		break;
	case VOOSMP_PID_AUDIO_EFFECT_ENABLE:
		mpID = VOMP_PID_AUDIO_EFFECT_ENABLE;
		break;
	case VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE:
		mpID = VOMP_PID_AUDIO_EFFECT_MODULE;
		break;
	case VOOSMP_PID_LICENSE_TEXT:
		mpID = VOMP_PID_LICENSE_TEXT;
		break;
	case VOOSMP_PID_LICENSE_CONTENT:
		mpID = VOMP_PID_LICENSE_DATA;
		break;
	case VOOSMP_PID_LICENSE_FILE_PATH:
		mpID = VOMP_PID_LICENSE_FILE_PATH;
		break;
	case VOOSMP_PID_FUNC_DRM:
		mpID = VOMP_PID_FUNC_DRM;
		break;
	case VOOSMP_PID_AUDIO_REND_ADJUST_TIME:
		mpID = VOMP_PID_AUDIO_REND_ADJUTIME;
		break;
	case VOOSMP_PID_CPU_NUMBER:
		mpID = VOMP_PID_CPU_NUMBER;
		break;
	case VOOSMP_PID_AUDIO_STEP_BUFFTIME:
		mpID = VOMP_PID_AUDIO_STEP_BUFFTIME;
		break;
	case VOOSMP_PID_AUDIO_REND_BUFFER_TIME:		
		mpID = VOMP_PID_AUDIO_SINK_BUFFTIME;
		break;
	case VOOSMP_PID_AUDIO_SINK_BUFFTIME:
		mpID = VOMP_PID_AUDIO_SINK_BUFFTIME;
		break;
	case VOOSMP_PID_LOW_LATENCY:
		mpID = VOMP_PID_LOW_LATENCE;
		break;
	case VOOSMP_PID_MFW_CFG_FILE:
		mpID = VOMP_PID_PLAY_CONFIG_FILE;
		break;
	case VOOSMP_PID_DEBLOCK_ONOFF:
		mpID = VOMP_PID_VIDEO_DISABLE_DEBLOCK;
		break;
	case VOOSMP_PID_AUDIO_STREAM_ONOFF:
		mpID = VOMP_PID_AUDIO_STREAM_ONOFF;
		break;
	case VOOSMP_PID_VIDEO_STREAM_ONOFF:
		mpID = VOMP_PID_VIDEO_STREAM_ONOFF;
		break;
	case VOOSMP_PID_LISTENER:
		pListenInfo = (VOOSMP_LISTENERINFO *)pValue;
		if(pListenInfo != NULL)
		{
			m_fListener.pUserData = pListenInfo->pUserData;
			m_fListener.pListener = pListenInfo->pListener;
		}
		return VOOSMP_ERR_None;
	case VOOSMP_PID_PLAYER_PATH:
		mpID = VOMP_PID_PLAYER_PATH;
		if(pValue)
			vostrcpy(m_szPathLib, (VO_TCHAR *)pValue);
		break;
	case VOOSMP_PID_COMMAND_STRING:
		mpID = VOMP_PID_COMMON_STRING;
		break;
	case VOOSMP_PID_VIDEO_PERFORMANCE_CACHE:
		mpID = VOMP_PID_VIDEO_PERFORMANCE_CACHE;
		break;
	case VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF:
		mpID = VOMP_PID_VIDEO_PERFORMANCE_ONOFF;
		break;
	case VOOSMP_PID_AUDIO_PCM_OUTPUT:
		mpID = VOMP_PID_AUDIO_PCM_OUTPUT;
		break;

	case VOOSMP_PID_AUDIO_PLAYBACK_SPEED:
		mpID = VOMP_PID_AUDIO_PLAYBACK_SPEED;
		mpValue = pValue;		
		break;

	case VOOSMP_PID_DRM_UNIQUE_IDENTIFIER:
		mpID = VOMP_PID_DRM_UNIQUE_IDENTIFIER;
		break;
	case VOOSMP_PID_AUDIO_DSP_CLOCK:
		mpID = VOMP_PID_AUDIO_DSP_CLOCK ;
		break;

	case VOOSMP_PID_PAUSE_REFERENCE_COLOCK:
		mpID = VOMP_PID_PAUSE_REFCOLOK;
		mpValue = pValue;
		break;

	case VOOSMP_PID_LOAD_DOLBY_DECODER_MODULE:
		mpID = VOMP_PID_LOAD_DOLBY_DECODER_MODULE;
		mpValue = pValue;
		break;

	case VOOSMP_PID_VIDEO_PERFORMANCE_OPTION:
		mpID = VOMP_PID_VIDEO_PERFORMANCE_OPTION;

		if(pValue)
		{
			pPerformance = (VOOSMP_PERFORMANCE_DATA *)pValue;

			m_sPerformanceData.nLastTime		= pPerformance->nLastTime;
			m_sPerformanceData.nSourceDropNum	= pPerformance->nSourceDropNum;
			m_sPerformanceData.nCodecDropNum	= pPerformance->nCodecDropNum;
			m_sPerformanceData.nRenderDropNum	= pPerformance->nRenderDropNum;
			m_sPerformanceData.nDecodedNum		= pPerformance->nDecodedNum;
			m_sPerformanceData.nRenderNum		= pPerformance->nRenderNum;
			m_sPerformanceData.nSourceTimeNum	= pPerformance->nSourceTimeNum;
			m_sPerformanceData.nCodecTimeNum	= pPerformance->nCodecTimeNum;
			m_sPerformanceData.nRenderTimeNum	= pPerformance->nRenderTimeNum;
			m_sPerformanceData.nJitterNum		= pPerformance->nJitterNum;
			m_sPerformanceData.nCodecErrorsNum	= pPerformance->nCodecErrorsNum;
			m_sPerformanceData.nCodecErrors		= pPerformance->nCodecErrors;
			m_sPerformanceData.nCPULoad			= pPerformance->nCPULoad;
			m_sPerformanceData.nFrequency		= pPerformance->nFrequency;
			m_sPerformanceData.nMaxFrequency	= pPerformance->nMaxFrequency;
			m_sPerformanceData.nWorstDecodeTime	= pPerformance->nWorstDecodeTime;
			m_sPerformanceData.nWorstRenderTime	= pPerformance->nWorstRenderTime;
			m_sPerformanceData.nAverageDecodeTime= pPerformance->nAverageDecodeTime;
			m_sPerformanceData.nAverageRenderTime= pPerformance->nAverageRenderTime;
			m_sPerformanceData.nTotalCPULoad = pPerformance->nTotalCPULoad;
		}

		mpValue = (void *)&m_sPerformanceData;
		break;
	}

	if((nID & 0xFFFF0000) == VOOSMP_PID_AUDIO_DECODER_FILE_NAME)
	{
		int nCodec = ConvertCodecOS2MP(nID&0xFFFF, VOOSMP_SS_AUDIO);
		mpID = VOMP_PID_AUDIO_CODEC_FILENAME | nCodec;			
	}
	else if((nID & 0xFFFF0000) == VOOSMP_PID_VIDEO_DECODER_FILE_NAME)
	{
		int nCodec = ConvertCodecOS2MP(nID&0xFFFF, VOOSMP_SS_VIDEO);
		mpID = VOMP_PID_VIDEO_CODEC_FILENAME | nCodec;
	}
	else if((nID & 0xFFFF0000) == VOOSMP_PID_AUDIO_DECODER_API_NAME)
	{
		int nCodec = ConvertCodecOS2MP(nID&0xFFFF, VOOSMP_SS_AUDIO);
		mpID = VOMP_PID_AUDIO_CODEC_APINAME | nCodec;
	}
	else if((nID & 0xFFFF0000) == VOOSMP_PID_VIDEO_DECODER_API_NAME)
	{
		int nCodec = ConvertCodecOS2MP(nID&0xFFFF, VOOSMP_SS_VIDEO);
		mpID = VOMP_PID_VIDEO_CODEC_APINAME | nCodec;
	}

	int nRC = COSVomeEngine::SetParam(mpID, mpValue); 
	
	if ((VO_U32	)nRC == VO_ERR_LICENSE_ERROR)
		return VOOSMP_ERR_LicenseFailed;

	return ReturnCode(nRC);
}

int	COSVomePlayer::voReadAudio(void * pUserData, VOMP_BUFFERTYPE * pBuffer)
{
	COSVomePlayer * pPlayer = (COSVomePlayer *)pUserData;
	
	if(pPlayer->m_pOSReadBufFunc == NULL|| pPlayer->m_pOSReadBufFunc->ReadAudio == NULL)
		return VOOSMP_ERR_Implement;

	memset(&pPlayer->m_nAudioBuffer, 0, sizeof(VOOSMP_BUFFERTYPE));

	pPlayer->m_nAudioBuffer.llTime = pBuffer->llTime;
	int nRC =  pPlayer->doReadAudio(&pPlayer->m_nAudioBuffer);

	pPlayer->ConvertBufOS2MP(&pPlayer->m_nAudioBuffer, pBuffer, VOOSMP_SS_AUDIO);

	return pPlayer->ReturnMPCode (nRC);
}

int	COSVomePlayer::voReadVideo(void * pUserData, VOMP_BUFFERTYPE * pBuffer)
{
	COSVomePlayer * pPlayer = (COSVomePlayer *)pUserData;
	
	if(pPlayer->m_pOSReadBufFunc == NULL|| pPlayer->m_pOSReadBufFunc->ReadVideo == NULL)
		return VOOSMP_ERR_Implement;

	memset(&pPlayer->m_nVideoBuffer, 0, sizeof(VOOSMP_BUFFERTYPE));
	pPlayer->m_nVideoBuffer.llTime = pBuffer->llTime;
	pPlayer->m_nVideoBuffer.nDuration = pBuffer->nReserve;

	int nRC = pPlayer->doReadVideo(&pPlayer->m_nVideoBuffer);

	pPlayer->ConvertBufOS2MP(&pPlayer->m_nVideoBuffer, pBuffer, VOOSMP_SS_VIDEO);

	return pPlayer->ReturnMPCode (nRC);
}

int	COSVomePlayer::voSendBuffer (void * pUserData, int nSSType, VOOSMP_BUFFERTYPE * pBuffer)
{
	COSVomePlayer * pPlayer = (COSVomePlayer *)pUserData;
	
	pPlayer->ConvertBufOS2MP(pBuffer, &pPlayer->m_nSendBuffer, nSSType);

	return pPlayer->SendBuffer(nSSType, &pPlayer->m_nSendBuffer);
}

int	COSVomePlayer::ReturnCode (unsigned int nRC)
{
	int osnRC = nRC; 

	switch(nRC)
	{
	case	VOMP_ERR_None:
		osnRC = VOOSMP_ERR_None;
		break;
	case	VOMP_ERR_EOS:
		osnRC = VOOSMP_ERR_EOS;
		break;
	case	VOMP_ERR_Retry:
		osnRC = VOOSMP_ERR_Retry;
		break;
	case	VOMP_ERR_FormatChange:
		osnRC = VOOSMP_ERR_FormatChange;
		break;
	case	VOMP_ERR_File:
		osnRC = VOOSMP_ERR_Unknown;
		break;
	case	VOMP_ERR_Video:
		osnRC = VOOSMP_ERR_Video;
		break;
	case	VOMP_ERR_Audio:
		osnRC = VOOSMP_ERR_Audio;
		break;
	case	VOMP_ERR_OutMemory:
		osnRC = VOOSMP_ERR_OutMemory;
		break;
	case	VOMP_ERR_Pointer:
		osnRC = VOOSMP_ERR_Pointer;
		break;
	case	VOMP_ERR_ParamID:
		osnRC = VOOSMP_ERR_ParamID;
		break;
	case	VOMP_ERR_Status:
		osnRC = VOOSMP_ERR_Status;
		break;
	case	VOMP_ERR_Implement:
		osnRC = VOOSMP_ERR_Implement;
		break;
	case	VOMP_ERR_SmallSize:
		osnRC = VOOSMP_ERR_SmallSize;
		break;
	case	VOMP_ERR_OutOfTime:
		osnRC = VOOSMP_ERR_OutOfTime;
		break;
	case	VOMP_ERR_WaitTime:
		osnRC = VOOSMP_ERR_WaitTime;
		break;
	case	VOMP_ERR_Unknown:
		osnRC = VOOSMP_ERR_Unknown;
		break;
	case	VOMP_ERR_Audio_No_Now:
		osnRC = VOOSMP_ERR_Audio_No_Now;
		break;
	case	VOMP_ERR_Video_No_Now:
		osnRC = VOOSMP_ERR_Video_No_Now;
		break;
	case	VOMP_ERR_FLush_Buffer:
		osnRC = VOOSMP_ERR_FLush_Buffer;
		break;
	}

	return osnRC;
}

int	COSVomePlayer::ReturnMPCode (unsigned int nRC)
{
	int mpRC = nRC; 

	switch(nRC)
	{
	case	VOOSMP_ERR_None:
		mpRC = VOMP_ERR_None;
		break;
	case	VOOSMP_ERR_EOS:
		mpRC = VOMP_ERR_EOS;
		break;
	case	VOOSMP_ERR_Retry:
		mpRC = VOMP_ERR_Retry;
		break;
	case	VOOSMP_ERR_FormatChange:
		mpRC = VOMP_ERR_FormatChange;
		break;
	case	VOOSMP_ERR_Video:
		mpRC = VOMP_ERR_Video;
		break;
	case	VOOSMP_ERR_Audio:
		mpRC = VOMP_ERR_Audio;
		break;
	case	VOOSMP_ERR_OutMemory:
		mpRC = VOMP_ERR_OutMemory;
		break;
	case	VOOSMP_ERR_Pointer:
		mpRC = VOMP_ERR_Pointer;
		break;
	case	VOOSMP_ERR_ParamID:
		mpRC = VOMP_ERR_ParamID;
		break;
	case	VOOSMP_ERR_Status:
		mpRC = VOMP_ERR_Status;
		break;
	case	VOOSMP_ERR_Implement:
		mpRC = VOMP_ERR_Implement;
		break;
	case	VOOSMP_ERR_SmallSize:
		mpRC = VOMP_ERR_SmallSize;
		break;
	case	VOOSMP_ERR_OutOfTime:
		mpRC = VOMP_ERR_OutOfTime;
		break;
	case	VOOSMP_ERR_WaitTime:
		mpRC = VOMP_ERR_WaitTime;
		break;
	case	VOOSMP_ERR_Unknown:
		mpRC = VOMP_ERR_Unknown;
		break;
	case	VOOSMP_ERR_Audio_No_Now:
		mpRC = VOMP_ERR_Audio_No_Now;
		break;
	case	VOOSMP_ERR_Video_No_Now:
		mpRC = VOMP_ERR_Video_No_Now;
		break;
	case	VOOSMP_ERR_FLush_Buffer:
		mpRC = VOMP_ERR_FLush_Buffer;
		break;
	}

	return mpRC;
}

int	COSVomePlayer::ChangEventIDEvent (unsigned int nID, void * pParam1, void * pParam2)
{
	int nEventID = nID;
	
	switch(nID)
	{
	case	VOMP_CB_PlayComplete:
		nEventID = VOOSMP_CB_PlayComplete;
		break;
	case	VOMP_CB_BufferStatus:
		nEventID = VOOSMP_CB_BufferStatus;
		break;
	case	VOMP_CB_VideoStartBuff:
		nEventID = VOOSMP_CB_VideoStartBuff;
		break;
	case	VOMP_CB_VideoStopBuff:
		nEventID = VOOSMP_CB_VideoStopBuff;
		break;
	case	VOMP_CB_AudioStartBuff:
		nEventID = VOOSMP_CB_AudioStartBuff;
		break;
	case	VOMP_CB_AudioStopBuff:
		nEventID = VOOSMP_CB_AudioStopBuff;
		break;
	case	VOMP_CB_SourceBuffTime:
		nEventID = VOOSMP_CB_SourceBuffTime;
		break;
	case	VOMP_CB_SeekComplete:
		nEventID = VOOSMP_CB_SeekComplete;
		m_bSeeking = VO_FALSE;
		break;
	case	VOMP_CB_VideoDelayTime:
		nEventID = VOOSMP_CB_VideoDelayTime;
		break;
	case	VOMP_CB_VideoLastTime:
		nEventID = VOOSMP_CB_VideoLastTime;
		break;
	case	VOMP_CB_VideoDropFrames:
		nEventID = VOOSMP_CB_VideoDropFrames;
		break;
	case	VOMP_CB_VideoFrameRate:
		nEventID = VOOSMP_CB_VideoFrameRate;
		break;
	case	VOMP_CB_Error:
		nEventID = VOOSMP_CB_Error;
		break;
	case VOMP_CB_CodecNotSupport:
		nEventID = VOOSMP_CB_CodecNotSupport;
		break;
	case	VOMP_CB_Deblock:
		nEventID = VOOSMP_CB_Deblock;
		break;
	case	VOMP_CB_HWStatus:
		nEventID = VOOSMP_CB_HWDecoderStatus;
		break;
	case	VOMP_CB_VideoAspectRatio:
		{
			if(pParam1)
			{
				if(*(int *)pParam1 == VOMP_RATIO_00)
					*(int *)pParam1 = VOOSMP_RATIO_00;
				else if(*(int *)pParam1 == VOMP_RATIO_11)
					*(int *)pParam1 = VOOSMP_RATIO_11;
				else if(*(int *)pParam1 == VOMP_RATIO_43)
					*(int *)pParam1 = VOOSMP_RATIO_43;
				else if(*(int *)pParam1 == VOMP_RATIO_169)
					*(int *)pParam1 = VOOSMP_RATIO_169;
				else if(*(int *)pParam1 == VOMP_RATIO_21)
					*(int *)pParam1 = VOOSMP_RATIO_21;
			}

			nEventID = VOOSMP_CB_VideoAspectRatio;
		}
		break;
	case	VOMP_CB_MediaChanged:
		{
			if(pParam1)
			{
				if(*(int *)pParam1 == VOMP_AVAILABLE_PUREAUDIO)
					*(int *)pParam1 = VOOSMP_AVAILABLE_PUREAUDIO;
				else if(*(int *)pParam1 == VOMP_AVAILABLE_PUREVIDEO)
					*(int *)pParam1 = VOOSMP_AVAILABLE_PUREVIDEO;
				else if(*(int *)pParam1 == VOMP_AVAILABLE_AUDIOVIDEO)
					*(int *)pParam1 = VOOSMP_AVAILABLE_AUDIOVIDEO;
			}

			nEventID = VOOSMP_CB_MediaTypeChanged;
		}
		break;
	case VOMP_CB_FirstFrameRender:
		nEventID = VOOSMP_CB_VideoRenderStart;
		break;
  case VOMP_CB_Pcm_Output:
		nEventID = VOOSMP_CB_PCM_OUTPUT;
		break;
  case VOMP_CB_LicenseFailed:
	  nEventID = VOOSMP_CB_LicenseFailed;
	  break;
	}

	return nEventID;
}

int	COSVomePlayer::HandleEvent (int nID, void * pParam1, void * pParam2)
{
	COSVomeEngine::HandleEvent(nID, pParam1, pParam2);

	if(m_fListener.pUserData != NULL && m_fListener.pListener != NULL)
		m_fListener.pListener(m_fListener.pUserData, nID, pParam1, pParam2);

	return 0;
}

int COSVomePlayer::ConvertCodecOS2MP(int sCodec, int nSSType)
{
	int dstCodec = 0;

	if(nSSType == VOOSMP_SS_AUDIO)
	{
		switch(sCodec)
		{
		case VOOSMP_AUDIO_CodingUnused:
			dstCodec = VOMP_AUDIO_CodingUnused;
			break;
		case VOOSMP_AUDIO_CodingPCM:
			dstCodec = VOMP_AUDIO_CodingPCM;
			break;
		case VOOSMP_AUDIO_CodingADPCM:
			dstCodec = VOMP_AUDIO_CodingADPCM;
			break;
		case VOOSMP_AUDIO_CodingAMRNB:
			dstCodec = VOMP_AUDIO_CodingAMRNB;
			break;
		case VOOSMP_AUDIO_CodingAMRWB:
			dstCodec = VOMP_AUDIO_CodingAMRWB;
			break;
		case VOOSMP_AUDIO_CodingAMRWBP:
			dstCodec = VOMP_AUDIO_CodingAMRWBP;
			break;
		case VOOSMP_AUDIO_CodingQCELP13:
			dstCodec = VOMP_AUDIO_CodingQCELP13;
			break;
		case VOOSMP_AUDIO_CodingEVRC:
			dstCodec = VOMP_AUDIO_CodingEVRC;
			break;
		case VOOSMP_AUDIO_CodingAAC:  
			dstCodec = VOMP_AUDIO_CodingAAC;
			break;
		case VOOSMP_AUDIO_CodingAC3:
			dstCodec = VOMP_AUDIO_CodingAC3;
			break;
		case VOOSMP_AUDIO_CodingFLAC:
			dstCodec = VOMP_AUDIO_CodingFLAC;
			break;
		case VOOSMP_AUDIO_CodingMP1:
			dstCodec = VOMP_AUDIO_CodingMP1;
			break;
		case VOOSMP_AUDIO_CodingMP3:
			dstCodec = VOMP_AUDIO_CodingMP3;
			break;
		case VOOSMP_AUDIO_CodingOGG:
			dstCodec = VOMP_AUDIO_CodingOGG;
			break;
		case VOOSMP_AUDIO_CodingWMA:
			dstCodec = VOMP_AUDIO_CodingWMA;
			break;
		case VOOSMP_AUDIO_CodingRA:
			dstCodec = VOMP_AUDIO_CodingRA;
			break;
		case VOOSMP_AUDIO_CodingMIDI:
			dstCodec = VOMP_AUDIO_CodingMIDI;
			break;
		case VOOSMP_AUDIO_CodingDRA:
			dstCodec = VOMP_AUDIO_CodingDRA;
			break;
		case VOOSMP_AUDIO_CodingG729:
			dstCodec = VOMP_AUDIO_CodingG729;
			break;
		case VOOSMP_AUDIO_CodingEAC3:
			dstCodec = VOMP_AUDIO_CodingEAC3;
			break;
		case VOOSMP_AUDIO_CodingAPE:
			dstCodec = VOMP_AUDIO_CodingAPE;
			break;
		case VOOSMP_AUDIO_CodingALAC:
			dstCodec = VOMP_AUDIO_CodingALAC;
			break;
		case VOOSMP_AUDIO_CodingDTS:
			dstCodec = VOMP_AUDIO_CodingDTS;
			break;
		}
	}
	else if(nSSType == VOOSMP_SS_VIDEO)
	{
		switch(sCodec)
		{
		case VOOSMP_VIDEO_CodingUnused:
			dstCodec = VOMP_VIDEO_CodingUnused;
			break;
		case VOOSMP_VIDEO_CodingMPEG2:
			dstCodec = VOMP_VIDEO_CodingMPEG2;
			break;
		case VOOSMP_VIDEO_CodingH263:
			dstCodec = VOMP_VIDEO_CodingH263;
			break;
		case VOOSMP_VIDEO_CodingS263:
			dstCodec = VOMP_VIDEO_CodingS263;
			break;
		case VOOSMP_VIDEO_CodingMPEG4:
			dstCodec = VOMP_VIDEO_CodingMPEG4;
			break;
		case VOOSMP_VIDEO_CodingH264:
			dstCodec = VOMP_VIDEO_CodingH264;
			break;
		case VOOSMP_VIDEO_CodingH265:
			dstCodec = VOMP_VIDEO_CodingH265;
			break;
		case VOOSMP_VIDEO_CodingWMV:
			dstCodec = VOMP_VIDEO_CodingWMV;
			break;
		case VOOSMP_VIDEO_CodingRV:
			dstCodec = VOMP_VIDEO_CodingRV;
			break;
		case VOOSMP_VIDEO_CodingMJPEG:
			dstCodec = VOMP_VIDEO_CodingMJPEG;
			break;
		case VOOSMP_VIDEO_CodingDIVX:
			dstCodec = VOMP_VIDEO_CodingDIVX;
			break;
		case VOOSMP_VIDEO_CodingVP6:
			dstCodec = VOMP_VIDEO_CodingVP6;
			break;
		case VOOSMP_VIDEO_CodingVP8:
			dstCodec = VOMP_VIDEO_CodingVP8;
			break;
		case VOOSMP_VIDEO_CodingVP7:
			dstCodec = VOMP_VIDEO_CodingVP7;
			break;
		case VOOSMP_VIDEO_CodingVC1:
			dstCodec = VOMP_VIDEO_CodingVC1;
			break;
		}
	}

	return dstCodec;
}


void COSVomePlayer::ConvertBufOS2MP(VOOSMP_BUFFERTYPE *sBuf, VOMP_BUFFERTYPE *dBuf, int nSSType)
{
	dBuf->nSize		= sBuf->nSize;
	dBuf->pBuffer	= sBuf->pBuffer;
	dBuf->llTime	= sBuf->llTime;
	dBuf->pData		= sBuf->pData;
	dBuf->nReserve  = (int)sBuf->llReserve;

	dBuf->nFlag = 0;

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_KEYFRAME) == VOOSMP_FLAG_BUFFER_KEYFRAME)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_KEYFRAME;
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_NEW_PROGRAM) == VOOSMP_FLAG_BUFFER_NEW_PROGRAM)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_NEW_PROGRAM;

		if(sBuf->pData)
		{
			VOOSMP_BUFFER_FORMAT *pOSTackInfo = (VOOSMP_BUFFER_FORMAT *)sBuf->pData;
			if(nSSType == VOOSMP_SS_AUDIO)
			{
				m_sAudioTrackInfo.nTrackType = VOMP_SS_Audio;
				m_sAudioTrackInfo.nCodec = ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sAudioTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sAudioTrackInfo.sAudioInfo.SampleRate = pOSTackInfo->sFormat.audio.SampleRate;
				m_sAudioTrackInfo.sAudioInfo.Channels = pOSTackInfo->sFormat.audio.Channels;  
				m_sAudioTrackInfo.sAudioInfo.SampleBits =  pOSTackInfo->sFormat.audio.SampleBits;

				m_sAudioTrackInfo.nHeadSize = pOSTackInfo->nHeadDataLen;
				m_sAudioTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sAudioTrackInfo;
			}
			else if(nSSType == VOOSMP_SS_VIDEO)
			{
				m_sVideoTrackInfo.nTrackType = VOMP_SS_Video;
				m_sVideoTrackInfo.nCodec = ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sVideoTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sVideoTrackInfo.sVideoInfo.Width = pOSTackInfo->sFormat.video.Width;
				m_sVideoTrackInfo.sVideoInfo.Height = pOSTackInfo->sFormat.video.Height;  
				m_sVideoTrackInfo.sVideoInfo.Type =  pOSTackInfo->sFormat.video.Type;

				m_sVideoTrackInfo.nHeadSize = pOSTackInfo->nHeadDataLen;
				m_sVideoTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sVideoTrackInfo;
			}			
		}
		else
		{
			dBuf->pData = NULL;
		}
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_NEW_FORMAT) == VOOSMP_FLAG_BUFFER_NEW_FORMAT)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_NEW_FORMAT;
		
		// added by gtxia 
		if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_ADD_DRMDATA) == VOOSMP_FLAG_BUFFER_ADD_DRMDATA)
		{
			//VOLOGE("HUI_FA  sBuf->nFlag & VOOSMP_FLAG_BUFFER_ADD_DRMDATA\n");
			dBuf->nFlag |= VOMP_FLAG_BUFFER_ADD_DRMDATA; 
		}

		if(sBuf->pData)
		{
			VOOSMP_BUFFER_FORMAT *pOSTackInfo = (VOOSMP_BUFFER_FORMAT *)sBuf->pData;
			if(nSSType == VOOSMP_SS_AUDIO)
			{
				m_sAudioTrackInfo.nTrackType = VOMP_SS_Audio;
				m_sAudioTrackInfo.nCodec = ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sAudioTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sAudioTrackInfo.sAudioInfo.SampleRate = pOSTackInfo->sFormat.audio.SampleRate;
				m_sAudioTrackInfo.sAudioInfo.Channels = pOSTackInfo->sFormat.audio.Channels;  
				m_sAudioTrackInfo.sAudioInfo.SampleBits =  pOSTackInfo->sFormat.audio.SampleBits;

				m_sAudioTrackInfo.nHeadSize = pOSTackInfo->nHeadDataLen;
				m_sAudioTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sAudioTrackInfo;
			}
			else if(nSSType == VOOSMP_SS_VIDEO)
			{
				m_sVideoTrackInfo.nTrackType = VOMP_SS_Video;
				m_sVideoTrackInfo.nCodec = ConvertCodecOS2MP(pOSTackInfo->nCodec, nSSType);				
				m_sVideoTrackInfo.nFourCC = pOSTackInfo->nFourCC;
				m_sVideoTrackInfo.sVideoInfo.Width = pOSTackInfo->sFormat.video.Width;
				m_sVideoTrackInfo.sVideoInfo.Height = pOSTackInfo->sFormat.video.Height;  
				m_sVideoTrackInfo.sVideoInfo.Type =  pOSTackInfo->sFormat.video.Type;

				m_sVideoTrackInfo.nHeadSize = pOSTackInfo->nHeadDataLen;
				m_sVideoTrackInfo.pHeadData = pOSTackInfo->pHeadData;

				dBuf->pData = &m_sVideoTrackInfo;
			}			
		}
		else
		{
			dBuf->pData = NULL;
		}
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_HEADDATA) == VOOSMP_FLAG_BUFFER_HEADDATA)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_HEADDATA;
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_DROP_FRAME) == VOOSMP_FLAG_BUFFER_DROP_FRAME)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_DROP_FRAME;
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_DELAY_TO_DROP) == VOOSMP_FLAG_BUFFER_DELAY_TO_DROP)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_DELAY_TO_DROP;
	}

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_TIMESTAMP_RESET) == VOOSMP_FLAG_BUFFER_TIMESTAMP_RESET)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_TIME_RESET;
	}	

	if((sBuf->nFlag & VOOSMP_FLAG_BUFFER_FRAME_DECODE_ONLY) == VOOSMP_FLAG_BUFFER_FRAME_DECODE_ONLY)
	{
		dBuf->nFlag |= VOMP_FLAG_BUFFER_FRAME_DECODE_ONLY;
	}
}

int COSVomePlayer::ConvertCodecMP2OS(int sCodec, int nSSType)
{
	int dstCodec = 0;

	if(nSSType == VOMP_SS_Audio)
	{
		switch(sCodec)
		{
		case VOMP_AUDIO_CodingUnused:
			dstCodec = VOOSMP_AUDIO_CodingUnused;
			break;
		case VOMP_AUDIO_CodingPCM:
			dstCodec = VOOSMP_AUDIO_CodingPCM;
			break;
		case VOMP_AUDIO_CodingADPCM:
			dstCodec = VOOSMP_AUDIO_CodingADPCM;
			break;
		case VOMP_AUDIO_CodingAMRNB:
			dstCodec = VOOSMP_AUDIO_CodingAMRNB;
			break;
		case VOMP_AUDIO_CodingAMRWB:
			dstCodec = VOOSMP_AUDIO_CodingAMRWB;
			break;
		case VOMP_AUDIO_CodingAMRWBP:
			dstCodec = VOOSMP_AUDIO_CodingAMRWBP;
			break;
		case VOMP_AUDIO_CodingQCELP13:
			dstCodec = VOOSMP_AUDIO_CodingQCELP13;
			break;
		case VOMP_AUDIO_CodingEVRC:
			dstCodec = VOOSMP_AUDIO_CodingEVRC;
			break;
		case VOMP_AUDIO_CodingAAC:  
			dstCodec = VOOSMP_AUDIO_CodingAAC;
			break;
		case VOMP_AUDIO_CodingAC3:
			dstCodec = VOOSMP_AUDIO_CodingAC3;
			break;
		case VOMP_AUDIO_CodingFLAC:
			dstCodec = VOOSMP_AUDIO_CodingFLAC;
			break;
		case VOMP_AUDIO_CodingMP1:
			dstCodec = VOOSMP_AUDIO_CodingMP1;
			break;
		case VOMP_AUDIO_CodingMP3:
			dstCodec = VOOSMP_AUDIO_CodingMP3;
			break;
		case VOMP_AUDIO_CodingOGG:
			dstCodec = VOOSMP_AUDIO_CodingOGG;
			break;
		case VOMP_AUDIO_CodingWMA:
			dstCodec = VOOSMP_AUDIO_CodingWMA;
			break;
		case VOMP_AUDIO_CodingRA:
			dstCodec = VOOSMP_AUDIO_CodingRA;
			break;
		case VOMP_AUDIO_CodingMIDI:
			dstCodec = VOOSMP_AUDIO_CodingMIDI;
			break;
		case VOMP_AUDIO_CodingDRA:
			dstCodec = VOOSMP_AUDIO_CodingDRA;
			break;
		case VOMP_AUDIO_CodingG729:
			dstCodec = VOOSMP_AUDIO_CodingG729;
			break;
		case VOMP_AUDIO_CodingEAC3:
			dstCodec = VOOSMP_AUDIO_CodingEAC3;
			break;
		case VOMP_AUDIO_CodingAPE:
			dstCodec = VOOSMP_AUDIO_CodingAPE;
			break;
		case VOMP_AUDIO_CodingALAC:
			dstCodec = VOOSMP_AUDIO_CodingALAC;
			break;
		case VOMP_AUDIO_CodingDTS:
			dstCodec = VOOSMP_AUDIO_CodingDTS;
			break;
		}
	}
	else if(nSSType == VOMP_SS_Video)
	{
		switch(sCodec)
		{
		case VOMP_VIDEO_CodingUnused:
			dstCodec = VOOSMP_VIDEO_CodingUnused;
			break;
		case VOMP_VIDEO_CodingMPEG2:
			dstCodec = VOOSMP_VIDEO_CodingMPEG2;
			break;
		case VOMP_VIDEO_CodingH263:
			dstCodec = VOOSMP_VIDEO_CodingH263;
			break;
		case VOMP_VIDEO_CodingS263:
			dstCodec = VOOSMP_VIDEO_CodingS263;
			break;
		case VOMP_VIDEO_CodingMPEG4:
			dstCodec = VOOSMP_VIDEO_CodingMPEG4;
			break;
		case VOMP_VIDEO_CodingH264:
			dstCodec = VOOSMP_VIDEO_CodingH264;
			break;
		case VOMP_VIDEO_CodingWMV:
			dstCodec = VOOSMP_VIDEO_CodingWMV;
			break;
		case VOMP_VIDEO_CodingRV:
			dstCodec = VOOSMP_VIDEO_CodingRV;
			break;
		case VOMP_VIDEO_CodingMJPEG:
			dstCodec = VOOSMP_VIDEO_CodingMJPEG;
			break;
		case VOMP_VIDEO_CodingDIVX:
			dstCodec = VOOSMP_VIDEO_CodingDIVX;
			break;
		case VOMP_VIDEO_CodingVP6:
			dstCodec = VOOSMP_VIDEO_CodingVP6;
			break;
		case VOMP_VIDEO_CodingVP8:
			dstCodec = VOOSMP_VIDEO_CodingVP8;
			break;
		case VOMP_VIDEO_CodingVP7:
			dstCodec = VOOSMP_VIDEO_CodingVP7;
			break;
		case VOMP_VIDEO_CodingVC1:
			dstCodec = VOOSMP_VIDEO_CodingVC1;
			break;
		}
	}

	return dstCodec;
}


void COSVomePlayer::ConvertBufMP2OS(VOMP_BUFFERTYPE *sBuf, VOOSMP_BUFFERTYPE *dBuf, int nSSType)
{
	dBuf->nSize		= sBuf->nSize;
	dBuf->pBuffer	= sBuf->pBuffer;
	dBuf->llTime	= sBuf->llTime;
	dBuf->pData		= sBuf->pData;
	dBuf->llReserve = sBuf->nReserve;

	dBuf->nFlag		= 0;

	if((sBuf->nFlag & VOMP_FLAG_BUFFER_KEYFRAME) == VOMP_FLAG_BUFFER_KEYFRAME)
	{
		dBuf->nFlag |= VOOSMP_FLAG_BUFFER_KEYFRAME;
	}

	if((sBuf->nFlag & VOMP_FLAG_BUFFER_NEW_PROGRAM) == VOMP_FLAG_BUFFER_NEW_PROGRAM)
	{
		dBuf->nFlag |= VOOSMP_FLAG_BUFFER_NEW_PROGRAM;
		dBuf->pData = (void *)ConvertCodecMP2OS((int)sBuf->pData, nSSType);		
	}

	if((sBuf->nFlag & VOMP_FLAG_BUFFER_NEW_FORMAT) == VOMP_FLAG_BUFFER_NEW_FORMAT)
	{
		dBuf->nFlag |= VOOSMP_FLAG_BUFFER_NEW_FORMAT;
		dBuf->pData = (void *)ConvertCodecMP2OS((int)sBuf->pData, nSSType);
	}

	if((sBuf->nFlag & VOMP_FLAG_BUFFER_HEADDATA) == VOMP_FLAG_BUFFER_HEADDATA)
	{
		dBuf->nFlag |= VOOSMP_FLAG_BUFFER_HEADDATA;
	}

	if((sBuf->nFlag & VOMP_FLAG_BUFFER_DROP_FRAME) == VOMP_FLAG_BUFFER_DROP_FRAME)
	{
		dBuf->nFlag |= VOOSMP_FLAG_BUFFER_DROP_FRAME;
	}
}

int COSVomePlayer::ConvertColorOS2MP(VOOSMP_COLORTYPE *sColor)
{
	int srcColor = *sColor;
	int dstColor = 0;

	switch(srcColor)
	{
	case VOOSMP_COLOR_YUV_PLANAR444:
		dstColor = VOMP_COLOR_YUV_PLANAR444;
		break;
	case VOOSMP_COLOR_YUV_PLANAR422_12:
		dstColor = VOMP_COLOR_YUV_PLANAR422_12;
		break;
	case VOOSMP_COLOR_YUV_PLANAR422_21:
		dstColor = VOMP_COLOR_YUV_PLANAR422_21;
		break;
	case VOOSMP_COLOR_YUV_PLANAR420:
		dstColor = VOMP_COLOR_YUV_PLANAR420;
		break;
	case VOOSMP_COLOR_YUV_PLANAR411:
		dstColor = VOMP_COLOR_YUV_PLANAR411;
		break;
	case VOOSMP_COLOR_YUV_PLANAR411V:
		dstColor = VOMP_COLOR_YUV_PLANAR411V;
		break;
	case VOOSMP_COLOR_GRAY_PLANARGRAY:
		dstColor = VOMP_COLOR_GRAY_PLANARGRAY;
		break;
	case VOOSMP_COLOR_YUYV422_PACKED:
		dstColor = VOMP_COLOR_YUYV422_PACKED;
		break;
	case VOOSMP_COLOR_YVYU422_PACKED:
		dstColor = VOMP_COLOR_YVYU422_PACKED;
		break;
	case VOOSMP_COLOR_UYVY422_PACKED:
		dstColor = VOMP_COLOR_UYVY422_PACKED;
		break;
	case VOOSMP_COLOR_VYUY422_PACKED:
		dstColor = VOMP_COLOR_VYUY422_PACKED;
		break;
	case VOOSMP_COLOR_YUV444_PACKED:
		dstColor = VOMP_COLOR_YUV444_PACKED;
		break;
	case VOOSMP_COLOR_YUV_420_PACK:
		dstColor = VOMP_COLOR_YUV_420_PACK;
		break;
	case VOOSMP_COLOR_YUV_420_PACK_2:
		dstColor = VOMP_COLOR_YUV_420_PACK_2;
		break;
	case VOOSMP_COLOR_YVU_PLANAR420:
		dstColor = VOMP_COLOR_YVU_PLANAR420;
		break;
	case VOOSMP_COLOR_YVU_PLANAR422_12:
		dstColor = VOMP_COLOR_YVU_PLANAR422_12;
		break;
	case VOOSMP_COLOR_YUYV422_PACKED_2:
		dstColor = VOMP_COLOR_YUYV422_PACKED_2;
		break;
	case VOOSMP_COLOR_YVYU422_PACKED_2:
		dstColor = VOMP_COLOR_YVYU422_PACKED_2;
		break;
	case VOOSMP_COLOR_UYVY422_PACKED_2:
		dstColor = VOMP_COLOR_UYVY422_PACKED_2;
		break;
	case VOOSMP_COLOR_VYUY422_PACKED_2:
		dstColor = VOMP_COLOR_VYUY422_PACKED_2;
		break;
	case VOOSMP_COLOR_RGB565_PACKED:
		dstColor = VOMP_COLOR_RGB565_PACKED;
		break;
	case VOOSMP_COLOR_RGB555_PACKED:
		dstColor = VOMP_COLOR_RGB555_PACKED;
		break;
	case VOOSMP_COLOR_RGB888_PACKED:
		dstColor = VOMP_COLOR_RGB888_PACKED;
		break;
	case VOOSMP_COLOR_RGB32_PACKED:
		dstColor = VOMP_COLOR_RGB32_PACKED;
		break;
	case VOOSMP_COLOR_RGB888_PLANAR:
		dstColor = VOMP_COLOR_RGB888_PLANAR;
		break;
	case VOOSMP_COLOR_YUV_PLANAR420_NV12:
		dstColor = VOMP_COLOR_YUV_PLANAR420_NV12;
		break;
	case VOOSMP_COLOR_ARGB32_PACKED:
		dstColor = VOMP_COLOR_ARGB32_PACKED;
		break;
	}

	return dstColor;
}
