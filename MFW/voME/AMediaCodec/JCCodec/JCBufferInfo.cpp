#include <stdio.h>
#include <assert.h>

#define LOG_TAG "JCBufferInfo"
//#define __DEBUG

#include "JCBufferInfo.h"





                                         
JCBufferInfo::JCBufferInfo(JNIEnv* env):JCObject(env, "android/media/MediaCodec$BufferInfo"),
										mFlags(0),
										mOffset(0),
										mTimeUs(0),
										mSize(0),
										mFlagsID(NULL),
										mOffsetID(NULL),
										mTimeUsID(NULL),
										mSizeID(NULL),
										mCons(NULL)
{
	
}

JCBufferInfo::~JCBufferInfo()
{

}

bool JCBufferInfo::update()
{
	assert(mObj != NULL && mEnv != NULL);
	if(mObj)
	{
		mFlags =  mEnv->GetIntField(mObj, mFlagsID);	
		mOffset = mEnv->GetIntField(mObj, mOffsetID);
		mTimeUs = mEnv->GetLongField(mObj, mTimeUsID);
		mSize = mEnv->GetIntField(mObj, mSizeID);
	}
	return true;
}

bool JCBufferInfo::onInit()
{
	assert(mEnv != NULL);
	if(mEnv)
	{
		mCons = mEnv->GetMethodID(mClass, "<init>", "()V");
		if(!mCons)
		{
			LOGE("Can not find the %s constructor\n", mClassName);
			return false;
		}
		struct _FieldInfo
		{
			jfieldID&       id;
			const char*     name;
			const char*     sig;
		} infos[] =
		  {
			  {mFlagsID,         "flags",                "I"},
			  {mOffsetID,        "offset",               "I"},
			  {mTimeUsID,        "presentationTimeUs",   "J"},
			  {mSizeID,          "size",                 "I"},		  
		  };
		for(int i = 0 ; i < sizeof(infos) / sizeof(infos[0]); i ++)
		{
			infos[i].id = mEnv->GetFieldID(mClass, infos[i].name, infos[i].sig);
			if(infos[i].id == NULL)
			{
				LOGE("can not find the field = %s sig = %s\n", infos[i].name, infos[i].sig);
				return false;
			}
		}
	}
	return true;
}
bool JCBufferInfo::createObject()
{
	if(!mEnv || !mClass || !mCons)
		return false;
	return createObjWithOutParams(mCons);
}
