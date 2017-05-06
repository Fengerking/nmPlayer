# ifndef LIBMP3_STREAM_H
# define LIBMP3_STREAM_H

#include "global.h"
#include "bit.h"

# define BUFFER_GUARD	8
# define BUFFER_MDLEN	(512 + 2048 + BUFFER_GUARD)
# define BUFFER_DATA	2048*32 + BUFFER_GUARD

typedef struct _FrameStream{
  unsigned char	*buffer;				
  unsigned char	*buffer_bk;				
  unsigned int	inlen;				
  unsigned char	*main_data;				
  unsigned char *this_frame;
  unsigned char *next_frame;
  unsigned int	length;
  int	storelength;
  int   usedlength;
  unsigned int 	md_len;					
  unsigned int	free_bitrates;
  Bitstream	bitptr;				
} FrameStream;

void voMP3DecStreamInit(FrameStream *fstream);
void voMP3DecStreamFinish(FrameStream *fstream);
int  voMP3DecStreamRefillBuffer(FrameStream* fstream, unsigned char const *psrc, unsigned int length);
void voMP3DecStreamBufferUpdata(FrameStream *fstream, unsigned int length);

# endif
