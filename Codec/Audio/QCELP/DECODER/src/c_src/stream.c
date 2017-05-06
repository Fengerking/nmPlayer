/************************************************************************
*									     *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2009	     *
*									     *
************************************************************************/
/*******************************************************************************
File:		stream.c

Contains:       VOME API Buffer Operator Implement Code

Written by:	Huaping Liu

Change History (most recent first):
2009-05-22		LHP			Create file
*******************************************************************************/
#include "stream.h"
void InitFrameBuffer(FrameStream *stream)
{
	stream->set_ptr = NULL;
    stream->frame_ptr_bk = stream->frame_ptr;
    stream->set_len = 0;
	stream->framebuffer_len = 0;
    stream->frame_storelen = 0;	
}
void UpdateFrameBuffer(FrameStream *stream, VO_MEM_OPERATOR *pMemOP)
{
        int  len;
        len  = MIN(Frame_Maxsize - stream->frame_storelen, stream->set_len);
        pMemOP->Copy(VO_INDEX_DEC_QCELP, stream->frame_ptr_bk + stream->frame_storelen , stream->set_ptr, len);
        stream->set_len -= len;
        stream->set_ptr += len;
        stream->framebuffer_len = stream->frame_storelen + len;
        stream->frame_ptr = stream->frame_ptr_bk;
		stream->used_len += len;
}
void FlushFrameBuffer(FrameStream *stream)
{
	stream->set_ptr = NULL;
    stream->set_len = 0;
	stream->framebuffer_len = 0;
    stream->frame_storelen = 0;	
}

