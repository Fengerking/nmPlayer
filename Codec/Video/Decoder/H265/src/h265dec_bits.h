 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_bits.h
    \brief    Read Bits functions
    \author   
	\change
*/


#ifndef __H265DEC_BITS_H__
#define __H265DEC_BITS_H__
#include "h265dec_porting.h"


#if defined(ARM) && defined(LINUX)
static VOINLINE  VO_U32 BYTESWAP(VO_U32 a)
{
       __asm__  __volatile__("rev %0, %0" : "+r"(a));
 return a;
}
#else
#define BYTESWAP(a) ((a<<24) | ((a<<8)&0x00ff0000) | ((a>>8)&0x0000ff00) | (a>>24));
#endif

typedef struct {
  /*bitstream */
  VO_U32 cache_a;
  VO_U32 cache_b;
  VO_S32 bit_pos;
  VO_S32 first_byte;
  const VO_U8 *p_bit_start;
  const VO_U8 *p_bit_ptr;
  const VO_U8 *p_bit_end;
}BIT_STREAM;

static VOINLINE void CopyBitStream( BIT_STREAM* p_bs_dst, 
	BIT_STREAM* p_bs_src )
{
    p_bs_dst->cache_a = p_bs_src->cache_a;
	p_bs_dst->cache_b = p_bs_src->cache_b;
	p_bs_dst->bit_pos = p_bs_src->bit_pos;
    p_bs_dst->p_bit_start = p_bs_src->p_bit_start;
	p_bs_dst->p_bit_ptr = p_bs_src->p_bit_ptr;
	p_bs_dst->p_bit_end = p_bs_src->p_bit_end;
} 

static VOINLINE void InitBitStream(BIT_STREAM* p_bs, const VO_U8 *stream, VO_S32 len)
{
	VO_U32 tmp;
	VO_S32 align4, i, shif_bit;
    p_bs->p_bit_start = stream;
	p_bs->p_bit_end = stream + len + 8;		//adding more just to be safe, +8 is good   YU_TBD
	align4 = (VO_S32)(4 - ((VO_S32)stream & 3));
	shif_bit = 24;

	p_bs->cache_a = 0;
	for (i = 0; i < (align4 < len ? align4 : len); i++){
		p_bs->cache_a |= (stream[i]<<shif_bit);
		shif_bit     -= 8;
	}

	stream += align4;
	p_bs->p_bit_ptr = stream + 4;
	if (align4+4 <= len)
	{
		tmp = *((VO_U32*)stream);
		p_bs->cache_b = BYTESWAP(tmp);
	}
	else if (align4 >= len)
	{
		p_bs->cache_b = 0;
	}
	else
	{
		len -= align4;
		shif_bit = 24;
		p_bs->cache_b = 0;
		for (i = 0; i < len; i++){
			p_bs->cache_b |= (stream[i]<<shif_bit);
			shif_bit     -= 8;
		}
	}

	if (4 == align4) {
		p_bs->bit_pos   = 32;
	} else {
		p_bs->cache_a |= (p_bs->cache_b >> (align4*8));
		p_bs->cache_b<<= (4 -align4)*8;
		p_bs->bit_pos   = 32 - (4 - align4)*8;
	}
}
#if 0
#define UPDATE_CACHE(p_bs)	\
{\
	VO_U32 tmp;\
	if (p_bs->bit_pos < 0){\
		tmp = *(VO_U32*)p_bs->p_bit_ptr;\
		tmp = BYTESWAP(tmp);\
		p_bs->cache_b = (tmp << (-p_bs->bit_pos));\
		p_bs->bit_pos += 32;\
		p_bs->cache_a |= (tmp >> p_bs->bit_pos);\
		p_bs->p_bit_ptr += 4;\
	}\
}
#else
#define UPDATE_CACHE(p_bs)	\
{\
	VO_U32 tmp;\
	if (p_bs->bit_pos < 0){\
		VO_S32 nleft = p_bs->p_bit_end - p_bs->p_bit_ptr - 8;\
		if (nleft >= 4)\
		{\
			tmp = *(VO_U32*)p_bs->p_bit_ptr;\
			tmp = BYTESWAP(tmp);\
			p_bs->cache_b = (tmp << (-p_bs->bit_pos));\
			p_bs->bit_pos += 32;\
			p_bs->cache_a |= (tmp >> p_bs->bit_pos);\
			p_bs->p_bit_ptr += 4;\
		}\
		else if(nleft > 0)\
		{\
			VO_S32 shift = 32 - (nleft<<3);\
		    tmp = (*p_bs->p_bit_ptr++);\
			while (--nleft > 0)\
			{\
			  tmp = (tmp << 8) | (*p_bs->p_bit_ptr++);\
			}\
			tmp <<= shift;\
			p_bs->cache_b = (tmp << (-p_bs->bit_pos));\
			p_bs->bit_pos += 32;\
			p_bs->cache_a |= (tmp >> p_bs->bit_pos);\
			p_bs->p_bit_ptr += (shift>>3);\
		}\
		else{\
			p_bs->bit_pos += 32;\
			p_bs->p_bit_ptr += 4;\
		}\
	}\
}
#endif
#define UPDATE_CACHE_LARGE(p_bs) \
{\
	VO_U32 tmp;\
	while (p_bs->bit_pos <= -64)\
	{\
		p_bs->p_bit_ptr += 4;\
		p_bs->bit_pos += 32;\
	}\
	if (p_bs->bit_pos <= -32)\
	{\
		tmp = *(VO_U32*)p_bs->p_bit_ptr;\
		tmp = BYTESWAP(tmp);\
		p_bs->bit_pos += 32;\
		p_bs->cache_a = (tmp << (-p_bs->bit_pos));\
		tmp = *(VO_U32*)(p_bs->p_bit_ptr+4);\
		tmp = BYTESWAP(tmp);\
		p_bs->cache_b = (tmp << (-p_bs->bit_pos));\
		p_bs->bit_pos += 32;\
		p_bs->cache_a |= (tmp >> p_bs->bit_pos);\
		p_bs->p_bit_ptr += 8;\
	}\
	else if (p_bs->bit_pos < 0)\
	{\
		tmp = *(VO_U32*)p_bs->p_bit_ptr;\
		tmp = BYTESWAP(tmp);\
		p_bs->cache_b = (tmp << (-p_bs->bit_pos));\
		p_bs->bit_pos += 32;\
		p_bs->cache_a |= (tmp >> p_bs->bit_pos);\
		p_bs->p_bit_ptr += 4;\
	}\
}


#define SHOW_BITS(p_bs,n) ((VO_U32)(p_bs->cache_a) >> (32-(n)))
#define FLUSH_BITS(p_bs,n)	\
{\
	VO_S32 nTmp = n;	\
	p_bs->bit_pos -= nTmp;\
	p_bs->cache_a   = (p_bs->cache_a << nTmp)|(p_bs->cache_b >> (32-nTmp));\
	p_bs->cache_b <<= nTmp;\
}

#define FLUSH_BITS_LARGE(p_bs,n)	\
{\
	VO_S32 nTmp = n;	\
	p_bs->bit_pos -= nTmp;\
	if (nTmp < 32) \
	{\
		p_bs->cache_a   = (p_bs->cache_a << nTmp)|(p_bs->cache_b >> (32-nTmp));\
		p_bs->cache_b <<= nTmp;\
	}\
	else\
	{\
		p_bs->cache_a = p_bs->cache_b << (nTmp & 31);\
		p_bs->cache_b = 0;\
		UPDATE_CACHE_LARGE(p_bs);\
	}\
}


static VOINLINE VO_S32 GetBits(BIT_STREAM* p_bs,VO_S32 n)
{
	VO_S32 i = SHOW_BITS(p_bs,n);
	FLUSH_BITS(p_bs,n);
	return i;
}

static VOINLINE VO_S32 GetByte(BIT_STREAM* p_bs)
{
	VO_S32 i = (VO_S32)(p_bs->cache_a >> 24);
	p_bs->bit_pos -= 8;
	p_bs->cache_a   = (p_bs->cache_a << 8)|(p_bs->cache_b >> 24);
	p_bs->cache_b <<= 8;
	return i;
}

static VOINLINE VO_U32 ReadOneBit(BIT_STREAM* p_bs)//YU_TBD
{
	VO_S32 i = SHOW_BITS(p_bs, 1);
	FLUSH_BITS(p_bs,1);
	VO_LOG(LOG_OUTTYPE_ALL, LOG_MODULE_ALL | LOG_INFO_ALL,   "\n %d", i);
	return i;
}

static VOINLINE VO_S32 GetBits32(BIT_STREAM* p_bs)
{
	VO_S32 i = SHOW_BITS(p_bs, 16);
	FLUSH_BITS(p_bs,16);
	i = (i << 16)|SHOW_BITS(p_bs, 16);
	FLUSH_BITS(p_bs,16);
	UPDATE_CACHE_LARGE(p_bs);
	return i;
}

static VOINLINE VO_S32 EofBits(BIT_STREAM* p_bs)
{
	return p_bs->p_bit_ptr >= p_bs->p_bit_end;
}

static VOINLINE void ByteAlign(BIT_STREAM* p_bs)
{
	VO_S32 n = p_bs->bit_pos & 7;
	if (n) {
		FLUSH_BITS(p_bs, n);
	}
}

static VOINLINE void ReadByteAlignment(BIT_STREAM* p_bs)
{
	 ReadOneBit(p_bs);
	ByteAlign(p_bs);
}

static VOINLINE VO_S32 BitsToNextByte(BIT_STREAM* p_bs)
{
	return ((p_bs->bit_pos - 1) & 7) + 1;
}

static VOINLINE const VO_U8 *prebytepos(BIT_STREAM* p_bs)
{
	return p_bs->p_bit_ptr - ((p_bs->bit_pos+7) >> 3) - 4;
}
static VOINLINE const VO_U8 *bytepos(BIT_STREAM* p_bs)
{
	return p_bs->p_bit_ptr - ((p_bs->bit_pos) >> 3) - 4;
}

#define BEGIN_BITS_POS( p_bs )	\
	VO_S32 bitpos = p_bs->bit_pos;	\
	VO_U32 cache_a = p_bs->cache_a; \
	VO_U32 cache_b = p_bs->cache_b; 

#define END_BITS_POS( p_bs )		\
	p_bs->cache_a = cache_a;	\
	p_bs->cache_b = cache_b;	\
	p_bs->bit_pos = bitpos;

#define UPDATE_CACHE_POS(p_bs)	\
{\
	if (bitpos < 0){\
		VO_U32 tmp = *(VO_U32*)(p_bs->p_bit_ptr);\
		tmp = BYTESWAP(tmp);\
		cache_a |= (tmp >> (32 + bitpos));\
		cache_b |= (tmp << (-bitpos));\
		p_bs->p_bit_ptr += 4;\
		bitpos += 32;\
	}\
}

#define SHOW_BITS_POS(n) ((VO_U32)(cache_a) >> (32-(n)))
#define SHOW_BITS_NEG_POS(n) ((VO_U32)(~(cache_a)) >> (32-(n)))
#define FLUSH_BITS_POS(n)	\
{\
	cache_a  = (cache_a << (n))|(cache_b >> (32 -(n)));\
	cache_b<<= (n);\
	bitpos  -= (n);\
}
#define GET_BITS_1_POS \
	(--bitpos, (cache_a>>31) ? (cache_a = (cache_a<<1) | (cache_b>>31), cache_b <<= 1, 1) : (cache_a = (cache_a<<1) | (cache_b>>31), cache_b <<= 1, 0))



#endif //__H265DEC_BITS_H__
