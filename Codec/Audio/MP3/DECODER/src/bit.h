#ifndef _LIBMP3_BIT_H__
#define _LIBMP3_BIT_H__

#include "global.h"

typedef struct {
  unsigned char *byte;
  unsigned int  cache;
  int   bitsleft;
  int   nbytes;		
}Bitstream;

void	voMP3DecInitBits(Bitstream* bitptr, unsigned char  *ptr, unsigned int length);
unsigned int    voMP3DecGetBits(Bitstream *bitptr, int length);
unsigned int    voMP3DecSkipBits(Bitstream *bitptr, int nBits);
int		voMP3DecCalcBitsUsed(Bitstream *bitptr, Bitstream *startptr);
unsigned int    voMP3DecBits_Crc(Bitstream *bitptr, unsigned int   length, unsigned int  check_init);

#endif
