	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFilterOneRender.cpp

	Contains:	CFilterOneRender class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-08-14		JBF			Create file

*******************************************************************************/
#include <streams.h>
#include <tchar.h>

#include "CFilterOneRender.h"
#include "COneInputPin.h"

#include "FilterOne_Guids.h"

#pragma warning (disable : 4996)

// Constructor
CFilterOneRender::CFilterOneRender(LPUNKNOWN pUnk, HRESULT *phr)
	: CBaseFilter(NAME("CFilterOneRender"), pUnk, &m_csRender, CLSID_FilterOneRender)
	, m_pInput (NULL)
	, m_pWndVideo (NULL)
	, m_pSeeking (NULL)
	, m_pPlayer (NULL)
	, m_bVideo (FALSE)
#ifdef _THREAD_IDEL
	, m_hThread (NULL)
	, m_bIdleStop (VO_TRUE)
#endif // _THREAD_IDEL
{
	HRESULT hr = S_OK;

	m_pInput = new COneInputPin (NAME("RenderInputPin"), this, &m_csRender, &hr, L"Input");
	m_pWndVideo = new CVideoWindow (NAME("VOW Window"), GetOwner (), phr, &m_csRender, this);

	*phr = S_OK;
}

CFilterOneRender::~CFilterOneRender (void)
{
	if (m_pSeeking != NULL)
		delete m_pSeeking;
	m_pSeeking = NULL;

	if (m_pInput != NULL)
		delete m_pInput;
	m_pInput = NULL;

	if (m_pPlayer != NULL)
		delete m_pPlayer;
	m_pPlayer = NULL;

	if (m_pWndVideo != NULL)
		delete m_pWndVideo;
	m_pWndVideo = NULL;
}

// NonDelegatingQueryInterface
// Override CUnknown method.
// Part of the basic COM (Compound Object Model) mechanism.
// This is how we expose our interfaces.
STDMETHODIMP CFilterOneRender::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);

	if (riid == IID_IVideoWindow) 
	{
		if (m_pWndVideo != NULL && m_bVideo)
			return m_pWndVideo->NonDelegatingQueryInterface(riid,ppv);
	} 
	else if (riid == IID_IBasicVideo) 
	{
		if (m_pWndVideo != NULL && m_bVideo)
			return m_pWndVideo->NonDelegatingQueryInterface(riid,ppv);
	}
	else if (riid == IID_IMediaSeeking || riid == IID_IMediaPosition)
	{
		if (m_pSeeking == NULL)
		{
			HRESULT hr = S_OK;
			m_pSeeking = new COneSeeking (this, NAME("OneSeeking"), NULL, &hr);
			m_pSeeking->AddRef ();
		}

		return GetInterface( static_cast<IMediaSeeking *>(m_pSeeking), ppv);
	}

	return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
} // NonDelegatingQueryInterface

// GetPin
CBasePin * CFilterOneRender::GetPin(int n)
{
	return m_pInput;
}

// GetPinCount
int CFilterOneRender::GetPinCount()
{
	if (m_pInput == NULL)
		return 0;

	return 1;
}

// Stop
STDMETHODIMP CFilterOneRender::Stop()
{
	HRESULT hr = CBaseFilter::Stop();

	if (m_pPlayer != NULL)
		m_pPlayer->Stop ();

#ifdef _THREAD_IDEL
	m_bIdleStop = VO_TRUE;
	Sleep (100);
#endif // _THREAD_IDEL

	return S_OK;
}

// Pause
// Overriden to open the dump file
STDMETHODIMP CFilterOneRender::Pause()
{
	HRESULT hr = CBaseFilter::Pause();
	
	if (m_pPlayer != NULL)
		m_pPlayer->Pause ();

	return hr;
}

// Run
STDMETHODIMP CFilterOneRender::Run(REFERENCE_TIME tStart)
{
    HRESULT hr = CBaseFilter::Run(tStart);

	if (m_pPlayer != NULL)
		m_pPlayer->Run ();

#ifdef _THREAD_IDEL
	DWORD dwThreadID = 0;
	m_bIdleStop = VO_FALSE;
	if (m_hThread == NULL)
		m_hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) PlaybackIdleThreadProc, this, 0, &dwThreadID);
#endif // _THREAD_IDEL

	return hr;
}

STDMETHODIMP CFilterOneRender::SetParam(int nID, int nValue)
{
	return E_FAIL;
}

STDMETHODIMP CFilterOneRender::GetParam(int nID, int * pValue)
{
	return E_FAIL;
}

HRESULT CFilterOneRender::OpenSource (TCHAR * pSource)
{
	if (m_pPlayer != NULL)
	{
		m_pPlayer->Stop ();
		delete m_pPlayer;
	}

	m_pPlayer = new CVOWPlayer ();
	if (m_pPlayer == NULL)
		return E_FAIL;

	HWND hWnd = NULL;
	if (m_pWndVideo != NULL)
		hWnd = m_pWndVideo->GetWindowHWND ();

	VO_VOMM_INITPARAM initParam;
	initParam.nFlag = 0;
	initParam.pFileOP = NULL;
	initParam.pMemOP = NULL;

	m_pPlayer->Init (&initParam, vowPlayCallBackProc, this);

	RECT rcView;
	GetClientRect (hWnd, &rcView);
	m_pPlayer->SetView (hWnd, (VO_RECT *) &rcView);

	VO_U32 nRC = m_pPlayer->Create (pSource, VO_VOMM_CREATE_SOURCE_URL);
	if (nRC != VO_ERR_NONE)
		return E_FAIL;

	VO_VIDEO_FORMAT fmtVideo;
	m_pPlayer->GetParam (VO_VOMMP_PMID_VideoFormat, &fmtVideo);
	if (fmtVideo.Width != 0 && fmtVideo.Height != 0)
		m_bVideo = TRUE;
	else
		m_bVideo = FALSE;

	if (m_bVideo)
	{
		m_pWndVideo->put_Visible (OATRUE);
		Resize ();
	}
	else
	{
		m_pWndVideo->put_Visible (OAFALSE);
	}

	return S_OK;;
}

HRESULT CFilterOneRender::Close (void)
{
	if (m_pPlayer != NULL)
	{
		m_pPlayer->Stop ();
		delete m_pPlayer;
	}
	m_pPlayer = NULL;

	return S_OK;
}

HRESULT CFilterOneRender::Resize (void)
{
	if (m_pPlayer != NULL && m_pWndVideo != NULL)
	{
		RECT rcView;
		GetClientRect (m_pWndVideo->GetWindowHWND (), &rcView);

		m_pPlayer->SetParam (VO_VOMMP_PMID_Redraw, (VO_PTR)&rcView);
	}

	return S_OK;
}

VO_U32 CFilterOneRender::vowPlayCallBackProc (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2)
{
	CFilterOneRender * pRender = (CFilterOneRender *)pUserData;

	if (nID == VO_VOMMP_CBID_EndOfFile)
	{
		pRender->NotifyEvent (EC_COMPLETE, S_OK, (LONG_PTR)pUserData);
	}

	return 0;
}

#ifdef _THREAD_IDEL
VO_U32 CFilterOneRender::PlaybackIdleThreadProc (VO_PTR pParam)
{
	CFilterOneRender * pRender = (CFilterOneRender *)pParam;
	pRender->PlaybackIdleThreadLoop ();

	return 0;
}

VO_U32 CFilterOneRender::PlaybackIdleThreadLoop (void)
{
	SetThreadPriority (GetCurrentThread (), THREAD_PRIORITY_IDLE);

	int nValue = 0;

	while (!m_bIdleStop)
	{
		nValue = 2;
	}

	m_hThread = NULL;

	return 0;
}
#endif // _THREAD_IDEL