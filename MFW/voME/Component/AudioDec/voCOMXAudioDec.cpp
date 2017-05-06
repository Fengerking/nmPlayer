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
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voAMRNB.h"
#include "voAAC.h"
#include "voRealAudio.h"
#include "voEAC3.h"
#include "voAC3.h"
#include "voCOMXAudioDec.h"

#define LOG_TAG "voCOMXAudioDec"
#include "voLog.h"

extern VO_TCHAR * g_pvoOneWorkingPath;

#define VO_AUDIODEC_MAX_PASSABLEERROR		10
voCOMXAudioDec::voCOMXAudioDec(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_nCoding (OMX_AUDIO_CodingUnused)
	, m_pFormatData (NULL)
	, m_nExtSize (0)
	, m_pExtData (NULL)
	, m_nSampleRate (44100)
	, m_nChannels (2)
	, m_nBits (16)
	, m_nStereoMode(2)
	, m_bSetThreadPriority (OMX_FALSE)
	, m_nPassableError (0)
	, mnSourceType(1)
	, m_pAudioDec (NULL)
	, m_nCodec (0)
	, m_nStepSize (4410 * 2)
	, m_nOutputSize (0)
	, m_bNofityError (VO_FALSE)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Decoder.XXX");

	m_inBuffer.Length = 0;
	m_inBuffer.Time = 0;

	cmnMemFillPointer (1);
	m_pMemOP = &g_memOP;

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

	m_nDumpRuntimeLog = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpRuntimeLog", 0);

	m_pCfgCodec = new CBaseConfig ();
	FILE * hFile = fopen ("/sdcard/pvcodec.mmp", "rb");
	if (hFile != NULL)
		fclose (hFile);

	if (hFile  == NULL)
	{
		if (g_pvoOneWorkingPath == NULL)
			vostrcpy (szCfgFile, _T("vommcodec.cfg"));
		else
		{
			vostrcpy (szCfgFile, g_pvoOneWorkingPath);
			vostrcat (szCfgFile, _T("vommcodec.cfg"));
		}
		m_pCfgCodec->Open (szCfgFile);
	}
	else
	{
		if (g_pvoOneWorkingPath == NULL)
			vostrcpy (szCfgFile, _T("vommcodec_pv.cfg"));
		else
		{
			vostrcpy (szCfgFile, g_pvoOneWorkingPath);
			vostrcat (szCfgFile, _T("vommcodec_pv.cfg"));
		}
		m_pCfgCodec->Open (szCfgFile);
	}

	if (m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpLog", 0) > 0)
	{
		m_nPfmFrameSize = 10240;
		m_pPfmFrameTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCodecThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmCompnThreadTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmMediaTime = new OMX_U32[m_nPfmFrameSize];
		m_pPfmSystemTime = new OMX_U32[m_nPfmFrameSize];
	}

	char * pFile = m_pCfgComponent->GetItemText (m_pName, (char*)"DumpInputDataFile");
	if (pFile != NULL)
	{
#ifdef _WIN32
		TCHAR szFile[256];
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, pFile, -1, szFile, sizeof (szFile)); \
		m_pDumpInputFile = voOMXFileOpen ((OMX_STRING)szFile, VOOMX_FILE_WRITE_ONLY);		
#else
		m_pDumpInputFile = voOMXFileOpen (pFile, VOOMX_FILE_WRITE_ONLY);
#endif // _WIN32
		m_nDumpInputFormat = m_pCfgComponent->GetItemValue (m_pName, (char*)"DumpInputDataFormat", 0);
		VOLOGI ("Create Input file %s, Format is %d The handle is 0X%08X.", pFile, (int)m_nDumpInputFormat, (int)m_pDumpInputFile);
	}
	pFile = m_pCfgComponent->GetItemText (m_pName, (char*)"DumpOututDataFile");
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
	m_nPassCodec = m_pCfgComponent->GetItemValue (m_pName, (char*)"PassCodec", 0);
	m_nLogLevel = m_pCfgComponent->GetItemValue (m_pName, (char*)"ShowLogLevel", 0);

	OMX_PTR hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoAACDec.so"), VOOMX_FILE_READ_ONLY);
	if (hCodecFile != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingAAC;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoMP3Dec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingMP3;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoAMRNBDec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingAMR;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoWMADec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingWMA;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoQCELPDec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingQCELP13;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoEVRCDec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingEVRC;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoAC3Dec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoRADec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = OMX_AUDIO_CodingRA;
		voOMXFileClose (hCodecFile);
	}
	else if ((hCodecFile = voOMXFileOpen ((OMX_STRING)_T("/system/lib/libvoFLACDec.so"), VOOMX_FILE_READ_ONLY)) != NULL)
	{
		m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingFlac;
		voOMXFileClose (hCodecFile);
	}
}

voCOMXAudioDec::~voCOMXAudioDec(void)
{
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

	if (m_pAudioDec != NULL)
	{
		delete m_pAudioDec;
		m_pAudioDec = NULL;
	}
 
	delete m_pCfgCodec;

//	if (m_pCallBack != NULL)
//		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);
}

OMX_ERRORTYPE voCOMXAudioDec::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	return voCOMXCompFilter::EmptyThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXAudioDec::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
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

	case OMX_IndexParamAudioWma:
		{
			OMX_AUDIO_PARAM_WMATYPE * pWMAFormat = (OMX_AUDIO_PARAM_WMATYPE*)pComponentParameterStructure;

			if(pWMAFormat->nPortIndex == 0 && m_pFormatData)
			{
				voOMXMemCopy (pWMAFormat, m_pFormatData, sizeof (OMX_AUDIO_PARAM_WMATYPE));
			}

			pWMAFormat->nChannels = (OMX_U16)m_nChannels;
			pWMAFormat->nSamplingRate = pWMAFormat->nSamplingRate;

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioQcelp13:
		{
			OMX_AUDIO_PARAM_QCELP13TYPE * pQCELP13Format = (OMX_AUDIO_PARAM_QCELP13TYPE*)pComponentParameterStructure;

			if (pQCELP13Format->nPortIndex == 0)
			{
				if(m_pFormatData) 
				{
					voOMXMemCopy (pQCELP13Format, m_pFormatData, sizeof (OMX_AUDIO_PARAM_QCELP13TYPE));
				}
				else
				{
					pQCELP13Format->nChannels = 1;
				}			
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioEvrc:
		{
			OMX_AUDIO_PARAM_EVRCTYPE * pEVRCFormat = (OMX_AUDIO_PARAM_EVRCTYPE*)pComponentParameterStructure;

			if (pEVRCFormat->nPortIndex == 0)
			{
				if(m_pFormatData) 
				{
					voOMXMemCopy (pEVRCFormat, m_pFormatData, sizeof (OMX_AUDIO_PARAM_EVRCTYPE));
				}
				else
				{
					pEVRCFormat->nChannels = 1;
				}
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioMp3:
		{
			OMX_AUDIO_PARAM_MP3TYPE * pMP3Format = (OMX_AUDIO_PARAM_MP3TYPE*)pComponentParameterStructure;

			if (pMP3Format->nPortIndex == 0)
			{
				if(m_pFormatData)
				{
					voOMXMemCopy (pMP3Format, m_pFormatData, sizeof (OMX_AUDIO_PARAM_MP3TYPE));
					pMP3Format->nChannels = m_pcmType.nChannels;
					pMP3Format->nSampleRate = m_pcmType.nSamplingRate;
				}
				else
				{
					pMP3Format->nChannels = m_pcmType.nChannels;
					pMP3Format->nSampleRate = m_pcmType.nSamplingRate;
				}
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioRa:
		{
			OMX_AUDIO_PARAM_RATYPE * pRAFormat = (OMX_AUDIO_PARAM_RATYPE*)pComponentParameterStructure;

			if (pRAFormat->nPortIndex == 0)
			{
				if(m_pFormatData)
				{
					voOMXMemCopy (pRAFormat, m_pFormatData, sizeof (OMX_AUDIO_PARAM_RATYPE));
					pRAFormat->nChannels = m_pcmType.nChannels;
					pRAFormat->nSamplingRate = m_pcmType.nSamplingRate;
				}
				else
				{
					pRAFormat->nChannels = m_pcmType.nChannels;
					pRAFormat->nSamplingRate = m_pcmType.nSamplingRate;
				}
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAmr:
		{
			OMX_AUDIO_PARAM_AMRTYPE * pAMRFormat = (OMX_AUDIO_PARAM_AMRTYPE*)pComponentParameterStructure;

			if (pAMRFormat->nPortIndex == 0)
			{
				if(m_pFormatData) 
				{
					voOMXMemCopy (pAMRFormat, m_pFormatData, sizeof (OMX_AUDIO_PARAM_AMRTYPE));
				}
				else
				{
					pAMRFormat->nChannels = 1;
				}
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAac:
		{
			OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)pComponentParameterStructure;

			if (pAACFormat->nPortIndex == 0)
			{
				int chanMode = 0;
				if(m_pFormatData) 
				{					
					voOMXMemCopy (pAACFormat, m_pFormatData, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
					pAACFormat->nChannels = m_pcmType.nChannels;
					pAACFormat->nSampleRate = m_pcmType.nSamplingRate;
					pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMax;
							
					if(m_pAudioDec) 
					{
						m_pAudioDec->GetParam(VO_PID_AUDIO_CHANNELMODE, &chanMode);
						if(chanMode <= VO_AUDIO_CHANNEL_MONO)
						{
							pAACFormat->eChannelMode = (OMX_AUDIO_CHANNELMODETYPE)chanMode;
						}
					}
					else
					{
						if(pAACFormat->nChannels == 1)
						{
							pAACFormat->eChannelMode = OMX_AUDIO_ChannelModeMono;
						}
						else 
						{
							pAACFormat->eChannelMode = OMX_AUDIO_ChannelModeStereo;
						}
					}
				}
				else
				{
					pAACFormat->nChannels = m_pcmType.nChannels;
					pAACFormat->nSampleRate = m_pcmType.nSamplingRate;
					pAACFormat->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMax;

					if(m_pAudioDec) 
					{
						m_pAudioDec->GetParam(VO_PID_AUDIO_CHANNELMODE, &chanMode);
						if(chanMode <= VO_AUDIO_CHANNEL_MONO)
						{
							pAACFormat->eChannelMode = (OMX_AUDIO_CHANNELMODETYPE)chanMode;
						}
					}
					else
					{
						if(pAACFormat->nChannels == 1)
						{
							pAACFormat->eChannelMode = OMX_AUDIO_ChannelModeMono;
						}
						else 
						{
							pAACFormat->eChannelMode = OMX_AUDIO_ChannelModeStereo;
						}
					}
				}
			}
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioAdpcm:
		{
			OMX_AUDIO_PARAM_ADPCMTYPE * pADPCMFormat = (OMX_AUDIO_PARAM_ADPCMTYPE*)pComponentParameterStructure;

			if (pADPCMFormat->nPortIndex == 0)
			{
				if(m_pFormatData) 
				{
					voOMXMemCopy (pADPCMFormat, m_pFormatData, sizeof (OMX_AUDIO_PARAM_ADPCMTYPE));
				}
				else
				{
					pADPCMFormat->nChannels = 2;
					pADPCMFormat->nSampleRate = 44100;
				}
			}

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
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAMRWBP)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAPE)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingALAC)
					errType = OMX_ErrorNone;
				else if (m_nCoding == OMX_AUDIO_CodingVORBIS)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingFlac)
					errType = OMX_ErrorNone;
				else
				{
					VOLOGW ("The coding is %d.", m_nCoding);
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
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
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

	case OMX_IndexParamAudioRa:
		{
			OMX_AUDIO_PARAM_RATYPE * pRAFormat = (OMX_AUDIO_PARAM_RATYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pRAFormat->nPortIndex, pRAFormat, sizeof(OMX_AUDIO_PARAM_RATYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_RATYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_RATYPE));

			pRAFormat = (OMX_AUDIO_PARAM_RATYPE*)m_pFormatData;

			if (pRAFormat->nChannels > 0)
			{
				m_nChannels = pRAFormat->nChannels;
				m_nSampleRate = pRAFormat->nSamplingRate;

				m_pcmType.nChannels = pRAFormat->nChannels;
				m_pcmType.nSamplingRate = pRAFormat->nSamplingRate;
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
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
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
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
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
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
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
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
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

	case OMX_IndexParamAudioQcelp13:
		{
			OMX_AUDIO_PARAM_QCELP13TYPE * pQCELP13Format = (OMX_AUDIO_PARAM_QCELP13TYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pQCELP13Format->nPortIndex, pQCELP13Format, sizeof(OMX_AUDIO_PARAM_QCELP13TYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_QCELP13TYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_QCELP13TYPE));

			pQCELP13Format = (OMX_AUDIO_PARAM_QCELP13TYPE*)m_pFormatData;

			if (pQCELP13Format->nChannels > 0)
			{
				m_nChannels = pQCELP13Format->nChannels;
				m_nSampleRate = 8000;
				m_pcmType.nChannels =m_nChannels;
				m_pcmType.nSamplingRate = m_nSampleRate;
			}

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioEvrc:
		{
			OMX_AUDIO_PARAM_EVRCTYPE * pEVRCFormat = (OMX_AUDIO_PARAM_EVRCTYPE*)pComponentParameterStructure;
			errType = CheckParam(pComp, pEVRCFormat->nPortIndex, pEVRCFormat, sizeof(OMX_AUDIO_PARAM_EVRCTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (m_pFormatData != NULL)
				voOMXMemFree (m_pFormatData);
			m_pFormatData = voOMXMemAlloc (sizeof (OMX_AUDIO_PARAM_EVRCTYPE));
			if (m_pFormatData == NULL)
				return OMX_ErrorInsufficientResources;
			voOMXMemCopy (m_pFormatData, pComponentParameterStructure, sizeof (OMX_AUDIO_PARAM_EVRCTYPE));

			pEVRCFormat = (OMX_AUDIO_PARAM_EVRCTYPE*)m_pFormatData;

			if (pEVRCFormat->nChannels > 0)
			{
				m_nChannels = pEVRCFormat->nChannels;
				m_nSampleRate = 8000;
				m_pcmType.nChannels =m_nChannels;
				m_pcmType.nSamplingRate = m_nSampleRate;
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

           m_pcmType.nChannels = pData->nChannels;
           m_pcmType.nBitPerSample = pData->nBitPerSample;
           m_pcmType.nSamplingRate = pData->nSamplingRate;
		   m_nChannels = m_pcmType.nChannels;
		   m_nSampleRate = m_pcmType.nSamplingRate;
		   m_nBits    = m_pcmType.nBitPerSample;

			VOLOGI ("m_nChannels %d, m_nSampleRate: %d, m_nBits: %d", (int)m_nChannels, (int)m_nSampleRate, (int)m_nBits);

           errType = OMX_ErrorNone;
        }
        break;
	case OMX_VO_IndexAudioParamAC3:
        {
		   OMX_VO_AUDIO_PARAM_AC3TYPE * pAC3Format = (OMX_VO_AUDIO_PARAM_AC3TYPE *) pComponentParameterStructure;
		   errType = CheckParam(pComp, pAC3Format->nPortIndex, pAC3Format, sizeof(OMX_VO_AUDIO_PARAM_AC3TYPE));
		   if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

           m_pcmType.nChannels = pAC3Format->nChannels;
           m_pcmType.nSamplingRate = pAC3Format->nSampleRate;
		   m_nChannels = m_pcmType.nChannels;
		   m_nSampleRate = m_pcmType.nSamplingRate;
		   m_nBits    = m_pcmType.nBitPerSample;

			VOLOGI ("m_nChannels %d, m_nSampleRate: %d, m_nBits: %d", (int)m_nChannels, (int)m_nSampleRate, (int)m_nBits);

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
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.qcelp13"))
				m_nCoding = OMX_AUDIO_CodingQCELP13;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.evrc"))
				m_nCoding = OMX_AUDIO_CodingEVRC;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.adpcm"))
				m_nCoding = OMX_AUDIO_CodingADPCM;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.ra"))
				m_nCoding = OMX_AUDIO_CodingRA;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.ogg"))
				m_nCoding = OMX_AUDIO_CodingVORBIS;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.ac3"))
				m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.eac3"))
				m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingEAC3;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.amrwbp"))
				m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAMRWBP;
			else if (!strcmp ((char *)pRoleType->cRole, "audio_decoder.flac"))
				m_nCoding = (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingFlac;

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

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_VO_IndexSourceType:
		{
			mnSourceType = *(OMX_U32 *)pComponentParameterStructure;
			errType = OMX_ErrorNone;
		}
		break;

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDec::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure)
{
	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	//OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentConfigStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{	
	case OMX_VO_IndexConfigAudioChanConfig:
		if(pComponentConfigStructure && m_pAudioDec)
		{
			m_pAudioDec->SetParam(VO_PID_AUDIO_CHANNELCONFIG, pComponentConfigStructure);
		}

		errType = OMX_ErrorNone;
		break;

	case OMX_VO_IndexConfigAC3Effect:
		m_nStereoMode = (*(int *)pComponentConfigStructure);	
		if(m_pAudioDec)
		{
			VOLOGI ("Set the AC3 Audio Effect %d", (int)m_nStereoMode);
			
			if(m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingEAC3)
			{
				m_pAudioDec->SetParam(VO_PID_EAC3_DownMix_Mode, &m_nStereoMode);
			}
			else if(m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3)
			{
				m_pAudioDec->SetParam(VO_PID_AC3_STEREOMODE, &m_nStereoMode);
			}
		}

		errType = OMX_ErrorNone;
		break;

	default:
		errType = voCOMXCompFilter::SetConfig(hComponent, nIndex, pComponentConfigStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDec::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure)
{

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	//OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentConfigStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{	
	case OMX_VO_IndexConfigAudioChanConfig:
		if(pComponentConfigStructure && m_pAudioDec)
		{
			m_pAudioDec->GetParam(VO_PID_AUDIO_CHANNELCONFIG, pComponentConfigStructure);
		}

		errType = OMX_ErrorNone;
		break;

	case OMX_VO_IndexConfigAC3Effect:
		(*(int *)pComponentConfigStructure) = m_nStereoMode;

		errType = OMX_ErrorNone;
		break;

	default:
		errType = voCOMXCompFilter::GetConfig(hComponent, nIndex, pComponentConfigStructure);
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
		strcpy ((char *)cRole, "audio_decoder.qcelp13");
	else if (nIndex == 5)
		strcpy ((char *)cRole, "audio_decoder.evrc");
	else if (nIndex == 6)
		strcpy ((char *)cRole, "audio_decoder.adpcm");
	else if (nIndex == 7)
		strcpy ((char *)cRole, "audio_decoder.ac3");
	else if (nIndex == 8)
		strcpy ((char *)cRole, "audio_decoder.amrwbp");
	else if (nIndex == 9)
		strcpy ((char *)cRole, "audio_decoder.eac3");
	else
		return OMX_ErrorNoMore;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioDec::SetNewState (OMX_STATETYPE newState)
{
	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		int nBits;

		if (m_nSampleRate < 8000)
			m_nSampleRate = 44100;
		if (m_nChannels == 0)
			m_nChannels = 2;

		nBits = m_nBits;

		if (m_pAudioDec == NULL)
		{
			if (m_nCoding == OMX_AUDIO_CodingMP3)
			{
				m_nCodec = VO_AUDIO_CodingMP3;
			}
			else if (m_nCoding == OMX_AUDIO_CodingAAC)
			{
				m_nCodec = VO_AUDIO_CodingAAC;
			}
			else if (m_nCoding == OMX_AUDIO_CodingAMR)
			{
				OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE *) m_pFormatData;
				if (pAmrFormat != NULL)
				{
					if (pAmrFormat->eAMRBandMode >= OMX_AUDIO_AMRBandModeNB0 && pAmrFormat->eAMRBandMode <= OMX_AUDIO_AMRBandModeNB7)
						m_nCodec = VO_AUDIO_CodingAMRNB;
					else if (pAmrFormat->eAMRBandMode >= OMX_AUDIO_AMRBandModeWB0 && pAmrFormat->eAMRBandMode <= OMX_AUDIO_AMRBandModeWB8)
						m_nCodec = VO_AUDIO_CodingAMRWB;
					else
						m_nCodec = VO_AUDIO_CodingAMRNB;
				}
			}
			else if (m_nCoding == OMX_AUDIO_CodingWMA)
			{
				m_nCodec = VO_AUDIO_CodingWMA;
			}
			else if (m_nCoding == OMX_AUDIO_CodingADPCM)
			{
				m_nCodec = VO_AUDIO_CodingADPCM;
			}
			else if (m_nCoding == OMX_AUDIO_CodingQCELP13)
			{
				m_nCodec = VO_AUDIO_CodingQCELP13;
			}
			else if (m_nCoding == OMX_AUDIO_CodingEVRC)
			{
				m_nCodec = VO_AUDIO_CodingEVRC;
			}
			else if (m_nCoding == OMX_AUDIO_CodingRA)
			{
				m_nCodec = VO_AUDIO_CodingRA;
			}
			else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
			{
				m_nCodec = VO_AUDIO_CodingAC3;
			}
			else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3)
			{
				m_nCodec = VO_AUDIO_CodingEAC3;
			}
			else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAMRWBP)
			{
				m_nCodec = VO_AUDIO_CodingAMRWBP;
			}
			else if (m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAPE)
			{
				m_nCodec = VO_AUDIO_CodingAPE;
			}
			else if (m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingALAC)
			{
				m_nCodec = VO_AUDIO_CodingALAC;
			}
			else if (m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingFlac)
			{
				m_nCodec = VO_AUDIO_CodingFLAC;
			}
			else if (m_nCoding == OMX_AUDIO_CodingVORBIS)
				m_nCodec = VO_AUDIO_CodingOGG;

			m_pAudioDec = new CAudioDecoder (NULL, m_nCodec, m_pMemOP);
			if (m_pAudioDec == NULL)
			{
				VOLOGE ("Create the audio decoder failed.");
				return OMX_ErrorInsufficientResources;
			}
			m_pAudioDec->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
			m_pAudioDec->SetOMXComp (VO_TRUE);
			m_pAudioDec->SetConfig (m_pCfgCodec);
			m_pAudioDec->SetWorkPath ((VO_TCHAR *)m_pWorkPath);


			if (m_nCoding == OMX_AUDIO_CodingAAC)
			{
				OMX_AUDIO_PARAM_AACPROFILETYPE * pAACFormat = (OMX_AUDIO_PARAM_AACPROFILETYPE*)m_pFormatData;
				if (pAACFormat != NULL)
				{
					if (pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatADIF)
					{
						m_pAudioDec->SetFourCC (0XF6);
					}
					else if (pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP2ADTS ||
						pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4ADTS)
					{
						m_pAudioDec->SetFourCC (0XF7);
					}
					else if (pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4LATM)
					{
						m_pAudioDec->SetFourCC (0XF8);
					}
					else if (pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatMP4LOAS)
					{
						m_pAudioDec->SetFourCC (0XF9);
					}
					else if (pAACFormat->eAACStreamFormat  == OMX_AUDIO_AACStreamFormatMP4FF ||
						pAACFormat->eAACStreamFormat == OMX_AUDIO_AACStreamFormatRAW)
					{
						m_pAudioDec->SetFourCC (0XFF);
					}
					
					//send the samplebits use this ID nAudioBandWidth
					if(pAACFormat->nAudioBandWidth == 16 || pAACFormat->nAudioBandWidth == 32)
						nBits = pAACFormat->nAudioBandWidth;

				}
			}
			else if (m_nCoding == OMX_AUDIO_CodingAMR)
			{
				OMX_AUDIO_PARAM_AMRTYPE * pAmrFormat = (OMX_AUDIO_PARAM_AMRTYPE*)m_pFormatData;
				VOAMRNBFRAMETYPE format = VOAMRNB_RFC3267;

				if (pAmrFormat != NULL)
				{
					if(pAmrFormat->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatIF1)
					{
						format = VOAMRNB_IF1;
					}
					else if(pAmrFormat->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatIF2)
					{
						format = VOAMRNB_IF2;
					}
					else if(pAmrFormat->eAMRFrameFormat == OMX_AUDIO_AMRFrameFormatFSF)
					{
						format = VOAMRNB_RFC3267;
					}
					else
					{
						format = VOAMRNB_RFC3267;
					}
				}

				m_pAudioDec->SetParam(VO_PID_AMRNB_FRAMETYPE, &format);
			}

			VO_AUDIO_FORMAT	fmtAudio;
			fmtAudio.Channels = m_nChannels;
			fmtAudio.SampleRate = m_nSampleRate;
			fmtAudio.SampleBits = nBits;

			int nStepTime = m_pCfgComponent->GetItemValue (m_pName, (char*)"StepDuration", 200);
			m_nStepSize = m_nSampleRate * m_nChannels * m_nBits  / (1000 * 8 / nStepTime);

			VOLOGI ("m_nChannels %d, m_nSampleRate: %d, m_nBits: %d", (int)m_nChannels, (int)m_nSampleRate, (int)m_nBits);

			VO_U32 nRC = m_pAudioDec->Init ((VO_PBYTE)m_pExtData, m_nExtSize, &fmtAudio);
			if (nRC != VO_ERR_NONE)
			{
				VOLOGE ("m_pAudioDec->Init was failed. 0X%08X", (int)nRC);
				if(m_pCallBack)
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecUnsupported, 0, 0, NULL);
				return OMX_ErrorResourcesLost;
			}

			if(m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingEAC3)
			{
				m_pAudioDec->SetParam(VO_PID_EAC3_DownMix_Mode, &m_nStereoMode);
			}
			else if(m_nCoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3)
			{
				m_pAudioDec->SetParam(VO_PID_AC3_STEREOMODE, &m_nStereoMode);
			}
		}
	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		if (m_pAudioDec != NULL)
			m_pAudioDec->Flush ();

		m_inBuffer.Length = 0;
		m_nOutputSize = 0;
	}
	else if (newState == OMX_StateIdle)
	{
		if (m_sTrans == COMP_TRANSSTATE_PauseToIdle || m_sTrans == COMP_TRANSSTATE_ExecuteToIdle)
			ResetPortBuffer ();

		m_inBuffer.Length = 0;
		m_nOutBuffTime = 0;
		m_nOutBuffSize = 0;

		VOLOGI ("Audio Dec output time %d", (int)((m_nOutputSize * 1000) / (m_nSampleRate * m_nChannels * m_nBits / 8)));
	}

	m_bNofityError = VO_FALSE;
	m_inBuffer.Time = 0;

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioDec::Flush (OMX_U32	nPort)
{
	voCOMXAutoLock lokc (&m_mutCodec);

	OMX_ERRORTYPE errType = voCOMXCompFilter::Flush (nPort);

	m_inBuffer.Length = 0;
	m_inBuffer.Time = 0;
	m_nOutBuffTime = 0;
	m_nOutBuffSize = 0;
	m_bNofityError = VO_FALSE;

	if (m_pAudioDec != NULL)
		m_pAudioDec->Flush ();

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDec::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, (int)pInput->nFilledLen, (int)pInput->nTimeStamp);

	if (!m_bSetThreadPriority)
	{
		voOMXThreadSetPriority (m_hBuffThread, VOOMX_THREAD_PRIORITY_ABOVE_NORMAL);
		m_bSetThreadPriority = OMX_TRUE;
	}

	if (m_nPassableError >= VO_AUDIODEC_MAX_PASSABLEERROR)
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;

		return OMX_ErrorNone;
	}

	OMX_U32 nRC = 0;
	if (m_inBuffer.Length == 0)
	{
		m_inBuffer.Buffer = pInput->pBuffer + pInput->nOffset;
		m_inBuffer.Length = pInput->nFilledLen;

		nRC = m_pAudioDec->SetInputData (&m_inBuffer);
		if (nRC != VO_ERR_NONE)
		{
			if(nRC != VO_ERR_INPUT_BUFFER_SMALL && m_pCallBack)
				m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecPassableError, 1, 0, NULL);

			*pEmptied = OMX_TRUE;
			m_inBuffer.Length = 0;

			pOutput->nFilledLen = 0;
			*pFilled = OMX_FALSE;

			VOLOGW ("Set Input Failed! 0X%08X", (int)nRC);
			return OMX_ErrorNone;
		}

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
	}

	if (m_nLogLevel > 0)
	{
		if (m_nLogSysStartTime == 0)
			m_nLogSysStartTime = voOMXOS_GetSysTime ();

		VOLOGI ("Sys Time: %d  frame time is %d, Diff %d, size %d", (int)(voOMXOS_GetSysTime () - m_nLogSysStartTime), (int)pInput->nTimeStamp, (int)((voOMXOS_GetSysTime () - m_nLogSysStartTime) - pInput->nTimeStamp), (int)pInput->nFilledLen);

		m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
	}


	VO_AUDIO_FORMAT		fmtAudio;
	m_outBuffer.Buffer = pOutput->pBuffer + (OMX_U32)m_nOutBuffSize;
	m_outBuffer.Length = pOutput->nAllocLen -  (OMX_U32)m_nOutBuffSize;

	nRC = GetOutputData (&m_outBuffer, &fmtAudio, (VO_U32)pInput->nTimeStamp);
	if(m_nDumpRuntimeLog)
	{
		VOLOGI ("nRC 0x%08X, Size %d, Time %d, Duration %d", (int)nRC, (int)m_outBuffer.Length, (int)pInput->nTimeStamp, (int)((m_outBuffer.Length * 8000) / (m_nSampleRate * m_nBits * m_nChannels)));
	}

	if(VO_ERR_NONE != nRC && VO_ERR_INPUT_BUFFER_SMALL != nRC)
	{
		if(m_pCallBack)
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_VO_Event_CodecPassableError, 1, 0, NULL);

		// for streaming, we will block MAX_PASSABLEERROR, East 20111013
		if(0 != mnSourceType)
			m_nPassableError++;
		if(m_nPassableError >= VO_AUDIODEC_MAX_PASSABLEERROR)
		{
			VOLOGW ("m_nPassableError reach max passable error %d", (int)m_nPassableError);

			pOutput->nFlags |= OMX_BUFFERFLAG_EOS;
			pOutput->nFilledLen  = 0;
			pOutput->nTimeStamp = m_nOutBuffTime;

			m_nOutBuffTime = m_nOutBuffTime + (m_nOutBuffSize * 1000LL) / (m_nSampleRate * m_nChannels * m_nBits / 8);
			m_nOutBuffSize = 0;

			*pFilled = OMX_TRUE;

			*pEmptied = OMX_TRUE;
			m_inBuffer.Length = 0;

			return OMX_ErrorNone;
		}
	}

	if (m_nLogLevel > 0)
	{
		VOLOGI ("GetOutputData used time %d., Size: %d, Result 0X%08X", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime), (int)m_outBuffer.Length, (int)nRC);
	}

	if (nRC == VO_ERR_NONE &&
		((int)fmtAudio.Channels != (int)m_pcmType.nChannels || (int)fmtAudio.SampleRate != (int)m_pcmType.nSamplingRate || (int)fmtAudio.SampleBits != (int)m_pcmType.nBitPerSample))
	{
		VOLOGI ("Audio Format was changed. S %d, C %d", (int)m_pcmType.nSamplingRate, (int)m_pcmType.nChannels);

		pOutput->nFilledLen = 0;
		m_pOutput->ReturnBuffer (pOutput);
		m_pOutputBuffer = NULL;
		*pFilled = OMX_FALSE;

		m_pcmType.nSamplingRate = fmtAudio.SampleRate;
		m_pcmType.nChannels = fmtAudio.Channels;
		m_pcmType.nBitPerSample = fmtAudio.SampleBits;

		m_nSampleRate = fmtAudio.SampleRate;
		m_nChannels = fmtAudio.Channels;
		m_nBits = fmtAudio.SampleBits;

		int nStepTime = m_pCfgComponent->GetItemValue (m_pName, (char*)"StepDuration", 200);
		m_nStepSize = m_nSampleRate * m_nChannels * m_nBits  / (1000 * 8 / nStepTime);
		// m_nStepSize = m_pcmType.nSamplingRate * m_pcmType.nChannels * 2 / 5;

		m_pOutput->SettingsChanging();

		if (m_pCallBack != NULL)
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventPortSettingsChanged, 1, 0, NULL);
		
		return OMX_ErrorNone;
	}

	if(pOutput->nFlags & OMX_BUFFERFLAG_EOS)
	{
		if(nRC == VO_ERR_NONE && m_outBuffer.Length > 0)
		{
			m_nOutBuffSize = m_nOutBuffSize + m_outBuffer.Length;
		}

		pOutput->nFilledLen  = (OMX_U32)m_nOutBuffSize;
		pOutput->nTimeStamp = m_nOutBuffTime ;

		m_nOutBuffTime = m_nOutBuffTime + (m_nOutBuffSize * 1000LL) / (m_nSampleRate * m_nChannels * m_nBits / 8);
		m_nOutBuffSize = 0;

		*pFilled = OMX_TRUE;

		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;	
	}
	else if (nRC == VO_ERR_INPUT_BUFFER_SMALL)
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
	else if (nRC == VO_ERR_NONE && m_outBuffer.Length > 0)
	{
		if(m_nPassableError < VO_AUDIODEC_MAX_PASSABLEERROR)
			m_nPassableError = 0;

		pOutput->nFilledLen = m_outBuffer.Length;
		m_nOutBuffSize = m_nOutBuffSize + m_outBuffer.Length;

		if (m_nOutBuffSize >= m_nStepSize)
//		if (m_nOutBuffSize >= 0)
		{
			pOutput->nFilledLen  = (OMX_U32)m_nOutBuffSize;
			pOutput->nTimeStamp = m_nOutBuffTime ;

			m_nOutBuffTime = m_nOutBuffTime + (m_nOutBuffSize * 1000LL) / (m_nSampleRate * m_nChannels * m_nBits / 8);
			m_nOutBuffSize = 0;

		//	VOLOGI ("Audio Time: %d", (int)pOutput->nTimeStamp);

			*pFilled = OMX_TRUE;

			m_nOutputSize += pOutput->nFilledLen;
		}
		else
		{
			*pFilled = OMX_FALSE;
		}
	}
	else if(m_nCoding == OMX_AUDIO_CodingAAC && (nRC == (VO_ERR_AAC_UNSPROFILE & 0X8000FFFF)|| nRC == (VO_ERR_AAC_UNSFILEFORMAT & 0X8000FFFF)))
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;

		if (m_nOutBuffTime > 0 && m_inBuffer.Time > 0)
		{
			OMX_S64 nFrameTime = m_nOutBuffTime - m_inBuffer.Time;
			pOutput->nFilledLen = (OMX_U32)(nFrameTime * (m_nSampleRate * m_nChannels * m_nBits / 8) / 1000);
			pOutput->nFilledLen = pOutput->nFilledLen & 0XFFFFFFFC;
			if (pOutput->nFilledLen > pOutput->nAllocLen)
				pOutput->nFilledLen = pOutput->nAllocLen;
			memset (pOutput->pBuffer + pOutput->nOffset, 0, pOutput->nFilledLen);
			*pFilled = OMX_TRUE;
		}
		else
		{
			pOutput->nFilledLen = 0;
			*pFilled = OMX_FALSE;
		}

		m_inBuffer.Time = m_nOutBuffTime;

		if( m_pCallBack != NULL && !m_bNofityError) 
		{
			m_bNofityError = VO_TRUE;

			VOLOGE ("Audio : Not supported profile  ! ");
			m_pCallBack->EventHandler(m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventError, 0, 0, 0);
		}
	}
	else
	{
//		*pEmptied = OMX_TRUE;
//		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;

		if(VO_ERR_NONE != nRC)
		{
			VOLOGW ("Decoder Audio error 0X%08X.", (int)nRC);
		}
	}

	return OMX_ErrorNone;
}

VO_U32 voCOMXAudioDec::GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat, VO_U32 nStart)
{
	if (m_nPfmFrameIndex < m_nPfmFrameSize)
	{
		m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
		m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL);
		m_pPfmCompnThreadTime[m_nPfmFrameIndex] = m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		m_pPfmMediaTime[m_nPfmFrameIndex] = (VO_U32) nStart;
		m_pPfmSystemTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime ();
	}

//	voCOMXAutoLock lokc (&m_mutCodec);

	VO_U32	nRC = m_pAudioDec->GetOutputData (pOutput, pAudioFormat);

	if (nRC == VO_ERR_NONE && (pOutput->Buffer[0]))
	{												   
		if (m_nPfmFrameIndex < m_nPfmFrameSize)
		{
			m_pPfmFrameTime[m_nPfmFrameIndex] = voOMXOS_GetSysTime () - m_pPfmFrameTime[m_nPfmFrameIndex];
			m_pPfmCodecThreadTime[m_nPfmFrameIndex] = voOMXOS_GetThreadTime (NULL) - m_pPfmCodecThreadTime[m_nPfmFrameIndex];
		}
		m_nPfmFrameIndex++;
	}

	return nRC;
}

OMX_ERRORTYPE voCOMXAudioDec::InitPortType (void)
{
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
