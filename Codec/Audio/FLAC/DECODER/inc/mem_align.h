/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef __MEM_ALIGN_H__
#define __MEM_ALIGN_H__

#include "voMem.h"

void *voFLACDecmem_malloc(VO_MEM_OPERATOR *pMemop, unsigned int size, unsigned char alignment);
void voFLACDecmem_free(VO_MEM_OPERATOR *pMemop, void *mem_ptr);

#endif							/* __MEM_ALIGN_H__ */
