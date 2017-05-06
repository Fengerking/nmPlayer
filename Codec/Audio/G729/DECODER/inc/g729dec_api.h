/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		g729dec_api.h

Contains:	G729AB Decode Codec global header

Written by:	Huaping Liu

Change History (most recent first):
2009-11-02		LHP			Create file

*******************************************************************************/

#ifndef __G729DEC_API_H__
#define __G729DEC_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmnMemory.h"
#include "typedef.h"

extern void *g_hG729DecInst;

typedef struct{
	unsigned char *inbuf_ptr;
	unsigned char *outbuf_ptr;
	int inbuf_len;
	int outbuf_len;
	int PackType;
	int bad_lsf;
	VO_MEM_OPERATOR  *pvoMemop;
	VO_MEM_OPERATOR  voMemoprator;	
	void *hCheck;
}G729DGlobalData;

#ifdef __cplusplus
}
#endif

#endif   //__G729DEC_API_H__

