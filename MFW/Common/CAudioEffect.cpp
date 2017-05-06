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
#include "voOSFunc.h"
#include "voEFTDolby.h"

#include "voMMPlay.h"

#include "CAudioEffect.h"

#define LOG_TAG "CAudioEffect"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

CAudioEffect::CAudioEffect (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hEffect (NULL)
	, m_pInputData (0)
	, m_nInputUsed (0)
{
	memset (&m_sFormat, 0, sizeof (m_sFormat));
}

CAudioEffect::~CAudioEffect ()
{
	Uninit ();
}

VO_U32 CAudioEffect::Init (VO_AUDIO_FORMAT * pFormat)
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

	if (LoadLib (m_hInst) == 0)
	{
		return VO_ERR_VOMMP_AudioDecoder;
	}

	nRC = m_funEffect.Init (&m_hEffect, VO_AUDIO_CodingPCM, &initInfo);
	if (m_hEffect == NULL)
	{
		VOLOGE ("m_funEffect.Init was failed!");
		return VO_ERR_VOMMP_AudioDecoder;
	}

	SetFormat (pFormat);

	return nRC;
}

VO_U32 CAudioEffect::Uninit (void)
{
	if (m_hEffect != NULL)
	{
		m_funEffect.Uninit (m_hEffect);
		m_hEffect = NULL;
	}

	FreeLib ();

	return 0;
}

VO_U32 CAudioEffect::Start(void)
{
	return 	SetParam (VO_PID_COMMON_START, 0);
}

VO_U32 CAudioEffect::Pause(void)
{
	return 	SetParam (VO_PID_COMMON_PAUSE, 0);
}

VO_U32 CAudioEffect::Stop(void)
{
	return 	SetParam (VO_PID_COMMON_STOP, 0);
}

VO_U32 CAudioEffect::SetInputData (VO_CODECBUFFER * pInput)
{
//	VOLOGI ("Input Data Len is %d", pInput->Length);

	m_nInputUsed = 0;
	m_pInputData = pInput;

	if (m_hEffect == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

//	VOLOGI ("Set Input Data size %d Time is %d", pInput->Length, (int)pInput->Time);
	return 	m_funEffect.SetInputData (m_hEffect, pInput);
}

VO_U32 CAudioEffect::GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	VOLOGS ("Get Output Data!");

	voCAutoLock lock (&m_Mutex);

	VO_U32 nRC = m_funEffect.GetOutputData (m_hEffect, pOutput, &m_OutputInfo);
	if (nRC == VO_ERR_NONE)
		memcpy (pAudioFormat, &m_OutputInfo.Format, sizeof (VO_AUDIO_FORMAT));

	m_nInputUsed = m_OutputInfo.InputUsed;

//	nRC = nRC & 0X8000FFFF;

//	VOLOGI ("Output Data Len is %d", pOutput->Length);

	return nRC;
}

VO_U32 CAudioEffect::Flush (void)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nFlush = 1;
	VO_U32 nRC = m_funEffect.SetParam (m_hEffect, VO_PID_COMMON_FLUSH, &nFlush);

	return nRC;
}

VO_U32 CAudioEffect::Enable (VO_BOOL bEnable)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nRC = m_funEffect.SetParam (m_hEffect, VO_PID_EFFDOLBY_DEFAULT, &bEnable);

	return nRC;
}

VO_U32 CAudioEffect::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	if (m_hEffect == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	VO_U32 nRC = m_funEffect.SetParam (m_hEffect, VO_PID_AUDIO_FORMAT, pFormat);

	memcpy (&m_sFormat, pFormat, sizeof (VO_AUDIO_FORMAT));

	return nRC;
}

VO_U32 CAudioEffect::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hEffect == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funEffect.SetParam (m_hEffect, uParamID, pData);
}

VO_U32 CAudioEffect::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hEffect == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funEffect.GetParam (m_hEffect, uParamID, pData);
}

VO_U32 CAudioEffect::GetFormat (VO_AUDIO_FORMAT * pFormat)
{
	int nRC = GetParam(VO_PID_AUDIO_FORMAT, pFormat);
	
	if (pFormat->Channels > 2)
		pFormat->Channels = 2;

	return nRC;
}

VO_U32 CAudioEffect::LoadLib (VO_HANDLE hInst)
{
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	strcpy (m_szCfgItem, "Audio_Effect");
	vostrcpy (m_szDllFile, _T("voDolbyEffect"));
	vostrcpy (m_szAPIName, _T("voGetDolbyEffectAPI"));

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

	//if (pDllFile == NULL || pApiName == NULL)
	//{
	//	VOLOGI ("It could not find audio effect info in config file.");
	//	return 0;
	//}

#if defined _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL)
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
	pAPI (&m_funEffect);

	return 1;
}
