#pragma once
#include "streams.h"
#include "CMemoryOpt.h"
#include "voAudio.h"

class CAudioDecoder;
class CFilterAudioDecoder :	public CTransformFilter , public CMemoryOpt
{
public:
	CFilterAudioDecoder(TCHAR *pName , LPUNKNOWN pUnk , REFCLSID  clsid);
	virtual ~CFilterAudioDecoder(void);

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
	CAudioDecoder *	mpAudioDecoder;

	//Audio property
	VO_U32		mnCodec;
	VO_U32		mnFourCC;

	int			mnSampleRate;
	int			mnChannels;
	int			mnBits;
	int			mnAvgBytesPerSec;

	//header data
	BYTE *		mpHeaderData;
	int			mnDataSize;

	//for audio format changed
	CMediaType	mCurMediaType;

	//Output sample property
	int			mnPCMLen;
	int			mnMinPCMSize;
	IMediaSample *	mpOutSample;
	LONGLONG		mllOutTime;

};
