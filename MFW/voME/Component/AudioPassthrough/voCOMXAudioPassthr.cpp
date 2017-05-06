	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioPassthr.cpp

	Contains:	voCOMXAudioPassthr class file

	Written by:	Rogine Xu

	Change History (most recent first):
	2012-05-09	XRJ			Create file

*******************************************************************************/
#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "voOMXFile.h"
#include "voEAC3.h"
#include "voAC3.h"
#include "voCOMXAudioPassthr.h"

#define LOG_TAG "voCOMXAudioPassthr"

#include "voLog.h"

extern VO_TCHAR * g_pvoOneWorkingPath;

#define HOLD_SAMPLE_NUM							   1
#define PassthrAC3Len							6144
#define PassthrDecSampleEqual_AC3				 256
#define PassthrDTSLen						    2048 
#define VO_ERR_INPUT_BUFFER_REMAIN		  0x80000011
#define VO_ERR_INPUT_BUFFER_SAMPLE_SKIP	  0x80000012
#define Complete_Sample					  0x10101010

voCOMXAudioPassthr::voCOMXAudioPassthr(OMX_COMPONENTTYPE * pComponent)
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
	, m_nCodec (0)
	, m_nStepSize (4410 * 2)
	, m_nOutputSize (0)
	, m_bNofityError (VO_FALSE)
	, m_isAssbBuffer (OMX_FALSE)
	, m_bPass (OMX_FALSE)
	, m_nRoute (0)
	, m_nFrameSize (0)
	, m_nReadOffset (0)
	, m_nInputNum (0)
	, m_nFillSampleNum (0)
	, m_nPassthrDTSLen (2048)
	, m_pAccumBuffer (NULL)
	, m_nAccumLength (0)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.Passthrough");

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

	/////
	m_pAccumBuffer = (OMX_U8*)voOMXMemAlloc(61440);
	if (m_pAccumBuffer==NULL)
		VOLOGE("@#@#@# m_pAccumBuffer allocate memory error!!")
}

voCOMXAudioPassthr::~voCOMXAudioPassthr(void)
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

	if (m_pCallBack != NULL)
		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);

	if (m_pAccumBuffer != NULL)
		voOMXMemFree(m_pAccumBuffer);
}

OMX_ERRORTYPE voCOMXAudioPassthr::EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
	return voCOMXCompFilter::EmptyThisBuffer (hComponent, pBuffer);
}

OMX_ERRORTYPE voCOMXAudioPassthr::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
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
			if (pAudioFormat->nPortIndex == 0 || pAudioFormat->nPortIndex == 1)
				pAudioFormat->eEncoding = m_nCoding;
			
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


OMX_ERRORTYPE voCOMXAudioPassthr::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
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

				if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingDTS)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
					errType = OMX_ErrorNone;
				else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3)
				{
					//m_nCoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3;  //same as AC3
					errType = OMX_ErrorNone;
				}
				else
				{
					VOLOGR ("The coding is %d.", m_nCoding);
					errType = OMX_ErrorComponentNotFound;
				}
			}
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

		   VOLOGI ("m_nChannels %d, m_nSampleRate: %d, m_nBits: %d", 
					(int)m_nChannels, (int)m_nSampleRate, (int)m_nBits);

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

	case OMX_VO_IndexAudioParamDTS:
		{
			OMX_VO_AUDIO_PARAM_DTSTYPE * pDTSFormat = (OMX_VO_AUDIO_PARAM_DTSTYPE *) pComponentParameterStructure;
			errType = CheckParam(pComp, pDTSFormat->nPortIndex, pDTSFormat, sizeof(OMX_VO_AUDIO_PARAM_DTSTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			m_pcmType.nChannels = pDTSFormat->nChannels;
			m_pcmType.nSamplingRate = pDTSFormat->nSampleRate;
			m_nChannels = m_pcmType.nChannels;
			m_nSampleRate = m_pcmType.nSamplingRate;
			m_nBits    = m_pcmType.nBitPerSample;

			VOLOGI ("m_nChannels %d, m_nSampleRate: %d, m_nBits: %d", (int)m_nChannels, (int)m_nSampleRate, (int)m_nBits);

			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamStandardComponentRole:
		{
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

OMX_ERRORTYPE voCOMXAudioPassthr::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. New State %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		int nBits;

		if (m_nSampleRate < 8000)
			m_nSampleRate = 44100;
		if (m_nChannels == 0)
			m_nChannels = 2;

		nBits = m_nBits;		
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

// 		int nAudioTime = (m_nOutputSize * 1000) / (m_nSampleRate * m_nChannels * m_nBits / 8);
// 		VOLOGI ("Audio Dec output time %d", nAudioTime);
	}

	m_bNofityError = VO_FALSE;
	m_inBuffer.Time = 0;

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioPassthr::Flush (OMX_U32	nPort)
{
	voCOMXAutoLock lokc (&m_mutCodec);

	OMX_ERRORTYPE errType = voCOMXCompFilter::Flush (nPort);

	m_inBuffer.Length = 0;
	m_inBuffer.Time = 0;
	m_nOutBuffTime = 0;
	m_nOutBuffSize = 0;
	m_bNofityError = VO_FALSE;

	return errType;
}

OMX_ERRORTYPE voCOMXAudioPassthr::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("\n Name: %s. Size: %d, Time %d", m_pName, (int)pInput->nFilledLen, (int)pInput->nTimeStamp);

	m_nInputNum++;   
	VOLOGR("@#@#@#@# m_nInputNum == %d", m_nInputNum);

	if (!m_bSetThreadPriority)
	{
		voOMXThreadSetPriority (m_hBuffThread, VOOMX_THREAD_PRIORITY_ABOVE_NORMAL);
		m_bSetThreadPriority = OMX_TRUE;
	}
	
	bool bBigEndian;
	int nFrameSize, nBsmod;
	nFrameSize = 0;
	nBsmod = 0;

	OMX_U32 nRC = 0;
	if (m_inBuffer.Length == 0)
	{
// 		m_inBuffer.Buffer = pInput->pBuffer + pInput->nOffset;
		m_inBuffer.Length = pInput->nFilledLen - pInput->nOffset;

		if (m_nRoute==1)
		{
			VOLOGR("@#@#@ goto DEFAULT_PROC!!");
			goto DEFAULT_PROC;
		}

		if (m_bPass==OMX_TRUE)
		{
			m_inBuffer.Buffer = pInput->pBuffer;
			m_outBuffer.Buffer = pOutput->pBuffer;
			m_outBuffer.Length = m_inBuffer.Length; 

			voOMXMemCopy(m_outBuffer.Buffer, m_inBuffer.Buffer, m_inBuffer.Length);

			VOLOGR("@#@#@# m_outBuffer.Length = m_inBuffer.Length = %d   Buffer Passthrough!!", m_outBuffer.Length);
			goto BUFFER_FILLED;
		}

		if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingDTS)
		{
			m_nReadOffset = 0;
			m_inBuffer.Buffer = pInput->pBuffer;
			m_outBuffer.Buffer = pOutput->pBuffer + (OMX_U32)m_nOutBuffSize;
			if(GetSampleLenDTS(m_inBuffer.Buffer, nFrameSize))
			{
// 				m_inBuffer.Buffer = pInput->pBuffer + m_nReadOffset;
// 				m_inBuffer.Length = pInput->nFilledLen - m_nReadOffset;

				if ((nFrameSize==2048)||(nFrameSize==4096))
				{
					m_bPass = OMX_TRUE;

					OMX_U16 * pData = (OMX_U16 *) m_inBuffer.Buffer;
					if (IsBigEndian(&m_inBuffer)) {
						for (int i = 0; i < (int)(m_inBuffer.Length / sizeof(OMX_U16)); i++) {
							pData[i] = (pData[i] << 8) | (pData[i] >> 8);
						}
					}

					m_outBuffer.Length = m_inBuffer.Length; 

					voOMXMemCopy(m_outBuffer.Buffer, m_inBuffer.Buffer, m_inBuffer.Length);

					VOLOGR("@#@#@# m_outBuffer.Length = m_inBuffer.Length = %d   Buffer without process!!", m_outBuffer.Length);
					goto BUFFER_FILLED;
				}
				else if (nFrameSize < (int)(pInput->nFilledLen - m_nReadOffset))
				{
					m_nRoute = 1;
					VOLOGR("@#@#@ set m_nRoute -> 1   ====>   goto DEFAULT_PROC!!");
					goto DEFAULT_PROC;
				}
				else
				{
					nRC = VO_ERR_NONE;
					goto DTS_PROC;
				}
			}			
		}

DEFAULT_PROC:
		//if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
		{			
			int nSampleRate, nChannels, nBits;
			nSampleRate = 0;
			nChannels = 0;
			nBits = 0;
			if(ParseHeader(pInput->pBuffer + pInput->nOffset, nFrameSize,
					  nSampleRate, nChannels, nBits, nBsmod, bBigEndian))
			{
				if (nSampleRate&&nChannels&&nBits)
				{
					m_nSampleRate = nSampleRate;
					m_nChannels = nChannels;
					m_nBits = nBits;
				}
// 				else if (nFrameSize == pInput->nFilledLen)
// 				{
// 					nRC = VO_ERR_INPUT_BUFFER_SMALL;
// 				}
			}
			else
				VOLOGR("@#@#@ ParseHeader() return false!");
		}

		VOLOGR("@#@#@# nFrameSize == %d  m_isAssbBuffer == %d", nFrameSize, m_isAssbBuffer);

		if (!nFrameSize && !m_isAssbBuffer)
		{
			pInput->nOffset = m_nReadOffset;
			nRC = VO_ERR_INPUT_BUFFER_SAMPLE_SKIP;

			VOLOGR("@#@#@ INPUT_BUFFER_SAMPLE_SKIP!!");
		}
		else if (!m_isAssbBuffer)
		{
			m_nFrameSize = nFrameSize;
		}		

		if (nRC != VO_ERR_INPUT_BUFFER_SAMPLE_SKIP)
		{
			nRC = GetSingleSample(pInput, &m_inBuffer, m_isAssbBuffer);
		}		

		if (nRC == VO_ERR_INPUT_BUFFER_SMALL)
		{
			OMX_U32 tpBuffLen = pInput->nFilledLen - pInput->nOffset;
// 			if (m_assbBuffer.Buffer != NULL)
// 			{
// 				voOMXMemFree(m_assbBuffer.Buffer);
// 				m_assbBuffer.Buffer = NULL;
// 				m_assbBuffer.Length = 0;
// 			}
			m_assbBuffer.Length = tpBuffLen;
			m_assbBuffer.Buffer = (OMX_U8*)voOMXMemAlloc(tpBuffLen);
			voOMXMemCopy(m_assbBuffer.Buffer, pInput->pBuffer + pInput->nOffset, tpBuffLen);				
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

		VOLOGR ("Sys Time: %d  frame time is %d, Diff %d, size %d", (int)(voOMXOS_GetSysTime () - m_nLogSysStartTime), (int)pInput->nTimeStamp, (int)((voOMXOS_GetSysTime () - m_nLogSysStartTime) - pInput->nTimeStamp), (int)pInput->nFilledLen);

		m_nLogSysCurrentTime = voOMXOS_GetSysTime ();
	}


	//VO_AUDIO_FORMAT		fmtAudio;
	m_outBuffer.Buffer = pOutput->pBuffer + (OMX_U32)m_nOutBuffSize;
	//m_outBuffer.Length = pOutput->nAllocLen -  (OMX_U32)m_nOutBuffSize;
// 	if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
// 	{
// 		m_outBuffer.Length = PassthrAC3Len; //(VO_U32)1536 * 2 * m_nChannels;
// 	}
// 	else
	{
		m_outBuffer.Length = PassthrAC3Len;
	}

DTS_PROC:
	if ((nRC != VO_ERR_INPUT_BUFFER_SMALL)&&(nRC != VO_ERR_INPUT_BUFFER_SAMPLE_SKIP))
	{
		PassThroughDataProc(&m_inBuffer, &m_outBuffer);
	}

	if(m_nDumpRuntimeLog)
	{
		VOLOGR ("nRC 0x%08X, Size %d, Time %d, Duration %d", (int)nRC, (int)m_outBuffer.Length, (int)pInput->nTimeStamp, (int)((m_outBuffer.Length * 8000) / (m_nSampleRate * m_nBits * m_nChannels)));
	}

	if (m_nLogLevel > 0)
	{
		VOLOGR ("GetOutputData used time %d., Size: %d, Result 0X%08X", (int)(voOMXOS_GetSysTime () - m_nLogSysCurrentTime), (int)m_outBuffer.Length, (int)nRC);
	}

#if 0
	if (nRC == VO_ERR_NONE &&
		((int)fmtAudio.Channels != (int)m_pcmType.nChannels || (int)fmtAudio.SampleRate != (int)m_pcmType.nSamplingRate || (int)fmtAudio.SampleBits != (int)m_pcmType.nBitPerSample))
	{
		VOLOGR ("Audio Format was changed. S %d, C %d", (int)m_pcmType.nSamplingRate, (int)m_pcmType.nChannels);

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
#endif

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
		m_nReadOffset = 0;
	}
	else if (nRC == VO_ERR_INPUT_BUFFER_SMALL)
	{
		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;
		m_nReadOffset = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
	else if (nRC == VO_ERR_INPUT_BUFFER_SAMPLE_SKIP)
	{
		*pEmptied = OMX_FALSE;
		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}
	else if (nRC == VO_ERR_INPUT_BUFFER_REMAIN && m_outBuffer.Length > 0)
	{
		VOLOGR("@#@#@# INPUT_BUFFER_REMAIN !!!");

		pOutput->nFilledLen = m_outBuffer.Length;
		m_nOutBuffSize = m_nOutBuffSize + m_outBuffer.Length;

		//AC3 decoder latency, unit is us, not ms
		pOutput->nTickCount = (OMX_U32)(PassthrDecSampleEqual_AC3 * 1000000 / m_pcmType.nSamplingRate);		

		/*if (m_nOutBuffSize >= m_nStepSize)*/
		//		if (m_nOutBuffSize >= 0)
		{
			pOutput->nFilledLen  = (OMX_U32)m_nOutBuffSize;

			/*m_nOutBuffTime calculate value is not correct*/
			//pOutput->nTimeStamp = m_nOutBuffTime ;
			
			//m_nOutBuffTime = m_nOutBuffTime + (m_nOutBuffSize * 1000LL) / (m_nSampleRate * m_nChannels * m_nBits / 8);
			m_nOutBuffSize = 0;

			VOLOGR("@#@#@ pOutput->nTickCount = %ld   pOutput->nTimeStamp = %lld   pOutput->nFilledLen = %lu", 
				pOutput->nTickCount, pOutput->nTimeStamp, pOutput->nFilledLen);

			//	VOLOGR ("Audio Time: %d", (int)pOutput->nTimeStamp);

			*pEmptied = OMX_FALSE;
			m_inBuffer.Length = 0;

#if 0
			*pFilled = OMX_TRUE;

			m_nOutputSize += pOutput->nFilledLen;
#else
			m_nFillSampleNum++;
			if (m_nFillSampleNum==HOLD_SAMPLE_NUM)
			{
				*pFilled = OMX_TRUE;
				//copy
				voOMXMemCopy(m_pAccumBuffer+m_nAccumLength, pOutput->pBuffer, pOutput->nFilledLen);
				m_nAccumLength += pOutput->nFilledLen;

				voOMXMemCopy(pOutput->pBuffer, m_pAccumBuffer, m_nAccumLength);
				pOutput->nFilledLen = m_nAccumLength;

				VOLOGR("@#@#@## pFilled = OMX_TRUE     Hold sample: %lu", m_nFillSampleNum);
				m_nAccumLength = 0;
				m_nFillSampleNum = 0;

				m_nOutputSize += pOutput->nFilledLen;
			}
			else
			{
				*pFilled = OMX_FALSE;
				//copy
				voOMXMemCopy(m_pAccumBuffer+m_nAccumLength, pOutput->pBuffer, pOutput->nFilledLen);
				m_nAccumLength += pOutput->nFilledLen;
			}
#endif
		}
	}
	else if (nRC == VO_ERR_NONE && m_outBuffer.Length > 0) 
	{
BUFFER_FILLED:

		pOutput->nFilledLen = m_outBuffer.Length;
		m_nOutBuffSize = m_nOutBuffSize + m_outBuffer.Length;

		//AC3 decoder latency, unit is us, not ms
		pOutput->nTickCount = (OMX_U32)(PassthrDecSampleEqual_AC3 * 1000000 / m_pcmType.nSamplingRate);

		/*if (m_nOutBuffSize >= m_nStepSize)*/
			//		if (m_nOutBuffSize >= 0)
		{
			pOutput->nFilledLen  = (OMX_U32)m_nOutBuffSize;

			//shen.feng modify resolve #17966 just let the out timestamp equal input
			//pOutput->nTimeStamp = m_nOutBuffTime ; /*m_nOutBuffTime calculate value is not correct*/
			
			//m_nOutBuffTime = m_nOutBuffTime + (m_nOutBuffSize * 1000LL) / (m_nSampleRate * m_nChannels * m_nBits / 8);
			m_nOutBuffSize = 0;

			VOLOGR("@#@#@ pOutput->nTickCount = %ld   pOutput->nTimeStamp = %ld   pOutput->nFilledLen = %ld", 
					pOutput->nTickCount, pOutput->nTimeStamp, pOutput->nFilledLen);

			//	VOLOGR ("Audio Time: %d", (int)pOutput->nTimeStamp);

			*pEmptied = OMX_TRUE;
			m_inBuffer.Length = 0;
			m_nReadOffset = 0;

#if 0
			*pFilled = OMX_TRUE;

			m_nOutputSize += pOutput->nFilledLen;
#else
			m_nFillSampleNum++;
			if (m_nFillSampleNum == HOLD_SAMPLE_NUM)
			{
				*pFilled = OMX_TRUE;
				//copy
				voOMXMemCopy(m_pAccumBuffer+m_nAccumLength, pOutput->pBuffer, pOutput->nFilledLen);
				m_nAccumLength += pOutput->nFilledLen;

				voOMXMemCopy(pOutput->pBuffer, m_pAccumBuffer, m_nAccumLength);
				pOutput->nFilledLen = m_nAccumLength;

				VOLOGR("@#@#@## pFilled = OMX_TRUE     Hold sample: %lu", m_nFillSampleNum);
				m_nAccumLength = 0;
				m_nFillSampleNum = 0;

				m_nOutputSize += pOutput->nFilledLen;
			}
			else
			{
				*pFilled = OMX_FALSE;
				//copy
				voOMXMemCopy(m_pAccumBuffer+m_nAccumLength, pOutput->pBuffer, pOutput->nFilledLen);
				m_nAccumLength += pOutput->nFilledLen;
			}
#endif
		}
// 		else
// 		{
// 			*pFilled = OMX_FALSE;
// 		}
	}	
	else
	{
//		*pEmptied = OMX_TRUE;
//		m_inBuffer.Length = 0;

		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;

		if(VO_ERR_NONE != nRC)
		{
			VOLOGW ("Passthrough Audio error 0X%08X.", (int)nRC);
		}
	}

	return OMX_ErrorNone;
}

VO_U32 voCOMXAudioPassthr::GetSingleSample(OMX_BUFFERHEADERTYPE * pObj, VO_CODECBUFFER * pDst, OMX_BOOL assemble)
{
	VOLOGR("@#@#@# GetSingleSample();");

	OMX_BUFFERHEADERTYPE testBuff;
	//voOMXMemSet(testBuff.pBuffer);
	testBuff.pBuffer = pObj->pBuffer + pObj->nOffset;
	testBuff.nFilledLen = pObj->nFilledLen;
	testBuff.nOffset = pObj->nOffset;

	OMX_U8 hdr[2];
	voOMXMemCopy(&(hdr[0]), testBuff.pBuffer, sizeof(hdr));

	VO_U32 PassthrSampleLen;
	if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
		PassthrSampleLen = PassthrAC3Len;
	else 
		PassthrSampleLen = PassthrDTSLen;

	bool SyncwordIsFront;
	if((hdr[0] == 0x0b) && (hdr[1] == 0x77) || (hdr[0] == 0x77) && (hdr[1] == 0x0b))
		SyncwordIsFront = true;
	else if ((hdr[0] == 0x7F) && (hdr[1] == 0xFE) || (hdr[0] == 0xFE) && (hdr[1] == 0x7F))
		SyncwordIsFront = true;
	else
		SyncwordIsFront = false;

	if (assemble == OMX_FALSE)
	{
		if(SyncwordIsFront)
		{
			VOLOGR("@#@#@# assemble == OMX_FALSE && SyncwordIsFront == true");

			VO_U32 tResultLen = FindSyncword(&testBuff);
			if (tResultLen == 0)	
			{				
				m_isAssbBuffer = OMX_TRUE;
				VOLOGR("@#@#@#@ INPUT_BUFFER_SMALL!!  m_isAssbBuffer = OMX_TRUE");

				return VO_ERR_INPUT_BUFFER_SMALL;
			}
			else if(tResultLen == Complete_Sample)
			{
				pDst->Length = pObj->nFilledLen;
				pDst->Buffer = pObj->pBuffer + pObj->nOffset;

				VOLOGR("@#@#@## Complete_Sample!!");
				return VO_ERR_NONE;
			}
			else if (tResultLen > PassthrSampleLen)
			{
				pObj->nOffset += tResultLen;				
				VOLOGR("@#@#@ tResultLen > PassthrSampleLen ==> INPUT_BUFFER_SAMPLE_SKIP !!");

				return VO_ERR_INPUT_BUFFER_SAMPLE_SKIP;
			}
			else			 
			{
				pDst->Length = tResultLen;
				pDst->Buffer = pObj->pBuffer + pObj->nOffset;

				VOLOGR("@#@#@ pDst->Length = tResultLen = %d", pDst->Length);
			}
		} 
		else
		{
			VOLOGR("@#@#@# assemble == OMX_FALSE && SyncwordIsFront == false");

			pObj->nOffset += FindSyncword(&testBuff);
			testBuff.pBuffer = pObj->pBuffer + pObj->nOffset;
			pDst->Length = FindSyncword(&testBuff);
			pDst->Buffer = pObj->pBuffer + pObj->nOffset;
		}

		pObj->nOffset += pDst->Length;	

	} //assemble == OMX_FALSE -- end
	else
	{
// 		if(SyncwordIsFront)
// 		{
// 			
// 		}
// 		else
		{
			VOLOGR("@#@#@# assemble == OMX_TRUE");

			OMX_U8* tpBuffer = (OMX_U8*)voOMXMemAlloc(pObj->nFilledLen);
			voOMXMemCopy(tpBuffer, pObj->pBuffer, pObj->nFilledLen);
			voOMXMemCopy(pObj->pBuffer, m_assbBuffer.Buffer, m_assbBuffer.Length);
			voOMXMemCopy(pObj->pBuffer + m_assbBuffer.Length, tpBuffer, pObj->nFilledLen);
			pObj->nFilledLen += m_assbBuffer.Length;

			VO_U32 tResultLen = FindSyncword(&testBuff);
			pDst->Length = tResultLen;
			pDst->Buffer = pObj->pBuffer + pObj->nOffset;

			pObj->nOffset += pDst->Length;

			voOMXMemFree(tpBuffer);
			voOMXMemFree(m_assbBuffer.Buffer);		
			m_assbBuffer.Buffer = NULL;
			m_assbBuffer.Length = 0;
			m_isAssbBuffer = OMX_FALSE;
		}		
	}

	return VO_ERR_INPUT_BUFFER_REMAIN;	
}

VO_U32 voCOMXAudioPassthr::FindSyncword(OMX_BUFFERHEADERTYPE * pInput)
{
	VO_U32 sampleLen = 0;

	OMX_U8* hdr[4];

	//voOMXMemCopy(&(hdr[0]), pInput->pBuffer, sizeof(hdr));

	int maxLen = (int)(pInput->nFilledLen - pInput->nOffset);
	VOLOGR("@#@##@@ maxLen = pInput->nFilledLen - pInput->nOffset = %d", maxLen);

	if ((m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
		||(m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3))
	{
		/*for audio only parser will send an excess header(length < 25) in first time, so add
		  "(maxLen < 25)&&(m_nInputNum == 1)" condition to treat as Complete_Sample*/
		if ((maxLen == m_nFrameSize)||((maxLen < 25)&&(m_nInputNum == 1)))  
		{
			sampleLen = Complete_Sample;
		}
		else if (maxLen > (int)m_nFrameSize)
		{
			sampleLen = m_nFrameSize;

			if (sampleLen==0)
			{
				for (int i=4; i<maxLen; i=i+1)
				{
					hdr[0] = pInput->pBuffer + i;
					hdr[1] = pInput->pBuffer + i + 1;
					if((*hdr[0] == 0x0b) && (*hdr[1] == 0x77) || (*hdr[1] == 0x0b) && (*hdr[0] == 0x77))
					{
						sampleLen = i;
						break;
					}	
				}
			}			
			
			VOLOGR("@#@#@## sampleLen = m_nFrameSize = %d", sampleLen);

			if (sampleLen == 0)
			{
				hdr[0] = pInput->pBuffer + maxLen - 4;
				hdr[1] = pInput->pBuffer + maxLen - 3;
				if((*hdr[0] == 0x00) && (*hdr[1] == 0x00))
				{
					sampleLen = Complete_Sample;
					VOLOGR("@#@#@## hdr[0] == 0x00  ==> sampleLen = Complete_Sample");
				}
			}
		}
	}
	else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingDTS)
	{
		for (int i=4; i<maxLen; i++)
		{
			hdr[0] = pInput->pBuffer + i;
			hdr[1] = pInput->pBuffer + i + 1;
			hdr[2] = pInput->pBuffer + i + 2;
			hdr[3] = pInput->pBuffer + i + 3;
			if((*hdr[0] == 0x7F)&&(*hdr[1] == 0xFE)&&(*hdr[2] == 0x80)&&(*hdr[3] == 0x01)
				|| (*hdr[1] == 0x7F) && (*hdr[0] == 0xFE) && (*hdr[3] == 0x80) && (*hdr[2] == 0x01))
			{
				sampleLen = i;
				break;
			}	
		}	

		if (sampleLen == 0)
		{
			hdr[0] = pInput->pBuffer + maxLen - 1;
			hdr[1] = pInput->pBuffer + maxLen;
			if((*hdr[0] == 0x00) && (*hdr[1] == 0x00) || (maxLen < 25))
				sampleLen = Complete_Sample;
		}
	}
	else
	{
		VOLOGE("Input Buffer is neither AC3 nor DTS !!");
	}	

	return sampleLen; 
}

VO_U32 voCOMXAudioPassthr::PassThroughDataProc(VO_CODECBUFFER * pInput, VO_CODECBUFFER * pOutput)
{	
	VOLOGR("@#@#@#@ PassThroughDataProc();");

	OMX_U32 nRC = VO_ERR_NONE;

	m_Header[0] = 0xF872;
	m_Header[1] = 0x4E1F;

	if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3
		|| m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3) 
	{
		m_Header[2] = 0x1;  // | (mBsmod << 8); // AC3 format indicator | bsmod

		m_Header[3] = (OMX_U16)(pInput->Length * 8); //size in bit

		//VOLOGR("pInput->nFilledLen = %d, m_Header[3] = %d", pInput->Length, m_Header[3]);

		voOMXMemSet(pOutput->Buffer, 0, pOutput->Length);
		voOMXMemCopy(pOutput->Buffer, &(m_Header[0]), sizeof(m_Header));


		VOLOGR("@#@#@@#@#@@# pInput->Length = %d",pInput->Length);


		OMX_U16 * pData = (OMX_U16 *) pInput->Buffer;
		if (IsBigEndian(pInput)) {
			for (int i = 0; i < (int)(pInput->Length / sizeof(OMX_U16)); i++) {
				pData[i] = (pData[i] << 8) | (pData[i] >> 8);
			}
		}

		voOMXMemCopy(pOutput->Buffer + sizeof(m_Header), pInput->Buffer, pInput->Length);

		//pOutput->Length = 1536 * 2 * nChannels;  //set 6144
	} 
	else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingDTS) 
	{		
    	voOMXMemSet(pOutput->Buffer, 0, PassthrDTSLen);

		OMX_U16 * pData = (OMX_U16 *) pInput->Buffer;
		if (IsBigEndian(pInput)) {
			for (int i = 0; i < (int)(pInput->Length / sizeof(OMX_U16)); i++) {
				pData[i] = (pData[i] << 8) | (pData[i] >> 8);
			}
		}

		pOutput->Length = PassthrDTSLen; 

		voOMXMemCopy(pOutput->Buffer, pInput->Buffer, pInput->Length);

		VOLOGR("pInput->Length = %d, pOutput->Length = %d", pInput->Length, pOutput->Length);
	} 
	else 
	{
		VOLOGE("AC3 and DTS are supported only.");

		nRC = VO_ERR_NOT_IMPLEMENT;
	}

	return nRC;
}

bool voCOMXAudioPassthr::IsBigEndian(VO_CODECBUFFER * pInput)
{
	OMX_U8 hdr[2];

	voOMXMemCopy(&(hdr[0]), pInput->Buffer, sizeof(hdr));

	if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3
		|| m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3)
	{
		if((hdr[0] == 0x0b) && (hdr[1] == 0x77))
		{
			VOLOGR("Input Buffer is BigEndian!");
			return true;
		}
		else
		{
			VOLOGR("Input Buffer is LittleEndian!");
			return false;
		}
	}
	else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingDTS)
	{
		if((hdr[0] == 0x7F) && (hdr[1] == 0xFE) || (hdr[0] == 0xEF) && (hdr[1] == 0xFF))
		{
			VOLOGR("Input Buffer is BigEndian!");
			return true;
		}
		else
		{
			VOLOGR("Input Buffer is LittleEndian!");
			return false;
		}
	}
	else
	{
		VOLOGR("Input Buffer is neither AC3 nor DTS !!");
		return false;
	}
	
}

OMX_ERRORTYPE voCOMXAudioPassthr::InitPortType (void)
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

VO_U32 voCOMXAudioPassthr::ParseHeader(unsigned char *hdr,int &frameSize, int &sampleRate, 
									   int &numChannels, int &bitRate, int &bsmod, bool &isBigEndian)
{
	VOLOGR("@#@#@# ParseHeader();");

	if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingAC3)
	{
		int fscod;
		int frmsizecod;
		int acmod;
		int halfrate;

		/////////////////////////////////////////////////////////
		// 8 bit or 16 bit big endian stream sync
		if ((hdr[0] == 0x0b) && (hdr[1] == 0x77)) 
		{
			// constraints
			if (hdr[5] >= 0x60)
			{
				//m_nCoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3;
				VOLOGR("@#@#@# hdr[5] >= 0x60 ===> goto eac3_proc");
				goto eac3_proc;
			}  // 'bsid'
			if ((hdr[4] & 0x3f) > 0x25) return false;   // 'frmesizecod'
			if ((hdr[4] & 0xc0) > 0x80) return false;   // 'fscod'

			fscod      = hdr[4] >> 6;
			frmsizecod = hdr[4] & 0x3f;
			bsmod      = hdr[5] >> 5;
			acmod      = hdr[6] >> 5;

			numChannels = acmod2channels_tbl[acmod];
			if (hdr[6] & lfe_mask[acmod])
				(numChannels)++;

			halfrate   = halfrate_tbl[hdr[5] >> 3];
			bitRate    = bitrate_tbl[frmsizecod >> 1];
			isBigEndian = true;
		}
		/////////////////////////////////////////////////////////
		// 16 bit low endian stream sync
		else if ((hdr[1] == 0x0b) && (hdr[0] == 0x77)) 
		{
			// constraints
			if (hdr[4] >= 0x60)
			{
				//m_nCoding = (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3;
				VOLOGR("@#@#@# hdr[5] >= 0x60 ===> goto eac3_proc")
				goto eac3_proc;
			}	// 'bsid'
			if ((hdr[5] & 0x3f) > 0x25) return false;   // 'frmesizecod'
			if ((hdr[5] & 0xc0) > 0x80) return false;   // 'fscod'

			fscod      = hdr[5] >> 6;
			frmsizecod = hdr[5] & 0x3f;
			bsmod      = hdr[4] >> 5;
			acmod      = hdr[7] >> 5;

			numChannels = acmod2channels_tbl[acmod];
			if (hdr[7] & lfe_mask[acmod])
				(numChannels)++;

			halfrate   = halfrate_tbl[hdr[4] >> 3];
			bitRate    = bitrate_tbl[frmsizecod >> 1];
			isBigEndian = false;
		}
		else
		{
			int maxLen = (int)(m_inBuffer.Length);
			for (int i=4; i<maxLen; i=i+1)
			{
				if((hdr[i] == 0x0b) && (hdr[i+1] == 0x77) || (hdr[i+1] == 0x0b) && (hdr[i] == 0x77))
				{
					m_nReadOffset = i;
					break;
				}	
			}
			return false;
		}

		switch (fscod) 
		{
		case 0:    
			frameSize = 4 * bitRate;
			sampleRate = 48000 >> halfrate;
			break;

		case 1: 
			frameSize = 2 * (320 * bitRate / 147 + (frmsizecod & 1));
			sampleRate = 44100 >> halfrate;
			break;

		case 2: 
			frameSize = 6 * bitRate;
			sampleRate = 32000 >> halfrate;

		default:
			return false;
		}

		VOLOGR("@#@ frameSize=%d, sampleRate=%d, numChannels=%d, bitRate=%d, bsmod=%d",
			frameSize, sampleRate, numChannels, bitRate, bsmod);		
	}
	else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingDTS)
	{
		int maxLen = (int)(m_inBuffer.Length);

		if((hdr[0] == 0x7F)&&(hdr[1] == 0xFE)&&(hdr[2] == 0x80)&&(hdr[3] == 0x01)
			|| (hdr[1] == 0x7F) && (hdr[0] == 0xFE) && (hdr[3] == 0x80) && (hdr[2] == 0x01))
		{
			for (int i=4; i<maxLen; i++)
			{
				if((hdr[i] == 0x7F)&&(hdr[i+1] == 0xFE)&&(hdr[i+2] == 0x80)&&(hdr[i+3] == 0x01)
					|| (hdr[i+1] == 0x7F) && (hdr[i] == 0xFE) && (hdr[i+3] == 0x80) && (hdr[i+2] == 0x01))
				{
					frameSize = i;
					break;
				}	
			}

			if (frameSize == 0)
			{
				frameSize = maxLen;
			}
		}
		else if((hdr[0] == 0x1F)&&(hdr[1] == 0xFF)&&(hdr[2] == 0xE8)&&(hdr[3] == 0x00)
				|| (hdr[1] == 0x1F) && (hdr[0] == 0xFF) && (hdr[3] == 0xE8) && (hdr[2] == 0x00))
		{
			for (int i=4; i<maxLen; i++)
			{
				if((hdr[0] == 0x1F)&&(hdr[1] == 0xFF)&&(hdr[2] == 0xE8)&&(hdr[3] == 0x00)
					|| (hdr[1] == 0x1F) && (hdr[0] == 0xFF) && (hdr[3] == 0xE8) && (hdr[2] == 0x00))
				{
					frameSize = i;
					break;
				}	
			}

			if (frameSize == 0)
			{
				frameSize = maxLen;
			}
		}
		else
		{
			for (int i=4; i<maxLen; i=i+1)
			{
				if((hdr[i] == 0x7F)&&(hdr[i+1] == 0xFE)&&(hdr[i+2] == 0x80)&&(hdr[i+3] == 0x01)
					|| (hdr[i+1] == 0x7F) && (hdr[i] == 0xFE) && (hdr[i+3] == 0x80) && (hdr[i+2] == 0x01))
				{
					m_nReadOffset = i;
					break;
				}	
			}
			return false;
		}		
	}
	else if (m_nCoding == (OMX_AUDIO_CODINGTYPE) OMX_VO_AUDIO_CodingEAC3)
	{
eac3_proc:
		VOLOGR("@#@#@# ParserHeader: EAC3_proc!!");

		int maxLen = (int)(m_inBuffer.Length);

		if((hdr[0] == 0x0b)&&(hdr[1] == 0x77))
		{
			for (int i=4; i<maxLen; i++)
			{
				if((hdr[i] == 0x0b)&&(hdr[i+1] == 0x77) || (hdr[i+1] == 0x0b) && (hdr[i] == 0x77))
				{
					frameSize = i;
					break;
				}	
			}

			if (frameSize == 0)
			{
				frameSize = maxLen;
			}
		}
		else if ((hdr[1] == 0x0b) && (hdr[0] == 0x77))
		{
			for (int i=4; i<maxLen; i++)
			{
				if((hdr[i+1] == 0x0b) && (hdr[i] == 0x77))
				{
					frameSize = i;
					break;
				}	
			}

			if (frameSize == 0)
			{
				frameSize = maxLen;
			}
		}
		else
		{
			int maxLen = (int)(m_inBuffer.Length);
			for (int i=4; i<maxLen; i=i+1)
			{
				if((hdr[i] == 0x0b) && (hdr[i+1] == 0x77) || (hdr[i+1] == 0x0b) && (hdr[i] == 0x77))
				{
					m_nReadOffset = i;
					break;
				}	
			}
			return false;
		}
	}
	else
	{
		VOLOGE("Input Buffer is neither AC3 nor DTS !!");
		return false;
	}	

	return true;
}


VO_U32 voCOMXAudioPassthr::GetSampleLenDTS(unsigned char *hdr, int &frameSize)
{
	VOLOGR("@#@## GetSampleLenDTS()")

	int readoffset = 0;

	int maxLen = (int)(m_inBuffer.Length);

	if((hdr[0] == 0x7F)&&(hdr[1] == 0xFE)&&(hdr[2] == 0x80)&&(hdr[3] == 0x01)
		|| (hdr[1] == 0x7F) && (hdr[0] == 0xFE) && (hdr[3] == 0x80) && (hdr[2] == 0x01))
	{
		for (int i=4; i<maxLen; i++)
		{
			if((hdr[i] == 0x7F)&&(hdr[i+1] == 0xFE)&&(hdr[i+2] == 0x80)&&(hdr[i+3] == 0x01)
				|| (hdr[i+1] == 0x7F) && (hdr[i] == 0xFE) && (hdr[i+3] == 0x80) && (hdr[i+2] == 0x01))
			{
				frameSize = i;
				break;
			}	
		}

		if (frameSize == 0)
		{
			frameSize = maxLen;
		}
	}
	else if((hdr[0] == 0x1F)&&(hdr[1] == 0xFF)&&(hdr[2] == 0xE8)&&(hdr[3] == 0x00)
		|| (hdr[1] == 0x1F) && (hdr[0] == 0xFF) && (hdr[3] == 0xE8) && (hdr[2] == 0x00))
	{
		for (int i=4; i<maxLen; i++)
		{
			if((hdr[i] == 0x1F)&&(hdr[i+1] == 0xFF)&&(hdr[i+2] == 0xE8)&&(hdr[i+3] == 0x00)
				|| (hdr[i+1] == 0x1F) && (hdr[i] == 0xFF) && (hdr[i+3] == 0xE8) && (hdr[i+2] == 0x00))
			{
				frameSize = i;
				break;
			}	
		}

		if (frameSize == 0)
		{
			frameSize = maxLen;
		}
	}
	else
	{
		for (int i=4; i<maxLen; i=i+1)
		{
			if((hdr[i] == 0x7F)&&(hdr[i+1] == 0xFE)&&(hdr[i+2] == 0x80)&&(hdr[i+3] == 0x01)
				|| (hdr[i+1] == 0x7F) && (hdr[i] == 0xFE) && (hdr[i+3] == 0x80) && (hdr[i+2] == 0x01)
				|| (hdr[i] == 0x1F)&&(hdr[i+1] == 0xFF)&&(hdr[i+2] == 0xE8)&&(hdr[i+3] == 0x00)
				|| (hdr[i+1] == 0x1F) && (hdr[i] == 0xFF) && (hdr[i+3] == 0xE8) && (hdr[i+2] == 0x00))
			{
				readoffset = i;
				break;
			}	
		}
		/*return false;*/
	}	

	if (readoffset > 0)
	{		
		for (int i=readoffset+4; i<maxLen; i++)
		{
			if((hdr[i] == 0x7F)&&(hdr[i+1] == 0xFE)&&(hdr[i+2] == 0x80)&&(hdr[i+3] == 0x01)
				|| (hdr[i+1] == 0x7F) && (hdr[i] == 0xFE) && (hdr[i+3] == 0x80) && (hdr[i+2] == 0x01)
				|| (hdr[i] == 0x1F)&&(hdr[i+1] == 0xFF)&&(hdr[i+2] == 0xE8)&&(hdr[i+3] == 0x00)
				|| (hdr[i+1] == 0x1F) && (hdr[i] == 0xFF) && (hdr[i+3] == 0xE8) && (hdr[i+2] == 0x00))
			{
				frameSize = i - readoffset;
				break;
			}	
		}

		m_nReadOffset = readoffset;
	}

	VOLOGR("@#@#@# maxLen: %d readoffset: %d frameSize: %d", maxLen, readoffset, frameSize);

	if (frameSize < 10)
		return false;
	else
	    return true;
}
