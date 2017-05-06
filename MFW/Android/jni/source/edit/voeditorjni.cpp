
#define LOG_TAG "voeditorjni"

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
#include "voeditor.h"
#include "vojnilog.h"
#include "CVOWEditor.h"


using namespace android;


struct fields_t {
    jfieldID    context;
    jfieldID    surface;
    jfieldID    surface_native;
    jmethodID   post_event;
};

static fields_t fields;
static voCMutex g_mautolock;

// ref-counted object for callbacks
class JNIvoeditorListener: public voeditorListener
{
public:
    JNIvoeditorListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIvoeditorListener();
    void notify(int msg, int ext1, int ext2);
private:
    JNIvoeditorListener();
    jclass      mClass;     // Reference to voeditor class
    jobject     mObject;    // Weak ref to voeditor Java object to call on
};

JNIvoeditorListener::JNIvoeditorListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{
    // Hold onto the voeditor class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        JNILOGE("Can't find com/visualon/voeditor");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }
    
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the voeditor object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);
}

JNIvoeditorListener::~JNIvoeditorListener()
{
    // remove global references
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);
}

void JNIvoeditorListener::notify(int msg, int ext1, int ext2)
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

static sp<voeditor> getvoeditor(JNIEnv* env, jobject thiz)
{	
    voCAutoLock l(&g_mautolock);
    voeditor* const p = (voeditor*)env->GetIntField(thiz, fields.context);
    return sp<voeditor>(p);
}

static sp<voeditor> setvoeditor(JNIEnv* env, jobject thiz, const sp<voeditor>& editor)
{

    voCAutoLock l(&g_mautolock);
    sp<voeditor> old = (voeditor*)env->GetIntField(thiz, fields.context);
    if (editor.get()) {
        editor->incStrong(thiz);
    }
    if (old != 0) {
        old->decStrong(thiz);
    }
    env->SetIntField(thiz, fields.context, (int)editor.get());
    return old;
}

// If exception is NULL and opStatus is not OK, this method sends an error
// event to the client application; otherwise, if exception is not NULL and
// opStatus is not OK, this method throws the given exception to the client
// application.
static void process_media_player_call(JNIEnv *env, jobject thiz, status_t opStatus, const char* exception, const char *message)
{
   return;
    if (exception == NULL) {  // Don't throw exception. Instead, send an event.
        if (opStatus != (status_t) OK) {
            sp<voeditor> mp = getvoeditor(env, thiz);
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
voeditorsetDataSource(JNIEnv *env, jobject thiz, jstring path)
{
   JNILOGI("SetDataSource");
   sp<voeditor> editor = getvoeditor(env, thiz);
    if (editor == NULL ) {
    	 JNILOGE("editor is null");
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

    status_t opStatus = editor->setDataSource(pathStr);

    // Make sure that local ref is released before a potential exception
    env->ReleaseStringUTFChars(path, pathStr);
    process_media_player_call( env, thiz, opStatus, "java/io/IOException", "setDataSource failed." );

	editor->ExportFile("/sdcard/dump.3gp");
    
/*
	JNILOGI("before voeditorsetDataSource v7");
	
	 const char *pathStr = env->GetStringUTFChars(path, NULL);
	  
	int nRC = 0;
	CVOWEditor *player = new CVOWEditor();

	if (player->Init () < 0)
		return ;


	nRC = player->SetSource (pathStr);


  
	if (nRC < 0)
	{
			JNILOGE("failed to voeditorsetDataSource");
			player->Uninit ();
			return ;
	}



	nRC = player->ExportFile();
	player->Run();
 // sleep(60);
  JNILOGI("end set datasource");
	return ;
	*/
	   
	   /*
  */
}

static void
voeditorsetDataSourceFD(JNIEnv *env, jobject thiz, jobject fileDescriptor, jlong offset, jlong length)
{
	   return;
    sp<voeditor> mp = getvoeditor(env, thiz);
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

static void setVideoSurface(const sp<voeditor>& mp, JNIEnv *env, jobject thiz)
{
	  return;
    jobject surface = env->GetObjectField(thiz, fields.surface);
    if (surface != NULL) {
        const sp<Surface> native_surface = get_surface(env, surface);
        LOGV("prepare: surface=%p (id=%d)",
             native_surface.get(), native_surface->ID());
        mp->setVideoSurface(native_surface);
    }
}

static void
voeditorsetVideoSurface(JNIEnv *env, jobject thiz)
{
	  
	  return;
	  JNILOGI("Set Video Surface");
	  
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    setVideoSurface(mp, env, thiz);
}

static void
voeditorprepare(JNIEnv *env, jobject thiz)
{
	return;
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    setVideoSurface(mp, env, thiz);
    process_media_player_call( env, thiz, mp->prepare(), "java/io/IOException", "Prepare failed." );
}

static void
voeditorprepareAsync(JNIEnv *env, jobject thiz)
{
	  return;
    sp<voeditor> mp = getvoeditor(env, thiz);
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
voeditorstart(JNIEnv *env, jobject thiz)
{
	 return;
	JNILOGI2("%s", "start...");
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
		mp->start();
    //process_media_player_call(env, thiz, mp->start(), NULL, NULL );
}

static void
voeditorstop(JNIEnv *env, jobject thiz)
{
	  return;
    JNILOGI2("%s\n", "stop...");
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->stop(), NULL, NULL );
}

static void
voeditorpause(JNIEnv *env, jobject thiz)
{
	  return;
    JNILOGI2("%s", "pause");
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
		mp->pause();
    //process_media_player_call( env, thiz, mp->pause(), NULL, NULL );
}

static jboolean
voeditorisPlaying(JNIEnv *env, jobject thiz)
{
	  return true;
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    const jboolean is_playing = mp->isPlaying();

    LOGV("isPlaying: %d", is_playing);
    return is_playing;
}

static void
voeditorseekTo(JNIEnv *env, jobject thiz, int msec)
{
	  return;
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    LOGV("seekTo: %d(msec)", msec);
    process_media_player_call( env, thiz, mp->seekTo(msec), NULL, NULL );
}

static int
voeditorgetVideoWidth(JNIEnv *env, jobject thiz)
{
	   return 0;
    sp<voeditor> mp = getvoeditor(env, thiz);
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
voeditorgetVideoHeight(JNIEnv *env, jobject thiz)
{
	  return 0;
    sp<voeditor> mp = getvoeditor(env, thiz);
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
voeditorgetCurrentPosition(JNIEnv *env, jobject thiz)
{
	  return 0;
    sp<voeditor> mp = getvoeditor(env, thiz);
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
voeditorgetDuration(JNIEnv *env, jobject thiz)
{
	  return 0;
    sp<voeditor> mp = getvoeditor(env, thiz);
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
voeditorreset(JNIEnv *env, jobject thiz)
{
	   return;
    LOGV("reset");
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->reset(), NULL, NULL );
}

static void
voeditorsetAudioStreamType(JNIEnv *env, jobject thiz, int streamtype)
{
	  return;
    LOGV("setAudioStreamType: %d", streamtype);
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setAudioStreamType(streamtype) , NULL, NULL );
}

static void
voeditorsetLooping(JNIEnv *env, jobject thiz, jboolean looping)
{
	 return;
    LOGV("setLooping: %d", looping);
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setLooping(looping), NULL, NULL );
}

static jboolean
voeditorisLooping(JNIEnv *env, jobject thiz)
{
	   return true;
    LOGV("isLooping");
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    return mp->isLooping();
}

static void
voeditorsetVolume(JNIEnv *env, jobject thiz, float leftVolume, float rightVolume)
{
	  return;
    LOGV("setVolume: left %f  right %f", leftVolume, rightVolume);
    sp<voeditor> mp = getvoeditor(env, thiz);
    if (mp == NULL ) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }
    process_media_player_call( env, thiz, mp->setVolume(leftVolume, rightVolume), NULL, NULL );
}

// FIXME: deprecated
static jobject
voeditorgetFrameAt(JNIEnv *env, jobject thiz, jint msec)
{
	  
    return NULL;
}


static jobject 
voeditorgetStoryBoardThumbnail(JNIEnv *env, jobject thiz, jint indexNumber, jint totalNumber)
{
	  sp<voeditor> myeditor = getvoeditor(env, thiz);
	  if (myeditor == NULL)
	  {
	  	  JNILOGE("myeditor is NULL");
	  	   jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return UNKNOWN_ERROR;
	  }
	  
	  
	  
    return ;
}

    	

// Sends the request and reply parcels to the media player via the
// binder interface.
static jint
voeditorinvoke(JNIEnv *env, jobject thiz,
                                 jobject java_request, jobject java_reply)
{

      return 0;
    sp<voeditor> media_player = getvoeditor(env, thiz);
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
voeditorsetMetadataFilter(JNIEnv *env, jobject thiz, jobject request)
{
	  return 0;
    sp<voeditor> media_player = getvoeditor(env, thiz);
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
voeditorgetMetadata(JNIEnv *env, jobject thiz, jboolean update_only,
                                      jboolean apply_filter, jobject reply)
{
	   return true;
    sp<voeditor> media_player = getvoeditor(env, thiz);
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
// It is called from a static block in voeditor, which won't run until the
// first time an instance of this class is used.
static const char* const g_szvoeditclspathname = "com/visualon/voeditor/voeditorjni";

static void voeditornative_init(JNIEnv *env) {


	uid_t uid = getuid();
	uid_t euid = geteuid();
	gid_t gid = getgid();
	gid_t egid = getegid();
	JNILOGI2("uid: %d, gid: %d, euid: %d, egid: %d\n", uid, gid, euid, egid);

	int fb = open("/dev/graphics/fb0", O_RDWR);
	if(fb < 0){
		JNILOGI2("%s", "Error opening /dev/graphics/fb0: %m. Check kernel config.");
	}

	struct fb_var_screeninfo vi;
	if (-1 == ioctl(fb,FBIOGET_VSCREENINFO,&vi)){
		JNILOGI2("%s", "ioctl FBIOGET_VSCREENINFO error.");
	}
	JNILOGI2("x: %d, y:%d, d:%d \n",vi.xres,vi.yres,vi.bits_per_pixel);

    jclass clazz;
    clazz = env->FindClass(g_szvoeditclspathname);
    if (clazz == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find com/visualon/vome/voeditor");
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (fields.context == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find voeditor.mNativeContext");
        return;
    }

    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;IIILjava/lang/Object;)V");
    if (fields.post_event == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find voeditor.postEventFromNative");
        return;
    }

    fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
    if (fields.surface == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find voeditor.mSurface");
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
}

static void
voeditornative_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
    sp<voeditor> editor = new voeditor();
    if (editor == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

    // create new listener and give it to voeditor
    sp<JNIvoeditorListener> listener = new JNIvoeditorListener(env, thiz, weak_this);
    editor->setListener(listener);

    // Stow our new C++ voeditor in an opaque field in the Java object.
    
    setvoeditor(env, thiz, editor);
    
    JNILOGI("after setvoeditor");
    
}

static void
voeditorrelease(JNIEnv *env, jobject thiz)
{
    JNILOGV("release");
    sp<voeditor> editor = setvoeditor(env, thiz, 0);
    if (editor != NULL) {
        // this prevents native callbacks after the object is released
        editor->setListener(0);
        editor->disconnect();
    }
}

static void
voeditornative_finalize(JNIEnv *env, jobject thiz)
{
    JNILOGV("native_finalize");
    voeditorrelease(env, thiz);
}

static jint
voeditorsnoop(JNIEnv* env, jobject thiz, jobject data, jint kind) {
    jshort* ar = (jshort*)env->GetPrimitiveArrayCritical((jarray)data, 0);
    jsize len = env->GetArrayLength((jarray)data);
    int ret = 0;
    if (ar) {
        ret = voeditor::snoop(ar, len, kind);
        env->ReleasePrimitiveArrayCritical((jarray)data, ar, 0);
    }
    return ret;
}

// ----------------------------------------------------------------------------

static JNINativeMethod g_voeditmethods[] = {
    {"setDataSource",       "(Ljava/lang/String;)V",            (void *)voeditorsetDataSource},
    {"setDataSource",       "(Ljava/io/FileDescriptor;JJ)V",    (void *)voeditorsetDataSourceFD},
    {"_setVideoSurface",    "()V",                              (void *)voeditorsetVideoSurface},
    {"prepare",             "()V",                              (void *)voeditorprepare},
    {"prepareAsync",        "()V",                              (void *)voeditorprepareAsync},
    {"_start",              "()V",                              (void *)voeditorstart},
    {"_stop",               "()V",                              (void *)voeditorstop},
    {"getVideoWidth",       "()I",                              (void *)voeditorgetVideoWidth},
    {"getVideoHeight",      "()I",                              (void *)voeditorgetVideoHeight},
    {"seekTo",              "(I)V",                             (void *)voeditorseekTo},
    {"_pause",              "()V",                              (void *)voeditorpause},
    {"isPlaying",           "()Z",                              (void *)voeditorisPlaying},
    {"getCurrentPosition",  "()I",                              (void *)voeditorgetCurrentPosition},
    {"getDuration",         "()I",                              (void *)voeditorgetDuration},
    {"_release",            "()V",                              (void *)voeditorrelease},
    {"_reset",              "()V",                              (void *)voeditorreset},
    {"setAudioStreamType",  "(I)V",                             (void *)voeditorsetAudioStreamType},
    {"setLooping",          "(Z)V",                             (void *)voeditorsetLooping},
    {"isLooping",           "()Z",                              (void *)voeditorisLooping},
    {"setVolume",           "(FF)V",                            (void *)voeditorsetVolume},
    {"getFrameAt",          "(I)Landroid/graphics/Bitmap;",     (void *)voeditorgetFrameAt},
    {"native_invoke",       "(Landroid/os/Parcel;Landroid/os/Parcel;)I",(void *)voeditorinvoke},
    {"native_setMetadataFilter", "(Landroid/os/Parcel;)I",      (void *)voeditorsetMetadataFilter},
    {"native_getMetadata", "(ZZLandroid/os/Parcel;)Z",          (void *)voeditorgetMetadata},
    {"native_init",         "()V",                              (void *)voeditornative_init},
    {"native_setup",        "(Ljava/lang/Object;)V",            (void *)voeditornative_setup},
    {"native_finalize",     "()V",                              (void *)voeditornative_finalize},
    {"snoop",               "([SI)I",                           (void *)voeditorsnoop},   
    {"getStoryBoardThumbnail",          "(II)Landroid/graphics/Bitmap",     (void *)voeditorgetStoryBoardThumbnail},
};


// This function only registers the native methods
static int register_voedit(JNIEnv *env) {

	jclass clazz;
       JNILOGI2("RegisterNatives for %s",  g_szvoeditclspathname);
	clazz = env->FindClass(g_szvoeditclspathname);
	if (clazz == NULL) {
		JNILOGI2("%s not found!", g_szvoeditclspathname);
		return JNI_FALSE;
	}

	if (env->RegisterNatives(clazz, g_voeditmethods, sizeof(g_voeditmethods) / sizeof(g_voeditmethods[0])) < 0) {
		JNILOGI2("RegisterNatives failed: '%s' ", g_szvoeditclspathname);
		return JNI_FALSE;
	}
	
  JNILOGI2("Succeed to register %s.",  g_szvoeditclspathname);
	return JNI_TRUE;
}



jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNILOGI("Editor OnLoad");

	JNIEnv* env = NULL;
	jint result = -1;

	jint jniver = JNI_VERSION_1_4;
	if (vm->GetEnv((void**) &env, jniver) != JNI_OK) {
		jniver = JNI_VERSION_1_6;
		if (vm->GetEnv((void**)&env, jniver) != JNI_OK) {
			JNILOGE("ERROR: GetEnv failed\n");
			goto bail;
		}
	}
	assert(env != NULL);

	if (register_voedit(env) < 0) {
		JNILOGE("ERROR: voeditor native registration failed.");
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

