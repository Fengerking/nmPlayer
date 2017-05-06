#include <stdio.h>
#include <assert.h>

//#define __DEBUG
#include "JCObject.h"

#define LOG_TAG "JCObject"



JCObject::JCObject(JNIEnv* env, const char* inName):mEnv(env),
													mClass(NULL),
													mObj(NULL),
													mClassName(inName)
{
	mIsGlobal = true;
}

JCObject::~JCObject()
{
	if(mObj && mIsGlobal)
	{
		__D();
		mEnv->DeleteGlobalRef(mObj);
		mObj = NULL;
		__D();
	}
	if(mClass && mIsGlobal)
	{
		__D();
		mEnv->DeleteGlobalRef(mClass);
		mClass = NULL;
		__D();
	}
}

bool JCObject::setClass()
{
	assert(mEnv != NULL);
	if(mClassName && mEnv)
	{
		jclass lclass = mEnv->FindClass(mClassName);
		if(lclass == NULL)
		{
			LOGE("can not find the %s class \n", mClassName);
			if (mEnv->ExceptionCheck()) 
			{				
				mEnv->ExceptionDescribe();
				mEnv->ExceptionClear();
			}
			return false;
		}
   		mClass = (jclass)mEnv->NewGlobalRef(lclass);
		mEnv->DeleteLocalRef(lclass);
	}
	return true;
}
bool JCObject::init()
{
	if(setClass())
	{
		__D();
		return onInit();
	}
	__D();
	return false;
}

bool JCObject::setObj(jobject obj, bool toGlobal)
{
	mIsGlobal=toGlobal;
	if(mIsGlobal)
	{
		mObj = mEnv->NewGlobalRef(obj);
		mEnv->DeleteLocalRef(obj);
	}
	mObj = obj;
	return true;
}

bool JCObject::createObjFromChar(const char* name,  jmethodID& jm, bool isStatic)
{
	assert(isStatic == true);
	jstring jstr = mEnv->NewStringUTF(name);
	jobject obj = NULL;
	__D("name = %s jm = %p\n", name, jm);
	obj = mEnv->CallStaticObjectMethod(mClass, jm, jstr);
	if(obj == NULL)
	{
		__D("obj = %p\n", obj);
		if (mEnv->ExceptionCheck()) 
		{
			
			LOGE("Could not create instance of %s from string %s\n.", mClassName, name);
			mEnv->ExceptionDescribe();
			mEnv->ExceptionClear();
			mEnv->DeleteLocalRef(jstr);
			return false;
		}
	}
	__D("obj = %p\n", obj);
	mObj = mEnv->NewGlobalRef(obj);
	mEnv->DeleteLocalRef(obj);
	mEnv->DeleteLocalRef(jstr);
	__D();
	return true;
}
bool JCObject::createObjFromBytes(const unsigned char* inBytes, int inLen,  jmethodID jm, bool isStatic)
{
	if(isStatic)
	{
		jbyteArray jdata = mEnv->NewByteArray(inLen);
		mEnv->SetByteArrayRegion(jdata, 0, inLen, (jbyte*)inBytes);

		jobject obj = mEnv->CallStaticObjectMethod(mClass, jm, jdata);

		if(obj == NULL)
		{
			if (mEnv->ExceptionCheck()) 
			{
				LOGE("Could not create instance of %s from bytes\n", mClassName);
				mEnv->ExceptionDescribe();
				mEnv->ExceptionClear();
				return false;
			}
		}
		mObj = mEnv->NewGlobalRef(obj);
		mEnv->DeleteLocalRef(obj);
		mEnv->DeleteLocalRef(jdata);
		return true;
	}
	return false;
}

bool JCObject::createObjWithOutParams(jmethodID jm)
{
	jobject jobj = mEnv->NewObject(mClass, jm);
	if(jobj == NULL)
	{
		if (mEnv->ExceptionCheck()) 
		{
			LOGE("Could not create instance of %s\n", mClassName);
			mEnv->ExceptionDescribe();
			mEnv->ExceptionClear();
			return false;
		}
	}
	mObj = mEnv->NewGlobalRef(jobj);
	mEnv->DeleteLocalRef(jobj);
	return true;
}

bool JCObject::setEnv(JNIEnv* inEnv)
{
	if(mEnv != inEnv)
	{
		mEnv = inEnv;
		return true;
	}
	return false;
}
