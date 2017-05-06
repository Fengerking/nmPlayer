#ifndef __LIBFLAC_STREAM_H__
#define __LIBFLAC_STREAM_H__

#include	<stdlib.h>
#include	<string.h>
#include    "bit.h"
#include    "voMem.h"
#include    "mem_align.h"
#include    "voFlacDecID.h"

#define BUFFER_GUARD	8

typedef struct _FrameStream{
  unsigned char	*buffer;				
  unsigned char	*buffer_bk;				
  unsigned char	*bufend;
  unsigned char *str_ptr;                 //input_buffer pointer
  int           str_len;                  //input_buffer length
  int			maxframesize;
  int           used_len;
  Bitstream		bitptr;	
  Bitstream     PostBitptr;
}FrameStream;

void voFLACDecStreamInit(FrameStream *fstream);
void voFLACDecStreamFinish(FrameStream *fstream, VO_MEM_OPERATOR *pMemOP);
void voFLACDecStreamBufferUpdata(FrameStream *fstream, int length);
void voFLACDecStreamFlush(FrameStream *fstream);
int  voFLACDecInitStreamBuf(FrameStream *stream, int MaxFrameSize, VO_MEM_OPERATOR *pMemOP);
//int  StreamRefillBuffer(FrameStream* fstream, unsigned char *psrc, int length);

#endif //__LIBFLAC_STREAM_H__
