#include "Bits.h"

int	initstream(Bitstream *stream, unsigned char* outBuffer, unsigned int length)
{
	stream->buffer_size = length;
	stream->free_in_buffer = length; 
	stream->next_output_byte = outBuffer;
	stream->bits_index = 0;
	stream->cache = 0;	

	return 0;
}

int	putbits(Bitstream *stream, unsigned int val, unsigned int n)
{
	unsigned int  index;
	unsigned char  ch;

	if (n<0 || n>32)
		return -1;
	
	if (n < 32 && val > ((unsigned long)1<<n)-1)
		return -1;
	
	if (n == 0)
		return 0;
	
	index = stream->bits_index;
	index += n;
	val <<= 32 - index;	
	val |= stream->cache;
	
	while (index >= 8) 
	{
		ch = (unsigned char)((val >> 24) & 0xFF);

		stream->free_in_buffer--;		
		*stream->next_output_byte++ = ch;		
		val <<= 8;
		index -= 8;
	}

	stream->bits_index = index;
	stream->cache = val;

	return 0;
}
