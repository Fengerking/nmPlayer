/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_
#include "jcommon.h"

extern void *mem_malloc(UINT32 size, UINT8 alignment);
extern void mem_free(void *mem_ptr);

#endif							/* _MEM_ALIGN_H_ */
