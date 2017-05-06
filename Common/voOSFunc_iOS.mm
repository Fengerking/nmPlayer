	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOSFunc.cpp

	Contains:	component api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifdef _METRO
#include <windows.h>
#include "concrt.h"
#elif defined _WIN32
#include <winsock2.h>
#include <time.h>
#include "Ws2tcpip.h"
#ifdef WINCE
#include <windows.h>
#pragma comment( lib , "Ws2" )
#else
#pragma comment( lib , "Ws2_32.lib" )
#endif //WINCE
#elif defined _LINUX || defined (_IOS) || defined (_MAC_OS)
#include <stdio.h>
#include <unistd.h>
#include <time.h>      
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#elif defined __SYMBIAN32__
#include <e32std.h>
#include <e32base.h>
#endif


#if defined (_IOS) || defined (_MAC_OS)
#import <sys/sysctl.h>
#import <Foundation/Foundation.h>
#import <mach/mach.h>

#import "voLog.h"
#endif

#include "voOSFunc.h"

#define LOG_TAG "voOSFunc"
#define HAVE_SYS_UIO_H
#include "voLog.h"
#undef HAVE_SYS_UIO_H

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

void voOS_Sleep (VO_U32 nTime)
{
#ifdef _WIN32
	Sleep (nTime);
#elif defined _LINUX
	usleep (1000 * nTime);
#elif defined __SYMBIAN32__
	User::After (nTime * 1000);
#elif defined _IOS || defined _MAC_OS
	usleep (1000 * nTime);
#endif // _WIN32
}

void voOS_SleepExitable(VO_U32 nTime, VO_BOOL * pbExit)
{
	VO_U32 nStart = voOS_GetSysTime();
	while(voOS_GetSysTime() < nStart + nTime)
	{
		if(pbExit && VO_TRUE == *pbExit)
			return;

		voOS_Sleep(5);
	}
}

VO_U32 voOS_GetSysTime (void)
{
	VO_U32	nTime = 0;
#ifdef _WIN32
	nTime = GetTickCount ();
#elif defined _LINUX
    timespec tv;
	clock_gettime(CLOCK_MONOTONIC, &tv);

    static timespec stv = {0, 0};
    if ((0 == stv.tv_sec) && (0 == stv.tv_nsec))
	{
		stv.tv_sec = tv.tv_sec;
		stv.tv_nsec = tv.tv_nsec;
	}
    
    nTime = (VO_U32)((tv.tv_sec - stv.tv_sec) * 1000 + (tv.tv_nsec - stv.tv_nsec) / 1000000);
/*	
	struct timeval tval;
	gettimeofday(&tval, NULL);
	nTime = tval.tv_sec*1000 + tval.tv_usec/1000;
*/

#elif defined __SYMBIAN32__
//	unsigned int uTickTime = User::NTickCount();
//	TInt nanokernel_tick_period;
//	HAL::Get(HAL::ENanoTickPeriod, nanokernel_tick_period);
//	nTime = uTickTime / (nanokernel_tick_period / 1000);
	TTime	tNow;
	tNow.HomeTime();
	TDateTime tTime = tNow.DateTime ();
	nTime = tTime.Hour () * 3600000 + tTime.Minute () * 60000 + tTime.Second () * 1000 + tTime.MicroSecond () / 1000;
	
#elif defined _IOS || defined _MAC_OS
	/*
	//NSTimeInterval t = [NSDate timeIntervalSinceReferenceDate];
	nTime = t*1000;
	printf("GetTime = %lld\n", nTime);
	return nTime;
	 */
    
    // Use the running time of system
    nTime = [[NSProcessInfo processInfo] systemUptime] * 1000;
	
//	struct timeval tval;
//    
//	gettimeofday(&tval, NULL);
//	VO_U64 second = tval.tv_sec;
//	second = second*1000 + tval.tv_usec/1000;
//	nTime = second & 0x000000007FFFFFFF;
	//nTime = (VO_U32)second;
	//nTime = tval.tv_sec*1000 + tval.tv_usec/1000;
	
#endif // _WIN32

	return nTime;
}

    VO_U32  voOS_GetModuleFileName(VO_PTR pHandle, VO_PTCHAR buf,  VO_U32 size)
    {
#if defined (_LINUX)
        int r = readlink("/proc/self/exe", buf, size);
		if (r<0 || r>=(int)size)
            return r;
        
        buf[r] = '\0';
        return r;
#elif defined(_METRO)
        
        return 0;
#elif defined(_WIN32)
        return ::GetModuleFileName((HMODULE)pHandle, buf, size);
#elif defined(_MAC_OS)
        CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef,
                                                      kCFURLPOSIXPathStyle);
        const char *pathPtr = CFStringGetCStringPtr(macPath,
                                                    CFStringGetSystemEncoding());
        strcpy(buf, pathPtr);
        CFRelease(appUrlRef);
        CFRelease(macPath);
        
        return 0;
#endif // _LINUX
        
        return -1;
    }

VO_U32 voOS_GetPluginModuleFolder(VO_PTCHAR pFolder, VO_U32 nSize)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
	VO_TCHAR szFullAppName[256];
	voOS_GetModuleFileName(NULL, szFullAppName, strlen(szFullAppName));
	
	if(vostrstr(szFullAppName, _T("Safari")))
		voOS_GetBundleFolderByIdentifier((char *)"com.visualon.safari", pFolder, nSize);
	else if(vostrstr(szFullAppName, _T("Firefox")))
		voOS_GetBundleFolderByIdentifier((char *)"com.visualon.firefox", pFolder, nSize);
	
	VOLOGI("bundle path = %s\n", pFolder);
	
	vostrcat(pFolder, "/Contents/MacOS/");
	
	VOLOGI("Plugin module folder path = %s\n", pFolder);
	
    [pool release];
    
	return 0;
}

VO_U32	voOS_GetBundleFolderByIdentifier(VO_PTCHAR pIdentifier, VO_PTCHAR pFolder, VO_U32 nSize)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
	NSString* bundle_id = [[NSString alloc] initWithFormat: @"%s", pIdentifier];
	NSString* path = [[NSBundle bundleWithIdentifier: bundle_id] bundlePath];
	strcpy(pFolder, [path UTF8String]);
	[bundle_id release];
    
    [pool release];
    
	return 0;
}

/*
@class mobiTVSafariPluginView;
VO_U32	voOS_GetBundleFolder(VO_PTCHAR pFolder, VO_U32 nSize)
{
	NSString *mpath = [[NSBundle bundleForClass:[mobiTVSafariPluginView  class]] bundlePath];
	VOLOGI("bundle path = %s\n", [mpath UTF8String]);
	return 0;
}
 */

VO_U32 voOS_GetAppFolder (VO_PTCHAR pFolder, VO_U32 nSize)
{
#ifdef _WIN32
	GetModuleFileName (NULL, pFolder, nSize);
#elif defined _LINUX
	VO_S32 r = readlink("/proc/self/exe", pFolder, nSize);
	if (r < 0 || r >= (VO_S32)nSize)
		return r;
	pFolder[r] = '\0';
#elif defined _MAC_OS
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	// tag:20110123
    NSString *filePath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/"];
	vostrcpy(pFolder, [filePath UTF8String]);
    [pool release];
	//vostrcpy(pFolder, "/Library/Internet Plug-Ins/");
#elif defined _IOS
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
	strcpy(pFolder, [filePath UTF8String]);
	[pool release];
	//NSString* dir = [[NSBundle mainBundle] bundlePath];
#endif // _LINUX

    VO_PTCHAR pPos = vostrrchr(pFolder, _T('/'));
	if (pPos == NULL)
		pPos = vostrrchr(pFolder, _T('\\'));
    VO_S32 nPos = pPos - pFolder;
    pFolder[nPos+1] = _T('\0');

	return 0;
}

VO_U32 voOS_GetThreadTime (VO_PTR	hThread)
{
	VO_U32 nTime = 0;

#ifdef _WIN32
	if(hThread == NULL)
		hThread = GetCurrentThread();

	if(!hThread)
		return 0;

	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	BOOL bRC = GetThreadTimes(hThread, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);
	if (!bRC)
		return -1;

	LONGLONG llKernelTime = ftKernelTime.dwHighDateTime;
	llKernelTime = llKernelTime << 32;
	llKernelTime += ftKernelTime.dwLowDateTime;

	LONGLONG llUserTime = ftUserTime.dwHighDateTime;
	llUserTime = llUserTime << 32;
	llUserTime += ftUserTime.dwLowDateTime;

	nTime = int((llKernelTime + llUserTime) / 10000);
#elif defined _LINUX
//	nTime = voOS_GetSysTime ();
    timespec tv;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tv);

    static timespec stvThread = {0, 0};
    if ((0 == stvThread.tv_sec) && (0 == stvThread.tv_nsec))
	{
		stvThread.tv_sec = tv.tv_sec;
		stvThread.tv_nsec = tv.tv_nsec;
	}
    
    nTime = (VO_U32)((tv.tv_sec - stvThread.tv_sec) * 1000 + (tv.tv_nsec - stvThread.tv_nsec) / 1000000);
#elif defined __SYMBIAN32__
	nTime = voOS_GetSysTime ();
#elif defined _IOS || defined _MAC_OS
	nTime = voOS_GetSysTime ();
#endif // _WIN32

	return nTime;
}

VO_S32 voOS_EnableDebugMode (int nShowLog)
{ 
#if defined _IOS || defined _MAC_OS || defined _LINUX_ANDROID
    
        char            szAppDir[1024];
        int 			nRC = 0;
        char 			szFile[1024];
        char			szBuff[256];	
        struct stat 	stFile;
        time_t 			tNow;
        struct tm *		ptm;
        
#ifdef _LINUX_ANDROID
        strcpy (szFile,  "/data/local/voDebugFolder/vosystemtime.txt");
        nRC = stat (szFile, &stFile); 
        if (nRC != 0)
        {
            strcpy (szFile,  "/data/local/tmp/voDebugFolder/vosystemtime.txt");
            nRC = stat (szFile, &stFile); 		
        }
#else //_IOS _MAC_OS
        voOS_GetAppFolder(szAppDir, 1024);
        memset(szFile, 0, 1024);
        sprintf (szFile, "%s%s", szAppDir, "voDebugFolder/vosystemtime.txt");
        nRC = stat (szFile, &stFile);
#endif //_LINUX_ANDROID
    
        if (nRC != 0)
        {
            if (nShowLog > 0)
                VOLOGI ("It could not get file time from file %s", szFile);
            return 0;
        }
		
        time_t tFile = stFile.st_atime;
        ptm = localtime(&tFile);
        if (nShowLog > 0)
        {
            VOLOGI ("File   Time: %d  %d-%d-%d %d:%d:%d\n", (int)tFile, ptm->tm_year + 1900, ptm->tm_mon + 1,
                    ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);   
        }
        
        tNow = time(NULL);		
        
        ptm = localtime(&tNow);
        if (nShowLog > 0)	
        {
            VOLOGI ("System Time: %d  %d-%d-%d %d:%d:%d   %d", (int)tNow, ptm->tm_year + 1900, ptm->tm_mon + 1,
                    ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, abs (tNow - tFile)); 
        } 
        
        if (abs (tNow - tFile) > 7200)
            return 0;
    	
        FILE * hFile = fopen (szFile, "rb");
        if (hFile == NULL)
            return 01;
		
        fgets (szBuff, 256, hFile);
        fclose(hFile);
        
        int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMin = 0, nSec = 0;
        sscanf (szBuff, "%d-%d-%d %d:%d:%d", &nYear, &nMonth, &nDay, &nHour, &nMin, &nSec);
        
        if (nShowLog > 0)
            VOLOGI ("File: %s,  Time: %d-%d-%d  %d:%d:%d", szBuff, nYear, nMonth, nDay, nHour, nMin, nSec);
        
        if (nYear - 1900 != ptm->tm_year || nMonth != ptm->tm_mon + 1 || nDay != ptm->tm_mday)
            return 0;
		
        int nFileTime = nHour * 3600 + nMin * 60 + nSec;
        int nSysteimTime = ptm->tm_hour * 3600 + ptm->tm_min * 60 + ptm->tm_sec;
        
        
        if (nShowLog > 0)
            VOLOGI ("File Time %d, System time %d, Offset: %d", nFileTime, nSysteimTime, abs (nSysteimTime - nFileTime));
        
        if (abs (nSysteimTime - nFileTime) > 7200)
            return 0;
    
#ifdef _LINUX_ANDROID
        LOGW ("The EnableDebugMode successful !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
#else
        printf ("The EnableDebugMode successful !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
#endif
        
        return 1;
        
#else
    return 0;
#endif // _LINUX_ANDROID _IOS _MAC_OS
}
    
VO_U32 voOS_Log (char * pText)
{
#if defined _IOS || defined _MAC_OS
	printf("%s", pText);
#else
	printf(pText);
#endif // _IOS _MAC_OS
	
	return 0;
}

VO_U32 voOS_GetApplicationID(VO_PTCHAR pAppID, VO_U32 nSize)
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
	int nRet = -1;
	
#if defined _IOS || defined _MAC_OS
	NSString* pID = [[NSBundle mainBundle] bundleIdentifier];
	strcpy(pAppID, [pID UTF8String]);
	nRet = 0;
#endif // _IOS
	
    [pool release];
	return nRet;
}

VO_U32	voOS_GetCPUNum ()
{
#if defined(_LINUX_ANDROID)
	int nTemps[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
	char cCpuName[512];
	memset(cCpuName, 0, sizeof(cCpuName));

	for(int i = (sizeof(nTemps)/sizeof(nTemps[0])) - 1 ; i >= 0; i--)
	{
		sprintf(cCpuName, "/sys/devices/system/cpu/cpu%d", nTemps[i]);
		int nOk = access(cCpuName, F_OK);
		if( nOk == 0)
		{
			return nTemps[i]+1;
		}
	}
	return 1;
#elif defined(_IOS)
    static dispatch_once_t pred;
    static unsigned int ncpu = 1;
    
    dispatch_once(&pred, ^{
        size_t len;
        len = sizeof(ncpu);
        sysctlbyname ("hw.ncpu",&ncpu,&len,NULL,0);
    });
    
	return ncpu; 
#else
	return 1;
#endif
}

#if defined(_IOS) || defined(_MAC_OS)
typedef void (* voSigPipe) (int);
voSigPipe g_SIGPIPEHandler = NULL;
    
void voReceiveSignal(int signal) {
    VOLOGI("voReceiveSignal signal %d", signal);
}

VO_S32 voMoreUNIXErrno(VO_S32 result)
{
    int err;
    
    err = 0;
    if (result < 0) {
        err = errno;
    }
    return err;
}

VO_S32 voMoreUNIXIgnoreSIGPIPE(void)
{
    int err;
    struct sigaction signalState;
    
    err = sigaction(SIGPIPE, NULL, &signalState);
    err = voMoreUNIXErrno(err);
    if (err == 0) {
        if (NULL != signalState.sa_handler) {
            g_SIGPIPEHandler = signalState.sa_handler;
        }
        
        VOLOGI("iOS voMoreUNIXIgnoreSIGPIPE Old sa_handler:%p", g_SIGPIPEHandler);
        signalState.sa_handler = voReceiveSignal;
        
        err = sigaction(SIGPIPE, &signalState, NULL);
        err = voMoreUNIXErrno(err);
    }
    
    return err;
}

VO_S32 voMoreUNIXRecoverySIGPIPE(void)
{
    int err;
    struct sigaction signalState;
    
    err = sigaction(SIGPIPE, NULL, &signalState);
    err = voMoreUNIXErrno(err);
    if (err == 0) {
        if (NULL == signalState.sa_handler) {
            return 0;
        }
        else if (voReceiveSignal != signalState.sa_handler) {
            VOLOGI("iOS voMoreUNIXRecoverySIGPIPE return by sa_handler have be changed to:%p", signalState.sa_handler);
            return -1;
        }
        signalState.sa_handler = g_SIGPIPEHandler;
        VOLOGI("iOS voMoreUNIXRecoverySIGPIPE return OK by sa_handler:%p", signalState.sa_handler);
        
        err = sigaction(SIGPIPE, &signalState, NULL);
        err = voMoreUNIXErrno(err);
    }
    
    return err;
}

#endif

#if defined (_IOS) || defined (_MAC_OS)
    float cpu_usage()
    {
        kern_return_t kr;
        task_info_data_t tinfo;
        mach_msg_type_number_t task_info_count;
        
        task_info_count = TASK_INFO_MAX;
        kr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)tinfo, &task_info_count);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        
        task_basic_info_t      basic_info;
        thread_array_t         thread_list;
        mach_msg_type_number_t thread_count;
        
        thread_info_data_t     thinfo;
        mach_msg_type_number_t thread_info_count;
        
        thread_basic_info_t basic_info_th;
        uint32_t stat_thread = 0; // Mach threads
        
        basic_info = (task_basic_info_t)tinfo;
        
        // get threads in the task
        kr = task_threads(mach_task_self(), &thread_list, &thread_count);
        if (kr != KERN_SUCCESS) {
            return -1;
        }
        if (thread_count > 0)
            stat_thread += thread_count;
        
        long tot_sec = 0;
        long tot_usec = 0;
        float tot_cpu = 0;
        int j;
        
        for (j = 0; j < thread_count; j++)
        {
            thread_info_count = THREAD_INFO_MAX;
            kr = thread_info(thread_list[j], THREAD_BASIC_INFO,
                             (thread_info_t)thinfo, &thread_info_count);
            if (kr != KERN_SUCCESS) {
                return -1;
            }
            
            basic_info_th = (thread_basic_info_t)thinfo;
            
            if (!(basic_info_th->flags & TH_FLAGS_IDLE)) {
                tot_sec = tot_sec + basic_info_th->user_time.seconds + basic_info_th->system_time.seconds;
                tot_usec = tot_usec + basic_info_th->system_time.microseconds + basic_info_th->system_time.microseconds;
                tot_cpu = tot_cpu + basic_info_th->cpu_usage / (float)TH_USAGE_SCALE * 100.0;
            }
            
        } // for each thread
        
        kr = vm_deallocate(mach_task_self(), (vm_offset_t)thread_list, thread_count * sizeof(thread_t));
        assert(kr == KERN_SUCCESS);
        
        return tot_cpu;
    }
#endif
    
int voOS_GetCpuUsage(VO_U32* pOutSys, VO_U32* pOutUsr, VO_BOOL* pInGoing, VO_U32 inDelay, VO_U32 inGrid)
{
    int nOk = 0;
#if defined(_LINUX)
	assert(pOutSys != NULL);
	assert(pOutUsr != NULL);
	assert(pInGoing != NULL);
	gOsDelay = inDelay;
	gOsGrid = inGrid;
	gCpuGoing = pInGoing;
	nOk = pthread_once(&gCpuUThread, get_cpu);
	gCpuUThread = PTHREAD_ONCE_INIT;
	*pOutSys = gCpuSys;
	*pOutUsr = gCpuUsr;
#endif

#if defined (_IOS) || defined (_MAC_OS)
    
    if (NULL == pOutSys || NULL == pOutUsr) {
        return -1;
    }
    
    int nCpu = voOS_GetCPUNum();
    if (0 == nCpu) {
        nCpu = 1;
    }
    
    processor_info_array_t startCpuInfo = 0;
    natural_t numCPUsU = 0U;
    mach_msg_type_number_t numStartCpuInfo = 0;
    mach_msg_type_number_t numEndCpuInfo = 0;
    
    kern_return_t err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numCPUsU, &startCpuInfo, &numStartCpuInfo);
    if((KERN_SUCCESS != err) || (0 == startCpuInfo)) {
        return -1;
    }
    
    useconds_t uTimeToSleep = 0;
    if (0 >= inDelay) {
        uTimeToSleep = 100; // at least 100ms
    }
    else {
        uTimeToSleep = inDelay * 1000; // millisecond
    }

    if ((0 >= inGrid) || (inGrid >= uTimeToSleep) || (NULL == pInGoing)) {
        usleep(uTimeToSleep * 1000);
    }
    else {
        int nTimes = uTimeToSleep / inGrid;
        for (int i=0; i < nTimes; ++i)
        {
            if (*pInGoing == VO_FALSE)
                break;
            
            usleep(inGrid * 1000);
        }
    }
    
    processor_info_array_t endCpuInfo = 0;
    err = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numCPUsU, &endCpuInfo, &numEndCpuInfo);
    if ((KERN_SUCCESS != err) || (0 == endCpuInfo)) {
        
        if (startCpuInfo) {
            size_t startCpuInfoSize = sizeof(integer_t) * numStartCpuInfo;
            vm_deallocate(mach_task_self(), (vm_address_t)startCpuInfo, startCpuInfoSize);
        }
        
        return -1;
    }
    
    float systemUsedPer = 0;
    float userUsedPer = 0;
    
    for (unsigned i = 0U; i < nCpu; ++i) {

        float userUsed = endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_USER];
        
        float systemUsed = (endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_SYSTEM])
                 + (endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_NICE]);
        
        float totalCpu = userUsed + systemUsed + (endCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE] - startCpuInfo[(CPU_STATE_MAX * i) + CPU_STATE_IDLE]);
        
        if (0 != totalCpu) {
            systemUsedPer = (systemUsed / totalCpu) + systemUsedPer;
            userUsedPer = (userUsed / totalCpu) + userUsedPer;
        }
    }
    
    if (startCpuInfo) {
        size_t startCpuInfoSize = sizeof(integer_t) * numStartCpuInfo;
        vm_deallocate(mach_task_self(), (vm_address_t)startCpuInfo, startCpuInfoSize);
    }
    
    if (endCpuInfo) {
        size_t endCpuInfoSize = sizeof(integer_t) * numEndCpuInfo;
        vm_deallocate(mach_task_self(), (vm_address_t)endCpuInfo, endCpuInfoSize);
    }
    
    *pOutSys = (systemUsedPer / nCpu) * 100;
    *pOutUsr = (userUsedPer / nCpu) * 100;

#endif
    
    return nOk;
}

#ifdef _LINUX_ANDROID
static long unsigned g_nMaxCpuFrequency = 0;
#endif	// _LINUX_ANDROID
VO_BOOL voOS_GetCurrentCpuFrequencyPercent(VO_U32 * pPercent)
{
#ifdef _LINUX_ANDROID
	FILE * pFile = NULL;
	if(g_nMaxCpuFrequency == 0)
	{
		pFile = fopen("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", "r");
		if(!pFile)
		{
			VOLOGE("failed to open the cpuinfo_max_freq");
			return VO_FALSE;
		}

		fscanf(pFile, "%lu", &g_nMaxCpuFrequency);
		fclose(pFile);
		pFile = NULL;
	}

	pFile = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
	if(!pFile)
	{
		VOLOGE("failed to open the scaling_cur_freq");
		return VO_FALSE;
	}

	long unsigned nCurrentCpuFrequency;
	fscanf(pFile, "%lu", &nCurrentCpuFrequency);
	fclose(pFile);

	VO_U64 ullCurrentCpuFrequency = nCurrentCpuFrequency;
	if(pPercent)
	{
		*pPercent = ullCurrentCpuFrequency * 10000 / g_nMaxCpuFrequency;
//		VOLOGI("percent %d, current freq %d, max freq %d", *pPercent, nCurrentCpuFrequency, g_nMaxCpuFrequency);
	}

	return VO_TRUE;
#else
	return VO_FALSE;
#endif	// _LINUX_ANDROID
}

    VO_VOID voOS_SendPacket(int fd)
    {
#ifdef _METRO
#else //_METRO
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x))>>11))
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))
        unsigned int data[12];
        struct timeval now;
        if (sizeof(data) != 48)
            return;
        memset((char*)data, 0, sizeof(data));
        data[0] = htonl((0 << 30) | (3 << 27) | (3 << 24)
                        | (0 << 16) | (4 << 8) | (-6 & 0xff));
        data[1] = htonl(1<<16);  /* Root Delay (seconds) */
        data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */
#ifdef _WIN32
        struct tm tm;
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);
        tm.tm_year = wtm.wYear - 1900;
        tm.tm_mon = wtm.wMonth - 1;
        tm.tm_mday = wtm.wDay;
        tm.tm_hour = wtm.wHour;
        tm.tm_min = wtm.wMinute;
        tm.tm_sec = wtm.wSecond;
        tm.tm_isdst = -1;
        now.tv_sec = (long)mktime(&tm);
        now.tv_usec = wtm.wMilliseconds * 1000;
#else
        gettimeofday(&now, NULL);
#endif
        data[10] = htonl(now.tv_sec + 0x83aa7e80); /* Transmit Timestamp coarse */
        data[11] = htonl(NTPFRAC(now.tv_usec));  /* Transmit Timestamp fine   */
#ifdef _WIN32
        send(fd, (char*)data, 48, 0);
#else
        send(fd, data, 48, 0);
#endif
#endif //_METRO
    }
    
    VO_BOOL voOS_GetUTCFromNetwork(char *host, time_t * ptime, char *pPort)
    {
#ifdef _METRO
        return VO_FALSE;
#else //_METRO
#ifdef _WIN32
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;
        wVersionRequested = MAKEWORD( 2, 2 );
        err = WSAStartup( wVersionRequested, &wsaData );
        
        if( err != 0 )
            return VO_FALSE;
#endif
        int sockfd, numbytes;
        char buffer[101];
        struct addrinfo info;
        memset(&info, 0, sizeof(info));
        info.ai_family = AF_INET;
        info.ai_socktype = SOCK_STREAM;
        info.ai_protocol = 6;
        struct addrinfo * ptr_ret;
        //char * str_host = host;
        int ret = getaddrinfo(host , pPort , &info , &ptr_ret );
        if(ret != 0){
#ifdef _WIN32
            WSACleanup();
#endif
            return VO_FALSE;
        }
        if((sockfd = socket(ptr_ret->ai_family, ptr_ret->ai_socktype, ptr_ret->ai_protocol)) == -1){
#ifdef _WIN32
            WSACleanup();
#endif
            freeaddrinfo(ptr_ret);
            return VO_FALSE;
        }
        unsigned long ul = 1;
#ifdef _WIN32
        ioctlsocket(sockfd, FIONBIO, (unsigned long*)&ul);
#else
        ioctl(sockfd, FIONBIO, &ul);
#endif
        struct timeval timeout;
        int error, len = sizeof(int);
        fd_set   r;
        FD_ZERO(&r);
        FD_SET(sockfd,   &r);
        timeout.tv_sec = 2;
        timeout.tv_usec =0;
        if(connect(sockfd, ptr_ret->ai_addr, ptr_ret->ai_addrlen) == -1){
            if(select(sockfd + 1 , 0 , &r , 0 , &timeout ) > 0){
#ifdef _WIN32
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
#else
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
#endif
                if(error != 0){
#ifdef _WIN32
                    WSACleanup();
                    closesocket(sockfd);
#else
                    close(sockfd);
#endif
                    freeaddrinfo(ptr_ret);
                    return VO_FALSE;
                }
            }
        }
        ul = 0;
#ifdef _WIN32
        ioctlsocket(sockfd, FIONBIO, (unsigned long*)&ul);
#else
        ioctl(sockfd, FIONBIO, &ul);
#endif
        freeaddrinfo(ptr_ret);
        FD_ZERO(&r);
        FD_SET(sockfd,   &r);
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
        if(select(sockfd + 1 , 0 , &r , 0 , &timeout ) <= 0){
#ifdef _WIN32
            WSACleanup();
            closesocket(sockfd);
#else
            close(sockfd);
#endif
            return VO_FALSE;
        }
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(struct timeval));
        voOS_SendPacket(sockfd);
        if((numbytes = recv(sockfd, buffer, 100, 0)) == -1){
#ifdef _WIN32
            WSACleanup();
            closesocket(sockfd);
#else
            close(sockfd);
#endif
            return VO_FALSE;
        }
        if(numbytes < 20){
#ifdef _WIN32
            WSACleanup();
            closesocket(sockfd);
#else
            close(sockfd);
#endif
            return VO_FALSE;
        }else{
            char szTemp[100]="";
            char *pdest;
            pdest = strchr( buffer, ' ' );
            strcpy(szTemp, pdest+1);
            struct tm tm1;
            memset((void*)&tm1, 0, sizeof(struct tm));
            sscanf(szTemp, "%2d-%2d-%2d %2d:%2d:%2d",
                   &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);
            tm1.tm_year += (2000 - 1900);
            tm1.tm_mon --;
            tm1.tm_isdst=-1;
            *ptime = mktime(&tm1);
        }
#ifdef _WIN32
        WSACleanup();
        closesocket(sockfd);
#else
        close(sockfd);
#endif
        return VO_TRUE;
        
#endif // _METRO 
    }

    VO_S32 voOS_GetUTCServer(VO_PCHAR server, VO_PCHAR sPort)
    {
#ifdef _WIN32
        return 1;
#else
        VO_CHAR BufLine[256], Index = '0';
        VO_PCHAR target;
#if defined (_IOS) || defined (_MAC_OS)
        char szTmp[512];
        voOS_GetAppFolder(szTmp, 256);
        strcat(szTmp, "timeServer.cfg");
        
        FILE * cfgFD = fopen(szTmp, "r");
#else
        FILE * cfgFD = fopen("/data/local/tmp/timeServer.cfg", "r");
#endif
        if(cfgFD == NULL){
            return 1;
        }
        while((fgets(BufLine, 256, cfgFD)) != NULL){
            if(!strcmp(BufLine, "\n"))
                continue;
            BufLine[strlen(BufLine) - 1] = '\0';
            target = strstr(BufLine, "target=");
            if(target != NULL){
                Index=*(target + 7);
            }
            
            if(!strncmp((VO_PCHAR)&BufLine[0], (VO_PCHAR)&Index, 1)){
                char * portIndex = strstr(BufLine, ":");
                memcpy((void*)sPort, portIndex + 1, strlen(portIndex) - 1);
                *(sPort + strlen(portIndex) - 1) = '\0';
                memcpy((void*)server, BufLine + 2, strlen(BufLine) - 1 - strlen(portIndex));
                fclose(cfgFD);
                return 0;
            }
        }
        fclose(cfgFD);
        
        return 1;
#endif
    }
    
    VO_S64 voOS_GetUTC(void)
    {
#ifdef _METRO
        return -1;
#else //_METRO
#if defined(_LINUX_ANDROID) || defined(_WIN32) || defined (_IOS) || defined (_MAC_OS)
#ifdef WINCE
        SYSTEMTIME st;
        GetSystemTime(&st);
        
        long long t;
        SystemTimeToFileTime(&st , (FILETIME *)&t);
        
        return t;
#else
        time_t curr = time(&curr);
        return curr;
#endif // WINCE
#else
        return -1;
#endif /// LINUX_WIN32
#endif //_METRO
    }
const char* voOS_GetOSName()
{
	const char* pName = NULL;
#if defined(_LINUX)
	pName = "Linux";
#   if defined(_LINUX_ANDROID) || defined(__VO_NDK__)
	pName = "Android";
#   endif // _LINUX_ANDROID || __VO_NDK__
#elif defined(_IOS)
	pName = "IPhone_OS";
#elif defined(_MAC_OS)
	pName="Mac_OS";
#elif defined(_WIN32)
	pName= "Windows_PC";
#   if defined(_WIN32_WCE)
	pName = "Windows_CE";
#   elif defined(_WIN32_MOBILE)
	pName= "Windows_MB";
#   endif // _WIN32
#else
	pName = "Unkown"
#endif
   return pName;
}
    
    
VO_S32 voOS_GetAppResourceFolder (VO_PTCHAR pFolder, VO_U32 nSize)
{
    int nRet = -1;
    
    if(!pFolder)
        return nRet;
    
#if (defined _IOS) || (defined _MAC_OS)
    
    NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
    //printf("resourcePath is  %s\n", [resourcePath UTF8String]);
    
    strcpy(pFolder, [resourcePath UTF8String]);
    
    nRet = 0;
    
#endif
    
    return nRet;
}

    
#ifdef _VONAMESPACE
}
#endif
