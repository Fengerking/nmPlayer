#include "jconfig.h"
#include "jcommon.h"
#include "jencoder.h"

#ifdef IPP_SUPPORT
INT32	jpeg_encodehuffman(JPEG_STREAM*			bitstream,
						   JPEG_MCU_BLOCK*		outblock,
						   JPEG_HUFF_ENTROPY*	entropy)
{
	UINT32 bc;
	
	for(bc = 0; bc < outblock->lumblockNum; bc++)
	{
		ippiEncodeHuffman8x8_Direct_JPEG_16s1u_C1(
			outblock->WorkBlock[bc],
			bitstream->next_output_byte,  
			&bitstream->bits_index,
			&outblock->DC_pred[outblock->dcImage[bc]],
			entropy->dc_huff_tbls[0].pHuffTbl,
			entropy->ac_huff_tbls[0].pHuffTbl
			);
	}

	for(; bc < outblock->blockNum; bc++)
	{
		ippiEncodeHuffman8x8_Direct_JPEG_16s1u_C1(
			outblock->WorkBlock[bc],
			bitstream->next_output_byte,  
			&bitstream->bits_index,
			&outblock->DC_pred[outblock->dcImage[bc]],
			entropy->dc_huff_tbls[1].pHuffTbl,
			entropy->ac_huff_tbls[1].pHuffTbl
			);
	}

	if((bitstream->bits_index >> 3) > bitstream->free_in_buffer)
		return -1;
	
	bitstream->next_output_byte = bitstream->next_output_byte 
									+ (bitstream->bits_index >> 3);
	bitstream->free_in_buffer = bitstream->free_in_buffer
									- (bitstream->bits_index >> 3);
	bitstream->bits_index = bitstream->bits_index & 0x7;

	return 0;
}

#else //IPP_SUPPORT

#define USE_PUTBIT		1

#define PUTBIT(indx, cach)						\
	while (indx >= 8)							\
	{											\
		ch = (UINT8)((cach >> 16) & 0xFF);		\
		totalbytes++;							\
		*next_output_byte++ = ch;				\
		if(ch == 0xFF)							\
		{										\
			totalbytes++;						\
			*next_output_byte++ = 0;			\
		}										\
		cach <<= 8;								\
		indx -= 8;								\
	}											\


static INT32 encode_block(
						  INT16*		block, 
						  INT16*		DC_pred, 
						  JPEG_STREAM*	bitstream, 						
						  UINT8*		dc_hufsize,
						  UINT16*		dc_hufcode,
						  UINT8*		ac_hufsize,
						  UINT16*		ac_hufcode
						  )
{
	INT32 val, val_bk;
	INT32 run, nbits;
	INT32 k,  index;

#if USE_PUTBIT
	UINT32 indx, cach, val1, totalbytes;
	UINT8 ch, *next_output_byte;
	
	totalbytes = 0;
	indx = bitstream->bits_index;
	cach = bitstream->cache;
	next_output_byte = bitstream->next_output_byte;

	val = val_bk = block[0] - *DC_pred;
	*DC_pred = block[0];
	if (val < 0) {
		val = -val;
		val_bk--;
	}

	nbits = 0;
	while(val) {
		nbits++;
		val >>= 1;
	}
/**/	
	indx += dc_hufsize[nbits]; // put_bits += size;
	val1 = dc_hufcode[nbits];  // dc_hufcode[nbits] -->code
	val1 <<= (24 - indx);  // put_buffer <<= 24 - put_bits;
	cach |= val1;
	//PUTBIT(indx, cach);
	while (indx >= 8)							
	{											
		ch = (UINT8)((cach >> 16) & 0xFF);		
		totalbytes++;							
		*next_output_byte++ = ch;				
		if(ch == 0xFF)							
		{										
			totalbytes++;						
			*next_output_byte++ = 0;			
		}										
		cach <<= 8;								
		indx -= 8;								
	}
/**/

	if(nbits) {
		indx += nbits;
		val1 = (UINT32)val_bk & ((1 << nbits) - 1);
		val1 <<= (24 - indx);
		cach |= val1;
		PUTBIT(indx, cach);
	}
	
	run = 0;	
	for(k = 1; k < JPEG_BLOCK_SIZE; k++) 
	{
		if((val = block[zig_zag_tab_index[k]]) == 0) 
		{
			run++;
		} 
		else 
		{
			while(run > 15) 
			{
				indx += ac_hufsize[0xF0];
				val1 = ac_hufcode[0xF0];
				val1 <<= (24 - indx);
				cach |= val1;
				PUTBIT(indx, cach);
				run -= 16;
			}
			
			val_bk = val;
			if (val < 0) 
			{
				val = -val;
				val_bk--;
			}

			nbits = 1;
			while((val >>= 1))
				nbits++;

			index = (run << 4) + nbits;
			indx += ac_hufsize[index];
			val1 = ac_hufcode[index];
			val1 <<= (24 - indx);
			cach |= val1;
			PUTBIT(indx, cach);
			
			indx += nbits;
			val1 = (UINT32)val_bk & ((1 << nbits) - 1);
			val1 <<= (24 - indx);
			cach |= val1;
			PUTBIT(indx, cach);

			run = 0;
		}
	}
	
	if (run > 0)
	{
		indx += ac_hufsize[0];
		val1 = ac_hufcode[0];
		val1 <<= (24 - indx);
		cach |= val1;
		PUTBIT(indx, cach);
	}

	bitstream->bits_index = indx;
	bitstream->cache = cach;
	bitstream->next_output_byte += totalbytes;
	bitstream->free_in_buffer -= totalbytes;

#else  //USE_PUTBIT
	
	val = val_bk = block[0] - *DC_pred;
	*DC_pred = block[0];
	if (val < 0) 
	{
		val = -val;
		val_bk--;
	}

	nbits = 0;
	while(val) 
	{
		nbits++;
		val >>= 1;
	}

	putbits(bitstream, dc_hufcode[nbits], dc_hufsize[nbits]);
	if(nbits)
	{
		putbits(bitstream, (UINT32)val_bk & ((1 << nbits) - 1), nbits);			
	}
	
	run = 0;	
	for(k = 1; k < JPEG_BLOCK_SIZE; k++) 
	{
		if((val = block[zig_zag_tab_index[k]]) == 0) 
		{
			run++;
		} 
		else 
		{
			while(run > 15) 
			{
				putbits(bitstream, ac_hufcode[0xF0], ac_hufsize[0xF0]);
				run -= 16;
			}
			
			val_bk = val;
			if (val < 0) 
			{
				val = -val;
				val_bk--;
			}

			nbits = 1;
			while((val >>= 1))
				nbits++;

			index = (run << 4) + nbits;
			putbits(bitstream, ac_hufcode[index], ac_hufsize[index]);
			putbits(bitstream, (UINT32)val_bk & ((1 << nbits) - 1), nbits);			

			run = 0;
		}
	}
	
	if (run > 0)
	{
		putbits(bitstream, ac_hufcode[0], ac_hufsize[0]);
	}
#endif //USE_PUTBIT
	
	return 0;
}

INT32	jpeg_encodehuffman(JPEG_STREAM*			bitstream,
						   JPEG_MCU_BLOCK*		outblock,
						   JPEG_HUFF_ENTROPY*	entropy)
{
	UINT32 bc;
	INT32  ret;			
	
	for(bc = 0; bc < outblock->lumblockNum; bc++)
	{
		ret = encode_block(
			outblock->WorkBlock[bc], 
			&outblock->DC_pred[outblock->dcImage[bc]],
			bitstream, 
			entropy->dc_huff_tbls[0].hufsize,
			entropy->dc_huff_tbls[0].hufcode,
			entropy->ac_huff_tbls[0].hufsize,
			entropy->ac_huff_tbls[0].hufcode
			);
		
		if(ret < 0)
			return -1;
	}

	for(; bc < outblock->blockNum; bc++)
	{
		ret = encode_block(
			outblock->WorkBlock[bc], 
			&outblock->DC_pred[outblock->dcImage[bc]],
			bitstream, 
			entropy->dc_huff_tbls[1].hufsize,
			entropy->dc_huff_tbls[1].hufcode,
			entropy->ac_huff_tbls[1].hufsize,
			entropy->ac_huff_tbls[1].hufcode
			);
		
		if(ret < 0)
			return -1;
	}
	
	return 0;
}

#endif //IPP_SUPPORT