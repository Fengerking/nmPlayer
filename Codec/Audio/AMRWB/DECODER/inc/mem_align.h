/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef __MEM_ALIGN_H__
#define __MEM_ALIGN_H__

#include "voAMRWBDecID.h"
#include "voMem.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void *voAMRWBDec_mem_malloc(VO_MEM_OPERATOR *pMemop, unsigned int size, unsigned char alignment);
void voAMRWBDec_mem_free(VO_MEM_OPERATOR *pMemop, void *mem_ptr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif							/* __MEM_ALIGN_H__ */
