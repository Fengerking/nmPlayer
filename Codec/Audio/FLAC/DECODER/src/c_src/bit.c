#include "bit.h"


void voFLACDecInitBits(Bitstream *bitptr, unsigned char *byte, int length)
{
	bitptr->byte  = byte;
	bitptr->cache = 0;
	bitptr->bitsleft  = 0;
	bitptr->nbytes = length;
}

static __inline void Reflush(Bitstream *bitptr)
{
	int nBytes = bitptr->nbytes;

	if (nBytes >= 4) {
		bitptr->cache  = (*bitptr->byte++) << 24;
		bitptr->cache |= (*bitptr->byte++) << 16;
		bitptr->cache |= (*bitptr->byte++) <<  8;
		bitptr->cache |= (*bitptr->byte++);
		bitptr->bitsleft = 32;
		bitptr->nbytes -= 4;
	} else {
		bitptr->cache = 0;
		while (nBytes-- > 0) {
			bitptr->cache |= (*bitptr->byte++);
			bitptr->cache <<= 8;
		}
		bitptr->cache <<= ((3 - bitptr->nbytes)*8);
		bitptr->bitsleft = 8*bitptr->nbytes;
		bitptr->nbytes = 0;
	}
}

int voFLACDecGetBits(Bitstream *bitptr, int nBits)
{
	unsigned int data;
	int			lowBits;

	data = bitptr->cache >> (32 - nBits);	
	bitptr->cache <<= nBits;				
	bitptr->bitsleft -= nBits;			

	if (bitptr->bitsleft < 0) {
		lowBits = -bitptr->bitsleft;
		Reflush(bitptr);
		data |= bitptr->cache >> (32 - lowBits);
	
		bitptr->bitsleft -= lowBits;			
		bitptr->cache <<= lowBits;			
	}

	return data;
}

int voFLACDecGetsBits(Bitstream *bitptr, int nBits)
{
	int data, lowBits;

	if(bitptr->bitsleft <= 0)
		Reflush(bitptr);

	nBits &= 0x1f;						
	data = (int)bitptr->cache >> (31 - nBits);	
	data >>= 1;							
	bitptr->cache <<= nBits;				
	bitptr->bitsleft -= nBits;			

	if (bitptr->bitsleft < 0) {
		lowBits = -bitptr->bitsleft;
		Reflush(bitptr);
		data |= bitptr->cache >> (32 - lowBits);
	
		bitptr->bitsleft -= lowBits;			
		bitptr->cache <<= lowBits;	
	}

	return data;
}

void  voFLACDecAlignBits(Bitstream *bitptr)
{
	int bits;

	bits = bitptr->bitsleft & 7;
	
	bitptr->bitsleft -= bits;

	return;
}

int voFLACDecSkipBits(Bitstream *bitptr, int nBits)
{
	int nsBits;
	int nSByte;
	
	if(nBits < bitptr->bitsleft)
	{
		nsBits = nBits;
		bitptr->cache <<= nBits;				
		bitptr->bitsleft -= nBits;
	}
	else
	{
		nsBits = bitptr->bitsleft;
		nBits -= bitptr->bitsleft;
		bitptr->cache = 0;				
		bitptr->bitsleft = 0;
		nSByte = nBits >> 3;
		nBits = nBits & 7;
		if(nSByte <= bitptr->nbytes)
		{
			bitptr->byte += nSByte;
			bitptr->nbytes -= nSByte;
			nsBits += nSByte*8;
			Reflush(bitptr);
			bitptr->cache <<= nBits;				
			bitptr->bitsleft -= nBits;
			nsBits += nBits;
		}
		else
		{
			bitptr->nbytes = 0;
			bitptr->byte += bitptr->nbytes;
			nsBits += bitptr->nbytes * 8;
		}
	}

	return nsBits;
}

int	 voFLACDecCalcBytesUsed(Bitstream *bitptr, unsigned char *buf)
{
	int bitsUsed, bytesUsed;

	bitsUsed  = (bitptr->byte - buf) * 8;

	if (bitptr->bitsleft >=0)
	{
		bitsUsed -= bitptr->bitsleft;
	}
	
	bytesUsed = (bitsUsed + 7) >> 3;

	return bytesUsed;
}

int64 voFLACDecGetUtf8(Bitstream *bitptr)
{
	int64 rv;
	uint64 v = 0;
	int x;
	unsigned int i;

	x = voFLACDecGetBits(bitptr, 8);

	if(!(x & 0x80)) {						/* 0xxxxxxx */
		v = x;
		i = 0;
	}
	else if(x & 0xC0 && !(x & 0x20)) {		/* 110xxxxx */
		v = x & 0x1F;
		i = 1;
	}
	else if(x & 0xE0 && !(x & 0x10)) {		/* 1110xxxx */
		v = x & 0x0F;
		i = 2;
	}
	else if(x & 0xF0 && !(x & 0x08)) {		/* 11110xxx */
		v = x & 0x07;
		i = 3;
	}
	else if(x & 0xF8 && !(x & 0x04)) {		/* 111110xx */
		v = x & 0x03;
		i = 4;
	}
	else if(x & 0xFC && !(x & 0x02)) {		/* 1111110x */
		v = x & 0x01;
		i = 5;
	}
	else if(x & 0xFE && !(x & 0x01)) {		/* 11111110 */
		v = 0;
		i = 6;
	}
	else {
		//2012/2/29 Clean up compile warning issue
		rv = 0xffffffff;
		return rv;
	}
	for( ; i; i--) {
		x = voFLACDecGetBits(bitptr, 8); 
		if(!(x & 0x80) || (x & 0x40)) {		/* 10xxxxxx */
			rv = 0xffffffff;
			return rv;
		}
		v <<= 6;
		v |= (x & 0x3F);
	}
	rv = v;
	return rv;
}

int voFLACDecGetUnaryBits(Bitstream *bitptr)
{
//	int data = 0;
//
//     while (!voFLACDecGetBits(bitptr, 1))
//          data++;
//	 
//	return data;

	int data = 0;
	unsigned int cach, bit;
	int bitsleft;
	
	bitsleft = bitptr->bitsleft;

	if(bitsleft <= 0)
	{
		Reflush(bitptr);
		bitsleft = bitptr->bitsleft;
	}
	
	cach = bitptr->cache;

	bit = cach & 0x80000000;
	cach <<= 1;
	bitsleft--;
	
	while(!bit)
	{
		if(bitsleft <= 0)
		{
			Reflush(bitptr);
			bitsleft = bitptr->bitsleft;
			cach = bitptr->cache;

			if(bitptr->nbytes == 0 && bitsleft <= 0)
				break;
		}

		data++;		
		bit = (cach & 0x80000000);		

		bitsleft--;	
		cach <<= 1;	
	}
	
	bitptr->bitsleft = bitsleft;
	bitptr->cache = cach;

	return data;
}

int  voFLACDecGetRiceBits(Bitstream *bitptr, int bits)
{
	int data = 0;
	unsigned int msbs, lsbs, uval;

	msbs = voFLACDecGetUnaryBits(bitptr);

	if(bits)
		lsbs = voFLACDecGetBits(bitptr, bits);
	else
		lsbs = 0;

	uval = (msbs << bits) | lsbs;
	if(uval & 1)
		data = -((int)(uval >> 1)) - 1;
	else
		data = (int)(uval >> 1);
	
	return data;	
}