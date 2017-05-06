/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#include "voH264EncGlobal.h"

void Dct4x4DC_C( VO_S16* d )
{
  VO_S16 tmp[16];
  VO_S32 i,d0,d1,d2,d3,p0,p2;
  VO_S16 *p_tmp = tmp;
  VO_S16 *pd = d;

  for( i = 0; i < 4; i++ )
  {
	d0 = *(pd++);
	d1 = *(pd++);
	d2 = *(pd++);
	d3 = *(pd++);

	p0 = d0 + d1;
	p2 = d0 - d1;
	d0 = d2 + d3;
	d1 = d2 - d3;		

    tmp[0*4+i] = (VO_S16)(p0 + d0);
    tmp[1*4+i] = (VO_S16)(p0 - d0);
    tmp[2*4+i] = (VO_S16)(p2 - d1);
    tmp[3*4+i] = (VO_S16)(p2 + d1);
  }
    
  for( i = 0; i < 4; i++ )
  {
	d0 = *(p_tmp++);
	d1 = *(p_tmp++);
	d2 = *(p_tmp++);
	d3 = *(p_tmp++);

	p0 = d0 + d1;
	p2 = d0 - d1;
	d0 = d2 + d3;
	d1 = d2 - d3;

    *(d++) = (VO_S16)(( p0 + d0 + 1 ) >> 1);
    *(d++) = (VO_S16)(( p0 - d0 + 1 ) >> 1);
    *(d++) = (VO_S16)(( p2 - d1 + 1 ) >> 1);
    *(d++) = (VO_S16)(( p2 + d1 + 1 ) >> 1);
  }
}

void Idct4x4Dc_C( VO_S16* d )
{
  VO_S16 tmp[16];
  VO_S32 i,d0,d1,d2,d3,p0,p2;
  VO_S16 *p_tmp = tmp;
  VO_S16 *pd = d;

  for( i = 0; i < 4; i++ )
  {
	d0 = *(pd++);
	d1 = *(pd++);
	d2 = *(pd++);
	d3 = *(pd++);

	p0 = d0 + d1;
	p2 = d0 - d1;
	d0 = d2 + d3;
	d1 = d2 - d3;

	tmp[0*4+i] = (VO_S16)(p0 + d0);
    tmp[1*4+i] = (VO_S16)(p0 - d0);
    tmp[2*4+i] = (VO_S16)(p2 - d1);
    tmp[3*4+i] = (VO_S16)(p2 + d1);
  }

  for( i = 0; i < 4; i++ )
  {
	d0 = *(p_tmp++);
	d1 = *(p_tmp++);
	d2 = *(p_tmp++);
	d3 = *(p_tmp++);

	p0 = d0 + d1;
	p2 = d0 - d1;
	d0 = d2 + d3;
	d1 = d2 - d3;

    *(d++) = (VO_S16)(p0 + d0);
    *(d++) = (VO_S16)(p0 - d0);
    *(d++) = (VO_S16)(p2 - d1);
    *(d++) = (VO_S16)(p2 + d1);
  }
}


void Sub4x4Dct_C( VO_S16 *dct, VO_U8 *src, VO_U8 *dst , VO_S32 dst_stride)
{
  VO_S16 tmp[16];
  VO_S32 i,d0,d1,d2,d3,p0,p2;
  VO_S16 *p_tmp = tmp;

  for( i = 0; i < 4; i++ )
  {
	d0 = src[0] - dst[0];
	d1 = src[1] - dst[1];
	d2 = src[2] - dst[2];
	d3 = src[3] - dst[3];

	p0 = d0 + d3;
	p2 = d0 - d3;
	d0 = d1 + d2;
	d3 = d1 - d2;

    tmp[0*4+i] =(VO_S16) (    p0 +      d0);
    tmp[1*4+i] =(VO_S16) ((p2<<1) +     d3);
    tmp[2*4+i] =(VO_S16) (    p0 -      d0);
    tmp[3*4+i] =(VO_S16) (    p2 - (d3<<1));
	src += 16;
	dst += dst_stride;
  }

  for( i = 0; i < 4; i++ )
  {
	d0 = *(p_tmp++);
	d1 = *(p_tmp++);
	d2 = *(p_tmp++);
	d3 = *(p_tmp++);

	p0 = d0 + d3;
	p2 = d0 - d3;
	d0 = d1 + d2;
	d3 = d1 - d2;

	*(dct++) =(VO_S16) (    p0 +      d0);
	*(dct++) =(VO_S16) ((p2<<1) +     d3);
	*(dct++) =(VO_S16) (    p0 -      d0);
	*(dct++) =(VO_S16) (    p2 - (d3<<1));
  }
}

//YU_TBD
void Sub8x8Dct_C( VO_S16 *dct, VO_U8 *src, VO_U8 *dst , VO_S32 dst_stride)
{
  Sub4x4Dct( &dct[0],    &src[0], &dst[0] ,dst_stride);
  Sub4x4Dct( &dct[1*16], &src[4], &dst[4] ,dst_stride);
  Sub4x4Dct( &dct[2*16], &src[4*16+0], &dst[4*dst_stride+0] ,dst_stride);
  Sub4x4Dct( &dct[3*16], &src[4*16+4], &dst[4*dst_stride+4] ,dst_stride);
}

//YU_TBD
void Sub16x16Dct_C( VO_S16 *dct, VO_U8 *src, VO_U8 *dst, VO_S32 dst_stride)
{
  Sub8x8Dct_C( &dct[ 0], &src[0], &dst[0] ,dst_stride);
  Sub8x8Dct_C( &dct[ 4*16], &src[8], &dst[8] ,dst_stride);
  Sub8x8Dct_C( &dct[ 8*16], &src[8*16+0], &dst[8*dst_stride+0] ,dst_stride);
  Sub8x8Dct_C( &dct[12*16], &src[8*16+8], &dst[8*dst_stride+8] ,dst_stride);
}

VO_S32 Sub4x4DctDc( VO_U8 *src, VO_U8 *dst )
{
  VO_S32 sum = 0;
  VO_U32 i;

  for (i = 4; i != 0; i--)
  {
	sum += src[0] - dst[0];
	sum += src[1] - dst[1];
	sum += src[2] - dst[2];
	sum += src[3] - dst[3];
	src += 16;
	dst += 32;
  }

  return sum;
}


void Sub8x8DctDc_C( VO_S16 dct[4], VO_U8 *src, VO_U8 *dst)
{
  dct[0] = (VO_S16)Sub4x4DctDc( &src[0], &dst[0] );
  dct[1] = (VO_S16)Sub4x4DctDc( &src[4], &dst[4] );
  dct[2] = (VO_S16)Sub4x4DctDc( &src[4*16+0], &dst[4*32+0] );
  dct[3] = (VO_S16)Sub4x4DctDc( &src[4*16+4], &dst[4*32+4] );
}

void Add4x4Idct_C( VO_U8 *p_dst, VO_S16* dct )
{
  VO_S16 tmp[16];
  VO_S32 i;
  VO_S16 *p_tmp = tmp;
  VO_S32 p0,p2,d0,d1,d2,d3;
  VO_U8* mpr = p_dst;

  for( i = 0; i < 4; i++ )
  {
	d0 = dct[i];
	d1 = dct[1*4+i];
	d2 = dct[2*4+i];
	d3 = dct[3*4+i];

	p0 =  d0 + d2;
	d0 =  d0 - d2;
	p2 = (d1 >> 1) - d3;
	d1 =  d1 + (d3 >> 1);

	*(p_tmp++) = (VO_S16)(p0 + d1);
	*(p_tmp++) = (VO_S16)(d0 + p2);
	*(p_tmp++) = (VO_S16)(d0 - p2);
	*(p_tmp++) = (VO_S16)(p0 - d1);
  }

  for( i = 0; i < 4; i++ )
  {
	p_tmp = tmp + i;
	d0 = *p_tmp+32;
	d2 = *(p_tmp += 4);
	d1 = *(p_tmp += 4);
	d3 = *(p_tmp += 4);

	p0 = d0 + d1;
	d0 = d0 - d1;
	p2 =(d2 >> 1) - d3;
	d2 = d2 + (d3 >> 1);

	d1 = ((p0 + d2 ) >> 6) + *mpr;//0--1
	d3 = ((d0 + p2 ) >> 6) + *(mpr+32);
	d0 = ((d0 - p2 ) >> 6) + *(mpr+2*32);
	d2 = ((p0 - d2 ) >> 6) + *(mpr+3*32);

    *p_dst = Clip255(d1);
	*(p_dst + 32) = Clip255(d3);
	*(p_dst + 2*32) = Clip255(d0);
	*(p_dst + 3*32) = Clip255(d2);

	p_dst++;
	mpr = p_dst;
  }
}


void Add8x8Idct_C( VO_U8 *p_dst, VO_S16 *dct )
{
  Add4x4Idct_C( &p_dst[0],      &dct[0] );
  Add4x4Idct_C( &p_dst[4],      &dct[1*16] );
  Add4x4Idct_C( &p_dst[4*32+0], &dct[2*16] );
  Add4x4Idct_C( &p_dst[4*32+4], &dct[3*16] );
}

void Add16x16Idct_C( VO_U8 *p_dst, VO_S16 *dct )
{
  Add8x8Idct_C( &p_dst[0],      &dct[0] );
  Add8x8Idct_C( &p_dst[8],      &dct[4*16] );
  Add8x8Idct_C( &p_dst[8*32+0], &dct[8*16] );
  Add8x8Idct_C( &p_dst[8*32+8], &dct[12*16] );
}


static void VOINLINE Add4x4IdctDc( VO_U8 *p_dst, VO_S16 dc )
{
  VO_U32 i;
  dc = (dc + 32) >> 6;
  for( i = 4; i != 0; i-- )
  {
    p_dst[0] = Clip255( p_dst[0] + dc );
    p_dst[1] = Clip255( p_dst[1] + dc );
    p_dst[2] = Clip255( p_dst[2] + dc );
    p_dst[3] = Clip255( p_dst[3] + dc );
	p_dst += 32;
  }
}

void Add8x8IdctDc_C( VO_U8 *p_dst, VO_S16 dct[4] )
{
  Add4x4IdctDc( &p_dst[0], dct[0] );
  Add4x4IdctDc( &p_dst[4], dct[1] );
  Add4x4IdctDc( &p_dst[4*32+0], dct[2] );
  Add4x4IdctDc( &p_dst[4*32+4], dct[3] );
}

void Add16x16IdctDc_C( VO_U8 *p_dst, VO_S16 dct[16] )
{
  VO_S32 i;
  for( i = 0; i < 4; i++, dct += 4, p_dst += 4*32 )
  {
    Add4x4IdctDc( &p_dst[ 0], dct[0] );
    Add4x4IdctDc( &p_dst[ 4], dct[1] );
    Add4x4IdctDc( &p_dst[ 8], dct[2] );
    Add4x4IdctDc( &p_dst[12], dct[3] );
  }
}

void Zigzag4x4_C( VO_S16 level[16], VO_S16 dct[16] )
{
  level[0] = dct[0];level[1] = dct[4];
  level[2] = dct[1];level[3] = dct[2];
  level[4] = dct[5];level[5] = dct[8];
  level[6] = dct[12];level[7] = dct[9];
  level[8] = dct[6];level[9] = dct[3];
  level[10] = dct[7];level[11] = dct[10];
  level[12] = dct[13];level[13] = dct[14];
  level[14] = dct[11];level[15] = dct[15];
}

void Zigzag2x2Dc( VO_S16 level[4], VO_S16 dct[4] )
{
  level[0] = dct[0];level[1] = dct[2];
  level[2] = dct[1];level[3] = dct[3];
}

#define START_DC_IDCT \
  VO_S32 d0,d1,d2,d3,q0,q1,q2,q3; \
  VO_S32 dq_mf = dequant_mf[nQP%6<<4]; \
  VO_S32 q_bits = nQP/6 - 5; \
  if( q_bits > 0 ) \
  { \
    dq_mf <<= q_bits; \
    q_bits = 0; \
  } \
  d0 = dct_DC[0]; \
  d1 = dct_DC[1]; \
  d2 = dct_DC[2]; \
  d3 = dct_DC[3]; \
  q0 = d0 + d1 + d2 + d3; \
  q1 = d0 + d1 - d2 - d3; \
  q2 = d0 - d1 + d2 - d3; \
  q3 = d0 - d1 - d2 + d3; 

void IdctDQ2x2Dc( VO_S16 dct_DC[4], VO_S16 *dct16, VO_S32 *dequant_mf, VO_S32 nQP )
{
  START_DC_IDCT
  dct16[0  ] = (VO_S16)(q0 * dq_mf >> -q_bits);
  dct16[1*16] = (VO_S16)(q1 * dq_mf >> -q_bits);
  dct16[2*16] = (VO_S16)(q2 * dq_mf >> -q_bits);
  dct16[3*16] = (VO_S16)(q3 * dq_mf >> -q_bits);
}


void IdctDQ2x2Dc4( VO_S16 out[4], VO_S16 dct_DC[4], VO_S32 *dequant_mf, VO_S32 nQP )
{
  START_DC_IDCT
  out[0] = (VO_S16)(q0 * dq_mf >> -q_bits);
  out[1] = (VO_S16)(q1 * dq_mf >> -q_bits);
  out[2] = (VO_S16)(q2 * dq_mf >> -q_bits);
  out[3] = (VO_S16)(q3 * dq_mf >> -q_bits);
}

void Dct2x2Dc ( VO_S16 d[4], VO_S16 *dct16 )
{
  VO_S32 d0,d1,d2,d3,q0,q1;
  d0 = dct16[0];
  d1 = dct16[1*16];
  d2 = dct16[2*16];
  d3 = dct16[3*16];

  q0 = d0 + d1;
  d0 = d0 - d1;
  q1 = d2 + d3;
  d1 = d2 - d3;
	
  d[0] = (VO_S16)(q0 + q1);
  d[2] = (VO_S16)(d0 + d1);
  d[1] = (VO_S16)(q0 - q1);
  d[3] = (VO_S16)(d0 - d1);
	
  dct16[0] = 0;
  dct16[1*16] = 0;
  dct16[2*16] = 0;
  dct16[3*16] = 0;
}

void Dct2x2DcOnly( VO_S16 d[4] )
{
  VO_S32 d0,d1,d2,d3,q0,q1;
  d0 = d[0];
  d1 = d[1];
  d2 = d[2];
  d3 = d[3];

  q0 = d0 + d1;
  d0 = d0 - d1;
  q1 = d2 + d3;
  d1 = d2 - d3;
	
  d[0] = (VO_S16)(q0 + q1);
  d[2] = (VO_S16)(d0 + d1);
  d[1] = (VO_S16)(q0 - q1);
  d[3] = (VO_S16)(d0 - d1);
}

