/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#include "voH264EncGlobal.h"

VO_S32 SSD8x8_C( VO_U8 *src1,VO_U8 *src2,VO_S32 stride1, VO_S32 stride2)
{
  VO_S32 sum = 0;                                  
  VO_U32 x, y; 
  VO_S32 d;
  for( y = 8; y != 0; y-- )                       
  {                                               
    for( x = 0; x < 8; x++ )                   
    {                                           
      d = src1[x] - src2[x];              
      sum += d*d;
    }                                           
    src1 += stride1;                      
    src2 += stride2;                      
  }                                               
  return sum; 
}

#define VOGETABS(a)      ((a)-(((a)<<1)&((a)>>31)))
//#define VOGETABS(a)      abs((a))

#define VOSADLINE8(sad)  \
     sad += VOGETABS(*(ptr_cur_mb+0) - *(ptr_ref_mb+0)) \
            + VOGETABS(*(ptr_cur_mb+1) - *(ptr_ref_mb+1))\
            + VOGETABS(*(ptr_cur_mb+2) - *(ptr_ref_mb+2)) \
            + VOGETABS(*(ptr_cur_mb+3) - *(ptr_ref_mb+3))\
            + VOGETABS(*(ptr_cur_mb+4) - *(ptr_ref_mb+4)) \
            + VOGETABS(*(ptr_cur_mb+5) - *(ptr_ref_mb+5))\
            + VOGETABS(*(ptr_cur_mb+6) - *(ptr_ref_mb+6)) \
            + VOGETABS(*(ptr_cur_mb+7) - *(ptr_ref_mb+7));\
    ptr_cur_mb += cur_stride;\
    ptr_ref_mb += ref_stride;

#define VOSADLINE16(sad)  \
     sad += VOGETABS(*(ptr_cur_mb+0) - *(ptr_ref_mb+0)) \
            + VOGETABS(*(ptr_cur_mb+1) - *(ptr_ref_mb+1))\
            + VOGETABS(*(ptr_cur_mb+2) - *(ptr_ref_mb+2)) \
            + VOGETABS(*(ptr_cur_mb+3) - *(ptr_ref_mb+3))\
            + VOGETABS(*(ptr_cur_mb+4) - *(ptr_ref_mb+4)) \
            + VOGETABS(*(ptr_cur_mb+5) - *(ptr_ref_mb+5))\
            + VOGETABS(*(ptr_cur_mb+6) - *(ptr_ref_mb+6)) \
            + VOGETABS(*(ptr_cur_mb+7) - *(ptr_ref_mb+7))\
            + VOGETABS(*(ptr_cur_mb+8) - *(ptr_ref_mb+8))\
            + VOGETABS(*(ptr_cur_mb+9) - *(ptr_ref_mb+9))\
            + VOGETABS(*(ptr_cur_mb+10) - *(ptr_ref_mb+10))\
            + VOGETABS(*(ptr_cur_mb+11) - *(ptr_ref_mb+11))\
            + VOGETABS(*(ptr_cur_mb+12) - *(ptr_ref_mb+12))\
            + VOGETABS(*(ptr_cur_mb+13) - *(ptr_ref_mb+13))\
            + VOGETABS(*(ptr_cur_mb+14) - *(ptr_ref_mb+14))\
            + VOGETABS(*(ptr_cur_mb+15) - *(ptr_ref_mb+15));\
     ptr_cur_mb += cur_stride;\
     ptr_ref_mb += ref_stride;

VO_S32 Sad16x16_C( VO_U8 *ptr_cur_mb,VO_U8 * ptr_ref_mb,VO_U32 cur_stride,
                   VO_U32 ref_stride,VO_U32 best_sad)
{
  VO_U32 sad = 0;
  VO_U32 i;

  for (i = 16; i != 0; i--) 
  {
    VOSADLINE16(sad);
    if(sad >= best_sad)
      return sad;
  }
  return sad;
}

VO_S32 Sad8x8_C(VO_U8 *ptr_cur_mb,VO_U8 *ptr_ref_mb,VO_U32 cur_stride,
                   VO_U32 ref_stride,VO_U32 best_sad)
{
  VO_U32 sad = 0;
  VO_U32 i;
  for (i = 4; i != 0; i--) 
  {
    VOSADLINE8(sad);
	VOSADLINE8(sad);
	if(sad >= best_sad)
      return sad;
  }
  return sad;
}
/****************************************************************************
 * pixel_var2_wxh
 ****************************************************************************/
VO_S32 Var28x8_C( VO_U8 *pix1, VO_U8 *pix2, VO_S32 i_stride1, VO_S32 i_stride2, VO_S32 *ssd )
{
  VO_U32 var = 0,  sqr = 0;
  VO_S32 x, y,sum = 0;
  for( y = 0; y < 8; y++ )
  {
    for( x = 0; x < 8; x++ )
    {
      VO_S32 diff = pix1[x] - pix2[x];
      sum += diff;
      sqr += diff * diff;
    }
    pix1 += i_stride1;
    pix2 += i_stride2;
  }
  sum = VOGETABS(sum);
  var = sqr - (sum * sum >> 6);
  *ssd = sqr;
  return var;
}


SadFunc SadBlock[2] = {
	sad_16x16, sad_8x8
};
