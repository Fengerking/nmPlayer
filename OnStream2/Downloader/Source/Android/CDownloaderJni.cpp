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
 * @file CDownloaderJni.cpp
 * jni interface for java.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2003-2013 
 ************************************************************************/
#include <jni.h>

#include "CDownloaderImpl.h"
#include "CJniEnvUtil.h"
#define  LOG_TAG    "CDownloaderJni"
#include "voLog.h"

static const char*	g_szosmp_suffix = "_OSMP";
JavaVM *g_JVM = 0;
jobject g_envobj = NULL;

#ifdef __cplusplus
extern "C" {
#endif

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

void setIntegerValue(JNIEnv * env, jobject obj, jint value)
{
	if (obj == NULL)
		return;

	jclass clazz = env->FindClass("java/lang/Integer");

	jfieldID field = env->GetFieldID (clazz, "value" , "I");

	env->SetIntField (obj, field, value);

	env->DeleteLocalRef(clazz);
}

int SendEvent (void* pUserData, unsigned int nID, void* nParam1, void* nParam2)
{

	VOLOGI("voDownloaderJNI_CallBack , uID is %X " ,(int)nID);


	CJniEnvUtil	env(g_JVM);

	if(env.getEnv() == NULL)
		return VOOSMP_ERR_Unknown;

	jclass clazz = env.getEnv()->GetObjectClass(g_envobj);
	jmethodID method = env.getEnv()->GetMethodID(clazz, "VOOSMPStreamingDownloaderCallBack", "(IIILjava/lang/Object;)I");

	jobject obj = NULL;
	int param1 = 0, param2 = 0;

	switch (nID)
	{
	case EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_OK:
		obj = env.getEnv()->NewStringUTF((const char*)nParam1);
		break;
	case EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_UPDATE:
		{
			DOWNLOADER_PROGRESS_INFO* info = (DOWNLOADER_PROGRESS_INFO*)nParam1;
			param1 = info->uCurrDuration;
			param2 = info->TotalDuration;
		}
		break;
	default:
		break;
	}

	int ret = env.getEnv()->CallIntMethod(g_envobj, method, nID, param1, param2, obj);

	return ret;
}

jobjectArray GetProperty ( JNIEnv * env,VOOSMP_SRC_TRACK_PROPERTY* property)
{
	int count = property->nPropertyCount;

	jclass clazz = env->FindClass("java/lang/String");
	jobjectArray array = env->NewObjectArray(count*2, clazz, NULL);

	for (int i = 0; i < count; i++)
	{
		env->SetObjectArrayElement(array, 2*i, (jobject)env->NewStringUTF((const char *)property->ppItemProperties[i]->szKey));
		env->SetObjectArrayElement(array, 2*i + 1, (jobject)env->NewStringUTF((const char *)property->ppItemProperties[i]->pszProperty));
	}

	return array;

}
/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeInit
 * Signature: (Ljava/lang/Integer;Lcom/visualon/VOOSMPStreamingDownloader/VOOSMPStreamingDownloaderInitParam;)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeInit
  (JNIEnv * env, jobject obj, jobject context, jobject initParam)
{
    env->GetJavaVM(&g_JVM);
	g_envobj = env->NewGlobalRef(obj);
    voSetLibraryParam(VO_PID_LIB_SUFFIX, (void *)g_szosmp_suffix);
    VOLOGI("voSetLibraryParam, id is %d, suffix is %s", VO_PID_LIB_SUFFIX, g_szosmp_suffix);

	if (initParam == NULL)
		return VOOSMP_ERR_ParamID;

	jclass clazz = env->GetObjectClass(initParam);
	if (clazz == NULL)
	{
		VOLOGE (" Error! Can't find Java class com/visualon/VOOSMPStreamingDownloader/VOOSMPStreamingDownloaderInitParam.");
		return VOOSMP_ERR_JNI;
	}
	jmethodID method = env->GetMethodID(clazz, "getLibraryPath" , "()Ljava/lang/String;");
	if (method == NULL)
	{
		VOLOGE (" Error! Can't find Java class method getLibraryPath.");
		return VOOSMP_ERR_JNI;
	}
	jstring str = (jstring)env->CallObjectMethod(initParam, method);
	const char* libPath = env->GetStringUTFChars(str, NULL);

	CDownloaderImpl* downloader = new CDownloaderImpl();

	DOWNLOADER_API_INITPARAM api_init;
	api_init.strWorkPath = (void*)libPath;
	DOWNLOADER_API_EVENTCALLBACK callbackFunc;
	callbackFunc.pUserData = downloader;
	callbackFunc.SendEvent = SendEvent;
	
	int ret = downloader->Init(&callbackFunc, &api_init);

	setIntegerValue(env, context, (jint)downloader);
	VOLOGI("DownloaderJNI init, library path is %s, context is %p", libPath, downloader);
	env->ReleaseStringUTFChars (str, libPath);
    return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeUnInit
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeUnInit
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->Uninit();

	if(g_envobj!=NULL)
		env->DeleteGlobalRef((jobject)g_envobj);

	g_envobj = NULL;

	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeOpen
 * Signature: (ILjava/lang/String;ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeOpen
  (JNIEnv * env, jobject obj, jint context, jstring source, jint flag, jstring localDir)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	const char* szSource;
	const char* szLocalDir;

	szSource = env->GetStringUTFChars(source, NULL);
	szLocalDir = env->GetStringUTFChars(localDir, NULL);
	VOLOGI("DownloaderJNI open, source is %s, flag is %d, localDir is %s", szSource, flag, szLocalDir);

	int ret = downloader->Open((void*)szSource, flag, (void*)szLocalDir);
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeClose
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeClose
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->Close();
    return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeStart
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeStart
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->StartDownload();
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeStop
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeStop
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->StopDownload();
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativePause
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativePause
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->PauseDownload();
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeResume
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeResume
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->ResumeDownload();
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetVideoCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetVideoCount
  (JNIEnv * env, jobject obj, jint context)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int count = downloader->GetVideoCount();
	return count;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetAudioCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetAudioCount
  (JNIEnv * env, jobject obj, jint context)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int count = downloader->GetAudioCount();
	return count;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetSubtitleCount
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetSubtitleCount
  (JNIEnv * env, jobject obj, jint context)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int count = downloader->GetSubtitleCount();
	return count;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeSelectVideo
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeSelectVideo
  (JNIEnv * env, jobject obj, jint context, jint index)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->SelectVideo(index);
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeSelectAudio
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeSelectAudio
  (JNIEnv * env, jobject obj, jint context, jint index)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->SelectAudio(index);
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeSelectSubtitle
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeSelectSubtitle
  (JNIEnv * env, jobject obj, jint context, jint index)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	int ret = downloader->SelectSubtitle(index);
	return ret;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeIsVideoAvailable
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeIsVideoAvailable
  (JNIEnv * env, jobject obj, jint context, jint index)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return false;
	}

	return downloader->IsVideoAvailable(index);
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeIsAudioAvailable
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeIsAudioAvailable
  (JNIEnv * env, jobject obj, jint context, jint index)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return false;
	}

	return downloader->IsAudioAvailable(index);
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeIsSubtitleAvailable
 * Signature: (II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeIsSubtitleAvailable
  (JNIEnv * env, jobject obj, jint context, jint index)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return false;
	}

	return downloader->IsSubtitleAvailable(index);
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeCommitSelection
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeCommitSelection
  (JNIEnv * env, jobject obj, jint context)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	return downloader->CommitSelection();
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeClearSelection
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeClearSelection
  (JNIEnv * env, jobject obj, jint context)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	return downloader->ClearSelection();
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetVideoProperty
 * Signature: (II)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetVideoProperty
  (JNIEnv * env, jobject obj, jint context, jint index)
{
    CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return NULL;
	}

	VOOSMP_SRC_TRACK_PROPERTY* property = NULL;

	int ret = downloader->GetVideoProperty(index, &property);
	if (ret != VOOSMP_ERR_None || property == NULL)
	{
		VOLOGE("DownloaderJNI GetVideoProperty failed, index is %d", index);
		return NULL;
	}

	return GetProperty(env, property);
}
/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetAudioProperty
 * Signature: (II)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetAudioProperty
  (JNIEnv * env, jobject obj, jint context, jint index)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return NULL;
	}

	VOOSMP_SRC_TRACK_PROPERTY* property = NULL;

	int ret = downloader->GetAudioProperty(index, &property);
	if (ret != VOOSMP_ERR_None || property == NULL)
	{
		VOLOGE("DownloaderJNI GetAudioProperty failed, index is %d", index);
		return NULL;
	}

	return GetProperty(env, property);
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetSubtitleProperty
 * Signature: (II)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetSubtitleProperty
  (JNIEnv * env, jobject obj, jint context, jint index)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return NULL;
	}

	VOOSMP_SRC_TRACK_PROPERTY* property = NULL;

	int ret = downloader->GetSubtitleProperty(index, &property);
	if (ret != VOOSMP_ERR_None || property == NULL)
	{
		VOLOGE("DownloaderJNI GetSubtitleProperty failed, index is %d", index);
		return NULL;
	}

	return GetProperty(env, property);
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetCurrentSelection
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetCurrentSelection
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return NULL;
	}

	VOOSMP_SRC_CURR_TRACK_INDEX currentIndex;

	int ret = downloader->GetCurrSelectedTrackIndex(&currentIndex);
	if (ret != VOOSMP_ERR_None)
	{
		VOLOGE("DownloaderJNI GetCurrSelectedTrackIndex failed, index is %d", index);
		return NULL;
	}

	jint data[3];
	data[0] = currentIndex.nCurrVideoIdx;
	data[1] = currentIndex.nCurrAudioIdx;
	data[2] = currentIndex.nCurrSubtitleIdx;

	jintArray array = env->NewIntArray(3);
	env->SetIntArrayRegion(array, 0, 3, data);
	return array;
}

/*
 * Class:     com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl
 * Method:    nativeGetPlayingAsset
 * Signature: (I)[I
 */
JNIEXPORT jintArray JNICALL Java_com_visualon_VOOSMPStreamingDownloaderImpl_VOOSMPStreamingDownloaderImpl_nativeGetPlayingAsset
  (JNIEnv * env, jobject obj, jint context)
{
	CDownloaderImpl* downloader = (CDownloaderImpl*)context;
	if (downloader == NULL)
	{
		VOLOGE("The downloader context pointer is NULL!");
		return NULL;
	}

	VOOSMP_SRC_CURR_TRACK_INDEX currentIndex;

	int ret = downloader->GetCurrPlayingTrackIndex(&currentIndex);
	if (ret != VOOSMP_ERR_None)
	{
		VOLOGE("DownloaderJNI GetCurrSelectedTrackIndex failed, index is %d", index);
		return NULL;
	}

	jint data[3];
	data[0] = currentIndex.nCurrVideoIdx;
	data[1] = currentIndex.nCurrAudioIdx;
	data[2] = currentIndex.nCurrSubtitleIdx;

	jintArray array = env->NewIntArray(3);
	env->SetIntArrayRegion(array, 0, 3, data);
	return array;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	//VOLOGI ();

	JNIEnv* env = NULL;
	jint result = -1;

	jint jniver = JNI_VERSION_1_4;
	if (vm->GetEnv((void**) &env, jniver) != JNI_OK)
	{
		jniver = JNI_VERSION_1_6;
		if (vm->GetEnv((void**)&env, jniver) != JNI_OK)
		{
			LOGE("ERROR: GetEnv failed\n");
			return result;
		}
	}

	// success -- return valid version number
	result = jniver;
	return result;
}

#ifdef __cplusplus
}
#endif

