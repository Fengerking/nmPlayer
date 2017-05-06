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
#include <stdio.h>
#include "CAudioRender.h"

#include "voLog.h"

typedef VO_S32 (VO_API * VOGETAUDIORENDERAPI) (VO_AUDIO_RENDERAPI * pRender, VO_U32 nFlag);

CAudioRender::CAudioRender (VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hRender (NULL)
	, m_nBuffTime (0)
	, m_nPlayTime (0)
{

}

CAudioRender::~CAudioRender ()
{


	Uninit ();
}

VO_U32 CAudioRender::Init (VO_AUDIO_FORMAT * pFormat)
{


	Uninit ();

	if (LoadLib (m_hInst) == 0)
		return 0;

	VO_U32 nRC = m_funRender.Init (&m_hRender, m_pMemOP, 0);

	SetParam (VO_PID_COMMON_LIBOP, m_pLibOP);
	nRC = m_funRender.SetFormat (m_hRender, pFormat);

	return VO_ERR_NONE;
}

VO_U32 CAudioRender::Uninit (void)
{


	if (m_hRender != NULL)
	{
		m_funRender.Uninit (m_hRender);
		m_hRender = NULL;
	}

	return 0;
}

void CAudioRender::SetLibOperator (VO_LIB_OPERATOR * pLibOP)
{
	m_pLibOP = pLibOP;
	SetParam (VO_PID_COMMON_LIBOP, pLibOP);
}

VO_U32 CAudioRender::SetFormat(VO_AUDIO_FORMAT * pFormat)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funRender.SetFormat (m_hRender, pFormat);

	return nRC;
}

VO_U32 CAudioRender::Start(void)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Start (m_hRender);
}

VO_U32 CAudioRender::Pause(void)
{


	VOLOGI ("VO_STREAM_BUFFERSTART 1111");

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Pause (m_hRender);
}

VO_U32 CAudioRender::Stop(void)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Stop (m_hRender);
}

VO_U32 CAudioRender::Render(VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_funRender.Render (m_hRender, pBuffer, nSize, nStart, VO_FALSE);

	return nRC;
}

VO_U32 CAudioRender::Flush(void)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	m_nPlayTime = 0;

	VO_U32 nRC = m_funRender.Flush (m_hRender);

	return nRC;
}

VO_S64 CAudioRender::GetPlayingTime(void)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	m_funRender.GetPlayingTime (m_hRender, &m_nPlayTime);

	return m_nPlayTime;
}

VO_S32 CAudioRender::GetBufferTime(void)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	m_funRender.GetBufferTime (m_hRender, &m_nBuffTime);

	return m_nBuffTime;
}

VO_U32 CAudioRender::SetCallBack(VOAUDIOCALLBACKPROC pCallBack, VO_PTR pUserData)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.SetCallBack (m_hRender, pCallBack, pUserData);
}

VO_U32 CAudioRender::SetParam(VO_U32 nID, VO_PTR pValue)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.SetParam (m_hRender, nID, pValue);
}

VO_U32 CAudioRender::GetParam(VO_U32 nID, VO_PTR pValue)
{


	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.GetParam (m_hRender, nID, pValue);
}

VO_U32 CAudioRender::LoadLib (VO_HANDLE hInst)
{


#ifdef _LIB
	voGetAudioRenderAPI(&m_funRender, 0);
#else	//_LIB
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText ((char*)"AudioRender", (char*)"RenderFile");
		pApiName = m_pConfig->GetItemText ((char*)"AudioRender", (char*)"RenderApi");
	}

#ifdef _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
		vostrcat (m_szDllFile, _T(".Dll"));
	}
	else
	{
		vostrcpy (m_szDllFile, _T("voMMAR.Dll"));
	}

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
	else
	{
		vostrcpy (m_szAPIName, _T("voGetAudioRenderAPI"));
	}
#elif defined LINUX
	if (pDllFile != NULL)
	{
		vostrcpy (m_szDllFile, pDllFile);
		vostrcat (m_szDllFile, _T(".so"));
	}
	else
		vostrcpy (m_szDllFile, _T("voMMAR.so"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
	else
		vostrcpy (m_szAPIName, _T("voGetAudioRenderAPI"));
#endif


	if (CBaseNode::LoadLib (m_hInst) == 0)
		return 0;

	VOGETAUDIORENDERAPI pAPI = (VOGETAUDIORENDERAPI) m_pAPIEntry;
	pAPI (&m_funRender, 0);
#endif	//_LIB

	return 1;
}
