#include <stdio.h>
#include <assert.h>
//#define __DEBUG

#include "JCMediaCodecList.h"
#include "JCMediaCodecInfo.h"

#define LOG_TAG "JCMediaCodecList"

             




JCMediaCodecList::JCMediaCodecList(JNIEnv* env):JCObject(env, "android/media/MediaCodecList"),
												mGetCodecCount(NULL),
												mGetCodecInfoAt(NULL)
{

}

JCMediaCodecList::~JCMediaCodecList()
{
	
}

bool JCMediaCodecList::onInit()
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
			  {mGetCodecCount,         "getCodecCount",          "()I"},
			  {mGetCodecInfoAt,        "getCodecInfoAt",         "(I)Landroid/media/MediaCodecInfo;"},	 
		  };
		for(int i = 0 ; i < sizeof(infos)/sizeof(infos[0]); i++)
		{
			__D();		
			infos[i].id = mEnv->GetStaticMethodID(mClass, infos[i].name, infos[i].sig);
			if(infos[i].id == NULL)
			{
				LOGE("can not find the method = %s sig = %s\n", infos[i].name, infos[i].sig);
				return false;
			}
		}
	}
	return true;
}

int JCMediaCodecList::getCodecCount()
{
	if(!mEnv || !mGetCodecCount)
		return -1;
	int nCount = mEnv->CallStaticIntMethod(mClass, mGetCodecCount);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return -1;
	}
	return nCount;
}

JCMediaCodecInfo* JCMediaCodecList::getCodecInfoAt(int index)
{
	if(!mEnv || ! mGetCodecInfoAt)
		return NULL;
	jobject obj = mEnv->CallStaticObjectMethod(mClass, mGetCodecInfoAt, index);
	if (mEnv->ExceptionCheck()) 
	{
		mEnv->ExceptionDescribe();
		mEnv->ExceptionClear();
		return NULL;
	}
	JCMediaCodecInfo* pInfo = new JCMediaCodecInfo(mEnv);
	pInfo->init();
	pInfo->setObj(obj, false);
	return pInfo;
}
