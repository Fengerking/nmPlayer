
#define LOG_TAG "vomejni"

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
#include <linux/fb.h>
#include <utils/threads.h>
#include <jni.h>
#include <JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include <utils/Errors.h> 
#include <android_util_Binder.h>
#include "voCMutex.h"
#include "vomeplayer.h"
#include "vojnilog.h"



using namespace android;


struct fields_t {
    jfieldID    context;
    jfieldID    surface;
    jfieldID    surface_native;
    jmethodID   post_event;
};


static fields_t fields;
static voCMutex g_mautolock;
static const char* const g_szvompclsname = "com/visualon/vome/vomeplayer";
static char g_szpackagename[256]; 


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
	LOGW ("@@@@@@@@@@@@@   JNIvomeplayerListener   0000");

    // Hold onto the vomeplayer class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        JNILOGE("Can't find com/visualon/vomeplayer");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }

	LOGW ("@@@@@@@@@@@@@   JNIvomeplayerListener   1111");

    mClass = (jclass)env->NewGlobalRef(clazz);

	LOGW ("@@@@@@@@@@@@@   JNIvomeplayerListener   2222");

    // We use a weak reference so the vomeplayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);

	LOGW ("@@@@@@@@@@@@@   JNIvomeplayerListener   3333");

}

JNIvomeplayerListener::~JNIvomeplayerListener()
{
    // remove global references
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}

void JNIvomeplayerListener::notify(int msg, int ext1, int ext2)
{
    JNIEnv *env = AndroidRuntime::getJNIEnv();
   
   env->CallStaticVoidMethod(mClass, fields.post_event, mObject, msg, ext1, ext2, 0);
  JNILOGE2("listener2 evn %d", env); 
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
	  JNILOGI2("getvomeplayer %p", env);
    voCAutoLock l(&g_mautolock);
    vomeplayer* const p = (vomeplayer*)env->GetIntField(thiz, fields.context);
    
    JNILOGI("after get int field");
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
    if (pathStr == NULL) {  // Out of memory
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

	
    status_t opStatus = mp->setDataSource(pathStr);

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(path, pathStr);
    process_media_player_call( env, thiz, opStatus, "java/io/IOException", "setDataSource failed." );
}

static void
vomp_setDataSourceFD(JNIEnv *env, jobject thiz, jobject fileDescriptor, jlong offset, jlong length)
{
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
}

static void setVideoSurface(const sp<vomeplayer>& mp, JNIEnv *env, jobject thiz) {

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
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    setVideoSurface(mp, env, thiz);
}

static void
vomp_prepare(JNIEnv *env, jobject thiz)
{
//	  JNILOGI("vomp_prepare");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    setVideoSurface(mp, env, thiz);
    process_media_player_call( env, thiz, mp->prepare(), "java/io/IOException", "Prepare failed." );
}

static void
vomp_prepareAsync(JNIEnv *env, jobject thiz)
{
//	 JNILOGI("vomp_prepareAsync");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    jobject surface = env->GetObjectField(thiz, fields.surface);
    if (surface != NULL) {
        const sp<Surface> native_surface = get_surface(env, surface);
        LOGV("prepareAsync: surface=%p (id=%d)",
             native_surface.get(), native_surface->ID());
        mp->setVideoSurface(native_surface);
    }
    process_media_player_call( env, thiz, mp->prepareAsync(), "java/io/IOException", "Prepare Async failed." );
}

static void
vomp_start(JNIEnv *env, jobject thiz)
{
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
		mp->start();
    //process_media_player_call(env, thiz, mp->start(), NULL, NULL );
}

static void
vomp_stop(JNIEnv *env, jobject thiz)
{
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->stop(), NULL, NULL );
}

static void
vomp_pause(JNIEnv *env, jobject thiz)
{
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
		mp->pause();
    //process_media_player_call( env, thiz, mp->pause(), NULL, NULL );
}

static jboolean
vomp_isPlaying(JNIEnv *env, jobject thiz)
{
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    const jboolean is_playing = mp->isPlaying();

    LOGV("isPlaying: %d", is_playing);
    return is_playing;
}

static void
vomp_seekTo(JNIEnv *env, jobject thiz, int msec)
{
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    LOGV("seekTo: %d(msec)", msec);
    process_media_player_call( env, thiz, mp->seekTo(msec), NULL, NULL );
}

static int
vomp_getVideoWidth(JNIEnv *env, jobject thiz)
{
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
    return w;
}

static int
vomp_getVideoHeight(JNIEnv *env, jobject thiz)
{
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
    return h;
}


static int
vomp_getCurrentPosition(JNIEnv *env, jobject thiz)
{
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int msec;
    process_media_player_call( env, thiz, mp->getCurrentPosition(&msec), NULL, NULL );
    LOGV("getCurrentPosition: %d (msec)", msec);
    return msec;
}

static int
vomp_getDuration(JNIEnv *env, jobject thiz)
{
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return 0;
    }
    int msec;
    process_media_player_call( env, thiz, mp->getDuration(&msec), NULL, NULL );
    LOGV("getDuration: %d (msec)", msec);
    return msec;
}

static void
vomp_reset(JNIEnv *env, jobject thiz)
{
    LOGV("reset");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->reset(), NULL, NULL );
}

static void
vomp_setAudioStreamType(JNIEnv *env, jobject thiz, int streamtype)
{
    LOGV("setAudioStreamType: %d", streamtype);
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setAudioStreamType(streamtype) , NULL, NULL );
}

static void
vomp_setLooping(JNIEnv *env, jobject thiz, jboolean looping)
{
    LOGV("setLooping: %d", looping);
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setLooping(looping), NULL, NULL );
}

static jboolean
vomp_isLooping(JNIEnv *env, jobject thiz)
{
    LOGV("isLooping");
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    return mp->isLooping();
}

static void
vomp_setVolume(JNIEnv *env, jobject thiz, float leftVolume, float rightVolume)
{
    LOGV("setVolume: left %f  right %f", leftVolume, rightVolume);
    sp<vomeplayer> mp = getvomeplayer(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setVolume(leftVolume, rightVolume), NULL, NULL );
}

// FIXME: deprecated
static jobject
vomp_getFrameAt(JNIEnv *env, jobject thiz, jint msec)
{
    return NULL;
}


// Sends the request and reply parcels to the media player via the
// binder interface.
static jint
vomp_invoke(JNIEnv *env, jobject thiz,
                                 jobject java_request, jobject java_reply)
{
    sp<vomeplayer> media_player = getvomeplayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }


    Parcel *request = parcelForJavaObject(env, java_request);
    Parcel *reply = parcelForJavaObject(env, java_reply);

    // Don't use process_media_player_call which use the async loop to
    // report errors, instead returns the status.
    return media_player->invoke(*request, reply);
}

// Sends the new filter to the client.
static jint
vomp_setMetadataFilter(JNIEnv *env, jobject thiz, jobject request)
{
    sp<vomeplayer> media_player = getvomeplayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
    }

    Parcel *filter = parcelForJavaObject(env, request);

    if (filter == NULL ) {
        jniThrowException(env, "java/lang/RuntimeException", "Filter is null");
        return UNKNOWN_ERROR;
    }

    return media_player->setMetadataFilter(*filter);
}

static jboolean
vomp_getMetadata(JNIEnv *env, jobject thiz, jboolean update_only,
                                      jboolean apply_filter, jobject reply)
{
    sp<vomeplayer> media_player = getvomeplayer(env, thiz);
    if (media_player == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }

    Parcel *metadata = parcelForJavaObject(env, reply);

    if (metadata == NULL ) {
        jniThrowException(env, "java/lang/RuntimeException", "Reply parcel is null");
        return false;
    }

    metadata->freeData();
    // On return metadata is positioned at the beginning of the
    // metadata. Note however that the parcel actually starts with the
    // return code so you should not rewind the parcel using
    // setDataPosition(0).
    return media_player->getMetadata(update_only, apply_filter, metadata) == OK;
}

// This function gets some field IDs, which in turn causes class initialization.
// It is called from a static block in vomeplayer, which won't run until the
// first time an instance of this class is used.


static void vomp_native_init(JNIEnv *env,  jobject thiz, jstring packagename) {
	
	LOGW ("@@@@@@@@@@@@@   vomp_native_init   0000,  point is %d", (int)env);

	const char* szpath = env->GetStringUTFChars(packagename, NULL);
	if (szpath == NULL) {  // Out of memory
		jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
		return;
	}

	LOGW ("@@@@@@@@@@@@@   vomp_native_init   0000111111111");

//	VOINFO("%s", szpath);
	if (strlen(szpath) < 128)
		strcpy(g_szpackagename, szpath);
	else 
		strcpy(g_szpackagename, "/data/data/com.visualon.vome");

	LOGW ("@@@@@@@@@@@@@   vomp_native_init   1111");

	uid_t uid = getuid();
	uid_t euid = geteuid();
	gid_t gid = getgid();
	gid_t egid = getegid();
//	JNILOGI2("uid: %d, gid: %d, euid: %d, egid: %d\n", uid, gid, euid, egid);

	char buf[128];
	int rs = readlink("/proc/self/exe", buf, 128);
	if (rs<0 || rs>=128)
		rs = 0;
	buf[rs] = '\0';
//	VOINFO("module name: %s\n", buf);

	LOGW ("@@@@@@@@@@@@@   vomp_native_init   2222");

    jclass clazz;
    clazz = env->FindClass(g_szvompclsname);
    if (clazz == NULL) {
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

	LOGW ("@@@@@@@@@@@@@   vomp_native_init   3333");

}	

static void
vomp_native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
	LOGW ("@@@@@@@@@@@@@   vomp_native_setup   44444");

    sp<vomeplayer> mp = new vomeplayer();
    if (mp == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

	LOGW ("@@@@@@@@@@@@@   vomp_native_setup   1111");

    // create new listener and give it to vomeplayer
    sp<JNIvomeplayerListener> listener = new JNIvomeplayerListener(env, thiz, weak_this);
    mp->setListener(listener);
		mp->setWorkingPath(g_szpackagename);

	LOGW ("@@@@@@@@@@@@@   vomp_native_setup   22222");

    // Stow our new C++ vomeplayer in an opaque field in the Java object.
    setvomeplayer(env, thiz, mp);

	LOGW ("@@@@@@@@@@@@@   vomp_native_setup   33333");

}

static void
vomp_release(JNIEnv *env, jobject thiz)
{
    JNILOGV("release");
    sp<vomeplayer> mp = setvomeplayer(env, thiz, 0);
    if (mp != NULL) {
        // this prevents native callbacks after the object is released
        mp->setListener(0);
        //mp->disconnect();
    }
}

static void
vomp_native_finalize(JNIEnv *env, jobject thiz)
{
    JNILOGV("native_finalize");
    vomp_release(env, thiz);
}

static jint
vomp_snoop(JNIEnv* env, jobject thiz, jobject data, jint kind) {
    jshort* ar = (jshort*)env->GetPrimitiveArrayCritical((jarray)data, 0);
    jsize len = env->GetArrayLength((jarray)data);
    int ret = 0;
    if (ar) {
        ret = vomeplayer::snoop(ar, len, kind);
        env->ReleasePrimitiveArrayCritical((jarray)data, ar, 0);
    }
    return ret;
}

// ----------------------------------------------------------------------------

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
    {"getFrameAt",          "(I)Landroid/graphics/Bitmap;",     (void *)vomp_getFrameAt},
    {"native_invoke",       "(Landroid/os/Parcel;Landroid/os/Parcel;)I",(void *)vomp_invoke},
    {"native_setMetadataFilter", "(Landroid/os/Parcel;)I",      (void *)vomp_setMetadataFilter},
    {"native_getMetadata", "(ZZLandroid/os/Parcel;)Z",          (void *)vomp_getMetadata},
    {"native_init",         "(Ljava/lang/String;)V",						(void *)vomp_native_init},
    {"native_setup",        "(Ljava/lang/Object;)V",            (void *)vomp_native_setup},
    {"native_finalize",     "()V",                              (void *)vomp_native_finalize},
    {"snoop",               "([SI)I",                           (void *)vomp_snoop},
};


// This function only registers the native methods
static int register_vomeplayer(JNIEnv *env) {

	jclass clazz;
  JNILOGI2("RegisterNatives for %s",  g_szvompclsname);
	clazz = env->FindClass(g_szvompclsname);
	if (clazz == NULL) {
		JNILOGI2("%s not found!", g_szvompclsname);
		return JNI_FALSE;
	}

	if (env->RegisterNatives(clazz, g_vomemethods, sizeof(g_vomemethods) / sizeof(g_vomemethods[0])) < 0) {
		JNILOGI2("RegisterNatives failed: '%s' ", g_szvompclsname);
		return JNI_FALSE;
	}
	
  JNILOGI2("Succeed to register %s.",  g_szvompclsname);
	return JNI_TRUE;
}

extern int register_vommretriever(JNIEnv *env);

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	LOGW ("@@@@@@@@@@@@@   JNI_OnLoad   0000");

	jint jniver = JNI_VERSION_1_4;
	if (vm->GetEnv((void**) &env, jniver) != JNI_OK) {
		jniver = JNI_VERSION_1_6;
		if (vm->GetEnv((void**)&env, jniver) != JNI_OK) {
			LOGE("ERROR: GetEnv failed\n");
			goto bail;
		}
	}
	assert(env != NULL);

	LOGW ("@@@@@@@@@@@@@   JNI_OnLoad   1111");

	if (register_vomeplayer(env) < 0) {
		JNILOGE("ERROR: vomeplayer native registration failed.");
		goto bail;
	}

	LOGW ("@@@@@@@@@@@@@   JNI_OnLoad   55555");

/* tmp
	if (register_vommretriever(env) < 0) {
		JNILOGE("ERROR: vommretriever native registration failed.");
		goto bail;
	}
	*/

	/* success -- return valid version number */
	result = jniver;

bail:
	return result;
}
#if defined __cplusplus
}
#endif

