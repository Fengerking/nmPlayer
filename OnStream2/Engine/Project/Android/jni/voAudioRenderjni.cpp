/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved
 
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
 * jni interface for voAudioRender.
 *
 * 
 *
 *
 * @author  Huan-Chih Tsai
 * @date    2013-09-07
 ************************************************************************/
#include <jni.h>
#include <android/log.h>
#include "IAudioDSPClock.h"
#include "CJniEnvUtil.h"
#include "voLog.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "voAudioRenderJni"

#define AUDIOFMT_CLSPATH      "android/media/AudioFormat" 
#define AUDIOTRACK_CLSPATH    "android/media/AudioTrack" 
#define VOONSTREAMSDK_CLSPATH "com/visualon/OSMPEngine/voOnStreamSDK"
#define VOOSTYPE_CLSPATH      "com/visualon/OSMPUtils/voOSType"

using libvoOSEng::CJniEnvUtil;

/***************************************************************************
 *  Local type declaration
 **************************************************************************/
enum NativeAudioFormat {
#ifdef F
#undef F
#endif
#define F(f1, f2) NATF_##f1,
#include "voAudioFmtTbl.h"
    NATF_NUM
};

static const char* NativeAudioFormatName_s[NATF_NUM+1] = 
{
#ifdef F
#undef F
#endif
#define F(f1, f2) "ENCODING_"f2,
#include "voAudioFmtTbl.h"
        ""
};

class NativeAudioRender : public IAudioDSPClock
{
    public:
        NativeAudioRender(JNIEnv*, jobject);
        ~NativeAudioRender();

        bool setJavaAudioTrack(jobject);

        bool isValid() const;
        bool isAudioFormatSupported(NativeAudioFormat) const;
        int  getAudioFormat(int renderFormat, int sampleBit) const;
        long getDSPTimeStamp() const;

    private:
        void checkJavaException(JNIEnv*, bool) const;

        JavaVM*   mJavaVM;
        int       mRenderFmtAC3;
        int       mRenderFmtEAC3;
        int       mAudioFmt[NATF_NUM];
        long      mDSPAudioClockPID;
        jobject   mJavaAudioTrack;
        jmethodID mGetTimestampMID;
};

/***************************************************************************
 *  class NativeAudioRender definition
 **************************************************************************/
NativeAudioRender::NativeAudioRender(JNIEnv* env, jobject obj) 
                  : mRenderFmtAC3(-1)
                  , mRenderFmtEAC3(-1)
                  , mDSPAudioClockPID(-1)
                  , mJavaAudioTrack(0)
                  , mGetTimestampMID(0)
{
    env->GetJavaVM(&mJavaVM);
    jclass cls = env->FindClass(AUDIOFMT_CLSPATH);

    if (cls == NULL) { // should not happen
	VOLOGE("Cannot find Java class %s", AUDIOFMT_CLSPATH);
        for (unsigned i=0; i<NATF_NUM; ++i) mAudioFmt[i] = -1;
        return; 
    }

    bool checkGetTimestamp = false;
    for (unsigned i=0; i<NATF_NUM; ++i) {
        jfieldID fid = 
        env->GetStaticFieldID(cls, NativeAudioFormatName_s[i], "I"); 
        if (fid == 0) {
            checkJavaException(env, true);
            mAudioFmt[i] = -1;
        }
        else {
            checkGetTimestamp |= (i >= NATF_PCM16_DIRECT);
            mAudioFmt[i] = env->GetStaticIntField(cls, fid);
        }
	VOLOGI("%s is %d", NativeAudioFormatName_s[i], mAudioFmt[i]);
    }

    // Cache JNI fieldID and methodID
    {
        cls = env->GetObjectClass(obj);
        jfieldID fid = env->GetStaticFieldID(cls, "RENDER_FORMAT_AC3", "I");
        mRenderFmtAC3 = env->GetStaticIntField(cls, fid);
        fid = env->GetStaticFieldID(cls, "RENDER_FORMAT_EAC3", "I");
        mRenderFmtEAC3 = env->GetStaticIntField(cls, fid);
    }

    if (checkGetTimestamp == true) { // find getTimeStamp() private API
        cls = env->FindClass(AUDIOTRACK_CLSPATH);
        if (cls == NULL) { // should not happen
            VOLOGE("Cannot find Java class %s", AUDIOTRACK_CLSPATH);
            return; 
        }

        mGetTimestampMID = env->GetMethodID(cls, "native_get_timestamp", "()J");
        if (mGetTimestampMID == 0) {
            checkJavaException(env, true);
            mGetTimestampMID = env->GetMethodID(cls, "getTimestamp", "()J");
            if (mGetTimestampMID == 0) 
                checkJavaException(env, true);
        }
    }
    VOLOGI("DSP timestamp is %ssupported", (mGetTimestampMID == 0) ? "not " : "");
}

NativeAudioRender::~NativeAudioRender()
{
    CJniEnvUtil envUtil(mJavaVM);
    JNIEnv* env = envUtil.getEnv();

    if (mJavaAudioTrack != 0) {
        env->DeleteGlobalRef(mJavaAudioTrack);
        mJavaAudioTrack = 0;
    }
}

bool NativeAudioRender::setJavaAudioTrack(jobject obj)
{
    CJniEnvUtil envUtil(mJavaVM);
    JNIEnv* env = envUtil.getEnv();
    if (mJavaAudioTrack != 0) {
        env->DeleteGlobalRef(mJavaAudioTrack);
        mJavaAudioTrack = 0;
    }
    if (obj != 0) {
        VOLOGI("setJavaAudioTrack with object (mGetTimestampMID is %s)", (mGetTimestampMID == 0) ? "0" : "not 0");
        mJavaAudioTrack = env->NewGlobalRef(obj);
        return (mGetTimestampMID != 0);
    }
    VOLOGI("setJavaAudioTrack without object");
    return false;
}

bool NativeAudioRender::isValid() const
{
    for (unsigned i=0; i<NATF_NUM; ++i) 
        if (mAudioFmt[i] != -1) return true;
    return false;
} 

bool NativeAudioRender::isAudioFormatSupported(NativeAudioFormat fmt) const
{
    return (mAudioFmt[fmt] != -1);
}

int  NativeAudioRender::getAudioFormat(int renderFormat, int sampleBit) const
{
    if (renderFormat == mRenderFmtAC3 && mAudioFmt[NATF_AC3] != -1)
        return mAudioFmt[NATF_AC3];

    if (renderFormat == mRenderFmtEAC3 && mAudioFmt[NATF_EAC3] != -1)
        return mAudioFmt[NATF_EAC3];

    VOLOGI("getAudioFormat: renderFormat %x sampleBit %d", renderFormat, sampleBit);

    // use PCM format, consider sampleBit size
    switch (sampleBit) {
        case 8:
            return mAudioFmt[NATF_PCM8];
        case 16:
            if (mAudioFmt[NATF_PCM16_DIRECT] != -1) {
                VOLOGI("Return AudioFormat %d", mAudioFmt[NATF_PCM16_DIRECT]);
                return mAudioFmt[NATF_PCM16_DIRECT]; 
            }
            break;
        case 24:
            if (mAudioFmt[NATF_PCM24_DIRECT] != -1) {
                VOLOGI("Return AudioFormat %d", mAudioFmt[NATF_PCM24_DIRECT]);
                return mAudioFmt[NATF_PCM24_DIRECT];
            }
            break;
        default: break;
    }
    VOLOGI("Return AudioFormat %d", mAudioFmt[NATF_PCM16]);
    return mAudioFmt[NATF_PCM16];
}

long NativeAudioRender::getDSPTimeStamp() const
{
    if (mJavaAudioTrack != 0 && mGetTimestampMID != 0) {
        CJniEnvUtil envUtil(mJavaVM);
        JNIEnv* env = envUtil.getEnv();
#if 0
        long t = env->CallLongMethod(mJavaAudioTrack, mGetTimestampMID)/1000;
        VOLOGI("DSP TimeStamp %ld", t);
        return t;
#else
        return (env->CallLongMethod(mJavaAudioTrack, mGetTimestampMID)/1000);
#endif
    }
    return 0;
}

void NativeAudioRender::checkJavaException(JNIEnv* env, bool clear) const
{
    if (env->ExceptionCheck()) {
        //env->ExceptionDescribe();
        if (clear == true)
            env->ExceptionClear();
    }
}

#if defined __cplusplus 
extern "C" {
#endif

/***************************************************************************
 *  JNI
 **************************************************************************/
/*
 * Class:     com_visualon_OSMPEngine_voAudioRender
 * Method:    nativeInit
 * Signature: ()V
 */ 
JNIEXPORT void JNICALL
Java_com_visualon_OSMPEngine_voAudioRender_nativeInit(JNIEnv *env, jobject obj)
{
    // create native context and store it in Java object
    NativeAudioRender* ctx = new NativeAudioRender(env, obj);
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls, "mNativeContext" , "J");
    if (ctx->isValid() == false) {
        delete ctx;
        env->SetLongField(obj, fid, 0);
    }
    else
        env->SetLongField(obj, fid, (long)(ctx));
}

/*
 * Class:     com_visualon_OSMPEngine_voAudioRender
 * Method:    nativeUninit
 * Signature: ()V
 */ 
JNIEXPORT void JNICALL
Java_com_visualon_OSMPEngine_voAudioRender_nativeUninit(JNIEnv* env, jobject obj)
{
    jclass cls = env->GetObjectClass(obj);
    jfieldID fid = env->GetFieldID(cls, "mNativeContext" , "J");
    NativeAudioRender* ctx = (NativeAudioRender*)env->GetLongField(obj, fid);
    if (ctx != NULL) {
        delete ctx;
        env->SetLongField(obj, fid, 0);
    }
}

/*
 * Class:     com_visualon_OSMPEngine_voAudioRender
 * Method:    nativeSetAudioTrack
 * Signature: (JLandroid/media/AudioTrack;)Z
 */ 
JNIEXPORT jboolean JNICALL
Java_com_visualon_OSMPEngine_voAudioRender_nativeSetAudioTrack(JNIEnv* env, jobject obj, jlong context, jobject track)
{
    if (context == 0) return JNI_FALSE;
    NativeAudioRender* nar = (NativeAudioRender*)(context);
    return (nar->setJavaAudioTrack(track)) ? JNI_TRUE : JNI_FALSE;
}

/*
 * Class:     com_visualon_OSMPEngine_voAudioRender
 * Method:    nativeGetAudioFormat
 * Signature: (JII)I
 */ 
JNIEXPORT jint JNICALL
Java_com_visualon_OSMPEngine_voAudioRender_nativeGetAudioFormat(JNIEnv* env, jobject obj, jlong context, jint renderFormat, jint sampleBit)
{
    if (context != 0) {
        NativeAudioRender* nar = (NativeAudioRender*)(context);
        return nar->getAudioFormat(renderFormat, sampleBit);
    }

    // No native context. Cannot determine AudioFormat 
    // Leave it to Java level to decide
    return -1;
}

#if defined __cplusplus
}
#endif


