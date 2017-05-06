#ifdef _LINUX_ANDROID
#include <pthread.h>
#if !defined __VO_NDK__
#include <utils/Log.h>
#else
#include <android/log.h>
#endif
#include <string.h>
#include <errno.h>

#include "voLog_android.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#if defined __VO_NDK__

#ifndef LOG_TAG
#define LOG_TAG "VORTSP"
#endif

#if !defined LOGW
#define LOGW(...) ((int)__android_log_print(ANDROID_LOG_WARN, LOG_TAG,__VA_ARGS__))
#endif

#if !defined LOGI
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#if !defined LOGE
#define LOGE(...) ((int)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#endif //__VO_NDK__

void voLog_android(const char * logFileName, const char * logText)
{
	if(errno)
		LOGI("voStreaming %s,[%s],%s\n",strerror(errno),logFileName,logText);	
	else
		LOGI("voStreaming [%s],%s\n",logFileName,logText);
}
#endif // _LINUX_ANDROID
