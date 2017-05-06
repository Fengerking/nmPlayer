	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXFileAudioPort.cpp

	Contains:	voCOMXFileAudioPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voCOMXFileSource.h"
#include "voCOMXFileAudioPort.h"

#define LOG_TAG "voCOMXFileAudioPort"
#include "voLog.h"

voCOMXFileAudioPort::voCOMXFileAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXFileOutputPort (pParent, nIndex)
	, m_bDownMix (OMX_FALSE)
	, m_nBitrate (0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	m_sType.eDomain = OMX_PortDomainAudio;

	m_fmtAudio.Channels = 2;
	m_fmtAudio.SampleRate = 44100;
	m_fmtAudio.SampleBits = 16;

	voOMXBase_SetHeader (&m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	m_pcmType.nPortIndex = nIndex;
	m_pcmType.nChannels = m_fmtAudio.Channels;
	m_pcmType.eNumData = OMX_NumericalDataSigned;
	m_pcmType.eEndian = OMX_EndianBig;
	m_pcmType.bInterleaved = OMX_TRUE;
	m_pcmType.nBitPerSample = m_fmtAudio.SampleBits;
	m_pcmType.nSamplingRate = m_fmtAudio.SampleRate;
	m_pcmType.ePCMMode = OMX_AUDIO_PCMModeLinear;
}

voCOMXFileAudioPort::~voCOMXFileAudioPort(void)
{
}

OMX_ERRORTYPE voCOMXFileAudioPort::SetTrack (CBaseSource * pSource, OMX_S32 nTrackIndex)
{
	OMX_ERRORTYPE errType = voCOMXFileOutputPort::SetTrack (pSource, nTrackIndex);
	if (errType != OMX_ErrorNone)
		return errType;

	m_pSource->GetAudioFormat (m_nTrackIndex, &m_fmtAudio);

	OMX_U32	nBufferSize = 40960;
	VO_U32	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_MAXSAMPLESIZE, &nBufferSize);
	if (nRC == VO_ERR_NONE)
		m_sType.nBufferSize = nBufferSize + 10240;
	else
		m_sType.nBufferSize = 40960;

	VO_U32 nBitrate = 0;
	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_BITRATE, &nBitrate);
	if(VO_ERR_NONE == nRC && nBitrate > 0)
		m_nBitrate = nBitrate * 8;	// History reason, VO_PID_SOURCE_BITRATE is for Byte/Sec

	m_sType.nBufferCountActual = 2;
	m_sType.nBufferCountMin = 1;
	m_sType.bBuffersContiguous = OMX_FALSE;
	m_sType.nBufferAlignment = 1;

	m_sType.format.audio.pNativeRender = NULL;
	m_sType.format.audio.bFlagErrorConcealment = OMX_FALSE;
	m_sType.format.audio.cMIMEType = m_pMIMEType;

	if (m_trkInfo.Codec == VO_AUDIO_CodingAMRNB)
	{
		strcpy (m_pMIMEType, "AMR");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingAMR;
		m_fmtAudio.Channels = 1;
		m_fmtAudio.SampleRate = 8000;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingAMRWB)
	{
		strcpy (m_pMIMEType, "AWB");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingAMR;
		//m_fmtAudio.Channels = 1;
		//m_fmtAudio.SampleRate = 8000;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingAAC)
	{
		strcpy (m_pMIMEType, "AAC");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingAAC;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingMP3)
	{
		strcpy (m_pMIMEType, "MP3");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingMP3;
	}
	//support MP1 audio format
	//East 2009/12/7
	else if (m_trkInfo.Codec == VO_AUDIO_CodingMP1)
	{
		strcpy (m_pMIMEType, "MP1");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingMP3;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingWMA)
	{
		strcpy (m_pMIMEType, "WMA");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingWMA;

		VO_U32 nRC = m_pSource->GetTrackParam (nTrackIndex, VO_PID_SOURCE_WAVEFORMATEX, &m_trkInfo.HeadData);
		if (nRC == VO_ERR_NONE)
		{
			VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)m_trkInfo.HeadData;
			m_trkInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
		}
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingADPCM)
	{
		strcpy (m_pMIMEType, "APCM");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingADPCM;

		VO_U32 nRC = m_pSource->GetTrackParam (nTrackIndex, VO_PID_SOURCE_WAVEFORMATEX, &m_trkInfo.HeadData);
		if (nRC == VO_ERR_NONE)
		{
			VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)m_trkInfo.HeadData;
			m_trkInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
		}
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingQCELP13)
	{
		strcpy (m_pMIMEType, "QP13");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingQCELP13;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingEVRC)
	{
		strcpy (m_pMIMEType, "EVRC");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingEVRC;
	}
	//else if (m_trkInfo.Codec == VO_AUDIO_CodingAC3)
	//{
	//	VO_BOOL bEac3 = VO_FALSE;

	//	//AVI file EAC3 information is same to AC3
	//	//So we should parse first sample to get if it is EAC3 actually
	//	VO_SOURCE_SAMPLE sample;
	//	voOMXMemSet(&sample, 0, sizeof(sample));
	//	VO_U32 nRC = m_pSource->GetTrackParam(m_nTrackIndex, VO_PID_SOURCE_FIRSTFRAME, &sample);
	//	if(nRC == VO_ERR_NONE)
	//	{
	//		VO_U32 dwLen = sample.Size & 0x7FFFFFFF;
	//		if(dwLen > 5)
	//		{
	//			VO_PBYTE pHead = sample.Buffer;
	//			VO_PBYTE pTail = sample.Buffer + dwLen - 5;
	//			VO_U32 dwTimes = 0;

	//			while(pHead < pTail)
	//			{
	//				if(0xB != pHead[0])
	//				{
	//					pHead++;
	//					continue;
	//				}

	//				if(0x77 != pHead[1])
	//				{
	//					pHead += 2;
	//					continue;
	//				}

	//				if(((pHead[5] >> 3) & 0x1F) == 0x10)
	//				{
	//					bEac3 = VO_TRUE;
	//					break;
	//				}
	//				
	//				pHead += 6;
	//				dwTimes++;
	//				if(dwTimes >= 2)
	//					break;
	//			}

	//			if(bEac3 == VO_FALSE) {
	//				int channel = 0;
	//				int acmod = 0;
	//				int lfeon = 0;
	//				int bits = 0;

	//				acmod = (pHead[6] >> 5) & 0x7;
	//				bits = 3;
	//				if ((acmod != 1) && (acmod & 0x1))
	//				{
	//					bits += 2;
	//				}
	//				if (acmod & 0x4)
	//				{
	//					bits += 2;
	//				}
	//				if (acmod == 2)
	//				{
	//					bits += 2;
	//				}

	//				lfeon = pHead[6] & (1 << (7 - bits));

	//				static const int channelNum[8] = {2, 1, 2, 3, 3, 4, 4, 5}; 

	//				channel = channelNum[acmod] + lfeon;

	//				m_fmtAudio.Channels = channel;
	//			}
	//		}
	//	}

	//	if(bEac3 == VO_FALSE)
	//	{
	//		strcpy (m_pMIMEType, "AC3");
	//		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3;
	//	}
	//	else
	//	{
	//		strcpy (m_pMIMEType, "EAC3");
	//		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3;
	//	}
	//}
	// AVI file format has the same FourCC for EAC3 and AC3, now default is AC3, and check it at 
	// CAudioDecoder::HandleFirstSample. Fix #9625 issue.
	else if (m_trkInfo.Codec == VO_AUDIO_CodingAC3)
	{
		strcpy (m_pMIMEType, "AC3");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingEAC3)
	{
		strcpy (m_pMIMEType, "EAC3");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingPCM)
	{
		strcpy (m_pMIMEType, "PCM");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingPCM;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingAMRWBP)
	{
		strcpy (m_pMIMEType, "AWBP");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAMRWBP;
	}
	else if (m_trkInfo.Codec == VO_AUDIO_CodingRA)
	{
		strcpy (m_pMIMEType, "RA");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_AUDIO_CodingRA;
	}
	else if(m_trkInfo.Codec == VO_AUDIO_CodingAPE)
	{
		strcpy (m_pMIMEType, "MAC ");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAPE;
	}
	else if(m_trkInfo.Codec == VO_AUDIO_CodingALAC)
	{
		strcpy (m_pMIMEType, "ALAC");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingALAC;
	}
	else if(m_trkInfo.Codec == VO_AUDIO_CodingDTS)
	{
		strcpy (m_pMIMEType, "DTS");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingDTS;
	}
	else if(m_trkInfo.Codec == VO_AUDIO_CodingOGG)
	{
		strcpy (m_pMIMEType, "OGG");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_AUDIO_CodingVORBIS;
	}
	else if(m_trkInfo.Codec == VO_AUDIO_CodingFLAC)
	{
		strcpy (m_pMIMEType, "FLAC");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingFlac;
	}

	m_pcmType.nChannels = m_fmtAudio.Channels;
	m_pcmType.nBitPerSample = m_fmtAudio.SampleBits;
	m_pcmType.nSamplingRate = m_fmtAudio.SampleRate;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXFileAudioPort::SetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	if(nIndex == OMX_IndexParamAudioPcm)
	{
		OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pParam;
		if (pPCMFormat->nPortIndex != m_sType.nPortIndex)
			return OMX_ErrorPortsNotCompatible;

		m_pcmType.nChannels = pPCMFormat->nChannels;

		if(m_fmtAudio.Channels > 2 && m_pcmType.nChannels <= 2)
			m_bDownMix = OMX_TRUE; 

		return OMX_ErrorNone;
	}

	return voCOMXFileOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXFileAudioPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
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
			pMP3Format->nChannels = m_fmtAudio.Channels;
			pMP3Format->nSampleRate = m_fmtAudio.SampleRate;

			pMP3Format->nBitRate = m_nBitrate;
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

			pRAFormat->nChannels = m_fmtAudio.Channels;
			pRAFormat->nSamplingRate = m_fmtAudio.SampleRate;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingAAC)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE *) pParam;
			pAACFormat->nChannels = m_fmtAudio.Channels;
			pAACFormat->nSampleRate = m_fmtAudio.SampleRate;

			pAACFormat->nBitRate = m_nBitrate;
			pAACFormat->nAudioBandWidth = m_fmtAudio.SampleBits; //send the samplebits use this ID

			pAACFormat->eChannelMode = OMX_AUDIO_ChannelModeStereo;

			pAACFormat->eAACProfile = OMX_AUDIO_AACObjectLC;

			if (m_nFourCC == 0XF6)
				pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatADIF;
			else if (m_nFourCC == 0XF7)
				pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP2ADTS;
			else if (m_nFourCC == 0xF8)
				pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4LATM;
			else if (m_nFourCC == 0xF9)
				pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP4LOAS;
			else
				pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatRAW;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingAMR)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE *) pParam;
			pAmrFormat->nChannels = m_fmtAudio.Channels;
			pAmrFormat->nBitRate = m_nBitrate;

			if (m_trkInfo.Codec == VO_AUDIO_CodingAMRNB)
				pAmrFormat->eAMRBandMode = OMX_AUDIO_AMRBandModeNB0;
			else if (m_trkInfo.Codec == VO_AUDIO_CodingAMRWB)
				pAmrFormat->eAMRBandMode = OMX_AUDIO_AMRBandModeWB0;

			pAmrFormat->eAMRFrameFormat = OMX_AUDIO_AMRFrameFormatFSF;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioWma:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingWMA)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_WMATYPE * pWMAFormat = (OMX_AUDIO_PARAM_WMATYPE *) pParam;
			pWMAFormat->nChannels = (OMX_U16)m_fmtAudio.Channels;
			pWMAFormat->nSamplingRate =(OMX_U32) m_fmtAudio.SampleRate;
			pWMAFormat->nBitRate = m_nBitrate;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAdpcm:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingADPCM)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_ADPCMTYPE * pADPCMFormat = (OMX_AUDIO_PARAM_ADPCMTYPE *) pParam;
			pADPCMFormat->nChannels = m_fmtAudio.Channels;
			pADPCMFormat->nSampleRate = m_fmtAudio.SampleRate;
			pADPCMFormat->nBitsPerSample = m_fmtAudio.SampleBits;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioQcelp13:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingQCELP13)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_QCELP13TYPE * pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE *) pParam;
			pQcelp13Format->nChannels = m_fmtAudio.Channels;

			return OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioEvrc:
		{
			if (m_sType.format.audio.eEncoding != OMX_AUDIO_CodingEVRC)
				return OMX_ErrorPortsNotCompatible;

			OMX_AUDIO_PARAM_EVRCTYPE * pEVRCFormat = (OMX_AUDIO_PARAM_EVRCTYPE *) pParam;
			pEVRCFormat->nChannels = m_fmtAudio.Channels;

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
				//if (m_pcmType.nChannels > 2)
				//	m_pcmType.nChannels = 2;

				voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
			}
			else
			{
				OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pParam;
				pPCMFormat->nChannels = m_fmtAudio.Channels;
				pPCMFormat->nBitPerSample = m_fmtAudio.SampleBits;
				pPCMFormat->nSamplingRate = m_fmtAudio.SampleRate;
			}

			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexAudioParamDTS:
		{
			if (m_sType.format.audio.eEncoding != (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS)
				return OMX_ErrorPortsNotCompatible;

			OMX_VO_AUDIO_PARAM_DTSTYPE * pDTSFormat = (OMX_VO_AUDIO_PARAM_DTSTYPE *) pParam;
			pDTSFormat->nChannels = m_fmtAudio.Channels;
			pDTSFormat->nSampleRate = m_fmtAudio.SampleRate;
			pDTSFormat->nBitRate = m_nBitrate;

			return OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexAudioParamAC3:
		{
			if (m_sType.format.audio.eEncoding != (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3 && m_sType.format.audio.eEncoding != (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingEAC3)
				return OMX_ErrorPortsNotCompatible;

			OMX_VO_AUDIO_PARAM_AC3TYPE * pAC3Format = (OMX_VO_AUDIO_PARAM_AC3TYPE *) pParam;
			pAC3Format->nChannels = m_fmtAudio.Channels;
			pAC3Format->nSampleRate = m_fmtAudio.SampleRate;
			pAC3Format->nBitRate = m_nBitrate;

			return OMX_ErrorNone;
		}
		break;

	default:
		break;
	}

	return voCOMXFileOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXFileAudioPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	if ((m_nLogLevel & 0XF0) > 0)
	{
		if (m_nLogSysStartTime == 0)
			m_nLogSysStartTime = voOMXOS_GetSysTime ();

		VOLOGI ("Sys Time: %d", (int)(voOMXOS_GetSysTime () - m_nLogSysStartTime));

		m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
	}

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	if (m_pParent->SetPosInPause ())
	{
		pBuffer->nFilledLen = 0;
		voOMXOS_Sleep (2);

		return OMX_ErrorNone;
	}

	if (m_pVideoPort != NULL)
	{
//		if (!m_pVideoPort->IsEOS () && m_pVideoPort->GetCurrentPos () < m_nStartPos)
		if (!m_pVideoPort->IsEOS () && m_pVideoPort->IsDecOnly ())
		{
			pBuffer->nFilledLen = 0;
			voOMXOS_Sleep (2);

			return OMX_ErrorNone;
		}

		if (m_pParent->TranscodeMode ())
		{
			if (!m_pVideoPort->IsEOS () && m_nCurrentPos >= m_pVideoPort->GetCurrentPos ())
			{
				pBuffer->nFilledLen = 0;
				voOMXOS_Sleep (2);

				return OMX_ErrorNone;
			}
		}
	}

	errType = voCOMXFileOutputPort::FillBuffer (pBuffer);
	if (pBuffer->nTimeStamp < m_nSeekPos && m_nSeekMode == OMX_TIME_SeekModeAccurate)
	{
		pBuffer->nFlags = pBuffer->nFlags | OMX_BUFFERFLAG_DECODEONLY;
	}
	else
	{
		m_nSeekPos = 0;
	}

	if ((m_nLogLevel & 0XF0) > 0)
	{
		VOLOGI ("Read video used time is %d, Sample Size %d, Time: %d, Result 0X%08X", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime), (int)(m_Sample.Size & 0X7FFFFFFF), (int)m_Sample.Time, errType);
	}

	if(m_bDownMix && m_trkInfo.Codec == VO_AUDIO_CodingPCM)
	{
		errType = DownMixBuffer(pBuffer);				
	}

	return errType;
}


OMX_ERRORTYPE voCOMXFileAudioPort::DownMixBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;
	OMX_U32 i;
	OMX_S16* InBuf = (OMX_S16*)pBuffer->pBuffer;
	OMX_S16* OutBuf = (OMX_S16*)pBuffer->pBuffer;
	OMX_U32	 Length = pBuffer->nFilledLen/(m_fmtAudio.Channels*sizeof(OMX_S16));
	OMX_U32  InChan = m_fmtAudio.Channels;
	OMX_U32  OutChan = m_pcmType.nChannels;

	if(InChan == 6 && OutChan == 2)
	{

		OMX_S32 C,L_S,R_S,tmp,tmp1,cum;
			
	
#define DM_MUL 5248/16384  //3203/10000
#define RSQRT2 5818/8192	//7071/10000
#define CLIPTOSHORT(x)  ((((x) >> 31) == (x >> 15))?(x):((x) >> 31) ^ 0x7fff)
		
		for(i = 0; i < Length; i++)
		{
			C   = InBuf[2]*RSQRT2;
			L_S = InBuf[4]*RSQRT2;
			cum = InBuf[0] + C + L_S;
			tmp = cum*DM_MUL;

			R_S = InBuf[5]*RSQRT2;
			cum = InBuf[1] + C + R_S;
			tmp1 = cum*DM_MUL;

			OutBuf[0] = (OMX_S16)CLIPTOSHORT(tmp);
			OutBuf[1] = (OMX_S16)CLIPTOSHORT(tmp1);
			OutBuf+=OutChan;
			InBuf+=InChan;
		}

		pBuffer->nFilledLen = Length*sizeof(OMX_S16)*m_pcmType.nChannels;
	}
	else if(InChan > 2 && InChan != 6 && OutChan == 2)
	{
		for(i = 0; i < Length; i++)
		{
			OutBuf[0] = InBuf[0];
			OutBuf[1] = InBuf[1];
			OutBuf += OutChan;
			InBuf += InChan;
		}

		pBuffer->nFilledLen = Length*sizeof(OMX_S16)*m_pcmType.nChannels;		
	}


	return errType;
}
