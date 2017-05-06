#include "defines.h"
#include "global.h"

static char tmpBuf[1024]; 

#if defined(ANDROID)//&&!defined(NDK_BUILD)
//#include <pthread.h>
#ifdef NDK_BUILD
#include <android/log.h>
#define LOG_TAG "_VOH264"
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#else//NDK_BUILD
#include <utils/Log.h>
#endif//NDK_BUILD
#define LOG_TAG "_VOH264"
#else//_LINUX_ANDROID
#define LOGI(...) 
#endif//_LINUX_ANDROID

#include <string.h>
#include "stdio.h"
#include 		<stdarg.h>
#include "voLog_android.h"
//void voH264Printf(const char * format,...);

void voLog_264_debug(char *format, ...)
{ 
#ifdef DEBUG_INFO
	va_list arg;
	va_start(arg, format);
	vsprintf(tmpBuf,format,arg);
	LOGI("%s\n",tmpBuf);
	printf("VOH264:%s\n",tmpBuf);
	va_end(arg);
#endif	
}
void voLogAndroidInfo(char* text)
{
#if CALC_FPS
    LOGI("%s",text);	
#else
#ifdef DEBUG_INFO
	LOGI("%s",text);	
#ifndef ARMHF	
	printf("%s\n",text);
#endif
#endif
#endif
}

