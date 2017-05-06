#include "CBaseStream.h"
#include "CBaseTrack.h"
#include "CSourceAlloctor.h"
#include "voLog.h"

CBaseStream::CBaseStream(CBaseTrack *pTrack ,  HRESULT *phr,	CFilterSource *pms , LPCWSTR pPinName ,  VO_MEM_OPERATOR *pMemOpt , bool bVideo)
: CSourceStream(NAME("File Reader Stream"), phr, (CSource *)pms, pPinName)
, CMemoryOpt(pMemOpt , VO_INDEX_FILTER_SOURCE)
, mbVideo(bVideo)
, mpSource(pms)
, mpTrack(pTrack)
, mllSeekPos(0)
, mbSeeking(false)
, mbEOS(false)
{
	mpAllocator = new CSourceAlloctor(_T("Source Alloctor") , NULL , phr);
	mpAllocator->AddRef();
}

CBaseStream::~CBaseStream(void)
{
	if(mpAllocator != NULL)
	{
		mpAllocator->Release();
		mpAllocator = NULL;
	}
}

STDMETHODIMP CBaseStream::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
		return CSourceStream::NonDelegatingQueryInterface(riid , ppv);
}

HRESULT CBaseStream::Active(void)
{
	VOLOGI("Active ")
	HRESULT hr = CSourceStream::Active();
	return hr;
}

HRESULT CBaseStream::Inactive(void)
{
	VOLOGI("Inactive ");
	mbEOS = false;
	mbSeeking = false; 
	return CSourceStream::Inactive();
}

HRESULT CBaseStream::DoBufferProcessingLoop(void)
{
	Command com;

	OnThreadStartPlay();

	HRESULT hr = S_OK;
	LONGLONG llStart , llStop;
	LONG	 lBufSize = 0;
	BYTE *	 pDataBuf = NULL;

	CRefTime rfTime;
	FILTER_STATE state;

	do {
		while (!CheckRequest(&com))
		{
			rfTime = 0;
			m_pFilter->GetState( 0 , &state);
			if(state == State_Running)
			{
				hr = m_pFilter->StreamTime(rfTime);
				if(hr != S_OK)
				{
					rfTime = 0;	
				}
			}

			if(mbSeeking || mbEOS || (!mbVideo && m_dRate != 1.0) )
			{
				Sleep(5);
				continue;
			}

			{
				//VOLOGI("&&& Get track data %s" , m_pName);
				CAutoLock lockit(&mcsSeeking);
				hr = mpTrack->GetTrackData(&pDataBuf , &lBufSize , &llStart , &llStop , rfTime.m_time / 10000 + 100);
				//VOLOGI("&&& get track data %lld hr = %x  %s", llStart , hr , m_pName);
				if(hr == S_FALSE)
				{
					VOLOGI("&&&  Deliver Endofstream %s " , m_pName);
					DeliverEndOfStream();
					mbEOS = true;
					continue;
				}
				else if(hr != S_OK)
				{
					DeliverEndOfStream();
					VOLOGI("&&&  %s NotifyEvent  EC_ERRORABORT " , m_pName);
					m_pFilter->NotifyEvent(EC_ERRORABORT, hr, 0);
					return hr;
				}

				llStart -= mllSeekPos;
				llStop  -= mllSeekPos;

				//VOLOGI("&&& Get sample = %lld " , llStart);

				IMediaSample *pSample = mpAllocator->GetBuffer(pDataBuf , lBufSize & 0x7FFFFFFF , &llStart , &llStop);
				if (pSample == NULL) {
					Sleep(1);
					continue;
				}

				if(lBufSize & 0x80000000)
					pSample->SetSyncPoint(TRUE);
				else 
					pSample->SetSyncPoint(FALSE);

				if(m_dRate != 1.0)
				{
					llStart = llStart / m_dRate;
					llStop  = llStop / m_dRate;

					pSample->SetTime(&llStart , &llStop);					
				}

				//VOLOGI("&&& Before deliver %s %lld,  " ,m_pName ,  llStart);
				
				hr = Deliver(pSample);
				pSample->Release();

				//VOLOGI("&&& after deliver hr = %d  %s " , hr ,  m_pName);
				if(FAILED(hr))
				{
					VOLOGI("&&& Exit loop thread !!! hr = %d %s" , hr ,  m_pName);
					return S_OK;
				}
			}
		}

		// For all commands sent to us there must be a Reply call!

		if (com == CMD_RUN || com == CMD_PAUSE) {
			Reply(NOERROR);
		} else if (com != CMD_STOP) {
			Reply((DWORD) E_UNEXPECTED);
			DbgLog((LOG_ERROR, 1, TEXT("Unexpected command!!!")));
		}
	} while (com != CMD_STOP);

	return S_FALSE;
}

HRESULT CBaseStream::CheckMediaType(const CMediaType *pMediaType)
{
	return S_OK;
}

HRESULT CBaseStream::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if(iPosition != 0)
		return VFW_S_NO_MORE_ITEMS;

	if(mpTrack == NULL)
		return E_FAIL;

	return mpTrack->GetMediaType(pMediaType);
}

HRESULT CBaseStream::DeliverEndOfStream(void)
{
	VOLOGI("Deliver End of stream EEEEEEEEEEEEEEEE");
	return CSourceStream::DeliverEndOfStream();
}

HRESULT CBaseStream::DeliverBeginFlush(void)
{
	mbSeeking = true;
	return CSourceStream::DeliverBeginFlush();
}

HRESULT CBaseStream::DeliverEndFlush(void)
{
	mbSeeking = false;
	return CSourceStream::DeliverEndFlush();
}

HRESULT	CBaseStream::DeliverNewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate)
{
	VOLOGI("&&& Deliver New Segment");
	return CSourceStream::DeliverNewSegment(tStart , tStop , dRate);
}

HRESULT CBaseStream::DecideAllocator(IMemInputPin * pPin, IMemAllocator ** pAlloc)
{
	if(mpAllocator == NULL)
		return E_FAIL;

	*pAlloc = mpAllocator;
	mpAllocator->AddRef();

	HRESULT hr = pPin->NotifyAllocator(*pAlloc, FALSE);
	if(hr != NOERROR)
		return hr;

	ALLOCATOR_PROPERTIES propInputRequest;
	return DecideBufferSize (mpAllocator , &propInputRequest);

}

HRESULT CBaseStream::DecideBufferSize(IMemAllocator * pAlloc,ALLOCATOR_PROPERTIES * ppropInputRequest)
{
	ppropInputRequest->cbAlign = 1;
	ppropInputRequest->cbPrefix = 0;
	ppropInputRequest->cBuffers = 1;
	ppropInputRequest->cbBuffer = m_mt.lSampleSize;

	ALLOCATOR_PROPERTIES propActual;
	HRESULT hr = pAlloc->SetProperties (ppropInputRequest, &propActual);

	if(ppropInputRequest->cbBuffer != propActual.cbBuffer || ppropInputRequest->cBuffers != propActual.cBuffers)
		return E_FAIL;

	return S_OK;
}


HRESULT		CBaseStream::SetCurPos(LONGLONG &llStart , LONGLONG &llStop , double &dRate)
{
	VOLOGI("&&& before Seek pos = %lld  %s" , llStart , m_pName);
	LONGLONG llPos = llStart / 10000;

	CAutoLock lockit(&mcsSeeking);
	HRESULT hr = mpTrack->SetPos(llPos);
	if(hr != S_OK)
	{
		VOLOGI("&&& setpos return %x %s" , hr , m_pName);
		return hr;
	}

	llStart = mllSeekPos = llPos * 10000;
	VOLOGI("&&& after Seek pos = %lld  %s" , llStart , m_pName);
	
	mbEOS = false;
	return hr;
}

LONGLONG	CBaseStream::GetDuration()
{
	if(mpTrack != NULL) 
		return mpTrack->GetTrackDuration();
}
