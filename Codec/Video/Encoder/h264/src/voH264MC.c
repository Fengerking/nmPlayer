/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#include "voH264EncGlobal.h"

#if defined(VOARMV7)

extern void MCCopy16x16_ARMV7(VO_U8 *src, VO_S32 src_stride,
			VO_U8 *dst, VO_S32 dst_stride);
extern void MCCopy8x8_ARMV7(VO_U8 *src, VO_S32 src_stride,
			VO_U8 *dst, VO_S32 dst_stride);

#endif


//add by Really Yang 1209
#if !defined(VOARMV7)
static void MCCopy16x16_C( VO_U8 *src, VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
  VO_U32 i;
  for( i = 4; i != 0; i-- )
  {
    CP32(dst,src);CP32(dst+4,src+4);CP32(dst+8,src+8);CP32(dst+12,src+12);
    src += src_stride;
    dst += dst_stride;
    CP32(dst,src);CP32(dst+4,src+4);CP32(dst+8,src+8);CP32(dst+12,src+12);
    src += src_stride;
    dst += dst_stride;
    CP32(dst,src);CP32(dst+4,src+4);CP32(dst+8,src+8);CP32(dst+12,src+12);
    src += src_stride;
    dst += dst_stride;
	CP32(dst,src);CP32(dst+4,src+4);CP32(dst+8,src+8);CP32(dst+12,src+12);
    src += src_stride;
    dst += dst_stride;
  }
}

static void MCCopy8x8_C( VO_U8 *src, VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
  VO_U32 i;
  for( i = 2; i != 0; i-- )
  {
    CP32(dst,src);CP32(dst+4,src+4);
    src += src_stride;
    dst += dst_stride;
    CP32(dst,src);CP32(dst+4,src+4);
    src += src_stride;
    dst += dst_stride;
	CP32(dst,src);CP32(dst+4,src+4);
    src += src_stride;
    dst += dst_stride;
    CP32(dst,src);CP32(dst+4,src+4);
    src += src_stride;
    dst += dst_stride;
  }
}
#endif
void MCLuma( VO_U8 *dst, VO_S32 dst_stride,
                     VO_U8 *src, VO_S32 src_stride,
                     VO_S32 width, VO_S32 height)
{

  //YU_TBD fraction pixels
#if defined(VOARMV7)
  MCCopy16x16_ARMV7( src, src_stride, dst, dst_stride );
#else
  MCCopy16x16_C( src, src_stride, dst, dst_stride );
#endif
}

//add by Really Yang 1210
void MCChroma8x8_c( VO_U8 *pDst, VO_S32 dst_stride,
                       VO_U8 *pSrc, VO_S32 src_stride,
                       VO_S32 dmvx, VO_S32 dmvy)
{
  VO_U8 *pSrc_nextline;
  VO_U32 x,y,dx,dy,w00,w01,w10,w11;

  dx = 8-dmvx;
  dy = 8-dmvy;
  w00 = dx*dy;
  w01 = dmvx*dy;
  w10 = dx*dmvy;
  w11 = dmvx*dmvy;

  pSrc_nextline = pSrc + src_stride;
  for( y = 8; y != 0; y-- )
  {
    for( x = 0; x < 8; x++ )
    {
      pDst[x] = (VO_U8)((w00*pSrc[x]+w01*pSrc[x+1]+w10*pSrc_nextline[x]+w11*pSrc_nextline[x+1]+32)>>6);
    }
    pDst  += dst_stride;
    pSrc   = pSrc_nextline;
    pSrc_nextline += src_stride;
  }
}
//end of add
void Hpel16x16SearchFilter_HV_C( VO_U8 *dsth, VO_U8 *dstv, VO_U8 *src,VO_S32 stride )
{
  VO_S32 x, y;
  VO_U8 *src1 = src;
  src1 = src - stride;
  for( y=0; y<16+1; y++ )
  {
    for( x=0; x<16; x++ )
      dstv[x] = Clip255((src1[x-2*stride] + src1[x+3*stride] - 5*(src1[x-stride] + src1[x+2*stride]) + 20*(src1[x] + src1[x+stride]) + 16) >> 5);
    dstv += 32;
    src1 += stride;
  }
  src1 = src-1;
  for( y=0; y<16; y++ )
  {
    for( x=0; x<16+1; x++ )
      dsth[x] = Clip255((src1[x-2] + src1[x+3] - 5*(src1[x-1] + src1[x+2]) + 20*(src1[x] + src1[x+1]) + 16) >> 5);
	dsth += 32;
	src1 += stride;
  }
}
//add by Really Yang 1124
void HpelFilter16X16_C_C(VO_U8 *dst, VO_S32 dst_stride,
                     					VO_U8 *src, VO_S32 src_stride,
                     					VO_S16 *buf )
{
  VO_S32 x, y;
  VO_S16 *src2 = buf+2;
  for( y=0; y<16; y++ )
  {
    for( x=-2; x<16+3; x++ )
    {
      buf[x+2]= (VO_S16)(src[x-2*src_stride] + src[x+3*src_stride] - 5*(src[x-src_stride] + src[x+2*src_stride]) + 20*(src[x] + src[x+src_stride]));
    }
    buf += 32;
    src += src_stride;
  }
  for( y=0; y<16; y++ )
  {
    for( x=0; x<16; x++ )
      dst[x] = Clip255(((src2[x-2] + src2[x+3] - 5*(src2[x-1] + src2[x+2]) + 20*(src2[x] + src2[x+1])) + 512) >> 10);
	dst += dst_stride;
	src2 += 32;
  }
}

void HpelFilter16X16_V_C(VO_U8 *dst, VO_S32 dst_stride,
                     			VO_U8 *src, VO_S32 src_stride
                     					)
{
  VO_S32 x, y;
  for( y=0; y<16; y++ )
  {
    for( x=0; x<16; x++ )
      dst[x] = Clip255(((src[x-2*src_stride] + src[x+3*src_stride] - 5*(src[x-src_stride] + src[x+2*src_stride]) + 20*(src[x] + src[x+src_stride])) + 16) >> 5);
	dst += dst_stride;
	src += src_stride;
  }
}

void HpelFilter16X16_H_C(VO_U8 *dst, VO_S32 dst_stride,
                     			VO_U8 *src, VO_S32 src_stride
                     					)
{
  VO_S32 x, y;
  for( y=0; y<16; y++ )
  {
    for( x=0; x<16; x++ )
      dst[x] = Clip255(((src[x-2] + src[x+3] - 5*(src[x-1] + src[x+2]) + 20*(src[x] + src[x+1])) + 16) >> 5);
	dst += dst_stride;
	src += src_stride;
  }
}
//end of add   

#if defined(VOARMV7)
const MC_COPY_FUNC CopyBlock[2] = 
{
  MCCopy16x16_ARMV7, MCCopy8x8_ARMV7
};

#else
const MC_COPY_FUNC CopyBlock[2] = 
{
  MCCopy16x16_C, MCCopy8x8_C
};
#endif

//end of edit

