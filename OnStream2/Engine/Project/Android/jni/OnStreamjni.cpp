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
 * @file OnStreamjni.cpp
 * jni interface for java.
 *
 * 
 *
 *
 * @author  Zhang Yi
 * @date    2012-2012 
 ************************************************************************/
#include <jni.h>
#include <android/log.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>
#include "voOSFunc.h"
#include "voOnStreamType.h"
#include "COSNdkVomePlayer.h"
#include "COMXALPlayer.h"
#include "voSubtitleType.h"
#include "CJavaParcelWrap.h"
#include "CJavaParcelWrapOSMP.h"


#define  LOG_TAG    "OnStreamJni"
#include "voLog.h"

static char g_szOnStreamPlayerName[256];
static char g_szOnStreamVideoRndName[256];
static char g_szOnStreamAudioRndName[256];
static char g_szSubtitleLanguage[256];
static const char*	g_szosmp_suffix = "_OSMP";

static int 			g_nLOGLevel = 2;
static VO_TCHAR *	g_pLOGFilter = NULL;

// added by gtxia for fixing issue that set android:minSdkVersion over 14
// JNI ERROR (app bug): attempt to use stale local reference
static jobject      g_Surface = NULL; 

#if defined __cplusplus 
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

static int updateVideoSize (JNIEnv* env, jobject thiz, int nWidth, int nHeight)
{
	jclass clsVR = env->GetObjectClass(thiz);
	if (clsVR == NULL) 
	{
		//LOGE (" Error! Can't find Java class:");
		return -1;
	}

	jfieldID cntWidth = env->GetFieldID (clsVR, "mWidthVideo", "I");
	if (cntWidth == NULL)
	{
		//LOGE (" Error! Can't find Java field mWidthVideo:");
		return -1;
	}
	env->SetIntField(thiz, cntWidth, nWidth);

	jfieldID cntHeight = env->GetFieldID (clsVR, "mHeightVideo", "I");
	if (cntHeight == NULL)
	{
		//LOGE (" Error! Can't find Java field mWidthVideo:");
		return -1;
	}
	env->SetIntField(thiz, cntHeight, nHeight);

	//LOGE (" SetVideo size, Width %d Height %d", nWidth, nHeight);

	env->DeleteLocalRef(clsVR);

	return 0;
}

static int updateAudioFormat (JNIEnv* env, jobject thiz, int nSampleRate, int nChannels, int nSampleBits = 16, int nAOutputType= 1)
{
	jclass clsVR = env->GetObjectClass(thiz);
	if (clsVR == NULL) 
	{
		//LOGE (" Error! Can't find Java class:");
		return -1;
	}

	jfieldID cntSampleRate = env->GetFieldID (clsVR, "mSampleRate", "I");
	if (cntSampleRate == NULL)
	{
		//LOGE (" Error! Can't find Java field nSampleRate:");
		return -1;
	}
	env->SetIntField(thiz, cntSampleRate, nSampleRate);

	jfieldID cntChannels = env->GetFieldID (clsVR, "mChannels", "I");
	if (cntChannels == NULL)
	{
		//LOGE (" Error! Can't find Java field mChannels:");
		return -1;
	}
	env->SetIntField(thiz, cntChannels, nChannels);


	jfieldID cntSampleBits = env->GetFieldID (clsVR,"mSampleBit", "I");
	if (cntSampleBits== NULL)
	{
		//LOGE (" Error! Can't find Java field cntSampleBits:");
		return -1;
	}
	env->SetIntField(thiz, cntSampleBits, nSampleBits);

	jfieldID cntAOutputType = env->GetFieldID (clsVR, "mAudioRenderFormat", "I");
	if (cntAOutputType == NULL)
	{
		//LOGE (" Error! Can't find Java field mAudioRenderFormat:");
		return -1;
	}
	env->SetIntField(thiz, cntAOutputType, nAOutputType);

	//LOGE (" SetAudio sample, sample rate %d channel %d outputType %d, sampleBit %d", nSampleRate, nChannels, nAOutputType, nSampleBits);

	env->DeleteLocalRef(clsVR);

	return 0;
}

static void SetSurface(JNIEnv* env,jobject obj, IOSBasePlayer * pPlayer)
{
	jclass		clazz = env->FindClass(g_szOnStreamPlayerName);
	if (clazz == NULL) return;

	jfieldID	surfaceID = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
	if (surfaceID == NULL) return;

	jobject		surfaceObj = env->GetObjectField(obj, surfaceID);
	//if (surfaceObj == NULL) return;

	if(g_Surface != NULL)
	{
		env->DeleteGlobalRef(g_Surface);
		g_Surface = NULL;
	}	
	assert(g_Surface == NULL);
	g_Surface = env->NewGlobalRef(surfaceObj);
	if(pPlayer) 
   {	
	   pPlayer->SetView(g_Surface);
   }

	env->DeleteLocalRef(clazz);

	return;
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


static jlong onstream_player_nativeCreate(JNIEnv * env, jobject obj, jobject weak_this, jstring apkPath, jlong jlPlayerType, jlong initParam, jlong jFlag)
{
	VO_LOG_PRINT_CB cbVOLOG;
	cbVOLOG.pUserData = NULL;
	cbVOLOG.fCallBack = VOLOGPrinf;
	
	
	long nPlayerType = (long)jlPlayerType;
	long nFlag = jFlag;
	void* pInitParam = (void*)initParam;
	void* pLibOP = NULL;
	IOSBasePlayer * pPlayer = 0;

	char g_szPathLib[1024];
	memset(g_szPathLib, 0, 1024*sizeof(char));

	char szProp[256];
	memset (szProp, 0, 256);
	voSetLibraryParam(VO_PID_LIB_SUFFIX, (void *)g_szosmp_suffix);
	if (strlen (szProp) > 0)
		g_nLOGLevel = atoi (szProp);
		
	memset (szProp, 0, 256);
	if (strlen (szProp) > 0)
	{
		if (g_pLOGFilter != NULL)
			delete []g_pLOGFilter;
		g_pLOGFilter = new VO_TCHAR[vostrlen (szProp) + 1];
		vostrcpy (g_pLOGFilter, szProp);
	}

	char * str= (char *)env->GetStringUTFChars (apkPath, NULL);
	strcpy(g_szPathLib, str);
	env->ReleaseStringUTFChars (apkPath, str);
	VOLOGI("apk path is %s", g_szPathLib);

	if(nPlayerType == VOOSMP_VOME2_PLAYER)
		pPlayer = new COSNdkVomePlayer();
	else if(nPlayerType == VOOSMP_OMXAL_PLAYER)
		pPlayer = new COMXALPlayer();
	else
		return VOOSMP_ERR_ParamID;

	if(pPlayer == 0)
		return VOOSMP_ERR_OutMemory;

	int nRC = pPlayer->SetParam (VOOSMP_PID_COMMON_LOGFUNC, &cbVOLOG);
	nRC = pPlayer->SetParam(VOOSMP_PID_PLAYER_PATH, g_szPathLib);


	if(nFlag == VOOSMP_FLAG_INIT_LIBOP && pInitParam != 0)
	{
		pLibOP = pInitParam;
		nRC = pPlayer->SetParam(VOOSMP_PID_FUNC_LIB, pLibOP);
	}

	nRC = pPlayer->Init();
	if(nRC) 
	{
		VOLOGE (" Error! Can't Init the player");
		delete pPlayer;
		return nRC;
	}
	
	JavaVM *jvm = 0;
	jobject envobj;
	
	env->GetJavaVM(&jvm);
	envobj = env->NewGlobalRef(obj);

	pPlayer->SetJavaVM((void *)jvm, envobj);

	jclass		clazz;
	clazz = env->GetObjectClass(obj);
	if (clazz == NULL) 
	{
		pPlayer->Uninit();
		env->DeleteGlobalRef(envobj);
		delete pPlayer;
		VOLOGE (" Error! Can't find Java class:");
		return VOOSMP_ERR_Pointer;
	}
	jfieldID context = env->GetFieldID(clazz, "mNativeContext", "I");
	if (context == NULL)
	{
		pPlayer->Uninit();
		env->DeleteGlobalRef(envobj);
		delete pPlayer;
		VOLOGE (" Error! Can't find Java field mNativeContext:");
		return VOOSMP_ERR_Pointer;
	}

	env->SetIntField(obj, context, (int)pPlayer);

	env->DeleteLocalRef(clazz);

	return VOOSMP_ERR_None;
}

static jlong onstream_player_nativeDestroy(JNIEnv * env, jobject obj, jint context)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	pPlayer->Uninit();
	void* obj2 = pPlayer->GetJavaObj();
	if(obj2!=NULL)
		env->DeleteGlobalRef((jobject)obj2);

	delete pPlayer;
	VOLOGI ("Leave Function");

	if (g_pLOGFilter != NULL)
	{
		delete []g_pLOGFilter;
		g_pLOGFilter = NULL;
	}
	
	if(g_Surface != NULL)
	{
		env->DeleteGlobalRef(g_Surface);
		g_Surface = NULL;
	}	
	return 0;
}

static jlong onstream_player_nativeOpen(JNIEnv * env, jobject obj, jint context, jobject source, jlong flag)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	SetSurface(env,obj,pPlayer);

	int nOpenFlag = flag;
	int nRC = 0;

	if (nOpenFlag == VOOSMP_FLAG_SOURCE_URL)
	{
		const char* pSource = env->GetStringUTFChars ((jstring)source, NULL);
		nRC = pPlayer->SetDataSource((void*)pSource, nOpenFlag);
		env->ReleaseStringUTFChars((jstring)source, pSource);
	}else 
	{
		int iSource = getIntegerValue (env, source);
		nRC = pPlayer->SetDataSource((void*)iSource, nOpenFlag);
	}
	if (nRC != 0) 
		VOLOGE ("Open Source failed. Return %08X", nRC);

	return nRC;
}

static jlong onstream_player_nativeRun(JNIEnv * env, jobject obj, jint context)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}
	
	return pPlayer->Run();
}

static jlong onstream_player_nativePause(JNIEnv * env, jobject obj, jint context)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}
	
	return pPlayer->Pause();
}

static jlong onstream_player_nativeStop(JNIEnv * env, jobject obj, jint context)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}
	
	return pPlayer->Stop();
}

static jlong onstream_player_nativeClose(JNIEnv * env, jobject obj, jint context)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}
	
//	return pPlayer->Close();
	return 0;
}


static jlong onstream_player_nativeSetPos(JNIEnv * env, jobject obj, jint context, jlong pos)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}
	
	int nPos = pos;
	return 	pPlayer->SetPos(nPos);
}

static jlong onstream_player_nativeGetPos(JNIEnv * env, jobject obj, jint context)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}
	
	int nCurPos = 0;

	int nRC = pPlayer->GetPos(&nCurPos);

	if(nRC) return 0;

	return nCurPos;
}

static jlong onstream_player_nativeGetVideoData (JNIEnv* env, jobject obj, jint context, jcharArray data)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return -1;
	}


	int nPlayerType = 0;
	pPlayer->GetParam(VOOSMP_PID_PLAYER_TYPE, &nPlayerType);
	
	if(nPlayerType != 0)
	{
		voOS_Sleep(20);
		return -1;
	}

	int nRC = 0;
	unsigned long lSize  = 0;
	jchar* pData = 0;
	
	if (data != NULL)
	{
		lSize = env->GetArrayLength(data);
		pData = env->GetCharArrayElements(data, 0);
		if (pData == NULL)
			return -1;
	}

	nRC = ((COSNdkVomePlayer *)pPlayer)->GetVideoData((unsigned char *)pData, (int)lSize, env);

	if (data != NULL) 
		env->ReleaseCharArrayElements(data, pData, 0);

	if (nRC == VOOSMP_ERR_FormatChange)
	{
		VOOSMP_VIDEO_FORMAT	fmtVideo;
		memset (&fmtVideo, 0, sizeof (VOOSMP_VIDEO_FORMAT));
		pPlayer->GetParam(VOOSMP_PID_VIDEO_FORMAT, &fmtVideo);
		updateVideoSize (env, obj, fmtVideo.Width , fmtVideo.Height);
		return 0;
	}

	return nRC;
}

static jlong onstream_player_nativeGetAudioData (JNIEnv* env, jobject obj, jint context, jcharArray data)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return -1;
	}

	int nPlayerType = 0;
	pPlayer->GetParam(VOOSMP_PID_PLAYER_TYPE, &nPlayerType);
	
	int nRC = 0;
	int Filled = 0;
	if (data == NULL)
	{
		VOOSMP_AUDIO_FORMAT	fmtAudio;
		memset (&fmtAudio, 0, sizeof (VOOSMP_AUDIO_FORMAT));
		pPlayer->GetParam(VOOSMP_PID_AUDIO_FORMAT, &fmtAudio);

		
		// added by gtxia for get the audio decoder output type
		int nType = 0;
		//VOLOGE("EAC3 data == NULL before call VOOSMP_PID_AUDIO_RENDER_DATA_FORMAT)\n");
		
		pPlayer->GetParam(VOOSMP_PID_AUDIO_RENDER_DATA_FORMAT, &nType);
		//VOLOGE("EAC3 nType = %d(%x)\n", nType, nType);
		updateAudioFormat (env, obj, fmtAudio.SampleRate , fmtAudio.Channels, fmtAudio.SampleBits, nType);
	
		return 0;
	}
	
	unsigned long lSize = env->GetArrayLength(data);
	jchar* pData = env->GetCharArrayElements(data, 0);
	if (pData == NULL)
		return VOOSMP_ERR_SmallSize;

	if(nPlayerType == 0)
		nRC = ((COSNdkVomePlayer *)pPlayer)->GetAudioData((unsigned char *)pData, (int)lSize, &Filled);
	else
		nRC = ((COMXALPlayer *)pPlayer)->GetAudioData((unsigned char *)pData, (int)lSize, &Filled);

	env->ReleaseCharArrayElements(data, pData, 0);

	if (nRC == VOOSMP_ERR_FormatChange)
	{
		VOOSMP_AUDIO_FORMAT	fmtAudio;
		memset (&fmtAudio, 0, sizeof (VOOSMP_AUDIO_FORMAT));
		pPlayer->GetParam(VOOSMP_PID_AUDIO_FORMAT, &fmtAudio);

			// added by gtxia for get the audio decoder output type
		int nType = 0;
		//VOLOGE("EAC3  VOOSMP_ERR_FormatChange before call VOOSMP_PID_AUDIO_RENDER_DATA_FORMAT)\n");
		pPlayer->GetParam(VOOSMP_PID_AUDIO_RENDER_DATA_FORMAT, &nType);
		
		updateAudioFormat (env, obj, fmtAudio.SampleRate , fmtAudio.Channels, fmtAudio.SampleBits, nType);
		return 0;
	}

	if (nRC == 0)
		return Filled;

	return 0;
}

static jobject onstream_player_nativeGetSubTitleSample (JNIEnv * env, jobject obj , jint context, jint nTimeStamp)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		return 0;
	}

	voSubtitleInfo sample;
	memset(&sample,0,sizeof(voSubtitleInfo));
	sample.nTimeStamp = nTimeStamp;
	//	VOLOGI ("CloseCaption GetSubtileSample 000!");
	int nRC = pPlayer->GetSubtileSample(&sample);
	if(nRC == VOOSMP_ERR_None)
	{
	//	VOLOGI ("CloseCaption GetSubtileSample 222111!");
		CJavaParcelWrap	javaParcel(env);
	//	VOLOGI ("CloseCaption GetSubtileSample 555!");
		CJavaParcelWrap::getParcelFromSubtitleInfo(&sample, &javaParcel);
	//	VOLOGI ("CloseCaption GetSubtileSample 777!");
		return javaParcel.getParcel();
	}
	//	VOLOGI ("CloseCaption GetSubtileSample 6666!");

	return 0; //pSource->Flush ();
}

static jlong onstream_player_nativeSetView (JNIEnv* env, jobject obj,jint context)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}
	
	int lRendType = 0;

	pPlayer->GetParam(VOOSMP_PID_VIDEO_RENDER_TYPE, (void *)&lRendType);

	if(lRendType == VOOSMP_RENDER_TYPE_BITMAP)
	{
		pPlayer->SetView(obj);
	}
	else
		SetSurface(env,obj,pPlayer);
	VOLOGI("END");
	return 0;
}


static jlong onstream_player_nativeSetParam(JNIEnv * env, jobject obj, jint context, jlong id, jobject param)
{
	VOLOGI ("Param ID %d", (int)id);
		
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return VOOSMP_ERR_Pointer;
	}

	if (param == NULL)
	{
		return pPlayer->SetParam (id, param);
	}

	if (id >= 0x00100000 
		|| id == VOOSMP_PID_LICENSE_TEXT 
		|| id == VOOSMP_PID_LICENSE_FILE_PATH
		|| id == VOOSMP_PID_PLAYER_PATH
		|| id == VOOSMP_PID_MFW_CFG_FILE
		|| id == VOOSMP_PID_SOURCE_FILENAME
		|| id == VOOSMP_PID_LIB_NAME_PREFIX
		|| id == VOOSMP_PID_LIB_NAME_SUFFIX
		|| id == VOOSMP_PID_COMMAND_STRING
		|| id == VOOSMP_PID_DRM_UNIQUE_IDENTIFIER
		|| id == VOOSMP_PID_SUBTITLE_FILE_NAME)	//param is String
	{
		char * str = (char*)env->GetStringUTFChars ((jstring)param, NULL);
		int iRet = pPlayer->SetParam (id, str);
		env->ReleaseStringUTFChars ((jstring) param, (const char *) str);
		return iRet;
	}else if (id == VOOSMP_PID_DRAW_RECT )		//param is int array
	{
		VOOSMP_RECT rect;
		jint* arr;
		arr = env->GetIntArrayElements ((jintArray)param, NULL);
		rect.nLeft = arr[0];
		rect.nTop = arr[1];
		rect.nRight = arr[2];
		rect.nBottom = arr[3];
		int iRet = pPlayer->SetParam (id, &rect);
		VOLOGI("VOOSMP_RECT, Left = %d, Top = %d, Right = %d, Buttom = %d", arr[0], arr[1], arr[2], arr[3]);
		env->ReleaseIntArrayElements ((jintArray)param, arr, 0);
		return iRet;
	}else if (id == VOOSMP_PID_VIDEO_PERFORMANCE_OPTION )		//param is int array
	{
		VOOSMP_PERFORMANCE_DATA perf;
		jint* arr;
		arr = env->GetIntArrayElements ((jintArray)param, NULL);
		perf.nLastTime = arr[0];
		perf.nSourceDropNum = arr[1];
		perf.nCodecDropNum = arr[2];
		perf.nRenderDropNum = arr[3];
		perf.nDecodedNum = arr[4];
		perf.nRenderNum = arr[5];
		perf.nSourceTimeNum = arr[6];
		perf.nCodecTimeNum = arr[7];
		perf.nRenderTimeNum = arr[8];
		perf.nJitterNum = arr[9];
		perf.nCodecErrorsNum = arr[10];
		perf.nCodecErrors = (int *)arr[11];
		perf.nCPULoad = arr[12];
		perf.nFrequency = arr[13];
		perf.nMaxFrequency = arr[14];
		perf.nWorstDecodeTime = arr[15];
		perf.nWorstRenderTime = arr[16];
		perf.nAverageDecodeTime = arr[17];
		perf.nAverageRenderTime = arr[18];
		perf.nTotalCPULoad = arr[19];
		int iRet = pPlayer->SetParam (id, &perf);
		VOLOGI("VOOSMP_PERFORMANCE_DATA, nLastTime = %d, nSourceTimeNum = %d, nCodecTimeNum = %d, nRenderTimeNum = %d, nJitterNum = %d, nCPULoad = %d",
				arr[0], arr[6], arr[7], arr[8], arr[9], arr[12]);
		env->ReleaseIntArrayElements ((jintArray)param, arr, 0);
		return iRet;
	}else if (id == VOOSMP_PID_PERFORMANCE_DATA)		//set the param on Source
	{
		return VOOSMP_ERR_Implement;
	}else if (id == VOOSMP_PID_LICENSE_CONTENT)	
	{
		jbyte* txt = env->GetByteArrayElements ((jbyteArray)param, NULL);
		
		int iRet = pPlayer->SetParam(id, (void *)txt);
		env->ReleaseByteArrayElements ((jbyteArray)param, txt, 0);
		return iRet;
			
	}else if (id == VOOSMP_PID_VIEW_ACTIVE
			|| id == VONP_PID_VIEW_ACTIVE
			|| id == VOOSMP_PID_APPLICATION_RESUME
			|| id == VOOSMP_PID_BITMAP_HANDLE
			|| id == VOOSMP_PID_IOMX_PROBE)
	{
		VOLOGI(" VOOSMP_PID_BITMAP_HANDLE is %d. %p ", id );
		VOLOGI ("Check IOMX result is %p ",param);
		long iRet = pPlayer->SetParam(id, (void *)param);
		VOLOGI ("Check IOMX result is %d ",(int)iRet);
		return iRet;
		
			
	}else if(id == VOOSMP_PID_AUDIO_PLAYBACK_SPEED){
			VOLOGI(" VOOSMP_PID_AUDIO_PLAYBACK_SPEED  ");
			float fParam = getFloatValue (env, param);
			return pPlayer->SetParam(id, (void *)&fParam);
	
	}else if(id == VOOSMP_PID_AUDIO_DSP_CLOCK){
		long lParam = getIntegerValue (env, param);
		return pPlayer->SetParam(id, (void*)lParam);
	}else if (id == VOOSMP_PID_AUDIO_RENDER_FORMAT)
    {
        jint* arr;
		arr = env->GetIntArrayElements ((jintArray)param, NULL);
        
        VOOSMP_AUDIO_RENDER_FORMAT audioRendeFormat;
        audioRendeFormat.SampleRate = arr[0];
        audioRendeFormat.Channels = arr[1];
        audioRendeFormat.SampleBits = arr[2];
        audioRendeFormat.BufferSize = arr[3];
        VOLOGI("VOOSMP_PID_AUDIO_RENDER_FORMAT, SampleRate is %d, Channels is %d, SampleBits is %d, BufferSize is %d", audioRendeFormat.SampleRate, audioRendeFormat.Channels, audioRendeFormat.SampleBits, audioRendeFormat.BufferSize);
        
        int iRet = pPlayer->SetParam(id, (void *)&audioRendeFormat);
        env->ReleaseIntArrayElements ((jintArray)param, arr, 0);
        return iRet;
    }
    else	//param is int
	{
		long lParam = getIntegerValue (env, param);
		return pPlayer->SetParam(id, (void *)&lParam);
	}
	
}

static jobject onstream_player_nativeGetSEITimingInfo(JNIEnv * env, jobject obj, jint context, jint subid, jlong timeStamp)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return NULL;
	}
	if(subid == VOOSMP_FLAG_SEI_PIC_TIMING)
	{
		VOOSMP_SEI_INFO sample;
		sample.nFlag = subid;
		sample.llTime = timeStamp;
		sample.pInfo = NULL;
		if(pPlayer->GetSEISample (&sample) == 0)
		{
			if(sample.pInfo!=NULL)
			{
				VOOSMP_SEI_PIC_TIMING* pInfo = (VOOSMP_SEI_PIC_TIMING*)sample.pInfo;
				int size = sizeof(VOOSMP_SEI_PIC_TIMING)/sizeof(int);
				jintArray arr = env->NewIntArray(size);
				env->SetIntArrayRegion(arr, 0, size, (const jint*)pInfo);
				
				VOLOGI("nativeGetSEITimingInfo int array size = %d",size);
				return arr;
			}
		}
		else
		{
			VOLOGI ("pPlayer->GetSEISample return error!");
		}
		
	}
	
	return NULL;
}

static jobject onstream_player_nativeGetParam(JNIEnv * env, jobject obj, jint context, jlong id)
{
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return 0;
	}
	VOLOGI(" Get Param %d  ",(int)id);

	if (id == VOOSMP_PID_CPU_INFO 
		|| id == VOOSMP_PID_VIDEO_DEC_BITRATE
		|| id == VOOSMP_PID_AUDIO_DEC_BITRATE 
		|| id == VOOSMP_PID_AUDIO_FORMAT 
		|| id == VOOSMP_PID_VIDEO_FORMAT
		)
	{
		int nInt[30];
		memset (nInt, 0, 30*sizeof(int));
		int nRC = pPlayer->GetParam(id, nInt);
		if(nRC != VOOSMP_ERR_None)
		{
			VOLOGE("GetParam %d error!", (int)id);
			return NULL;

		}

		jintArray intArray = env->NewIntArray (30);
		env->SetIntArrayRegion (intArray, 0, 30, nInt);
		return intArray;
	}else if (id == VOOSMP_PID_COMMAND_STRING)
	{
		char sChar[2048];
		memset(sChar, 0, 2048);
		int nRC = pPlayer->GetParam(id, sChar);
		if (nRC == 0)
		{
			jstring str = env->NewStringUTF ((const char *) sChar);
			return str;
		}else
		{
			VOLOGE("GetParam error! id is %d, error code is %d", (int)id, nRC);
			return NULL;
		}
			
	}else if (id == VOOSMP_PID_VIDEO_PERFORMANCE_OPTION)
	{
		VOOSMP_PERFORMANCE_DATA perf;
		memset (&perf, 0, sizeof(perf));
		int nRC = pPlayer->GetParam(id, &perf);

		if (nRC ==0)
		{
			CJavaParcelWrap	javaParcel(env);
			CJavaParcelWrap::getParcelFromPerformanceData(&perf, &javaParcel);
			return javaParcel.getParcel();
		}else
		{
			VOLOGE("GetParam error! VOOSMP_PID_VIDEO_PERFORMANCE_OPTION id is %d, error code is %d", (int)id, nRC);
			return NULL;
		}
		
	}else if(id == VOOSMP_PID_AUDIO_PLAYBACK_SPEED){
			VOLOGI(" VOOSMP_PID_AUDIO_PLAYBACK_SPEED  ");
			float fParam = 0;
			int nRC = pPlayer->GetParam(id, (void *)&fParam);
			if(nRC != VOOSMP_ERR_None)
				return 0;
			jclass clazz = env->FindClass("java/lang/Float");
			jmethodID methodID = env->GetMethodID (clazz, "<init>" , "(F)V");
			jobject jobj = env->NewObject (clazz, methodID, fParam);
			env->DeleteLocalRef(clazz);
			
			return jobj;
	}else if((id & 0x0000FFFF) == VOOSMP_PID_MODULE_VERSION){
			VOOSMP_MODULE_VERSION ver ;
			memset (&ver, 0, sizeof(ver));
			ver.nModuleType = ((id ^ VOOSMP_PID_MODULE_VERSION) >> 16 );
			VOLOGI(" VOOSMP_PID_MODULE_VERSION start,  nModuleType is %d , module name is %s",ver.nModuleType, ver.pszVersion);

			int nRC = pPlayer->GetParam(VOOSMP_PID_MODULE_VERSION, &ver);
            VOLOGI(" VOOSMP_PID_MODULE_VERSION ,  nModuleType is %d , module name is %s",ver.nModuleType, ver.pszVersion);
			if (nRC == VOOSMP_ERR_None )
			{
				CJavaParcelWrapOSMP parc(env);
				parc.fillParcelData(&ver);
			
				jobject jobj = NULL;
				jobj = parc.getParcel();
				return jobj;
			}
			return NULL;	
			
	}
	VOLOGI(" GetParam return Integer");

	int nParam = 0;
	int nRC = pPlayer->GetParam(id, (void *)&nParam);
	if(nRC != VOOSMP_ERR_None)
		return 0;

	jclass clazz = env->FindClass("java/lang/Integer");
	jmethodID methodID = env->GetMethodID (clazz, "<init>" , "(I)V");
	jobject jobj = env->NewObject (clazz, methodID, nParam);
	env->DeleteLocalRef (clazz);
	return jobj;
	
}


/*
 * Class:     com_visualon_OSMPEngine_voOnStreamSDK
 * Method:    nativeGetSubtitleLanguageCount
 * Signature: (I)I
 */
static jint onstream_player_nativeGetSubtitleLanguageCount(JNIEnv *env, jobject obj, jint context)
{
	VOLOGI("onstream_player_nativeGetSubtitleLanguageCount");
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return 0;
	}

	int iNum = 0;
	pPlayer->GetSubLangNum (&iNum);
	
	return iNum;
}


/*
 * Class:     com_visualon_OSMPEngine_voOnStreamSDK
 * Method:    nativeGetSubtitleLanguageInfo
 * Signature: (II)Ljava/lang/Object;
 */
static jobject onstream_player_nativeGetSubtitleLanguageInfo (JNIEnv *env, jobject obj, jint context, jint nIndex)
{
	VOLOGI("onstream_player_nativeGetSubtitleLanguageInfo");
	
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return 0;
	}


	VOOSMP_SUBTITLE_LANGUAGE* pvsl = NULL; 
	pPlayer->GetSubLangItem (nIndex, &pvsl);
	
//	pvsl = new VOOSMP_SUBTITLE_LANGUAGE();
//	strcpy((char*)pvsl->szLangName, (const char*)"EngineFrench");
//	pvsl->nLanguageType = 2;

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
	env->DeleteLocalRef (jstr);
	env->DeleteLocalRef(cls);
	return object;
}



/*
 * Class:     com_visualon_OSMPEngine_voOnStreamSDK
 * Method:    nativeSelectSubtitelLanguage
 * Signature: (II)I
 */
static jint onstream_player_nativeSelectSubtitleLanguage(JNIEnv *env, jobject obj, jint context, jint nIndex)
{
	VOLOGI("Index is %d", nIndex);
	
	IOSBasePlayer* pPlayer = (IOSBasePlayer*) context;
	if (pPlayer == NULL)
	{
		VOLOGE ("The player context pointer is NULL!");
		return 0;
	}

	return pPlayer->SelectLanguage (nIndex);

}



// JNI mapping between Java methods and native methods
static JNINativeMethod nativeMethods[] = {
	{"nativeCreate", "(Ljava/lang/Object;Ljava/lang/String;JJJ)J", (void *)onstream_player_nativeCreate},
	{"nativeDestroy", "(I)J", (void *)onstream_player_nativeDestroy},
	{"nativeOpen", "(ILjava/lang/Object;J)J", (void *)onstream_player_nativeOpen},
	{"nativeRun", "(I)J", (void *)onstream_player_nativeRun},
	{"nativePause", "(I)J", (void*)onstream_player_nativePause},
	{"nativeStop", "(I)J", (void *)onstream_player_nativeStop},
	{"nativeClose", "(I)J", (void *)onstream_player_nativeClose},
	{"nativeSetPos", "(IJ)J", (void *)onstream_player_nativeSetPos},
	{"nativeGetPos", "(I)J", (void *)onstream_player_nativeGetPos},
	{"nativeGetVideoData", "(I[B)J", (void *)onstream_player_nativeGetVideoData},
	{"nativeGetAudioData", "(I[B)J", (void *)onstream_player_nativeGetAudioData},
	{"nativeGetSubTitleSample", "(II)Ljava/lang/Object;", (void *)onstream_player_nativeGetSubTitleSample},
	{"nativeSetParam", "(IJLjava/lang/Object;)J", (void *)onstream_player_nativeSetParam},
	{"nativeGetSEITimingInfo", "(IIJ)Ljava/lang/Object;", (void *)onstream_player_nativeGetSEITimingInfo},
	{"nativeGetParam", "(IJ)Ljava/lang/Object;", (void *)onstream_player_nativeGetParam},
	{"nativeSetSurface", "(I)J", (void *)onstream_player_nativeSetView},
	{"nativeGetSubtitleLanguageCount", "(I)I", (void *)onstream_player_nativeGetSubtitleLanguageCount},
	{"nativeGetSubtitleLanguageInfo", "(II)Ljava/lang/Object;", (void *)onstream_player_nativeGetSubtitleLanguageInfo},
	{"nativeSelectSubtitleLanguage", "(II)I", (void *)onstream_player_nativeSelectSubtitleLanguage},
	};

// This function only registers the native methods
static int register_voStreamJni (JNIEnv *env)
{
	//VOLOGI ();

	jclass clazz;
	clazz = env->FindClass(g_szOnStreamPlayerName);
	if (clazz == NULL)
	{
		VOLOGI ("%s not found!", g_szOnStreamPlayerName);
		return JNI_FALSE;
	}else
	{
		VOLOGI ("FindClass success!");
	}

	if (env->RegisterNatives(clazz, nativeMethods, sizeof(nativeMethods) / sizeof(nativeMethods[0])) < 0)
	{
		VOLOGI ("RegisterNatives failed: '%s' ", g_szOnStreamPlayerName);
		env->DeleteLocalRef(clazz);
		return JNI_FALSE;
	}

	env->DeleteLocalRef(clazz);

	VOLOGI ("Succeed to register %s.",  g_szOnStreamPlayerName);

	return JNI_TRUE;
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

	strcpy (g_szOnStreamPlayerName, "com/visualon/OSMPEngine/voOnStreamSDK");
	strcpy (g_szOnStreamVideoRndName, "com/visualon/OSMPEngine/voVideoRender");
	strcpy (g_szOnStreamAudioRndName, "com/visualon/OSMPEngine/voAudioRender");
	strcpy (g_szSubtitleLanguage, "com/visualon/OSMPUtils/voOSSubtitleLanguageImpl");
	

	if (register_voStreamJni(env) < 0) 
	{
		VOLOGE ("ERROR: register_voStreamJni native registration failed.");
		return result;
	}

	// success -- return valid version number
	result = jniver;
	return result;
}

#if defined __cplusplus
}
#endif


