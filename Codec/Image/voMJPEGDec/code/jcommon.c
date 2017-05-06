#if !( defined(LINUX) || defined(_IOS))
#include <tchar.h>
#endif
#include <string.h>
#include "jcommon.h"
#include "mem_align.h"


void* vo_memmove(void *dest, const void *src,int n) 
{ 
	char *psrc = (char*)src; 
	char *pdest = (char*)dest;
	int i;

	if (n == 0) return 0; 
	if (dest == NULL) return 0; 
	if (src == NULL)    return 0; 

	if((pdest <= psrc) || (pdest >= psrc + n))
	{ 
		for(i=0; i < n; i++)
		{ 
			*pdest = *psrc; 
			psrc++; 
			pdest++; 
		} 
	} 
	else 
	{ 
		psrc += n; 
		pdest += n; 
		for(i = 0;i < n; i++) 
		{ 
			psrc--; 
			pdest--; 
			*pdest = *psrc; 
		} 
	} 
	return dest;
}


INT32	initstream(JPEG_STREAM *stream, UINT8* outBuffer, UINT32 length)
{
	stream->buffer_size = length;
	stream->free_in_buffer = length; 
	stream->input_data_size = length;//huwei 20091019 add input buffer
	stream->next_output_byte = outBuffer;
	stream->buffer_init_pos = outBuffer;//huwei 20091019 add input buffer
	stream->bits_index = 0;
	stream->inFile = 0;
	stream->buffer_init = 0;

	return 0;
}

INT32 initfstream(Jpeg_DecOBJ* decoder, JPEG_STREAM *stream, char* pfilepath)
{
	FILE* pfile;
	
	if(stream->inFile)
	{
		fclose(stream->inFile);
		stream->inFile = NULL;
	}

#if ( defined(LINUX) || defined(_IOS))
	pfile = fopen(pfilepath, "rb");
	if(pfile == NULL)
		return -2;
#else
	pfile = _wfopen((TCHAR *)pfilepath, L"rb");
	if(pfile == NULL)
		return -2;
#endif

	fflush(pfile);

	stream->bits_index = 0;
	stream->buffer_size = 0;
	stream->free_in_buffer = 0; 
	
	/*if(decoder->memoryOperator.Alloc)
	{
		if(stream->buffer_init == NULL)
		{
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = JPEG_BUFFER_LENGTH;
			decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
			stream->buffer_init = MemInfo.VBuffer;
		}
	} 
	else
	{
		if(stream->buffer_init == NULL)
			stream->buffer_init = mem_malloc(JPEG_BUFFER_LENGTH, DCACHE_ALIGN_NUM);
	}*/
    if(stream->buffer_init == NULL)
        stream->buffer_init = (UINT8*)voMemMalloc(decoder,JPEG_BUFFER_LENGTH, DCACHE_ALIGN_NUM);

	if(stream->buffer_init == NULL)
		return -1;

	stream->inFile = pfile;

	return fillbuffer(stream);
}

INT32 initfstream2(Jpeg_DecOBJ* decoder, JPEG_STREAM *stream, FILE* infile)
{
	FILE* pfile;
	
	if(stream->inFile)
	{
		fclose(stream->inFile);
		stream->inFile = NULL;
	}

//#ifdef LINUX
//#warning "using LINUX"
//	pfile = fopen(pfilepath, "rb");
//	if(pfile == NULL)
//		return -2;
//#else
//	pfile = _wfopen((TCHAR *)pfilepath, L"rb");
//	if(pfile == NULL)
//		return -2;
//#endif
	pfile = infile; 

	fflush(pfile);

	stream->bits_index = 0;
	stream->buffer_size = 0;
	stream->free_in_buffer = 0; 
	
	/*if(decoder->memoryOperator.Alloc)
	{
		if(stream->buffer_init == NULL)
		{
			VO_MEM_INFO MemInfo;

			MemInfo.Flag = 0;
			MemInfo.Size = JPEG_BUFFER_LENGTH;
			decoder->memoryOperator.Alloc(VO_INDEX_DEC_JPEG, &MemInfo);
			stream->buffer_init = MemInfo.VBuffer;
		}
	} 
	else
	{
		if(stream->buffer_init == NULL)
			stream->buffer_init = mem_malloc(JPEG_BUFFER_LENGTH, DCACHE_ALIGN_NUM);
	}*/

    if(stream->buffer_init == NULL)
        stream->buffer_init = voMemMalloc(decoder,JPEG_BUFFER_LENGTH, DCACHE_ALIGN_NUM);

	if(stream->buffer_init == NULL)
		return -1;

	stream->inFile = pfile;

	return fillbuffer(stream);
}

INT32	feekstream(JPEG_STREAM *stream,   UINT32 length)
{
	INT32 ret;

	stream->bits_index = 0;
	stream->buffer_size = 0;
	stream->free_in_buffer = 0; 

	ret = fseek(stream->inFile, length, SEEK_SET);
	if(ret < 0)
		return -1;

	return fillbuffer(stream);
}

/*huwei 20091019 add input buffer*/
INT32	seekstream(JPEG_STREAM *stream,   UINT32 length)
{
	stream->bits_index = 0;
	stream->buffer_size = 0;
	stream->inFile = 0;
	stream->free_in_buffer = 0;
	stream->buffer_init = 0;

	stream->buffer_size = stream->input_data_size - length;
	stream->free_in_buffer = stream->input_data_size - length; 

	stream->next_output_byte =  stream->buffer_init_pos + length;

	return 0;
}

INT32	releasestream(Jpeg_DecOBJ* decoder, JPEG_STREAM *stream)
{
	if(stream->buffer_init)
	{
		/*if(decoder->memoryOperator.Free)
		{
			decoder->memoryOperator.Free(VO_INDEX_DEC_JPEG, stream->buffer_init);
		} 
		else
		{
			mem_free(stream->buffer_init);
		}
		
		stream->buffer_init = NULL;*/
        voMemFree(decoder,stream->buffer_init);
	}

	if(stream->inFile)
	{
		fclose(stream->inFile);
		stream->inFile = NULL;
	}

	stream->bits_index = 0;
	stream->buffer_size = 0;
	stream->free_in_buffer = 0; 
	stream->next_output_byte = NULL;
	return 0;
}
	
INT32	fillbuffer(JPEG_STREAM * stream)
{
	UINT32 readlen;

	if(stream->inFile == NULL)
		return -2;

	if(stream->free_in_buffer > 0)
		vo_memmove(stream->buffer_init, stream->next_output_byte, stream->free_in_buffer);
	else
		stream->free_in_buffer = 0;

	readlen = fread(stream->buffer_init + stream->free_in_buffer, 1, 
				JPEG_BUFFER_LENGTH - stream->free_in_buffer, stream->inFile);
	if(readlen == 0) return -1;
	
	stream->next_output_byte = 	stream->buffer_init;
	stream->free_in_buffer = readlen + stream->free_in_buffer;
	stream->buffer_size = readlen + stream->free_in_buffer;	

	return 0;
}

INT32	getbyte(JPEG_STREAM *stream, UINT32* val, UINT32 bigend)
{
	INT32 ret;
	if((stream->free_in_buffer - 1) < 0)
	{
		ret = fillbuffer(stream);
		if(ret) return -1;
	}

	stream->free_in_buffer --;
	*val = *(stream)->next_output_byte++;
	
	return 0;
}

INT32	get2byte(JPEG_STREAM *stream, UINT32* val, UINT32 bigend)
{
	INT32 ret;
	
	if((stream->free_in_buffer - 2) <= 0)
	{
		ret = fillbuffer(stream);
		if(ret) return -1;
	}

	stream->free_in_buffer -= 2;
	
	if(bigend)
	{
		*val = (*stream->next_output_byte) << 8;	
		stream->next_output_byte++;
		*val += *stream->next_output_byte++;	
	}
	else
	{
		*val = (*stream->next_output_byte);	
		stream->next_output_byte++;
		*val += (*stream->next_output_byte) << 8;	
		stream->next_output_byte++;
	}
	
	return 0;
}

INT32	get4byte(JPEG_STREAM *stream, UINT32* val, UINT32 bigend)
{
	INT32 ret;

	if((stream->free_in_buffer - 4) <= 0)
	{
		ret = fillbuffer(stream);
		if(ret) return -1;
	}

	stream->free_in_buffer -= 4;
	
	if(bigend)
	{
		*val = (*stream->next_output_byte) << 24;	
		stream->next_output_byte++;
		*val |= (*stream->next_output_byte) << 16;
		stream->next_output_byte++;
		*val |= (*stream->next_output_byte) << 8;	
		stream->next_output_byte++;
		*val |= *stream->next_output_byte++;		
	}
	else
	{
		*val = (*stream->next_output_byte);	
		stream->next_output_byte++;
		*val |= (*stream->next_output_byte) << 8;	
		stream->next_output_byte++;
		*val |= (*stream->next_output_byte) << 16;	
		stream->next_output_byte++;
		*val |= (*stream->next_output_byte) << 24;	
		stream->next_output_byte++;
	}

	return 0;
}




