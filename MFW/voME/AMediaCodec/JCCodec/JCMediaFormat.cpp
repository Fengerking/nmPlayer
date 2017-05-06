#include <assert.h>
#include <stdio.h>


//#define __DEBUG
#include "JCMediaFormat.h"

#define LOG_TAG "JCMediaFormat"


JCMediaFormat::JCMediaFormat(JNIEnv* env):JCObject(env, "android/media/MediaFormat"),
										  mCreateVideoFormat(NULL),
										  mCreateAudioFormat(NULL),
										  mToString(NULL),
										  mGetInteger(NULL),
										  mGetLong(NULL),
										  mGetFloat(NULL),
										  mGetString(NULL),
										  mSetInteger(NULL),
										  mSetLong(NULL),
										  mSetFloat(NULL),
										  mSetString(NULL),
										  mInfo(NULL)

{
}

static void safe_delete_m(char* pStr)
{
	if(pStr)
	{
		delete [] pStr;
		pStr = NULL;
	}
}
static void sf_delete_info(JCMediaFormat::MFInfo* pInfo)
{
	if(pInfo)
	{
		__D();
		if(pInfo->mMime)
		{
			__D("pInfo->mMime = %s\n", pInfo->mMime);
			safe_delete_m(pInfo->mMime);
		}
		delete pInfo;
		pInfo = NULL;
		__D();
	}	
	__D();
}
JCMediaFormat::~JCMediaFormat()
{
	sf_delete_info(mInfo);
}

JCMediaFormat::MFInfo* JCMediaFormat::getMediaInfo(const bool bIsVideo)
{
	assert(mEnv != NULL);
	if(mInfo)
	{
		sf_delete_info(mInfo);
	}
	if(mInfo == NULL)
	{
		mInfo = new MFInfo;
		memset(mInfo, 0, sizeof(MFInfo));
	}
	
	assert(mInfo != NULL);
	if(mInfo != NULL)
	{
		__D();
		
		__D();
		if(bIsVideo)
		{
			__D();
			mInfo->mMime   = getStringValue("mime");
			__D();
			mInfo->mWidth  = getIntValue("width");
			mInfo->mHeight = getIntValue("height");
			mInfo->mColorFormat = getIntValue("color-format");
		}
		else
		{
			__D();
 			mInfo->mChannels = getIntValue("channel-count");
			__D();
			mInfo->mSampleRate = getIntValue("sample-rate");
			__D();
		}
	}
	return mInfo;
}

bool JCMediaFormat::createVideoFormat(const char* mime, int width, int height)
{
	assert(mEnv != NULL);
	if(mEnv && mCreateVideoFormat)
	{
		jstring jMime = mEnv->NewStringUTF(mime);
		assert(jMime != NULL);
		jthrowable exc = NULL;
		jint jWidth = width;
		jint jHeight = height;
		jobject obj = mEnv->CallStaticObjectMethod(mClass, mCreateVideoFormat, jMime, jWidth, jHeight);
		exc = mEnv->ExceptionOccurred();
		if(exc)
		{
			mEnv->ExceptionDescribe();
			mEnv->ExceptionClear();
			mEnv->DeleteLocalRef(jMime);
			return false;
		}
		mObj = mEnv->NewGlobalRef(obj);
		mEnv->DeleteLocalRef(obj);
		mEnv->DeleteLocalRef(jMime);
		
	}
	return true;
}

bool JCMediaFormat::createAudioFormat(const char* mime, int sampleRate, int channels)
{
	assert(mEnv != NULL);
	if(mEnv && mCreateAudioFormat)
	{
		jstring jMime = mEnv->NewStringUTF(mime);
		assert(jMime != NULL);
		jthrowable exc = NULL;
		jint jChannels = channels;
		jint jSampleRate = sampleRate;
		jobject obj = mEnv->CallStaticObjectMethod(mClass, mCreateAudioFormat, jMime, jSampleRate, jChannels);
		exc = mEnv->ExceptionOccurred();
		if(exc)
		{
			mEnv->ExceptionDescribe();
			mEnv->ExceptionClear();
			mEnv->DeleteLocalRef(jMime);
			return false;
		}
		mObj = mEnv->NewGlobalRef(obj);
		mEnv->DeleteLocalRef(obj);
		mEnv->DeleteLocalRef(jMime);
		
	}
	return true;
}


bool JCMediaFormat::onInit()
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
				  {mCreateVideoFormat,         "createVideoFormat", "(Ljava/lang/String;II)Landroid/media/MediaFormat;"},
				  {mCreateAudioFormat,         "createAudioFormat", "(Ljava/lang/String;II)Landroid/media/MediaFormat;"},
				  {mToString,                  "toString",          "()Ljava/lang/String;"},
				  {mGetInteger,                "getInteger",        "(Ljava/lang/String;)I"},
				  {mGetLong,                   "getLong",           "(Ljava/lang/String;)J"},
				  {mGetFloat,                  "getFloat",          "(Ljava/lang/String;)F"},
				  {mGetString,                 "getString",         "(Ljava/lang/String;)Ljava/lang/String;"},
				  {mSetInteger,                "setInteger",        "(Ljava/lang/String;I)V"},
				  {mSetLong,                   "setLong",           "(Ljava/lang/String;J)V"},
				  {mSetFloat,                  "setFloat",          "(Ljava/lang/String;F)V"},
				  {mSetString,                 "setString",         "(Ljava/lang/String;Ljava/lang/String;)V"},
				  {mSetByteBuffer,             "setByteBuffer",     "(Ljava/lang/String;Ljava/nio/ByteBuffer;)V"},
				  {mGetByteBuffer,             "getByteBuffer",     "(Ljava/lang/String;)Ljava/nio/ByteBuffer;"},
			  };
		for(int i = 0 ; i < sizeof(infos)/sizeof(infos[0]); i++)
		{
			if( i<=1 )
				infos[i].id = mEnv->GetStaticMethodID(mClass, infos[i].name, infos[i].sig);		
			else
				infos[i].id = mEnv->GetMethodID(mClass, infos[i].name, infos[i].sig); 			  
		
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
		return true;
	}
	return false;
}

bool JCMediaFormat::dump()
{
	if(!mToString)
		return false;
	__D("why_why\n");
	jstring jstr = (jstring)mEnv->CallObjectMethod(mObj, mToString);
	__D("why_why\n");
	
	const char* pStr = mEnv->GetStringUTFChars(jstr, NULL);
	__D("%s\n",pStr);
	mEnv->ReleaseStringUTFChars(jstr, pStr);
	mEnv->DeleteLocalRef(jstr);
	return true;
}


int JCMediaFormat::getIntValue(const char* name)
{
	if(!mGetInteger || !name)
		return -1;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		int n = mEnv->CallIntMethod(mObj, mGetInteger, jstr);
		mEnv->DeleteLocalRef(jstr);
		return n;
	}
	return -1;
}
long JCMediaFormat:: getLongValue(const char* name)
{
	if(!mGetLong || !name)
		return -1;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		long n = mEnv->CallLongMethod(mObj, mGetLong, jstr);
		mEnv->DeleteLocalRef(jstr);
		return n;
	}
	return -1;
}

float JCMediaFormat::getFloatValue(const char* name)
{
	if(!mGetFloat || !name)
		return -1.0f;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		float n = mEnv->CallFloatMethod(mObj, mGetFloat, jstr);
		mEnv->DeleteLocalRef(jstr);
		return n;
	}
	return -1;
}
char* JCMediaFormat::getStringValue(const char* name)
{
	if(!mGetString || !name)
		return NULL;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		char* pRet = NULL;
		jstring s = (jstring)mEnv->CallObjectMethod(mObj, mGetString, jstr);
		const char* pStr = mEnv->GetStringUTFChars(s, NULL);
		if(pStr)
		{
			int nLen = strlen(pStr) + 1 ;
			pRet = new char[nLen];
			strcpy(pRet, pStr);
			mEnv->ReleaseStringUTFChars(s, pStr);
			mEnv->DeleteLocalRef(s);
		}
		mEnv->DeleteLocalRef(jstr);
		return pRet;
	}
	return NULL;
}

bool JCMediaFormat::setIntValue(const char* name, int value)
{
	if(!mSetInteger || !name)
		return false;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		mEnv->CallVoidMethod(mObj, mSetInteger, jstr, value);
		mEnv->DeleteLocalRef(jstr);
		return true;
	}
	return false;
}
bool JCMediaFormat:: setLongValue(const char* name, long value)
{
	if(!mSetLong || !name)
		return false;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		mEnv->CallVoidMethod(mObj, mSetLong, jstr, value);
		mEnv->DeleteLocalRef(jstr);
		return true;
	}
	return false;
}

bool JCMediaFormat::setFloatValue(const char* name, float value)
{
	if(!mSetFloat || !name)
		return false;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		mEnv->CallVoidMethod(mObj, mSetFloat, jstr, value);
		mEnv->DeleteLocalRef(jstr);
		return true;
	}
	return false;
}
bool JCMediaFormat::setStringValue(const char* name, const char* value)
{
	if(!mSetString || !name || !value)
		return NULL;
	jstring jstr = mEnv->NewStringUTF(name);
	jstring jv  = mEnv->NewStringUTF(value);
	if(jstr)
	{
		mEnv->CallVoidMethod(mObj, mSetString, jstr, jv);
		mEnv->DeleteLocalRef(jv);
		mEnv->DeleteLocalRef(jstr);
		return true;
	}
	return false;
}

bool JCMediaFormat::setByteBuffer(const char* name, unsigned char* bytes, const long inLen)
{
	if(!mSetByteBuffer || !name || !bytes || inLen <=0)
		return false;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		jobject obj = mEnv->NewDirectByteBuffer(bytes, inLen);
		if(obj)
		{
			mEnv->CallVoidMethod(mObj, mSetByteBuffer, jstr, obj);
			mEnv->DeleteLocalRef(obj);
		}
		mEnv->DeleteLocalRef(jstr);
		return true;
	}
	return false;
}

jobject JCMediaFormat::getByteBuffer(const char* name)
{
	if(!mGetByteBuffer || !name)
		return NULL;
	jstring jstr = mEnv->NewStringUTF(name);
	if(jstr)
	{
		jobject obj = mEnv->CallObjectMethod(mObj, mGetByteBuffer, jstr);
		return obj;
	}
	return NULL;
}

