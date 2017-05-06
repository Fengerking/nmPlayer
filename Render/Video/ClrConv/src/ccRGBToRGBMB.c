/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "stdio.h"

#include "ccRGBToRGBMB.h"
#include "ccConstant.h"

CC_RGB565_MB_PTR RGB565TORGB565_MB;

#define LOADRGB888_DATA(a3, a7, c){\
	a0 = psrc_r[c];\
	a1 = psrc_g[c];\
	a3 = psrc_b[c];	\
	a4 = psrc_r[in_stride+c];\
	a5 = psrc_g[in_stride+c];\
	a7 = psrc_b[in_stride+c];\
}

#define LOADRGB888H_DATA(a3, a7, c){\
	a0 = psrc_r[c];\
	a1 = psrc_g[c];\
	a3 = psrc_b[c];	\
	a4 = psrc_r[c + 1];\
	a5 = psrc_g[c + 1];\
	a7 = psrc_b[c + 1];\
}

#define PIXRGB_0(){\
	a0 = ((a0 << 8) & 0xf800) | ((a1 << 3) & 0x7e0) | (a2 >> 3);\
}

#define PIXRGB_1(){\
	a4 = ((a4 << 8) & 0xf800) | ((a5 << 3) & 0x7e0) | (a7 >> 3);\
	a0 |= (a4 << 16);\
}

#define PIXRGB_2(){\
	a4 = ((a4 << 8) & 0xf800) | ((a5 << 3) & 0x7e0) | (a7 >> 3);\
	a4 |= (a0 << 16);\
}

#ifdef COLCONVENABLE
void RGB888Planar_to_RGB565_cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
										  VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
										  const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U16 i,j,t0,t1;
	VO_U32 red_temp, green_temp, blue_temp;
	VO_U32 red, green, blue;
	VO_U32 *pred, *pgreen, *pblue;
	VO_U32 *dst;

	pred   = (VO_U32 *)psrc_y;
	pgreen = (VO_U32 *)psrc_u;
	pblue  = (VO_U32 *)psrc_v;

	dst = (VO_U32 *)out_buf;

	for(i = 0; i < 16; i++){
		for(j = 0; j < 4; j++){
			red_temp   = pred[j];
			green_temp = pgreen[j];
			blue_temp  = pblue[j];

			red    = red_temp   & 0x000000ff;
			green  = green_temp & 0x000000ff;
			blue   = blue_temp  & 0x000000ff;

			t0 = ((red << 8) & 0xf800) | ((green << 3) & 0x7e0) | (blue >> 3);

			red    = (red_temp   & 0x0000ff00) >> 8;
			green  = (green_temp & 0x0000ff00) >> 8;
			blue   = (blue_temp  & 0x0000ff00) >> 8;

			t1 = ((red << 8) & 0xf800) | ((green << 3) & 0x7e0) | (blue >> 3);

			dst[2*j] = t0 | (t1 << 16); 

			red    = (red_temp   & 0x00ff0000) >> 16;
			green  = (green_temp & 0x00ff0000) >> 16;
			blue   = (blue_temp  & 0x00ff0000) >> 16;

			t0 = ((red << 8) & 0xf800) | ((green << 3) & 0x7e0) | (blue >> 3);

			red    = (red_temp   & 0xff000000) >> 24;
			green  = (green_temp & 0xff000000) >> 24;
			blue   = (blue_temp  & 0xff000000) >> 24;

			t1 = ((red << 8) & 0xf800) | ((green << 3) & 0x7e0) | (blue >> 3);

			dst[2*j + 1] = t0 | (t1 << 16); 

		}

		pred   += (in_stride >> 2);
		pgreen += (in_stride >> 2);
		pblue  += (in_stride >> 2);

		dst += (out_stride >> 2);
	}

}

void RGB888Planar_to_RGB565_cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
								  VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
								  const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = src_stride , out_width = width, out_height = height;
	//const VO_S32 *param = yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf;
	VO_U8 *psrcR, *psrcG, *psrcB;
	VO_U32 red, green, blue;
	VO_U32 t;

	VO_S32 step_y, step_x, out_stride = dst_stride;

	/*start of output address*/
	step_y = 2;
	step_x = out_stride;
	start_outbuf = dst;

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy * in_stride;
		psrcR = src_y + vx;
		psrcG = src_u + vx;
		psrcB = src_v + vx;

		for(vx = 0; vx < out_width; vx++) {


			red    = psrcR[vx];
			green  = psrcG[vx];
			blue   = psrcB[vx];

			t = ((red << 8) & 0xf800) | ((green << 3) & 0x7e0) | (blue >> 3);

			*((short*)outbuf) = (unsigned short)t;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
}
#endif //COLCONVENABLE

#ifdef COLCONVENABLE
void rgb888_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	VO_S32 a0, a4, a5;
	VO_S32 b4, b5;
	VO_S32 c4, c5;
	VO_S32 d0, d4, d5;
	VO_S32 e4, e5;
	VO_S32 f4, f5;
	VO_S32 aa, bb, cc, dd;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *pRed0   = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *pGreen0 = src_u + y_scale_par[0] * uin_stride;
		VO_U8 *pBlue0  = src_v + y_scale_par[0] * vin_stride;

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *pRed1   = src_y + y_scale_par[3] * src_stride;
		VO_U8 *pGreen1 = src_u + y_scale_par[3] * src_stride;
		VO_U8 *pBlue1  = src_v + y_scale_par[3] * src_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];

		VO_S32 t1,t2;
		VO_S32 tt1,tt2;

		i = width;

		do{
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			d0 = x_scale_par0[3];//src pos1
			cc = x_scale_par0[4];
			dd = x_scale_par0[5];

			x_scale_par0 += 6;

			a4 = ((pRed0[a0]*bb + pRed0[a0+1]*aa)*bb0 + (pRed0[a0+src_stride]*bb + pRed0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((pRed1[a0]*bb + pRed1[a0+1]*aa)*bb1 + (pRed1[a0+src_stride]*bb + pRed1[a0+src_stride+1]*aa)*aa1)>>20;

			d4 = ((pRed0[d0]*dd + pRed0[d0+1]*cc)*bb0 + (pRed0[d0+src_stride]*dd + pRed0[d0+src_stride+1]*cc)*aa0)>>20;
			d5 = ((pRed1[d0]*dd + pRed1[d0+1]*cc)*bb1 + (pRed1[d0+src_stride]*dd + pRed1[d0+src_stride+1]*cc)*aa1)>>20;

			b4 = ((pGreen0[a0]*bb + pGreen0[a0+1]*aa)*bb0 + (pGreen0[a0+src_stride]*bb + pGreen0[a0+src_stride+1]*aa)*aa0)>>20;
			b5 = ((pGreen1[a0]*bb + pGreen1[a0+1]*aa)*bb1 + (pGreen1[a0+src_stride]*bb + pGreen1[a0+src_stride+1]*aa)*aa1)>>20;

			e4 = ((pGreen0[d0]*dd + pGreen0[d0+1]*cc)*bb0 + (pGreen0[d0+src_stride]*dd + pGreen0[d0+src_stride+1]*cc)*aa0)>>20;
			e5 = ((pGreen1[d0]*dd + pGreen1[d0+1]*cc)*bb1 + (pGreen1[d0+src_stride]*dd + pGreen1[d0+src_stride+1]*cc)*aa1)>>20;

			c4 = ((pBlue0[a0]*bb + pBlue0[a0+1]*aa)*bb0 + (pBlue0[a0+src_stride]*bb + pBlue0[a0+src_stride+1]*aa)*aa0)>>20;
			c5 = ((pBlue1[a0]*bb + pBlue1[a0+1]*aa)*bb1 + (pBlue1[a0+src_stride]*bb + pBlue1[a0+src_stride+1]*aa)*aa1)>>20;

			f4 = ((pBlue0[d0]*dd + pBlue0[d0+1]*cc)*bb0 + (pBlue0[d0+src_stride]*dd + pBlue0[d0+src_stride+1]*cc)*aa0)>>20;
			f5 = ((pBlue1[d0]*dd + pBlue1[d0+1]*cc)*bb1 + (pBlue1[d0+src_stride]*dd + pBlue1[d0+src_stride+1]*cc)*aa1)>>20;

			t1 = ((a4 << 8) & 0xf800) | ((b4 << 3) & 0x7e0) | (c4 >> 3);
			t2 = ((a5 << 8) & 0xf800) | ((b5 << 3) & 0x7e0) | (c5 >> 3);

			tt1 = ((d4 << 8) & 0xf800) | ((e4 << 3) & 0x7e0) | (f4 >> 3);
			tt2 = ((d5 << 8) & 0xf800) | ((e5 << 3) & 0x7e0) | (f5 >> 3);

			*((VO_U32*)dst) = t1 | (tt1 << 16);
			*((VO_U32*)(dst+dst_stride)) = t2 | (tt2 << 16);

			dst += 4;
		}while((i-=2) != 0);

		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);
}
#endif //COLCONVENABLE

void cc_rgb24_mb_s(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, 
				   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *psrc = src + y_scale_par[0] * src_stride; // TBD


		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];

		i = width;		
		do{
			VO_S32 a0, a1, a2;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1) + a0;
			//B
			a1 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a0++;
			//G
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>22;
			a1 |= (a2 << 5);
			a0++;
			//R
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a1 |= (a2 << 11);

			x_scale_par0 += 3;

			/////////////////////////////////////////////////////////////////////
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1) + a0;
			//B
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a1 |= (a2 << 16);
			a0++;
			//G
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>22;
			a1 |= (a2 << 21);
			a0++;
			//R
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a1 |= (a2 << 27);

			x_scale_par0 += 3;	

			*((VO_U32*)dst) = a1;
			dst += 4;
		}while((i -= 2) != 0);

		dst -= (width<<1);
		dst += dst_stride;
		y_scale_par += 3;
	}while(--height != 0);
}

void cc_rgb565_mb_s(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst,VO_S32 src_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *psrc = src + y_scale_par[0] * src_stride; // TBD


		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1);
			a1 = *((VO_U16*)(psrc + a0));
			a2 = *((VO_U16*)(psrc + a0 + 2));
			a3 = *((VO_U16*)(psrc + a0 + src_stride));
			a4 = *((VO_U16*)(psrc + a0 + 2 + src_stride));
			//B
			a0 = (((a1&0x1f)*bb + (a2&0x1f)*aa)*bb0 + ((a3&0x1f)*bb + (a4&0x1f)*aa)*aa0)>>20;
			//G
			a5 = ((((a1&0x7e0)>>5)*bb + ((a2&0x7e0)>>5)*aa)*bb0 + (((a3&0x7e0)>>5)*bb + ((a4&0x7e0)>>5)*aa)*aa0)>>20;
			a0 |= (a5 << 5);
			//R
			a5 = (((a1>>11)*bb + (a2>>11)*aa)*bb0 + ((a3>>11)*bb + (a4>>11)*aa)*aa0)>>20;
			a5 = (a5 << 11) | a0;

			x_scale_par0 += 3;
			/////////////////////////////////////////////////////////////////////
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1);

			a1 = *((VO_U16*)(psrc + a0));
			a2 = *((VO_U16*)(psrc + a0 + 2));
			a3 = *((VO_U16*)(psrc + a0 + src_stride));
			a4 = *((VO_U16*)(psrc + a0 + 2 + src_stride));

			//B
			a0 = (((a1&0x1f)*bb + (a2&0x1f)*aa)*bb0 + ((a3&0x1f)*bb + (a4&0x1f)*aa)*aa0)>>20;
			a5 |= (a0 << 16);
			//G
			a0 = ((((a1&0x7e0)>>5)*bb + ((a2&0x7e0)>>5)*aa)*bb0 + (((a3&0x7e0)>>5)*bb + ((a4&0x7e0)>>5)*aa)*aa0)>>20;
			a5 |= (a0 << 21);
			//R
			a0 = (((a1>>11)*bb + (a2>>11)*aa)*bb0 + ((a3>>11)*bb + (a4>>11)*aa)*aa0)>>20;
			a5 |= (a0 << 27);

			x_scale_par0 += 3;	

			*((VO_U32*)dst) = a5;
			dst += 4;
		}while((i -= 2) != 0);

		dst -= (width<<1);
		dst += dst_stride;
		y_scale_par += 3;
	}while(--height != 0);
}

#define WRITE_RGBPIX(a4, x){\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a4;\
	out_buf += (x);\
}

#ifdef COLCONVENABLE
void RGB888PlanarToRGB16_MB_rotation_90L(VO_U8 *psrc_r, VO_U8 *psrc_g, VO_U8 *psrc_b, const VO_S32 in_stride, VO_U8 *out_buf,const VO_S32 out_stride,
										 const VO_S32* const param_tab, const VO_U32 is420planar, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;

	do{
		VO_S32 a0, a1, a2, a4, a5,a7;// a3,
		//0
		LOADRGB888_DATA(a2, a7, 0)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, 0)

		LOADRGB888_DATA(a2, a7, 1)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		//1	
		LOADRGB888_DATA(a2, a7, 2)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		LOADRGB888_DATA(a2, a7, 3)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		//2
		LOADRGB888_DATA(a2, a7, 4)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		LOADRGB888_DATA(a2, a7, 5)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		//3
		LOADRGB888_DATA(a2, a7, 6)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		LOADRGB888_DATA(a2, a7, 7)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		//4
		LOADRGB888_DATA(a2, a7, 8)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		LOADRGB888_DATA(a2, a7, 9)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		//5
		LOADRGB888_DATA(a2, a7, 10)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		LOADRGB888_DATA(a2, a7, 11)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		//6
		LOADRGB888_DATA(a2, a7, 12)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		LOADRGB888_DATA(a2, a7, 13)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		//7
		LOADRGB888_DATA(a2, a7, 14)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		LOADRGB888_DATA(a2, a7, 15)
		PIXRGB_0()
		PIXRGB_1()
		WRITE_RGBPIX(a0, -out_stride)

		psrc_r += (in_stride<<1);
		psrc_g += (in_stride<<1);
		psrc_b += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void RGB888PlanarToRGB16_MB_rotation_90R(VO_U8 *psrc_r, VO_U8 *psrc_g, VO_U8 *psrc_b, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
										 const VO_S32* const param_tab, const VO_U32 is420planar, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;

	do{
		VO_S32 a0, a1, a2, a4, a5,a7;// a3,
		//0
		LOADRGB888_DATA(a2, a7, 0)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 0)

		LOADRGB888_DATA(a2, a7, 1)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//1	
		LOADRGB888_DATA(a2, a7, 2)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		LOADRGB888_DATA(a2, a7, 3)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//2
		LOADRGB888_DATA(a2, a7, 4)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		LOADRGB888_DATA(a2, a7, 5)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//3
		LOADRGB888_DATA(a2, a7, 6)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		LOADRGB888_DATA(a2, a7, 7)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//4
		LOADRGB888_DATA(a2, a7, 8)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		LOADRGB888_DATA(a2, a7, 9)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//5
		LOADRGB888_DATA(a2, a7, 10)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		LOADRGB888_DATA(a2, a7, 11)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//6
		LOADRGB888_DATA(a2, a7, 12)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		LOADRGB888_DATA(a2, a7, 13)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//7
		LOADRGB888_DATA(a2, a7, 14)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		LOADRGB888_DATA(a2, a7, 15)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		psrc_r += (in_stride<<1);
		psrc_g += (in_stride<<1);
		psrc_b += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void rgb888_to_rgb565_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride,VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, 
										  const VO_S32 green_stride, const VO_S32 blue_stride)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_r0 = src_r + y_scale_par[0] * red_stride; // TBD
		VO_U8 *src_g0 = src_g + y_scale_par[0] * green_stride;
		VO_U8 *src_b0 = src_b + y_scale_par[0] * blue_stride;

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_r1 = src_r + y_scale_par[3] * red_stride;
		VO_U8 *src_g1 = src_g + y_scale_par[3] * green_stride;
		VO_U8 *src_b1 = src_b + y_scale_par[3] * blue_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		i = width;		
		do{
			VO_S32 a0, a1, a2,a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];	

			a4 = ((src_r0[a0]*bb + src_r0[a0+1]*aa)*bb0 + (src_r0[a0+red_stride]*bb + src_r0[a0+red_stride+1]*aa)*aa0)>>20;
			a5 = ((src_r1[a0]*bb + src_r1[a0+1]*aa)*bb1 + (src_r1[a0+red_stride]*bb + src_r1[a0+red_stride+1]*aa)*aa1)>>20;

			a1 = ((src_g0[a0]*bb + src_g0[a0+1]*aa)*bb0 + (src_g0[a0+green_stride]*bb + src_g0[a0+green_stride+1]*aa)*aa0)>>20;
			a2 = ((src_g1[a0]*bb + src_g1[a0+1]*aa)*bb1 + (src_g1[a0+green_stride]*bb + src_g1[a0+green_stride+1]*aa)*aa1)>>20;

			a7 = ((src_b0[a0]*bb + src_b0[a0+1]*aa)*bb0 + (src_b0[a0+blue_stride]*bb + src_b0[a0+blue_stride+1]*aa)*aa0)>>20;
			a8 = ((src_b1[a0]*bb + src_b1[a0+1]*aa)*bb1 + (src_b1[a0+blue_stride]*bb + src_b1[a0+blue_stride+1]*aa)*aa1)>>20;

			a4 = ((a4 << 8) & 0xf800) | ((a1 << 3) & 0x7e0) | (a7 >> 3);
			a5 = ((a5 << 8) & 0xf800) | ((a2 << 3) & 0x7e0) | (a8 >> 3);
			a4 = (a5 << 16) | a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a4 = ((src_r0[a6]*bb + src_r0[a6+1]*aa)*bb0 + (src_r0[a6+red_stride]*bb + src_r0[a6+red_stride+1]*aa)*aa0)>>20;
			a5 = ((src_r1[a6]*bb + src_r1[a6+1]*aa)*bb1 + (src_r1[a6+red_stride]*bb + src_r1[a6+red_stride+1]*aa)*aa1)>>20;

			a1 = ((src_g0[a6]*bb + src_g0[a6+1]*aa)*bb0 + (src_g0[a6+green_stride]*bb + src_g0[a6+green_stride+1]*aa)*aa0)>>20;
			a2 = ((src_g1[a6]*bb + src_g1[a6+1]*aa)*bb1 + (src_g1[a6+green_stride]*bb + src_g1[a6+green_stride+1]*aa)*aa1)>>20;

			a7 = ((src_b0[a6]*bb + src_b0[a6+1]*aa)*bb0 + (src_b0[a6+blue_stride]*bb + src_b0[a6+blue_stride+1]*aa)*aa0)>>20;
			a8 = ((src_b1[a6]*bb + src_b1[a6+1]*aa)*bb1 + (src_b1[a6+blue_stride]*bb + src_b1[a6+blue_stride+1]*aa)*aa1)>>20;

			a4 = ((a4 << 8) & 0xf800) | ((a1 << 3) & 0x7e0) | (a7 >> 3);
			a5 = ((a5 << 8) & 0xf800) | ((a2 << 3) & 0x7e0) | (a8 >> 3);
			a4 = (a5 << 16) | a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void rgb888_to_rgb565_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride,VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, 
										  const VO_S32 green_stride, const VO_S32 blue_stride)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_r0 = src_r + y_scale_par[0] * red_stride; // TBD
		VO_U8 *src_g0 = src_g + y_scale_par[0] * green_stride;
		VO_U8 *src_b0 = src_b + y_scale_par[0] * blue_stride;

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_r1 = src_r + y_scale_par[3] * red_stride;
		VO_U8 *src_g1 = src_g + y_scale_par[3] * green_stride;
		VO_U8 *src_b1 = src_b + y_scale_par[3] * blue_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a4 = ((src_r0[a0]*bb + src_r0[a0+1]*aa)*bb0 + (src_r0[a0+red_stride]*bb + src_r0[a0+red_stride+1]*aa)*aa0)>>20;
			a5 = ((src_r1[a0]*bb + src_r1[a0+1]*aa)*bb1 + (src_r1[a0+red_stride]*bb + src_r1[a0+red_stride+1]*aa)*aa1)>>20;

			a1 = ((src_g0[a0]*bb + src_g0[a0+1]*aa)*bb0 + (src_g0[a0+green_stride]*bb + src_g0[a0+green_stride+1]*aa)*aa0)>>20;
			a2 = ((src_g1[a0]*bb + src_g1[a0+1]*aa)*bb1 + (src_g1[a0+green_stride]*bb + src_g1[a0+green_stride+1]*aa)*aa1)>>20;

			a7 = ((src_b0[a0]*bb + src_b0[a0+1]*aa)*bb0 + (src_b0[a0+blue_stride]*bb + src_b0[a0+blue_stride+1]*aa)*aa0)>>20;
			a8 = ((src_b1[a0]*bb + src_b1[a0+1]*aa)*bb1 + (src_b1[a0+blue_stride]*bb + src_b1[a0+blue_stride+1]*aa)*aa1)>>20;

			a4 = ((a4 << 8) & 0xf800) | ((a1 << 3) & 0x7e0) | (a7 >> 3);
			a5 = ((a5 << 8) & 0xf800) | ((a2 << 3) & 0x7e0) | (a8 >> 3);
			a4 = (a4 << 16) | a5;

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a4 = ((src_r0[a6]*bb + src_r0[a6+1]*aa)*bb0 + (src_r0[a6+red_stride]*bb + src_r0[a6+red_stride+1]*aa)*aa0)>>20;
			a5 = ((src_r1[a6]*bb + src_r1[a6+1]*aa)*bb1 + (src_r1[a6+red_stride]*bb + src_r1[a6+red_stride+1]*aa)*aa1)>>20;

			a1 = ((src_g0[a6]*bb + src_g0[a6+1]*aa)*bb0 + (src_g0[a6+green_stride]*bb + src_g0[a6+green_stride+1]*aa)*aa0)>>20;
			a2 = ((src_g1[a6]*bb + src_g1[a6+1]*aa)*bb1 + (src_g1[a6+green_stride]*bb + src_g1[a6+green_stride+1]*aa)*aa1)>>20;

			a7 = ((src_b0[a6]*bb + src_b0[a6+1]*aa)*bb0 + (src_b0[a6+blue_stride]*bb + src_b0[a6+blue_stride+1]*aa)*aa0)>>20;
			a8 = ((src_b1[a6]*bb + src_b1[a6+1]*aa)*bb1 + (src_b1[a6+blue_stride]*bb + src_b1[a6+blue_stride+1]*aa)*aa1)>>20;

			a4 = ((a4 << 8) & 0xf800) | ((a1 << 3) & 0x7e0) | (a7 >> 3);
			a5 = ((a5 << 8) & 0xf800) | ((a2 << 3) & 0x7e0) | (a8 >> 3);
			a4 = (a4 << 16) | a5;


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);

}
#endif //COLCONVENABLE

void cc_rgb24_mb_s_l90(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst,VO_S32 src_stride,
					   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		VO_U8 *psrc = src + y_scale_par[0] * src_stride; // TBD

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];

		VO_U8 *psrc1 = src + y_scale_par[3] * src_stride; 
		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];

		i = width;		
		do{
			VO_S32 a0, a1, a2;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1) + a0;
			//B
			a1 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a0++;
			//G
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>22;
			a1 |= (a2 << 5);
			a0++;
			//R
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a1 |= (a2 << 11);

			//B
			a0 -= 2;

			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 16);
			a0++;
			//G
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>22;
			a1 |= (a2 << 21);
			a0++;
			//R
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 27);

			*((VO_U32*)dst) = a1;
			dst -= dst_stride;

			x_scale_par0 += 3;

			/////////////////////////////////////////////////////////////////////
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1) + a0;
			//B
			a1 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a0++;
			//G
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>22;
			a1 |= (a2 << 5);
			a0++;
			//R
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a1 |= (a2 << 11);

			//B
			a0 -= 2;

			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 16);
			a0++;
			//G
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>22;
			a1 |= (a2 << 21);
			a0++;
			//R
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 27);

			*((VO_U32*)dst) = a1;
			dst -= dst_stride;

			x_scale_par0 += 3;	

		}while((i -= 2) != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_rgb24_mb_s_r90(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		VO_U8 *psrc1 = src + y_scale_par[0] * src_stride; // TBD

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];

		VO_U8 *psrc = src + y_scale_par[3] * src_stride; 
		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];

		i = width;		
		do{
			VO_S32 a0, a1, a2;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1) + a0;
			//B
			a1 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a0++;
			//G
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>22;
			a1 |= (a2 << 5);
			a0++;
			//R
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a1 |= (a2 << 11);

			//B
			a0 -= 2;

			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 16);
			a0++;
			//G
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>22;
			a1 |= (a2 << 21);
			a0++;
			//R
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 27);

			*((VO_U32*)dst) = a1;
			dst += dst_stride;

			x_scale_par0 += 3;

			/////////////////////////////////////////////////////////////////////
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1) + a0;
			//B
			a1 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a0++;
			//G
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>22;
			a1 |= (a2 << 5);
			a0++;
			//R
			a2 = ((psrc[a0]*bb + psrc[a0+3]*aa)*bb0 + (psrc[a0+src_stride]*bb + psrc[a0+src_stride+3]*aa)*aa0)>>23;
			a1 |= (a2 << 11);

			//B
			a0 -= 2;

			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 16);
			a0++;
			//G
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>22;
			a1 |= (a2 << 21);
			a0++;
			//R
			a2 = ((psrc1[a0]*bb + psrc1[a0+3]*aa)*bb1 + (psrc1[a0+src_stride]*bb + psrc1[a0+src_stride+3]*aa)*aa1)>>23;
			a1 |= (a2 << 27);

			*((VO_U32*)dst) = a1;
			dst += dst_stride;

			x_scale_par0 += 3;	

		}while((i -= 2) != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

///////////////////////////////////////////////////////////////////////Rotation right 180
#undef WRITE_RGBPIX
#define WRITE_RGBPIX(a4, x){\
	*((VO_S32*)(out_buf-(x))) = (VO_S32)a4;\
}

#ifdef COLCONVENABLE
void RGB888PlanarToRGB16_MB_rotation_180(VO_U8 *psrc_r, VO_U8 *psrc_g, VO_U8 *psrc_b, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
										 const VO_S32* const param_tab, const VO_U32 is420planar, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2,a4, a5,a7;// a3, 
		//0
		LOADRGB888H_DATA(a2, a7, 0)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 0)

		LOADRGB888H_DATA(a2, a7, in_stride)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride)

		//1	
		LOADRGB888H_DATA(a2, a7, 2)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 4)

		LOADRGB888H_DATA(a2, a7, in_stride+2)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride + 4)

		//2
		LOADRGB888H_DATA(a2, a7, 4)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 8)

		LOADRGB888H_DATA(a2, a7, in_stride+4)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride + 8)

		//3
		LOADRGB888H_DATA(a2, a7, 6)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 12)

		LOADRGB888H_DATA(a2, a7, in_stride+6)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride + 12)

		//4
		LOADRGB888H_DATA(a2, a7, 8)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 16)

		LOADRGB888H_DATA(a2, a7, in_stride+8)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride + 16)

		//5
		LOADRGB888H_DATA(a2, a7, 10)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 20)

		LOADRGB888H_DATA(a2, a7, in_stride+10)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride + 20)

		//6
		LOADRGB888H_DATA(a2, a7, 12)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 24)

		LOADRGB888H_DATA(a2, a7, in_stride+12)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride + 24)

		//7
		LOADRGB888H_DATA(a2, a7, 14)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, 28)

		LOADRGB888H_DATA(a2, a7, in_stride+14)
		PIXRGB_0()
		PIXRGB_2()
		WRITE_RGBPIX(a4, out_stride + 28)

		psrc_r += (in_stride<<1);
		psrc_g += (in_stride<<1);
		psrc_b += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

void rgb888_to_rgb565_mb_rotation_180_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride,VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, 
										  const VO_S32 green_stride, const VO_S32 blue_stride)
{
	VO_S32 i;
	dst += dst_stride*(height - 1); 

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_r0 = src_r + y_scale_par[0] * red_stride; // TBD
		VO_U8 *src_g0 = src_g + y_scale_par[0] * green_stride;
		VO_U8 *src_b0 = src_b + y_scale_par[0] * blue_stride;

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_r1 = src_r + y_scale_par[3] * red_stride;
		VO_U8 *src_g1 = src_g + y_scale_par[3] * green_stride;
		VO_U8 *src_b1 = src_b + y_scale_par[3] * blue_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		dst = dst + (width<<1) - 4;
		i = width;		
		do{
			VO_S32 a0, a1, a2, a4, a5, a6, a7, a8;
			VO_S32 b1, b2, b4, b5, b7, b8;
			VO_S32 aa, bb;
			VO_U32 tmp;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a4 = ((src_r0[a0]*bb + src_r0[a0+1]*aa)*bb0 + (src_r0[a0+red_stride]*bb + src_r0[a0+red_stride+1]*aa)*aa0)>>20;
			a5 = ((src_r1[a0]*bb + src_r1[a0+1]*aa)*bb1 + (src_r1[a0+red_stride]*bb + src_r1[a0+red_stride+1]*aa)*aa1)>>20;

			a1 = ((src_g0[a0]*bb + src_g0[a0+1]*aa)*bb0 + (src_g0[a0+green_stride]*bb + src_g0[a0+green_stride+1]*aa)*aa0)>>20;
			a2 = ((src_g1[a0]*bb + src_g1[a0+1]*aa)*bb1 + (src_g1[a0+green_stride]*bb + src_g1[a0+green_stride+1]*aa)*aa1)>>20;

			a7 = ((src_b0[a0]*bb + src_b0[a0+1]*aa)*bb0 + (src_b0[a0+blue_stride]*bb + src_b0[a0+blue_stride+1]*aa)*aa0)>>20;
			a8 = ((src_b1[a0]*bb + src_b1[a0+1]*aa)*bb1 + (src_b1[a0+blue_stride]*bb + src_b1[a0+blue_stride+1]*aa)*aa1)>>20;

			a4 = ((a4 << 8) & 0xf800) | ((a1 << 3) & 0x7e0) | (a7 >> 3);
			a7 = ((a5 << 8) & 0xf800) | ((a2 << 3) & 0x7e0) | (a8 >> 3);
			/////////////////////////////////////////////////////////////////////
			//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			b4 = ((src_r0[a6]*bb + src_r0[a6+1]*aa)*bb0 + (src_r0[a6+red_stride]*bb + src_r0[a6+red_stride+1]*aa)*aa0)>>20;
			b5 = ((src_r1[a6]*bb + src_r1[a6+1]*aa)*bb1 + (src_r1[a6+red_stride]*bb + src_r1[a6+red_stride+1]*aa)*aa1)>>20;

			b1 = ((src_g0[a6]*bb + src_g0[a6+1]*aa)*bb0 + (src_g0[a6+green_stride]*bb + src_g0[a6+green_stride+1]*aa)*aa0)>>20;
			b2 = ((src_g1[a6]*bb + src_g1[a6+1]*aa)*bb1 + (src_g1[a6+green_stride]*bb + src_g1[a6+green_stride+1]*aa)*aa1)>>20;

			b7 = ((src_b0[a6]*bb + src_b0[a6+1]*aa)*bb0 + (src_b0[a6+blue_stride]*bb + src_b0[a6+blue_stride+1]*aa)*aa0)>>20;
			b8 = ((src_b1[a6]*bb + src_b1[a6+1]*aa)*bb1 + (src_b1[a6+blue_stride]*bb + src_b1[a6+blue_stride+1]*aa)*aa1)>>20;

			b4 = ((b4 << 8) & 0xf800) | ((b1 << 3) & 0x7e0) | (b7 >> 3);
			b8 = ((b5 << 8) & 0xf800) | ((b2 << 3) & 0x7e0) | (b8 >> 3);

			a4 |= (b4 << 16);
			a7 |= (b8 << 16);

			tmp = (VO_U32)a4;
			tmp = (tmp>>16)|(tmp<<16);
			*((VO_U32*)dst) = tmp;
			tmp = (VO_U32)a7;
			tmp = (tmp>>16)|(tmp<<16);
			*((VO_U32*)(dst-dst_stride)) = tmp;
			dst -= 4;
		}while((i-=2) != 0);
		dst += 4;
		//		dst -= (width<<1);
		dst -= (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);
}
#endif //COLCONVENABLE

void cc_rgb565_mb_s_l90(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst,VO_S32 src_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		VO_U8 *psrc = src + y_scale_par[0] * src_stride; // TBD

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];

		VO_U8 *psrc1 = src + y_scale_par[3] * src_stride; 
		//VO_S32 aa1 = y_scale_par[4];
		//VO_S32 bb1 = y_scale_par[5];

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1);
			a1 = *((VO_U16*)(psrc + a0));
			a2 = *((VO_U16*)(psrc + a0 + 2));
			a3 = *((VO_U16*)(psrc + a0 + src_stride));
			a4 = *((VO_U16*)(psrc + a0 + 2 + src_stride));
			//B
			a5 = (((a1&0x1f)*bb + (a2&0x1f)*aa)*bb0 + ((a3&0x1f)*bb + (a4&0x1f)*aa)*aa0)>>20;
			//G
			a6 = ((((a1&0x7e0)>>5)*bb + ((a2&0x7e0)>>5)*aa)*bb0 + (((a3&0x7e0)>>5)*bb + ((a4&0x7e0)>>5)*aa)*aa0)>>20;
			a5 |= (a6 << 5);
			//R
			a6 = (((a1>>11)*bb + (a2>>11)*aa)*bb0 + ((a3>>11)*bb + (a4>>11)*aa)*aa0)>>20;
			a5 |= (a6 << 11);

			a1 = *((VO_U16*)(psrc1 + a0));
			a2 = *((VO_U16*)(psrc1 + a0 + 2));
			a3 = *((VO_U16*)(psrc1 + a0 + src_stride));
			a4 = *((VO_U16*)(psrc1 + a0 + 2 + src_stride));

			//B
			a6 = (((a1&0x1f)*bb + (a2&0x1f)*aa)*bb0 + ((a3&0x1f)*bb + (a4&0x1f)*aa)*aa0)>>20;
			a5 |= (a6 << 16);
			//G
			a6 = ((((a1&0x7e0)>>5)*bb + ((a2&0x7e0)>>5)*aa)*bb0 + (((a3&0x7e0)>>5)*bb + ((a4&0x7e0)>>5)*aa)*aa0)>>20;
			a5 |= (a6 << 21);
			//R
			a6 = (((a1>>11)*bb + (a2>>11)*aa)*bb0 + ((a3>>11)*bb + (a4>>11)*aa)*aa0)>>20;
			a5 |= (a6 << 27);

			*((VO_U32*)dst) = a5;
			dst -= dst_stride;

			x_scale_par0 += 3;

		}while(--i != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_rgb565_mb_s_r90(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst,VO_S32 src_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		VO_U8 *psrc1 = src + y_scale_par[0] * src_stride; // TBD

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];

		VO_U8 *psrc = src + y_scale_par[3] * src_stride; 
		//VO_S32 aa1 = y_scale_par[4];
		//VO_S32 bb1 = y_scale_par[5];

		i = width;	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			a0 = (a0<<1);
			a1 = *((VO_U16*)(psrc + a0));
			a2 = *((VO_U16*)(psrc + a0 + 2));
			a3 = *((VO_U16*)(psrc + a0 + src_stride));
			a4 = *((VO_U16*)(psrc + a0 + 2 + src_stride));
			//B
			a5 = (((a1&0x1f)*bb + (a2&0x1f)*aa)*bb0 + ((a3&0x1f)*bb + (a4&0x1f)*aa)*aa0)>>20;
			//G
			a6 = ((((a1&0x7e0)>>5)*bb + ((a2&0x7e0)>>5)*aa)*bb0 + (((a3&0x7e0)>>5)*bb + ((a4&0x7e0)>>5)*aa)*aa0)>>20;
			a5 |= (a6 << 5);
			//R
			a6 = (((a1>>11)*bb + (a2>>11)*aa)*bb0 + ((a3>>11)*bb + (a4>>11)*aa)*aa0)>>20;
			a5 |= (a6 << 11);

			a1 = *((VO_U16*)(psrc1 + a0));
			a2 = *((VO_U16*)(psrc1 + a0 + 2));
			a3 = *((VO_U16*)(psrc1 + a0 + src_stride));
			a4 = *((VO_U16*)(psrc1 + a0 + 2 + src_stride));

			//B
			a6 = (((a1&0x1f)*bb + (a2&0x1f)*aa)*bb0 + ((a3&0x1f)*bb + (a4&0x1f)*aa)*aa0)>>20;
			a5 |= (a6 << 16);
			//G
			a6 = ((((a1&0x7e0)>>5)*bb + ((a2&0x7e0)>>5)*aa)*bb0 + (((a3&0x7e0)>>5)*bb + ((a4&0x7e0)>>5)*aa)*aa0)>>20;
			a5 |= (a6 << 21);
			//R
			a6 = (((a1>>11)*bb + (a2>>11)*aa)*bb0 + ((a3>>11)*bb + (a4>>11)*aa)*aa0)>>20;
			a5 |= (a6 << 27);

			*((VO_U32*)dst) = a5;
			dst += dst_stride;

			x_scale_par0 += 3;

		}while(--i != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}