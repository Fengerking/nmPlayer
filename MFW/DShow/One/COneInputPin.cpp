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

#include "CFilterOneRender.h"
#include "COneInputPin.h"

#include "FilterOne_Guids.h"

/////////////////////////////////////////////////////////////////////////////////////
COneInputPin::COneInputPin (TCHAR * pObjectName, CFilterOneRender * pRenderer,
								CCritSec * pInterfaceLock, HRESULT * phr, LPCWSTR pPinName)
	: CBaseInputPin(pObjectName, pRenderer, pInterfaceLock, phr, pPinName)
	, m_pRender (pRenderer)
{
}

COneInputPin::~COneInputPin ()
{
}

// NonDelegatingQueryInterface
STDMETHODIMP COneInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
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


HRESULT COneInputPin::CheckMediaType (const CMediaType * pmt)
{
	if (pmt->subtype == MEDIASUBTYPE_VOONE)
		return S_OK;

	return VFW_E_INVALID_MEDIA_TYPE;
}

HRESULT COneInputPin::BreakConnect ()
{
	HRESULT hr = S_OK;

	m_pRender->Close ();

	hr = CBaseInputPin::BreakConnect  ();

	return hr;
}

HRESULT COneInputPin::CompleteConnect (IPin * pReceivePin)
{
	if (pReceivePin == NULL)
		return E_POINTER;

	HRESULT hr = S_OK;

	PIN_INFO pinInfo;

	hr = pReceivePin->QueryPinInfo (&pinInfo);

	IFileSourceFilter * pFileSource = NULL;
	hr = pinInfo.pFilter->QueryInterface (IID_IFileSourceFilter, (void**)&pFileSource);
	pinInfo.pFilter->Release ();

	if (pFileSource == NULL)
		return E_POINTER;

	TCHAR * pFileName = NULL;
	hr = pFileSource->GetCurFile (&pFileName, NULL);

	hr = m_pRender->OpenSource (pFileName);
	CoTaskMemFree (pFileName);

	if (FAILED (hr))
		return hr;

	hr = CBaseInputPin::CompleteConnect  (pReceivePin);

	return hr;
}

HRESULT COneInputPin::Inactive(void)
{
	return S_OK;
}
