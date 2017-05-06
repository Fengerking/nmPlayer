/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "../voMpegEnc.h"
#include "voMpegEncFrameProc.h"
#include "voMpegMemmory.h"
#include "../lowlevel/voMpegEncPreProcess.h"

VO_S32 FrameCreate( ENCHND *enc_hnd,  
                            Mpeg4Frame * frame, 
                            VO_U32 frame_width, 
                            VO_U32 frame_height )
{
	const VO_U32 half_frame_width = frame_width >>1;
	const VO_U32 half_frame_height = frame_height >>1;
	VO_U32 total_yuv_size = frame_width * (frame_height + 1) + 64
		+half_frame_width * half_frame_height + 64
		+half_frame_width * half_frame_height + 64;

	frame->y = voMpegMalloc(enc_hnd, total_yuv_size, CACHE_LINE);
	if (frame->y == NULL) {
		return -1;
	}
	memset(frame->y, 0, total_yuv_size);
	frame->uv[0] = frame->y + frame_width * (frame_height + 1) + 64;
	frame->uv[1] = frame->uv[0] + half_frame_width * half_frame_height + 64;
	
	frame->y += EDGE_SIZE * frame_width + EDGE_SIZE;
	frame->uv[0] += 16 * half_frame_width + 16;
	frame->uv[1] += 16 * half_frame_width + 16;
	
	return 0;
}

VO_VOID FrameDestroy( ENCHND *enc_hnd, 
                        Mpeg4Frame * frame, 
                        VO_U32 frame_width, 
                        VO_U32 frame_height )
{
	//const VO_U32 half_frame_width = frame_width>>1;	
	if (frame->y) {
		voMpegFree(enc_hnd, frame->y - (EDGE_SIZE * frame_width + EDGE_SIZE));
		frame->y = NULL;
	}
}

VO_VOID FrameSetEdge( Mpeg4Frame * frame, 
                         const VO_U32 y_stride, 
                         const VO_U32 width, 
                         const VO_U32 height )
{
	const VO_U32 uv_stride = y_stride >>1;
	VO_U32 half_width, half_height;
	VO_U32 i;
	VO_U8 *y_dst,*u_dst,*v_dst;
	VO_U8 *y_src,*u_src,*v_src;
	
	y_dst = frame->y - (EDGE_SIZE + EDGE_SIZE * y_stride);
	y_src = frame->y;	
	
	half_width = width>>1;
	half_height = height>>1;
	
	for (i = 0; i < EDGE_SIZE; i++) {
		memset(y_dst, *y_src, EDGE_SIZE);
		memcpy(y_dst + EDGE_SIZE, y_src, width);
		memset(y_dst + y_stride - EDGE_SIZE, *(y_src + width - 1), EDGE_SIZE);
		y_dst += y_stride;
	}
	
	for (i = 0; i < height; i++) {
		memset(y_dst, *y_src, EDGE_SIZE);
		memset(y_dst + y_stride - EDGE_SIZE, *(y_src+width - 1), EDGE_SIZE);
		y_dst += y_stride;
		y_src += y_stride;
	}
	
	y_src -= y_stride;
	for (i = 0; i < EDGE_SIZE; i++) {
		memset(y_dst, *y_src, EDGE_SIZE);
		memcpy(y_dst + EDGE_SIZE, y_src, width);
		memset(y_dst + y_stride - EDGE_SIZE, *(y_src + width - 1), EDGE_SIZE);
		y_dst += y_stride;
	}	
	
	/* U V*/
	u_dst = frame->uv[0] - (16 + 16 * uv_stride);
	u_src = frame->uv[0];

	v_dst = frame->uv[1] - (16 + 16 * uv_stride);
	v_src = frame->uv[1];
	
	for (i = 0; i < 16; i++) {
		memset(u_dst, *u_src, 16);
		memset(v_dst, *v_src, 16);
		memcpy(u_dst + 16, u_src, half_width);
		memcpy(v_dst + 16, v_src, half_width);
		memset(u_dst + uv_stride - 16, *(u_src + half_width - 1), 16);
		memset(v_dst + uv_stride - 16, *(v_src + half_width - 1), 16);
		u_dst += uv_stride;
		v_dst += uv_stride;
	}
	
	for (i = 0; i < half_height; i++) {
		memset(u_dst, *u_src, 16);
		memset(v_dst, *v_src, 16);
		memset(u_dst + uv_stride - 16, *(u_src+half_width - 1), 16);
		memset(v_dst + uv_stride - 16, *(v_src+half_width - 1), 16);
		u_dst += uv_stride;
		u_src += uv_stride;
		v_dst += uv_stride;
		v_src += uv_stride;
	}

	u_src -= uv_stride;
	v_src -= uv_stride;
	for (i = 0; i < 16; i++) {
		memset(u_dst, *u_src, 16);
		memset(v_dst, *v_src, 16);
		memcpy(u_dst + 16, u_src, half_width);
		memcpy(v_dst + 16, v_src, half_width);
		memset(u_dst + uv_stride - 16, *(u_src + half_width - 1), 16);
		memset(v_dst + uv_stride - 16, *(v_src + half_width - 1), 16);
		u_dst += uv_stride;
		v_dst += uv_stride;
	}
}

RETURN_CODE FramePreProcess( Mpeg4Frame * const img_cur,
						                 const VO_U32 image_type,
						                 const VO_IV_RTTYPE rt_type,
						                 const VO_U32 width,
						                 const VO_U32 height,
						                 const VO_U32 dst_stride,
						                 VO_U8 * src,
						                 const VO_U32 src_stride )
{
	const VO_U32 half_dst_stride = (dst_stride>>1);
	const VO_U32 half_height = (height>>1);
	
	switch(image_type){
	case VO_COLOR_YUV_PLANAR420:
		yuv420_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, src + src_stride*height, src + src_stride*height + (src_stride>>1)*half_height,
			src_stride, (src_stride>>1), width, height);
		break;
	case VO_COLOR_YUV_420_PACK_2:
		yuv420_pack_2_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, src + src_stride*height, src_stride, src_stride, width, height);
		break;
	case VO_COLOR_YUV_PLANAR422_12:
		yuv420_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, src + src_stride*height, src + src_stride*height + (src_stride>>1)*height,
			src_stride, src_stride, width, height);
		break;
	case VO_COLOR_UYVY422_PACKED:
		uyvy_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, width, height);
		break;
	case VO_COLOR_RGB565_PACKED:
		rgb565_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, width, height);
		break;
	case VO_COLOR_YVU_PLANAR420:
		yuv420_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[1], img_cur->uv[0], dst_stride, half_dst_stride,
			src, src + src_stride*height, src + src_stride*height + (src_stride>>1)*half_height,
			src_stride, src_stride>>1, width, height);
		break;
	case VO_COLOR_VYUY422_PACKED_2:
		vyuy2_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, width, height);
		break;
	case VO_COLOR_UYVY422_PACKED_2:
		vyuy2_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[1], img_cur->uv[0], dst_stride, half_dst_stride,
			src, width, height);
		break;
	case VO_COLOR_YUYV422_PACKED:
		yuyv_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, width, height);
		break;
	case VO_COLOR_ARGB32_PACKED:
		rgb32_to_yuv420_all[rt_type](img_cur->y, img_cur->uv[0], img_cur->uv[1], dst_stride, half_dst_stride,
			src, width, height);
		break;
	default :
		return UNSUPPORT_INPUT_TYPE;
	}
	
	return MPEG_SUCCEEDED;
}

#ifdef CAL_PSNR
#include <math.h>
#include "../lowlevel/voMpegEncSad.h"

#define CALCLINESEE(value,b1,b2,width,stride) \
    for(i=0;i<width;i++) {\
        (value) += (*(b1+i) - *(b2+i))*(*(b1+i) - *(b2+i));\
    }\
    (b1) += (stride);\
    (b2) += (stride);

VO_U32 CalculateSEE8x8_C(const VO_U8 * b1, const VO_U8 * b2, const VO_U32 stride)
{
    VO_U32 i=0;
	VO_S32 sse = 0;
    CALCLINESEE(sse,b1,b2,8,stride);
    CALCLINESEE(sse,b1,b2,8,stride);
    CALCLINESEE(sse,b1,b2,8,stride);
    CALCLINESEE(sse,b1,b2,8,stride);
    CALCLINESEE(sse,b1,b2,8,stride);
    CALCLINESEE(sse,b1,b2,8,stride);
    CALCLINESEE(sse,b1,b2,8,stride);
    CALCLINESEE(sse,b1,b2,8,stride);		
	return(sse);
}

VO_VOID FrameCopy(Mpeg4Frame * const frame_dst,
		            const Mpeg4Frame * const frame_src,
		            const VO_U32 stride,
		            const VO_U32 height)
{
	memcpy(frame_dst->y, frame_src->y, stride * height);
	memcpy(frame_dst->uv[0], frame_src->uv[0], stride * height / 4);
	memcpy(frame_dst->uv[1], frame_src->uv[1], stride * height / 4);
}


float FramePsnr( Mpeg4Frame * orig_frame,  
                    Mpeg4Frame * recon_frame, 
                    VO_U16 stride, 
                    VO_U16 width, 
                    VO_U16 height )
{
	VO_S32 i, j, quad = 0;
	VO_U8 *y_orig_frame = orig_frame->y;
	VO_U8 *y_recon_frame = recon_frame->y;
	float psnr;
	
	for (j = 0; j < height; j++) 
        CALCLINESEE(quad,y_orig_frame,y_recon_frame,width,stride);

	if(quad )	{
		psnr = (float)((65025.0*width * height)/quad);
		psnr = 10 * (float) log10(psnr);
	}
    else {
		psnr = (float) 99.99;
    }
	
	return psnr;
}

float SeeToPsnr(VO_S32 sse, VO_S32 pixels)
{
    if (sse==0) {
		return 99.99F;	
    }
	else {
		float psnr = (float)sse/(float)(pixels);
		return 48.131F - 10*(float)log10(psnr);
	}
}

VO_S32 PlaneSSE(VO_U8 *orig_frame, 
                        VO_U8 *recon_frame, 
                        VO_U32 stride, 
                        VO_U32 width, 
                        VO_U32 height)
{
	VO_S32 y, block_width, block_height;
	VO_S32 sse = 0;
	
	block_width  = width  & (~0x07);
	block_height = height & (~0x07);
	
	// Compute the 8x8 integer part 
	for (y = 0; y<block_height; y += 8) {
		VO_S32 x;		
		// Compute sse for the band 
		for (x = 0; x<block_width; x += 8)
			sse += CalculateSEE8x8_C(orig_frame  + x, recon_frame + x, stride);
		
		// remaining pixels of the 8 pixels high band
		for (x = block_width; x < (VO_S32)width; x++) {
			VO_S32 i=0,diff;
            for(i=0;i<8;i++) {
                diff = *(orig_frame + i*stride + x) - *(recon_frame + i*stride + x);
			    sse += diff * diff;
            }
		}		
		orig_frame  += 8*stride;
		recon_frame += 8*stride;
	}
	
	//Compute the down rectangle sse
	for (y = block_height; y < (VO_S32)height; y++) {
		VO_S32 i=0;
        CALCLINESEE(sse,orig_frame,recon_frame,width,stride);
	}
	
	return (sse);
}
#endif