#include "CBaseTrack.h"
#include "initguid.h"
#include "voGuids.h"
#include "voLog.h"

CBaseTrack::CBaseTrack( VO_MEM_OPERATOR *pMemOpt ,VO_PTR hFileHandle , VO_SOURCE_READAPI *pReadAPI ,  int nTrackIndex)
: CMemoryOpt(pMemOpt , VO_INDEX_FILTER_SOURCE)
, mnTrackIndex(nTrackIndex)
, mpReadAPI(pReadAPI)
, mhFileHandle(hFileHandle)
, mnExtDataSize(0)
, mpExtData(NULL)
, mnTrackType(VO_SOURCE_TT_MAX)
, mnCodec(0)
, mllDuration(0)
, mllStartTime(0)
{
}

CBaseTrack::~CBaseTrack(void)
{
}

HRESULT	CBaseTrack::InitTrack()
{
	if(mpReadAPI == NULL || mhFileHandle == NULL)
		return E_FAIL;

	mnExtDataSize = 0;
	if(mpExtData != NULL)
	{
		Free(mpExtData);
		mpExtData = NULL;
	}

	VO_SOURCE_TRACKINFO trackInfo;
	VO_U32 nRC = mpReadAPI->GetTrackInfo(mhFileHandle , mnTrackIndex , &trackInfo);
	if(nRC != VO_ERR_SOURCE_OK)
		return E_FAIL;

	mnCodec = trackInfo.Codec;
	mnTrackType = trackInfo.Type;

	mllDuration = trackInfo.Duration ;
	mllStartTime = trackInfo.Start;

	mllDuration *= 10000;
	mllStartTime *= 10000;

	if(trackInfo.HeadSize > 0)
	{
		mnExtDataSize = trackInfo.HeadSize;
		mpExtData = (BYTE *)Alloc(mnExtDataSize);
		if(mpExtData == NULL)
			return E_OUTOFMEMORY;

		Copy(mpExtData , trackInfo.HeadData , mnExtDataSize);
	}

	LONGLONG llPos = 0;
	return SetPos(llPos);
}

HRESULT CBaseTrack::GetMediaType(CMediaType *pMT)
{
	if(mpReadAPI == NULL || mhFileHandle == NULL)
		return E_FAIL;

	if(mnTrackType == VO_SOURCE_TT_VIDEO || mnTrackType == VO_SOURCE_TT_RTSP_VIDEO)
	{
	
		pMT->majortype	= MEDIATYPE_Video;
		pMT->subtype	= MEDIASUBTYPE_NULL;

		switch(mnCodec)
		{
		case VO_VIDEO_CodingMPEG2:       /*!< AKA: H.262 */
			pMT->subtype	= MEDIASUBTYPE_MPEG2_VIDEO;
			break;
		case VO_VIDEO_CodingH263:       /*!< H.263 */
			pMT->subtype	= MEDIASUBTYPE_H263;
			break;
		case VO_VIDEO_CodingS263:       /*!< H.263 */
			pMT->subtype	= MEDIASUBTYPE_S263;
			break;
		case VO_VIDEO_CodingMPEG4:      /*!< MPEG-4 */
			pMT->subtype	= MEDIASUBTYPE_MPG4;
			break;
		case VO_VIDEO_CodingH264:      /*!< H.264/AVC */
			pMT->subtype	= MEDIASUBTYPE_H264;
			break;
		case VO_VIDEO_CodingWMV:        /*!< all versions of Windows Media Video */
			pMT->subtype	= WMMEDIASUBTYPE_WMV3;
			break;
		case VO_VIDEO_CodingRV:         /*!< all versions of Real Video */
			pMT->subtype	= MEDIASUBTYPE_RV40;
			break;
		case VO_VIDEO_CodingMJPEG:      /*!< Motion JPEG */
			pMT->subtype	= MEDIASUBTYPE_MJPG;
			break;
		case VO_VIDEO_CodingDIVX:	   /*!< DIV3 */
			pMT->subtype	= MEDIASUBTYPE_DIVX;
			break;
		case VO_VIDEO_CodingVP6:
			pMT->subtype	= MEDIASUBTYPE_VP60;
			break;
		default:
			return E_FAIL;
		}

		pMT->formattype = FORMAT_VideoInfo;

		VO_BITMAPINFOHEADER * pBmpHeader = (VO_BITMAPINFOHEADER *)Alloc(sizeof(VO_BITMAPINFOHEADER));
		Set(pBmpHeader , 0 , sizeof(VO_BITMAPINFOHEADER));
		VO_BITMAPINFOHEADER * pTmpBmpHd = NULL;
		VO_U32 nRC = mpReadAPI->GetTrackParam(mhFileHandle , mnTrackIndex , VO_PID_SOURCE_BITMAPINFOHEADER , &pTmpBmpHd);
		if(nRC == VO_ERR_SOURCE_OK)
		{
			Copy(pBmpHeader , pTmpBmpHd , sizeof(VO_BITMAPINFOHEADER));
		}
		else
		{
			VO_VIDEO_FORMAT VideoFmt;
			nRC = mpReadAPI->GetTrackParam(mhFileHandle , mnTrackIndex , VO_PID_VIDEO_FORMAT , &VideoFmt);
			if(nRC != VO_ERR_SOURCE_OK)
				return E_FAIL;

			pBmpHeader->biHeight = VideoFmt.Height;
			pBmpHeader->biWidth  = VideoFmt.Width;
			pBmpHeader->biSize = sizeof(VO_BITMAPINFOHEADER);
		}

		ULONG lFormatLen  = sizeof(VIDEOINFOHEADER) + mnExtDataSize;
		pMT->pbFormat = pMT->AllocFormatBuffer(lFormatLen);
		if(pMT->pbFormat == NULL)
			return E_OUTOFMEMORY;

		Set(pMT->pbFormat , 0 , pMT->cbFormat);

		VIDEOINFOHEADER *pVideoInfo = (VIDEOINFOHEADER *)pMT->pbFormat;

		SetRect(&pVideoInfo->rcSource , 0 , 0 , pBmpHeader->biWidth , pBmpHeader->biHeight);
		SetRect(&pVideoInfo->rcTarget , 0 , 0 , pBmpHeader->biWidth , pBmpHeader->biHeight);

		Copy(&pVideoInfo->bmiHeader , pBmpHeader , sizeof(VO_BITMAPINFOHEADER));
		
		if(mnExtDataSize > 0)
		{
			Copy(pMT->pbFormat + sizeof(VIDEOINFOHEADER) , mpExtData , mnExtDataSize);
		}

		pMT->bFixedSizeSamples = FALSE;
		pMT->bTemporalCompression  = TRUE;
		mpReadAPI->GetTrackParam(mhFileHandle , mnTrackIndex , VO_PID_SOURCE_MAXSAMPLESIZE , &pMT->lSampleSize);

		Free(pBmpHeader);

		return S_OK;

	}
	else if(mnTrackType == VO_SOURCE_TT_AUDIO || mnTrackType == VO_SOURCE_TT_RTSP_AUDIO)
	{
		pMT->majortype = MEDIATYPE_Audio;

		VO_WAVEFORMATEX	 *pWaveForamt = (VO_WAVEFORMATEX *)Alloc(sizeof(VO_WAVEFORMATEX));
		void *pTemp = NULL;
		VO_U32 nRC = mpReadAPI->GetTrackParam(mhFileHandle , mnTrackIndex , VO_PID_SOURCE_WAVEFORMATEX , &pTemp);
		if(nRC == VO_ERR_SOURCE_OK)
		{
			Copy(pWaveForamt , pTemp , sizeof(VO_WAVEFORMATEX));
		}
		else
		{
			VO_AUDIO_FORMAT voAudFmt;
			mpReadAPI->GetTrackParam(mhFileHandle , mnTrackIndex , VO_PID_AUDIO_FORMAT , &voAudFmt);
			pWaveForamt->cbSize = 0;
			pWaveForamt->nChannels = voAudFmt.Channels;
			pWaveForamt->nSamplesPerSec = voAudFmt.SampleRate;
			pWaveForamt->wBitsPerSample = voAudFmt.SampleBits;
			pWaveForamt->nBlockAlign = voAudFmt.SampleBits * voAudFmt.Channels / 8;
			pWaveForamt->nAvgBytesPerSec = pWaveForamt->nBlockAlign * pWaveForamt->nSamplesPerSec;
		}

		switch(mnCodec)
		{
		case VO_AUDIO_CodingPCM:         /**< Any variant of PCM coding */
			pMT->subtype	= MEDIASUBTYPE_PCM;
			break;
		case VO_AUDIO_CodingADPCM:       /**< Any variant of ADPCM encoded data */
			pMT->subtype	= MEDIASUBTYPE_DVD_LPCM_AUDIO;
			pWaveForamt->wFormatTag = 0x1;
			break;
		case VO_AUDIO_CodingAMRNB:       /**< Any variant of AMR encoded data */
			pMT->subtype	= MEDIASUBTYPE_VOAMR;
			break;
		case VO_AUDIO_CodingAMRWB:       /**< Any variant of AMR encoded data */
			pMT->subtype	= MEDIASUBTYPE_AMRWB;
			break;
		case VO_AUDIO_CodingAMRWBP:      /**< Any variant of AMR encoded data */
			pMT->subtype	= MEDIASUBTYPE_AMRWBP;
			break;
		case VO_AUDIO_CodingQCELP13:     /**< Any variant of QCELP 13kbps encoded data */
			pMT->subtype	= MEDIASUBTYPE_VOQCP;
			break;
		case VO_AUDIO_CodingEVRC:        /**< Any variant of EVRC encoded data */
			pMT->subtype	= MEDIASUBTYPE_VOEVRC;
			break;
		case VO_AUDIO_CodingAAC:         /**< Any variant of AAC encoded data, 0xA106 - ISO/MPEG-4 AAC, 0xFF - AAC */
			pMT->subtype	= MEDIASUBTYPE_VOAAC;
			break;
		case VO_AUDIO_CodingAC3:         /**< Any variant of AC3 encoded data */
			pMT->subtype	= MEDIASUBTYPE_DOLBY_AC3;
			break;
		case VO_AUDIO_CodingFLAC:        /**< Any variant of FLAC encoded data */
			pMT->subtype	= MEDIASUBTYPE_FLAC;
			break;
		case VO_AUDIO_CodingMP1:			/**< Any variant of MP1 encoded data */
			pMT->subtype	= MEDIASUBTYPE_MPEG2_AUDIO;
			break;
		case VO_AUDIO_CodingMP3:         /**< Any variant of MP3 encoded data */
			pMT->subtype	= MEDIASUBTYPE_MP3;
			break;
		case VO_AUDIO_CodingOGG:         /**< Any variant of OGG encoded data */
			pMT->subtype	= MEDIASUBTYPE_Ogg;
			break;
		case VO_AUDIO_CodingWMA:         /**< Any variant of WMA encoded data */
			pMT->subtype	= WMMEDIASUBTYPE_WMAudioV9;
			break;
		case VO_AUDIO_CodingRA:          /**< Any variant of RA encoded data */
			pMT->subtype	= MEDIASUBTYPE_REALAUDIO;
			break;
		case VO_AUDIO_CodingMIDI:        /**< Any variant of MIDI encoded data */
			pMT->subtype	= MEDIATYPE_Midi;
			break;
		case VO_AUDIO_CodingDRA:        /**< Any variant of dra encoded data */
			pMT->subtype	= MEDIASUBTYPE_DRA;
			break;
		case VO_AUDIO_CodingG729:        /**< Any variant of dra encoded data */
			pMT->subtype	= MEDIASUBTYPE_G729;
			break;
		case VO_AUDIO_CodingEAC3:		/**< Any variant of Enhanced AC3 encoded data */
			pMT->subtype	= MEDIASUBTYPE_EAC3;
			break;
		}

		pMT->formattype = FORMAT_WaveFormatEx;
		ULONG lFormatLen  = sizeof(WAVEFORMATEX) + mnExtDataSize;
		pMT->pbFormat = pMT->AllocFormatBuffer(lFormatLen);
		if(pMT->pbFormat == NULL)
			return E_OUTOFMEMORY;

		Set(pMT->pbFormat , 0 , pMT->cbFormat);
		Copy(pMT->pbFormat , pWaveForamt , sizeof(WAVEFORMATEX));

		if(mnExtDataSize > 0)
		{
			Copy(pMT->pbFormat + sizeof(VO_WAVEFORMATEX) , mpExtData , mnExtDataSize);
		}

		
		pMT->bFixedSizeSamples = FALSE;
		pMT->bTemporalCompression  = TRUE;
		mpReadAPI->GetTrackParam(mhFileHandle , mnTrackIndex , VO_PID_SOURCE_MAXSAMPLESIZE , &pMT->lSampleSize);

		Free(pWaveForamt);
		return S_OK;
	}

	return E_FAIL;
}

HRESULT	CBaseTrack::GetTrackData(BYTE **ppBuf , LONG *pBufSize , LONGLONG* pllStart , LONGLONG* pllStop , LONGLONG llStreamTime)
{
	if(mpReadAPI == NULL || mhFileHandle == NULL)
		return E_FAIL;

	VO_SOURCE_SAMPLE sample ;
	sample.Time = llStreamTime;
	VO_U32 nRC = mpReadAPI->GetSample(mhFileHandle , mnTrackIndex , &sample);
	if(nRC == VO_ERR_SOURCE_OK)
	{
		//VOLOGI("&&& Get sample size = %d , time = %d" , sample.Size & 0x7FFFFFFF , (int)sample.Time);
		*ppBuf = sample.Buffer;
		*pBufSize = sample.Size;

		*pllStart = sample.Time * 10000;
		*pllStop = (sample.Time + sample.Duration) * 10000;

		return S_OK;
	}
	else if(nRC == VO_ERR_SOURCE_END)
	{
		return S_FALSE;
	}
	else
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT	CBaseTrack::SetPos(LONGLONG &llPos)
{
	if(mpReadAPI == NULL || mhFileHandle == NULL)
		return E_FAIL;

	VO_U32 nRC = mpReadAPI->SetPos(mhFileHandle , mnTrackIndex , &llPos);
	if(nRC != VO_ERR_SOURCE_OK)
		return E_FAIL;

	return S_OK;;
}

HRESULT CBaseTrack::Flush()
{
	if(mpReadAPI == NULL || mhFileHandle == NULL)
		return E_FAIL;

	return S_OK;
}


HRESULT	CBaseTrack::Stop()
{
	return E_NOTIMPL;
}

HRESULT	CBaseTrack::Pause()
{
	return E_NOTIMPL;
}

HRESULT CBaseTrack::Run()
{
	return E_NOTIMPL;
}

	
