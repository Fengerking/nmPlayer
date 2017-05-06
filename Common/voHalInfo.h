/*********************************************************************************
 * Aim: define a abtract level to get hardware informaion
 *
**********************************************************************************/

#ifndef __VO_HAL_INFO__
#define __VO_HAL_INFO__
    
#ifdef _VONAMESPACE
    namespace _VONAMESPACE {
#else
#ifdef __cplusplus
        extern "C" {
#endif /* __cplusplus */
#endif

typedef enum {
    CPU_FAMILY_UNKNOWN = 0,
    CPU_FAMILY_ARM,
    CPU_FAMILY_X86,

    CPU_FAMILY_MAX  /* do not remove */

} CpuFamily;

	
enum {
    CPU_ARM_FEATURE_ARMv7       = (1 << 0),
    CPU_ARM_FEATURE_VFPv3       = (1 << 1),
    CPU_ARM_FEATURE_NEON        = (1 << 2),
    CPU_ARM_FEATURE_LDREX_STREX = (1 << 3),
};	

enum {
    CPU_X86_FEATURE_SSSE3  = (1 << 0),
    CPU_X86_FEATURE_POPCNT = (1 << 1),
    CPU_X86_FEATURE_MOVBE  = (1 << 2),
};
	
struct VO_CPU_Info
{
	CpuFamily              mType;
	unsigned long long      mFeatures; //iOS reference to <mach/machine.h>
	unsigned int           mCount;
	unsigned int			mMaxCpuSpeed;
	unsigned int			mMinCpuSpeed;
};

#if defined(_IOS) || defined(_MAC_OS)
/* ret: 0 is success*/
int voGetSysInfoIntByName(const char *typeSpecifier);
int voGetSysInfoInt(int iName, int iSubName); //such as: CTL_HW, HW_CPU_FREQ
int voGetSysInfoStr(int iName, int iSubName, char *pOutGet, int iMaxSize);
int voGetSysInfoStrByName(const char *typeSpecifier, char *pOutGet, int iMaxSize);
int64_t voGetSysInfoInt64ByName(const char *typeSpecifier);
#endif

int get_cpu_info(VO_CPU_Info* pInf);

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

#endif //  __VO_HAL_INFO__
//Hardware level information
