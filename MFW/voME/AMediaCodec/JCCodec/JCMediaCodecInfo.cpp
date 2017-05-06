#include <stdio.h>
#include <assert.h>
#include <android/log.h>

//#define __DEBUG
#include "JCMediaCodecInfo.h"

#define LOG_TAG "JCMediaCodecInfo"

JCMediaCodecInfo::JCMediaCodecInfo(JNIEnv* env):JCObject(env, "android/media/MediaCodecInfo"),
												mGetName(NULL),
												mGetSupportedTypes(NULL),
												mIsEncoder(NULL)
{

}

JCMediaCodecInfo::~JCMediaCodecInfo()
{
	
}

bool JCMediaCodecInfo::onInit()
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
			  {mGetName,                  "getName",               "()Ljava/lang/String;"},
			  {mGetSupportedTypes,        "getSupportedTypes",     "()[Ljava/lang/String;"},	 
			  {mIsEncoder,                "isEncoder",             "()Z"},
		  };
		for(int i = 0 ; i < sizeof(infos)/sizeof(infos[0]); i++)
		{
			__D();		
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

jstring JCMediaCodecInfo::getName()
{
	if(!mEnv || !mGetName || !mObj)
		return NULL;
	jstring str = (jstring)mEnv->CallObjectMethod(mObj, mGetName);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return NULL;
	}
	return str;
}

jobjectArray JCMediaCodecInfo::getSupportedTypes()
{
	if(!mEnv || !mGetName || !mObj)
		return NULL;
	jobjectArray arr = (jobjectArray)mEnv->CallObjectMethod(mObj, mGetSupportedTypes);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return NULL;
	}
	return arr;
}

bool JCMediaCodecInfo::isEncoder()
{
	if(!mEnv || !mIsEncoder || !mObj)
		return false;
	
	jboolean jEncoder = mEnv->CallBooleanMethod(mObj, mIsEncoder);
	return (jEncoder == JNI_TRUE)? true:false;
}
