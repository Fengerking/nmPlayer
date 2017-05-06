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
#include <streams.h>

#include "CVOWPlayer.h"

#pragma warning (disable : 4996)

typedef VO_S32 (VO_API * VOMMGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

CVOWPlayer::CVOWPlayer(void)
	: m_hDll (NULL)
	, m_hPlay (NULL)
{
}

CVOWPlayer::~CVOWPlayer ()
{
	Release ();
}

VO_U32 CVOWPlayer::Init (VO_VOMM_INITPARAM * pParam, VOMMPlayCallBack pCallBack, VO_PTR pUserData)
{
	if (LoadDll () == 0)
		return VO_ERR_NOT_IMPLEMENT;

	m_playFunc.Init (&m_hPlay, pParam);
	if (m_hPlay != NULL)
	   m_playFunc.SetCallBack (m_hPlay, pCallBack, pUserData);

	return VO_ERR_NONE;;
}

VO_U32 CVOWPlayer::SetView (VO_PTR hView, VO_RECT * pRect)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.SetViewInfo (m_hPlay, hView, pRect);
}

VO_U32 CVOWPlayer::Create (VO_PTR pURL, VO_U32 nType)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.Create (m_hPlay, pURL, nType , 0 , 0);
}

VO_U32 CVOWPlayer::Run (void)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.Run (m_hPlay);
}

VO_U32 CVOWPlayer::Pause (void)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.Pause (m_hPlay);
}

VO_U32 CVOWPlayer::Stop (void)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.Stop (m_hPlay);
}

VO_U32 CVOWPlayer::GetDuration (void)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	VO_U32 nDuration = 0;

	m_playFunc.GetDuration (m_hPlay, &nDuration);

	return nDuration;
}

VO_S32 CVOWPlayer::GetCurPos (void)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	VO_S32 nPos = 0;

	m_playFunc.GetCurPos (m_hPlay, &nPos);

	return nPos;
}

VO_U32 CVOWPlayer::SetCurPos (VO_S32 nPos)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.SetCurPos (m_hPlay, nPos);
}

VO_U32 CVOWPlayer::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.SetParam (m_hPlay, nID, pValue);
}

VO_U32 CVOWPlayer::GetParam (VO_U32 nID, VO_PTR pValue)
{
	if (m_hPlay == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	return m_playFunc.GetParam (m_hPlay, nID, pValue);
}

VO_U32 CVOWPlayer::LoadDll (void)
{
	if (m_hDll != NULL)
		FreeLibrary (m_hDll);

	vostrcpy (m_szDllFile, _T("voMMPlay.Dll"));

	VO_TCHAR szDll[MAX_PATH];
	VO_TCHAR szPath[MAX_PATH];
	GetModuleFileName (NULL, szPath, sizeof (szPath));
	VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
	*(pPos + 1) = 0;

	vostrcpy (szDll, szPath);
	vostrcat (szDll, m_szDllFile);
	m_hDll = LoadLibrary (szDll);

	if (m_hDll == NULL)
	{
		vostrcpy (szDll, szPath);
		vostrcat (szDll, _T("Dll\\"));
		vostrcat (szDll, m_szDllFile);
		m_hDll = LoadLibrary (szDll);
	}

	if (m_hDll == NULL)
	{
		m_hDll = LoadLibrary (m_szDllFile);
	}

	if (m_hDll == NULL)
		return 0;

#ifdef _WIN32_WCE
	VOMMGETAPI pAPI = (VOMMGETAPI) GetProcAddress (m_hDll, _T("vommGetPlayAPI"));
#else
	VOMMGETAPI pAPI = (VOMMGETAPI) GetProcAddress (m_hDll, ("vommGetPlayAPI"));
#endif //_WIN32_WCE
	pAPI (&m_playFunc, 0);

	return 1;
}

VO_U32 CVOWPlayer::Release (void)
{
	if (m_hPlay != NULL)
	{
		m_playFunc.Uninit (m_hPlay);
		m_hPlay = NULL;
	}

	if (m_hDll != NULL)
		FreeLibrary (m_hDll);

	m_hDll = NULL;

	return 1;
}
