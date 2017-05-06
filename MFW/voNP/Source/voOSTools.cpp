	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voZipTools.mm

	Written by:	Jim Lin

	Change History (most recent first):
	2013-01-08		Jim			Create file

*******************************************************************************/

#ifdef _IOS
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/machine.h>
#include "iOSApiWrapper.h"
#endif

#include "voOSTools.h"
#include "voLog.h"
#include "voHalInfo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

int voCompress(unsigned char* pInput, int nInputSize, unsigned char* pOutput, int nOutputSize)
{
//    if(!pInput || !pOutput)
//        return -1;
//    
//    VOLOGI("[NPW]pInput:%d, %s\n", nInputSize, pInput);
//    
//    NSData *pInBuffer = [NSData dataWithBytes:pInput length:nInputSize];
//    
//    NSData *pOutDataCompress = [LFCGzipUtility gzipData:pInBuffer];
//    
//    if([pOutDataCompress length] > nOutputSize)
//    {
//        VOLOGE("[NPW]Output size error!!!");
//        return -1;
//    }
//    
//    [pOutDataCompress getBytes:pOutput length:[pOutDataCompress length]];
//    printf("[NPW]pOutput:%d, %s\n", [pOutDataCompress length], pOutput);
//    
//    if([pOutDataCompress length] > 0)
//        return [pOutDataCompress length];
    
    return -1;
}
        
bool IsHighPerformanceDevices()
{
#ifdef _IOS
    size_t nSize = 0;
    
    if(sysctlbyname("hw.machine", NULL, &nSize, NULL, 0) == -1)
    {
        VOLOGW("[NPW]Get device type length error!");
        return false;
    }
    
    char szChar[nSize];
    
    if (sysctlbyname("hw.machine", szChar, &nSize, NULL, 0) == -1)
    {
        VOLOGW("[NPW]Get device type error!");
        return false;
    }
        
    if (!strncmp(szChar, "iPhone1,1", strlen("iPhone1,1")))     {return false;} // @"iPhone1G";
    if (!strncmp(szChar, "iPhone1,2", strlen("iPhone1,2")))     {return false;} // @"iPhone3G";
    if (!strncmp(szChar, "iPhone2", strlen("iPhone2")))         {return false;} // @"iPhone3GS";
    
    //ipod family
    if (!strncmp(szChar, "iPod1", strlen("iPod1")))      {return false;} // @"iPod 1G";
    if (!strncmp(szChar, "iPod2", strlen("iPod2")))      {return false;} // @"iPod 2G";
    if (!strncmp(szChar, "iPod3", strlen("iPod3")))      {return false;} // @"iPod 3G";
    if (!strncmp(szChar, "iPod4", strlen("iPod4")))      {return false;} // @"iPod 4G";
    
    //ipad family
#endif
    
    return true;
}
    
bool GetCpuInfo(VONP_CPU_INFO* pInfo)
{
    VO_CPU_Info info;
    memset(&info, 0, sizeof(VO_CPU_Info));
    
    int nRet = get_cpu_info(&info);
    
    if(nRet < 0)
        return false;
    
    VOLOGI("[NPW]CPU Number is %d, freq %d", info.mCount, info.mMaxCpuSpeed);
    
    if (CPU_SUBTYPE_ARM_V7 <= info.mFeatures)
    {
        pInfo->nCPUType = 1;
    }
    else
    {
        pInfo->nCPUType = 0;
    }
    
    pInfo->nCoreCount = info.mCount;
    pInfo->nFrequency = info.mMaxCpuSpeed;
    pInfo->llReserved = info.mMinCpuSpeed;

    return true;
}
    
const char* GetOSVersion()
{
#ifdef _IOS
    return GetiOSVersion();
#endif
    
    return NULL;
}

#ifdef _VONAMESPACE
}
#endif
