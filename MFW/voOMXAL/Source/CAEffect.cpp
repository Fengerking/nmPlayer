	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CAEffect class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voString.h"
#include "voOSFunc.h"
#include "voEFTDolby.h"

#include "voMMPlay.h"

#include "CAEffect.h"

#define LOG_TAG "CAEffect"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

CAEffect::CAEffect (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hEffect (NULL)
	, m_pInputData (0)
	, m_nInputUsed (0)
	, m_pLibFunc(NULL)
	, m_bEnable(VO_FALSE)
{
	memset (&m_sFormat, 0, sizeof (m_sFormat));
	memset (&m_sBuffer, 0, sizeof (m_sBuffer));
}

CAEffect::~CAEffect ()
{
	Uninit ();
}

VO_U32 CAEffect::Init (VO_AUDIO_FORMAT * pFormat)
{
	VO_CODEC_INIT_USERDATA	initInfo;
	memset (&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = m_pMemOP;

	if (m_pLibOP != NULL)
	{
		initInfo.memflag |= 0X10;
		initInfo.libOperator = m_pLibOP;
	}

	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;

	Uninit ();

	if (LoadLib (m_hInst) != VONP_ERR_None)
	{
		return VONP_ERR_AudioCodec;
	}

	nRC = m_funEffect.Init (&m_hEffect, VO_AUDIO_CodingPCM, &initInfo);
	if (m_hEffect == NULL)
	{
		VOLOGE ("[OMXAL]m_funEffect.Init was failed!");
		return VONP_ERR_AudioCodec;
	}

	SetFormat (pFormat);
	
	Enable(m_bEnable);

	return nRC;
}

VO_U32 CAEffect::Uninit (void)
{
	if (m_hEffect != NULL)
	{
		m_funEffect.Uninit (m_hEffect);
		m_hEffect = NULL;
	}

	FreeLib ();

	if (m_sBuffer.Buffer != NULL)
		delete []m_sBuffer.Buffer;
	m_sBuffer.Buffer = NULL;
	m_sBuffer.Length = 0;

	return 0;
}

VO_U32 CAEffect::Start(void)
{
	return 	SetParam (VO_PID_COMMON_START, 0);
}

VO_U32 CAEffect::Pause(void)
{
	return 	SetParam (VO_PID_COMMON_PAUSE, 0);
}

VO_U32 CAEffect::Stop(void)
{
	return 	SetParam (VO_PID_COMMON_STOP, 0);
}

VO_U32 CAEffect::SetInputData (VO_CODECBUFFER * pInput)
{
//	VOLOGI ("[OMXAL]Input Data Len is %d", pInput->Length);

	m_nInputUsed = 0;
	m_pInputData = pInput;

	if (m_hEffect == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

//	VOLOGI ("[OMXAL]Set Input Data size %d Time is %d", pInput->Length, (int)pInput->Time);
	return 	m_funEffect.SetInputData (m_hEffect, pInput);
}

VO_U32 CAEffect::GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	VOLOGS ("[OMXAL]Get Output Data!");

	voCAutoLock lock (&m_Mutex);

	VO_U32 nRC = m_funEffect.GetOutputData (m_hEffect, pOutput, &m_OutputInfo);
	if (nRC == VO_ERR_NONE)
		memcpy (pAudioFormat, &m_OutputInfo.Format, sizeof (VO_AUDIO_FORMAT));

	m_nInputUsed = m_OutputInfo.InputUsed;

	nRC = nRC & 0X8000FFFF;

//	VOLOGI ("Output Data Len is %d", pOutput->Length);

	return nRC;
}

VO_U32 CAEffect::Flush (void)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nFlush = 1;
	VO_U32 nRC = m_funEffect.SetParam (m_hEffect, VO_PID_COMMON_FLUSH, &nFlush);

	return nRC;
}

VO_BOOL CAEffect::IsEnable()
{
	if (m_hEffect == NULL)
		return VO_FALSE;
	
	return m_bEnable;
}

VO_U32 CAEffect::Enable (VO_BOOL bEnable)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	m_bEnable = bEnable;
	VO_U32 nRC = m_funEffect.SetParam (m_hEffect, VO_PID_EFFDOLBY_DEFAULT, &bEnable);

	return nRC;
}

VO_U32 CAEffect::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nRC = m_funEffect.SetParam (m_hEffect, VO_PID_AUDIO_FORMAT, pFormat);

	memcpy (&m_sFormat, pFormat, sizeof (VO_AUDIO_FORMAT));

	if (m_sBuffer.Buffer != NULL)
		delete []m_sBuffer.Buffer;
	m_sBuffer.Length = m_sFormat.SampleRate * m_sFormat.Channels;
	m_sBuffer.Buffer = new VO_BYTE[m_sBuffer.Length];

	return nRC;
}

VO_U32 CAEffect::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hEffect == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funEffect.SetParam (m_hEffect, uParamID, pData);
}

VO_U32 CAEffect::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hEffect == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funEffect.GetParam (m_hEffect, uParamID, pData);
}

VO_U32 CAEffect::GetFormat (VO_AUDIO_FORMAT * pFormat)
{
	pFormat->SampleRate = m_sFormat.SampleRate;
	pFormat->Channels = m_sFormat.Channels;
	pFormat->SampleBits = m_sFormat.SampleBits;

	if (pFormat->Channels > 2)
		pFormat->Channels = 2;

	return VO_ERR_NONE;
}

VO_U32 CAEffect::LoadLib (VO_HANDLE hInst)
{
	VOLOGI("[OMXAL]+Load audio effect, lib op %x", (unsigned int)m_pLibFunc);
	
	if(!m_pLibFunc) 
		return VONP_ERR_Pointer;
	
	if (m_hDll != 0) 
	{
		m_pLibFunc->FreeLib (m_pLibFunc->pUserData, m_hDll, 0);
		memset(&m_funEffect, 0, sizeof(m_funEffect));
	}

#ifdef _WIN32
	m_hDll = (HMODULE)m_pLibFunc->LoadLib(m_pLibFunc->pUserData, (char*)"voDolbyEffect", 0);
#else	// _WIN32
	m_hDll = m_pLibFunc->LoadLib(m_pLibFunc->pUserData, (char*)"voDolbyEffect", 0);
#endif	// _WIN32

	if (!m_hDll) 
	{
		return VONP_ERR_Unknown;
	}
	VOLOGI("[OMXAL]Load audio effect lib ok");
	
	VOGETAUDIODECAPI	pGetFuncSet = NULL;
	pGetFuncSet	= (VOGETAUDIODECAPI)m_pLibFunc->GetAddress(m_pLibFunc->pUserData, m_hDll, (char*)"voGetDolbyEffectAPI", 0);
	
	if(pGetFuncSet == NULL)
	{
		VOLOGI("[OMXAL]Get audio effect func set failed");
		return VONP_ERR_Pointer;
	}
	
	pGetFuncSet(&m_funEffect);
	
	return VONP_ERR_None;
}
