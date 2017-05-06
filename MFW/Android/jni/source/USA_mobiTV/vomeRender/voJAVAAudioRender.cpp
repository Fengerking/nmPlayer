 
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voJAVAAudioRender.cpp

	Contains:	voJAVAAudioRender class file

	Written by:	Tom Yu Wei

	Change History (most recent first):
	2010-08-31		Tom Yu			Create file

*******************************************************************************/

#define LOG_TAG "VOJAVAAudioRender"

#include <android_runtime/AndroidRuntime.h>

#include "voJAVAAudioRender.h"
#include "voOMXOSFun.h"
#include "voOMXMemory.h"
#include "voLog.h"

namespace android {

VOJAVAAudioRender::VOJAVAAudioRender()
	: mClass(NULL)
	, mMethodOpen(NULL)
	, mMethodStart(NULL)
	, mMethodWrite(NULL)
	, mMethodStop(NULL)
	, mMethodFlush(NULL)
	, mMethodPause(NULL)
	, mMethodClose(NULL)
	, mMethodSetVolume(NULL)
	, mRD(NULL)
	, mLeftVolume(1.0)
	, mRightVolume(1.0)
	, m_bThreadEventStop(false)
	, m_bThreadExit(false)
	, m_bStoped(true)
	, m_nCmdEvent (0)
	, m_nBufferSize(0)
{	
	VOLOGI("");

	Init();
}

VOJAVAAudioRender::~VOJAVAAudioRender()
{
	voCOMXAutoLock lock (&m_tmTrack);

	m_bThreadEventStop = true;
	m_sEvent.msg	   = JAVAAR_EXIT;
	m_tsEventStart.Up ();

	int nTimes = 500;
	while (!m_bThreadExit && nTimes > 0)
	{
		nTimes --;
		voOMXOS_Sleep(2);
	}

	if (mClass != NULL)
	{
		JNIEnv * pEnv = AndroidRuntime::getJNIEnv();
		if (pEnv)
			pEnv->DeleteGlobalRef(mClass);
		mClass = NULL;
	}

	VOLOGI("~");
}

status_t VOJAVAAudioRender::open(uint32_t sampleRate, int channelCount, int format, int bufferCount)
{
	voCOMXAutoLock lock (&m_tmTrack);
	if (mMethodOpen == NULL)
	{
		VOLOGE ("open mMethodOpen == NULL");
		return NO_INIT;
	}

	m_sEvent.msg = JAVAAR_OPEN;
	m_sEvent.ext1 = (int)sampleRate;
	m_sEvent.ext2 = (int)channelCount;
	m_tsEventStart.Up ();

	m_bOpen = true;

	m_tsEventFinish.Down ();

	return NO_ERROR;
}

void VOJAVAAudioRender::start()
{
	VOLOGI();
	voCOMXAutoLock lock (&m_tmTrack);
	
	m_sEvent.msg =  JAVAAR_START;
	m_tsEventStart.Up ();

	m_bStoped = false;

	m_tsEventFinish.Down ();
}

void VOJAVAAudioRender::stop()
{
	VOLOGI();
	voCOMXAutoLock lock (&m_tmTrack);

	m_bStoped = true;
	m_sEvent.msg =  JAVAAR_STOP;
	m_tsEventStart.Up ();
	m_tsEventFinish.Down ();
}

void VOJAVAAudioRender::flush()
{
	VOLOGI();
	voCOMXAutoLock lock (&m_tmTrack);

	m_sEvent.msg =  JAVAAR_FLUSH;
	m_tsEventStart.Up ();
	m_tsEventFinish.Down ();
}

void VOJAVAAudioRender::pause()
{
	VOLOGI();
	voCOMXAutoLock lock (&m_tmTrack);

	m_sEvent.msg =  JAVAAR_PAUSE;
	m_tsEventStart.Up ();
	m_tsEventFinish.Down ();
}

ssize_t VOJAVAAudioRender::write(const void* buffer, size_t size)
{
	VOLOGI();
	voCOMXAutoLock lock (&m_tmTrack);

	if(m_bStoped)
		return NO_ERROR;
					
	m_sEvent.msg = JAVAAR_WRITE;
	m_sEvent.ext1 = (int)buffer;
	m_sEvent.ext2 = (int)size;
	m_tsEventStart.Up ();	
	m_tsEventFinish.Down ();

	return NO_ERROR;
}

void VOJAVAAudioRender::close()
{
	voCOMXAutoLock lock (&m_tmTrack);
	
	m_sEvent.msg =  JAVAAR_CLOSE;
	m_tsEventStart.Up ();

	m_bOpen = false;

	m_tsEventFinish.Down ();
}

void VOJAVAAudioRender::setVolume(float left, float right)
{
	voCOMXAutoLock lock (&m_tmTrack);

	m_dLeftVolume = left;
	m_dRightVolume = right;

	m_sEvent.msg =  JAVAAR_SETVOLUME;
	mLeftVolume  = left;
	mRightVolume = right;
	m_tsEventStart.Up ();
	m_tsEventFinish.Down ();
}

void VOJAVAAudioRender::Init()
{
	if (mClass != NULL)
		return;

	VOLOGI("Init JAVA Audio AR");
	JNIEnv* env = AndroidRuntime::getJNIEnv();
	if (env == NULL)
	{
		VOLOGE("Env is NULL");
		return;
	}
	
	jclass clzAudioRender;
	clzAudioRender = env->FindClass("com/mobitv/vome/VOAudioRender");
	if (clzAudioRender != NULL) 
	{
		mMethodOpen = env->GetStaticMethodID(clzAudioRender, "aropen", "(IIII)I");
		if (mMethodOpen == NULL)
		{
			VOLOGE("Can't find mMethodOpen");
			return;
		}	 

		mMethodStart = env->GetStaticMethodID(clzAudioRender, "arstart", "()V");
		if (mMethodStart == NULL)
		{
			VOLOGE("Can't find mMethodStart");
			return;
		}	

		mMethodStop = env->GetStaticMethodID(clzAudioRender, "arstop", "()V");
		if (mMethodStop == NULL)
		{
			VOLOGE("Can't find mMethodStop");
			return;
		}	

		mMethodFlush = env->GetStaticMethodID(clzAudioRender, "arflush", "()V");
		if (mMethodFlush == NULL)
		{
			VOLOGE("Can't find mMethodFlush");
			return;
		}	

		mMethodPause = env->GetStaticMethodID(clzAudioRender, "arpause", "()V");
		if (mMethodPause == NULL)
		{
			VOLOGE("Can't find mMethodPause");
			return;
		}	

		mMethodClose = env->GetStaticMethodID(clzAudioRender, "arclose", "()V");
		if (mMethodClose == NULL)
		{
			VOLOGE("Can't find mMethodClose");
			return;
		}	

		mMethodSetVolume = env->GetStaticMethodID(clzAudioRender, "arsetVolume", "(FF)V");
		if (mMethodSetVolume == NULL)
		{
			VOLOGE("Can't find mMethodSetVolume");
			return;
		}	

		mMethodWrite = env->GetStaticMethodID(clzAudioRender, "arwrite", "([BII)V");
		if (mMethodWrite == NULL)
		{
			VOLOGE("Can't find mMethodWrite");
			return;
		}	

		mClass = (jclass)env->NewGlobalRef(clzAudioRender);
	
		m_bThreadEventStop = false;
		m_bThreadExit = false;
		char name[128];
		sprintf(name, "java_ar_thread-%s-%s", __DATE__, __TIME__);
		AndroidRuntime::createJavaThread(name, (void(*)(void*))vomeARThreadProc, (void*)this); 
	
		VOLOGI("Finished JAVAAudioRender init");
	}
	else
	{
		VOLOGE("clzAudioRender is NULL");
	}
}

void VOJAVAAudioRender::AllocateBuffer(JNIEnv* env, int nBufferSize)
{
	if (nBufferSize > m_nBufferSize)
	{
		if (mRD != NULL)
		{
			env->DeleteLocalRef(mRD);
			mRD = NULL;
		}

		m_nBufferSize = nBufferSize;
		mRD = env->NewByteArray(m_nBufferSize);	
	}
}


void VOJAVAAudioRender::vomeARThreadProc (void* pparam)
{
	VOJAVAAudioRender* pJAVAAudioRender = (VOJAVAAudioRender*)pparam;

	JNIEnv* env = AndroidRuntime::getJNIEnv();
	pJAVAAudioRender->AllocateBuffer(env, 96 * 1024);

	while (!pJAVAAudioRender->m_bThreadEventStop)
		pJAVAAudioRender->RenderAudioLoop(env);

	pJAVAAudioRender->m_bThreadExit = true;
}


void VOJAVAAudioRender::RenderAudioLoop(JNIEnv * env )
{
	m_tsEventStart.Down ();
	
	switch(m_sEvent.msg) 
	{
	case JAVAAR_OPEN:
		env->CallStaticIntMethod(mClass, mMethodOpen, m_sEvent.ext1, m_sEvent.ext2, 0, 0);
		break;

	case JAVAAR_START:
		env->CallStaticVoidMethod(mClass, mMethodStart);
		break;

	case JAVAAR_STOP:
		env->CallStaticVoidMethod(mClass, mMethodStop);
		break;

	case JAVAAR_FLUSH:
		env->CallStaticVoidMethod(mClass, mMethodFlush);
		break;

	case JAVAAR_PAUSE:
		env->CallStaticVoidMethod(mClass, mMethodPause);
		break;

	case JAVAAR_SETVOLUME:
		env->CallStaticVoidMethod(mClass, mMethodSetVolume, mLeftVolume, mRightVolume);
		break;

	case JAVAAR_WRITE:
		if(!m_bStoped)
		{
			void * buffer = (void*)    m_sEvent.ext1;
			size_t size   =  (size_t)  m_sEvent.ext2;						 
			AllocateBuffer(env, size);

			jbyte* b =  env->GetByteArrayElements(mRD, NULL);
			memcpy(b, buffer, size);									  
			env->ReleaseByteArrayElements(mRD, b, 0);

			env->CallStaticVoidMethod(mClass, mMethodWrite, mRD, 0, size);
		}
		break;

	case JAVAAR_CLOSE:
		env->CallStaticVoidMethod(mClass, mMethodClose);
		break;

	case JAVAAR_EXIT:
		break;

	default:
		break;
	}

	m_tsEventFinish.Up ();
}

} // namespace android

