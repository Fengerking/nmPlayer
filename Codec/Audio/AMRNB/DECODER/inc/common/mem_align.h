/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef __MEM_ALIGN_H__
#define __MEM_ALIGN_H__

#include "voAMRNBDecID.h"
#include "voMem.h"

void *voAMRNBDecmem_malloc(VO_MEM_OPERATOR *pMemop, unsigned int size, unsigned char alignment);
void voAMRNBDecmem_free(VO_MEM_OPERATOR *pMemop, void *mem_ptr);

#endif							/* __MEM_ALIGN_H__ */
