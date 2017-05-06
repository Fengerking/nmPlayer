/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef __MEM_ALIGN_H__
#define __MEM_ALIGN_H__
#include "jcommon.h"

extern void* voMemMalloc(Jpeg_DecOBJ * decoder,UINT32 size, UINT8 alignment);
void voMemFree(Jpeg_DecOBJ * decoder,void *mem_ptr);
extern void *mem_malloc(UINT32 size, UINT8 alignment);
extern void mem_free(void *mem_ptr);

#endif							/* __MEM_ALIGN_H__ */
