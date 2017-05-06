#include "CFilterVideoDecoder.h"
#include "initguid.h"
#include "voGuids.h"
#include "cmnMemory.h"
#include "Dvdmedia.h"
#include "CVideoDecoder.h"
#include "CCCRRRFunc.h"
#include "IFilterVideoRender.h"
#include "FilterVideoRender_Guids.h"
#include "voLog.h"

CFilterVideoDecoder::CFilterVideoDecoder(TCHAR *pName , LPUNKNOWN pUnk , REFCLSID  clsid)
: CTransformFilter(pName , pUnk , clsid)
, CMemoryOpt(&g_memOP , VO_INDEX_FILTER_VIDEODECODER)
, mnVideoHeight(0)
, mnVideoWidth(0)
, mnExtDataLen(0)
, mpExtData(NULL)
, mpDecoder(NULL)
, mnOutImageSize(0)
, mpCCRRRR(NULL)
, mnOutVideoWidth(0)
, mnOutVideoHeight(0)
{
	cmnMemFillPointer(0);
}

CFilterVideoDecoder::~CFilterVideoDecoder(void)
{
	if(mpExtData != NULL)
	{
		Free(mpExtData);
		mpExtData = NULL;
	}


	if(mpDecoder != NULL)
	{
		CAutoLock lockit(&mcsDecoder);
		delete mpDecoder;
		mpDecoder = NULL;
	}

	if(mpCCRRRR != NULL)
	{
		delete mpCCRRRR;
		mpCCRRRR = NULL;
	}
}


HRESULT CFilterVideoDecoder::CheckInputType(const CMediaType* mtIn) 
{
	if(mtIn->majortype != MEDIATYPE_Video && mtIn->majortype != MEDIATYPE_Stream)
		return E_FAIL;

	if(mtIn->subtype == MEDIASUBTYPE_MPEG2_VIDEO ||
		//mtIn->subtype == MEDIASUBTYPE_XVID ||
		mtIn->subtype == MEDIASUBTYPE_DX50 ||
		mtIn->subtype == MEDIASUBTYPE_DIVX ||
		mtIn->subtype == MEDIASUBTYPE_DIV3 ||
		mtIn->subtype == MEDIASUBTYPE_DIV4 ||
		mtIn->subtype == MEDIASUBTYPE_DIV5 ||
		mtIn->subtype == MEDIASUBTYPE_DIV6 ||
		mtIn->subtype == MEDIASUBTYPE_MPG4 ||
		mtIn->subtype == MEDIASUBTYPE_MP4V ||
		mtIn->subtype == MEDIASUBTYPE_AVC ||
		mtIn->subtype == MEDIASUBTYPE_H264 ||
		mtIn->subtype == MEDIASUBTYPE_X264 ||
		mtIn->subtype == MEDIASUBTYPE_H263 ||
		mtIn->subtype == MEDIASUBTYPE_S263 ||
		mtIn->subtype == MEDIASUBTYPE_MJPG ||
		mtIn->subtype == MEDIASUBTYPE_QTJpeg ||
		mtIn->subtype == MEDIASUBTYPE_RV20 ||
		mtIn->subtype == MEDIASUBTYPE_RV30 ||
		mtIn->subtype == MEDIASUBTYPE_RV40 ||
		mtIn->subtype == MEDIASUBTYPE_VP60 ||
		mtIn->subtype == MEDIASUBTYPE_VP61 ||
		mtIn->subtype == MEDIASUBTYPE_VP62 ||
		mtIn->subtype == MEDIASUBTYPE_VP70 ||
		mtIn->subtype == WMMEDIASUBTYPE_WMV1 ||
		mtIn->subtype == WMMEDIASUBTYPE_WMV2 ||
		mtIn->subtype == WMMEDIASUBTYPE_WMV3 ||
		mtIn->subtype == WMMEDIASUBTYPE_WMVP)
	{
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CFilterVideoDecoder::CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut)
{
	if(mtOut->majortype != MEDIATYPE_Video)
		return E_FAIL;

	if(mtOut->formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER *pVideoInfo = (VIDEOINFOHEADER *)mtOut->pbFormat;
		mnOutVideoWidth = pVideoInfo->bmiHeader.biWidth;
		mnOutVideoHeight = abs(pVideoInfo->bmiHeader.biHeight);

		VOLOGI(" 1 Out video width = %d , height = %d" , mnOutVideoWidth , mnOutVideoHeight);
		
	}
	else if(mtOut->formattype == FORMAT_VideoInfo2)
	{
		VIDEOINFOHEADER2 *pVideoInfo = (VIDEOINFOHEADER2 *)mtOut->pbFormat;
		mnOutVideoWidth = pVideoInfo->bmiHeader.biWidth;
		mnOutVideoHeight = abs(pVideoInfo->bmiHeader.biHeight);

		VOLOGI(" 2 Out video width = %d , height = %d" , mnOutVideoWidth , mnOutVideoHeight);
	}

	return S_OK;
}

HRESULT CFilterVideoDecoder::SetMediaType(PIN_DIRECTION direction,const CMediaType *pmt)
{
	if(direction == PINDIR_INPUT)
	{
		if(pmt->subtype == MEDIASUBTYPE_MPEG2_VIDEO)
		{
			mnCodec = VO_VIDEO_CodingMPEG2;
		}
		else if(pmt->subtype == MEDIASUBTYPE_DX50 ||
			pmt->subtype == MEDIASUBTYPE_DIVX ||
			pmt->subtype == MEDIASUBTYPE_DIV3 ||
			pmt->subtype == MEDIASUBTYPE_DIV4 ||
			pmt->subtype == MEDIASUBTYPE_DIV5 ||
			pmt->subtype == MEDIASUBTYPE_DIV6)
		{
			mnCodec = VO_VIDEO_CodingDIVX;
		}
		else if(pmt->subtype == MEDIASUBTYPE_MPG4 ||
			pmt->subtype == MEDIASUBTYPE_MP4V)
		{
			mnCodec = VO_VIDEO_CodingMPEG4;
		}
		else if(pmt->subtype == MEDIASUBTYPE_AVC ||
			pmt->subtype == MEDIASUBTYPE_H264 ||
			pmt->subtype == MEDIASUBTYPE_X264)
		{
			mnCodec = VO_VIDEO_CodingH264;
		}
		else if(pmt->subtype == MEDIASUBTYPE_H263)
		{
			mnCodec = VO_VIDEO_CodingH263;
		}
		else if(pmt->subtype == MEDIASUBTYPE_S263)
		{
			mnCodec = VO_VIDEO_CodingS263;
		}
		else if(pmt->subtype == MEDIASUBTYPE_MJPG ||
			pmt->subtype == MEDIASUBTYPE_QTJpeg)
		{
			mnCodec = VO_VIDEO_CodingMJPEG;
		}
		else if(pmt->subtype == MEDIASUBTYPE_RV20 ||
			pmt->subtype == MEDIASUBTYPE_RV30 ||
			pmt->subtype == MEDIASUBTYPE_RV40)
		{
			mnCodec = VO_VIDEO_CodingRV;
		}
		else if(pmt->subtype == MEDIASUBTYPE_VP60 ||
			pmt->subtype == MEDIASUBTYPE_VP61 ||
			pmt->subtype == MEDIASUBTYPE_VP62)
		{
			mnCodec = VO_VIDEO_CodingVP6;
		}
		else if(pmt->subtype == WMMEDIASUBTYPE_WMV1 ||
			pmt->subtype == WMMEDIASUBTYPE_WMV2 ||
			pmt->subtype == WMMEDIASUBTYPE_WMV3 ||
			pmt->subtype == WMMEDIASUBTYPE_WMVP)
		{
			mnCodec = VO_VIDEO_CodingWMV;
		}
		else
			return E_FAIL;

		if(mpExtData != NULL)
		{
			Free(mpExtData) ;
			mpExtData = NULL;
		}
		if(pmt->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER *pVideoInfo = (VIDEOINFOHEADER *)pmt->pbFormat;
			mnVideoWidth = pVideoInfo->bmiHeader.biWidth;
			mnVideoHeight = pVideoInfo->bmiHeader.biHeight;
			mnFourCC = pVideoInfo->bmiHeader.biCompression;

			mnExtDataLen = pmt->cbFormat - sizeof(VIDEOINFOHEADER);
			if(mnExtDataLen > 0)
			{
				mpExtData = (BYTE *)Alloc(mnExtDataLen);
				if(mpExtData == NULL)
					return E_OUTOFMEMORY;

				Copy(mpExtData , pmt->pbFormat + sizeof(VIDEOINFOHEADER ) , mnExtDataLen);
			}
		}
		else if(pmt->formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2 *pVideoInfo = (VIDEOINFOHEADER2 *)pmt->pbFormat;
			mnVideoWidth = pVideoInfo->bmiHeader.biWidth;
			mnVideoHeight = pVideoInfo->bmiHeader.biHeight;
			mnFourCC = pVideoInfo->bmiHeader.biCompression;

			mnExtDataLen = pVideoInfo->bmiHeader.biSize;
			if(mnExtDataLen > 0)
			{
				mpExtData = (BYTE *)Alloc(mnExtDataLen);
				if(mpExtData == NULL)
					return E_OUTOFMEMORY;

				Copy(mpExtData , pmt->pbFormat + sizeof(BITMAPINFOHEADER ) , mnExtDataLen);
			}
		}
	}
	else
	{
		if(mpCCRRRR != NULL)
		{
			delete mpCCRRRR;
			mpCCRRRR = NULL;
		}

		mOutputType = pmt->subtype;
		mnOutImageSize = pmt->lSampleSize;
	}

	return CTransformFilter::SetMediaType(direction , pmt);
}

HRESULT CFilterVideoDecoder::CompleteConnect(PIN_DIRECTION direction,IPin *pReceivePin)
{
	//Get memory operator form render
	if(direction == PINDIR_OUTPUT)
	{
		PIN_INFO pinInfo;
		HRESULT	 hr = pReceivePin->QueryPinInfo(&pinInfo);
		if(hr == S_OK)
		{
			pinInfo.pFilter->Release();

			IFilterVideoRender *pVR = NULL;
			pinInfo.pFilter->QueryInterface(IID_IFilterVideoRender , (void **)&pVR);
			if(pVR != NULL)
			{
				pVR->Release();

				VO_MEM_VIDEO_OPERATOR * pVMOP = NULL;
				pVR->GetParam(VO_PID_VIDEO_VIDEOMEMOP , (int *)&pVMOP);
				if(pVMOP != NULL)
				{
					mpDecoder->SetParam(VO_PID_VIDEO_VIDEOMEMOP , pVMOP);
				}
			}
		}
	}

	return CTransformFilter::CompleteConnect(direction , pReceivePin);
}

HRESULT CFilterVideoDecoder::DecideBufferSize(IMemAllocator * pAllocator,ALLOCATOR_PROPERTIES *pprop)
{
	HRESULT hr = NOERROR;

	pprop->cBuffers = 1;
	pprop->cbAlign  = 1;
	pprop->cbPrefix = 0;

	if (mOutputType == MEDIASUBTYPE_WYUV)
	{
		pprop->cbBuffer = sizeof (VO_VIDEO_BUFFER);
	}
	else if(mOutputType == MEDIASUBTYPE_YUY2 ||
		mOutputType == MEDIASUBTYPE_UYVY ||
		mOutputType == MEDIASUBTYPE_RGB555 ||
		mOutputType == MEDIASUBTYPE_RGB565 )
	{
		pprop->cbBuffer = mnVideoHeight * mnVideoWidth * 2;
	}
	else if(mOutputType == MEDIASUBTYPE_IYUV ||
		mOutputType == MEDIASUBTYPE_YV12 ||
		mOutputType == MEDIASUBTYPE_NV12)
	{
		pprop->cbBuffer = mnVideoHeight * mnVideoWidth * 3 / 2;
	}
	else if(mOutputType == MEDIASUBTYPE_RGB24)
	{
		pprop->cbBuffer = mnVideoHeight * mnVideoWidth * 3;
	}
	else if(mOutputType == MEDIASUBTYPE_RGB32)
	{
		pprop->cbBuffer = mnVideoHeight * mnVideoWidth * 4;
	}
	else
	{
		return E_FAIL;
	}


	// Ask the allocator to reserve us some sample memory, NOTE the function
	// can succeed (that is return NOERROR) but still not have allocated the
	// memory that we requested, so we must check we got whatever we wanted
	ALLOCATOR_PROPERTIES Actual;
	hr = pAllocator->SetProperties(pprop, &Actual);
	if (FAILED(hr))
		return hr;

	// ASSERT (Actual.cBuffers == 1);
	if (pprop->cBuffers > Actual.cBuffers || pprop->cbBuffer > Actual.cbBuffer)
		return E_FAIL;

	return NOERROR;
}

HRESULT CFilterVideoDecoder::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	if(iPosition < 0 || iPosition > 3)
		return VFW_S_NO_MORE_ITEMS;

	pMediaType->majortype = MEDIATYPE_Video;
	pMediaType->formattype = FORMAT_VideoInfo;

	VIDEOINFOHEADER *pVideoInfo = (VIDEOINFOHEADER *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
	if(pVideoInfo == NULL)
		return E_OUTOFMEMORY;

	
	Set(pVideoInfo , 0 , sizeof(VIDEOINFOHEADER));
	SetRect(&pVideoInfo->rcSource , 0 , 0 , mnVideoWidth , mnVideoHeight);
	SetRect(&pVideoInfo->rcTarget , 0 , 0 , mnVideoWidth , mnVideoHeight);

	pVideoInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pVideoInfo->bmiHeader.biHeight = mnVideoHeight;
	pVideoInfo->bmiHeader.biWidth  = mnVideoWidth;

	pMediaType->bFixedSizeSamples = TRUE;
	pMediaType->bTemporalCompression = FALSE;

	if(iPosition == 0)
	{
		pMediaType->subtype = MEDIASUBTYPE_WYUV;
		pMediaType->lSampleSize = sizeof(VO_VIDEO_BUFFER);
		pVideoInfo->bmiHeader.biBitCount = 0;
	}
	else if(iPosition == 1)
	{
		pMediaType->subtype = MEDIASUBTYPE_YV12;
		pMediaType->lSampleSize = mnVideoWidth * mnVideoHeight * 3 / 2;
		pVideoInfo->bmiHeader.biBitCount = 12;
		pVideoInfo->bmiHeader.biSizeImage = pMediaType->lSampleSize;
	}
	else if(iPosition == 2)
	{
		pMediaType->subtype = MEDIASUBTYPE_YUY2;
		pMediaType->lSampleSize = mnVideoWidth * mnVideoHeight *  2;
		pVideoInfo->bmiHeader.biBitCount = 16;
		pVideoInfo->bmiHeader.biSizeImage = pMediaType->lSampleSize;
	}
	else if(iPosition == 3)
	{
		pMediaType->subtype = MEDIASUBTYPE_RGB565;
		int nStride = (mnVideoWidth + 1) / 2 * 2;
		pMediaType->lSampleSize = nStride * mnVideoHeight *  2;
		pVideoInfo->bmiHeader.biBitCount = 16;
		pVideoInfo->bmiHeader.biSizeImage = pMediaType->lSampleSize;
	}
	else
		return E_FAIL;

	return S_OK;
}

HRESULT CFilterVideoDecoder::StartStreaming()
{
	CAutoLock lockit(&mcsDecoder);
	if(mpDecoder == NULL)
	{
		mpDecoder = new CVideoDecoder(g_hInst , mnCodec , &g_memOP);
		if(mpDecoder == NULL)
			return E_OUTOFMEMORY;

		mpDecoder->SetFourCC(mnFourCC);

		VO_VIDEO_FORMAT videoFormat;
		Set(&videoFormat , 0 , sizeof(VO_VIDEO_FORMAT));

		videoFormat.Height = mnVideoHeight;
		videoFormat.Width  = mnVideoWidth;

		VO_U32 nRC = mpDecoder->Init(mpExtData , mnExtDataLen , &videoFormat);
		if(nRC != VO_ERR_NONE)
			return E_FAIL;
	}

	if(mpDecoder == NULL)
		return E_FAIL;

	mpDecoder->Start();

	return CTransformFilter::StartStreaming();
}

HRESULT CFilterVideoDecoder::StopStreaming()
{
	if(mpDecoder != NULL)
	{
		CAutoLock lockit(&mcsDecoder);
		mpDecoder->Stop();
		delete mpDecoder;
		mpDecoder = NULL;
	}
	
	return CTransformFilter::StartStreaming();
}

 HRESULT CFilterVideoDecoder::Receive(IMediaSample *pSample)
{
	BYTE *pInputBuf = NULL;
	long nDataLen = pSample->GetActualDataLength();
	if(nDataLen <= 0)
		return S_OK;

	
	HRESULT hr = pSample->GetPointer(&pInputBuf);
	if(hr != S_OK || pInputBuf == NULL)
		return E_FAIL;

	LONGLONG llStart , llStop;
	pSample->GetTime(&llStart , &llStop);

	//VOLOGI("Receive sample data len = %d  timestamp = %d" , nDataLen , (int)(llStart / 10000));

	VO_CODECBUFFER cbBuffer ;
	cbBuffer.Buffer = pInputBuf;
	cbBuffer.Length = nDataLen;
	cbBuffer.Time = llStart / 10000;

	CAutoLock lockit(&mcsDecoder);
	VO_U32 nRC = mpDecoder->SetInputData(&cbBuffer);
	if(nRC != VO_ERR_NONE)
		return S_OK;

	VO_VIDEO_BUFFER voVBuf ;
	VO_VIDEO_FORMAT voVFormat;
	Set(&voVFormat , 0 , sizeof(VO_VIDEO_FORMAT));

	VO_BOOL bContinuous = VO_FALSE;
	while(mpDecoder->GetOutputData(&voVBuf , &voVFormat , &bContinuous) == VO_ERR_NONE)
	{
		if(voVFormat.Height > 0 && voVFormat.Width > 0 && voVBuf.Buffer[0] != NULL)
		{
			IMediaSample *pOutSample = NULL;

			hr = m_pOutput->GetDeliveryBuffer(&pOutSample , NULL , NULL , 0);
			if(hr != S_OK)
				return E_FAIL;

			//llStart = voVBuf.Time;
			llStop = llStart + 1;

			BYTE *pOutBuf = NULL;
			pOutSample->GetPointer(&pOutBuf);
			pOutSample->SetTime(&llStart , &llStop);

			//VOLOGI("Start time = %d " , (int)(llStart / 10000));

			if(mOutputType == MEDIASUBTYPE_WYUV)
			{
				Copy(pOutBuf , &voVBuf , sizeof(VO_VIDEO_BUFFER ));
				pOutSample->SetActualDataLength(sizeof(VO_VIDEO_BUFFER ));
			}
			else
			{
				long nBufSize = 0;

				CopyRawData(&voVBuf , pOutBuf , pOutSample->GetSize());
				pOutSample->SetActualDataLength(mnOutImageSize);
			}

			hr = m_pOutput->Deliver(pOutSample);
			pOutSample->Release();
		}
	}

	return S_OK;
}

HRESULT CFilterVideoDecoder::CheckConnect(PIN_DIRECTION dir,IPin *pPin)
{
	return CTransformFilter::CheckConnect(dir , pPin);
}

HRESULT CFilterVideoDecoder::BreakConnect(PIN_DIRECTION dir)
{
	return CTransformFilter::BreakConnect(dir);
}

HRESULT CFilterVideoDecoder::EndOfStream(void)
{
	return CTransformFilter::EndOfStream();
}

HRESULT CFilterVideoDecoder::BeginFlush(void)
{
	return CTransformFilter::BeginFlush();
}

HRESULT CFilterVideoDecoder::EndFlush(void)
{
	if(mpDecoder != NULL)
	{
		CAutoLock lockit(&mcsDecoder);
		mpDecoder->Flush();
	}

	return CTransformFilter::EndFlush();
}

HRESULT CFilterVideoDecoder::NewSegment(	REFERENCE_TIME tStart,	REFERENCE_TIME tStop,double dRate)
{
	VOLOGI("&&& NewSegment : %lld " , tStart);
	return CTransformFilter::NewSegment(tStart , tStop , dRate);
}

HRESULT CFilterVideoDecoder::CopyRawData(VO_VIDEO_BUFFER *vopVBuf , BYTE *pOutBuf , int nOutBufLen )
{
	if(nOutBufLen < mnOutImageSize)
		return E_FAIL;

	BYTE *pSource = vopVBuf->Buffer[0];
	BYTE *pDest	  = pOutBuf;

	int nLoops = 0;
	int nCopySize = 0;
	if(mOutputType == MEDIASUBTYPE_YV12)
	{
		nLoops = mnOutVideoHeight;
		nCopySize = mnOutVideoWidth;
		for(int i = 0 ; i < nLoops ; i++)
		{
			Copy(pDest , pSource , nCopySize);
			pSource += vopVBuf->Stride[0];
			pDest += nCopySize;
		}

		pSource = vopVBuf->Buffer[1];
		nLoops = mnOutVideoHeight / 2;
		nCopySize = mnOutVideoWidth / 2;
		for(int i = 0 ; i < nLoops ; i++)
		{
			Copy(pDest , pSource , nCopySize);
			pSource += vopVBuf->Stride[1];
			pDest += nCopySize;
		}

		pSource = vopVBuf->Buffer[2];
		nLoops = mnOutVideoHeight / 2;
		nCopySize = mnOutVideoWidth / 2;
		for(int i = 0 ; i < nLoops ; i++)
		{
			Copy(pDest , pSource , nCopySize);
			pSource += vopVBuf->Stride[2];
			pDest += nCopySize;
		}
	}
	else
	{
		if(mpCCRRRR == NULL)
		{
			VO_IV_COLORTYPE nInColor = VO_COLOR_YUV_PLANAR420;
			VO_IV_COLORTYPE nOutColor = VO_COLOR_YUV_PLANAR420;

			if(mOutputType == MEDIASUBTYPE_YUY2)
			{
				nOutColor = VO_COLOR_YUYV422_PACKED;
			}
			else if(mOutputType == MEDIASUBTYPE_RGB565)
			{
				nOutColor = VO_COLOR_RGB565_PACKED;
			}

			mpCCRRRR = new CCCRRRFunc(VO_TRUE , NULL);
			mpCCRRRR->SetColorType(nInColor , nOutColor);
			mpCCRRRR->SetCCRRSize((VO_U32 *)&mnVideoWidth , (VO_U32 *)&mnVideoHeight , (VO_U32 *)&mnVideoWidth , (VO_U32 *)&mnVideoHeight , VO_RT_DISABLE);
		}

		VO_VIDEO_BUFFER voOutVideoBuf;
		Set(&voOutVideoBuf , 0 , sizeof(VO_VIDEO_BUFFER ));

		if(mOutputType == MEDIASUBTYPE_YUY2)
		{
			voOutVideoBuf.Buffer[0] = pOutBuf;
			voOutVideoBuf.Stride[0] = mnOutVideoWidth * 2;
		}
		else if(mOutputType == MEDIASUBTYPE_RGB565)
		{
			voOutVideoBuf.Buffer[0] = pOutBuf;
			voOutVideoBuf.Stride[0] = mnOutVideoWidth * 2;
		}

		if(mpCCRRRR)
		{
			mpCCRRRR->Process(vopVBuf , &voOutVideoBuf , 0 , VO_FALSE);
		}
	}

	return S_OK;
}

STDMETHODIMP CFilterVideoDecoder::Stop()
{
	HRESULT hr = CTransformFilter::Stop();

	VOLOGI("&&& Video decoder STOP return %x " , hr);
	return hr;
}

STDMETHODIMP CFilterVideoDecoder::Pause()
{
	HRESULT hr = CTransformFilter::Pause();
	VOLOGI("&&& Video decoder PAUSE return %x " , hr);
	return hr;
}

STDMETHODIMP CFilterVideoDecoder::Run(REFERENCE_TIME tStart)
{
	HRESULT hr = CTransformFilter::Run(tStart);
	VOLOGI("&&& Video decoder RUN return %x " , hr);
	return hr;
}