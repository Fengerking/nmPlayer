#include "bit.h"


void voADPCMDecInitBits(Bitstream *bitptr, unsigned char *byte, int length)
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

int voADPCMDecGetBits(Bitstream *bitptr, int nBits)
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

void  voADPCMDecAlignBits(Bitstream *bitptr)
{
	int bits;

	bits = bitptr->bitsleft & 7;
	
	bitptr->bitsleft -= bits;

	return;
}

int voADPCMDecSkipBits(Bitstream *bitptr, int nBits)
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

