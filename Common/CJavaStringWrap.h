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
 * @file CJavaStringWrap.h
 * conver strings to different format.
 *
 * This is a wrap class of Java String class
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/

#ifndef __CJavaStringWrap_H__
#define __CJavaStringWrap_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CJavaStringWrap
{
public:
	CJavaStringWrap(JNIEnv* env)
	{
	   m_env = env;
	   m_java_class = env->FindClass("java/lang/String");
	   if (m_java_class == 0) 
	   {
		   return;
	   }

		m_java_method_init = 0;
		m_java_method_getBytes = 0;

		m_str = 0;
	 
	   m_java_method_init = env->GetMethodID(m_java_class, "<init>", "([BIILjava/lang/String;)V");
	   if(m_java_method_init == 0)
	   {
		   return;
	   }
	   m_java_method_getBytes = env->GetMethodID(m_java_class, "getBytes", "(Ljava/lang/String;)[B");
	   if(m_java_method_getBytes == 0)
	   {
		   return;
	   }
	}
	bool Convert(unsigned char * sourceBuffer, int sourceSize, char* sourceCharsetName, unsigned char*& destBuffer, int& destSize, char* destCharsetName)
	{
	   if(m_java_method_init == 0)
	   {
		   return false;
	   }
	   if(m_env == 0)
	   {
		   return false;
	   }

	   if(strlen(sourceCharsetName)==0)
		   return false;

	   jstring js = (m_env)->NewStringUTF((const char*)sourceCharsetName);
	   jbyteArray bArr = m_env->NewByteArray(sourceSize);
	   if(bArr == NULL)
		   return false;
	   jbyte * pData = m_env->GetByteArrayElements(bArr, 0);
	   if(pData == NULL)
		   return false;
	   memcpy(pData, sourceBuffer, sourceSize);
	   if (pData != NULL) 
		   m_env->ReleaseByteArrayElements(bArr, pData, 0);
	   
	   m_str = m_env->NewObject(m_java_class, m_java_method_init,bArr ,(jint)0,(jint)sourceSize,js);
	   jstring js2 = (m_env)->NewStringUTF((const char*)destCharsetName);
	   jbyteArray byteArr = (jbyteArray) m_env->CallObjectMethod(m_str, m_java_method_getBytes, js2); 
	   if(byteArr==NULL)
		   return false;

	   destSize = m_env->GetArrayLength(byteArr);
	   pData = m_env->GetByteArrayElements(byteArr, 0);
	   if(pData == NULL)
		   return false;
	   destBuffer = new unsigned char[destSize];
	   memcpy(destBuffer, pData, destSize);
	   m_env->ReleaseByteArrayElements(byteArr, pData, 0);

	   return true;
	}
private:
	JNIEnv* m_env;
	jmethodID m_java_method_init;
	jmethodID m_java_method_getBytes;

	jclass	 m_java_class;
	jobject  m_str;
};

#ifdef _VONAMESPACE
}
#endif

#endif