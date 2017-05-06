	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXMediaAudioPort.cpp

	Contains:	voCOMXMediaAudioPort class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMX_Index.h"
#include <voOMXBase.h>
#include <voOMXMemory.h>

#include "voCOMXMediaSource.h"
#include "voCOMXMediaAudioPort.h"

#include "voLog.h"

voCOMXMediaAudioPort::voCOMXMediaAudioPort(voCOMXBaseComponent * pParent, OMX_S32 nIndex)
	: voCOMXMediaOutputPort (pParent, nIndex)
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

voCOMXMediaAudioPort::~voCOMXMediaAudioPort(void)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);
}

OMX_ERRORTYPE voCOMXMediaAudioPort::SetTrackInfo (OMX_S32 nTrackIndex, VO_SOURCE_TRACKINFO *pTrackInfo)
{
	VOLOGF ("Comp %s, Index %d", m_pParent->GetName (), m_sType.nPortIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	errType = voCOMXMediaOutputPort::SetTrackInfo (nTrackIndex, pTrackInfo);
	if (errType != OMX_ErrorNone)
		return errType;

	//m_pSource->GetAudioFormat (m_nTrackIndex, &m_fmtAudio);

	OMX_U32	nBufferSize = 40960;
	//VO_U32	nRC = m_pSource->GetTrackParam (m_nTrackIndex, VO_PID_SOURCE_MAXSAMPLESIZE, &nBufferSize);
	//if (nRC == VO_ERR_NONE)
	//	m_sType.nBufferSize = nBufferSize + 10240;
	//else
		m_sType.nBufferSize = nBufferSize;

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
	else if (m_trkInfo.Codec == VO_AUDIO_CodingPCM)
	{
		strcpy (m_pMIMEType, "PCM");
		m_sType.format.audio.eEncoding = OMX_AUDIO_CodingPCM;
	}

	m_pcmType.nChannels = m_fmtAudio.Channels;
	m_pcmType.nBitPerSample = m_fmtAudio.SampleBits;
	m_pcmType.nSamplingRate = m_fmtAudio.SampleRate;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXMediaAudioPort::GetParameter (OMX_INDEXTYPE nIndex, OMX_PTR pParam)
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

			if (m_nFourCC == 0XFF)
				pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP2ADTS;
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

	return voCOMXMediaOutputPort::GetParameter (nIndex, pParam);
}
