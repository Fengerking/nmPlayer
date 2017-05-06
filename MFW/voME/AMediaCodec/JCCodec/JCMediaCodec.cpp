#include <assert.h>
#include <stdio.h>

//#define __DEBUG
#define LOG_TAG "JCMediaCodec"

#include "JCMediaCodec.h"
#include "JCBufferInfo.h"
#include "JCCryptoInfo.h"
#include "JCMediaCrypto.h"
#include "JCMediaFormat.h"









JCMediaCodec::JCMediaCodec(JNIEnv* env):JCObject(env, "android/media/MediaCodec"),
										mFormat(NULL),
										mConfigure(NULL),
										mCreateByCodecName(NULL),
										mCreateEncoderByType(NULL),
										mDequeueInputBuffer(NULL),
										mDequeueOutputBuffer(NULL),
										mFlush(NULL),
										mGetInputBuffers(NULL),
										mGetOutputBuffers(NULL),
										mGetOutputFormat(NULL),
										mQueueInputBuffer(NULL),
										mQueueSecureInputBuffer(NULL),
										mRelease(NULL),
										mReleaseOutputBuffer(NULL),
										mSetVideoScalingMode(NULL),
										mStart(NULL),
										mStop(NULL)
{
}

JCMediaCodec::~JCMediaCodec()
{
	if(mFormat)
	{
		delete mFormat;
		mFormat = NULL;
	}

}
	

void JCMediaCodec::configure(JCMediaFormat& inFormat, void* pSur, JCMediaCrypto* pCrypto, int flags)
{
	if(!mEnv || !mClass || !mConfigure || !mObj)
		return ;
	jobject jf = inFormat.getObj();
	jobject jc = NULL;
	if(pCrypto)
		jc = pCrypto->getObj();
	jobject sur = NULL;
	if(pSur)
		sur = (jobject)pSur;
	__D("jf = %p, mObj = %p, pSur = %p, jc = %p\n", jf, mObj, pSur, jc); 
	mEnv->CallVoidMethod(mObj, mConfigure, jf, sur, jc, flags);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}

bool JCMediaCodec::createByCodecName(const char* pName)
{
	if(!mEnv || !mClass || !mCreateByCodecName || ! pName)
		return false;
	return createObjFromChar(pName, mCreateByCodecName);
}
bool JCMediaCodec::createDecoderByType(const char* pType)
{
	if(!mEnv || !mClass || !mCreateDecoderByType || !pType)
		return false;
	__D("mCreateDecoderByType = %p\n", mCreateDecoderByType);
	return createObjFromChar(pType, mCreateDecoderByType);
}

bool JCMediaCodec::createEncoderByType(const char* pType)
{
	if(!mEnv || !mClass || !mCreateEncoderByType || !pType)
		return false;
	return createObjFromChar(pType, mCreateEncoderByType);
}

int JCMediaCodec::dequeueInputBuffer(long long llTimeUs)
{
	__D("HUI_FA \n");
	__D("HUI_FA mEnv = %p\n", mEnv);
	if(!mEnv || !mDequeueInputBuffer || !mObj)
		return -1;
	__D("HUI_FA\n");
	jint index = mEnv->CallIntMethod(mObj, mDequeueInputBuffer, llTimeUs);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return -100;
	}
	return index;
}

int JCMediaCodec::dequeueOutputBuffer(JCBufferInfo& info, long long llTimeUs)
{
	__D("WHY mEnv = %p\n", mEnv);
	if(!mEnv || !mDequeueOutputBuffer || !mObj)
		return -1;
	__D("WHY mObj = %p\n", mObj);
	jint index = mEnv->CallIntMethod(mObj, mDequeueOutputBuffer, info.getObj(), llTimeUs);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return -100;
	}
	__D();
	info.update();
	__D();
	return index;
}
void JCMediaCodec::flush()
{
	if(!mEnv || !mFlush || !mObj)
		return ;
	mEnv->CallVoidMethod(mObj, mFlush);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}
void JCMediaCodec::queueInputBuffer(int index, int offset, int size, long long llTimeUs, int flags)
{
	if(!mEnv || !mClass || !mObj || !mQueueInputBuffer)
		return ;
	assert(index >= 0);
	mEnv->CallVoidMethod(mObj, mQueueInputBuffer, index, offset, size, llTimeUs, flags);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}

void JCMediaCodec::queueSecureInputBuffer(int index, int offset, JCCryptoInfo& info, long long llTimeUs, int flags)
{
	if(!mEnv || !mClass || !mObj || !mQueueSecureInputBuffer)
		return ;
	assert(index >= 0);
	__D();
	mEnv->CallVoidMethod(mObj, mQueueSecureInputBuffer, index, offset, info.getObj(), llTimeUs, flags);
	__D();
	
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}
void JCMediaCodec::release()
{
	if(!mEnv || !mClass || !mObj || !mRelease)
		return ;
	mEnv->CallVoidMethod(mObj, mRelease);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}

void JCMediaCodec::releaseOutputBuffer(int index, bool render)
{
	if(!mEnv || !mClass || !mObj || !mReleaseOutputBuffer)
		return ;
	assert(index >=0);
	jboolean jb = render;
	mEnv->CallVoidMethod(mObj, mReleaseOutputBuffer, index, jb);
	if (mEnv->ExceptionCheck()) 
	{
#if defined(__DEBUG)
		mEnv->ExceptionDescribe();
#endif
		mEnv->ExceptionClear();
	}
}

void JCMediaCodec::setVideoScalingMode(int mode)
{
	if(!mEnv || !mClass || !mObj || !mSetVideoScalingMode)
		return ;
	mEnv->CallVoidMethod(mObj, mSetVideoScalingMode, mode);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}
void JCMediaCodec::start()
{
	if(!mEnv || !mClass || !mObj || !mStart)
		return ;
	mEnv->CallVoidMethod(mObj, mStart);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}
void JCMediaCodec::stop()
{
	if(!mEnv || !mClass || !mObj || !mStop)
		return ;
	mEnv->CallVoidMethod(mObj, mStop);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
	}
}

jobjectArray JCMediaCodec::getInputBuffers()
{
	if(!mEnv || !mClass || !mObj || !mGetInputBuffers)
		return NULL;
	jobjectArray arr = (jobjectArray)mEnv->CallObjectMethod(mObj, mGetInputBuffers);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return NULL;
	}
	return arr;
}
jobjectArray JCMediaCodec::getOutputBuffers()
{
	if(!mEnv || !mClass || !mObj || !mGetOutputBuffers)
		return NULL;
	jobjectArray arr = (jobjectArray)mEnv->CallObjectMethod(mObj, mGetOutputBuffers);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return NULL;
	}
	return arr;
}
JCMediaFormat* JCMediaCodec:: getOutputFormat()
{
	if(!mEnv || !mClass || !mObj || !mGetOutputFormat)
		return NULL;
	
	if(mFormat != NULL)
	{
		delete mFormat;
		mFormat = NULL;
	}
	
	mFormat = new JCMediaFormat(mEnv);
	mFormat->init();
	jobject obj = mEnv->CallObjectMethod(mObj, mGetOutputFormat);
	if(obj == NULL)
	{
		if (mEnv->ExceptionCheck()) 
		{
			mEnv->ExceptionDescribe();
			mEnv->ExceptionClear();
		}
		delete mFormat;
		mFormat = NULL;
		return NULL;
	}
	mFormat->setObj(obj, false);
	return mFormat;
}

bool JCMediaCodec::onInit()
{
	assert(mEnv != NULL);
	if(mEnv)
	{
		struct _NameInfo
		{
			jmethodID& id;
			const char*     name;
			const char*     sig;
		} infos[] =
		  {
			  {mConfigure,                "configure",              "(Landroid/media/MediaFormat;Landroid/view/Surface;Landroid/media/MediaCrypto;I)V"},
			  {mDequeueInputBuffer,       "dequeueInputBuffer",     "(J)I"},    
			  {mDequeueOutputBuffer,      "dequeueOutputBuffer",    "(Landroid/media/MediaCodec$BufferInfo;J)I"},
			  {mFlush,                    "flush",                  "()V"},
			  {mGetInputBuffers,          "getInputBuffers",         "()[Ljava/nio/ByteBuffer;"},
			  {mGetOutputBuffers,         "getOutputBuffers",       "()[Ljava/nio/ByteBuffer;"},
			  {mGetOutputFormat,          "getOutputFormat",        "()Landroid/media/MediaFormat;"},
			  {mQueueInputBuffer,         "queueInputBuffer",       "(IIIJI)V"},
			  {mQueueSecureInputBuffer,   "queueSecureInputBuffer", "(IILandroid/media/MediaCodec$CryptoInfo;JI)V"},
			  {mRelease,                  "release",                "()V"},
			  {mReleaseOutputBuffer,      "releaseOutputBuffer",    "(IZ)V"},
			  {mSetVideoScalingMode,      "setVideoScalingMode",    "(I)V"},
			  {mStart,                    "start",                  "()V"},
			  {mStop,                     "stop",                   "()V"},

			  {mCreateByCodecName,        "createByCodecName",      "(Ljava/lang/String;)Landroid/media/MediaCodec;"},
			  {mCreateDecoderByType,      "createDecoderByType",    "(Ljava/lang/String;)Landroid/media/MediaCodec;"},
			  {mCreateEncoderByType,      "createEncoderByType",    "(Ljava/lang/String;)Landroid/media/MediaCodec;"},
		  };
		for(int i = 0 ; i < sizeof(infos)/sizeof(infos[0]); i++)
		{
			if( i <=13 )
				infos[i].id = mEnv->GetMethodID(mClass, infos[i].name, infos[i].sig); 
			else
				infos[i].id = mEnv->GetStaticMethodID(mClass, infos[i].name, infos[i].sig);		  
		
			if(infos[i].id == NULL)
			{
				LOGE("can not find the method = %s sig = %s\n", infos[i].name, infos[i].sig);
				if (mEnv->ExceptionCheck()) 
				{
					mEnv->ExceptionDescribe();
					mEnv->ExceptionClear();
				}
				
				return false;
			}
		}
	}
	return true;
}
