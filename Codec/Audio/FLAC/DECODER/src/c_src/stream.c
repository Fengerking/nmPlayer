# include <stdlib.h>
# include "stream.h"

void voFLACDecStreamInit(FrameStream *stream)
{
	stream->buffer = 0;
	stream->bufend = 0;
	stream->buffer_bk = 0;
	stream->maxframesize = 0;
	stream->str_ptr = 0;
	stream->str_len = 0;
	voFLACDecInitBits(&stream->bitptr, 0, 0);
}

int voFLACDecInitStreamBuf(FrameStream *stream, int MaxFrameSize, VO_MEM_OPERATOR *pMemOP)
{
	if(stream->maxframesize < MaxFrameSize*2)
	{
		stream->maxframesize = MaxFrameSize*2;
		if(stream->buffer_bk)
			voFLACDecmem_free(pMemOP, stream->buffer_bk);
	
		stream->buffer_bk = (unsigned char *)voFLACDecmem_malloc(pMemOP, stream->maxframesize, 32);
		if(stream->buffer_bk == NULL)
			return -1;
	}	
	stream->buffer = stream->buffer_bk;
	stream->bufend = stream->buffer;
	return 0;
}

void voFLACDecStreamFinish(FrameStream *fstream, VO_MEM_OPERATOR *pMemOP)
{
	if(fstream->buffer_bk)
	{
		voFLACDecmem_free(pMemOP, fstream->buffer_bk);
		fstream->buffer_bk = NULL;
	}

	fstream->buffer = 0;
	fstream->bufend = 0;
	fstream->maxframesize = 0;
}

//int StreamRefillBuffer(FrameStream *stream, unsigned char *buffer, int length)
//{
//	int len;
//	
//	if(!length)
//		return length;
//
//	len  = stream->bufend - stream->buffer_bk;
//	if(len + length > stream->maxframesize)	
//	{
//		StreamBufferUpdata(stream, 0);
//	}
//
//	len = stream->bufend - stream->buffer_bk;
//	if(len + length > stream->maxframesize)
//		len = stream->maxframesize - len;
//	else
//		len = length;
//
//	memcpy(stream->bufend, buffer, len);
//	
//	stream->bufend += len;
//
//	return len;
//}

void voFLACDecStreamBufferUpdata(FrameStream *stream, int length)
{
	int len; // = stream->bufend - stream->buffer;
	stream->buffer += length;
	len = stream->bufend - stream->buffer;

	//if(length)
	//{
	//	stream->buffer += length;
	//}
	//else
	//{
	if(len > 0)
		memmove(stream->buffer_bk, stream->buffer, len);
	stream->buffer = stream->buffer_bk;
	stream->bufend = stream->buffer + len;
	//}
}


void voFLACDecStreamFlush(FrameStream *stream)
{
	stream->buffer = stream->buffer_bk;
	stream->bufend = stream->buffer;
}