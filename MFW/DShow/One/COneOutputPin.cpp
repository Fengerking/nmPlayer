	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFilterTee.cpp

	Contains:	CFilterTee class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-08-14		JBF			Create file

*******************************************************************************/
#include <streams.h>
#include <tchar.h>
#include <stdio.h>

#include "COneOutputPin.h"
#include "FilterOne_Guids.h"

#pragma warning (disable : 4996)


COneOutputPin::COneOutputPin(TCHAR *pName, CBaseFilter * pSource, CCritSec * pCSFile, HRESULT *phr, LPCWSTR pPinName)
	: CBasePin(pName, pSource, pCSFile, phr, pPinName, PINDIR_OUTPUT)
{
}

// COneOutputPin destructor
COneOutputPin::~COneOutputPin()
{
}


// NonDelegatingQueryInterface
STDMETHODIMP COneOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);
	ASSERT(ppv);
	*ppv = NULL;

	if (riid == IID_IMediaSeeking || riid == IID_IMediaPosition)
	{
		int i = 0;
	}

	return CBasePin::NonDelegatingQueryInterface(riid, ppv);
} // NonDelegatingQueryInterface

STDMETHODIMP COneOutputPin::Connect(IPin * pReceivePin, const AM_MEDIA_TYPE *pmt)
{
	HRESULT hr = CBasePin::Connect (pReceivePin, pmt);

	return hr;
}

HRESULT COneOutputPin::CheckConnect(IPin * pPin)
{
	CAutoLock lock_it(m_pLock);

	HRESULT hr = CBasePin::CheckConnect(pPin);
	if (FAILED(hr))
		return hr;

	return NOERROR;
}

// CheckMediaType
HRESULT COneOutputPin::CheckMediaType(const CMediaType *pmt)
{
	return S_OK;
} // CheckMediaType


HRESULT COneOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	CAutoLock lock_it(m_pLock);

	if (iPosition != 0)
		return VFW_S_NO_MORE_ITEMS;

	pMediaType->InitMediaType ();

	pMediaType->majortype = MEDIATYPE_Stream;
	pMediaType->subtype = MEDIASUBTYPE_VOONE;

	return S_OK;
}

// SetMediaType
HRESULT COneOutputPin::SetMediaType(const CMediaType *pmt)
{
	CAutoLock lock_it(m_pLock);

	// Make sure that the base class likes it
	HRESULT hr = NOERROR;
	hr = CBasePin::SetMediaType(pmt);
	if(FAILED(hr))
		return hr;

	return NOERROR;
} // SetMediaType

// CompleteConnect
HRESULT COneOutputPin::CompleteConnect(IPin *pReceivePin)
{
	CAutoLock lock_it(m_pLock);
	ASSERT(m_Connected == pReceivePin);

	HRESULT	hr = CBasePin::CompleteConnect (pReceivePin);

	return hr;
} // CompleteConnect

// BreakConnect
HRESULT COneOutputPin::BreakConnect()
{
	CAutoLock lock_it(m_pLock);

	HRESULT hr = CBasePin::BreakConnect ();

	return hr;
} // BreakConnect

HRESULT COneOutputPin::Active(void)
{
	CAutoLock lock_it(m_pLock);

	HRESULT hr = CBasePin::Active();

	return S_OK;
}

HRESULT COneOutputPin::Inactive(void)
{
	CAutoLock lock_it(m_pLock);

	HRESULT hr = CBasePin::Inactive();

	return S_OK;
}

STDMETHODIMP COneOutputPin::BeginFlush( void)
{
	return S_OK;
}

STDMETHODIMP COneOutputPin::EndFlush( void)
{
	return S_OK;
}