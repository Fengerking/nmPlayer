/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		g729_api.h

Contains:	G729A Codec global header

Written by:	Huaping Liu

Change History (most recent first):
2009-10-30		LHP			Create file

*******************************************************************************/

#ifndef   __G729_API_H__
#define   __G729_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmnMemory.h"

extern void *g_hG729EncInst;

typedef struct
{
	unsigned char* inbuf_ptr;
	unsigned char* outbuf_ptr;
	int inbuf_len;
	int outbuf_len;
	int PackType;
	VO_MEM_OPERATOR  *pvoMemop;
	VO_MEM_OPERATOR  voMemoprator;	
	void *hCheck;

}G729GlobalData;


#ifdef __cplusplus
}
#endif

#endif   //__G729_API_H__
