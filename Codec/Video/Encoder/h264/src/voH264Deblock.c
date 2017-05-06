/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/
#include <math.h>
#include <stdlib.h>
#include "voH264EncGlobal.h"



//#define ALIGN(x,a) (((x)+((a)-1))&~((a)-1))

extern void DeblockChromaV_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraChromaV_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockChromaH_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraChromaH_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockLumaV_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraLumaV_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockLumaH_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraLumaH_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );

/* Deblocking filter */
static  VO_U8 alpha_table[76] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0a,0x0c,0x0d,0x0f,0x11,0x14,0x16,0x19,0x1c,
  0x20,0x24,0x28,0x2d,0x32,0x38,0x3f,0x47,0x50,0x5a,0x65,
  0x71,0x7f,0x90,0xa2,0xb6,0xcb,0xe2,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
static  VO_U8 beta_table[76] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x03,
  0x03,0x03,0x04,0x04,0x04,0x06,0x06,0x07,0x07,0x08,0x08,
  0x09,0x09,0x0a,0x0a,0x0b,0x0b,0x0c,0x0c,0x0d,0x0d,0x0e,
  0x0e,0x0f,0x0f,0x10,0x10,0x11,0x11,0x12,0x12,0x12,0x12,
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12
};
static  VO_S8 skip_table[76][4] =
{
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x01},{0xff,0x00,0x00,0x01},{0xff,0x00,0x00,0x01},
  {0xff,0x00,0x00,0x01},{0xff,0x00,0x01,0x01},{0xff,0x00,0x01,0x01},{0xff,0x01,0x01,0x01},
  {0xff,0x01,0x01,0x01},{0xff,0x01,0x01,0x01},{0xff,0x01,0x01,0x01},{0xff,0x01,0x01,0x02},
  {0xff,0x01,0x01,0x02},{0xff,0x01,0x01,0x02},{0xff,0x01,0x01,0x02},{0xff,0x01,0x02,0x03},
  {0xff,0x01,0x02,0x03},{0xff,0x02,0x02,0x03},{0xff,0x02,0x02,0x04},{0xff,0x02,0x03,0x04},
  {0xff,0x02,0x03,0x04},{0xff,0x03,0x03,0x05},{0xff,0x03,0x04,0x06},{0xff,0x03,0x04,0x06},
  {0xff,0x04,0x05,0x07},{0xff,0x04,0x05,0x08},{0xff,0x04,0x06,0x09},{0xff,0x05,0x07,0x0a},
  {0xff,0x06,0x08,0x0b},{0xff,0x06,0x08,0x0d},{0xff,0x07,0x0a,0x0e},{0xff,0x08,0x0b,0x10},
  {0xff,0x09,0x0c,0x12},{0xff,0x0a,0x0d,0x14},{0xff,0x0b,0x0f,0x17},{0xff,0x0d,0x11,0x19},
  {0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},
  {0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},
  {0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19}
};

static VOINLINE void DeblockLuma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *skip )
{
  VO_U32 i, j;
  VO_S32 l2,l1,l0,r0,r1,r2;
  VO_S32 delta_luma;
  VO_S32 threshold;
  for( i = 0; i < 4; i++ )
  {
    if( skip[i] < 0 )
    {
      src += 4*stride;
    }
	else
	{
	  for( j = 4; j != 0; j-- )
	  {
	    l2 = src[-3*step];l1 = src[-2*step];l0 = src[-1*step];
	    r0 = src[ 0*step];r1 = src[ 1*step];r2 = src[ 2*step];

	    if( abs( l0 - r0 ) < alpha && abs( l1 - l0 ) < beta && abs( r1 - r0 ) < beta )
	    {
	      threshold = skip[i];	                
	      if( abs( l2 - l0 ) < beta )
	      {
	        src[-2*step] = (VO_U8)(l1 + AVSClip3( (( l2 + ((l0 + r0 + 1) >> 1)) >> 1) - l1, -skip[i], skip[i] ));
	        threshold++;
	      }
	      if( abs( r2 - r0 ) < beta )
	      {
	        src[ 1*step] = (VO_U8)(r1 + AVSClip3( (( r2 + ((l0 + r0 + 1) >> 1)) >> 1) - r1, -skip[i], skip[i] ));
	        threshold++;
	      }
	      delta_luma = AVSClip3( (((r0 - l0 ) << 2) + (l1 - r1) + 4) >> 3, -threshold, threshold );
	      src[-1*step] = Clip255( l0 + delta_luma );    
	      src[ 0*step] = Clip255( r0 - delta_luma );    
	    }
	    src += stride;
	  }
    }
  }
}
static void DeblockLumaV( H264ENC *pEncGlobal, VO_U8 *src, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
  VO_S32 index_alpha = nQP + pEncGlobal->sh.nAlphaC0Offset+12;
  VO_S32 alpha = alpha_table[index_alpha];
  VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];
  VO_S8 skip[4];

  if( alpha && beta )
  {
    VO_S8* st = skip_table[index_alpha];
    skip[0] = st[strength[0]];skip[1] = st[strength[1]];
    skip[2] = st[strength[2]];skip[3] = st[strength[3]];
#if defined(VOARMV7)
	DeblockLumaV_ARMV7( src, src_stride, alpha, beta, skip );
#else
    DeblockLuma( src, src_stride, 1, alpha, beta, skip );
#endif
  }
}
static void DeblockLumaH( H264ENC *pEncGlobal, VO_U8 *src, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
  VO_S32 index_alpha = nQP + pEncGlobal->sh.nAlphaC0Offset+12;
  VO_S32 alpha = alpha_table[index_alpha];
  VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];
  VO_S8 skip[4];

  if( alpha && beta )
  {
    VO_S8* st = skip_table[index_alpha];
    skip[0] = st[strength[0]];skip[1] = st[strength[1]];
    skip[2] = st[strength[2]];skip[3] = st[strength[3]];
#if defined(VOARMV7)
	DeblockLumaH_ARMV7( src, src_stride, alpha, beta, skip );
#else
    DeblockLuma( src, 1, src_stride, alpha, beta, skip );
#endif
  }
}
static VOINLINE void DeblockChroma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *skip )
{
  VO_U32 i, j;
  VO_S32 l1,l0,r0,r1;
  for( i = 0; i < 4; i++ )
  {
    if( skip[i] <= 0 )
    {
      src += 2*stride;
    }
	else
    {
	  for( j = 2; j != 0; j-- )
	  {
	    l1 = src[-2*step];l0 = src[-1*step];
	    r0 = src[ 0*step];r1 = src[ 1*step];

	    if( abs( l0 - r0 ) < alpha && abs( l1 - l0 ) < beta && abs( r1 - r0 ) < beta )
	    {
	      VO_S32 delta = AVSClip3( (((r0 - l0 ) << 2) + (l1 - r1) + 4) >> 3, -skip[i], skip[i] );
	      src[-1*step] = Clip255( l0 + delta );    
	      src[ 0*step] = Clip255( r0 - delta );    
	    }
	    src += stride;
	  }
    }
  }
}
static void DeblockChromaV( H264ENC *pEncGlobal, VO_U8 *src1,VO_U8 *src2, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
    VO_S32 index_alpha = nQP + pEncGlobal->sh.nAlphaC0Offset+12;
	VO_S32 alpha = alpha_table[index_alpha];
	VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];
    VO_S8 skip[4];

    if( alpha && beta )
    {
	    VO_S8* st = skip_table[index_alpha];
    	skip[0] = st[strength[0]]+1;skip[1] = st[strength[1]]+1;
    	skip[2] = st[strength[2]]+1;skip[3] = st[strength[3]]+1;
#if defined(VOARMV7)
		DeblockChromaV_ARMV7( src1,src2, src_stride, alpha, beta, skip );
#else
    	DeblockChroma( src1, src_stride, 1, alpha, beta, skip );
		DeblockChroma( src2, src_stride, 1, alpha, beta, skip );
#endif
    }
}
static void DeblockChromaH( H264ENC *pEncGlobal, VO_U8 *src1,VO_U8 *src2, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
  VO_S32 index_alpha = nQP + pEncGlobal->sh.nAlphaC0Offset+12;
  VO_S32 alpha = alpha_table[index_alpha];
  VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];
  VO_S8 skip[4];

  if( alpha && beta )
  {
   	VO_S8* st = skip_table[index_alpha];
   	skip[0] = st[strength[0]]+1;skip[1] = st[strength[1]]+1;
   	skip[2] = st[strength[2]]+1;skip[3] = st[strength[3]]+1;
#if defined(VOARMV7)
	DeblockChromaH_ARMV7( src1,src2, src_stride, alpha, beta, skip );
#else
    DeblockChroma( src1, 1, src_stride, alpha, beta, skip );
	DeblockChroma( src2, 1, src_stride, alpha, beta, skip );
#endif
  }
}

static VOINLINE void DeblocIntraLuma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta )
{
  VO_U32 i;
  VO_S32 l3,l2,l1,l0,r0,r1,r2,r3;
  for( i = 16; i != 0; i-- )
  {
	l2 = src[-3*step];l1 = src[-2*step];l0 = src[-1*step];
	r0 = src[ 0*step];r1 = src[ 1*step];r2 = src[ 2*step];

    if( abs( l0 - r0 ) < alpha && abs( l1 - l0 ) < beta && abs( r1 - r0 ) < beta )
    {
      if(abs( l0 - r0 ) < ((alpha >> 2) + 2) )
      {
        if( abs( l2 - l0 ) < beta ) 
        {
          l3 = src[-4*step];
          src[-1*step] = (VO_U8)(( l2 + 2*l1 + 2*l0 + 2*r0 + r1 + 4 ) >> 3);
          src[-2*step] = (VO_U8)(( l2 + l1 + l0 + r0 + 2 ) >> 2);
          src[-3*step] = (VO_U8)(( 2*l3 + 3*l2 + l1 + l0 + r0 + 4 ) >> 3);
        }
        else 
          src[-1*step] = (VO_U8)(( 2*l1 + l0 + r1 + 2 ) >> 2);
        if( abs( r2 - r0 ) < beta ) 
        {
          r3 = src[3*step];
          src[0*step] = (VO_U8)(( l1 + 2*l0 + 2*r0 + 2*r1 + r2 + 4 ) >> 3);
          src[1*step] = (VO_U8)(( l0 + r0 + r1 + r2 + 2 ) >> 2);
          src[2*step] = (VO_U8)(( 2*r3 + 3*r2 + r1 + r0 + l0 + 4 ) >> 3);
        }
        else /* q0' */
          src[0*step] = (VO_U8)(( 2*r1 + r0 + l1 + 2 ) >> 2);
      }
      else /* p0', q0' */
      {
        src[-1*step] = (VO_U8)(( 2*l1 + l0 + r1 + 2 ) >> 2);
        src[ 0*step] = (VO_U8)(( 2*r1 + r0 + l1 + 2 ) >> 2);
      }
    }
    src += stride;
  }
}
static void DeblocIntraLumaV( H264ENC *pEncGlobal, VO_U8 *src, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pEncGlobal->sh.nAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];
  if( alpha && beta)
  {
#if defined(VOARMV7)
	DeblockIntraLumaV_ARMV7( src, src_stride, alpha, beta);
#else
    DeblocIntraLuma( src, src_stride,1 , alpha, beta );
#endif
  }
}
static void DeblocIntraLumaH( H264ENC *pEncGlobal, VO_U8 *src, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pEncGlobal->sh.nAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];
  if( alpha && beta )
  {
#if defined(VOARMV7)
	DeblockIntraLumaH_ARMV7( src, src_stride, alpha, beta);
#else
   	DeblocIntraLuma( src, 1, src_stride, alpha, beta );
#endif
  }
}
static VOINLINE void DeblocIntraChroma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta )
{
  VO_U32 i;
  VO_S32 l1,l0,r0,r1;
  for( i = 8; i != 0; i-- )
  {
    l1 = src[-2*step];l0 = src[-1*step];
    r0 = src[ 0*step];r1 = src[ 1*step];

    if( abs( l0 - r0 ) < alpha && abs( l1 - l0 ) < beta && abs( r1 - r0 ) < beta )
    {
      src[-1*step] = (VO_U8)((2*l1 + l0 + r1 + 2) >> 2);   
      src[ 0*step] = (VO_U8)((2*r1 + r0 + l1 + 2) >> 2);   
    }
    src += stride;
  }
}
static void DeblocIntraChromaV( H264ENC *pEncGlobal, VO_U8 *src1, VO_U8 *src2, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pEncGlobal->sh.nAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];
  if( alpha && beta )
  {
#if defined(VOARMV7)
	DeblockIntraChromaV_ARMV7( src1,src2, src_stride, alpha, beta);
#else
    DeblocIntraChroma( src1, src_stride,1 , alpha, beta );
	DeblocIntraChroma( src2, src_stride,1 , alpha, beta );
#endif
  }
}
static void DeblocIntraChromaH( H264ENC *pEncGlobal, VO_U8 *src1, VO_U8 *src2, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pEncGlobal->sh.nAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pEncGlobal->sh.nBetaOffset+12];

  if( alpha && beta )
  {
#if defined(VOARMV7)
	DeblockIntraChromaH_ARMV7( src1,src2, src_stride, alpha, beta);
#else
    DeblocIntraChroma( src1, 1 ,src_stride, alpha, beta );
	DeblocIntraChroma( src2, 1 ,src_stride, alpha, beta );
#endif
  }
}
void GetStrengthHor(VO_U8* strength,VO_S32 edge,VO_S32 mb_type,VO_S32 mb_4x4,
					   VO_S32 mbn_4x4,VO_S32 stride4,VO_U8 *nzc,VO_U8 *nzc_dir,H264ENC *pEncGlobal)
{
  VO_S32 i;
  if( IS_INTRA( mb_type )&&edge)
  {
    M32( strength ) = 0x03030303;
  }
  else
  {
    M32( strength ) = 0x00000000;
    for( i = 0; i < 4; i++ )
    {
      VO_S32 left_x = (edge - 1)&0x03;
      if( nzc[edge+i*4] != 0 ||nzc_dir[left_x+i*4] != 0 )
        strength[i] = 2;
      else if(!(edge&1))
      {
        if((i&1) && strength[i-1] != 2)
          strength[i] = strength[i-1];
        else
        {
		  VO_S32 i4p= mb_4x4+edge+i*stride4;
          VO_S32 i4q= mbn_4x4+left_x+i*stride4;
          if(abs( pEncGlobal->mv[i4p][0] - pEncGlobal->mv[i4q][0] ) >= 4 ||
             abs( pEncGlobal->mv[i4p][1] - pEncGlobal->mv[i4q][1] ) >= 4 )
          {
            strength[i] = 1;
          }
        }
      }
    }
  }
}
void GetStrengthVer(VO_U8* strength,VO_S32 edge,VO_S32 mb_type,VO_S32 mb_4x4,
					   VO_S32 mbn_4x4,VO_S32 stride4,VO_U8 *nzc,VO_U8 *nzc_dir,H264ENC *pEncGlobal)
{
  VO_S32 i;
  if( IS_INTRA( mb_type )&&edge)
    M32( strength ) = 0x03030303;
  else
  {
    M32( strength ) = 0x00000000;
    for( i = 0; i < 4; i++ )
    {
      VO_S32 up_y = (edge - 1)&0x03;
      if( nzc[i+edge*4] != 0 ||nzc_dir[i+up_y*4] != 0 )
      {
        strength[i] = 2;
      }
      else if(!(edge&1))
      {
        if((i&1) && strength[i-1] != 2)
                    strength[i] = strength[i-1];
        else
        {
		  VO_S32 i4p= mb_4x4+i+edge*stride4;
          VO_S32 i4q= mbn_4x4+i+up_y*stride4;
          if(abs( pEncGlobal->mv[i4p][0] - pEncGlobal->mv[i4q][0] ) >= 4 ||
             abs( pEncGlobal->mv[i4p][1] - pEncGlobal->mv[i4q][1] ) >= 4 )
          {
            strength[i] = 1;
          }
        }
      }
    }
  }
}
#define LUMA_START_POS 12
#define CB_START_POS 9
#define CR_START_POS 33
void DeblocMB( H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
  const VO_S32 mb_type = MBTypeFix[pEncLocal->nMBType];
  const VO_S32 stride4 = 4 * pEncGlobal->mb_stride;

  const VO_S32 qp_limit = 15 - AVC_MIN(pEncGlobal->sh.nAlphaC0Offset, pEncGlobal->sh.nBetaOffset);
	
  VO_S32 luma_stride   = pEncGlobal->i_stride[0];
  VO_S32 chroma_stride  = pEncGlobal->i_stride[1];
  VO_S32 nMBX  = pEncLocal->nMBX;
  VO_S32 nMBY	 = pEncLocal->nMBY;
  VO_S32 nMBXY = pEncLocal->nMBXY;
  const VO_S32 sub_block = 4 * stride4 * nMBY + 4 * nMBX; //for mv
  const VO_S32 nQP = pEncGlobal->qp[nMBXY];

  VO_S32 edge_end = (mb_type == PSKIP) ? 1 : 4;
  VO_U8 *pixy = pEncGlobal->fdec->plane[0] + 16*nMBY*luma_stride  + 16*nMBX;
  VO_U8 *pixu = pEncGlobal->fdec->plane[1] +  8*nMBY*chroma_stride +  8*nMBX;
  VO_U8 *pixv = pEncGlobal->fdec->plane[2] +  8*nMBY*chroma_stride +  8*nMBX;
  VO_S32 edge,qp_lt;
  VO_ALIGNED_4( VO_U8 strength[4] );
  VO_U8 nzc[16+4+4];
  CP32( &nzc[0*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+0*8] );
  CP32( &nzc[1*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+1*8] );
  CP32( &nzc[2*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+2*8] );
  CP32( &nzc[3*4], &pEncGlobal->mb.exmb_nzc[LUMA_START_POS+3*8] );
  M16( &nzc[16+0*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CB_START_POS-1] ) >> 8);
  M16( &nzc[16+1*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CB_START_POS+8-1] ) >> 8);
  M16( &nzc[16+2*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CR_START_POS-1] ) >> 8);
  M16( &nzc[16+3*2] ) = (VO_U16)(M32( &pEncGlobal->mb.exmb_nzc[CR_START_POS+8-1] ) >> 8);
		
  if( nQP <= qp_limit )
    edge_end = 1;

  //vertical edge
  edge = (nMBX == 0);
  if( !edge )
  {
	qp_lt= pEncGlobal->qp[nMBXY-1];
    if( IS_INTRA( mb_type ) || IS_INTRA( pEncGlobal->nMBType[nMBX-1]) )
    {
      DeblocIntraLumaH( pEncGlobal, pixy + 4*edge,luma_stride,(nQP+qp_lt+1) >> 1);
      if( !(edge & 1) )
      {
        VO_S32 qpc = (pEncGlobal->pChromaQP[nQP] + pEncGlobal->pChromaQP[qp_lt] + 1) >> 1;
        DeblocIntraChromaH( pEncGlobal, pixu + 2*edge, pixv + 2*edge, chroma_stride,qpc);
      }
    }
    else
    {
      GetStrengthHor(strength,edge,mb_type,sub_block,sub_block-4,stride4,nzc,
       				pEncGlobal->mzc_mbrow[nMBX-1],pEncGlobal);
      if( M32( strength ) )
      {
        DeblockLumaH ( pEncGlobal, pixy + 4*edge,luma_stride, strength, (nQP+qp_lt+1) >> 1);
	    if( !(edge & 1) )
	    {
	      VO_S32 qpc = (pEncGlobal->pChromaQP[nQP] + pEncGlobal->pChromaQP[qp_lt] + 1) >> 1;
	      DeblockChromaH( pEncGlobal, pixu + 2*edge, pixv + 2*edge,chroma_stride, strength, qpc);
	    }
      }
    }
    edge++;
  }
  for( ; edge < edge_end; edge++ )
  {
    GetStrengthHor(strength,edge,mb_type,sub_block,sub_block,stride4,nzc,nzc,pEncGlobal);
    if( M32( strength ) )
    {
        	DeblockLumaH ( pEncGlobal, pixy + 4*edge,luma_stride, strength, nQP);
	        if( !(edge & 1) )
	        {
	            VO_S32 qpc = pEncGlobal->pChromaQP[nQP];
	            DeblockChromaH( pEncGlobal, pixu + 2*edge, pixv + 2*edge,chroma_stride, strength, qpc);
	        }
    }
  }
    //horizontal edge 
    edge = (nMBY <= 0);
    if( !edge )
    {
		qp_lt= pEncGlobal->qp[nMBXY-pEncGlobal->mb_stride];
        if( IS_INTRA( mb_type ) || IS_INTRA( pEncGlobal->nMBType[nMBX]) )
        {
            DeblocIntraLumaV( pEncGlobal, pixy + 4*edge*luma_stride,luma_stride,(nQP+qp_lt+1) >> 1);
            if( !(edge & 1) )
            {
                VO_S32 qpc = (pEncGlobal->pChromaQP[nQP] + pEncGlobal->pChromaQP[qp_lt] + 1) >> 1;
                DeblocIntraChromaV ( pEncGlobal, pixu + 2*edge*chroma_stride, pixv + 2*edge*chroma_stride,
                              chroma_stride,qpc);
            }
        }
		else
		{
           	GetStrengthVer(strength,edge,mb_type,sub_block,sub_block-4*stride4,stride4,nzc,
            				pEncGlobal->mzc_mbrow[nMBX],pEncGlobal);
           	if( M32( strength ) )
            {
            	DeblockLumaV( pEncGlobal, pixy + 4*edge*luma_stride,luma_stride, strength, (nQP+qp_lt+1) >> 1);
	            if( !(edge & 1) )
	            {
	                VO_S32 qpc = (pEncGlobal->pChromaQP[nQP] + pEncGlobal->pChromaQP[qp_lt] + 1) >> 1;
	                DeblockChromaV( pEncGlobal, pixu + 2*edge*chroma_stride, pixv + 2*edge*chroma_stride,
	                              chroma_stride, strength, qpc);
	            }
	        }
		}
        edge++;
    }
    for( ; edge < edge_end; edge++ )
    {
        GetStrengthVer(strength,edge,mb_type,sub_block,sub_block,stride4,nzc,nzc,pEncGlobal);
        if( M32( strength ) )
        {
            DeblockLumaV( pEncGlobal, pixy + 4*edge*luma_stride,luma_stride, strength, nQP);
	        if( !(edge & 1) )
	        {
	            VO_S32 qpc = pEncGlobal->pChromaQP[nQP];
	            DeblockChromaV( pEncGlobal, pixu + 2*edge*chroma_stride, pixv + 2*edge*chroma_stride,
	                             chroma_stride, strength, qpc);
	        }
	    }
    }
}
#undef LUMA_START_POS
#undef CB_START_POS
#undef CR_START_POS
//end of add

