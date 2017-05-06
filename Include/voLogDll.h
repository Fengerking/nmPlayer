#ifndef __voLogDll_H__
#define __voLogDll_H__

#include "voType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

VO_U32 VO_API voLogDllInit (VO_U32 uMoudleID,  VO_CHAR* pCfgPath, VO_S32* pMaxLogLevel);

VO_U32 VO_API voLogDllGetMaxLevel (VO_U32 uMoudleID, VO_S32* pMaxLogLevel);

VO_U32 VO_API voLogDllLogPrint (int nLevel, VO_CHAR* pLogText);

VO_U32 VO_API voLogDllUninit ();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voLogDll_H__