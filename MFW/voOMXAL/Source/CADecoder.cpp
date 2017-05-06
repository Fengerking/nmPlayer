	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CADecoder.cpp

	Contains:	CADecoder class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voString.h"

#include "voMMPlay.h"
#include "CADecoder.h"

#include "voAAC.h"
#include "voAC3.h"
#include "voFLAC.h"
#include "voADPCM.h"
#include "voRealAudio.h"
#include "voOSFunc.h"

#define LOG_TAG "CADecoder"
#include "voLog.h"

#ifdef _IOS
#include "voWMA.h"
#include "voRealAudio.h"
#include "voMP3.h"
#endif

typedef VO_S32 (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

CADecoder::CADecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hDec (NULL)
	, m_nCoding (nCoding)
	, m_nFourCC (0xF7)
	, m_bFirstSample (VO_TRUE)
	, m_pInputData (0)
	, m_nInputUsed (0)
	, m_bSetHeadData (VO_FALSE)
	, m_bDisableAACP (VO_FALSE)
	, m_pLibFunc(NULL)
	//, m_hDumpFile (NULL)
{
}

CADecoder::~CADecoder ()
{
	Uninit ();

	//if (m_hDumpFile != NULL)
		//fclose (m_hDumpFile);
}


VO_U32 CADecoder::Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_AUDIO_FORMAT * pFormat)
{
	if (m_nCoding > VO_AUDIO_CodingALAC || m_nCoding < VO_AUDIO_CodingPCM)
	{
		if (m_pError != NULL)
			vostrcpy (m_pError, _T("The audio format is unknown!"));

		VOLOGE ("[OMXAL]The coding %d was unknown!", (int)m_nCoding);
		return VO_ERR_VOMMP_AudioDecoder;
	}
	
	VOLOGI ("[OMXAL]Audio Format is S %d, C %d, B %d", (int)pFormat->SampleRate, (int)pFormat->Channels, (int)pFormat->SampleBits);

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
	initInfo.memData = (void*)m_pMemOP;
	
	VOLOGI("[OMXAL]m_pMemOP %x, m_nCoding %x", (unsigned int)m_pMemOP, (unsigned int)m_nCoding);

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

		if (LoadLib (m_hInst) != VONP_ERR_None)
		{
			VOLOGE ("[OMXAL]LoadLib was failed!");
			return VO_ERR_VOMMP_AudioDecoder;
		}
		
		VOLOGI("[OMXAL]+Init %x", (unsigned int)m_funDec.Init);
		nRC = m_funDec.Init (&m_hDec, (VO_AUDIO_CODINGTYPE)m_nCoding, &initInfo);
		VOLOGI("-Init %x", (unsigned int)nRC);
		
		if (m_hDec == NULL)
		{
			VOLOGE ("[OMXAL]m_funDec.Init was failed! Result 0X%08X", (unsigned int)nRC);
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

VO_U32 CADecoder::UpdateParam (void)
{
	//VO_U32 nRC = 0;
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

		if (m_bDisableAACP)
		{
			int nDisable = 1;
			SetParam (VO_PID_AAC_DISABLEAACPLUSV1, &nDisable);
			SetParam (VO_PID_AAC_DISABLEAACPLUSV2, &nDisable);
		}
	}
	else if(m_nCoding == VO_AUDIO_CodingAC3)
	{
		if (m_OutputInfo.Format.Channels > 2)
		{
			int channel = 6;
			SetParam (VO_PID_AC3_NUMCHANS, &channel);
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

	return 0;
}

VO_U32 CADecoder::SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	if (m_hDec == NULL)
		return 0;

	VO_U32 nRC = 0;
	if (nHeadSize > 0)
	{
		VO_CODECBUFFER buffHead;
		buffHead.Buffer = pHeadData;
		buffHead.Length = nHeadSize;

		VOLOGS ("[OMXAL]Set the head data to decoder module.");

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

VO_U32 CADecoder::Uninit (void)
{
	if (m_hDec != NULL)
	{
		m_funDec.Uninit (m_hDec);
		m_hDec = NULL;
	}

	FreeLib ();

	return 0;
}

VO_U32 CADecoder::Start(void)
{
	return 	SetParam (VO_PID_COMMON_START, 0);
}

VO_U32 CADecoder::Pause(void)
{
	return 	SetParam (VO_PID_COMMON_PAUSE, 0);
}

VO_U32 CADecoder::Stop(void)
{
	return 	SetParam (VO_PID_COMMON_STOP, 0);
}

VO_U32 CADecoder::SetInputData (VO_CODECBUFFER * pInput)
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

//	VOLOGI ("[OMXAL]Set Input Data size %d Time is %d", pInput->Length, (int)pInput->Time);
	return 	m_funDec.SetInputData (m_hDec, pInput);
}

VO_U32 CADecoder::GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat, VO_BOOL& bFormatChanged)
{
//	VOLOGI ("[OMXAL]GetPutputData buffer Size %d.", pOutput->Length);

	if (m_nCoding == VO_AUDIO_CodingPCM)
	{
		if (m_pInputData == NULL || m_pInputData->Length == 0)
			return VO_ERR_INPUT_BUFFER_SMALL;

		memcpy (pOutput->Buffer, m_pInputData->Buffer, m_pInputData->Length);
		pOutput->Length = m_pInputData->Length;

		pAudioFormat->SampleRate = m_OutputInfo.Format.SampleRate;
		pAudioFormat->Channels = m_OutputInfo.Format.Channels;
		pAudioFormat->SampleBits = m_OutputInfo.Format.SampleBits;

		m_pInputData->Length = 0;

		return VO_ERR_NONE;
	}

	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	VO_U32 nRC = m_funDec.GetOutputData (m_hDec, pOutput, &m_OutputInfo);
//	VOLOGI ("[OMXAL]GetPutputData Size %d, Result 0X%08X !", pOutput->Length, nRC);

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
			bFormatChanged = VO_TRUE;

			VOLOGI ("[OMXAL]Audio Format was changed. S %d, C %d, B %d", (int)pAudioFormat->SampleRate, (int)pAudioFormat->Channels, (int)pAudioFormat->SampleBits);
		}
		else
			bFormatChanged = VO_FALSE;

		//for real audio, we regard all buffer used!!
		//East, 2009/09/24
		if(VO_AUDIO_CodingRA == m_nCoding)
			m_OutputInfo.InputUsed = m_pInputData->Length;
	}

//	if (m_nInputUsed >= m_OutputInfo.InputUsed)
//		return VO_ERR_INPUT_BUFFER_SMALL;

	m_nInputUsed = m_OutputInfo.InputUsed;

	nRC = nRC & 0X8000FFFF;

	return nRC;
}

VO_U32 CADecoder::HandleFirstSample (VO_CODECBUFFER * pInput)
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

		if(pInput->Length < sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO))
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
		if (pInput->Length < VO_WAVEFORMATEX_STRUCTLEN)
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
		if(dwLen > 5)
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
				m_fmtAudio.Channels = channel;

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
		SetHeadData (pInput->Buffer, pInput->Length);
		return VO_ERR_NONE;
	}
	else if( m_nCoding == VO_AUDIO_CodingALAC )
		return VO_ERR_NOT_IMPLEMENT;

	return VO_ERR_NONE;
}

VO_U32 CADecoder::Flush (void)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nFlush = 1;
	VO_U32 nRC = m_funDec.SetParam (m_hDec, VO_PID_COMMON_FLUSH, &nFlush);

	return nRC;
}

VO_U32 CADecoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funDec.SetParam (m_hDec, uParamID, pData);
}

VO_U32 CADecoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funDec.GetParam (m_hDec, uParamID, pData);
}

VO_U32 CADecoder::GetFormat (VO_AUDIO_FORMAT * pFormat)
{
	pFormat->SampleRate = m_OutputInfo.Format.SampleRate;
	pFormat->Channels = m_OutputInfo.Format.Channels;
	pFormat->SampleBits = m_OutputInfo.Format.SampleBits;

	return VO_ERR_NONE;
}

VO_U32 CADecoder::LoadLib (VO_HANDLE hInst)
{
	VOLOGI("[OMXAL]+Load audio decoder, lib op %x", (unsigned int)m_pLibFunc);
	
	if(!m_pLibFunc) 
		return VONP_ERR_Pointer;
	
	if (m_hDll != 0) 
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_funDec, 0, sizeof(m_funDec));
	}
	
#ifdef _WIN32
	m_hDll = (HMODULE)m_pLibFunc->LoadLib(m_pLibFunc->pUserData, (char*)"voAACDec", 0);
#else	// _WIN32
	m_hDll = m_pLibFunc->LoadLib(m_pLibFunc->pUserData, (char*)"voAACDec", 0);
#endif	// _WIN32

	if (!m_hDll) 
	{
		return VONP_ERR_Unknown;
	}
	VOLOGI("[OMXAL]Load audio dec lib ok");
	
	VOGETAUDIODECAPI	pGetFuncSet = NULL;
	pGetFuncSet	= (VOGETAUDIODECAPI)m_pLibFunc->GetAddress(m_pLibFunc->pUserData, m_hDll, (char*)"voGetAACDecAPI", 0);
	
	if(pGetFuncSet == NULL)
	{
		VOLOGI("[OMXAL]Get audio dec func set failed");
		return VONP_ERR_Pointer;
	}
	
	pGetFuncSet(&m_funDec);
		
	return VONP_ERR_None;
	
/*	VO_PCHAR pDllFile = NULL;
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
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETAUDIODECAPI pAPI = (VOGETAUDIODECAPI) m_pAPIEntry;
	pAPI (&m_funDec);

	return 1;
 */
}
