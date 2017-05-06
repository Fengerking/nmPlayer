/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved
 
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
 * @file CJavaJNI.cpp
 * CJavaJNI
 *
 * CJavaJNI
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#include "CJavaJNI.h"

#include "voLog.h"
#include "CJniEnvUtil.h"

CJavaJNI::CJavaJNI()
{
}

CJavaJNI::~CJavaJNI()
{
}

int CJavaJNI::getIntegerValue(JNIEnv * env, jobject value)
{
	if (value == NULL)
		return 0;
	
	jclass clazz = env->FindClass("java/lang/Integer");
    
	jfieldID field = env->GetFieldID (clazz, "value" , "I");
    
	int nRC = env->GetIntField (value, field);
    
	env->DeleteLocalRef(clazz);
    
	return nRC;
}

void CJavaJNI::setIntegerValue(JNIEnv * env, jobject obj, jint value)
{
	if (obj == NULL)
		return;
	
	jclass clazz = env->FindClass("java/lang/Integer");
    
	jfieldID field = env->GetFieldID (clazz, "value" , "I");
    
	env->SetIntField (obj, field, value);
    
	env->DeleteLocalRef(clazz);
}


float CJavaJNI::getFloatValue(JNIEnv * env, jobject value)
{
	if (value == NULL)
		return 0;
	
	jclass clazz = env->FindClass("java/lang/Float");
    
	jfieldID field = env->GetFieldID (clazz, "value" , "F");
    
	float nRC = env->GetFloatField (value, field);
    
	env->DeleteLocalRef(clazz);
    
	return nRC;
}
