#pragma once
#include "streams.h"

class CSourceAlloctor :	public CBaseAllocator
{
public:
	CSourceAlloctor(TCHAR *pName , LPUNKNOWN pUnk , HRESULT *phr);
	virtual ~CSourceAlloctor(void);

	STDMETHODIMP_(ULONG) NonDelegatingAddRef();
	STDMETHODIMP_(ULONG) NonDelegatingRelease();

	virtual void	Free(void);
	virtual HRESULT Alloc(void);

	STDMETHODIMP	Commit();
	STDMETHODIMP	Decommit();
	STDMETHODIMP	GetBuffer(IMediaSample **ppBuffer,	REFERENCE_TIME * pStartTime,REFERENCE_TIME * pEndTime,DWORD dwFlags);
	IMediaSample *	GetBuffer(BYTE *pBuf , LONG nDataLen , REFERENCE_TIME * pStartTime,REFERENCE_TIME * pEndTime) ;
	STDMETHODIMP	ReleaseBuffer(IMediaSample *pSample);

	STDMETHODIMP	SetProperties(	ALLOCATOR_PROPERTIES* pRequest,	ALLOCATOR_PROPERTIES* pActual);


protected:
	LONG			m_cRef;
	BOOL		mbDecommit;
};
