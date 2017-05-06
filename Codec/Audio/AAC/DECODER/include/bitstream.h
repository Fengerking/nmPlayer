        /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#ifndef _BITSTREAM_H
#define _BITSTREAM_H

#include "global.h"

typedef struct{
	unsigned char *bytePtr;
	unsigned int iCache;
	int cachedBits;
	int nBytes;
	int noBytes;
} BitStream;

#define INLINE __inline

static void INLINE
BitStreamInit(BitStream * const bsi,
			  unsigned int		len,
			  void *const		buf )
{

	bsi->bytePtr = buf;
	bsi->iCache = 0;		
	bsi->cachedBits = 0;	
	bsi->nBytes = len;
	bsi->noBytes = 0;
}

static __inline void 
RefillBitstreamCache(BitStream *bsi)
{
	int nBytes = bsi->nBytes;

	/* optimize for common case, independent of machine endian-ness */
	if (nBytes >= 4) {
		bsi->iCache  = (*bsi->bytePtr++) << 24;
		bsi->iCache |= (*bsi->bytePtr++) << 16;
		bsi->iCache |= (*bsi->bytePtr++) <<  8;
		bsi->iCache |= (*bsi->bytePtr++);
		bsi->cachedBits = 32;
		bsi->nBytes -= 4;
	} else {
		bsi->iCache = 0;
		if(nBytes<=0)
		{	
			bsi->cachedBits = 32;
			bsi->noBytes+=4;
			return;
		}

		while (nBytes--) {
			bsi->iCache |= (*bsi->bytePtr++);
			bsi->iCache <<= 8;
		}
		bsi->iCache <<= ((3 - bsi->nBytes)*8);
		bsi->cachedBits = 8*bsi->nBytes;
		bsi->nBytes = 0;
	}
}

/* read n bits from bitstream */
static VO_U32 INLINE
BitStreamGetBits(BitStream * const bsi,
				 const unsigned int nBits)
{
	unsigned int data, lowBits;

	data = bsi->iCache >> (32 - nBits);		
	bsi->iCache <<= nBits;					
	bsi->cachedBits -= nBits;				

	if (bsi->cachedBits < 0) {
		lowBits = -bsi->cachedBits;
		RefillBitstreamCache(bsi);
		data |= bsi->iCache >> (32 - lowBits);		
	
		bsi->cachedBits -= lowBits;			
		bsi->iCache <<= lowBits;			
	}

	return data;
}


/* read single bit from bitstream */
static unsigned int INLINE
BitStreamGetBit(BitStream * const bs)
{
	return BitStreamGetBits(bs, 1);
}

/* reads n bits from bitstream without changing the stream pos */
static unsigned int INLINE
BitStreamShowBits(BitStream * const  bsi,
				  const unsigned int nBits)
{
	unsigned char *buf;
	unsigned int data, iCache;
	signed int lowBits;

	data = bsi->iCache >> (32 - nBits);		
	lowBits = nBits - bsi->cachedBits;		

	if (lowBits > 0) {
		iCache = 0;
		buf = bsi->bytePtr;
		while (lowBits > 0) {
			iCache <<= 8;
			if(bsi->nBytes)
			{
				iCache |= (unsigned int)*buf++;
			}
			lowBits -= 8;
		}
		lowBits = -lowBits;
		data |= iCache >> lowBits;
	}

	return data;
}


/* skip n bits forward in bitstream */
static INLINE void
BitStreamSkip(BitStream * const bsi,
			  int nBits)
{
	if (nBits > bsi->cachedBits) {
		nBits -= bsi->cachedBits;
		RefillBitstreamCache(bsi);
	}
	bsi->iCache <<= nBits;
	bsi->cachedBits -= nBits;
}

static INLINE void
AdvanceByBigStep(BitStream * const bsi,
			     int nBits)
{
	int nsBits;
	int nSByte;
	
	if(nBits < bsi->cachedBits)
	{
		nsBits = nBits;
		bsi->iCache <<= nBits;				
		bsi->cachedBits -= nBits;
	}
	else
	{
		nsBits = bsi->cachedBits;
		nBits -= bsi->cachedBits;
		bsi->iCache = 0;				
		bsi->cachedBits = 0;
		nSByte = nBits >> 3;
		nBits = nBits & 7;
		if(nSByte <= bsi->nBytes)
		{
			bsi->bytePtr += nSByte;
			bsi->nBytes -= nSByte;
			nsBits += nSByte*8;
			RefillBitstreamCache(bsi);
			bsi->iCache <<= nBits;				
			bsi->cachedBits -= nBits;
			nsBits += nBits;
		}
		else
		{
			bsi->nBytes = 0;
			bsi->bytePtr += bsi->nBytes;
			nsBits += bsi->nBytes * 8;
		}
	}

	return;
}

/* move forward to the next byte boundary */
static INLINE void
BitStreamByteAlign(BitStream * const bsi)
{
	int bits;

	bits = bsi->cachedBits & 7;
	
	bsi->cachedBits -= bits;
	bsi->iCache <<= bits;

	return;
}


static unsigned int INLINE
CalcBitsUsed(BitStream *bsi, unsigned char *startBuf, int startOffset)
{
	int bitsUsed;

	bitsUsed  = (bsi->bytePtr - startBuf) * 8;
	bitsUsed -= bsi->cachedBits;
	bitsUsed -= startOffset;

	return bitsUsed;
}

#endif	/* _BITSTREAM_H */
