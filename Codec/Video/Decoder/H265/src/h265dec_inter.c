/**
 *  h265dec_inter.c - HEVC Inter block prediction implementations
 *  
 *  Copyright (c) 2012 VisualOn, Inc, Confidential and Proprietary.
 *  
 */

#include <assert.h>
#include "h265dec_inter.h"
#include "h265dec_utils.h"


#if !MC_ASM_ENABLED


#define MC_LUMA_EXT_SIZE    7
#define MC_CHROMA_EXT_SIZE  3

// Interpolation luma coeff definitions here.
static VO_S16 g_lumaFilter[4][NTAPS_LUMA] =
{
  {  0, 0,   0, 64,  0,   0, 0,  0 },
  { -1, 4, -10, 58, 17,  -5, 1,  0 },
  { -1, 4, -11, 40, 40, -11, 4, -1 },
  {  0, 1,  -5, 17, 58, -10, 4, -1 }
};

// Interpolation chroma coeff definitions here.
static VO_S16 g_chromaFilter[8][NTAPS_CHROMA] =
{
  {  0, 64,  0,  0 },
  { -2, 58, 10, -2 },
  { -4, 54, 16, -2 },
  { -6, 46, 28, -4 },
  { -4, 36, 36, -4 },
  { -4, 28, 46, -6 },
  { -2, 16, 54, -4 },
  { -2, 10, 58, -2 }
};

// Bit-depth used in this module.
static const VO_U8 g_bitDepth = 8;


static void FilterCopy8to8(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height)
{
  VO_S32 row, col;
  //assert(width & 0x03 == 0);
  //assert(height & 0x03 == 0);

  for (row = 0; row < height; row++)
  {
    //memcpy(dst, src, width);
     for (col = 0; col < width; col++)
     {
       dst[col] = src[col];
     }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterCopy8to16(VO_U8* src, VO_S32 srcStride, VO_S16* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height)
{
  VO_S32 row, col;
  VO_S32 shift = IF_INTERNAL_PREC - g_bitDepth;
  VO_S16 val;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      val = (src[col] << shift);
      dst[col] = val - (VO_S16)IF_INTERNAL_OFFS;
    }

    src += srcStride;
    dst += dstStride;
  } 
}

// This function performs an execution of 8-tap luma horizontal interpolation filter 
// for single reference frame.
// MODE: 1 src(8-bit) --> dst(8-bit)
static void FilterLumaHor8to8(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 1 << (shift - 1);
  VO_S32 sum_val;

  src -= 3;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 ] * coeff[0];
      sum_val += src[ col + 1 ] * coeff[1];
      sum_val += src[ col + 2 ] * coeff[2];
      sum_val += src[ col + 3 ] * coeff[3];
      sum_val += src[ col + 4 ] * coeff[4];
      sum_val += src[ col + 5 ] * coeff[5];
      sum_val += src[ col + 6 ] * coeff[6];
      sum_val += src[ col + 7 ] * coeff[7];

      sum_val = (( sum_val + offset ) >> shift);    
      dst[col] = (VO_U8)Clip(sum_val); 
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaHor8to8(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 1 << (shift - 1);
  VO_S32 sum_val;

  src -= 1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 ] * coeff[0];
      sum_val += src[ col + 1 ] * coeff[1];
      sum_val += src[ col + 2 ] * coeff[2];
      sum_val += src[ col + 3 ] * coeff[3];

      sum_val = (( sum_val + offset ) >> shift);    
      dst[col] = (VO_U8)Clip(sum_val); 
    }

    src += srcStride;
    dst += dstStride;
  }
}

// When xFrac!=0 && yFrac!=0, we need to perform both horizontal and vertical interpolation. 
// First do horizontal interpolation, and the  put the temp result to dst. which is a value 
// probably bigger than 255, so the dst data type is extended to VO_S16.
// MODE: 2 src(8-bit) --> tmp(16-bit)
static void FilterLumaHor8to16(VO_U8* src, VO_S32 srcStride, VO_S16* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 sum_val;

  src -= 3;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 ] * coeff[0];
      sum_val += src[ col + 1 ] * coeff[1];
      sum_val += src[ col + 2 ] * coeff[2];
      sum_val += src[ col + 3 ] * coeff[3];
      sum_val += src[ col + 4 ] * coeff[4];
      sum_val += src[ col + 5 ] * coeff[5];
      sum_val += src[ col + 6 ] * coeff[6];
      sum_val += src[ col + 7 ] * coeff[7];

      dst[col] = (VO_S16)(( sum_val + offset ) >> shift);    
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaHor8to16(VO_U8* src, VO_S32 srcStride, VO_S16* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 sum_val;

  src -= 1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 ] * coeff[0];
      sum_val += src[ col + 1 ] * coeff[1];
      sum_val += src[ col + 2 ] * coeff[2];
      sum_val += src[ col + 3 ] * coeff[3];

      dst[col] = (VO_S16)(( sum_val + offset ) >> shift);    
    }

    src += srcStride;
    dst += dstStride;
  }
}

// This function performs an execution of 8-tap luma vertical interpolation filter 
// for single reference frame(unique).
// MODE: 1 src(8-bit) --> dst(8-bit)
static void FilterLumaVer8to8(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 1 << (shift - 1);
  VO_S32 sum_val;

  src -= 3*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];
      sum_val += src[ col + 4 * srcStride] * coeff[4];
      sum_val += src[ col + 5 * srcStride] * coeff[5];
      sum_val += src[ col + 6 * srcStride] * coeff[6];
      sum_val += src[ col + 7 * srcStride] * coeff[7];

      sum_val = (( sum_val + offset ) >> shift);   
      dst[col] = (VO_U8)Clip(sum_val);
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaVer8to8(VO_U8* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 1 << (shift - 1);
  VO_S32 sum_val;

  src -= 1*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];

      sum_val = (( sum_val + offset ) >> shift);   
      dst[col] = (VO_U8)Clip(sum_val);
    }

    src += srcStride;
    dst += dstStride;
  }
}

// MODE: 2 src(8-bit) --> temp(16 bit)
static void FilterLumaVer8to16(VO_U8* src, VO_S32 srcStride, VO_S16* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;

  src -= 3*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];
      sum_val += src[ col + 4 * srcStride] * coeff[4];
      sum_val += src[ col + 5 * srcStride] * coeff[5];
      sum_val += src[ col + 6 * srcStride] * coeff[6];
      sum_val += src[ col + 7 * srcStride] * coeff[7];

      dst[col] = (VO_S16)(( sum_val + offset ) >> shift);   
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaVer8to16(VO_U8* src, VO_S32 srcStride, VO_S16* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;

  src -= 1*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];

      dst[col] = (VO_S16)(( sum_val + offset ) >> shift);   
    }

    src += srcStride;
    dst += dstStride;
  }
}

// This routine use the last horizontal filter temp buffer to do next vertical interpolation 
// filter. At this time, final result the real MC pixels.
// MODE: 3 temp(16-bit) --> dst(8-bit)
static void FilterLumaVer16to8(VO_S16* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC + head_room;
  VO_S32 offset = (1 << (shift - 1)) + (IF_INTERNAL_OFFS << IF_FILTER_PREC);

  src -= 3*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];
      sum_val += src[ col + 4 * srcStride] * coeff[4];
      sum_val += src[ col + 5 * srcStride] * coeff[5];
      sum_val += src[ col + 6 * srcStride] * coeff[6];
      sum_val += src[ col + 7 * srcStride] * coeff[7];

      sum_val = (( sum_val + offset ) >> shift);   
      dst[col] = (VO_U8)Clip(sum_val);
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaVer16to8(VO_S16* src, VO_S32 srcStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC + head_room;
  VO_S32 offset = (1 << (shift - 1)) + (IF_INTERNAL_OFFS << IF_FILTER_PREC);

  src -= 1*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];

      sum_val = (( sum_val + offset ) >> shift);   
      dst[col] = (VO_U8)Clip(sum_val);
    }

    src += srcStride;
    dst += dstStride;
  }
}

// MODE: 4 temp(16-bit) --> temp(16-bit)
static void FilterLumaVer16to16(VO_S16* src, VO_S32 srcStride, VO_S16* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 0;

  src -= 3*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];
      sum_val += src[ col + 4 * srcStride] * coeff[4];
      sum_val += src[ col + 5 * srcStride] * coeff[5];
      sum_val += src[ col + 6 * srcStride] * coeff[6];
      sum_val += src[ col + 7 * srcStride] * coeff[7];

      dst[col] = (VO_S16)(( sum_val + offset ) >> shift);   
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaVer16to16(VO_S16* src, VO_S32 srcStride, VO_S16* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S16* coeff)
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 0;

  src -= 1*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];

      dst[col] = (VO_S16)(( sum_val + offset ) >> shift);   
    }

    src += srcStride;
    dst += dstStride;
  }
}

//==================================================================================
// ref: reference frame pixels start address.
// dst: output frame pixels start address
// This module performs an interpolation without weighted pred for unique direction.
//==================================================================================

void MC_InterLuma( VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S32 xFrac, VO_S32 yFrac )
{
  if ( xFrac == 0 && yFrac == 0 )
  {
    FilterCopy8to8(ref, refStride, dst, dstStride, width, height);
  }
  else if ( yFrac == 0 )
  {
    FilterLumaHor8to8(ref, refStride, dst, dstStride, width, height, g_lumaFilter[xFrac]);
  }
  else if ( xFrac == 0 )
  {
    FilterLumaVer8to8(ref, refStride, dst, dstStride, width, height, g_lumaFilter[yFrac]);
  }
  else // xFrac != 0 && yFrac != 0
  {
    DECLARE_ALIGNED(4, VO_S16, block_temp[(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)*(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)]);
    VO_S32 tmp_stride       = MAX_CU_SIZE+MC_LUMA_EXT_SIZE; // this stride is equal to block_temp size.
    VO_S32 filter_size      = NTAPS_LUMA;
    VO_S32 half_filter_size = filter_size >> 1;

    FilterLumaHor8to16(ref - (half_filter_size-1)*refStride, refStride, block_temp, tmp_stride, width, height+filter_size-1, g_lumaFilter[xFrac]);
    FilterLumaVer16to8(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, dst, dstStride, width, height, g_lumaFilter[yFrac]);
  }
}

void MC_InterChroma( VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S32 xFrac, VO_S32 yFrac )
{
  if ( xFrac == 0 && yFrac == 0 )
  {
    FilterCopy8to8(ref, refStride, dst, dstStride, width, height);
  }
  else if ( yFrac == 0 )
  {
    FilterChromaHor8to8(ref, refStride, dst, dstStride, width, height, g_chromaFilter[xFrac]);
  }
  else if ( xFrac == 0 )
  {
    FilterChromaVer8to8(ref, refStride, dst, dstStride, width, height, g_chromaFilter[yFrac]);
  }
  else // xFrac != 0 && yFrac != 0
  {
    DECLARE_ALIGNED(4, VO_S16, block_temp[(MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1))*(MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1))]);
    VO_S32 tmp_stride       = MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1);
    VO_S32 filter_size      = NTAPS_CHROMA;
    VO_S32 half_filter_size = filter_size >> 1;

    // First filter horizontal and put to temp. 
    FilterChromaHor8to16(ref - (half_filter_size-1)*refStride, refStride, block_temp, tmp_stride, width, height+filter_size-1, g_chromaFilter[xFrac]);
    // Second use tmp to filter vertical and put to dst.
    FilterChromaVer16to8(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, dst, dstStride, width, height, g_chromaFilter[yFrac]);
  }
}

void AddAverageBi( VO_S16* pred0, VO_S32 predStride0, VO_S16* pred1, VO_S32 predStride1, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height )
{
  VO_S32 x, y;
  VO_S32 shift = IF_INTERNAL_PREC + 1 - g_bitDepth;
  VO_S32 offset = ( 1 << ( shift - 1 ) ) + 2 * IF_INTERNAL_OFFS;

  for ( y = 0; y < height; y++ )
  {
    for ( x = 0; x < width; x ++ )
    {
      dst[ x + 0 ] = (VO_U8)Clip( ( pred0[ x + 0 ] + pred1[ x + 0 ] + offset ) >> shift );
    }

    pred0 += predStride0;
    pred1 += predStride1;
    dst  += dstStride;
  }
}

//==================================================================================
// ref0: reference frame in ref-list 0 where pixels start address.
// refStride0: stride for ref0 frame.
// ref1: reference frame in ref-list 1 where pixels start address.
// refStride1: stride for ref1 frame.
// dst: final bi-prediction pixels 
// dstStride: dst frame stride.
// This module performs an interpolation without weighted pred for bi-direction.
//==================================================================================
void MC_InterLumaBi( 
  VO_U8* ref0, VO_S32 refStride0,
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height,
  VO_S32 xFrac0, VO_S32 yFrac0,
  VO_S32 xFrac1, VO_S32 yFrac1
  )
{
  VO_S32 filter_size      = NTAPS_LUMA;
  VO_S32 half_filter_size = (NTAPS_LUMA >> 1);
  VO_S32 tmp_stride       = MAX_CU_SIZE+MC_LUMA_EXT_SIZE; // this stride is equal to block_temp size.
  DECLARE_ALIGNED(4, VO_S16, block_temp[(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)*(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)]);
  DECLARE_ALIGNED(4, VO_S16, pred_temp[2][MAX_CU_SIZE*MAX_CU_SIZE]);
  //VO_S16 block_temp[(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)*(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)];
  //VO_S16 pred_temp[2][MAX_CU_SIZE*MAX_CU_SIZE];

  // MC Interpolation with ref0.
  if ( xFrac0 == 0 && yFrac0 == 0 )
  {
    FilterCopy8to16(ref0, refStride0, pred_temp[0], MAX_CU_SIZE, width, height);
  }
  else if ( yFrac0 == 0 )
  {
    FilterLumaHor8to16(ref0, refStride0, pred_temp[0], MAX_CU_SIZE, width, height, g_lumaFilter[xFrac0]);
  }
  else if ( xFrac0 == 0 )
  {
    FilterLumaVer8to16(ref0, refStride0, pred_temp[0], MAX_CU_SIZE, width, height, g_lumaFilter[yFrac0]);
  }
  else // xFrac != 0 && yFrac != 0
  {
    FilterLumaHor8to16(ref0 - (half_filter_size-1)*refStride0, refStride0, block_temp, tmp_stride, width, height+filter_size-1, g_lumaFilter[xFrac0]);
    FilterLumaVer16to16(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, pred_temp[0], MAX_CU_SIZE, width, height, g_lumaFilter[yFrac0]);
  }

  // MC Interpolation with ref1.
  if ( xFrac1 == 0 && yFrac1 == 0 )
  {
    FilterCopy8to16(ref1, refStride1, pred_temp[1], MAX_CU_SIZE, width, height);
  }
  else if ( yFrac1 == 0 )
  {
    FilterLumaHor8to16(ref1, refStride1, pred_temp[1], MAX_CU_SIZE, width, height, g_lumaFilter[xFrac1]);
  }
  else if ( xFrac1 == 0 )
  {
    FilterLumaVer8to16(ref1, refStride1, pred_temp[1], MAX_CU_SIZE, width, height, g_lumaFilter[yFrac1]);
  }
  else // xFrac != 0 && yFrac != 0
  {
    FilterLumaHor8to16(ref1 - (half_filter_size-1)*refStride1, refStride1, block_temp, tmp_stride, width, height+filter_size-1, g_lumaFilter[xFrac1]);
    FilterLumaVer16to16(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, pred_temp[1], MAX_CU_SIZE, width, height, g_lumaFilter[yFrac1]);
  }

  AddAverageBi(pred_temp[0], MAX_CU_SIZE, pred_temp[1], MAX_CU_SIZE, dst, dstStride, width, height);
}

void MC_InterChromaBi( 
  VO_U8* ref0, VO_S32 refStride0,
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height,
  VO_S32 xFrac0, VO_S32 yFrac0,
  VO_S32 xFrac1, VO_S32 yFrac1
  )
{
  VO_S32 filter_size      = NTAPS_CHROMA;
  VO_S32 half_filter_size = NTAPS_CHROMA >> 1;
  VO_S32 tmp_stride       = MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1);
  DECLARE_ALIGNED(4, VO_S16, block_temp[(MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1))*(MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1))]);
  DECLARE_ALIGNED(4, VO_S16, pred_temp[2][(MAX_CU_SIZE>>1)*(MAX_CU_SIZE>>1)]);

  // MC Interpolation with ref0.
  if ( xFrac0 == 0 && yFrac0 == 0 )
  {
    FilterCopy8to16(ref0, refStride0, pred_temp[0], MAX_CU_SIZE>>1, width, height);
  }
  else if ( yFrac0 == 0 )
  {
    FilterChromaHor8to16(ref0, refStride0, pred_temp[0], MAX_CU_SIZE>>1, width, height, g_chromaFilter[xFrac0]);
  }
  else if ( xFrac0 == 0 )
  {
    FilterChromaVer8to16(ref0, refStride0, pred_temp[0], MAX_CU_SIZE>>1, width, height, g_chromaFilter[yFrac0]);
  }
  else // xFrac != 0 && yFrac != 0
  {
    FilterChromaHor8to16(ref0 - (half_filter_size-1)*refStride0, refStride0, block_temp, tmp_stride, width, height+filter_size-1, g_chromaFilter[xFrac0]);
    FilterChromaVer16to16(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, pred_temp[0], MAX_CU_SIZE>>1, width, height, g_chromaFilter[yFrac0]);
  }

  // MC Interpolation with ref1.
  if ( xFrac1 == 0 && yFrac1 == 0 )
  {
    FilterCopy8to16(ref1, refStride1, pred_temp[1], MAX_CU_SIZE>>1, width, height);
  }
  else if ( yFrac1 == 0 )
  {
    FilterChromaHor8to16(ref1, refStride1, pred_temp[1], MAX_CU_SIZE>>1, width, height, g_chromaFilter[xFrac1]);
  }
  else if ( xFrac1 == 0 )
  {
    FilterChromaVer8to16(ref1, refStride1, pred_temp[1], MAX_CU_SIZE>>1, width, height, g_chromaFilter[yFrac1]);
  }
  else // xFrac != 0 && yFrac != 0
  {
    FilterChromaHor8to16(ref1 - (half_filter_size-1)*refStride1, refStride1, block_temp, tmp_stride, width, height+filter_size-1, g_chromaFilter[xFrac1]);
    FilterChromaVer16to16(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, pred_temp[1], MAX_CU_SIZE>>1, width, height, g_chromaFilter[yFrac1]);
  }

  AddAverageBi(pred_temp[0], MAX_CU_SIZE>>1, pred_temp[1], MAX_CU_SIZE>>1, dst, dstStride, width, height);
}

#if 0
VO_VOID MC_InterLuma_neon(VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S32 xFrac, VO_S32 yFrac)
{
  VO_U32 i = 0, j = 0;
  VO_U32 row = 0, col = 0;
  VO_U32 row2 = 0, col2 = 0;
  VO_U32 rowResi = 0, colResi = 0;
  VO_U8 *refAddr, *dstAddr;

  row = height/4, col = width/4;
  row2 = height/8, col2 = width/8;

  if (row2 > 0 && col2 > 0)
  {
    // (width > 8) & (height > 8)
    for (i = 0; i < row2; i++)
    {
      for (j = 0; j < col2; j ++)
      {
        refAddr = ref + i*8*refStride + j*8;
        dstAddr = dst + i*8*dstStride + j*8;
        MC_InterLuma_8x8_neon(refAddr, refStride, dstAddr, dstStride, xFrac, yFrac);
      }
    }

    rowResi = row - row2*2;
    colResi = col - col2*2;
    if (rowResi > 0 || colResi > 0)
    {
      // calculate residual block.
      if (row2 > col2)
      {
        // residual is 4x16
        ref = ref + col2*8;
        dst = dst + col2*8;
        col -= col2*2;
        goto ASYMETRIC;
      }
      else if (row2 < col2)
      {
        // residual is 16x4
        ref = ref + row2*8*refStride;
        dst = dst + row2*8*dstStride;
        row -= row2*2;
        goto ASYMETRIC;
      }
      else
      {
        assert(0);
      }
    }
    else
    {
      return;
    }
  }
  else
  {
ASYMETRIC:
    // (width == 4) || (height == 4)
    for (i = 0; i < row; i++)
    {
      for (j = 0; j < col; j++)
      {
        refAddr = ref + i*4*refStride + j*4;
        dstAddr = dst + i*4*dstStride + j*4;
        MC_InterLuma_4x4_neon(refAddr, refStride, dstAddr, dstStride, xFrac, yFrac);
      }
    }
  }
}

VO_VOID MC_InterChroma_neon( VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S32 xFrac, VO_S32 yFrac )
{
  VO_U32 i = 0, j = 0;
  VO_U32 row = 0, col = 0;
  VO_U32 row2 = 0, col2 = 0;
  VO_U32 row3 = 0, col3 = 0;
  VO_U32 rowResi = 0, colResi = 0;
  VO_U8 *refAddr, *dstAddr;

  row  = height/2, col  = width/2;
  row2 = height/4, col2 = width/4;
  row3 = height/8, col3 = width/8;

  if ( (row == 1) || (col == 1) || (row == 3) || (col == 3) )
  { // 2x8 6x8 8x2 8x6 block.
    for (i = 0; i < row; i++)
    {
      for (j = 0; j < col; j ++)
      {
        refAddr = ref + i*2*refStride + j*2;
        dstAddr = dst + i*2*dstStride + j*2;

        MC_InterChroma(refAddr, refStride, dstAddr, dstStride, 2, 2, xFrac, yFrac);
      }
    }
    return;
  }

  if (row3 > 0 && col3 > 0)
  {
    // (width > 8) & (height > 8)
    for (i = 0; i < row3; i++)
    {
      for (j = 0; j < col3; j ++)
      {
        refAddr = ref + i*8*refStride + j*8;
        dstAddr = dst + i*8*dstStride + j*8;
        MC_InterChroma_8x8_neon(refAddr, refStride, dstAddr, dstStride, xFrac, yFrac);
      }
    }

    rowResi = row2 - row3*2;
    colResi = col2 - col3*2;
    if (rowResi > 0 || colResi > 0)
    {
      // calculate residual block.
      if (row3 > col3)
      {
        // residual is 4x16
        ref = ref + col3*8;
        dst = dst + col3*8;
        col2 -= col3*2;
        goto ASYMETRIC;
      }
      else if (row3 < col3)
      {
        // residual is 16x4
        ref = ref + row3*8*refStride;
        dst = dst + row3*8*dstStride;
        row2 -= row3*2;
        goto ASYMETRIC;
      }
      else
      {
        assert(0);
      }
    }
    else
    {
      return;
    }
  }
  else
  {
ASYMETRIC:
    // (width == 4) || (height == 4)
    for (i = 0; i < row2; i++)
    {
      for (j = 0; j < col2; j++)
      {
        refAddr = ref + i*4*refStride + j*4;
        dstAddr = dst + i*4*dstStride + j*4;
        MC_InterChroma_4x4_neon(refAddr, refStride, dstAddr, dstStride, xFrac, yFrac);
      }
    }
  }
}

VO_VOID MC_InterLumaBi_neon(VO_U8* ref0, VO_S32 refStride0, VO_U8* ref1, VO_S32 refStride1, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S32 xFrac0, VO_S32 yFrac0, VO_S32 xFrac1, VO_S32 yFrac1)
{
  DECLARE_ALIGNED(4, VO_S16, pred_temp[2][MAX_CU_SIZE*MAX_CU_SIZE]);
  VO_U32 i = 0, j = 0;
  VO_U32 row = 0, col = 0;
  VO_U32 row2 = 0, col2 = 0;
  VO_U32 rowResi = 0, colResi = 0;
  VO_U8 *refAddr0, *refAddr1, *dstAddr;

  row = height/4, col = width/4;
  row2 = height/8, col2 = width/8;

  if (row2 > 0 && col2 > 0)
  {
    // (width > 8) & (height > 8)
    for (i = 0; i < row2; i++)
    {
      for (j = 0; j < col2; j ++)
      {
        refAddr0 = ref0 + i*8*refStride0 + j*8;
        refAddr1 = ref1 + i*8*refStride1 + j*8;
        dstAddr = dst + i*8*dstStride + j*8;

        MC_InterLumaBi_8x8_neon(refAddr0, refStride0, pred_temp[0], MAX_CU_SIZE, xFrac0, yFrac0);
        MC_InterLumaBi_8x8_neon(refAddr1, refStride1, pred_temp[1], MAX_CU_SIZE, xFrac1, yFrac1);
        AddAverageBi_8x8_neon(pred_temp[0], MAX_CU_SIZE, pred_temp[1], MAX_CU_SIZE, dstAddr, dstStride);
      }
    }

    rowResi = row - row2*2;
    colResi = col - col2*2;
    if (rowResi > 0 || colResi > 0)
    {
      // calculate residual block.
      if (row2 > col2)
      {
        // residual is 4x16
        ref0 += col2*8;
        ref1 += col2*8;
        dst += col2*8;
        col -= col2*2;
        goto ASYMETRIC;
      }
      else if (row2 < col2)
      {
        // residual is 16x4
        ref0 += row2*8*refStride0;
        ref1 += row2*8*refStride1;
        dst += row2*8*dstStride;
        row -= row2*2;
        goto ASYMETRIC;
      }
      else
      {
        assert(0);
      }
    }
    else
    {
      return;
    }
  }
  else
  {
ASYMETRIC:
    // (width == 4) || (height == 4)
    for (i = 0; i < row; i++)
    {
      for (j = 0; j < col; j++)
      {
        refAddr0 = ref0 + i*4*refStride0 + j*4;
        refAddr1 = ref1 + i*4*refStride1 + j*4;
        dstAddr = dst + i*4*dstStride + j*4;

        MC_InterLumaBi_4x4_neon(refAddr0, refStride0, pred_temp[0], MAX_CU_SIZE, xFrac0, yFrac0);
        MC_InterLumaBi_4x4_neon(refAddr1, refStride1, pred_temp[1], MAX_CU_SIZE, xFrac1, yFrac1);
        AddAverageBi_4x4_neon(pred_temp[0], MAX_CU_SIZE, pred_temp[1], MAX_CU_SIZE, dstAddr, dstStride);
      }
    }
  }
}

VO_VOID MC_InterChromaBi_neon(VO_U8* ref0, VO_S32 refStride0, VO_U8* ref1, VO_S32 refStride1, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height, VO_S32 xFrac0, VO_S32 yFrac0, VO_S32 xFrac1, VO_S32 yFrac1)
{ 
  DECLARE_ALIGNED(4, VO_S16, pred_temp[2][(MAX_CU_SIZE>>1)*(MAX_CU_SIZE>>1)]);
  VO_U32 i = 0, j = 0;
  VO_U32 row = 0, col = 0;
  VO_U32 row2 = 0, col2 = 0;
  VO_U32 row3 = 0, col3 = 0;
  VO_U32 rowResi = 0, colResi = 0;
  VO_U8 *refAddr0, *refAddr1, *dstAddr;

  row  = height/2, col  = width/2;
  row2 = height/4, col2 = width/4;
  row3 = height/8, col3 = width/8;

  if ( (row == 1) || (col == 1) || (row == 3) || (col == 3) )
  { // 2x8 6x8 8x2 8x6 block.
    for (i = 0; i < row; i++)
    {
      for (j = 0; j < col; j ++)
      {
        refAddr0 = ref0 + i*2*refStride0 + j*2;
        refAddr1 = ref1 + i*2*refStride1 + j*2;
        dstAddr  = dst + i*2*dstStride + j*2;

        MC_InterChromaBi(refAddr0, refStride0, refAddr1, refStride1, dstAddr, dstStride, 2, 2, xFrac0, yFrac0, xFrac1, yFrac1);
      }
    }
    return;
  }

  if (row3 > 0 && col3 > 0)
  {
    // (width > 8) & (height > 8)
    for (i = 0; i < row3; i++)
    {
      for (j = 0; j < col3; j ++)
      {
        refAddr0 = ref0 + i*8*refStride0 + j*8;
        refAddr1 = ref1 + i*8*refStride1 + j*8;
        dstAddr  = dst + i*8*dstStride + j*8;

        MC_InterChromaBi_8x8_neon(refAddr0, refStride0, pred_temp[0], MAX_CU_SIZE>>1, xFrac0, yFrac0);
        MC_InterChromaBi_8x8_neon(refAddr1, refStride1, pred_temp[1], MAX_CU_SIZE>>1, xFrac1, yFrac1);
        AddAverageBi_8x8_neon(pred_temp[0], MAX_CU_SIZE>>1, pred_temp[1], MAX_CU_SIZE>>1, dstAddr, dstStride);
      }
    }

    rowResi = row2 - row3*2;
    colResi = col2 - col3*2;
    if (rowResi > 0 || colResi > 0)
    {
      // calculate residual block.
      if (row3 > col3)
      {
        // residual is 4x16
        ref0 += col3*8;
        ref1 += col3*8;
        dst  += col3*8;
        col2 -= col3*2;
        goto ASYMETRIC;
      }
      else if (row3 < col3)
      {
        // residual is 16x4
        ref0 = ref0 + row3*8*refStride0;
        ref1 = ref1 + row3*8*refStride1;
        dst = dst + row3*8*dstStride;
        row2 -= row3*2;
        goto ASYMETRIC;
      }
      else
      {
        assert(0);
      }
    }
    else
    {
      return;
    }
  }
  else
  {
ASYMETRIC:
    // (width == 4) || (height == 4)
    for (i = 0; i < row2; i++)
    {
      for (j = 0; j < col2; j++)
      {
        refAddr0 = ref0 + i*4*refStride0 + j*4;
        refAddr1 = ref1 + i*4*refStride1 + j*4;
        dstAddr  = dst + i*4*dstStride + j*4;

        MC_InterChromaBi_4x4_neon(refAddr0, refStride0, pred_temp[0], MAX_CU_SIZE>>1, xFrac0, yFrac0);
        MC_InterChromaBi_4x4_neon(refAddr1, refStride1, pred_temp[1], MAX_CU_SIZE>>1, xFrac1, yFrac1);
        AddAverageBi_4x4_neon(pred_temp[0], MAX_CU_SIZE>>1, pred_temp[1], MAX_CU_SIZE>>1, dstAddr, dstStride);
      }
    }
  }
}

VO_VOID CopyBlock_neon(VO_U8* ref, VO_S32 refStride, VO_U8* dst, VO_S32 dstStride, VO_S32 width, VO_S32 height)
{
  VO_U32 i = 0, j = 0;
  VO_U32 row = 0, col = 0;
  VO_U32 row2 = 0, col2 = 0;
  VO_U32 row3 = 0, col3 = 0;
  VO_U32 rowResi = 0, colResi = 0;
  VO_U8 *refAddr, *dstAddr;

  row = height/4, col = width/4;
  row2 = height/8, col2 = width/8;
  row3 = height/16, col3 = width/16;

  if (row3 > 0 && col3 > 0)
  {// (width >= 16) & (height >= 16)
   // 16x16 16x32 16x64 24x32 32x16 32x24 32x32 32x64 48x64 64x16 64x32 64x48 64x64
    for (i = 0; i < row3; i++)
    {
      for (j = 0; j < col3; j ++)
      {
        refAddr = ref + i*16*refStride + j*16;
        dstAddr = dst + i*16*dstStride + j*16;
        CopyBlock_16x16_neon(refAddr, refStride, dstAddr, dstStride);
      }
    }

    rowResi = row2 - row3*2;
    colResi = col2 - col3*2;
    if (rowResi > 0 || colResi > 0)
    {
      // calculate residual block.
      if (row3 > col3)
      {// residual is 8x32
        ref += col3*16;
        dst += col3*16;
        for (i = 0; i < 4; i++)
        {
          refAddr = ref+i*8*refStride;
          dstAddr = dst+i*8*dstStride;
          CopyBlock_8x8_neon(refAddr, refStride, dstAddr, dstStride);
        }
      }
      else if (row3 < col3)
      {// residual is 32x8
        ref += row3*16*refStride;
        dst += row3*16*dstStride;
        for (j = 0; j < 4; j++)
        {
          refAddr = ref + j*8;
          dstAddr = dst + j*8;
          CopyBlock_8x8_neon(refAddr, refStride, dstAddr, dstStride);
        }
      }
      else
      {
        assert(0);
      }
    }
    else
    {
      return;
    }
  }
  else if (row2 > 0 && col2 > 0)
  {// 8x8 8x16 8x32 12x16 16x8 16x12 32x8
    for (i = 0; i < row2; i++)
    {
      for (j = 0; j < col2; j ++)
      {
        refAddr = ref + i*8*refStride + j*8;
        dstAddr = dst + i*8*dstStride + j*8;
        CopyBlock_8x8_neon(refAddr, refStride, dstAddr, dstStride);
      }
    }

    rowResi = row - row2*2;
    colResi = col - col2*2;
    if (rowResi > 0 || colResi > 0)
    {
      // calculate residual block.
      if (row2 > col2)
      {// residual is 4x16
        ref += col2*8;
        dst += col2*8;
        for (i = 0; i < 4; i++)
        {
          refAddr = ref + i*4*refStride;
          dstAddr = dst + i*4*dstStride;
          CopyBlock_4x4_neon(refAddr, refStride, dstAddr, dstStride);
        }
      }
      else if (row2 < col2)
      {// residual is 16x4
        ref += row2*8*refStride;
        dst += row2*8*dstStride;
        for (j = 0; j < 2; j++)
        {
          refAddr = ref + j*8;
          dstAddr = dst + j*8;
          CopyBlock_8x4_neon(refAddr, refStride, dstAddr, dstStride);
        }
      }
      else
      {
        assert(0);
      }
    }
    else
    {
      return;
    }
  }
  else
  {
    row = height/2, col = width/2;
    if((row == 1) || (col == 1) || (row == 3) || (col == 3))
    {// 2x4 4x2 2x8 6x8 8x2 8x6
      for (i = 0; i < height; i++)
      {
        for (j = 0; j < width; j++)
        {
          dst[j] = ref[j];
        }

        ref += refStride;
        dst += dstStride;
      }
      return;
    }

    // 4x4 4x8 4x16 8x4 16x4
    row = height/4, col = width/4;
    if (row >= col)
    {// 4x4 4x8 4x16
      for (i = 0; i < row; i++)
      {
        for (j = 0; j < col; j ++)
        {
          refAddr = ref + i*4*refStride + j*4;
          dstAddr = dst + i*4*dstStride + j*4;
          CopyBlock_4x4_neon(refAddr, refStride, dstAddr, dstStride);
        }
      }
    }
    else
    {// 8x4 16x4
      for (j = 0; j < col2; j++)
      {
        refAddr = ref + j*8;
        dstAddr = dst + j*8;
        CopyBlock_8x4_neon(refAddr, refStride, dstAddr, dstStride);
      }
    }
  }
}
#endif

__inline VO_U8 WeightUnidir( VO_S32 w0, VO_S32 p0, VO_S32 round, VO_S32 shift, VO_S32 offset) 
{
  return (VO_U8)Clip( ( (w0*(p0 + IF_INTERNAL_OFFS) + round) >> shift ) + offset );
}

//=======================================================================================================
// Weighted routine module implementations.

static void FilterCopyWeighted8to8(
  VO_U8* src, VO_S32 srcStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  const WP_SCALING_PARAM* wp 
  )
{
  VO_S32 row, col;
  VO_S32 shift = IF_INTERNAL_PREC - g_bitDepth;
  VO_S16 val;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      val = (src[col] << shift) - (VO_S16)IF_INTERNAL_OFFS;

      // do weighted prediction copy.
      dst[col] = WeightUnidir(wp->w, val, wp->round, wp->shift, wp->offset);
    }

    src += srcStride;
    dst += dstStride;
  } 
}


// This case is invoked only when weighted prediction is enabled, and yFrac = 0 && xFrac != 0, 
// which means only perform horizontal interpolation.
static void FilterLumaHorWeighted8to8( 
  VO_U8* src, VO_S32 srcStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S16* coeff, 
  const WP_SCALING_PARAM* wp 
  )
{
  VO_S32 row, col;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 sum_val;

  src -= 3;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 ] * coeff[0];
      sum_val += src[ col + 1 ] * coeff[1];
      sum_val += src[ col + 2 ] * coeff[2];
      sum_val += src[ col + 3 ] * coeff[3];
      sum_val += src[ col + 4 ] * coeff[4];
      sum_val += src[ col + 5 ] * coeff[5];
      sum_val += src[ col + 6 ] * coeff[6];
      sum_val += src[ col + 7 ] * coeff[7];

      sum_val = ( sum_val + offset ) >> shift;  

      // Do weighted calculation.
      dst[col] = WeightUnidir(wp->w, sum_val, wp->round, wp->shift, wp->offset);
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaHorWeighted8to8( 
  VO_U8* src, VO_S32 srcStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S16* coeff, 
  const WP_SCALING_PARAM* wp 
  )
{
  VO_S32 row, col;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 sum_val;

  src -= 1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 ] * coeff[0];
      sum_val += src[ col + 1 ] * coeff[1];
      sum_val += src[ col + 2 ] * coeff[2];
      sum_val += src[ col + 3 ] * coeff[3];

      sum_val = ( sum_val + offset ) >> shift;  

      // Do weighted calculation.
      dst[col] = WeightUnidir(wp->w, sum_val, wp->round, wp->shift, wp->offset);
    }

    src += srcStride;
    dst += dstStride;
  }
}


// this routine is only used in the condition of bi-dir weighted prediction.
static void FilterLumaHorWeighted8to16( 
  VO_U8* src, VO_S32 srcStride, 
  VO_S16* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S16* coeff, 
  const WP_SCALING_PARAM* wp
  )
{
  VO_S32 row, col;
  VO_S32 head_room;
  VO_S32 offset;
  VO_S32 shift;
  VO_S32 sum_val;

  src -= 3;
  head_room = IF_INTERNAL_PREC - g_bitDepth;
  shift = IF_FILTER_PREC;

  shift -= head_room;
  offset = -IF_INTERNAL_OFFS << shift;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 ] * coeff[0];
      sum_val += src[ col + 1 ] * coeff[1];
      sum_val += src[ col + 2 ] * coeff[2];
      sum_val += src[ col + 3 ] * coeff[3];
      sum_val += src[ col + 4 ] * coeff[4];
      sum_val += src[ col + 5 ] * coeff[5];
      sum_val += src[ col + 6 ] * coeff[6];
      sum_val += src[ col + 7 ] * coeff[7];

      dst[col] = (VO_S16)(( sum_val + offset ) >> shift);    
    }

    src += srcStride;
    dst += dstStride;
  }
}

// This case is invoked only when weighted prediction is enabled, and xFrac = 0 && yFrac != 0, 
// which means only perform vertical interpolation. 
static void FilterLumaVerWeighted8to8( 
  VO_U8* src, VO_S32 srcStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S16* coeff, 
  const WP_SCALING_PARAM* wp )
{
  VO_S32 row, col;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 sum_val;

  src -= 3*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];
      sum_val += src[ col + 4 * srcStride] * coeff[4];
      sum_val += src[ col + 5 * srcStride] * coeff[5];
      sum_val += src[ col + 6 * srcStride] * coeff[6];
      sum_val += src[ col + 7 * srcStride] * coeff[7];

      sum_val = ( sum_val + offset ) >> shift;
      
      // Do weighted calculation.
      dst[col] = WeightUnidir(wp->w, sum_val, wp->round, wp->shift, wp->offset);
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaVerWeighted8to8( 
  VO_U8* src, VO_S32 srcStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S16* coeff, 
  const WP_SCALING_PARAM* wp 
  )
{
  VO_S32 row, col;
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 sum_val;

  src -= 1*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];

      sum_val = ( sum_val + offset ) >> shift;

      // Do weighted calculation.
      dst[col] = WeightUnidir(wp->w, sum_val, wp->round, wp->shift, wp->offset);
    }

    src += srcStride;
    dst += dstStride;
  }
}


static void FilterLumaVerWeighted16to8( 
  VO_S16* src, VO_S32 srcStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S16* coeff, 
  const WP_SCALING_PARAM* wp 
  )
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 0;

  src -= 3*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];
      sum_val += src[ col + 4 * srcStride] * coeff[4];
      sum_val += src[ col + 5 * srcStride] * coeff[5];
      sum_val += src[ col + 6 * srcStride] * coeff[6];
      sum_val += src[ col + 7 * srcStride] * coeff[7];

      sum_val = ( sum_val + offset ) >> shift;   
      
      // Do weighted calculation.
      dst[col] = WeightUnidir(wp->w, sum_val, wp->round, wp->shift, wp->offset);
    }

    src += srcStride;
    dst += dstStride;
  }
}

static void FilterChromaVerWeighted16to8( 
  VO_S16* src, VO_S32 srcStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S16* coeff, 
  const WP_SCALING_PARAM* wp 
  )
{
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 0;

  src -= 1*srcStride;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = src[ col + 0 * srcStride] * coeff[0];
      sum_val += src[ col + 1 * srcStride] * coeff[1];
      sum_val += src[ col + 2 * srcStride] * coeff[2];
      sum_val += src[ col + 3 * srcStride] * coeff[3];

      sum_val = ( sum_val + offset ) >> shift;   

      // Do weighted calculation.
      dst[col] = WeightUnidir(wp->w, sum_val, wp->round, wp->shift, wp->offset);
    }

    src += srcStride;
    dst += dstStride;
  }
}

//==================================================================================
// ref: reference frame pixels start address.
// dst: output frame pixels start address
// This module performs an interpolation with weighted pred for unique direction(with multiplication wp coeff).
//==================================================================================
void MC_InterLumaWeighted( 
  VO_U8* ref, VO_S32 refStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S32 xFrac, VO_S32 yFrac, 
  const WP_SCALING_PARAM* wp 
  )
{
  if ( xFrac == 0 && yFrac == 0 )
  {
    FilterCopyWeighted8to8(ref, refStride, dst, dstStride, width, height, wp);
  }
  else if ( yFrac == 0 )
  {
    FilterLumaHorWeighted8to8(ref, refStride, dst, dstStride, width, height, g_lumaFilter[xFrac], wp);
  }
  else if ( xFrac == 0 )
  {
    FilterLumaVerWeighted8to8(ref, refStride, dst, dstStride, width, height, g_lumaFilter[yFrac], wp);
  }
  else // xFrac != 0 && yFrac != 0
  {
    DECLARE_ALIGNED(4, VO_S16, block_temp[MAX_CU_SIZE*MAX_CU_SIZE]);
    VO_S32 tmp_stride = MAX_CU_SIZE;
    VO_S32 filter_size = NTAPS_LUMA;
    VO_S32 half_filter_size = filter_size >> 1;

    FilterLumaHor8to16(ref - (half_filter_size-1)*refStride, refStride, block_temp, tmp_stride, width, height+filter_size-1, g_lumaFilter[xFrac]);
    FilterLumaVerWeighted16to8(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, dst, dstStride, width, height, g_lumaFilter[yFrac], wp);
  }
}

void MC_InterChromaWeighted( 
  VO_U8* ref, VO_S32 refStride, 
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S32 xFrac, VO_S32 yFrac, 
  const WP_SCALING_PARAM* wp 
  )
{
  if ( xFrac == 0 && yFrac == 0 )
  {
    FilterCopyWeighted8to8(ref, refStride, dst, dstStride, width, height, wp);
  }
  else if ( yFrac == 0 )
  {
    FilterChromaHorWeighted8to8(ref, refStride, dst, dstStride, width, height, g_chromaFilter[xFrac], wp);
  }
  else if ( xFrac == 0 )
  {
    FilterChromaVerWeighted8to8(ref, refStride, dst, dstStride, width, height, g_chromaFilter[yFrac], wp);
  }
  else // xFrac != 0 && yFrac != 0
  {
    DECLARE_ALIGNED(4, VO_S16, block_temp[MAX_CU_SIZE*MAX_CU_SIZE]);
    VO_S32 tmp_stride = MAX_CU_SIZE;
    VO_S32 filter_size = NTAPS_CHROMA;
    VO_S32 half_filter_size = filter_size >> 1;
    FilterChromaHor8to16(ref - (half_filter_size-1)*refStride, refStride, block_temp, tmp_stride, width, height+filter_size-1, g_chromaFilter[xFrac]);
    FilterChromaVerWeighted16to8(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, dst, dstStride, width, height, g_chromaFilter[yFrac], wp);
  }
}

__inline VO_U8 WeightBidir( VO_S32 w0, VO_S16 p0, VO_S32 w1, VO_S16 p1, VO_S32 round, VO_S32 shift, VO_S32 offset)
{
  return (VO_U8)Clip( ( (w0*(p0 + IF_INTERNAL_OFFS) + w1*(p1 + IF_INTERNAL_OFFS) + round + (offset << (shift-1))) >> shift ) );
}

//================================================================================
// ref0: src ref0 with  no weighted coeff MC temp buffer ,used to do bi-weighted with ref1.
// ref1: src ref1 of original buffer.
// dst:  final buffer with bi-weighted prediction.
// DESC: This routine accepts one ref0 buffer of MC no weighted coeff and original
//       ref1 coeff. In method, ref1 is pred with MC no weighted coeff and this result
//       is treated as another temp buffer. Finally two temp buffer are add weighted
//       and put the result to dst buffer.
//================================================================================
static void FilterCopyWeightedBi(
  VO_S16* ref0, VO_S32 refStride0,
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride,
  VO_S32 width, VO_S32 height,
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 row, col;
  VO_S32 shift = IF_INTERNAL_PREC - g_bitDepth;
  VO_S16 pred_temp;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      // Do MC for ref1 with no weighted coeff.
      pred_temp = (ref1[col] << shift) - (VO_S16)IF_INTERNAL_OFFS;

      // Do weighted avg using wp0 and wp1 with bi-temp buffer.
      dst[col] = WeightBidir( wp0->w, ref0[col], wp1->w, pred_temp, wp0->round, wp0->shift, wp0->offset);
    }

    ref0 += refStride0;
    ref1 += refStride1;
    dst  += dstStride;
  } 
}

static void FilterLumaHorWeightedBi(
  VO_S16* ref0, VO_S32 refStride0,
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride,
  VO_S32 width, VO_S32 height,
  VO_S16* coeff, /* coeff belongs to ref1 */
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S16 pred_temp;

  ref1 -= 3;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = ref1[ col + 0 ] * coeff[0];
      sum_val += ref1[ col + 1 ] * coeff[1];
      sum_val += ref1[ col + 2 ] * coeff[2];
      sum_val += ref1[ col + 3 ] * coeff[3];
      sum_val += ref1[ col + 4 ] * coeff[4];
      sum_val += ref1[ col + 5 ] * coeff[5];
      sum_val += ref1[ col + 6 ] * coeff[6];
      sum_val += ref1[ col + 7 ] * coeff[7];

      // Do normal MC using ref1, and save the val to temp.
      pred_temp = (VO_S16)(( sum_val + offset ) >> shift);  

      // Do weighted avg using wp0 and wp1 with bi-temp buffer.
      dst[col] = WeightBidir( wp0->w, ref0[col], wp1->w, pred_temp, wp0->round, wp0->shift, wp0->offset);
    }

    ref0 += refStride0;
    ref1 += refStride1;
    dst += dstStride;
  }
}

static void FilterChromaHorWeightedBi(
  VO_S16* ref0, VO_S32 refStride0,
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride,
  VO_S32 width, VO_S32 height,
  VO_S16* coeff, /* coeff belongs to ref1 */
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S16 pred_temp;

  ref1 -= 1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = ref1[ col + 0 ] * coeff[0];
      sum_val += ref1[ col + 1 ] * coeff[1];
      sum_val += ref1[ col + 2 ] * coeff[2];
      sum_val += ref1[ col + 3 ] * coeff[3];

      // Do normal MC using ref1, and save the val to temp.
      pred_temp = (VO_S16)(( sum_val + offset ) >> shift);  

      // Do weighted avg using wp0 and wp1 with bi-temp buffer.
      dst[col] = WeightBidir( wp0->w, ref0[col], wp1->w, pred_temp, wp0->round, wp0->shift, wp0->offset);
    }

    ref0 += refStride0;
    ref1 += refStride1;
    dst += dstStride;
  }
}

static void FilterLumaVerWeightedBi(
  VO_S16* ref0, VO_S32 refStride0,
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride,
  VO_S32 width, VO_S32 height,
  VO_S16* coeff, /* coeff belongs to ref1 */
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S16 pred_temp;

  ref1 -= 3*refStride1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = ref1[ col + 0 * refStride1] * coeff[0];
      sum_val += ref1[ col + 1 * refStride1] * coeff[1];
      sum_val += ref1[ col + 2 * refStride1] * coeff[2];
      sum_val += ref1[ col + 3 * refStride1] * coeff[3];
      sum_val += ref1[ col + 4 * refStride1] * coeff[4];
      sum_val += ref1[ col + 5 * refStride1] * coeff[5];
      sum_val += ref1[ col + 6 * refStride1] * coeff[6];
      sum_val += ref1[ col + 7 * refStride1] * coeff[7];

      // Do normal MC using ref1, and save the val to temp.
      pred_temp = (VO_S16)(( sum_val + offset ) >> shift);   

      // Do weighted avg using wp0 and wp1 with bi-temp buffer.
      dst[col] = WeightBidir( wp0->w, ref0[col], wp1->w, pred_temp, wp0->round, wp0->shift, wp0->offset);
    }

    ref0 += refStride0;
    ref1 += refStride1;
    dst += dstStride;
  }
}

static void FilterChromaVerWeightedBi(
  VO_S16* ref0, VO_S32 refStride0,
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride,
  VO_S32 width, VO_S32 height,
  VO_S16* coeff, /* coeff belongs to ref1 */
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 head_room = IF_INTERNAL_PREC - g_bitDepth;
  VO_S32 shift = IF_FILTER_PREC - head_room;
  VO_S32 offset = -IF_INTERNAL_OFFS << shift;
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S16 pred_temp;

  ref1 -= 1*refStride1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = ref1[ col + 0 * refStride1] * coeff[0];
      sum_val += ref1[ col + 1 * refStride1] * coeff[1];
      sum_val += ref1[ col + 2 * refStride1] * coeff[2];
      sum_val += ref1[ col + 3 * refStride1] * coeff[3];

      // Do normal MC using ref1, and save the val to temp.
      pred_temp = (VO_S16)(( sum_val + offset ) >> shift);   

      // Do weighted avg using wp0 and wp1 with bi-temp buffer.
      dst[col] = WeightBidir( wp0->w, ref0[col], wp1->w, pred_temp, wp0->round, wp0->shift, wp0->offset);
    }

    ref0 += refStride0;
    ref1 += refStride1;
    dst += dstStride;
  }
}

//================================================================================
// DESC: This routine is invoked when the following conditions are meet. When do Bi-
//       weighted pred. and xFrac1 != 0 && yFrac != 0. Cosidering ref1. First do MC
//       horizontal with FilterLumaHor8to16() and the result is this routine's input
//       parameter ref1.
//       The final result is bi-weighted pred result and put to dst.
//================================================================================
static void FilterLumaCompleteVerWeightedBi(
  VO_S16* ref0, VO_S32 refStride0,
  VO_S16* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride,
  VO_S32 width, VO_S32 height,
  VO_S16* coeff, /* coeff belongs to ref1 yFrac */
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 0;
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S16 pred_temp;

  ref1 -= 3*refStride1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = ref1[ col + 0 * refStride1] * coeff[0];
      sum_val += ref1[ col + 1 * refStride1] * coeff[1];
      sum_val += ref1[ col + 2 * refStride1] * coeff[2];
      sum_val += ref1[ col + 3 * refStride1] * coeff[3];
      sum_val += ref1[ col + 4 * refStride1] * coeff[4];
      sum_val += ref1[ col + 5 * refStride1] * coeff[5];
      sum_val += ref1[ col + 6 * refStride1] * coeff[6];
      sum_val += ref1[ col + 7 * refStride1] * coeff[7];

      pred_temp = (VO_S16)(( sum_val + offset ) >> shift);   

      dst[col] = WeightBidir( wp0->w, ref0[col], wp1->w, pred_temp, wp0->round, wp0->shift, wp0->offset);
    }

    ref0 += refStride0;
    ref1 += refStride1;
    dst += dstStride;
  }
}

static void FilterChromaCompleteVerWeightedBi(
  VO_S16* ref0, VO_S32 refStride0,
  VO_S16* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride,
  VO_S32 width, VO_S32 height,
  VO_S16* coeff, /* coeff belongs to ref1 yFrac */
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 shift = IF_FILTER_PREC;
  VO_S32 offset = 0;
  VO_S32 row, col;
  VO_S32 sum_val;
  VO_S16 pred_temp;

  ref1 -= 1*refStride1;

  for (row = 0; row < height; row++)
  {
    for (col = 0; col < width; col++)
    {
      sum_val  = ref1[ col + 0 * refStride1] * coeff[0];
      sum_val += ref1[ col + 1 * refStride1] * coeff[1];
      sum_val += ref1[ col + 2 * refStride1] * coeff[2];
      sum_val += ref1[ col + 3 * refStride1] * coeff[3];

      pred_temp = (VO_S16)(( sum_val + offset ) >> shift);   

      dst[col] = WeightBidir( wp0->w, ref0[col], wp1->w, pred_temp, wp0->round, wp0->shift, wp0->offset);
    }

    ref0 += refStride0;
    ref1 += refStride1;
    dst += dstStride;
  }
}

//==================================================================================
// ref: reference frame pixels start address.
// dst: output frame pixels start address
// This module performs an interpolation with weighted pred for bi-direction(without 
// multiplication wp coeff). Detail wp attached to pixels are handled in specified method.
//==================================================================================
void MC_InterLumaWeightedBi( 
  VO_U8* ref0, VO_S32 refStride0, 
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S32 xFrac0, VO_S32 yFrac0,
  VO_S32 xFrac1, VO_S32 yFrac1,
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 filter_size = NTAPS_LUMA;
  VO_S32 half_filter_size = filter_size >> 1;
  VO_S32 tmp_stride = MAX_CU_SIZE + MC_LUMA_EXT_SIZE;
  DECLARE_ALIGNED(4, VO_S16, block_temp[(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)*(MAX_CU_SIZE+MC_LUMA_EXT_SIZE)]);
  DECLARE_ALIGNED(4, VO_S16, pred_temp[MAX_CU_SIZE*MAX_CU_SIZE]);

  // First do MC using ref0 without weighted coeff, and put this result to pred_temp.
  // pred_temp is used as second ref1 bi-weighted input reference.
  if ( xFrac0 == 0 && yFrac0 == 0 )
  {
    FilterCopy8to16(ref0, refStride0, pred_temp, MAX_CU_SIZE, width, height);
  }
  else if ( yFrac0 == 0 )
  {
    FilterLumaHor8to16(ref0, refStride0, pred_temp, MAX_CU_SIZE, width, height, g_lumaFilter[xFrac0]);
  }
  else if ( xFrac0 == 0 )
  {
    FilterLumaVer8to16(ref0, refStride0, pred_temp, MAX_CU_SIZE, width, height, g_lumaFilter[yFrac0]);
  }
  else // xFrac != 0 && yFrac != 0
  { 
    FilterLumaHor8to16(ref0 - (half_filter_size-1)*refStride0, refStride0, block_temp, tmp_stride, width, height+filter_size-1, g_lumaFilter[xFrac0]);
    FilterLumaVer16to16(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, pred_temp, MAX_CU_SIZE, width, height, g_lumaFilter[yFrac0]);
  }

  // Second do MC using ref1 and put ref0 and both wp params input method, the final value
  // is what we want.
  if ( xFrac1 == 0 && yFrac1 == 0 )
  {
    FilterCopyWeightedBi(pred_temp, MAX_CU_SIZE, ref1, refStride1, dst, dstStride, width, height, wp0, wp1);
  }
  else if ( yFrac1 == 0 )
  {
    FilterLumaHorWeightedBi(pred_temp, MAX_CU_SIZE, ref1, refStride1, dst, dstStride, width, height, g_lumaFilter[xFrac1], wp0, wp1);
  }
  else if ( xFrac1 == 0 )
  {
    FilterLumaVerWeightedBi(pred_temp, MAX_CU_SIZE, ref1, refStride1, dst, dstStride, width, height, g_lumaFilter[yFrac1], wp0, wp1);
  }
  else
  {
    FilterLumaHor8to16(ref1 - (half_filter_size-1)*refStride1, refStride1, block_temp, tmp_stride, width, height+filter_size-1, g_lumaFilter[xFrac1]);
    FilterLumaCompleteVerWeightedBi(pred_temp, MAX_CU_SIZE, block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, dst, dstStride, width, height, g_lumaFilter[yFrac1], wp0, wp1);
  }
}

void MC_InterChromaWeightedBi( 
  VO_U8* ref0, VO_S32 refStride0, 
  VO_U8* ref1, VO_S32 refStride1,
  VO_U8* dst, VO_S32 dstStride, 
  VO_S32 width, VO_S32 height, 
  VO_S32 xFrac0, VO_S32 yFrac0,
  VO_S32 xFrac1, VO_S32 yFrac1,
  const WP_SCALING_PARAM* wp0, 
  const WP_SCALING_PARAM* wp1
  )
{
  VO_S32 filter_size = NTAPS_CHROMA;
  VO_S32 half_filter_size = filter_size >> 1;
  VO_S32 tmp_stride = MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1);
  DECLARE_ALIGNED(4, VO_S16, block_temp[(MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1))*(MC_CHROMA_EXT_SIZE + (MAX_CU_SIZE >> 1))]);
  DECLARE_ALIGNED(4, VO_S16, pred_temp[(MAX_CU_SIZE >> 1)*(MAX_CU_SIZE >> 1)]);

  if ( xFrac0 == 0 && yFrac0 == 0 )
  {
    FilterCopy8to16(ref0, refStride0, pred_temp, MAX_CU_SIZE>>1, width, height);
  }
  else if ( yFrac0 == 0 )
  {
    FilterChromaHor8to16(ref0, refStride0, pred_temp, MAX_CU_SIZE>>1, width, height, g_chromaFilter[xFrac0]);
  }
  else if ( xFrac0 == 0 )
  {
    FilterChromaVer8to16(ref0, refStride0, pred_temp, MAX_CU_SIZE>>1, width, height, g_chromaFilter[yFrac0]);
  }
  else // xFrac != 0 && yFrac != 0
  { 
    FilterChromaHor8to16(ref0 - (half_filter_size-1)*refStride0, refStride0, block_temp, tmp_stride, width, height+filter_size-1, g_chromaFilter[xFrac0]);
    FilterChromaVer16to16(block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, pred_temp, MAX_CU_SIZE>>1, width, height, g_chromaFilter[yFrac0]);
  }

  if ( xFrac1 == 0 && yFrac1 == 0 )
  {
    FilterCopyWeightedBi(pred_temp, MAX_CU_SIZE>>1, ref1, refStride1, dst, dstStride, width, height, wp0, wp1);
  }
  else if ( yFrac1 == 0 )
  {
    FilterChromaHorWeightedBi(pred_temp, MAX_CU_SIZE>>1, ref1, refStride1, dst, dstStride, width, height, g_chromaFilter[xFrac1], wp0, wp1);
  }
  else if ( xFrac1 == 0 )
  {
    FilterChromaVerWeightedBi(pred_temp, MAX_CU_SIZE>>1, ref1, refStride1, dst, dstStride, width, height, g_chromaFilter[yFrac1], wp0, wp1);
  }
  else
  {
    FilterChromaHor8to16(ref1 - (half_filter_size-1)*refStride1, refStride1, block_temp, tmp_stride, width, height+filter_size-1, g_chromaFilter[xFrac1]);
    FilterChromaCompleteVerWeightedBi(pred_temp, MAX_CU_SIZE>>1, block_temp + (half_filter_size-1)*tmp_stride, tmp_stride, dst, dstStride, width, height, g_chromaFilter[yFrac1], wp0, wp1);
  }
}
#endif
