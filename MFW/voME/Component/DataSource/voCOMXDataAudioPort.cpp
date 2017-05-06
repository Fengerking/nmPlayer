	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXDataAudioPort.cpp

	Contains:	voCOMXDataAudioPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voCOMXPortClock.h"
#include "voCOMXDataAudioPort.h"

#define LOG_TAG "voCOMXDataAudioPort"
#include "voLog.h"

voCOMXDataAudioPort::voCOMXDataAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXDataOutputPort (pParent, nIndex)
	, mbReceivedEOS(OMX_FALSE)
	, m_bBuffering (OMX_FALSE)
	, mbUnderflow (OMX_FALSE)
	, mnBufPercent(0)
	, mnBufPosPercent(0)
	, mbSendPercent0(OMX_FALSE)
	, mnOutLog(0)
{
	strcpy (m_pObjName, __FILE__);

	m_sType.eDomain = OMX_PortDomainAudio;

	m_sType.nBufferSize = 40960;
	m_sType.nBufferCountActual = 2;
	m_sType.nBufferCountMin = 1;
	m_sType.bBuffersContiguous = OMX_FALSE;
	m_sType.nBufferAlignment = 1;

	strcpy (m_pMIMEType, "AAC");
	m_sType.format.audio.cMIMEType = m_pMIMEType;
	m_sType.format.audio.pNativeRender = NULL;
	m_sType.format.audio.bFlagErrorConcealment = OMX_FALSE;
	m_sType.format.audio.eEncoding = OMX_AUDIO_CodingAAC;

	voOMXBase_SetHeader (&m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	m_pcmType.nPortIndex = nIndex;
	m_pcmType.nChannels = 2;
	m_pcmType.eNumData = OMX_NumericalDataSigned;
	m_pcmType.eEndian = OMX_EndianBig;
	m_pcmType.bInterleaved = OMX_TRUE;
	m_pcmType.nBitPerSample = 16;
	m_pcmType.nSamplingRate = 44100;
	m_pcmType.ePCMMode = OMX_AUDIO_PCMModeLinear;
}

voCOMXDataAudioPort::~voCOMXDataAudioPort(void)
{
}

void voCOMXDataAudioPort::SetConfigFile (CBaseConfig * pCfgFile)
{
	voCOMXDataOutputPort::SetConfigFile (pCfgFile);

	if (m_pCfgComponent != NULL)
	{
		char * pFile = m_pCfgComponent->GetItemText (m_pParent->GetName (), (char*)"AudioSourceFile");
		if (pFile != NULL)
		{
#ifdef _WIN32
			TCHAR szFile[256];
			memset (szFile, 0, sizeof (szFile));
			MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
			m_hSourceFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_READ_ONLY);		
#else
			m_hSourceFile = voOMXFileOpen (pFile, VOOMX_FILE_READ_ONLY);
#endif // _WIN32
			VOLOGI ("Open source file %s, The handle is 0X%08X.", pFile, (int)m_hSourceFile);
		}
		m_nSourceFormat = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"AudioSourceFormat", 0);
		if (m_hSourceFile != NULL)
		{
			if (m_nSourceFormat == 0)
			{
				m_nSourceSize = (OMX_S32)voOMXFileSize (m_hSourceFile);
				m_pSourceBuffer = new OMX_U8[m_nSourceSize];
				m_pCurrentBuffer = m_pSourceBuffer;

				voOMXFileRead (m_hSourceFile, m_pSourceBuffer, m_nSourceSize);
			}
			else
			{
				m_nSourceSize = 10240;
				m_pSourceBuffer = new OMX_U8[m_nSourceSize];
				m_pCurrentBuffer = m_pSourceBuffer;
			}
		}

		pFile = m_pCfgComponent->GetItemText (m_pParent->GetName (), (char*)"AudioDumpFile");
		if (pFile != NULL)
		{
#ifdef _WIN32
			TCHAR szFile[256];
			memset (szFile, 0, sizeof (szFile));
			MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
			m_hDumpFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
			m_hDumpFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
			VOLOGI ("Create dump file %s, The handle is 0X%08X.", pFile, (int)m_hDumpFile);
		}
		m_nDumpFormat = m_pCfgComponent->GetItemValue (m_pParent->GetName (), (char*)"AudioDumpFormat", 0);

		m_nSyncWord = 0X4050F1FF;
	}
}

OMX_ERRORTYPE voCOMXDataAudioPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	//OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pParam;
			if (pAudioFormat->nIndex > 0)
				return OMX_ErrorNoMore;

			pAudioFormat->eEncoding = m_sType.format.audio.eEncoding;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioMp3:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingMP3)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE *) pParam;
			pMP3Format->nChannels = m_pcmType.nChannels;
			pMP3Format->nSampleRate = m_pcmType.nSamplingRate;

			pMP3Format->nBitRate = 0;
			pMP3Format->nAudioBandWidth = 0;
			pMP3Format->eChannelMode = OMX_AUDIO_ChannelModeStereo;
			pMP3Format->eFormat = OMX_AUDIO_MP3StreamFormatMP1Layer3;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioRa:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingRA)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_RATYPE * pRAFormat = (OMX_AUDIO_PARAM_RATYPE *) pParam;

			pRAFormat->nChannels = m_pcmType.nChannels;
			pRAFormat->nSamplingRate = m_pcmType.nSamplingRate;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingAAC)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE *) pParam;
			pAACFormat->nChannels = m_pcmType.nChannels;
			pAACFormat->nSampleRate = m_pcmType.nSamplingRate;

			pAACFormat->nBitRate = 0;
			pAACFormat->nAudioBandWidth = 0;

			pAACFormat->eChannelMode = OMX_AUDIO_ChannelModeStereo;

			pAACFormat->eAACProfile = OMX_AUDIO_AACObjectLC;
			pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP2ADTS;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingAMR)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE *) pParam;
			pAmrFormat->nChannels = m_pcmType.nChannels;
			pAmrFormat->nBitRate = 0;
			pAmrFormat->eAMRBandMode = OMX_AUDIO_AMRBandModeNB0;

			pAmrFormat->eAMRFrameFormat = OMX_AUDIO_AMRFrameFormatFSF;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioWma:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingWMA)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_WMATYPE * pWMAFormat = (OMX_AUDIO_PARAM_WMATYPE *) pParam;
			pWMAFormat->nChannels = (OMX_U16)m_pcmType.nChannels;
			pWMAFormat->nSamplingRate = m_pcmType.nSamplingRate;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAdpcm:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingADPCM)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_ADPCMTYPE * pADPCMFormat = (OMX_AUDIO_PARAM_ADPCMTYPE *) pParam;
			pADPCMFormat->nChannels = m_pcmType.nChannels;
			pADPCMFormat->nSampleRate = m_pcmType.nSamplingRate;
			pADPCMFormat->nBitsPerSample = m_pcmType.nBitPerSample;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioQcelp13:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingQCELP13)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_QCELP13TYPE * pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE *) pParam;
			pQcelp13Format->nChannels = m_pcmType.nChannels;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioEvrc:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingEVRC)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_EVRCTYPE * pEVRCFormat = (OMX_AUDIO_PARAM_EVRCTYPE *) pParam;
			pEVRCFormat->nChannels = m_pcmType.nChannels;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pParam;
			if (pPCMFormat->nPortIndex != m_sType.nPortIndex)
				return OMX_ErrorPortsNotCompatible;

			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingPCM)
			{
				voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
			}
			else
			{
				OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pParam;
				pPCMFormat->nChannels = m_pcmType.nChannels;
				pPCMFormat->nBitPerSample = m_pcmType.nBitPerSample;
				pPCMFormat->nSamplingRate = m_pcmType.nSamplingRate;
			}

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXDataOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXDataAudioPort::SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	switch (nIndex)
	{
	case OMX_IndexParamAudioPcm:
	{
		OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pParam;
		if (pPCMFormat->nPortIndex != m_sType.nPortIndex)
			return OMX_ErrorPortsNotCompatible;

		voOMXMemCopy (&m_pcmType, pPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));

		return OMX_ErrorNone;
	}

	default:
		break;
	}

	return voCOMXDataOutputPort::SetParameter (nIndex, pParam);
}

OMX_ERRORTYPE	voCOMXDataAudioPort::Flush (void)
{
	//modified by Eric, 2011.12.02, fix LG issue 8880, remove unnecessary buffer event
	if(m_pLstBuffer && m_pLstBuffer->GetBuffTime() > 0)
	{
		mbReceivedEOS = OMX_FALSE;
		mnBufPercent = 0;
		mbSendPercent0 = OMX_FALSE;
		mnBufPosPercent = 0;
	}
	return voCOMXDataOutputPort::Flush();
}

OMX_ERRORTYPE voCOMXDataAudioPort::FlushBuffer()
{
	if(m_pLstBuffer && m_pLstBuffer->GetBuffTime() > 0)
	{
		mbReceivedEOS = OMX_FALSE;
		mnBufPercent = 0;
		mbSendPercent0 = OMX_FALSE;
		mnBufPosPercent = 0;
	}

	return voCOMXDataOutputPort::FlushBuffer();
}

OMX_ERRORTYPE voCOMXDataAudioPort::SendBuffer (OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (m_hDumpFile != NULL)
	{
		if (m_nDumpFormat >= 1)
			voOMXFileWrite (m_hDumpFile, (OMX_U8*)&pBuffer->nFilledLen, 4);
		if (m_nDumpFormat >= 2)
			voOMXFileWrite (m_hDumpFile, (OMX_U8*)&pBuffer->nTimeStamp, 8);
		voOMXFileWrite (m_hDumpFile, pBuffer->pBuffer, pBuffer->nFilledLen);
	}

	if (m_hSourceFile != NULL)
	{
		voOMXOS_Sleep (10);
		return OMX_ErrorNone;
	}

	if (m_pLstBuffer == NULL)
		m_pLstBuffer = new voCOMXDataBufferList (4096, false);

	if (m_pLstBuffer->GetBuffTime () > m_llMaxBufferTime)
	{
		return OMX_ErrorOverflow;
	}

	if(pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
	{
		mbReceivedEOS = OMX_TRUE;
	}

	OMX_ERRORTYPE errType = m_pLstBuffer->AddBuffer (pBuffer);

	if(m_bBuffering)
	{
		OMX_S32 nBufPercent = (OMX_S32)(m_pLstBuffer->GetBuffTime() * 100 / m_llMinBufferTime);
		if(nBufPercent > 100)
		{
			nBufPercent = 100;
		}

		if((nBufPercent != 100 && nBufPercent - mnBufPercent >= 5) ||  (nBufPercent == 100 && nBufPercent - mnBufPercent > 0))
		{
			mnBufPercent = nBufPercent;
			m_pCallBack->EventHandler(m_pParent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_Bufferstatus , nBufPercent , 0 , NULL);
		}
	}

	// some live source duration is 0, crash issue!!! East 20120206
	OMX_S32 nBufPos = 0;
	if(mTrackInfo.Duration > 0)
		nBufPos =(OMX_S32) (pBuffer->nTimeStamp * 100 / mTrackInfo.Duration);

	if(nBufPos > 100)
		nBufPos = 100;

	if((nBufPos != 100 && nBufPos - mnBufPosPercent > 5) || (nBufPos == 100 && nBufPos - mnBufPosPercent > 0))
	{
		mnBufPosPercent = nBufPos;
		m_pCallBack->EventHandler(m_pParent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_DownloadPos, nBufPos, 0, NULL);
	}

	return errType;
}

OMX_ERRORTYPE voCOMXDataAudioPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_hSourceFile != NULL)
	{
		pBuffer->nAllocLen = m_nSourceSize;
		pBuffer->nFlags = 0;
		pBuffer->nFilledLen = 0;
		pBuffer->nTimeStamp = m_llSourceSampleTime;
		m_llSourceSampleTime += 40;

		if (m_nSourceFormat == 0)
		{
			OMX_U8 * pNextData = m_pCurrentBuffer + 4;
			while (true)
			{
				if (pNextData - m_pSourceBuffer >= m_nSourceSize)
				{
					m_pCurrentBuffer = m_pSourceBuffer;
					pNextData = m_pCurrentBuffer + 4;
				}


				if (!memcmp (pNextData, &m_nSyncWord, 2))
				{
					if (pNextData - m_pCurrentBuffer > 32)
						break;
				}

				pNextData++;
			}

			pBuffer->nFilledLen = pNextData - m_pCurrentBuffer;
			pBuffer->pBuffer = m_pCurrentBuffer;

			m_pCurrentBuffer = pNextData;
		}
		else if (m_nSourceFormat == 1)
		{
			int nFrameSize = 0;
			int nReadSize = voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			if (nReadSize != 4)
			{
				voOMXFileSeek (m_hSourceFile, 0, VOOMX_FILE_BEGIN);
				voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			}
			voOMXFileRead (m_hSourceFile, m_pSourceBuffer, nFrameSize);

			pBuffer->nFilledLen = nFrameSize;
			pBuffer->pBuffer = m_pSourceBuffer;
		}
		else if (m_nSourceFormat == 2)
		{
			int nFrameSize = 0;
			int nReadSize = voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			if (nReadSize != 4)
			{
				voOMXFileSeek (m_hSourceFile, 0, VOOMX_FILE_BEGIN);
				voOMXFileRead (m_hSourceFile, (OMX_U8 *)&nFrameSize, 4);
			}

			voOMXFileRead (m_hSourceFile, (OMX_U8*)&m_llSourceSampleTime, 8);
			voOMXFileRead (m_hSourceFile, m_pSourceBuffer, nFrameSize);

			pBuffer->nTimeStamp = m_llSourceSampleTime;
			pBuffer->nFilledLen = nFrameSize;
			pBuffer->pBuffer = m_pSourceBuffer;
		}

		return OMX_ErrorNone;
	}

	OMX_S32 nOtherBufferTime = -1;
	if (m_pOtherPort != NULL)
	{
		if (!m_pOtherPort->IsEOS () && m_pOtherPort->IsDecOnly ())
		{
			pBuffer->nFilledLen = 0;
			voOMXOS_Sleep (2);

			VOLOGW ("******  m_pVideoPort->IsDecOnly");

			return OMX_ErrorNone;
		}
		nOtherBufferTime = m_pOtherPort->GetBufferTime();
	}
	
	if (((nOtherBufferTime != -1 && nOtherBufferTime < 100) || (m_pLstBuffer != NULL && m_pLstBuffer->GetBuffTime () < 100)) && !mbReceivedEOS)
	{
		m_nFillBufferCount = 0;
		mbUnderflow = OMX_TRUE;
		mnBufPercent = 0;
		mnOutLog = 0;

		VOLOGI ("mbReceivedEOS: %d", (int)mbReceivedEOS);

		if(mbSendPercent0 == OMX_FALSE)
		{	
			m_pCallBack->EventHandler (m_pParent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_BufferStart, 0, 0, NULL);
			m_pCallBack->EventHandler(m_pParent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_Bufferstatus , mnBufPercent , 0 , NULL);
			mbSendPercent0 = OMX_TRUE;
		}
	}

	if (m_nFillBufferCount == 0)
	{
		if (m_pLstBuffer != NULL && m_pLstBuffer->GetBuffTime () < m_llMinBufferTime && !mbReceivedEOS)
		{
			if (!m_bBuffering)
			{
				if (m_pClockPort != NULL)
					m_pClockPort->StartWallClock (0);
				m_bBuffering = OMX_TRUE;
			}

			voOMXOS_Sleep (100);

			if(mnOutLog == 0)
			{
				mnOutLog = 6;
				VOLOGW ("m_pLstBuffer->GetBuffTime () = %d < m_llMinBufferTime (%d)",(int)m_pLstBuffer->GetBuffTime (),(int)m_llMinBufferTime);
			}

			mnOutLog --;
			return OMX_ErrorUnderflow;
		}
	}

	if(mbUnderflow == OMX_TRUE)
	{
		mbSendPercent0 = OMX_FALSE;
		mbUnderflow = OMX_FALSE;

		if(mnBufPercent != 100)
		{
			mnBufPercent = 100;
			m_pCallBack->EventHandler(m_pParent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_Bufferstatus , mnBufPercent , 0 , NULL);
		}	
		m_pCallBack->EventHandler (m_pParent, m_pAppData,(OMX_EVENTTYPE)OMX_VO_Event_BufferStop, 100, 0, NULL);
	}

	pBuffer->nTimeStamp = 0;

	errType = voCOMXDataOutputPort::FillBuffer (pBuffer);

	VOLOGR ("[Audio]errType 0x%08X, timestamp %d, system time %d, buffering time %d", errType, (OMX_U32)pBuffer->nTimeStamp, voOMXOS_GetSysTime(), m_pLstBuffer->GetBuffTime());

	if (m_bBuffering)
	{
		m_bBuffering = OMX_FALSE;
		pBuffer->nFlags |= OMX_BUFFERFLAG_STARTTIME;
	}

	if ((m_nLogLevel & 0XF0) > 0)
	{
		VOLOGI ("Read video used time is %d, Sample Size %d, Time: %d, Flags %08X, Result 0X%08X", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime), (int)pBuffer->nFilledLen, (int)pBuffer->nTimeStamp, (int)pBuffer->nFlags, (int)errType);
	}

	return errType;
}

OMX_ERRORTYPE voCOMXDataAudioPort::ReconfigPort(VO_LIVESRC_TRACK_INFOEX * pInfoEx)
{
	VO_SOURCE_TRACKINFO trkInfo;
	memset(&trkInfo, 0, sizeof(trkInfo));
	trkInfo.Type = pInfoEx->Type;
	trkInfo.Codec = pInfoEx->Codec;
	trkInfo.Start = 0;
	trkInfo.Duration = -2;	// invalid
	trkInfo.HeadData = pInfoEx->Padding;
	trkInfo.HeadSize = pInfoEx->HeadSize;
	SetTrackInfo(&trkInfo);

	OMX_PARAM_PORTDEFINITIONTYPE *pType;
	GetPortType(&pType);
	if(pInfoEx->Codec == VO_AUDIO_CodingAAC)
		pType->format.audio.eEncoding = OMX_AUDIO_CodingAAC;
	if(pInfoEx->Codec == VO_AUDIO_CodingAMRNB)
		pType->format.audio.eEncoding = OMX_AUDIO_CodingAMR;
	if(pInfoEx->Codec == VO_AUDIO_CodingWMA)
		pType->format.audio.eEncoding = OMX_AUDIO_CodingWMA;
	if(pInfoEx->Codec == VO_AUDIO_CodingMP3)
		pType->format.audio.eEncoding = OMX_AUDIO_CodingMP3;

	OMX_AUDIO_PARAM_PCMMODETYPE *pPCMType = NULL;
	GetPCMType(&pPCMType);
	if(pPCMType)
	{
		pPCMType->nChannels = pInfoEx->audio_info.Channels;
		pPCMType->nBitPerSample = pInfoEx->audio_info.SampleBits;
		pPCMType->nSamplingRate = pInfoEx->audio_info.SampleRate;
	}

	VOLOGI ("ReconfigPort Codec %d, Channel %d, SampleBits %d, SampleRate %d", 
		(int)pInfoEx->Codec, (int)pInfoEx->audio_info.Channels, (int)pInfoEx->audio_info.SampleBits, (int)pInfoEx->audio_info.SampleRate);

	return OMX_ErrorNone;
}
