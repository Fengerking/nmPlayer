#ifndef ___BITS_H__
#define ___BITS_H__

#include "config.h"

typedef struct {
  unsigned char  *next_output_byte;
  unsigned int buffer_size;
  unsigned int free_in_buffer;	
  unsigned int bits_index;
  unsigned int cache;
}Bitstream;

int	initstream(Bitstream *stream, unsigned char* outBuffer, unsigned int length);
int	putbits(Bitstream *stream, unsigned int val, unsigned int n);


#endif