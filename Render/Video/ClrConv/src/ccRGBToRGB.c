/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include <stdlib.h>

#include "ccRGBToRGB.h"
#include "ccRGBToRGBMB.h"
#include "ccConstant.h"

CLRCONV_FUNC_PTR RGB16_RESIZE[3][2] =
{
	{Rgb16_noresize, Rgb16_noresize_RT90},
	{Rgb16_resize,   Rgb16_resize_RT90},
	{rgb565_resize_anti_aliasing, NULL}
};

CLRCONV_FUNC_PTR RGB24TORGB565[6] =
{	
	cc_rgb24_disable, cc_rgb24_l90, cc_rgb24_r90 ,
	cc_s_rgb24_disable, cc_s_rgb24_l90, cc_s_rgb24_r90
};

#ifdef COLCONVENABLE
CLRCONV_FUNC_PTR RGB888PlanarToRGB16[2] =
{
	RGB888PlanarToRGB16_noresize, NULL/*RGB888PlanarToRGB16_resize*/ 
};

CLRCONV_FUNC_PTR RGB888PlanarToRGB16ResizeNoRotation[5] =
{
	RGB888PlanarToRGB16_resize_noRotation,RGB888PlanarToRGB16_antiAlias_resize_noRotation,
	RGB888PlanarToRGB16_resize_Rotation_90L,RGB888PlanarToRGB16_resize_Rotation_90R,
	RGB888PlanarToRGB16_resize_Rotation_180
};

static const CC_MB_NEW_PTR RGB888Planar_to_RGB565_mb_all[2] = 
{
	RGB888Planar_to_RGB565_cc_mb, RGB888Planar_to_RGB565_cc_mb_16x16
};
#endif //COLCONVENABLE

#ifdef COLCONVENABLE
VOCCRETURNCODE RGB888PlanarToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_S32 vx = 0, width, height;
	VO_U8 *psrc_r = conv_data->pInBuf[0];
	VO_U8 *psrc_g = conv_data->pInBuf[1];
	VO_U8 *psrc_b = conv_data->pInBuf[2];
	VO_U8 *r, *g, *b;
	VO_U8	*start_out_buf, *out_buf;
	VO_S32 in_stride, uin_stride,vin_stride;
	VO_S32 out_width = conv_data->nOutWidth;
	VO_S32 out_height = conv_data->nOutHeight;
	const VO_S32 out_stride = conv_data->nOutStride;
	//const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
	//const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
	VO_S32 *param = cc_hnd->yuv2rgbmatrix;

	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInStride;
	vin_stride = conv_data->nInStride;	

	r = psrc_r;
	g = psrc_g;
	b = psrc_b;
	//add end	

	start_out_buf = out_buf = conv_data->pOutBuf[0];

	do{
		vx = out_width;
		out_buf = start_out_buf;	
		do{

			width = vx < 16 ? vx : 16;
			height = out_height < 16 ? out_height : 16;
			RGB888Planar_to_RGB565_mb_all[(width==16)&&(height==16)](psrc_r, psrc_g, psrc_b, in_stride, out_buf, out_stride,
				width, height, uin_stride, vin_stride, param);

			psrc_r += 16;
			psrc_g += 16;
			psrc_b += 16;
			out_buf += 32;
		}while((vx -= 16)> 0);

		psrc_r = r = r + (in_stride  << 4);
		psrc_g = g = g + (uin_stride << 4);
		psrc_b = b = b + (vin_stride << 4);
		start_out_buf += (out_stride << 4);
	}while((out_height -= 16)>0);

	return VO_ERR_NONE;

}
#endif //COLCONVENABLE

VOCCRETURNCODE cc_rgb24_disable(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_S32 a4;
	VO_S32 in_stride  = conv_data->nInStride;
	VO_U8 *y         = conv_data->pInBuf[0];
	//VO_U8 *u         = conv_data->pInBuf[1]; 
	//VO_U8 *v         = conv_data->pInBuf[2];
	//VO_S32 in_width   = conv_data->nInWidth;
	//VO_S32 in_height  = conv_data->nInHeight;
	VO_S32 out_width  = conv_data->nOutWidth;
	VO_S32 out_height = conv_data->nOutHeight; 
	VO_U8 *out_buf   = conv_data->pOutBuf[0]; 
	VO_S32 out_stride = conv_data->nOutStride; 
	//VO_U8 *mb_skip   = conv_data->mb_skip;

	//add end	
	do{
		a4 = out_width;
		do {
			VO_S32 a0, a1, a2, a3;
			//0
			a0 = *((VO_U32*)y);
			y += 4;
			a1 = *((VO_U32*)y);
			y += 4;
			a2 = *((VO_U32*)y);
			y += 4;

			a3 = ((a0>>3)&0x1F) | ((a0>>5)&0x7E0) | ((a0>>8)&0xF800);
			a3 |= (((a0>>11)&0x1F0000) | ((a1<<19)&0x7E00000) | ((a1<<16)&0xF8000000));
			*((VO_S32*)(out_buf)) = a3;

			out_buf += 4;
			a0 = ((a1>>19)&0x1F) | ((a1>>21)&0x7E0) | ((a2<<8)&0xF800);
			a0 |= (((a2<<5)&0x1F0000) | ((a2<<3)&0x7E00000) | (a2&0xF8000000));

			*((VO_S32*)(out_buf)) = a0;
			out_buf += 4;
		}while((a4-=4)>0);

		out_buf -= (out_width<<1);
		out_buf += out_stride;

		y -= out_width*3;
		y += in_stride;

	}while(--out_height != 0);

	return VO_ERR_NONE;
}

VOCCRETURNCODE Rgb16_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vy;//vx = 0, 
	const VO_U32 in_stride = conv_data->nInStride, out_stride = conv_data->nOutStride, out_width = (conv_data->nOutWidth<<1), out_height = conv_data->nOutHeight;
	VO_U8 *out_buf = conv_data->pOutBuf[0] , *psrc = conv_data->pInBuf[0];

	for(vy = 0; vy < out_height; vy++) {
		memcpy(out_buf, psrc, out_width);
		out_buf += out_stride;
		psrc += in_stride;
	}
	return VO_ERR_NONE;
}

//Resize
#ifdef COLCONVENABLE
VOCCRETURNCODE RGB888PlanarToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst, *out_buf;
	VO_U8 *r, *g, *b;
	VO_S32 i, j, width, height;
#ifndef VOARMV7
	VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
	VO_S32 mb_height = (conv_data->nInHeight + 15)/16;
#else
	VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
	VO_S32 block_height = (conv_data->nOutHeight) / 8;
	VO_S32 height_remainder = 0;
#endif
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 in_stride, uin_stride, vin_stride;

	const VO_S32 out_stride = conv_data->nOutStride;
	//const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
	//const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

	r = conv_data->pInBuf[0];
	g = conv_data->pInBuf[1];
	b = conv_data->pInBuf[2];

	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInStride;
	vin_stride = conv_data->nInStride;	

	x_scale_tab = x_resize_tab;
	y_scale_tab = y_resize_tab;
	dst_start = out_buf = conv_data->pOutBuf[0];

#ifdef VOARMV7
	for(j = 0; j < block_height; j++){
		height = y_scale_tab[0];

		dst = dst_start;
		for(i = 0; i < block_width; i++){
			width = x_scale_tab[0];

			if((width != 0) && (height != 0)){
				rgb888_to_s_rgb16_8x8_armv7(width, height, r, g, b, 
					dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			}

			dst += 2*width;
			x_scale_tab += (width * 3/2 + 1);
		}

		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);
		dst_start += (height * out_stride);
	}

	height_remainder = conv_data->nOutHeight-block_height*8;
	if (height_remainder){
		height = y_scale_tab[0];
		dst = dst_start;
		for (i = 0; i < block_width;i++){
			width = x_scale_tab[0];

			if((width != 0) && (height != 0)){
				rgb888_to_s_rgb16_8x2n_armv7(width, height, r, g, b, 
					dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			}

			dst += 2*width;
			x_scale_tab += (width * 3/2 + 1);
		}
	}
#else
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];

			if((width != 0) && (height != 0))
				rgb888_to_rgb565_mb_s(width, height, r, g, b, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);

		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}
#endif

	return VO_ERR_NONE;
}
#endif //COLCONVENABLE

VOCCRETURNCODE Rgb16_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst, *src;
	VO_U8 *mb_skip;
	VO_S32 i, j, width, height, skip = 0;
	VO_S32 mb_width  = (conv_data->nInWidth + 15) / 16;
	VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 in_stride;

	const VO_S32 out_stride = conv_data->nOutStride;

	in_stride  = conv_data->nInStride;
	x_scale_tab = (VO_S32 *)x_resize_tab;
	y_scale_tab = (VO_S32 *)y_resize_tab;
	dst_start = conv_data->pOutBuf[0];

	mb_skip = conv_data->mb_skip;
	src     = conv_data->pInBuf[0];

	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_rgb565_mb_s(width, height, src, dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);

		x_scale_tab = (VO_S32 *)x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE cc_s_rgb24_disable(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst;
	VO_S32 i, j, width, height, skip = 0;
	VO_U8 *y         = conv_data->pInBuf[0];
	//VO_U8 *u         = conv_data->pInBuf[1]; 
	//VO_U8 *v         = conv_data->pInBuf[2];
	VO_S32 in_width   = conv_data->nInWidth;
	VO_S32 in_height  = conv_data->nInHeight;
	//VO_S32 out_width  = conv_data->nOutWidth;
	//VO_S32 out_height = conv_data->nOutHeight; 
	VO_U8 *out_buf   = conv_data->pOutBuf[0]; 
	VO_S32 out_stride = conv_data->nOutStride; 
	VO_U8 *mb_skip   = conv_data->mb_skip;
	VO_S32 mb_width   = (in_width+15)/16;
	VO_S32 mb_height  = (in_height+15)/16;
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 in_stride;

	in_stride  = conv_data->nInStride;

	x_scale_tab = x_resize_tab;
	y_scale_tab = y_resize_tab;
	dst_start = out_buf;

	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_rgb24_mb_s(width, height, y, dst, in_stride, 
				out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);

		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}

	return VO_ERR_NONE;
}

//Rotation
#ifdef COLCONVENABLE
static VOCCRETURNCODE RGB888PlanarToRGB16_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	//const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf;
	VO_U8 *psrcR, *psrcG, *psrcB;

	VO_S32 step_y, step_x, out_stride = conv_data->nOutStride;

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_DISABLE){/*disable rotation*/
		step_y = 2;
		step_x = out_stride;
		start_outbuf = conv_data->pOutBuf[0];
	}else if (conv_data->nRotationType == ROTATION_180){/*rotation180*/
		step_y = -2;
		step_x = -out_stride;
		start_outbuf = conv_data->pOutBuf[0];
	}else if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		step_y = -out_stride;
		step_x = 2;
		start_outbuf = conv_data->pOutBuf[0] - (out_width - 1)* step_y;
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		step_x = -2;
		step_y = out_stride;
		start_outbuf = conv_data->pOutBuf[0] + (out_height<<1) - 2 ;
	}

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy*in_stride;
		psrcR = conv_data->pInBuf[0] + vx;
		psrcG = conv_data->pInBuf[1] + vx;
		psrcB = conv_data->pInBuf[2] + vx;

		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1, a2;

			a0 = psrcR[vx];
			a1 = psrcG[vx];
			a2 = psrcB[vx];

			a0 = ((a0 << 8) & 0xf800) | ((a1 << 3) & 0x7e0) | (a2 >> 3);
			*((short*)outbuf) = (unsigned short)a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
	return VO_ERR_NONE;
}

VOCCRETURNCODE RGB888PlanarToRGB16_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{

	VO_U32 vx = 0, vy = 0;
	const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	const VO_S32 out_stride = conv_data->nOutStride; 
	const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
	const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
	VO_U8 *psrc_r, *psrc_g, *psrc_b, *out_buf, *start_out_buf=NULL;
	VO_S32 *param = cc_hnd->yuv2rgbmatrix;
	VO_S32 step_x=0, step_y=0;

	if((out_width < 16)||(out_height < 16))
		goto Normal_convert;

	if(conv_data->nRotationType == ROTATION_90L){
		RGB888PLANARTORGB16_MB = RGB888PlanarToRGB16_MB_rotation_90L;
		start_out_buf = conv_data->pOutBuf[0] + (out_width - 1)* out_stride;
		step_x = -(out_stride << 4);
		step_y = 32;
	}else if(conv_data->nRotationType == ROTATION_90R){
		RGB888PLANARTORGB16_MB = RGB888PlanarToRGB16_MB_rotation_90R;
		start_out_buf = conv_data->pOutBuf[0] + (out_height << 1) - 4;
		step_x = (out_stride << 4);
		step_y = -32;
	}else if(conv_data->nRotationType == ROTATION_180){
		RGB888PLANARTORGB16_MB = RGB888PlanarToRGB16_MB_rotation_180;
		start_out_buf = conv_data->pOutBuf[0] + (out_height* out_stride) - 4;
		step_x = -32;
		step_y = -(out_stride << 4);
	}

	//else{
	//	RGB888PLANARTORGB16_MB = RGB888PlanarToRGB16_MB_rotation_no;
	//	start_out_buf = conv_data->pOutBuf[0];
	//	step_x = 32;
	//	step_y = (out_stride << 4);
	//}

	/*prepare resize*/	
	for(vy = 16; vy <= out_height; vy+=16) {
		VO_S32 offset = (vy - 16)*in_stride;

		psrc_r = conv_data->pInBuf[0] + offset;
		psrc_g = conv_data->pInBuf[1] + offset;
		psrc_b = conv_data->pInBuf[2] + offset;

		out_buf = start_out_buf;	
		for(vx = 16; vx <= out_width; vx+=16) {
			RGB888PLANARTORGB16_MB(psrc_r, psrc_g, psrc_b, in_stride, out_buf, out_stride, param, is420Planar, isRGB565);
			psrc_r += 16;
			psrc_g += 16;
			psrc_b += 16;
			out_buf += step_x;

		}
		start_out_buf += step_y;
	}	


	vx = vx - 16;
	vy = vy - 16;

Normal_convert:

	if(vx != out_width){/*not multiple of 16*/

		ClrConvData tmp_conv_data;
		tmp_conv_data = *conv_data;

		tmp_conv_data.nOutWidth = out_width - vx;
		tmp_conv_data.nOutHeight = vy;

		tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vx;
		tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vx>>1);
		tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vx>>1);
		if(conv_data->nRotationType == ROTATION_90L){
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
		}else if(conv_data->nRotationType == ROTATION_90R){
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + out_stride *(vx+1) -(vy<<1) ;
		}else if(conv_data->nRotationType == ROTATION_180){
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + ((out_width - vx)<<1);
		}else{
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vx<<1);
		}

		RGB888PlanarToRGB16_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);

	}

	if(vy != out_height){
		ClrConvData tmp_conv_data;
		tmp_conv_data = *conv_data;

		tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vy*in_stride;
		tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + vy*in_stride;
		tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + vy*in_stride;

		if(conv_data->nRotationType == ROTATION_90L){
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vy<<1);
		}else if(conv_data->nRotationType == ROTATION_90R){
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
		}else if(conv_data->nRotationType == ROTATION_180){
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (out_height - vy)*out_stride;
		}else{
			tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + vy*out_stride;
		}

		tmp_conv_data.nOutWidth = out_width;
		tmp_conv_data.nOutHeight = out_height - vy;
		RGB888PlanarToRGB16_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE RGB888PlanarToRGB16_resize_Rotation_90L(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst, *out_buf;
	VO_U8 *r, *g, *b;
	VO_S32 i, j, width, height;
	VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
	VO_S32 mb_height = (conv_data->nInHeight + 15)/16;
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 red_stride, green_stride, blue_stride;

	const VO_S32 out_stride = conv_data->nOutStride;
	//const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
	//const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

	r = conv_data->pInBuf[0];
	g = conv_data->pInBuf[1];
	b = conv_data->pInBuf[2];

	red_stride  = conv_data->nInStride;
	green_stride = conv_data->nInStride;
	blue_stride = conv_data->nInStride;	

	x_scale_tab = x_resize_tab;
	y_scale_tab = y_resize_tab;
	dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutWidth - 1)* out_stride;

	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if((width != 0) && (height != 0)){
				rgb888_to_rgb565_mb_rotation_90l_s(width, height, r, g, b, 
					dst, red_stride, out_stride, x_scale_tab+1, y_scale_tab+1,green_stride, blue_stride);
			}
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);

		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE RGB888PlanarToRGB16_resize_Rotation_90R(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst, *out_buf;
	VO_U8 *r, *g, *b;
	VO_S32 i, j, width, height;
	VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
	VO_S32 mb_height = (conv_data->nInHeight + 15)/16;
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 red_stride, green_stride, blue_stride;

	const VO_S32 out_stride = conv_data->nOutStride;
	//const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
	//const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

	r = conv_data->pInBuf[0];
	g = conv_data->pInBuf[1];
	b = conv_data->pInBuf[2];

	red_stride  = conv_data->nInStride;
	green_stride = conv_data->nInStride;
	blue_stride = conv_data->nInStride;

	x_scale_tab = x_resize_tab;
	y_scale_tab = y_resize_tab;
	dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 1) - 4;

	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];

			if((width != 0) && (height != 0))
				rgb888_to_rgb565_mb_rotation_90r_s(width, height, r, g, b, 
				dst, red_stride, out_stride, x_scale_tab+1, y_scale_tab+1, green_stride, blue_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);

		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}

	return VO_ERR_NONE;

}

VOCCRETURNCODE RGB888PlanarToRGB16_resize_Rotation_180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst, *out_buf;
	VO_U8 *r, *g, *b;
	VO_S32 i, j, width, height;
	VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
	VO_S32 mb_height = (conv_data->nInHeight + 15)/16;
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 red_stride, green_stride, blue_stride;

	const VO_S32 out_stride = conv_data->nOutStride;
	//const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
	//const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

	r = conv_data->pInBuf[0];
	g = conv_data->pInBuf[1];
	b = conv_data->pInBuf[2];

	red_stride  = conv_data->nInStride;
	green_stride = conv_data->nInStride;
	blue_stride = conv_data->nInStride;

	x_scale_tab = x_resize_tab;
	y_scale_tab = y_resize_tab;

	height = y_scale_tab[0];
	dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - height);

	for(j = 0; j < mb_height; j++){

		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];

			if((width != 0) && (height != 0))
				rgb888_to_rgb565_mb_rotation_180_s(width, height, r, g, b, 
				dst + out_stride - (width<<1), red_stride, out_stride, x_scale_tab+1, y_scale_tab+1, green_stride, blue_stride);
			x_scale_tab += (width * 3 + 1);
			dst -= (width<<1);
		}
		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);

		height = y_scale_tab[0];
		dst_start -= (height * out_stride);
	}

	return VO_ERR_NONE;

}
#endif //COLCONVENABLE

VOCCRETURNCODE Rgb16_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//VO_U32 vx = 0;
	const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 step_y=0, step_x=0, out_stride = conv_data->nOutStride;
	VO_U8 *dst_start=NULL, *dst=NULL;
	VO_U8 *src = conv_data->pInBuf[0];
	VO_U8 *mb_skip = conv_data->mb_skip;
	VO_S32 i, j, width, height, skip = 0;
	VO_S32 mb_width  = (conv_data->nInWidth + 15) / 16;
	VO_S32 mb_height = (conv_data->nInHeight+ 15) / 16;
	VO_S32 *x_scale_tab = (VO_S32 *)x_resize_tab;
	VO_S32	*y_scale_tab = (VO_S32 *)y_resize_tab;

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		RGB565TORGB565_MB = cc_rgb565_mb_s_l90;
		step_x = -out_stride;
		step_y = 1;
		dst_start = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		RGB565TORGB565_MB = cc_rgb565_mb_s_r90;
		step_x = out_stride;
		step_y = -1;
		dst_start = conv_data->pOutBuf[0] + (out_height << 1) - 4;
	}

	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				RGB565TORGB565_MB(width, height, src, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst += width*step_x;
		}
		dst_start += (height << 1)*step_y;

		x_scale_tab = (VO_S32 *)x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}

	return VO_ERR_NONE;
}



VOCCRETURNCODE Rgb16_noresize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	const VO_U32 in_stride = conv_data->nInStride >>2, out_stride = conv_data->nOutStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_U32 *p_src0=NULL;
	VO_U32 *p_src1=NULL;
	VO_U16 *p_src0_16bit = NULL;
	VO_U16 *p_src1_16bit = NULL; 
	VO_U8 *start_out=NULL;
	VO_S32 step_x=0, step_y=0;
	VO_U32 vx, vy;


	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		step_x = 4;
		step_y = -((VO_S32)out_stride);
		start_out = conv_data->pOutBuf[0] + out_stride * (out_width - 1);
		p_src0 = (VO_U32 *)conv_data->pInBuf[0];
		p_src1 = p_src0 + in_stride;
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		step_x = -4;
		step_y = out_stride;
		start_out = conv_data->pOutBuf[0] + (out_height<<1) - 4;
		p_src1 = (VO_U32 *)conv_data->pInBuf[0];
		p_src0 = p_src1 + in_stride;
	}

	vy = out_height&0xfffffffe;
	do{
		VO_U8 *out_dst = start_out;

		vx = out_width&0xfffffffc;
		do{
			VO_U32 a, b, c, d, e, f;
			a = *(p_src0++);			//2p
			b = *(p_src1++);
			c = *(p_src0++);			//4p
			d = *(p_src1++);

			e = a&0xFFFF;
			f = (b&0xFFFF)<<16;
			e = e|f;//
			(*(VO_U32 *)out_dst) = e;	//0
			out_dst += step_y;

			e = (a>>16);
			f = b&0xFFFF0000;


			e = e|f;
			(*(VO_U32 *)out_dst) = e;	//1
			out_dst += step_y;
			


			e = c&0xFFFF;
			f = (d&0xFFFF)<<16;
			e = e|f;
			(*(VO_U32 *)out_dst) = e;	//2
			out_dst += step_y;

			
			e = (c>>16);
			f = d&0xFFFF0000;


			e = e|f;
			(*(VO_U32 *)out_dst) = e;	//3
			out_dst += step_y;

		}while(vx-=4);

		vx = out_width&0x3;
		/*while(vx){
			VO_U32 a, b;
			a = (VO_U16)*((VO_U16 *)p_src0);
			b = (VO_U16)*((VO_U16 *)p_src1);

			((VO_U16 *)p_src0)++;
			((VO_U16 *)p_src1)++;
			
			a |= (b << 16);
			(*(VO_U32 *)out_dst) = a;
			out_dst += step_y;
			vx--;
		}
		p_src0 -= (out_width>>1);
		p_src1 -= (out_width>>1);

		p_src0 += (in_stride<<1);
		p_src1 += (in_stride<<1);
		start_out += step_x; 
	}while(vy-=2);

	if(vy){
		VO_U8 *out_dst = start_out;
		vx = out_width;
		do{
			VO_U32 a;
			a = (VO_U16)*((VO_U16 *)p_src0);
			(*(VO_U16 *)out_dst) = (VO_U16)a;
			out_dst += step_y;

		}while(--vx);
	}*/
			while(vx){
			VO_U32 a, b;
			
			p_src0_16bit = (VO_U16 *)p_src0;
			p_src1_16bit = (VO_U16 *)p_src1;
			a = (VO_U16)*(p_src0_16bit);
			b = (VO_U16)*(p_src1_16bit);

			p_src0_16bit++;
			p_src1_16bit++;
			
			a |= (b << 16);
			(*(VO_U32 *)out_dst) = a;
			out_dst += step_y;
			vx--;
		}
		p_src0 -= (out_width>>1);
		p_src1 -= (out_width>>1);

		p_src0 += (in_stride<<1);
		p_src1 += (in_stride<<1);
		start_out += step_x; 
	}while(vy-=2);

	if(vy){
		VO_U8 *out_dst = start_out;
		vx = out_width;
		do{
			VO_U32 a;
			a = (VO_U16)*(p_src0_16bit);
			(*(VO_U16 *)out_dst) = (VO_U16)a;
			out_dst += step_y;

		}while(--vx);
	}
	
	return VO_ERR_NONE;
}

VOCCRETURNCODE cc_rgb24_l90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_S32 a4;
	VO_S32 in_stride  = conv_data->nInStride;
	VO_U8 *y         = conv_data->pInBuf[0];
	//VO_U8 *u         = conv_data->pInBuf[1]; 
	//VO_U8 *v         = conv_data->pInBuf[2];
	//VO_S32 in_width   = conv_data->nInWidth;
	//VO_S32 in_height  = conv_data->nInHeight;
	VO_S32 out_width  = conv_data->nOutWidth;
	VO_S32 out_height = conv_data->nOutHeight; 
	VO_U8 *out_buf   = conv_data->pOutBuf[0]; 
	VO_S32 out_stride = conv_data->nOutStride; 
	//VO_U8 *mb_skip   = conv_data->mb_skip;
	VO_U8 *psrc      = y + in_stride;


	out_buf += (out_width - 1) * out_stride;
	do{
		a4 = out_width;
		do {
			VO_S32 a0, a1, a2, a3, a5, a6, a7;
			//0
			a0 = *((VO_U32*)y);
			y += 4;
			a1 = *((VO_U32*)y);
			y += 4;
			a2 = *((VO_U32*)y);
			y += 4;

			a5 = *((VO_U32*)psrc);
			psrc += 4;
			a6 = *((VO_U32*)psrc);
			psrc += 4;
			a7 = *((VO_U32*)psrc);
			psrc += 4;

			a3 = ((a0>>3)&0x1F) | ((a0>>5)&0x7E0) | ((a0>>8)&0xF800);
			a3 |= (((a5<<13)&0x1F0000) | ((a5<<11)&0x7E00000) | ((a5<<8)&0xF8000000));
			*((VO_S32*)(out_buf)) = a3;

			out_buf -= out_stride;
			a3 = ((a0>>27)&0x1F) | ((a1<<3)&0x7E0) | (a1&0xF800);
			a3 |= (((a5>>11)&0x1F0000) | ((a6<<19)&0x7E00000) | ((a6<<16)&0xF8000000));

			*((VO_S32*)(out_buf)) = a3;
			out_buf -= out_stride;

			a3 = ((a1>>19)&0x1F) | ((a1>>21)&0x7E0) | ((a2<<8)&0xF800);
			a3 |= (((a6>>3)&0x1F0000) | ((a6>>5)&0x7E00000) | ((a7<<24)&0xF8000000));
			*((VO_S32*)(out_buf)) = a3;

			out_buf -= out_stride;
			a3 = ((a2>>11)&0x1F) | ((a2>>13)&0x7E0) | ((a2>>16)&0xF800);
			a3 |= (((a7<<5)&0x1F0000) | ((a7<<3)&0x7E00000) | (a7&0xF8000000));

			*((VO_S32*)(out_buf)) = a3;
			out_buf -= out_stride;
		}while((a4-=4)>0);

		out_buf += (out_width * out_stride);
		out_buf += 4;

		y -= out_width*3;
		y += (in_stride<<1);
		psrc = y + in_stride;

	}while((out_height-=2)>0);

	return VO_ERR_NONE;
}

VOCCRETURNCODE cc_rgb24_r90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_S32 a4;
	VO_S32 in_stride  = conv_data->nInStride;
	VO_U8 *y         = conv_data->pInBuf[0];
	//VO_U8 *u         = conv_data->pInBuf[1]; 
	//VO_U8 *v         = conv_data->pInBuf[2];
	//VO_S32 in_width   = conv_data->nInWidth;
	//VO_S32 in_height  = conv_data->nInHeight;
	VO_S32 out_width  = conv_data->nOutWidth;
	VO_S32 out_height = conv_data->nOutHeight; 
	VO_U8 *out_buf   = conv_data->pOutBuf[0]; 
	VO_S32 out_stride = conv_data->nOutStride; 
	//VO_U8 *mb_skip   = conv_data->mb_skip;
	VO_U8 *psrc;

	psrc = y + in_stride;

	out_buf += (out_height << 1) - 4;

	do{
		a4 = out_width;
		do {
			VO_S32 a0, a1, a2, a3, a5, a6, a7;
			//0
			a5 = *((VO_U32*)y);
			y += 4;
			a6 = *((VO_U32*)y);
			y += 4;
			a7 = *((VO_U32*)y);
			y += 4;

			a0 = *((VO_U32*)psrc);
			psrc += 4;
			a1 = *((VO_U32*)psrc);
			psrc += 4;
			a2 = *((VO_U32*)psrc);
			psrc += 4;

			a3 = ((a0>>3)&0x1F) | ((a0>>5)&0x7E0) | ((a0>>8)&0xF800);
			a3 |= (((a5<<13)&0x1F0000) | ((a5<<11)&0x7E00000) | ((a5<<8)&0xF8000000));
			*((VO_S32*)(out_buf)) = a3;

			out_buf += out_stride;
			a3 = ((a0>>27)&0x1F) | ((a1<<3)&0x7E0) | (a1&0xF800);
			a3 |= (((a5>>11)&0x1F0000) | ((a6<<19)&0x7E00000) | ((a6<<16)&0xF8000000));

			*((VO_S32*)(out_buf)) = a3;
			out_buf += out_stride;

			a3 = ((a1>>19)&0x1F) | ((a1>>21)&0x7E0) | ((a2<<8)&0xF800);
			a3 |= (((a6>>3)&0x1F0000) | ((a6>>5)&0x7E00000) | ((a7<<24)&0xF8000000));
			*((VO_S32*)(out_buf)) = a3;

			out_buf += out_stride;
			a3 = ((a2>>11)&0x1F) | ((a2>>13)&0x7E0) | ((a2>>16)&0xF800);
			a3 |= (((a7<<5)&0x1F0000) | ((a7<<3)&0x7E00000) | (a7&0xF8000000));

			*((VO_S32*)(out_buf)) = a3;
			out_buf += out_stride;
		}while((a4-=4)>0);

		out_buf -= (out_width * out_stride);
		out_buf -= 4;

		y -= out_width*3;
		y += (in_stride<<1);
		psrc = y + in_stride;

	}while((out_height-=2)>0);

	return VO_ERR_NONE;
}

VOCCRETURNCODE cc_s_rgb24_r90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst;
	VO_S32 i, j, width, height, skip = 0;
	VO_U8 *y         = conv_data->pInBuf[0];
	//VO_U8 *u         = conv_data->pInBuf[1]; 
	//VO_U8 *v         = conv_data->pInBuf[2];
	VO_S32 in_width   = conv_data->nInWidth;
	VO_S32 in_height  = conv_data->nInHeight;
	//VO_S32 out_width  = conv_data->nOutWidth;
	VO_S32 out_height = conv_data->nOutHeight; 
	VO_U8 *out_buf   = conv_data->pOutBuf[0]; 
	VO_S32 out_stride = conv_data->nOutStride; 
	VO_U8 *mb_skip   = conv_data->mb_skip;
	VO_S32 mb_width   = (in_width+15)/16;
	VO_S32 mb_height  = (in_height+15)/16;
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 in_stride;
	in_stride  = conv_data->nInStride;

	x_scale_tab = x_resize_tab;
	y_scale_tab = y_resize_tab;
	dst_start = out_buf + (out_height << 1) - 4;

	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_rgb24_mb_s_r90(width, height, y, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);

		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE cc_s_rgb24_l90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *dst_start, *dst;
	VO_S32 i, j, width, height, skip = 0;
	VO_U8 *y         = conv_data->pInBuf[0];
	//VO_U8 *u         = conv_data->pInBuf[1]; 
	//VO_U8 *v         = conv_data->pInBuf[2];
	VO_S32 in_width   = conv_data->nInWidth;
	VO_S32 in_height  = conv_data->nInHeight;
	VO_S32 out_width  = conv_data->nOutWidth;
	//VO_S32 out_height = conv_data->nOutHeight; 
	VO_U8 *out_buf   = conv_data->pOutBuf[0]; 
	VO_S32 out_stride = conv_data->nOutStride; 
	VO_U8 *mb_skip   = conv_data->mb_skip;
	VO_S32 mb_width   = (in_width+15)/16;
	VO_S32 mb_height  = (in_height+15)/16;
	VO_S32 *x_scale_tab, *y_scale_tab;
	VO_S32 in_stride;
	in_stride  = conv_data->nInStride;


	x_scale_tab = x_resize_tab;
	y_scale_tab = y_resize_tab;
	dst_start = out_buf + (out_width - 1) * out_stride;

	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_rgb24_mb_s_l90(width, height, y, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);

		x_scale_tab = x_resize_tab;
		y_scale_tab += (height * 3 + 1);
	}

	return VO_ERR_NONE;
}

#define ONEPART(a1, a2, a3, a4)  (((a1)*bb + (a2)*aa)*bb0 + ((a3)*bb + (a4)*aa)*aa0)>>20

#ifdef COLCONVENABLE

#define GETRGB(srcData, srcStride, dst, XTAB, YTAB, nMaxHeight) {\
	int ij;\
	a1 = 0;		\
	if(antialias_level < 3)	{			\
	for(ij=0; ij<5;ij++){   \
	int a2YTAB;\
	a2 = ij*3;                 \
	a2YTAB = YTAB[a2];              \
	if(a2YTAB>=nMaxHeight-1){a2YTAB = nMaxHeight-2;};         \
	psrc1 = srcData + a2YTAB * srcStride;                                                              \
	aa1 = YTAB[1+a2];                                                                                     \
	bb1 = YTAB[2+a2];                                                                                      \
	a0 = XTAB[0];                                                                                                  \
	if(a0>=srcStride-1){a0 = srcStride-2;};    \
	aa = XTAB[1];                                                                                                  \
	bb = XTAB[2];                                                                                                  \
	a1 += ((((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+srcStride]*bb + psrc1[a0+srcStride+1]*aa)*aa1)>>20)&0x0FF);       \
	XTAB += 3;};                   \
	if(antialias_level==1||antialias_level == 2){\
	dst = a1 / 5;                                                                              \
	}} \
	else{\
	int ki,li;\
	a0 = 0;aa=0;             \
	for(ki = 0;ki<15;ki++)	{  if(YTAB[ki]==-1 || YTAB[ki]>=nMaxHeight){break;};                       \
	for(li = 0;li<15;li++){                    \
	if(XTAB[li]==-1 || XTAB[li]>=srcStride ){break;}                   \
{a0+=(VO_U8)*(srcData + YTAB[ki] * srcStride + XTAB[li]); aa++;};} }              \
	if(aa>0) {dst = a0/aa; } else {dst = 0;};XTAB += 15;};\
}

VOCCRETURNCODE RGB888PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx, vy;
	VO_U32 in_stride = conv_data->nInStride;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 *y_taby;
	VO_S32 * pRGB;
	VO_S32 out_stride = conv_data->nOutStride;
	const VO_S32 nEach = 15;
	const VO_S32 antialias_level = conv_data->nAntiAliasLevel;
	//const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
	//const VO_U32 is420Planar =  (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

	VO_U8 *pRData = conv_data->pInBuf[0];
	VO_U8 *pGData = conv_data->pInBuf[1];
	VO_U8 *pBData = conv_data->pInBuf[2];

	VO_S32 nR=0, nG=0, nB=0;

	y_taby  = y_resize_tab;

	for (vy = 0; vy < out_height; vy++){

		VO_S32 *x_tabr = x_resize_tab;
		VO_S32	*x_tabg = x_resize_tab;
		VO_S32 *x_tabb = x_resize_tab;

		pRGB = (VO_S32 *)(conv_data->pOutBuf[0] + vy*out_stride);

		for (vx = 0; vx < out_width; vx += 2){

			VO_U32 a0, a1, a2;
			VO_S32 b0, b1;
			VO_S32 aa, bb, aa1, bb1;
			VO_U8 *psrc1;

			GETRGB(pRData, in_stride, nR, x_tabr, y_taby, conv_data->nInHeight)
			GETRGB(pGData, in_stride, nG, x_tabg, y_taby, conv_data->nInHeight)
			GETRGB(pBData, in_stride, nB, x_tabb, y_taby, conv_data->nInHeight)

			b0 = ((nR << 8) & 0xf800) | ((nG << 3) & 0x7e0) | (nB >> 3);

			GETRGB(pRData, in_stride, nR, x_tabr, y_taby, conv_data->nInHeight)
			GETRGB(pGData, in_stride, nG, x_tabg, y_taby, conv_data->nInHeight)
			GETRGB(pBData, in_stride, nB, x_tabb, y_taby, conv_data->nInHeight)

			b1 = ((nR << 8) & 0xf800) | ((nG << 3) & 0x7e0) | (nB >> 3);

			b0 |= (b1 << 16);

			*pRGB++ = b0;
		}
		y_taby  += nEach;
	}

	return VO_ERR_NONE;

}

#endif //COLCONVENABLE

VOCCRETURNCODE rgb565_resize_anti_aliasing(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = (conv_data->nInStride>>1) ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_S32 out_stride = (conv_data->nOutStride>>1);

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U16 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		psrc = (VO_U16*)conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = (VO_U16*)conv_data->pOutBuf[0] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_U32 a0, a1, a2, a3, a4, r, g, b;
			VO_S32 aa, bb, aa1, bb1;
			VO_U16 *psrc1;

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];


			//0
			a1 = psrc[a0];
			a2 = psrc[a0+1];
			a3 = psrc[a0+in_stride];
			a4 = psrc[a0+in_stride+1];

			r = ONEPART(a1>>11, a2>>11, a3>>11, a4>>11);
			g = ONEPART((a1&0x7E0)>>5, (a2&0x7E0)>>5, (a3&0x7E0)>>5, (a4&0x7E0)>>5);
			b = ONEPART(a1&0x1F, a2&0x1F, a3&0x1F, a4&0x1F);

			//1
			x_tab += 3;	
			psrc1 = (VO_U16*)conv_data->pInBuf[0] + y_tab[3] * in_stride;
			aa1 = y_tab[4];
			bb1 = y_tab[5];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = psrc1[a0];
			a2 = psrc1[a0+1];
			a3 = psrc1[a0+in_stride];
			a4 = psrc1[a0+in_stride+1];

			r += ONEPART(a1>>11, a2>>11, a3>>11, a4>>11);
			g += ONEPART((a1&0x7E0)>>5, (a2&0x7E0)>>5, (a3&0x7E0)>>5, (a4&0x7E0)>>5);
			b += ONEPART(a1&0x1F, a2&0x1F, a3&0x1F, a4&0x1F);		
			//2
			x_tab += 3;	
			psrc1 = (VO_U16*)conv_data->pInBuf[0] + y_tab[6] * in_stride;
			aa1 = y_tab[7];
			bb1 = y_tab[8];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = psrc1[a0];
			a2 = psrc1[a0+1];
			a3 = psrc1[a0+in_stride];
			a4 = psrc1[a0+in_stride+1];

			r += ONEPART(a1>>11, a2>>11, a3>>11, a4>>11);
			g += ONEPART((a1&0x7E0)>>5, (a2&0x7E0)>>5, (a3&0x7E0)>>5, (a4&0x7E0)>>5);
			b += ONEPART(a1&0x1F, a2&0x1F, a3&0x1F, a4&0x1F);

			//3
			x_tab += 3;	
			psrc1 = (VO_U16*)conv_data->pInBuf[0] + y_tab[9] * in_stride;
			aa1 = y_tab[10];
			bb1 = y_tab[11];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a1 = psrc1[a0];
			a2 = psrc1[a0+1];
			a3 = psrc1[a0+in_stride];
			a4 = psrc1[a0+in_stride+1];

			r += ONEPART(a1>>11, a2>>11, a3>>11, a4>>11);
			g += ONEPART((a1&0x7E0)>>5, (a2&0x7E0)>>5, (a3&0x7E0)>>5, (a4&0x7E0)>>5);
			b += ONEPART(a1&0x1F, a2&0x1F, a3&0x1F, a4&0x1F);

			//4
			x_tab += 3;	
			psrc1 = (VO_U16*)conv_data->pInBuf[0] + y_tab[12] * in_stride;
			aa1 = y_tab[13];
			bb1 = y_tab[14];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a1 = psrc1[a0];
			a2 = psrc1[a0+1];
			a3 = psrc1[a0+in_stride];
			a4 = psrc1[a0+in_stride+1];

			r += ONEPART(a1>>11, a2>>11, a3>>11, a4>>11);
			g += ONEPART((a1&0x7E0)>>5, (a2&0x7E0)>>5, (a3&0x7E0)>>5, (a4&0x7E0)>>5);
			b += ONEPART(a1&0x1F, a2&0x1F, a3&0x1F, a4&0x1F);


			r /= 5;
			g /= 5;
			b /= 5;
			a1 = (r<<11) | ((g<<5)&0x7e0) | (b&0x1f);
			*(pdst++) = (VO_U16)a1;
			x_tab += 3;	

		}		
		y_tab += 15;
	}

	return VO_ERR_NONE;
}
