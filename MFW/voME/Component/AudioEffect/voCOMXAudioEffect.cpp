	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioEffect.cpp

	Contains:	voCOMXAudioEffect class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"

#include "voCOMXAudioEffect.h"

#include "voLog.h"

voCOMXAudioEffect::voCOMXAudioEffect(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompInplace (pComponent)
	, m_nSampleRate (44100)
	, m_nChannels (2)
	, m_nBits (16)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Effect");


}

voCOMXAudioEffect::~voCOMXAudioEffect(void)
{

}

OMX_ERRORTYPE voCOMXAudioEffect::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{


	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nParamIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			pAudioFormat->eEncoding = OMX_AUDIO_CodingPCM;
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
			voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompInplace::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioEffect::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{


	OMX_ERRORTYPE errType = OMX_ErrorNone;

	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if(pAudioFormat->eEncoding != OMX_AUDIO_CodingPCM)
				return OMX_ErrorPortsNotCompatible;
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE*)pComponentParameterStructure;
			m_nChannels = pPCMFormat->nChannels;
			m_nSampleRate = pPCMFormat->nSamplingRate;

			m_pcmType.nChannels = m_nChannels;
			m_pcmType.nSamplingRate = m_nSampleRate;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompInplace::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioEffect::ModifyBuffer (OMX_BUFFERHEADERTYPE * pBuffer)
{
	OMX_ERRORTYPE errType = OMX_ErrorNone;

	OMX_S16 * pValue = (OMX_S16 *)pBuffer->pBuffer + pBuffer->nOffset;
	for (OMX_U32 i = 0; i < pBuffer->nFilledLen / 2; i++) {
		//*pValue = *pValue++ / 2;
		*pValue >>= 1;
		pValue++;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioEffect::InitPortType (void)
{


	m_portParam[OMX_PortDomainAudio].nPorts = 2;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 10240;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	m_pOutput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 1;
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
