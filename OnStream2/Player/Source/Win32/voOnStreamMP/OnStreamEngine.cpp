/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#include "OnStreamEngine.h"

OnStreamEngine::OnStreamEngine(int nPlayerType, void* pInitParam, int nInitParamFlag)
	: m_hDll (NULL)
	, m_hPlay (NULL)
	, m_hWnd (NULL)
	, m_pLibop(NULL)
{
	if((nInitParamFlag | VOOSMP_FLAG_INIT_LIBOP) != 0 )
	{
		m_pLibop = (VOOSMP_LIB_FUNC *)((VOOSMP_INIT_PARAM *)pInitParam)->pLibOP;
	}

	memset(&m_pPlayerAPI, 0, sizeof(voOnStreamEngnAPI));
	memset(&m_rcDraw, 0, sizeof(VOOSMP_RECT));
	LoadDll ();

	Init(nPlayerType , pInitParam , nInitParamFlag);
}

OnStreamEngine::~OnStreamEngine ()
{
	Uninit();

	if (m_hDll != NULL && m_pLibop != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
	}
}

int	OnStreamEngine::Init(int nPlayerType, void* pInitParam, int nInitParamFlag)
{
	if (m_pPlayerAPI.Init == NULL)
		return VOOSMP_ERR_Pointer;
	
	Uninit();

	int nRC = m_pPlayerAPI.Init(&m_hPlay, nPlayerType, pInitParam, nInitParamFlag);

   return nRC;
}

int	OnStreamEngine::Uninit()
{
	if (m_hPlay == NULL || m_pPlayerAPI.Uninit == NULL)
		return VOOSMP_ERR_Pointer;

	 m_pPlayerAPI.Uninit(m_hPlay);
	 m_hPlay = NULL;

	return 0;
}

int OnStreamEngine::SetView(void* pView)
{
	m_hWnd = (HWND)pView;

	if (m_hPlay != NULL || m_pPlayerAPI.SetView != NULL)
		m_pPlayerAPI.SetView(m_hPlay, (void*) m_hWnd);

	return 0;
}

int OnStreamEngine::SetDataSource (void * pSource, int nFlag)
{
	if (m_hPlay == NULL || m_pPlayerAPI.Open == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.Open (m_hPlay, pSource, nFlag);
}

int OnStreamEngine::Run (void)
{
	if (m_hPlay == NULL || m_pPlayerAPI.Run == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.Run (m_hPlay);
}

int OnStreamEngine::Pause (void)
{
	if (m_hPlay == NULL || m_pPlayerAPI.Pause == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.Pause (m_hPlay);
}

int OnStreamEngine::Stop (void)
{
	if (m_hPlay == NULL || m_pPlayerAPI.Stop == NULL)
		return VOOSMP_ERR_None;

	return m_pPlayerAPI.Stop(m_hPlay);
}

int OnStreamEngine::GetStatus (VOOSMP_STATUS * pStatus)
{
	if (m_hPlay == NULL || m_pPlayerAPI.GetParam == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.GetParam(m_hPlay, VOOSMP_PID_STATUS, (void *)pStatus);
}

int OnStreamEngine::GetCurPos (int * pCurPos)
{
	if (m_hPlay == NULL || m_pPlayerAPI.GetPos == NULL)
		return VOOSMP_ERR_Pointer;

	*pCurPos = m_pPlayerAPI.GetPos(m_hPlay);

	return VOOSMP_ERR_None;
}

int OnStreamEngine::SetCurPos (int nCurPos)
{
	if (m_hPlay == NULL || m_pPlayerAPI.SetPos == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.SetPos(m_hPlay, nCurPos);
}

int	OnStreamEngine::GetSubtitleSample(void* pHandle, voSubtitleInfo* pSample)
{
	if (m_hPlay == NULL || m_pPlayerAPI.GetSubtitleSample == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.GetSubtitleSample(m_hPlay , pSample);
}

int	OnStreamEngine::SelectLanguage(void* pHandle, int nIndex)
{
	if (m_hPlay == NULL || m_pPlayerAPI.SelectLanguage == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.SelectLanguage(pHandle , nIndex);
}

int	OnStreamEngine::GetLanguage(void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo)
{
	if (m_hPlay == NULL || m_pPlayerAPI.GetLanguage == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.GetLanguage(pHandle , ppLangInfo);
}

 int OnStreamEngine::GetSEI(void* pHandle, VOOSMP_SEI_INFO* pInfo)
 {
	 if (m_hPlay == NULL || m_pPlayerAPI.GetSEI == NULL)
		 return VOOSMP_ERR_Pointer;

	 return m_pPlayerAPI.GetSEI(pHandle , pInfo);
 }

int OnStreamEngine::GetParam (int nID, void * pValue)
{
	if (m_hPlay == NULL || m_pPlayerAPI.GetParam == NULL || NULL==pValue)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.GetParam(m_hPlay, nID, pValue);
}

int OnStreamEngine::SetParam (int nID, void * pValue)
{
  if (m_hPlay == NULL || m_pPlayerAPI.SetParam == NULL || NULL==pValue)
    return VOOSMP_ERR_Pointer;

	if(nID == VOOSMP_PID_DRAW_RECT)
	{
		VOOSMP_RECT *pRect = (VOOSMP_RECT *)pValue;

		m_rcDraw.nLeft = pRect->nLeft;	
		m_rcDraw.nTop = pRect->nTop;	
		m_rcDraw.nRight = pRect->nRight;	
		m_rcDraw.nBottom = pRect->nBottom;
	}

	if (m_hPlay == NULL || m_pPlayerAPI.SetParam == NULL)
		return VOOSMP_ERR_Pointer;

	return m_pPlayerAPI.SetParam(m_hPlay, nID, pValue);
}

int OnStreamEngine::LoadDll (void)
{
	if(m_pLibop == NULL)
		return -1;
	
	if (m_hDll != NULL)
	{
		m_pLibop->FreeLib(m_pLibop->pUserData, m_hDll, 0);
		m_hDll = NULL;
	}

	m_hDll = (HMODULE)m_pLibop->LoadLib(m_pLibop->pUserData, "voOSEng", 0);

	if (m_hDll == NULL)
		return -1;

	VOGETONSTREAMENGNAPI pGetAPI = (VOGETONSTREAMENGNAPI) m_pLibop->GetAddress (m_pLibop->pUserData, m_hDll, "voGetOnStreamSDKAPI", 0);

	if (pGetAPI == NULL)
		return -1;

	pGetAPI(&m_pPlayerAPI);

	if(m_pPlayerAPI.Init == NULL)
		return -1;

	return 0;
}
