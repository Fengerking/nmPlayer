#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include "jcommon.h"

#define	INITCACHE(bcache)			\
	(bcache)->cache = 0;			\
	(bcache)->used_bits = 32;		\
	(bcache)->unread_marker = 0;	\


#define FILLSBUFFER(stream)							\
	{												\
	if((stream)->inFile == NULL)					\
	return -1;									\
	if(free_in_buffer > 0)	 \
{\
	vo_memmove((stream)->buffer_init, next_output_byte,      \
	free_in_buffer);\
}\
	readlen = fread((stream)->buffer_init + free_in_buffer,   \
	1, JPEG_BUFFER_LENGTH - free_in_buffer, (stream)->inFile); \
	if (readlen == 0)	return -1;										\
	(stream)->next_output_byte = 	(stream)->buffer_init;				\
	(stream)->free_in_buffer = readlen + free_in_buffer;		\
	(stream)->buffer_size = readlen + free_in_buffer;			\
	next_output_byte = (stream)->next_output_byte;				\
	free_in_buffer   = (stream)->free_in_buffer;				\
	}


#define UPDATACACHE(stream, n)  \
	used_bits += n;			\
	while(used_bits >= 8 && (!unread_marker))		\
	{							    	\
		if(free_in_buffer <= 0)	\
			FILLSBUFFER(stream);			\
		ch = *next_output_byte++;			\
		free_in_buffer--;					\
		if(ch == 0xFF)						\
		{									\
			do{								\
				if(free_in_buffer <= 0)		\
					FILLSBUFFER(stream);			\
				free_in_buffer--;					\
				ch = *next_output_byte++;			\
			} while (ch == 0xFF);					\
													\
			if(!ch)									\
			{										\
				ch = 0xFF;							\
			}										\
			else									\
			{										\
				unread_marker = ch;		\
				break;								\
			}										\
		}											\
														\
		used_bits -= 8;						\
		cache = (cache << 8) | ch;			\
	}											
  
#define	RELEASECACHE(bcache)					\
	(bcache)->cache = 0;						\
	(bcache)->used_bits = 32;					\
	(bcache)->unread_marker = 0;				

#define	GETCAVALUE(n, value) \
	value = ((cache << used_bits) >> (32-(n)))


#define BITGETCAVALUE(bitstream, val, tmp_v)				\
{															\
	tmp_v = cache << used_bits;								\
	UPDATACACHE(bitstream, val);							\
	tmp_v >>= 1;											\
	tmp_v ^= 0x80000000;									\
	tmp_v >>= (31-val);										\
	val = tmp_v - (tmp_v >> 31);							\
}


#define GETVLCVALUE(bitstream, index, tval, htbl)			\
		GETCAVALUE(VLC_BITS, index);						\
		tval = htbl->table[index];							\
		index = tval >> 12;									\
		if(index >= 12)										\
		{													\
			UPDATACACHE(bitstream, VLC_BITS);				\
			index = (tval >> 10) & 15;						\
			tval = (tval & 1023) << 3;						\
			GETCAVALUE(index, index);						\
			tval = htbl->table[tval+index];					\
			index = tval >> 12;								\
		}													\
		UPDATACACHE(bitstream, index);				\
		tval = tval & 0xFF;

#define FILLSBUFFER2(stream)							\
	{												\
	if((stream)->inFile == NULL)					\
		return -1;									\
	if((stream)->free_in_buffer > 0)				\
		memmove((stream)->buffer_init, (stream)->next_output_byte,      \
				(stream)->free_in_buffer);			\
	readlen = fread((stream)->buffer_init + (stream)->free_in_buffer,   \
					1, JPEG_BUFFER_LENGTH - (stream)->free_in_buffer, (stream)->inFile); \
	if (readlen == 0)	return -1;										\
	(stream)->next_output_byte = 	(stream)->buffer_init;				\
	(stream)->free_in_buffer = readlen + (stream)->free_in_buffer;		\
	(stream)->buffer_size = readlen + (stream)->free_in_buffer;			\
	}


#define	GETBYTE(stream, val)					\
	if((stream)->free_in_buffer <= 0)			\
	{											\
		FILLSBUFFER2(stream);					\
	}											\
	(stream)->free_in_buffer--;					\
	(val) = *(stream)->next_output_byte++;		
	

#define	GET2BYTE(stream, val)					\
	if((stream)->free_in_buffer-2 <= 0)			\
	{											\
		FILLSBUFFER2(stream);					\
	}											\
	(stream)->free_in_buffer -= 2;				\
	(val) = (*(stream)->next_output_byte) << 8;	\
	(stream)->next_output_byte++;				\
	(val) += *(stream)->next_output_byte++;		
	
#define UPDATACACHE2(stream, bcache, n)  \
	(bcache)->used_bits += n;			\
	while((bcache)->used_bits >= 8 && (!(bcache)->unread_marker))		\
	{							    	\
		if((stream)->free_in_buffer <= 0)	\
			FILLSBUFFER2(stream);			\
		ch = *(stream)->next_output_byte++;	\
		(stream)->free_in_buffer--;			\
		if(ch == 0xFF)						\
		{									\
			do{								\
				if((stream)->free_in_buffer <= 0)	\
					FILLSBUFFER2(stream);			\
				(stream)->free_in_buffer--;			\
				ch = *(stream)->next_output_byte++;	\
			} while (ch == 0xFF);					\
													\
			if(!ch)									\
			{										\
				ch = 0xFF;							\
			}										\
			else									\
			{										\
				(bcache)->unread_marker = ch;		\
				break;								\
			}										\
		}											\
														\
		(bcache)->used_bits -= 8;						\
		(bcache)->cache = ((bcache)->cache << 8) | ch;	\
	}							
	
#endif

