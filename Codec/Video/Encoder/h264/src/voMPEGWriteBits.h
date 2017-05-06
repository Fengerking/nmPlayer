/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOMPEG_WRITE_BS_
#define _VOMPEG_WRITE_BS_

#include "voVCodecCommon.h"

typedef struct
{
    VO_U8 nBits;
    VO_U8 nSize;
} VLC_TYPE;

typedef struct
{
    VO_U16 nBits;
    VO_U8  nSize;
    /* Next level table to use */
    VO_U8  nNext;
} VLC_LARGE;

typedef struct
{
    VO_U16 val;
    VO_U8  len;
    VO_U8  suffix;
} LEVEL_TYPE;


typedef struct BS_TYPE
{
    VO_U8 *head;
    VO_U8 *tail;
    VO_U32 cache;
    VO_S32 left_bits;    /* i_count number of available bits */
} BS_TYPE;


//extern const VLC_TYPE TotalZeros[15][16];
//extern const VLC_TYPE TotalZerosDc[3][4];
//extern const VLC_TYPE RunBefore[7][15];


#define LEVEL_SIZE 128
//extern VLC_LARGE LevelToken[7][LEVEL_SIZE];
#define VOSWAPTOBIGENDIAN(X)   X = ((X<<8)&0xFF00FF00) | ((X>>8)&0x00FF00FF); \
                                                X = (X>>16) | (X<<16);

static VOINLINE void InitBits( BS_TYPE *bs, void *buffer )
{
  VO_S32 offset = ((VOINTPTR)buffer & 3);
  bs->tail = bs->head = (VO_U8*)buffer - offset;
  bs->left_bits  = (4 - offset)*8;
  bs->cache = EndianFix32( M32(bs->tail) );
  bs->cache >>= (4-offset)*8;
}
static VOINLINE VO_S32 GetBitsPos( BS_TYPE *bs )
{
  return( 8 * (bs->tail - bs->head) + 32 - bs->left_bits );
}

/* Write the rest of nCurBits to the bitstream; results in a bitstream no longer 32-bit aligned. */
static VOINLINE void FlushBits( BS_TYPE *bs )
{
  M32( bs->tail ) = EndianFix32( bs->cache << (bs->left_bits&31) );
  bs->tail += 4 - bs->left_bits / 8;
  bs->left_bits = 32;
}
/* The inverse of BitFlush: prepare the bitstream to be written to again. */
static VOINLINE void RealignBits( BS_TYPE *bs )
{
  VO_S32 offset = ((VOINTPTR)bs->tail & 3);
  if( offset )
  {
    bs->tail = (VO_U8*)bs->tail - offset;
    bs->left_bits = (4 - offset)<<3;
    bs->cache = EndianFix32( M32(bs->tail) );
    bs->cache >>= bs->left_bits;
  }
}

static VOINLINE void PutBits( BS_TYPE *bs, VO_S32 len, VO_U32 value )
{
  if( len < bs->left_bits )
  {
    bs->cache = (bs->cache << len) | value;
    bs->left_bits -= len;
  }
  else
  {
    len -= bs->left_bits;
    bs->cache = (bs->cache << bs->left_bits) | (value >> len);
    M32( bs->tail ) = EndianFix32( bs->cache );
    bs->tail += 4;
    bs->cache = value;
    bs->left_bits = 32 - len;
  }
}

static VOINLINE void PutBit( BS_TYPE *bs, VO_U32 value )
{
    bs->cache <<= 1;
    bs->cache |= value;
    bs->left_bits--;
    if( bs->left_bits == 0 )
    {
        M32( bs->tail ) = EndianFix32( bs->cache );
        bs->tail += 4;
        bs->left_bits = 32;
    }
}

// golomb functions 

static const VO_U8 ue_lenth_table[256] =
{
  0x01, 0x01, 0x03, 0x03, 0x05, 0x05, 0x05, 0x05, 
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
  0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
  0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
};

static VOINLINE void LargeUEBits( BS_TYPE *bs, VO_U32 value )
{
  VO_S32 len = 0;
  VO_S32 tmp_val = ++value;
  if( tmp_val >= 65536 )
  {
    len = 32;
    tmp_val >>= 16;
  }
  if( tmp_val >= 256 )
  {
    len += 16;
    tmp_val >>= 8;
  }
  len += ue_lenth_table[tmp_val];
  PutBits( bs, len>>1, 0 );
  PutBits( bs, (len>>1)+1, value );
}

/* Only works on values under 255. */
static VOINLINE void UEBits( BS_TYPE *bs, VO_S32 value )
{
  value++;
  PutBits( bs, ue_lenth_table[value], value );
}

static VOINLINE void SEBits( BS_TYPE *bs, VO_S32 value )
{
    VO_S32 len = 0;
    VO_S32 tmp_val = 1 - value*2;
    if( tmp_val < 0 ) 
	  tmp_val = value*2;
    value = tmp_val;
    if( tmp_val >= 256 )
    {
        len = 16;
        tmp_val >>= 8;
    }
    len += ue_lenth_table[tmp_val];
    PutBits( bs, len, value );
}
static VOINLINE void RBSPBits( BS_TYPE *bs )
{
  PutBit( bs, 1 );
  PutBits( bs, bs->left_bits&7, 0  );
}
#endif// _VOMPEG_WRITE_BS_
