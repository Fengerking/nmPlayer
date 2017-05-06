	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXRTSPAudioPort.cpp

	Contains:	voCOMXRTSPAudioPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMX_Index.h"
#include <voOMXBase.h>
#include <voOMXMemory.h>
#include <voOMXOSFun.h>

#include "voCOMXRTSPSource.h"
#include "voCOMXRTSPAudioPort.h"

#include "voLog.h"

voCOMXRTSPAudioPort::voCOMXRTSPAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXRTSPOutputPort (pParent, nIndex)
{
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

	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

voCOMXRTSPAudioPort::~voCOMXRTSPAudioPort(void)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

OMX_ERRORTYPE voCOMXRTSPAudioPort::SetTrack (CFileSource * pSource, OMX_S32 nTrackIndex)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = voCOMXRTSPOutputPort::SetTrack (pSource, nTrackIndex);
	if (errType != OMX_ErrorNone)
		return errType;

	m_pSource->GetAudioFormat (m_nTrackIndex, &m_fmtAudio);

	OMX_U32	nBufferSize = 40960;
	VO_U32	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_MAXSAMPLESIZE, &nBufferSize);
	if (nRC == VO_ERR_NONE)
		m_sType.nBufferSize = nBufferSize + 10240;
	else
		m_sType.nBufferSize = 40960;

	m_sType.nBufferCountActual = 2;
	m_sType.nBufferCountMin = 1;
	m_sType.bBuffersContiguous = OMX_FALSE;
	m_sType.nBufferAlignment = 1;

	m_sType.format.audio.pNativeRender = NULL;
	m_sType.format.audio.bFlagErrorConcealment = OMX_FALSE;

	m_sType.format.audio.cMIMEType = m_pMIMEType;

//	VOLOGI ("Audio Codec %d ", m_trkInfo.Codec);

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
	else if (m_trkInfo.Codec == VO_AUDIO_CodingAC3)
	{
		strcpy (m_pMIMEType, "AC3");
		m_sType.format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3;
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

	m_pcmType.nChannels = m_fmtAudio.Channels;
	m_pcmType.nBitPerSample = m_fmtAudio.SampleBits;
	m_pcmType.nSamplingRate = m_fmtAudio.SampleRate;

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXRTSPAudioPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	errType = errType;
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

			pMP3Format->nBitRate = 0;
			pMP3Format->nAudioBandWidth = 0;
			pMP3Format->eChannelMode = OMX_AUDIO_ChannelModeStereo;
			pMP3Format->eFormat = OMX_AUDIO_MP3StreamFormatMP1Layer3;

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

			pAACFormat->nBitRate = 0;
			pAACFormat->nAudioBandWidth = 0;

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
			pAmrFormat->nBitRate = 0;

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
			pWMAFormat->nChannels = m_fmtAudio.Channels;
			pWMAFormat->nSamplingRate = m_fmtAudio.SampleRate;

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

	default:
		break;
	}

	return voCOMXRTSPOutputPort::GetParameter (nIndex, pParam);
}

OMX_ERRORTYPE voCOMXRTSPAudioPort::FillBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if (m_pVideoPort != NULL)
	{
//		if (!m_pVideoPort->IsEOS () && m_pVideoPort->GetCurrentPos () < m_nStartPos)
		if (!m_pVideoPort->IsEOS () && m_pVideoPort->IsDecOnly ())
		{
			pBuffer->nFilledLen = 0;
			voOMXOS_Sleep (2);

			return OMX_ErrorNone;
		}
	}

	errType = voCOMXRTSPOutputPort::FillBuffer (pBuffer);

	return errType;
}
