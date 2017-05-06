	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOSFunc.h

	Contains:	voOSFunc header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voOSFunc_H__
#define __voOSFunc_H__
#include "voYYDef_Common.h"
#include "voType.h"
#include "voString.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif

void			voOS_Sleep (VO_U32 nTime);
// this function is exit-able but not accurate, please use it carefully
void			voOS_SleepExitable (VO_U32 nTime, VO_BOOL * pbExit);
VO_U32			voOS_GetSysTime (void);
void			voOS_SetTimePeriod(VO_BOOL bStartOrEnd, VO_U32 nPeriod);

VO_S64			voOS_GetUTC (void);
#if defined(_LINUX_ANDROID) || defined(_WIN32)
VO_BOOL     voOS_GetUTCFromNetwork(char *host, time_t * ptime, char *port);
#endif
VO_VOID     voOS_SendPacket(int fd);
VO_S32      voOS_GetUTCServer(VO_PCHAR server, char *sPort);

VO_U32			voOS_GetThreadTime (VO_PTR	hThread);

VO_U32      voOS_GetModuleFileName(VO_PTR, VO_PTCHAR,  VO_U32);
VO_U32			voOS_GetAppFolder (VO_PTCHAR pFolder, VO_U32 nSize);
	
VO_U32			voOS_GetPluginModuleFolder(VO_PTCHAR pFolder, VO_U32 nSize);
VO_U32			voOS_GetBundleFolderByIdentifier(VO_PTCHAR pIdentifier, VO_PTCHAR pFolder, VO_U32 nSize);
VO_U32			voOS_GetApplicationID(VO_PTCHAR pAppID, VO_U32 nSize);

VO_U32			voOS_GetCPUNum (void);
VO_U32			voOS_GetCPUFrequency (void);
void			voOS_GetCPUType(char *cpuString,int length);

VO_S32 			voOS_EnableDebugMode (int nShowLog);

VO_U32			voOS_Log (char * pText);

const char*	    voOS_GetOSName();
		
// outSys means the system cpu usage, outUsr means the user cpu usage, generally speaking cpu usage should use
// outSys+outUsr. These output values have been x100. inDelay use second. InDelay suggestion value is 1
// in order to solve exiting this function at least inDelay second problem, I have add the pInGoing parameters, the user can
// set it with pInGoing = false to return from this function with at least 	inGrid (ms)	
int				voOS_GetCpuUsage(VO_U32* pOutSys, VO_U32* pOutUsr, VO_BOOL* pInGoing, VO_U32 inDelay, VO_U32 inGrid);
// *pPercent: percent * 100, for example 8512 means 85.12%
VO_BOOL			voOS_GetCurrentCpuFrequencyPercent(VO_U32 * pPercent);
    
#if defined(_IOS) || defined(_MAC_OS)
VO_S32          voMoreUNIXErrno(VO_S32 result);

// Sets the handler for SIGPIPE to voReceiveSignal.  If you don't call 
// this, writing to a broken pipe will cause SIGPIPE (rather 
// than having "write" return EPIPE), which is hardly ever what you want. 
// This function will remember old sa_handler.
VO_S32          voMoreUNIXIgnoreSIGPIPE(void);

// Return -1 if current handler not equals to voReceiveSignal.
// Recovery sigpipe handler to old sa_handler if current handler equals to voReceiveSignal.
// Return errno value if failed
VO_S32          voMoreUNIXRecoverySIGPIPE(void);
#endif
        
VO_S32			voOS_GetAppResourceFolder (VO_PTCHAR pFolder, VO_U32 nSize);
        
#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */

#endif //__voOSFunc_H__
