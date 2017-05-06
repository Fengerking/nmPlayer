	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		pmem.c

	Contains:	physical memroy source file

	Written by:	East Zhou

	Change History (most recent first):
	2010-07-05		East		Create file

*******************************************************************************/
#include "pmem.h"

#ifdef _LINUX_ANDROID
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/android_pmem.h>
#include <unistd.h>
#include <fcntl.h>
#endif	//_LINUX_ANDROID

VO_S32 pmem_alloc(VO_PMEM* pPMem, VO_U32 nSize)
{
#ifdef _LINUX_ANDROID
	pPMem->nFD = open("/dev/pmem_adsp", O_RDWR);
	if(pPMem->nFD < 0)
		return -1;

	pPMem->pData = mmap(NULL, nSize, PROT_READ | PROT_WRITE, MAP_SHARED, pPMem->nFD, 0);
	if(pPMem->pData == MAP_FAILED)
	{
		close(pPMem->nFD);
		pPMem->nFD = -1;

		return -1;
	}
	pPMem->nSize = nSize;

	pmem_region sRegion;
	if(ioctl(pPMem->nFD, PMEM_GET_PHYS, &sRegion))
	{
		close(pPMem->nFD);
		pPMem->nFD = -1;

		munmap(pPMem->pData, pPMem->nSize);

		return -1;
	}

	pPMem->nPhysical = sRegion.offset;
#endif	//_LINUX_ANDROID

	return 0;
}

VO_VOID pmem_free(VO_PMEM* pPMem)
{
#ifdef _LINUX_ANDROID
	if(pPMem->nFD >= 0)
	{
		close(pPMem->nFD);
		pPMem->nFD = -1;

		munmap(pPMem->pData, pPMem->nSize);
	}
#endif	//_LINUX_ANDROID
}

VO_S32 pmem_getpagesize()
{
#ifdef _LINUX_ANDROID
	return sysconf(_SC_PAGESIZE);
#else	//_LINUX_ANDROID
	return 0;
#endif	//_LINUX_ANDROID
}