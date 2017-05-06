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
 * @file CJavaJNI.h
 * CJavaJNI
 *
 * CJavaJNI
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/
#ifndef __CJavaJNI_H__
#define __CJavaJNI_H__

#ifdef _LINUX_ANDROID
#include "voYYDef_Common.h"
#include <jni.h>
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
    
    class CJavaJNI
    {
    public:
        CJavaJNI(void);
        virtual ~CJavaJNI(void);
        
        static int getIntegerValue(JNIEnv * env, jobject value);
        static void setIntegerValue(JNIEnv * env, jobject obj, jint value);
        static float getFloatValue(JNIEnv * env, jobject value);
    };

#ifdef _VONAMESPACE
}
#endif /* _VONAMESPACE */

#endif
