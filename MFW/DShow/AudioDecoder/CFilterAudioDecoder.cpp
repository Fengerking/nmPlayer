#include "CFilterAudioDecoder.h"
#include "initguid.h"
#include "voGuids.h"
#include "cmnMemory.h"
#include "CAudioDecoder.h"

CFilterAudioDecoder::CFilterAudioDecoder(TCHAR *pName , LPUNKNOWN pUnk , REFCLSID  clsid)
: CTransformFilter(pName , pUnk , clsid)
, CMemoryOpt(&g_memOP , VO_INDEX_FILTER_VIDEODECODER)
, mpAudioDecoder(NULL)
, mnCodec(0)
, mnFourCC(0)
, mnSampleRate(0)
, mnBits(0)
, mnChannels(0)
, mpHeaderData(NULL)
, mnDataSize(0)
, mnAvgBytesPerSec(0)
, mnPCMLen(0)
, mpOutSample(0)
, mllOutTime(-1)
, mnMinPCMSize(0)
{
	cmnMemFillPointer(0);
}

CFilterAudioDecoder::~CFilterAudioDecoder(void)
{
	if(mpHeaderData != NULL)
	{
		delete [] mpHeaderData;
		mpHeaderData = NULL;
	}
}

HRESULT CFilterAudioDecoder::CheckInputType(const CMediaType* mtIn)
{
	if(mtIn->majortype != MEDIATYPE_Audio)
		return E_FAIL;

	if(mtIn->subtype == MEDIASUBTYPE_DOLBY_AC3_SPDIF ||
		mtIn->subtype == MEDIASUBTYPE_DOLBY_AC3 ||
		mtIn->subtype == MEDIASUBTYPE_MPEG1Payload ||
		mtIn->subtype == MEDIASUBTYPE_MPEG1Packet ||
		mtIn->subtype == MEDIASUBTYPE_MPEG2_AUDIO ||
		mtIn->subtype == MEDIASUBTYPE_MP3 ||
		mtIn->subtype == MEDIASUBTYPE_DVD_LPCM_AUDIO ||
		mtIn->subtype == WMMEDIASUBTYPE_WMAudioV2 ||
		mtIn->subtype == WMMEDIASUBTYPE_WMAudioV9 ||
		mtIn->subtype == WMMEDIASUBTYPE_WMAudio_Lossless ||
		mtIn->subtype == MEDIASUBTYPE_Ogg ||
		mtIn->subtype == MEDIASUBTYPE_VOAMR ||
		mtIn->subtype == MEDIASUBTYPE_AMRWB ||
		mtIn->subtype == MEDIASUBTYPE_AMRWBP ||
		mtIn->subtype == MEDIASUBTYPE_VOQCP ||
		mtIn->subtype == MEDIASUBTYPE_VOEVRC ||
		mtIn->subtype == MEDIASUBTYPE_VOAAC ||
		mtIn->subtype == MEDIASUBTYPE_NEROAAC ||
		mtIn->subtype == MEDIASUBTYPE_ADTSAAC ||
		mtIn->subtype == MEDIASUBTYPE_ADIFAAC ||
		mtIn->subtype == MEDIASUBTYPE_REALAUDIO)
		return S_OK;

	return E_FAIL;
}

HRESULT CFilterAudioDecoder::SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt)
{
	if(direction == PINDIR_INPUT)
	{
		if(pmt->subtype == MEDIASUBTYPE_DOLBY_AC3_SPDIF ||
			pmt->subtype == MEDIASUBTYPE_DOLBY_AC3)
		{
			mnCodec = VO_AUDIO_CodingAC3;
		}
		else if(pmt->subtype == MEDIASUBTYPE_MPEG1Payload ||
			pmt->subtype == MEDIASUBTYPE_MPEG1Packet ||
			pmt->subtype == MEDIASUBTYPE_MPEG2_AUDIO )
		{
			mnCodec = VO_AUDIO_CodingMP1;
		}
		else if(pmt->subtype == MEDIASUBTYPE_MP3)
		{
			mnCodec = VO_AUDIO_CodingMP3;
		}
		else if(pmt->subtype == WMMEDIASUBTYPE_WMAudioV2 ||
			pmt->subtype == WMMEDIASUBTYPE_WMAudioV9 || 
			pmt->subtype == WMMEDIASUBTYPE_WMAudio_Lossless)
		{
			mnCodec = VO_AUDIO_CodingWMA;
		}
		else if(pmt->subtype == MEDIASUBTYPE_Ogg)
		{
			mnCodec = VO_AUDIO_CodingOGG;
		}
		else if(pmt->subtype == MEDIASUBTYPE_VOAMR)
		{
			mnCodec = VO_AUDIO_CodingAMRNB;
		}
		else if(pmt->subtype == MEDIASUBTYPE_AMRWB)
		{
			mnCodec = VO_AUDIO_CodingAMRWB;
		}
		else if(pmt->subtype == MEDIASUBTYPE_AMRWBP)
		{
			mnCodec = VO_AUDIO_CodingAMRWBP;
		}
		else if(pmt->subtype == MEDIASUBTYPE_VOQCP)
		{
			mnCodec = VO_AUDIO_CodingQCELP13;
		}
		else if(pmt->subtype == MEDIASUBTYPE_VOEVRC)
		{
			mnCodec = VO_AUDIO_CodingEVRC;
		}
		else if(pmt->subtype == MEDIASUBTYPE_VOAAC ||
			pmt->subtype == MEDIASUBTYPE_NEROAAC ||
			pmt->subtype == MEDIASUBTYPE_ADTSAAC ||
			pmt->subtype == MEDIASUBTYPE_ADIFAAC)
		{
			mnCodec = VO_AUDIO_CodingAAC;
		}
		else if(pmt->subtype == MEDIASUBTYPE_REALAUDIO)
		{
			mnCodec = VO_AUDIO_CodingRA;
		}
		else 
		{
			return E_FAIL;
		}

		if(pmt->formattype == FORMAT_WaveFormatEx)
		{
			WAVEFORMATEX * pWaveFmt = (WAVEFORMATEX *)pmt->pbFormat;

			mnFourCC		= pWaveFmt->wFormatTag;
			mnSampleRate	= pWaveFmt->nSamplesPerSec;
			mnChannels		= pWaveFmt->nChannels;
			mnBits			= pWaveFmt->wBitsPerSample;

			if(mnBits == 0)
				mnBits = 16;

			mnAvgBytesPerSec = mnSampleRate * mnChannels * mnBits / 8;
			mnMinPCMSize = mnAvgBytesPerSec * 4 / 10;

			if(mpHeaderData != NULL)
			{
				delete [] mpHeaderData;
				mpHeaderData = NULL;

				mnDataSize = 0;
			}

			if(pWaveFmt->cbSize > 0)
			{
				mnDataSize = pWaveFmt->cbSize;

				mpHeaderData = new BYTE[mnDataSize];
				if(mpHeaderData == NULL)
					return E_OUTOFMEMORY;

				Copy(mpHeaderData , pmt->pbFormat + sizeof(WAVEFORMATEX) , mnDataSize);
			}
		}
		else
		{
			return E_FAIL;
		}

	}
	return CTransformFilter::SetMediaType(direction , pmt);
}

HRESULT CFilterAudioDecoder::CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut)
{
	if(mtOut->majortype == MEDIATYPE_Audio && mtOut->subtype == MEDIASUBTYPE_PCM)
		return S_OK;

	return S_OK;
}

HRESULT CFilterAudioDecoder::CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin)
{
	HRESULT hr = CTransformFilter::CompleteConnect(direction , pReceivePin);
	return hr;
}

HRESULT CFilterAudioDecoder::DecideBufferSize(IMemAllocator * pAllocator,ALLOCATOR_PROPERTIES *pprop)
{
	pprop->cBuffers = 1;
	pprop->cbAlign  = 4;
	pprop->cbBuffer = mnBits * mnChannels * mnSampleRate / 8;
	pprop->cbPrefix = 0; 

	ALLOCATOR_PROPERTIES Actual;
	HRESULT hr = pAllocator->SetProperties(pprop, &Actual);
	if (FAILED(hr))
		return hr;

	if (pprop->cBuffers > Actual.cBuffers || pprop->cbBuffer > Actual.cbBuffer)
		return E_FAIL;

	return NOERROR;
}

HRESULT CFilterAudioDecoder::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if(m_pInput->IsConnected() == FALSE)
		return E_FAIL;

	if(iPosition != 0)
		return VFW_S_NO_MORE_ITEMS;

	pMediaType->majortype = MEDIATYPE_Audio;
	pMediaType->subtype   = MEDIASUBTYPE_PCM;
	pMediaType->bFixedSizeSamples = FALSE;
	pMediaType->bTemporalCompression = FALSE;
	pMediaType->formattype =  FORMAT_WaveFormatEx;

	WAVEFORMATEX *pFmt = (WAVEFORMATEX *)pMediaType->AllocFormatBuffer(sizeof(WAVEFORMATEX));
	if(pFmt == NULL)
		return E_OUTOFMEMORY;

	pFmt->cbSize = 0;
	pFmt->wFormatTag = 0X1;
	pFmt->nChannels = mnChannels;
	pFmt->nSamplesPerSec = mnSampleRate;
	pFmt->wBitsPerSample = mnBits;
	pFmt->nBlockAlign = mnBits * mnChannels / 8;
	pFmt->nAvgBytesPerSec = mnSampleRate * pFmt->nBlockAlign;

	return S_OK;
}

HRESULT CFilterAudioDecoder::StartStreaming()
{
	if(mpAudioDecoder == NULL)
	{
		mpAudioDecoder = new CAudioDecoder(g_hInst , mnCodec , &g_memOP);
		if(mpAudioDecoder == NULL)
			return E_OUTOFMEMORY;

		VO_AUDIO_FORMAT audioFmt;
		audioFmt.Channels	= mnChannels;
		audioFmt.SampleBits = mnBits;
		audioFmt.SampleRate	= mnSampleRate;

		VO_U32 nRC = mpAudioDecoder->Init(mpHeaderData , mnDataSize , &audioFmt);
		if(nRC != VO_ERR_NONE)
			return E_FAIL;
	}

	mpAudioDecoder->Start();

	return CTransformFilter::StartStreaming();
}

HRESULT CFilterAudioDecoder::StopStreaming()
{
	if(mpAudioDecoder != NULL)
	{
		mpAudioDecoder->Stop();
	}

	return CTransformFilter::StartStreaming();
}

HRESULT CFilterAudioDecoder::Receive(IMediaSample *pSample)
{
	if(mpAudioDecoder == NULL)
		return E_FAIL;

	VO_CODECBUFFER incdBuffer , outcdBuffer;
	
	LONGLONG llStart , llStop;
	pSample->GetTime(&llStart , &llStop);

	long lInBufLen = pSample->GetActualDataLength();
	if(lInBufLen <= 0)
		return S_OK;

	BYTE *pInBuf = NULL;
	HRESULT hr = pSample->GetPointer(&pInBuf);
	if(hr != S_OK || pInBuf == NULL)
		return S_OK;

	incdBuffer.Buffer	= pInBuf;
	incdBuffer.Length	= lInBufLen;
	incdBuffer.Time		= llStart / 10000;

	VO_U32 nRC = mpAudioDecoder->SetInputData(&incdBuffer);
	if(nRC != VO_ERR_NONE)
		return E_FAIL;

	if(mllOutTime == -1)
	{
		mllOutTime = llStart;
	}

	if(mpOutSample == NULL)
	{
		hr = InitializeOutputSample(pSample , &mpOutSample);
		if(hr != S_OK)
			return hr;
	}
	
	BYTE *pOutBuf = NULL;
	hr = mpOutSample->GetPointer(&pOutBuf);
	if(pOutBuf == NULL)
		return S_OK;

	long lOutBufLen = mpOutSample->GetSize();

	outcdBuffer.Buffer = pOutBuf + mnPCMLen;
	outcdBuffer.Length = lOutBufLen - mnPCMLen;


	VO_AUDIO_FORMAT audioFmt;
	while(mpAudioDecoder->GetOutputData(&outcdBuffer , &audioFmt) == VO_ERR_NONE)
	{
		if(audioFmt.Channels != mnChannels || audioFmt.SampleBits != mnBits || audioFmt.SampleRate != mnSampleRate)
		{
			mnChannels	= audioFmt.Channels;
			mnBits		= audioFmt.SampleBits;
			mnSampleRate= audioFmt.SampleRate;

			mnAvgBytesPerSec = mnSampleRate * mnChannels * mnBits / 8;
			mnMinPCMSize = mnAvgBytesPerSec * 4 / 10;

			GetMediaType(0 , &mCurMediaType);
			mpOutSample->SetMediaType(&mCurMediaType);

			//NotifyEvent (EC_MEDIATYPE_CHNAGE, 0, 0);
		}

		mnPCMLen += outcdBuffer.Length;

		if(mnPCMLen > mnMinPCMSize)
		{
			mpOutSample->SetActualDataLength(mnPCMLen);
			LONGLONG llTemp = mnPCMLen;
			llTemp = llTemp * 10000000 / mnAvgBytesPerSec;
			llTemp += mllOutTime;

			mpOutSample->SetTime(&mllOutTime , &llTemp);

			mpOutSample->SetSyncPoint(TRUE);

			FILTER_STATE state ;
			GetState(0 , &state);

			hr = m_pOutput->Deliver(mpOutSample);
			mpOutSample->Release();

			mpOutSample = NULL;
			mnPCMLen = 0;
			mllOutTime = -1;

			//if(hr != S_OK)
			//	return hr;

			if(mpOutSample == NULL)
			{
				hr = InitializeOutputSample(pSample , &mpOutSample);
				if(hr != S_OK)
					return hr;

				hr = mpOutSample->GetPointer(&pOutBuf);
				if(pOutBuf == NULL)
					return S_OK;

				lOutBufLen = mpOutSample->GetSize();
			}
		}

		outcdBuffer.Buffer = pOutBuf + mnPCMLen;
		outcdBuffer.Length = lOutBufLen - mnPCMLen;
	}

	return S_OK;
}

HRESULT CFilterAudioDecoder::CheckConnect(PIN_DIRECTION dir,IPin *pPin)
{
	return CTransformFilter::CheckConnect(dir , pPin);
}

HRESULT CFilterAudioDecoder::BreakConnect(PIN_DIRECTION dir)
{
	if(mpAudioDecoder != NULL)
	{
		mpAudioDecoder->Stop();
		delete mpAudioDecoder;
		mpAudioDecoder = NULL;
	}

	return CTransformFilter::BreakConnect(dir);
}

HRESULT CFilterAudioDecoder::EndOfStream(void)
{
	//deliver remain data
	if(mpOutSample != NULL)
	{
		mpOutSample->SetActualDataLength(mnPCMLen);
		LONGLONG llTemp = mnPCMLen;
		llTemp = llTemp * 10000000 / mnAvgBytesPerSec;
		llTemp += mllOutTime;

		mpOutSample->SetTime(&mllOutTime , &llTemp);

		mpOutSample->SetSyncPoint(TRUE);

		m_pOutput->Deliver(mpOutSample);
		mpOutSample->Release();

		mpOutSample = NULL;
		mnPCMLen = 0;
		mllOutTime = -1;
	}

	return CTransformFilter::EndOfStream();
}

HRESULT CFilterAudioDecoder::BeginFlush(void)
{
	return CTransformFilter::BeginFlush();
}

HRESULT CFilterAudioDecoder::EndFlush(void)
{
	if(mpAudioDecoder != NULL)
	{
		mpAudioDecoder->Flush();
	}

	return CTransformFilter::EndFlush();
}

HRESULT CFilterAudioDecoder::NewSegment(	REFERENCE_TIME tStart,	REFERENCE_TIME tStop,double dRate)
{
	return CTransformFilter::NewSegment(tStart , tStop , dRate);
}

STDMETHODIMP CFilterAudioDecoder::Stop()
{
	HRESULT hr = CTransformFilter::Stop();
	return hr;
}

STDMETHODIMP CFilterAudioDecoder::Pause()
{
	HRESULT hr = CTransformFilter::Pause();
	return hr;
}

STDMETHODIMP CFilterAudioDecoder::Run(REFERENCE_TIME tStart)
{
	HRESULT hr = CTransformFilter::Run(tStart);
	return hr;
}