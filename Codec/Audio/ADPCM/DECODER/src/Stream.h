#ifndef __ASTREAM_H___
#define __ASTREAM_H___

#include "voAdpcmDecID.h"

#define			IN_BUFFERSIZE		8*1024

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
} FrameStream;

void voADPCMDecStreamInit(FrameStream *fstream);

#endif //__ASTREAM_H___