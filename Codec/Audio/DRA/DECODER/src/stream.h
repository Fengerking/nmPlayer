#ifndef LIBDRA_STREAM_H
#define LIBDRA_STREAM_H

#include "global.h"
#include "bit.h"

#define BUFFER_GUARD	8
#define BUFFER_DATA	2048 + BUFFER_GUARD

typedef struct _FrameStream{
  unsigned char	*buffer;				
  unsigned char	*buffer_bk;				
  unsigned int	inlen;				
  unsigned char *this_frame;
  unsigned char *next_frame;
  unsigned int	length;
  unsigned int	storelength;
  unsigned int  usedlength;				
  int		len;			
  Bitstream	bitptr;
} FrameStream;

void StreamInit(FrameStream *fstream);

# endif
