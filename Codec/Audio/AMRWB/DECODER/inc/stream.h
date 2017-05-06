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

#include "voAMRWBDecID.h"
#include "voMem.h"
#define Frame_Maxsize  1 * 1024 
#define Frame_MaxByte  62      //AMRWB full bitrate 62Bytes/frame
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

void voAMRWBDecUpdateFrameBuffer(FrameStream *stream,VO_MEM_OPERATOR *pMemOP);
void voAMRWBDecInitFrameBuffer(FrameStream *stream);
void voAMRWBDecFlushFrameBuffer(FrameStream *stream);

#endif //__STREAM_H__

