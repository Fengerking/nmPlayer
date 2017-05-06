#include "global.h"
#include "intra4x4_pred.h"
#include "mbuffer.h"
//#include "mb_access.h"
//#include "image.h"

#define PREDICT4x4DC(dc)\
    M32(dst) = dc;\
    M32(dst+dst_stride) = dc;\
	M32(dst+(dst_stride<<1)) = dc;\
	M32(dst+dst_stride+(dst_stride<<1)) = dc;

//edit by Really Yang 20110302
static void Predict4x4DC128(VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    PREDICT4x4DC(0x80808080);
}
static void Predict4x4DCLeft( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
	VO_U32 l = src[-1]+src[-1+src_stride]+src[-1+(src_stride<<1)]+src[-1+src_stride+(src_stride<<1)]+2;
	l = (l>>2)*0x01010101;
    PREDICT4x4DC(l);
}
static void Predict4x4DCTop( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    VO_U32 u = src[-src_stride]+src[1-src_stride]+src[2-src_stride]+src[3-src_stride]+2;
	u = (u>>2)*0x01010101;
    PREDICT4x4DC(u);
}
static void Predict4x4DC( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    VO_U32 dc = src[-src_stride]+src[1-src_stride]+src[2-src_stride]+src[3-src_stride]+src[-1]
		        +src[-1+src_stride]+src[-1+(src_stride<<1)]+src[-1+src_stride+(src_stride<<1)]+ 4;
	dc = (dc>>3)*0x01010101;
    PREDICT4x4DC(dc);
}
static void Predict4x4H( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    M32(dst) = src[-1] * 0x01010101;
    M32(dst+dst_stride) = src[-1+src_stride] * 0x01010101;
    M32(dst+(dst_stride<<1)) = src[-1+(src_stride<<1)] * 0x01010101;
    M32(dst+dst_stride+(dst_stride<<1)) = src[-1+src_stride+(src_stride<<1)] * 0x01010101;
}
static void Predict4x4V( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    PREDICT4x4DC(M32(src-src_stride));
}
static void Predict4x4DDR( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    VO_U8 PredPixel[7];
	VO_S32 up_left = src[-src_stride-1];
	VO_S32 up0 = src[-src_stride];
	VO_S32 up1 = src[-src_stride+1];
	VO_S32 up2 = src[-src_stride+2];
	VO_S32 up3 = src[-src_stride+3];
	VO_S32 left0 = src[-1];
	VO_S32 left1 = src[-1+src_stride];
	VO_S32 left2 = src[-1+(src_stride<<1)];
	VO_S32 left3 = src[-1+src_stride+(src_stride<<1)];	

    PredPixel[0] = (VO_U8) ((left1 + (left2<<1) + left3 + 2) >> 2);
    PredPixel[1] = (VO_U8) ((left0 + (left1<<1) + left2 + 2) >> 2);
    PredPixel[2] = (VO_U8) ((up_left + (left0<<1) + left1 + 2) >> 2);
    PredPixel[3] = (VO_U8) ((up0 + (up_left<<1) + left0 + 2) >> 2);
    PredPixel[4] = (VO_U8) ((up1 + (up0<<1) + up_left + 2) >> 2);
    PredPixel[5] = (VO_U8) ((up2 + (up1<<1) + up0 + 2) >> 2);
    PredPixel[6] = (VO_U8) ((up3 + (up2<<1) + up1 + 2) >> 2);

	CP32(dst,PredPixel+3);
	CP32(dst+dst_stride,PredPixel+2);
	CP32(dst+(dst_stride<<1),PredPixel+1);
	CP32(dst+dst_stride+(dst_stride<<1),PredPixel);
}

static void Predict4x4VR( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    VO_U8 PredPixel[10];
	VO_S32 up_left = src[-src_stride-1];
	VO_S32 up0 = src[-src_stride];
	VO_S32 up1 = src[-src_stride+1];
	VO_S32 up2 = src[-src_stride+2];
	VO_S32 up3 = src[-src_stride+3];
	VO_S32 left0 = src[-1];
	VO_S32 left1 = src[-1+src_stride];
	VO_S32 left2 = src[-1+(src_stride<<1)];		

    PredPixel[0] = (VO_U8) ((left1 + (left0<<1) + up_left + 2) >> 2);
    PredPixel[1] = (VO_U8) ((up_left + up0 + 1) >> 1);
    PredPixel[2] = (VO_U8) ((up0 + up1 + 1) >> 1);
    PredPixel[3] = (VO_U8) ((up1 + up2 + 1) >> 1);
    PredPixel[4] = (VO_U8) ((up2 + up3 + 1) >> 1);
    PredPixel[5] = (VO_U8) ((left2 + (left1<<1) + left0 + 2) >> 2);
    PredPixel[6] = (VO_U8) ((left0 + (up_left<<1) + up0 + 2) >> 2);
	PredPixel[7] = (VO_U8) ((up_left + (up0<<1) + up1 + 2) >> 2);
	PredPixel[8] = (VO_U8) ((up0 + (up1<<1) + up2 + 2) >> 2);
	PredPixel[9] = (VO_U8) ((up1 + (up2<<1) + up3 + 2) >> 2);

	CP32(dst,PredPixel+1);
	CP32(dst+dst_stride,PredPixel+6);
	CP32(dst+(dst_stride<<1),PredPixel);
	CP32(dst+dst_stride+(dst_stride<<1),PredPixel+5);
}


static void Predict4x4HD( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    VO_U8 PredPixel[10];
	VO_S32 up_left = src[-src_stride-1];
	VO_S32 up0 = src[-src_stride];
	VO_S32 up1 = src[-src_stride+1];
	VO_S32 up2 = src[-src_stride+2];
	VO_S32 left0 = src[-1];
	VO_S32 left1 = src[-1+src_stride];
	VO_S32 left2 = src[-1+(src_stride<<1)];
	VO_S32 left3 = src[-1+src_stride+(src_stride<<1)];	

    PredPixel[0] = (VO_U8) ((left2 + left3 + 1) >> 1);
    PredPixel[1] = (VO_U8) ((left1 + (left2<<1) + left3 + 2) >> 2);
    PredPixel[2] = (VO_U8) ((left1 + left2 + 1) >> 1);
    PredPixel[3] = (VO_U8) ((left0 + (left1<<1) + left2 + 2) >> 2);
    PredPixel[4] = (VO_U8) ((left0 + left1 + 1) >> 1);
    PredPixel[5] = (VO_U8) ((up_left + (left0<<1) + left1 + 2) >> 2);
    PredPixel[6] = (VO_U8) ((up_left + left0 + 1) >> 1);
	PredPixel[7] = (VO_U8) ((up0 + (up_left<<1) + left0 + 2) >> 2);
	PredPixel[8] = (VO_U8) ((up1 + (up0<<1) + up_left + 2) >> 2);
	PredPixel[9] = (VO_U8) ((up2 + (up1<<1) + up0 + 2) >> 2);

	CP32(dst,PredPixel+6);
	CP32(dst+dst_stride,PredPixel+4);
	CP32(dst+(dst_stride<<1),PredPixel+2);
	CP32(dst+dst_stride+(dst_stride<<1),PredPixel);
}

static void Predict4x4HU( VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride )
{
    VO_U8 PredPixel[10];
	VO_S32 left0 = src[-1];
	VO_S32 left1 = src[-1+src_stride];
	VO_S32 left2 = src[-1+(src_stride<<1)];
	VO_S32 left3 = src[-1+src_stride+(src_stride<<1)];
	
    PredPixel[0] = (VO_U8) ((left0 + left1 + 1) >> 1);
    PredPixel[1] = (VO_U8) ((left0 + (left1<<1) + left2 + 2) >> 2);
    PredPixel[2] = (VO_U8) ((left1 + left2 + 1) >> 1);
    PredPixel[3] = (VO_U8) ((left1 + (left2<<1) + left3 + 2) >> 2);
    PredPixel[4] = (VO_U8) ((left2 + left3 + 1) >> 1);
    PredPixel[5] = (VO_U8) ((left2 + (left3<<1) + left3 + 2) >> 2);
    PredPixel[6] = (VO_U8) left3;
	PredPixel[7] = (VO_U8) left3;
	PredPixel[8] = (VO_U8) left3;
	PredPixel[9] = (VO_U8) left3;

	CP32(dst,PredPixel);
	CP32(dst+dst_stride,PredPixel+2);
	CP32(dst+(dst_stride<<1),PredPixel+4);
	CP32(dst+dst_stride+(dst_stride<<1),PredPixel+6);
}


static void Predict4x4DDL(VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    src[0] = (VO_U8) ((src[0] + src[2] + (src[1]<<1) + 2) >> 2);
    src[1] = (VO_U8) ((src[1] + src[3] + (src[2]<<1) + 2) >> 2);
    src[2] = (VO_U8) ((src[2] + src[4] + (src[3]<<1) + 2) >> 2);
    src[3] = (VO_U8) ((src[3] + src[5] + (src[4]<<1) + 2) >> 2);
    src[4] = (VO_U8) ((src[4] + src[6] + (src[5]<<1) + 2) >> 2);
    src[5] = (VO_U8) ((src[5] + src[7] + (src[6]<<1) + 2) >> 2);
    src[6] = (VO_U8) ((src[6] + src[7] + (src[7]<<1) + 2) >> 2);

    CP32(dst,src);
	CP32(dst+dst_stride,src+1);
	CP32(dst+(dst_stride<<1),src+2);
	CP32(dst+dst_stride+(dst_stride<<1),src+3);
}

static void Predict4x4VL(VO_U8 *src , VO_S32 src_stride, VO_U8 *dst, VO_S32 dst_stride)
{
    VO_U8 PredPixel[10];

    PredPixel[0] = (VO_U8) ((src[0] + src[1] + 1) >> 1);
    PredPixel[1] = (VO_U8) ((src[1] + src[2] + 1) >> 1);
    PredPixel[2] = (VO_U8) ((src[2] + src[3] + 1) >> 1);
    PredPixel[3] = (VO_U8) ((src[3] + src[4] + 1) >> 1);
    PredPixel[4] = (VO_U8) ((src[4] + src[5] + 1) >> 1);
    PredPixel[5] = (VO_U8) ((src[0] + (src[1]<<1) + src[2] + 2) >> 2);
    PredPixel[6] = (VO_U8) ((src[1] + (src[2]<<1) + src[3] + 2) >> 2);
    PredPixel[7] = (VO_U8) ((src[2] + (src[3]<<1) + src[4] + 2) >> 2);
    PredPixel[8] = (VO_U8) ((src[3] + (src[4]<<1) + src[5] + 2) >> 2);
    PredPixel[9] = (VO_U8) ((src[4] + (src[5]<<1) + src[6] + 2) >> 2);

	CP32(dst,PredPixel);
	CP32(dst+dst_stride,PredPixel+5);
	CP32(dst+(dst_stride<<1),PredPixel+1);
	CP32(dst+dst_stride+(dst_stride<<1),PredPixel+6);
}	

int intra4x4_pred_normal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,    //!< current macroblock
						          //!< current image plane
						 int ioff,              //!< pixel offset X within MB
						 int joff)      
{
	//VideoParameters *p_Vid = currMB->p_Vid;
	//byte predmode = p_Vid->ipredmode[img_block_y][img_block_x];
	//byte predmode = p_Vid->ipredmode_line[currMB->mb.x*16+joff+(ioff>>2)];
// #if USE_MULTI_THREAD
// 	VO_S8 predmode = pDecGlobal->ipredmode_frame[pDecLocal->mbAddrX*16 + joff + (ioff>>2)];
// #else
	byte predmode = pDecLocal->ipredmode[12+(joff<<1)+(ioff>>2)];
// #endif


	Slice *currSlice = pDecLocal->p_Slice;
	VO_S32 iLumaStride = pDecGlobal->iLumaStride<<pDecLocal->mb_field;
	imgpel *imgY = pDecGlobal->dec_picture->imgY;

	//VO_U8 *pSrc = (VO_U8*)&(imgY[pDecLocal->pix_y + joff][pDecLocal->pix_x + ioff]);
	VO_U8 *pSrc = imgY+(pDecLocal->pix_y + joff)*iLumaStride+pDecLocal->pix_x + ioff;
	VO_U8 *pSrc1 = pSrc;
	VO_U8 topright[8] = {0};
	if(pDecLocal->mb_field)
    {
      pSrc = imgY+(pDecLocal->pix_y)*pDecGlobal->iLumaStride+pDecLocal->pix_x + ioff;
	  //if(pDecLocal->mb.y&1)
	  //  pSrc -= 15*pDecGlobal->iLumaStride;
	  pSrc += joff*iLumaStride;
    }
	
	if(predmode == DIAG_DOWN_LEFT_PRED || predmode == VERT_LEFT_PRED)
	{
	    int i = (ioff>>2)+((ioff>>3)<<1)+(joff>>1)+((joff>>3)<<2);
		pSrc1 = pSrc;
		pSrc -= iLumaStride;
		topright[0] = *pSrc++;
		topright[1] = *pSrc++;
		topright[2] = *pSrc++;
		topright[3] = *pSrc;
		if(!((pDecLocal->intra_ava_topright<<i)&0x8000))
			M32(topright+4) = (VO_U32)((*pSrc)*0X01010101);
		else
		{
			pSrc++;
			topright[4] = *pSrc++;
			topright[5] = *pSrc++;
			topright[6] = *pSrc++;
			topright[7] = *pSrc;
		}
		
		pSrc = topright;
	}


	//currMB->ipmode_DPCM = predmode; //For residual DPCM

	switch (predmode)
	{
	case DC_PRED:
		Predict4x4DC(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case DC_128_PRED:
		Predict4x4DC128(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case DC_LEFT_PRED:
		Predict4x4DCLeft(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case DC_TOP_PRED:
		Predict4x4DCTop(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case VERT_PRED:
		Predict4x4V(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case HOR_PRED:
		Predict4x4H(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case DIAG_DOWN_RIGHT_PRED:
		Predict4x4DDR(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case DIAG_DOWN_LEFT_PRED:
		Predict4x4DDL(pSrc,iLumaStride, pSrc1 , iLumaStride);
		return DECODING_OK;
		break;
	case VERT_RIGHT_PRED:
		Predict4x4VR(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case VERT_LEFT_PRED:
		Predict4x4VL(pSrc,iLumaStride, pSrc1 , iLumaStride);
		return DECODING_OK;
		break;
	case HOR_UP_PRED:
		Predict4x4HU(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
		break;
	case HOR_DOWN_PRED:  
		Predict4x4HD(pSrc,iLumaStride, pSrc , iLumaStride);
		return DECODING_OK;
	default:
		//printf("Error: illegal intra_4x4 prediction mode: %d\n", (int) predmode);
		return SEARCH_SYNC;
		break;
	}
}
