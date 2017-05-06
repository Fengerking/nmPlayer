#pragma once

#include "streams.h"
#include "Qnetwork.h"

class CBaseFileParser;
class CMediaSeeking;
class CFilterSource :public CSource  , public IFileSourceFilter , public IAMNetworkStatus
{
public:
	CFilterSource(LPUNKNOWN lpunk, HRESULT *phr);
	~CFilterSource(void);

public:
	DECLARE_IUNKNOWN;

	// Basic COM - used here to reveal our property interface.
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	int       GetPinCount(void);
	CBasePin *GetPin(int n);

	STDMETHODIMP Stop();
	STDMETHODIMP Pause();
	STDMETHODIMP Run(REFERENCE_TIME tStart);

	// handle IFileSourceFilter interface
	STDMETHODIMP Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt);
	STDMETHODIMP GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt);

	/* IAMNetworkStatus methods */
	STDMETHODIMP get_ReceivedPackets(long * pReceivedPackets);
	STDMETHODIMP get_RecoveredPackets(long * pRecoveredPackets);
	STDMETHODIMP get_LostPackets( long * pLostPackets);
	STDMETHODIMP get_ReceptionQuality(long * pReceptionQuality);
	STDMETHODIMP get_BufferingCount(long * pBufferingCount);
	STDMETHODIMP get_IsBroadcast( VARIANT_BOOL * pIsBroadcast);
	STDMETHODIMP get_BufferingProgress(long * pBufferingProgress);

	// handle IDispatch interface
	STDMETHODIMP GetTypeInfoCount(UINT * pctinfo);
	STDMETHODIMP GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo ** pptinfo);
	STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR  ** rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid);
	STDMETHODIMP Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams,
		VARIANT * pvarResult,EXCEPINFO * pexcepinfo, UINT * puArgErr);
protected:
	TCHAR	mszSourceFile[MAX_PATH];
	CBaseFileParser *	mpFileParser;
	CMediaSeeking *		mpSeeking;
	CCritSec			mcsSeeker;
	bool				mbIsURL;

	CBaseDispatch		m_basedisp;

protected:

	bool	IsURL(TCHAR *pStr);
};
