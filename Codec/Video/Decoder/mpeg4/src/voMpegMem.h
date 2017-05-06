/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_
#include "voMpeg4DecGlobal.h"

void *alignMalloc(VO_U32 size, VO_U32 alignment);
void alignFree(void *mem_ptr);

#endif							/* _MEM_ALIGN_H_ */
