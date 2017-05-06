#include "global.h"
#include "intra16x16_pred.h"

#if defined(VOARMV7)
void PredIntraLuma16x16P_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraLuma16x16Dc_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraLuma16x16DcTop_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraLuma16x16DcLeft_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraLuma16x16Dc128_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraLuma16x16H_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
void PredIntraLuma16x16V_ARMV7( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
#else
extern	void PredIntraLuma16x16P_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
extern	void PredIntraLuma16x16DC_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride, VO_U32 availability);
extern	void PredIntraLuma16x16H_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
extern	void PredIntraLuma16x16V_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride);
static inline VO_U8 Clip255( VO_S32 x )
{
	return (VO_U8)(x&(~255) ? (-x)>>31 : x);
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


static void PredIntraLuma16x16P( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
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

    a = ( src_up[-1 + (src_stride<<4)] + src_up[15] )<<4;
    b = ( (sum_h<<2) + sum_h + 32 ) >> 6;
    c = ( (sum_v<<2) + sum_v + 32 ) >> 6;

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

#define PREDICT_16x16_DC(v) \
	for( i = 16; i != 0; i-- )\
	{\
		M32( dst+ 0 ) = v;\
		M32( dst+ 4 ) = v;\
		M32( dst+ 8 ) = v;\
		M32( dst+12 ) = v;\
		dst += dst_stride;\
	}

static void PredIntraLuma16x16Dc_C( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U32 sum,i;
	VO_U8 *src_up = src - src_stride;
	VO_U8 *src_left = src_up - 1;
	for( i = 4,sum = 0; i != 0; i-- )
    {
        sum += *(src_up++);
		sum += *(src_up++);
		sum += *(src_up++);
		sum += *(src_up++);
		sum += *(src_left+=src_stride);
		sum += *(src_left+=src_stride);
		sum += *(src_left+=src_stride);
		sum += *(src_left+=src_stride);
    }
    sum = (( sum + 16 ) >> 5) * 0x01010101;

    PREDICT_16x16_DC(sum);
}
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
#endif


int intrapred_16x16_normal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, //!< Current Macroblock
						         //!< Current colorplane (for 4:4:4)                         
						   int predmode)        //!< prediction mode
{
	Slice *currSlice = pDecLocal->p_Slice;
	imgpel *imgY = pDecGlobal->dec_picture->imgY;
    VO_S32 iLumaStride = pDecGlobal->iLumaStride<<pDecLocal->mb_field;
	VO_U8 *pSrc;
    if(pDecLocal->mb_field)
    {
      pSrc = imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	  //if(pDecLocal->mb.y&1)
	  //  pSrc -= 15*pDecGlobal->iLumaStride;
    }
	else
	{
		 pSrc = imgY+pDecLocal->pix_y*iLumaStride+pDecLocal->pix_x;
	}

	switch (predmode)
	{
	case VERT_PRED_16:                       // vertical prediction from block above
#if defined(VOARMV7)
		PredIntraLuma16x16V_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride);
#elif defined(VOARMV6)	
		PredIntraLuma16x16V_ARMV6( pSrc , iLumaStride, pSrc , iLumaStride);
#else
		PredIntraLuma16x16V_C( pSrc , iLumaStride, pSrc , iLumaStride);
#endif
		return DECODING_OK;
		break;
	case HOR_PRED_16:                        // horizontal prediction from left block
#if defined(VOARMV7)
		PredIntraLuma16x16H_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride);
#elif defined(VOARMV6)	
		PredIntraLuma16x16H_ARMV6( pSrc , iLumaStride, pSrc , iLumaStride);
#else
		PredIntraLuma16x16H_C( pSrc , iLumaStride, pSrc , iLumaStride);
#endif
		return DECODING_OK;
		break;
	case DC_PRED_16:                         // DC prediction
#if defined(VOARMV7)
		PredIntraLuma16x16Dc_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride);
#elif defined(VOARMV6)	
		PredIntraLuma16x16DC_ARMV6( pSrc , iLumaStride, pSrc , iLumaStride, 0x3);
#else
		PredIntraLuma16x16Dc_C( pSrc , iLumaStride, pSrc , iLumaStride);
#endif	
		return DECODING_OK;
		break;
	case DC_LEFT_PRED_16:
#if defined(VOARMV7)
		PredIntraLuma16x16DcLeft_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride);
#else
		PredIntraLuma16x16DcLeft( pSrc , iLumaStride, pSrc , iLumaStride);
#endif
		return DECODING_OK;
		break;
	case DC_TOP_PRED_16:
#if defined(VOARMV7)
		PredIntraLuma16x16DcTop_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride); 
#else
        PredIntraLuma16x16DcTop( pSrc , iLumaStride, pSrc , iLumaStride); 
#endif
		return DECODING_OK;
		break;
	case DC_128_PRED_16:
#if defined(VOARMV7)
		PredIntraLuma16x16Dc128_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride);
#else
		PredIntraLuma16x16Dc128( pSrc , iLumaStride, pSrc , iLumaStride);
#endif
		return DECODING_OK;
		break;
	case PLANE_16:// 16 bit integer plan pred
#if defined(VOARMV7)
		PredIntraLuma16x16P_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride);
#elif defined(VOARMV6)	
		PredIntraLuma16x16P_ARMV6( pSrc , iLumaStride, pSrc , iLumaStride);
#else
		PredIntraLuma16x16P( pSrc , iLumaStride, pSrc , iLumaStride);
#endif
		return DECODING_OK;
		break;
	default:
#if defined(VOARMV7)
		PredIntraLuma16x16Dc128_ARMV7( pSrc , iLumaStride, pSrc , iLumaStride);
#else
		PredIntraLuma16x16Dc128( pSrc , iLumaStride, pSrc , iLumaStride);
#endif
		return DECODING_OK;
	}
}

