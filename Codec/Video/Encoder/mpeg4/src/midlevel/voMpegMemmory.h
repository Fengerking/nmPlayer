/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _MEM_ALIGN_H_
#define _MEM_ALIGN_H_

#include "../voMpegProtect.h"
#include "../voMpegEnc.h"

/*The alignment must be multiple of 4.*/
VO_VOID *voMpegMalloc(ENCHND *enc_hnd, VO_U32 size, VO_U8 alignment);
VO_VOID voMpegFree   (ENCHND *enc_hnd, VO_VOID *mem_ptr);

#endif							/* _MEM_ALIGN_H_ */
