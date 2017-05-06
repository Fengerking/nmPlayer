	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		pmem.h

	Contains:	physical memory header file

	Written by:	East Zhou

	Change History (most recent first):
	2010-07-05		East		Create file

*******************************************************************************/

#ifndef __PHYSICAL_MEMORY_H__
#define __PHYSICAL_MEMORY_H__

#include "voType.h"

typedef struct VO_PMEM
{
	VO_PTR				pData;
	VO_S32				nFD;
	VO_U32				nPhysical;
	VO_U32				nSize;
} VO_PMEM;

VO_S32	pmem_alloc(VO_PMEM* pPMem, VO_U32 nSize);
VO_VOID	pmem_free(VO_PMEM* pPMem);
VO_S32	pmem_getpagesize();

#endif // __PHYSICAL_MEMORY_H__
