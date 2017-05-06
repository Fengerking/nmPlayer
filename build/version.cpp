/**
 * Auto output version information in logcat when a module is loaded/unloaded
 * @created Jason Gao, 9/10/2010
 */

//#include <utils/Log.h>
#include <stdio.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif //LOG_TAG
#define LOG_TAG MODULE_NAME
#define __VERINFO MODULE_NAME " - " TARGET_CPUARCH " - " TARGETOS " - " MODULE_BRANCH " - r" SOURCE_REVISIONNO " - B" MODULE_VERSION "." MODULE_BUILD " - " __TIME__ " " __DATE__ " - V" GLOBALVER "\n"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

static class __CShowVersion
{
public:
    __CShowVersion()
    {
        printf("voAbout> " __VERINFO);
    }

/*
    ~__CShowVersion()
    {
        //LOGI("voAbout< " __VERINFO);
        printf("voAbout< " __VERINFO);
    }
*/
}
__dummy_show_version;

#ifdef _VONAMESPACE
}
#endif

