#if defined(_LINUX) && defined(__LINUX_ANDROID)
#  include <sys/system_properties.h>
#  include <cutils/log.h>
#endif

#if defined(_LINUX) && defined(__VOTT_ARM__)
#   include <machine/cpu-features.h>
#endif

#if defined(_LINUX)
#   include <unistd.h>
#   include <pthread.h>
#endif

#if defined(_IOS) || defined(_MAC_OS)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/machine.h>
#include "voLog.h"
#endif

#undef LOG_TAG
#define LOG_TAG "voHalInfo"

#include <stdio.h>
#include <stdlib.h>
#ifndef WINCE
#include <fcntl.h>
#include <errno.h>
#endif //WINCE

#include <assert.h>
#include "voLog.h"
#include "voHalInfo.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

#define STRLEN_CONST(x)  ((sizeof(x)-1)

class CCpuInfo
{
private:
	CCpuInfo();
public:
	static int getInfo(VO_CPU_Info* pInf);
private:
#if defined(_IOS) || defined(_MAC_OS)
    static int initIOS();
    
#elif defined(_LINUX)
	static pthread_once_t           mThread ;
	static int   read_file(const char*  pathname, char*  buffer, size_t  buffsize);
	static char* extract_cpuinfo_field(char* buffer, int buflen, const char* field);
	static int   count_cpu();
	static int   has_list_item(const char* list, const char* item);
	static void  cpu_init();

#if defined(__VOTT_PC__)
static __inline__ void x86_cpuid(int func, int values[4])
{
    int a, b, c, d;
	a = b = c = d = 0;
    /* We need to preserve ebx since we're compiling PIC code */
    /* this means we can't use "=b" for the second output register */
		/*
    __asm__ __volatile__ ( \
      "push %%ebx\n"
      "cpuid\n" \
      "mov %1, %%ebx\n"
      "pop %%ebx\n"
      : "=a" (a), "=r" (b), "=c" (c), "=d" (d) \
      : "a" (func) \
    );
		*/
    values[0] = a;
    values[1] = b;
    values[2] = c;
    values[3] = d;
}
#endif //__VOTT_PC__)

#endif //_LINUX	
	static CpuFamily                 mType;
	static unsigned long long         mFeatures;
	static unsigned int	             mCount;
	static unsigned int			      mMaxCpuSpeed;
	static unsigned int              mMinCpuSpeed;


};

CpuFamily           CCpuInfo::mType = CPU_FAMILY_UNKNOWN;
unsigned long long  CCpuInfo::mFeatures = 0;
unsigned int       CCpuInfo::mCount = 0;
unsigned int	    CCpuInfo::mMaxCpuSpeed = 0;
unsigned int       CCpuInfo::mMinCpuSpeed = 0;

#if defined(_LINUX)
pthread_once_t      CCpuInfo::mThread = PTHREAD_ONCE_INIT;
#endif


int CCpuInfo::getInfo(VO_CPU_Info* pInfo)
{
	int nOk = 0;
#if defined(_LINUX)
	if(0 == mCount)
		nOk = pthread_once(&mThread, cpu_init);
#endif
    
#if defined(_IOS) || defined(_MAC_OS)
    if (pInfo == NULL) {
        return -1;
    }
    if (0 == mCount) {
        nOk = initIOS();
    }
#else
	assert(pInfo != NULL);
#endif
	pInfo->mType = mType;
	pInfo->mFeatures = mFeatures;
	pInfo->mCount = mCount;
	pInfo->mMaxCpuSpeed = mMaxCpuSpeed;
	pInfo->mMinCpuSpeed = mMinCpuSpeed;

	return nOk;
}

#if defined(_IOS) || defined(_MAC_OS)

int voGetIOSCpuByDevice()
{
    char szChar[20];
    memset(szChar, 0, sizeof(szChar));
    voGetSysInfoStrByName("hw.machine", szChar, sizeof(szChar));
    
    //iphone family
    if (!strncmp(szChar, "iPhone1,1", strlen("iPhone1,1")))    {return 400000000;} // @"iPhone1G";
    if (!strncmp(szChar, "iPhone1,2", strlen("iPhone1,2")))    {return 400000000;} // @"iPhone3G";
    if (!strncmp(szChar, "iPhone2", strlen("iPhone2")))    {return 600000000;} // @"iPhone3GS";
    if (!strncmp(szChar, "iPhone3", strlen("iPhone3")))    {return 800000000;} // @"iPhone4";
    if (!strncmp(szChar, "iPhone4", strlen("iPhone4")))    {return 800000000;} // @"iPhone4S";
    if (!strncmp(szChar, "iPhone5", strlen("iPhone5")))    {return 1300000000;} // @"iPhone5";
    if (!strncmp(szChar, "iPhone6", strlen("iPhone6")))    {return 1300000000;} // @"iPhone5s";
    if (!strncmp(szChar, "iPhone", strlen("iPhone")))    {return 1300000000;} // @"iPhone";
    
    //ipod family
    if (!strncmp(szChar, "iPod1", strlen("iPod1")))      {return 400000000;} // @"iPod 1G";
    if (!strncmp(szChar, "iPod2", strlen("iPod2")))      {return 400000000;} // @"iPod 2G";
    if (!strncmp(szChar, "iPod3", strlen("iPod3")))      {return 600000000;} // @"iPod 3G";
    if (!strncmp(szChar, "iPod4", strlen("iPod4")))      {return 800000000;} // @"iPod 4G";
    if (!strncmp(szChar, "iPod5", strlen("iPod5")))      {return 800000000;} // @"iPod 5G";
    if (!strncmp(szChar, "iPod", strlen("iPod")))      {return 800000000;} // @"iPod";
    
    //ipad family
    if (!strncmp(szChar, "iPad1", strlen("iPad1")))      {return 1000000000;} // @"iPad 1G";
    if (!strncmp(szChar, "iPad2,1", strlen("iPad2,1")))  {return 1000000000;} // @"iPad 2G";
    if (!strncmp(szChar, "iPad2,2", strlen("iPad2,2")))  {return 1000000000;} // @"iPad 2G";
    if (!strncmp(szChar, "iPad2,3", strlen("iPad2,3")))  {return 1000000000;} // @"iPad 2G";
    if (!strncmp(szChar, "iPad2,4", strlen("iPad2,4")))  {return 1000000000;} // @"iPad 2G";
    if (!strncmp(szChar, "iPad2", strlen("iPad2")))  {return 1000000000;} // @"5->7: iPad mini";
    if (!strncmp(szChar, "iPad3,1", strlen("iPad3,1")))  {return 1000000000;} // @"iPad 3G";
    if (!strncmp(szChar, "iPad3,2", strlen("iPad3,2")))  {return 1000000000;} // @"iPad 3G";
    if (!strncmp(szChar, "iPad3,3", strlen("iPad3,3")))  {return 1000000000;} // @"iPad 3G";
    if (!strncmp(szChar, "iPad3", strlen("iPad3")))  {return 1400000000;} // @"4->6: iPad 4G";
    if (!strncmp(szChar, "iPad", strlen("iPad")))  {return 1400000000;} // @"iPad";
    
//    if (!strcmp(szChar, "i386"))         {return 0;} // @"Simulator";
//    if (!strcmp(szChar, "x86_64"))       {return 0;} // @"Simulator";
    
    return 1000000000;
}

int voGetSysInfoIntByName(const char *typeSpecifier)
{
    if (NULL == typeSpecifier) {
        return -1;
    }
    
    int nValue = 0;
    size_t nSize = sizeof(nValue);
    if (sysctlbyname(typeSpecifier, &nValue, &nSize, NULL, 0) == -1)
    {
        VOLOGE("voGetSysInfoIntByName sysctl()");
        return -1;
    }
    
    return nValue;
}
    
int64_t voGetSysInfoInt64ByName(const char *typeSpecifier)
{
    if (NULL == typeSpecifier) {
        return -1;
    }
    
    int64_t nValue = 0;
    size_t nSize = sizeof(nValue);
    if (sysctlbyname(typeSpecifier, &nValue, &nSize, NULL, 0) == -1)
    {
        VOLOGE("voGetSysInfoInt64ByName sysctl()");
        return -1;
    }
    
    return nValue;
}

int voGetSysInfoInt(int iName, int iSubName) //such as: CTL_HW, HW_CPU_FREQ
{
    int mib[2] = {iName, iSubName};
    
//    size_t nSize = 0;
//    
//    if (sysctl(mib, 2, NULL, &nSize, NULL, 0) == -1)
//    {
//        VOLOGE("voGetSysInfoInt sysctl()");
//        return -1;
//    }
    
    int nValue;
    size_t nSize = sizeof(nValue);
    
    if (sysctl(mib, 2, &nValue, &nSize, NULL, 0) == -1)
    {
        VOLOGE("voGetSysInfoInt sysctl()");
        return -1;
    }
    return nValue;
}

int voGetSysInfoStr(int iName, int iSubName, char *pOutGet, int iMaxSize)
{
    size_t nSize;
    int mib[2] = {iName, iSubName};
    
    if ((NULL == pOutGet) || (sysctl(mib, 2, NULL, &nSize, NULL, 0) == -1))
    {
        VOLOGE("voGetSysInfoStr sysctl()");
        return -1;
    }
    
    char szGet[nSize];
    
    if (sysctl(mib, 2, szGet, &nSize, NULL, 0) == -1)
    {
        VOLOGE("voGetSysInfoStr sysctl()");
        return -1;
    }
    
    if (nSize > iMaxSize) {
        nSize = iMaxSize;
    }
    
    strncpy(pOutGet, szGet, nSize);
    
    return 0;
}

int voGetSysInfoStrByName(const char *typeSpecifier, char *pOutGet, int iMaxSize)
{
    size_t nSize;
    
    if ((NULL == pOutGet) || (sysctlbyname(typeSpecifier, NULL, &nSize, NULL, 0) == -1))
    {
        VOLOGE("voGetSysInfoStrByName sysctlbyname()");
        return -1;
    }
    
    char szGet[nSize];
    
    if (sysctlbyname(typeSpecifier, szGet, &nSize, NULL, 0) == -1)
    {
        VOLOGE("voGetSysInfoStrByName sysctlbyname()");
        return -1;
    }
    
    strncpy(pOutGet, szGet, nSize);
    
    return 0;
}

int CCpuInfo::initIOS()
{
    int nRet = 0;
        
    mFeatures = voGetSysInfoIntByName("hw.cpusubtype");
    mCount = voGetSysInfoIntByName("hw.ncpu");
    mMaxCpuSpeed = voGetSysInfoInt64ByName("hw.cpufrequency_min");
	mMinCpuSpeed = voGetSysInfoInt64ByName("hw.cpufrequency_max");
    
    int iCpuType = voGetSysInfoIntByName("hw.cputype");
    
    switch (iCpuType) {
        case CPU_TYPE_ARM:
            mType = CPU_FAMILY_ARM;
            if (0 == mMinCpuSpeed || -1 == mMinCpuSpeed) {
                mMinCpuSpeed = voGetIOSCpuByDevice();
            }
            
            if (0 == mMaxCpuSpeed || -1 == mMaxCpuSpeed) {
                mMaxCpuSpeed = voGetIOSCpuByDevice();
            }
            break;
        case CPU_TYPE_X86:
        case CPU_TYPE_X86_64:
            mType = CPU_FAMILY_X86;
            break;
        default:
            mType = CPU_FAMILY_UNKNOWN;
            break;
    }
    
    mMinCpuSpeed = mMinCpuSpeed/1000;
    mMaxCpuSpeed = mMaxCpuSpeed/1000;
    
    return nRet;
}
#endif

#if defined(_LINUX)
int CCpuInfo::read_file(const char*  pathname, char*  buffer, size_t  buffsize)
{
	 int  fd, len;

    fd = open(pathname, O_RDONLY);
    if (fd < 0)
        return -1;

    do {
        len = read(fd, buffer, buffsize);
    } while (len < 0 && errno == EINTR);

    close(fd);

    return len;
}

char* CCpuInfo::extract_cpuinfo_field(char* buffer, int buflen, const char* field)
{
    int  fieldlen = strlen(field);
    char* bufend = buffer + buflen;
    char* result = NULL;
    int len;
    const char *p, *q;

    /* Look for first field occurence, and ensures it starts the line.
     */
    p = buffer;
    bufend = buffer + buflen;
    for (;;) {
        p = (const char*)memmem(p, bufend-p, field, fieldlen);
        if (p == NULL)
            goto EXIT;

        if (p == buffer || p[-1] == '\n')
            break;

        p += fieldlen;
    }

    /* Skip to the first column followed by a space */
    p += fieldlen;
    p  = (const char*)memchr(p, ':', bufend-p);
    if (p == NULL || p[1] != ' ')
        goto EXIT;

    /* Find the end of the line */
    p += 2;
    q = (const char*)memchr(p, '\n', bufend-p);
    if (q == NULL)
        q = bufend;

    /* Copy the line into a heap-allocated buffer */
    len = q-p;
    result = (char*)malloc(len+1);
    if (result == NULL)
        goto EXIT;

    memcpy(result, p, len);
    result[len] = '\0';

EXIT:
    return result;
}


int CCpuInfo::count_cpu()
{
	int nTemps[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21};
	char cCpuName[512];
	memset(cCpuName, 0, sizeof(cCpuName));
	for(int i = (sizeof(nTemps)/sizeof(nTemps[0])) - 1 ; i >= 0; i--)
	{
		sprintf(cCpuName, "/sys/devices/system/cpu/cpu%d", nTemps[i]);
		int nOk = access(cCpuName, F_OK);
		if( nOk == 0)
		{
			//VOLOGI("The device has %d CPU\n", (nTemps[i]+1));
			return nTemps[i]+1;
		}
	}
	return 1;
}

int CCpuInfo::has_list_item(const char* list, const char* item)
{
	const char*  p = list;
    int itemlen = strlen(item);

	//if(p)
	//	LOGE("%s::%s::%d list = %s itemlen = %d\n", __FILE__,__FUNCTION__, __LINE__, p, itemlen);
    if (list == NULL)
        return 0;

    while (*p) {
        const char*  q;

		
        /* skip spaces */
        while (*p == ' ' || *p == '\t')
            p++;

		// fix the Marvell crashing isssue
		usleep(1);
		//LOGE("%s::%s::%d\n", __FILE__,__FUNCTION__, __LINE__);
        /* find end of current list item */
        q = p;
        while (*q && *q != ' ' && *q != '\t')
            q++;

		usleep(1);
		//LOGE("%s::%s::%d p = %s itemlen = %d\n", __FILE__,__FUNCTION__, __LINE__, p, itemlen);
        if (p && itemlen == q-p && !memcmp(p, item, itemlen))
            return 1;

        /* skip to next item */
        p = q;
    }
    return 0;
}

void  CCpuInfo::cpu_init()
{
	char cpuinfo[10240];
    int  cpuinfo_len;

	mType= CPU_FAMILY_UNKNOWN;
    mFeatures = 0;
    mCount    = 1;
	
    cpuinfo_len = read_file("/proc/cpuinfo", cpuinfo, sizeof cpuinfo);

	

    if (cpuinfo_len < 0) {
        return;
    }

    mCount = count_cpu();
  

#if defined(__VOTT_ARM__) || defined(__VO_NDK__)
    {
	 mType = CPU_FAMILY_ARM;
 	    char* cpuArch = extract_cpuinfo_field(cpuinfo, cpuinfo_len, "CPU architecture");
	
		
        if (cpuArch != NULL) {
            char*  end;
            long   archNumber;
            int    hasARMv7 = 0;

             archNumber = strtol(cpuArch, &end, 10);

             if (end > cpuArch && archNumber >= 7) {
                hasARMv7 = 1;
            }
 	
            if (hasARMv7) {
                char* cpuProc = extract_cpuinfo_field(cpuinfo, cpuinfo_len,
                                                      "Processor");
                if (cpuProc != NULL) {
                     if (has_list_item(cpuProc, "(v6l)")) {
                        hasARMv7 = 0;
                    }
                    free(cpuProc);
                }
            }
			
            if (hasARMv7) {
                mFeatures |= CPU_ARM_FEATURE_ARMv7;
            }

            if (archNumber >= 6) {
                mFeatures |= CPU_ARM_FEATURE_LDREX_STREX;
            }
            free(cpuArch);
        }

     
		
        char* cpuFeatures = extract_cpuinfo_field(cpuinfo, cpuinfo_len, "Features");
        if (cpuFeatures != NULL) {

            if (has_list_item(cpuFeatures, "vfpv3"))
			{
                mFeatures |= CPU_ARM_FEATURE_VFPv3;
			}

            else if (has_list_item(cpuFeatures, "vfpv3d16"))
			{
                mFeatures |= CPU_ARM_FEATURE_VFPv3;
			}

            if (has_list_item(cpuFeatures, "neon")) 
			{
                mFeatures |= CPU_ARM_FEATURE_NEON | CPU_ARM_FEATURE_VFPv3;
            }
            free(cpuFeatures);
        }
    }
	
	cpuinfo_len = read_file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_max_freq", cpuinfo, sizeof cpuinfo);
	sscanf(cpuinfo, "%d", &mMaxCpuSpeed);

	cpuinfo_len = read_file("/sys/devices/system/cpu/cpu0/cpufreq/cpuinfo_min_freq", cpuinfo, sizeof cpuinfo);
	sscanf(cpuinfo, "%d", &mMinCpuSpeed);
	//VOLOGI("Max cpu is (%d) MinCPU is %d", mMaxCpuSpeed, mMinCpuSpeed);

#endif /* _ARM_VER || __VO_NDK__ */

#if defined(__VOTT_PC__)
	mType = CPU_FAMILY_X86;

    int regs[4];

/* According to http://en.wikipedia.org/wiki/CPUID */
#define VENDOR_INTEL_b  0x756e6547
#define VENDOR_INTEL_c  0x6c65746e
#define VENDOR_INTEL_d  0x49656e69

    x86_cpuid(0, regs);
    int vendorIsIntel = (regs[1] == VENDOR_INTEL_b &&
                         regs[2] == VENDOR_INTEL_c &&
                         regs[3] == VENDOR_INTEL_d);

    x86_cpuid(1, regs);
    if ((regs[2] & (1 << 9)) != 0) {
        mFeatures |= CPU_X86_FEATURE_SSSE3;
    }
    if ((regs[2] & (1 << 23)) != 0) {
        mFeatures |= CPU_X86_FEATURE_POPCNT;
    }
    if (vendorIsIntel && (regs[2] & (1 << 22)) != 0) {
        mFeatures |= CPU_X86_FEATURE_MOVBE;
    }
#endif // /*__VOTT_PC__*/
}

#endif // /*_LINUX*/

int get_cpu_info(VO_CPU_Info* pInfo)
{
    return CCpuInfo::getInfo(pInfo);
}
    
#ifdef _VONAMESPACE
}
#endif

