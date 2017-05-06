#include <stdio.h>
#include <assert.h>

//#define __DEBUG
#include "JCMediaCrypto.h"
#include "JCUUID.h"

#define LOG_TAG "JCMediaCrypto"



JCMediaCrypto::JCMediaCrypto(JNIEnv* env): JCObject(env, "android/media/MediaCrypto"),
										   mCons(NULL),
										   mIsCryptoSchemeSupported(NULL),
										   mRelease(NULL),
										   mRequiresSecureDecoderComponent(NULL)
{
	
}

JCMediaCrypto::~JCMediaCrypto()
{

}


bool JCMediaCrypto::onInit()
{
	assert(mEnv != NULL);
	__D("mClass = %p\n", mClass);
	if(mEnv)
	{	
		struct _NameInfo
		{
			jmethodID& id;
			const char*     name;
			const char*     sig;
		} infos[] =
		  {                                                                           
			  {mIsCryptoSchemeSupported,         "isCryptoSchemeSupported",          "(Ljava/util/UUID;)Z"},
			  {mCons,                            "<init>",                           "(Ljava/util/UUID;[B)V"},
			  {mRelease,                         "release",                          "()V"},
			  {mRequiresSecureDecoderComponent,  "requiresSecureDecoderComponent",   "(Ljava/lang/String;)Z"}, 
		  };
		for(int i = 0 ; i < sizeof(infos)/sizeof(infos[0]); i++)
		{
			__D();
			if(0 == i)
				infos[i].id = mEnv->GetStaticMethodID(mClass, infos[i].name, infos[i].sig);
			else	
				infos[i].id = mEnv->GetMethodID(mClass, infos[i].name, infos[i].sig);
			if(infos[i].id == NULL)
			{
				LOGE("can not find the method = %s sig = %s\n", infos[i].name, infos[i].sig);
				return false;
			}
		}
	}
	return true;
}

bool JCMediaCrypto::createMediaCrypto(JCUUID& pID, const unsigned char* data, int nLen)
{
	if(!mEnv || !mCons)
		return false;
	if(nLen > 0)
	{
		jbyteArray jdata = mEnv->NewByteArray(nLen);
		mEnv->SetByteArrayRegion(jdata, 0, nLen, (jbyte*)data);
		jobject jid = pID.getObj();

		assert(jid != NULL);

		jobject obj = mEnv->NewObject(mClass, mCons, jid, jdata);
		if(obj == NULL)
		{
			if (mEnv->ExceptionCheck()) 
			{
				LOGE("Could not create instance of android.media.MediaCodec.MediaCrypto\n.");
				mEnv->ExceptionDescribe();
				mEnv->ExceptionClear();
				return false;
			}
		}
		mObj = mEnv->NewGlobalRef(obj);
		mEnv->DeleteLocalRef(obj);
		mEnv->DeleteLocalRef(jdata);
	}
	return true;
}

bool JCMediaCrypto::isCryptoSchemeSupported(JCUUID& uuid)
{
	if(!mEnv || !mIsCryptoSchemeSupported)
		return false;
	jobject jid = uuid.getObj();
	assert(jid != NULL);
	jboolean jSupported = mEnv->CallStaticBooleanMethod(mClass, mIsCryptoSchemeSupported, jid);
	return (jSupported == JNI_TRUE)? true:false;
}

bool JCMediaCrypto::requiresSecureDecoderComponent(const char* pMime)
{
	if(!mEnv || !mRequiresSecureDecoderComponent || !mObj)
		return false;
	if(pMime)
	{
		jstring jstr = mEnv->NewStringUTF(pMime);
		jboolean jRequire = mEnv->CallBooleanMethod(mObj, mRequiresSecureDecoderComponent, jstr);
		mEnv->DeleteLocalRef(jstr);
		return (jRequire == JNI_TRUE) ? true : false;
	}
	return false;
}

void JCMediaCrypto::release()
{
	if(!mEnv || !mRelease || !mObj)
		return ;
	mEnv->CallVoidMethod(mObj, mRelease);	
}
