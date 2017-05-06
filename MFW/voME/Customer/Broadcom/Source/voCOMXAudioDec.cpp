	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioDec.cpp

	Contains:	voCOMXAudioDec class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"

#include "voCOMXAudioDec.h"

#include "voLog.h"

voCOMXAudioDec::voCOMXAudioDec(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_AUDIO_CodingUnused)
	, m_pFormatData (NULL)
	, m_nExtSize (0)
	, m_pExtData (NULL)
	, m_nSampleRate (44100)
	, m_nChannels (2)
	, m_nBits (16)
	, m_bSetThreadPriority (OMX_FALSE)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Decoder.XXX");

	VOLOGF ("Name: %s", m_pName);
}

voCOMXAudioDec::~voCOMXAudioDec(void)
{
	VOLOGF ("Name: %s", m_pName);

	if (m_pFormatData != NULL)
	{
		voOMXMemFree (m_pFormatData);
		m_pFormatData = NULL;
	}

	if (m_pExtData != NULL)
	{
		voOMXMemFree (m_pExtData);
		m_pExtData = NULL;
	}
}

OMX_ERRORTYPE voCOMXAudioDec::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	VOLOGF ("Name: %s", m_pName);

	return voCOMXCompFilter::EmptyThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXAudioDec::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nParamIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nParamIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			if (pAudioFormat->nPortIndex == 0)
			{
				pAudioFormat->eEncoding = m_nCoding;
			}
			else if (pAudioFormat->nPortIndex == 1)
			{
				pAudioFormat->eEncoding = OMX_AUDIO_CodingPCM;
			}
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
			if (pPCMFormat->nPortIndex == 1)
			{
				voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
				errType = OMX_ErrorNone;
			}
		}
		break;

	case OMX_IndexParamAudioWma:
		{
			OMX_AUDIO_PARAM_WMATYPE * pWMAFormat = (OMX_AUDIO_PARAM_WMATYPE*)pComponentParameterStructure;

			pWMAFormat->nChannels = m_nChannels;
			pWMAFormat->nSamplingRate = pWMAFormat->nSamplingRate;

			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioDec::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam(pComp, pAudioFormat->nPortIndex, pAudioFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (pAudioFormat->nPortIndex == 0)
			{
				m_nCoding = pAudioFormat->eEncoding;

				if (m_nCoding == OMX_AUDIO_CodingADPCM)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingAMR)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingQCELP13)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingEVRC)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingAAC)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingMP3)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingWMA)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingRA)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAMRWBP)
					errType = OMX_ErrorNone;
				else
				{
					VOLOGW ("The codeing is %d.", m_nCoding);
					errType = OMX_ErrorComponentNotFound;
				}
			}
		}
		break;

	case OMX_IndexParamAudioMp3:
		{
			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pMP3Format->nPortIndex, pMP3Format, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_MP3TYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_MP3TYPE));

			pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)m_pFormatData;

			if (pMP3Format->nChannels > 0)
			{
				m_nChannels = pMP3Format->nChannels;
				m_nSampleRate = pMP3Format->nSampleRate;

				m_pcmType.nChannels = pMP3Format->nChannels;
				m_pcmType.nSamplingRate = pMP3Format->nSampleRate;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pAmrFormat->nPortIndex, pAmrFormat, sizeof(OMX_AUDIO_PARAM_AMRTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_AMRTYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_AMRTYPE));

			pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE*)m_pFormatData;

			if (pAmrFormat->nChannels > 0)
			{
				m_nChannels = pAmrFormat->nChannels;
				m_nSampleRate = 8000;
				if (pAmrFormat->eAMRBandMode >= OMX_AUDIO_AMRBandModeWB0 && pAmrFormat->eAMRBandMode <= OMX_AUDIO_AMRBandModeWB8)
					m_nSampleRate = 16000;

				m_pcmType.nChannels =m_nChannels;
				m_pcmType.nSamplingRate = m_nSampleRate;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pAACFormat->nPortIndex, pAACFormat, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));

			pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)m_pFormatData;

			if (pAACFormat->nChannels > 0)
			{
				m_nChannels = pAACFormat->nChannels;
				m_nSampleRate = pAACFormat->nSampleRate;

				m_pcmType.nChannels = pAACFormat->nChannels;
				m_pcmType.nSamplingRate = pAACFormat->nSampleRate;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioWma:
		{
			OMX_AUDIO_PARAM_WMATYPE * pWMAFormat = (OMX_AUDIO_PARAM_WMATYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pWMAFormat->nPortIndex, pWMAFormat, sizeof(OMX_AUDIO_PARAM_WMATYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_WMATYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_WMATYPE));

			pWMAFormat = (OMX_AUDIO_PARAM_WMATYPE*)m_pFormatData;
			if (pWMAFormat->nChannels > 0)
			{
				m_nChannels = pWMAFormat->nChannels;
				m_nSampleRate = pWMAFormat->nSamplingRate;

				m_pcmType.nChannels = pWMAFormat->nChannels;
				m_pcmType.nSamplingRate = pWMAFormat->nSamplingRate;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAdpcm:
		{
			OMX_AUDIO_PARAM_ADPCMTYPE * pADPCMFormat = (OMX_AUDIO_PARAM_ADPCMTYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pADPCMFormat->nPortIndex, pADPCMFormat, sizeof(OMX_AUDIO_PARAM_ADPCMTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_ADPCMTYPE));
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_ADPCMTYPE));

			pADPCMFormat = (OMX_AUDIO_PARAM_ADPCMTYPE*)m_pFormatData;
			if (pADPCMFormat->nChannels > 0)
			{
				m_nChannels = pADPCMFormat->nChannels;
				m_nSampleRate = pADPCMFormat->nSampleRate;
				m_nBits = pADPCMFormat->nBitsPerSample;

				m_pcmType.nChannels = pADPCMFormat->nChannels;
				m_pcmType.nSamplingRate = pADPCMFormat->nSampleRate;
				m_pcmType.nBitPerSample = pADPCMFormat->nBitsPerSample;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamCommonExtraQuantData:
		{
			OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)pComponentParameterStructure;
			//errType = voOMXBase_CheckHeader (pExtData, sizeof (OMX_OTHER_EXTRADATATYPE));
			//if (errType != OMX_ErrorNone)
			//	return errType;

			if (m_pExtData != NULL)
				voOMXMemFree (m_pExtData);
			m_pExtData = NULL;
			m_nExtSize = 0;

			if (pExtData->nDataSize > 0)
			{
				m_nExtSize = pExtData->nDataSize;
				m_pExtData = (OMX_S8*)voOMXMemAlloc (m_nExtSize);
				voOMXMemCopy (m_pExtData, pExtData->data, m_nExtSize);
			}

			errType = OMX_ErrorNone;
		}
		break;

    case OMX_IndexParamAudioPcm:
        {
           OMX_AUDIO_PARAM_PCMMODETYPE* pData = (OMX_AUDIO_PARAM_PCMMODETYPE*)pComponentParameterStructure;
		   errType = CheckParam(pComp, pData->nPortIndex, pData, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
		   if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

           m_pcmType.nChannels = pData->nChannels;
           m_pcmType.nBitPerSample = pData->nBitPerSample;
           m_pcmType.nSamplingRate = pData->nSamplingRate;
           m_pcmType.nBitPerSample = pData->nBitPerSample;

           errType = OMX_ErrorNone;
        }
        break;

	case OMX_IndexParamStandardComponentRole:
		{
			OMX_PARAM_COMPONENTROLETYPE * pRoleType = (OMX_PARAM_COMPONENTROLETYPE *)pComponentParameterStructure;
			errType = voOMXBase_CheckHeader (pRoleType, sizeof (OMX_PARAM_COMPONENTROLETYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Header error. 0X%08X", errType);
				return errType;
			}

			m_nCoding = OMX_AUDIO_CodingUnused;
			if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.aac"))
				m_nCoding = OMX_AUDIO_CodingAAC;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.mp3"))
				m_nCoding = OMX_AUDIO_CodingMP3;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.wma"))
				m_nCoding =  OMX_AUDIO_CodingWMA;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.amr"))
				m_nCoding = OMX_AUDIO_CodingAMR;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.qcelp"))
				m_nCoding = OMX_AUDIO_CodingQCELP13;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.evrc"))
				m_nCoding = OMX_AUDIO_CodingEVRC;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.adpcm"))
				m_nCoding = OMX_AUDIO_CodingADPCM;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.ra"))
				m_nCoding = OMX_AUDIO_CodingRA;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.ac3"))
				m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.amrwbp"))
				m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAMRWBP;

			if (m_nCoding != OMX_AUDIO_CodingUnused)
				errType = OMX_ErrorNone;
			else
				errType = OMX_ErrorInvalidComponent;

			if(NULL == m_pInput)
				CreatePorts();
			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pInput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
			pPortType->nBufferSize = 10240 * 2;

			m_pOutput->GetPortType (&pPortType);
			pPortType->nBufferCountActual = 2;
		}
		break;

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDec::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");

	if (nIndex == 0)
		strcpy ((char *)cRole, "audio_decoder.aac");
	else if (nIndex == 1)
		strcpy ((char *)cRole, "audio_decoder.mp3");
	else if (nIndex == 2)
		strcpy ((char *)cRole, "audio_decoder.wma");
	else if (nIndex == 3)
		strcpy ((char *)cRole, "audio_decoder.amr");
	else if (nIndex == 4)
		strcpy ((char *)cRole, "audio_decoder.qcelp");
	else if (nIndex == 5)
		strcpy ((char *)cRole, "audio_decoder.evrc");
	else if (nIndex == 6)
		strcpy ((char *)cRole, "audio_decoder.adpcm");
	else if (nIndex == 7)
		strcpy ((char *)cRole, "audio_decoder.ac3");
	else if (nIndex == 8)
		strcpy ((char *)cRole, "audio_decoder.amrwbp");
	else
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioDec::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. New State %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_nSampleRate < 8000)
			m_nSampleRate = 44100;
		if (m_nChannels == 0)
			m_nChannels = 2;
	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
	}

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioDec::Flush (OMX_U32	nPort)
{
	VOLOGF ("Name: %s", m_pName);

	return voCOMXCompFilter::Flush (nPort);
}

OMX_ERRORTYPE voCOMXAudioDec::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);

	if (!m_bSetThreadPriority)
	{
		voOMXThreadSetPriority (m_hBuffThread, VOOMX_THREAD_PRIORITY_ABOVE_NORMAL);
		m_bSetThreadPriority = OMX_TRUE;
	}

	OMX_U32 nRC = 0;

/*

	if (nRC == VO_ERR_NONE &&
		((int)fmtAudio.Channels != (int)m_pcmType.nChannels || (int)fmtAudio.SampleRate != (int)m_pcmType.nSamplingRate))
	{
		VOLOGI ("Audio Format was changed. S %d, C %d", m_pcmType.nSamplingRate, m_pcmType.nChannels);

		pOutput->nFilledLen = 0;
		m_pOutput->ReturnBuffer (pOutput);
		m_pOutputBuffer = NULL;
		*pFilled = OMX_FALSE;

		m_pcmType.nSamplingRate = fmtAudio.SampleRate;
		m_pcmType.nChannels = fmtAudio.Channels;

		m_nStepSize = m_pcmType.nSamplingRate * m_pcmType.nChannels * 2 / 5;

		if (m_pCallBack != NULL)
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventPortSettingsChanged, 1, 0, NULL);
		
		return OMX_ErrorNone;
	}

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
		m_nBuffSize = m_nBuffSize + m_outBuffer.Length;

		if (m_nBuffSize >= m_nStepSize)
		{
			pOutput->nFilledLen  = m_nBuffSize;
			pOutput->nTimeStamp = m_nStartTime;

			m_nStartTime = m_nStartTime + (m_nBuffSize * 1000) / (m_nSampleRate * m_nChannels * 2);
			m_nBuffSize = 0;

			*pFilled = OMX_TRUE;
		}
		else
		{
			*pFilled = OMX_FALSE;
		}
	}
	else
	{
		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
*/
	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXAudioDec::InitPortType (void)
{
	VOLOGF ("Name: %s", m_pName);

	m_portParam[OMX_PortDomainAudio].nPorts = 2;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 2;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 10240;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.audio.eEncoding = OMX_AUDIO_CodingUnused;

	m_pOutput->GetPortType (&pPortType);
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
