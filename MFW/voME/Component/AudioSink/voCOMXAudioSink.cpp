	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioSink.cpp

	Contains:	voCOMXAudioSink class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifdef _LINUX_ANDROID
#include <cutils/properties.h>
#endif // _LINUX_ANDROID

#include "voOMXOSFun.h"
#include "voOMXBase.h"
#include "voOMXFile.h"
#include "voOMXMemory.h"
#include "voCOMXAudioSink.h"

#define LOG_TAG "voCOMXAudioSink"
#include "voLog.h"

extern VO_TCHAR * g_pvoOneWorkingPath;

#define VO_AUDIOSINK_UNVALID_DELAY		10000	// 10s
voCOMXAudioSink::voCOMXAudioSink(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompSink (pComponent)
	, m_pClockPort (NULL)
	, m_nSampleRate (44100)
	, m_nChannels (2)
	, m_nBits (16)
	, m_bSetThreadPriority (OMX_FALSE)
	, m_nOffsetTime (0)
	, m_nCopyBuffer (1)
	, m_pDataBuffer (NULL)
	, m_nDataSize (0)
	, m_nDataFill (0)
	, m_nDataStep (0)
	, m_nDataTime (200)
	, m_llRenderBufferTime (0)
	, m_llRenderSystemTime (0)
	, m_nRenderAdjustTime (100)
	, m_nRenderDelay (VO_AUDIOSINK_UNVALID_DELAY)
	, m_pDumpInputFile (NULL)
	, m_pAudioRender (NULL)
	, m_nBufferTime (400)
	, m_bThread (OMX_TRUE)
	, m_bFixedSampleRate (OMX_FALSE)
	, m_bVideoFirstOutput (OMX_FALSE)
	, m_bSupportMultiChannel(OMX_FALSE)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Sink");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	m_cbRenderBuffer.pCallBack = NULL;
	m_cbRenderBuffer.pUserData = NULL;

	VO_TCHAR szCfgFile[256];
	if (g_pvoOneWorkingPath == NULL)
		vostrcpy (szCfgFile, _T("vomeplay.cfg"));
	else
	{
		vostrcpy (szCfgFile, g_pvoOneWorkingPath);
		vostrcat (szCfgFile, _T("vomeplay.cfg"));
	}
	m_pCfgVOME = new CBaseConfig ();
	if (m_pCfgVOME != NULL)
	{
		m_pCfgVOME->Open (szCfgFile);
		if (m_pCfgVOME->GetItemValue (m_pName, (char *)"BufferThread", 1) == 0)
			m_bThread = OMX_FALSE;
		m_nOffsetTime = m_pCfgVOME->GetItemValue (m_pName, (char *)"OffsetTime", 0);
		VOLOGI ("The offset time is %d", (int)m_nOffsetTime);
		m_nRenderAdjustTime = m_pCfgVOME->GetItemValue (m_pName, (char *)"AdjustTime", 100);
		m_nLogLevel = m_pCfgVOME->GetItemValue (m_pName, (char*)"ShowLogLevel", 0);
		m_nDataTime = m_pCfgVOME->GetItemValue (m_pName, (char*)"DataFrameTime", 200);
		m_nCopyBuffer = m_pCfgVOME->GetItemValue (m_pName, (char*)"DataCopyBuffer", 0);
		m_nDumpRuntimeLog = m_pCfgVOME->GetItemValue (m_pName, (char*)"DumpRuntimeLog", 0);

		char * pFile = m_pCfgVOME->GetItemText (m_pName, (char*)"DumpInputDataFile");
		if (pFile != NULL)
		{
#ifdef _WIN32
			TCHAR szFile[256];
			memset (szFile, 0, sizeof (szFile));
			MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
			m_pDumpInputFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
			m_pDumpInputFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
			VOLOGI ("Create Input file %s, The handle is 0X%08X.", pFile, (int)m_pDumpInputFile);
		}
	}

#ifdef _LINUX_ANDROID
	if (m_pCfgVOME != NULL)
	{
		char	szPropName[256];
		char	szPropProduct[256];
		char	szPropModel[256];
		char	szPropText[1024];

		memset (szPropName, 0, sizeof (szPropName));
		property_get("ro.product.manufacturer", szPropName, "VisualOn");

		memset (szPropProduct, 0, sizeof (szPropProduct));
		property_get("ro.product.name", szPropProduct, "VisualOn");

		memset (szPropModel, 0, sizeof (szPropModel));
		property_get("ro.product.model", szPropModel, "VisualOn");

		sprintf (szPropText, "%s_%s_%s", szPropName, szPropProduct, szPropModel);
		m_nOffsetTime = m_pCfgVOME->GetItemValue (szPropText, (char *)"AudioSinkOffsetTime", m_nOffsetTime);
		VOLOGI ("The device prop is %s. Offset %d", szPropText, (int)m_nOffsetTime);
	}
#endif // _LINUX_ANDROID

#ifdef _WIN32
	m_pARTimeStamp = (OMX_S32*)voOMXMemAlloc(sizeof(OMX_S32));
#endif
}

voCOMXAudioSink::~voCOMXAudioSink(void)
{
	if (m_pDumpInputFile != NULL)
		voOMXFileClose (m_pDumpInputFile);

	if (m_pAudioRender != NULL)
	{
		m_pAudioRender->Stop ();
		delete m_pAudioRender;
	}

	delete m_pCfgVOME;

	if (m_pDataBuffer != NULL)
		voOMXMemFree (m_pDataBuffer);

//	if (m_pCallBack != NULL)
//		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);

#ifdef _WIN32
	voOMXMemFree(m_pARTimeStamp);
#endif
}

OMX_ERRORTYPE voCOMXAudioSink::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nParamIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if (pAudioFormat->nPortIndex == 0)
			{
				pAudioFormat->eEncoding = OMX_AUDIO_CodingPCM;
				errType = OMX_ErrorNone;
			}
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
			if (pPCMFormat->nPortIndex == 0)
			{
				voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
				errType = OMX_ErrorNone;
			}
		}
		break;

	default:
		errType = voCOMXCompSink::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioSink::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if (pAudioFormat->nPortIndex == 0)
			{
				if(pAudioFormat->eEncoding != OMX_AUDIO_CodingPCM)
					return OMX_ErrorPortsNotCompatible;

				errType = OMX_ErrorNone;
			}
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE*)pComponentParameterStructure;
			if (!m_bSupportMultiChannel && pPCMFormat->nChannels != 1 && pPCMFormat->nChannels != 2)
			{
				VOLOGI ("m_bSupportMultiChannel %d, input channel: %d", m_bSupportMultiChannel, (int)pPCMFormat->nChannels);
				if (pPCMFormat->nChannels > 2)
					m_pcmType.nChannels = 2;				
				return OMX_ErrorPortsNotCompatible;
			}
			
			m_nChannels = pPCMFormat->nChannels;
			m_nSampleRate = pPCMFormat->nSamplingRate;
			m_nBits = pPCMFormat->nBitPerSample;

			if(OMX_FALSE == m_bFixedSampleRate)		// Some device (LG) can support illegal sample rate, so we need not modify it, East 2011/03/24
			{
				if (m_nSampleRate <= 8000)
					m_nSampleRate = 8000;
				else if (m_nSampleRate <= 11025)
					m_nSampleRate = 11025;
				else if (m_nSampleRate <= 12000)
					m_nSampleRate = 12000;
				else if (m_nSampleRate <= 16000)
					m_nSampleRate = 16000;
				else if (m_nSampleRate <= 22050)
					m_nSampleRate = 22050;
				else if (m_nSampleRate <= 24000)
					m_nSampleRate = 24000;
				else if (m_nSampleRate <= 32000)
					m_nSampleRate = 32000;
				else if (m_nSampleRate <= 44100)
					m_nSampleRate = 44100;
				else if (m_nSampleRate <= 48000)
					m_nSampleRate = 48000;
			}

			m_pcmType.nChannels = m_nChannels;
			m_pcmType.nSamplingRate = m_nSampleRate;
			m_pcmType.nBitPerSample = m_nBits;

			if (m_pAudioRender != NULL)
			{
				VO_AUDIO_FORMAT	fmtAudio;
				fmtAudio.SampleRate = m_nSampleRate;
				fmtAudio.Channels = m_nChannels;

				//permit 8 bit!!
				fmtAudio.SampleBits = (8 == m_nBits) ? 8 : 16;

				m_pAudioRender->SetFormat (&fmtAudio);
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexRenderBufferCallBack:
		{
			OMX_VO_CHECKRENDERBUFFERTYPE * pRenderType = (OMX_VO_CHECKRENDERBUFFERTYPE *)pComponentParameterStructure;
			m_cbRenderBuffer.pCallBack = pRenderType->pCallBack;
			m_cbRenderBuffer.pUserData = pRenderType->pUserData;
		}
		break;

	case OMX_VO_IndexAudioRenderFixedSampleRate:
		{
			m_bFixedSampleRate = *(OMX_BOOL*)pComponentParameterStructure;
			VOLOGI ("m_bFixedSampleRate %d", m_bFixedSampleRate);
		}
		break;

	case OMX_VO_IndexAudioRenderVideoFirstOutput:
		{
			m_bVideoFirstOutput = *(OMX_BOOL*)pComponentParameterStructure;
			VOLOGI ("m_bVideoFirstOutput %d", m_bVideoFirstOutput);
		}
		break;

	case OMX_VO_IndexMultiChannelSupport:
		{
			m_bSupportMultiChannel = *(OMX_BOOL*)pComponentParameterStructure;
			VOLOGI ("m_bSupportMultiChannel %d", m_bSupportMultiChannel);
		}
		break;

	case OMX_VO_IndexAudioRenderTimeOffset:
		{
			m_nOffsetTime = *(OMX_S32*)pComponentParameterStructure;
			VOLOGI ("m_nOffsetTime %d", (int)m_nOffsetTime);
		}
		break;

	default:
		errType = voCOMXCompSink::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioSink::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure)
{
	switch (nIndex)
	{
	case OMX_IndexConfigTimePosition:
		m_pClockPort->StartWallClock (0);
		break;

	default:
		break;
	}

	return voCOMXCompSink::SetConfig (hComponent, nIndex, pComponentConfigStructure);
}

OMX_ERRORTYPE voCOMXAudioSink::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput = m_ppPorts[0] = new voCOMXBasePort (this, 0, OMX_DirInput);
		if (m_pInput == NULL)
			return OMX_ErrorInsufficientResources;
		m_pInput->SetCallbacks (m_pCallBack, m_pAppData);

		m_pBufferQueue = m_pInput->GetBufferQueue ();
		m_pSemaphore = m_pInput->GetBufferSem ();

		m_ppPorts[1] = new voCOMXPortClock (this, 1, OMX_DirInput);
		if (m_ppPorts[1] == NULL)
			return OMX_ErrorInsufficientResources;

		m_pClockPort = (voCOMXPortClock *)m_ppPorts[1];
		m_pClockPort->SetCallbacks (m_pCallBack, m_pAppData);
		m_pClockPort->SetClockType (OMX_TIME_RefClockAudio);

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioSink::CreateRender (void)
{
	if (m_pAudioRender == NULL)
	{
		m_pAudioRender = new CAudioRender (NULL, m_pMemOP);
		m_pAudioRender->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
		m_pAudioRender->SetWorkPath ((VO_TCHAR *)m_pWorkPath);

		VO_AUDIO_FORMAT	fmtAudio;
		fmtAudio.SampleRate = m_nSampleRate;
		fmtAudio.Channels = m_nChannels;
		//permit 8 bit!!
		fmtAudio.SampleBits = (8 == m_nBits) ? 8 : 16;

		m_pAudioRender->Init (&fmtAudio);

		VO_U32 nBufferTime = 0;
		VO_U32	nRC = m_pAudioRender->GetParam (VO_PID_AUDIORENDER_MINBUFF, &nBufferTime);
		if (nRC == VO_ERR_NONE)
			m_nBufferTime = nBufferTime;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioSink::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle)
		m_llSeekTime = 0;

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_cbRenderBuffer.pCallBack == NULL)
		{
			if (m_pAudioRender == NULL)
				CreateRender ();
			else
				m_pAudioRender->Stop ();
		}

		if (!m_bThread)
			return OMX_ErrorNone;
	}
	else if (newState == OMX_StateExecuting) 
	{
		if (m_pAudioRender != NULL)
			m_pAudioRender->Start ();

		//because pause will stop wall clock
		//so when pause -> executing, we should re-start wall clock
		//East 2010/02/04
		if (m_sTrans == COMP_TRANSSTATE_PauseToExecute)
			m_nRenderFrames = 0;
	}
	else if (newState == OMX_StatePause && m_sTrans == COMP_TRANSSTATE_ExecuteToPause)
	{
		if (m_pAudioRender != NULL)
			m_pAudioRender->Pause ();
	}
	else if (newState == OMX_StateIdle)
	{
		m_llSeekTime = 0;
		m_nRenderFrames = 0;
		m_llPlayTime = 0;

		if (m_pAudioRender != NULL)
			m_pAudioRender->Stop ();
	}

	return voCOMXCompSink::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioSink::Flush (OMX_U32	nPort)
{
	{
		voCOMXAutoLock lock (&m_mutRender);
		if (m_pAudioRender != NULL)
			m_pAudioRender->Flush ();
	}

	OMX_ERRORTYPE errType = voCOMXCompSink::Flush (nPort);

	m_llSeekTime = 0;
	m_nRenderFrames = 0;
	m_nDataFill = 0;

	m_llRenderBufferTime = 0;
	m_llRenderSystemTime = 0;

	return errType;
}

OMX_ERRORTYPE voCOMXAudioSink::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
												   OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	VOLOGR ("EmptyThisBuffer pBuffer 0x%08X", pBuffer);

	if (m_bThread)
	{
		return voCOMXCompSink::EmptyThisBuffer (hComponent, pBuffer);
	}
	else
	{
		OMX_ERRORTYPE	errType = OMX_ErrorNone;

		errType = FillBuffer (pBuffer);

		//errType = m_pInput->ReturnBuffer (pBuffer);
		if (m_pInput->IsTunnel ())
		{
			pBuffer->nOutputPortIndex = m_pInput->GetTunnelPort ();
			pBuffer->nInputPortIndex = 0;
			errType = m_pInput->GetTunnelComp()->FillThisBuffer (m_pInput->GetTunnelComp (), pBuffer);
		}
		else
		{
			if (m_pCallBack != NULL)
				errType = m_pCallBack->EmptyBufferDone (m_pComponent, m_pAppData, pBuffer);
		}

		if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
		{			
			if (m_pCallBack != NULL)
			{
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventBufferFlag, 0, pBuffer->nFlags, NULL);
			}
		}

		return errType;
	}
}

OMX_ERRORTYPE voCOMXAudioSink::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if (!m_bSetThreadPriority)
	{
		voOMXThreadSetPriority (m_hBuffThread, VOOMX_THREAD_PRIORITY_ABOVE_NORMAL);
		m_bSetThreadPriority = OMX_TRUE;
	}

	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	if (m_pDumpInputFile != NULL)
		voOMXFileWrite (m_pDumpInputFile, pBuffer->pBuffer, pBuffer->nFilledLen);

	if (pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
	{
		m_pClockPort->StartWallClock (1);
		m_pClockPort->AudioStreamStop (1);
	}

//	if (m_nRenderFrames == 0)
//		pBuffer->nTimeStamp = 0;

	if (pBuffer->nTimeStamp < m_llSeekTime)
		return OMX_ErrorNone;

	m_llSeekTime = 0;

	if (m_nLogLevel > 0)
	{
		if (m_nLogSysStartTime == 0)
			m_nLogSysStartTime = voOMXOS_GetSysTime ();

		VOLOGI ("Sys Time: %d  frame time is %d, Diff %d, size %d", (int)(voOMXOS_GetSysTime () - m_nLogSysStartTime), (int)pBuffer->nTimeStamp, (int)((voOMXOS_GetSysTime () - m_nLogSysStartTime) - pBuffer->nTimeStamp), (int)pBuffer->nFilledLen);

		m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
	}

	if (m_nCopyBuffer > 0)
	{
		if (m_pDataBuffer == NULL)
		{
			m_nDataSize = m_nSampleRate * m_nChannels * m_nBits / 4;
			m_nDataStep = m_nDataSize * m_nDataTime / 1000;
			m_pDataBuffer = (OMX_U8*)voOMXMemAlloc (m_nDataSize);
		}

		if (m_nDataFill == 0)
			voOMXMemCopy (&m_buffRender, pBuffer, sizeof (OMX_BUFFERHEADERTYPE));

		voOMXMemCopy (m_pDataBuffer + m_nDataFill, pBuffer->pBuffer, pBuffer->nFilledLen);
		m_nDataFill += pBuffer->nFilledLen;

		if ((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) == 0)
		{
			if (m_nDataFill < m_nDataStep)
				return OMX_ErrorNone;
		}

		m_buffRender.pBuffer = m_pDataBuffer;
		m_buffRender.nFilledLen = m_nDataFill;
		m_nDataFill = 0;

		pBuffer = &m_buffRender;
	}

	OMX_ERRORTYPE	errType = OMX_ErrorNone;
	if (m_cbRenderBuffer.pCallBack != NULL)
	{
		if((0 == m_nRenderFrames) && (OMX_TRUE == m_bVideoFirstOutput))	// first frame
		{
			OMX_U32 nTryTimes = 0;
			while(nTryTimes < 300)
			{
				OMX_TICKS nVideoRenderTime = -1;
				if(OMX_ErrorNone == m_pClockPort->GetVideoRenderTime(&nVideoRenderTime) && -1 != nVideoRenderTime)
					break;

				voOMXOS_Sleep(2);
				nTryTimes++;
			}
			if(nTryTimes >= 300)
				VOLOGW("It was time out when wait video sink output!");
		}

		voCOMXAutoLock lock (&m_mutRender);

		OMX_U32 nDuration = 0;
		OMX_S64 nRefTime = pBuffer->nTimeStamp;
		if (pBuffer->nFilledLen > 0)
		{
			nDuration = (pBuffer->nFilledLen * 8000) / (m_pcmType.nChannels * m_pcmType.nBitPerSample * m_pcmType.nSamplingRate);

			// first time we will calculate delay time
			OMX_U32 dwStartTime = 0;
			if(m_nRenderDelay == VO_AUDIOSINK_UNVALID_DELAY)
				dwStartTime = voOMXOS_GetSysTime();

			// render audio data
			VOLOGR ("111 m_cbRenderBuffer.pCallBack, pBuffer 0x%08X", pBuffer);
			errType = m_cbRenderBuffer.pCallBack (this, m_cbRenderBuffer.pUserData, pBuffer, OMX_IndexParamAudioPcm, &m_pcmType);
			VOLOGR ("222 m_cbRenderBuffer.pCallBack, pBuffer 0x%08X", pBuffer);

			// first time we will calculate delay time
			if(m_nRenderDelay == VO_AUDIOSINK_UNVALID_DELAY)
			{
				m_nRenderDelay = nDuration - (voOMXOS_GetSysTime() - dwStartTime);
				VOLOGI ("m_nRenderDelay %d", (int)m_nRenderDelay);
			}

			nRefTime += nDuration;
			if((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) == 0 || pBuffer->nFilledLen > 0)
				m_llPlayTime = nRefTime;

			if(m_nRenderDelay > 0 && m_nRenderDelay != VO_AUDIOSINK_UNVALID_DELAY)
				nRefTime -= m_nRenderDelay;
			if(nRefTime < 0)
				nRefTime = 0;

			if (m_llRenderSystemTime == 0)
			{
				m_llRenderSystemTime = voOMXOS_GetSysTime ();
				m_llRenderBufferTime = nRefTime;
			}
		}

		// if (errType == OMX_ErrorNone)
		{
			if (m_pClockPort != NULL)
			{
				if ((!(pBuffer->nFlags & OMX_BUFFERFLAG_EOS)) || pBuffer->nTimeStamp != 0)
				{
					if (m_sState == OMX_StateExecuting)
					{
						if (abs (((int)nRefTime - (int)m_llRenderBufferTime) - (voOMXOS_GetSysTime () - (int)m_llRenderSystemTime)) > m_nRenderAdjustTime)
						{
							m_pClockPort->UpdateMediaTime (nRefTime + m_nOffsetTime);
							m_llRenderBufferTime = 0;
							m_llRenderSystemTime = 0;

							if(m_nDumpRuntimeLog)
							{
								VOLOGI ("time stamp %d, offset time %d", (int)pBuffer->nTimeStamp, (int)m_nOffsetTime);
							}
						}
						else
						{
							m_pClockPort->UpdateMediaTime ((voOMXOS_GetSysTime() - m_llRenderSystemTime) + m_llRenderBufferTime + m_nOffsetTime);

							if(m_nDumpRuntimeLog)
							{
								VOLOGI ("system time %d, render system time %d, render buffer time %d, offset time %d", 
									(int)voOMXOS_GetSysTime(), (int)m_llRenderSystemTime, (int)m_llRenderBufferTime, (int)m_nOffsetTime);
							}
						}
					}
				}

				if ((m_nRenderFrames <= 1 || (pBuffer->nFlags & OMX_BUFFERFLAG_STARTTIME)) && !(pBuffer->nFlags & OMX_BUFFERFLAG_EOS))
				{
					if (pBuffer->nFilledLen > 0)
					{
						VOLOGI ("OMX_VO_IndexStartWallClock 1");
						m_pClockPort->StartWallClock (1);
						m_pClockPort->AudioStreamStop (0);
					}
				}

			}

			if (pBuffer->nFilledLen > 0)
				m_nRenderFrames++;

			if (m_nLogLevel > 1)
			{
				VOLOGI ("Render audio used time is %d",(int)( voOMXOS_GetSysTime () - m_nLogSysCurrentTime));
			}

			return errType;
		}
	}

	if (m_pAudioRender == NULL)
		CreateRender ();

	while ((unsigned int)m_pAudioRender->GetBufferTime () > (unsigned int)m_nBufferTime)
	{
		voOMXOS_Sleep(2);
		if (m_sTrans != COMP_TRANSSTATE_None)
			return OMX_ErrorNone;

		if (m_sState != OMX_StateExecuting)
			return OMX_ErrorNone;
	}

	if (pBuffer->nFilledLen > 0)
	{
		voCOMXAutoLock lock (&m_mutRender);

		//Rogine add buffer duration calculation
		OMX_U32 nDuration = 0;
		nDuration = (pBuffer->nFilledLen * 8000) / (m_pcmType.nChannels * m_pcmType.nBitPerSample * m_pcmType.nSamplingRate);
		VOLOGR ("@#@#@ AR: Buffer Duration %ld", nDuration);

#ifdef  _WIN32
		//Rogine add for MFW Self-test
		if (m_pCallBack != NULL)
		{	
			//VOLOGI ("@@#@#@ Audio Render Time Stamp %d", pBuffer->nTimeStamp);
			voOMXMemCopy(m_pARTimeStamp, &pBuffer->nTimeStamp, sizeof(OMX_S32));
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_AudioRenderTimeStamp, NULL, NULL, m_pARTimeStamp);
		}
#endif

		m_pAudioRender->Render (pBuffer->pBuffer + pBuffer->nOffset, pBuffer->nFilledLen, pBuffer->nTimeStamp);
		if (m_pClockPort != NULL)
		{
			OMX_S32 nPlayTime = (OMX_S32)m_pAudioRender->GetPlayingTime();
			VOLOGR ("@#@#@ AR: PlayTime %ld", nPlayTime);

			//playing time will not be updated immediately
			if (nPlayTime > 0 && m_sState == OMX_StateExecuting)
			{
#if 0
				//add buffer duration to adjust playtime
				OMX_S32 playtime = nPlayTime + nDuration; 
#else
				OMX_S32 playtime = nPlayTime;
#endif
				m_pClockPort->UpdateMediaTime (playtime);
				VOLOGR ("@#@#@ SysTime %ld: AR: Update mediaTime %ld", voOMXOS_GetSysTime(), playtime);
			}

			if (m_nRenderFrames <= 1)
			{
				if (pBuffer->nFilledLen > 0)
				{
					VOLOGI ("OMX_VO_IndexStartWallClock 1");
					m_pClockPort->StartWallClock (1);
				}
			}
		}
	}
	else
	{
		if (m_pClockPort != NULL)
		{
			if ((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) && (pBuffer->nTimeStamp != 0))
			{
				if (m_sState == OMX_StateExecuting)
					m_pClockPort->UpdateMediaTime (pBuffer->nTimeStamp);
			}
		}
	}

	if((pBuffer->nFlags & OMX_BUFFERFLAG_EOS) == 0 || pBuffer->nFilledLen > 0)
		m_llPlayTime = pBuffer->nTimeStamp + (pBuffer->nFilledLen * 1000) / (m_pcmType.nChannels * m_pcmType.nBitPerSample * m_pcmType.nSamplingRate / 8);

	if (pBuffer->nFilledLen > 0)
		m_nRenderFrames++;

	return errType;
}

OMX_ERRORTYPE voCOMXAudioSink::InitPortType (void)
{
	m_portParam[OMX_PortDomainAudio].nPorts = 1;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	m_portParam[OMX_PortDomainOther].nPorts = 1;
	m_portParam[OMX_PortDomainOther].nStartPortNumber = 1;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	m_pInput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 2;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 44100 * 4;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	voOMXBase_SetHeader (&m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	m_pcmType.nPortIndex = 1;
	m_pcmType.nChannels = 2;
	m_pcmType.eNumData = OMX_NumericalDataSigned;
	m_pcmType.eEndian = OMX_EndianBig;
	m_pcmType.bInterleaved = OMX_TRUE;
	m_pcmType.nBitPerSample = 16;
	m_pcmType.nSamplingRate = 44100;
	m_pcmType.ePCMMode = OMX_AUDIO_PCMModeLinear;

	return OMX_ErrorNone;
}
