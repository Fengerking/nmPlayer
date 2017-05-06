#pragma once
#include "CBaseMediaSource.h"
#include "CAudioEncoder.h"
#include "CVideoEncoder.h"

class CMediaEncoderSource : public CMediaBufferObserver , public CBaseMediaSource
{
public:
	CMediaEncoderSource(void * pMediaSource,  VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP , void *pMetaData);
	virtual ~CMediaEncoderSource(void);

	virtual void		signalBufferReturned (void * pBuffer);

	virtual int			Start (void * pParam);
	virtual int			Stop (void);
	virtual void *		GetFormat (void);
	virtual int			Read (void ** ppBuffer, void * pOptions);

protected:

	virtual int			GetCodecProfile(void *pMetaData);
	virtual int			InitCodec();

	virtual VO_CODECBUFFER *	ReadSource(void * pOptions);
	virtual bool				EncodeAudio(void * pOptions);
	virtual bool				EncodeVideo(void * pOptions);
	virtual bool				YUV420SPTO420Planar(VO_CODECBUFFER * pBufferIn,VO_CODECBUFFER * pBufferOut, int nWidth, int nHeight);
protected:
	VOSF_MEDIASOURCETYPE *	m_pMediaSource;
	bool				mbIsAudio;
	char				mstrMIMEType[256];

	//video profile
	CVideoEncoder *		mpVideoEncoder;
	VO_U32				mnVideoCoding;
	int 				mnVideoHeight;
	int 				mnVideoWidth;

	//audio profile
	CAudioEncoder *		mpAudioEncoder;
	VO_U32				mnAudioCoding;
	int					mnAudioSampleRate;
	int					mnAudioChannels;
	int					mnAudioBitsPerSample;
	int 				mnAudioBitrate;
	VO_AUDIO_OUTPUTINFO msAudioOutputInfo;

	void *				mpMeidaBuffer;
	void *				mpInputBuffer;

	VO_CODECBUFFER		msReadBuffer;
	VO_CODECBUFFER		msYUV420Buffer;
	long long			mllMediaTime;
	VO_CODECBUFFER		msOutBuffer;
	int					mnOutBufSize;
	int					mnReadBufferSize;
	long long			mllAudioTime;
};
