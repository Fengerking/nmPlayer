#include "CSourceAlloctor.h"
#include "stdio.h"


CSourceAlloctor::CSourceAlloctor(TCHAR *pName , LPUNKNOWN pUnk , HRESULT *phr)
: CBaseAllocator(pName , pUnk , phr)
, mbDecommit(TRUE)
, m_cRef(0)
{
}

CSourceAlloctor::~CSourceAlloctor(void)
{
}

STDMETHODIMP_(ULONG) CSourceAlloctor::NonDelegatingAddRef()
{
	LONG lRef = InterlockedIncrement( &m_cRef );

	//RETAILMSG( 1 , (L"Ref = %d +++\r\n" , lRef));

	return max(ULONG(m_cRef), 1ul);
}
STDMETHODIMP_(ULONG) CSourceAlloctor::NonDelegatingRelease()
{
	LONG lRef = InterlockedDecrement( &m_cRef );

	//RETAILMSG( 1 , (L"Ref = %d --- \r\n" , lRef));
	if (lRef == 0) {

		// COM rules say we must protect against re-entrancy.
		// If we are an aggregator and we hold our own interfaces
		// on the aggregatee, the QI for these interfaces will
		// addref ourselves. So after doing the QI we must release
		// a ref count on ourselves. Then, before releasing the
		// private interface, we must addref ourselves. When we do
		// this from the destructor here it will result in the ref
		// count going to 1 and then back to 0 causing us to
		// re-enter the destructor. Hence we add an extra refcount here
		// once we know we will delete the object.
		// for an example aggregator see filgraph\distrib.cpp.

		m_cRef++;

		delete this;
		return ULONG(0);
	} else {
		return max(ULONG(m_cRef), 1ul);
	}
}

void	CSourceAlloctor::Free(void)
{
	CMediaSample *pSample;
	for (;;) {
		pSample = m_lFree.RemoveHead();
		if (pSample != NULL) {
			delete pSample;
		} else {
			break;
		}
	}
}

HRESULT CSourceAlloctor::Alloc(void)
{
	 m_bChanged = FALSE; 
	return S_OK;
}

STDMETHODIMP	CSourceAlloctor::Commit()
{

	HRESULT hr = Alloc();
	if(hr == S_OK)
	{
		AddRef();
		mbDecommit = FALSE;
	}

	return hr;
}
STDMETHODIMP	CSourceAlloctor::Decommit()
{
	if(mbDecommit == TRUE)
		return S_OK;

	mbDecommit = TRUE;

	Free();
	Release();
	return S_OK;
}

STDMETHODIMP	CSourceAlloctor::GetBuffer(IMediaSample **ppBuffer,	REFERENCE_TIME * pStartTime,REFERENCE_TIME * pEndTime,DWORD dwFlags)
{
	return E_FAIL;
}

IMediaSample *	CSourceAlloctor::GetBuffer(BYTE *pBuf , LONG nDataLen , REFERENCE_TIME * pStartTime,REFERENCE_TIME * pEndTime) 
{
	HRESULT hr = S_OK;
	CMediaSample *pSample =  m_lFree.RemoveHead();
	if(pSample == NULL)
	{
		pSample = new CMediaSample(TEXT("Buffering Sample") , this , &hr , pBuf , nDataLen);
		if(pSample == NULL)
		{
			return NULL;
		}
	}
	else
	{
		pSample->SetPointer(pBuf , nDataLen);
	}

	pSample->AddRef();

	hr = pSample->SetTime(pStartTime , pEndTime);
	return pSample;
}

STDMETHODIMP	CSourceAlloctor::ReleaseBuffer(IMediaSample *pSample)
{
	CheckPointer(pSample,E_POINTER);
    ValidateReadPtr(pSample,sizeof(IMediaSample));

    BOOL bRelease = FALSE;
    {
        CAutoLock cal(this);

        /* Put back on the free list */

        m_lFree.Add((CMediaSample *)pSample);
        if (m_lWaiting != 0) {
            NotifySample();
        }

        // if there is a pending Decommit, then we need to complete it by
        // calling Free() when the last buffer is placed on the free list

        LONG l1 = m_lFree.GetCount();
        if (m_bDecommitInProgress && (l1 == m_lAllocated)) {
            Free();
            m_bDecommitInProgress = FALSE;
            bRelease = TRUE;
        }
    }

    if (m_pNotify) {

        ASSERT(m_fEnableReleaseCallback);

        //
        // Note that this is not synchronized with setting up a notification
        // method.
        //
        m_pNotify->NotifyRelease();
    }

    /* For each buffer there is one AddRef, made in GetBuffer and released
       here. This may cause the allocator and all samples to be deleted */

    if (bRelease) {
        Release();
    }
    return NOERROR;
	//return CBaseAllocator::ReleaseBuffer(pBuffer);
}

STDMETHODIMP CSourceAlloctor::SetProperties(	ALLOCATOR_PROPERTIES* pRequest,	ALLOCATOR_PROPERTIES* pActual)
{
	if(m_bChanged == TRUE)
	{
		pActual->cbBuffer = m_lSize = pRequest->cbBuffer;
		pActual->cBuffers = m_lCount = pRequest->cBuffers;
		pActual->cbAlign = m_lAlignment = pRequest->cbAlign;
		pActual->cbPrefix = m_lPrefix = pRequest->cbPrefix;
	}

    CAutoLock cObjectLock(this);

    ZeroMemory(pActual, sizeof(ALLOCATOR_PROPERTIES));

    ASSERT(pRequest->cbBuffer > 0);

    /*  Check the alignment requested */
    if (pRequest->cbAlign != 1) {
        DbgLog((LOG_ERROR, 2, TEXT("Alignment requested was 0x%x, not 1"),
               pRequest->cbAlign));
        return VFW_E_BADALIGN;
    }

    /* Can't do this if already committed, there is an argument that says we
       should not reject the SetProperties call if there are buffers still
       active. However this is called by the source filter, which is the same
       person who is holding the samples. Therefore it is not unreasonable
       for them to free all their samples before changing the requirements */

    if (m_bCommitted) {
        return VFW_E_ALREADY_COMMITTED;
    }

    /* Must be no outstanding buffers */

    if (m_lAllocated != m_lFree.GetCount()) {
        return VFW_E_BUFFERS_OUTSTANDING;
    }

    /* There isn't any real need to check the parameters as they
       will just be rejected when the user finally calls Commit */

    pActual->cbBuffer = m_lSize = pRequest->cbBuffer;
    pActual->cBuffers = m_lCount = pRequest->cBuffers;
    pActual->cbAlign = m_lAlignment = pRequest->cbAlign;
    pActual->cbPrefix = m_lPrefix = pRequest->cbPrefix;

    m_bChanged = TRUE;
    return NOERROR;
}
