#ifndef _LIBDRA_BIT_H__
#define _LIBDRA_BIT_H__

#include "global.h"

typedef struct {
  unsigned char* byte;
  unsigned int   cache;
  int   bitsleft;
  int   nbytes;		
}Bitstream;

void	InitBits(Bitstream* bitptr, unsigned char* ptr, unsigned int length);
unsigned int  GetBits(Bitstream *bitptr, int length);
unsigned int  SkipBits(Bitstream *bitptr, int nBits);
int		CalcBitsUsed(Bitstream *bitptr, Bitstream *startptr);

#endif
