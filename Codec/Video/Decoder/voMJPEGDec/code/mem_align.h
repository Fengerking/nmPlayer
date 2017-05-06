/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef __MEM_ALIGN_H__
#define __MEM_ALIGN_H__
#include "jcommon.h"

extern void *mem_malloc(UINT32 size, UINT8 alignment);
extern void mem_free(void *mem_ptr);

#endif							/* __MEM_ALIGN_H__ */
