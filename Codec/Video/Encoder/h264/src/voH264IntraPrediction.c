/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/


#include "voH264EncGlobal.h"
/****************************************************************************
 * 16x16 prediction for intra luma block
 ****************************************************************************/


#define PREDICT_16x16_DC(v) \
    for( i = 16; i != 0; i-- )\
    {\
        M32( dst+ 0 ) = v;\
        M32( dst+ 4 ) = v;\
        M32( dst+ 8 ) = v;\
        M32( dst+12 ) = v;\
        dst += dst_stride;\
    }
#if !defined(VOARMV7)
static void PredIntraLuma16x16Dc_C( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 sum,i;
	VO_U8 *src_up = src - src_stride;
	VO_U8 *src_left = src_up - 1;

    for( i = 4,sum = 0; i != 0; i-- )
    {
		sum += *(src_left+=src_stride);sum += *(src_up++);
		sum += *(src_left+=src_stride);sum += *(src_up++);
		sum += *(src_left+=src_stride);sum += *(src_up++);
		sum += *(src_left+=src_stride);sum += *(src_up++);
    }
    sum = (( sum + 16 ) >> 5) * 0x01010101;

    PREDICT_16x16_DC(sum);
}
#endif
static void PredIntraLuma16x16DcLeft( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 sum,i;
	VO_U8 *src_left = src - src_stride - 1;

    for( i = 4,sum = 0; i != 0; i-- )
    {
        sum += *(src_left+=src_stride);
		sum += *(src_left+=src_stride);
		sum += *(src_left+=src_stride);
		sum += *(src_left+=src_stride);
    }
    sum = (( sum + 8 ) >> 4) * 0x01010101;

    PREDICT_16x16_DC(sum);
}
static void PredIntraLuma16x16DcTop( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 sum,i;
	VO_U8 *src_up = src - src_stride;

    for( i = 4,sum = 0; i != 0; i-- )
    {
        sum += *(src_up++);
		sum += *(src_up++);
		sum += *(src_up++);
		sum += *(src_up++);
    }
    sum = (( sum + 8 ) >> 4) * 0x01010101;

    PREDICT_16x16_DC(sum);
}
static void PredIntraLuma16x16Dc128( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 i;
    PREDICT_16x16_DC(0x80808080);
}
#if !defined(VOARMV7)
static void PredIntraLuma16x16H_C( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 i;
	VO_U8 *src_left = src - 1;

    for( i = 16; i != 0; i-- )
    {
        VO_U32 h = 0x01010101 * src_left[0];
        M32( dst+ 0 ) = h;
        M32( dst+ 4 ) = h;
        M32( dst+ 8 ) = h;
        M32( dst+12 ) = h;
        src_left += src_stride;
		dst += dst_stride;
    }
}
static void PredIntraLuma16x16V_C( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 v0 = M32( &src[ 0-src_stride] );
    VO_U32 v4 = M32( &src[ 4-src_stride] );
    VO_U32 v8 = M32( &src[ 8-src_stride] );
    VO_U32 v12 = M32( &src[12-src_stride] );
    VO_U32 i;

    for( i = 16; i != 0; i-- )
    {
        M32( dst+ 0 ) = v0;
        M32( dst+ 4 ) = v4;
        M32( dst+ 8 ) = v8;
        M32( dst+12 ) = v12;
		dst += dst_stride;
    }
}

static void PredIntraLuma16x16P_C( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 x, y, i;
    VO_S32 a, b, c;
    VO_S32 sum_h = 0;
    VO_S32 sum_v = 0;
    VO_S32 i00;
	VO_S32 pix_line;
	VO_U8 *src_up = src - src_stride;
	VO_U8 *src_left = src - 1;

    /* calculate H and V */
    for( i = 0; i <= 7; i++ )
    {
        sum_h += ( i + 1 ) * ( src_up[ 8 + i] - src_up[6 -i] );
        sum_v += ( i + 1 ) * ( src_left[(8+i)*src_stride] - src_left[(6-i)*src_stride] );
    }

    a = 16 * ( src[-1 + 15*src_stride] + src[15 - src_stride] );
    b = ( 5 * sum_h + 32 ) >> 6;
    c = ( 5 * sum_v + 32 ) >> 6;

    i00 = a + (b<<3) + (c<<3) + 16;
	
    dst += dst_stride<<4;	
    for( y = 16; y != 0; y-- )
    {
        pix_line = i00;
		dst -= dst_stride;
        for( x = 16; x != 0; x-- )
        {
            dst[x-1] = Clip255( pix_line>>5 );
            pix_line -= b;
        }
        i00 -= c;
    }
}
#endif

/****************************************************************************
 * 8x8 prediction for intra chroma block
 ****************************************************************************/

static void PredIntraChroma8x8Dc128( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 i;

    for( i = 8; i != 0; i-- )
    {
        M32(dst)=M32(dst+4)=0x80808080;
		dst += dst_stride;
    }
}
static void PredIntraChroma8x8DcLeft( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 i;
    VO_U32 l0 = 0, l1 = 0;
	VO_U8 *src_left = src - src_stride - 1;
	VO_U8 *src_left4 = src_left + (src_stride<<2);

    for( i = 2; i != 0; i-- )
    {
        l0 += *(src_left+=src_stride);
		l0 += *(src_left+=src_stride);
        l1 += *(src_left4+=src_stride);
		l1 += *(src_left4+=src_stride);
    }
    l0 = (( l0 + 2 ) >> 2)*0x01010101;
    l1 = (( l1 + 2 ) >> 2)*0x01010101;

	src_left = dst;
	src_left4 = dst + (dst_stride<<2);
    for( i = 4; i != 0; i-- )
    {
        M32(src_left)=M32(src_left+4)=l0;
		M32(src_left4)=M32(src_left4+4)=l1;
		src_left += dst_stride;
		src_left4 += dst_stride;
    }
}
static void PredIntraChroma8x8DcTop( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 i;
    VO_U32 u0 = 0, u1 = 0;
	VO_U8 *src_up = src - src_stride;

	u0 += *(src_up++);u0 += *(src_up++);
	u0 += *(src_up++);u0 += *(src_up++);
	u1 += *(src_up++);u1 += *(src_up++);
	u1 += *(src_up++);u1 += *(src_up++);
    u0 = (( u0 + 2 ) >> 2)*0x01010101;
    u1 = (( u1 + 2 ) >> 2)*0x01010101;

	src_up = dst + (dst_stride<<2);
    for( i = 4; i != 0; i-- )
    {
        M32(dst)=M32(src_up)=u0;
        M32(dst+4)=M32(src_up+4)=u1;
        src_up += dst_stride;
		dst += dst_stride;
    }
}
static void PredIntraChroma8x8Dc( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_S32 sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    VO_U32 dc0, dc1, dc2, dc3;
    VO_U32 i;
	VO_U8 *src_up = src - src_stride;
	VO_U8 *src_left = src - 1;

	sum0 = src_up[0]+src_up[1]+src_up[2]+src_up[3];
	sum1 = src_up[4]+src_up[5]+src_up[6]+src_up[7];
	sum2 = src_left[0]+src_left[src_stride]+src_left[src_stride*2]+src_left[src_stride*3];
	sum3 = src_left[src_stride*4]+src_left[src_stride*5]+src_left[src_stride*6]+src_left[src_stride*7];
		
    dc0 = (( sum0 + sum2 + 4 ) >> 3)*0x01010101;
    dc1 = (( sum1 + 2 ) >> 2)*0x01010101;
    dc2 = (( sum3 + 2 ) >> 2)*0x01010101;
    dc3 = (( sum1 + sum3 + 4 ) >> 3)*0x01010101;

	src_up = dst + (dst_stride<<2);
    for( i = 0; i < 4; i++ )
    {
        M32( dst ) = dc0;
        M32( dst+4 ) = dc1;
		M32( src_up ) = dc2;
        M32( src_up+4 ) = dc3;
		dst += dst_stride;
		src_up += dst_stride;
    }
}
#if !defined(VOARMV7)
static void PredIntraChroma8x8H_C( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 i;
	VO_U8 *src_left = src - 1;

    for( i = 8; i != 0; i-- )
    {
        VO_U32 v = 0x01010101 * src_left[0];
        M32( dst+0 ) = v;
        M32( dst+4 ) = v;
        src_left += src_stride;
		dst += dst_stride;
    }
}
static void PredIntraChroma8x8V_C( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 v0 = M32( src-src_stride );
    VO_U32 v4 = M32( src+4-src_stride );
    VO_U32 i;

    for( i = 8; i != 0; i-- )
    {
        M32( dst+0 ) = v0;
        M32( dst+4 ) = v4;
		dst += dst_stride;
    }
}
#endif
static void PredIntraChroma8x8P( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_S32 i;
    VO_U32 x,y;
    VO_S32 a, b, c;
    VO_S32 sum_h = 0;
    VO_S32 sum_v = 0;
    VO_S32 i00;
	VO_S32 pix_line;
	VO_U8 *src_up = src - src_stride;
	VO_U8 *src_left = src - 1;

    for( i = 0; i < 4; i++ )
    {
        sum_h += ( i + 1 ) * ( src_up[4 + i] - src_up[2 - i] );
        sum_v += ( i + 1 ) * ( src_left[(i+4)*src_stride] - src_left[(2-i)*src_stride] );
    }

    a = 16 * ( src[-1+7*src_stride] + src[7 - src_stride] );
    b = ( 17 * sum_h + 16 ) >> 5;
    c = ( 17 * sum_v + 16 ) >> 5;
	
    i00 = a + (b<<2) + (c<<2) + 16;

	dst += dst_stride<<3;
    for( y = 8; y != 0; y-- )
    {
        pix_line = i00;
		dst -= dst_stride;
        for( x = 8; x != 0; x-- )
        {
            dst[x-1] = Clip255( pix_line>>5 );
            pix_line -= b;
        }
        i00 -= c;
    }
}


const PredictFunc  PredictIntraLuma16x16[4+3] = {
	PredIntraLuma16x16V, PredIntraLuma16x16H, PredIntraLuma16x16Dc, PredIntraLuma16x16P,
    PredIntraLuma16x16DcLeft,PredIntraLuma16x16DcTop, PredIntraLuma16x16Dc128
};

const PredictFunc  PredictIntraChroma8x8[4+3] = {
    PredIntraChroma8x8Dc, PredIntraChroma8x8H, PredIntraChroma8x8V,PredIntraChroma8x8P,
    PredIntraChroma8x8DcLeft,PredIntraChroma8x8DcTop, PredIntraChroma8x8Dc128
};

