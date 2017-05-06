/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "voMpegEncPreProcess.h"
#include "voMpegEncTable.h"
#include "voType.h"
#include <string.h>

/*
	VO_RT_DISABLE
	VO_RT_90L	
	VO_RT_90R    
	VO_RT_180	
	VO_RT_FLIP_Y
	VO_RT_FLIP_X
*/
const YUV420_TO_YUV420_PTR yuv420_to_yuv420_all[6] = 
{
	YUV420toYUV420_C, YUV420toYUV420_C, YUV420toYUV420_C,
	YUV420toYUV420_C, YUV420toYUV420_C, YUV420toYUV420_C		
};

const IMG_TO_YUV420_PTR uyvy_to_yuv420_all[6] = 
{
	UYVYtoYUV420_C, UYVYtoYUV420_C, UYVYtoYUV420_C,
	uyvy_to_yuv420_rt180, uyvy_to_yuv420_rt180, uyvy_to_yuv420_rt180
		
};

const IMG_TO_YUV420_PTR yuyv_to_yuv420_all[6] = 
{
	YUYVtoYUV420_C, YUYVtoYUV420_C, YUYVtoYUV420_C,
	YUYVtoYUV420RT180_C, YUYVtoYUV420RT180_C, YUYVtoYUV420RT180_C
		
};

const IMG_TO_YUV420_PTR rgb565_to_yuv420_all[6] = 
{
	RGB565toYUV420Disable_C, RGB565toYV12RT90L_C, RGB565toYUV420Disable_C,
	RGB565toYUV420RT180_C, RGB565toYUV420FilpY_C, RGB565toYUV420FilpY_C
		
};

const IMG_TO_YUV420_PTR rgb32_to_yuv420_all[6] = //TBD
{
	RGB32toYUV420Disable_C, RGB32toYUV420Disable_C, RGB32toYUV420Disable_C,
	RGB32toYUV420Disable_C, RGB32toYUV420Disable_C, RGB32toYUV420Disable_C
		
};

const IMG_TO_YUV420_PTR vyuy2_to_yuv420_all[6] = 
{
	VYUY2toYUV420Disable_C, VYUY2toYUV420RT180_C, VYUY2toYUV420RT180_C,
	VYUY2toYUV420RT180_C, VYUY2toYUV420RT180_C, VYUY2toYUV420RT180_C
		
};

const YUV420PACK_TO_YUV420_PTR yuv420_pack_2_all[6] = 
{
	YUV420PacktoYUV420_C,YUV420PacktoYUV420_C,YUV420PacktoYUV420_C,
	YUV420PacktoYUV420_C,YUV420PacktoYUV420_C,YUV420PacktoYUV420_C
};


VO_VOID YUV420toYUV420_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				            VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				            VO_U8 * y_src, VO_U8 * u_src, VO_U8 * v_src,
				            VO_S32 y_src_stride, VO_S32 uv_src_stride,
				            VO_S32 width, VO_S32 height)
{
	VO_S32 half_width = width >>1;
	VO_S32 half_height = height >>1;
	VO_S32 i;

    for (i = half_height; i>0; i--) {

        memcpy(y_dst, y_src, width);
        y_src += y_src_stride;
		y_dst += y_dst_stride;

        memcpy(y_dst, y_src, width);
        y_src += y_src_stride;
		y_dst += y_dst_stride;

		memcpy(u_dst, u_src, half_width);
		memcpy(v_dst, v_src, half_width);
		u_src += uv_src_stride;
		u_dst += uv_dst_stride;		
		v_src += uv_src_stride;
		v_dst += uv_dst_stride;
	}
}


VO_VOID UYVYtoYUV420_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				 VO_U8 * uyvy_src,	VO_S32 width, VO_S32 height)
{
	VO_U8 * src0 = uyvy_src;
	VO_U8 * src1 = src0 + width*2;
	VO_U8 *dst_y1 = y_dst + y_dst_stride;
	
	VO_S32 i, j;
	
	for(j = 0; j < height; j += 2){
		for(i = 0; i < width; i += 2){

			VO_U32 u0, u1, v0, v1;
			u0 = *(src0++);
			*(y_dst++) = *(src0++);
			v0 = *(src0++);
			*(y_dst++) = *(src0++);

			u1 = *(src1++);
			*(dst_y1++) = *(src1++);
			v1 = *(src1++);
			*(dst_y1++) = *(src1++);

			*(u_dst++) = (VO_U8)((u0 + u1 + 1)/2);
			*(v_dst++) = (VO_U8)((v0 + v1 + 1)/2);
		}
		src0 += width*2;
		src1 = src0 + width*2;
		y_dst -= width;
		y_dst += y_dst_stride*2;
		dst_y1 = y_dst + y_dst_stride;
		
		u_dst -= width/2;
		u_dst += uv_dst_stride;
		v_dst -= width/2;
		v_dst += uv_dst_stride;
	}
}

VO_VOID YUYVtoYUV420_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				 VO_U8 * yuyv_src, VO_S32 width, VO_S32 height)
{
	VO_U8 * src0 = yuyv_src;
	VO_U8 * src1 = src0 + (width<<1);
	
	VO_S32 i, j;
	
	for(j = 0; j < height; j += 2){
		for(i = 0; i < width; i += 4){
			VO_U32 u0, v0, y0, y1;
			y0 = *(src0++);
			u0 = *(src0++);
			y0 |= (*(src0++)<<8);
			v0 = *(src0++);

			y1 = *(src1++);
			u0 += *(src1++);
			y1 |= (*(src1++)<<8);
			v0 += *(src1++);

			*(u_dst++) = (VO_U8)((u0 + 1)>>1);
			*(v_dst++) = (VO_U8)((v0 + 1)>>1);

			y0 |= (*(src0++)<<16);
			u0 = *(src0++);
			y0 |= (*(src0++)<<24);
			v0 = *(src0++);

			y1 |= (*(src1++)<<16);
			u0 += *(src1++);
			y1 |= (*(src1++)<<24);
			v0 += *(src1++);

			*(u_dst++) = (VO_U8)((u0 + 1)>>1);
			*(v_dst++) = (VO_U8)((v0 + 1)>>1);

			*((VO_U32 *)y_dst) = y0;
			*((VO_U32 *)(y_dst + y_dst_stride)) = y1;
			y_dst += 4;

		}
		src0 += (width<<1);
		src1 = src0 + (width<<1);
		y_dst -= width;
		y_dst += (y_dst_stride<<1);
		
		u_dst -= (width>>1);
		u_dst += uv_dst_stride;
		v_dst -= (width>>1);
		v_dst += uv_dst_stride;
	}
}

VO_VOID UYVYtoYUV420RT180_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						 VO_U8 * uyvy_src,	VO_S32 width, VO_S32 height)
{
	VO_U32 * src0 = (VO_U32 *)uyvy_src;
	VO_U32 * src1 = (VO_U32 *)(uyvy_src + (width<<1));

	VO_S32 i, j;
	y_dst += (height * y_dst_stride) - 4 - (y_dst_stride - width);
	u_dst += ((height >>1) * uv_dst_stride) - 1 - (uv_dst_stride - (width >> 1));
	v_dst += ((height >>1) * uv_dst_stride) - 1 - (uv_dst_stride - (width >> 1));

	for(j = 0; j < height; j += 2){
		for(i = 0; i < width; i += 4){
			VO_U32 a, b, u0, v0, y0, y1;
			a = *src0++;
			b = *src1++;

			u0 = a&0xff;
			v0 = (a&0xff0000)>>16;
			
			y0 = ((a&0xff000000)>>8) | ((a&0xff00)<<16);
			a = *src0++;
			u0 += (b&0xff);
			v0 += ((b&0xff0000)>>16);

			*(u_dst--) = (VO_U8)((u0 >> 1));
			*(v_dst--) = (VO_U8)((v0 >> 1));
			
			y1 = ((b&0xff000000)>>8) | ((b&0xff00)<<16);
			b = *src1++;

			u0 = a&0xff;
			v0 = (a&0xff0000)>>16;
			u0 += (b&0xff);
			v0 += ((b&0xff0000)>>16);
			*(u_dst--) = (VO_U8)((u0 >> 1));
			*(v_dst--) = (VO_U8)((v0 >> 1));

			*((VO_U32 *)y_dst) = y0 | ((a&0xff000000)>>24) | (a&0xff00);
			*((VO_U32 *)(y_dst - y_dst_stride)) = y1 | ((b&0xff000000)>>24) | (b&0xff00);
			y_dst -= 4;
		}
		src0 = src1;
		src1 = src0 + (width >> 1);

		y_dst -= ((y_dst_stride - width) + y_dst_stride);
		u_dst -= (uv_dst_stride - (width>>1));
		v_dst -= (uv_dst_stride - (width>>1));
	}
}

VO_VOID YUYVtoYUV420RT180_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						 VO_U8 * uyvy_src,	VO_S32 width, VO_S32 height)
{
	VO_U32 * src0 = (VO_U32 *)uyvy_src;
	VO_U32 * src1 = (VO_U32 *)(uyvy_src + (width<<1));

	VO_S32 i, j;
	y_dst += (height * y_dst_stride) - 4 - (y_dst_stride - width);
	u_dst += ((height >>1) * uv_dst_stride) - 1 - (uv_dst_stride - (width >> 1));
	v_dst += ((height >>1) * uv_dst_stride) - 1 - (uv_dst_stride - (width >> 1));

	for(j = 0; j < height; j += 2){
		for(i = 0; i < width; i += 4){
			VO_U32 a, b, u0, v0, y0, y1;
			a = *src0++;
			b = *src1++;

			u0 = (a&0xff00)>>8;
			v0 = (a&0xff000000)>>24;
			
			y0 = ((a&0xff0000) | ((a&0xff)<<24));
			a = *src0++;
			u0 += ((b&0xff00)>>8);
			v0 += ((b&0xff000000)>>24);

			*(u_dst--) = (VO_U8)((u0 >> 1));
			*(v_dst--) = (VO_U8)((v0 >> 1));
			
			y1 = ((b&0xff0000) | ((b&0xff)<<24));
			b = *src1++;

			u0 = (a&0xff00)>>8;
			v0 = (a&0xff000000)>>24;
			u0 += ((b&0xff00)>>8);
			v0 += ((b&0xff000000)>>24);
			*(u_dst--) = (VO_U8)((u0 >> 1));
			*(v_dst--) = (VO_U8)((v0 >> 1));

			*((VO_U32 *)y_dst) = y0 | ((a&0xff0000)>>16) | ((a&0xff)<<8);
			*((VO_U32 *)(y_dst - y_dst_stride)) = y1 | ((b&0xff0000)>>16) | ((b&0xff)<<8);
			y_dst -= 4;
		}
		src0 = src1;
		src1 = src0 + (width >> 1);

		y_dst -= ((y_dst_stride - width) + y_dst_stride);
		u_dst -= (uv_dst_stride - (width>>1));
		v_dst -= (uv_dst_stride - (width>>1));
	}
}

VO_VOID VYUY2toYUV420RT180_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						 VO_U8 * uyvy_src,	VO_S32 width, VO_S32 height)
{
	VO_U32 * src0 = (VO_U32 *)uyvy_src;
	VO_U32 * src1 = (VO_U32 *)(uyvy_src + (width<<1));

	VO_S32 i, j;
	y_dst += (height * y_dst_stride) - 4 - (y_dst_stride - width);
	u_dst += ((height >>1) * uv_dst_stride) - 1 - (uv_dst_stride - (width >> 1));
	v_dst += ((height >>1) * uv_dst_stride) - 1 - (uv_dst_stride - (width >> 1));

	for(j = 0; j < height; j += 2){
		for(i = 0; i < width; i += 4){
			VO_U32 a, b, u0, v0, y0, y1;
			a = *src0++;
			b = *src1++;

			v0 = a&0xff;
			u0 = (a&0xff0000)>>16;
			
			y0 = (a&0xff000000) | ((a&0xff00)<<8);
			a = *src0++;
			v0 += (b&0xff);
			u0 += ((b&0xff0000)>>16);

			*(u_dst--) = (VO_U8)((u0 >> 1));
			*(v_dst--) = (VO_U8)((v0 >> 1));
			
			y1 = (b&0xff000000) | ((b&0xff00)<<8);
			b = *src1++;

			v0 = a&0xff;
			u0 = (a&0xff0000)>>16;
			v0 += (b&0xff);
			u0 += ((b&0xff0000)>>16);
			*(u_dst--) = (VO_U8)((u0 >> 1));
			*(v_dst--) = (VO_U8)((v0 >> 1));

			*((VO_U32 *)y_dst) = y0 | ((a&0xff000000)>>16) | ((a&0xff00) >>8);
			*((VO_U32 *)(y_dst - y_dst_stride)) = y1 | ((b&0xff000000)>>16) |  ((b&0xff00) >>8);
			y_dst -= 4;
		}
		src0 = src1;
		src1 = src0 + (width >> 1);

		y_dst -= ((y_dst_stride - width) + y_dst_stride);
		u_dst -= (uv_dst_stride - (width>>1));
		v_dst -= (uv_dst_stride - (width>>1));
	}
}

VO_VOID VYUY2toYUV420Disable_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						  VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						  VO_U8 * uyvy_src,	VO_S32 width, VO_S32 height)
{
	VO_U32 * src0 = (VO_U32 *)uyvy_src;
	VO_U32 * src1 = (VO_U32 *)(uyvy_src + (width<<1));
	
	VO_S32 i, j;
	
	for(j = 0; j < height; j += 2){
		for(i = 0; i < width; i += 4){
			VO_U32 a, b, u0, v0, y0, y1;
			a = *src0++;
			b = *src1++;
			
			v0 = a&0xff;
			u0 = (a&0xff0000)>>16;
			
			y0 = (a>>24) | (a&0xff00);
			a = *src0++;
			v0 += (b&0xff);
			u0 += ((b&0xff0000)>>16);
			
			*(u_dst++) = (VO_U8)((u0 >> 1));
			*(v_dst++) = (VO_U8)((v0 >> 1));
			
			y1 = (b>>24) | (b&0xff00);
			b = *src1++;
			
			v0 = a&0xff;
			u0 = (a&0xff0000)>>16;
			v0 += (b&0xff);
			u0 += ((b&0xff0000)>>16);
			*(u_dst++) = (VO_U8)((u0 >> 1));
			*(v_dst++) = (VO_U8)((v0 >> 1));
			
			*((VO_U32 *)y_dst) = y0 | ((a&0xff000000)>>8) | ((a&0xff00)<<16);
			*((VO_U32 *)(y_dst + y_dst_stride)) = y1 | ((b&0xff000000)>>8) | ((b&0xff00)<<16);
			y_dst += 4;
		}
		src0 = src1;
		src1 = src0 + (width >> 1);
		
		y_dst -= width;
		y_dst += (y_dst_stride << 1);
		
		
		u_dst -= (width>>1);
		u_dst += uv_dst_stride;
		v_dst -= (width>>1);
		v_dst += uv_dst_stride;
	}
}
// Y = 0.257*R + 0.504*G + 0.098*B + 16;
// U = -0.148*R - 0.291*G + 0.439*B + 128;
// V = 0.439*R - 0.368*G - 0.071*B +128;

//0.257*1024=263 0.504*1024=516 0.098*1024=100
// 0.148*1024=152 0.291*1024=
VO_VOID RGB565toYUV420RT180_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						 VO_U8 * rgb565_src, VO_S32 width, VO_S32 height)
{
	VO_S32 j; 
	
	VO_U32 *src	= (VO_U32*) rgb565_src;
	VO_U32 *src1	= (VO_U32*) (rgb565_src + (width << 1));
	
	y_dst += ((height - 1) * y_dst_stride + width - 4);
	u_dst += (((height >>1) - 1) * uv_dst_stride + (width >> 1) - 1);
	v_dst += (((height >>1) - 1) * uv_dst_stride + (width >> 1) - 1);
	
	do{
		j = width;
		do{
			VO_U32 p2_rgb  = *(src++);
			VO_U32 p2_rgb1 = *(src1++);
			
			VO_S32 r, g, b, y, u, v, y1;
			
			/////////////////0, 1
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3;
			
			y = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - y)*0x48);
			v = ((r - y)*0x5b);
			y <<= 24;
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;
			
			p2_rgb  = *(src++);
			
			y |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<16);
			
			//////////////////2, 3
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			
			y1 = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			y1 <<= 24;
			
			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;
			
			p2_rgb1  = *(src1++);
			
			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 16);	
			
			*(u_dst--) = Clip255_table[(u>>8) + 128];
			*(v_dst--) = Clip255_table[(v>>8) + 128];
			
			///////////////////4, 5
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3;
			
			
			g = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - g)*0x48);
			v = ((r - g)*0x5b);
			y |= (g << 8);
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;
			
			y |= (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			
			*((VO_U32*)y_dst) = y;
			
			/////////////////////6, 7
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			
			
			y1 |= (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]<<8);
			
			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;
			
			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= g;	
			
			*(u_dst--) = Clip255_table[(u>>8) + 128];
			*(v_dst--) = Clip255_table[(v>>8) + 128];			
			
			*((VO_S32*)(y_dst - y_dst_stride)) = y1;
			y_dst -= 4;
			
		}while(j -= 4);
		
		y_dst -= ((y_dst_stride << 1) - width);
		u_dst -= (uv_dst_stride - (width>>1));
		v_dst -= (uv_dst_stride - (width>>1));
		
		src	= src1;
		src1 = src + (width>>1);
		
	}while(height -= 2);
}

VO_VOID RGB565toYUV420FilpY_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						 VO_U8 * rgb565_src, VO_S32 width, VO_S32 height)
{
	VO_S32 j; 

	VO_U32 *src	= (VO_U32*) rgb565_src;
	VO_U32 *src1	= (VO_U32*) (rgb565_src + (width << 1));

	y_dst += ((height - 1) * y_dst_stride);
	u_dst += (((height >>1) - 1) * uv_dst_stride);
	v_dst += (((height >>1) - 1) * uv_dst_stride);

	do{
		j = width;
		do{
			VO_U32 p2_rgb  = *(src++);
			VO_U32 p2_rgb1 = *(src1++);

			VO_S32 r, g, b, y, u, v, y1;

			/////////////////0, 1
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3;
			
			y = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - y)*0x48);
			v = ((r - y)*0x5b);
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;

			p2_rgb  = *(src++);

			y |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<8);

			//////////////////2, 3
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			
			y1 = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];

			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;

			p2_rgb1  = *(src1++);

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 8);	
			
			*(u_dst++) = Clip255_table[(u>>8) + 128];
			*(v_dst++) = Clip255_table[(v>>8) + 128];

			///////////////////4, 5
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3;
			

			g = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - g)*0x48);
			v = ((r - g)*0x5b);
			y |= (g << 16);
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;

			y |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<24);

			*((VO_U32*)y_dst) = y;

			/////////////////////6, 7
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			

			y1 |= (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]<<16);

			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 24);	

			*(u_dst++) = Clip255_table[(u>>8) + 128];
			*(v_dst++) = Clip255_table[(v>>8) + 128];			

			*((VO_S32*)(y_dst - y_dst_stride)) = y1;
			y_dst += 4;

		}while(j -= 4);

		y_dst -= ((y_dst_stride << 1) + width);
		u_dst -= (uv_dst_stride + (width>>1));
		v_dst -= (uv_dst_stride + (width>>1));

		src	= src1;
		src1 = src + (width>>1);

	}while(height -= 2);
}

VO_VOID RGB565toYUV420Disable_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						   VO_U8 * rgb565_src, VO_S32 width, VO_S32 height)
{
	VO_S32 i,j; 

	VO_U32 *src	= (VO_U32*) rgb565_src;
	VO_U32 *src1	= (VO_U32*) (rgb565_src + (width << 1));

	for(i = 0; i < height; i += 2) {
		j = width;
		do{
			VO_U32 p2_rgb  = *(src++);
			VO_U32 p2_rgb1 = *(src1++);

			VO_S32 r, g, b, y, u, v, y1;

			/////////////////0, 1
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3;
			
			y = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - y)*0x48);
			v = ((r - y)*0x5b);
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;

			p2_rgb  = *(src++);

			y |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<8);

			//////////////////2, 3
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			
			y1 = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];

			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;

			p2_rgb1  = *(src1++);

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 8);	
			
			*(u_dst++) = Clip255_table[(u>>8) + 128];
			*(v_dst++) = Clip255_table[(v>>8) + 128];

			///////////////////4, 5
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3;
			

			g = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - g)*0x48);
			v = ((r - g)*0x5b);
			y |= (g << 16);
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;

			y |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<24);

			*((VO_U32*)y_dst) = y;

			/////////////////////6, 7
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			

			y1 |= (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]<<16);

			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 24);	

			*(u_dst++) = Clip255_table[(u>>8) + 128];
			*(v_dst++) = Clip255_table[(v>>8) + 128];			

			*((VO_S32*)(y_dst + y_dst_stride)) = y1;
			y_dst += 4;

		}while(j -= 4);

		y_dst += ((y_dst_stride << 1) - width);
		u_dst += (uv_dst_stride - (width>>1));
		v_dst += (uv_dst_stride - (width>>1));

		src	= src1;
		src1 = src + (width>>1);
	}
}


VO_VOID RGB565toYV12RT90L_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				VO_U8 * rgb565_src, VO_S32 width, VO_S32 height)
{
	VO_S32 j; 

	VO_U32 *src	= (VO_U32*) rgb565_src;
	VO_U32 *src1	= (VO_U32*) (rgb565_src + (height << 2));

//	y_dst += ((height - 1) * y_dst_stride);
//	u_dst += (((height >>1) - 1) * uv_dst_stride);
//	v_dst += (((height >>1) - 1) * uv_dst_stride);

	height >>= 1;
	do{
		j = height;
		do{
			VO_U32 p2_rgb  = *(src);
			VO_U32 p2_rgb1 = *(src + height);

			VO_S32 r, g, b, y, u, v, y1;

			src++;

			/////////////////0, 1
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3;
			
			y = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - y)*0x48);
			v = ((r - y)*0x5b);
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;

			p2_rgb  = *(src1);

			y1 = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);

			//////////////////2, 3
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			
			y |= (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]<<8);

			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;

			p2_rgb1  = *(src1 + height);
			src1++;

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 8);	
			
			*(u_dst) = Clip255_table[(u>>8) + 128];
			*(v_dst) = Clip255_table[(v>>8) + 128];

			///////////////////4, 5
			r = (p2_rgb&0xf800)>>8;
			g = (p2_rgb&0x07c0)>>3;
			b = (p2_rgb&0x001f)<<3; 
			

			g = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - g)*0x48);
			v = ((r - g)*0x5b);
			y |= (g << 16);
			
			r = (p2_rgb&0xf8000000)>>24;
			g = (p2_rgb&0x07c00000)>>19;
			b = (p2_rgb&0x001f0000)>>13;

			y1 |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<16);

			/////////////////////6, 7
			r = (p2_rgb1&0xf800)>>8;
			g = (p2_rgb1&0x07c0)>>3;
			b = (p2_rgb1&0x001f)<<3;
			

			y |= (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]<<24);

			r = (p2_rgb1&0xf8000000)>>24;
			g = (p2_rgb1&0x07c00000)>>19;
			b = (p2_rgb1&0x001f0000)>>13;

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 24);	

			*(u_dst + 1) = Clip255_table[(u>>8) + 128];
			*(v_dst + 1) = Clip255_table[(v>>8) + 128];
			u_dst += uv_dst_stride;
			v_dst += uv_dst_stride;			

			*((VO_S32*)y_dst) = y;
			y_dst += y_dst_stride;
			*((VO_S32*)(y_dst)) = y1;
			y_dst += y_dst_stride;

		}while(j -= 1);

		y_dst -= (y_dst_stride*(height<<1) - 4);
		u_dst -= (uv_dst_stride*height - 2);
		v_dst -= (uv_dst_stride*height - 2);

		src	= src1 + height;
		src1 = src + (height << 1);

	}while(width -= 4);

}

VO_VOID YUV420PacktoYUV420_C (VO_U8 * dst_y, VO_U8 * dst_u, VO_U8 * dst_v,
				   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				   VO_U8 * src_y, VO_U8 * src_uv,
				   VO_S32 y_src_stride, VO_S32 uv_src_stride,
				   VO_S32 width, VO_S32 height)
{
	VO_S32 half_width = width / 2;
	VO_S32 half_height = height / 2;
	VO_S32 i, j;
	
	for (i = height; i; i--){
		memcpy(dst_y, src_y, width);
		src_y += y_src_stride;
		dst_y += y_dst_stride;
	}
	
	for (i = half_height; i; i--){
		for(j = half_width; j; j--){ 
			*dst_v++ = *src_uv++;
			*dst_u++ = *src_uv++;
		}
		src_uv -= width;
		src_uv += uv_src_stride;
		dst_u -= half_width;
		dst_u += uv_dst_stride;
		dst_v -= half_width;
		dst_v += uv_dst_stride;
	}
}

VO_VOID RGB32toYUV420Disable_C (VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						   VO_U8 * rgb32_src, VO_S32 width, VO_S32 height)
{
	VO_S32 i,j; 

	VO_U32 *src	= (VO_U32*) rgb32_src;
	VO_U32 *src1	= (VO_U32*) (rgb32_src + (width << 2));

	for(i = 0; i < height; i += 2) {
		j = width;
		do{
			VO_U32 p1_rgb  = *(src++);
			VO_U32 p1_rgb1 = *(src1++);

			VO_S32 r, g, b, y, u, v, y1;

			/////////////////0, 1
			b = (p1_rgb>>16)&0xFF;
			g = (p1_rgb>>8)&0xFF;
			r = p1_rgb&0xFF;

			p1_rgb  = *(src++);	

			y = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - y)*0x48);
			v = ((r - y)*0x5b);

			b = (p1_rgb>>16)&0xFF;
			g = (p1_rgb>>8)&0xFF;
			r = p1_rgb&0xFF;

			p1_rgb  = *(src++);

			y |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<8);

			//////////////////2, 3
			b = (p1_rgb1>>16)&0xFF;
			g = (p1_rgb1>>8)&0xFF;
			r = p1_rgb1&0xFF;

			p1_rgb1  = *(src1++);

			y1 = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];

			b = (p1_rgb1>>16)&0xFF;
			g = (p1_rgb1>>8)&0xFF;
			r = p1_rgb1&0xFF;

			p1_rgb1  = *(src1++);

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 8);	
			
			*(u_dst++) = Clip255_table[(u>>8) + 128];
			*(v_dst++) = Clip255_table[(v>>8) + 128];

			///////////////////4, 5
			b = (p1_rgb>>16)&0xFF;
			g = (p1_rgb>>8)&0xFF;
			r = p1_rgb&0xFF;

			p1_rgb  = *(src++);
			

			g = Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7];
			u = ((b - g)*0x48);
			v = ((r - g)*0x5b);
			y |= (g << 16);
			
			b = (p1_rgb>>16)&0xFF;
			g = (p1_rgb>>8)&0xFF;
			r = p1_rgb&0xFF;

			y |= ((Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7])<<24);

			*((VO_U32*)y_dst) = y;

			/////////////////////6, 7
			b = (p1_rgb1>>16)&0xFF;
			g = (p1_rgb1>>8)&0xFF;
			r = p1_rgb1&0xFF;

			p1_rgb1  = *(src1++);
			

			y1 |= (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]<<16);

			b = (p1_rgb1>>16)&0xFF;
			g = (p1_rgb1>>8)&0xFF;
			r = p1_rgb1&0xFF;

			g = (Clip255_table[(0x26*r + 0x4b*g + 0xf*b)>>7]);
			u += ((b - g)*0x48);
			v += ((r - g)*0x5b);
			y1 |= (g << 24);	

			*(u_dst++) = Clip255_table[(u>>8) + 128];
			*(v_dst++) = Clip255_table[(v>>8) + 128];			

			*((VO_S32*)(y_dst + y_dst_stride)) = y1;
			y_dst += 4;

		}while(j -= 4);

		y_dst += ((y_dst_stride << 1) - width);
		u_dst += (uv_dst_stride - (width>>1));
		v_dst += (uv_dst_stride - (width>>1));

		src	= src1;
		src1 = src + width;
	}
}

