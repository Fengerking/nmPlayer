		/************************************************************************
		*																		*
		*		VisualOn, Inc. Confidential and Proprietary, 2003				*
		*																		*
		************************************************************************/
	/*******************************************************************************
		File:		vomDRMJni.cpp
	
		Contains:	voDRMJni entry file
	
		Written by: Steven Zhang
	
		Change History (most recent first):
		2012-02-03		Steven Zhang 		Create file
	
	*******************************************************************************/
#include <jni.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dlfcn.h>

#include "voOSFunc.h"
#include "voType.h"
#include "voDRM2.h"

#include "vodlfcn.h"
#include "voOnStreamType.h"
#include "voOnStreamSourceType.h"
#include "voSource2.h"
#include "CJavaParcelWrap.h"
#include "CJavaParcelWrapOSMP.h"
#include "CJniEnvUtil.h"


#include "voLog.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



static const char*		g_szvodrmclsname = "com/visualon/OSMPBasePlayer/voOSDRM";
static char				g_szPackagePath[1024];
static const char*	g_szosmp_suffix = "_OSMP";


JavaVM*   gJavaVM;

jobject g_pThis = 0;
jmethodID g_midSendData = 0;

VO_PTR g_hDll = 0;


const int VO_DRM_THIRD_LIBOP = 		0x00001001;
const int VO_DRM_THIRD_FILEOP = 	0x00001002;


typedef VO_S32 (VO_API * VOGETAPI)(VO_DRM2_API * pDRMHandle, VO_U32 uFlag);

VO_U32 voDRM_CallBack(VO_PTR hHandle , VO_U32 uID , VO_PTR pUserData );



static void * vomtGetAddress (void * pUserData, void * hLib, VO_PCHAR pFuncName, VO_S32 nFlag)
{
	void * ptr = voGetAddressSymbol (hLib, pFuncName);

	VOLOGI( "Func Name: %s  %p" , pFuncName , ptr );
	return ptr;
}

static VO_S32 vomtFreeLib (void * pUserData, void * hLib, VO_S32 nFlag)
{
	VOLOGI ("Free Lib: %0xd", (unsigned int) hLib);
	voFreeLibrary (hLib);
	return 0;
}

static void * vomtLoadLib (void * pUserData, VO_PCHAR pLibName, VO_S32 nFlag)
{
	void * hDll = NULL;

	hDll = voLoadLibrary (pLibName, RTLD_NOW);

	if (hDll == NULL)
	{
		VOLOGE("Load Library File %s, failed!", pLibName);
	}else
	{
		VOLOGI ("Load Library File %s, Handle %p", pLibName, hDll);
	}

	return hDll;
}

int getIntegerValue(JNIEnv * env, jobject value)
{
	jclass clazz = env->FindClass("java/lang/Integer");

	jfieldID field = env->GetFieldID (clazz, "value" , "I");

	return env->GetIntField (value, field);
}

void setIntegerValue(JNIEnv * env, jobject obj, jint value)
{
	jclass clazz = env->FindClass("java/lang/Integer");

	jfieldID field = env->GetFieldID (clazz, "value" , "I");

	env->SetIntField (obj, field, value);
}



/*
 * Class:     com_visualon_vome_voDRM
 * Method:    nativeInit
 * Signature: (Ljava/lang/Integer;Ljava/lang/Object;)J
 */
static VO_S32 voDRM_Init (JNIEnv * env, jobject obj, jobject context, jobject handle, jlong nflag)
{

	VO_DRM2_API* pApi = (VO_DRM2_API* )getIntegerValue(env, context);
	VO_PTR ppHandle;
	VO_DRM_OPENPARAM pDRMOpenParam;

	env->GetJavaVM(&gJavaVM);
	
	jclass clazz = env->GetObjectClass (obj);
	g_midSendData = env->GetMethodID(clazz, "voDRMCallback", "(IIILjava/lang/Object;)I");
	g_pThis = env->NewGlobalRef(obj);
	
	VOLOGI("Ready for init");
	
	pDRMOpenParam.nFlag = nflag;
	pDRMOpenParam.nReserve = 0;
	pDRMOpenParam.pLibOP = NULL;

	VO_S32 nRet = pApi->Init(&ppHandle, &pDRMOpenParam);
	VOLOGI("ppHandle is %p", ppHandle);
	setIntegerValue (env, handle, jint(ppHandle));

	VO_SOURCE2_IO_HTTP_VERIFYCALLBACK callBack;
	callBack.hHandle = NULL;
	callBack.HTTP_Callback = &voDRM_CallBack;
	pApi->SetParameter(ppHandle,VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK, &callBack);

	pApi->SetParameter(ppHandle,VO_PID_DRM2_PackagePath, g_szPackagePath); 
	VOLOGI("pApi->SetParameter id is %d, g_szPackagePath is %s", VO_PID_DRM2_PackagePath, g_szPackagePath);
	VOLOGI("Callback struct is %p, function is %p", &callBack, &voDRM_CallBack);

	return nRet;
	
}


/*
 * Class:     com_visualon_vome_voDRM
 * Method:    nativeUninit
 * Signature: (I)J
 */
static VO_S32 voDRM_nativeUninit  (JNIEnv * env, jobject obj, jint context, jint handle)
{
	VO_DRM2_API* pApi = (VO_DRM2_API* )context;
	VO_S32 nRet = pApi->Uninit((VO_PTR)handle);

	delete pApi;
	pApi = NULL;

	vomtFreeLib (0, g_hDll, 0);
	g_hDll = NULL;
	if (g_pThis != NULL)
	{
		env->DeleteGlobalRef(g_pThis);
		g_pThis = NULL;
	}

	VOLOGI("voDRM_nativeUninit");

	return nRet;
}


/*
 * Class:     com_visualon_vome_voDRM
 * Method:    nativeSetThirdpartyAPI
 * Signature: (ILjava/lang/Object;)J
 */
static VO_S32 voDRM_nativeSetThirdpartyAPI (JNIEnv * env, jobject obj, jint context, jint handle, jobject pTPApi)
{
	VO_DRM2_API* pApi = (VO_DRM2_API* )context;
	int tpApi = getIntegerValue(env, pTPApi);
	VOLOGI("ThirdpartyAPI is %d", tpApi);
	VO_S32 nRet = pApi->SetThirdpartyAPI((VO_PTR)handle, (void *)tpApi);

	return nRet;
}


/*
 * Class:     com_visualon_vome_voDRM
 * Method:    nativeGetInternalAPI
 * Signature: (ILjava/lang/Object;)J
 */
static VO_S32 voDRM_nativeGetInternalAPI (JNIEnv * env, jobject obj, jint context,jint handle, jobject internalAPI)
{
	VO_DRM2_API* pApi = (VO_DRM2_API* )context;
	
	VO_PTR ppParam;
	VO_S32 nRet = pApi->GetInternalAPI((VO_PTR)handle, &ppParam);

	VOLOGI("internalAPI is %p", ppParam);
	setIntegerValue(env, internalAPI, (int)ppParam);
		

	return nRet;
}


/*
 * Class:     com_visualon_vome_voDRM
 * Method:    nativeSetParameter
 * Signature: (IJLjava/lang/Object;)J
 */
static VO_S32 voDRM_nativeSetParameter (JNIEnv * env, jobject obj, jint context, jint handle, jlong nPid, jobject pParam)
{
	VO_DRM2_API* pApi = (VO_DRM2_API* )context;
	int iParam = 0;
	VO_S32 nRet = -1;

	if (pParam != NULL)
	{
		switch (nPid)
		{
			case VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER:
			{
				jboolean isCopy = false;
				
				const char* pStr = env->GetStringUTFChars ((jstring)pParam, &isCopy);
	
				nRet = pApi->SetParameter((VO_PTR)handle, VO_PID_DRM2_UNIQUE_IDENTIFIER, (VO_PTR)pStr);
			
				VOLOGI("SetParameter, nPid is %d, param is %s", (int)nPid, pStr);

				env->ReleaseStringUTFChars ((jstring)pParam, pStr);
				break;
			}
	//		case VOOSMP_SRC_PID_DOHTTPVERIFICATION:
			case VOOSMP_SRC_PID_DODRMVERIFICATION:
			{
				VOOSMP_SRC_VERIFICATIONINFO VerifyInfo;
				VerifyInfo.nResponseSize = 0;
				VerifyInfo.pszResponse = NULL;

				int iRet = 0;

				jclass clazz = env->GetObjectClass (pParam);
				if (clazz == NULL)
				{
					VOLOGE("GetObjectClass, voOSVerificationInfo error!");
					iRet = VOOSMP_ERR_JNI;
					return iRet;
				}

				jfieldID fid = env->GetFieldID (clazz, "VerificationData", "Ljava/lang/String;");
				if (fid == NULL)
				{
					VOLOGE("voOSVerificationInfo, GetFieldID VerificationData error!");
					iRet = VOOSMP_ERR_JNI;
					return iRet;
				}
				jstring jstr = (jstring)env->GetObjectField(pParam, fid);



				const char* str = env->GetStringUTFChars (jstr, NULL);
				int length = env->GetStringUTFLength (jstr);

				VerifyInfo.pData = new char[length];
				memcpy (VerifyInfo.pData, str, length);
				env->ReleaseStringUTFChars (jstr, str);


				VerifyInfo.nDataSize = length;

				jmethodID mid = env->GetMethodID (clazz, "getDataFlag", "()I");
				if (mid == NULL)
				{
					VOLOGE("voOSVerificationInfo, GetMethodID getDataFlag error!");
					iRet = VOOSMP_ERR_JNI;
					return iRet;
				}
				VerifyInfo.nDataFlag = env->CallIntMethod (pParam, mid);
				iRet = pApi->SetParameter((VO_PTR)handle, VO_PID_SOURCE2_DOHTTPVERIFICATION, &VerifyInfo);

				if (VerifyInfo.nResponseSize > 0)
				{
					jfieldID fid = env->GetFieldID (clazz, "ResponseData", "[B");
					if (fid == NULL)
					{
						VOLOGE("voOSVerificationInfo, GetFieldID ResponseData error!");
						iRet = VOOSMP_ERR_JNI;
						return iRet;
					}

					jbyteArray array = env->NewByteArray (VerifyInfo.nResponseSize);
					env->SetByteArrayRegion (array, 0, VerifyInfo.nResponseSize, (const jbyte*)VerifyInfo.pszResponse);
					env->SetObjectField (pParam, fid, array);
				}

				VOLOGI("VerifyInfo data is %s, length is %d, datatype is %d, ResponseData is %s", (char*)VerifyInfo.pData, VerifyInfo.nDataSize, VerifyInfo.nDataFlag, VerifyInfo.pszResponse);

				break;
			}
			default:
				iParam = getIntegerValue(env, pParam);
				nRet = pApi->SetParameter((VO_PTR)handle, nPid, &iParam);
				VOLOGI("SetParameter, nPid is %lld, param is %d", nPid, iParam);
				break;
		}
	}
		
	return nRet;
}


/*
 * Class:     com_visualon_vome_voDRM
 * Method:    nativeGetParameter
 * Signature: (IJLjava/lang/Object;)J
 */
static VO_S32 voDRM_nativeGetParameter (JNIEnv * env, jobject obj, jint context, jint handle, jlong nPid, jobject pParam)
{
	VO_DRM2_API* pApi = (VO_DRM2_API* )context;
	
	int iParam = 0;
	VO_S32 nRet;

	if (pParam != NULL)
	{
		switch(nPid)
		{
			default:
			{
				nRet = pApi->GetParameter((VO_PTR)handle, nPid, &iParam);
				setIntegerValue (env, pParam, iParam);
			}
		}
	}else
	{
		nRet = -1;
	}
	
	
		
	VOLOGI("GetParameter is %d", iParam);
		
	return nRet;
}




static VO_S32 voDRM_GetDRMAPI(JNIEnv *env, jobject thiz, jobject pHandle, jstring packagePath, jstring libName, jlong nflag)
{
	VOLOGI("voDRM_GetDRMAPI");
	VO_DRM2_API* pvoDRMApi = new VO_DRM2_API();
	VO_S32 nRet = 0;

	char*	pszPackage;
	pszPackage = (char *)env->GetStringUTFChars(packagePath, NULL);

	char*	pszDllFile;
	pszDllFile = (char *)env->GetStringUTFChars(libName, NULL);

	char dllFile[1024];
	memset(dllFile, 0, sizeof(dllFile));
	strcat(dllFile, pszPackage);
	strcat(dllFile, pszDllFile);

	strcpy(g_szPackagePath, pszPackage);
	voSetLibraryParam(VO_PID_LIB_SUFFIX, (void *)g_szosmp_suffix);
	g_hDll = vomtLoadLib (0, dllFile, RTLD_NOW);
	if (g_hDll == NULL)
	{
		nRet = -1;
		VOLOGE("Failed to load %s.", dllFile);
		return nRet;
	}else
	{
		VOLOGI("Success to load %s.", dllFile);
	}

	env->ReleaseStringUTFChars(libName, (const char *)pszPackage);
	pszPackage = NULL;

	env->ReleaseStringUTFChars(libName, (const char *)pszDllFile);
	pszDllFile = NULL;

	VOGETAPI pAPI = (VOGETAPI) vomtGetAddress(0, g_hDll, "voGetDRMAPI", 0);
	if (pAPI == NULL)
	{
		VOLOGI ("@@@@@@ Get function voGetDRMAPI address Error %s \n", dlerror ());
		return -1;
	}
	
	nRet = pAPI(pvoDRMApi, nflag);

	jclass clazz = env->FindClass ("java/lang/Integer");
	if (clazz == NULL)
	{
		nRet = -1;
		VOLOGE("Don't get Integer class from env!");
		return nRet;
	}	

	jfieldID field = env->GetFieldID (clazz, "value", "I");
	if (field == NULL)
	{
		nRet = -2;
		VOLOGE("Don't get 'value' field of Integer class from env!");
		return nRet;
	}
	env->SetIntField(pHandle,field,(jint)pvoDRMApi);
	VOLOGI("voDRM_GetDRMAPI, return api is %d.", (jint)pvoDRMApi);
	return nRet;
}

VO_U32 voDRM_CallBack(VO_PTR hHandle , VO_U32 uID , VO_PTR pUserData )
{
	VOLOGI("voDRM_CallBack , uID is %X " ,(int)uID);

	int nParam1 = uID;

	CJniEnvUtil	env(gJavaVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	env.getEnv()->CallIntMethod(g_pThis, g_midSendData, VOOSMP_SRC_CB_Authentication_Request, nParam1, 0, NULL);


	return 0;
}


// JNI mapping between Java methods and native methods
static JNINativeMethod nativeMethods[] = {
		{"nativeGetDRMAPI",   "(Ljava/lang/Integer;Ljava/lang/String;Ljava/lang/String;J)J", (void *)voDRM_GetDRMAPI},
		{"nativeInit", "(Ljava/lang/Integer;Ljava/lang/Integer;J)J", (void *)voDRM_Init},
		{"nativeUninit", "(II)J", (void *)voDRM_nativeUninit},
		{"nativeSetThirdpartyAPI", "(IILjava/lang/Object;)J", (void *)voDRM_nativeSetThirdpartyAPI},
		{"nativeGetInternalAPI", "(IILjava/lang/Object;)J", (void *)voDRM_nativeGetInternalAPI},
		{"nativeSetParameter", "(IIJLjava/lang/Object;)J", (void *)voDRM_nativeSetParameter},
		{"nativeGetParameter", "(IIJLjava/lang/Object;)J", (void *)voDRM_nativeGetParameter},
		};

// This function only registers the native methods
static int register_voDRMJni (JNIEnv *env)
{
	VOLOGI ("register_voDRMJni");

	jclass clazz;
	clazz = env->FindClass(g_szvodrmclsname);
	if (clazz == NULL)
	{
		VOLOGI ("%s not found!", g_szvodrmclsname);
		return JNI_FALSE;
	}

	if (env->RegisterNatives(clazz, nativeMethods, sizeof(nativeMethods) / sizeof(nativeMethods[0])) < 0)
	{
		VOLOGI ("RegisterNatives failed: '%s' ", g_szvodrmclsname);
		return JNI_FALSE;
	}
	
	VOLOGI ("Succeed to register %s.",  g_szvodrmclsname);

	return JNI_TRUE;
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	VOLOGI ("JNI_OnLoad");

	JNIEnv* env = NULL;
	jint result = -1;

	jint jniver = JNI_VERSION_1_4;
	if (vm->GetEnv((void**) &env, jniver) != JNI_OK)
	{
		jniver = JNI_VERSION_1_6;
		if (vm->GetEnv((void**)&env, jniver) != JNI_OK)
		{
			VOLOGE("ERROR: GetEnv failed\n");
			return result;
		}
	}
//	assert(env != NULL);

	if (register_voDRMJni(env) < 0) 
	{
		VOLOGE ("ERROR: voDRM native registration failed.");
		return result;
	}

	memset(g_szPackagePath, 0, sizeof(g_szPackagePath));

	// success -- return valid version number
	result = jniver;

	return result;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */



