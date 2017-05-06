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
#include "voOMXBase.h"
#include "voOMX_Index.h"
#include "voOMXMemory.h"
#include "voOMXFile.h"
#include "voAMRNB.h"
#include "voAMRWB.h"
#include "voAAC.h"
#include "voCOMXAudioEnc.h"

#define LOG_TAG "voCOMXAudioEnc"
#include "voLog.h"

voCOMXAudioEnc::voCOMXAudioEnc(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_AUDIO_CodingAMR)
	, m_pFormatData (NULL)
	, m_pAudioEnc (NULL)
	, m_nStartTime (0)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Encoder.XXX");

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;
	m_inBuffer.Length = 0;

	OMX_PTR hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoAMRNBEnc.so"), VOOMX_FILE_READ_ONLY);
	if (hCodecFile != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingAMR;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoMP3Enc.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingMP3;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoAACEnc.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingAAC;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoQCELPEnc.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingQCELP13;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoEVRCEnc.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingEVRC;
		voOMXFileClose (hCodecFile);
	}
}

voCOMXAudioEnc::~voCOMXAudioEnc(void)
{
	if (m_pFormatData != NULL)
	{
		voOMXMemFree (m_pFormatData);
		m_pFormatData = NULL;
	}

	if (m_pAudioEnc != NULL)
		delete m_pAudioEnc;
}

OMX_ERRORTYPE voCOMXAudioEnc::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorNone;
	switch (nParamIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = voOMXBase_CheckHeader (pAudioFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (pAudioFormat->nPortIndex == 0)
				pAudioFormat->eEncoding = OMX_AUDIO_CodingPCM;
			else if (pAudioFormat->nPortIndex == 1)
				pAudioFormat->eEncoding = m_nCoding;
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pPCMFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
			if (pPCMFormat->nPortIndex == 0)
				voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
		}
		break;

	case OMX_IndexParamAudioMp3:
		{
			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)pComponentParameterStructure;
			pMP3Format->nChannels = m_pcmType.nChannels;
			pMP3Format->nSampleRate = m_pcmType.nSamplingRate;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE*)pComponentParameterStructure;
			pAmrFormat->nChannels = m_pcmType.nChannels;
			if(m_pFormatData)
				pAmrFormat->eAMRBandMode = ((OMX_AUDIO_PARAM_AMRTYPE*)m_pFormatData)->eAMRBandMode;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)pComponentParameterStructure;
			pAACFormat->nChannels = m_pcmType.nChannels;
			pAACFormat->nSampleRate = m_pcmType.nSamplingRate;
		}
		break;

	case OMX_IndexParamAudioQcelp13:
		{
			OMX_AUDIO_PARAM_QCELP13TYPE * pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE*)pComponentParameterStructure;
			pQcelp13Format->nChannels = m_pcmType.nChannels;
		}
		break;

	case OMX_IndexParamAudioQcelp8:
		{
			OMX_AUDIO_PARAM_QCELP8TYPE * pQcelp8Format = (OMX_AUDIO_PARAM_QCELP8TYPE*)pComponentParameterStructure;
			pQcelp8Format->nChannels = m_pcmType.nChannels;
		}
		break;

	case OMX_IndexParamAudioEvrc:
		{
			OMX_AUDIO_PARAM_EVRCTYPE * pEvrcFormat = (OMX_AUDIO_PARAM_EVRCTYPE*)pComponentParameterStructure;
			pEvrcFormat->nChannels = m_pcmType.nChannels;
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
	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;
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
			m_pcmType.nBitPerSample = pPCMFormat->nBitPerSample;

			// update output buffer size
			OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
			m_pOutput->GetPortType (&pPortType);
			pPortType->nBufferSize = m_pcmType.nChannels * m_pcmType.nBitPerSample * m_pcmType.nSamplingRate / 8;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioMp3:
		{
			CreatePorts();
			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE *)pComponentParameterStructure;

			errType = CheckParam (pComp, pMP3Format->nPortIndex, pMP3Format, sizeof(OMX_AUDIO_PARAM_MP3TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_MP3TYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_MP3TYPE));

			pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)m_pFormatData;

			m_pcmType.nChannels     = pMP3Format->nChannels;
			m_pcmType.nSamplingRate = pMP3Format->nSampleRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			CreatePorts();
			OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pAmrFormat->nPortIndex, pAmrFormat, sizeof(OMX_AUDIO_PARAM_AMRTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_AMRTYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
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
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));

			pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)m_pFormatData;

			m_pcmType.nChannels     = pAACFormat->nChannels;
			m_pcmType.nSamplingRate = pAACFormat->nSampleRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioQcelp13:
		{
			CreatePorts();
			OMX_AUDIO_PARAM_QCELP13TYPE * pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pQcelp13Format->nPortIndex, pQcelp13Format, sizeof(OMX_AUDIO_PARAM_QCELP13TYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_QCELP13TYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_QCELP13TYPE));

			pQcelp13Format = (OMX_AUDIO_PARAM_QCELP13TYPE*)m_pFormatData;

			m_pcmType.nChannels = pQcelp13Format->nChannels;
			m_pcmType.nSamplingRate = 8000;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioEvrc:
		{
			CreatePorts();
			OMX_AUDIO_PARAM_EVRCTYPE * pEvrcFormat = (OMX_AUDIO_PARAM_EVRCTYPE *)pComponentParameterStructure;
			errType = CheckParam(pComp, pEvrcFormat->nPortIndex, pEvrcFormat, sizeof(OMX_AUDIO_PARAM_EVRCTYPE));
			if (errType != OMX_ErrorNone)
				return errType;

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_EVRCTYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_EVRCTYPE));

			pEvrcFormat = (OMX_AUDIO_PARAM_EVRCTYPE*)m_pFormatData;

			m_pcmType.nChannels = pEvrcFormat->nChannels;
			m_pcmType.nSamplingRate = 8550;

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
			else if(!strcmp ((char *)pRoleType->cRole, "OMX.VisualOn.Audio.Encoder.XXX"))
				return OMX_ErrorNone;

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
	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_pAudioEnc == NULL)
		{
			VO_U32 nCodec = 0;

			switch(m_nCoding)
			{
			case OMX_AUDIO_CodingMP3:     nCodec = VO_AUDIO_CodingMP3;		break;
			case OMX_AUDIO_CodingAAC:     nCodec = VO_AUDIO_CodingAAC;      break;
			case OMX_AUDIO_CodingAMR: 
				{
					nCodec = VO_AUDIO_CodingAMRNB; 
					OMX_AUDIO_PARAM_AMRTYPE* pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE*)m_pFormatData;
					if(pAmrFormat->eAMRBandMode > OMX_AUDIO_AMRBandModeNB7 && pAmrFormat->eAMRBandMode <= OMX_AUDIO_AMRBandModeKhronosExtensions)
						nCodec = VO_AUDIO_CodingAMRWB; 
				}
				
				break;
			case OMX_AUDIO_CodingQCELP13: nCodec = VO_AUDIO_CodingQCELP13;  break;
			case OMX_AUDIO_CodingEVRC:    nCodec = VO_AUDIO_CodingEVRC;     break;
			default:                      nCodec = VO_AUDIO_CodingAMRNB;    break;
			}

			m_pAudioEnc = new CAudioEncoder (NULL, nCodec, m_pMemOP);
			if (m_pAudioEnc == NULL)
			{
				return OMX_ErrorInsufficientResources;
			}
			m_pAudioEnc->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
			m_pAudioEnc->SetWorkPath ((VO_TCHAR *)m_pWorkPath);

			VO_AUDIO_FORMAT	fmtAudio;
			fmtAudio.Channels   = m_pcmType.nChannels;
			fmtAudio.SampleRate = m_pcmType.nSamplingRate;
			fmtAudio.SampleBits = m_pcmType.nBitPerSample;

			VO_U32 nRC = m_pAudioEnc->Init (&fmtAudio);
			if (nRC != VO_ERR_NONE)
			{
				VOLOGE ("m_pAudioEnc->Init failed. 0X%08X", (int)nRC);
				return OMX_ErrorResourcesLost;
			}

			switch(m_nCoding) 
			{
			case OMX_AUDIO_CodingMP3:
            {
                OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)m_pFormatData;

				if (pMP3Format != NULL)
				{
					m_pAudioEnc->SetParam(VO_PID_AUDIO_SAMPLEREATE, &pMP3Format->nSampleRate);
					m_pAudioEnc->SetParam(VO_PID_AUDIO_CHANNELS, &pMP3Format->nChannels);
					m_pAudioEnc->SetParam(VO_PID_AUDIO_BITRATE, &pMP3Format->nBitRate);
				}
            }
            break;

			case OMX_AUDIO_CodingAAC:
            {
				AACENC_PARAM sAACEncParam;
				memset(&sAACEncParam, 0, sizeof(sAACEncParam));
				if (m_pFormatData != NULL)
				{
					OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)m_pFormatData;
					if(pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP2ADTS || pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4ADTS)
						sAACEncParam.adtsUsed = 1;
					else if(pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4FF || pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatRAW)
						sAACEncParam.adtsUsed = 0;
					else
						return OMX_ErrorNotImplemented;

					sAACEncParam.bitRate = pAACFormat->nBitRate;
					sAACEncParam.nChannels = (short)pAACFormat->nChannels;
					sAACEncParam.sampleRate = pAACFormat->nSampleRate;
				}
				else
				{
					sAACEncParam.adtsUsed = 1;
					sAACEncParam.bitRate = 0x20000;	// 128 Kbps
					sAACEncParam.nChannels = (short)m_pcmType.nChannels;
					sAACEncParam.sampleRate = m_pcmType.nSamplingRate;
				}

				nRC = m_pAudioEnc->SetParam(VO_PID_AAC_ENCPARAM, &sAACEncParam);
            }
            break;

			case OMX_AUDIO_CodingAMR:
			{
				OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE *)m_pFormatData;
				if(nCodec == VO_AUDIO_CodingAMRNB)
				{
					VOAMRNBFRAMETYPE frameType = VOAMRNB_RFC3267;
					int              mode      = VOAMRNB_MD122;

					if (pAmrFormat != NULL)
					{
						if(pAmrFormat->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatIF1)
						{
							frameType = VOAMRNB_IF1;
						}
						else if(pAmrFormat->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatIF2)
						{
							frameType = VOAMRNB_IF2;
						}
						else if(pAmrFormat->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF)
						{
							frameType = VOAMRNB_RFC3267;
						}
						else
						{
							frameType = VOAMRNB_RFC3267;
						}

						if(pAmrFormat->eAMRBandMode > OMX_AUDIO_AMRBandModeUnused
							&& pAmrFormat->eAMRBandMode <= OMX_AUDIO_AMRBandModeNB7)
							mode = (int)pAmrFormat->eAMRBandMode - 1;
					}

					m_pAudioEnc->SetParam(VO_PID_AMRNB_FRAMETYPE, &frameType);
					m_pAudioEnc->SetParam(VO_PID_AMRNB_MODE,      &mode);
				}
				else
				{
					VOAMRWBFRAMETYPE frameType = VOAMRWB_RFC3267;
					int              mode      = VOAMRWB_MD2385;
					int				 dts       = 0;

					if(pAmrFormat != NULL)
					{
						if(pAmrFormat->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatITU)
						{
							frameType = VOAMRWB_ITU;
						}

						if(pAmrFormat->eAMRBandMode > OMX_AUDIO_AMRBandModeWB0
							&& pAmrFormat->eAMRBandMode <= OMX_AUDIO_AMRBandModeKhronosExtensions)
							mode = (int)pAmrFormat->eAMRBandMode - 9;
					}
					

					m_pAudioEnc->SetParam(VO_PID_AMRWB_FRAMETYPE, &frameType);
					m_pAudioEnc->SetParam(VO_PID_AMRWB_MODE,      &mode);
					m_pAudioEnc->SetParam(VO_PID_AMRWB_DTX , &dts);
				}
				
			}
			break;

			case OMX_AUDIO_CodingQCELP13:  break;
			case OMX_AUDIO_CodingEVRC:     break;
			default:                       break;
			}
		}
	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		m_inBuffer.Length = 0;
	}
	else if (newState == OMX_StateIdle && (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle))
		ResetPortBuffer ();

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioEnc::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	if (m_pAudioEnc == NULL)
		return OMX_ErrorInvalidState;

	OMX_U32 nRC = 0;

	if (m_inBuffer.Length == 0)
	{
		m_inBuffer.Buffer = pInput->pBuffer + pInput->nOffset;
		m_inBuffer.Length = pInput->nFilledLen;
		m_nStartTime      = pInput->nTimeStamp;

		nRC = m_pAudioEnc->SetInputData (&m_inBuffer);
	}

	m_outBuffer.Buffer = pOutput->pBuffer;
	m_outBuffer.Length = pOutput->nAllocLen;

	nRC = m_pAudioEnc->GetOutputData (&m_outBuffer, &m_outInfo);
	if(VO_ERR_NONE != nRC && VO_ERR_INPUT_BUFFER_SMALL != nRC)
	{
		if(m_pCallBack)
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecPassableError, 3, 0, NULL);
	}

//	VOLOGI ("Name: %s. GetOutputData Size: %d, Result %d", m_pName, m_outBuffer.Length, nRC);

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
		pOutput->nTimeStamp = m_nStartTime + (m_outInfo.InputUsed * 1000) / (m_pcmType.nSamplingRate * m_pcmType.nChannels * 2);
		*pFilled = OMX_TRUE;
	}
	else
	{
		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioEnc::InitPortType (void)
{
	m_portParam[OMX_PortDomainAudio].nPorts = 2;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 2;
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
