#include "jcommon.h"

int	initstream(JPEG_STREAM *stream, UINT8* outBuffer, UINT32 length)
{
	stream->buffer_size = length;
	stream->free_in_buffer = length; 
	stream->next_output_byte = outBuffer;
//	stream->bits_index = 0;
//	stream->cache = 0;
	
	memset(outBuffer, 0, length);
	return 0;
}

int	putbyte(JPEG_STREAM *stream, UINT32 val)
{
	stream->free_in_buffer--;
	*stream->next_output_byte++ = (UINT8) val;

	return 0;
}

int	put2byte(JPEG_STREAM *stream, UINT32 val, UINT32 endianflag)
{
	if(endianflag)
	{
		putbyte(stream, val & 0xFF);
		putbyte(stream, (val >> 8) & 0xFF);		
	}
	else
	{
		putbyte(stream, (val >> 8) & 0xFF);
		putbyte(stream, val & 0xFF);
	}

	return 0;
}

int	put4byte(JPEG_STREAM *stream, UINT32 val, UINT32 endianflag)
{
	if(endianflag)
	{
		putbyte(stream, val & 0xFF);
		putbyte(stream, (val >> 8) & 0xFF);
		putbyte(stream, (val >> 16) & 0xFF);
		putbyte(stream, (val >> 24) & 0xFF);		
	}
	else
	{
		putbyte(stream, (val >> 24) & 0xFF);
		putbyte(stream, (val >> 16) & 0xFF);
		putbyte(stream, (val >> 8) & 0xFF);
		putbyte(stream, val & 0xFF);
	}

	return 0;
}


int putmarker(JPEG_STREAM *stream, JPEG_MARKER mark)
/* Emit a marker code */
{
	putbyte(stream, 0xFF);
	putbyte(stream, (UINT32) mark);
	
	return 0;
}

int	putbits(JPEG_STREAM *stream, UINT32 val, UINT32 n)
{
	UINT32 index;
	UINT8 ch;
	
	index = stream->bits_index;
	index += n;
	val <<= 24 - index;	
	val |= stream->cache;
	
	while (index >= 8) 
	{
		ch = (UINT8)((val >> 16) & 0xFF);

		stream->free_in_buffer--;		
		*stream->next_output_byte++ = ch;		
		if(ch == 0xFF)		
		{
			stream->free_in_buffer--;
			*stream->next_output_byte++ = 0;
		}
		val <<= 8;
		index -= 8;
	}

	stream->bits_index = index;
	stream->cache = val;

	return 0;
}

