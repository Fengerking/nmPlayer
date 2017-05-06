#include <stdlib.h>
#include <stdio.h>
#include "voHalInfo.h"
#include "voOSFunc.h"
int main(int argc, char** argv)
{
	VO_CPU_Info info;
	get_cpu_info(&info);
	switch(info.mType)
	{
	case CPU_FAMILY_ARM:
	{
		printf("It is arm architecutre\n");
		if(info.mFeatures & CPU_ARM_FEATURE_ARMv7)
			printf("it has CPU_ARM_FEATURE_ARMv7\n");
		if(info.mFeatures & CPU_ARM_FEATURE_VFPv3)
			printf("it has  CPU_ARM_FEATURE_VFPv3\n");
		if(info.mFeatures & CPU_ARM_FEATURE_NEON)
			printf("it has CPU_ARM_FEATURE_NEON\n");
		if(info.mFeatures & CPU_ARM_FEATURE_LDREX_STREX)
			printf("it has CPU_ARM_FEATURE_LDREX_STREX\n");
		break;
	}
	case CPU_FAMILY_X86:
	{
		printf("It is x86 architecutre\n");
		if(info.mFeatures & CPU_X86_FEATURE_SSSE3)
			printf("it has CPU_X86_FEATURE_SSSE3\n");
		if(info.mFeatures & CPU_X86_FEATURE_POPCNT)
			printf("it has  CPU_X86_FEATURE_POPCNT\n");
		if(info.mFeatures & CPU_X86_FEATURE_MOVBE)
			printf("it has CPU_X86_FEATURE_MOVBE\n");
		break;
	}
	default:
		printf("It is unknown architecutre\n");
		break;
	}
	printf("this device has %d CPUs\n", info.mCount);

	VO_U32 sys, usr;
	VO_BOOL bGoing = VO_TRUE;
	voOS_GetCpuUsage(&sys, &usr, &bGoing, 1, 50);
	printf("the cpu usage is %d, %d, %d\n", sys, usr, sys+usr);
}
