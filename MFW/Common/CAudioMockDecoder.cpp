	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioMockDecoder.cpp

	Contains:	CAudioMockDecoder class file

	Written by:	Lin Jun

	Change History (most recent first):
	2010-12-04		Lin Jun			Create file

*******************************************************************************/
#include "voString.h"
#include "vompType.h"

#include "voMMPlay.h"
#include "CAudioMockDecoder.h"

#include "voAAC.h"
#include "voADPCM.h"
#include "voRealAudio.h"

#define LOG_TAG "CAudioMockDecoder"
#include "voLog.h"

#ifdef _IOS
#include "voWMA.h"
#endif

typedef VO_S32 (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

CAudioMockDecoder::CAudioMockDecoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CAudioDecoder (hInst, nCoding, pMemOP)
, m_pLastOutputBuf(NULL)
, m_pAQS(NULL)
, m_bPause(VO_TRUE)
{
	m_pAQS = new CAudioQueueService;
}

CAudioMockDecoder::~CAudioMockDecoder ()
{
	Uninit();
}


VO_U32 CAudioMockDecoder::Init (VO_PBYTE pHeadData, VO_U32 nHeadSize, VO_AUDIO_FORMAT * pFormat)
{
	m_OutputInfo.Format.SampleRate = pFormat->SampleRate;
	m_OutputInfo.Format.Channels = pFormat->Channels;
	m_OutputInfo.Format.SampleBits = pFormat->SampleBits;

	m_fmtAudio.SampleRate = pFormat->SampleRate;
	m_fmtAudio.Channels = pFormat->Channels;
	m_fmtAudio.SampleBits = pFormat->SampleBits;
	
	if(m_nCoding== VO_AUDIO_CodingAAC)
		m_pAQS->SetInputFormat(INPUT_AAC);
	else if(m_nCoding == VO_AUDIO_CodingMP3)
		m_pAQS->SetInputFormat(INPUT_MP3);
	else 
	{
		m_pAQS->SetInputFormat(INPUT_PCM);
	}
	
	m_pAQS->SetAudioFormat(pFormat->SampleRate, pFormat->Channels, pFormat->SampleBits);
	


	
	m_pAQS->InitDevice();
	m_pAQS->Start();

	return 0;
}

VO_U32 CAudioMockDecoder::UpdateParam (void)
{
	VO_U32 nRC = 0;

	return 0;
}

VO_U32 CAudioMockDecoder::SetHeadData (VO_PBYTE pHeadData, VO_U32 nHeadSize)
{
	VO_U32 nRC = 0;

	return nRC;
}

VO_U32 CAudioMockDecoder::Uninit (void)
{
	if(m_pAQS)
	{
		delete m_pAQS;
		m_pAQS = NULL;
	}
	
	return 0;
}


VO_U32 CAudioMockDecoder::Start(void)
{
	if(m_pAQS)
	{
		m_bPause = VO_FALSE;
		m_pAQS->Start();
	}
		
	return 	0;
}

VO_U32 CAudioMockDecoder::Pause(void)
{
	if(m_pAQS)
	{
		m_bPause = VO_TRUE;
		m_pAQS->Pause();
	}
		
	
	return 	0;
}

VO_U32 CAudioMockDecoder::Stop(void)
{
	if(m_pAQS)
	{
		m_bPause = VO_TRUE;
		m_pAQS->Stop();
	}
		
	return 	0;
}

VO_U32 CAudioMockDecoder::SetInputData (VO_CODECBUFFER * pInput)
{
	if(m_bPause)
		return 0;
	
	m_pInputData = pInput;
	
	// if ADTS , need skip ADTS header
	if(m_nCoding== VO_AUDIO_CodingAAC)
		m_pAQS->Render(pInput->Buffer+7, pInput->Length-7, pInput->Time, 0, false);
	else if(m_nCoding == VO_AUDIO_CodingMP3)
		m_pAQS->Render(pInput->Buffer, pInput->Length, pInput->Time, 0, false);
	
	return 0;
}

VO_U32 CAudioMockDecoder::GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat)
{
	if(m_pLastOutputBuf == pOutput->Buffer)
		return VOMP_ERR_Retry;
	
	//memcpy(pOutput->Buffer, m_pInputData->Buffer, m_pInputData->Length);
	pOutput->Length	= m_pInputData->Length;
	pOutput->Time	= m_pInputData->Time;
	

	pAudioFormat->SampleRate	= m_OutputInfo.Format.SampleRate;
	pAudioFormat->Channels		= m_OutputInfo.Format.Channels;
	pAudioFormat->SampleBits	= m_OutputInfo.Format.SampleBits;
	
	m_pLastOutputBuf = pOutput->Buffer;
	
	int nRC = 0;

	return nRC;
}

VO_U32 CAudioMockDecoder::HandleFirstSample (VO_CODECBUFFER * pInput)
{
	return VO_ERR_NONE;
}

VO_U32 CAudioMockDecoder::Flush (void)
{
	VO_U32 nRC = 0;
	
	if (m_pAQS)
	{
		m_pAQS->Flush();
	}

	return nRC;
}

VO_U32 CAudioMockDecoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funDec.SetParam (m_hDec, uParamID, pData);
}

VO_U32 CAudioMockDecoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hDec == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funDec.GetParam (m_hDec, uParamID, pData);
}

VO_U32 CAudioMockDecoder::GetFormat (VO_AUDIO_FORMAT * pFormat)
{
	pFormat->SampleRate = m_OutputInfo.Format.SampleRate;
	pFormat->Channels = m_OutputInfo.Format.Channels;
	pFormat->SampleBits = m_OutputInfo.Format.SampleBits;

	return VO_ERR_NONE;
}

VO_U32 CAudioMockDecoder::LoadLib (VO_HANDLE hInst)
{
	return 1;
}
