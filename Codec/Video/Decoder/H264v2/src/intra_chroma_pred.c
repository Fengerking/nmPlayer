#include "global.h"
#include "block.h"
//#include "mb_access.h"
#include "image.h"
static inline VO_U8 Clip255( VO_S32 x )
{
	return (VO_U8)(x&(~255) ? (-x)>>31 : x);
}

#if defined(VOARMV7)
void PredIntraChroma8x8H_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraChroma8x8V_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraChroma8x8Dc128_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraChroma8x8Dc_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraChroma8x8DcTop_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraChroma8x8DcLeft_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraChroma8x8P_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
#else


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
	sum2 = src_left[0]+src_left[src_stride]+src_left[src_stride<<1]+src_left[(src_stride<<1)+src_stride];
	sum3 = src_left[src_stride<<2]+src_left[(src_stride<<2)+src_stride]
		   +src_left[(src_stride<<2)+(src_stride<<1)]+src_left[(src_stride<<3)-src_stride];
		
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

    a = ( src_up[-1+(src_stride<<3)] + src_up[7] )<<4;
    b = ( (sum_h<<4) + sum_h + 16 ) >> 5;
    c = ( (sum_v<<4) + sum_v + 16 ) >> 5;
	
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

void intrapred_chroma(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
	Slice *currSlice = pDecLocal->p_Slice;
	int src_stride = pDecGlobal->iChromaStride<<pDecLocal->mb_field;
	VO_U8 *pSrcU = pDecGlobal->dec_picture->imgUV[0]+pDecLocal->pix_c_y*src_stride+pDecLocal->pix_c_x;
	VO_U8 *pSrcV = pDecGlobal->dec_picture->imgUV[1]+pDecLocal->pix_c_y*src_stride+pDecLocal->pix_c_x;
	if(pDecLocal->mb_field)
    {
      pSrcU = pDecGlobal->dec_picture->imgUV[0]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  //if(pDecLocal->mb.y&1)
	  //  pSrcU -= 7*pDecGlobal->iChromaStride;
	  pSrcV= pDecGlobal->dec_picture->imgUV[1]+(pDecLocal->pix_c_y)*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  //if(pDecLocal->mb.y&1)
	  //  pSrcV -= 7*pDecGlobal->iChromaStride;
    }

	switch (pDecLocal->c_ipred_mode) 
	{
	case DC_PRED_8:  
		//intrapred_chroma_dc(currMB);
#if defined(VOARMV7)
		PredIntraChroma8x8Dc_ARMV7( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8Dc_ARMV7( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8Dc( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8Dc( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	case DC_128_PRED_8:
		//intrapred_chroma_dc(currMB);
#if defined(VOARMV7)
		PredIntraChroma8x8Dc128_ARMV7( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8Dc128_ARMV7( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8Dc128( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8Dc128( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	case DC_LEFT_PRED_8:  
		//intrapred_chroma_dc(currMB);
#if defined(VOARMV7)
		PredIntraChroma8x8DcLeft_ARMV7( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8DcLeft_ARMV7( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8DcLeft( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8DcLeft( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	case DC_TOP_PRED_8:  
		//intrapred_chroma_dc(currMB);
#if defined(VOARMV7)
		PredIntraChroma8x8DcTop_ARMV7( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8DcTop_ARMV7( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8DcTop( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8DcTop( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	case HOR_PRED_8: 
#if defined(VOARMV7)
		PredIntraChroma8x8H_ARMV7( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8H_ARMV7( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8H_C( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8H_C( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	case VERT_PRED_8: 
#if defined(VOARMV7)
		PredIntraChroma8x8V_ARMV7( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8V_ARMV7( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8V_C( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8V_C( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	case PLANE_8: 
#if defined(VOARMV7)
		PredIntraChroma8x8P( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8P( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8P( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8P( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	default:
		//warning_dtt
#if defined(VOARMV7)
		PredIntraChroma8x8Dc128_ARMV7( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8Dc128_ARMV7( pSrcV , src_stride, pSrcV , src_stride);
#else
		PredIntraChroma8x8Dc128( pSrcU , src_stride, pSrcU , src_stride);
		PredIntraChroma8x8Dc128( pSrcV , src_stride, pSrcV , src_stride);
#endif
		break;
	}
}

