#pragma once
#include "streams.h"
#include "voVideo.h"
#include "CMemoryOpt.h"

class CVideoDecoder;
class CCCRRRFunc;

class CFilterVideoDecoder :	public CTransformFilter , public CMemoryOpt
{
public:
	CFilterVideoDecoder(TCHAR *pName , LPUNKNOWN pUnk , REFCLSID  clsid);
	virtual ~CFilterVideoDecoder(void);

	
	virtual HRESULT CheckInputType(const CMediaType* mtIn) ;
	virtual HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
	virtual HRESULT SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt);
	virtual HRESULT CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin);
	virtual HRESULT DecideBufferSize(IMemAllocator * pAllocator,ALLOCATOR_PROPERTIES *pprop);

	virtual HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);

	virtual HRESULT StartStreaming();
	virtual HRESULT StopStreaming();

	virtual HRESULT Receive(IMediaSample *pSample);

	// chance to grab extra interfaces on connection
	virtual HRESULT CheckConnect(PIN_DIRECTION dir,IPin *pPin);
	virtual HRESULT BreakConnect(PIN_DIRECTION dir);
	
	virtual HRESULT EndOfStream(void);
	virtual HRESULT BeginFlush(void);
	virtual HRESULT EndFlush(void);
	virtual HRESULT NewSegment(	REFERENCE_TIME tStart,	REFERENCE_TIME tStop,double dRate);

	STDMETHODIMP Stop();
	STDMETHODIMP Pause();
	STDMETHODIMP Run(REFERENCE_TIME tStart);

protected:
	HRESULT CopyRawData(VO_VIDEO_BUFFER *vopVBuf , BYTE *pOutBuf , int nOutBufLen);
protected:
	CVideoDecoder *			mpDecoder;
	CCCRRRFunc	*			mpCCRRRR;

	CCritSec				mcsDecoder;

	VO_VIDEO_CODINGTYPE		mnCodec;
	int						mnVideoHeight;
	int						mnVideoWidth;
	DWORD					mnFourCC;

	int						mnExtDataLen;
	BYTE *					mpExtData;

	GUID					mOutputType;
	int						mnOutImageSize;

	int						mnOutVideoWidth;
	int						mnOutVideoHeight;
};
