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
#include <windows.h>
#include <time.h>
#include "Ws2tcpip.h"
#include <stdio.h>
#ifdef WINCE
#pragma comment(lib, "mmtimer.lib")
#pragma comment(lib, "Ws2")
#else
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Ws2_32.lib")
#endif //WINCE
#elif defined _LINUX
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
#include <assert.h>
#elif defined __SYMBIAN32__
#include <e32std.h>
#include <e32base.h>
#elif defined _MAC_OS
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#import <Foundation/Foundation.h>
//#include <string>
#elif defined _IOS
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#import <Foundation/Foundation.h>
#endif

#include "voOSFunc.h"

#define LOG_TAG "voOSFunc"
#define HAVE_SYS_UIO_H
#include "voLog.h"
#undef HAVE_SYS_UIO_H

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#ifdef WINCE
const DOUBLE VT_SECOND_1970 = 2209161600.0;
const DOUBLE SECONDS_IN_ONE_DAY = 86400;
time_t __cdecl mktime(tm* pTM)
{
    SYSTEMTIME stToConvert = {
        pTM->tm_year+1900,
        pTM->tm_mon+1,
        pTM->tm_wday,
        pTM->tm_mday,
        pTM->tm_hour,
        pTM->tm_min,
        pTM->tm_sec,
        0};
    DOUBLE dToCal;
    SystemTimeToVariantTime(&stToConvert, &dToCal);
    return (time_t)(dToCal*SECONDS_IN_ONE_DAY - VT_SECOND_1970);
}
#endif //WINCE

void voOS_Sleep (VO_U32 nTime)
{
#ifdef _METRO
	Concurrency::wait(nTime);
#elif defined _WIN32
	Sleep (nTime);
#elif defined _LINUX
	usleep (1000 * nTime);
#elif defined __SYMBIAN32__
	User::After (nTime * 1000);
#elif defined _IOS
	usleep (1000 * nTime);
#elif defined _MAC_OS
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

#ifdef _METRO
	nTime = GetTickCount64();
#elif defined _WIN32
	nTime = ::timeGetTime();
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
#elif defined _IOS
	/*
	 //NSTimeInterval t = [NSDate timeIntervalSinceReferenceDate];
	 nTime = t*1000;
	 printf("GetTime = %lld\n", nTime);
	 return nTime;
	 */
	
	struct timeval tval;
	gettimeofday(&tval, NULL);
	VO_U64 second = tval.tv_sec;
	second = second*1000 + tval.tv_usec/1000;
	nTime = second & 0x000000007FFFFFFF;
	//nTime = (VO_U32)second;
	//nTime = tval.tv_sec*1000 + tval.tv_usec/1000;
	
#elif defined _MAC_OS
	struct timeval tval;
	gettimeofday(&tval, NULL);
	nTime = (VO_U64)(tval.tv_sec*1000 + tval.tv_usec/1000);	
#endif // _WIN32

	return nTime;
}

void voOS_SetTimePeriod(VO_BOOL bStartOrEnd, VO_U32 nPeriod)
{
#ifdef _WIN32
	if(bStartOrEnd)
		::timeBeginPeriod(nPeriod);
	else
		::timeEndPeriod(nPeriod);
#endif	// _WIN32
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
#endif // _LINUX

}

VO_U32 voOS_GetAppFolder (VO_PTCHAR pFolder, VO_U32 nSize)
{
#ifdef _METRO
	Platform::String^ fullPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
	_tcscpy (pFolder , fullPath->Data());
	return 0;
#elif defined(_WIN32)
	GetModuleFileName (NULL, pFolder, nSize);
#elif defined _LINUX
	VO_S32 r = readlink("/proc/self/exe", pFolder, nSize);
	if (r < 0 || r >= (VO_S32)nSize)
		return r;
	pFolder[r] = '\0';
#elif defined _MAC_OS
	// tag:20110123
	const char* dir = [[[NSBundle mainBundle] bundlePath] UTF8String];
	vostrcpy(pFolder, dir);
	//vostrcpy(pFolder, "/Library/Internet Plug-Ins/");
#elif defined _IOS
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString *filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
	strcpy(pFolder, [filePath UTF8String]);
	//NSString* dir = [[NSBundle mainBundle] bundlePath];
#endif // _LINUX

    VO_PTCHAR pPos = vostrrchr(pFolder, _T('/'));
	if (pPos == NULL)
		pPos = vostrrchr(pFolder, _T('\\'));
    VO_S32 nPos = pPos - pFolder;
    pFolder[nPos+1] = _T('\0');

	return 0;
}

VO_U32 voOS_GetPluginModuleFolder(VO_PTCHAR pFolder, VO_U32 nSize)
{
#ifdef _MAC_OS
	VO_TCHAR szFullAppName[256];
	voOS_GetModuleFileName(NULL, szFullAppName, strlen(szFullAppName));
	
	if(vostrstr(szFullAppName, _T("Safari")))
		voOS_GetBundleFolderByIdentifier("com.visualon.safari", pFolder, nSize);
	else if(vostrstr(szFullAppName, _T("Firefox")))
		voOS_GetBundleFolderByIdentifier("com.visualon.firefox", pFolder, nSize);
	
	NSLog(@"bundle path = %s\n", pFolder);
	
	vostrcat(pFolder, "/Contents/MacOS/");
	
	NSLog(@"Plugin module folder path = %s\n", pFolder);
#endif
	
	return 0;
}

VO_U32	voOS_GetBundleFolderByIdentifier(VO_PTCHAR pIdentifier, VO_PTCHAR pFolder, VO_U32 nSize)
{
#ifdef _MAC_OS
	NSString* bundle_id = [[NSString alloc] initWithFormat: @"%s", pIdentifier];
	NSString* path = [[NSBundle bundleWithIdentifier: bundle_id] bundlePath];
	strcpy(pFolder, [path UTF8String]);
	[bundle_id release];
#endif
	return 0;
}

VO_U32 voOS_GetThreadTime (VO_PTR	hThread)
{
	VO_U32 nTime = 0;

#ifdef _METRO
#elif defined _WIN32
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
#endif // _WIN32

	return nTime;
}

VO_S32 voOS_EnableDebugMode (int nShowLog)
{ 
#ifdef _LINUX_ANDROID_NOUSE	
	int 			nRC = 0;   
	char 			szFile[256];
	char			szBuff[256];	
	struct stat 	stFile;
    time_t 			tNow;
    struct tm *		ptm;	
	
	strcpy (szFile,  "/data/local/voDebugFolder/vosystemtime.txt");
	nRC = stat (szFile, &stFile); 
	if (nRC != 0)
	{
		strcpy (szFile,  "/data/local/tmp/voDebugFolder/vosystemtime.txt");
		nRC = stat (szFile, &stFile); 		
		
		if (nRC != 0)
		{
			if (nShowLog > 0)
			{
				VOLOGI ("It could not get file time from file %s", szFile);
			}
			return 0;	
		}
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
	{
		VOLOGI ("File: %s,  Time: %d-%d-%d  %d:%d:%d", szBuff, nYear, nMonth, nDay, nHour, nMin, nSec);
	}
	
	if (nYear - 1900 != ptm->tm_year || nMonth != ptm->tm_mon + 1 || nDay != ptm->tm_mday)
		return 0;
		
	int nFileTime = nHour * 3600 + nMin * 60 + nSec;
	int nSysteimTime = ptm->tm_hour * 3600 + ptm->tm_min * 60 + ptm->tm_sec;
	

	if (nShowLog > 0)
	{
		VOLOGI ("File Time %d, System time %d, Offset: %d", nFileTime, nSysteimTime, abs (nSysteimTime - nFileTime));
	}
 
    if (abs (nSysteimTime - nFileTime) > 7200)
    	return 0;		 	
 
 	VOLOGW ("The EnableDebugMode successful !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	   
    return 1;
    
#else

	return 0;
	
#endif // _LINUX_ANDROID 
}

VO_U32 voOS_Log (char * pText)
{
//	FILE * hFile = fopen ("/data/local/volog.txt", "wb");
//		hFile = hFile;

//	fseek(hFile, 0, SEEK_END);

//	fwrite(pText,1, strlen (pText), hFile);

//	fclose (hFile);

	return 0;
}

VO_U32 voOS_GetApplicationID(VO_PTCHAR pAppID, VO_U32 nSize)
{
	int nRet = -1;
	
#ifdef _IOS
	NSString* pID = [[NSBundle mainBundle] bundleIdentifier];
	strcpy(pAppID, [pID UTF8String]);
	nRet = 0;
#elif defined _MAC_OS
	NSString* pID = [[NSBundle mainBundle] bundleIdentifier];
	strcpy(pAppID, [pID UTF8String]);
	nRet = 0;
#endif // _IOS
	
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
#elif defined(_WIN32)
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwNumberOfProcessors;
#endif
	return 1;
}

VO_U32	voOS_GetCPUFrequency ()
{
#ifdef _WIN32
#ifndef WINCE
    LARGE_INTEGER f;            
    QueryPerformanceFrequency(&f);
	return (int)f.LowPart;
#endif
#endif
	return 0;
}

#ifdef _WIN32
#ifndef WINCE
DWORD deax;
DWORD debx;
DWORD decx;
DWORD dedx;
void ExeCPUID(DWORD veax)  
{

	__asm
	{
		mov eax,veax
			cpuid
			mov deax,eax
			mov debx,ebx
			mov decx,ecx
			mov dedx,edx
	}
}
#endif
#endif

void voOS_GetCPUType(char *cpuString,int length)
{
#ifdef _WIN32
#ifndef WINCE
	const DWORD id = 0x80000002; 

	memset(cpuString,0,length);
	for(DWORD t = 0 ; t < 3 ; t++ )
	{
		ExeCPUID(id+t);
		memcpy(cpuString+16*t+ 0,&deax,4);
		memcpy(cpuString+16*t+ 4,&debx,4);
		memcpy(cpuString+16*t+ 8,&decx,4);
		memcpy(cpuString+16*t+12,&dedx,4);
	}
#endif
#endif
}

#if defined(_LINUX) 
struct cpu_info {
    long unsigned utime, ntime, stime, itime;
    long unsigned iowtime, irqtime, sirqtime;
};

static unsigned int gCpuSys, gCpuUsr, gOsDelay, gOsGrid;
static VO_BOOL*     gCpuGoing;
static int  set_cpu_values(cpu_info& inInfo)
{
	FILE* pFile = NULL;
	pFile = fopen("/proc/stat", "r");
	if(!pFile)
	{
		VOLOGE("Can not open the /proc/stat and set the values\n");
		return -1;
	}
    fscanf(pFile, "cpu  %lu %lu %lu %lu %lu %lu %lu", &inInfo.utime, &inInfo.ntime, &inInfo.stime,
            &inInfo.itime, &inInfo.iowtime, &inInfo.irqtime, &inInfo.sirqtime);
    fclose(pFile);
	return 0;
}
static void get_cpu()
{
	cpu_info old_cpu, new_cpu;
	long unsigned total_delta_time;
	memset(&old_cpu, 0, sizeof(cpu_info));
	memset(&new_cpu, 0, sizeof(cpu_info));
	int nOk = set_cpu_values(old_cpu);
	if(nOk < 0)
		return ;
	if(gOsDelay <=0)
		gOsDelay = 1;
	
	int nTimes = gOsDelay * 1000/ gOsGrid;
	for(int i=0; i < nTimes; i++)
	{
		if(*gCpuGoing == VO_FALSE)
			return;
		usleep(gOsGrid*1000);	
	}
	nOk = set_cpu_values(new_cpu);
	if(nOk < 0)
		return;
	
	total_delta_time = (new_cpu.utime + new_cpu.ntime + new_cpu.stime + new_cpu.itime
                        + new_cpu.iowtime + new_cpu.irqtime + new_cpu.sirqtime)
                     - (old_cpu.utime + old_cpu.ntime + old_cpu.stime + old_cpu.itime
                        + old_cpu.iowtime + old_cpu.irqtime + old_cpu.sirqtime);
	if(total_delta_time != 0)
	{
		gCpuUsr = ((new_cpu.utime + new_cpu.ntime) - (old_cpu.utime + old_cpu.ntime)) * 100  / total_delta_time;
		gCpuSys = ((new_cpu.stime ) - (old_cpu.stime)) * 100 / total_delta_time;
	}
	else
	{
		gCpuUsr = -1;
		gCpuSys = -1;
	}
}

static pthread_once_t           gCpuUThread = PTHREAD_ONCE_INIT;
#endif

#ifdef _WIN32
#ifndef WINCE
double m_fOldCPUIdleTime;
double m_fOldCPUKernelTime;
double m_fOldCPUUserTime;

double FileTimeToDouble(FILETIME &filetime)
{
	return (double)(filetime.dwHighDateTime * 4.294967296E9) + (double)filetime.dwLowDateTime;
}
BOOL Initialize() 
{
	FILETIME ftIdle, ftKernel, ftUser;
	BOOL flag = FALSE;
	if (flag = GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
	{
		m_fOldCPUIdleTime = FileTimeToDouble(ftIdle);
		m_fOldCPUKernelTime = FileTimeToDouble(ftKernel);
		m_fOldCPUUserTime = FileTimeToDouble(ftUser);
	}
	return flag;
}
int GetCPUUseRate()
{
	Sleep(20);
	int nCPUUseRate = -1;
	FILETIME ftIdle, ftKernel, ftUser;
	if (GetSystemTimes(&ftIdle, &ftKernel, &ftUser))
	{
			double fCPUIdleTime = FileTimeToDouble(ftIdle);
			double fCPUKernelTime = FileTimeToDouble(ftKernel);
			double fCPUUserTime = FileTimeToDouble(ftUser);
			nCPUUseRate= (int)(fCPUKernelTime - m_fOldCPUKernelTime + fCPUUserTime - m_fOldCPUUserTime - fCPUIdleTime + m_fOldCPUIdleTime)*100 / (fCPUKernelTime - m_fOldCPUKernelTime + fCPUUserTime - m_fOldCPUUserTime);
			m_fOldCPUIdleTime = fCPUIdleTime;
			m_fOldCPUKernelTime = fCPUKernelTime;
			m_fOldCPUUserTime = fCPUUserTime;
	}
	return nCPUUseRate;
}

//------------------------------------------------------------------------------------------------------------------
int getcpuload()
{
  Sleep(500);
  static PDH_STATUS            status;
  static PDH_FMT_COUNTERVALUE  value;
  static HQUERY                query;
  static HCOUNTER              counter;
  static DWORD                 ret;
  static char                  runonce=1;
  int                         cput=0;

  if(runonce)
  {
    status = PdhOpenQuery(NULL, 0, &query);
    if(status != ERROR_SUCCESS)
    {
      printf("PdhOpenQuery() ***Error: 0x%X\n",status);
      return 0;
    }

    PdhAddCounter(query, TEXT("\\Processor(_Total)\\% Processor Time"),0,&counter); // A total of ALL CPU's in the system
    //PdhAddCounter(query, TEXT("\\Processor(0)\\% Processor Time"),0,&counter);    // For systems with more than one CPU (Cpu0)
    //PdhAddCounter(query, TEXT("\\Processor(1)\\% Processor Time"),0,&counter);    // For systems with more than one CPU (Cpu1)
    runonce=0;
    PdhCollectQueryData(query); // No error checking here
    return 0;
  }

  status = PdhCollectQueryData(query);
  if(status != ERROR_SUCCESS)
  {
    printf("PhdCollectQueryData() ***Error: 0x%X\n",status);
    if(status==PDH_INVALID_HANDLE) 
      printf("PDH_INVALID_HANDLE\n");
    else if(status==PDH_NO_DATA)
      printf("PDH_NO_DATA\n");
    else
      printf("Unknown error\n");
    return 0;
  }

  status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE | PDH_FMT_NOCAP100 ,&ret, &value);
  if(status != ERROR_SUCCESS)
  {
    printf("PdhGetFormattedCounterValue() ***Error: 0x%X\n",status);
    return 0;
  }
  cput = value.doubleValue;
  
  return cput;
}

#endif
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
	//nOk = pthread_once(&gCpuUThread, get_cpu);
        get_cpu();// the time two time invoke this method must over 1 seconds
	gCpuUThread = PTHREAD_ONCE_INIT;
	*pOutSys = gCpuSys;
	*pOutUsr = gCpuUsr;
#endif

#if defined(_WIN32)
#ifndef WINCE
	if (!Initialize())
	{
		printf("Error! %d\n", GetLastError());
		return -1;
	}
	else
	{	
			*pOutSys = 0;
			*pOutUsr = getcpuload();
	}
#endif
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

//#if 1
VO_S64 voOS_GetUTC(void)
{
#ifdef _METRO
	return -1;
#else //_METRO
#if defined(_LINUX_ANDROID) || defined(_WIN32)
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
#endif // _WIN32
    return VO_FALSE;
  }
  struct addrinfo * ptr_entry = ptr_ret;
  if((sockfd = socket(ptr_entry->ai_family, ptr_entry->ai_socktype, ptr_entry->ai_protocol)) == -1){
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
  if(connect(sockfd, ptr_entry->ai_addr, ptr_entry->ai_addrlen) == -1){
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

VO_S32 voOS_GetUTCServer(VO_PCHAR server, VO_PCHAR sPort)
{
#ifdef _WIN32
  return 1;
#else
  VO_CHAR BufLine[256], Index = '0';
  VO_PCHAR target;

  FILE * cfgFD = fopen("/data/local/tmp/timeServer.cfg", "r");
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
#endif //_WIN32
}
//#endif // 1


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
    return -1;
}
    
    
    
#ifdef _VONAMESPACE
}
#endif //_VONAMESPACE

