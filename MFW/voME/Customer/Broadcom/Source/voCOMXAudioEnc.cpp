	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioEnc.cpp

	Contains:	voCOMXAudioEnc class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"

#include "voCOMXAudioEnc.h"

#include "voLog.h"

#ifndef _LINUX
#pragma warning (disable : 4996)
#endif

voCOMXAudioEnc::voCOMXAudioEnc(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_AUDIO_CodingAMR)
	, m_pFormatData (NULL)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Encoder.XXX");

	VOLOGF ("Name: %s", m_pName);
}

voCOMXAudioEnc::~voCOMXAudioEnc(void)
{
	VOLOGF ("Name: %s", m_pName);

	if (m_pFormatData != NULL)
	{
		voOMXMemFree (m_pFormatData);
		m_pFormatData = NULL;
	}
}


OMX_ERRORTYPE voCOMXAudioEnc::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s. The Param Index is 0X%08X", m_pName, nParamIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nParamIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = voOMXBase_CheckHeader (pAudioFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pAudioFormat->nPortIndex == 0)
			{
				pAudioFormat->eEncoding = OMX_AUDIO_CodingPCM;
				errType = OMX_ErrorNone;
			}
			else if (pAudioFormat->nPortIndex == 1)
			{
				pAudioFormat->eEncoding = m_nCoding;
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

	case OMX_IndexParamAudioMp3:
		{
			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)pComponentParameterStructure;

			pMP3Format->nChannels = m_pcmType.nChannels;
			pMP3Format->nSampleRate = m_pcmType.nSamplingRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE*)pComponentParameterStructure;

			pAmrFormat->nChannels = m_pcmType.nChannels;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)pComponentParameterStructure;

			pAACFormat->nChannels = m_pcmType.nChannels;
			pAACFormat->nSampleRate = m_pcmType.nSamplingRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioQcelp13:
		{
			OMX_AUDIO_PARAM_QCELP13TYPE * pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE*)pComponentParameterStructure;

			pQcelp13Format->nChannels = m_pcmType.nChannels;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioQcelp8:
		{
			OMX_AUDIO_PARAM_QCELP8TYPE * pQcelp8Format = (OMX_AUDIO_PARAM_QCELP8TYPE*)pComponentParameterStructure;

			pQcelp8Format->nChannels = m_pcmType.nChannels;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioEnc::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s. The Param Index is 0X%08X", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam (pComp, pAudioFormat->nPortIndex, pAudioFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pAudioFormat->nPortIndex == 0)
			{
				if (pAudioFormat->eEncoding == OMX_AUDIO_CodingPCM)
					errType = OMX_ErrorNone;
			}
			else if (pAudioFormat->nPortIndex == 1)
			{
				m_nCoding = pAudioFormat->eEncoding;

				if (m_nCoding == OMX_AUDIO_CodingAMR) {
					errType = OMX_ErrorNone;
				} else if (m_nCoding == OMX_AUDIO_CodingQCELP13){
					errType = OMX_ErrorNone;
				} else if (m_nCoding == OMX_AUDIO_CodingEVRC) {
					errType = OMX_ErrorNone;
				} else if (m_nCoding == OMX_AUDIO_CodingAAC) {
					errType = OMX_ErrorNone;
				} else if (m_nCoding == OMX_AUDIO_CodingMP3) {
					errType = OMX_ErrorNone;
				} else {
					errType = OMX_ErrorComponentNotFound;
				}
			}
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE*)pComponentParameterStructure;
			errType = CheckParam (pComp, pPCMFormat->nPortIndex, pPCMFormat, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_pcmType.nChannels     = pPCMFormat->nChannels;
			m_pcmType.nSamplingRate = pPCMFormat->nSamplingRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioMp3:
		{
			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE *)pComponentParameterStructure;

			errType = CheckParam (pComp, pMP3Format->nPortIndex, pMP3Format, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_MP3TYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_MP3TYPE));

			pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)m_pFormatData;

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

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_AMRTYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_AMRTYPE));

			pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE*)m_pFormatData;

			m_pcmType.nChannels     = pAmrFormat->nChannels;
			m_pcmType.nSamplingRate = 8000;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pAACFormat->nPortIndex, pAACFormat, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));

			pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)m_pFormatData;

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

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_QCELP13TYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_QCELP13TYPE));

			pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE*)m_pFormatData;

			m_pcmType.nChannels = pQcelp13Format->nChannels;
			m_pcmType.nSamplingRate = 8000;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;

			errType = voOMXBase_CheckHeader(pRoleType, sizeof(OMX_PARAM_COMPONENTROLETYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			m_nCoding = OMX_AUDIO_CodingUnused;
			if (!strcmp ((char *)pRoleType->cRole, "audio_encoder.aac"))
				m_nCoding = OMX_AUDIO_CodingAAC;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_encoder.mp3"))
				m_nCoding = OMX_AUDIO_CodingMP3;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_encoder.amrnb"))
				m_nCoding = OMX_AUDIO_CodingAMR;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_encoder.qcelp13"))
				m_nCoding = OMX_AUDIO_CodingQCELP13;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_encoder.evrc"))
				m_nCoding = OMX_AUDIO_CodingEVRC;

			if (m_nCoding != OMX_AUDIO_CodingUnused)
				errType = OMX_ErrorNone;
			else
				errType = OMX_ErrorInvalidComponent;
		}
		break;

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioEnc::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");

	if (nIndex == 0)
		strcpy ((char *)cRole, "audio_encoder.amrnb");
	else if (nIndex == 1)
		strcpy ((char *)cRole, "audio_encoder.aac");
	else if (nIndex == 2)
		strcpy ((char *)cRole, "audio_encoder.mp3");
	else if (nIndex == 3)
		strcpy ((char *)cRole, "audio_encoder.qcelp13");
	else if (nIndex == 4)
		strcpy ((char *)cRole, "audio_encoder.evrc");
	else
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioEnc::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. The new state is %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{

	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
	}

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioEnc::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);

	OMX_U32 nRC = 0;

/*

	if (nRC == VO_ERR_INPUT_BUFFER_SMALL)
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
	else if (nRC == VO_ERR_NONE && m_outBuffer.Length > 0)
	{
		pOutput->nFilledLen = m_outBuffer.Length;

		pOutput->nTimeStamp = m_nStartTime + (m_outInfo.InputUsed * 1000) / (m_pcmType.nSamplingRate * m_pcmType.nChannels * 2);;

		*pFilled = OMX_TRUE;
	}
	else
	{
		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
*/
	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioEnc::InitPortType (void)
{
	VOLOGF ("Name: %s.", m_pName);

	m_portParam[OMX_PortDomainAudio].nPorts = 2;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 44100;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	m_pOutput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 1;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 44100;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;

	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;

	pPortType->format.audio.eEncoding = m_nCoding;

	voOMXBase_SetHeader (&m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	m_pcmType.nPortIndex = 1;
	m_pcmType.nChannels = 1;
	m_pcmType.eNumData = OMX_NumericalDataSigned;
	m_pcmType.eEndian = OMX_EndianBig;
	m_pcmType.bInterleaved = OMX_TRUE;
	m_pcmType.nBitPerSample = 16;
	m_pcmType.nSamplingRate = 8000;
	m_pcmType.ePCMMode = OMX_AUDIO_PCMModeLinear;

	return OMX_ErrorNone;
}
