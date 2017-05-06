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
#include "OMX_Audio.h"
#include "voCOMXAudioDTSDec.h"
#include "cmnMemory.h"

#include "voOMXFile.h"
#include "voLog.h"

#define LOG_TAG "voCOMXAudioDTSDec"

extern VO_TCHAR * g_pvoOneWorkingPath;

static const OMX_AUDIO_CHANNELTYPE SpkrMask2ChannelMap[DTS_SPEAKER_MAX_SPEAKERS] =
{
    OMX_AUDIO_ChannelCF,    /* DTS_MASK_SPEAKER_CENTRE = 0x00000001 */
    OMX_AUDIO_ChannelLF,    /* DTS_MASK_SPEAKER_LEFT   = 0x00000002 */
    OMX_AUDIO_ChannelRF,    /* DTS_MASK_SPEAKER_RIGHT  = 0x00000004 */
    OMX_AUDIO_ChannelLS,    /* DTS_MASK_SPEAKER_LS     = 0x00000008 */
    OMX_AUDIO_ChannelRS,    /* DTS_MASK_SPEAKER_RS     = 0x00000010 */
    OMX_AUDIO_ChannelLFE,   /* DTS_MASK_SPEAKER_LFE1   = 0x00000020 */
    OMX_AUDIO_ChannelCS,    /* DTS_MASK_SPEAKER_Cs     = 0x00000040 */
    OMX_AUDIO_ChannelLR,    /* DTS_MASK_SPEAKER_Lsr    = 0x00000080 */
    OMX_AUDIO_ChannelRR,    /* DTS_MASK_SPEAKER_Rsr    = 0x00000100 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLSS,   /* DTS_MASK_SPEAKER_Lss    = 0x00000200 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRSS,   /* DTS_MASK_SPEAKER_Rss    = 0x00000400 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLC,    /* DTS_MASK_SPEAKER_Lc     = 0x00000800 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRC,    /* DTS_MASK_SPEAKER_Rc     = 0x00001000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLH,    /* DTS_MASK_SPEAKER_Lh     = 0x00002000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelCH,    /* DTS_MASK_SPEAKER_Ch     = 0x00004000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRH,    /* DTS_MASK_SPEAKER_Rh     = 0x00008000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLFE2,  /* DTS_MASK_SPEAKER_LFE2   = 0x00010000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLW,    /* DTS_MASK_SPEAKER_Lw     = 0x00020000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRW,    /* DTS_MASK_SPEAKER_Rw     = 0x00040000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelOH,    /* DTS_MASK_SPEAKER_Oh     = 0x00080000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLHS,   /* DTS_MASK_SPEAKER_Lhs    = 0x00100000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRHS,   /* DTS_MASK_SPEAKER_Rhs    = 0x00200000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelCHR,   /* DTS_MASK_SPEAKER_Chr    = 0x00400000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLHR,   /* DTS_MASK_SPEAKER_Lhr    = 0x00800000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRHR,   /* DTS_MASK_SPEAKER_Rhr    = 0x01000000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelCLF,   /* DTS_MASK_SPEAKER_Clf	   = 0x02000000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLLF,   /* DTS_MASK_SPEAKER_Llf	   = 0x04000000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRLF,   /* DTS_MASK_SPEAKER_Rlf	   = 0x08000000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelLT,    /* DTS_MASK_SPEAKER_Lt     = 0x10000000 */
    (OMX_AUDIO_CHANNELTYPE)OMX_AUDIO_ChannelRT     /* DTS_MASK_SPEAKER_Rt     = 0x20000000 */
};



voCOMXAudioDTSDec::voCOMXAudioDTSDec(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_AUDIO_CodingUnused)
	, m_pFormatData (NULL)
	, m_nExtSize (0)
	, m_pExtData (NULL)
	, m_nSampleRate (48000)
	, m_nChannels (2)
	, m_nBits (16)
	, m_bSetThreadPriority (OMX_FALSE)
	, m_nCodec (0)
	, m_nDecFrames (0)
	, m_nStepSize (4410 * 2)
	, m_nOutputSize (0)
	, m_bDownMix(OMX_FALSE)
	, m_bNofityError (VO_FALSE)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Decoder.DTS");

	OMX_ERRORTYPE rc = OMX_ErrorNone;

	m_inBuffer.Length = 0;
	m_inBuffer.Time = 0;

	/* dump pcm data  update for #17623 task */  
	VO_TCHAR szCfgFile[256];
	vostrcpy (szCfgFile, _T("vomeplay.cfg"));

	m_pCfgComponent = new CBaseConfig ();
	m_pCfgComponent->Open (szCfgFile);

	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpLog", 0) > 0)
	{
		m_nPfmFrameSize = 10240;
		m_pPfmFrameTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCodecThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCompnThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmMediaTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmSystemTime = new OMX_U32[m_nPfmFrameSize];
	}

	char *pFile = m_pCfgComponent->GetItemText (m_pName, (char*)"DumpOututDataFile");
	if (pFile != NULL)
	{
#ifdef _WIN32
		TCHAR szFile[256];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
			m_pDumpOutputFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
		m_pDumpOutputFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
		m_nDumpOutputFormat = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpOutputDataFormat", 0);
		VOLOGI ("Create Input file %s, Format is %d The handle is 0X%08X.", pFile, (int)m_nDumpOutputFormat, (int)m_pDumpOutputFile);
	}
	/************************* end update #17623 task */

	//out_pcm_mode_speaker_mask = DTS_MASK_SPEAKER_CENTRE | DTS_MASK_SPEAKER_LEFT | DTS_MASK_SPEAKER_RIGHT | DTS_MASK_SPEAKER_LS | DTS_MASK_SPEAKER_RS | DTS_MASK_SPEAKER_LFE1;
	bDialNorm = OMX_TRUE;
	bMixLFEIntoFront = OMX_FALSE;
	nAudioPresentIndex = 0;
	nExSSID = 0;
	nDRCPercent = 0;
	nOutputBitWidth = 16;
	nSpkrOut = (OMX_AUDIO_DTS_SPKROUTTYPE)0xF;
	nParserTimeStamp = 0;

	/* initialize DTS decoder */
	if( rc == OMX_ErrorNone )
	{
		rc = DTSHD_FRAME_PLAYER_ERROR_OK == DTSDecFramePlayer_SAPI_Create( &player ) ? OMX_ErrorNone : OMX_ErrorInsufficientResources;
	}

	/* initialize DTS parser */
	if( rc == OMX_ErrorNone )
	{
		rc = DTSHD_PARSER_ERROR_OK == DTSDecParser_SAPI_Create( &parser ) ? OMX_ErrorNone : OMX_ErrorInsufficientResources;
	}

	if( rc != OMX_ErrorNone )
	{
		DTSDecFramePlayer_SAPI_Destroy( player );
		DTSDecParser_SAPI_Destroy( parser );
	}

	m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS;
}

voCOMXAudioDTSDec::~voCOMXAudioDTSDec(void)
{
	//VOLOGF ("Name: %s", m_pName);

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


	DTSDecFramePlayer_SAPI_Destroy( player );
	DTSDecParser_SAPI_Destroy( parser );
}

OMX_ERRORTYPE voCOMXAudioDTSDec::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	//VOLOGF ("Name: %s", m_pName);

	return voCOMXCompFilter::EmptyThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXAudioDTSDec::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	//VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nParamIndex);

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
//				if (m_pcmType.nChannels > 2)
//					m_pcmType.nChannels = 2;

				voOMXMemCopy (pPCMFormat, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
				errType = OMX_ErrorNone;
			}
		}
		break;	

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioDTSDec::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	//VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nIndex);

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

				if( m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS )
					errType = OMX_ErrorNone;
				else
				{
					VOLOGW ("The codeing is %d.", m_nCoding);
					//errType = OMX_ErrorComponentNotFound;
				}
			}
		}
		break;

	case OMX_VO_IndexAudioParamDTS:
		{
			OMX_VO_AUDIO_PARAM_DTSTYPE * pDTSFormat = (OMX_VO_AUDIO_PARAM_DTSTYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pDTSFormat->nPortIndex, pDTSFormat, sizeof(OMX_VO_AUDIO_PARAM_DTSTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_VO_AUDIO_PARAM_DTSTYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_VO_AUDIO_PARAM_DTSTYPE));

			pDTSFormat = (OMX_VO_AUDIO_PARAM_DTSTYPE*)m_pFormatData;

			if (pDTSFormat->nChannels > 0)
			{
				m_nChannels = pDTSFormat->nChannels;
				m_nSampleRate = pDTSFormat->nSampleRate;

				m_pcmType.nChannels = pDTSFormat->nChannels;
				m_pcmType.nSamplingRate = pDTSFormat->nSampleRate;

				if(m_nChannels > 2)
				{
					m_nChannels = 6;
					m_pcmType.nChannels = 6;
				}
				else if(m_nChannels == 1)
				{
					m_nChannels = 2;
					m_pcmType.nChannels = 2;
				}
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
				if (m_pExtData == NULL)
					return OMX_ErrorInsufficientResources;
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
			
		   if(pData->nChannels < m_pcmType.nChannels)
		   {
			   nSpkrOut	=	OMX_AUDIO_DTSSPKROUT_MASK_LR;			
			   bMixLFEIntoFront = OMX_FALSE;     /* fix 22737 */
			   m_bDownMix = OMX_TRUE;
		   }
		   m_pcmType.nChannels = pData->nChannels;
		   m_pcmType.nBitPerSample = pData->nBitPerSample;
		   m_pcmType.nSamplingRate = pData->nSamplingRate;
		   m_nChannels = m_pcmType.nChannels;
		   m_nSampleRate = m_pcmType.nSamplingRate;
		   m_nBits    = m_pcmType.nBitPerSample;

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
			if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.dts"))
				m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS;

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

OMX_ERRORTYPE voCOMXAudioDTSDec::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	return voCOMXCompFilter::GetConfig (hComponent, nIndex, pComponentConfigStructure);
}


OMX_ERRORTYPE voCOMXAudioDTSDec::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure)
{
	//VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentConfigStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_VO_IndexConfigDTSChanOutMask:
		{
			voCOMXAutoLock lokc (&m_mutCodec);		
			nSpkrOut = *((OMX_AUDIO_DTS_SPKROUTTYPE *)pComponentConfigStructure);

			DTSDecFramePlayer_SAPI_Reset_SpkrOut_Config(player, nSpkrOut);
		}
		errType = OMX_ErrorNone;
		break;		
	default:
		errType = voCOMXCompFilter::SetConfig (hComponent, nIndex, pComponentConfigStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioDTSDec::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");

	if (nIndex == 0)
		strcpy ((char *)cRole, "audio_decoder.dts");
	else
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioDTSDec::SetNewState (OMX_STATETYPE newState)
{
	//VOLOGF ("Name: %s. New State %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_nSampleRate < 8000)
			m_nSampleRate = 44100;
		if (m_nChannels == 0)
			m_nChannels = 2;
		
		//VOLOGI ("Init decoder begin!!!");
		m_nStepSize = m_nSampleRate * m_nChannels * m_nBits  / (1000 * 8 / 200);

		ResetDTSParser( );
		ResetDTSDecoder( );
		
		//VOLOGI ("Init decoder OK!!!");
	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		m_inBuffer.Length = 0;

		m_nOutputSize = 0;
	}
	else if (newState == OMX_StateIdle)
	{
		m_inBuffer.Length = 0;
		m_nOutBuffTime = 0;
		m_nOutBuffSize = 0;
	}

	m_bNofityError = VO_FALSE;
	m_inBuffer.Time = 0;

	m_nDecFrames = 0;
	m_nSysStartTime = 0;

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioDTSDec::Flush (OMX_U32	nPort)
{
	//VOLOGF ("Name: %s", m_pName);

	voCOMXAutoLock lokc (&m_mutCodec);

	OMX_ERRORTYPE errType = voCOMXCompFilter::Flush (nPort);

	ResetDTSParser();

	m_inBuffer.Length = 0;
	m_inBuffer.Time = 0;
	m_nOutBuffTime = 0;
	m_nOutBuffSize = 0;
	m_bNofityError = VO_FALSE;

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDTSDec::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	//VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);

    OMX_ERRORTYPE rc = OMX_ErrorNone;
    VO_U32 nChannels, nSampleRate, nBitWidth, i;
    OMX_BOOL bIsFirstSubFrame;
    dtsBitstreamInfo *pBitstreamInfo;

	if (!m_bSetThreadPriority)
	{
		voOMXThreadSetPriority (m_hBuffThread, VOOMX_THREAD_PRIORITY_ABOVE_NORMAL);
		m_bSetThreadPriority = OMX_TRUE;
	}

	voCOMXAutoLock lokc (&m_mutCodec);
	if( pInput->nFilledLen > 0 &&
		( nParserFlags & DTSHD_DEC_API_FLAG_FRAME_AVAILABLE ) == 0 )
	{
		if (m_nOutBuffSize == 0)
		{
			if (m_nOutBuffTime == 0)
			{
				m_nOutBuffTime = pInput->nTimeStamp;
			}
			else if (pInput->nTimeStamp != 0)
			{
				m_nOutBuffTime = pInput->nTimeStamp;
			}
		}
		/* parse frame */
		nParserFlags = pInput->nFlags & OMX_BUFFERFLAG_EOS ? DTSHD_DEC_API_FLAG_EOF : 0;
		DTSDecParser_SAPI_CaptureFrame( parser,
			pInput->pBuffer,
			( dtsUint32 * )&pInput->nOffset,
			( dtsUint32 * )&pInput->nFilledLen,
			&nParserFlags,
			&pnParserBitstream,
			&nParserFrameSize);

	}

	*pEmptied = OMX_FALSE;
	if( pInput->nFilledLen == 0 )
	{
		pInput->nFilledLen = 0;
		pInput->nOffset = 0;
		*pEmptied = OMX_TRUE;
	}

	if( (nParserFlags & DTSHD_DEC_API_FLAG_FRAME_AVAILABLE ) == 0)
	{
		*pFilled = OMX_FALSE;
		return OMX_ErrorNone;
	}

	if (m_nLogLevel > 0)
	{
		if (m_nLogSysStartTime == 0)
			m_nLogSysStartTime = voOMXOS_GetSysTime ();

		VOLOGI ("Sys Time: %d  frame time is %d, Diff %d, size %d", voOMXOS_GetSysTime () - m_nLogSysStartTime, (int)pInput->nTimeStamp, (int)((voOMXOS_GetSysTime () - m_nLogSysStartTime) - pInput->nTimeStamp), pInput->nFilledLen);

		m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
	}

	while(1)
	{
		bIsFirstSubFrame = nPlayerFlags & DTSHD_DEC_API_FLAG_HOLD_INPUT ? OMX_FALSE : OMX_TRUE;

		if( nParserFlags & DTSHD_DEC_API_FLAG_RESYNC )
		{
			nPlayerFlags = nParserFlags & DTSHD_DEC_API_FLAG_RESYNC ? DTSHD_DEC_API_FLAG_RESYNC : 0;

			if ( DTSDecParser_SAPI_GetBitstreamInfo( parser, &pBitstreamInfo ) >= 0 )
			{
				if( pBitstreamInfo != NULL )
				{
					nMaxNumChannels = pBitstreamInfo->nMaxNumChannels;
					nMaxChannelMask = pBitstreamInfo->nMaxChannelMask;
					nMaxSampleRate = pBitstreamInfo->nMaxSampleRate;
					nSamplesInFrameAtMaxSR = pBitstreamInfo->nSamplesInFrameAtMaxSR;
				}
			}
		}

		dtsDecoderPCMOutput *pPCMOutput = NULL;

		VOLOGI("m_bDownMix = %d\n", m_bDownMix);
		if(m_bDownMix == OMX_TRUE)
			 pPCMOutput = m_pPCMOutput20;

		if( DTSHD_FRAME_PLAYER_ERROR_OK == DTSDecFramePlayer_SAPI_DecodeSubFrame( player,
			pnParserBitstream,
			nParserFrameSize,
			&nPlayerFlags,
			&m_pPCMOutput,
			NULL,
			&pPCMOutput ) )
		{
			pPCMOutput = m_pPCMOutput;

			if( pPCMOutput != NULL)
			{
				/* capture available samples and sample rate */
				nPlayerAvailableSamples = pPCMOutput->numberOfSamples;

				nSampleRate = 0;
				nBitWidth = 0;
				nChannels = 0;
				for( i = 0; i < DTS_SPEAKER_MAX_SPEAKERS; i++ )
				{
					if( pPCMOutput->speakerMask & ( 1 << i ) )
					{
						nSampleRate = pPCMOutput->samplingRate[i];
						nBitWidth = pPCMOutput->bitsPerSample[i];
						ppnPlayerPCMBuf[nChannels++] = pPCMOutput->pSamples[i];
					}

					if(m_bDownMix == OMX_TRUE && nChannels == 2)
						break;
				}

				if(nChannels > 2 || m_pcmType.nChannels == 6)
					nChannels = 6;
				if(m_bDownMix == OMX_FALSE && nMaxNumChannels > 2)
					nChannels = 6;
				else if(nChannels == 1)
					nChannels = 2;

   			   /* update sample rate, speaker mask and channel pointers */
				if( nSampleRate != m_nSampleRate || nBitWidth != m_nBits ||
					nChannels != m_nChannels )
				{
					m_pcmType.nSamplingRate = nSampleRate;
					m_nSampleRate = nSampleRate;

					m_pcmType.nChannels = nChannels;
					m_nChannels = nChannels;

					m_nBits = nBitWidth;
					m_pcmType.nBitPerSample = nBitWidth;

					pOutput->nFilledLen = 0;
					m_pOutput->ReturnBuffer (pOutput);
					m_pOutputBuffer = NULL;
					*pFilled = OMX_FALSE;
					//out_pcm_mode_speaker_mask = pPCMOutput->speakerMask;

					m_nStepSize = m_nSampleRate * m_nChannels * m_nBits  / (1000 * 8 / 200);

					m_pOutput->SettingsChanging();

					if (m_pCallBack != NULL)
						m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventPortSettingsChanged, 1, 0, NULL);

					return OMX_ErrorNone;
				}

				m_outBuffer.Buffer = pOutput->pBuffer + m_nOutBuffSize;
				m_outBuffer.Length = pOutput->nAllocLen -  m_nOutBuffSize;

				CopyPCMSamples(nPlayerAvailableSamples, pPCMOutput);

				/* inform parser one frame consumed */
				if(( nPlayerFlags & DTSHD_DEC_API_FLAG_HOLD_INPUT ) == 0 )
				{
					nParserFlags &= ~DTSHD_DEC_API_FLAG_FRAME_AVAILABLE;
					pOutput->nFilledLen  = m_nOutBuffSize;
					
					if (m_nOutBuffSize >= m_nStepSize || (pOutput->nFlags & OMX_BUFFERFLAG_EOS))
					{
						pOutput->nFilledLen  = m_nOutBuffSize;
						pOutput->nTimeStamp = m_nOutBuffTime ;

						m_nOutBuffTime = m_nOutBuffTime + (m_nOutBuffSize * 1000LL) / (m_nSampleRate * m_nChannels * m_nBits / 8);
						m_nOutBuffSize = 0;

						VOLOGI ("DTS Audio Time: %d", (int)pOutput->nTimeStamp);

						*pFilled = OMX_TRUE;

						m_nOutputSize += pOutput->nFilledLen;
					}
					else
					{
						*pFilled = OMX_FALSE;
					}

					break;
				}
			}
		}
		else
		{
			ResetDTSParser(  );
			ResetDTSDecoder(  );
			*pFilled = OMX_FALSE;
			break;
		}	
	}

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXAudioDTSDec::InitPortType (void)
{
	//VOLOGF ("Name: %s", m_pName);

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
	pPortType->format.audio.eEncoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS;

	m_pOutput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 2;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 44100*4;
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
	m_pcmType.nSamplingRate = 48000;
	m_pcmType.ePCMMode = OMX_AUDIO_PCMModeLinear;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioDTSDec::CopyPCMSamples(OMX_U32 nSamples, dtsDecoderPCMOutput *pPCMOutput)
{
    OMX_U32 nCh, i, nStride, nChannel;
    OMX_S32 *pSrc;
    OMX_S16 *p16;
    OMX_U8 *p;

	static const int ChanMap[8] = {2, 0, 1, 4, 5, 3, 4, 0}; //{1, 0, 2, 3, 4, 5, 3, 0};

    if( m_pcmType.nBitPerSample == 16 )
    {
		if(m_pcmType.nChannels <= 2) 
		{
			nStride = m_pcmType.nChannels;
			if(pPCMOutput->speakerMask == DTS_MASK_SPEAKER_CENTRE || pPCMOutput->speakerMask == (DTS_MASK_SPEAKER_CENTRE | DTS_MASK_SPEAKER_LFE1))
			{
				p16 = ( OMX_S16 * )( m_outBuffer.Buffer ) + nCh;
				pSrc = (OMX_S32 *) ppnPlayerPCMBuf[0];
#define RSQRT2 5818/8192	//7071/10000

				for( i = 0; i < nSamples; i++ )
				{
					*p16++ = ( OMX_S16 )( (pSrc[i]*RSQRT2) & 0xFFFF );
					*p16++ = ( OMX_S16 )( (pSrc[i]*RSQRT2) & 0xFFFF );
				}
			}
			else 
			{
				for( nCh = 0; nCh < m_pcmType.nChannels; nCh++ )
				{
					p16 = ( OMX_S16 * )( m_outBuffer.Buffer ) + nCh;
					pSrc = (OMX_S32 *) ppnPlayerPCMBuf[nCh];
					for( i = 0; i < nSamples; i++ )
					{
						*p16 = ( OMX_S16 )( pSrc[i] & 0xFFFF );
						p16 += nStride;
					}
				}
			}
		}
		else
		{
			nStride = 6;
			for( nCh = 0; nCh < nStride; nCh++ )
			{
				p16 = ( OMX_S16 * )( m_outBuffer.Buffer ) + ChanMap[nCh];
				pSrc = (OMX_S32 *) pPCMOutput->pSamples[nCh];
				if( pPCMOutput->speakerMask & ( 1 << nCh ) )
				{
					for( i = 0; i < nSamples; i++ )
					{
						*p16 = ( OMX_S16 )( pSrc[i] & 0xFFFF );
						p16 += nStride;
					}
				}
				else
				{
					for( i = 0; i < nSamples; i++ )
					{
						*p16 = 0;
						p16 += nStride;
					}
				}
			}

			if( pPCMOutput->speakerMask & ( 1 << nCh ) )
			{
				p16 = ( OMX_S16 * )( m_outBuffer.Buffer ) + ChanMap[nCh];
				pSrc = (OMX_S32 *) pPCMOutput->pSamples[nCh];
				for( i = 0; i < nSamples; i++ )
				{
					*p16 = ( OMX_S16 )( pSrc[i] & 0xFFFF );
					p16 += nStride;
				}
			}

			if(pPCMOutput->speakerMask == (DTS_MASK_SPEAKER_Lt | DTS_MASK_SPEAKER_Rt | DTS_MASK_SPEAKER_LFE1))
			{
				for( nCh = 0; nCh < 2; nCh++ )
				{
					p16 = ( OMX_S16 * )( m_outBuffer.Buffer ) + nCh;
					pSrc = (OMX_S32 *) ppnPlayerPCMBuf[nCh + 1];
					for( i = 0; i < nSamples; i++ )
					{
						*p16 = ( OMX_S16 )( pSrc[i] & 0xFFFF );
						p16 += nStride;
					}
				}
			}
		}

        m_nOutBuffSize += m_pcmType.nChannels * nSamples * sizeof(OMX_S16);
    }
    else if( m_pcmType.nBitPerSample == 24 )
    {
       nStride = m_pcmType.nChannels * 3;
		if(m_pcmType.nChannels <= 2) 
		{  
			if(pPCMOutput->speakerMask == DTS_MASK_SPEAKER_CENTRE || pPCMOutput->speakerMask == (DTS_MASK_SPEAKER_CENTRE | DTS_MASK_SPEAKER_LFE1))
			{
				p = m_outBuffer.Buffer + nCh * 3;
				pSrc = (OMX_S32 *) ppnPlayerPCMBuf[0];
				for( i = 0; i < nSamples; i++ )
				{
					p[0] = ( dtsUint8 )( pSrc[i] & 0xFF );
					p[1] = ( dtsUint8 )(( pSrc[i] >> 8 ) & 0xFF );
					p[2] = ( dtsUint8 )(( pSrc[i] >> 16 ) & 0xFF );
					p[4] = ( dtsUint8 )( pSrc[i] & 0xFF );
					p[5] = ( dtsUint8 )(( pSrc[i] >> 8 ) & 0xFF );
					p[6] = ( dtsUint8 )(( pSrc[i] >> 16 ) & 0xFF );
					p += nStride;
				}
			}
			else
			{
				for( nCh = 0; nCh < m_pcmType.nChannels; nCh++ )
				{
					p = m_outBuffer.Buffer + nCh * 3;
					pSrc = (OMX_S32 *)ppnPlayerPCMBuf[nCh];
					for( i = 0; i < nSamples; i++ )
					{
						p[0] = ( dtsUint8 )( pSrc[i] & 0xFF );
						p[1] = ( dtsUint8 )(( pSrc[i] >> 8 ) & 0xFF );
						p[2] = ( dtsUint8 )(( pSrc[i] >> 16 ) & 0xFF );
						p += nStride;
					}
				}
			}
		}
		else
		{
			nStride = 6*3;
			for( nCh = 0; nCh < nStride; nCh++ )
			{
				p = m_outBuffer.Buffer + ChanMap[nCh]*3;
				pSrc = (OMX_S32 *) pPCMOutput->pSamples[nCh];
				if( pPCMOutput->speakerMask & ( 1 << nCh ) )
				{
					for( i = 0; i < nSamples; i++ )
					{
						p[0] = ( dtsUint8 )( pSrc[i] & 0xFF );
						p[1] = ( dtsUint8 )(( pSrc[i] >> 8 ) & 0xFF );
						p[2] = ( dtsUint8 )(( pSrc[i] >> 16 ) & 0xFF );
						p += nStride;
					}
				}
				else
				{
					for( i = 0; i < nSamples; i++ )
					{
						p[0] = ( dtsUint8 )0;
						p[1] = ( dtsUint8 )0;
						p[2] = ( dtsUint8 )0;
						p += nStride;
					}
				}
			}

			if( pPCMOutput->speakerMask & ( 1 << nCh ) )
			{
				p = m_outBuffer.Buffer + ChanMap[nCh]*3;
				pSrc = (OMX_S32 *) pPCMOutput->pSamples[nCh];
				for( i = 0; i < nSamples; i++ )
				{
					p[0] = ( dtsUint8 )( pSrc[i] & 0xFF );
					p[1] = ( dtsUint8 )(( pSrc[i] >> 8 ) & 0xFF );
					p[2] = ( dtsUint8 )(( pSrc[i] >> 16 ) & 0xFF );
					p += nStride;
				}
			}

			if(pPCMOutput->speakerMask == (DTS_MASK_SPEAKER_Lt | DTS_MASK_SPEAKER_Rt | DTS_MASK_SPEAKER_LFE1))
			{
				for( nCh = 0; nCh < 2; nCh++ )
				{
					p = m_outBuffer.Buffer + nCh * 3;
					pSrc = (OMX_S32 *) ppnPlayerPCMBuf[nCh + 1];
					for( i = 0; i < nSamples; i++ )
					{
						p[0] = ( dtsUint8 )( pSrc[i] & 0xFF );
						p[1] = ( dtsUint8 )(( pSrc[i] >> 8 ) & 0xFF );
						p[2] = ( dtsUint8 )(( pSrc[i] >> 16 ) & 0xFF );
						p += nStride;
					}
				}
			}
		}

        m_nOutBuffSize += m_pcmType.nChannels * nSamples * 3;
    }

	return OMX_ErrorNone;
}


OMX_ERRORTYPE voCOMXAudioDTSDec::ResetDTSParser()
{
   voCOMXAutoLock lokc (&m_mutCodec); 
	DTSDecParser_SAPI_Deinitialize( parser );
    DTSDecParser_SAPI_Initialize( parser );
    DTSDecParser_SAPI_AllowUnalignedSyncWord( parser );
    if( nExSSID != 0 || nAudioPresentIndex != 0 )
    {
        DTSDecParser_SAPI_SelectAudioPresentation( parser, nExSSID, nAudioPresentIndex );
    }

    nParserFlags = 0;
    bParserNewTimeStamp = OMX_FALSE;
    bParserTimeStampAvailable = OMX_FALSE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioDTSDec::ResetDTSDecoder()
{
   voCOMXAutoLock lokc (&m_mutCodec); 
	DTSDecFramePlayer_SAPI_Deinitialize(player );
    DTSDecFramePlayer_SAPI_Initialize( player );
    DTSDecFramePlayer_SAPI_Config_Init_SpkrOut( player, nSpkrOut );
    if( nExSSID != 0 || nAudioPresentIndex != 0 )
    {
        DTSDecFramePlayer_SAPI_Config_Init_AudioPresentation( player, nExSSID, nAudioPresentIndex );
    }
    if( bMixLFEIntoFront == OMX_TRUE )
    {
        DTSDecFramePlayer_SAPI_Config_Init_Enable_LFE_DMixIntoFront( player );
    }
    DTSDecFramePlayer_SAPI_Config_SetOutputBitwidth( player, nOutputBitWidth );
    DTSDecFramePlayer_SAPI_Config_Init_DRC_Percent( player, nDRCPercent );
    if( bDialNorm == OMX_FALSE )
    {
        DTSDecFramePlayer_SAPI_Config_Init_DisableDialNorm( player );
    }
    nPlayerFlags = 0;
    nPlayerAvailableSamples = 0;

	return OMX_ErrorNone;
}
