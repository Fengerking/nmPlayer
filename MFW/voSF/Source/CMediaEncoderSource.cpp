#include "CMediaEncoderSource.h"

#include "voAudio.h"
#include "voVideo.h"
#include "cmnMemory.h"
#include "voOSFunc.h"


#define LOG_TAG "CMediaDecoderSource"
#include "voLog.h"

CMediaEncoderSource::CMediaEncoderSource(void * pMediaSource,  VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP , void *pMetaData)
: CBaseMediaSource (pMediaBufferOP, pMetaDataOP)
, m_pMediaSource((VOSF_MEDIASOURCETYPE *)pMediaSource)
, mbIsAudio(true)
, mpVideoEncoder(NULL)
, mnVideoCoding(VO_VIDEO_CodingUnused)
, mnVideoHeight(0)
, mnVideoWidth(0)
, mpAudioEncoder(NULL)
, mnAudioCoding(VO_AUDIO_CodingUnused)
, mnAudioSampleRate (0)
, mnAudioChannels(0)
, mnAudioBitsPerSample(16)
, mnAudioBitrate(0)
, mpMeidaBuffer(NULL)
, mpInputBuffer(NULL)
, mnOutBufSize(0)
, mnReadBufferSize(0)
, mllAudioTime(0)
{
	cmnMemFillPointer('SFPS');

	memset(&msReadBuffer , 0 , sizeof(VO_CODECBUFFER));
	memset(&msOutBuffer , 0 , sizeof(VO_CODECBUFFER));
	memset(&msAudioOutputInfo , 0 , sizeof(VO_AUDIO_OUTPUTINFO));
	memset(&msYUV420Buffer, 0, sizeof(VO_CODECBUFFER));

	if(GetCodecProfile(pMetaData) == VOSF_ERR_NONE)
	{
		InitCodec();
	}
}

CMediaEncoderSource::~CMediaEncoderSource(void)
{
	if (mpVideoEncoder != NULL)
	{
		mpVideoEncoder->Uninit ();
		delete mpVideoEncoder;
	}
	if (mpAudioEncoder != NULL)
	{
		mpAudioEncoder->Uninit ();
		delete mpAudioEncoder;
	}

	if(msOutBuffer.Buffer != NULL)
	{
		delete [] msOutBuffer.Buffer;
		 msOutBuffer.Buffer = NULL;
	}

	if (mpMeidaBuffer != NULL)
	{
		m_pMediaBufferOP->setObserver (mpMeidaBuffer, NULL);
		m_pMediaBufferOP->release (mpMeidaBuffer);
		mpMeidaBuffer = NULL;
	}
	if (mpInputBuffer != NULL)
	{
		m_pMediaBufferOP->release (mpInputBuffer);
		mpInputBuffer = NULL;
	}

	if(msYUV420Buffer.Buffer)
	{
		delete msYUV420Buffer.Buffer;
		msYUV420Buffer.Buffer = NULL;
	}
}

void		CMediaEncoderSource::signalBufferReturned (void * pBuffer)
{

}

int			CMediaEncoderSource::Start (void * pParam)
{
	if (mpInputBuffer != NULL)
	{
		m_pMediaBufferOP->release (mpInputBuffer);
		mpInputBuffer = NULL;
	}

	int nRC = m_pMediaSource->start (m_pMediaSource->pMediaSource, pParam);

	VOLOGI ("start return %d", nRC);

	return nRC;
}

int			CMediaEncoderSource::Stop (void)
{
	if (mpInputBuffer != NULL)
	{
		m_pMediaBufferOP->release (mpInputBuffer);
		mpInputBuffer = NULL;
	}

	int nRC =  m_pMediaSource->stop (m_pMediaSource->pMediaSource);

	VOLOGI ("stop return %d", nRC);

	return nRC;
}

void *		CMediaEncoderSource::GetFormat (void)
{
	void * pMetaData = m_pMetaDataOP->create ();
	if (mbIsAudio)
	{
		m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , mstrMIMEType);
		m_pMetaDataOP->setInt32(pMetaData, kKeySampleRate , mnAudioSampleRate);
		m_pMetaDataOP->setInt32(pMetaData, kKeyChannelCount , mnAudioChannels);
	}
	else
	{
		m_pMetaDataOP->setCString(pMetaData, kKeyMIMEType , mstrMIMEType);
		m_pMetaDataOP->setInt32(pMetaData, kKeyWidth , mnVideoWidth);
		m_pMetaDataOP->setInt32(pMetaData, kKeyHeight , mnVideoHeight);	
	}

	return pMetaData;
}

int			CMediaEncoderSource::Read (void ** ppBuffer, void * pOptions)
{
	if(m_pMediaSource == NULL)
		return VOSF_ERR_FAILED;

	if(mbIsAudio)
	{
		if(EncodeAudio(pOptions))
			*ppBuffer = mpMeidaBuffer;	
		m_pMediaBufferOP->add_ref(mpMeidaBuffer);
	}
	else
	{
		if(EncodeVideo(pOptions))
			*ppBuffer = mpMeidaBuffer;
		m_pMediaBufferOP->add_ref(mpMeidaBuffer);
	}

	return VOSF_ERR_NONE;
}

VO_CODECBUFFER *	CMediaEncoderSource::ReadSource(void * pOptions)
{
	if(mpInputBuffer != NULL)
	{
		m_pMediaBufferOP->release (mpInputBuffer);
		mpInputBuffer = NULL;
	}

	int nRC = m_pMediaSource->read (m_pMediaSource->pMediaSource, &mpInputBuffer, pOptions);
	if(nRC != VO_ERR_NONE)
	{
		if(nRC == ERROR_END_OF_STREAM)
		{
			VOLOGI ("Read at end of track!");
			return false;
		}
		else
		{
			VOLOGE ("Read data from track error %08X!", nRC);
			return false;
		}
	}

	m_pMetaDataOP->findInt64 (m_pMediaBufferOP->meta_data (mpInputBuffer), kKeyTime, &mllMediaTime);

	msReadBuffer.Buffer = (VO_PBYTE)m_pMediaBufferOP->data (mpInputBuffer) + m_pMediaBufferOP->range_offset (mpInputBuffer);
	msReadBuffer.Length = m_pMediaBufferOP->range_length (mpInputBuffer);
	msReadBuffer.Time   = mllMediaTime;

	return &msReadBuffer;
}

bool			CMediaEncoderSource::EncodeAudio(void * pOptions)
{
	if(msOutBuffer.Buffer == NULL)
	{
		mnOutBufSize = mnAudioChannels * mnAudioSampleRate / 5;
		msOutBuffer.Buffer = new VO_BYTE[mnOutBufSize];
	}

	msOutBuffer.Length = mnOutBufSize;
	int nRC = mpAudioEncoder->GetOutputData(&msOutBuffer , &msAudioOutputInfo);
	if(nRC != VOSF_ERR_NONE)
	{
		do 
		{
			VO_CODECBUFFER  *pInput = ReadSource(pOptions);
			mpAudioEncoder->SetInputData(pInput);
			mnReadBufferSize = pInput->Length;
			mllAudioTime	 = mllMediaTime;
			msOutBuffer.Length = mnOutBufSize;
			nRC = mpAudioEncoder->GetOutputData(&msOutBuffer , &msAudioOutputInfo);
		} while (nRC != VOSF_ERR_NONE);
	}
	if (mpMeidaBuffer == NULL)
	{
		mpMeidaBuffer = m_pMediaBufferOP->create (msOutBuffer.Buffer, mnOutBufSize);
		if (mpMeidaBuffer == NULL)
			return false;
		m_pMediaBufferOP->setObserver(mpMeidaBuffer, this);
	}

	m_pMediaBufferOP->set_range (mpMeidaBuffer, 0, msOutBuffer.Length);

	m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (mpMeidaBuffer));
	m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (mpMeidaBuffer), kKeyTime,mllAudioTime);
	mllAudioTime = 	mllMediaTime + 
		(long long)msAudioOutputInfo.InputUsed*1000000/(mnAudioSampleRate*mnAudioChannels*(mnAudioBitsPerSample>>3));
	return true;
}

bool			CMediaEncoderSource::EncodeVideo(void * pOptions)
{
	if(msOutBuffer.Buffer == NULL)
	{
		mnOutBufSize = mnVideoHeight * mnVideoWidth;
		msOutBuffer.Buffer = new VO_BYTE[mnOutBufSize];
	}

	int nRC = VOSF_ERR_NONE;
	VO_VIDEO_FRAMETYPE  videoFrame;

	do 
	{
		VO_CODECBUFFER  *pInput = ReadSource(pOptions);
		
		if(msYUV420Buffer.Buffer== NULL)
			msYUV420Buffer.Buffer = new VO_BYTE[pInput->Length];

		YUV420SPTO420Planar(pInput, &msYUV420Buffer,mnVideoWidth, mnVideoHeight);

		msYUV420Buffer.Length = pInput->Length;
		pInput = &msYUV420Buffer;		
		
		VO_VIDEO_BUFFER videoBuf;
		videoBuf.Buffer[0] = pInput->Buffer;
		videoBuf.Buffer[1] = pInput->Buffer + mnVideoWidth * mnVideoHeight;
		videoBuf.Buffer[2] = pInput->Buffer + mnVideoWidth * mnVideoHeight * 5 / 4;

		videoBuf.Stride[0] = mnVideoWidth;
		videoBuf.Stride[1] = videoBuf.Stride[2] = mnVideoWidth;
		videoBuf.ColorType = VO_COLOR_YUV_PLANAR420;

		msOutBuffer.Length = mnOutBufSize;		
		nRC = mpVideoEncoder->Process(&videoBuf  , &msOutBuffer , &videoFrame);	
	} while (nRC != VOSF_ERR_NONE);	

	if (mpMeidaBuffer == NULL)
	{
		mpMeidaBuffer = m_pMediaBufferOP->create (msOutBuffer.Buffer, mnOutBufSize);
		if (mpMeidaBuffer == NULL)
			return false;
		m_pMediaBufferOP->setObserver(mpMeidaBuffer, this);
	}

	m_pMediaBufferOP->set_range (mpMeidaBuffer, 0, msOutBuffer.Length);

	m_pMetaDataOP->clear (m_pMediaBufferOP->meta_data (mpMeidaBuffer));
	m_pMetaDataOP->setInt64 (m_pMediaBufferOP->meta_data (mpMeidaBuffer), kKeyTime, mllMediaTime);
	if(videoFrame == VO_VIDEO_FRAME_I)
	{
		int nKeyFrame = 1;
		m_pMetaDataOP->setInt32 (m_pMediaBufferOP->meta_data (mpMeidaBuffer), kKeyIsSyncFrame, nKeyFrame);
	}
	
	return true;
}

int			CMediaEncoderSource::GetCodecProfile(void *pMetaData)
{
	if(m_pMetaDataOP == NULL || pMetaData == NULL)
		return VOSF_ERR_FAILED;

	const char * pMIME = NULL;
	m_pMetaDataOP->findCString(pMetaData , kKeyMIMEType , &pMIME);
	if(pMIME == NULL)
		return VOSF_ERR_FAILED;

	memset(mstrMIMEType , 0 , sizeof(char) * 256);
	strcpy(mstrMIMEType , pMIME);

	if (strstr (pMIME, "audio/") == NULL)
	{
		mbIsAudio = false;
		if(strstr(pMIME , "mp4v-es") != NULL)
		{
			mnVideoCoding = VO_VIDEO_CodingMPEG4;
		}
		else if(strstr(pMIME , "3gpp") != NULL)
		{
			mnVideoCoding = VO_VIDEO_CodingH263;
		}
	}
	else
	{
		mbIsAudio = true;
		if(strstr(pMIME , "evrc") != NULL)
		{
			mnAudioCoding = VO_AUDIO_CodingEVRC;
		}
		else if(strstr(pMIME , "qcelp") != NULL)
		{
			mnAudioCoding = VO_AUDIO_CodingQCELP13;
		}
		else if(strstr(pMIME , "3gpp")  != NULL)
		{
			mnAudioCoding = VO_AUDIO_CodingAMRNB;
		}
		else if(strstr(pMIME , "amr-wb")  != NULL)
		{
			mnAudioCoding = VO_AUDIO_CodingAMRWB;
		}
	}

	if(mbIsAudio)
	{
		m_pMetaDataOP->findInt32(pMetaData , kKeyChannelCount , &mnAudioChannels);
		m_pMetaDataOP->findInt32(pMetaData , kKeySampleRate , &mnAudioSampleRate);
		m_pMetaDataOP->findInt32(pMetaData , kKeyBitRate , &mnAudioBitrate);
	}
	else 
	{
		m_pMetaDataOP->findInt32(pMetaData , kKeyWidth , &mnVideoWidth);
		m_pMetaDataOP->findInt32(pMetaData , kKeyHeight , &mnVideoHeight);
	}


	return VOSF_ERR_NONE;
}

int			CMediaEncoderSource::InitCodec()
{
	if(mbIsAudio)
	{
		if(mpAudioEncoder != NULL)
		{
			delete mpAudioEncoder;
			mpAudioEncoder = NULL;
		}

		mpAudioEncoder = new CAudioEncoder(NULL , mnAudioCoding , &g_memOP);
		if(mpAudioEncoder == NULL)
			return VOSF_ERR_FAILED;

		VO_AUDIO_FORMAT audFmt;
		memset(&audFmt , 0 , sizeof(VO_AUDIO_FORMAT));
		audFmt.Channels = mnAudioChannels;
		audFmt.SampleBits =  mnAudioBitsPerSample;
		audFmt.SampleRate =  mnAudioSampleRate;
		VO_U32 nRC = mpAudioEncoder->Init(&audFmt);
		if(nRC != 0)
		{
			VOLOGE("Init audio encoder fails, ret=%d", nRC);
			return nRC;
		}

	}
	else
	{
		if(mpVideoEncoder != NULL)
		{
			delete mpVideoEncoder;
			mpVideoEncoder = NULL;
		}

		mpVideoEncoder =  new CVideoEncoder(NULL , mnVideoCoding , &g_memOP);
		if(mpVideoEncoder == NULL)
			return VOSF_ERR_FAILED;

		VO_VIDEO_FORMAT vidFmt;
		memset(&vidFmt , 0 , sizeof(VO_VIDEO_FORMAT));
		vidFmt.Height = mnVideoHeight;
		vidFmt.Width  = mnVideoWidth;

		VO_U32 nRC = mpVideoEncoder->Init();
		if(nRC != 0)
		{
			VOLOGE("Init video encoder fails, ret= %d", nRC);
			return nRC;
		}		
	}

	if (mpInputBuffer != NULL)
	{
		m_pMediaBufferOP->release (mpInputBuffer);
		mpInputBuffer = NULL;
	}

	if(mpMeidaBuffer != NULL)
	{
		m_pMediaBufferOP->release(mpMeidaBuffer);
		mpMeidaBuffer = NULL;
	}

	return VOSF_ERR_NONE;
}

bool CMediaEncoderSource::YUV420SPTO420Planar(VO_CODECBUFFER * pBufferIn,VO_CODECBUFFER * pBufferOut,  int nWidth, int nHeight)
{
	if(pBufferIn == NULL|| pBufferOut == NULL)
		return false;
	memcpy(pBufferOut->Buffer, pBufferIn->Buffer, nWidth*nHeight);

	VO_PBYTE pDataIn = pBufferIn->Buffer +  nWidth*nHeight;
	VO_PBYTE pDataOut= pBufferOut->Buffer+  nWidth*nHeight;
	int nSize = nWidth*nHeight/4;
	for (int i=0; i<nSize; i++, pDataOut++, pDataIn +=2)
	{
		*pDataOut = *(pDataIn+1);
		*(pDataOut+nSize) = *(pDataIn);
	}
	return true;
}
