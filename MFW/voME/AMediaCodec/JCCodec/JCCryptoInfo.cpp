#include <stdio.h>
#include <assert.h>
#define LOG_TAG "JCCryptoInfo"
//#define __DEBUG

#include "JCCryptoInfo.h"



//android.media.MediaCodec$CryptoInfo
//"android/media/MediaCodec$CryptoInfo"

JCCryptoInfo::JCCryptoInfo(JNIEnv* env):JCObject(env, "android/media/MediaCodec$CryptoInfo"),
										mSet(NULL),
										mCons(NULL)
{
};

JCCryptoInfo::~JCCryptoInfo()
{

}

bool JCCryptoInfo::createCryptoInfo(const unsigned char* iv, int ivLen, const unsigned char* key, int keyLen, int* numBytesOfCD, int* numBytesOfED, int nums, int nMode)
{
	assert(mEnv != NULL);
	if(!mEnv || !mClass || !mCons)
		return false;

   
	bool bOk = false;
	if(!mObj)
		bOk = createObjWithOutParams(mCons);
	else
		bOk = true;
	
	__D("bOk = %d\n", bOk);
	__D("iv = %p ivLen = %d\n", iv, ivLen);
	__D("numBytesOfCD = %p\n", numBytesOfCD);
	__D("numBytesOfED = %p\n", numBytesOfED);
	__D("nums = %d\n", nums);
	if(bOk && iv /*&& key*/ && numBytesOfCD && numBytesOfED && mSet)
	{
		jbyteArray jiv =  mEnv->NewByteArray(ivLen);
		jbyteArray jkey = NULL;
		if(keyLen > 0)	
			jkey  = mEnv->NewByteArray(keyLen);
		jintArray  jCDs = mEnv->NewIntArray(nums);
		jintArray  jEDs = mEnv->NewIntArray(nums);
		mEnv->SetByteArrayRegion(jiv, 0, ivLen, (jbyte*)iv);
		if(key)
			mEnv->SetByteArrayRegion(jkey, 0, keyLen, (jbyte*)key);
		mEnv->SetIntArrayRegion(jCDs, 0, nums, numBytesOfCD);
		mEnv->SetIntArrayRegion(jEDs, 0, nums, numBytesOfED);
		
		mEnv->CallVoidMethod(mObj, mSet, nums, jCDs, jEDs, jkey, jiv, nMode);
		
		mEnv->DeleteLocalRef(jiv);

		if(jkey)
			mEnv->DeleteLocalRef(jkey);
		mEnv->DeleteLocalRef(jCDs);
		mEnv->DeleteLocalRef(jEDs);
		if (mEnv->ExceptionCheck()) 
		{
			LOGE("Could not create instance of android.media.MediaCodec.CryptoInfo\n.");
			mEnv->ExceptionDescribe();
			mEnv->ExceptionClear();
			return false;
		}
		return true;
	}
	return false;
}

bool JCCryptoInfo::onInit()
{
	assert(mEnv != NULL);
	assert(mClass != NULL);
	if(mEnv && mClass)
	{	
		struct _NameInfo
		{
			jmethodID&      id;
			const char*     name;
			const char*     sig;
		} infos[] =
		  {
			  {mSet,        "set",        "(I[I[I[B[BI)V"},
			  {mCons,       "<init>",     "()V"},    
		  };
		for(int i = 0 ; i < sizeof(infos)/sizeof(infos[0]); i++)
		{
			infos[i].id = mEnv->GetMethodID(mClass, infos[i].name, infos[i].sig);
			if(infos[i].id == NULL)
			{
				LOGE("can not find the method = %s sig = %s\n", infos[i].name, infos[i].sig);
				return false;
			}
		}
		return true;
	}
	return false;
}
