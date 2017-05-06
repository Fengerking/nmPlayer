/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file CJavaParcelWrap.cpp
 * Convert some C struct data to Java Parcel object.
 *
 * This is a wrap class of Java Parcel class
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/

#ifndef __CJavaParcelWrap_H__
#define __CJavaParcelWrap_H__
#include "voYYDef_Common.h"
#include	<string.h>
#include	<jni.h>
#include	<stdio.h>
#include    <stdlib.h>
#include	<dlfcn.h>
#include	"voType.h"
#include "voSubtitleType.h"
#include "voOnStreamType.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CJavaParcelWrap
{
public:
	CJavaParcelWrap(JNIEnv* env)
	{
	   m_bNeedRecycle = true;
	   m_env = env;
	   m_java_class = env->FindClass("android/os/Parcel");
	   if (m_java_class == 0) 
	   {
		   return;
	   }

		m_java_method_obtain = 0;
		m_java_method_writeint32 = 0;
		m_java_method_writeint64 = 0;
		m_java_method_writeBytes = 0;
		m_java_method_writeString = 0;
		m_java_method_recycle = 0;
		m_java_method_marshall = 0;

		m_parc = 0;
	 
	   m_java_method_obtain = env->GetStaticMethodID(m_java_class, "obtain", "()Landroid/os/Parcel;");
	   if(m_java_method_obtain == 0)
	   {
		   return;
	   }

	   m_parc = env->CallStaticObjectMethod(m_java_class, m_java_method_obtain);
	   m_java_method_writeint32 = env->GetMethodID(m_java_class, "writeInt", "(I)V");
	   if(m_java_method_writeint32 == 0)
	   {
		   return;
	   }
	   m_java_method_writeint64 = env->GetMethodID(m_java_class, "writeLong", "(J)V");
	   if(m_java_method_writeint64 == 0)
	   {
		   return;
	   }

	   m_java_method_writeString = env->GetMethodID(m_java_class, "writeString", "(Ljava/lang/String;)V");
	   if(m_java_method_writeString == 0)
	   {
		   return;
	   }

	   m_java_method_recycle = env->GetMethodID(m_java_class, "recycle", "()V");
	   if(m_java_method_recycle == 0)
	   {
		   return;
	   }
		
	   m_java_method_writeBytes = env->GetMethodID(m_java_class, "writeByteArray", "([B)V");
	   if(m_java_method_writeBytes == 0)
	   {
		   return;
	   }
   
	}
	virtual ~CJavaParcelWrap()
	{
		if(m_env && m_java_method_recycle && m_parc && m_bNeedRecycle)
			m_env->CallVoidMethod(m_parc, m_java_method_recycle); 
	}
public:
	jobject getParcel(){
		SetNeedRecycle(false);//need call recycle in java code
		return m_parc;};
	void writeInt32(jint nData)
	{
		if(m_java_method_writeint32 == 0)
			return;
		m_env->CallVoidMethod(m_parc, m_java_method_writeint32, nData); 
	}
	void writeInt64(jlong nData)
	{
		if(m_java_method_writeint64 == 0)
			return;
		m_env->CallVoidMethod(m_parc, m_java_method_writeint64, nData); 
	}
	void writeString(jstring nData)
	{
		if(m_java_method_writeString == 0)
			return;
		m_env->CallVoidMethod(m_parc, m_java_method_writeString, nData); 
	}
	void write(unsigned char* pByte, VO_U32 nCount)
	{
		if(nCount == 0 || pByte == NULL || m_java_method_writeBytes == 0)
			return;
		jbyteArray arr = m_env->NewByteArray(nCount);
		//jboolean b = true;
		jbyte* jb =   m_env->GetByteArrayElements(arr, 0);
		if (jb != NULL) 
		{
			memcpy(jb, pByte, nCount);
			m_env->ReleaseByteArrayElements(arr, jb, 0);
		}
		
	   m_env->CallVoidMethod(m_parc, m_java_method_writeBytes, arr); 
	   m_env->DeleteLocalRef(arr);
	}
	void SetNeedRecycle(bool bRecycle){m_bNeedRecycle = bRecycle;};
	//static VO_U8 convertTransparent(VO_U8 transp)
	//{
	//	VO_U32 intTransp = transp;
	//	intTransp = intTransp*255/100;
	//	if(intTransp>255)
	//		intTransp = 255;
	//	return (VO_U8)intTransp;
	//};

	static int getParcelFromSubtitleInfo(voSubtitleInfo* sample,CJavaParcelWrap* parcel);
	static int getParcelFromPerformanceData(VOOSMP_PERFORMANCE_DATA* sample,CJavaParcelWrap* parcel);
protected:
	JNIEnv* m_env;
private:
	jmethodID m_java_method_obtain;
	jmethodID m_java_method_writeint32;
	jmethodID m_java_method_writeint64;
	jmethodID m_java_method_writeBytes;
	jmethodID m_java_method_writeString;
	jmethodID m_java_method_recycle;
	jmethodID m_java_method_marshall;

	jclass	 m_java_class;
	jobject  m_parc;
	bool	 m_bNeedRecycle;
};

#ifdef _VONAMESPACE
}
#endif

#endif
