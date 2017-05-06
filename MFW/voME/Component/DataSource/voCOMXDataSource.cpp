	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataSource.cpp

	Contains:	voCOMXDataSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voCOMXDataSource.h"

#define LOG_TAG "voCOMXDataSource"
#include "voLog.h"

extern VO_TCHAR * g_pvoOneWorkingPath;

voCOMXDataSource::voCOMXDataSource(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSource (pComponent)
	, m_pAudioPort (NULL)
	, m_pVideoPort (NULL)
	, m_pClockPort (NULL)
	, m_pLiveSource (NULL)
	, m_bCancelSendBuffer (VO_FALSE)
	, m_nLiveSourceDuration (-1)
	, m_nLiveSourceTracks (0)
	, m_nLiveSourceInitializedTracks (0)
	, m_llSeekPos(-1)
	, m_bBlockPortSettingsChangedEvent (OMX_FALSE)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.DataSource");

	OMX_S32 nIndex = 0;
	m_uPorts = 3;
	m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
	if (m_ppPorts != NULL)
	{
		m_ppPorts[nIndex] = new voCOMXDataAudioPort (this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pAudioPort = (voCOMXDataAudioPort *) m_ppPorts[nIndex];

		m_portParam[OMX_PortDomainAudio].nPorts = 1;
		m_portParam[OMX_PortDomainAudio].nStartPortNumber = nIndex;
		nIndex++;

		m_ppPorts[nIndex] = new voCOMXDataVideoPort (this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pVideoPort = (voCOMXDataVideoPort *) m_ppPorts[nIndex];

		m_portParam[OMX_PortDomainVideo].nPorts = 1;
		m_portParam[OMX_PortDomainVideo].nStartPortNumber = nIndex;
		nIndex++;
	}

	m_ppPorts[nIndex] = new voCOMXPortClock (this, nIndex, OMX_DirInput);
	m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
	m_pClockPort = (voCOMXPortClock *) m_ppPorts[nIndex];
	m_portParam[OMX_PortDomainOther].nPorts = 1;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = nIndex;

	m_pVideoPort->SetClockPort (m_pClockPort);
	m_pVideoPort->SetOtherPort (m_pAudioPort);
	m_pClockPort->SetClockType(OMX_TIME_RefClockAudio);
	m_pAudioPort->SetOtherPort (m_pVideoPort);
	m_pAudioPort->SetClockPort (m_pClockPort);

	VO_TCHAR szCfgFile[256];
	if (g_pvoOneWorkingPath == NULL)
		vostrcpy (szCfgFile, _T("vomeplay.cfg"));
	else
	{
		vostrcpy (szCfgFile, g_pvoOneWorkingPath);
		vostrcat (szCfgFile, _T("vomeplay.cfg"));
	}
	m_pCfgComponent = new CBaseConfig ();
	m_pCfgComponent->Open (szCfgFile);

	m_nDumpRuntimeLog = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpRuntimeLog", 0);

	m_pVideoPort->SetConfigFile (m_pCfgComponent);
	m_pAudioPort->SetConfigFile (m_pCfgComponent);
}

voCOMXDataSource::~voCOMXDataSource()
{
	if(m_pLiveSource)
		delete m_pLiveSource;
}
 
OMX_ERRORTYPE voCOMXDataSource::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	switch (nParamIndex)
	{
	case OMX_IndexParamNumAvailableStreams:
		{
			OMX_PARAM_U32TYPE * pU32Type = (OMX_PARAM_U32TYPE *)pComponentParameterStructure;
			if (pU32Type->nPortIndex >= m_uPorts)
				return OMX_ErrorBadPortIndex;

			pU32Type->nU32 = 1;
		}
		break;

	case OMX_IndexParamActiveStream:
		{
			OMX_PARAM_U32TYPE * pU32Type = (OMX_PARAM_U32TYPE *)pComponentParameterStructure;
			if (pU32Type->nPortIndex >= m_uPorts)
				return OMX_ErrorBadPortIndex;

			return errType;

		}
		break;

	default:
		errType = voCOMXCompSource::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXDataSource::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	switch (nIndex)
	{
	case OMX_IndexParamContentURI:
		{
			OMX_PARAM_CONTENTURITYPE* pContent = (OMX_PARAM_CONTENTURITYPE*)pComponentParameterStructure;
			errType = voOMXBase_CheckHeader(pContent, pContent->nSize);
// 			if (errType != OMX_ErrorNone)
// 				return errType;

			VO_CHAR szFile[1024];
			strcpy(szFile, (VO_PCHAR)pContent->contentURI);
			VO_U32 nLen = strlen(szFile);
			VO_PCHAR pExt = szFile + nLen - 1;
			while(*pExt != '.')
				pExt--;
			pExt++;

			VO_PCHAR pChar = pExt;
			VO_U32 nChars = strlen(pExt);
			for(VO_U32 i = 0; i < nChars; i++)
			{
				if((*pChar) <= 'z' && (*pChar) >= 'a')
					*pChar -= ('a' - 'A');
				pChar++;
			}

			if(strcmp(pExt, "M3U8"))
				return OMX_ErrorBadParameter;

			return OpenLiveSource(VO_LIVESRC_FT_HTTP_LS, (OMX_STRING)pContent->contentURI);
		}
		break;

	case OMX_VO_IndexSendBuffer:
		{
			voCOMXAutoLock lock (&m_tmStatus);

			OMX_BUFFERHEADERTYPE * pBuffer = (OMX_BUFFERHEADERTYPE *)pComponentParameterStructure;

			if (pBuffer->nOutputPortIndex == 0)
			{
				errType = m_pAudioPort->SendBuffer (pBuffer);
			}
			else if (pBuffer->nOutputPortIndex == 1)
			{
				errType = m_pVideoPort->SendBuffer (pBuffer);
			}

			return errType;
		}
		break;

	case OMX_VO_IndexStreamingType:
		{
			OMX_VO_StreamType *pStreamType = (OMX_VO_StreamType *) pComponentParameterStructure;

			return OpenLiveSource(pStreamType->nStreamingType, pStreamType->strURL);
		}
		break;

	case OMX_VO_IndexBlockPortSettingsChangedEvent:
		{
			m_bBlockPortSettingsChangedEvent = *((OMX_BOOL *)pComponentParameterStructure);
			VOLOGI ("m_bBlockPortSettingsChangedEvent %d", m_bBlockPortSettingsChangedEvent);
			return OMX_ErrorNone;
		}

	default:
		errType = voCOMXCompSource::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXDataSource::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	switch (nIndex)
	{
	case OMX_IndexConfigTimePosition:
		{
			if(m_pLiveSource == NULL)
				return OMX_ErrorUnsupportedIndex;

			OMX_TIME_CONFIG_TIMESTAMPTYPE * pTime = (OMX_TIME_CONFIG_TIMESTAMPTYPE *)pComponentConfigStructure;
			VO_S64 nNewPos = pTime->nTimestamp;
			// call Live Source Seek so that LiveSource will start sending data from special position
			VO_U32 nRC = m_pLiveSource->Seek(nNewPos);
			if(nRC != VO_ERR_NONE)
			{
				VOLOGW ("Live Source Seek To %d return 0x%08X", (int)nNewPos, (int)nRC);
				return OMX_ErrorUndefined;
			}
			VOLOGI ("Live Source Seek To %d OK VideoPort 0x%08X, AudioPort 0x%08X", (int)nNewPos, (int)m_pVideoPort, (int)m_pAudioPort);
			if(m_pVideoPort)
				m_pVideoPort->SetBufferFull(OMX_FALSE);
			if(m_pAudioPort)
				m_pAudioPort->SetBufferFull(OMX_FALSE);

			m_pLiveSource->Start();
			OMX_ERRORTYPE errType = OMX_ErrorNone;
			if(m_pVideoPort)
			{
				// check video data for key frame
				errType = m_pVideoPort->GetSeekKeyFrame(&nNewPos);
				if(errType != OMX_ErrorNone)
				{
					VOLOGW ("VideoPort GetSeekKeyFrame return 0x%08X", (int)errType);
					m_pLiveSource->Pause();
					return errType;
				}
				VOLOGI ("VideoPort GetSeekKeyFrame new Position %d", (int)nNewPos);
			}
			m_pLiveSource->Pause();

			if(m_pVideoPort)
				m_pVideoPort->SetSeekPos(nNewPos);
			if(m_pAudioPort)
				m_pAudioPort->SetSeekPos(nNewPos);
			if (m_pClockPort != NULL)
				m_pClockPort->UpdateMediaTime (nNewPos);
			m_llSeekPos = nNewPos;
			return OMX_ErrorNone;
		}
		break;
	default:
		break;
	}

	return voCOMXCompSource::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXDataSource::SetNewState (OMX_STATETYPE newState)
{
	if(m_pLiveSource)
	{
		if (newState == OMX_StateExecuting && (m_sTrans == COMP_TRANSSTATE_IdleToExecute || m_sTrans == COMP_TRANSSTATE_PauseToExecute))
		{
			m_bCancelSendBuffer = VO_FALSE;
			m_pLiveSource->Start();
		}
		else if (newState == OMX_StatePause && (m_sTrans == COMP_TRANSSTATE_ExecuteToPause))
			m_pLiveSource->Pause();
		else if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
		{
			m_bCancelSendBuffer = VO_TRUE;
			m_pLiveSource->Stop();
		}
	}

//	m_pClockPort->AudioStreamStop (1);
	OMX_COMPONENTTYPE* pComp = m_pClockPort->GetTunnelComp();
	if(pComp)
	{
		OMX_U32 nSourceType = 1;
		pComp->SetConfig(pComp, (OMX_INDEXTYPE)OMX_VO_IndexSourceType, &nSourceType);
	}

	return voCOMXCompSource::SetNewState (newState);
}

VO_VOID voCOMXDataSource::LiveSourceStatusCallback(VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	voCOMXDataSource* pThis = (voCOMXDataSource*)pUserData;
	pThis->OnLiveSourceStatus(nID, nParam1, nParam2);
}

VO_VOID voCOMXDataSource::LiveSourceDataCallback(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData)
{
	voCOMXDataSource* pThis = (voCOMXDataSource*)pUserData;
	pThis->OnLiveSourceData(nOutputType, pData);
}

VO_VOID voCOMXDataSource::OnLiveSourceStatus(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	// we will not use it at all.
	VOLOGR ("nID 0x%08X, nParam1 0x%08X, nParam2 0x%08X", nID, nParam1, nParam2);
}

VO_VOID voCOMXDataSource::OnLiveSourceData(VO_U16 nOutputType, VO_PTR pData)
{
	VO_LIVESRC_SAMPLE* pSample = (VO_LIVESRC_SAMPLE*)pData;

	if(m_nDumpRuntimeLog)
	{
		VOLOGI ("nOutputType %d, pSample 0x%08X, Time %d, Flag 0x%08X, Size %d, Buffer 0x%08X", 
			(int)nOutputType,(int)pSample, (int)pSample->Sample.Time, (int)pSample->Sample.Flag, (int)pSample->Sample.Size, (int)pSample->Sample.Buffer);
	}

	/*
	if((pSample->Sample.Flag & OMX_BUFFERFLAG_EXTRADATA) && ((pSample->Sample.Flag & OMX_VO_BUFFERFLAG_NEWSTREAM) == OMX_VO_BUFFERFLAG_NEWSTREAM))
	{
		VO_LIVESRC_TRACK_INFOEX * pInfoEx = (VO_LIVESRC_TRACK_INFOEX *)pSample->Sample.Buffer;
		if(nOutputType == 1)	// audio data
		{
			VOLOGI ("OMX_VO_BUFFERFLAG_NEWSTREAM Codec %d, Channel %d, SampleBits %d, SampleRate %d", 
				pInfoEx->Codec, pInfoEx->audio_info.Channels, pInfoEx->audio_info.SampleBits, pInfoEx->audio_info.SampleRate);
		}
		else if(nOutputType == 2)	// video data
		{
			VOLOGI ("OMX_VO_BUFFERFLAG_NEWSTREAM Codec %d, Width %d, Height %d", pInfoEx->Codec, pInfoEx->video_info.Width, pInfoEx->video_info.Height);
		}
	}
	*/

	if(m_nLiveSourceInitializedTracks < m_nLiveSourceTracks)
	{
		if((pSample->Sample.Flag & OMX_BUFFERFLAG_EXTRADATA) && ((pSample->Sample.Flag & OMX_VO_BUFFERFLAG_NEWSTREAM) == OMX_VO_BUFFERFLAG_NEWSTREAM))
		{
			if(nOutputType == 1)	// audio data
			{
				if(!m_pAudioPort)
				{
					// audio should be first port always
					CreatePort(OMX_PortDomainAudio, 0);

					VO_LIVESRC_TRACK_INFOEX * pInfoEx = (VO_LIVESRC_TRACK_INFOEX *)pSample->Sample.Buffer;

					VO_SOURCE_TRACKINFO trkInfo;
					memset(&trkInfo, 0, sizeof(trkInfo));
					trkInfo.Type = pInfoEx->Type;
					trkInfo.Codec = pInfoEx->Codec;
					trkInfo.Start = 0;
					trkInfo.Duration = m_nLiveSourceDuration;
					trkInfo.HeadData = pInfoEx->Padding;
					trkInfo.HeadSize = pInfoEx->HeadSize;

					m_pAudioPort->SetTrackInfo(&trkInfo);
					OMX_PARAM_PORTDEFINITIONTYPE *pType;
					m_pAudioPort->GetPortType(&pType);
					if(pInfoEx->Codec == VO_AUDIO_CodingAAC)
						pType->format.audio.eEncoding = OMX_AUDIO_CodingAAC;
					if(pInfoEx->Codec == VO_AUDIO_CodingAMRNB)
						pType->format.audio.eEncoding = OMX_AUDIO_CodingAMR;
					if(pInfoEx->Codec == VO_AUDIO_CodingWMA)
						pType->format.audio.eEncoding = OMX_AUDIO_CodingWMA;
					if(pInfoEx->Codec == VO_AUDIO_CodingMP3)
						pType->format.audio.eEncoding = OMX_AUDIO_CodingMP3;

					OMX_AUDIO_PARAM_PCMMODETYPE *pPCMType = NULL;
					m_pAudioPort->GetPCMType(&pPCMType);
					if(pPCMType)
					{
						pPCMType->nChannels = pInfoEx->audio_info.Channels;
						pPCMType->nBitPerSample = pInfoEx->audio_info.SampleBits;
						pPCMType->nSamplingRate = pInfoEx->audio_info.SampleRate;
					}

					m_nLiveSourceInitializedTracks++;

					if(m_nLiveSourceInitializedTracks == m_nLiveSourceTracks)
						CreateClockPort();
					// we will not use this data, so drop it.
					return;
				}
			}
			else if(nOutputType == 2)	// video data
			{
				if(!m_pVideoPort)
				{
					// just video only can be first port
					CreatePort(OMX_PortDomainVideo, (m_nLiveSourceTracks > 1) ? 1 : 0);

					VO_LIVESRC_TRACK_INFOEX * pInfoEx = (VO_LIVESRC_TRACK_INFOEX *)pSample->Sample.Buffer;

					VO_SOURCE_TRACKINFO trkInfo;
					memset(&trkInfo, 0, sizeof(trkInfo));
					trkInfo.Type = pInfoEx->Type;
					trkInfo.Codec = pInfoEx->Codec;
					trkInfo.Start = 0;
					trkInfo.Duration = m_nLiveSourceDuration;
					trkInfo.HeadData = pInfoEx->Padding;
					trkInfo.HeadSize = pInfoEx->HeadSize;

					//for h/w decoder, need to add sequence header into buffer list
					if(trkInfo.HeadSize>0)
					{
						OMX_BUFFERHEADERTYPE sBufferHeader;
						voOMXBase_SetHeader(&sBufferHeader, sizeof(OMX_BUFFERHEADERTYPE));
						sBufferHeader.nTimeStamp = 0;
						sBufferHeader.nFlags = OMX_BUFFERFLAG_CODECCONFIG;
						sBufferHeader.nFilledLen = trkInfo.HeadSize;
						sBufferHeader.pBuffer = trkInfo.HeadData;

						m_pVideoPort->SendBuffer(&sBufferHeader);
					}

					m_pVideoPort->SetTrackInfo(&trkInfo);
					OMX_PARAM_PORTDEFINITIONTYPE *pType;
					m_pVideoPort->GetPortType(&pType);
					if(pInfoEx->Codec == VO_VIDEO_CodingH264)
						pType->format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
					else if(pInfoEx->Codec == VO_VIDEO_CodingH263)
						pType->format.video.eCompressionFormat = OMX_VIDEO_CodingH263;
					else if(pInfoEx->Codec == VO_VIDEO_CodingMPEG4)
						pType->format.video.eCompressionFormat = OMX_VIDEO_CodingMPEG4;
					else if(pInfoEx->Codec == VO_VIDEO_CodingWMV||pInfoEx->Codec == VO_VIDEO_CodingVC1)
						pType->format.video.eCompressionFormat = OMX_VIDEO_CodingWMV;

					pType->format.video.nFrameWidth = pInfoEx->video_info.Width;
					pType->format.video.nFrameHeight = pInfoEx->video_info.Height;

					m_nLiveSourceInitializedTracks++;

					if(m_nLiveSourceInitializedTracks == m_nLiveSourceTracks)
						CreateClockPort();
					// we will not use this data, so drop it.
					return;			 
				}			 
			}
		}
	}

	OMX_BUFFERHEADERTYPE sBufferHeader;
	voOMXBase_SetHeader(&sBufferHeader, sizeof(OMX_BUFFERHEADERTYPE));
	sBufferHeader.nTimeStamp = pSample->Sample.Time;
	sBufferHeader.nFlags = pSample->Sample.Flag;
	sBufferHeader.nFilledLen = pSample->Sample.Size;
	sBufferHeader.pBuffer = pSample->Sample.Buffer;

	OMX_ERRORTYPE errType = OMX_ErrorUndefined;
	while(errType != OMX_ErrorNone)
	{
		if(nOutputType == 1)	// audio data
		{
			voCOMXAutoLock lock (&m_tmStatus);
			if(m_pAudioPort == NULL)
				break;
			if(sBufferHeader.nFlags & 0x100)
				errType = m_pAudioPort->FlushBuffer();
			else
			{
				if(m_llSeekPos != -1 && sBufferHeader.nTimeStamp < m_llSeekPos)
					return;
				else if(m_llSeekPos != -1 && sBufferHeader.nTimeStamp >= m_llSeekPos)
					m_llSeekPos = -1;
			
				errType = m_pAudioPort->SendBuffer(&sBufferHeader);
			}
		}
		else if(nOutputType == 2)	// video data
		{
			voCOMXAutoLock lock (&m_tmStatus);
			if(m_pVideoPort == NULL)
				break;
			if(sBufferHeader.nFlags & 0x100)
				errType = m_pVideoPort->FlushBuffer();
			else
				errType = m_pVideoPort->SendBuffer(&sBufferHeader);
		}

		// buffer is full, should retry
		if(errType != OMX_ErrorNone)
		{
			if(m_pVideoPort)
				m_pVideoPort->SetBufferFull(OMX_TRUE);
			if(m_pAudioPort)
				m_pAudioPort->SetBufferFull(OMX_TRUE);

			voOMXOS_Sleep(5);
		}

		// cancel retry
		if(m_bCancelSendBuffer)
			break;
	}
}

OMX_ERRORTYPE voCOMXDataSource::OpenLiveSource(OMX_U32 nStreamingType, OMX_STRING pUrl)
{
	// close previous session
	if(m_pLiveSource)
	{
		delete m_pLiveSource;
		m_pLiveSource = NULL;
	}
	m_nLiveSourceDuration = -1;
	m_nLiveSourceTracks = 0;
	m_nLiveSourceInitializedTracks = 0;
	ReleasePort();

	// create new session
	if(nStreamingType == VO_LIVESRC_FT_HTTP_LS || nStreamingType == VO_LIVESRC_FT_IIS_SS)
		m_pLiveSource = new CBaseLiveSource((VO_U16)nStreamingType);

	if(!m_pLiveSource)
	{
		VOLOGE ("Create CBaseLiveSource Fail");
		return OMX_ErrorInsufficientResources;
	}

	VO_U32 nRC = m_pLiveSource->Open(this, LiveSourceStatusCallback, LiveSourceDataCallback);

	if(m_pPlayReadyAPI)
	{
		VOLOGI ("Framework Set APIs directly, so we should implement callback in source component.");

		VO_SOURCEDRM_CALLBACK cbSourceDrm;
		cbSourceDrm.fCallback = voOMXSourceDrmCallBack;
		cbSourceDrm.pUserData = this;

		nRC = m_pLiveSource->SetParam(VO_PID_LIVESRC_DRMCALLBACK, &cbSourceDrm);
	}

	// open URL and get duration and track number
	nRC = m_pLiveSource->SetDataSource(pUrl , 0);
	if(VO_ERR_LIVESRC_OK != nRC)
	{
		VOLOGE ("Live Streaming Open %s return 0x%08X", pUrl, (int)nRC);
		return OMX_ErrorUndefined;
	}

	nRC = m_pLiveSource->GetParam(VO_PID_LIVESRC_DURATION, &m_nLiveSourceDuration);
	if(VO_ERR_LIVESRC_OK != nRC)
	{
		VOLOGW ("Live Streaming VO_PID_LIVESRC_DURATION %s return 0x%08X", pUrl, (int)nRC);
	}
	nRC = m_pLiveSource->GetParam(VO_PID_LIVESRC_TRACKNUMBER, &m_nLiveSourceTracks);
	if(VO_ERR_LIVESRC_OK != nRC)
	{
		VOLOGW ("Live Streaming VO_PID_LIVESRC_TRACKNUMBER %s return 0x%08X", pUrl, (int)nRC);
	}

	VOLOGI ("Live Streaming %s Duration %d Track Number %d", pUrl, (int)m_nLiveSourceDuration, (int)m_nLiveSourceTracks);

	// start to initialize tracks
	m_pLiveSource->Start();
	OMX_U32 nTryTimes = 0;
	while(m_nLiveSourceInitializedTracks < m_nLiveSourceTracks)
	{
		voOMXOS_Sleep(2);
		if((nTryTimes++) > 7500)	// wait for 15s
			break;
	}
	m_pLiveSource->Pause();

	if(m_nLiveSourceInitializedTracks < m_nLiveSourceTracks)
	{
		VOLOGE ("Can not finish initialized tracks %d %d", (int)m_nLiveSourceInitializedTracks, (int)m_nLiveSourceTracks);
		return OMX_ErrorUndefined;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE	voCOMXDataSource::ReleasePort (void)
{
	OMX_ERRORTYPE err = voCOMXCompSource::ReleasePort();
	m_pAudioPort = NULL;
	m_pVideoPort = NULL;
	m_pClockPort = NULL;

	m_portParam[OMX_PortDomainAudio].nPorts = 0;
	m_portParam[OMX_PortDomainVideo].nPorts = 0;
	m_portParam[OMX_PortDomainOther].nPorts = 0;
	m_portParam[OMX_PortDomainImage].nPorts = 0;

	return err;
}

OMX_ERRORTYPE voCOMXDataSource::CreatePort(OMX_PORTDOMAINTYPE eType, OMX_U32 nIndex)
{
	if(!m_ppPorts)
	{
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc(3 * sizeof(voCOMXBasePort *));
		voOMXMemSet(m_ppPorts, 0, 3 * sizeof(voCOMXBasePort *));
	}

	if(eType == OMX_PortDomainAudio)
	{
		if(m_pAudioPort)
		{
			VOLOGW("Audio Port already occur");
			return OMX_ErrorUndefined;
		}

		VOLOGI("Create Audio Port Index %d, Ports %d", (int)nIndex, (int)m_uPorts);

		m_ppPorts[nIndex] = new voCOMXDataAudioPort(this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pAudioPort = (voCOMXDataAudioPort *)m_ppPorts[nIndex];

		m_portParam[OMX_PortDomainAudio].nPorts = 1;
		m_portParam[OMX_PortDomainAudio].nStartPortNumber = nIndex;

		m_uPorts++;

	}
	else if(eType == OMX_PortDomainVideo)
	{
		if(m_pVideoPort)
		{
			VOLOGW("Video Port already occur");
			return OMX_ErrorUndefined;
		}

		VOLOGI("Create Video Port Index %d, Ports %d", (int)nIndex, (int)m_uPorts);

		m_ppPorts[nIndex] = new voCOMXDataVideoPort(this, nIndex);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pVideoPort = (voCOMXDataVideoPort *)m_ppPorts[nIndex];

		m_portParam[OMX_PortDomainVideo].nPorts = 1;
		m_portParam[OMX_PortDomainVideo].nStartPortNumber = nIndex;

		m_uPorts++;
	}
	else if(eType == OMX_PortDomainOther)
	{
		if(m_pClockPort)
		{
			VOLOGW("Clock Port already occur");
			return OMX_ErrorUndefined;
		}

		VOLOGI("Create Other Port Index %d, Ports %d", (int)nIndex, (int)m_uPorts);

		m_ppPorts[nIndex] = new voCOMXPortClock(this, nIndex, OMX_DirInput);
		m_ppPorts[nIndex]->SetCallbacks (m_pCallBack, m_pAppData);
		m_pClockPort = (voCOMXPortClock *)m_ppPorts[nIndex];

		m_portParam[OMX_PortDomainOther].nPorts = 1;
		m_portParam[OMX_PortDomainOther].nStartPortNumber = nIndex;

		m_uPorts++;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXDataSource::CreateClockPort()
{
	CreatePort(OMX_PortDomainOther, (m_nLiveSourceTracks > 1) ? 2 : 1);

	if(m_pVideoPort != NULL)
	{
		if(m_pClockPort != NULL)
			m_pVideoPort->SetClockPort (m_pClockPort);

		if(m_pAudioPort != NULL)
			m_pVideoPort->SetOtherPort (m_pAudioPort);
	}

	if(m_pAudioPort != NULL)
	{
		if(m_pClockPort != NULL)
		{
			m_pClockPort->SetClockType(OMX_TIME_RefClockAudio);
			m_pAudioPort->SetClockPort (m_pClockPort);
		}

		if(m_pVideoPort != NULL)
		{
			m_pAudioPort->SetOtherPort (m_pVideoPort);
		}
	}
	else if(m_pVideoPort != NULL)
	{
		m_pClockPort->SetClockType(OMX_TIME_RefClockVideo);
	}

	return OMX_ErrorNone;
}
