/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/**
 * Module version info & others
 * Usage:
 *   #define MODULE_NAME "voOMXME"
 *   #define BUILD_NO 1001
 *   #include "voVersion.c"
 * 
 */

#ifdef _LINUX_ANDROID
#include <dlfcn.h>
#include <utils/Log.h>
#endif //_LINUX

#undef  LOG_TAG
#define LOG_TAG "Util"

#include "voVersion.h"

int voGetModuleVersion(char* info)
{
#if DXLNK
    extern void *stext, *etext;
    sprintf(info, "%s - Build %04d - %s %s [%p - %p]",
        MODULE_NAME, BUILD_NO,
        __DATE__, __TIME__,
        stext, etext
        );
#else //DXLNK
    sprintf(info, "%s - Build %04d - %s %s",
        MODULE_NAME, BUILD_NO,
        __DATE__, __TIME__
        );
#endif //DXLNK
    return 0;
}

void voShowModuleVersion(void* hLib)
{
#ifdef _LINUX_ANDROID
    char info[256];
    if (hLib == NULL)
    {
        voGetModuleVersion(info);
        LOGI("About: %s", info);
    }
    else
    {
        int (*modver)(char*) = (int(*)(char*))dlsym(hLib, "voGetModuleVersion");
        if (modver)
        {
            modver(info);
            LOGI("About: %s", info);
        }
    }
#endif // _LINUX
}

