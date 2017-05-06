/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/

#ifndef __READBITS_H
#define __READBITS_H
#include "voMpegPort.h"
#include "voMpeg2Decoder.h"

#define BYTESWAP(a) ((a<<24) | ((a<<8)&0x00ff0000) | ((a>>8)&0x0000ff00) | (a>>24))

typedef MpegDecode BitStream;

static INLINE VO_VOID InitBitStream(BitStream* bs_ptr, const VO_U8 *stream, VO_S32 len)
{
	VO_U32 tmp;
	VO_S32 align4, i, shif_bit;

	bs_ptr->pBitEnd = stream+len+8;		//adding more just to be safe, +8 is good
	align4 = (VO_S32)(4 - ((VO_S32)stream & 3));
	shif_bit = 24;

	bs_ptr->cache_a = 0;
	for (i = 0; i < align4; i++){
		bs_ptr->cache_a |= (stream[i]<<shif_bit);
		shif_bit     -= 8;
	}

	stream += align4;
	tmp = *((VO_U32*)stream);
	bs_ptr->cache_b = BYTESWAP(tmp);
	bs_ptr->pBitPtr = stream + 4;

	if (4 == align4){
		bs_ptr->nBitPos   = 32;
	}else{
		bs_ptr->cache_a |= (bs_ptr->cache_b >> (align4*8));
		bs_ptr->cache_b<<= (4 -align4)*8;
		bs_ptr->nBitPos   = 32 - (4 - align4)*8;
	}
}

#define UPDATE_CACHE(bs_ptr)	\
{\
	VO_U32 tmp;\
	if (bs_ptr->nBitPos < 0){\
		VO_S32 nleft = bs_ptr->pBitEnd - bs_ptr->pBitPtr - 8;\
		if (nleft >= 4)\
		{\
			tmp = *(VO_U32*)bs_ptr->pBitPtr;\
			tmp = BYTESWAP(tmp);\
			bs_ptr->cache_b = (tmp << (-bs_ptr->nBitPos));\
			bs_ptr->nBitPos += 32;\
			bs_ptr->cache_a |= (tmp >> bs_ptr->nBitPos);\
			bs_ptr->pBitPtr += 4;\
		}\
		else if(nleft > 0)\
		{\
			VO_S32 shift = 32 - (nleft<<3);\
			tmp = (*bs_ptr->pBitPtr++);\
			while (--nleft > 0)\
			{\
				tmp = (tmp << 8) | (*bs_ptr->pBitPtr++);\
			}\
			tmp <<= shift;\
			bs_ptr->cache_b = (tmp << (-bs_ptr->nBitPos));\
			bs_ptr->nBitPos += 32;\
			bs_ptr->cache_a |= (tmp >> bs_ptr->nBitPos);\
			bs_ptr->pBitPtr += (shift>>3);\
		}\
		else{\
			bs_ptr->nBitPos += 32;\
			bs_ptr->pBitPtr += 4;\
		}\
	}\
}

#define UPDATE_CACHE_LARGE(bs_ptr) \
{\
	VO_U32 tmp;\
	while (bs_ptr->nBitPos <= -64)\
	{\
		bs_ptr->pBitPtr += 4;\
		bs_ptr->nBitPos += 32;\
	}\
	if (bs_ptr->nBitPos <= -32)\
	{\
		tmp = *(VO_U32*)bs_ptr->pBitPtr;\
		tmp = BYTESWAP(tmp);\
		bs_ptr->nBitPos += 32;\
		bs_ptr->cache_a = (tmp << (-bs_ptr->nBitPos));\
		tmp = *(VO_U32*)(bs_ptr->pBitPtr+4);\
		tmp = BYTESWAP(tmp);\
		if (bs_ptr->nBitPos == 0)\
		{\
			bs_ptr->cache_b = tmp;\
			bs_ptr->nBitPos = 32;\
		}\
		else\
		{\
			bs_ptr->cache_b = (tmp << (-bs_ptr->nBitPos));\
			bs_ptr->nBitPos += 32;\
			bs_ptr->cache_a |= (tmp >> bs_ptr->nBitPos);\
		}\
		bs_ptr->pBitPtr += 8;\
	}\
	else if (bs_ptr->nBitPos < 0)\
	{\
		tmp = *(VO_U32*)bs_ptr->pBitPtr;\
		tmp = BYTESWAP(tmp);\
		bs_ptr->cache_b = (tmp << (-bs_ptr->nBitPos));\
		bs_ptr->nBitPos += 32;\
		bs_ptr->cache_a |= (tmp >> bs_ptr->nBitPos);\
		bs_ptr->pBitPtr += 4;\
	}\
}

#define SHOW_BITS(bs_ptr,n) ((VO_U32)(bs_ptr->cache_a) >> (32-(n)))
#define FLUSH_BITS(bs_ptr,n)	\
{\
	VO_S32 nTmp = n;	\
	bs_ptr->nBitPos -= nTmp;\
	bs_ptr->cache_a   = (bs_ptr->cache_a << nTmp)|(bs_ptr->cache_b >> (32-nTmp));\
	bs_ptr->cache_b <<= nTmp;\
}

#define FLUSH_BITS_LARGE(bs_ptr,n)	\
{\
	VO_S32 nTmp = n;	\
	bs_ptr->nBitPos -= nTmp;\
	if (nTmp < 32) \
	{\
		bs_ptr->cache_a   = (bs_ptr->cache_a << nTmp)|(bs_ptr->cache_b >> (32-nTmp));\
		bs_ptr->cache_b <<= nTmp;\
	}\
	else\
	{\
		bs_ptr->cache_a = bs_ptr->cache_b << (nTmp & 31);\
		bs_ptr->cache_b = 0;\
		UPDATE_CACHE_LARGE(bs_ptr);\
	}\
}

static INLINE VO_S32 GetBits(BitStream* bs_ptr,VO_S32 n)
{
	VO_S32 i = SHOW_BITS(bs_ptr,n);
	FLUSH_BITS(bs_ptr,n);
	return i;
}

static INLINE VO_S32 GetBits32(BitStream* bs_ptr)
{
	VO_S32 i = SHOW_BITS(bs_ptr, 16);
	FLUSH_BITS(bs_ptr,16);
	i = (i << 16)|SHOW_BITS(bs_ptr, 16);
	FLUSH_BITS(bs_ptr,16);
	UPDATE_CACHE_LARGE(bs_ptr);
	return i;
}

static INLINE VO_S32 EofBits(BitStream* bs_ptr)
{
	return bs_ptr->pBitPtr >= bs_ptr->pBitEnd;
}

static INLINE VO_VOID bytealign(BitStream* bs_ptr)
{
	VO_S32 n = bs_ptr->nBitPos & 7;
	if (n)
	{
		FLUSH_BITS(bs_ptr, n);
	}
}

static INLINE VO_S32 BitsToNextByte(BitStream* bs_ptr)
{
	return ((bs_ptr->nBitPos - 1) & 7) + 1;
}

static INLINE const VO_U8 *bytepos(BitStream* bs_ptr)
{
	return bs_ptr->pBitPtr - (bs_ptr->nBitPos >> 3) - 4;
}

#define BEGIN_BITS_POS( bs_ptr )	\
	bitpos = bs_ptr->nBitPos;	\
	cache_a = bs_ptr->cache_a; \
	cache_b = bs_ptr->cache_b; 

#define END_BITS_POS( bs_ptr )		\
	bs_ptr->cache_a = cache_a;	\
	bs_ptr->cache_b = cache_b;	\
	bs_ptr->nBitPos = bitpos;

#define CACHE_CHECK(bs_ptr, cache_a, cache_b, bitpos)	\
{\
	if (bitpos < 0){\
		VO_U32 tmp = *(VO_U32*)(bs_ptr->pBitPtr);\
		tmp = BYTESWAP(tmp);\
		cache_a |= (tmp >> (32 + bitpos));\
		cache_b |= (tmp << (-bitpos));\
		bs_ptr->pBitPtr += 4;\
		bitpos += 32;\
	}\
}

#define SHOW_BIT_POS(cache_a, n) ((VO_U32)(cache_a) >> (32-(n)))
#define SHOW_BIT_NEG_POS(cache_a, n) ((VO_U32)(~(cache_a)) >> (32-(n)))
#define FLUSH_BIT_POS(cache_a, cache_b, bitpos, n)	\
{\
	cache_a  = (cache_a << (n))|(cache_b >> (32 -(n)));\
	cache_b<<= (n);\
	bitpos  -= (n);\
}
#define GET_BITS_1_POS(cache_a, cache_b, bitpos) \
	(--bitpos, (cache_a>>31) ? (cache_a = (cache_a<<1) | (cache_b>>31), cache_b <<= 1, 1) : (cache_a = (cache_a<<1) | (cache_b>>31), cache_b <<= 1, 0))



#endif//__READBITS_H