	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFilterOneSource.cpp

	Contains:	CFilterOneSource class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-08-14		JBF			Create file

*******************************************************************************/
#include <streams.h>
#include <tchar.h>

#include "CFilterOneSource.h"
#include "COneOutputPin.h"

#include "FilterOne_Guids.h"

//#include <initguid.h>
/*
#if _WIN32_WCE >= 0x500 && defined WIN32_PLATFORM_PSPC
//#include "IFDRMHandler_Guids.h"
#endif //_WIN32_WCE
*/

#pragma warning (disable : 4996)

// Constructor
CFilterOneSource::CFilterOneSource(LPUNKNOWN pUnk, HRESULT *phr)
	: CBaseFilter(NAME("CFilterOne"), pUnk, &m_csFileSource, CLSID_FilterOneSource)
	, m_pOutput (NULL)
{
	_tcscpy (m_szFile, _T("No File"));

	*phr = S_OK;
}

CFilterOneSource::~CFilterOneSource (void)
{
	if (m_pOutput != NULL)
	{
		delete m_pOutput;
		m_pOutput = NULL;
	}
}

// NonDelegatingQueryInterface
// Override CUnknown method.
// Part of the basic COM (Compound Object Model) mechanism.
// This is how we expose our interfaces.
STDMETHODIMP CFilterOneSource::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv,E_POINTER);
	if (riid == IID_IFileSourceFilter)
	{
		return GetInterface ((IFileSourceFilter *)this, ppv);
	}
/*
#if _WIN32_WCE >= 0x500 && defined WIN32_PLATFORM_PSPC
	else if (riid == IID_IFileDrmHandler)
	{
		return GetInterface((IFileDrmHandler *) this, ppv);
	}
#endif // //_WIN32_WCE >= 0X500
	else
*/
	{
		return CBaseFilter::NonDelegatingQueryInterface(riid, ppv);
	}
} // NonDelegatingQueryInterface

// GetPin
CBasePin * CFilterOneSource::GetPin(int n)
{
	return m_pOutput;
}

// GetPinCount
int CFilterOneSource::GetPinCount()
{
	if (m_pOutput == NULL)
		return 0;

	return 1;
}

// Stop
STDMETHODIMP CFilterOneSource::Stop()
{
	HRESULT hr = CBaseFilter::Stop();

	return hr;
}

// Pause
// Overriden to open the dump file
STDMETHODIMP CFilterOneSource::Pause()
{
	HRESULT hr = CBaseFilter::Pause();

	return hr;
}

// Run
STDMETHODIMP CFilterOneSource::Run(REFERENCE_TIME tStart)
{
    HRESULT hr = CBaseFilter::Run(tStart);

	return hr;
}

STDMETHODIMP CFilterOneSource::Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt)
{
	CAutoLock lock_it(m_pLock);

#ifdef _UNICODE
	_tcscpy(m_szFile, pszFileName);
#else
	WideCharToMultiByte(CP_ACP, 0, pszFileName, -1, m_szFile, sizeof (m_szFile), NULL, NULL);
#endif //_UNICODE

/*
#if _WIN32_WCE >= 0x500 && defined WIN32_PLATFORM_PSPC
	DWORD dwRightsStatus;
	HRESULT hr = FileDrmVerifyRights (NULL, m_szFile, FDRRF_PLAY, FDRVF_NOUI, &dwRightsStatus);
	if (FAILED (hr))
		return S_OK;
#endif // _WIN32_WCE
*/
	if (m_pOutput == NULL)
	{
		HRESULT hr = S_OK;

		m_pOutput = new COneOutputPin (NAME("FileOutputPin"), this, &m_csFileSource, &hr, L"Output");
	}

	return S_OK;

}

STDMETHODIMP CFilterOneSource::GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt)
{
	CAutoLock lock_it(m_pLock);

	int nLen = _tcslen (m_szFile) + 1;

	*ppszFileName = (LPOLESTR) CoTaskMemAlloc (nLen * sizeof (WCHAR));
	memset (*ppszFileName, 0, nLen * sizeof (WCHAR));

#ifdef _UNICODE
	wcscpy (*ppszFileName, m_szFile);
#else
	MultiByteToWideChar (CP_ACP, 0,m_szFile, -1, *ppszFileName, nLen);
#endif //_UNICODE

	pmt = NULL;

	return S_OK;
}

/*
#if _WIN32_WCE >= 0x500 && defined WIN32_PLATFORM_PSPC
STDMETHODIMP CFilterOneSource::IsPlayable(BOOL __RPC_FAR *pfRequestUI)
{
	DWORD dwRightsStatus;
	HRESULT hr = S_OK;//FileDrmVerifyRights (NULL, m_szFile, FDRRF_PLAY, FDRVF_NOUI, &dwRightsStatus);
	if (SUCCEEDED (hr))
		*pfRequestUI = FALSE;	
	else
		*pfRequestUI = TRUE;	

	return hr;
}

STDMETHODIMP CFilterOneSource::HandleError(HWND hwndParent,HRESULT hrError)
{
	DWORD dwStatus = 0;
	HRESULT hr = S_OK;//FileDrmHandleError (hwndParent, hrError, m_szFile, 0, &dwStatus);
	if (dwStatus != FDRHF_RETRY)
		return VFW_E_INVALID_FILE_FORMAT;

	DWORD dwRightsStatus;
//	hr = FileDrmVerifyRights (NULL, m_szFile, FDRRF_PLAY, FDRVF_NOUI, &dwRightsStatus);
	if (SUCCEEDED (hr))
		return Load (m_szFile, NULL);

	return VFW_E_INVALID_FILE_FORMAT;
}
#endif //_WIN32_WCE >= 0X500
*/