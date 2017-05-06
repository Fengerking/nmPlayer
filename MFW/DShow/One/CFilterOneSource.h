	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFilterOneSource.h

	Contains:	CFilterOneSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-08-14		JBF			Create file

*******************************************************************************/
#ifndef __CFilterOneSource_H__
#define __CFilterOneSource_H__

/*
#if _WIN32_WCE >= 0x500 && defined WIN32_PLATFORM_PSPC
#include "FDRMDSHW.h"
#include "FDRM.h"
#endif //_WIN32_WCE
*/

class COneOutputPin;

class CFilterOneSource : public CBaseFilter
						, public IFileSourceFilter
//#if _WIN32_WCE >= 0x500 && defined WIN32_PLATFORM_PSPC
//						, public IFileDrmHandler
//#endif //_WIN32_WCE
{
public:
	DECLARE_IUNKNOWN;

	// Constructor
	CFilterOneSource(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~CFilterOneSource (void);

	// Basic COM - used here to reveal our property interface.
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	// Pin enumeration
	CBasePin * GetPin(int n);
	int GetPinCount();

	// Open and close the file as necessary
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause();
	STDMETHODIMP Stop();

	// handle IFileSourceFilter interface
	virtual STDMETHODIMP Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt);
    virtual STDMETHODIMP GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt);
/*
#if _WIN32_WCE >= 0x500 && defined WIN32_PLATFORM_PSPC
	virtual STDMETHODIMP IsPlayable(BOOL __RPC_FAR *pfRequestUI);
	virtual STDMETHODIMP HandleError(HWND hwndParent,HRESULT hrError);
#endif //_WIN32_WCE >= 0X500
*/

protected:
	CCritSec			m_csFileSource;
	COneOutputPin *		m_pOutput;

	TCHAR				m_szFile[MAX_PATH];
};

#endif //__CFilterOneSource_H__