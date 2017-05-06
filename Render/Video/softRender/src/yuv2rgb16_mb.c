#include "yuv2rgb16_mb.h"
#include "ccConstant.h"
//////////////////////
void cc_yuv420_mb_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		 VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];


			a4 = src_y0[a0];
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
			a5 = src_y0[a6];
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			x_scale_par0 += 6;
			dst += 4;
		}while((i-=2) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_mb_s_l90_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			x_scale_par0 += 6;
			
		}while((i-=2) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}
void cc_yuv420_mb_s_r90_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			x_scale_par0 += 6;
			
		}while((i-=2) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}
//3quarter
void cc_yuv420_mb_s_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	//3/2
//0x0, 0x400
//0x155, 0x2ab
//0x2aa, 0x156
	//3/4
//0x0, 0x400
//0x2aa, 0x156
//0x155, 0x2ab

	do{
//y0
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);		
		
//		VO_S32 aa0;// = 0x0;//y_scale_par[1];
//		VO_S32 bb0;// = 0x400;//y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		VO_S32 aa1;// = 0x155;//y_scale_par[4];
//		VO_S32 bb1;// = 0x2ab;//y_scale_par[5];

		VO_S32 first1 = 0x155;//y_scale_par[4];
		VO_S32 first2 = 0x2ab;//y_scale_par[5];
		
//		VO_S32 aa;// = 0x2aa;//x_scale_par0[1];
//		VO_S32 bb;// = 0x156;//x_scale_par0[2];	

		VO_S32 second1 = 0x2aa;//x_scale_par0[1];
		VO_S32 second2 = 0x156;//x_scale_par0[2];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = (src_y1[a0]*first2 + src_y1[a0+src_stride]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x1
			a0 = x_scale_par0[6];
//			aa = 0x2aa;//x_scale_par0[1];
//			bb = 0x156;//x_scale_par0[2];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+1]*second1)>>10;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*first2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = src_y0[a6];
			a8 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+1]*first1)>>10;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*first2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			x_scale_par0 += 18;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			
		}while((i-=6) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;


//y1
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 =src_v + ((y_scale_par[0]>>1) * vin_stride);		
		
//		aa0 = 0x2aa;//y_scale_par[1];
//		bb0 = 0x156;//y_scale_par[2];

		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa = 0x155;////x_scale_par0[1];
//		bb = 0x2ab;//x_scale_par0[2];



		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;

//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+src_stride]*second1)>>10;
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
//			aa = 0x155;////x_scale_par0[1];
//			bb = 0x2ab;//x_scale_par0[2];
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a8 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*second2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a0]*second2 + src_y1[a0+1]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*second2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a0]*first2 + src_y1[a0+1]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a8 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			x_scale_par0 += 18;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			
		}while((i-=6) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;

//y2
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 =src_v + ((y_scale_par[0]>>1) * vin_stride);		
		
//		aa0 = 0x155;//y_scale_par[1];
//		bb0 = 0x2ab;//y_scale_par[2];
		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa1 = 0x2aa;//y_scale_par[4];
//		bb1 = 0x156;//y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+src_stride]*first1)>>10;
			a5 = (src_y1[a0]*second2 + src_y1[a0+src_stride]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*first2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*second2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a8 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*first2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*second2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			x_scale_par0 += 18;			
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			
		}while((i-=6) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;

	}while((height-=6) != 0);
}


void cc_yuv420_mb_s_l90_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	//3/2
//0x0, 0x400
//0x155, 0x2ab
//0x2aa, 0x156
	//3/4
//0x0, 0x400
//0x2aa, 0x156
//0x155, 0x2ab

	do{
//y0
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		VO_S32 aa0;// = 0x0;//y_scale_par[1];
//		VO_S32 bb0;// = 0x400;//y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		VO_S32 aa1;// = 0x155;//y_scale_par[4];
//		VO_S32 bb1;// = 0x2ab;//y_scale_par[5];

		VO_S32 first1 = 0x155;//y_scale_par[4];
		VO_S32 first2 = 0x2ab;//y_scale_par[5];
		
//		VO_S32 aa;// = 0x2aa;//x_scale_par0[1];
//		VO_S32 bb;// = 0x156;//x_scale_par0[2];	

		VO_S32 second1 = 0x2aa;//x_scale_par0[1];
		VO_S32 second2 = 0x156;//x_scale_par0[2];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = (src_y1[a0]*first2 + src_y1[a0+src_stride]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

//x1
			a0 = x_scale_par0[6];
//			aa = 0x2aa;//x_scale_par0[1];
//			bb = 0x156;//x_scale_par0[2];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+1]*second1)>>10;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*first2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+1]*first1)>>10;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*first2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			x_scale_par0 += 18;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=6) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;


//y1
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x2aa;//y_scale_par[1];
//		bb0 = 0x156;//y_scale_par[2];

		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa = 0x155;////x_scale_par0[1];
//		bb = 0x2ab;//x_scale_par0[2];


		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;

//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+src_stride]*second1)>>10;
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
//			aa = 0x155;////x_scale_par0[1];
//			bb = 0x2ab;//x_scale_par0[2];
				
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*second2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a0]*second2 + src_y1[a0+1]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*second2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a0]*first2 + src_y1[a0+1]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			x_scale_par0 += 18;
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=6) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;

//y2
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x155;//y_scale_par[1];
//		bb0 = 0x2ab;//y_scale_par[2];
		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa1 = 0x2aa;//y_scale_par[4];
//		bb1 = 0x156;//y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+src_stride]*first1)>>10;
			a5 = (src_y1[a0]*second2 + src_y1[a0+src_stride]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*first2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*second2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*first2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*second2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=6) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}



void cc_yuv420_mb_s_r90_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	//3/2
//0x0, 0x400
//0x155, 0x2ab
//0x2aa, 0x156
	//3/4
//0x0, 0x400
//0x2aa, 0x156
//0x155, 0x2ab

	do{
//y0
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		VO_S32 aa0;// = 0x0;//y_scale_par[1];
//		VO_S32 bb0;// = 0x400;//y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		VO_S32 aa1;// = 0x155;//y_scale_par[4];
//		VO_S32 bb1;// = 0x2ab;//y_scale_par[5];

		VO_S32 first1 = 0x155;//y_scale_par[4];
		VO_S32 first2 = 0x2ab;//y_scale_par[5];
		
//		VO_S32 aa;// = 0x2aa;//x_scale_par0[1];
//		VO_S32 bb;// = 0x156;//x_scale_par0[2];	

		VO_S32 second1 = 0x2aa;//x_scale_par0[1];
		VO_S32 second2 = 0x156;//x_scale_par0[2];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = (src_y1[a0]*first2 + src_y1[a0+src_stride]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

//x1
			a0 = x_scale_par0[6];
//			aa = 0x2aa;//x_scale_par0[1];
//			bb = 0x156;//x_scale_par0[2];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+1]*second1)>>10;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*first2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+1]*first1)>>10;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*first2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
						
			x_scale_par0 += 18;
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=6) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;


//y1
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x2aa;//y_scale_par[1];
//		bb0 = 0x156;//y_scale_par[2];

		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa = 0x155;////x_scale_par0[1];
//		bb = 0x2ab;//x_scale_par0[2];



		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;

//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+src_stride]*second1)>>10;
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
//			aa = 0x155;////x_scale_par0[1];
//			bb = 0x2ab;//x_scale_par0[2];
				
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*second2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a0]*second2 + src_y1[a0+1]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*second2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a0]*first2 + src_y1[a0+1]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			x_scale_par0 += 18;
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=6) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;

//y2
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x155;//y_scale_par[1];
//		bb0 = 0x2ab;//y_scale_par[2];
		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa1 = 0x2aa;//y_scale_par[4];
//		bb1 = 0x156;//y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+src_stride]*first1)>>10;
			a5 = (src_y1[a0]*second2 + src_y1[a0+src_stride]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*first2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*second2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*first2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*second2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=6) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}
//1point5
void cc_yuv420_mb_s_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	//3/2
//0x0, 0x400
//0x155, 0x2ab
//0x2aa, 0x156
	//3/4
//0x0, 0x400
//0x2aa, 0x156
//0x155, 0x2ab

	do{
//y0
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		VO_S32 aa0;// = 0x0;//y_scale_par[1];
//		VO_S32 bb0;// = 0x400;//y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		VO_S32 aa1;// = 0x155;//y_scale_par[4];
//		VO_S32 bb1;// = 0x2ab;//y_scale_par[5];

		VO_S32 second1 = 0x155;//y_scale_par[4];
		VO_S32 second2= 0x2ab;//y_scale_par[5];
		
//		VO_S32 aa;// = 0x2aa;//x_scale_par0[1];
//		VO_S32 bb;// = 0x156;//x_scale_par0[2];	

		VO_S32 first1= 0x2aa;//x_scale_par0[1];
		VO_S32 first2 = 0x156;//x_scale_par0[2];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = (src_y1[a0]*first2 + src_y1[a0+src_stride]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x1
			a0 = x_scale_par0[6];
//			aa = 0x2aa;//x_scale_par0[1];
//			bb = 0x156;//x_scale_par0[2];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+1]*second1)>>10;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*first2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = src_y0[a6];
			a8 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+1]*first1)>>10;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*first2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			x_scale_par0 += 18;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			
		}while((i-=6) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;


//y1
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x2aa;//y_scale_par[1];
//		bb0 = 0x156;//y_scale_par[2];

		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa = 0x155;////x_scale_par0[1];
//		bb = 0x2ab;//x_scale_par0[2];



		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;

//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+src_stride]*second1)>>10;
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
//			aa = 0x155;////x_scale_par0[1];
//			bb = 0x2ab;//x_scale_par0[2];
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a8 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*second2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a0]*second2 + src_y1[a0+1]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*second2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a0]*first2 + src_y1[a0+1]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a8 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			x_scale_par0 += 18;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			
		}while((i-=6) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;

//y2
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x155;//y_scale_par[1];
//		bb0 = 0x2ab;//y_scale_par[2];
		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa1 = 0x2aa;//y_scale_par[4];
//		bb1 = 0x156;//y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+src_stride]*first1)>>10;
			a5 = (src_y1[a0]*second2 + src_y1[a0+src_stride]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*first2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*second2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a8 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*first2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*second2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			x_scale_par0 += 18;			
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			
		}while((i-=6) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;

	}while((height-=6) != 0);
}


void cc_yuv420_mb_s_l90_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	//3/2
//0x0, 0x400
//0x155, 0x2ab
//0x2aa, 0x156
	//3/4
//0x0, 0x400
//0x2aa, 0x156
//0x155, 0x2ab

	do{
//y0
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		VO_S32 aa0;// = 0x0;//y_scale_par[1];
//		VO_S32 bb0;// = 0x400;//y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		VO_S32 aa1;// = 0x155;//y_scale_par[4];
//		VO_S32 bb1;// = 0x2ab;//y_scale_par[5];

		VO_S32 second1 = 0x155;//y_scale_par[4];
		VO_S32 second2= 0x2ab;//y_scale_par[5];
		
//		VO_S32 aa;// = 0x2aa;//x_scale_par0[1];
//		VO_S32 bb;// = 0x156;//x_scale_par0[2];	

		VO_S32 first1= 0x2aa;//x_scale_par0[1];
		VO_S32 first2 = 0x156;//x_scale_par0[2];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = (src_y1[a0]*first2 + src_y1[a0+src_stride]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

//x1
			a0 = x_scale_par0[6];
//			aa = 0x2aa;//x_scale_par0[1];
//			bb = 0x156;//x_scale_par0[2];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+1]*second1)>>10;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*first2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+1]*first1)>>10;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*first2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			x_scale_par0 += 18;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=6) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;


//y1
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x2aa;//y_scale_par[1];
//		bb0 = 0x156;//y_scale_par[2];

		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa = 0x155;////x_scale_par0[1];
//		bb = 0x2ab;//x_scale_par0[2];


		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;

//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+src_stride]*second1)>>10;
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
//			aa = 0x155;////x_scale_par0[1];
//			bb = 0x2ab;//x_scale_par0[2];
				
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*second2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a0]*second2 + src_y1[a0+1]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*second2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a0]*first2 + src_y1[a0+1]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			x_scale_par0 += 18;
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=6) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;

//y2
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x155;//y_scale_par[1];
//		bb0 = 0x2ab;//y_scale_par[2];
		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa1 = 0x2aa;//y_scale_par[4];
//		bb1 = 0x156;//y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+src_stride]*first1)>>10;
			a5 = (src_y1[a0]*second2 + src_y1[a0+src_stride]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*first2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*second2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*first2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*second2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=6) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}



void cc_yuv420_mb_s_r90_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	//3/2
//0x0, 0x400
//0x155, 0x2ab
//0x2aa, 0x156
	//3/4
//0x0, 0x400
//0x2aa, 0x156
//0x155, 0x2ab

	do{
//y0
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		VO_S32 aa0;// = 0x0;//y_scale_par[1];
//		VO_S32 bb0;// = 0x400;//y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		VO_S32 aa1;// = 0x155;//y_scale_par[4];
//		VO_S32 bb1;// = 0x2ab;//y_scale_par[5];

		VO_S32 second1 = 0x155;//y_scale_par[4];
		VO_S32 second2= 0x2ab;//y_scale_par[5];
		
//		VO_S32 aa;// = 0x2aa;//x_scale_par0[1];
//		VO_S32 bb;// = 0x156;//x_scale_par0[2];	

		VO_S32 first1= 0x2aa;//x_scale_par0[1];
		VO_S32 first2 = 0x156;//x_scale_par0[2];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a5 = (src_y1[a0]*first2 + src_y1[a0+src_stride]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
				
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

//x1
			a0 = x_scale_par0[6];
//			aa = 0x2aa;//x_scale_par0[1];
//			bb = 0x156;//x_scale_par0[2];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+1]*second1)>>10;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*first2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+1]*first1)>>10;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*first2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*first1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
						
			x_scale_par0 += 18;
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=6) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;


//y1
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x2aa;//y_scale_par[1];
//		bb0 = 0x156;//y_scale_par[2];

		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa = 0x155;////x_scale_par0[1];
//		bb = 0x2ab;//x_scale_par0[2];



		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;

//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*second2 + src_y0[a0+src_stride]*second1)>>10;
			a5 = src_y1[a0];
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
//			aa = 0x155;////x_scale_par0[1];
//			bb = 0x2ab;//x_scale_par0[2];
				
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*second2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a0]*second2 + src_y1[a0+1]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*second2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a0]*first2 + src_y1[a0+1]*first1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			x_scale_par0 += 18;
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=6) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;

//y2
		x_scale_par0 = x_scale_par;
		
		src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
//		aa0 = 0x155;//y_scale_par[1];
//		bb0 = 0x2ab;//y_scale_par[2];
		src_y1 = src_y + y_scale_par[3] * src_stride;
		
//		aa1 = 0x2aa;//y_scale_par[4];
//		bb1 = 0x156;//y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
//x0
			a0 = x_scale_par0[0];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = (src_y0[a0]*first2 + src_y0[a0+src_stride]*first1)>>10;
			a5 = (src_y1[a0]*second2 + src_y1[a0+src_stride]*second1)>>10;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

//x1
			a0 = x_scale_par0[6];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[9];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*second2 + src_y0[a0+1]*second1)*first2 + (src_y0[a0+src_stride]*second2 + src_y0[a0+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a0]*second2 + src_y1[a0+1]*second1)*second2 + (src_y1[a0+src_stride]*second2 + src_y1[a0+src_stride+1]*second1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
//x2
			a0 = x_scale_par0[12];
			
//			x_scale_par0 += 3;
			a6 = x_scale_par0[15];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*first2 + src_y0[a0+1]*first1)*first2 + (src_y0[a0+src_stride]*first2 + src_y0[a0+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a0]*first2 + src_y1[a0+1]*first1)*second2 + (src_y1[a0+src_stride]*first2 + src_y1[a0+src_stride+1]*first1)*second1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
					
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			
//			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=6) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}
//double
void cc_yuv420_mb_s_c_double(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];
			a4 = src_y0[a0];
			a5 = (src_y1[a0] + src_y1[a0+src_stride])>>1;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////

			a5 = (src_y0[a6] + src_y0[a6+1])>>1;
			a8 = (src_y1[a6] + src_y1[a6+1] + src_y1[a6+src_stride] + src_y1[a6+src_stride+1])>>2;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			x_scale_par0 += 6;
			dst += 4;
		}while((i-=2) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);
}
void cc_yuv420_mb_s_l90_c_double(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];
			a4 = src_y0[a0];
			a5 = (src_y1[a0] + src_y1[a0+src_stride])>>1;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			a4 = (src_y0[a6] + src_y0[a6+1])>>1;
			a5 = (src_y1[a6] + src_y1[a6+1] + src_y1[a6+src_stride] + src_y1[a6+src_stride+1])>>2;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			x_scale_par0 += 6;
			
		}while((i-=2) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}
void cc_yuv420_mb_s_r90_c_double(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			a6 = x_scale_par0[3];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];
			a4 = src_y0[a0];
			a5 = (src_y1[a0] + src_y1[a0+src_stride])>>1;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			a4 = (src_y0[a6] + src_y0[a6+1])>>1;
			a5 = (src_y1[a6] + src_y1[a6+1] + src_y1[a6+src_stride] + src_y1[a6+src_stride+1])>>2;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			x_scale_par0 += 6;
			
		}while((i-=2) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

//////////////////////
void cc_yuv420_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
					VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
					 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		 VO_S32 aa0 = y_scale_par[1];
		 VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		 VO_S32 aa1 = y_scale_par[4];
		 VO_S32 bb1 = y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
//			a1 = ((a0+1)>>1);
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
			
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];//(src_u0[a1]*(bb*bb0) + src_u0[a1+1]*(aa*bb0)+src_u0[a1+src_stride2]*(aa0*bb) + src_u0[a1+src_stride2+1]*(aa0*aa))>>20;
			a3 = src_v0[a1];//(src_v0[a1]*(bb*bb0) + src_v0[a1+1]*(aa*bb0)+src_v0[a1+src_stride2]*(aa0*bb) + src_v0[a1+src_stride2+1]*(aa0*aa))>>20;
			
			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;
		}while((i-=2) != 0);
		
		dst -= (width<<1);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_mb_s_180_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
					VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
					 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	dst += dst_stride*(height - 1); 
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		 VO_S32 aa0 = y_scale_par[1];
		 VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		 VO_S32 aa1 = y_scale_par[4];
		 VO_S32 bb1 = y_scale_par[5];	
		
		 dst = dst + (width<<1) - 4;
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			VO_U32 tmp;
			// u, v
			a0 = x_scale_par0[0];//src pos0
//			a1 = ((a0+1)>>1);
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
			
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];//(src_u0[a1]*(bb*bb0) + src_u0[a1+1]*(aa*bb0)+src_u0[a1+src_stride2]*(aa0*bb) + src_u0[a1+src_stride2+1]*(aa0*aa))>>20;
			a3 = src_v0[a1];//(src_v0[a1]*(bb*bb0) + src_v0[a1+1]*(aa*bb0)+src_v0[a1+src_stride2]*(aa0*bb) + src_v0[a1+src_stride2+1]*(aa0*aa))>>20;
			
			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1);
			
			/////////////////////////////////////////////////////////////////////
//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;
			
			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHER_P3)>>2]>>1);//|a7;
			
			a4 |= (a5 << 16);
			a7 |= (a8 << 16);

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
void cc_yuv420_mb_s_l90_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		 VO_S32 aa0 = y_scale_par[1];
		 VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		 VO_S32 aa1 = y_scale_par[4];
		 VO_S32 bb1 = y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];//(src_u0[a1]*(bb*bb0) + src_u0[a1+1]*(aa*bb0)+src_u0[a1+src_stride2]*(aa0*bb) + src_u0[a1+src_stride2+1]*(aa0*aa))>>20;
			a3 = src_v0[a1];//(src_v0[a1]*(bb*bb0) + src_v0[a1+1]*(aa*bb0)+src_v0[a1+src_stride2]*(aa0*bb) + src_v0[a1+src_stride2+1]*(aa0*aa))>>20;		

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))<<16)|a4;
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))<<16)|a4;
			
			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=2) != 0);
		
		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_mb_s_r90_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
						VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		
		 VO_S32 aa0 = y_scale_par[1];
		 VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		
		 VO_S32 aa1 = y_scale_par[4];
		 VO_S32 bb1 = y_scale_par[5];	
		
		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			VO_S32 aa, bb;
			// u, v
			a0 = x_scale_par0[0];//src pos0
//			a1 = ((a0+1)>>1);
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];//(src_u0[a1]*(bb*bb0) + src_u0[a1+1]*(aa*bb0)+src_u0[a1+src_stride2]*(aa0*bb) + src_u0[a1+src_stride2+1]*(aa0*aa))>>20;
			a3 = src_v0[a1];//(src_v0[a1]*(bb*bb0) + src_v0[a1+1]*(aa*bb0)+src_v0[a1+src_stride2]*(aa0*bb) + src_v0[a1+src_stride2+1]*(aa0*aa))>>20;

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;
			
			a2 -= 128;
			a3 -= 128;
			
			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
		
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P0)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P1)>>2]>>1))|(a4<<16);
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];
			
			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;
			
			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHER_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHER_P2)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHER_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHER_P3)>>2]>>1))|(a4<<16);
			
			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=2) != 0);
		
		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cc_rgb24_mb_s_l90(int width, int height, VO_U8 *src, VO_U8 *dst,
					   VO_S32 src_stride, VO_S32 dst_stride,
					   VO_S32 *x_scale_par, VO_S32 *y_scale_par)
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

void cc_rgb24_mb_s_r90(int width, int height, VO_U8 *src, VO_U8 *dst,
					   VO_S32 src_stride, VO_S32 dst_stride,
					   VO_S32 *x_scale_par, VO_S32 *y_scale_par)
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

void cc_rgb24_mb_s(int width, int height, VO_U8 *src, VO_U8 *dst,
				   VO_S32 src_stride, VO_S32 dst_stride,
				   VO_S32 *x_scale_par, VO_S32 *y_scale_par)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cc_rgb565_mb_s(int width, int height, VO_U8 *src, VO_U8 *dst,
					 VO_S32 src_stride, VO_S32 dst_stride,
					 VO_S32 *x_scale_par, VO_S32 *y_scale_par)
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

void cc_rgb565_mb_s_l90(int width, int height, VO_U8 *src, VO_U8 *dst,
						 VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par)
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

void cc_rgb565_mb_s_r90(int width, int height, VO_U8 *src, VO_U8 *dst,
						 VO_S32 src_stride, VO_S32 dst_stride,
						 VO_S32 *x_scale_par, VO_S32 *y_scale_par)
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//scale tale int |length|pos x0_par x1_par...|

void init_scale(float x_scale, int out_width, int *scale_tab, int in_width)
{
	int i, j, *mb_start_pos, len, tmp, x_par;
	float out_pos;
	
	i = 1;
	len = 0;
	mb_start_pos = scale_tab;
	scale_tab++;
	in_width--;

	for(j = 0; j < out_width; j++){
		out_pos = x_scale * j;
		
		tmp = (int)out_pos;
		
		x_par = (int)((out_pos - tmp)*1024);//1024
		
		len ++;
		if(tmp >= (i*16-1)){
			
			if(j&1){// finish current mb
				if(tmp>=in_width)
					tmp = in_width - 1;
				*(scale_tab++) = tmp; // out_pos
				*(scale_tab++) = x_par; // x
				*(scale_tab++) = ((1<<10) - x_par); // y
				*mb_start_pos = len;
				//				printf("%d length: %d\n", i, len);
				
				len = 0;
				mb_start_pos = scale_tab;
				scale_tab ++;
			}else{// start next mb
				if(tmp>=in_width)
					tmp = in_width - 1;
				*mb_start_pos = len - 1;
				//				printf("%d length: %d\n", i, len-1);
				
				mb_start_pos = scale_tab++;
				
				*(scale_tab++) = tmp; // out_pos
				*(scale_tab++) = x_par; // x
				*(scale_tab++) = ((1<<10) - x_par); // y
				len = 1;
			}
			i++;
		}else{
			if(tmp>=in_width)
				tmp = in_width - 1;
			*(scale_tab++) = tmp; // out_pos
			*(scale_tab++) = x_par; // x
			*(scale_tab++) = ((1<<10) - x_par); // y
		}
	}
	*mb_start_pos = len;
}


int *y_tab = 0;
int *x_tab = 0;



void cc_s_yuv420_disable(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_180(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	

	x_scale_tab = x_tab;
	y_scale_tab = y_tab;

	height = y_scale_tab[0];
	dst_start = out_buf + out_stride * (out_height -height);

	for(j = 0; j < mb_height; j++){

		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0));
				cc_yuv420_mb_s_180(width, height, y, u, v, 
				dst + out_stride - (width<<1), in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst -= (width<<1);
		}

		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
		height = y_scale_tab[0];
		dst_start -= (height * out_stride);
	}
}

void cc_s_yuv420_r90(unsigned char *y, unsigned char *u, unsigned char *v,
					 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_height << 1) - 4;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_r90(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}


void cc_s_yuv420_l90(unsigned char *y, unsigned char *u, unsigned char *v,
					 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_width - 1) * out_stride;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_l90(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}

void cc_s_yuv420_disable_half(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	

	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf;		
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_c_half(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_l90_half(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_width - 1) * out_stride;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_l90_c_half(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_r90_half(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_height << 1) - 4;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_r90_c_half(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}

void cc_s_yuv420_disable_3quarter(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_c_3quarter(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_l90_3quarter(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_width - 1) * out_stride;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_l90_c_3quarter(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_r90_3quarter(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_height << 1) - 4;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_r90_c_3quarter(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}

void cc_s_yuv420_disable_1point5(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_c_1point5(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_l90_1point5(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{

	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_width - 1) * out_stride;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_l90_c_1point5(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_r90_1point5(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_height << 1) - 4;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_r90_c_1point5(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_disable_double(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end

	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_c_double(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_l90_double(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_width - 1) * out_stride;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_l90_c_double(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
void cc_s_yuv420_r90_double(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_height << 1) - 4;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_yuv420_mb_s_r90_c_double(width, height, y, u, v, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1, uin_stride, vin_stride);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cc_s_rgb24_disable(unsigned char *y, unsigned char *u, unsigned char *v,
						int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
//	uin_stride = Pin_stride[1];
//	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
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
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}

void cc_s_rgb24_r90(unsigned char *y, unsigned char *u, unsigned char *v,
					int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
//	uin_stride = Pin_stride[1];
//	vin_stride = Pin_stride[2];	
//add end	

	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
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
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}


void cc_s_rgb24_l90(unsigned char *y, unsigned char *u, unsigned char *v,
					int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
//	uin_stride = Pin_stride[1];
//	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
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
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void cc_s_rgb565_disable(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
//	uin_stride = Pin_stride[1];
//	vin_stride = Pin_stride[2];	
//add end

	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_rgb565_mb_s(width, height, y, dst, in_stride, 
				out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst += (width<<1);
		}
		dst_start += (height * out_stride);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}

void cc_s_rgb565_l90(unsigned char *y, unsigned char *u, unsigned char *v,
					 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
//	uin_stride = Pin_stride[1];
//	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_width - 1) * out_stride;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_rgb565_mb_s_l90(width, height, y, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst -= (width * out_stride);
		}
		dst_start += (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}


void cc_s_rgb565_r90(unsigned char *y, unsigned char *u, unsigned char *v,
					 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					 unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	unsigned char *dst_start, *dst;
	int i, j, width, height, skip = 0;
	int mb_width = (in_width+15)/16;
	int mb_height = (in_height+15)/16;
	int *x_scale_tab, *y_scale_tab;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
//	uin_stride = Pin_stride[1];
//	vin_stride = Pin_stride[2];	
//add end	
	
	x_scale_tab = x_tab;
	y_scale_tab = y_tab;
	dst_start = out_buf + (out_height << 1) - 4;
	
	for(j = 0; j < mb_height; j++){
		height = y_scale_tab[0];
		dst = dst_start;
		for(i = 0; i < mb_width; i++){
			width = x_scale_tab[0];
			if(mb_skip) skip = *(mb_skip++);
			if((!skip) && (width != 0) && (height != 0))
				cc_rgb565_mb_s_r90(width, height, y, 
				dst, in_stride, out_stride, x_scale_tab+1, y_scale_tab+1);
			x_scale_tab += (width * 3 + 1);
			dst += (width * out_stride);
		}
		dst_start -= (height << 1);
		
		x_scale_tab = x_tab;
		y_scale_tab += (height * 3 + 1);
	}
}
