#define LOG_TAG "vomejni"
//#define THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS

#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
#include <utils/Parcel.h>
#elif defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__ 
#include <binder/Parcel.h>
#endif

#if defined LINUX
#include <dlfcn.h>
//#include "voOSFunc.h"
#endif
#include <utils/Log.h>
#include <media/MediaPlayerInterface.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <utils/threads.h>
#include <jni.h>
#include <nativehelper/JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include <utils/Errors.h> 
using namespace android;

#include "voCMutex.h"
#include "vomeplayer.h"
#include "vomeAPPath.h"
#include "COMXMobiTVDataSourcePlayer.h"
#include "voLog.h"

char vo_application_location[256];
char vo_application_lib_location[256];
TMKPlayerFunc gMKPlayerFunc={0};

#if 1
#define VOINFO(format, ...)
#else
#define VOINFO(format, ...) { \
	LOGI("[david] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }
#endif

using namespace android;


// ----------------------------------------------------------------------------
static void _MK_Log_AndroidMsg(void* aObj, MK_U32 aTm, MK_U32 aLev, const MK_Char* aLn[], MK_U32 aLnN[], MK_U32 aN)
{
  MK_U32 i = 0, p = ANDROID_LOG_UNKNOWN;
  switch (aLev)
    {
    case MK_LOG_L_TRAC: p = ANDROID_LOG_VERBOSE; break;
    case MK_LOG_L_DBG: p = ANDROID_LOG_DEBUG; break;
    case MK_LOG_L_INFO: p = ANDROID_LOG_INFO; break;
    case MK_LOG_L_WARN: p = ANDROID_LOG_WARN; break;
    case MK_LOG_L_ERR: p = ANDROID_LOG_ERROR; break;
    }
  for (; i < aN; ++i)
    {
      __android_log_write(p, "MediaKit", aLn[i]);
      /* alt: __android_log_print(lLev, "MediaKit", "%s", aLn[i]); */
    }
}

// ----------------------------------------------------------------------------
static void* LoadMKPlayerLib(void)
{
	int   pathIndex = 0;
	char  tmpPath[256];
	char* libName = "libMKPlayer.so";
	void* lMKPlayerLib;

	strcpy(tmpPath, vo_application_lib_location);
	strcat(tmpPath, libName);
	lMKPlayerLib = dlopen(tmpPath, RTLD_NOW);

	if (lMKPlayerLib == NULL)
	{
		VOLOGE("@@@@@@ Load Module %s Error %s \n", tmpPath, dlerror());
		strcpy(tmpPath,"/system/lib/");
		strcat(tmpPath,libName);
		lMKPlayerLib = dlopen(tmpPath, RTLD_NOW);
		pathIndex=1;
	}
	if (lMKPlayerLib == NULL)
	{
		VOLOGE("@@@@@@ Load Module %s Error %s \n", tmpPath, dlerror());
		return NULL;
	}
	gMKPlayerFunc.Instance = lMKPlayerLib;
	return lMKPlayerLib;
}



static void  FreeMKPlayerLib(void)
{
	dlclose(gMKPlayerFunc.Instance);
}

static int DestroyMKSource()
{
	VOLOGI ("<<<JNI>>> enter");
	VOLOGI ("<<<JNI>>> gMKPlayerFunc.lPlayer = 0x%x", gMKPlayerFunc.lPlayer);
	if (gMKPlayerFunc.lPlayer)
	{
		VOLOGI("VO_MK_Player_Destroy");
		gMKPlayerFunc.VO_MK_Player_Close(gMKPlayerFunc.lPlayer,1);
		VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_Close");
		gMKPlayerFunc.VO_MK_Player_Destroy(gMKPlayerFunc.lPlayer);
		VOLOGI ("<<<MK_API>>> called gMKPlayerFunc.VO_MK_Player_Destroy");
		FreeMKPlayerLib();
	}
	memset(&gMKPlayerFunc,0,sizeof(TMKPlayerFunc));
	
	VOLOGI ("<<<JNI>>> leave");
	return 0;
}

static int CreateMKSource()
{
	VOLOGI ("<<<JNI>>> enter1");
	DestroyMKSource();
	if(LoadMKPlayerLib()==NULL)
	{
		VOLOGE("invalid instnace");
		return -1;
	}
	//load function
#define FUNC_PPOINT_ASSIGN2(a)\
	gMKPlayerFunc.VO_##a = (a##_FP)dlsym(gMKPlayerFunc.Instance, #a);\
	if (gMKPlayerFunc.VO_##a==NULL)\
	{\
		VOLOGI("failed to get %s",#a);\
		gMKPlayerFunc.functionMiss++;\
	}
		FUNC_PPOINT_ASSIGN2(MK_Player_Create) //VO_MK_Player_Create = dlsym(m_hModMobiTV, "MK_Player_Create");
		FUNC_PPOINT_ASSIGN2(MK_Log_ToFunc)
		FUNC_PPOINT_ASSIGN2(MK_Log_Discard)
		FUNC_PPOINT_ASSIGN2(MK_Player_Destroy)
		FUNC_PPOINT_ASSIGN2(MK_Sample_Destroy)
		FUNC_PPOINT_ASSIGN2(MK_Player_Open)
		FUNC_PPOINT_ASSIGN2(MK_Player_HTTP_SetUseBA)
		FUNC_PPOINT_ASSIGN2(MK_Player_Close)
		FUNC_PPOINT_ASSIGN2(MK_Player_SetBufferFunc)
		FUNC_PPOINT_ASSIGN2(MK_Player_SetErrorFunc)
		FUNC_PPOINT_ASSIGN2(MK_Player_SetEventFunc)
		FUNC_PPOINT_ASSIGN2(MK_Player_SetStateFunc)
		FUNC_PPOINT_ASSIGN2(MK_Player_GetTrackCount)
		FUNC_PPOINT_ASSIGN2(MK_Player_GetTrackInfo)
		FUNC_PPOINT_ASSIGN2(MK_Player_EnableTrack)
		FUNC_PPOINT_ASSIGN2(MK_Player_GetTrackSample)
		FUNC_PPOINT_ASSIGN2(MK_Player_Play)
		FUNC_PPOINT_ASSIGN2(MK_Player_Seek)
		FUNC_PPOINT_ASSIGN2(MK_Player_GetState)
		FUNC_PPOINT_ASSIGN2(MK_Player_GetStartTime)
		FUNC_PPOINT_ASSIGN2(MK_Player_Stop)
		FUNC_PPOINT_ASSIGN2(MK_Player_Wait)
		FUNC_PPOINT_ASSIGN2(MK_TrackInfo_GetH264ConfSPS)
		FUNC_PPOINT_ASSIGN2(MK_TrackInfo_GetH264ConfPPS)
		FUNC_PPOINT_ASSIGN2(MK_Log_ToFile)
		FUNC_PPOINT_ASSIGN2(MK_MobiDrmContentIsEncrypted)
		FUNC_PPOINT_ASSIGN2(MK_MobiDrmGetContentInfo)
		FUNC_PPOINT_ASSIGN2(MK_MobiDrmGetLicenseInfo)
		FUNC_PPOINT_ASSIGN2(MK_Player_Net_SetTimeout)
		FUNC_PPOINT_ASSIGN2(MK_Player_MobiDRM_SetKey)
		FUNC_PPOINT_ASSIGN2(MK_Player_MobiDRM_SetDeviceIDs)
		FUNC_PPOINT_ASSIGN2(MK_Player_MobiDRM_SetUserID)
		FUNC_PPOINT_ASSIGN2(MK_Player_MobiDRM_SetLicense)
		FUNC_PPOINT_ASSIGN2(MK_Player_HTTP_SetProxy)
		FUNC_PPOINT_ASSIGN2(MK_Player_HTTP_SetUserAgentVals)
		FUNC_PPOINT_ASSIGN2(MK_Player_HTTP_SetUseKeepAlive)
		FUNC_PPOINT_ASSIGN2(MK_Player_HTTP_SetBALimits)
		FUNC_PPOINT_ASSIGN2(MK_Player_Media_SetBufferLimits)
		FUNC_PPOINT_ASSIGN2(MK_Player_SetOption)
	        FUNC_PPOINT_ASSIGN2(MK_Player_SetOptions)
	        FUNC_PPOINT_ASSIGN2(MK_Player_SetOptionsF)

		if (gMKPlayerFunc.functionMiss)
		{
			VOLOGE("function miss");
			return -1;
		}

	char s[256];
	strcpy(s, vo_application_location); strcat(s, "mkplayer.log");
	//gMKPlayerFunc.VO_MK_Log_ToFile(s , /*MK_LOG_L_DBG*/MK_LOG_L_TRAC);
	//gMKPlayerFunc.VO_MK_Log_ToFile("/sdcard/mkplayer.log" , /*MK_LOG_L_DBG*/MK_LOG_L_TRAC);
#ifdef _VOLOG_INFO
	gMKPlayerFunc.VO_MK_Log_ToFunc(_MK_Log_AndroidMsg, NULL, NULL, MK_LOG_L_TRAC);
#else
	gMKPlayerFunc.VO_MK_Log_Discard();
#endif
	gMKPlayerFunc.lPlayer = gMKPlayerFunc.VO_MK_Player_Create();
	VOLOGI ("<<<MK_API>>> called VO_MK_Player_Create");
	VOLOGI ("<<<JNI>>> gMKPlayerFunc.lPlayer = 0x%x", gMKPlayerFunc.lPlayer);
	VOLOGI("gMKPlayerFunc.lPlayer %p", gMKPlayerFunc.lPlayer);

	if (gMKPlayerFunc.lPlayer)
	{
		VOLOGI("VO_MK_Player_Create");
		VOLOGI ("<<<JNI>>> leave");
		return 0;
	}
	else
	{
		VOLOGE("VO_MK_Player_Create fails");
		VOLOGI ("<<<JNI>>> leave");
		return -1;
	}

}

struct fields_t {
    jfieldID    context;
    jfieldID    surface;
    jfieldID    surface_native;
    jmethodID   post_event;
};

static fields_t fields;
static voCMutex g_mautolock;

// ref-counted object for callbacks
class JNIvomeplayerListener: public vomeplayerListener
{
public:
    JNIvomeplayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIvomeplayerListener();
    void notify(int msg, int ext1, int ext2);
private:
    JNIvomeplayerListener();
    jclass      mClass;     // Reference to vomeplayer class
    jobject     mObject;    // Weak ref to vomeplayer Java object to call on
};

JNIvomeplayerListener::JNIvomeplayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{
  VOINFO("%s", "JNIvomeplayerListener()");      
    // Hold onto the vomeplayer class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        //LOGE("Can't find com/mobitv/vomeplayer");
        LOGE("Can't find com/visualon/vomeplayer");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the vomeplayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);
}

JNIvomeplayerListener::~JNIvomeplayerListener()
{
  VOINFO("%s", "~JNIvomeplayerListener()");      
    // remove global references
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}

void JNIvomeplayerListener::notify(int msg, int ext1, int ext2)
{
  VOINFO("begin %d %08x %08x", msg, ext1, ext2);      
  JNIEnv *env = AndroidRuntime::getJNIEnv();
  env->CallStaticVoidMethod(mClass, fields.post_event, mObject, msg, ext1, ext2, 0);
  VOINFO("%s", "done");      
}

// ----------------------------------------------------------------------------

#if defined __cplusplus 
extern "C" {
#endif

static Surface* get_surface(JNIEnv* env, jobject clazz)
{
    return (Surface*)env->GetIntField(clazz, fields.surface_native);
}

static sp<vomeplayer> getvomeplayer(JNIEnv* env, jobject thiz)
{
    voCAutoLock l(&g_mautolock);
    vomeplayer* const p = (vomeplayer*)env->GetIntField(thiz, fields.context);
    return sp<vomeplayer>(p);
}

static sp<vomeplayer> setvomeplayer(JNIEnv* env, jobject thiz, const sp<vomeplayer>& player)
{
    voCAutoLock l(&g_mautolock);
    sp<vomeplayer> old = (vomeplayer*)env->GetIntField(thiz, fields.context);
    if (player.get()) {
        player->incStrong(thiz);
    }
    if (old != 0) {
        old->decStrong(thiz);
    }
    env->SetIntField(thiz, fields.context, (int)player.get());
    return old;
}

// If exception is NULL and opStatus is not OK, this method sends an error
// event to the client application; otherwise, if exception is not NULL and
// opStatus is not OK, this method throws the given exception to the client
// application.
static void process_media_player_call(JNIEnv *env, jobject thiz, status_t opStatus, const char* exception, const char *message)
{
    if (exception == NULL) {  // Don't throw exception. Instead, send an event.
        if (opStatus != (status_t) OK) {
            sp<vomeplayer> mp = getvomeplayer(env, thiz);
            if (mp != 0) mp->notify(VOME_ERROR, opStatus, 0);
        }
    } else {  // Throw exception!
        if ( opStatus == (status_t) INVALID_OPERATION ) {
            jniThrowException(env, "java/lang/IllegalStateException", NULL);
        } else if ( opStatus != (status_t) OK ) {
            if (strlen(message) > 230) {
               // if the message is too long, don't bother displaying the status code
               jniThrowException( env, exception, message);
            } else {
               char msg[256];
                // append the status code to the message
               sprintf(msg, "%s: status=0x%X", message, opStatus);
               jniThrowException( env, exception, msg);
            }
        }
    }
}

static void
vomp_setDataSource(JNIEnv *env, jobject thiz, jstring path)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    if (path == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }

    const char *pathStr = env->GetStringUTFChars(path, NULL);
    //const char *pathStr = "file:///data/local/spiderman.3gp";
    if (pathStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }
	
    VOINFO("pathStr: %s", pathStr);
    status_t opStatus = mp->setDataSource(pathStr);
    VOINFO("opStatus: %d", opStatus);
	
    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(path, pathStr);
    process_media_player_call( env, thiz, opStatus, "java/io/IOException", "setDataSource failed." );
    VOINFO("%s", "<<<JNI>>> leave");
}

static void
vomp_setDataSourceFD(JNIEnv *env, jobject thiz, jobject fileDescriptor, jlong offset, jlong length)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    if (fileDescriptor == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    int fd = getParcelFileDescriptorFD(env, fileDescriptor);
    process_media_player_call( env, thiz, mp->setDataSource(fd, offset, length), "java/io/IOException", "setDataSourceFD failed." );
    VOINFO("%s", "<<<JNI>>> leave");
}

static void setVideoSurface(const sp<vomeplayer>& mp, JNIEnv *env, jobject thiz)
{
    jobject surface = env->GetObjectField(thiz, fields.surface);
    if (surface != NULL) {
        const sp<Surface> native_surface = get_surface(env, surface);
        LOGV("prepare: surface=%p (id=%d)",
             native_surface.get(), native_surface->ID());
        mp->setVideoSurface(native_surface);
    }
}

static void
vomp_setVideoSurface(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    setVideoSurface(mp, env, thiz);
    VOINFO("%s", "<<<JNI>>> leave");
}

static void
vomp_prepare(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    setVideoSurface(mp, env, thiz);
    process_media_player_call( env, thiz, mp->prepare(), "java/io/IOException", "Prepare failed." );
    VOINFO("%s", "<<<JNI>>> leave");
}

static void
vomp_prepareAsync(JNIEnv *env, jobject thiz)
{
  //VOINFO("%s", "<<<JNI>>> enter");
  //vomp_prepare(env, thiz);
  //VOINFO("%s", "<<<JNI>>> leave");
  VOINFO("%s", "<<<JNI>>> enter");
  sp<vomeplayer> mp = getvomeplayer(env, thiz);
  if (mp == NULL ) {
    jniThrowException(env, "java/lang/IllegalStateException", NULL);
    return;
  }
  setVideoSurface(mp, env, thiz);
  process_media_player_call( env, thiz, mp->prepareAsync(), "java/io/IOException", "Prepare Async failed." );
  VOINFO("%s", "<<<JNI>>> leave");
}

static void
vomp_start(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
	VOINFO("%s", "start...");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
		mp->start();
    //process_media_player_call(env, thiz, mp->start(), NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
}

static void
vomp_stop(JNIEnv *env, jobject thiz)
{
    VOLOGI("stop...");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->stop(), NULL, NULL );
    VOLOGI("<<<JNI>>> leave");
}

static void
vomp_pause(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    VOINFO("%s", "pause");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
		mp->pause();
    //process_media_player_call( env, thiz, mp->pause(), NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
}

static jboolean
vomp_isPlaying(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    const jboolean is_playing = mp->isPlaying();

    LOGV("isPlaying: %d", is_playing);
    bool lRes = is_playing;
    VOINFO("%s", "<<<JNI>>> leave");
    return lRes;
//    return is_playing;
}

static void
vomp_seekTo(JNIEnv *env, jobject thiz, int msec)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    LOGV("seekTo: %d(msec)", msec);
    process_media_player_call( env, thiz, mp->seekTo(msec), NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
}

static int
vomp_getVideoWidth(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int w;
    if (0 != mp->getVideoWidth(&w)) {
        LOGE("getVideoWidth failed");
        w = 0;
    }
    LOGV("getVideoWidth: %d", w);
    VOINFO("%s", "<<<JNI>>> leave");
    return w;
}

static int
vomp_getVideoHeight(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int h;
    if (0 != mp->getVideoHeight(&h)) {
        LOGE("getVideoHeight failed");
        h = 0;
    }
    LOGV("getVideoHeight: %d", h);
    VOINFO("%s", "<<<JNI>>> leave");
    return h;
}


static int
vomp_getCurrentPosition(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int msec = 0;
    process_media_player_call( env, thiz, mp->getCurrentPosition(&msec), NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
    return msec;
}

static int
vomp_getDuration(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int msec = -1;
    process_media_player_call( env, thiz, mp->getDuration(&msec), NULL, NULL );
    LOGV("getDuration: %d (msec)", msec);
    VOINFO("%s", "<<<JNI>>> leave");
    return msec;
}

static void
vomp_reset(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    LOGV("reset");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->reset(), NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
}

static void
vomp_setAudioStreamType(JNIEnv *env, jobject thiz, int streamtype)
{
    VOINFO("%s", "<<<JNI>>> enter");
    LOGV("setAudioStreamType: %d", streamtype);
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setAudioStreamType(streamtype) , NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
}

static void
vomp_setLooping(JNIEnv *env, jobject thiz, jboolean looping)
{
    VOINFO("%s", "<<<JNI>>> enter");
    LOGV("setLooping: %d", looping);
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setLooping(looping), NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
}

static jboolean
vomp_isLooping(JNIEnv *env, jobject thiz)
{
    VOINFO("%s", "<<<JNI>>> enter");
    LOGV("isLooping");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    bool lRes = mp->isLooping();
    VOINFO("%s", "<<<JNI>>> leave");
    return lRes;
//    return mp->isLooping();
}

static void
vomp_setVolume(JNIEnv *env, jobject thiz, float leftVolume, float rightVolume)
{
    VOINFO("%s", "<<<JNI>>> enter");
    LOGV("setVolume: left %f  right %f", leftVolume, rightVolume);
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setVolume(leftVolume, rightVolume), NULL, NULL );
    VOINFO("%s", "<<<JNI>>> leave");
}

// FIXME: deprecated
static jobject
vomp_getFrameAt(JNIEnv *env, jobject thiz, jint msec)
{
    VOINFO("%s", "<<<JNI>>> enter");
    VOINFO("%s", "<<<JNI>>> leave");
    return NULL;
}


// Sends the request and reply parcels to the media player via the
// binder interface.
static jint
vomp_invoke(JNIEnv *env, jobject thiz,
                                 jobject java_request, jobject java_reply)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> media_player = getvomeplayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }


   // Parcel *request = parcelForJavaObject(env, java_request);
  //  Parcel *reply = parcelForJavaObject(env, java_reply);

    // Don't use process_media_player_call which use the async loop to
    // report errors, instead returns the status.
    VOINFO("%s", "<<<JNI>>> leave");
    return 0;//media_player->invoke(*request, reply);
}

// Sends the new filter to the client.
static jint
vomp_setMetadataFilter(JNIEnv *env, jobject thiz, jobject request)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> media_player = getvomeplayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }

    
    VOINFO("%s", "<<<JNI>>> leave");
    return 0;//media_player->setMetadataFilter(*filter);
}

static jboolean
vomp_getMetadata(JNIEnv *env, jobject thiz, jboolean update_only,
                                      jboolean apply_filter, jobject reply)
{
    VOINFO("%s", "<<<JNI>>> enter");
    sp<vomeplayer> media_player = getvomeplayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

  
    // On return metadata is positioned at the beginning of the
    // metadata. Note however that the parcel actually starts with the
    // return code so you should not rewind the parcel using
    // setDataPosition(0).
    VOINFO("%s", "<<<JNI>>> leave");
    return 0;//media_player->getMetadata(update_only, apply_filter, metadata) == OK;
}

// This function gets some field IDs, which in turn causes class initialization.
// It is called from a static block in vomeplayer, which won't run until the
// first time an instance of this class is used.
static const char* const g_szvompclspathname = VO_APPLICATION_CLASSPATH;

static void vomp_native_init(JNIEnv *env) {
    VOINFO("%s", "<<<JNI>>> enter");
    
    gMKPlayerFunc.lPlayer = 0;
    
    uid_t uid = getuid();
    uid_t euid = geteuid();
    gid_t gid = getgid();
    gid_t egid = getegid();
    VOINFO("uid: %d, gid: %d, euid: %d, egid: %d\n", uid, gid, euid, egid);
    
    jclass clazz;
    clazz = env->FindClass(g_szvompclspathname);
    if (clazz == NULL) {
      //jniThrowException(env, "java/lang/RuntimeException", "Can't find com/mobitv/mkplayer/vomeplayer");
      jniThrowException(env, "java/lang/RuntimeException", "Can't find com/visualon/vome/vomeplayer");
      return;
    }
    
    fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (fields.context == NULL) {
      jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.mNativeContext");
      return;
    }
    
    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
      jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.postEventFromNative");
      return;
    }
#ifdef GINGERBREAD
	fields.surface = env->GetFieldID(clazz, "mNativeSurface", "Landroid/view/Surface;");
#else//GINGERBREAD
    fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
#endif//GINGERBREAD
    if (fields.surface == NULL) {
      jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.mSurface");
      return;
    }
    
    jclass surface = env->FindClass("android/view/Surface");
    if (surface == NULL) {
      jniThrowException(env, "java/lang/RuntimeException", "Can't find android/view/Surface");
      return;
    }
    
    fields.surface_native = env->GetFieldID(surface, "mSurface", "I");
    if (fields.surface_native == NULL) {
      jniThrowException(env, "java/lang/RuntimeException", "Can't find Surface.mSurface");
      return;
    }
    VOINFO("%s", "<<<JNI>>> leave");
}


static void vomp_native_init_location(JNIEnv *env, jobject thiz, jstring location) 
{
  VOINFO("%s", "<<<JNI>>> enter");

  char scwd[256]; getcwd(scwd, sizeof(scwd));
  VOINFO("getcwd: %s", scwd);

  uid_t uid = getuid();
  uid_t euid = geteuid();
  gid_t gid = getgid();
  gid_t egid = getegid();
  VOINFO("uid: %d, gid: %d, euid: %d, egid: %d\n", uid, gid, euid, egid);
  
  const char *s = env->GetStringUTFChars(location, NULL);
  if (s == NULL) {  // Out of memory
    jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
    return;
  }
  strcpy(vo_application_location, s);
  //strcpy(vo_application_lib_location, s); strcat(vo_application_lib_location, "lib/");
  strcpy(vo_application_lib_location, s); strcat(vo_application_lib_location, "lib/");
  VOINFO("the location is %s",vo_application_lib_location);
  jclass clazz;
  clazz = env->FindClass(g_szvompclspathname);
  if (clazz == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find <class>");
    return;
  }
  
  fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
  if (fields.context == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.mNativeContext");
    return;
  }
  
  fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
					     "(Ljava/lang/Object;IIILjava/lang/Object;)V");
  if (fields.post_event == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.postEventFromNative");
    return;
  }
  
  fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
  if (fields.surface == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.mSurface");
    return;
  }
  
  jclass surface = env->FindClass("android/view/Surface");
  if (surface == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find android/view/Surface");
    return;
  }
  
  fields.surface_native = env->GetFieldID(surface, "mSurface", "I");
  if (fields.surface_native == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find Surface.mSurface");
    return;
  }
   
  VOINFO("%s", "<<<JNI>>> leave");
}


static void vomp_native_init_location_library(JNIEnv *env, jobject thiz, jstring location, jstring library)
{
  VOINFO("%s", "<<<JNI>>> enter");

  char scwd[256]; getcwd(scwd, sizeof(scwd));
  VOINFO("getcwd: %s", scwd);

  uid_t uid = getuid();
  uid_t euid = geteuid();
  gid_t gid = getgid();
  gid_t egid = getegid();
  VOINFO("uid: %d, gid: %d, euid: %d, egid: %d\n", uid, gid, euid, egid);

  {
    const char *s = env->GetStringUTFChars(location, NULL);
    if (s == NULL) {  // Out of memory
      jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
      return;
    }
    strcpy(vo_application_location, s);
  }

  {
    const char *s = env->GetStringUTFChars(library, NULL);
    if (s == NULL) {  // Out of memory
      jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
      return;
    }
    strcpy(vo_application_lib_location, s);
    VOINFO("the location is %s",vo_application_lib_location);
  }

  jclass clazz;
  clazz = env->FindClass(g_szvompclspathname);
  if (clazz == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find <class>");
    return;
  }
  
  fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
  if (fields.context == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.mNativeContext");
    return;
  }
  
  fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
					     "(Ljava/lang/Object;IIILjava/lang/Object;)V");
  if (fields.post_event == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.postEventFromNative");
    return;
  }
  
  fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
  if (fields.surface == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find vomeplayer.mSurface");
    return;
  }
  
  jclass surface = env->FindClass("android/view/Surface");
  if (surface == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find android/view/Surface");
    return;
  }
  
  fields.surface_native = env->GetFieldID(surface, "mSurface", "I");
  if (fields.surface_native == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Can't find Surface.mSurface");
    return;
  }
   
  VOINFO("%s", "<<<JNI>>> leave");
}


static void
vomp_native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
  VOINFO("%s", "<<<JNI>>> enter");
  gMKPlayerFunc.lPlayer=0;
  if (CreateMKSource()<0) {
	  jniThrowException(env, "java/lang/RuntimeException", "Can't CreateMKSource()");
	  return;
  }
  sp<vomeplayer> mp = new vomeplayer();
  if (mp == NULL) {
    jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
    return;
  }

  // create new listener and give it to vomeplayer
  sp<JNIvomeplayerListener> listener = new JNIvomeplayerListener(env, thiz, weak_this);
  mp->setListener(listener);
 // mp->CreateAudioRender();
  
  // Stow our new C++ vomeplayer in an opaque field in the Java object.
  setvomeplayer(env, thiz, mp);
  VOINFO("%s", "<<<JNI>>> leave");
}


static void
vomp_release(JNIEnv *env, jobject thiz)
{
  VOINFO("%s", "<<<JNI>>> enter");
  LOGV("release");

  if (gMKPlayerFunc.lPlayer) {
    MK_Result lRes;

    lRes = gMKPlayerFunc.VO_MK_Player_SetBufferFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
    lRes = gMKPlayerFunc.VO_MK_Player_SetErrorFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
    lRes = gMKPlayerFunc.VO_MK_Player_SetEventFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
    lRes = gMKPlayerFunc.VO_MK_Player_SetStateFunc(gMKPlayerFunc.lPlayer, NULL, NULL);
  }

  sp<vomeplayer> mp = setvomeplayer(env, thiz, 0);
  if (mp != NULL) {
    // this prevents native callbacks after the object is released
    mp->setListener(0);
    mp->disconnect();
	 mp->SetParam(MOBITV_FORCE_STOP,0);
  }
  //force_stop to make sure that the 

  DestroyMKSource();
  VOINFO("%s", "<<<JNI>>> leave");
}


static void
vomp_native_finalize(JNIEnv *env, jobject thiz)
{
  VOINFO("%s", "<<<JNI>>> enter");
  LOGV("native_finalize");
  // Release has to be done in JAVA. If we do it here it will get executed when the Garbage Collector runs and
  // one can already have created a new object and it will override the existing one.
  //vomp_release(env, thiz);
  VOINFO("%s", "<<<JNI>>> leave");
}

static jint
//vomp_snoop(JNIEnv* env, jobject thiz, jobject data, jint kind) {
vomp_snoop(JNIEnv* env, jobject thiz, jshortArray data, jint kind) {
    VOINFO("%s", "<<<JNI>>> enter");
    jsize len = env->GetArrayLength((jarray)data);
    //jshort* ar = (jshort*)env->GetPrimitiveArrayCritical((jarray)data, 0);
    jshort* ar = env->GetShortArrayElements(data, 0);
    int ret = 0;
    if (ar) {
        ret = vomeplayer::snoop(ar, len, kind);
        //env->ReleasePrimitiveArrayCritical((jarray)data, ar, 0);
	env->ReleaseShortArrayElements(data, ar, 0);
    }
    VOINFO("%s", "<<<JNI>>> leave");
    return ret;
}

static jlong vomp_mkplayer(JNIEnv* env, jobject thiz) {
    VOINFO("%s", "<<<JNI>>> enter");
    VOINFO("%s", "<<<JNI>>> leave");
  return (jlong)gMKPlayerFunc.lPlayer;
}

//static jboolean vomp_mkplayer_MobiDrmContentIsEncrypted(JNIEnv* env, jobject thiz, jarray data)
static jboolean vomp_mkplayer_MobiDrmContentIsEncrypted(JNIEnv* env, jobject thiz, jcharArray data)
{
    bool lRes = FALSE;
    VOINFO("<<<JNI>>> enter data=0x%x", data);
    if (NULL == data)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "data cannot be null");
        return lRes;
    }
    if (NULL == gMKPlayerFunc.VO_MK_MobiDrmContentIsEncrypted)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.VO_MK_MobiDrmContentIsEncrypted cannot be null");
	return lRes;
    }

    unsigned long lSize = env->GetArrayLength(data);
    //char* lBuf = (char*)env->GetPrimitiveArrayCritical(data, 0);
    jchar* lBuf = env->GetCharArrayElements(data, 0);
    if (NULL == lBuf)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return lRes;
    }

    lRes = gMKPlayerFunc.VO_MK_MobiDrmContentIsEncrypted((MK_U8*)lBuf, lSize);

    //env->ReleasePrimitiveArrayCritical(data, lBuf, 0);
    env->ReleaseCharArrayElements(data, lBuf, 0);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
    return lRes;
}

//static jlong vomp_mkplayer_MobiDrmGetContentInfo(JNIEnv* env, jobject thiz, jarray data, jobject contentinfo)
static jlong vomp_mkplayer_MobiDrmGetContentInfo(JNIEnv* env, jobject thiz, jcharArray data, jobject contentinfo)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter data=0x%x, contentinfo=0x%x", data, contentinfo);
    if (NULL == data || NULL == contentinfo)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "data or contentinfo cannot be null");
        return MK_E_IARG;
    }
    if (NULL == gMKPlayerFunc.VO_MK_MobiDrmGetContentInfo)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.VO_MK_MobiDrmGetContentInfo cannot be null");
        return MK_E_PCOND;
    }

    unsigned long lSize = env->GetArrayLength(data);
    //char* lBuf = (char*)env->GetPrimitiveArrayCritical(data, 0);
    jchar* lBuf = env->GetCharArrayElements(data, 0);
    if (NULL == lBuf)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    sMK_MobiDrmMbdcHeader lHeader;
    lRes = gMKPlayerFunc.VO_MK_MobiDrmGetContentInfo((MK_U8*)lBuf, lSize, &lHeader);

    jfieldID lFieldId;
    jclass lObjClass = env->GetObjectClass(contentinfo);

    lFieldId = env->GetFieldID(lObjClass, "mMajorVersion", "B");
    env->SetByteField(contentinfo, lFieldId, lHeader.mMajorVersion);

    lFieldId = env->GetFieldID(lObjClass, "mMinorVersion", "B");
    env->SetByteField(contentinfo, lFieldId, lHeader.mMinorVersion);

    lFieldId = env->GetFieldID(lObjClass, "mInventoryID", "J");
    env->SetLongField(contentinfo, lFieldId, lHeader.mInventoryID);

    lFieldId = env->GetFieldID(lObjClass, "mContentExpiry", "J");
    env->SetLongField(contentinfo, lFieldId, lHeader.mContentExpiry);

    lFieldId = env->GetFieldID(lObjClass, "mBroadcastID", "I");
    env->SetIntField(contentinfo, lFieldId, lHeader.mBroadcastID);

    // Make sure that local ref is released before a potential exception
    //env->ReleasePrimitiveArrayCritical(data, lBuf, 0);
    env->ReleaseCharArrayElements(data, lBuf, 0);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

//static jlong vomp_mkplayer_MobiDrmGetLicenseInfo(JNIEnv* env, jobject thiz, jarray license, jarray privatekey, jlong currenttime, jobject licenseinfo)
static jlong vomp_mkplayer_MobiDrmGetLicenseInfo(JNIEnv* env, jobject thiz, jcharArray license, jcharArray privatekey, jlong currenttime, jobject licenseinfo)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter license=0x%x, licenseinfo=0x%x, privatekey=0x%x, currenttime=0x%x", license, licenseinfo, privatekey, currenttime);
    if (NULL == license || NULL == licenseinfo)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "license or licenseinfo cannot be null");
        return MK_E_IARG;
    }
    if (NULL == gMKPlayerFunc.VO_MK_MobiDrmGetLicenseInfo)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.VO_MK_MobiDrmGetLicenseInfo cannot be null");
        return MK_E_PCOND;
    }

    unsigned long lSize = env->GetArrayLength(license);
    //char* lLic = (char*)env->GetPrimitiveArrayCritical(license, 0);
    jchar* lLic = env->GetCharArrayElements(license, 0);
    if (NULL == lLic)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    jchar* lKey = NULL;
    unsigned long lKeySize = 0;
    if (NULL != privatekey)
    {
        lKeySize = env->GetArrayLength(privatekey);
        //lKey = (char*)env->GetPrimitiveArrayCritical(privatekey, 0);
        lKey = env->GetCharArrayElements(privatekey, 0);
        if (NULL == lKey)
        {
            jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
            return MK_E_NOMEM;
        }
    }

    sMK_MobiDrmLicense lLicense;
    lRes = gMKPlayerFunc.VO_MK_MobiDrmGetLicenseInfo((MK_U8*)lLic, lSize, (MK_U8*)lKey, lKeySize, currenttime, NULL, &lLicense, NULL);

    jfieldID lFieldId;
    jclass lObjClass = env->GetObjectClass(licenseinfo);

    lFieldId = env->GetFieldID(lObjClass, "mMajorVersion", "B");
    env->SetByteField(licenseinfo, lFieldId, lLicense.mMajorVersion);

    lFieldId = env->GetFieldID(lObjClass, "mMinorVersion", "B");
    env->SetByteField(licenseinfo, lFieldId, lLicense.mMinorVersion);

    lFieldId = env->GetFieldID(lObjClass, "mLicenseID", "I");
    env->SetIntField(licenseinfo, lFieldId, lLicense.mLicenseID);

    lFieldId = env->GetFieldID(lObjClass, "mInventoryID", "J");
    env->SetLongField(licenseinfo, lFieldId, lLicense.mInventoryID);

    lFieldId = env->GetFieldID(lObjClass, "mBroadcastID", "I");
    env->SetIntField(licenseinfo, lFieldId, lLicense.mBroadcastID);

    lFieldId = env->GetFieldID(lObjClass, "mPackageID", "I");
    env->SetIntField(licenseinfo, lFieldId, lLicense.mPackageID);

    lFieldId = env->GetFieldID(lObjClass, "mLicenseExpiry", "J");
    env->SetLongField(licenseinfo, lFieldId, lLicense.mLicenseExpiry);

    lFieldId = env->GetFieldID(lObjClass, "mLicenseStart", "J");
    env->SetLongField(licenseinfo, lFieldId, lLicense.mLicenseStart);
    
    lFieldId = env->GetFieldID(lObjClass, "mContentExpiry", "J");
    env->SetLongField(licenseinfo, lFieldId, lLicense.mContentExpiry);

    lFieldId = env->GetFieldID(lObjClass, "mMaxPlaybackCount", "I");
    env->SetIntField(licenseinfo, lFieldId, lLicense.mMaxPlaybackCount);

    lFieldId = env->GetFieldID(lObjClass, "mAutoRenew", "B");
    env->SetByteField(licenseinfo, lFieldId, lLicense.mAutoRenew);

    lFieldId = env->GetFieldID(lObjClass, "mDeleteOnExpiry", "B");
    env->SetByteField(licenseinfo, lFieldId, lLicense.mDeleteOnExpiry);

    lFieldId = env->GetFieldID(lObjClass, "mLicenseType", "B");
    env->SetByteField(licenseinfo, lFieldId, lLicense.mLicenseType);

    jstring lUIdStr = NULL;
    jstring lDIdStr = NULL;
    jstring lXIdStr = NULL;
    // only need to check if one of the strings are not NULL
    if (NULL != lLicense.mIDs.mUserID)
    {
        lUIdStr = env->NewStringUTF((const char*)lLicense.mIDs.mUserID);
        lDIdStr = env->NewStringUTF((const char*)lLicense.mIDs.mDeviceID);
        lXIdStr = env->NewStringUTF((const char*)lLicense.mIDs.mXvalueID);
        free(lLicense.mIDs.mUserID);
        free(lLicense.mIDs.mDeviceID);
        free(lLicense.mIDs.mXvalueID);
    }
    lFieldId = env->GetFieldID(lObjClass, "mUId", "Ljava/lang/String;");
    env->SetObjectField(licenseinfo, lFieldId, lUIdStr);

    lFieldId = env->GetFieldID(lObjClass, "mDId", "Ljava/lang/String;");
    env->SetObjectField(licenseinfo, lFieldId, lDIdStr);
    
    lFieldId = env->GetFieldID(lObjClass, "mXId", "Ljava/lang/String;");
    env->SetObjectField(licenseinfo, lFieldId, lXIdStr);

    // Make sure that local ref is released before a potential exception
    if (NULL != lKey)
    {
      //env->ReleasePrimitiveArrayCritical(privatekey, lKey, 0);
      env->ReleaseCharArrayElements(privatekey, lKey, 0);
    }
    //env->ReleasePrimitiveArrayCritical(license, lLic, 0);
    env->ReleaseCharArrayElements(license, lLic, 0);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes && MK_E_DRM_LICNSTART != lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_Net_SetTimeout(JNIEnv* env, jobject thiz, int msec)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, msec=%d", gMKPlayerFunc.lPlayer, msec);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_Net_SetTimeout(gMKPlayerFunc.lPlayer, msec);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

//static jlong vomp_mkplayer_MobiDrmSetPrivateKey(JNIEnv* env, jobject thiz, jarray key)
static jlong vomp_mkplayer_MobiDrmSetPrivateKey(JNIEnv* env, jobject thiz, jcharArray key)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, key=0x%x", gMKPlayerFunc.lPlayer, key);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (NULL == key)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "key cannot be null");
        return MK_E_IARG;
    }
    unsigned long lSize = env->GetArrayLength(key);
    jchar* lKey = env->GetCharArrayElements(key, 0);
    if (NULL == lKey)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_MobiDRM_SetKey(gMKPlayerFunc.lPlayer, lKey, lSize);

    // Make sure that local ref is released before a potential exception
    //env->ReleasePrimitiveArrayCritical(key, lKey, 0);
    env->ReleaseCharArrayElements(key, lKey, 0);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_MobiDrmSetDeviceIDs(JNIEnv* env, jobject thiz, jstring did, jstring xid)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, did=0x%x, xid=0x%x", gMKPlayerFunc.lPlayer, did, xid);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (did == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "device id cannot be null");
        return MK_E_IARG;
    }
    const char *lDidStr = env->GetStringUTFChars(did, NULL);
    if (lDidStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    if (xid == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "xvalue id cannot be null");
        return MK_E_IARG;
    }
    const char *lXidStr = env->GetStringUTFChars(xid, NULL);
    if (lXidStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_MobiDRM_SetDeviceIDs(gMKPlayerFunc.lPlayer, lDidStr, strlen(lDidStr), lXidStr, strlen(lXidStr));

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(did, lDidStr);
    env->ReleaseStringUTFChars(xid, lXidStr);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_MobiDrmSetUserID(JNIEnv* env, jobject thiz, jstring uid)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, uid=0x%x", gMKPlayerFunc.lPlayer, uid);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (uid == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "user id cannot be null");
        return MK_E_IARG;
    }
    const char *lUidStr = env->GetStringUTFChars(uid, NULL);
    if (lUidStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_MobiDRM_SetUserID(gMKPlayerFunc.lPlayer, lUidStr, strlen(lUidStr));

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(uid, lUidStr);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

//static jlong vomp_mkplayer_MobiDrmSetLicense(JNIEnv* env, jobject thiz, jarray license, jlong currenttime)
static jlong vomp_mkplayer_MobiDrmSetLicense(JNIEnv* env, jobject thiz, jcharArray license, jlong currenttime)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, license=0x%x, currenttime=0x%x", gMKPlayerFunc.lPlayer, license, currenttime);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    jchar* lLicense = NULL;
    unsigned long lSize = 0; 
    if (NULL != license)
    {
        lSize = env->GetArrayLength(license);
        //lLicense = (char*)env->GetPrimitiveArrayCritical(license, 0);
	lLicense = env->GetCharArrayElements(license, 0);
        if (NULL == lLicense)
        {
            jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
            return MK_E_NOMEM;
        }
    }

    lRes = gMKPlayerFunc.VO_MK_Player_MobiDRM_SetLicense(gMKPlayerFunc.lPlayer, lLicense, lSize, currenttime);

    // Make sure that local ref is released before a potential exception
    //env->ReleasePrimitiveArrayCritical(license, lLicense, 0);
    env->ReleaseCharArrayElements(license, lLicense, 0);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_HTTP_SetProxy(JNIEnv* env, jobject thiz, jstring host, short port)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, host=0x%x, port=0x%x", gMKPlayerFunc.lPlayer, host, port);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (NULL == host)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "host cannot be null");
        return MK_E_IARG;
    }

    const char *lHostStr = env->GetStringUTFChars(host, NULL);
    if (lHostStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_HTTP_SetProxy(gMKPlayerFunc.lPlayer, lHostStr, port);

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(host, lHostStr);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_HTTP_SetUseBA(JNIEnv* env, jobject thiz, int onOff)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, onOff=%d", gMKPlayerFunc.lPlayer, onOff);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (onOff==0)
      lRes = gMKPlayerFunc.VO_MK_Player_HTTP_SetUseBA(gMKPlayerFunc.lPlayer, MK_FALSE);
    else
      lRes = gMKPlayerFunc.VO_MK_Player_HTTP_SetUseBA(gMKPlayerFunc.lPlayer, MK_TRUE);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_HTTP_SetUserAgentVals(JNIEnv* env, jobject thiz, jstring name, jstring version, jstring device, jstring extra)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, name=0x%x, version=0x%x, device=0x%x, extra=0x%x", gMKPlayerFunc.lPlayer, name, version, device, extra);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (name == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "name cannot be null");
        return MK_E_IARG;
    }
    const char *lNameStr = env->GetStringUTFChars(name, NULL);
    if (lNameStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    if (version == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "version cannot be null");
        return MK_E_IARG;
    }
    const char *lVersionStr = env->GetStringUTFChars(version, NULL);
    if (lVersionStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    if (device == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "device cannot be null");
        return MK_E_IARG;
    }
    const char *lDeviceStr = env->GetStringUTFChars(device, NULL);
    if (lNameStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    if (extra == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "extra cannot be null");
        return MK_E_IARG;
    }
    const char *lExtraStr = env->GetStringUTFChars(extra, NULL);
    if (lVersionStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_HTTP_SetUserAgentVals(gMKPlayerFunc.lPlayer, lNameStr, lVersionStr, lDeviceStr, lExtraStr);

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(name, lNameStr);
    env->ReleaseStringUTFChars(version, lVersionStr);
    env->ReleaseStringUTFChars(device, lDeviceStr);
    env->ReleaseStringUTFChars(extra, lExtraStr);
    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_HTTP_SetUseKeepAlive(JNIEnv* env, jobject thiz, jboolean on)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, on=%s", gMKPlayerFunc.lPlayer, on ? "TRUE" : "FALSE");
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_HTTP_SetUseKeepAlive(gMKPlayerFunc.lPlayer, on);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}

static jlong vomp_mkplayer_HTTP_SetBALimits(JNIEnv* env, jobject thiz, int lower, int upper)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, lower=%d, upper=%d", gMKPlayerFunc.lPlayer, lower, upper);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_HTTP_SetBALimits(gMKPlayerFunc.lPlayer, lower, upper);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}


static jlong vomp_mkplayer_Media_SetBufferLimits(JNIEnv* env, jobject thiz, int minmsec, int lowmsec, int maxmsec)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, minmsec=%d, lowmsec=%d, maxmsec=%d", gMKPlayerFunc.lPlayer, minmsec, lowmsec, maxmsec);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_Media_SetBufferLimits(gMKPlayerFunc.lPlayer, minmsec, lowmsec, maxmsec);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
#ifdef THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    if (0 > lRes)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Unknown result code");
    }
#endif // THROW_RUNTIME_EXCEPTION_WITH_MEDIAKIT_RESULTS
    return lRes;
}


static jlong vomp_mkplayer_SetOption(JNIEnv* env, jobject thiz, jstring key, jstring val)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, host=0x%x, port=0x%x", gMKPlayerFunc.lPlayer, key, value);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (NULL==key)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "key cannot be null");
        return MK_E_IARG;
    }

    if (NULL==val)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "value cannot be null");
        return MK_E_IARG;
    }

    const char *lKeyStr = env->GetStringUTFChars(key, NULL);
    if (lKeyStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    const char *lValStr = env->GetStringUTFChars(val, NULL);
    if (lKeyStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_SetOption(gMKPlayerFunc.lPlayer, lKeyStr, lValStr);

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(key, lKeyStr);
    env->ReleaseStringUTFChars(val, lValStr);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
    return lRes;
}


static jlong vomp_mkplayer_SetOptions(JNIEnv* env, jobject thiz, jstring str)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, str=0x%x", gMKPlayerFunc.lPlayer, str);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (NULL==str)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "string cannot be null");
        return MK_E_IARG;
    }

    const char *lStr = env->GetStringUTFChars(str, NULL);
    if (lStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_SetOptions(gMKPlayerFunc.lPlayer, lStr);

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(str, lStr);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
    return lRes;
}


static jlong vomp_mkplayer_SetOptionsF(JNIEnv* env, jobject thiz, jstring str)
{
    long lRes = MK_E_FAIL;
    VOINFO("<<<JNI>>> enter gMKPlayerFunc.lPlayer=0x%x, str=0x%x", gMKPlayerFunc.lPlayer, str);
    if (NULL == gMKPlayerFunc.lPlayer)
    {
        jniThrowException(env, "java/lang/IllegalStateException", "gMKPlayerFunc.lPlayer cannot be null");
        return MK_E_PCOND;
    }

    if (NULL==str)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", "string cannot be null");
        return MK_E_IARG;
    }

    const char *lStr = env->GetStringUTFChars(str, NULL);
    if (lStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return MK_E_NOMEM;
    }

    lRes = gMKPlayerFunc.VO_MK_Player_SetOptionsF(gMKPlayerFunc.lPlayer, lStr);

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(str, lStr);

    VOINFO("<<<JNI>>> leave lRes=0x%08x", lRes);
    return lRes;
}


static JNINativeMethod g_vomemethods[] = {
    {"setDataSource",       "(Ljava/lang/String;)V",            (void *)vomp_setDataSource},
    {"setDataSource",       "(Ljava/io/FileDescriptor;JJ)V",    (void *)vomp_setDataSourceFD},
    {"_setVideoSurface",    "()V",                              (void *)vomp_setVideoSurface},
    {"prepare",             "()V",                              (void *)vomp_prepare},
    {"prepareAsync",        "()V",                              (void *)vomp_prepareAsync},
    {"_start",              "()V",                              (void *)vomp_start},
    {"_stop",               "()V",                              (void *)vomp_stop},
    {"getVideoWidth",       "()I",                              (void *)vomp_getVideoWidth},
    {"getVideoHeight",      "()I",                              (void *)vomp_getVideoHeight},
    {"seekTo",              "(I)V",                             (void *)vomp_seekTo},
    {"_pause",              "()V",                              (void *)vomp_pause},
    {"isPlaying",           "()Z",                              (void *)vomp_isPlaying},
    {"getCurrentPosition",  "()I",                              (void *)vomp_getCurrentPosition},
    {"getDuration",         "()I",                              (void *)vomp_getDuration},
    {"_release",            "()V",                              (void *)vomp_release},
    {"_reset",              "()V",                              (void *)vomp_reset},
    {"setAudioStreamType",  "(I)V",                             (void *)vomp_setAudioStreamType},
    {"setLooping",          "(Z)V",                             (void *)vomp_setLooping},
    {"isLooping",           "()Z",                              (void *)vomp_isLooping},
    {"setVolume",           "(FF)V",                            (void *)vomp_setVolume},
    //{"getFrameAt",          "(I)Landroid/graphics/Bitmap;",     (void *)vomp_getFrameAt},
    {"native_invoke",       "(Landroid/os/Parcel;Landroid/os/Parcel;)I",(void *)vomp_invoke},
    //{"native_setMetadataFilter", "(Landroid/os/Parcel;)I",      (void *)vomp_setMetadataFilter},
    //{"native_getMetadata", "(ZZLandroid/os/Parcel;)Z",          (void *)vomp_getMetadata},
    {"native_init",         "()V",                              (void *)vomp_native_init},
    {"native_init",         "(Ljava/lang/String;)V",            (void *)vomp_native_init_location},
    {"native_init",         "(Ljava/lang/String;Ljava/lang/String;)V", (void *)vomp_native_init_location_library},
    {"native_setup",        "(Ljava/lang/Object;)V",            (void *)vomp_native_setup},
    {"native_finalize",     "()V",                              (void *)vomp_native_finalize},
    //{"snoop",               "([SI)I",                           (void *)vomp_snoop},
    //{"mkplayer_ptr",        "()J",                              (void *)vomp_mkplayer},
    {"mkplayer_MobiDrmContentIsEncrypted",  "([B)Z",                                    (void *)vomp_mkplayer_MobiDrmContentIsEncrypted},
    {"mkplayer_MobiDrmGetContentInfo",      "([BLjava/lang/Object;)J",                  (void *)vomp_mkplayer_MobiDrmGetContentInfo},
    {"mkplayer_MobiDrmGetLicenseInfo",      "([B[BJLjava/lang/Object;)J",               (void *)vomp_mkplayer_MobiDrmGetLicenseInfo},
    {"mkplayer_Net_SetTimeout",             "(I)J",                                     (void *)vomp_mkplayer_Net_SetTimeout},
    {"mkplayer_MobiDrmSetPrivateKey",       "([B)J",                                    (void *)vomp_mkplayer_MobiDrmSetPrivateKey},
    {"mkplayer_MobiDrmSetDeviceIDs",        "(Ljava/lang/String;Ljava/lang/String;)J",  (void *)vomp_mkplayer_MobiDrmSetDeviceIDs},
    {"mkplayer_MobiDrmSetUserID",           "(Ljava/lang/String;)J",                    (void *)vomp_mkplayer_MobiDrmSetUserID},
    {"mkplayer_MobiDrmSetLicense",          "([BJ)J",                                   (void *)vomp_mkplayer_MobiDrmSetLicense},
    {"mkplayer_HTTP_SetProxy",              "(Ljava/lang/String;S)J",                   (void *)vomp_mkplayer_HTTP_SetProxy},
    {"mk_player_HTTP_SetUseBA",             "(I)J",                                     (void *)vomp_mkplayer_HTTP_SetUseBA},
    {"mkplayer_HTTP_SetUserAgentVals",      "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)J",  (void *)vomp_mkplayer_HTTP_SetUserAgentVals},
    {"mkplayer_HTTP_SetUseKeepAlive",       "(Z)J",                                     (void *)vomp_mkplayer_HTTP_SetUseKeepAlive},
    {"mkplayer_HTTP_SetBALimits",           "(II)J",                                    (void *)vomp_mkplayer_HTTP_SetBALimits},
    {"mkplayer_Media_SetBufferLimits",      "(III)J",                                   (void *)vomp_mkplayer_Media_SetBufferLimits},
    {"mkplayer_SetOption",                  "(Ljava/lang/String;Ljava/lang/String;)J",  (void *)vomp_mkplayer_SetOption},
    {"mkplayer_SetOptions",                 "(Ljava/lang/String;)J",                    (void *)vomp_mkplayer_SetOptions},
    {"mkplayer_SetOptionsF",                "(Ljava/lang/String;)J",                    (void *)vomp_mkplayer_SetOptionsF},
};


// This function only registers the native methods
static int register_vomeplayer(JNIEnv *env) {
  jclass clazz;
  VOINFO("RegisterNatives for %s",  g_szvompclspathname);
  clazz = env->FindClass(g_szvompclspathname);
  if (clazz == NULL) {
    VOINFO("%s not found!", g_szvompclspathname);
    return JNI_FALSE;
  }
  
  if (env->RegisterNatives(clazz, g_vomemethods, sizeof(g_vomemethods) / sizeof(g_vomemethods[0])) < 0) {
    VOINFO("RegisterNatives failed2: '%s' ", g_szvompclspathname);
    return JNI_FALSE;
  }
  
  VOINFO("Succeed to register %s.",  g_szvompclspathname);
  return JNI_TRUE;
}


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
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

  if (register_vomeplayer(env) < 0) {
    LOGE("ERROR: vomeplayer native registration failed.\n");
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


