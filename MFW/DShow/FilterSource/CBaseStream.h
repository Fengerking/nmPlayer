#pragma once
#include "streams.h"
#include "CMemoryOpt.h"
#include "voSource.h"

class CBaseTrack;
class CFilterSource;
class CSourceAlloctor;

class CBaseStream :	public CSourceStream , public CMemoryOpt
{
public:
	CBaseStream(CBaseTrack *pTrack ,  HRESULT *phr,	CFilterSource *pms , LPCWSTR pPinName ,  VO_MEM_OPERATOR *pMemOpt , bool bVideo);
	virtual ~CBaseStream(void);

	// override to expose IMediaPosition
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	HRESULT Active(void);    // Starts up the worker thread
	HRESULT Inactive(void);  // Exits the worker thread.

	virtual HRESULT DoBufferProcessingLoop(void);    // the loop executed whilst running

	virtual HRESULT CheckMediaType(const CMediaType *pMediaType);
	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);  // List pos. 0-n

	virtual HRESULT FillBuffer(IMediaSample *pSamp) {return E_NOTIMPL;}

	virtual HRESULT DecideAllocator(IMemInputPin * pPin, IMemAllocator ** pAlloc);
	virtual HRESULT DecideBufferSize(IMemAllocator * pAlloc,ALLOCATOR_PROPERTIES * ppropInputRequest);

	virtual HRESULT DeliverEndOfStream(void);
	virtual HRESULT DeliverBeginFlush(void);
	virtual HRESULT DeliverEndFlush(void);
	virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart,REFERENCE_TIME tStop,double dRate);

	//for IMediaSeeking interface
	HRESULT		SetRate(double dRate)
	{
		m_dRate = dRate;
		return S_OK;
	}
	HRESULT		SetCurPos(LONGLONG &llStart , LONGLONG &llStop , double &dRate);
	LONGLONG	GetDuration();

protected:
	CFilterSource *	mpSource;
	CBaseTrack *	mpTrack;

	bool			mbVideo;

	CSourceAlloctor * mpAllocator;

	LONGLONG		mllSeekPos;
	CCritSec		mcsSeeking;
	bool			mbSeeking;
	bool			mbEOS;
};
