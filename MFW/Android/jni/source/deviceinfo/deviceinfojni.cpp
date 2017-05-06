
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		  deviceinfojni.cpp

	Contains:	deviceinfojni class file

	Written by:	 Yu Wei (Tom)  based on David's JNI code

	Change History (most recent first):
	2010-03-11		YW			Create file

*******************************************************************************/

#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
#include <utils/Parcel.h>
#elif defined __VONJ_ECLAIR__
#include <binder/Parcel.h>
#endif

#include <utils/Log.h>
#include <media/MediaPlayerInterface.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <utils/threads.h>
#include <jni.h>
#include <JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include <utils/Errors.h> 
#include <cutils/properties.h>

#define VOINFO(format, ...) { \
	LOGI("jni_deviceinfo %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }


using namespace android;

// ----------------------------------------------------------------------------

#if defined __cplusplus 
extern "C" {
#endif


static const char* const g_szdeviceinfo = "com/VisualOn/DeviceInfo/DeviceInfo";
//static const char* const g_szdeviceinfo = "com/visualon/di/DeviceInfo";

// get the param from the JAVA layer
static jstring getparam(JNIEnv* env, jobject thiz, int nparam1, int nparam2)
{
	 char m_szPropName[256];
	 property_get("ro.product.manufacturer", m_szPropName, "VisualOn");
	
    jstring encoding = (env)->NewStringUTF( m_szPropName); 
    return encoding;
}





// ----------------------------------------------------------------------------

static JNINativeMethod gDIMethods[] = {
    {"jni_getparam",              "(II)Ljava/lang/String;",        (void *)getparam},    
};


// This function only registers the native methods
static int register_di(JNIEnv *env) {

	jclass clazz;
  VOINFO("RegisterNatives for %s", g_szdeviceinfo);
	clazz = env->FindClass(g_szdeviceinfo);
	if (clazz == NULL) {
		VOINFO("%s not found!", g_szdeviceinfo);
		return JNI_FALSE;
	}

	if (env->RegisterNatives(clazz, gDIMethods, sizeof(gDIMethods) / sizeof(gDIMethods[0])) < 0) {
		VOINFO("RegisterNatives failed: '%s' ", g_szdeviceinfo);
		return JNI_FALSE;
	}
	
	return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
		VOINFO("JNI_OnLoad '%s' ", g_szdeviceinfo);
    JNIEnv* env = NULL;
    jint result = -1;

		jint jniver = JNI_VERSION_1_4;
    if (vm->GetEnv((void**) &env, jniver) != JNI_OK) {
			jniver = JNI_VERSION_1_6;
			if (vm->GetEnv((void**)&env, jniver) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
        goto bail;
			}
    }
    assert(env != NULL);

    if (register_di(env) < 0) {
        LOGE("ERROR: vomeplayer native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = jniver;

bail:
    return result;
}
#if defined __cplusplus
}
#endif

