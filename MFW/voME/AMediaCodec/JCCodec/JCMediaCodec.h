#ifndef __JC_MEDIA_CODEC__H
#define __JC_MEDIA_CODEC__H

#include "JCObject.h"

class JCMediaFormat; // refer to MediaFormat
class JCMediaCrypto; // refer to MediaCrypto;
class JCBufferInfo;  // refer to MediaCodec.BufferInfo;
class JCCryptoInfo;  // refer to MediaCodec.CryptoInfo;


class JCMediaCodec : public JCObject
{
public:
	JCMediaCodec(JNIEnv* env);
	virtual ~JCMediaCodec();
	
public:
	void      configure(JCMediaFormat& inFormat, void* pSur, JCMediaCrypto* pCrypo, int flags);
	bool      createByCodecName(const char* pName);
	bool      createDecoderByType(const char* pType);
	bool      createEncoderByType(const char* pType);
	int       dequeueInputBuffer(long long llTimeUs);
	int       dequeueOutputBuffer(JCBufferInfo& info, long long llTimeUs);
	void      flush();
	void      queueInputBuffer(int index, int offset, int szie, long long llTimeUs, int flags);
	void      queueSecureInputBuffer(int index, int offset, JCCryptoInfo& info, long long llTimeUs, int flags);
	void      release();
	void      releaseOutputBuffer(int index, bool render);
	void      setVideoScalingMode(int mode);
	void      start();
	void      stop();
	jobjectArray getInputBuffers();
	jobjectArray getOutputBuffers();
	JCMediaFormat* getOutputFormat();

protected:
	bool       onInit();
private:
	JCMediaFormat*   mFormat;
	
	// the follow method are the placeholder for JavaMediaCodec
	jmethodID    mConfigure;
	jmethodID    mCreateByCodecName;
	jmethodID    mCreateDecoderByType;
	jmethodID    mCreateEncoderByType;
	jmethodID    mDequeueInputBuffer;
	jmethodID    mDequeueOutputBuffer;
	jmethodID    mFlush;
	jmethodID    mGetInputBuffers;
	jmethodID    mGetOutputBuffers;
	jmethodID    mGetOutputFormat;
	jmethodID    mQueueInputBuffer;
	jmethodID    mQueueSecureInputBuffer;
	jmethodID    mRelease;
	jmethodID    mReleaseOutputBuffer;
	jmethodID    mSetVideoScalingMode;
	jmethodID    mStart;
	jmethodID    mStop;
};

#endif //__JC_MEDIA_CODEC__H
