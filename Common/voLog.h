#ifndef __volog_H__
#define __volog_H__

#include "voYYDef_Common.h"
#include <string.h>
#include "voString.h"
#include "voType.h"
#include <stdarg.h>

#ifdef _LINUX_ANDROID
#include <pthread.h>
#if !defined __VO_NDK__
#include <cutils/log.h>
#else
#include <android/log.h>
#include <sys/system_properties.h>
#endif
#endif // _LINUX_ANDROID

#ifdef _LINUX_X86
#include <pthread.h>
#endif	//_LINUX_X86


#ifdef _WIN32
#include "windows.h"

#pragma warning (disable : 4996)
#pragma warning (disable : 4003)
#endif // _WIN32

#if defined(_IOS) || defined(_MAC_OS)
#include <pthread.h>
#include <stdarg.h>
#endif //_IOS _MAC_OS

/********************************************************************************
There were four types logs
1. VOLOGE (...)		Error.	 This type log will always show in debug window.
2. VOLOGW (...)		Warning. This type log will show if define _VOLOG_WARNING
3. VOLOGI (...)		Info	 This type log will show if define _VOLOG_INFO
4. VOLOGS (...)		Status.	 This type log will show if define _VOLOG_STATUS
5. VOLOGR (...)		Run.	 This type log will show if define _VOLOG_RUN.
This should be define in run loop. Performance.
6. VOLOGF (...)		Status.	 This type log will show if define _VOLOG_FUNC

********************************************************************************/
#if defined __VO_NDK__

#ifndef LOG_TAG
#define  LOG_TAG "VOLOG"
#endif // LOG_TAG

#if !defined LOGW
#define LOGW(...) ((int)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif

#if !defined LOGI
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#if !defined LOGE
#define LOGE(...) ((int)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#define __D(fmt, args...) LOGI("%s::%s::%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)
#elif defined(_IOS) || defined(_MAC_OS)
#define __D(fmt, ...)
#else //for win32
#define __D(...)
#endif //__VO_NDK__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



#ifdef _WIN32	
#define _logGETTHREADID() GetCurrentThreadId()
#define _logSLASH '\\'

#else
#define _logGETTHREADID() (int)pthread_self()
#define _logSLASH '/'
#endif //endif WIN32

	//new 
#ifdef _WIN32	
#ifdef WINCE
#define voLogSprintf _snprintf
#else
#define voLogSprintf _sprintf_p
#endif //endif WINCE

#else

#define voLogSprintf snprintf

#endif //endif WIN32

#if defined _IOS || defined _MAC_OS
#define LOG_LENGTH 1024
#else
#define LOG_LENGTH 2048
#endif


//#define _logtimeFormat "%04d-%02d-%02d %02d:%02d:%02d.%03d"
#define _logtimeFormat "%02d:%02d:%02d.%03d"
#define _defaultlogtime	"00:00:00.000"

#define _logFILENAME			strrchr(__FILE__, _logSLASH) ? strrchr(__FILE__, _logSLASH) +1: __FILE__
#define _logINFO(x)				_defaultlogtime,x,_logGETTHREADID(),_logFILENAME,__FUNCTION__, __LINE__ 
#define LOGFORMAT(_fmt,str)     "%s @@@VOLOG,%7s, %08X, %s, %s, %d, " _fmt,_logINFO(#str) 

#define MODULEVOLOGUNINIT(name) vologUninit##name
#define vologUninit(name) MODULEVOLOGUNINIT(name)
#define MODULEVOLOGINIT(name) vologInit##name
#define vologInit(name) MODULEVOLOGINIT(name)
#define MODULEVOLOGPRINT(name) vologPrint##name
#define vologPrint(name) MODULEVOLOGPRINT(name)

int		vologInit(_VOMODULEID)(VO_TCHAR* pWorkPath);/* Load library work path */
int		vologUninit(_VOMODULEID)();
void	vologPrint(_VOMODULEID)(VO_S32 level, const char *__fmt,...);

//#if defined _WIN32 || defined _LINUX_ANDROID || defined _IOS || defined _MAC_OS

#ifndef _VONDBG //in release mode, all log will be disabled
	#define VOLOGINIT(pWorkPath) \
	{\
		vologInit(_VOMODULEID)(pWorkPath);\
	}
	#define VOLOGUNINIT() \
	{\
		vologUninit(_VOMODULEID)();\
	}

	#define VOLOGE(fmt, ...) \
	{ \
		vologPrint(_VOMODULEID)(0,  LOGFORMAT(fmt,Error), ##__VA_ARGS__); \
	}

	#define VOLOGW(fmt, ...) \
	{ \
		vologPrint(_VOMODULEID)(1,  LOGFORMAT(fmt,Warning), ##__VA_ARGS__); \
	}

	#define VOLOGI(fmt, ...) \
	{ \
		vologPrint(_VOMODULEID)(2, LOGFORMAT(fmt,Info), ##__VA_ARGS__); \
	}

	#define VOLOGS(fmt, ...) \
	{ \
		vologPrint(_VOMODULEID)(3,  LOGFORMAT(fmt,Status), ##__VA_ARGS__); \
	}

	#define VOLOGR(fmt, ...) \
	{ \
		vologPrint(_VOMODULEID)(4,  LOGFORMAT(fmt,Run), ##__VA_ARGS__); \
	}
#else
	#define	VOLOGINIT(pWorkPath)
	#define	VOLOGUNINIT()

	#define VOLOGE(fmt, ...)
	#define VOLOGW(fmt, ...)
	#define VOLOGI(fmt, ...)
	#define VOLOGS(fmt, ...)
	#define VOLOGR(fmt, ...)
#endif 


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __volog_H__
