/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003				*
 *																		*
 ************************************************************************/
/*******************************************************************************
	File:		voCOMXFileSink.cpp

	Contains:	voCOMXFileSink class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMX_Index.h"
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voCOMXFileSink.h"

#define LOG_TAG "voCOMXFileSink"
#include "voLog.h"

extern VO_TCHAR * g_pvoOneWorkingPath;

voCOMXFileSink::voCOMXFileSink(OMX_COMPONENTTYPE * pComponent)
	: voCOMXBaseComponent (pComponent)
	, m_pFileSink (NULL)
	, m_nFileType (OMX_VO_FILE_FormatMP4)
	, m_nURISize (0)
	, m_pURI (NULL)
	, m_pFileOP(NULL)
	, m_nMaxDuration(0)
	, m_Start_ms(0)
	, m_nMaxSize(0)
	, m_nWriteSize(0)
	, m_bFinished (OMX_FALSE)
	, m_nMaxAVDiffTime (0)
	, m_nAudioCoding (OMX_AUDIO_CodingUnused)
	, m_bAudioEOS (OMX_FALSE)
	, m_nAudioExtSize (0)
	, m_pAudioExtData (NULL)
	, m_nCurrAudioTimeStamp (0)
	, m_pAudioKeptBuffer (NULL)
	, m_nVideoCoding (OMX_VIDEO_CodingUnused)
	, m_bVideoEOS (OMX_FALSE)
	, m_nVideoExtSize (0)
	, m_pVideoExtData (NULL)
	, m_nCurrVideoTimeStamp (0)
	, m_pVideoKeptBuffer (NULL)
	, mbIntraDetectIF(OMX_FALSE)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.File.Sink");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

	memset (&m_Source, 0, sizeof (m_Source));

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

	mbIntraDetectIF = (OMX_BOOL)m_pCfgComponent->GetItemValue (m_pName, (char *)"IntraDetectIFrm", 0);
	m_nDumpRuntimeLog = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpRuntimeLog", 0);
}

voCOMXFileSink::~voCOMXFileSink(void)
{
	if (m_pFileSink != NULL)
		delete m_pFileSink;

	if (m_pURI != NULL)
		voOMXMemFree (m_pURI);
	if (m_pAudioExtData != NULL)
		voOMXMemFree (m_pAudioExtData);
	if (m_pVideoExtData != NULL)
		voOMXMemFree (m_pVideoExtData);
}

OMX_ERRORTYPE voCOMXFileSink::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nParamIndex)
	{
	case OMX_IndexParamAudioPortFormat: {
		OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
		if (pAudioFormat->nPortIndex == 0) {
			pAudioFormat->eEncoding = m_nAudioCoding;
		}
		break;
	}

	case OMX_IndexParamAudioPcm: {
		OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
		if (pPCMFormat->nPortIndex == 0) {
			voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
		}
		break;
	}

	case OMX_IndexParamContentURI: {
		OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)pComponentParameterStructure;
		if (pContent->nSize < m_nURISize) {
			return OMX_ErrorBadParameter;
		}
		voOMXMemCopy (pContent->contentURI, m_pURI, m_nURISize);
		break;
	}

	default:
		errType = voCOMXBaseComponent::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	} // switch

	return errType;
}


OMX_ERRORTYPE voCOMXFileSink::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;
	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
	{
		OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
		if (pAudioFormat->nPortIndex == 0)
		{
			m_nAudioCoding = pAudioFormat->eEncoding;
			VOLOGI("Audio Codec: %d",  m_nAudioCoding);

			switch(m_nAudioCoding)
			{
			case OMX_AUDIO_CodingAMR: 
			case OMX_AUDIO_CodingQCELP13:
			case OMX_AUDIO_CodingEVRC:
			case OMX_AUDIO_CodingAAC:
			case OMX_AUDIO_CodingMP3:
				break; // OMX_ErrorNone

			default:
				errType = OMX_ErrorComponentNotFound;
			}
		}
		break;
	}

	case OMX_IndexParamAudioMp3:
		{
			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE *)pComponentParameterStructure;

			errType = CheckParam (pComp, pMP3Format->nPortIndex, pMP3Format, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_pcmType.nChannels     = pMP3Format->nChannels;
			m_pcmType.nSamplingRate = pMP3Format->nSampleRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pAmrFormat->nPortIndex, pAmrFormat, sizeof(OMX_AUDIO_PARAM_AMRTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			VOLOGI("AMRBandMode : %d " , pAmrFormat->eAMRBandMode);
			if(pAmrFormat->eAMRBandMode > OMX_AUDIO_AMRBandModeNB7 && pAmrFormat->eAMRBandMode < OMX_AUDIO_AMRBandModeKhronosExtensions)
			{
				m_pcmType.nChannels     = pAmrFormat->nChannels;
				m_pcmType.nSamplingRate = 16000;
			}
			else
			{
				m_pcmType.nChannels     = pAmrFormat->nChannels;
				m_pcmType.nSamplingRate = 8000;
			}
		
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pAACFormat->nPortIndex, pAACFormat, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_pcmType.nChannels     = pAACFormat->nChannels;
			m_pcmType.nSamplingRate = pAACFormat->nSampleRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioQcelp13:
		{
			OMX_AUDIO_PARAM_QCELP13TYPE * pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pQcelp13Format->nPortIndex, pQcelp13Format, sizeof(OMX_AUDIO_PARAM_QCELP13TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_pcmType.nChannels = pQcelp13Format->nChannels;
			m_pcmType.nSamplingRate = 8000;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioEvrc:
		{
			OMX_AUDIO_PARAM_EVRCTYPE * pEvrcFormat = (OMX_AUDIO_PARAM_EVRCTYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pEvrcFormat->nPortIndex, pEvrcFormat, sizeof(OMX_AUDIO_PARAM_EVRCTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_pcmType.nChannels = pEvrcFormat->nChannels;
			m_pcmType.nSamplingRate = 8550;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamVideoPortFormat:
	{
		OMX_VIDEO_PARAM_PORTFORMATTYPE * pVideoFormat = (OMX_VIDEO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;

		if (pVideoFormat->nPortIndex == 1) 
		{
			m_nVideoCoding = pVideoFormat->eCompressionFormat;
			VOLOGI("Video Codec: %d",  m_nVideoCoding);

			switch(m_nVideoCoding) 
			{
			case OMX_VIDEO_CodingH263: 
			case OMX_VIDEO_CodingAVC: 
			case OMX_VIDEO_CodingMPEG4: 
				break; // OMX_ErrorNone

			default:
				errType = OMX_ErrorComponentNotFound;
			}
		}
		break;
	}

	case OMX_VO_IndexFileFormat: {
		m_nFileType = *((OMX_VO_FILE_FORMATTYPE*) pComponentParameterStructure);
		VOLOGI("m_nFileType = %x", (int)m_nFileType);
		break;
	}

	case OMX_VO_IndexFileMaxSize: {
		m_nMaxSize  = (*(OMX_S64*) pComponentParameterStructure);
		m_nMaxSize = m_nMaxSize * 96 / 100;
		break;
	}

	case OMX_VO_IndexFileMaxDuration: {
		m_nMaxDuration  = (*(OMX_S64*) pComponentParameterStructure);
		break;
	}
	
	case OMX_IndexParamContentURI: {
		OMX_PARAM_CONTENTURITYPE * pContent = (OMX_PARAM_CONTENTURITYPE *)pComponentParameterStructure;
		errType = voOMXBase_CheckHeader (pContent, pContent->nSize);

		if (m_pURI != NULL)
			voOMXMemFree (m_pURI);

		m_nURISize = pContent->nSize - 8;
		m_pURI = (OMX_S8*)voOMXMemAlloc (m_nURISize);
		if (m_pURI == NULL)
			return OMX_ErrorInsufficientResources;
		memset (m_pURI, 0, m_nURISize);
		voOMXMemCopy (m_pURI, pContent->contentURI, m_nURISize);
		m_Source.pSource = (VO_PTR) m_pURI;
		m_Source.nFlag   = VO_FILE_TYPE_NAME;
		break;
	}

	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)pComponentParameterStructure;

			if (pExtData->nDataSize > 0)
			{
				if (pExtData->nPortIndex == 0)
				{
					VOLOGI ("The audio head data size is %d", (int)pExtData->nDataSize);

					if (m_pAudioExtData != NULL)
						voOMXMemFree (m_pAudioExtData);

					m_nAudioExtSize = pExtData->nDataSize;
					m_pAudioExtData = (OMX_S8*)voOMXMemAlloc (m_nAudioExtSize);
					if (m_pAudioExtData != NULL)
						voOMXMemCopy (m_pAudioExtData, pExtData->data, m_nAudioExtSize);
					else
						m_nAudioExtSize = 0;
				}
				else
				{
					VOLOGI ("The video head data size is %d",(int)pExtData->nDataSize);

					if (m_pVideoExtData != NULL)
						voOMXMemFree (m_pVideoExtData);

					m_nVideoExtSize = pExtData->nDataSize;
					m_pVideoExtData = (OMX_S8*)voOMXMemAlloc (m_nVideoExtSize);
					if (m_pVideoExtData != NULL)
						voOMXMemCopy (m_pVideoExtData, pExtData->data, m_nVideoExtSize);
					else
						m_nVideoExtSize = 0;
				}
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexSourceHandle:
		if((VO_PTR) pComponentParameterStructure)
		{
			m_Source.pSource = (VO_PTR) pComponentParameterStructure;
			m_Source.nFlag   = VO_FILE_TYPE_HANDLE;
		}
		break;

	case OMX_VO_IndexSourceID:
		m_Source.pSource = (VO_PTR) pComponentParameterStructure;
		m_Source.nFlag   = VO_FILE_TYPE_ID;
		break;

	case OMX_VO_IndexSourceOffset:
		m_Source.nOffset = *(VO_S64 *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexSourceLength:
		m_Source.nLength = *(VO_S64 *)pComponentParameterStructure;
		break;

	case OMX_VO_IndexSourcePipe: 
	{
/*
		OMX_VO_FILE_OPERATOR * pPipe = (OMX_VO_FILE_OPERATOR *)pComponentParameterStructure;
		if (pPipe)
		{
			m_pFileOP        = &m_FileOP;
			m_pFileOP->Open  = (VOFILESOURCEOPEN) pPipe->Open;
			m_pFileOP->Read  = pPipe->Read;
			m_pFileOP->Write = pPipe->Write;
			m_pFileOP->Seek  = (VOFILESOURCESEEK) pPipe->Seek;
			m_pFileOP->Flush = pPipe->Flush;
			m_pFileOP->Size  = pPipe->Size;
			m_pFileOP->Close = pPipe->Close;
		}
		else
*/
		{
			m_pFileOP = NULL;
		}
		break;
	}

	case OMX_VO_IndexFileMaxAVDiffTime:
		m_nMaxAVDiffTime = *(VO_U32 *)pComponentParameterStructure;
		VOLOGI("OMX_VO_IndexFileMaxAVDiffTime %d", (int)m_nMaxAVDiffTime);
		break;

	default:
		errType = voCOMXBaseComponent::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	} // switch

	return errType;
}

OMX_ERRORTYPE voCOMXFileSink::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_pFileOP == NULL)
		{
			cmnFileFillPointer ();
			m_pFileOP = &g_fileOP;
		}

		VO_FILE_FORMAT nFileFormat;
		switch(m_nFileType)
		{
		case OMX_VO_FILE_FormatASF:   nFileFormat = VO_FILE_FFMOVIE_ASF;  break;
		case OMX_VO_FILE_FormatAVI:   nFileFormat = VO_FILE_FFMOVIE_AVI;  break;
		case OMX_VO_FILE_FormatReal:  nFileFormat = VO_FILE_FFMOVIE_REAL; break;
		case OMX_VO_FILE_FormatTS:	  nFileFormat = VO_FILE_FFMOVIE_TS;   break;
		case OMX_VO_FILE_FormatOGG:   nFileFormat = VO_FILE_FFAUDIO_OGG;  break;
		case OMX_VO_FILE_FormatAAC:   nFileFormat = VO_FILE_FFAUDIO_AAC;  break;
		case OMX_VO_FILE_FormatAMRNB: nFileFormat = VO_FILE_FFAUDIO_AMR;  break;
		case OMX_VO_FILE_FormatAMRWB: nFileFormat = VO_FILE_FFAUDIO_AWB;  break;
		case OMX_VO_FILE_FormatQCP:   nFileFormat = VO_FILE_FFAUDIO_QCP;  break;
		case OMX_VO_FILE_FormatWAV:   nFileFormat = VO_FILE_FFAUDIO_WAV;  break;
		case OMX_VO_FILE_FormatMP3:   nFileFormat = VO_FILE_FFAUDIO_MP3;  break;

		default:
		case OMX_VO_FILE_FormatMP4:   nFileFormat = VO_FILE_FFMOVIE_MP4;  break;
		}

		if (m_pFileSink == NULL)
		{
			m_pFileSink = new CFileSink (NULL, nFileFormat, m_pMemOP, m_pFileOP);
			if (m_pFileSink == NULL)
				return OMX_ErrorInsufficientResources;
			m_pFileSink->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
			m_pFileSink->SetWorkPath ((VO_TCHAR *)m_pWorkPath);

			VO_U32 nACodec = 0;
			switch(m_nAudioCoding) {
			case OMX_AUDIO_CodingMP3:     nACodec = VO_AUDIO_CodingMP3;     break;
			case OMX_AUDIO_CodingAAC:     nACodec = VO_AUDIO_CodingAAC;     break;
			case OMX_AUDIO_CodingQCELP13: nACodec = VO_AUDIO_CodingQCELP13; break;
			case OMX_AUDIO_CodingEVRC:    nACodec = VO_AUDIO_CodingEVRC;    break;
			case OMX_AUDIO_CodingAMR:     nACodec = VO_AUDIO_CodingAMRNB;   break;
			default:                      nACodec = 0;                      break;
			}
			VO_U32 nVCodec;
			switch(m_nVideoCoding) {
			case OMX_VIDEO_CodingMPEG2:   nVCodec = VO_VIDEO_CodingMPEG2;  break;
			case OMX_VIDEO_CodingH263:    nVCodec = VO_VIDEO_CodingH263;   break;
			case OMX_VIDEO_CodingWMV:     nVCodec = VO_VIDEO_CodingWMV;    break;
			case OMX_VIDEO_CodingRV:      nVCodec = VO_VIDEO_CodingRV;     break;
			case OMX_VIDEO_CodingMPEG4:   nVCodec = VO_VIDEO_CodingMPEG4;  break;
			case OMX_VIDEO_CodingAVC:	  nVCodec = VO_VIDEO_CodingH264;   break;
			default:                      nVCodec = 0;                     break;
			}

			if(nACodec == VO_AUDIO_CodingAMRNB && m_pcmType.nSamplingRate == 16000)
				nACodec = VO_AUDIO_CodingAMRWB;

			VOLOGI("nAudioDec %d  PCM sample rate : %d " , (int)nACodec , (int)m_pcmType.nSamplingRate);

			m_Source.nMode    = VO_FILE_WRITE_ONLY;
			m_Source.nReserve = (OMX_U32)m_pURI;
	
			VO_U32 nRC = m_pFileSink->Init (&m_Source, nACodec, nVCodec);
			if (nRC != VO_ERR_NONE) 
			{
				VOLOGE ("Name %s, m_pFileSink->Init was failed. 0X%08X", m_pName, (unsigned int)nRC);
				return OMX_ErrorUndefined;
			}

			m_pFileSink->SetParam(VO_PID_SINK_INTRA_DETECT_IFRAME , &mbIntraDetectIF);

			if (nACodec > 0)
			{
				VO_AUDIO_FORMAT	fmtAudio;
				fmtAudio.Channels   = m_pcmType.nChannels;
				fmtAudio.SampleRate = m_pcmType.nSamplingRate;
				fmtAudio.SampleBits = m_pcmType.nBitPerSample;
				nRC = m_pFileSink->SetParam (VO_PID_AUDIO_FORMAT, &fmtAudio);
				VOLOGI ("Name %s. Audio Codec %d Format: S %d, C %d", m_pName, (int)nACodec, (int)fmtAudio.SampleRate, (int)fmtAudio.Channels);
			}

			if (nVCodec > 0)
			{
				OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
				m_pVideoPort->GetPortType (&pPortType);
				VO_VIDEO_FORMAT fmtVideo;
				fmtVideo.Width = pPortType->format.video.nFrameWidth;
				fmtVideo.Height  = pPortType->format.video.nFrameHeight;
				nRC = m_pFileSink->SetParam (VO_PID_VIDEO_FORMAT, &fmtVideo);
				VOLOGI ("Name %s. Video Codec %d Format: W %d, H %d", m_pName, (int)nVCodec, (int)fmtVideo.Width, (int)fmtVideo.Height);
			}

			VO_SINK_EXTENSION filExtData;
			if (m_nVideoExtSize > 0)
			{
				filExtData.Buffer = (VO_BYTE*)m_pVideoExtData;
				filExtData.Size = m_nVideoExtSize;
				filExtData.nAV = 1;
				nRC = m_pFileSink->SetParam (VO_PID_SINK_EXTDATA, &filExtData);
			}

			if (m_nAudioExtSize > 0)
			{
				filExtData.Buffer = (VO_BYTE*)m_pAudioExtData;
				filExtData.Size = m_nAudioExtSize;
				filExtData.nAV = 0;
				nRC = m_pFileSink->SetParam (VO_PID_SINK_EXTDATA, &filExtData);
			}
		}
		
		m_pFileSink->Start ();

		if (m_pAudioPort != NULL && m_pAudioPort->IsEnable ())
			m_bAudioEOS = OMX_FALSE;
		else
			m_bAudioEOS = OMX_TRUE;

		if (m_pVideoPort != NULL && m_pVideoPort->IsEnable ())
			m_bVideoEOS = OMX_FALSE;
		else
			m_bVideoEOS = OMX_TRUE;

		m_bFinished = OMX_FALSE;

		return OMX_ErrorNone;
	}
	else if (m_sTrans == COMP_TRANSSTATE_IdleToLoaded || m_sTrans == COMP_TRANSSTATE_AnyToInvalid)
	{
		return OMX_ErrorNone;
	}
	else if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_ExecuteToIdle)
	{
		if (m_pFileSink != NULL)
			m_pFileSink->Stop ();
	}

	OMX_ERRORTYPE errType = voCOMXBaseComponent::SetNewState (newState);

	if (newState == OMX_StateIdle)
		ReturnKeptBuffers();

	return errType;
}

OMX_ERRORTYPE voCOMXFileSink::Flush(OMX_U32 nPort)
{
	ReturnKeptBuffers();

	return voCOMXBaseComponent::Flush(nPort);
}

OMX_ERRORTYPE voCOMXFileSink::Disable(OMX_U32 nPort)
{
	ReturnKeptBuffers();

	return voCOMXBaseComponent::Disable(nPort);
}

OMX_ERRORTYPE voCOMXFileSink::FreeBuffer(OMX_IN OMX_HANDLETYPE hComponent, OMX_IN OMX_U32 nPortIndex, OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
	{
		voCOMXAutoLock lock(&m_tmKeptBuffer);
		if(nPortIndex == 0 && pBuffer == m_pAudioKeptBuffer)
			m_pAudioKeptBuffer = NULL;
		else if(nPortIndex == 1 && pBuffer == m_pVideoKeptBuffer)
			m_pVideoKeptBuffer = NULL;
	}

	return voCOMXBaseComponent::FreeBuffer(hComponent, nPortIndex, pBuffer);
}

OMX_ERRORTYPE voCOMXFileSink::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											   OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	if (pBuffer == NULL)
		return OMX_ErrorBadParameter;

	OMX_U32 nPortIndex = pBuffer->nInputPortIndex;

	if (m_pFileSink == NULL)
		return m_ppPorts[nPortIndex]->ReturnBuffer (pBuffer);

	// handle the audio buffer
	voCOMXAutoLock lokc (&m_tmStatus);
	OMX_U32 nReturnBufferFlag = 1;	// 0 - not return any buffer
									// 1 - return self buffer
									// 2 - return other port buffer
									// 3 - return both self and other port buffer
	if(m_nDumpRuntimeLog)
	{
		VOLOGI("Port %d (0 audio, 1 video) Size %d, Time %lld", (int)nPortIndex, (int)pBuffer->nFilledLen, pBuffer->nTimeStamp);
	}

	VO_SINK_SAMPLE sSinkSample;
	memset(&sSinkSample, 0, sizeof(sSinkSample));
	sSinkSample.Buffer		= pBuffer->pBuffer;
	sSinkSample.Size		= pBuffer->nFilledLen;
	sSinkSample.Time		= pBuffer->nTimeStamp;
	sSinkSample.Duration	= 1;
	if(pBuffer->nFlags & OMX_BUFFERFLAG_SYNCFRAME)
		sSinkSample.Size |= 0x80000000;
	
	voCOMXAutoLock lock(&m_tmKeptBuffer);

	if(nPortIndex == 0)
	{
		// audio data
		if(pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
			m_bAudioEOS = OMX_TRUE;
		sSinkSample.nAV = 0;
		m_nCurrAudioTimeStamp = pBuffer->nTimeStamp;
		if(m_pVideoKeptBuffer)
		{
			// if video pause, check if we can re-start it
			if(m_nCurrAudioTimeStamp >= m_nCurrVideoTimeStamp || pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
				nReturnBufferFlag = 3;
		}

		if(m_nMaxAVDiffTime > 0 && (m_ppPorts[1] && m_ppPorts[1]->IsEnable()) && m_nCurrAudioTimeStamp > m_nCurrVideoTimeStamp + m_nMaxAVDiffTime)
		{
			m_pAudioKeptBuffer = pBuffer;
			if(nReturnBufferFlag == 3)
				nReturnBufferFlag = 2;
			else
				nReturnBufferFlag = 0;
		}
	}
	else
	{
		// video data
		if(pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
			m_bVideoEOS = OMX_TRUE;
		sSinkSample.nAV = 1;
		m_nCurrVideoTimeStamp = pBuffer->nTimeStamp;
		if(m_pAudioKeptBuffer)
		{
			// if audio pause, check if we can re-start it
			if(m_nCurrVideoTimeStamp >= m_nCurrAudioTimeStamp || pBuffer->nFlags & OMX_BUFFERFLAG_EOS)
				nReturnBufferFlag = 3;
		}

		if(m_nMaxAVDiffTime > 0 && (m_ppPorts[0] && m_ppPorts[0]->IsEnable()) && m_nCurrVideoTimeStamp > m_nCurrAudioTimeStamp + m_nMaxAVDiffTime)
		{
			m_pVideoKeptBuffer = pBuffer;
			if(nReturnBufferFlag == 3)
				nReturnBufferFlag = 2;
			else
				nReturnBufferFlag = 0;
		}
	}

	if (!m_bFinished)
		m_pFileSink->AddSample (&sSinkSample);

	if (m_bVideoEOS && m_bAudioEOS)
	{
		VOLOGI("It reach the end of source.");
		m_bFinished = OMX_TRUE;
		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_ComponentComplete, 0, 0, NULL);
	}
	else if(m_nMaxSize > 0)
	{
		m_nWriteSize += pBuffer->nFilledLen;
		if(m_nWriteSize >= m_nMaxSize)
		{
			VOLOGI("It reach the max size now. m_nMaxSize(%lld) < %u", m_nMaxSize, (unsigned int)m_nWriteSize);
			m_bFinished = OMX_TRUE;
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_ComponentMaxFileSize, 0, 0, NULL);
		}
	}
	else if(m_nMaxDuration > 0)
	{
		if(m_Start_ms == 0) 
		{
			m_Start_ms = (OMX_U32)pBuffer->nTimeStamp;
		}
		else
		{
			if(pBuffer->nTimeStamp - m_Start_ms >= m_nMaxDuration)
			{
				VOLOGI("It reach the max time now. m_nMaxSize(%lld) < %u", m_nMaxDuration, (unsigned int)(pBuffer->nTimeStamp - m_Start_ms));
				m_bFinished = OMX_TRUE;
				m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_ComponentMaxDuration, 0, 0, NULL);
			}
		}
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if(1 == nReturnBufferFlag)
		errType = m_ppPorts[nPortIndex]->ReturnBuffer(pBuffer);
	else if(2 == nReturnBufferFlag)
	{
		if(nPortIndex == 0)
		{
			errType = m_ppPorts[1]->ReturnBuffer(m_pVideoKeptBuffer);
			m_pVideoKeptBuffer = NULL;
		}
		else
		{
			errType = m_ppPorts[0]->ReturnBuffer(m_pAudioKeptBuffer);
			m_pAudioKeptBuffer = NULL;
		}
	}
	else if(3 == nReturnBufferFlag)
	{
		errType = m_ppPorts[nPortIndex]->ReturnBuffer(pBuffer);
		if(OMX_ErrorNone != errType)
		{
			VOLOGE("Return %d port buffer fail 0x%08X", (int)nPortIndex, errType);
			return errType;
		}

		if(nPortIndex == 0)
		{
			errType = m_ppPorts[1]->ReturnBuffer(m_pVideoKeptBuffer);
			m_pVideoKeptBuffer = NULL;
		}
		else
		{
			errType = m_ppPorts[0]->ReturnBuffer(m_pAudioKeptBuffer);
			m_pAudioKeptBuffer = NULL;
		}
	}

	return errType;
}

OMX_ERRORTYPE voCOMXFileSink::CreatePorts (void)
{
	if (m_uPorts == 0)
	{
		m_uPorts = 2;
		m_ppPorts = (voCOMXBasePort **)voOMXMemAlloc (m_uPorts * sizeof (voCOMXBasePort *));
		if (m_ppPorts == NULL)
			return OMX_ErrorInsufficientResources;

		m_pAudioPort = m_ppPorts[0] = new voCOMXBasePort (this, 0, OMX_DirInput);
		if (m_pAudioPort == NULL)
			return OMX_ErrorInsufficientResources;
		m_pAudioPort->SetCallbacks (m_pCallBack, m_pAppData);

		m_pVideoPort = m_ppPorts[1] = new voCOMXBasePort (this, 1, OMX_DirInput);
		if (m_pVideoPort == NULL)
			return OMX_ErrorInsufficientResources;
		m_pVideoPort->SetCallbacks (m_pCallBack, m_pAppData);

		InitPortType ();
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileSink::InitPortType (void)
{
	m_portParam[OMX_PortDomainAudio].nPorts = 1;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	m_portParam[OMX_PortDomainVideo].nPorts = 1;
	m_portParam[OMX_PortDomainVideo].nStartPortNumber = 1;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;

	m_pAudioPort->GetPortType (&pPortType);
	pPortType->eDir                               = OMX_DirInput;
	pPortType->eDomain                            = OMX_PortDomainAudio;
	pPortType->nBufferCountActual                 = 1;
	pPortType->nBufferCountMin                    = 1;
	pPortType->nBufferSize                        = 44100;
	pPortType->bBuffersContiguous                 = OMX_FALSE;
	pPortType->nBufferAlignment                   = 1;
	pPortType->format.audio.pNativeRender         = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.audio.eEncoding             = OMX_AUDIO_CodingUnused;

	m_pVideoPort->GetPortType (&pPortType);
	pPortType->eDir                               = OMX_DirInput;
	pPortType->eDomain            = OMX_PortDomainVideo;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin    = 1;
	pPortType->nBufferSize        = 102400;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment   = 1;

	pPortType->format.video.nFrameWidth            = 320;
	pPortType->format.video.nFrameHeight           = 240;
	pPortType->format.video.nStride                = 320;
	pPortType->format.video.nSliceHeight           = 16;
	pPortType->format.video.nBitrate               = 0;
	pPortType->format.video.xFramerate             = 0;
	pPortType->format.video.bFlagErrorConcealment  = OMX_FALSE;
	pPortType->format.video.eCompressionFormat     = OMX_VIDEO_CodingUnused;
	pPortType->format.video.eColorFormat           = OMX_COLOR_FormatYUV420Planar;
	pPortType->format.video.pNativeWindow          = NULL;
	pPortType->format.video.pNativeRender          = NULL;

	voOMXBase_SetHeader (&m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	m_pcmType.nPortIndex    = 1;
	m_pcmType.nChannels     = 1;
	m_pcmType.eNumData      = OMX_NumericalDataSigned;
	m_pcmType.eEndian       = OMX_EndianBig;
	m_pcmType.bInterleaved  = OMX_TRUE;
	m_pcmType.nBitPerSample = 16;
	m_pcmType.nSamplingRate = 8000;
	m_pcmType.ePCMMode      = OMX_AUDIO_PCMModeLinear;

	return OMX_ErrorNone;
}

void voCOMXFileSink::ReturnKeptBuffers()
{
	voCOMXAutoLock lock(&m_tmKeptBuffer);

	if(m_pAudioKeptBuffer)
	{
		m_ppPorts[0]->ReturnBuffer(m_pAudioKeptBuffer);
		m_pAudioKeptBuffer = NULL;
	}

	if(m_pVideoKeptBuffer)
	{
		m_ppPorts[1]->ReturnBuffer(m_pVideoKeptBuffer);
		m_pVideoKeptBuffer = NULL;
	}
}
