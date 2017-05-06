/**
 * Auto output version information in logcat when a module is loaded/unloaded
 * @created Jason Gao, 9/10/2010
 */

#include <stdio.h>
#include <string.h>
#include <android/log.h>

#if !defined LOGI
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#define LOG_TAG MODULE_NAME
#define __VERINFO "voAbout> " MODULE_NAME " - " CPU_ARCH " - " BUILD_TOOL " - " MODULE_BRANCH " - r" SOURCE_REVISIONNO " - B" MODULE_VERSION "." MODULE_BUILD " - " __TIME__ " " __DATE__ " - V" GLOBALVER "\n"


static class __CShowVersion
{
public:
    __CShowVersion()
    {
#ifdef _VOLOG_ERROR
		LOGI(__VERINFO);
#else
    	char szVerInfo[512];
 	  	strcpy (szVerInfo, __VERINFO);		
#endif // _VOLOG_ERROR
		}

	// david 2011-05-04
	// to comply with ndk compiler
    /*~__CShowVersion()
    {
        printf("voAbout [NDK] < " __VERINFO);
		}*/
}__dummy_show_version;

