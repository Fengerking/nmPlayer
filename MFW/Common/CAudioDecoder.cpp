	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voString.h"

#include "voMMPlay.h"
#include "CAudioDecoder.h"

#include "voAAC.h"
#include "voAC3.h"
#include "voFLAC.h"
#include "voADPCM.h"
#include "voRealAudio.h"
#include "voWMA.h"
#include "voOSFunc.h"
#include "voAMediaCodec.h"

#define LOG_TAG "CAudioDecoder"
#include "voLog.h"

#ifdef _IOS
#include "voWMA.h"
#include "voRealAudio.h"
#include "voMP3.h"
#endif

#if defined(_LINUX_ANDROID)
#   include "voAMediaCodecCommon.h"
#endif
typedef VO_S32 (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

CAudioDecoder::CAudioDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hDec (NULL)
	, m_nCoding (nCoding)
	, m_nFourCC (0)
	, m_bFirstSample (VO_TRUE)
	, m_pInputData (0)
	, m_nInputUsed (0)
	, m_bSetHeadData (VO_FALSE)
	, m_bDisableAACP (VO_FALSE)
	, m_nSetAudioNameType(0)
	, m_nSetAudioAPIType(0)
	//, m_hDumpFile (NULL)
	,mIsDrmDataAppended(false)
	,mOutputType(VO_AUDIO_CodingPCM)
	, m_bSpptMltChanl(VO_TRUE)
{
	memset(&m_sSetAudioName, 0, sizeof(VO_TCHAR)*256);
	memset(&m_sSetAudioAPI, 0, sizeof(VO_TCHAR)*256);
}

CAudioDecoder::~CAudioDecoder ()
{
	Uninit ();

	//if (m_hDumpFile != NULL)
		//fclose (m_hDumpFile);
}

VO_U32 CAudioDecoder::Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_AUDIO_FORMAT * pFormat)
{
	if (m_nCoding > VO_AUDIO_CodingALAC || m_nCoding < VO_AUDIO_CodingPCM)
	{
		if (m_pError != NULL)
			vostrcpy (m_pError, _T("The audio format is unknown!"));

		VOLOGE ("The coding %d was unknown!", (int)m_nCoding);
		return VO_ERR_VOMMP_AudioDecoder;
	}

	m_OutputInfo.Format.SampleRate = pFormat->SampleRate;
	m_OutputInfo.Format.Channels = pFormat->Channels;
	m_OutputInfo.Format.SampleBits = pFormat->SampleBits;

	m_fmtAudio.SampleRate = pFormat->SampleRate;
	m_fmtAudio.Channels = pFormat->Channels;
	m_fmtAudio.SampleBits = pFormat->SampleBits;

	if (m_nCoding == VO_AUDIO_CodingPCM)
		return VO_ERR_NONE;

	VO_CODEC_INIT_USERDATA	initInfo;
	memset (&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = m_pMemOP;
	// Rogine add for passing work path to decoder
	initInfo.reserved1 = (VO_U32)m_sWorkPath;

	if (m_pLibOP != NULL)
	{
		initInfo.memflag |= 0X10;
		initInfo.libOperator = m_pLibOP;
	}

	// here we can force block license check for short play, East 20111129
//	initInfo.memflag |= 0x100;

	m_bSetHeadData = VO_FALSE;

	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;
	while (nRC != VO_ERR_NONE)
	{
		Uninit ();

		if (LoadLib (m_hInst) == 0)
		{
			VOLOGE ("LoadLib was failed!");
			return VO_ERR_VOMMP_AudioDecoder;
		}

		m_funDec.SetParam (NULL, VO_PID_COMMON_LOGFUNC, m_pcbVOLOG);
		
		nRC = m_funDec.Init (&m_hDec, (VO_AUDIO_CODINGTYPE)m_nCoding, &initInfo);
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE("m_funDec.Init was failed! Result 0X%08X", nRC);
			return nRC;
		}

		if (m_hDec == NULL)
		{
			VOLOGE ("m_funDec.Init was failed! Result 0X%08X", (unsigned int)nRC);
			return VO_ERR_VOMMP_AudioDecoder;
		}

		UpdateParam ();

		nRC = SetHeadData (pHeadData, nHeadSize);

		if (nRC == VO_ERR_NONE)
			break;

		if (m_bVOUsed)
			break;

		m_bVOUsed = VO_TRUE;
	}

	return nRC;
}

VO_U32 CAudioDecoder::UpdateParam (void)
{
	 VO_U32 nRC = 0;
//	if (m_pLibOP != NULL)
//		nRC = SetParam (VO_PID_COMMON_LIBOP, m_pLibOP);

	if (m_nCoding == VO_AUDIO_CodingAAC)
	{
		VOAACFRAMETYPE nFrameType = VOAAC_ADTS;
		if (m_nFourCC == 0xF6)
			nFrameType = VOAAC_ADIF;
		else if (m_nFourCC == 0xF7)
			nFrameType = VOAAC_ADTS;
		else if (m_nFourCC == 0xF8)
			nFrameType = VOAAC_LATM;
		else if (m_nFourCC == 0xF9)
			nFrameType = VOAAC_LOAS;
		else if (m_nFourCC == 0XFF || m_nFourCC == 0XA106)
			nFrameType = VOAAC_RAWDATA;

		SetParam (VO_PID_AAC_FRAMETYPE, &nFrameType);

		if (m_OutputInfo.Format.Channels <= 2)
		{
			VO_U32 nChannelSpec = VO_CHANNEL_FRONT_LEFT | VO_CHANNEL_FRONT_RIGHT;
			SetParam (VO_PID_AAC_SELECTCHS, &nChannelSpec);
			//VO_U32 nChannelSpec = VO_AUDIO_CHAN_MULDOWNMIX2;
			//SetParam (VO_PID_AAC_CHANNELSPEC, &nChannelSpec);
		}
		else
		{
			VOLOGI("VO_PID_WMA_SUPPTMTCHANL %d", m_OutputInfo.Format.Channels);
			 int tmp = VO_AUDIO_CHAN_MULDOWNMIX2;
			 SetParam(VO_PID_AAC_CHANNELSPEC, &tmp);
		}
		if (m_bDisableAACP)
		{
			int nDisable = 1;
			SetParam (VO_PID_AAC_DISABLEAACPLUSV1, &nDisable);
			SetParam (VO_PID_AAC_DISABLEAACPLUSV2, &nDisable);
		}
	}
	else if(m_nCoding == VO_AUDIO_CodingAC3)
	{
		if (m_OutputInfo.Format.Channels > 2) // !!we should check if the audio render support multichannel
		{
			int channel = 6;
			SetParam (VO_PID_AC3_NUMCHANS, &channel);
		}
	}
	else if(m_nCoding == VO_AUDIO_CodingWMA)
	{
		if (m_OutputInfo.Format.Channels > 2 /*&& m_bSpptMltChanl*/) // multichannel support has checked here
		{
			VOLOGI("VO_PID_WMA_SUPPTMTCHANL %d", m_OutputInfo.Format.Channels);
			VO_U32 nSupportMultiChannel = 1;
			nRC = m_funDec.SetParam(m_hDec, VO_PID_WMA_SUPPTMTCHANL, &nSupportMultiChannel);
		}
	}

	if (m_pConfig != NULL && !m_bVOUsed && !m_bOMXComp)
	{
		VO_PCHAR pCoreFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"Core");
		VO_PCHAR pCompName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Comp");

		if (pCoreFile != NULL)
			SetParam (VO_PID_COMMON_CoreFile, pCoreFile);
		if (pCoreFile != NULL)
			SetParam (VO_PID_COMMON_CompName, pCompName);
	}

	SetParam (VO_PID_AUDIO_FORMAT, &m_OutputInfo);
	
	SetParam (VO_ACODEC_DRM, &mIsDrmDataAppended);
	return 0;
}

VO_U32 CAudioDecoder::SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	if (m_hDec == NULL)
		return 0;

	VO_U32 nRC = 0;
	if (nHeadSize > 0)
	{
		VO_CODECBUFFER buffHead;
		buffHead.Buffer = pHeadData;
		buffHead.Length = nHeadSize;

		VOLOGS ("Set the head data to decoder module.");

		if(m_nCoding == VO_AUDIO_CodingRA)
			nRC = m_funDec.SetParam (m_hDec, VOID_PID_RA_FMT_INIT, &buffHead);
		else if (m_nCoding == VO_AUDIO_CodingFLAC)
			nRC = m_funDec.SetParam (m_hDec, VO_PID_FLAC_HEADER_PARAMETER, &buffHead);
		else
			nRC = m_funDec.SetParam (m_hDec, VO_PID_COMMON_HEADDATA, &buffHead);

		if (nRC == VO_ERR_NONE)
			 m_funDec.GetParam (m_hDec, VO_PID_AUDIO_FORMAT, &m_OutputInfo);

		if(m_nCoding == VO_AUDIO_CodingEAC3)
			nRC = VO_ERR_NONE;

		m_bSetHeadData = VO_TRUE;
	}
	else
	{
		if(m_nCoding != VO_AUDIO_CodingOGG)
			m_bSetHeadData = VO_TRUE;
	}

	return nRC;
}

VO_U32 CAudioDecoder::Uninit (void)
{
	if (m_hDec != NULL)
	{
		m_funDec.Uninit (m_hDec);
		m_hDec = NULL;
	}

	FreeLib ();

	return 0;
}

VO_U32 CAudioDecoder::Start(void)
{
	return 	SetParam (VO_PID_COMMON_START, 0);
}

VO_U32 CAudioDecoder::Pause(void)
{
	return 	SetParam (VO_PID_COMMON_PAUSE, 0);
}

VO_U32 CAudioDecoder::Stop(void)
{
	return 	SetParam (VO_PID_COMMON_STOP, 0);
}

VO_U32 CAudioDecoder::SetInputData (VO_CODECBUFFER * pInput)
{
	m_nInputUsed = 0;
	m_pInputData = pInput;

	if (m_nCoding == VO_AUDIO_CodingPCM)
		return VO_ERR_NONE;

	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	if (m_bFirstSample)
	{
		m_bFirstSample = VO_FALSE;

		if (HandleFirstSample (pInput) == VO_ERR_NONE)
			return VO_ERR_INPUT_BUFFER_SMALL;
	}

//	VOLOGI ("Set Input Data size %d Time is %d", pInput->Length, (int)pInput->Time);
	return 	m_funDec.SetInputData (m_hDec, pInput);
}

VO_U32 CAudioDecoder::GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat)
{
//	VOLOGI ("GetPutputData buffer Size %d.", pOutput->Length);

	if (m_nCoding == VO_AUDIO_CodingPCM)
	{
		if (m_pInputData == NULL || m_pInputData->Length == 0)
			return VO_ERR_INPUT_BUFFER_SMALL;

		DownMixBuffer (pOutput, pAudioFormat);

		return VO_ERR_NONE;
	}

	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	VOLOGS ("Get Output Data!");

	voCAutoLock lock (&m_Mutex);

#if 0
	static int time = 0;
	static int count = 0;
	
	int use = voOS_GetSysTime();
	VO_U32 nRC = m_funDec.GetOutputData (m_hDec, pOutput, &m_OutputInfo);
	time += voOS_GetSysTime() - use;
	
	count++;
	if (count >= 3000)
	{
		VOLOGI("Audio dec time = %d, total time = %d, count = %d\n", time/count, time, count);
		time = 0;
		count = 0;
	}
	
#else
	VO_U32 nRC = m_funDec.GetOutputData (m_hDec, pOutput, &m_OutputInfo);
#endif
	

	//VOLOGI ("GetPutputData Size %d, Result 0X%08X !", pOutput->Length, nRC);

	if (nRC == VO_ERR_NONE)
	{
/*
		if (m_OutputInfo.Format.Channels == 1)
		{
			m_OutputInfo.Format.Channels = 2;
			unsigned short * pData = (unsigned short *)pOutput->Buffer;
			for (int i = (pOutput->Length / 2) - 1; i >= 0; i--)
			{
				*(pData + i * 2) = *(pData + i);
				*(pData + i * 2 + 1) = *(pData + i);
			}
			pOutput->Length = pOutput->Length * 2;
		}
*/
//		if (m_hDumpFile == NULL)
//			m_hDumpFile = fopen ("/mnt/sdcard/vomeAudio.pcm", "wb");
//			m_hDumpFile = fopen ("/mnt/sdcard/vomeAudio.pcm", "rb");
//		if (pOutput->Buffer != NULL)
//			fwrite (pOutput->Buffer, 1, pOutput->Length, m_hDumpFile);
//			fread (pOutput->Buffer, 1, pOutput->Length, m_hDumpFile);

		pAudioFormat->SampleRate = m_OutputInfo.Format.SampleRate;
		pAudioFormat->Channels = m_OutputInfo.Format.Channels;
		pAudioFormat->SampleBits = m_OutputInfo.Format.SampleBits;

		if (m_fmtAudio.SampleRate != m_OutputInfo.Format.SampleRate || m_fmtAudio.Channels != m_OutputInfo.Format.Channels || m_fmtAudio.SampleBits != m_OutputInfo.Format.SampleBits)
		{
			m_fmtAudio.SampleRate = m_OutputInfo.Format.SampleRate;
			m_fmtAudio.Channels = m_OutputInfo.Format.Channels;
			m_fmtAudio.SampleBits = m_OutputInfo.Format.SampleBits;

			VOLOGI ("Audio Format was changed. S %d, C %d, B %d", (int)pAudioFormat->SampleRate, (int)pAudioFormat->Channels, (int)pAudioFormat->SampleBits);
		}

		//for real audio, we regard all buffer used!!
		//East, 2009/09/24
		if(VO_AUDIO_CodingRA == m_nCoding)
			m_OutputInfo.InputUsed = m_pInputData->Length;
	}

//	if (m_nInputUsed >= m_OutputInfo.InputUsed)
//		return VO_ERR_INPUT_BUFFER_SMALL;

	m_nInputUsed = m_OutputInfo.InputUsed;

// 	if(VO_ERR_IOMXDEC_NeedRetry == nRC)
// 		return nRC;

// 	nRC = nRC & 0X8000FFFF;

	return nRC;
}

VO_U32 CAudioDecoder::HandleFirstSample (VO_CODECBUFFER * pInput)
{
	if (m_nCoding== VO_AUDIO_CodingPCM)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingMP3)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingAAC)
	{
		if (m_nFourCC == 0XFF || m_nFourCC == 0XA106)	//AAC raw data
		{
			if (pInput->Length < 7)
			{
				SetHeadData (pInput->Buffer, pInput->Length);
				return VO_ERR_NONE;
			}
		}
		else  //make sure the first adts frame input successfully.
		{
			return VO_ERR_NOT_IMPLEMENT;
		}
	}
	else if (m_nCoding == VO_AUDIO_CodingRA)
	{
		if (m_bSetHeadData)
			return VO_ERR_NOT_IMPLEMENT;

		if(pInput->Length < sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO) || pInput->Buffer == NULL)
			return VO_ERR_NOT_IMPLEMENT;

		VORA_INIT_PARAM* pInitParam = (VORA_INIT_PARAM*)pInput->Buffer;
		if(pInitParam->format->ulOpaqueDataSize + sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO) == pInput->Length && 
			(pInitParam->format->usNumChannels > 0 && pInitParam->format->usNumChannels <= 6) && 
			pInitParam->format->usAudioQuality <= 100)
		{
			SetHeadData(pInput->Buffer, pInput->Length);
			return VO_ERR_NONE;
		}

		return VO_ERR_NOT_IMPLEMENT;
	}
	else if (m_nCoding== VO_AUDIO_CodingAMRNB)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingAMRWB)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingAMRWBP)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingWMA || m_nCoding== VO_AUDIO_CodingADPCM)
	{
		if (pInput->Length < VO_WAVEFORMATEX_STRUCTLEN || pInput->Buffer == NULL)
			return VO_ERR_NOT_IMPLEMENT;

		VO_WAVEFORMATEX * pWave = (VO_WAVEFORMATEX *)pInput->Buffer;
//		if (pWave->nSamplesPerSec == m_OutputInfo.Format.SampleRate &&
//			pWave->nChannels == m_OutputInfo.Format.Channels &&
//			pWave->wBitsPerSample == m_OutputInfo.Format.SampleBits)
		if ((unsigned int)(pWave->cbSize + VO_WAVEFORMATEX_STRUCTLEN) == (unsigned int)pInput->Length &&
			(pWave->nChannels > 0 && pWave->nChannels <= 6))
		{
			SetHeadData (pInput->Buffer, pInput->Length);
			return VO_ERR_NONE;
		}

		return VO_ERR_NOT_IMPLEMENT;
	}
	else if (m_nCoding== VO_AUDIO_CodingQCELP13)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingEVRC)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingAC3)
	{	
		//Fix 9625 issue, 2011-12-19 Lhp
		VO_U32 dwLen = pInput->Length & 0x7FFFFFFF;
		if(dwLen > 5 && pInput->Buffer != NULL)
		{
			VO_PBYTE pHead = pInput->Buffer;
			VO_PBYTE pTail = pInput->Buffer + dwLen - 5;
			VO_U32 dwTimes = 0;

			while(pHead < pTail)
			{
				if(0xB != pHead[0])
				{
					pHead++;
					continue;
				}

				if(0x77 != pHead[1])
				{
					pHead += 2;
					continue;
				}

				if(((pHead[5] >> 3) & 0x1F) == 0x10)
				{
					m_nCoding = VO_AUDIO_CodingEAC3;
					break;
				}

				pHead += 6;
				dwTimes++;
				if(dwTimes >= 2)
					break;
			}
			if(m_nCoding == VO_AUDIO_CodingEAC3)
			{
				int channel = 0;
				int acmod = 0;
				int lfeon = 0;
				int bits = 0;

				acmod = (pHead[6] >> 5) & 0x7;
				bits = 3;
				if ((acmod != 1) && (acmod & 0x1))
				{
					bits += 2;
				}
				if (acmod & 0x4)
				{
					bits += 2;
				}
				if (acmod == 2)
				{
					bits += 2;
				}

				lfeon = pHead[6] & (1 << (7 - bits));
				static const int channelNum[8] = {2, 1, 2, 3, 3, 4, 4, 5}; 
				channel = channelNum[acmod] + lfeon;
				
				if(channel <= 2)
				{
					m_fmtAudio.Channels = channel;
				}
				else if(channel > 2 && m_fmtAudio.Channels > 2)
				{
					m_fmtAudio.Channels = channel;
				}

				VO_AUDIO_FORMAT	fmtAudio;
				fmtAudio.Channels = m_fmtAudio.Channels;
				fmtAudio.SampleRate = m_fmtAudio.SampleRate;
				fmtAudio.SampleBits = m_fmtAudio.SampleBits;

				Init(pInput->Buffer, 0, &fmtAudio);
			}
		}
		return VO_ERR_NOT_IMPLEMENT;
	}
	else if (m_nCoding== VO_AUDIO_CodingEAC3)
		return VO_ERR_NOT_IMPLEMENT;
	else if (m_nCoding== VO_AUDIO_CodingFLAC)
	{
		if (m_bSetHeadData)
			return VO_ERR_NOT_IMPLEMENT;

		SetHeadData (pInput->Buffer, pInput->Length);
		return VO_ERR_NONE;
	}
	else if (m_nCoding== VO_AUDIO_CodingOGG)
	{
		if (m_bSetHeadData)
			return VO_ERR_NOT_IMPLEMENT;

		SetHeadData (pInput->Buffer, pInput->Length);
		return VO_ERR_NONE;
	}
	else if( m_nCoding == VO_AUDIO_CodingAPE )
	{
		if (m_bSetHeadData)
			return VO_ERR_NOT_IMPLEMENT;

		SetHeadData (pInput->Buffer, pInput->Length);
		return VO_ERR_NONE;
	}
	else if( m_nCoding == VO_AUDIO_CodingALAC )
		return VO_ERR_NOT_IMPLEMENT;

	return VO_ERR_NONE;
}

VO_U32 CAudioDecoder::Flush (void)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nFlush = 1;
	VO_U32 nRC = m_funDec.SetParam (m_hDec, VO_PID_COMMON_FLUSH, &nFlush);

	return nRC;
}

VO_U32 CAudioDecoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (uParamID == VO_PID_COMMON_LOGFUNC)
		m_pcbVOLOG = (VO_LOG_PRINT_CB *)pData;

	if (uParamID == VO_PID_COMMON_WORKPATH)
	{
		VO_TCHAR* pWorkPath = (VO_TCHAR*)pData;
		vostrcpy(m_sWorkPath, pWorkPath);
	}	
	
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funDec.SetParam (m_hDec, uParamID, pData);
}

VO_U32 CAudioDecoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funDec.GetParam (m_hDec, uParamID, pData);
}

VO_U32 CAudioDecoder::GetFormat (VO_AUDIO_FORMAT * pFormat)
{
	pFormat->SampleRate = m_OutputInfo.Format.SampleRate;
	pFormat->Channels = m_OutputInfo.Format.Channels;
	pFormat->SampleBits = m_OutputInfo.Format.SampleBits;

	return VO_ERR_NONE;
}

VO_U32 CAudioDecoder::DownMixBuffer (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat)
{
	int errType = 0;
	VO_U32	i;
	VO_S16*	InBuf = (VO_S16*)m_pInputData->Buffer;
	VO_S16*	OutBuf = (VO_S16*)pOutput->Buffer;
	VO_U32	Length = m_pInputData->Length/(m_fmtAudio.Channels*sizeof(VO_S16));
	VO_U32	InChan = m_fmtAudio.Channels;
	VO_U32	OutChan = 2;

	if(InChan == 6 && OutChan == 2)
	{
		VO_S32 C,L_S,R_S,tmp,tmp1,cum;			
	
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

			OutBuf[0] = (VO_S16)CLIPTOSHORT(tmp);
			OutBuf[1] = (VO_S16)CLIPTOSHORT(tmp1);
			OutBuf+=OutChan;
			InBuf+=InChan;
		}

		pOutput->Length = Length*sizeof(VO_S16)*2;
		m_pInputData->Length = 0;
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

		pOutput->Length = Length*sizeof(VO_S16)*2;	
		m_pInputData->Length = 0;
	}
	else if(InChan <= 2)
	{
		OutChan = InChan;
		if(pOutput->Length >= m_pInputData->Length)
		{
			memcpy (pOutput->Buffer, m_pInputData->Buffer, m_pInputData->Length);
			pOutput->Length = m_pInputData->Length;
			m_pInputData->Length = 0;
		}
		else
		{
			memcpy(pOutput->Buffer , m_pInputData->Buffer , pOutput->Length);
			m_pInputData->Buffer += pOutput->Length;
			m_pInputData->Length -= pOutput->Length;
		}
		
	}

	pAudioFormat->SampleRate = m_OutputInfo.Format.SampleRate;
	pAudioFormat->Channels = OutChan;
	pAudioFormat->SampleBits = m_OutputInfo.Format.SampleBits;

	return errType;
}


VO_U32 CAudioDecoder::LoadLib (VO_HANDLE hInst)
{
#ifdef _VO_LIB
#ifdef _VO_LIB_RA
	if (m_nCoding== VO_AUDIO_CodingRA)
		voGetRADecAPI(&m_funDec);
#endif //_VO_LIB_RA
#else
#ifdef _LIB
	if(m_nCoding== VO_AUDIO_CodingPCM)
		return 1;
	else if(m_nCoding== VO_AUDIO_CodingAAC)
		voGetAACDecAPI(&m_funDec);
	else
		return 0;
#else	//_LIB
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_nCoding== VO_AUDIO_CodingPCM)
	{
		return 1;
	}
	else if (m_nCoding== VO_AUDIO_CodingMP1 || m_nCoding== VO_AUDIO_CodingMP3)
	{
		strcpy (m_szCfgItem, "Audio_Dec_MP3");
		vostrcpy (m_szDllFile, _T("voMP3Dec"));
		vostrcpy (m_szAPIName, _T("voGetMP3DecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingAAC)
	{
		strcpy (m_szCfgItem, "Audio_Dec_AAC");
		vostrcpy (m_szDllFile, _T("voAACDec"));
		vostrcpy (m_szAPIName, _T("voGetAACDecAPI"));
		if (m_pConfig != NULL)
			m_bDisableAACP = m_pConfig->GetItemValue (m_szCfgItem, (char*)"DisableAACPlus", 0) == 0 ? VO_FALSE : VO_TRUE;
#ifdef __SYMBIAN32__
		m_bDisableAACP = VO_TRUE;
#endif // __SYMBIAN32__
	}
	else if (m_nCoding== VO_AUDIO_CodingAMRNB)
	{
		strcpy (m_szCfgItem, "Audio_Dec_AMRNB");
		vostrcpy (m_szDllFile, _T("voAMRNBDec"));
		vostrcpy (m_szAPIName, _T("voGetAMRNBDecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingAMRWB)
	{
		strcpy (m_szCfgItem, "Audio_Dec_AMRWB");
		vostrcpy (m_szDllFile, _T("voAMRWBDec"));
		vostrcpy (m_szAPIName, _T("voGetAMRWBDecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingAMRWBP)
	{
		strcpy (m_szCfgItem, "Audio_Dec_AMRWBP");
		vostrcpy (m_szDllFile, _T("voAMRWBPDec"));
		vostrcpy (m_szAPIName, _T("voGetAMRWBPDecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingWMA)
	{
		strcpy (m_szCfgItem, "Audio_Dec_WMA");
		vostrcpy (m_szDllFile, _T("voWMADec"));
		vostrcpy (m_szAPIName, _T("voGetWMADecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingADPCM)
	{
		strcpy (m_szCfgItem, "Audio_Dec_ADPCM");
		vostrcpy (m_szDllFile, _T("voADPCMDec"));
		vostrcpy (m_szAPIName, _T("voGetADPCMDecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingQCELP13)
	{
		strcpy (m_szCfgItem, "Audio_Dec_QCELP");
		vostrcpy (m_szDllFile, _T("voQCELPDec"));
		vostrcpy (m_szAPIName, _T("voGetQCELPDecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingEVRC)
	{
		strcpy (m_szCfgItem, "Audio_Dec_EVRC");
		vostrcpy (m_szDllFile, _T("voEVRCDec"));
		vostrcpy (m_szAPIName, _T("voGetEVRCDecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingAC3)
	{
		strcpy (m_szCfgItem, "Audio_Dec_AC3");
		vostrcpy (m_szDllFile, _T("voAC3Dec"));
		vostrcpy (m_szAPIName, _T("voGetAC3DecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingEAC3)
	{
		strcpy (m_szCfgItem, "Audio_Dec_EAC3");
		vostrcpy (m_szDllFile, _T("voEAC3Dec"));
		vostrcpy (m_szAPIName, _T("voGetEAC3DecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingFLAC)
	{
		strcpy (m_szCfgItem, "Audio_Dec_FLAC");
		vostrcpy (m_szDllFile, _T("voFLACDec"));
		vostrcpy (m_szAPIName, _T("voGetFLACDecAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingOGG)
	{
		strcpy (m_szCfgItem, "Audio_Dec_OGG");
		vostrcpy (m_szDllFile, _T("voOGGDec"));
		vostrcpy (m_szAPIName, _T("voGetOGGDecAPI"));
	}
	else if (m_nCoding == VO_AUDIO_CodingRA)
	{
		strcpy (m_szCfgItem, "Audio_Dec_RA");
		vostrcpy (m_szDllFile, _T("voRADec"));
		vostrcpy (m_szAPIName, _T("voGetRADecAPI"));
	}
	else if( m_nCoding == VO_AUDIO_CodingAPE )
	{
		strcpy (m_szCfgItem, "Audio_Dec_MAC ");
		vostrcpy (m_szDllFile, _T("voAPEDec"));
		vostrcpy (m_szAPIName, _T("voGetAPEDecAPI"));
	}
	else if( m_nCoding == VO_AUDIO_CodingALAC )
	{
		strcpy (m_szCfgItem, "Audio_Dec_ALAC ");
		vostrcpy (m_szDllFile, _T("voALACDec"));
		vostrcpy (m_szAPIName, _T("voGetALACDecAPI"));
	}

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"File");
		pApiName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Api");

		if (m_bOMXComp && pDllFile != NULL)
		{
			if (!strcmp (pDllFile, "voOMXWrap"))
			{
				pDllFile = NULL;
				pApiName = NULL;
			}
		}
	}

	if(m_nCoding == m_nSetAudioNameType)
	{
		pDllFile = (VO_PCHAR)m_sSetAudioName;
		VOLOGI ("The audio dec module is %s from setting.", pDllFile);
	}

	if(m_nCoding == m_nSetAudioAPIType)
	{
		pApiName = (VO_PCHAR)m_sSetAudioAPI;
		VOLOGI ("The audio api is %s from setting.", pApiName);
	}

    // added by gtxia for dealing with AC3/EAC3 with MediaCodec's audio
	bool isAMCDec = !vostrcmp(m_sSetAudioName, _T("voAudioMCDec"));
	
	
	if(pDllFile != (VO_PCHAR)m_sSetAudioName && isAMCDec && ((m_nCoding== VO_AUDIO_CodingAC3) || m_nCoding== VO_AUDIO_CodingEAC3))
	{
		pDllFile = (VO_PCHAR)m_sSetAudioName;
		pApiName = (VO_PCHAR)m_sSetAudioAPI;
		mOutputType = (VO_AUDIO_CODINGTYPE)m_nCoding;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (pDllFile == NULL)
		m_bVOUsed = VO_TRUE;

#if defined _WIN32
	if (pDllFile != NULL && !m_bVOUsed)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL && !m_bVOUsed)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL && !m_bVOUsed)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);
#elif defined __SYMBIAN32__
	if (pDllFile != NULL && !m_bVOUsed)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".dll"));

	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);

#elif defined _MAC_OS
	if (pDllFile != NULL && !m_bVOUsed)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".dylib"));
	
	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);	
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETAUDIODECAPI pAPI = (VOGETAUDIODECAPI) m_pAPIEntry;
	pAPI (&m_funDec);
#endif	//_LIB
#endif // _SYMBIAN_LIB

	return 1;
}


VO_U32 CAudioDecoder::SetAudioLibFile (VO_U32 nAudioType, VO_PTCHAR pFileName)
{
	m_nSetAudioNameType = nAudioType;
	
	if(pFileName)
		vostrcpy(m_sSetAudioName, pFileName);

	return 0;
}

VO_U32 CAudioDecoder::SetAudioAPIName (VO_U32 nAudioType, VO_PTCHAR pAPIName)
{
	m_nSetAudioAPIType = nAudioType;
	
	if(pAPIName)
		vostrcpy(m_sSetAudioAPI, pAPIName);

	return 0;
}

bool CAudioDecoder::setSampleDrmInfo(const bool isDrmAppended)
{
	mIsDrmDataAppended = isDrmAppended;
	return true;
}
