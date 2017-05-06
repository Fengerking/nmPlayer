	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		osmpSourceJni.cpp

	Contains:	osmpSource jni entry file

	Written by:	Steven Zhang

	Change History (most recent first):
	2012-03-023	Steven			Create file

*******************************************************************************/
#include <jni.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dlfcn.h>
#include "vodlfcn.h"
#include "voSubtitleType.h"
#include "CJavaParcelWrap.h"
#include "CJavaParcelWrapOSMP.h"
#include "memoryinfo.h"

#include "COSSourceBase.h"
#define  LOG_TAG    "VOLOG"

#include "voLog.h"

static int 			g_nLOGLevel = 2;
static VO_TCHAR *	g_pLOGFilter = NULL;

static char g_szSubtitleLanguage[256];
static const char*	g_szosmp_suffix = "_OSMP";


#ifdef __cplusplus
extern "C" {
#endif

jobject demoCallBackReturnObject(JNIEnv *env, jobject obj)
{
	//demo return char*, return jstring
//	char* p = "abcddd";
//	jstring str = env->NewStringUTF (p);
//	return (jobject)str;

	//demo return struct 
	VOOSMP_SRC_CHUNK_INFO vsci;
	memset(&vsci,0, sizeof(vsci));
	vsci.nType = (VOOSMP_SRC_CHUNK_TYPE)3;

	strcpy(vsci.szRootUrl, "aabbcc");
	
	strcpy(vsci.szUrl, "eeffggg");
	
	vsci.ullDuration = 50000;
	vsci.ullStartTime = 10000;
	vsci.ullTimeScale = 3000;
	jclass cls = env->FindClass ("com/visualon/OSMPUtils/voOSChunkInfoImpl");
	if (cls == NULL)
	{
		VOLOGE("Get com.visualon.OSMPUtils/voOSChunkInfoImpl class error");
		return NULL;
	}

	jmethodID mid = env->GetMethodID (cls, "<init>", "(ILjava/lang/String;Ljava/lang/String;JJJII)V");
	if (mid == NULL)
	{
		VOLOGE("Get construct of com.visualon.OSMPUtils/voOSChunkInfoImpl error!");
		return NULL;
	}

	jobject returnobj = env->NewObject (cls, mid, vsci.nType, env->NewStringUTF((const char*)vsci.szRootUrl), env->NewStringUTF((const char*)vsci.szUrl),
									vsci.ullStartTime, vsci.ullDuration, vsci.ullTimeScale, vsci.uReserved1, vsci.uReserved2);
	env->DeleteLocalRef(cls);
	return returnobj;
	
}



int getIntegerValue(JNIEnv * env, jobject value)
{
	if (value == NULL)
		return 0;
	
	jclass clazz = env->FindClass("java/lang/Integer");

	jfieldID field = env->GetFieldID (clazz, "value" , "I");

	int nRC = env->GetIntField (value, field);

	env->DeleteLocalRef(clazz);

	return nRC;
}

float getFloatValue(JNIEnv * env, jobject value)
{
	if (value == NULL)
		return 0;
	
	jclass clazz = env->FindClass("java/lang/Float");

	jfieldID field = env->GetFieldID (clazz, "value" , "F");

	float nRC = env->GetFloatField (value, field);

	env->DeleteLocalRef(clazz);

	return nRC;
}

void setIntegerValue(JNIEnv * env, jobject obj, jint value)
{
	if (obj == NULL)
		return;
	
	jclass clazz = env->FindClass("java/lang/Integer");

	jfieldID field = env->GetFieldID (clazz, "value" , "I");

	env->SetIntField (obj, field, value);

	env->DeleteLocalRef(clazz);
}

int	VOLOGPrinf (void * pUserData, int nLevel, VO_TCHAR * pLogText)
{
	if (nLevel > g_nLOGLevel)
		return -1;

	if (g_pLOGFilter != NULL)
	{
		if (vostrstr (pLogText, g_pLOGFilter) == NULL)
		{	
			return -1;
		}
	}	
	
	if (nLevel == 0)
		LOGE ("ERROR %s", pLogText);
	else if (nLevel == 1)
		LOGW ("*WARNING %s", pLogText);
	else if (nLevel == 2)		
		LOGI ("INFO %s", pLogText);
	else if(nLevel == 3)
		LOGI ("STATUS %s", pLogText);
	else 
		LOGI ("RUN %s", pLogText);
			
	return 0;
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeCreate
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeCreate
  (JNIEnv *env, jobject obj, jobject context)
{
	COSSourceBase* pSource = new COSSourceBase();

	JavaVM *jvm = 0;
	jobject envobj;
	
	env->GetJavaVM(&jvm);
	envobj = env->NewGlobalRef(obj);
	pSource->SetJavaVM((void *)jvm, envobj);
	
	setIntegerValue (env, context, (int)pSource);
	return 0;
}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeInit
 * Signature: (Ljava/lang/Integer;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeInit
  (JNIEnv *env, jobject obj, jint context, jstring apkPath, jstring str, jint flag, jlong nSourceType, jobject initParam, jlong jinitFlag)
{
	strcpy (g_szSubtitleLanguage, "com/visualon/OSMPUtils/voOSSubtitleLanguageImpl");
	voSetLibraryParam(VO_PID_LIB_SUFFIX, (void *)g_szosmp_suffix);

	const char * strApkPath= env->GetStringUTFChars (apkPath, NULL);

	VOOSMP_INIT_PARAM init ;
	memset(&init, 0, sizeof(VOOSMP_INIT_PARAM));
	if ((jinitFlag&VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE) > 0)
	{

		jclass cls = env->GetObjectClass (initParam);
		jmethodID mid = env->GetMethodID (cls, "getFileSize", "()J");
		
		init.llFileSize = env->CallLongMethod (initParam, mid);
		VOLOGI("Initparam id is %lld, filesize is %lld", jinitFlag, init.llFileSize);
	}

	if ((jinitFlag&VOOSMP_FLAG_INIT_IO_FILE_NAME) > 0)
	{

		jclass cls = env->GetObjectClass (initParam);
		jmethodID mid = env->GetMethodID (cls, "getIOFileName", "()Ljava/lang/String;");
		
		jstring str1 = (jstring)env->CallObjectMethod (initParam, mid);
		init.pszIOFileName = (char*)env->GetStringUTFChars (str1, NULL);
		VOLOGI("Initparam id is %lld, pszIOFileName is %s", jinitFlag, init.pszIOFileName);
	}

	if ((jinitFlag&VOOSMP_FLAG_INIT_IO_API_NAME) > 0)
	{

		jclass cls = env->GetObjectClass (initParam);
		jmethodID mid = env->GetMethodID (cls, "getIOApiName", "()Ljava/lang/String;");

		jstring str2 = (jstring)env->CallObjectMethod (initParam, mid);
		init.pszIOApiName= (char*)env->GetStringUTFChars (str2, NULL);
		VOLOGI("Initparam id is %lld, pszIOApiName is %s", jinitFlag, init.pszIOApiName);
	
	}
		

	COSSourceBase* pSource = (COSSourceBase*) context;

	pSource->SetParam(VOOSMP_PID_PLAYER_PATH, (void *)strApkPath);

	jboolean bValue = false;
	char * pStr = (char*)env->GetStringUTFChars (str, &bValue);
	int nRet = pSource->Init (pStr, flag, nSourceType, (void *)&init, jinitFlag);

	env->ReleaseStringUTFChars(str, pStr);
	env->ReleaseStringUTFChars(apkPath, strApkPath);

	return nRet;
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeDestroy
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeDestroy
  (JNIEnv * env, jobject obj, jint context)

{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	delete pSource;

	if (g_pLOGFilter != NULL)
	{
		delete []g_pLOGFilter;
		g_pLOGFilter = NULL;
	}

	char szProp[64];
	memset (szProp, 0, 64);
	__system_property_get ("libc.debug.malloc", szProp);
	VOLOGI ("libc.debug.malloc prop is %s", szProp);

	if(strstr(szProp, "1") == szProp)
		memStatus("/data/local/dump/after.dat");

	return 0;

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeUninit
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeUninit
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	int nRet = pSource->Uninit ();
	
	return nRet;
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeOpen
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeOpen
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->Open();
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeRun
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeRun
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->Run ();
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativePause
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativePause
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->Pause ();

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeStop
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeStop
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->Stop();

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeClose
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeClose
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->Close ();

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeFlush
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeFlush
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return 0; //pSource->Flush ();

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetDuration
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetDuration
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	long long duration = 0;
	pSource->GetDuration (&duration);
	return (jint)duration;

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeSetPos
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSetPos
  (JNIEnv * env, jobject obj, jint context, jint nCurPos)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return -1;
	}

	long long lCurPos = nCurPos;

	int nRC = pSource->SetCurPos (&lCurPos);

	if(nRC == 0)
		return (jint)lCurPos;
	else
		return -1;
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetParam
 * Signature: (II)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetParam
  (JNIEnv * env, jobject obj, jint context, jint nID)
{
	VOLOGI("@@@### GetParam from Java,  nID = 0x%08X", nID);
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}

	if (nID == 0x03110000)
	{
		return demoCallBackReturnObject (env, obj);
	}

	if (nID == VOOSMP_PID_COMMAND_STRING)
	{
		char* pChar = new char[2048];
		memset(pChar, 0, sizeof(pChar));
		int nRC = pSource->GetParam(nID, pChar);
		if (nRC == VOOSMP_ERR_None)
		{
			jstring str = env->NewStringUTF ((const char *) pChar);
			delete pChar;
			return str;
		}else
		{
			VOLOGE("GetParam error! id is %d, error code is %d", nID, nRC);
			delete pChar;
			return NULL;
		}
			
	}else if (nID == VOOSMP_SRC_PID_DVRINFO)
	{
		VOOSMP_SRC_DVRINFO info ;
		memset(&info, 0, sizeof(VOOSMP_SRC_DVRINFO));
		int nRC = pSource->GetParam(nID, &info);
		
		if (nRC == VOOSMP_ERR_None)
		{
			jlongArray arr = env->NewLongArray (10);
			jlong l[10];
			l[0] = info.uStartTime;
			l[1] = info.uEndTime;
			l[2] = info.uPlayingTime;
			l[3] = info.uLiveTime;
			
			env->SetLongArrayRegion (arr, 0, 10, l);
			return arr;
		}else
		{
			VOLOGE("GetParam error! id is %d, error code is %d", nID, nRC);
			return NULL;
		}
	}else if (nID == VOOSMP_SRC_PID_BA_WORKMODE)
	{
		VOOSMP_SRC_ADAPTIVESTREAMING_BA_MODE enumBAMode;
		memset(&enumBAMode,0,sizeof(VOOSMP_SRC_ADAPTIVESTREAMING_BA_MODE));
		int nRC = pSource->GetParam(nID, &enumBAMode);

		if (nRC == VOOSMP_ERR_None)
		{
			jclass clazz = env->FindClass("java/lang/Integer");

			jmethodID methodID = env->GetMethodID (clazz, "<init>" , "(I)V");
			jobject jobj = env->NewObject (clazz, methodID, enumBAMode);
			env->DeleteLocalRef(clazz);
			return jobj;
		}else
		{
			VOLOGE("GetParam VOOSMP_SRC_PID_BA_WORKMODE error! id is %d, error code is %d", nID, nRC);
			return NULL;
		}

	}else if (nID == VOOSMP_SRC_PID_RTSP_STATS)
	{
		VOOSMP_SRC_RTSP_STATS struRTSPStatus;
		memset(&struRTSPStatus,0,sizeof(VOOSMP_SRC_RTSP_STATS));
		int nRC = pSource->GetParam(nID, &struRTSPStatus);

		if(nRC == VOOSMP_ERR_None)
		{
			jintArray arrRTSPStatus = env->NewIntArray (7);
			jint iRTSPStatus[7];
			iRTSPStatus[0] = struRTSPStatus.nTrackType;
			iRTSPStatus[1] = struRTSPStatus.uPacketRecved;
			iRTSPStatus[2] = struRTSPStatus.uPacketDuplicated;
			iRTSPStatus[3] = struRTSPStatus.uPacketLost;
			iRTSPStatus[4] = struRTSPStatus.uPacketSent;
			iRTSPStatus[5] = struRTSPStatus.uAverageJitter;
			iRTSPStatus[6] = struRTSPStatus.uAverageLatency;

			env->SetIntArrayRegion (arrRTSPStatus, 0, 7, iRTSPStatus);
			return arrRTSPStatus;
		}else
		{
			VOLOGE("GetParam error! id is %d, error code is %d", nID, nRC);
			return NULL;
		}
	}
	
	int iParam;
	
	int nRC = pSource->GetParam (nID, &iParam);
	
	if(nRC != VOOSMP_ERR_None)
		return 0;

	jclass clazz = env->FindClass("java/lang/Integer");

	jmethodID methodID = env->GetMethodID (clazz, "<init>" , "(I)V");
	jobject jobj = env->NewObject (clazz, methodID, iParam);

	env->DeleteLocalRef(clazz);
	return jobj;

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeSetParam
 * Signature: (IILjava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSetParam
  (JNIEnv * env, jobject obj , jint context, jint nID, jobject pParam)
{
	VOLOGI("@@@### SetParam from Java,  nID = 0x%08X", nID);
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	
	if (nID == VOOSMP_SRC_PID_DRM_FILE_NAME 
		|| nID == VOOSMP_SRC_PID_DRM_API_NAME
		|| nID == VOOSMP_PID_AUTHENTICATION_INFO
		|| nID == VOOSMP_PID_COMMAND_STRING
		|| nID == VOOSMP_PID_SUBTITLE_FILE_NAME
		|| nID == VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER
		|| nID == VOOSMP_PID_LICENSE_TEXT 
		|| nID == VOOSMP_PID_LICENSE_FILE_PATH
		|| nID == VOOSMP_PID_PLAYER_PATH
    || nID == VOOSMP_SRC_PID_UPDATE_SOURCE_URL)
	{
        if (NULL == pParam) {
            VOLOGE("pParam is NULL!");
            return VOOSMP_ERR_Pointer;
        }
        
		char * str = (char*)env->GetStringUTFChars ((jstring)pParam, NULL);
        if (NULL == str) {
            VOLOGE("str is NULL!");
            return VOOSMP_ERR_Pointer;
        }
        
		int iRet = pSource->SetParam (nID, str);
		VOLOGI("nID %d DRM_FILE_NAME, DRM_API_NAME, AUTHENTICATION_INFO, VOOSMP_PID_COMMAND_STRING, %s",
		nID, str);
		env->ReleaseStringUTFChars ((jstring) pParam, (const char *) str);
		return iRet;
	}else if (nID == VOOSMP_SRC_PID_RTSP_CONNECTION_PORT)
	{
		jint* arr;
		arr = env->GetIntArrayElements ((jintArray)pParam, NULL);
		VOOSMP_SRC_RTSP_CONNECTION_PORT perPort;
		perPort.uAudioConnectionPort = arr[0];
		perPort.uVideoConnectionPort = arr[1];
		VOLOGI("VOOSMP_SRC_PID_RTSP_CONNECTION_PORT  nID = %x, : %d, %d", nID, arr[0], arr[1]);
		int iRet = pSource->SetParam(nID, &perPort);
		return iRet;
	
	}else if (nID == VOOSMP_PID_PERFORMANCE_DATA
			|| nID == VOOSMP_SRC_PID_BA_STARTCAP)
	{
		jint* arr;
		arr = env->GetIntArrayElements ((jintArray)pParam, NULL);
		VOOSMP_PERF_DATA perfData;
		perfData.nCodecType = arr[0];
		perfData.nBitRate = arr[1];
		perfData.nVideoWidth = arr[2];
		perfData.nVideoHeight = arr[3];
		perfData.nProfileLevel = arr[4];
		perfData.nFPS = arr[5];
		
		VOLOGI("VOOSMP_PID_PERFORMANCE_DATA VOOSMP_SRC_PID_BA_STARTCAP nID = %x, : %d, %d, %d, %d ,%d, %d", nID, arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
		int iRet = pSource->SetParam(nID, &perfData);
		env->ReleaseIntArrayElements ((jintArray)pParam, arr, 0);
		return iRet;
		
	}else if (nID == VOOSMP_SRC_PID_DOHTTPVERIFICATION
			|| nID == VOOSMP_SRC_PID_DODRMVERIFICATION)
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
		iRet = pSource->SetParam(nID, &VerifyInfo);

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

//		VOLOGI("VerifyInfo data is %s, datatype is %d, ResponseData is %s", VerifyInfo.pData, VerifyInfo.nDataFlag, VerifyInfo.pszResponse);
		
		return iRet;
	}else if (nID == VOOSMP_SRC_PID_HTTP_HEADER)
	{
		VOOSMP_SRC_HTTP_HEADER header;
		memset(&header, 0, sizeof(VOOSMP_SRC_HTTP_HEADER));

		jstring str0 = (jstring)env->GetObjectArrayElement((jobjectArray)pParam, 0);
		header.pszHeaderName = (unsigned char*)env->GetStringUTFChars(str0, NULL);

		jstring str1 = (jstring)env->GetObjectArrayElement((jobjectArray)pParam, 1);
		header.pszHeaderValue = (unsigned char*)env->GetStringUTFChars(str1, NULL);

		VOLOGI("SetParam VOOSMP_SRC_PID_HTTP_HEADER,id = %d, HeaderName is %s, HeaderValue is %s .",
			nID, header.pszHeaderName, header.pszHeaderValue);
		
		int iRet = pSource->SetParam(nID, (void*)&header);

		env->ReleaseStringUTFChars(str0, (const char*)header.pszHeaderName);
		env->ReleaseStringUTFChars(str1, (const char*)header.pszHeaderValue);

		memset(&header, 0, sizeof(VOOSMP_SRC_HTTP_HEADER));

		return iRet;

	}else if (nID == VOOSMP_SRC_PID_DRM_CALLBACK_FUNC)
	{
		int p;
		p = getIntegerValue (env, pParam);
		return pSource->SetParam (nID, (void*)p);
	}else if (nID == VOOSMP_PID_LICENSE_CONTENT)	
	{
		jbyte* txt = env->GetByteArrayElements ((jbyteArray)pParam, NULL);
		
		int iRet = pSource->SetParam(nID, (void *)txt);
		env->ReleaseByteArrayElements ((jbyteArray)pParam, txt, 0);
		return iRet;		
	}else if(nID == VOOSMP_PID_AUDIO_PLAYBACK_SPEED){
			VOLOGI("source VOOSMP_PID_AUDIO_PLAYBACK_SPEED  ");
			float fParam = getFloatValue (env, pParam);
			return pSource->SetParam(nID, (void *)&fParam);
	
	}else if (nID == VOOSMP_SRC_PID_PERIOD2TIME)
	{
		VOOSMP_SRC_PERIODTIME_INFO cInfo;
        memset(&cInfo, 0, sizeof(VOOSMP_SRC_PERIODTIME_INFO));
        
        VOLOGI("Set VOOSMP_SRC_PID_PERIOD2TIME");
        
        int iRet = VOOSMP_ERR_Unknown;
        jclass clazz = env->GetObjectClass (pParam);
		if (clazz == NULL)
		{
			VOLOGE("GetObjectClass, voOSPeriodTime error!");
			iRet = VOOSMP_ERR_JNI;
			return iRet;
		}
        
        jmethodID mid = env->GetMethodID (clazz, "getPeriodSequenceNumber", "()I");
		if (mid == NULL)
		{
			VOLOGE("voOSPeriodTime, GetMethodID getPeriodSequenceNumber error!");
			iRet = VOOSMP_ERR_JNI;
			return iRet;
		}
		cInfo.uPeriodSequenceNumber = env->CallIntMethod (pParam, mid);
        VOLOGI("Set VOOSMP_SRC_PID_PERIOD2TIME uPeriodSequenceNumber:%d", cInfo.uPeriodSequenceNumber);
        
        iRet = pSource->SetParam(nID, &cInfo);
        
        jfieldID fid = env->GetFieldID(clazz, "mTimeStamp", "J");
		if (fid == NULL)
		{
			VOLOGE("voOSPeriodTime, GetFieldID mTimeStamp error!");
			iRet = VOOSMP_ERR_JNI;
			return iRet;
		}
        env->SetLongField(pParam, fid, cInfo.ullTimeStamp);
        
		return iRet;
	}else if (nID == VOOSMP_SRC_PID_HTTP_PROXY_INFO)
	{
		VOOSMP_SRC_HTTP_PROXY httpProxyInfo;
		httpProxyInfo.pFlagData = NULL;
		httpProxyInfo.uFlag = 0;
		
		int iRet = 0;
				
		jclass clazz = env->GetObjectClass (pParam);
		if (clazz == NULL)
		{
			VOLOGE("GetObjectClass, voOSHTTPProxy error!");
			iRet = VOOSMP_ERR_JNI;
			return iRet;
		}
		
		jfieldID fid = env->GetFieldID (clazz, "mProxyHost", "Ljava/lang/String;");
		if (fid == NULL)
		{
			VOLOGE("voOSHTTPProxy, GetFieldID mProxyHost error!");
			iRet = VOOSMP_ERR_JNI;
			return iRet;
		}
		jstring jstr = (jstring)env->GetObjectField(pParam, fid);
		const char* str = env->GetStringUTFChars (jstr, NULL);
		int length = env->GetStringUTFLength (jstr);
		
		httpProxyInfo.pszProxyHost = new unsigned char[length];
		memcpy (httpProxyInfo.pszProxyHost, str, length);
		env->ReleaseStringUTFChars (jstr, str);
		jmethodID mid = env->GetMethodID (clazz, "getmProxyPort", "()I");
		if (mid == NULL)
		{
			VOLOGE("voOSHTTPProxy, GetMethodID getmProxyPort error!");
			iRet = VOOSMP_ERR_JNI;
			return iRet;
		}
		httpProxyInfo.nProxyPort = env->CallIntMethod (pParam, mid);
		
		iRet = pSource->SetParam(nID, &httpProxyInfo);
		return iRet;

	}else if (nID == VOOSMP_SRC_PID_DRM_INIT_DATA_RESPONSE) 
    {
        jclass clazz = env->GetObjectClass (pParam);
        if (clazz == NULL) {
            VOLOGE("GetObjectClass, voOSDRMInit error");
            return VOOSMP_ERR_JNI;
        }

        jfieldID handleFID = env->GetFieldID (clazz, "mDRMDataHandle", "I");
        if (handleFID == NULL) {
            VOLOGE("voOSDRMInit, GetFieldID mDRMDataHandle error");
            return VOOSMP_ERR_JNI;
        }
        jfieldID dataFID = env->GetFieldID (clazz, "mDRMData", "[B");
        if (dataFID == NULL) {
            VOLOGE("voOSDRMInit, GetFieldID mDRMData error");
            return VOOSMP_ERR_JNI;
        }
        int handle = env->GetIntField(pParam, handleFID);
        jbyteArray byteData = (jbyteArray)env->GetObjectField(pParam, dataFID);
        int len = env->GetArrayLength(byteData);

        VOOSMP_SRC_DRM_INIT_DATA_RESPONSE* initData = (VOOSMP_SRC_DRM_INIT_DATA_RESPONSE*)(handle);
        
        initData->uRetSize = (unsigned)len;
        initData->pRetData = (char*)(initData->fAllocate(len));
        env->GetByteArrayRegion(byteData, 0, len, (jbyte*)initData->pRetData);
    }else if (nID == VOOSMP_SRC_PID_BITRATE_THRESHOLD)
    {
        jint* arr;
		arr = env->GetIntArrayElements ((jintArray)pParam, NULL);
		VOOSMP_SRC_BA_THRESHOLD hold;
		hold.nUpper = arr[0];
		hold.nLower = arr[1];
		VOLOGI("VOOSMP_SRC_PID_BITRATE_THRESHOLD  nID = %x, upper is %d, lower is %d", nID, hold.nUpper, hold.nLower);
		int iRet = pSource->SetParam(nID, &hold);
		return iRet;
    }
	
	int iParam;
	iParam = getIntegerValue (env, pParam);
	return pSource->SetParam (nID, &iParam);

}

JNIEXPORT jobject JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetSample (JNIEnv * env, jobject obj , jint context, jint nTrackType, jint nTimeStamp)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}

	if(nTrackType == (jint)VO_SOURCE2_TT_SUBTITLE)
	{
		voSubtitleInfo sample;
		memset(&sample,0,sizeof(voSubtitleInfo));
		sample.nTimeStamp = nTimeStamp;
		pSource->SetParam(ID_SET_JAVA_ENV, env);
		int nRC = pSource->GetSample(VO_SOURCE2_TT_SUBTITLE, &sample);
		if(nRC == VOOSMP_ERR_None)
		{
			CJavaParcelWrap	javaParcel(env);
			CJavaParcelWrap::getParcelFromSubtitleInfo(&sample, &javaParcel);
			return javaParcel.getParcel();
		}
	}
	return 0; //pSource->Flush ();
}


JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetProgramCount (JNIEnv * env, jobject obj , jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}
	
	unsigned int nCount = 0;

	pSource->GetProgramCount(&nCount);

	return nCount;
}


JNIEXPORT jobject JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetProgramInfo(JNIEnv * env, jobject obj , jint context, jint nStream)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}
	
	VOOSMP_SRC_PROGRAM_INFO* pProgramInfo = NULL;

	int nRC = pSource->GetProgramInfo(nStream, &pProgramInfo);

	if(nRC == VOOSMP_ERR_None)
	{
		if(pProgramInfo == NULL)
			return 0;
		CJavaParcelWrapOSMP wrap(env);
		wrap.fillParcelData(pProgramInfo);
		return wrap.getParcel();
	}

	return 0;
}

JNIEXPORT jobject JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetCurTrackInfo (JNIEnv * env, jobject obj , jint context, jint nTrackType)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}

	VOOSMP_SRC_TRACK_INFO* pTrackInfo = NULL;

	int nRC = pSource->GetCurTrackInfo(nTrackType, &pTrackInfo);

	if(nRC == VOOSMP_ERR_None)
	{
		CJavaParcelWrapOSMP wrap(env);
		wrap.fillParcelData(pTrackInfo);
		return wrap.getParcel();
	}

	return 0;

}

JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSelectProgram (JNIEnv * env, jobject obj , jint context, jint nProgram)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->SelectProgram((int)nProgram);
}

JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSelectStream (JNIEnv * env, jobject obj , jint context, jint nStreamID)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->SelectStream((int)nStreamID);
}

JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSelectTrack (JNIEnv * env, jobject obj , jint context, jint nTrackID)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return pSource->SelectTrack((int)nTrackID);
}

JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSendBuffer ( JNIEnv * env, jobject obj , jint context, jint pBuffer)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	return 0; 
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetSubtitleLanguageCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetSubtitleLanguageCount (JNIEnv *env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	int iNum = 0;
	pSource->GetSubLangNum(&iNum);
	return iNum; 
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetSubtitleLanguageInfo
 * Signature: (II)Ljava/lang/Object;
 */
JNIEXPORT jobject JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetSubtitleLanguageInfo (JNIEnv *env, jobject obj, jint context, jint nIndex)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}

	VOOSMP_SUBTITLE_LANGUAGE* pvsl = NULL; 
	pSource->GetSubLangItem (nIndex, &pvsl);
	
	if (pvsl == NULL)

	{
		VOLOGE("GetSubLangItem is null");
		return NULL;
	}
	jclass cls = env->FindClass (g_szSubtitleLanguage);
	if (cls == NULL)
	{
		VOLOGE("Dont find class %s", g_szSubtitleLanguage);
		return NULL;
	}

	jmethodID mid = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;IIII)V");
	if (mid == NULL)
	{
		VOLOGE("Dont get construct of  class %s", g_szSubtitleLanguage);
		return NULL;
	}

	jstring jstr = env->NewStringUTF ((const char*)pvsl->szLangName);

	jobject object = env->NewObject (cls, mid, jstr, pvsl->nLanguageType, pvsl->nFlag, pvsl->nReserved1, pvsl->nReserved2);
	
	env->DeleteLocalRef(cls);
	return object;
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeSelectSubtitleLanguage
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSelectSubtitleLanguage (JNIEnv *env, jobject obj, jint context, jint nIndex)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->SelectLanguage(nIndex);

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetVideoCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetVideoCount
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->GetMediaCount(VOOSMP_SS_VIDEO);

}  

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetAudioCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetAudioCount
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->GetMediaCount(VOOSMP_SS_AUDIO);

}  


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetSubtitleCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetSubtitleCount
  (JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->GetMediaCount(VOOSMP_SS_SUBTITLE);

}  


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeSelectVideo
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSelectVideo
	(JNIEnv * env, jobject obj, jint context, jint index)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->SelectMediaIndex (VOOSMP_SS_VIDEO, index);

}  


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeSelectAudio
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSelectAudio
	(JNIEnv * env, jobject obj, jint context, jint index)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->SelectMediaIndex (VOOSMP_SS_AUDIO, index);

}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeSelectSubtitle
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeSelectSubtitle
	(JNIEnv * env, jobject obj, jint context, jint index)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->SelectMediaIndex (VOOSMP_SS_SUBTITLE, index);

}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeIsVideoAvailable
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeIsVideoAvailable
	(JNIEnv * env, jobject obj, jint context, jint index)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}

	return pSource->IsMediaIndexAvailable (VOOSMP_SS_VIDEO, index);

}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeIsAudioAvailable
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeIsAudioAvailable
	(JNIEnv * env, jobject obj, jint context, jint index)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}

	return pSource->IsMediaIndexAvailable (VOOSMP_SS_AUDIO, index);

}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeIsSubtitleAvailable
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeIsSubtitleAvailable
	(JNIEnv * env, jobject obj, jint context, jint index)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return 0;
	}

	return pSource->IsMediaIndexAvailable (VOOSMP_SS_SUBTITLE, index);

}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeCommitSelection
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeCommitSelection
	(JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->CommetSelection (1);

}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeClearSelection
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeClearSelection
	(JNIEnv * env, jobject obj, jint context)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	return pSource->CommetSelection (0);

}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetPropertyCount
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetPropertyCount
	(JNIEnv * env, jobject obj, jint context, jint type, jint index )
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}
	
	VOOSMP_SRC_TRACK_PROPERTY* pProperty = NULL;

	if (pSource->GetMediaProperty (type, index, &pProperty) != 0)
		return VOOSMP_ERR_Unknown;

	return pProperty->nPropertyCount;

}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetPropertyKeyName
 * Signature: (IIII)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetPropertyKeyName
  (JNIEnv * env, jobject obj, jint context, jint type, jint index, jint keyIndex)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return NULL;
	}
	
	VOOSMP_SRC_TRACK_PROPERTY* pProperty = NULL;

	if (pSource->GetMediaProperty (type, index, &pProperty) != 0 || keyIndex >= pProperty->nPropertyCount)
		return NULL;

	jstring jstr = env->NewStringUTF ((const char *)(pProperty->ppItemProperties[keyIndex]->szKey));
	return jstr;
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetPropertyValue
 * Signature: (IIII)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetPropertyValue
  (JNIEnv * env, jobject obj, jint context, jint type, jint index, jint keyIndex)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return NULL;
	}
	
	VOOSMP_SRC_TRACK_PROPERTY* pProperty = NULL;

	if (pSource->GetMediaProperty (type, index, &pProperty) != 0 || keyIndex >= pProperty->nPropertyCount)
		return NULL;

	if (keyIndex <0 || keyIndex >= pProperty->nPropertyCount)
	{
		VOLOGE("keyIndex is invalid, %d, PropertyCount is %d", keyIndex, pProperty->nPropertyCount);
		return NULL;
	}
	VOLOGI("Property get value, type is %d, index %d, keyIndex is %d, %s",
		type, index, keyIndex, pProperty->ppItemProperties);
	jstring jstr = env->NewStringUTF ((const char *)(pProperty->ppItemProperties[keyIndex]->pszProperty));
	return jstr;
}


/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetCurrTrackSelection
 * Signature: (ILjava/lang/Integer;Ljava/lang/Integer;Ljava/lang/Integer;)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetCurrTrackSelection
  (JNIEnv * env, jobject obj, jint context, jobject videoIndex, jobject audioIndex, jobject subtitleIndex)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	VOOSMP_SRC_CURR_TRACK_INDEX currentIndex;
	memset(&currentIndex, 0, sizeof(VOOSMP_SRC_CURR_TRACK_INDEX));

	pSource->GetCurSelectedMediaTrack (&currentIndex);

	setIntegerValue(env, videoIndex, currentIndex.nCurrVideoIdx);
	setIntegerValue(env, audioIndex, currentIndex.nCurrAudioIdx);
	setIntegerValue(env, subtitleIndex, currentIndex.nCurrSubtitleIdx);

	return VOOSMP_ERR_None;
}

/*
 * Class:     com_visualon_OSMPDataSource_voOSDataSource
 * Method:    nativeGetCurrTrackPlaying
 * Signature: (ILjava/lang/Integer;Ljava/lang/Integer;Ljava/lang/Integer;)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_OSMPDataSource_voOSDataSource_nativeGetCurrTrackPlaying
  (JNIEnv * env, jobject obj, jint context, jobject videoIndex, jobject audioIndex, jobject subtitleIndex)
{
	COSSourceBase* pSource = (COSSourceBase*) context;
	if (pSource == NULL)
	{
		return VOOSMP_ERR_Pointer;
	}

	VOOSMP_SRC_CURR_TRACK_INDEX currentIndex;
	memset(&currentIndex, 0, sizeof(VOOSMP_SRC_CURR_TRACK_INDEX));

	pSource->GetCurPlayingMediaTrack (&currentIndex);

	setIntegerValue(env, videoIndex, currentIndex.nCurrVideoIdx);
	setIntegerValue(env, audioIndex, currentIndex.nCurrAudioIdx);
	setIntegerValue(env, subtitleIndex, currentIndex.nCurrSubtitleIdx);

	return VOOSMP_ERR_None;
}


#ifdef __cplusplus
}
#endif

