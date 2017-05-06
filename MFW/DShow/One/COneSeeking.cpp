	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COneInputPin.cpp

	Contains:	COneInputPin class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-12-02		JBF			Create file

*******************************************************************************/
#include <streams.h>
#include <tchar.h>
#include <stdio.h>

#include "COneSeeking.h"

#include "CFilterOneRender.h"
#include "CVOWPlayer.h"

COneSeeking::COneSeeking(CFilterOneRender * pRender, const TCHAR *pName, LPUNKNOWN pUnk, HRESULT *phr)
	: CMediaPosition(pName, pUnk)
	, m_pRender (pRender)
	, m_llDuration (0)
{
	m_dwCaps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanSeekForwards | AM_SEEKING_CanSeekBackwards |
			   AM_SEEKING_CanGetCurrentPos | AM_SEEKING_CanGetStopPos | AM_SEEKING_CanGetDuration;
}

COneSeeking::~COneSeeking (void)
{
}

// Expose our IMediaSeeking and IMediaPosition interfaces
STDMETHODIMP COneSeeking::NonDelegatingQueryInterface(REFIID riid,void **ppv)
{
	CheckPointer(ppv,E_POINTER);
	*ppv = NULL;

	if (riid == IID_IMediaSeeking)
	{
		return GetInterface( static_cast<IMediaSeeking *>(this), ppv);
	}

	return CMediaPosition::NonDelegatingQueryInterface(riid,ppv);
}


// --- IMediaSeeking methods ----------
STDMETHODIMP COneSeeking::GetCapabilities(DWORD * pCaps)
{
	*pCaps = m_dwCaps;

    return S_OK;
}

STDMETHODIMP COneSeeking::CheckCapabilities(DWORD * pCaps)
{
	if (*pCaps == m_dwCaps)
		return S_OK;
	else if ((*pCaps & m_dwCaps) > 0)
		return S_FALSE;
	else
		return E_FAIL;
}

STDMETHODIMP COneSeeking::IsFormatSupported(const GUID * pFormat)
{
	if (*pFormat == TIME_FORMAT_MEDIA_TIME)
		return S_OK;
	else
		return S_FALSE;
}


STDMETHODIMP COneSeeking::QueryPreferredFormat(GUID *pFormat)
{
	*pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}


STDMETHODIMP COneSeeking::SetTimeFormat(const GUID * pFormat)
{
	if (*pFormat == TIME_FORMAT_MEDIA_TIME)
		return S_OK;
	else
		return E_FAIL;
}


STDMETHODIMP COneSeeking::GetTimeFormat(GUID *pFormat)
{
	*pFormat = TIME_FORMAT_MEDIA_TIME;
    return S_OK;
}


STDMETHODIMP COneSeeking::IsUsingTimeFormat(const GUID * pFormat)
{
	if (*pFormat == TIME_FORMAT_MEDIA_TIME)
		return S_OK;
	else
		return S_FALSE;
}


STDMETHODIMP COneSeeking::ConvertTimeFormat(LONGLONG * pTarget, const GUID * pTargetFormat,
											LONGLONG    Source, const GUID * pSourceFormat )
{
    return E_NOTIMPL;
}


STDMETHODIMP COneSeeking::SetPositions( LONGLONG * pCurrent, DWORD CurrentFlags, 
										LONGLONG * pStop, DWORD StopFlags )
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	LONGLONG llPos = 0;

	if (CurrentFlags & AM_SEEKING_AbsolutePositioning)
	{
		llPos = *pCurrent;
	}
	else if (CurrentFlags & AM_SEEKING_RelativePositioning)
	{
		llPos = m_pPlayer->GetCurPos ();
		llPos = llPos * 10000 + *pCurrent;
	}
	else
	{
		return E_NOTIMPL;
	}

	if (m_llDuration == 0)
		GetDuration (&m_llDuration);

	if (llPos >= m_llDuration)
		llPos = 0;
	else
		llPos = llPos / 10000;

	m_pPlayer->SetCurPos ((VO_U32)llPos);

	*pCurrent = llPos * 10000;

    return S_OK;
}

STDMETHODIMP COneSeeking::GetPositions(LONGLONG *pCurrent, LONGLONG * pStop)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	*pCurrent = m_pPlayer->GetCurPos ();
	*pCurrent = *pCurrent * 10000;

	*pStop = m_pPlayer->GetDuration ();
	*pStop = *pStop * 10000;

	return S_OK;
}

// If we don't have a current position then ask upstream
STDMETHODIMP COneSeeking::GetCurrentPosition(LONGLONG *pCurrent)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	*pCurrent = m_pPlayer->GetCurPos ();
	*pCurrent = *pCurrent * 10000;

	return S_OK;
}


STDMETHODIMP COneSeeking::GetStopPosition(LONGLONG *pStop)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	*pStop = m_pPlayer->GetDuration ();
	*pStop = *pStop * 10000;

	return S_OK;
}

STDMETHODIMP COneSeeking::GetDuration(LONGLONG *pDuration)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	*pDuration = m_pPlayer->GetDuration ();
	*pDuration = *pDuration * 10000;

	return S_OK;
}


STDMETHODIMP COneSeeking::GetPreroll(LONGLONG *pllPreroll)
{
	*pllPreroll = 0;

    return S_OK;
}


STDMETHODIMP COneSeeking::GetAvailable( LONGLONG *pEarliest, LONGLONG *pLatest )
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	//Do not assign a value to *pLatest in COneSeeking::GetAvailable function , 
	//because this will call data abort in WM6.5 version
	//*pLatest = m_pPlayer->GetDuration ();
	//*pLatest = *pLatest * 10000;

	return S_OK;
}


STDMETHODIMP COneSeeking::GetRate(double * pdRate)
{
	*pdRate = 1.0;

    return S_OK;
}


STDMETHODIMP COneSeeking::SetRate(double dRate)
{
    return E_NOTIMPL;
}

// --- IMediaPosition methods ----------
STDMETHODIMP COneSeeking::get_Duration(REFTIME * plength)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	*plength = m_pPlayer->GetDuration ();
	*plength = *plength * 10000;

	return S_OK;
}


STDMETHODIMP COneSeeking::get_CurrentPosition(REFTIME * pllTime)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	*pllTime = m_pPlayer->GetCurPos ();
	*pllTime = *pllTime * 10000;

	return S_OK;
}


STDMETHODIMP COneSeeking::put_CurrentPosition(REFTIME llTime)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	m_pPlayer->SetCurPos ((VO_U32)(llTime / 10000));

	return S_OK;
}


STDMETHODIMP COneSeeking::get_StopTime(REFTIME * pllTime)
{
	m_pPlayer = m_pRender->GetPlayer ();
	if (m_pPlayer == NULL)
		return E_NOTIMPL;

	*pllTime = m_pPlayer->GetDuration ();
	*pllTime = *pllTime * 10000;

	return S_OK;
}


STDMETHODIMP COneSeeking::put_StopTime(REFTIME llTime)
{
    return E_NOTIMPL;
}


STDMETHODIMP COneSeeking::get_PrerollTime(REFTIME * pllTime)
{
	*pllTime = 0;

    return S_OK;
}


STDMETHODIMP COneSeeking::put_PrerollTime(REFTIME llTime)
{
    return E_NOTIMPL;
}


STDMETHODIMP COneSeeking::get_Rate(double * pdRate)
{
	*pdRate = 1.0;

    return S_OK;
}


STDMETHODIMP COneSeeking::put_Rate(double dRate)
{
    return E_NOTIMPL;
}


STDMETHODIMP COneSeeking::CanSeekForward(LONG *pCanSeekForward)
{
    return S_FALSE;
}


STDMETHODIMP COneSeeking::CanSeekBackward(LONG *pCanSeekBackward)
{
    return S_FALSE;
}