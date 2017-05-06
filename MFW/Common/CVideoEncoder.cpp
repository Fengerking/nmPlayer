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
#include "CVideoEncoder.h"

#define LOG_TAG "CVideoEncoder"
#include "voLog.h"

typedef VO_S32 (VO_API * VOGETVIDEOENCAPI) (VO_VIDEO_ENCAPI * pDecHandle);

CVideoEncoder::CVideoEncoder (VO_PTR hInst, VO_U32 nCoding, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hEnc (NULL)
	, m_nCoding (nCoding)
{
}

CVideoEncoder::~CVideoEncoder ()
{
	Uninit ();
}

VO_U32 CVideoEncoder::Init ()
{
	if (m_nCoding > VO_VIDEO_CodingDIVX || m_nCoding < VO_VIDEO_CodingMPEG2)
	{
		if (m_pError != NULL)
			vostrcpy (m_pError, _T("The video format is unknown!"));
		return VO_ERR_INVALID_ARG;
	}

	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;
	while (nRC != VO_ERR_NONE)
	{
		Uninit ();

		if (LoadLib (m_hInst) == 0)
		{
			VOLOGE ("LoadLib (m_hInst) Failed.");
			return VO_ERR_WRONG_STATUS;
		}

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
		nRC = m_funEnc.Init (&m_hEnc, (VO_VIDEO_CODINGTYPE)m_nCoding, &initInfo);
		if (nRC != VO_ERR_NONE || m_hEnc == NULL)
		{
			VOLOGE ("m_funEnc.Init Failed. 0X%08X", (int)nRC);
			return VO_ERR_NOT_IMPLEMENT;
		}

		if (nRC == VO_ERR_NONE)
			break;

		if (m_bVOUsed)
			break;

		m_bVOUsed = VO_TRUE;
	}

	return nRC;
}

VO_U32 CVideoEncoder::Uninit (void)
{
	if (m_hEnc != NULL)
	{
		m_funEnc.Uninit (m_hEnc);
		m_hEnc = NULL;
	}

	FreeLib ();

	return 0;
}

VO_U32 CVideoEncoder::Start(void)
{
	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_START, &nValue);
}

VO_U32 CVideoEncoder::Pause(void)
{
	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_PAUSE, &nValue);
}

VO_U32 CVideoEncoder::Stop(void)
{
	VO_U32	nValue = 0;
	return 	SetParam (VO_PID_COMMON_STOP, &nValue);
}

VO_U32 CVideoEncoder::Process (VO_VIDEO_BUFFER * pInput, VO_CODECBUFFER * pOutput, VO_VIDEO_FRAMETYPE * pType)
{
	if (m_hEnc == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	return  m_funEnc.Process (m_hEnc, pInput, pOutput, pType);
}

VO_U32 CVideoEncoder::SetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	return 	m_funEnc.SetParam (m_hEnc, uParamID, pData);
}

VO_U32 CVideoEncoder::GetParam (VO_S32 uParamID, VO_PTR pData)
{
	if (m_hEnc == 0)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock (&m_Mutex);

	return 	m_funEnc.GetParam (m_hEnc, uParamID, pData);
}

VO_U32 CVideoEncoder::LoadLib (VO_HANDLE hInst)
{
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_nCoding== VO_VIDEO_CodingMPEG4)
	{
		strcpy (m_szCfgItem, "Video_Enc_MPEG4");
		vostrcpy (m_szDllFile, _T("voMPEG4Enc"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4EncAPI"));
	}
	else if (m_nCoding== VO_VIDEO_CodingH263)
	{
		strcpy (m_szCfgItem, "Video_Enc_H263");
		vostrcpy (m_szDllFile, _T("voMPEG4Enc"));
		vostrcpy (m_szAPIName, _T("voGetMPEG4EncAPI"));
	}
	else if(m_nCoding == VO_VIDEO_CodingH264)
	{
		strcpy (m_szCfgItem, "Video_Enc_H264");
		vostrcpy (m_szDllFile, _T("voH264Enc"));
		vostrcpy (m_szAPIName, _T("voGetH264EncAPI"));
	}

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText (m_szCfgItem, (char*)"File");
		pApiName = m_pConfig->GetItemText (m_szCfgItem, (char*)"Api");
	}
	if (pDllFile == NULL)
	{
		m_bVOUsed = VO_TRUE;
	}

#if defined _WIN32
	if (pDllFile != NULL && !m_bVOUsed)
	{
		TCHAR voDllFile[256];
		_tcscpy (voDllFile, m_szDllFile);

		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));

		if (!_tcscmp (voDllFile, m_szDllFile))
			m_bVOUsed = VO_TRUE;
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL && !m_bVOUsed)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL && !m_bVOUsed)
	{
		if (!vostrcmp (m_szDllFile, pDllFile))
			m_bVOUsed = VO_TRUE;

		vostrcpy (m_szDllFile, pDllFile);
	}
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL && !m_bVOUsed)
		vostrcpy (m_szAPIName, pApiName);
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETVIDEOENCAPI pAPI = (VOGETVIDEOENCAPI) m_pAPIEntry;
	pAPI (&m_funEnc);

	return 1;
}
