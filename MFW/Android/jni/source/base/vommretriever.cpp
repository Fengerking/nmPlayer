/*
**
** Copyright 2008, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "vommretriever"
#define VO_JNI_EXT_MODE 1

#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
#include <utils/Parcel.h>
#elif defined __VONJ_ECLAIR__
#include <binder/Parcel.h>
#endif

#include <assert.h>
#include <utils/Log.h>
#include <utils/threads.h>
#include <core/SkBitmap.h>
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include "CVOMEMetadataRetriever.h"
#include "bitmap.h"
#include "vojnilog.h" 



#define VOINFO(format, ...) { \
	LOGI("[david] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }

using namespace android;

struct fields_t {
    jfieldID context;
    jclass bitmapClazz;
    jmethodID bitmapConstructor;
};

static fields_t fields;
static Mutex sLock;
static const char* const g_szclsvommretriever = "visualon/vome/vommretriever";

static int   sDruation;
static int   sWidth;
static int   sHeight;

#if defined __cplusplus
extern "C" {
#endif

static void process_media_retriever_call(JNIEnv *env, status_t opStatus, const char* exception, const char *message)
{
    if (opStatus == (status_t) INVALID_OPERATION) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
    } else if (opStatus != (status_t) OK) {
        if (strlen(message) > 230) {
            // If the message is too long, don't bother displaying the status code.
            jniThrowException( env, exception, message);
        } else {
            char msg[256];
            // Append the status code to the message.
            sprintf(msg, "%s: status = 0x%X", message, opStatus);
            jniThrowException( env, exception, msg);
        }
    }
}

static CVOMEMetadataRetriever* getRetriever(JNIEnv* env, jobject thiz)
{
    // No lock is needed, since it is called internally by other methods that are protected
    CVOMEMetadataRetriever* retriever = (CVOMEMetadataRetriever*) env->GetIntField(thiz, fields.context);
    return retriever;
}

static void setRetriever(JNIEnv* env, jobject thiz, int retriever)
{
    // No lock is needed, since it is called internally by other methods that are protected
    //CVOMEMetadataRetriever *old = (CVOMEMetadataRetriever*) env->GetIntField(thiz, fields.context);
    env->SetIntField(thiz, fields.context, retriever);
}

static void vomm_retriever_setDataSource(JNIEnv *env, jobject thiz, jstring path)
{
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return;
    }
    if (!path) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Null pointer");
        return;
    }

    const char *pathStr = env->GetStringUTFChars(path, NULL);
    if (!pathStr) {  // OutOfMemoryError exception already thrown
        return;
    }

      

    VOINFO("filename: %s", pathStr);
    // Don't let somebody trick us in to reading some random block of memory
    if (strncmp("mem://", pathStr, 6) == 0) {
        jniThrowException(env, "java/lang/IllegalArgumentException", "Invalid pathname");
        return;
    }

    process_media_retriever_call(env, retriever->setDataSource(pathStr), "java/lang/RuntimeException", "setDataSource failed");
    env->ReleaseStringUTFChars(path, pathStr);
}

static void vomm_retriever_setDataSourceFD(JNIEnv *env, jobject thiz, jobject fileDescriptor, jlong offset, jlong length)
{
    LOGV("setDataSource");
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return;
    }
    if (!fileDescriptor) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    int fd = getParcelFileDescriptorFD(env, fileDescriptor);
    if (offset < 0 || length < 0 || fd < 0) {
        if (offset < 0) {
            LOGE("negative offset (%lld)", offset);
        }
        if (length < 0) {
            LOGE("negative length (%lld)", length);
        }
        if (fd < 0) {
            LOGE("invalid file descriptor");
        }
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }
    process_media_retriever_call(env, retriever->setDataSource(fd, offset, length), "java/lang/RuntimeException", "setDataSource failed");
}

static void vomm_retriever_setMode(JNIEnv *env, jobject thiz, jint mode)
{
    LOGV("setMode");
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return;
    }
    process_media_retriever_call(env, retriever->setMode(mode), "java/lang/RuntimeException", "setMode failed");
}

static int vomm_retriever_getMode(JNIEnv *env, jobject thiz)
{
    LOGV("getMode");
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return -1;  // Error
    }
    int mode = -1;
    retriever->getMode(&mode);
    return mode;
}

static jobject vomm_retriever_captureFrame(JNIEnv *env, jobject thiz)
{	
  return NULL;
 JNILOGV( "captureFrame : V38");
	
    Mutex::Autolock lock(sLock);

    int nWidth, nHeight, nDataSize;

    nWidth     = 160;
    nHeight    = 120;
    nDataSize = nWidth * nHeight * 2;
    	    	
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }

        // Create a SkBitmap to hold the pixels
    SkBitmap *bitmap = new SkBitmap();
    if (bitmap == NULL) {
        JNILOGE("captureFrame: cannot instantiate a SkBitmap object.");
        return NULL;
    }
    bitmap->setConfig(SkBitmap::kRGB_565_Config, nWidth, nHeight);
    if (!bitmap->allocPixels()) {
        delete bitmap;
        JNILOGE("failed to allocate pixel buffer");
        return NULL;
    }
    
	uint32_t nDuration = 0;
	uint32_t nOrgWidth = 0;
	uint32_t nOrgHeight = 0;
	
      VideoFrame *videoFrame = retriever->captureFrame2(nWidth, nHeight, (uint8_t*)bitmap->getPixels(), &nDuration, &nOrgWidth, &nOrgHeight);
    	sDruation = (int)nDuration;
    
    if (videoFrame == NULL) {

    	   JNILOGE("captureFrame: videoFrame is a NULL pointer");

    	   if (bitmap) {
    	   	delete bitmap;
    	   	bitmap = NULL;
    	   }
    	   	
          return NULL;
         
    		          
    }

    
    char szLog[256];
    sprintf( szLog, "DispWidth %d, Dispheight %d, Size %d", 
       videoFrame->mDisplayWidth, videoFrame->mDisplayHeight, videoFrame->mSize);
    JNILOGI2("captureFrame: %s", szLog);      

   
    return env->NewObject(fields.bitmapClazz, fields.bitmapConstructor, (int) bitmap, true, NULL);	
    
    /*
    JNILOGI( "captureFrame : V36");
	
    Mutex::Autolock lock(sLock);
    	    	
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }

    VideoFrame *videoFrame = retriever->captureFrame();
    if (videoFrame == NULL) {
    	   JNILOGE("captureFrame: videoFrame is a NULL pointer");
          return NULL;
    }
    else
         JNILOGI("pass get VideoFrame");    
    
    if (videoFrame == NULL) {    	
        JNILOGE("captureFrame: videoFrame is a NULL pointer");
        return NULL;
    }

    // Create a SkBitmap to hold the pixels
    SkBitmap *bitmap = new SkBitmap();
    if (bitmap == NULL) {
        JNILOGE("captureFrame: cannot instantiate a SkBitmap object.");
        return NULL;
    }
    bitmap->setConfig(SkBitmap::kRGB_565_Config, videoFrame->mDisplayWidth, videoFrame->mDisplayHeight);
    if (!bitmap->allocPixels()) {
        delete bitmap;
        JNILOGE("failed to allocate pixel buffer");
        return NULL;
    }
    
    char szLog[256];
    sprintf( szLog, "DispWidth %d, Dispheight %d, Size %d", 
       videoFrame->mDisplayWidth, videoFrame->mDisplayHeight, videoFrame->mSize);
    JNILOGI2("captureFrame: %s", szLog);      

     memcpy((uint8_t*)bitmap->getPixels(), videoFrame->mData , videoFrame->mSize);
   
    return env->NewObject(fields.bitmapClazz, fields.bitmapConstructor, (int) bitmap, true, NULL);	
    //*/
}




static jobject vomm_retriever_captureFrame2(JNIEnv *env, jobject thiz, jint in_nWidth, jint in_nHeight)
{	
  
   JNILOGV( "captureFrame2 : V39");
	
    Mutex::Autolock lock(sLock);


    int nWidth, nHeight, nDataSize;

    nWidth     = in_nWidth;
    nHeight    = in_nHeight;

  

    nDataSize = nWidth * nHeight * 2;
    	    	
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        sDruation = 0;
        sWidth = 0;
        sHeight = 0;
        return NULL;
    }

        // Create a SkBitmap to hold the pixels
    SkBitmap *bitmap = new SkBitmap();
    if (bitmap == NULL) {
        JNILOGE("captureFrame: cannot instantiate a SkBitmap object.");
        sDruation = 0;
        sWidth = 0;
        sHeight = 0;
        return NULL;
    }
    bitmap->setConfig(SkBitmap::kRGB_565_Config, nWidth, nHeight);
    if (!bitmap->allocPixels()) {
        delete bitmap;
        JNILOGE("failed to allocate pixel buffer");
        sDruation = 0;
           sWidth = 0;
        sHeight = 0;
        return NULL;
    }
    
    uint32_t nDuration = 0;
    uint32_t nOrgWidth     = 0;
    uint32_t nOrgHeight    = 0;
    
    
    VideoFrame *videoFrame = retriever->captureFrame2(nWidth, nHeight, (uint8_t*)bitmap->getPixels(), &nDuration, &nOrgWidth , &nOrgHeight);
    sDruation = nDuration;
    sWidth     = nOrgWidth;
    sHeight    = nOrgHeight;

//*
       JNILOGI("BEFORE GET DATA");
    retriever->extractMetadata();
    JNILOGI("AFTER GET DATA");
    
    sDruation = retriever->m_nDuration;
    sWidth     = retriever->m_nWidth;
    sHeight    = retriever->m_nHeight;
//    */

	
    if (videoFrame == NULL) {

    	   JNILOGE("captureFrame: videoFrame is a NULL pointer");

    	   if (bitmap) {
    	   	delete bitmap;
    	   	bitmap = NULL;
    	   }
    	   	
          return NULL;
        
    		          
    }


    char szLog[256];
    sprintf( szLog, "DispWidth %d, Dispheight %d, Size %d", 
       videoFrame->mDisplayWidth, videoFrame->mDisplayHeight, videoFrame->mSize);
    JNILOGI2("captureFrame: %s", szLog);      

   
    return env->NewObject(fields.bitmapClazz, fields.bitmapConstructor, (int) bitmap, true, NULL);	
   

}

static int vomm_retriever_getDuration(JNIEnv *env, jobject thiz)
{
   JNILOGI2("Return Duration : %d", sDruation);
    return sDruation;
}

static int vomm_retriever_getWidth(JNIEnv *env, jobject thiz)
{
   JNILOGI2("Return Width : %d", sWidth);
    return sWidth;
}

static int vomm_retriever_getHeight(JNIEnv *env, jobject thiz)
{
    
   JNILOGI2("Return Height : %d", sHeight);
    return sHeight;
}
	
static jbyteArray vomm_retriever_extractAlbumArt(JNIEnv *env, jobject thiz)
{
    LOGV("extractAlbumArt");
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }
    MediaAlbumArt* mediaAlbumArt = retriever->extractAlbumArt();
    if (mediaAlbumArt == NULL) {
        LOGE("extractAlbumArt: Call to extractAlbumArt failed.");
        return NULL;
    }

    unsigned int len = mediaAlbumArt->mSize;
    char* data = (char*) mediaAlbumArt + sizeof(MediaAlbumArt);
    jbyteArray array = env->NewByteArray(len);
    if (!array) {  // OutOfMemoryError exception has already been thrown.
        LOGE("extractAlbumArt: OutOfMemoryError is thrown.");
    } else {
        jbyte* bytes = env->GetByteArrayElements(array, NULL);
        if (bytes != NULL) {
            memcpy(bytes, data, len);
            env->ReleaseByteArrayElements(array, bytes, 0);
        }
    }

    // No need to delete mediaAlbumArt here
    return array;
}

static jobject vomm_retriever_extractMetadata(JNIEnv *env, jobject thiz, jint keyCode)
{
    LOGV("extractMetadata");
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }
    const char* value = retriever->extractMetadata(keyCode);
    if (!value) {
        LOGV("extractMetadata: Metadata is not found");
        return NULL;
    }
    LOGV("extractMetadata: value (%s) for keyCode(%d)", value, keyCode);
    return env->NewStringUTF(value);
}


static jobject vomm_retriever_getVideoFormat(JNIEnv *env, jobject thiz)
{
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }
    
    return env->NewStringUTF(retriever->m_szVideoFormat);
}

static jobject vomm_retriever_getAudioFormat(JNIEnv *env, jobject thiz)
{
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }
    
    return env->NewStringUTF(retriever->m_szAudioFormat);
}

static jobject vomm_retriever_getVideoFPS(JNIEnv *env, jobject thiz)
{
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    if (retriever == 0) {
        jniThrowException(env, "java/lang/IllegalStateException", "No retriever available");
        return NULL;
    }
    
    return env->NewStringUTF(retriever->m_szFPS);
}

static void vomm_retriever_release(JNIEnv *env, jobject thiz)
{
    LOGV("release");
    Mutex::Autolock lock(sLock);
    CVOMEMetadataRetriever* retriever = getRetriever(env, thiz);
    delete retriever;
    setRetriever(env, thiz, 0);
}

static void vomm_retriever_native_finalize(JNIEnv *env, jobject thiz)
{
    LOGV("native_finalize");
    
    // No lock is needed, since vomm_retriever_release() is protected
    vomm_retriever_release(env, thiz);
}

// This function gets a field ID, which in turn causes class initialization.
// It is called from a static block in CVOMEMetadataRetriever, which won't run until the
// first time an instance of this class is used.
static void vomm_retriever_native_init(JNIEnv *env)
{

    jclass clazz = env->FindClass(g_szclsvommretriever);
    if (clazz == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find com/visualon/vome/vommretriever");
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (fields.context == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find vommretriever.mNativeContext");
        return;
    }


    fields.bitmapClazz = env->FindClass("android/graphics/Bitmap");
    if (fields.bitmapClazz == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find android/graphics/Bitmap");
        return;
    }

    sDruation = 0;
    sWidth = 0;
    sHeight = 0;
		VOINFO("%s", "gonna register vommretriever...");
  //  fields.bitmapConstructor = env->GetMethodID(fields.bitmapClazz, "<init>", "(IZ[BI)V");


#if defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
    fields.bitmapConstructor = env->GetMethodID(fields.bitmapClazz, "<init>", "(IZ[B)V");
#else
    fields.bitmapConstructor = env->GetMethodID(fields.bitmapClazz, "<init>", "(IZ[BI)V");
#endif
       
    if (fields.bitmapConstructor == NULL) {
        	VOINFO("%s", "failed to init constructor");
        return;
    }
    else {
    	VOINFO("%s", "succeed to init constructor");
    }
    	   
   /*fields.bitmapConstructor = env->GetStaticMethodID(fields.bitmapClazz, "createBitmap", "([IIII)Ljava/lang/Object");
    if (fields.bitmapConstructor == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find Bitmap constructor");
        return;
    }*/
		VOINFO("%s", "succeed to register vommretriever...");
}

static void vomm_retriever_native_setup(JNIEnv *env, jobject thiz)
{
    LOGV("native_setup");
    CVOMEMetadataRetriever* retriever = new CVOMEMetadataRetriever();
    if (retriever == 0) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }
    setRetriever(env, thiz, (int)retriever);
}

// JNI mapping between Java methods and native methods
static JNINativeMethod nativeMethods[] = {
        {"setDataSource",   "(Ljava/lang/String;)V", (void *)vomm_retriever_setDataSource},
        {"setDataSource",   "(Ljava/io/FileDescriptor;JJ)V", (void *)vomm_retriever_setDataSourceFD},
        {"setMode",         "(I)V", (void *)vomm_retriever_setMode},
        {"getMode",         "()I",  (void *)vomm_retriever_getMode},
        {"captureFrame",    "()Landroid/graphics/Bitmap;", (void *)vomm_retriever_captureFrame},
        {"captureFrame2",    "(II)Landroid/graphics/Bitmap;", (void *)vomm_retriever_captureFrame2},
        {"getDuration",         "()I",  (void *)vomm_retriever_getDuration},
        {"getWidth",         "()I",  (void *)vomm_retriever_getWidth},
       {"getHeight",         "()I",  (void *)vomm_retriever_getHeight},
        {"extractMetadata", "(I)Ljava/lang/String;", (void *)vomm_retriever_extractMetadata},
        {"extractAlbumArt", "()[B", (void *)vomm_retriever_extractAlbumArt},
        {"release",         "()V", (void *)vomm_retriever_release},
        {"native_finalize", "()V", (void *)vomm_retriever_native_finalize},
        {"native_setup",    "()V", (void *)vomm_retriever_native_setup},
        {"native_init",     "()V", (void *)vomm_retriever_native_init},
        {"getVideoFormat", "()Ljava/lang/String;", (void *)vomm_retriever_getVideoFormat},
	 {"getAudioFormat", "()Ljava/lang/String;", (void *)vomm_retriever_getAudioFormat},
      	 {"getVideoFPS", "()Ljava/lang/String;", (void *)vomm_retriever_getVideoFPS},
        
};

// This function only registers the native methods, and is called from
// JNI_OnLoad in android_media_MediaPlayer.cpp

int register_vommretriever(JNIEnv *env)
{
    //return 0;
    return AndroidRuntime::registerNativeMethods(env, g_szclsvommretriever, nativeMethods, NELEM(nativeMethods));
}
#if defined __cplusplus
}
#endif

