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
#include "CAudioEncoder.h"

#include "voAMRNB.h"
#include "voAMRWB.h"
#include "voAAC.h"
#include "voMP3.h"
#include "voQCELP.h"

#include "voLog.h"

typedef VO_S32 (VO_API * VOGETAUDIODECAPI) (VO_AUDIO_CODECAPI * pDecHandle);

CAudioEncoder::CAudioEncoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hEnc (NULL)
	, m_nCoding (nCoding)
	, m_pInputData (0)
	, m_nInputUsed (0)
{

}

CAudioEncoder::~CAudioEncoder ()
{


	Uninit ();
}

VO_U32 CAudioEncoder::Init (VO_AUDIO_FORMAT * pFormat)
{


	if (m_nCoding > VO_AUDIO_CodingMIDI || m_nCoding < VO_AUDIO_CodingPCM)
	{
		if (m_pError != NULL)
			vostrcpy (m_pError, _T("The audio format is unknown!"));
		return VO_ERR_INVALID_ARG;
	}

	m_OutputInfo.Format.SampleRate = pFormat->SampleRate;
	m_OutputInfo.Format.Channels = pFormat->Channels;
	m_OutputInfo.Format.SampleBits = pFormat->SampleBits;

	VO_CODEC_INIT_USERDATA	initInfo;
	memset (&initInfo, 0, sizeof (VO_CODEC_INIT_USERDATA));
	initInfo.memflag = VO_IMF_USERMEMOPERATOR;
	initInfo.memData = m_pMemOP;
/*
	if (m_pLibOP != NULL)
	{
		initInfo.memflag |= 0X10;
		initInfo.libOperator = m_pLibOP;
	}
*/
	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;
	while (nRC != VO_ERR_NONE)
	{
		Uninit ();

		if (LoadLib (m_hInst) == 0)
		{
			VOLOGE ("LoadLib was failed!");
			return VO_ERR_VOMMP_AudioDecoder;
		}

		nRC = m_funEnc.Init (&m_hEnc, (VO_AUDIO_CODINGTYPE)m_nCoding, &initInfo);
		if (m_hEnc == NULL)
		{
			VOLOGE ("m_funDec.Init was failed! Result 0X%08X", (int)nRC);
			return VO_ERR_VOMMP_AudioDecoder;
		}

		UpdateParam ();

		if (nRC == VO_ERR_NONE)
			break;

		if (m_bVOUsed)
			break;

		m_bVOUsed = VO_TRUE;
	}

	return VO_ERR_NONE;
}

VO_U32 CAudioEncoder::UpdateParam (void)
{


	//VO_U32 nRC = 0;
//	nRC = SetParam (VO_PID_COMMON_LIBOP, m_pLibOP);

	if (m_nCoding == VO_AUDIO_CodingAMRNB)
	{

	}

	SetParam (VO_PID_AUDIO_FORMAT, &m_OutputInfo.Format);

	return 0;
}

VO_U32 CAudioEncoder::Uninit (void)
{


	if (m_hEnc != NULL)
	{
		m_funEnc.Uninit (m_hEnc);
		m_hEnc = NULL;
	}

	FreeLib ();

	return 0;
}

VO_U32 CAudioEncoder::Start(void)
{


	VO_U32 nParam = 0;
	return 	SetParam (VO_PID_COMMON_START, &nParam);
}

VO_U32 CAudioEncoder::Pause(void)
{


	VO_U32 nParam = 0;
	return 	SetParam (VO_PID_COMMON_PAUSE, &nParam);
}

VO_U32 CAudioEncoder::Stop(void)
{


	VO_U32 nParam = 0;
	return 	SetParam (VO_PID_COMMON_STOP, &nParam);
}

VO_U32 CAudioEncoder::SetInputData (VO_CODECBUFFER * pInput)
{
	VOLOGR ();

	m_nInputUsed = 0;
	m_pInputData = pInput;

	if (m_nCoding == VO_AUDIO_CodingPCM)
		return VO_ERR_NONE;

	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	VOLOGR ("Set Input Data size %d!", pInput->Length);

	return 	m_funEnc.SetInputData (m_hEnc, pInput);
}

VO_U32 CAudioEncoder::GetOutputData (VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pAudioInfo)
{
	VOLOGR ();

	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	VOLOGS ("Get Output Data!");

	VO_U32 nRC = m_funEnc.GetOutputData (m_hEnc, pOutput, &m_OutputInfo);

	VOLOGR ("GetPutputData Size %d, Result %d !", pOutput->Length, nRC);

	if (nRC == VO_ERR_NONE)
		memcpy (pAudioInfo, &m_OutputInfo, sizeof (VO_AUDIO_OUTPUTINFO));

	m_nInputUsed = m_OutputInfo.InputUsed;
        
	nRC = nRC & 0X8000FFFF;

	return nRC;
}

VO_U32 CAudioEncoder::Flush (void)
{


	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	VO_U32 nFlush = 1;
	VO_U32 nRC = m_funEnc.SetParam (m_hEnc, VO_PID_COMMON_FLUSH, &nFlush);

	return nRC;
}

VO_U32 CAudioEncoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{


	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funEnc.SetParam (m_hEnc, uParamID, pData);
}

VO_U32 CAudioEncoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{


	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);
	return 	m_funEnc.GetParam (m_hEnc, uParamID, pData);
}

VO_U32 CAudioEncoder::LoadLib (VO_HANDLE hInst)
{


	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_nCoding== VO_AUDIO_CodingMP3)
	{
		strcpy (m_szCfgItem, "Audio_Enc_MP3");
		vostrcpy (m_szDllFile, _T("voMP3Enc"));
		vostrcpy (m_szAPIName, _T("voGetMP3EncAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingAAC)
	{
		strcpy (m_szCfgItem, "Audio_Enc_AAC");
		vostrcpy (m_szDllFile, _T("voAACEnc"));
		vostrcpy (m_szAPIName, _T("voGetAACEncAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingAMRNB)
	{
		strcpy (m_szCfgItem, "Audio_Enc_AMRNB");
		vostrcpy (m_szDllFile, _T("voAMRNBEnc"));
		vostrcpy (m_szAPIName, _T("voGetAMRNBEncAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingQCELP13)
	{
		strcpy (m_szCfgItem, "Audio_Enc_QCELP");
		vostrcpy (m_szDllFile, _T("voQCELPEnc"));
		vostrcpy (m_szAPIName, _T("voGetQCELPEncAPI"));
	}
	else if (m_nCoding== VO_AUDIO_CodingEVRC)
	{
		strcpy (m_szCfgItem, "Audio_Enc_EVRC");
		vostrcpy (m_szDllFile, _T("voEVRCEnc"));
		vostrcpy (m_szAPIName, _T("voGetEVRCEncAPI"));
	}
	else if(m_nCoding == VO_AUDIO_CodingAMRWB)
	{
		strcpy (m_szCfgItem, "Audio_Enc_AMRWB");
		vostrcpy (m_szDllFile, _T("voAMRWBEnc"));
		vostrcpy (m_szAPIName, _T("voGetAMRWBEncAPI"));
	}

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

	VOLOGI ("File %s, API %s .", m_szDllFile, m_szAPIName);

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETAUDIODECAPI pAPI = (VOGETAUDIODECAPI) m_pAPIEntry;
	pAPI (&m_funEnc);

	return 1;
}
