/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		stream.h

Contains:       VOME API Buffer Operator Implement Header

Written by:	Huaping Liu

Change History (most recent first):
2009-05-21		LHP			Create file
*******************************************************************************/
#ifndef __STREAM_H__
#define __STREAM_H__

#include "voMem.h"
#define Frame_Maxsize  1024 * 10  //10K Work Buffer 
//#define Frame_MaxByte  40      
#define MIN(a,b)	 ((a) < (b)? (a) : (b))

typedef struct{
	unsigned char *set_ptr;
	unsigned char *frame_ptr;
	unsigned char *frame_ptr_bk;
	int  set_len;
	int  framebuffer_len; 
	int  frame_storelen;
	int  used_len;
}FrameStream;

void UpdateFrameBuffer(FrameStream *stream, VO_MEM_OPERATOR *pMemOP);
void InitFrameBuffer(FrameStream *stream);
void FlushFrameBuffer(FrameStream *stream);

#endif //__STREAM_H__

