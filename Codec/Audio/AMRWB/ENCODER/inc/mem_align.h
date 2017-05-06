	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2010			*
	*																		*
	************************************************************************/

#ifndef __MEM_ALIGN_H__
#define __MEM_ALIGN_H__

#include "voAMRWBEncID.h"
#include "voMem.h"

void *voAMRWBEncMem_malloc(VO_MEM_OPERATOR *pMemop, unsigned int size, unsigned char alignment);
void voAMRWBEncMem_free(VO_MEM_OPERATOR *pMemop, void *mem_ptr);

#endif							/* __MEM_ALIGN_H__ */
