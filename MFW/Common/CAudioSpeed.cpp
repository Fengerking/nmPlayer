	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2013				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioSpeed.cpp

	Contains:	CAudioSpeed class file

	Written by: Rogine Xu

	Change History (most recent first):
	2013-05-21		Rogine Xu			Create file

*******************************************************************************/
#include "voString.h"
#include "voAudio.h"
#include "voMMPlay.h"
#include "CAudioSpeed.h"

#define LOG_TAG "CAudioSpeed"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETAUDIOSPEEDAPI) (VO_AUDIO_CODECAPI * pDecHandle);

CAudioSpeed::CAudioSpeed (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hSpeed (NULL) 
	, m_pInputData (NULL)
	, m_nInputUsed (0)
	, m_nCurrSpeed (0)
{
	memset(&m_fmtAudio, 0, sizeof(VO_AUDIO_FORMAT));
	memset(&m_OutputInfo, 0, sizeof(VO_AUDIO_OUTPUTINFO));
	//memset(m_OutputBuf, 0, AUDIO_SPEED_UNIT_SZ);
}

CAudioSpeed::~CAudioSpeed()
{
	Uninit();
}

VO_U32 CAudioSpeed::Init(VO_AUDIO_FORMAT * pFormat)
{
	VO_CODEC_INIT_USERDATA initInfo;
	memset(&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = m_pMemOP;

	if (m_pLibOP != NULL)
	{
		initInfo.memflag    |= 0X10;
		initInfo.libOperator = m_pLibOP;
	}	
	
	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;
	while (nRC != VO_ERR_NONE)
	{
		Uninit();
		if (LoadLib(m_hInst) == 0)	
		{
			VOLOGE("@@@####rndAs Loadlib was failed!");
			return VO_ERR_VOMMP_AudioDecoder;
		}
		
		nRC = m_funSpeed.Init(&m_hSpeed, VO_AUDIO_CodingPCM, &initInfo);
		if (NULL ==  m_hSpeed)
		{
			VOLOGE("@@@####rndAs m_funSpeed.Init was failed! Result 0X%08X", nRC);
			return VO_ERR_VOMMP_AudioDecoder;
		}

		if (VO_ERR_NONE == nRC)
			break;

		if (m_bVOUsed)
			break;

		m_bVOUsed =  VO_TRUE;
	}

	nRC = SetFormat(pFormat);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGI("@@@#### SetFormat error!");
	}

	return nRC;
}

VO_U32 CAudioSpeed::Uninit()
{
	if (m_hSpeed != NULL)
	{
		m_funSpeed.Uninit(m_hSpeed);
		m_hSpeed = NULL;
	}

	FreeLib();
	return 0;
}

VO_U32 CAudioSpeed::LoadLib(VO_HANDLE hInst)
{
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	strcpy   (m_szCfgItem, "Audio_Effect_Speed");
	vostrcpy (m_szDllFile, _T("voAudioSpeed"));
	vostrcpy (m_szAPIName, _T("voGetAudioSpeedAPI"));

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"File");
		pApiName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Api");
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
	
	VOLOGI ("@@@####rndAs m_szDllFile: %s", m_szDllFile);
	VOLOGI ("@@@####rndAs m_szAPIName: %s", m_szAPIName);

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("@@@####rndAs CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	if (m_pAPIEntry == NULL)
	{
		VOLOGI ("@@@####rndAs m_pAPIEntry is NULL!");
		return 0;
	}	

 	VOGETAUDIOSPEEDAPI pAPI = (VOGETAUDIOSPEEDAPI) m_pAPIEntry;
	pAPI (&m_funSpeed);

	return 1;
}

VO_U32 CAudioSpeed::SetInputData(VO_CODECBUFFER * pInput)
{
	m_nInputUsed = 0;
	m_pInputData = pInput;

	if (NULL == pInput)
		return VO_ERR_VOMMP_AudioDecoder;

	if (NULL == m_hSpeed)
		return VO_ERR_WRONG_STATUS;
	
	return m_funSpeed.SetInputData(m_hSpeed, pInput);
}

VO_U32 CAudioSpeed::GetOutputData(VO_CODECBUFFER *pOutput, VO_AUDIO_OUTPUTINFO *pAudioFormat)
{
	if (NULL == pOutput)
		return VO_ERR_VOMMP_AudioDecoder;

	if (NULL == m_hSpeed)
		return VO_ERR_WRONG_STATUS;
	
	return m_funSpeed.GetOutputData(m_hSpeed, pOutput, pAudioFormat);
}

VO_U32 CAudioSpeed::Convert(const VO_U8 *pSrc, VO_U32 nInLen, VO_U8 *pOut, VO_U32 *pOutLen)
{
	VO_U32 nRC = VO_ERR_NONE;
	VO_U32 nOffsetSize = 0;
	VO_U32 nOutBuffSize = *pOutLen;
	VO_CODECBUFFER	inBuffer;
	VO_CODECBUFFER	outBuffer;
	VO_AUDIO_OUTPUTINFO outFormat;

	if (NULL == pSrc || NULL == pOut)
		return VO_ERR_VOMMP_AudioDecoder;

	memset(&inBuffer, 0, sizeof(VO_CODECBUFFER));

	inBuffer.Buffer = (unsigned char *)pSrc;
	inBuffer.Length = nInLen;
	nRC = SetInputData(&inBuffer);
	if (VO_ERR_NONE != nRC)	
		return VO_ERR_VOMMP_AudioDecoder;

	memset(&outBuffer, 0, sizeof(VO_CODECBUFFER));
	outBuffer.Buffer = pOut;  
		
	do{	
		nRC = GetOutputData(&outBuffer, &outFormat);
	
		if (VO_ERR_NONE == nRC)
		{
			nOffsetSize += outBuffer.Length;
			if (nOffsetSize > nOutBuffSize)
			{
				VOLOGE("Output length > Buffer size just break");
			}
			outBuffer.Buffer = pOut + nOffsetSize;

	//		VOLOGI("out len: %d", (int)outBuffer.Length);
	//		VOLOGI ("Unit size:%u", nOffsetSize);
		}
	} while (nRC != VO_ERR_INPUT_BUFFER_SMALL);
	
	*pOutLen = nOffsetSize;

	return VO_ERR_NONE;
}

VO_U32 CAudioSpeed::Flush (void)
{
	if (NULL == m_hSpeed)
		return VO_ERR_WRONG_STATUS;
	
	VO_U32 nFlush = 1;
	return m_funSpeed.SetParam(m_hSpeed, VO_PID_COMMON_FLUSH, &nFlush);
}

VO_U32 CAudioSpeed::SetParam(VO_S32 uParamID, VO_PTR pData)
{
	if (NULL == m_hSpeed)
		return VO_ERR_WRONG_STATUS;
	
	if (VO_PID_AUDIOSPEED_RATE == uParamID)
	{
		if (NULL == pData)
			return VO_ERR_WRONG_STATUS;

		float tempoRatio = 0;
		VO_S32 nAudioSpeed = *((VO_S32*)pData);
			
		if (nAudioSpeed < 10)
		{
			tempoRatio = -1 * (10-nAudioSpeed) * 10;
		} 	
		else if (nAudioSpeed > 10)
		{
			 tempoRatio = (nAudioSpeed - 10)*10;
		}

#if 0	//delete for still reset the same value, otherwise the play speed will change to normal
		if (m_nCurrSpeed == tempoRatio)
		{
			//same speed as previous, no need to set
			return VO_ERR_NONE;
		}
#endif

		m_nCurrSpeed = tempoRatio;
		
		VOLOGI ("@@@####spd Set the speed to %f in voAudioSpeed module",  m_nCurrSpeed);
	}
	else
	{
		VOLOGE("uParamID: 0x%08X is not supported", uParamID);
		return VO_ERR_WRONG_PARAM_ID;
	}

	return m_funSpeed.SetParam(m_hSpeed, uParamID, &m_nCurrSpeed);
}

VO_U32 CAudioSpeed::GetParam(VO_S32 uParamID, VO_PTR pData)
{
	if (NULL == m_hSpeed)
		return VO_ERR_WRONG_STATUS;
		
	return m_funSpeed.SetParam(m_hSpeed, uParamID, pData);
}

VO_U32 CAudioSpeed::SetFormat(VO_AUDIO_FORMAT * pFormat)
{
	voCAutoLock lock (&m_Mutex);

	if (pFormat == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nSamplerate = pFormat->SampleRate;
	VO_U32 nChannels   = pFormat->Channels;
	VO_U32 nSampleBits = pFormat->SampleBits;

	if (nChannels > 2)
	{
		VOLOGI("@@@####as Channel now is %u > 2, limit to 2", nChannels);
		nChannels = 2;
	}	

	VOLOGI("@@@####as S: %u		C: %u	B: %u", nSamplerate, nChannels, nSampleBits);

	if (m_hSpeed == NULL)
	{
		VOLOGE("m_hSpeed is NULL!");
		return VO_ERR_WRONG_STATUS;
	}	

	m_funSpeed.SetParam(m_hSpeed, VO_PID_AUDIO_CHANNELS, &nChannels);
	m_funSpeed.SetParam(m_hSpeed, VO_PID_AUDIO_SAMPLEREATE, &nSamplerate);
	m_funSpeed.SetParam(m_hSpeed, VO_PID_AUDIOSPEED_BITS, &nSampleBits);

	memcpy (&m_fmtAudio, pFormat, sizeof (VO_AUDIO_FORMAT));

	return VO_ERR_NONE;
}
