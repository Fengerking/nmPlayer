#ifndef __JC_OBJECT__H
#define __JC_OBJECT__H
#include "jni.h"

#include <android/log.h>
#include <pthread.h>

#if !defined LOGE
#define LOGE(...) ((int)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#if defined(__DEBUG)
#   define __D(fmt, args...) LOGE("%s::%s::%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)
#else
#   define __D(fmt, args...)
#endif

#define VOLOGE(fmt, args...) LOGE("%s::%s::%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)

class JCObject
{
public:
	JCObject(JNIEnv* env, const char* inName);
	virtual ~JCObject();

public:
	bool     init();
	bool     setObj(jobject obj, bool toGlobal=true);
	bool     setEnv(JNIEnv* inEnv);
	inline jobject  getObj()
	{
		return mObj;
	}
protected:
	bool    setClass();
	bool    createObjFromChar(const char* name, jmethodID& jm, bool isStatic=true);
	bool    createObjFromBytes(const unsigned char* inBytes, int inLen, jmethodID jm, bool isStatic=true);
	bool    createObjWithOutParams(jmethodID jm);
	virtual bool onInit() = 0;
protected:
	JNIEnv*       mEnv;
	jclass        mClass;
	jobject       mObj;
	const char*   mClassName;
	bool          mIsGlobal;
};
#endif
