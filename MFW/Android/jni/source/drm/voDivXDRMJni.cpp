
#include <string.h>
#include <jni.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>

#include <utils/Log.h>

#include "voDivXDRM.h"
#include "vomutex.h"
#include "CDrmEngine.h"

#define LOG_TAG "VODRMJNI"

#define VOINFO(format, ...) do { \
	const char* pfile = strrchr(__FILE__, '/'); \
	pfile = (pfile == NULL ? __FILE__ : pfile + 1); \
  LOGI("David %s::%s()->%d: " format, pfile, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0);



static Mutex		g_mvodivxlock;

#if defined __cplusplus 
extern "C" {
#endif

static CDrmEngine gEngine(VO_DRMTYPE_DIVX);


JNIEXPORT jint JNICALL jni_DivXDRMInit(JNIEnv* env, jobject thiz, jint flag)
{
	//VOINFO("jni_DivXDRMInit");
	Mutex::Autolock l(g_mvodivxlock);

	VO_PTR pHandle = NULL;

 	gEngine.LoadLib(NULL);
	VO_U32 rc = gEngine.Open();
	if (rc != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm Engine Open failed");
		return 0;
	}

	jint handle = (jint) pHandle;

/*
	// for most platform, jni has no permission to create dir at /data
	const char* szdeposition = "/data/vdif_data";
	if (access(szdeposition, R_OK) == -1) {
		mkdir(szdeposition, 0777);
	}
*/
	bool bMemInited = false;
	if (0 == gEngine.GetParameter(DIVX_DRM_ID_ISDRMMEMINITIALIZED, &bMemInited) && bMemInited == true) {
		VOINFO("vodivxdrm DIVX_DRM_ID_ISDRMMEMINITIALIZED failed");
		return handle;
	} 

	gEngine.SetParameter(DIVX_DRM_ID_INITDRMMEMORY , NULL);
	if (rc != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm DIVX_DRM_ID_INITDRMMEMORY failed");
		return handle;
	}

	return handle;
}

JNIEXPORT jboolean JNICALL jni_DivXDRMUninit(JNIEnv* env, jobject thiz, jint handle)
{
	//VOINFO("jni_DivXDRMUninit");
	Mutex::Autolock l(g_mvodivxlock);

	VO_U32 rc = gEngine.Close();
	if (rc != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm Engine Close failed");
		return false;
	}

	return true;
}

JNIEXPORT jboolean JNICALL jni_DivXDRMInitMemory(JNIEnv* env, jobject thiz, jint flag)
{
	//VOINFO("jni_DivXDRMInitMemory");
	Mutex::Autolock l(g_mvodivxlock);

	VO_U32 rc = gEngine.SetParameter(DIVX_DRM_ID_INITDRMMEMORY , NULL);
	if (rc != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm DIVX_DRM_ID_INITDRMMEMORY failed");
		return false;
	}

	return true;
}

JNIEXPORT jint JNICALL jni_GetDeviceStatus(JNIEnv* env, jobject thiz, jint handle)
{
	//VOINFO("jni_GetDeviceStatus");
	Mutex::Autolock l(g_mvodivxlock);

	VODIVXDRMDEVICESTATUS pDeviceStatus;
	
	VO_U32 nRet = gEngine.GetParameter( DIVX_DRM_ID_ACTIVATIONSTATUS, &pDeviceStatus);
	if (nRet != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm DIVX_DRM_ID_ACTIVATIONSTATUS failed");
		return -1;
	}

	return (jint) pDeviceStatus;
}

JNIEXPORT jstring JNICALL jni_GetRegistrationCodeString(JNIEnv* env, jobject thiz, jint handle)
{
	//VOINFO("jni_GetRegistrationCodeString");
	Mutex::Autolock l(g_mvodivxlock);
	
	char regcode[100];
	
	VO_U32 nRet = gEngine.GetParameter(DIVX_DRM_ID_REGCODESTRING, regcode);
	if (nRet != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm DIVX_DRM_ID_REGCODESTRING failed");
		return NULL;
	}
	
	return (env)->NewStringUTF((const char*)regcode);
}

JNIEXPORT jstring JNICALL jni_GetDeactivationCodeString(JNIEnv* env, jobject thiz, jint handle)
{
	//VOINFO("jni_GetDeactivationCodeString");
	Mutex::Autolock l(g_mvodivxlock);

	char deactcode[100];
	
	VO_U32 nRet = gEngine.GetParameter(DIVX_DRM_ID_DEACTCODESTRING, deactcode);
	if (nRet != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm DIVX_DRM_ID_DEACTCODESTRING failed");
		return NULL;
	}

	return (env)->NewStringUTF((const char*)deactcode);
}

JNIEXPORT jboolean JNICALL jni_SetRandomSample(JNIEnv* env, jobject thiz, jint handle)
{
	//VOINFO("jni_SetRandomSample");
	VO_U32 nRet = gEngine.SetParameter(DIVX_DRM_ID_RANDOMSAMPLE, NULL);
	if (nRet != VO_ERR_DRM_OK) {
		VOINFO("vodivxdrm DIVX_DRM_ID_RANDOMSAMPLE failed");
		return false;
	}

	return true;
}

static JNINativeMethod g_divxdrmmethods[] = {
	{"DivXDRMInit", "(I)I",				(void*)jni_DivXDRMInit},
	{"DivXDRMUninit", "(I)Z",			(void*)jni_DivXDRMUninit},
	//{"DivXDRMInitMemory", "(I)Z",			(void*)jni_DivXDRMInitMemory},
	{"GetDeviceStatus", "(I)I",			(void*)jni_GetDeviceStatus},
	{"GetRegistrationCodeString", "(I)Ljava/lang/String;", (void*)jni_GetRegistrationCodeString},
	{"GetDeactivationCodeString", "(I)Ljava/lang/String;", (void*)jni_GetDeactivationCodeString},
	//{"SetRandomSample", "(I)Z", (void*)jni_SetRandomSample},
};

static const char *g_szclasspathname = "com/visualon/divxdrm/divxdrm";

//Register native methods for all classes we know about. returns JNI_TRUE on success.
static jboolean registerNatives(JNIEnv* env)
{
	//VOINFO("registerNatives");

	jclass clazz;

	clazz = env->FindClass(g_szclasspathname);
	if (clazz == NULL) {
		VOINFO("vodivxdrm FindClass failed");
		return JNI_FALSE;
	}

	if (env->RegisterNatives(clazz, g_divxdrmmethods, sizeof(g_divxdrmmethods) / sizeof(g_divxdrmmethods[0])) < 0) {
		VOINFO("vodivxdrm RegisterNatives failed");
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	//VOINFO("JNI_OnLoad");

	jint result = -1;
	JNIEnv* env = NULL;

	jint jniver = JNI_VERSION_1_4;
	if (vm->GetEnv((void**)&env, jniver) != JNI_OK) {
		jniver = JNI_VERSION_1_6;
		if (vm->GetEnv((void**)&env, jniver) != JNI_OK) 
			VOINFO("vodivxdrm GetEnv failed");
			goto bail;
	}
	
	if (registerNatives(env) != JNI_TRUE) {
		VOINFO("vodivxdrm registerNatives failed");
		goto bail;
	}

	result = jniver;

bail:
	return result;
}

#if defined __cplusplus
}
#endif

